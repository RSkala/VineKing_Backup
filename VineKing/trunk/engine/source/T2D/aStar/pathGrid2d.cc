//------------------------------------------------------------------------------
// Phil Shenk
// pathGrid2d
//------------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include <string.h>
#include "./pathGrid2d.h"



//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
//  pathGrid2d.
//
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(pathGrid2d);
IMPLEMENT_CONOBJECT(pathAStar2d);

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
pathGrid2d::pathGrid2d( ) :	mpathGrid2d(StringTable->insert("")),
								mTileCountX(NULL),
								mTileCountY(NULL),
								mTileLayer(NULL),
								mDrawDebug(false)
{
}
pathGrid2d::pathGrid2d( t2dTileLayer *const pLayer ) :	
								mpathGrid2d(StringTable->insert("")),
								mTileCountX(NULL),
								mTileCountY(NULL),
								mDrawDebug(false)
{
	setTileLayer( pLayer );
}
//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
pathGrid2d::~pathGrid2d()
{
	deleteAllPaths();
}

ConsoleMethod(pathGrid2d, getTileLayer, S32, 2, 2, "returns the assigned tile layer")
{
	return object->getTileLayer()->getId();
}
//-----------------------------------------------------------------------------
// SetLayer.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, setTileLayer, bool, 3, 3, "(layer Name) - Sets the layer")
{
	t2dTileLayer* pTileLayerObject = (t2dTileLayer*)(Sim::findObject(argv[2]));
	// Validate Object
	if ( !pTileLayerObject )
	{
		Con::warnf("pathGrid2d::setTileLayer - Couldn't find object '%$s'.", argv[2]);
		return false;
	}
	// Set Layer
	return object->setTileLayer( pTileLayerObject );
}

bool pathGrid2d::setTileLayer( t2dTileLayer *const pLayer )
{
	mTileLayer = pLayer;
	mTileCountX = pLayer->getTileCountX();	
	mTileCountY = pLayer->getTileCountY();	

	if(mTileCountX <= 0 || mTileCountY <= 0)
	{
		Con::errorf("Tile Layer Size is invalid");
		return false;
	}

	// resize the mMapArray
	mMapArray.resize( mTileCountX, mTileCountY );
	//Con::printf("setTilelayer, mMapArray is set to size (%i), x was (%i), y was (%i)",
	//	mMapArray.size(), mTileCountX, mTileCountY);
	// set up the map array, getting the custom data
	clearMap( true );
	// set the size to equal mTileLayer
	setSize( mTileLayer->getSize() );
	// mount to mTileLayer, so it will always be in the right position
	mount( mTileLayer, t2dVector( 0, 0), 0, true, true, false, true );

	// set the size of the mDebugShapeList
	if (mDrawDebug)
	{
	  mClosedOpenList.resize( mTileCountX, mTileCountY );
	  // reset -- might not need to do this...but we do. funky access problems with finding a node
	  // efficiently and confirming the object there is valid.

	 // Con::printf("setTileLayer--set the size of mClosedOpenList to %i", mClosedOpenList.size() );
	  for ( U32 n = 0; n < (mTileCountX * mTileCountY); n++ )
          mClosedOpenList[n] = NULL;
	  
	}

	return true;
}

//-----------------------------------------------------------------------------
// setDrawDebug.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, setDebugDraw, void, 3, 3, "(bool) - Sets debug draw flag")
{
	bool val =  dAtob(argv[2]);
	object->setDebugDraw( val );
}

void  pathGrid2d::clearDebugSymbols()
{
	if (mDrawDebug)
	{
		clearDebugOpenList();
		clearDebugNodeList();
	}
}


ConsoleMethod(pathGrid2d, clearSymbols, void, 2, 2, "() -- clears current symbols on map")
{

	  object->clearDebugSymbols();

}

ConsoleMethod(pathGrid2d, setDiagonalMovement, void, 3, 3, "(bool) - Enable/Disable diagonal movement")
{
	bool val = dAtob(argv[2]);
	object->mNoDiagonal = !val;
}

void pathGrid2d::setDebugDraw( const bool val )
{
	// if changing to false, then clear the debug symbols
	if ( mDrawDebug == true && val == false )
	{
		clearDebugOpenList();
		clearDebugNodeList();
	}
	mDrawDebug = val;
}

//-----------------------------------------------------------------------------
// GetTileCountX/Y.
//-----------------------------------------------------------------------------
U32 pathGrid2d::getTileCountX()
{
	return mTileCountX;
}

U32 pathGrid2d::getTileCountY()
{
	return mTileCountY;
}

//-----------------------------------------------------------------------------
// tileToLocal.
//-----------------------------------------------------------------------------
void pathGrid2d::tileToLocal( const t2dVector &tPosIn, t2dVector &lPosOut )
{
	// set locals for calculating coords in tileLayer space
	F32 xCount = mTileLayer->getTileCountX();
	F32 yCount = mTileLayer->getTileCountY();
	F32 xShift = 2 / xCount;
	F32 yShift = 2 / yCount;

	// caculate local coords
	lPosOut.mX = (tPosIn.mX * xShift) + (xShift / 2) - 1;
	lPosOut.mY = (tPosIn.mY * yShift) + (yShift / 2) - 1;
}

//-----------------------------------------------------------------------------
// tileToWorld.
//-----------------------------------------------------------------------------
void pathGrid2d::localToWorld ( const t2dVector &lPos, t2dVector &outPos )
{
	mHalfSize = getParentPhysics().getHalfSize();
	// calculate world pos (this will set wPosPtr)
	transformPoint2D( getRotationMatrix(), t2dVector( lPos.mX * mHalfSize.mX, lPos.mY * mHalfSize.mY), getPosition(), outPos );
	// UNUSED: JOSEPH THOMAS -> int foo1 = 0;

}

//-----------------------------------------------------------------------------
// tileToWorld.
//-----------------------------------------------------------------------------
void pathGrid2d::localToTile ( const t2dVector &lPos, t2dVector &outPos )
{
	Point2I tPos;
	localToWorld( lPos, outPos );
	mTileLayer->pickTile( outPos, tPos);
	outPos = tPos;
}

//-----------------------------------------------------------------------------
// getAngle. Sets scene mAngle of mV1 to mV2
//-----------------------------------------------------------------------------
void pathGrid2d::getAngle ( const t2dVector &vIn1, const t2dVector &vIn2, F32 &angleOut )
{
	// find angle
	angleOut = mFmod( mRadToDeg( mAtan( vIn2.mX - vIn1.mX, vIn1.mY - vIn2.mY ) ), 360.0f );
	
	// normalize to 0 - 360
	if ( angleOut < 0.0f ) angleOut += 360.0f;
}

F32 pathGrid2d::getAngle ( const t2dVector &vIn1, const t2dVector &vIn2 )
{
	// find angle
	F32 angle = mFmod( mRadToDeg( mAtan( vIn2.mX - vIn1.mX, vIn1.mY - vIn2.mY ) ), 360.0f );
	
	// normalize to 0 - 360
	if ( angle < 0.0f ) angle += 360.0f;

	return angle;

}

//-----------------------------------------------------------------------------
// Find Path Index from ID.
//-----------------------------------------------------------------------------
pathAStar2d* pathGrid2d::findPath( U32 pathID ) const
{
	// quit if no paths
	if ( mPathList.size() == 0 )
		return NULL;

	for ( U32 n = 0; n < mPathList.size(); n++ )
		if ( mPathList[n]->getId() == pathID )
			return mPathList[n];

	// not found
	return NULL;
}

//-----------------------------------------------------------------------------
// Find Path from index.
//-----------------------------------------------------------------------------
S32 pathGrid2d::findPathIndex( pathAStar2d* pPath ) const
{
	// search for indentical path
	for ( U32 n = 0; n < mPathList.size(); n++ )
		// Return path index of match
		if ( mPathList[n] == pPath )
			return n;

	// Return "No Match!"
	return -1;
}

//-----------------------------------------------------------------------------
// Create a new path object.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, createPath, S32, 6, 8, "(startX / startY, destX / destY, [cut corners?], [optimise?]) - creates a path")
{
	if ( !object->isLayer() )
	{
		Con::warnf("pathGrid2d::createPath( sX / sy , dX dY ) - No tileLayer set!");
		return -1;
	}
	// check parameters
	if ( t2dSceneObject::getStringElementCount(argv[2]) < 2 || t2dSceneObject::getStringElementCount(argv[3]) < 2 )
	{
		Con::warnf("pathGrid2d::calculate( sX / sy , dX dY ) - Invalid number of parameters!");
		return false;
	}

	// set  corner-cutting
	bool cornerFlag = argc >= 4 ? dAtob(argv[4]) : false;
	// set optimise
	bool optFlag = argc >= 5 ? dAtob(argv[5]) : false;

	// set up the coords
	t2dVector start( dAtof(t2dSceneObject::getStringElement(argv[2], 0)), dAtof(t2dSceneObject::getStringElement(argv[2], 1)) );
	t2dVector dest( dAtof(t2dSceneObject::getStringElement(argv[3], 0)), dAtof(t2dSceneObject::getStringElement(argv[3], 1)) );
	// create a pathAStar2d, and return the simobject ID	

	return object->createPath( start, dest, cornerFlag, optFlag );
}

SimObjectId pathGrid2d::createPath( const t2dVector start, const t2dVector dest, const bool cornerFlag, const bool optFlag )
{
	bool isValidPath;
	if (mDrawDebug)
	{
      clearDebugSymbols();
	}
	pathAStar2d* pPathObj = new pathAStar2d( this, start, dest, cornerFlag, optFlag, mNoDiagonal );
	pPathObj->registerObject();
	isValidPath = pPathObj->calculate(start, dest, cornerFlag, optFlag);
    if (isValidPath)
	{
	  pPathObj->setChild( true );
      mPathList.push_back( pPathObj );
	  return pPathObj->getId();
	}
	else
	{
		//pPathObj->unregisterObject();
		pPathObj->safeDelete(); // RKS: I PUT THIS HERE, AS IT WAS CAUSING A MEMORY LEAK!
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Delete a Path by index.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, deletePath, bool, 3, 3, "(pathIndex) - Deletes a Path.")
{	
	return object->deletePath( dAtoi(argv[2]) );
}

bool pathGrid2d::deletePath( const U32 pathIndex )
{
	U32 pathCount = getPathCount();

	if ( pathIndex >= pathCount )
	{
		Con::warnf("pathGrid2d::getPath( index ) - Invalid Path '%d'! Currently '%d' Paths available.", pathIndex, pathCount);
		return false;
	}
	mPathList[pathIndex]->deleteObject();
	mPathList.erase( pathIndex );
	return true;
}

//-----------------------------------------------------------------------------
// Delete a Path by ID.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, deletePathId, void, 3, 3, "deletePathId(pathID) - deletes a Path by Id.")
{
	object->deletePathId( dAtoi(argv[2]) );
}

bool pathGrid2d::deletePathId( const SimObjectId pathID )
{
	// Find Path
	pathAStar2d* pPath = findPath( pathID );
	// Check the Return
	if ( !pPath )
	{
		Con::warnf("pathGrid2d::deletePathId() - Invalid Path ID! (%d)", pathID);
		return false;
	}
	// Find Path Index
	S32 pathIndex = findPathIndex( pPath );
	// Sanity
	AssertFatal( pathIndex != -1, "pathGrid2d::deletePathId() - Could not find Path" );
	// delete Path
	deletePath( pathIndex );
	return true;
}
//-----------------------------------------------------------------------------
// Deletes all Paths.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, deleteAllPaths, void, 2, 2, "Deletes all Path(s).")
{
	object->deleteAllPaths();
}

bool pathGrid2d::deleteAllPaths()
{
	// Delete all Tile Paths
	for ( U32 n = 0; n < mPathList.size(); n++ )
		// Delete the object
		mPathList[n]->deleteObject();

	// Clear Path List
	mPathList.clear();

	return true;
}

//-----------------------------------------------------------------------------
// Get Path Count.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, getPathCount, S32, 2, 2, "Returns the number of paths")
{
	return object->getPathCount();
}

U32 pathGrid2d::getPathCount(void)
{
	return mPathList.size();
}
//-----------------------------------------------------------------------------
// Get Path.
//-----------------------------------------------------------------------------
ConsoleMethod(pathGrid2d, getPath, S32, 3, 3, "(pathIndex) - Returns Path by Index.")
{
	return object->getPath( dAtoi(argv[2]) );
}

SimObjectId pathGrid2d::getPath( const U32 pathIndex )
{
	U32 pathCount = getPathCount();

	if ( pathIndex >= pathCount )
	{
		Con::warnf("pathGrid2d::getPath( index ) - Invalid Path '%d'! Currently '%d' Paths available.", pathIndex, pathCount);
		return -1;
	}
	return mPathList[pathIndex]->getId();
}

//-----------------------------------------------------------------------------
// clearMap
// clears mMapArray to defaults. If flag getCustomData is true, then it will
// look at mTileLayer to see if the collision has changed.
// getCustomData	- bool
//-----------------------------------------------------------------------------
void pathGrid2d::clearMap( const bool getCustomData )
{
	// set up the map array
	// UNUSED: JOSEPH THOMAS -> U32 index;
	GridCell dCell;
	dCell.open = true;
	dCell.mG = 0.0f;
	dCell.mH = 0.0f;
	dCell.mF = 0.0f;
	dCell.parent.mX = -1.0f;
	dCell.parent.mY = -1.0f;

	// UNUSED: JOSEPH THOMAS -> bool pass;
	// UNUSED: JOSEPH THOMAS -> F32 weight;

	for( U32 h = 0; h < mTileCountY; h++ )
	{
		for( U32 w = 0; w < mTileCountX; w++ )
		{
			// set defaults
			mMapArray.get( w, h ) = dCell;

			// set passable info
			if ( getCustomData == true )
			{
				getPassData( Point2I( w, h ) );

				//// get the custom data and check it
				//U32 cData = dAtof( mTileLayer->getTileCustomData( w, h ) );
				//if( cData == PASSABLE )
				//	mMapArray.get( w, h ).mPassable = true;
				//if( cData == BLOCKED )
				//	mMapArray.get( w, h ).mPassable = false;
				//// there is weighting data
				//if( cData > PASSABLE && cData < BLOCKED )
				//{
				//	mMapArray.get( w, h ).mWeight = cData;
				//	mMapArray.get( w, h ).mPassable = true;
				//}
				//else
				//{
				//	mMapArray.get( w, h ).mWeight = PASSABLE;
				//}
			}
		}
	}
}

ConsoleMethod(pathGrid2d, setWeight, void, 5, 5, "(weight) - Sets the weight for this tile, from 0 (passable) to 10 (blocked)")
{
	U32 xTile, yTile, newWeight;

	if (argc != 5)
	{
		Con::warnf("pathGrid2d::setWeight -- requires 3 parameters");
		return;
	}
	xTile = dAtoi(argv[2]);
	yTile = dAtoi(argv[3]);
	newWeight = dAtoi(argv[4]);
	object->setWeight(xTile, yTile, newWeight);
	return;

}

void pathGrid2d::setWeight( U32 xTile, U32 yTile, U32 newWeight )
{
	Point2I gridLoc;

	if ( (xTile < 0 || xTile >= mTileCountX) ||
		 (yTile < 0 || yTile >= mTileCountY) )
	{
		// tile out of bounds, do nothing
		return;
	}
	gridLoc.x = xTile;
	gridLoc.y = yTile;

	if( newWeight == PASSABLE )
		mMapArray.get( gridLoc ).mPassable = true;
	if( newWeight == BLOCKED )
		mMapArray.get( gridLoc ).mPassable = false;
	// there is weighting data
	if( newWeight > PASSABLE && newWeight < BLOCKED )
	{
		mMapArray.get( gridLoc ).mWeight = newWeight;
		mMapArray.get( gridLoc ).mPassable = true;
	}
	else
	{
		mMapArray.get( gridLoc ).mWeight = PASSABLE;
	}
}

U32 pathGrid2d::getWeight( U32 xTile, U32 yTile )
{
	Point2I gridLoc;

	if ( (xTile < 0 || xTile >= mTileCountX) ||
		 (yTile < 0 || yTile >= mTileCountY) )
	{
		// tile out of bounds, do nothing
		return 0;
	}
	gridLoc.x = xTile;
	gridLoc.y = yTile;

	return (mMapArray.get( gridLoc ).mWeight);
}

//-----------------------------------------------------------------------------
// getPassData - sets blocked and weight according to mTileLayer
//-----------------------------------------------------------------------------
void pathGrid2d::getPassData( const Point2I &tPos )
{
	F32 cData = 0.0f;
	const char * dataBuff = mTileLayer->getTileCustomData( tPos.x, tPos.y );
	if (dataBuff)
		cData = dAtoi(dataBuff);
	else
		cData = PASSABLE;
	//U32 dataCount = t2dSceneObject::getStringElementCount( cData );
	// UNUSED: JOSEPH THOMAS -> int foo = 0;
	if( cData == PASSABLE )
		mMapArray.get( tPos ).mPassable = true;
	if( cData == BLOCKED )
		mMapArray.get( tPos ).mPassable = false;
	// there is weighting data
	if( cData > PASSABLE && cData < BLOCKED )
	{
		mMapArray.get( tPos ).mWeight = cData;
		mMapArray.get( tPos ).mPassable = true;
	}
	else
	{
		mMapArray.get( tPos ).mWeight = PASSABLE;
	}

}

//-----------------------------------------------------------------------------
// isLayer - Check if there is a tileLayer set.
//-----------------------------------------------------------------------------
bool pathGrid2d::isLayer()
{
	if ( mTileLayer )
		return true;
	else 
		return false;
}

//-----------------------------------------------------------------------------
// Debug Drawing - functions for drawing debug shapes.
// Console Methods
//-----------------------------------------------------------------------------
/*
ConsoleMethod(pathGrid2d, debugOpen, S32, 3, 3, "( pos ) - makes an 'open' symbol")
{
	t2dShapeVector* pShape = object->addDebugShape( t2dVector( dAtof(t2dSceneObject::getStringElement(argv[2], 0)),
		dAtof(t2dSceneObject::getStringElement(argv[2], 1)) ), 0 );

	return pShape->getId();
}

ConsoleMethod(pathGrid2d, debugClosed, S32, 3, 3, "( pos ) - makes an 'closed' symbol")
{
	t2dShapeVector* pShape = object->addDebugShape( t2dVector( dAtof(t2dSceneObject::getStringElement(argv[2], 0)),
		dAtof(t2dSceneObject::getStringElement(argv[2], 1)) ), 1 );

	return pShape->getId();
}

ConsoleMethod(pathGrid2d, debugNode, S32, 3, 3, "( pos ) - makes an 'node' symbol")
{
	t2dShapeVector* pShape = object->addDebugShape( t2dVector( dAtof(t2dSceneObject::getStringElement(argv[2], 0)),
		dAtof(t2dSceneObject::getStringElement(argv[2], 1)) ), 2 );

	return pShape->getId();
}
*/
//-----------------------------------------------------------------------------
// addDebugOpen - 0 = "open", 1 = "closed"
// takes a tile pos (converts to local for mounting)
//-----------------------------------------------------------------------------
void pathGrid2d::addDebugOpen( const t2dVector &pos, const U32 index )
{
	return;
	// clear shape if it exists at that location
	deleteDebugOpen( pos );

	// add shape
	t2dShapeVector *pShape = new t2dShapeVector();
	pShape->setChild( true );
	pShape->registerObject();
	getSceneGraph()->addToScene(pShape);

	// set unique parameters
	switch ( index ) {
		case 0:
			// blue circle (Open)
			pShape->setPolyPrimitive( 16 );
			pShape->setSize( t2dVector( mTileLayer->getTileSizeX() * 0.5f, mTileLayer->getTileSizeY() * 0.5f ) );
			pShape->setLineColour(ColorF( 0, 255, 0 ) );
			pShape->setFillColour( ColorF( 0, 255, 0 ) );
			break;
		case 1:
			// Red rectangle (Closed)
			pShape->setPolyPrimitive( 4 );
			pShape->setSize( t2dVector( mTileLayer->getTileSizeX() * 0.5f, mTileLayer->getTileSizeY() * 0.15f ) );
			pShape->setLineColour(ColorF( 255, 0, 0 ) );
			pShape->setFillColour( ColorF( 255, 0, 0 ) );
			break;
		default:
			return;
	}
	// mount to the pathGrid
	t2dVector lPosOut;
	tileToLocal( pos, lPosOut );
	pShape->mount( this, lPosOut, 0, true, true, true, false ); 
	
	// set defaults
	pShape->setFillMode(true);
	pShape->setLineAlpha( 0.5f );
	pShape->setFillAlpha( 0.25f );

	// add the pointer to mDebugShapeList
	mClosedOpenList.push_back( pShape );
	// mClosedOpenList.get( pos ) = pShape;

	return;
}

//-----------------------------------------------------------------------------
// deleteDebugOpen - Deletes a debug shape at pos, if one exists there.
//-----------------------------------------------------------------------------
void pathGrid2d::deleteDebugOpen( const t2dVector& pos )
{
//	if (mClosedOpenList.size() > 0)
//	{
//		Con::printf("pathgrid2d::deleteDebugOpen, size > 0");
		// this is probably slow, but we don't have a good way of making sure a pos
		// is valid within our set

	  if ( mClosedOpenList.get( pos ) )
	  {
//		  Con::printf("--and we have an object with .get( pos )");
	     mClosedOpenList.get( pos )->deleteObject();
		 mClosedOpenList.get( pos ) = NULL;
	  }
	 
//  }
}

//-----------------------------------------------------------------------------
// clearDebugOpenList - clears all existing debug shapes.
//-----------------------------------------------------------------------------
void pathGrid2d::clearDebugOpenList( )
{

//Con::printf("pathGrid2d::clearDebugOpenList, iterating" );
/*
	for ( U32 i = 0; i < mClosedOpenList.size(); i++ )
	{
  	  if ( mClosedOpenList[ i ] )
	  {
		 mClosedOpenList[i]->deleteObject();
	  }
	}
	mClosedOpenList.clear();
*/	
	t2dVector pos;
	for( U32 h = 0; h < mTileCountY; h++ )
	{
		for( U32 w = 0; w < mTileCountX; w++ )
		{
			pos.mX = w;
			pos.mY = h;
			deleteDebugOpen( pos );
		}
	}

}

//-----------------------------------------------------------------------------
// addDebugNode - adds a path node debug shape (triangle)
// takes a local pos
//-----------------------------------------------------------------------------
void pathGrid2d::addDebugNode( const t2dVector &lpos, const F32 angle  )
{
	// clear shape if it exists at that location
	t2dVector outpos;
	localToTile(lpos, outpos);
	 deleteDebugOpen( outpos );

	// add shape
	t2dShapeVector *pShape = new t2dShapeVector();
	pShape->setChild( true );
	pShape->registerObject();
	getSceneGraph()->addToScene(pShape);

	// Blue arrow (Path Node)
	pShape->setPolyCustom( 3, "0 -1 1 1 -1 1" );
	pShape->setSize( t2dVector( mTileLayer->getTileSizeX() * 0.25f, mTileLayer->getTileSizeY() * 0.5f ) );
	pShape->setLineColour(ColorF( 0, 0, 255 ) );
	pShape->setFillColour( ColorF( 0, 0, 255 ) );
	pShape->setRotation( angle );

	// mount to the pathGrid
	pShape->mount( this, lpos, 0, false, true, true, false ); 
	
	// set defaults
	pShape->setFillMode(true);
	pShape->setLineAlpha( 0.5f );
	pShape->setFillAlpha( 0.25f );

	// add the pointer to mDebugShapeList 
	//mDebugShapeList.get( pos ) = pShape;
	mPathNodeList.push_back( pShape );

	return;
}

//-----------------------------------------------------------------------------
// deleteDebugNode - Deletes a debug shape at pos, if one exists there.
//-----------------------------------------------------------------------------
void pathGrid2d::deleteDebugNode( const U32 index )
{
	if ( mPathNodeList[ index ] )
	{
		mPathNodeList[ index ]->deleteObject();
		//mPathNodeList[ index ] = NULL;
		//mPathNodeList.erase_fast( index );
		// UNUSED: JOSEPH THOMAS -> int foo = 0;
	}
}

//-----------------------------------------------------------------------------
// clearDebugNodeList - clears all existing debug shapes.
//-----------------------------------------------------------------------------
void pathGrid2d::clearDebugNodeList( )
{
	//Con::printf("pathGrid2d::clearDebugNodeList, size is (%i)", mPathNodeList.size() );
	for ( U32 i = 0; i < mPathNodeList.size(); i++ )
	{
		deleteDebugNode( i );
	}
	mPathNodeList.clear();
	// UNUSED: JOSEPH THOMAS -> int foo = 0;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
//  pathAStar2d.
//
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
pathAStar2d::pathAStar2d( pathGrid2d* pPathGrid2d, const t2dVector start, const t2dVector dest, const bool cutCorners, const bool optFlag, const bool noDiagonal):	
								mStart( start ),
								mDest( dest ),
								mpPathGrid2dManager( pPathGrid2d ),
								mPathCoords(NULL),
								mOpenNodeQueue( QUEUESIZE, compareNodesDescending )
{
	// Set Vector Associations.
	VECTOR_SET_ASSOCIATION( mPathCoords );
	mNoDiagonal = noDiagonal;
	// calculate ( start, dest, cutCorners, optFlag );
}

//-----------------------------------------------------------------------------
// Default Constructor.
//-----------------------------------------------------------------------------
pathAStar2d::pathAStar2d() :	mpPathGrid2dManager( NULL ),
								mPathCoords(NULL),
								mOpenNodeQueue( QUEUESIZE, compareNodesDescending )
{
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
pathAStar2d::~pathAStar2d()
{
	//clean up stuff
	deletePath();
}

void pathAStar2d::deletePath()
{
	mOpenNodeQueue.clear();
	mPathCoords.clear();
	// Is clear() enough?
}

//-----------------------------------------------------------------------------
// checkCorner - Checks a diagonal cell to see if it's cuttng a corner.
//-----------------------------------------------------------------------------
bool pathAStar2d::checkCornerOk( const t2dVector& cellPos, U32 cornerIndex )
{
	// Pass in the coords of the corner GridCell 
	// and the index of the corner: 
	// 4-1
	// ---
	// 3-2
	// returns false if either adjacent GridCell is blocked

	cornerIndex -= 4; // corners squares start at 4 in the normal path checking order, this lookup array starts at 0

	// test the adjacent cells for passability
	if ( mpPathGrid2dManager->mMapArray.get( cellPos.mX + CORNEROFFSET[cornerIndex][0], cellPos.mY + CORNEROFFSET[cornerIndex][1] ).mPassable == false 
		|| mpPathGrid2dManager->mMapArray.get( cellPos.mX + CORNEROFFSET[cornerIndex][2], cellPos.mY + CORNEROFFSET[cornerIndex][3] ).mPassable == false )
		return false;
	else 
		return true;
}

//-----------------------------------------------------------------------------
// PointOnCurve. 
//-----------------------------------------------------------------------------
void pathAStar2d::pointOnCurve(t2dVector &out, const F32 t, const t2dVector &p0, const t2dVector &p1, const t2dVector &p2, const t2dVector &p3)
{
	F32 t2 = t * t;
	F32 t3 = t2 * t;

	out.mX = 0.5f * ( ( 2.0f * p1.mX ) + ( -p0.mX + p2.mX ) * t +
		( 2.0f * p0.mX - 5.0f * p1.mX + 4 * p2.mX - p3.mX ) * t2 +
		( -p0.mX + 3.0f * p1.mX - 3.0f * p2.mX + p3.mX ) * t3 );

	out.mY = 0.5f * ( ( 2.0f * p1.mY ) +
		( -p0.mY + p2.mY ) * t +
		( 2.0f * p0.mY - 5.0f * p1.mY + 4 * p2.mY - p3.mY ) * t2 +
		( -p0.mY + 3.0f * p1.mY - 3.0f * p2.mY + p3.mY ) * t3 );
}

//-----------------------------------------------------------------------------
// calculate. (a new Path)
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, calculate, bool, 4, 6, "find(startX / startY, destX / destY) - creates a path")
{
	// check parameters
	if ( t2dSceneObject::getStringElementCount(argv[2]) < 2 || t2dSceneObject::getStringElementCount(argv[3]) < 2 )
	{
		Con::warnf("pathGrid2d::calculate( sX / sy , dX dY ) - Invalid number of parameters!");
		return false;
	}

	// set  corner-cutting
	bool cornerFlag = argc >= 4 ? dAtob(argv[4]) : false;
	// set optimise
	bool optFlag = argc >= 5 ? dAtob(argv[5]) : false;

	// set up the coords
	t2dVector start( dAtof(t2dSceneObject::getStringElement(argv[2], 0)), dAtof(t2dSceneObject::getStringElement(argv[2], 1)) );
	t2dVector dest( dAtof(t2dSceneObject::getStringElement(argv[3], 0)), dAtof(t2dSceneObject::getStringElement(argv[3], 1)) );
	// create a pathAStar2d, and return the simobject ID
	return object->calculate( start, dest, cornerFlag, optFlag );
}

//-----------------------------------------------------------------------------
// Calculate. - Calculates a path
// Heap<Node> mOpenNodeQueue - 
//		Node 
//			- t2dVector with added mF data
//			- highest coord in queue is has least F value ( best path )
//			- mChildDir
//			- mChildDist
//
// mpPathGrid2dManager->mMapArray - is a 2d Vector of GridCell
//		GridCell	
//			- stores open/closed status
//			- G (distance to starting GridCell)
//			- H (heuristic to destination)
//			- F ( G + H ) This is the ranking of the path at this GridCell
//			- mPassable - is this GridCell passable
//			- mWeight - weighting value. Higher makes the path less desirable
//
// mPathCoords - Vector of Nodes. This is the generated Path
//
//-----------------------------------------------------------------------------
bool pathAStar2d::calculate( const t2dVector startPosWorld, const t2dVector destPosWorld, const bool cornerFlag, const bool optFlag )
{
	if( mpPathGrid2dManager->mDrawDebug)
	{
		mpPathGrid2dManager->clearDebugSymbols();
	}
	// get the tile coords from the world coords
	Point2I startTilePoint;						// pickTile needs Point2I
	Point2I destTilePoint;
	mpPathGrid2dManager->mTileLayer->pickTile( startPosWorld, startTilePoint );
	mpPathGrid2dManager->mTileLayer->pickTile( destPosWorld,  destTilePoint );

	// Here's where we'll set up the bounding box
	U32 minTileX = 0;
	U32 minTileY = 0;
	U32 tileCountX = mpPathGrid2dManager->getTileCountX();
	U32 tileCountY = mpPathGrid2dManager->getTileCountY();

	// make sure we're on the grid in the first place
	if ( (startTilePoint.x < minTileX || startTilePoint.x >= tileCountX) ||
		 (destTilePoint.y < minTileY || destTilePoint.y >= tileCountY) )
	{
		// we are no longer on the grid
		Con::warnf("PathAStar2d::calculate( sX / sY, dX / dY ) - start or end points are not on the grid");
	    mPathCoords.clear();
	    mPathDistance = 0;
        return false;
	}
	// let's see what happens if we remove this.

	// check if start and end are blocked
	if( mpPathGrid2dManager->mMapArray.get( startTilePoint ).mPassable == false ||
		mpPathGrid2dManager->mMapArray.get( destTilePoint  ).mPassable == false )
	{
		Con::warnf("PathAStar2d::calculate( sX / sY, dX / dY ) - start or end points are blocked");
	    mPathCoords.clear();
	    mPathDistance = 0;
		return false;
	}

	// clear out all old path data
	clear();
	
	// set locals
	Node n;		// cel being processed
	Node s;		// current "successor" cell that is adjacent to n
	F32 sG;
	F32 sH;
	F32 sF;
	Point2I sCellPos;
	
	// enqueue the starting cell
	Node c( startTilePoint );
	mOpenNodeQueue.enqueue( c );

	// main loop
	while( mOpenNodeQueue.size() != 0 )
	{
		// get the top node (least f) n from mOpenNodeQueue
		Node currentCoord = mOpenNodeQueue.item();
		mOpenNodeQueue.dequeue();

		// mark this node (currentCoord) closed (while checking it... it might be re-opened later)
		mpPathGrid2dManager->mMapArray.get( currentCoord ).open = false;
		
		/////// DEBUG ///////////////////////////
		// mark closed
		if ( mpPathGrid2dManager->mDrawDebug )
			mpPathGrid2dManager->addDebugOpen( currentCoord, 1 );

		//	cache the g value for GridCell being checked
		F32 nG = mpPathGrid2dManager->mMapArray.get( currentCoord ).mG;

		//	loop through the current Cells's successors 
		for( U32 step = 0; step < (!mNoDiagonal ? 8 : 4); step++ )
		{			
			// get the successor's x and y
			sCellPos.set( currentCoord.mX + DIRTABLE[step][0], currentCoord.mY + DIRTABLE[step][1] );

			// get a copy of the successor GridCell	
			GridCell sCell = mpPathGrid2dManager->mMapArray.get( sCellPos );

			// prevent corner-cutting, if flagged
			if ( !cornerFlag && step >= 4) // corners are 4, 5, 6, 7
					if ( !checkCornerOk( sCellPos, step ) )
						continue;								// corner blocked
		
			// Do we have a Path?
			if( sCellPos == destTilePoint )
			{
				// Enter the final GridCell info.
				sCell.parent = currentCoord;
				sCell.mG = nG + DISTTABLE[step];
				sCell.mH = aStarHeuristic( t2dVector( sCellPos ), destTilePoint );
				sCell.mF = sCell.mG + sCell.mH;
				sCell.open = true;
				mpPathGrid2dManager->mMapArray.get( sCellPos ) = sCell;								
				
				/////// DEBUG ///////////////////////////
				// mark open
				if ( mpPathGrid2dManager->mDrawDebug )
					mpPathGrid2dManager->addDebugOpen( t2dVector( sCellPos ), 0 );

				// Save out the path to mTileArray
				dumpPath( destTilePoint, startTilePoint, startPosWorld, optFlag ); 
				return true;
			}

			// if the successor GridCell is passable && open && in-bounds
			if( sCell.mPassable == true && sCell.open == true &&
				sCellPos.x >= minTileX && sCellPos.x < tileCountX &&
				sCellPos.y >= minTileY && sCellPos.y < tileCountY )
			{
				// create test values to test against the existing values (if there are any)
				sG = nG + DISTTABLE[step] + sCell.mWeight;
				sH = aStarHeuristic( t2dVector( sCellPos ), destTilePoint );
				sF = sG + sH;
	
				// check mapArray to see if this cell has it been checked yet AND
				// does it already have a shorter route stored?
				if( sCell.parent.mX != -1 && 
					sCell.mG <= sG )
					continue;
				// else this route is shorter, so update the GridCell 
				else
				{
					mOpenNodeQueue.enqueue( Node ( sCellPos.x, sCellPos.y, sF ) );
					sCell.parent = currentCoord;
					sCell.mF = sF;
					sCell.mG = sG;
					sCell.mH = sH;
					sCell.open = true;
					mpPathGrid2dManager->mMapArray.get( sCellPos ) = sCell;
					
					/////// DEBUG ///////////////////////////
					// mark open
					if ( mpPathGrid2dManager->mDrawDebug )
						mpPathGrid2dManager->addDebugOpen( t2dVector( sCellPos ), 0 );
				}
			}
			else
			{
				continue;	// GridCell is not passable or it's closed or it's out of bounds, continue to next GridCell
			}
		}					// Successor loop
	}						
	// Checked all open Nodes, no path found
	// DEBUG: see if we need to do this
	mPathCoords.clear();
	mPathDistance = 0;

	return false;
}

//-----------------------------------------------------------------------------
// clear.
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, clear, bool, 2, 2, "() - Clear Path")
{
	return object->clear();
}

bool pathAStar2d::clear()
{
	// clear the sorted queue
	mOpenNodeQueue.clear();

	// clear the mapArray, reset the passable info from the source tileLayer
	mpPathGrid2dManager->clearMap ( true ); 

	/////// DEBUG ///////////////////////////
	// clear all debug symbols
	if ( mpPathGrid2dManager->mDrawDebug )
		mpPathGrid2dManager->clearDebugOpenList();
	// clear path nodes, if there are any
	if ( mpPathGrid2dManager->mPathNodeList.size() > 0 )
		mpPathGrid2dManager->clearDebugNodeList();
	
	return true;
}

////-----------------------------------------------------------------------------
//// getSteps.
////-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, getSteps, S32, 2, 2, "() - Steps in Path")
{
	return object->getSteps();
}

U32 pathAStar2d::getSteps()
{
	return mPathSteps;
}

//-----------------------------------------------------------------------------
// getDistance.
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, getDistance, F32, 2, 2, "() - Distance of Path")
{
	return object->getDistance();
}

F32 pathAStar2d::getDistance()
{
	return mPathDistance;
}

//-----------------------------------------------------------------------------
// getSplineAngle - returns the angle of a path step (Catmull Rom interp).
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, getSplineAngle, S32, 3, 3, "( step ) - Angle at Path Step")
{
	// input index
	F32 distance = dAtof(argv[2]);
	
	if ( distance > object->getDistance() )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - greater than path distance");
		return -1;
	}
	if ( distance < 0 )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - less than zero");
		return -1;
	}

	object->getSplineAngle( distance, object->mConAngle );
	return object->mConAngle;
}

void pathAStar2d::getSplineAngle( F32 angleDist, F32 &outAngle  )
{
	// setup node and subNode
	F32 angleTarg = angleDist + 10.0f;

	// check if the look-at point out of range, decrement if true
	if ( angleTarg  > getDistance() )
	{
		angleDist -= 10.0f;
		angleTarg -= 10.0f;
	}

	// sets positions
	getSplineWorld( angleDist, mA0 );
	getSplineWorld( angleTarg, mA1 );

	//set outAngle
	mpPathGrid2dManager->getAngle( mA0, mA1, outAngle );
}

//-----------------------------------------------------------------------------
// getLinearAngle - returns the angle of a path step (Catmull Rom interp).
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, getLinearAngle, S32, 3, 3, "( step ) - Angle at Path Step")
{
	// input index
	F32 distance = dAtof(argv[2]);
	
	if ( distance > object->getDistance() )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - greater than path distance");
		return -1;
	}
	if ( distance < 0 )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - less than zero");
		return -1;
	}

	object->getLinearAngle( distance, object->mConAngle );
	return object->mConAngle;
}

void pathAStar2d::getLinearAngle( F32 angleDist, F32 &outAngle )
{
	// setup node and subNode
	F32 angleTarg = angleDist + 10.0f;

	// check if the look-at point out of range, decrement if true
	if ( angleTarg  > getDistance() )
	{
		angleDist -= 10.0f;
		angleTarg -= 10.0f;
	}

	// sets positions
	getLinearWorld( angleDist, mA0 );
	getLinearWorld( angleTarg, mA1 );

	//set outAngle
	mpPathGrid2dManager->getAngle( mA0, mA1, outAngle );
}

//-----------------------------------------------------------------------------
// getSplineWorld - sets mConWorldPos to be the world coord at this distance (Catmull Rom interp).
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, getSplineWorld, const char*, 3, 3, "( distance ) - coord at path step + subStep")
{
	// input distance
	F32 distance = dAtof(argv[2]);
	// check distance is in-bounds
	if ( distance > object->getDistance() )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - greater than path distance");
		return "false";
	}
	if ( distance < 0 )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - less than zero");
		return "false";
	}

	// get the world pos
	object->getSplineWorld( distance, object->mConWorldPos );

	// build returnable buffer (is 256 the size of an t2dVector?)
	char* pBuffer = Con::getReturnBuffer(256);
	//format buffer
	dSprintf(pBuffer, 256, "%f %f", object->mConWorldPos.mX, object->mConWorldPos.mY);
	// Return the t2dVector as buffer.
	return pBuffer;
}

void pathAStar2d::getSplineWorld( const F32 dist, t2dVector &outPos  )
{
	U32 n1;
	F32 nP;
	getCurrentNode( dist, n1, nP );

	// if last step, set coord 
	if ( n1 == mPathSteps )
	{
		mpPathGrid2dManager->localToWorld( mPathCoords[n1], outPos );
		return;
	}	
	
	// set other nodes
	U32 n0 = n1 - 1;
	U32 n2 = n1 + 1;
	U32 n3 = n2 + 1;

	// UNUSED: JOSEPH THOMAS -> int stop1 = 0;

	// check for end conditions
	// next to last node
	if ( n1 == mPathSteps - 1 )
		n3 = n2;
	// start node
	if ( n1 == 0 )
		n0 = 0;

	//set coords
	mpPathGrid2dManager->localToWorld( mPathCoords[n0], mV0 );
	mpPathGrid2dManager->localToWorld( mPathCoords[n1], mV1 );
	mpPathGrid2dManager->localToWorld( mPathCoords[n2], mV2 );
	mpPathGrid2dManager->localToWorld( mPathCoords[n3], mV3 );

	//get curve point
	pointOnCurve( outPos, nP, mV0, mV1, mV2, mV3 );
	// UNUSED: JOSEPH THOMAS -> int stop2 = 0;
}

//-----------------------------------------------------------------------------
// getLinearWorld - sets mConWorldPos to be the world coord at this distance (linear interp).
//-----------------------------------------------------------------------------
ConsoleMethod(pathAStar2d, getLinearWorld, const char*, 3, 3, "( distance ) - coord at path step + subStep")
{
	// input distance
	F32 distance = dAtof(argv[2]);
	// check distance is in-bounds
	if ( distance > object->getDistance() )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - greater than path distance");
		return "false";
	}
	if ( distance < 0 )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - less than zero");
		return "false";
	}

	// get the world pos
	object->getLinearWorld( distance, object->mConWorldPos );

	// build returnable buffer (is 256 the size of an t2dVector?)
	char* pBuffer = Con::getReturnBuffer(256);
	//format buffer
	dSprintf(pBuffer, 256, "%f %f", object->mConWorldPos.mX, object->mConWorldPos.mY);
	// Return the t2dVector as buffer.
	return pBuffer;
}
void pathAStar2d::getLinearWorld( const F32 dist, t2dVector &outPos )
{
	U32 n0;
	F32 nP;
	getCurrentNode( dist, n0, nP );

	// if last step, set coord 
	if ( n0 == mPathSteps )
	{
		mpPathGrid2dManager->localToWorld( mPathCoords[n0], outPos );
		return;
	}	

	// set node coord
	mpPathGrid2dManager->localToWorld( mPathCoords[n0], mV0 );
	mpPathGrid2dManager->localToWorld( mPathCoords[n0+1], mV1 );

	// lerp to result
	mV0.lerp( mV1, nP, outPos );
}

ConsoleMethod(pathAStar2d, getNodeWorldCoords, const char*, 3, 3, "(nodeIndex) -- returns the world Coordinates of a node")
{
	U32 nodeIndex = dAtoi(argv[2]);
	return (object->getNodeWorldCoords(nodeIndex));
}

char * pathAStar2d::getNodeWorldCoords(U32 nodeIndex)
{
	char * ret = Con::getReturnBuffer(64);
	if (  ( nodeIndex < 0 ) ||
		  ( nodeIndex > mPathSteps) ||
		  ( nodeIndex >= mPathCoords.size() ))
	{
		Con::errorf("pathAStart2d::getNodeWorldCoords() -- node (%i) does not exist!", nodeIndex);
		dSprintf(ret, 64, "InvalidNode");
		return ret;
	}

	t2dVector worldCoords;
	mpPathGrid2dManager->localToWorld(mPathCoords[nodeIndex], worldCoords);
	dSprintf(ret, 64, "%f %f", worldCoords.mX, worldCoords.mY);
	return ret;
}
char * pathAStar2d::getPathCoordsList()
{
	char * ret = Con::getReturnBuffer( 4096 );
	char* p = ret;
    t2dVector worldCoords;
	for ( U32 n = 0; n < mPathSteps; n++ )
	{
		char coordbuff[64];
		mpPathGrid2dManager->localToWorld( mPathCoords[n], worldCoords );
		dSprintf(coordbuff, 64, "%f %f ", worldCoords.mX, worldCoords.mY);
		dStrcpy(p, coordbuff);
		p += dStrlen(coordbuff);
	}
    p--;
    p[0] = '\0';

	return ret;
}
ConsoleMethod(pathAStar2d, getPathCoordsList, const char*, 2, 2, "returns a list of all node coordinates in world space")
{

	return object->getPathCoordsList();
}


Node pathAStar2d::getNodeByIndex(U32 nIndex)
{
	if (  (nIndex < 0) ||
		  (nIndex >= mPathSteps) )
	{
		Con::errorf("Invalid node index (%i)!", nIndex);
		return Node(0,0,0);
	}
	return (mPathCoords[nIndex]);
}
//-----------------------------------------------------------------------------
// getCurrentNode	- nIndex is the node previous to the position
//					- nPercent is the percent between nIndex and the next node
//-----------------------------------------------------------------------------
void pathAStar2d::getCurrentNode( const F32 distance, U32& nIndex, F32& nPercent )
{
	// check if out of bounds
	if ( distance > mPathDistance )
	{
		Con::warnf("pathAStar2d::getCurrentNode( distance ) - greater than path distance");
		return;
	}

	// iterate through nodes, incrementing distance until we find the node
	F32 d = 0;

	for ( U32 n = 0; n < mPathSteps; n++ )
	{
		//d += mPathCoords[n].mChildDist;
		if ( ( d + mPathCoords[n].mChildDist ) >= distance )
		{
			nIndex = n;	// this is the node we want
			nPercent = ( distance - d ) / mPathCoords[n].mChildDist;
			return;
		}
		d += mPathCoords[n].mChildDist;
	}
}

//-----------------------------------------------------------------------------
// dumpPath - saves out a created path to mPathArray.
//-----------------------------------------------------------------------------
void pathAStar2d::dumpPath( const t2dVector& destTilePoint, const t2dVector& startTilePoint, const t2dVector& startPosWorld, const bool optFlag )
{

	mPathCoords.clear();
	mPathDistance = 0;
	
	// set up locals
	Node pathNode;
	t2dVector nodePosTile  = destTilePoint;
	AssertFatal( nodePosTile == destTilePoint, "nodePosTile != destTilePoint" );
	t2dVector nodePosLocal;
	t2dVector nodePosWorld;
	t2dVector childPosWorld;

	// Set up last node (reverse angle calculation since it has no child)
	mpPathGrid2dManager->tileToLocal( nodePosTile,  nodePosLocal  );				
	mpPathGrid2dManager->localToWorld( nodePosLocal,  nodePosWorld  );

	pathNode.mChildDist = 0;																// last node, no child
	t2dVector destParentTile = mpPathGrid2dManager->mMapArray.get( nodePosTile ).parent;
	t2dVector destParentWorld;
	t2dVector destParentLocal;
	mpPathGrid2dManager->tileToLocal( destParentTile,  destParentLocal  );				
	mpPathGrid2dManager->localToWorld( destParentLocal,  destParentWorld  );

	pathNode.mChildDir  = mpPathGrid2dManager->getAngle( destParentWorld, nodePosWorld );		// angle from dest parent to dest
		
	pathNode = nodePosLocal;																// only copies  mX and mY.
	mPathCoords.push_front( pathNode ); 
		
	/////// DEBUG ///////////////////////////
	// mark node
	if ( mpPathGrid2dManager->mDrawDebug )
		mpPathGrid2dManager->addDebugNode( pathNode, pathNode.mChildDir );

	// done with last node, set up for the rest of the path
	// store the child world pos for next loop
	childPosWorld = nodePosWorld;					
	
	// get the next node, and calculate it's local and world positions
	nodePosTile = mpPathGrid2dManager->mMapArray.get( nodePosTile ).parent;
	mpPathGrid2dManager->tileToLocal( nodePosTile, nodePosLocal );
	mpPathGrid2dManager->localToWorld( nodePosLocal, nodePosWorld  );

	// Dump the path into mPathCoords
	while( nodePosTile != startTilePoint )
	{
		// set child distance and direction values
		t2dVector vDist( nodePosWorld - childPosWorld);
		pathNode.mChildDist = vDist.len();		
		mPathDistance += pathNode.mChildDist;		
		pathNode.mChildDir  = mpPathGrid2dManager->getAngle( nodePosWorld, childPosWorld );	

		// copy over the local coord to pathNode, and push 
		pathNode = nodePosLocal;			
		mPathCoords.push_front( pathNode ); 

		/////// DEBUG ///////////////////////////
		// mark node
		if ( mpPathGrid2dManager->mDrawDebug )
			mpPathGrid2dManager->addDebugNode( pathNode, pathNode.mChildDir );
		
		// store the child world pos for next loop
		childPosWorld = nodePosWorld;					
		
		// get the next node, and calculate it's local and world positions
		nodePosTile = mpPathGrid2dManager->mMapArray.get( nodePosTile ).parent;
		mpPathGrid2dManager->tileToLocal( nodePosTile, nodePosLocal );
		mpPathGrid2dManager->localToWorld( nodePosLocal, nodePosWorld  );
	}

	// Set up start node - Note: not using nodePosWorld anymore. Use startPosWorld (the real startPos)
	// set child distance and direction values
	t2dVector vDist( startPosWorld - childPosWorld);
	pathNode.mChildDist = vDist.len();				
	mPathDistance += pathNode.mChildDist;
	pathNode.mChildDir  = mpPathGrid2dManager->getAngle( startPosWorld, childPosWorld );	
	
	// copy over the local coord of startPosWorld to pathNode, and push 
	pathNode = mpPathGrid2dManager->mTileLayer->getLocalPoint( startPosWorld );		
	mPathCoords.push_front( pathNode ); 

	/////// DEBUG ///////////////////////////
	// mark node
	if ( mpPathGrid2dManager->mDrawDebug )
		mpPathGrid2dManager->addDebugNode( pathNode, pathNode.mChildDir );

	mPathSteps = mPathCoords.size() - 1;
	if ( optFlag )
		optimizePath();

	return;												
}

//-----------------------------------------------------------------------------
// optimizePath - returns true if node on either side is the same angle as this one.
//-----------------------------------------------------------------------------
void pathAStar2d::optimizePath()
{
	// locals
	U32 n = 0;
	F32 angPrev;
	F32 angThis;
	F32 angNext;

	// test angles, remove surplus nodes
	while ( n < mPathCoords.size() - 2 )
	{
		angPrev = mFloor( mPathCoords[n].mChildDir + 0.5f );
		angThis = mFloor( mPathCoords[n+1].mChildDir + 0.5f );
		angNext = mFloor( mPathCoords[n+2].mChildDir + 0.5f );

		if ( rangeComp( angPrev, angThis, 5 ) )
		{
			// set the distance of 0 to the distance to +2
			mPathCoords[n].mChildDist += mPathCoords[n+1].mChildDist;
			mPathCoords.erase( n + 1 );
		}
		else
		{
			n++;
		}
	}
	mPathSteps = mPathCoords.size() - 1;

	/////// DEBUG ///////////////////////////
	// mark node
	if ( mpPathGrid2dManager->mDrawDebug )
	{
		mpPathGrid2dManager->clearDebugNodeList();
		for ( U32 n = 0; n < mPathCoords.size(); n++ )
		{
			mpPathGrid2dManager->addDebugNode( mPathCoords[n], mPathCoords[n].mChildDir );
		}
	}
}
