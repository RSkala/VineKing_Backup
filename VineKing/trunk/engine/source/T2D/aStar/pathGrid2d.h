/*
----------------------------------------
pathGrid2D
----------------------------------------
	-This is the object that all paths are based on. Each grid can have many path objects (which are just holders for the generated path)
	-I imagine at some point that there will be other types of paths besides A-Star, but I'm not sure how to work that. They might need
	other kinds of grids than what mMapArray is designed to do.
	-Uses a several classes that inherit from Vector. 
		Heap - efficiently sorts searched nodes.
		Vector2d - simply adds a get( x, y ) and resize( x, y ) function, to make it easier to treat it as a 2d structure.
		Queue - simply adds a queue and dequeue function to make it work like the Heap.

		Node inherits from t2dVector, just adds an "F" value for sorting searched nodes. Used by Heap to store open and closed nodes
		GridCell is a simple class for storing node G, H, F, parent, passabiliy, weight, etc. This is basis of the path-finding "grid"


----------------------------------------
pathAStar2D 
----------------------------------------
	-Contains a generated path. It also contains the actual pathfinding code.
	NOTE that the path could contain invalid coordinates, if the tileLayer is resized after a path is made. Oh well. 
	The benefit to allowing this is that the pathGrid can be moved around and old paths will still work. I don't know 
	My intention is to allow users to "move" the grid around by specifying regions on the source t2dTileLayer, making paths 
	wherever they need them This way, user could have a very large tileLayer, and easily limit searches to contained
	(for example, screen-sized) areas. Since the path coords are stored separately from the grid, they will still work no matter
	where the grid happens to "be". 

TO DO:
	-Make sure that everytime a new path is made, that it checks to make sure that the mMapArray is still the same size as mTileLayer. 
	If it is different, then reset the whole array as default behavior. There are potential performance issues with this, but better than crashing
	-Allow the pathGrid to be able to use a limited section of the tileLayer. This would let the user "move" the grid around on a much larger tileLayer.
	-Make the pathGrid check if the tileLayer is scrolling or wrapping. What to do? It won't crash, but the path won't really be useful...
	-Figure out a way to pass world coods from the path. Ideally, this would work even if the tileLayer is rotated.
	-Add functions to set what values are "passable" and "blocked". Scale weigthing values between them.
*/	

//-------------------------------------------------------------------------------------------------
// Phil Shenk
// pathGrid2d		- "Grid" for creating paths on. References a t2dTileLayer for passability info
// pathAStar2d		- "Path" created and stored in pathGrid2d. Many paths can be on one grid
//-------------------------------------------------------------------------------------------------

#ifndef _PATHGRID2D_H_
#define _PATHGRID2D_H_

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _T2DTILEMAP_H_
#include "T2D/t2dTileMap.h"
#endif

#ifndef _PATHCLASSES_H_
#include "./pathClasses.h"
#endif

#ifndef _PATHUTILITIES_H_
#include "./pathUtilities.h"
#endif

#ifndef _T2DVECTOR_H_
#include "T2D/t2dVector.h"
#endif

#ifndef _VECTOREXT_H
#include "./tVectorExtension.h"
#endif

#ifndef _T2DSHAPEVECTOR_H_
#include "T2D/t2dShapeVector.h"
#endif


//#ifndef _T2DVECTORSHAPE_H_
//#include "./t2dVectorShapes.h"
//#endif

class pathAStar2d;								// forward declaration

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//  Path Grid 2D
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class pathGrid2d : public t2dSceneObject
{	
	typedef t2dSceneObject		Parent;

private:
	U32							mTileCountX;		// x count of tile layer. Have to set this every time path is made since layer can change
	U32							mTileCountY;		// y count of tile layer
	
	t2dTileLayer*				mTileLayer;			// the tileLayer that the path is based on. The path will look at tileCustomData for what
													// it can pass through.
													// 0				= passable
													// 10				= not passable
													// 1-9				= weight of tile
	Vector2d<GridCell>			mMapArray;			// for creating the path. Gets passable and weight data from mTileLayer	
	Vector<pathAStar2d*>		mPathList;			// list of active paths
	Vector2d<t2dShapeVector*>	mClosedOpenList;	// debug "closed/open" t2dShapeVector* 
	Vector<t2dShapeVector*>		mPathNodeList;		// debug "pathnode" t2dShapeVector*
	bool						mDrawDebug;			// draw the debug symbols?	

	// vars for local operations
	t2dVector					mLocalPos;			// stores current local pos
	t2dVector					mHalfSize;			// stores halfsize 

	// closed/open node debug shapes
	void addDebugOpen( const t2dVector &pos, const U32 index );				// creates an "open" debug shape
	void deleteDebugOpen( const t2dVector& pos );							// deletes a debug shape, if it exists
	void clearDebugOpenList();												// clears all the "open" debug shapes

	// path node debug shapes
	void addDebugNode( const t2dVector &pos, const F32 angle = 0.0f );		// creates a "node" debug shape
	void deleteDebugNode( const U32 index );								// deletes a debug shape, if it exists
	void clearDebugNodeList();												// clears all the "node" debug shapes

public:
	friend class pathAStar2d;

	pathGrid2d( );
	pathGrid2d( t2dTileLayer *const pLayer );		
	virtual ~pathGrid2d();								
	
	// setup
	bool setTileLayer( t2dTileLayer *const pLayer );	// sets mTileLayer, and all appropriate data members (will resize also)
	t2dTileLayer* getTileLayer() {return mTileLayer;};
	// Accessors
	U32 getTileCountX();
	U32 getTileCountY();
	void setDebugDraw( const bool );					// set the flag			
	void clearDebugSymbols();
    void					setWeight( U32 xTile, U32 yTile, U32 newWeight );
	U32						getWeight(U32 xTile, U32 yTile);
	// Path Object Retrieval
	pathAStar2d* findPath( U32 pathID ) const;										// returns pointer to a pathAStar2d
	S32 findPathIndex( pathAStar2d* pPath ) const;									// returns index of path

	//Path Object Management
	SimObjectId createPath( const t2dVector start, const t2dVector dest, const bool cornerCut = false, const bool optFlag = false );
	bool deletePath( const U32 pathIndex );											// deletes path by index
	bool deletePathId( const SimObjectId pathID );									// deletes path by ID	
	bool deleteAllPaths(void);														// deletes all paths
	U32  getPathCount(void);														// reutrns the number of active paths
	SimObjectId getPath( const U32 pathIndex );										// returns an object ID for the path at index 
	
	// Utility
	void tileToLocal ( const t2dVector &tPosIn, t2dVector &lPosOut );				// converts tile x/y coord to pivot offset for mTileLayer
	void localToWorld( const t2dVector &lPos, t2dVector &outPos );					// converts tile x/y coord to world coord
	void localToTile ( const t2dVector &lPos, t2dVector &outPos ); 
	void getAngle( const t2dVector &vIn1, const t2dVector &vIn2, F32 &angleOut );	// sets angleOut of vIn1 to vIn2
	F32  getAngle( const t2dVector &vIn1, const t2dVector &vIn2 );					// overloaded, returns angle.
	void clearMap( const bool getCustomData = true );								// clears mMapArray. Pass "true" to reinit the passable data
	void getPassData( const Point2I &tPos );
	bool isLayer();																	// checks if there is a tileLayer set or not

	bool mNoDiagonal;

	DECLARE_CONOBJECT( pathGrid2d);													// makes class creatable from console

	StringTableEntry			mpathGrid2d;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
//  Path A-Star 2D
//
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
class pathAStar2d : public t2dSceneObject
{
	typedef t2dSceneObject	Parent;

private:
	t2dVector				mStart;											// coords for start 
	t2dVector				mDest;											// coords for end
	U32						mPathSteps;										// number of path steps
	F32						mPathDistance;									// distance of path, in world units
	pathGrid2d*				mpPathGrid2dManager;							// pointer to the pathGrid2d object
	Vector<Node>			mPathCoords;									// Vector(derived to Heap) of type Node that makes up the path
	Heap<Node>				mOpenNodeQueue;

	// Utility	
	void					deletePath();												// cleanup
	bool					checkCornerOk(const t2dVector&, const U32 );				// checks a diagonal cell to see if it's cuttng a corner
	void					pointOnCurve(t2dVector &, const F32, const t2dVector &,		// Catmull-Rom point
							const t2dVector &, const t2dVector &, const t2dVector &);
	void					getCurrentNode( const F32, U32&, F32& );					// gets the last path node
	void					dumpPath( const t2dVector&, const t2dVector&, const t2dVector&, const bool );
	void					optimizePath();

	// for pass-by-reference in member functions  
	t2dVector				mLocalPos;
	t2dVector				mCurvePoint;
	t2dVector				mV0;										
	t2dVector				mV1;										
	t2dVector				mV2;										
	t2dVector				mV3;										
	t2dVector				mA0;
	t2dVector				mA1;

public:	
	friend class pathGrid2d;

	pathAStar2d( pathGrid2d* pPathGrid2d, const t2dVector start, const t2dVector dest, const bool cutCorners, const bool optFlag, const bool noDiagonal );			
	pathAStar2d();							
	virtual ~pathAStar2d();		

	// Utility
	bool calculate( const t2dVector start, const t2dVector dest,					// calculates a path (erases old path)
		const bool cornerCut = false, const bool optimise = true ); 
	bool clear();																	// clears all data and debug sympbols

	// Accessors
	U32						getSteps();												// get number of path steps
	F32						getDistance();											// gets path distance, in world units
	void					getSplineWorld( const F32 dist, t2dVector &outPos );	// sets outPos at world distance along path (Catmull-Rom interp)
	void					getLinearWorld( const F32 dist, t2dVector &outPos );	// sets outPos at world distance along path (Linear interp)
	void					getSplineAngle( F32 dist, F32 &outAngle );				// sets outAngle at world distance along path (Catmull-Rom interp)
	void					getLinearAngle( F32 angleDist, F32 &outAngle );			// sets outAngle at world distance along path (Linear interp)
    Node					getNodeByIndex(U32 nIndex);
    char*					getPathCoordsList();
    char*					getNodeWorldCoords(U32 nodeIndex);
	// public members for console access (pass-by-ref)
	t2dVector				mConWorldPos;								// stores world pos. 
	F32						mConAngle;									// stores angle at a path coord. 
	bool mNoDiagonal;
	DECLARE_CONOBJECT( pathAStar2d);		
};

#endif