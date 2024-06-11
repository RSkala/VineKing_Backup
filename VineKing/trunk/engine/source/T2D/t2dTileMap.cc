//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Tilemap Object.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "math/mMathFn.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dTileMap.h"
#include "core/frameAllocator.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dTileMap);
IMPLEMENT_CONOBJECT(t2dTileLayer);


//-----------------------------------------------------------------------------
//
//  t2dTileMap.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dTileMap::t2dTileMap() :  T2D_Stream_HeaderID(makeFourCCTag('2','D','T','M'))
{
   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( mTileLayerList );
   VECTOR_SET_ASSOCIATION( mTileNodes );
   VECTOR_SET_ASSOCIATION( mAnimationControllerList );
   VECTOR_SET_ASSOCIATION( mActiveTileList );

   // Picking is not Allowed!
   Parent::mPickingAllowed = false;
   // Collisions not Allowed!
   Parent::mCollisionSendAllowed = Parent::mCollisionReceiveAllowed = false;
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dTileMap::~t2dTileMap()
{
   // Delete All Tile-Layers.
   deleteAllTileLayers();
}

//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dTileMap::onAdd()
{
   // Eventually, we'll need to deal with Server/Client functionality!

   // Call Parent.
   if(!Parent::onAdd())
      return false;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dTileMap::onRemove()
{
   // Delete All Tile-Layers.
   deleteAllTileLayers();

   // Call Parent.
   Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Delete All Tile-Layers.
//-----------------------------------------------------------------------------
void t2dTileMap::deleteAllTileLayers(void)
{
   // Delete All Tile Layers.
   while ( mTileLayerList.size() > 0 )
      // Delete the Object.
      mTileLayerList[0]->deleteObject();

   // Clear the list just in case.
   mTileLayerList.clear();

   // This isn't actually empty sometimes, so I'm forcing it for now. ADL.
   mTileNodes.clear();
   mAnimationControllerList.clear();
   mActiveTileList.clear();

   // Sanity!
   AssertFatal( mTileNodes.size() == 0,                "t2dTileMap::deleteAllTileLayers() - Some Tile-Nodes were not removed correctly!" );
   AssertFatal( mAnimationControllerList.size() == 0,  "t2dTileMap::deleteAllTileLayers() - Some Animation-Controllers were not removed correctly!" );
   AssertFatal( mActiveTileList.size() == 0,           "t2dTileMap::deleteAllTileLayers() - Some Active-Tiles were not removed correctly!" );
}

//
// Delete notify's will take care of list removal. - JDD
//
void t2dTileMap::onDeleteNotify(SimObject* object)
{
   t2dTileLayer *pTileLayer = dynamic_cast<t2dTileLayer*>( object );  
   if( !pTileLayer )
      return;

   t2dTileLayer *pTileLookup = findTileLayer( pTileLayer->getId() );
   U32 layerIndex = findTileLayerIndex( pTileLayer );

   if( pTileLookup && layerIndex != -1 && pTileLookup == pTileLayer )
      // Remove Tile-Layer from list.
      mTileLayerList.erase( layerIndex );

};


//-----------------------------------------------------------------------------
// Create Tile Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, createTileLayer, S32, 3, 6, "(int tileCountX, int tileCountY, float tileSizeX, float tileSizeY) - Creates a Tile Layer.\n"
													  "@param tileCountX The number of tiles in the X direction\n"
													  "@param tileCountY The number of tiles in the Y direction\n"
													  "@param tileSizeX The width of an individual tile\n"
													  "@param tileSizeY The height of an individual tile\n"
													  "@return Returns either the ID of the new layer or 0 on failure")
{
   U32 tileCountX, tileCountY;
   F32 tileSizeX, tileSizeY;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("tileCountX tileCountY tileSizeX tileSizeY")
   if ((elementCount1 == 4) && (argc == 3))
   {
      tileCountX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileCountY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      tileSizeX = dAtof(t2dSceneObject::getStringElement(argv[2], 2));
      tileSizeY = dAtof(t2dSceneObject::getStringElement(argv[2], 3));
   }

   // ("tileCountX tileCountY", "tileSizeX tileSizeY")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      tileCountX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileCountY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      tileSizeX = dAtof(t2dSceneObject::getStringElement(argv[3], 0));
      tileSizeY = dAtof(t2dSceneObject::getStringElement(argv[3], 1));
   }

   // (tileCountX, tileCountY, tileSizeX, tileSizeY)
   else if (argc == 6)
   {
      tileCountX = dAtoi(argv[2]);
      tileCountY = dAtoi(argv[3]);
      tileSizeX = dAtof(argv[4]);
      tileSizeY = dAtof(argv[5]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileMap::createTileLayer() - Invalid number of parameters!");
      return 0;
   }

   // Save TileMap File.
   return object->createTileLayer(tileCountX, tileCountY, tileSizeX, tileSizeY);
}
// Create Tile Layer.
SimObjectId t2dTileMap::createTileLayer( U32 tileCountX, U32 tileCountY, F32 tileSizeX, F32 tileSizeY )
{
   // We *must* have a scenegraph!
   if ( getSceneGraph() == NULL )
   {
      Con::warnf("t2dTileMap::createTileLayer() - Cannot process without SceneGraph!");
      return 0;
   }

   // Check Dimensions.
   if ( tileCountX < 1 || tileCountY < 1 )
   {
      Con::warnf("t2dTileMap::createTileLayer() - Invalid Tile Count [%d/%d]", tileCountX, tileCountY);
      return 0;
   }

   // Check Dimensions.
   if ( tileSizeX <= 0.0f || tileSizeY <= 0.0f )
   {
      Con::warnf("t2dTileMap::createTileLayer() - Invalid Tile Size [%f/%f]", tileSizeX, tileSizeY);
      return 0;
   }

   // Generate Tile Layer.
   t2dTileLayer* pTileLayer = new t2dTileLayer(this);
   // Configure Layer.
	printf(" ========== IN t2dTileMap::createTileLayer(), CALLING pTileLayer->createLayer()\n" ); // RKS: I added this line to search for the memory leak possibly caused by "createLayer"
   pTileLayer->createLayer( tileCountX, tileCountY, tileSizeX, tileSizeY );
   // Set as Child.
   pTileLayer->setChild( true );
   // Register this Object.
   pTileLayer->registerObject();
   // We shouldn't be in a scene.
   AssertFatal( !pTileLayer->getSceneGraph(), "createTileLayer() - Child is already in a scenegraph!" );
   // Add it to our Scene Graph.
   getSceneGraph()->addToScene( pTileLayer );
   // Set Layer same as parent.
   pTileLayer->setLayer(getLayer());
   // Notify of Object Deletion
   deleteNotify( pTileLayer );
   // Reference Tile Layer.
   mTileLayerList.push_back( pTileLayer );

   // Return Object ID.
   return pTileLayer->getId();
}

ConsoleMethod(t2dTileMap, addTileLayer, void, 3, 3, "(t2dTileLayer layer) - Adds a layer to the map\n"
													"@param layer The layer to add to the tilemap\n"
													"@return No return value.")
{
   t2dTileLayer* layer = dynamic_cast<t2dTileLayer*>(Sim::findObject(argv[2]));
   if (layer)
      object->addTileLayer(layer);
   else
      Con::errorf("Unable to find layer %s for t2dTileMap::addTileLayer", argv[2]);
}

void t2dTileMap::addTileLayer(t2dTileLayer* pTileLayer)
{
   // If we've already got a tilemap, make sure we clear
   // our node references from it.
   if( pTileLayer->mpTileMap2DManager != this && pTileLayer->mpTileMap2DManager != NULL )
   {
      t2dTileMap *pTileMap = pTileLayer->mpTileMap2DManager;
      pTileMap->removeTileLayer( pTileLayer );
   }

   pTileLayer->mpTileMap2DManager = this;
   pTileLayer->setChild( true );
   getSceneGraph()->addToScene( pTileLayer );
   // Notify of Object Deletion
   deleteNotify( pTileLayer );
   mTileLayerList.push_back( pTileLayer );
}


ConsoleMethod(t2dTileMap, removeTileLayer, void, 3, 3, "(t2dTileLayer layer) Removes a layer from the map\n"
													   "@param layer The layer to remove\n"
													   "@return No return value.")
{
   t2dTileLayer* layer = dynamic_cast<t2dTileLayer*>(Sim::findObject(argv[2]));
   if (layer)
      object->removeTileLayer(layer);
   else
      Con::errorf("Unable to find layer %s for t2dTileMap::removeTileLayer", argv[2]);
}

void t2dTileMap::removeTileLayer(t2dTileLayer* pTileLayer)
{
   if( !pTileLayer )
      return;

   if( pTileLayer->mpTileMap2DManager != this )
      return;

   // Find Layer Index.
   S32 layerIndex = findTileLayerIndex( pTileLayer );
   if( layerIndex == -1 )
      return;

   // Restore Child Flag
   pTileLayer->setChild( false );
   
   // Clear Delete Notify
   clearNotify( pTileLayer );

   // Make sure we dereference our imagemaps for our nodes.
   pTileLayer->clearLayer();

   // Remove from list
   mTileLayerList.erase_fast( layerIndex );
}


//-----------------------------------------------------------------------------
// Deletes All Tile Layer(s).
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, deleteAllLayers, void, 2, 2, "() Deletes all Tile Layer(s).\n"
			  "@return No return value.")
{
   // Delete All Tile-Layers.
   object->deleteAllTileLayers();
}


//-----------------------------------------------------------------------------
// Delete Tile Layer 
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, deleteTileLayer, void, 3, 3, "(int layerIndex) - Deletes a Tile Layer.\n"
													   "@param layerIndex The index of the layer to delete\n" 
													   "@return No return value.")
{
   // Delete Tile Layer.
   object->deleteTileLayer( dAtoi(argv[2]) );
}
// Delete Tile Layer.
void t2dTileMap::deleteTileLayer( U32 layerIndex )
{
   // Get Layer Count.
   U32 layerCount = getTileLayerCount();

   // Check Layer Index.
   if ( layerIndex >= layerCount )
   {
      // Warn.
      Con::warnf("t2dTileMap::deleteTileLayer() - Invalid Tile Layer '%d'!  Currently '%d' Layers available.", layerIndex, layerCount);   
      return;
   }

   // Delete Layer Index Object.
   mTileLayerList[layerIndex]->deleteObject();

   // Remove Tile-Layer from list.
   mTileLayerList.erase( layerIndex );
}

//-----------------------------------------------------------------------------
// Delete Tile Layer by Id.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, deleteTileLayerId, void, 3, 3, "(int tileLayerID) - Deletes a Tile Layer (by Id).\n"
														 "@param tileLayerID The ID of the layer to delete\n"
														 "@return No return value.")
{
   // Delete Tile Layer Id.
   object->deleteTileLayerId( dAtoi(argv[2]) );
}
// Delete Tile Layer Id.
void t2dTileMap::deleteTileLayerId( SimObjectId tileLayerID )
{
   // Find Layer.
   t2dTileLayer* pTileLayer = findTileLayer( tileLayerID );
   // Check Layer.
   if ( !pTileLayer )
   {
      // Warn.
      Con::warnf("t2dTileMap::deleteTileLayer() - Invalid TileLayer ID! (%d)", tileLayerID);
      // Return Here.
      return;
   }

   // Find Layer Index.
   S32 layerIndex = findTileLayerIndex( pTileLayer );
   // Sanity.
   AssertFatal( layerIndex != -1, "t2dTileMap::deleteTileLayer() - Could not find Tile-Layer!" );

   // Delete Tile Layer.
   deleteTileLayer( layerIndex );
}


//-----------------------------------------------------------------------------
// Get Tile Layer Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, getTileLayerCount, S32, 2, 2, "() Get the number of layers in the tilemap.\n"
													    "@return (int layerCount) The number of layers in the tilemap")
{
   // Get Tile Layer Count.
   return object->getTileLayerCount();
}
// Get Tile Layer Count.
U32  t2dTileMap::getTileLayerCount(void)
{
   // Return Tile Layer Count.
   return mTileLayerList.size();
};


//-----------------------------------------------------------------------------
// Get Tile Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, getTileLayer, S32, 3, 3, "(int layerIndex) - Get a Tile Layer from the tilemap by Index.\n"
												   "@param layerIndex The index of the layer you want to get\n"
												   "@return (t2dTileLayer layer) the layer you requested")
{
   // Get Tile Layer.
   return object->getTileLayer( dAtoi(argv[2]) );
}
// Get Tile Layer.
SimObjectId t2dTileMap::getTileLayer( U32 layerIndex )
{
   // Get Layer Count.
   U32 layerCount = getTileLayerCount();

   // Check Layer Index.
   if ( layerIndex >= layerCount  )
   {
      // Warn.
      Con::warnf("t2dTileMap::getTileLayer() - Invalid Tile Layer '%d'!  Currently '%d' Layers available.", layerIndex, layerCount);  
      return -1;
   }

   // Return Tile Layer ID.
   return mTileLayerList[layerIndex]->getId();
}


//-----------------------------------------------------------------------------
// Set All Layers To Parent.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, setAllLayersToParent, void, 2, 2, "() - Synchronizes all tile-layers to parents position/size/rotation."
			  "@return No return value.")
{
   // Set All Layers To Parent.
   return object->setAllLayersToParent();
}
// Sync All Layers To Parent.
void t2dTileMap::setAllLayersToParent( void )
{
   // Fetch Parent Spatial Attributes.
   const t2dVector position = getPosition();
   const t2dVector size = getSize();
   const F32 rotation = getRotation();

   // Process All Layers.
   for ( U32 n = 0; n < mTileLayerList.size(); ++n )
   {
      mTileLayerList[n]->setPosition( position );
      mTileLayerList[n]->setSize( size );
      mTileLayerList[n]->setRotation( rotation );
   }
}


//-----------------------------------------------------------------------------
// Find Tile Layer.
//-----------------------------------------------------------------------------
t2dTileLayer* t2dTileMap::findTileLayer( U32 tileLayerID ) const
{
   // Quit early if no tile layers.
   if ( mTileLayerList.size() == 0 )
      return NULL;

   for ( U32 n = 0; n < mTileLayerList.size(); n++ )
      if ( mTileLayerList[n]->getId() == tileLayerID )
         return mTileLayerList[n];

   // Return "Not Found".
   return NULL;
}


//-----------------------------------------------------------------------------
// Find Tile-Layer Node Index.
//-----------------------------------------------------------------------------
S32 t2dTileMap::findTileLayerIndex( t2dTileLayer* pTileLayer ) const
{
   // Search for identical Tile-Layer.
   for ( U32 n = 0; n < mTileLayerList.size(); n++ )
      // Return node index if match...
      if (  mTileLayerList[n] == pTileLayer )
         return n;

   // Return "No Match!".
   return -1;
}


//-----------------------------------------------------------------------------
// Find Root-Tile Node Index.
//-----------------------------------------------------------------------------
S32 t2dTileMap::findRootTileNodeIndex( cRootTileNode* pRootTileNode ) const
{
   // Search for identical Root Tile Node.
   for ( U32 n = 0; n < mTileNodes.size(); n++ )
      // Return node index if match...
      if (  mTileNodes[n] == pRootTileNode )
         return n;

   // Return "No Match!".
   return -1;
}


//-----------------------------------------------------------------------------
// Find Animation-Controller Node Index.
//-----------------------------------------------------------------------------
S32 t2dTileMap::findAnimationControllerIndex( t2dAnimationController* pAnimationController ) const
{
   // Search for identical Animation Controller.
   for ( U32 n = 0; n < mAnimationControllerList.size(); n++ )
      // Return index if match...
      if (  mAnimationControllerList[n] == pAnimationController )
         return n;

   // Return "No Match!".
   return -1;
}


//-----------------------------------------------------------------------------
// Find Active-Tile Node Index.
//-----------------------------------------------------------------------------
S32 t2dTileMap::findActiveTileIndex( t2dActiveTile* pActiveTile2D ) const
{
   // Search for identical Active Tile.
   for ( U32 n = 0; n < mActiveTileList.size(); n++ )
      // Return index if match...
      if (  mActiveTileList[n] == pActiveTile2D )
         return n;

   // Return "No Match!".
   return -1;
}


//-----------------------------------------------------------------------------
// Find Static Tile Node.
//-----------------------------------------------------------------------------
t2dTileMap::cStaticTileNode* t2dTileMap::findStaticTileNode( t2dImageMapDatablock* pImageMapDataBlock, U32 frame ) const
{
   // Search for identify Static Tile.
   for ( U32 n = 0; n < mTileNodes.size(); n++ )
   {
      // Fetch Static Tile Node.
      cStaticTileNode* pTileNode = (cStaticTileNode*)mTileNodes[n];
      // Static Tile?
      if ( (pTileNode->mTileType == STATIC_TILE ) )
      {
         // Yes, so compare members.
         if (    pTileNode->pImageMapDataBlock2D == pImageMapDataBlock &&
            pTileNode->mFrame == frame )
         {
            // Yes, so return this tile node.
            return pTileNode;
         }
      }
   }

   // Return "No Match!".
   return NULL;
}


//-----------------------------------------------------------------------------
// Find Animation Tile Node.
//-----------------------------------------------------------------------------
t2dTileMap::cAnimationTileNode* t2dTileMap::findAnimationTileNode( t2dAnimationDatablock* pAnimationDataBlock ) const
{
   // Search for identify Animation Tile.
   for ( U32 n = 0; n < mTileNodes.size(); n++ )
   {
      // Fetch Animation Tile Node.
      cAnimationTileNode* pTileNode = (cAnimationTileNode*)mTileNodes[n];
      // Animation Tile?
      if ( (pTileNode->mTileType == ANIMATED_TILE ) )
      {
         // Yes, so compare members.
         if ( pTileNode->pAnimationController2D->getCurrentDataBlock() == pAnimationDataBlock )
         {
            // Yes, so return this tile node.
            return pTileNode;
         }
      }
   }

   // Return "No Match!".
   return NULL;
}


//-----------------------------------------------------------------------------
// Create Static Tile Node.
//-----------------------------------------------------------------------------
t2dTileMap::cStaticTileNode* t2dTileMap::createStaticTileNode( void )
{
   // Generate new Static Tile Node.
   cStaticTileNode* pStaticTileNode = new cStaticTileNode;

   // Store Node.
   mTileNodes.push_back( pStaticTileNode );

   // Return new Node.
   return pStaticTileNode;
}


//-----------------------------------------------------------------------------
// Create Animation Tile Node.
//-----------------------------------------------------------------------------
t2dTileMap::cAnimationTileNode* t2dTileMap::createAnimationTileNode( const char* animationName )
{
   // Generate new Animated Tile Node.
   cAnimationTileNode* pAnimationTileNode = new cAnimationTileNode;

   // Generate Animation DataBlock.
   pAnimationTileNode->pAnimationController2D = new t2dAnimationController;

   // Start Animation Controller.
   pAnimationTileNode->pAnimationController2D->playAnimation( animationName, false );

   // Store Animation Controller.
   mAnimationControllerList.push_back( pAnimationTileNode->pAnimationController2D );

   // Store Node.
   mTileNodes.push_back( pAnimationTileNode );

   // Return new Node.
   return pAnimationTileNode;
}


//-----------------------------------------------------------------------------
// Create Active Tile Node.
//-----------------------------------------------------------------------------
t2dTileMap::cActiveTileNode* t2dTileMap::createActiveTileNode( const char* activeTileClassName, const char* activeTileDataBlockName )
{
   // Generate Active Tile Class via Console.
   t2dActiveTile* pActiveTile2D = (t2dActiveTile*)ConsoleObject::create( activeTileClassName );
   // Check Active Tile Class.
   if ( !pActiveTile2D )
   {
      // Warn.
      Con::warnf("t2dTileMap::createActiveTileNode() - Couldn't create t2dActiveTile class (%s)", activeTileClassName);
      // Return Here.
      return NULL;
   }

   // Register Active Tile.
   pActiveTile2D->registerObject();

   // Reset Tile.
   pActiveTile2D->resetTile();

   // Generate new Static Tile Node.
   cActiveTileNode* pActiveTileNode = new cActiveTileNode;

   // Populate Node.
   pActiveTileNode->pActiveTile2D = pActiveTile2D;

   // Store Active Tile.
   mActiveTileList.push_back( pActiveTile2D );

   // Store Node.
   mTileNodes.push_back( pActiveTileNode );

   // Return new Node.
   return pActiveTileNode;
}


//-----------------------------------------------------------------------------
// Delete Tile Node.
//-----------------------------------------------------------------------------
void t2dTileMap::deleteTileNode( cRootTileNode* pRootTileNode )
{
   // Find Root Node Index.
   S32 rootNodeIndex = findRootTileNodeIndex( pRootTileNode );
   // Sanity.
   AssertFatal( rootNodeIndex != -1, "t2dTileMap::deleteTileNode() - Could not find Tile-Node!" );

	if( pRootTileNode->mReferenceCount == 0 )
	{
		// RKS: This is here to investigate a crash, possibly from out of memory.
		printf( "======= t2dTileMap::deleteTileNode - WARNING: DECREASING REFERENCE COUNT WHEN REFERENCE COUNT IS 0! ==================\n" );
		
		int stophere = 0;
		stophere++;
	}
	
   // Reduce Reference.
   pRootTileNode->mReferenceCount--;

   // Finish here if we've still got references to the node.
   if ( pRootTileNode->mReferenceCount > 0 )
      return;

   // Handle Node Type Data Appropriately.
   switch( pRootTileNode->mTileType )
   {
      // Static Tile.
   case STATIC_TILE:
      {
         // Cast to Static Tile Node.
         cStaticTileNode* pStaticTileNode = (cStaticTileNode*)pRootTileNode;
         // Reset Reference.
         pStaticTileNode->pImageMapDataBlock2D = NULL;


      } break;

      // Animation Tile.
   case ANIMATED_TILE:
      {
         // Cast to Animation Tile Node.
         cAnimationTileNode* pAnimationTileNode = (cAnimationTileNode*)pRootTileNode;

         // Find Animation Controller Index.
         const S32 animationControllerIndex = findAnimationControllerIndex( pAnimationTileNode->pAnimationController2D );
         // Sanity.
         AssertFatal( animationControllerIndex != -1, "t2dTileMap::deleteTileNode() - Could not find Tile-Node Animation Controller!" );

         // Remove Animation Controller Node.
         mAnimationControllerList.erase_fast( animationControllerIndex );

         // Delete Animation Controller.
         delete pAnimationTileNode->pAnimationController2D;

      } break;

      // Active Tile.
   case ACTIVE_TILE:
      {
         // Cast to Active Tile Node.
         cActiveTileNode* pActiveTileNode = (cActiveTileNode*)pRootTileNode;

         // Find Active Tile Index.
         const S32 activeTileIndex = findActiveTileIndex( pActiveTileNode->pActiveTile2D );
         // Sanity.
         AssertFatal( activeTileIndex != -1, "t2dTileMap::deleteTileNode() - Could not find Tile-Node Active-Tile!" );

         // Unregister Active Tile.
         pActiveTileNode->pActiveTile2D->unregisterObject();

         // Remove Active Tile Node.
         mActiveTileList.erase_fast( activeTileIndex );

         // Delete Active Tile.
         delete pActiveTileNode->pActiveTile2D;

      } break;
   }

   // Remove Root Node.
   mTileNodes.erase_fast( rootNodeIndex );

   // Delete Root Node.
   delete pRootTileNode;

   // Safe our pointer.
   pRootTileNode = NULL;
}


//-----------------------------------------------------------------------------
// Load TileMap DataBlock.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, loadTileMapDataBlock, bool, 3, 3, "(string tileMapDatablock) - Loads a Tilemap from a Datablock.\n"
														    "@param tileMapDatablock The tilemap datablock to load"
															"@return Returns true on success, false otherwise.")
{
   // Load TileMap DataBlock
   return object->loadTileMapDataBlock( argv[2] );
}   
// Load TileMap DataBlock.
bool t2dTileMap::loadTileMapDataBlock( const char* tileMapDataBlockName )
{
   // We *must* have a scenegraph!
   if ( getSceneGraph() == NULL )
   {
      Con::warnf("t2dTileMap::loadTileMapDataBlock() - Cannot process without SceneGraph!");
      return false;
   }

   // Expand relative paths.
   char buffer[1024];
   if ( tileMapDataBlockName )
      if ( Con::expandScriptFilename( buffer, sizeof( buffer ), tileMapDataBlockName ) )
         tileMapDataBlockName = buffer;

   // Find TileMap Datablock.
   t2dSceneObjectDatablock* pTileMapDataBlock = dynamic_cast<t2dSceneObjectDatablock*>(Sim::findObject( tileMapDataBlockName ));

   // Check Datablock (if we've got one).
   if ( !t2dCheckDatablock( pTileMapDataBlock ) )
   {
      Con::warnf("t2dTileMap::loadTileMapDataBlock() - Couldn't find TileMap datablock (%s)", tileMapDataBlockName);
      return false;
   }

   // Set Datablock.
   mConfigDataBlock = pTileMapDataBlock;

   // Load TileMap.
   return loadTileMap( mConfigDataBlock->getDataField("tilemapFile", NULL) );
}


//-----------------------------------------------------------------------------
// Load TileMap.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, loadTileMap, bool, 3, 3, "(string tileMapFile) - Loads a Tilemap File.\n"
												   "@param tileMapFile The tilemap file to load\n"
												   "@return Returns true on success and false otherwise.")
{
   // Load TileMap.
   return object->loadTileMap( argv[2] );
}
// Load TileMap.
bool t2dTileMap::loadTileMap( const char* tilemapFile )
{
   // We *must* have a scenegraph!
   if ( getSceneGraph() == NULL )
   {
      // Warn.
      Con::warnf("t2dTileMap::loadTileMap() - Cannot Load without SceneGraph!");
      return false;
   }

   // Expand relative paths.
   static char buffer[1024];
   if ( tilemapFile )
      if ( Con::expandScriptFilename( buffer, sizeof( buffer ), tilemapFile ) )
         tilemapFile = buffer;

   // Delete All Tile-Layers.
   deleteAllTileLayers();

   // Open Scene File.
   Stream* pStream = ResourceManager->openStream( tilemapFile );
   // Check Stream.
   if ( !pStream )
   {
      // Warn.
      Con::warnf("t2dTileMap::loadTileMap() - Could not Open File '%s' for TileMap Load.", tilemapFile);
      // Return Error.
      return false;
   }

   // Scene Objects.
   Vector<t2dSceneObject*> ObjReferenceList;

   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( ObjReferenceList );

   // Load TileMap Stream.
   if ( !loadStream( *pStream, getSceneGraph(), ObjReferenceList, true ) )
   {
      // Warn.
      Con::warnf("t2dTileMap::loadTileMap() - Could not Read from Stream '%s' for TileMap Load.", tilemapFile);
      // Return Error.
      return false;
   }

   // Iterate Tile-Layers.
   for ( U32 n = 0; n < mTileLayerList.size(); n++ )
   {
      // Reset Layer.
      mTileLayerList[n]->resetLayer();

      // Enable/Visible Layer.
      mTileLayerList[n]->setEnabled( true );
      mTileLayerList[n]->setVisible( true );

      // Added for inspector.
      mTileLayerList[n]->setModDynamicFields( true );
      mTileLayerList[n]->setModStaticFields( true );
   }

   // Reset Any Lifetime Counter.
   setLifetime( 0.0f );

   // Close Stream.
   ResourceManager->closeStream( pStream );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Save TileMap.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileMap, saveTileMap, bool, 3, 3, "(string tileMapFile) - Saves a Tilemap File.\n"
												   "@param tileMapFile Saves the tilemap to a file\n"
												   "@return Returns true on success, false otherwise.")
{
   // Save TileMap.
   return object->saveTileMap( argv[2] );
}
// Save TileMap.
bool t2dTileMap::saveTileMap( const char* tilemapFile )
{
   // Get Layer Count.
   if ( getTileLayerCount() == 0 )
   {
      // Warn.
      Con::warnf("t2dTileMap::saveTileMap() - Cannot Save without at least a single Tile Layer!");
      return false;
   }

   // We *must* have a scenegraph!
   if ( getSceneGraph() == NULL )
   {
      // Warn.
      Con::warnf("t2dTileMap::saveTileMap() - Cannot Save without SceneGraph!");
      return false;
   }

   // Expand relative paths.
   static char buffer[1024];
   if ( tilemapFile )
      if ( Con::expandToolScriptFilename( buffer, sizeof( buffer ), tilemapFile ) )
         tilemapFile = buffer;

   // Open Scene File.
   FileStream fileStream;
   if ( !ResourceManager->openFileForWrite( fileStream, tilemapFile, FileStream::Write ) )
   {
      // Warn.
      Con::warnf("t2dTileMap::saveTileMap() - Could not open File '%s' for TileMap Save.", tilemapFile);
      return false;
   }

   // Save TileMap Stream (Ensure Serialise ID doesn't get in the way).
   if ( !saveStream( fileStream, getSceneGraph()->getNextSerialiseID(), 0 ) )
   {
      // Warn.
      Con::warnf("t2dTileMap::saveTileMap() - Could not Save to Stream '%s' for TileMap Save.", tilemapFile);
      return false;
   }

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dTileMap )
REGISTER_SERIALISE_VERSION( t2dTileMap, 2, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dTileMap, 2 )


//-----------------------------------------------------------------------------
// Load v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dTileMap, 2 )
{
   S32 tileLayers;

   // Read Tile-Layer Count.
   if ( !stream.read( &tileLayers ) )
      return false;

   // Write All Tile Layers.
   for ( S32 n = 0; n < tileLayers; n++ )
   {
      // Generate Tile Layer.
      t2dTileLayer* pTileLayer = new t2dTileLayer( object );

      // Register this Object.
      pTileLayer->registerObject();
      // Set as Child.
      pTileLayer->setChild( true );
      // Add it to our Scene Graph.
      object->getSceneGraph()->addToScene( pTileLayer );
      // notify of object deletion
      object->deleteNotify( pTileLayer );
      // Reference Tile Layer.
      object->mTileLayerList.push_back( pTileLayer );

      // Do Self-Serialisation.
      if ( !pTileLayer->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
         return false;
   }

   // Return Okay.
   return true;
}

//-----------------------------------------------------------------------------
// Save v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dTileMap, 2 )
{
   // Fetch Tile-Layer Count.
   S32 tileLayers = object->mTileLayerList.size();

   // Write Tile-Layer Count.
   if ( !stream.write( tileLayers ) )
      return false;

   // Write All Tile Layers.
   for ( S32 n = 0; n < tileLayers; n++ )
   {
      // Fetch Tile Layer.
      t2dTileLayer* pTileLayer = object->mTileLayerList[n];

      // Self-Serialise Tile Layer.
      if ( !pTileLayer->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
         return false;
   }

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dTileMap::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
   // Call Parent.
   Parent::integrateObject( sceneTime, elapsedTime, pDebugStats );

   // Ensure that we're invisible!
   setVisible(false);

   // Update Animation Controllers.
   for ( U32 n = 0; n < mAnimationControllerList.size(); n++ )
      mAnimationControllerList[n]->updateAnimation( elapsedTime );

   // Update Active-Tiles.
   for ( U32 n = 0; n < mActiveTileList.size(); n++ )
      mActiveTileList[n]->integrateObject( sceneTime, elapsedTime, pDebugStats );
}



//-----------------------------------------------------------------------------
//
//  t2dTileLayer.
//
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Default Constructor.
//-----------------------------------------------------------------------------
t2dTileLayer::t2dTileLayer() :  T2D_Stream_HeaderID(makeFourCCTag('2','D','T','L')),
                                mppTileObjectArray(NULL),
                                mpTileMap2DManager(NULL),
                                mTileCountX(0),
                                mTileCountY(0),
                                mTileSizeX(0),
                                mTileSizeY(0),                                          
                                mWrapX(false),
                                mWrapY(false),
                                mPanPositionX(0.0f),
                                mPanPositionY(0.0f),
                                mAutoPanX(0.0f),
                                mAutoPanY(0.0f),
                                mGridActive(false),
                                mScriptIconActive(false),
                                mCustomIconActive(false),
                                mCursorIconActive(false),
                                mCursorBlinkTime(0.25f),
                                mCurrentCursorBlinkTime(0.0f),
                                mCurrentCursorBlinkState(true),
                                mCurrentCursorX(0),
                                mCurrentCursorY(0),
                                mTileIconImageMapDataBlock(NULL),
                                mLayerFile(NULL)
{
   // Send Collisions not allowed!
   Parent::mCollisionSendAllowed = false;

   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( mDebugRenderPhysics );
}


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dTileLayer::t2dTileLayer(t2dTileMap* pTileMap2D) :    T2D_Stream_HeaderID(makeFourCCTag('2','D','T','L')),
                                                        mppTileObjectArray(NULL),
                                                        mpTileMap2DManager(pTileMap2D),
                                                        mTileCountX(0),
                                                        mTileCountY(0),
                                                        mTileSizeX(0),
                                                        mTileSizeY(0),                                          
                                                        mWrapX(false),
                                                        mWrapY(false),
                                                        mPanPositionX(0.0f),
                                                        mPanPositionY(0.0f),
                                                        mAutoPanX(0.0f),
                                                        mAutoPanY(0.0f),
                                                        mGridActive(false),
                                                        mScriptIconActive(false),
                                                        mCustomIconActive(false),
                                                        mCursorIconActive(false),
                                                        mCursorBlinkTime(0.25f),
                                                        mCurrentCursorBlinkTime(0.0f),
                                                        mCurrentCursorBlinkState(true),
                                                        mCurrentCursorX(0),
                                                        mCurrentCursorY(0),
                                                        mTileIconImageMapDataBlock(NULL),
                                                        mLayerFile(NULL)
{
   // Send Collisions not allowed!
   Parent::mCollisionSendAllowed = false;

   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( mDebugRenderPhysics );
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dTileLayer::~t2dTileLayer()
{
   // Delete Layer.
   deleteLayer();
}


//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dTileLayer::onAdd( void )
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// OnRemove
//-----------------------------------------------------------------------------
void t2dTileLayer::onRemove( void )
{
   clearLayer();

   Parent::onRemove();
}

/// Clone support
void t2dTileLayer::copyTo(SimObject* obj)
{
	Parent::copyTo(obj);

	AssertFatal(dynamic_cast<t2dTileLayer*>(obj), "t2dTileLayer::copyTo - Copy object is not a t2dTileLayer!");
	t2dTileLayer* object = static_cast<t2dTileLayer*>(obj);

	// Copy fields
	object->setLayerFile(object, mLayerFile);
}

void t2dTileLayer::initPersistFields()
{
   addProtectedField("LayerFile", TypeFilename, Offset(mLayerFile, t2dTileLayer), &setLayerFile, &defaultProtectedGetFn, "");
   Parent::initPersistFields();
}

bool t2dTileLayer::setLayerFile(void* obj, const char* data)
{

   t2dTileLayer* layer = static_cast<t2dTileLayer*>(obj);
   
   if (data && *data && dStricmp(data, "-") == 0)
   {
      layer->mLayerFile = StringTable->insert("");
      return false;
   }
   else if (layer->getSceneGraph() && layer->getSceneGraph()->getGlobalTileMap() && data && *data)
   {
	   //printf( "==== t2dTileLayer::setLayerFile - CALLING createLayer(), %s - IN 11111111\n", layer->getName() ); // RKS: THIS WAS ADDED TO CHECK FOR THE createLayer() MEMORY LEAK
	   
      //layer->deleteLayer();
      //layer->createLayer(1, 1, 1, 1);
      if (layer->getSceneGraph()->getGlobalTileMap() != layer->mpTileMap2DManager)
         layer->getSceneGraph()->getGlobalTileMap()->addTileLayer(layer);
      layer->loadTileLayer(data);
      return false;
   }

   else if (gLoadingSceneGraph && gLoadingSceneGraph->getGlobalTileMap())
   {
	   //printf( "==== t2dTileLayer::setLayerFile - CALLING createLayer(), %s - IN 2222222\n", layer->getName() ); // RKS: THIS WAS ADDED TO CHECK FOR THE createLayer() MEMORY LEAK
      //layer->deleteLayer();
      //layer->createLayer(1, 1, 1, 1);
      if (gLoadingSceneGraph->getGlobalTileMap() != layer->mpTileMap2DManager)
         gLoadingSceneGraph->getGlobalTileMap()->addTileLayer(layer);
      layer->loadTileLayer(data);
      return false;
   }

   return true;
}

//-----------------------------------------------------------------------------
// Get Tile Object Node.
//-----------------------------------------------------------------------------
bool t2dTileLayer::getTileObject( const U32 tileX, const U32 tileY, tTileObject** ppTileObject ) const
{
   // Return Nothing if we've not got a tile array.
   if ( !mppTileObjectArray )
      return false;

   // Check Tile Bounds.
   if (    tileX < 0 || tileX >= mTileCountX ||
      tileY < 0 || tileY >= mTileCountY )
      return false;

   // Set Tile Object Pointer.
   *ppTileObject = mppTileObjectArray[tileX + ( tileY * mTileCountX )];

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Create Layer.
//-----------------------------------------------------------------------------
void t2dTileLayer::createLayer( const U32 tileCountX, const U32 tileCountY, const F32 tileSizeX, const F32 tileSizeY )
{
   // Store Dimensions.
   mTileCountX = tileCountX;
   mTileCountY = tileCountY;
   mTileSizeX  = tileSizeX;
   mTileSizeY  = tileSizeY;

   // Create Tile-Layer Object Array.
   mppTileObjectArray = new tTileObject*[ mTileCountX * mTileCountY ];

   // Reset Tile Array.
   for ( U32 n = 0; n < (mTileCountX * mTileCountY); n++ )
      mppTileObjectArray[n] = NULL;
}

ConsoleMethod( t2dTileLayer, createLayer, void, 6, 6, "(int tileCountX, int tileCountY, float tileSizeX, float tileSizeY) - Creates a tile layer.\n"
													  "@param tileCountX The number of tiles in the X direction\n"
													  "@param tileCountY The number of tiles in the Y direction\n"
													  "@param tileSizeX The width of an individual tile\n"
													  "@param tileSizeY The height of an individual tile\n"
													  "@return No return value.")
{
	printf( "IN SCRIPT CALL:  %s - createLayer()\n", object->getName() ); // RKS: I added this to track down a possible memory leak
	
   F32 tileCountX = dAtof( argv[2] );
   F32 tileCountY = dAtof( argv[3] );
   F32 tileSizeX = dAtof( argv[4] );
   F32 tileSizeY = dAtof( argv[5] );
   object->createLayer( tileCountX, tileCountY, tileSizeX, tileSizeY );
}


//-----------------------------------------------------------------------------
// Delete Layer.
//-----------------------------------------------------------------------------
void t2dTileLayer::deleteLayer( void )
{
   // Is there an active Tile Array?
   if ( mppTileObjectArray )
   {
      // Delete All Tiles.
      for ( U32 n = 0; n < (mTileCountX * mTileCountY); n++ )
      {
         // Is there a tile here?
         if ( mppTileObjectArray[n] )
         {
            // Yes, so delete the Tile Node.
            mpTileMap2DManager->deleteTileNode( mppTileObjectArray[n]->mpTileNode );
            // Delete the Physics.
            delete mppTileObjectArray[n]->mpPhysics;
            // Delete the tile object.
            delete mppTileObjectArray[n];
         }
      }

      // Delete Tile Array.
      delete [] mppTileObjectArray;

      // Reset Tile Array.
      mppTileObjectArray = NULL;
   }
}


//-----------------------------------------------------------------------------
// Clear Layer.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, clearLayer, void, 2, 2, "() Deletes All Layer Tiles.\n"
			  "@return No return value.")
{
   // Clear Layer.
   object->clearLayer();
}
// Clear Layer.
void t2dTileLayer::clearLayer( void )
{
   // Is there an active Tile Array?
   if ( mppTileObjectArray )
   {
      // Delete All Tiles.
      for ( U32 n = 0; n < (mTileCountX * mTileCountY); n++ )
      {
         // Is there a tile here?
         if ( mppTileObjectArray[n] )
         {
            // Yes, so delete the Tile Node.
            mpTileMap2DManager->deleteTileNode( mppTileObjectArray[n]->mpTileNode );
            // Delete the Physics.
            delete mppTileObjectArray[n]->mpPhysics;
            // Delete the tile object.
            delete mppTileObjectArray[n];
            // Reset Tile Object.
            mppTileObjectArray[n] = NULL;
         }
      }
   }
}


//-----------------------------------------------------------------------------
// Resize Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileCountX, void, 3, 3, "(int TileCountX)\n"
													   "@param TileCountX The new number of tiles in the X direction\n"
			  "@return No return value.")
{
   object->resizeLayer(dAtoi(argv[2]), object->getTileCountY());
}

ConsoleMethod(t2dTileLayer, setTileCountY, void, 3, 3, "(int TileCountY)\n"
													   "@param TileCountY The new number of tiles in the Y direction\n"
			  "@return No return value.")
{
   object->resizeLayer(object->getTileCountX(), dAtoi(argv[2]));
}

ConsoleMethod(t2dTileLayer, getTileCountX, S32, 2, 2, "() - Get the number of tiles in the X direction\n"
													  "@return (int TileCountX) The number of tiles in the X direction")
{
   return object->getTileCountX();
}

ConsoleMethod(t2dTileLayer, getTileCountY, S32, 2, 2, "() - Get the number of tiles in the Y direction\n"
													  "@return (int TileCountY) The number of tiles in the Y direction")
{
   return object->getTileCountY();
}

ConsoleMethod( t2dTileLayer, resizeLayer, void, 3, 4, "(int TileCountX, int TileCountY) - Resize Tile Layer.\n"
													  "@param TileCountX The new number of tiles in the X direction\n"
													  "@param TileCountY The new number of tiles in the Y direction\n"
			  "@return No return value.")
{
   // The tile count.
   U32 tileCountX;
   U32 tileCountY;

   // Elements in the first argument.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileCountX tileCountY")
   if ((elementCount == 2) && (argc == 3))
   {
      tileCountX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileCountY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileCountX, tileCountY)
   else if ((elementCount == 1) && (argc == 4))
   {
      tileCountX = dAtoi(argv[2]);
      tileCountY = dAtoi(argv[3]);
   }
   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::resizeLayer() - Invalid number of parameters!");
      return;
   }

   // Resize Layer.
   object->resizeLayer(tileCountX, tileCountY);
}
// Resize Layer.
void t2dTileLayer::resizeLayer( const U32 newTileCountX, const U32 newTileCountY )
{
   // Check we've got valid tiles!
   if ( newTileCountX < 1 || newTileCountY < 1 )
   {
      Con::warnf("t2dTileLayer::resizeLayer() - There must be at least a single tile in each axis!");
      return;
   }

   // Ignore if the new size is the same as the old!
   if ( newTileCountX == mTileCountX && newTileCountY == mTileCountY )
      // Well that was easy if not pointless!
      return;

   // Is there an active Tile Array?
   if ( mppTileObjectArray )
   {
      // Generate new Tile Object Array.
      tTileObject** ppTempObjectArray = new tTileObject*[ newTileCountX * newTileCountY ];

      // Reset Temp Tile Object Array.
      for ( U32 n = 0; n < (newTileCountX * newTileCountY); n++ )
         ppTempObjectArray[n] = NULL;

      // Calculate the smallest tile-spans in both axes.
      const U32 minCountX = getMin( mTileCountX, newTileCountX );
      const U32 minCountY = getMin( mTileCountY, newTileCountY );

      // Let's copy the least area to the new tile object array.
      for ( U32 y = 0; y < minCountY; y++ )
         for ( U32 x = 0; x < minCountX; x++ )
            ppTempObjectArray[x + ( y * newTileCountX )] = mppTileObjectArray[x + ( y * mTileCountX )];

      // Do we need to remove any excess tiles in the x axis?
      if ( newTileCountX < mTileCountX )
      {
         // Yes, so remove them from all Y-axis rows.
         for ( U32 y = 0; y < mTileCountY; y++ )
            for ( U32 x = newTileCountX; x < mTileCountX; x++ )
               clearTile( x, y );
      }

      // Do we need to remove any excess tiles in the y axis?
      if ( newTileCountY < mTileCountY )
      {
         // Yes, so remove them from Y-axis rows.
         for ( U32 y = newTileCountY; y < mTileCountY; y++ )
            for ( U32 x = 0; x < newTileCountX; x++ )
               clearTile( x, y );
      }

      // Delete Tile Array.
      delete [] mppTileObjectArray;

      // Transfer new tile object array.
      mppTileObjectArray = ppTempObjectArray;

      // Set new Tile Counts.
      mTileCountX = newTileCountX;
      mTileCountY = newTileCountY;
   }
}


//-----------------------------------------------------------------------------
// Create Tile Object.
//-----------------------------------------------------------------------------
void t2dTileLayer::createTileObject( const U32 tileX, const U32 tileY, t2dTileMap::cRootTileNode* pRootTileNode )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( getTileObject( tileX, tileY, &pTileObject ) )
      // Remove Object if it exists.
      if ( pTileObject )
         // Clear Existing Tile.
         clearTile( tileX, tileY );

   // Create Tile Object.
   pTileObject = new tTileObject;

   // Reset Update Sequence Key.
   pTileObject->mUpdateSequenceKey = 0;

   // Default Flip.
   pTileObject->mFlipHorizontal = pTileObject->mFlipVertical = false;

   // Default Receive Collisions.
   pTileObject->mReceiveCollisions = false;

   // Create Physics.
   pTileObject->mpPhysics = new t2dPhysics;
   // Format Reference String.
   static char pRefBuffer[64];
   dSprintf( pRefBuffer, 64, "%d %d", tileX, tileY );

   // Initialise Physics.
   pTileObject->mpPhysics->initialise( this, pRefBuffer );
   // Set Auto Mass/Inertial off.
   pTileObject->mpPhysics->setAutoMassInertia( false );
   // Set Density (Immovable).
   pTileObject->mpPhysics->setImmovable( true );
   // Set Tile Size.
   pTileObject->mpPhysics->setSize( t2dVector(mTileSizeX, mTileSizeY) );
   // Default to Quad-Poly.
   pTileObject->mpPhysics->setCollisionPolyPrimitive( 4 );
   // Set Detection Mode.
   pTileObject->mpPhysics->setCollisionDetectionMode( t2dPhysics::T2D_DETECTION_POLY_ONLY );

   // Reset Tile Script.
   pTileObject->mShowScript = StringTable->insert("");
   pTileObject->mScriptActioned = false;

   // Reset Custom Data.
   pTileObject->mCustomData = StringTable->insert("");

   // Reference Tile Node.
   pTileObject->mpTileNode = pRootTileNode;

   // Insert into Tile Object Array.
   mppTileObjectArray[tileX + ( tileY * mTileCountX )] = pTileObject;
}


//-----------------------------------------------------------------------------
// Clear Tile.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, clearTile, void, 3, 4, "(int tileX, int tileY ) - Clears a Tile.\n"
												    "@param tileX The X coordinate of the tile to clear\n"
												    "@param tileY The Y coordinate of the tile to clear\n"
			  "@return No return value.")
{
   // The tile index.
   U32 tileX, tileY;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc == 3))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc == 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::clearTile() - Invalid number of parameters!");
      return;
   }

   // Clear Tile.
   object->clearTile(tileX, tileY);
}
// Clear Tile.
void t2dTileLayer::clearTile( const U32 tileX, const U32 tileY )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
      return;

   // Ignore if no Tile Object.
   if ( !pTileObject )
      return;

   // Delete the Tile Node.
   mpTileMap2DManager->deleteTileNode( pTileObject->mpTileNode );
   // Delete the Physics.
   delete pTileObject->mpPhysics;
   // Delete the tile object.
   delete pTileObject;

   // Reset Tile Object.
   // NOTE:- This is not good!
   mppTileObjectArray[tileX + ( tileY * mTileCountX )] = NULL;
}





//-----------------------------------------------------------------------------
// Set Static Tile.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, setStaticTile, bool, 4, 6, "(int tileX, int tileY, string imageMapName, [int frame] ) - Sets a Static Tile.\n"
													    "@param tileX The X coordinate of the tile to set\n"
													    "@param tileY The Y coordinate of the tile to set\n"
													    "@param imageMapName The imagemap to set the tile to\n"
													    "@param frame The frame of the imagemap to use\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // The index of the image map name.
   U32 imageMapNameIndex;
   // The frame.
   U32 frame = 0;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", imageMapName, [frame])
   if ((elementCount == 2) && (argc < 6))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      imageMapNameIndex = 3;

      if (argc > 4)
         frame = dAtoi(argv[4]);
   }

   // (tileX, tileY, imageMapName, [frame])
   else if ((elementCount == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      imageMapNameIndex = 4;

      if (argc > 5)
         frame = dAtoi(argv[5]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setStaticTile() - Invalid number of parameters!");
      return 0;
   }

   // Set Static Tile.
   return object->setStaticTile(tileX, tileY, argv[imageMapNameIndex], frame);
}
// Set Static Tile.
bool t2dTileLayer::setStaticTile( const U32 tileX, const U32 tileY, const char* imageMapName, const U32 frame )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setStaticTile() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   bool collision = false;
   bool flipX = false;
   bool flipY = false;
   StringTableEntry script = StringTable->insert("");
   StringTableEntry data = StringTable->insert("");
   Vector<t2dVector> collisionPoly;
   if (pTileObject)
   {
      collision = pTileObject->mReceiveCollisions;
      flipX = pTileObject->mFlipHorizontal;
      flipY = pTileObject->mFlipVertical;
      script = pTileObject->mShowScript;
      data = pTileObject->mCustomData;

      if( collision && pTileObject->mpPhysics )
      {
         const t2dVector* poly = pTileObject->mpPhysics->getCollisionPolyBasis();
         for( U32 i = 0; i < pTileObject->mpPhysics->getCollisionPolyCount(); i++ )
            collisionPoly.push_back( poly[i] );
      }
   }

   // Clear Tile.
   clearTile( tileX, tileY );

   // Find ImageMap Datablock.
   t2dImageMapDatablock* pImageMapDataBlock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject( imageMapName ));

   // Set Datablock.
   if ( !t2dCheckDatablock( pImageMapDataBlock ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setStaticTile() - t2dImageMapDatablock Datablock is invalid! (%s)", imageMapName);
      // Return Here.
      return false;
   }

   // Check Frame in ImageMap.
   if ( frame >= pImageMapDataBlock->getImageMapFrameCount() )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setStaticTile() - Invalid Frame selected for t2dImageMapDatablock Datablock! (%s, %d)", imageMapName, frame);
      // Return Here.
      return false;
   }

   // Search for existing Static Tile Node.
   t2dTileMap::cStaticTileNode* pStaticTileNode = mpTileMap2DManager->findStaticTileNode( pImageMapDataBlock, frame );
   // Find One?
   if ( !pStaticTileNode )
   {
      // No, so create one.
      pStaticTileNode = mpTileMap2DManager->createStaticTileNode();

      // Populate Tile Node...
      pStaticTileNode->pImageMapDataBlock2D = pImageMapDataBlock;
      pStaticTileNode->mFrame = frame;
   }

   // Increase Tile Node Reference.
   pStaticTileNode->mReferenceCount++;

   // Set tile to this node.
   createTileObject( tileX, tileY, pStaticTileNode );

   setTileFlip(tileX, tileY, flipX, flipY);
   setTileCollisionActive(tileX, tileY, collision);
   setTileScript(tileX, tileY, script);
   setTileCustomData(tileX, tileY, data);
   if( collision )
      setTileCollisionPolyCustom( tileX, tileY, collisionPoly.size(), collisionPoly.address() );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Animated Tile.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setAnimatedTile, bool, 4, 6, "(int tileX, int tileY, string animationName, [bool forceUnique]) - Sets an Animated Tile.\n"
														 "@param tileX The X coordinate of the tile to set\n"
														 "@param tileY The Y coordinate of the tile to set\n"
														 "@param animationName The animation to set the tile to\n"
														 "@param forceUnique\n"
			  "@return Return true on success")
{
   // The tile.
   U32 tileX, tileY;
   // The index of the animation name.
   U32 animationNameIndex;
   // The force unique flag.
   bool forceUnique = false;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", animationName, [forceUnique])
   if ((elementCount == 2) && (argc < 6))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      animationNameIndex = 3;

      if (argc > 4)
         forceUnique = dAtob(argv[4]);
   }

   // (tileX, tileY, animationName, [forceUnique])
   else if ((elementCount == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      animationNameIndex = 4;

      if (argc > 5)
         forceUnique = dAtob(argv[5]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setAnimatedTile() - Invalid number of parameters!");
      return 0;
   }

   // Set Animated Tile.
   return object->setAnimatedTile(tileX, tileY, argv[animationNameIndex], forceUnique);
}
// Set Animated Tile.
bool t2dTileLayer::setAnimatedTile( const U32 tileX, const U32 tileY, const char* animationName, const bool forceUnique )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setAnimatedTile() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   bool collision = false;
   bool flipX = false;
   bool flipY = false;
   StringTableEntry script = StringTable->insert("");
   StringTableEntry data = StringTable->insert("");
   Vector<t2dVector> collisionPoly;
   if (pTileObject)
   {
      collision = pTileObject->mReceiveCollisions;
      flipX = pTileObject->mFlipHorizontal;
      flipY = pTileObject->mFlipVertical;
      script = pTileObject->mShowScript;
      data = pTileObject->mCustomData;

      if( collision && pTileObject->mpPhysics )
      {
         const t2dVector* poly = pTileObject->mpPhysics->getCollisionPolyBasis();
         for( U32 i = 0; i < pTileObject->mpPhysics->getCollisionPolyCount(); i++ )
            collisionPoly.push_back( poly[i] );
      }
   }

   // Clear Tile.
   clearTile( tileX, tileY );

   // Find Animation Datablock.
   t2dAnimationDatablock* pAnimationDataBlock = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject( animationName ));

   // Check Datablock (if we've got one).
   if ( !t2dCheckDatablock( pAnimationDataBlock ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setAnimatedTile() - Invalid t2dAnimationDatablock datablock (%s)", animationName);
      // Return Here.
      return false;
   }

   // Animation Tile Node.
   t2dTileMap::cAnimationTileNode* pAnimationTileNode = NULL;

   // Are we forcing a unique Animation Controller?
   if ( !forceUnique )
   {
      // No, so search for existing Animation Tile Node.
      pAnimationTileNode = mpTileMap2DManager->findAnimationTileNode( pAnimationDataBlock );
   }
   // Find One?
   if ( !pAnimationTileNode )
   {
      // No, so create one.
      pAnimationTileNode = mpTileMap2DManager->createAnimationTileNode( animationName );
   }

   // Increase Tile Node Reference.
   pAnimationTileNode->mReferenceCount++;

   // Set tile to this node.
   createTileObject( tileX, tileY, pAnimationTileNode );

   setTileFlip(tileX, tileY, flipX, flipY);
   setTileCollisionActive(tileX, tileY, collision);
   setTileScript(tileX, tileY, script);
   setTileCustomData(tileX, tileY, data);
   if( collision )
      setTileCollisionPolyCustom( tileX, tileY, collisionPoly.size(), collisionPoly.address() );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Active Tile.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setActiveTile, S32, 4, 6, "(int tileX, int tileY, string activeTileClass, [string activeTileDatablockName]) - Sets an Active Tile.\n"
													  "@param tileX the X coordinate of the tile to set\n"
													  "@param tileY the Y coordinate of the tile to set\n"
													  "@param activeTileClass The class of the active tile\n"
													  "@param activeTileDatablockName The datablock for the active tile\n"
														"@return Returns ID of tile or 0 on fail." )
{
   // The tile.
   U32 tileX, tileY;
   // The index of the tile class.
   U32 tileClassIndex;
   // The datablock.
   const char* pActiveTileDatablock = NULL;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", activeTileClass, [activeTileDatablock])
   if ((elementCount == 2) && (argc < 6))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      tileClassIndex = 3;

      if (argc > 4)
         pActiveTileDatablock = argv[4];
   }

   // (tileX, tileY, activeTileClass, [activeTileDatablock])
   else if ((elementCount == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      tileClassIndex = 4;

      if (argc > 5)
         pActiveTileDatablock = argv[5];
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setActiveTile() - Invalid number of parameters!");
      return 0;
   }

   // Set Active Tile.
   return object->setActiveTile(tileX, tileY, argv[tileClassIndex], pActiveTileDatablock);
}
// Set Active Tile.
S32 t2dTileLayer::setActiveTile( const U32 tileX, const U32 tileY, const char* activeTileClassName, const char* activeTileDatablockName )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setActiveTile() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return 0;
   }

   bool collision = false;
   bool flipX = false;
   bool flipY = false;
   StringTableEntry script = StringTable->insert("");
   StringTableEntry data = StringTable->insert("");
   Vector<t2dVector> collisionPoly;
   if (pTileObject)
   {
      collision = pTileObject->mReceiveCollisions;
      flipX = pTileObject->mFlipHorizontal;
      flipY = pTileObject->mFlipVertical;
      script = pTileObject->mShowScript;
      data = pTileObject->mCustomData;

      if( collision && pTileObject->mpPhysics )
      {
         const t2dVector* poly = pTileObject->mpPhysics->getCollisionPolyBasis();
         for( U32 i = 0; i < pTileObject->mpPhysics->getCollisionPolyCount(); i++ )
            collisionPoly.push_back( poly[i] );
      }
   }

   // Clear Tile.
   clearTile( tileX, tileY );

   // Create Active Tile Node.
   t2dTileMap::cActiveTileNode* pActiveTileNode = mpTileMap2DManager->createActiveTileNode( activeTileClassName, activeTileDatablockName );

   // Did we generate the Active Tile Node?
   if ( !pActiveTileNode )
   {
      // No, so warn.
      Con::warnf("t2dTileLayer::setActiveTile() - Couldn't find create ActiveTile!");
      // Return Here.
      return 0;
   }

   // Increase Tile Node Reference.
   pActiveTileNode->mReferenceCount++;

   // Set tile to this node.
   createTileObject( tileX, tileY, pActiveTileNode );

   setTileFlip(tileX, tileY, flipX, flipY);
   setTileCollisionActive(tileX, tileY, collision);
   setTileScript(tileX, tileY, script);
   setTileCustomData(tileX, tileY, data);
   if( collision )
      setTileCollisionPolyCustom( tileX, tileY, collisionPoly.size(), collisionPoly.address() );

   // Return Active Tile.
   return pActiveTileNode->pActiveTile2D->getId();
}


//-----------------------------------------------------------------------------
// Set Tile Script.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, setTileScript, bool, 4, 6, "(int tileX, int tileY, string script) - Sets a Script on a Tile.\n"
														"@param tileX The X coordinate of the tile to set\n"
														"@param tileY The Y coordinate of the tile to set\n"
														"@param script The script to assign to the tile\n\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // The index of the tile class.
   U32 scriptIndex;

   // Grab the element count.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", script)
   if ((elementCount == 2) && (argc < 6))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      scriptIndex = 3;
   }

   // (tileX, tileY, script)
   else if ((elementCount == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      scriptIndex = 4;
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileScript() - Invalid number of parameters!");
      return 0;
   }

   // Set Tile Script.
   return object->setTileScript(tileX, tileY, argv[scriptIndex]);
}
// Set Tile Script.
bool t2dTileLayer::setTileScript( const U32 tileX, const U32 tileY, const char* script )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileScript() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check for Empty Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileScript() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Set Show-Script.
   pTileObject->mShowScript = StringTable->insert( script, true );
   // Reset Script Flag.
   pTileObject->mScriptActioned = false;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Tile Custom Data.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, setTileCustomData, bool, 4, 5, "(int tileX, int tileY, string data) - Sets custom data on a Tile.\n"
															"@param tileX The X coordinate of the tile to set\n"
															"@param tileY The Y coordinate of the tile to set\n"
															"@param data The custom data to assign to the tile\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // The index of the tile class.
   U32 dataIndex;

   // Grab the element count.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", data)
   if ((elementCount == 2) && (argc < 5))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      dataIndex = 3;
   }

   // (tileX, tileY, data)
   else if ((elementCount == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      dataIndex = 4;
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileCustomData() - Invalid number of parameters!");
      return 0;
   }

   // Set Tile Custom Data.
   return object->setTileCustomData(tileX, tileY, argv[dataIndex]);
}
// Set Tile Custom Data.
bool t2dTileLayer::setTileCustomData( const U32 tileX, const U32 tileY, const char* customData )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCustomData() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check for Empty Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCustomData() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Set Custom Data.
   pTileObject->mCustomData = StringTable->insert( customData, true );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Tile Collision Active.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileCollisionActive, bool, 4, 5, "(int tileX, int tileY, bool receiveCollision) - Sets Tile Receive Collisions.\n"
																"@param tileX The X coordinate of the tile to set\n"
														        "@param tileY The Y coordinate of the tile to set\n"
																"@param recieveCollision Whether or not the tile should recieve collisions\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // Collision flag.
   bool receive;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", receive)
   if ((elementCount == 2) && (argc < 5))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      receive = dAtob(argv[3]);
   }

   // (tileX, tileY, receive)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      receive = dAtob(argv[4]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileCollisionActive() - Invalid number of parameters!");
      return false;
   }

   // Set Tile Collision Active.
   return object->setTileCollisionActive(tileX, tileY, receive);
}
// Set Tile Collision.
bool t2dTileLayer::setTileCollisionActive( const U32 tileX, const U32 tileY, const bool receiveCollisionActive )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionActive() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionActive() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Set Receive Collision Flag.
   pTileObject->mReceiveCollisions = receiveCollisionActive;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Tile Collision Poly Primitive.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileCollisionPolyPrimitive, bool, 4, 5, "(int tileX, int tileY, int poly-count) - Sets Tile Primitive Collision Polygon.\n"
																	   "@param tileX The X coordinate of the tile to set\n"
																	   "@param tileY The Y coordinate of the tile to set\n"
																	   "@param poly-count The number of sides of the regular polygon to set as the collision polygon for the tile\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // Number of edges.
   U32 edgeCount;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", edgeCount)
   if ((elementCount == 2) && (argc < 5))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      edgeCount = dAtoi(argv[3]);
   }

   // (tileX, tileY, edgeCount)
   else if ((elementCount == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      edgeCount = dAtoi(argv[4]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileCollisionPolyPrimitive() - Invalid number of parameters!");
      return false;
   }

   // Set Tile Collision Poly Primitive.
   return object->setTileCollisionPolyPrimitive(tileX, tileY, edgeCount);
}
// Set Tile Collision Poly Primitive.
bool t2dTileLayer::setTileCollisionPolyPrimitive( const U32 tileX, const U32 tileY, const U32 polyVertexCount )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionPolyPrimitive() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionPolyPrimitive() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Validate Polygon.
   if ( polyVertexCount < 1 )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionPolyPrimitive() - Invalid Polygon Edge Count '%d' (>1).", polyVertexCount);
      return false;
   }
   // Check for Maximum Polygon Edges.
   else if ( polyVertexCount > t2dPhysics::MAX_COLLISION_POLY_VERTEX )
   {
      Con::warnf("t2dTileLayer::setTileCollisionPolyPrimitive() - Cannot generate a %d edged collision polygon.  Maximum is %d!", polyVertexCount, t2dPhysics::MAX_COLLISION_POLY_VERTEX);
      return false;
   }

   // Set Collision Poly Primitive.
   pTileObject->mpPhysics->setCollisionPolyPrimitive( polyVertexCount );

   // Return Okay.
   return true;
}



//-----------------------------------------------------------------------------
// Set Tile Collision Poly Custom.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileCollisionPolyCustom, bool, 5, 5 + t2dPhysics::MAX_COLLISION_POLY_VERTEX, "int tileX, int tileY, int poly-count, string poly-Definition) - Sets Tile Custom Collision Polygon."
																											"@param tileX The X coordinate of the tile to set\n"
																											"@param tileY The Y coordinate of the tile to set\n"
																											"@param poly-count The number of sides in the custom polygon\n"
																											"@param poly-definition The custom polygon to assign to the tile\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // The first index after the tile indices.
   U32 index;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if (elementCount == 2)
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      index = 3;
   }

   // (tileX, tileY)
   else if (elementCount == 1)
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      index = 4;
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileCollision() - Invalid number of parameters!");
      return false;
   }

   U32 vertexCount = dAtoi(argv[index++]);

   // Vertex storage.
   t2dVector poly[t2dPhysics::MAX_COLLISION_POLY_VERTEX];

   // Validate Polygon.
   if (vertexCount > t2dPhysics::MAX_COLLISION_POLY_VERTEX)
   {
      Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Cannot generate a %d edged collision polygon.  Maximum is %d!", vertexCount, t2dPhysics::MAX_COLLISION_POLY_VERTEX);
      return false;
   }

   if (vertexCount < 1)
   {
      // Warn.
      Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Polygon Edge Count '%d' (>0).", vertexCount);
      return false;
   }

   elementCount = t2dSceneObject::getStringElementCount(argv[index]);

   // Comma separated list.
   if (elementCount == 1)
   {
      U32 actualCount = vertexCount * 2;

      // index will be either 4 or 5 at this point, depending on how the tile was specified. And,
      // the required number of args depends on this as well.
      if (argc != (actualCount + index))
      {
         // Warn.
         Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Custom Polygon Items '%d'; expected '%d'!", argc - index, actualCount);
         return false;
      }

      // Grab the poly's
      for (U32 i = 0; i < vertexCount; i++, index += 2)
      {
         poly[i] = t2dVector(dAtof(argv[index]), dAtof(argv[index + 1]));
      }
   }

   // Comma separated list of space separated points.
   else if (elementCount == 2)
   {
      if (argc != (vertexCount + index))
      {
         // Warn.
         Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Custom Polygon '%d'; expected '%d'!", argc - index, vertexCount );
         return false;
      }

      for (U32 i = 0; i < vertexCount; i++)
      {
         if (t2dSceneObject::getStringElementCount(argv[i + index]) != 2)
         {
            Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid vertex found at vertex '%d'", i);
            return false;
         }
         poly[i] = t2dSceneObject::getStringElementVector(argv[i + index]);
      }
   }

   // Space separated list.
   else if (elementCount >= 2)
   {
      // Validate Polygon Custom Length.
      if (elementCount != (vertexCount * 2))
      {
         // Warn.
         Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Custom Polygon Items '%d'; expected '%d'!", elementCount, vertexCount * 2 );
         return false;
      }

      for (U32 i = 0; i < vertexCount; i++)
      {
         poly[i] = t2dSceneObject::getStringElementVector(argv[index], i * 2);
      }
   }

   else
   {
      // Warn.
      Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Polygon Edge Count '%d' (>0).", elementCount);
      return false;
   }

   // Set Tile Collision Poly Custom.
   return object->setTileCollisionPolyCustom( tileX, tileY, vertexCount, poly );
}
// Set Tile Collision Poly Custom.
bool t2dTileLayer::setTileCollisionPolyCustom( const U32 tileX, const U32 tileY, const U32 polyVertexCount, const t2dVector* pCustomPolygon )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionPolyCustom() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionPolyCustom() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Validate Polygon Vertices.
   for ( U32 n = 0; n < polyVertexCount; n++ )
   {
      // Fetch Coordinate.
      const F32 x = pCustomPolygon[n].mX;
      const F32 y = pCustomPolygon[n].mY;
      // Check Range.
      if ( x < -1.0f || x > 1.0f || y < -1.0f || y > 1.0f )
      {
         // Warn.
         Con::warnf("t2dTileLayer::setTileCollisionPolyCustom() - Invalid Polygon Coordinate range; Must be -1 to +1! '(%f,%f)'", x, y );
         return false;
      }
   }

   // Set Collision Poly Custom.
   pTileObject->mpPhysics->setCollisionPolyCustom( polyVertexCount, pCustomPolygon );

   // Return Okay.
   return true;
}



//-----------------------------------------------------------------------------
// Set Tile Collision Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileCollisionScale, bool, 3, 6, "(int tileX, int tileY, float widthScale, [float heightScale]) - Sets Tile collision scale.\n"
															   "@param tileX The X coordinate of the tile to set\n"
															   "@param tileY The Y coordinate of the tile to set\n"
															   "@param widthScale The widthScale to set\n"
															   "@param heightScale The heightScale to set\n"
															   "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   t2dVector scale;

   // Grab the element counts.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("tileX tileY width [height]")
   if ((elementCount1 > 2) && (elementCount1 < 5) && (argc == 3))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      scale.mX = dAtof(t2dSceneObject::getStringElement(argv[2], 2));
      if (elementCount1 > 3)
         scale.mY = dAtof(t2dSceneObject::getStringElement(argv[2], 3));
      else
         scale.mY = scale.mX;
   }

   // ("tileX tileY", "width [height]")
   else if ((elementCount1 == 2) && (argc == 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      scale.mX = dAtof(t2dSceneObject::getStringElement(argv[3], 0));
      if (elementCount2 > 1)
         scale.mY = dAtof(t2dSceneObject::getStringElement(argv[3], 1));
      else
         scale.mY = scale.mX;
   }

   // (tileX, tileY, width, [height])
   else if ((elementCount1 == 1) && (argc > 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      scale.mX = dAtof(argv[4]);
      if (argc > 5)
         scale.mY = dAtof(argv[5]);
      else
         scale.mY = scale.mX;
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileCollisionScale() - Invalid number of parameters!");
      return 0;
   }

   // Set Tile Collision Scale.
   return object->setTileCollisionScale(tileX, tileY, scale);
}   
// Set Tile Collision Scale.
bool t2dTileLayer::setTileCollisionScale( const U32 tileX, const U32 tileY, const t2dVector& scale )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionScale() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollisionScale() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check Scales.
   if ( scale.mX <= 0.0f || scale.mY <= 0.0f )
   {
      Con::warnf("t2dTileLayer::setTileCollisionScale() - Collision Scales must be greater than zero! '%f,%f'.", scale.mX, scale.mY);
      return false;
   }
   // Check Scales.
   if ( scale.mX > 1.0f || scale.mY > 1.0f )
   {
      Con::warnf("t2dTileLayer::setTileCollisionScale() - Collision Scales cannot be greater than one! '%f,%f'.", scale.mX, scale.mY);
      return false;
   }

   // Set Tile Collision Scale.
   pTileObject->mpPhysics->setCollisionPolyScale( scale ); 

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Tile Flip.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileFlip, bool, 5, 6, "(int tileX, int tileY, bool flipHorz, bool flipVert) - Sets Tile Flipping.\n"
													 "@param tileX The X coordinate of the tile\n"
													 "@param tileY The Y coordinate of the tile\n"
													 "@param flipHorz Whether to flip the tile horizontally\n"
													 "@param flipVert Whether to flip the tile vertically\n"
			  "@return Returns true on success.")
{
   // The tile.
   U32 tileX, tileY;
   // The index of the tile class.
   bool flipX, flipY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY", flipHorizontal, flipVertical)
   if ((elementCount == 2) && (argc < 6))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
      flipX = dAtob(argv[3]);
      flipY = dAtob(argv[4]);
   }

   // (tileX, tileY, flipHorizontal, flipVertical)
   else if ((elementCount == 1) && (argc > 5))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
      flipX = dAtob(argv[4]);
      flipY = dAtob(argv[5]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileFlip() - Invalid number of parameters!");
      return 0;
   }

   // Set Tile Flip.
   return object->setTileFlip(tileX, tileY, flipX, flipY);
}
// Set Tile Flip.
bool t2dTileLayer::setTileFlip( const U32 tileX, const U32 tileY, bool flipHorz, bool flipVert  )
{
   // Tile Object.
   tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileFlip() - Invalid Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Check Tile.
   if ( !pTileObject )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileCollision() - Empty Tile Position! (%d, %d)", tileX, tileY);
      // Return Here.
      return false;
   }

   // Set Tile Flip.
   pTileObject->mFlipHorizontal = flipHorz;
   pTileObject->mFlipVertical = flipVert;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Wrap Options.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setWrapX, void, 3, 3, "(bool wrapX) - Sets Wrap Options.\n"
												  "@param wrapX Whether to wrap in the X direction\n"
			  "@return No return value.")
{
   // Set Wrap.
   object->setWrap( dAtob(argv[2]), object->getWrapY() );
}

ConsoleMethod(t2dTileLayer, setWrapY, void, 3, 3, "(bool wrapY) - Sets Wrap Options.\n"
												  "@param wrapY Whether to wrap in the Y direction\n"
			  "@return No return value.")
{
   // Set Wrap.
   object->setWrap( object->getWrapX(), dAtob(argv[2]) );
}

ConsoleMethod(t2dTileLayer, setWrap, void, 4, 4, "(bool wrapX, bool wrapY) - Sets Wrap Options.\n"
												 "@param wrapX Whether to wrap in the X direction\n"
												 "@param wrapY Whether to wrap in the Y direction\n"
			  "@return No return value.")
{
   // Set Wrap.
   object->setWrap( dAtob(argv[2]), dAtob(argv[3]) );
}
// Set Wrap Options.
void t2dTileLayer::setWrap( const bool wrapX, const bool wrapY )
{
   // Set Wrap Options.
   mWrapX = wrapX;
   mWrapY = wrapY;
}

ConsoleMethod(t2dTileLayer, getWrapX, bool, 2, 2, "() - Get Wrap Options.\n"
												  "@return (bool wrapX) Whether the tilelayer wraps in the X direction")
{
   return object->getWrapX();
}

ConsoleMethod(t2dTileLayer, getWrapY, bool, 2, 2, "() - Get Wrap Options.\n"
												  "@return (bool wrapY) Whether the tilelayer wraps in the Y direction")
{
   return object->getWrapY();
}


//-----------------------------------------------------------------------------
// Set Pan Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setPanPositionX, void, 3, 3, "(float panX) - Set the X pan position\n"
														 "@param panX The X pan value\n"
			  "@return No return value.")
{
   object->setPanPosition(dAtof(argv[2]), object->getPanPositionY());
}

ConsoleMethod(t2dTileLayer, setPanPositionY, void, 3, 3, "(float panY) - Set the Y pan position\n"
														 "@param panY The Y pan value\n"
			  "@return No return value.")
{
   object->setPanPosition(object->getPanPositionX(), dAtof(argv[2]));
}

ConsoleMethod(t2dTileLayer, getPanPositionX, F32, 2, 2, "() - Get the X pan position\n"
														"@return (float panX) The X pan position")
{
   return object->getPanPositionX();
}

ConsoleMethod(t2dTileLayer, getPanPositionY, F32, 2, 2, "() - Get the Y pan position\n"
														"@return (float panY) The Y pan position")
{
   return object->getPanPositionY();
}

ConsoleMethod(t2dTileLayer, setPanPosition, void, 3, 4, "(float panX, float panY) - Sets Pan Position.\n"
			  											"@param panX The X pan value\n"
		 												"@param panY The Y pan value\n"
			  "@return No return value.")
{
   // The new position.
   F32 panX, panY;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("panX panY")
   if ((elementCount == 2) && (argc == 3))
   {
      panX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      panY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (panX, panY)
   else if ((elementCount == 1) && (argc == 4))
   {
      panX = dAtof(argv[2]);
      panY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dChunkedSprite::setPanPosition() - Invalid number of parameters!");
      return;
   }

   // Set Pan Position.
   object->setPanPosition(panX, panY);
}
// Set Pan Position.
void t2dTileLayer::setPanPosition( const F32 panX, const F32 panY )
{
   // Set Pan Position.
   mPanPositionX = panX;
   mPanPositionY = panY;

   // Reset Tick Scroll Positions.
   resetTickScrollPositions();
}


//-----------------------------------------------------------------------------
// Set Auto Pan.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setAutoPanX, void, 3, 3, "(float autoPanX) - Set the auto-pan in the X direction\n"
													 "@param autoPanX The rate of horizontal panning\n"
			  "@return No return value.")
{
   object->setAutoPan(dAtof(argv[2]), object->getAutoPanY());
}

ConsoleMethod(t2dTileLayer, setAutoPanY, void, 3, 3, "(float autoPanY) - Set the auto-pan in the Y direction\n"
													 "@param autoPanY The rate of vertical panning\n"
			  "@return No return value.")
{
   object->setAutoPan(object->getAutoPanX(), dAtof(argv[2]));
}

ConsoleMethod(t2dTileLayer, getAutoPanX, F32, 2, 2, "() - Get the auto-pan rate\n"
												    "@return (float autoPanX) The rate of horizontal panning")
{
   return object->getAutoPanX();
}

ConsoleMethod(t2dTileLayer, getAutoPanY, F32, 2, 2, "() - Get the auto-pan rate\n"
												    "@return (float autoPanY) The rate of vertical panning")
{
   return object->getAutoPanY();
}

ConsoleMethod(t2dTileLayer, setAutoPan, void, 3, 4, "(float autoPanX, float autoPanY) - Sets Auto-Pan.\n"
													"@param autoPanX The rate of horizontal panning\n"
													"@param autoPanY The rate of vertical panning\n"
			  "@return No return value.")
{
   // The new position.
   F32 autoPanX;
   F32 autoPanY;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("autoPanX autoPanY")
   if ((elementCount == 2) && (argc == 3))
   {
      autoPanX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      autoPanY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (autoPanX, autoPanY)
   else if ((elementCount == 1) && (argc == 4))
   {
      autoPanX = dAtof(argv[2]);
      autoPanY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setAutoPan() - Invalid number of parameters!");
      return;
   }

   // Set Auto Pan.
   object->setAutoPan(autoPanX, autoPanY);
}
// Set Auto-Pan.
void t2dTileLayer::setAutoPan( const F32 autoPanX, const F32 autoPanY )
{
   // Set Auto-Pan.
   mAutoPanX = autoPanX;
   mAutoPanY = autoPanY;
}


//-----------------------------------------------------------------------------
// Set Auto Pan Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setAutoPanPolar, void, 4, 4, "(float angle, float panSpeed) - Sets Auto-Pan Polarwise.\n"
														 "@param angle The direction of panning\n"
														 "@param panSpeed The speed to pan in that direction\n"
			  "@return No return value.")
{
   // Renormalise Angle.
   const F32 angle = mFmod(dAtof(argv[2]), 360.0f);
   // Fetch Speed.
   const F32 panSpeed = dAtof(argv[3]);

   // Set Auto Pan Polar.
   object->setAutoPanPolar(angle, panSpeed );
}
// Set Auto Pan Polar.
void t2dTileLayer::setAutoPanPolar( const F32 angle, const F32 panSpeed )
{
   // Set Auto Pan.
   setAutoPan( mSin(mDegToRad(angle))*panSpeed, -mCos(mDegToRad(angle))*panSpeed );
}


//-----------------------------------------------------------------------------
// Set Tile Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileSizeX, void, 3, 3, "(float sizeX) - Set the tile size\n"
													  "@param sizeX The width of an individual tile\n"
			  "@return No return value.")
{
   object->setTileSize(dAtof(argv[2]), object->getTileSizeY());
}

ConsoleMethod(t2dTileLayer, setTileSizeY, void, 3, 3, "(float sizeY) - Set the tile size\n"
													  "@param sizeY The height of an individual tile\n"
			  "@return No return value.")
{
   object->setTileSize(object->getTileSizeX(), dAtof(argv[2]));
}

ConsoleMethod(t2dTileLayer, getTileSizeX, F32, 2, 2, "() - Get the width of an individual tile\n"
													 "@return (float sizeX) The width of an individual tile")
{
   return object->getTileSizeX();
}

ConsoleMethod(t2dTileLayer, getTileSizeY, F32, 2, 2, "() - Get the height of an individual tile\n"
													 "@return (float sizeY) The height of an individual tile")
{
   return object->getTileSizeY();
}

ConsoleMethod(t2dTileLayer, setTileSize, void, 3, 4, "(float tileSizeX, float tileSizeY) - Sets Tile Size.\n"
													 "@param sizeX The width of an individual tile\n"
													 "@param sizeY The height of an individual tile\n"
			  "@return No return value.")
{
   F32 width, height;

   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("width height")
   if ((elementCount == 2) && (argc == 3))
   {
      width = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      height = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (width, [height])
   else if (elementCount == 1)
   {
      width = dAtof(argv[2]);

      if (argc == 4)
         height = dAtof(argv[3]);
      else
         height = width;
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setTileSize() - Invalid number of parameters!");
      return;
   }

   // Set Tile Size.
   object->setTileSize(width, height);
}
// Set Tile Size.
void t2dTileLayer::setTileSize( const F32 tileSizeX, const F32 tileSizeY )
{
   // Set Tile Size.
   mTileSizeX = tileSizeX;
   mTileSizeY = tileSizeY;

   // Update Tile Sizes.
   updateTileSizes();
}


//-----------------------------------------------------------------------------
// Get Tile Size.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileSize, const char*, 2, 2, "() - Gets Tile Size.\n"
														     "@return (t2dVector size) A two-element vector with the width and height of an individual tile")
{
   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);
   // Format Buffer.
   dSprintf( pBuffer, 32, "%f %f", object->getTileSizeX(), object->getTileSizeY() );
   // Return Buffer.
   return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Tile Count.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileCount, const char*, 2, 2, "() - Gets Tile Count.\n"
															  "@return (t2dVector count) A two-element vector with the number of tiles in the X and Y direction")
{
   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);
   // Format Buffer.
   dSprintf( pBuffer, 32, "%d %d", object->getTileCountX(), object->getTileCountY() );
   // Return Buffer.
   return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Tile Type.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileType, const char*, 3, 4, "(int tileX, int tileY ) - Gets a tile type.\n"
															 "@param tileX the X coordinate of the tile\n"
															 "@param tileY the Y coordinate of the tile\n"
															 "@return (string tileType) The type of the tyle (static, animated, active)")
{
   // The tile.
   U32 tileX, tileY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileType() - Invalid number of parameters!");
      return 0;
   }

   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !object->getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return NULL;

   // Check for Empty Tile.
   if ( !pTileObject )
      return NULL;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(256);

   // Return Tile Type.
   switch( pTileObject->mpTileNode->mTileType )
   {
   case t2dTileMap::STATIC_TILE:
      {
         // Cast Static Tile.
         t2dTileMap::cStaticTileNode* pTileNode = (t2dTileMap::cStaticTileNode*)pTileObject->mpTileNode;

         if( !pTileNode || pTileNode->pImageMapDataBlock2D.isNull() )
            break;

         // Format Buffer.
         dSprintf( pBuffer, 256, "static %s %d", pTileNode->pImageMapDataBlock2D->getName(), pTileNode->mFrame );

      } break;

   case t2dTileMap::ANIMATED_TILE:
      {
         // Cast Animation Tile.
         t2dTileMap::cAnimationTileNode* pTileNode = (t2dTileMap::cAnimationTileNode*)pTileObject->mpTileNode;

         // Format Buffer.
         dSprintf( pBuffer, 256, "animated %s", pTileNode->pAnimationController2D->getCurrentAnimation() );

      } break;

   case t2dTileMap::ACTIVE_TILE:
      {
         // Cast Active Tile.
         t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pTileObject->mpTileNode;

         // Format Buffer.
         dSprintf( pBuffer, 256, "active %s", pTileNode->pActiveTile2D->getClassName() );

      } break;

   default:
      // Serious problem if we're here!
      return NULL;
   }

   // Return Buffer.
   return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Tile Type.
// NOTE:-   Unlike the script-call, this one only returns the tile-type without
//          further information.
//          
//-----------------------------------------------------------------------------
t2dTileMap::eTileType t2dTileLayer::getTileType( const U32 tileX, const U32 tileY ) const
{
   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return t2dTileMap::INVALID_TILE;

   // Check for Empty Tile.
   if ( !pTileObject )
      return t2dTileMap::INVALID_TILE;

   // Return Tile Type.
   return pTileObject->mpTileNode->mTileType;
}


//-----------------------------------------------------------------------------
// Get Tile Flip.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileFlip, const char*, 3, 4, "(int tileX, int tileY ) - Gets a tile flip options.\n"
															 "@param tileX The X coordinate of the tile\n"
															 "@param tileY The Y coordinate of the tile\n"
															 "@return (t2dVector flip) A two element vector that gives you the flipX and flipY values")
{
   // The tile.
   U32 tileX, tileY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileFlip() - Invalid number of parameters!");
      return 0;
   }

   bool flipX;
   bool flipY;
   // Get Tile Flip Options.
   if ( !object->getTileFlip( tileX, tileY, flipX, flipY ) )
   {
      // Problem.
      return NULL;
   }

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);

   // Format Buffer.
   dSprintf( pBuffer, 32, "%d %d", S32(flipX), S32(flipY) );

   // Return Buffer.
   return pBuffer;
}
// Get Tile Flip.
bool t2dTileLayer::getTileFlip( const U32 tileX, const U32 tileY, bool& flipX, bool& flipY ) const
{
   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return false;

   // Check for Empty Tile.
   if ( !pTileObject )
      return false;

   // Set Flip Options.
   flipX = pTileObject->mFlipHorizontal;
   flipY = pTileObject->mFlipVertical;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Get Tile Script.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileScript, const char*, 3, 4, "(int tileX / int tileY ) - Gets a tile script.\n"
															   "@param tileX The X coordinate of the tile\n"
															   "@param tileY The Y coordinate of the tile\n"
															   "@return (string script) The tileScript assigned to the tile")
{
   // The tile.
   U32 tileX, tileY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileScript() - Invalid number of parameters!");
      return NULL;
   }

   // Return Tile Script.
   return object->getTileScript( tileX, tileY );
}
// Get Tile Script.
const char* t2dTileLayer::getTileScript( const U32 tileX, const U32 tileY )
{
   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return NULL;

   // Check for Empty Tile.
   if ( !pTileObject )
      return NULL;

   // Check for no Script.
   if ( pTileObject->mShowScript == StringTable->insert("") )
      return NULL;

   // Return Script.
   return pTileObject->mShowScript;
}


//-----------------------------------------------------------------------------
// Get Tile Script Actioned.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileScriptActioned, bool, 3, 4, "(int tileX, int tileY) - Gets a tile script actioned-flag.\n"
																"@param tileX The X coordinate of the tile\n"
																"@param tileY The Y coordinate of the tile\n"
																"@return (bool actioned-flag) Whether the script actioned-flag has been set")
{
   // The tile.
   U32 tileX, tileY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileScriptActioned() - Invalid number of parameters!");
      return 0;
   }

   // Retuen Tile Script Actioned.
   return object->getTileScriptActioned( tileX, tileY );
}
// Get Tile Script Actioned.
bool t2dTileLayer::getTileScriptActioned( const U32 tileX, const U32 tileY )
{
   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return NULL;

   // Check for Empty Tile.
   if ( !pTileObject )
      return NULL;

   // Check for no Script.
   if ( pTileObject->mShowScript == StringTable->insert("") )
      return NULL;

   // Return Script-Actioned Flag.
   return pTileObject->mScriptActioned;
}


//-----------------------------------------------------------------------------
// Get Tile Custom Data.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileCustomData, const char*, 3, 4, "(int tileX, int tileY ) - Gets a tile custom-data.\n"
																   "@param tileX The X coordinate of the tile\n"
																   "@param tileY The Y coordinate of the tile\n"
																   "@return (string data) The custom data from the tile")
{
   // The tile.
   U32 tileX, tileY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileCustomData() - Invalid number of parameters!");
      return 0;
   }

   // Return Tile Custom Data.
   return object->getTileCustomData( tileX, tileY );
}
// Get Tile Custom Data.
const char* t2dTileLayer::getTileCustomData( const U32 tileX, const U32 tileY )
{
   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return NULL;

   // Check for Empty Tile.
   if ( !pTileObject )
      return NULL;

   // Check for no Custom-Data.
   if ( pTileObject->mCustomData == StringTable->insert("") )
      return NULL;

   // Return Custom Data.
   return pTileObject->mCustomData;
}


//-----------------------------------------------------------------------------
// Get Tile Collision.
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, getTileCollision, const char*, 3, 4, "(int tileX, int tileY) - Gets a tile collision options.\n"
																  "@param tileX The X coordinate of the tile\n"
																  "@param tileY The Y coordinate of the tile\n"
																  "@return (string info) The collision information from the tile")
{
   // The tile.
   U32 tileX, tileY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::getTileCollision() - Invalid number of parameters!");
      return 0;
   }

   // Tile Object.
   t2dTileLayer::tTileObject* pTileObject;
   // Check Tile Object.
   if ( !object->getTileObject( tileX, tileY, &pTileObject ) )
      // Problem.
      return NULL;

   // Check for Empty Tile.
   if ( !pTileObject )
      return NULL;


   // Fetch Collision Details.
   bool collisionActive = pTileObject->mReceiveCollisions;
   t2dVector collisionScale = pTileObject->mpPhysics->getCollisionPolyScale();
   U32 polyCount = pTileObject->mpPhysics->getCollisionPolyCount();
   const t2dVector* pPolyBasis = pTileObject->mpPhysics->getCollisionPolyBasis();

   // Set Max Buffer Size.
   const U32 maxBufferSize = 4096;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(maxBufferSize);

   // Set Buffer Counter.
   U32 bufferCount = dSprintf( pBuffer, maxBufferSize, "%d %f %f %d ", S32(collisionActive), collisionScale.mX, collisionScale.mY, polyCount );

   // Add Picked Objects to List.
   for ( U32 n = 0; n < polyCount; n++ )
   {
      // Output Object ID.
      bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%f %f ", pPolyBasis[n].mX, pPolyBasis[n].mY );

      // Finish early if we run out of buffer space.
      if ( bufferCount >= maxBufferSize )
      {
         // Warn.
         Con::warnf("t2dTileLayer::getTileCollision() - Too many collision-points to return to scripts!");
         break;
      }
   }

   // Return Buffer.
   return pBuffer;
}



//-----------------------------------------------------------------------------
// Pick Tile.
//
// NOTE:-   This function needs to be *more* efficient urgently as well as
//          fix a problem with accuracy when selecting arbitrary rotated
//          tile-layers!
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTileLayer, pickTile, const char*, 3, 4, "(int tileX, int tileY) - Picks Tile from world position.\n"
														  "@param tileX The X coordinate of the tile\n"
														  "@param tileY The Y coordinate of the tile\n"
														  "@return (t2dVector tileCoordinate) The tile at that world position")
{
   // The tile.
   F32 tileX, tileY;

   // Grab the element count.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileX tileY")
   if ((elementCount == 2) && (argc < 4))
   {
      tileX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileX, tileY)
   else if ((elementCount == 1) && (argc > 3))
   {
      tileX = dAtof(argv[2]);
      tileY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::pickTile() - Invalid number of parameters!");
      return 0;
   }

   // Fetch World Position.
   Point2F worldPosition( tileX, tileY );

   // Pick Tile.
   Point2I logicalTile = Point2I();
   if ( !object->pickTile( t2dVector(worldPosition.x, worldPosition.y), logicalTile ) )
      // No tile!
      return NULL;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);
   // Format Buffer.
   dSprintf( pBuffer, 32, "%d %d", logicalTile.x, logicalTile.y );
   // Return Buffer.
   return pBuffer;
}
// Pick Tile.
bool t2dTileLayer::pickTile( const t2dVector& worldPosition, Point2I& logicalTile )
{
   F32 logTileOffsetX;
   F32 logTileOffsetY;
   F32 phyTileOffsetX;
   F32 phyTileOffsetY;
   S32 logTileX;
   S32 logTileY;

   // Calculate Logical Fractional-Tile at Intersection.
   const F32 logFracTileX = mPanPositionX / mTileSizeX;
   const F32 logFracTileY = mPanPositionY / mTileSizeY;

   // Deal with Pan-X Direction Appropriately.
   if ( logFracTileX < 0.0f )
   {
      logTileOffsetX = 1.0f + (logFracTileX - S32(logFracTileX));
      phyTileOffsetX = mFabs(logTileOffsetX) * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(logFracTileX - logTileOffsetX + 0.5f) - 1;
   }
   else
   {
      logTileOffsetX = logFracTileX - mFloor(logFracTileX);
      phyTileOffsetX = logTileOffsetX * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(mFloor(logFracTileX - logTileOffsetX));
   }

   // Deal with Pan-Y Direction Appropriately.
   if ( logFracTileY < 0.0f )
   {
      logTileOffsetY = 1.0f + (logFracTileY - S32(logFracTileY));
      phyTileOffsetY = mFabs(logTileOffsetY) * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(logFracTileY - logTileOffsetY + 0.5f) - 1;
   }
   else
   {
      logTileOffsetY = logFracTileY - mFloor(logFracTileY);
      phyTileOffsetY = logTileOffsetY * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(mFloor(logFracTileY - logTileOffsetY));
   }

   // Calculate Physical Tile.
   S32 phyTileX = logTileX;
   S32 phyTileY = logTileY;
   // Wrap Tile-X.
   if ( phyTileX < 0 )
      phyTileX = (phyTileX+mTileCountX) % mTileCountX;
   else if ( phyTileX >= mTileCountX )
      phyTileX %= mTileCountX;
   // Wrap Tile-Y.
   if ( phyTileY < 0 )
      phyTileY = (phyTileY+mTileCountY) % mTileCountY;
   else if ( phyTileY >= mTileCountY )
      phyTileY %= mTileCountY;

   // Calculate Tile Clip Region.
   t2dVector tileClipStep[3];
   tileClipStep[0] = mWorldClipBoundary[1] - mWorldClipBoundary[0];
   tileClipStep[2] = mWorldClipBoundary[3] - mWorldClipBoundary[0];

   // Renormalise to Calculate Tile-Sized Clip Step.
   tileClipStep[0].normalise(mTileSizeX);
   tileClipStep[2].normalise(mTileSizeY);
   tileClipStep[1] = tileClipStep[0] + tileClipStep[2];

   // Calculate Tile Offset.
   const t2dVector tileOffset = (tileClipStep[0] * logTileOffsetX) + (tileClipStep[2] * logTileOffsetY);

   // Initialise Base/Current Tile World Clip Boundary.
   t2dVector baseTileWorldClipBoundary[4];
   // UNUSED: JOSEPH THOMAS -> t2dVector CurrentTileWorldClipBoundary[4];

   // Calculate Base/Current Tile Clip.
   baseTileWorldClipBoundary[0] = mWorldClipBoundary[0] - tileOffset;
   baseTileWorldClipBoundary[1] = mWorldClipBoundary[0] - tileOffset + tileClipStep[0];
   baseTileWorldClipBoundary[2] = mWorldClipBoundary[0] - tileOffset + tileClipStep[1];
   baseTileWorldClipBoundary[3] = mWorldClipBoundary[0] - tileOffset + tileClipStep[2];

   // Calculate Local Pick Coordinates.
   const t2dVector localPickCoord = (worldPosition - baseTileWorldClipBoundary[0]) * getInverseRotationMatrix();

	// RKS: THIS IS ONLY HERE TO FIGURE OUT WHY THE TILE LAYER'S WORLD CLIP IS MESSED UP
	//dPrintf( "pickTile(), World Clip Boundary: (%f, %f)\n", mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY );
	
   // Trivially check to see if we're actually inside the tile-layer boundary.
   if (    localPickCoord.mX < 0.0f || localPickCoord.mY < 0.0f ||
      localPickCoord.mX >= getSize().mX || localPickCoord.mY >= getSize().mY)
      return false;

   // Calculate Logical Output Tile Position.
   const S32 logOutTileX = S32(localPickCoord.mX/mTileSizeX) + logTileX;
   const S32 logOutTileY = S32(localPickCoord.mY/mTileSizeY) + logTileY;

    // Check We've got a valid tile.
    if (    mWrapX ||   // Wrap Mode-X On.
            ( !mWrapX && (logOutTileX >= 0 && logOutTileX < mTileCountX ) ) )// Wrap Mode-X Off but within tile-boundary.
    {
        // Adjust Physical Tile-X.
        phyTileX += logOutTileX - logTileX;
        if ( phyTileX < 0 )
            phyTileX = (phyTileX+mTileCountX) % mTileCountX;
        else if ( phyTileX >= mTileCountX )
            phyTileX %= mTileCountX;

        if (    mWrapY ||   // Wrap Mode-Y On.
                ( !mWrapY && (logOutTileY >= 0 && logOutTileY < mTileCountY ) ) )   // Wrap Mode-Y Off but within tile-boundary.
        {
            // Adjust Physical Tile-Y.
            phyTileY += logOutTileY - logTileY;
            if ( phyTileY < 0 )
                phyTileY = (phyTileY+mTileCountY) % mTileCountY;
            else if ( phyTileY >= mTileCountY )
                phyTileY %= mTileCountY;

         // Yes, so reference physical tile.
         logicalTile.set( phyTileX, phyTileY );
         // Return Tile selected!
         return true;
      }
      else
      {
         // No Tile!
         return false;
      }
   }
   else
   {
      // No Tile!
      return false;
   }
}


//-----------------------------------------------------------------------------
// Set Grid-Active Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setGridActive, void, 3, 3, "(bool status) - Set grid-lines status.\n"
													   "@param status Whether the grid-lines are on or off\n"
			  "@return No return value.")
{
   // Set Grid-Active Status.
   object->setGridActive( dAtob(argv[2]) );
}
// Set Grid-Active Status.
void t2dTileLayer::setGridActive( const bool status )
{
   // Set Grid-Active Status.
   mGridActive = status;
}


//-----------------------------------------------------------------------------
// Set Custom-Data Icon Active Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setCustomIconActive, void, 3, 3, "(bool status) - Set custom-data-icon status.\n"
															 "@param status Whether the custom data icon is active or not\n"
			  "@return No return value.")
{
   // Set Custom-Data Icon Active Status.
   object->setCustomIconActive( dAtob(argv[2]) );
}
// Set Custom-Data Icon Active Status.
void t2dTileLayer::setCustomIconActive( const bool status )
{
   // Set Custom-Data Icon Active Status.
   mCustomIconActive = status;
}


//-----------------------------------------------------------------------------
// Set Script-Icon Active Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setScriptIconActive, void, 3, 3, "(bool status) - Set script-icon status.\n"
															 "@param status Whether or not the script-icon is active\n"
			  "@return No return value.")
{
   // Set Script-Icon Active Status.
   object->setScriptIconActive( dAtob(argv[2]) );
}
// Set Script-Icon Active Status.
void t2dTileLayer::setScriptIconActive( const bool status )
{
   // Set Script-Icon Active Status.
   mScriptIconActive = status;
}


//-----------------------------------------------------------------------------
// Set Cursor Icon Active Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setCursorIconActive, void, 3, 4, "(bool status, float blinkTime) - Set cursor-icon status.\n"
															 "@param status Whether the cursor is active\n"
															 "@param blinkTime The cursors blink time\n"
			  "@return No return value.")
{
   // Calculate Blink-Time.
   const F32 blinkTime = (argc >= 4) ? dAtof(argv[3]) : 0.25f;

   // Set Script-Icon Active Status.
   object->setCursorIconActive( dAtob(argv[2]), blinkTime );
}
void t2dTileLayer::setCursorIconActive( const bool status, const F32 blinkTime )
{
   // Set Cursor Icon Active Status.
   mCursorIconActive = status;
   // Set Cursor Blink Time.
   mCurrentCursorBlinkTime = mCursorBlinkTime = blinkTime;
   // Reset Current Cursor Blink State.
   mCurrentCursorBlinkState = true;
}


//-----------------------------------------------------------------------------
// Set Cursor Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setCursorPosition, void, 3, 4, "(int tileX, int tileY) - Set cursor position.\n"
														   "@param tileX The X coordinate to position the cursor at\n"
														   "@param tileY the Y coordinate to position the cursor at\n"
			  "@return No return value.")
{
   U32 tileX;
   U32 tileY;

   // Elements in the first argument.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("tileCountX tileCountY")
   if ((elementCount == 2) && (argc == 3))
   {
      tileX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      tileY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (tileCountX, tileCountY)
   else if ((elementCount == 1) && (argc == 4))
   {
      tileX = dAtoi(argv[2]);
      tileY = dAtoi(argv[3]);
   }
   // Invalid.
   else
   {
      Con::warnf("t2dTileLayer::setCursorPosition() - Invalid number of parameters!");
      return;
   }

   // Set Cursor Position.
   object->setCursorPosition( tileX, tileY );
}
// Set Cursor Position.
void t2dTileLayer::setCursorPosition( const U32 tileX, const U32 tileY )
{
   // Set Cursor Blink Time.
   mCurrentCursorBlinkTime = mCursorBlinkTime;
   // Reset Current Cursor Blink State.
   mCurrentCursorBlinkState = true;

   // Set Current Cursor Position.
   mCurrentCursorX = tileX;
   mCurrentCursorY = tileY;
}


//-----------------------------------------------------------------------------
// Set Tile-Icon ImageMap.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, setTileIcons, void, 3, 3, "(string tileIconImageMap) - Set tile-icons imagemap.\n"
													  "@param tileIconImageMap The imagemap to use for the tile icons\n"
			  "@return No return value.")
{
   // Set Tile-Icon ImageMap.
   object->setTileIcons( argv[2] );
}
// Set Tile-Icon ImageMap.
void t2dTileLayer::setTileIcons( const char* imageMapName )
{
   // Find ImageMap Datablock.
   t2dImageMapDatablock* pImageMapDataBlock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject( imageMapName ));

   // Set Datablock.
   if ( !t2dCheckDatablock( pImageMapDataBlock ) )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileIcons() - t2dImageMapDatablock Datablock is invalid! (%s)", imageMapName);
      // Return Here.
      return;
   }

   // Set Frame Icons Required.
   const U32 frameIconsRequired = 2;

   // Check Frame Validity.
   if ( pImageMapDataBlock->getImageMapFrameCount() < frameIconsRequired )
   {
      // Warn.
      Con::warnf("t2dTileLayer::setTileIcons() - Invalid Number of Frames available %d; require at least %d! (%s)", pImageMapDataBlock->getImageMapFrameCount(), frameIconsRequired, imageMapName);
      // Return Here.
      return;
   }

   // Set ImageMap Datablock.
   mTileIconImageMapDataBlock = pImageMapDataBlock;
}


//-----------------------------------------------------------------------------
// Reset Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, resetLayer, void, 2, 2, "() Resets dynamic properties of the Tile-Layer.\n"
			  "@return No return value.")
{
   // Reset Layer.
   object->resetLayer();
}
// Reset Layer.
void t2dTileLayer::resetLayer( void )
{
   // Finish if we've not got a tile array.
   if ( !mppTileObjectArray )
      return;

   // Step through tiles.
   for ( U32 tileY = 0; tileY < mTileCountY; tileY++ )
   {
      // Calculate Tile Offset.
      U32 tileOffset = tileY * mTileCountX;

      // Step through tiles.
      for ( U32 tileX = 0; tileX < mTileCountX; tileX++ )
      {
         // Fetch Tile reference.
         tTileObject* pTileObject = mppTileObjectArray[tileOffset];

         // Tile Object?
         if ( pTileObject )
         {
            // Yes, so reset script actioned flag.
            pTileObject->mScriptActioned = false;

            // Handle Tile Types Appropriately.
            switch( pTileObject->mpTileNode->mTileType )
            {
               // Static Tile.
            case t2dTileMap::STATIC_TILE:
               {
                  // Nothing to do!

               } break;

               // Animation Tile.
            case t2dTileMap::ANIMATED_TILE:
               {
                  // Reset Animation Tile Time.
                  ((t2dTileMap::cAnimationTileNode*)pTileObject->mpTileNode)->pAnimationController2D->resetTime();

               } break;

               // Active Tile.
            case t2dTileMap::ACTIVE_TILE:
               {
                  // Reset Active Tile.
                  ((t2dTileMap::cActiveTileNode*)pTileObject->mpTileNode)->pActiveTile2D->resetTile();

               } break;
            }
         }

         // Next tile.
         tileOffset++;
      }
   }
}


//-----------------------------------------------------------------------------
// Update Tile Sizes.
//-----------------------------------------------------------------------------
void t2dTileLayer::updateTileSizes( void )
{
   // Finish if we've not got a tile array.
   if ( !mppTileObjectArray )
      return;

   // Calculate Total Tile Count.
   const U32 tileCount = mTileCountX * mTileCountY;

   // Calculate Tile Size.
   const t2dVector tileSize( mTileSizeX, mTileSizeY );

   // Fetch Tile Pointer.
   tTileObject** ppTileArray = mppTileObjectArray;

   // Step through all tiles.
   for ( U32 n = 0; n < tileCount; n++, ppTileArray++ )
      if ( (*ppTileArray) )
         (*ppTileArray)->mpPhysics->setSize( tileSize );
}


//-----------------------------------------------------------------------------
// Load Tile Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, loadTileLayer, bool, 3, 3, "(string tileLayerFile) - Loads a Tile-Layer File.\n"
													   "@param tileLayerFile The tileLayer file to load"
													   "@return Returns true on success.")
{
   // Load Tile Layer.
   return object->loadTileLayer( argv[2] );
}
// Load TileMap.
bool t2dTileLayer::loadTileLayer( const char* tileLayerFile )
{
   if (!mpTileMap2DManager)
   {
      Con::errorf("Cannot load tile layer! Layer not in a valid tile map.");
      return false;
   }

   // Expand relative paths.
   static char buffer[1024];
   if ( tileLayerFile )
      if ( Con::expandScriptFilename( buffer, sizeof( buffer ), tileLayerFile ) )
         tileLayerFile = buffer;

   // Clear Tile Layer.
   clearLayer();

   // Open Scene File.
   Stream* pStream = ResourceManager->openStream( tileLayerFile );
   // Check Stream.
   if ( !pStream )
   {
      // Warn.
      Con::warnf("t2dTileMap::loadTileLayer() - Could not Open File '%s' for Tile-Layer Load.", tileLayerFile);
      // Return Error.
      return false;
   }

   // Scene Objects.
   Vector<t2dSceneObject*> ObjReferenceList;

   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( ObjReferenceList );

   // Load TileMap Stream.
   if ( !loadStream( *pStream, getSceneGraph(), ObjReferenceList, true ) )
   {
      // Warn.
      Con::warnf("t2dTileMap::loadTileLayer() - Could not Read from Stream '%s' for Tile-Layer Load.", tileLayerFile);
      // Return Error.
      return false;
   }

   // Reset Layer.
   resetLayer();

   mLayerFile = StringTable->insert(tileLayerFile);

   // Close Stream.
   ResourceManager->closeStream( pStream );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Save Tile Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTileLayer, saveTileLayer, bool, 3, 3, "(string tileLayerFile) - Saves a Tile-Layer File.\n"
													   "@param tileLayerFile The tileLayer file to save to\n"
													   "@return Returns true on success.")
{
   // Save Tile Layer.
   return object->saveTileLayer( argv[2] );
}
// Save Tile Layer.
bool t2dTileLayer::saveTileLayer( const char* tileLayerFile )
{
   // Must have a parent!
   if ( !mpTileMap2DManager )
      return false;

   // We *must* have a scenegraph!
   if ( mpTileMap2DManager->getSceneGraph() == NULL )
   {
      // Warn.
      Con::warnf("t2dTileLayer::saveTileLayer() - Cannot Save without SceneGraph!");
      return false;
   }

   // Expand relative paths.
   static char buffer[1024];
   if ( tileLayerFile )
      if ( Con::expandToolScriptFilename( buffer, sizeof( buffer ), tileLayerFile ) )
         tileLayerFile = buffer;

   // Open Scene File.
   FileStream fileStream;
   if ( !ResourceManager->openFileForWrite( fileStream, tileLayerFile, FileStream::Write ) )
   {
      // Warn.
      Con::warnf("t2dTileMap::saveTileLayer() - Could not open File '%s' for Tile-Layer Save.", tileLayerFile);
      return false;
   }

   // Save Tile-Layer Stream (Ensure Serialise ID doesn't get in the way).
   if ( !saveStream( fileStream, mpTileMap2DManager->getSceneGraph()->getNextSerialiseID(), 0 ) )
   {
      // Warn.
      Con::warnf("t2dTileMap::saveTileLayer() - Could not Save to Stream '%s' for Tile-Layer Save.", tileLayerFile);
      return false;
   }

   Con::executef( this, 1, "onLayerSaved" );

   // Update Layer File 
   mLayerFile = StringTable->insert( tileLayerFile );

   // Return Okay.
   return true;
}


t2dVector t2dTileLayer::getTileLocalPoint( U32 tileX, U32 tileY, t2dVector layerLocalPoint )
{
   t2dVector tileLocalPoint = t2dVector::getZero();
   F32 tileXOffset = (F32)tileX / (F32)getTileCountX();
   F32 tileYOffset = (F32)tileY / (F32)getTileCountY();
   F32 pointXOffset = ( layerLocalPoint.mX + 1.0f ) / 2.0f;
   F32 pointYOffset = ( layerLocalPoint.mY + 1.0f ) / 2.0f;
   tileLocalPoint.mX = ( pointXOffset - tileXOffset ) * (F32)getTileCountX();
   tileLocalPoint.mY = ( pointYOffset - tileYOffset ) * (F32)getTileCountY();
   tileLocalPoint *= 2.0f;
   tileLocalPoint -= t2dVector( 1.0f, 1.0f );

   return tileLocalPoint;
}

//  0 = No Intersections
//  1 = 1 Intersection
// -1 = Coincident
U32 t2dTileLayer::getLineLineIntersection( t2dVector a1, t2dVector a2, t2dVector b1, t2dVector b2, Vector<t2dVector>& intersections )
{
   F32 ua_t = (b2.mX - b1.mX) * (a1.mY - b1.mY) - (b2.mY - b1.mY) * (a1.mX - b1.mX);
   F32 ub_t = (a2.mX - a1.mX) * (a1.mY - b1.mY) - (a2.mY - a1.mY) * (a1.mX - b1.mX);
   F32 u_b  = (b2.mY - b1.mY) * (a2.mX - a1.mX) - (b2.mX - b1.mX) * (a2.mY - a1.mY);

   if ( mNotEqual( u_b, 0.0f ) )
   {
      F32 ua = ua_t / u_b;
      F32 ub = ub_t / u_b;

      if ( 0.0f <= ua && ua <= 1.0f && 0.0f <= ub && ub <= 1.0f )
      {
         intersections.push_back( t2dVector( a1.mX + ua * (a2.mX - a1.mX), a1.mY + ua * (a2.mY - a1.mY) ) );
         return 1;
      }
      else
         return 0;

   }
   else
   {
      if ( ua_t == 0 || ub_t == 0 )
         return -1;
      else
         return 0;
   }

   return 0;
}

U32 t2dTileLayer::getLinePolygonIntersection( t2dVector a1, t2dVector a2, Vector<t2dVector>& poly, Vector<t2dVector>& intersections )
{
   U32 count = 0;
   for( U32 i = 0; i < poly.size(); i++ )
   {
      t2dVector b1 = poly[i];
      t2dVector b2 = poly[(i + 1) % poly.size()];
      count += getLineLineIntersection( a1, a2, b1, b2, intersections );
   }

   return count;
}

bool t2dTileLayer::getPointInPolygon( t2dVector point, Vector<t2dVector>& poly )
{
   U32 count = poly.size();
   bool in = false;
   for( S32 i = 0, j = count - 1; i < count; j = i, i++ )
   {
      if( ( ( poly[i].mY <= point.mY ) && ( point.mY < poly[j].mY ) ) ||
         ( ( poly[j].mY <= point.mY ) && ( point.mY < poly[i].mY ) ) )
      {
         F32 xint = ( point.mY - poly[i].mY ) * ( poly[j].mX - poly[i].mX ) / ( poly[j].mY - poly[i].mY ) + poly[i].mX;
         if( point.mX <= xint )
            in = !in;
      }
   }
   return in;
}

U32 t2dTileLayer::getRectPolygonIntersection( RectF rect, Vector<t2dVector>& poly, Vector<t2dVector>& intersections )
{
   t2dVector topLeft     = t2dVector( rect.point.x, rect.point.y );
   t2dVector topRight    = t2dVector( rect.point.x + rect.extent.x, rect.point.y );
   t2dVector bottomLeft  = t2dVector( rect.point.x, rect.point.y + rect.extent.y );
   t2dVector bottomRight = t2dVector( rect.point.x + rect.extent.x, rect.point.y + rect.extent.y );

   getLinePolygonIntersection( topLeft, topRight, poly, intersections );
   if( getPointInPolygon( topRight, poly ) )
      intersections.push_back( topRight );

   getLinePolygonIntersection( topRight, bottomRight, poly, intersections );
   if( getPointInPolygon( bottomRight, poly ) )
      intersections.push_back( bottomRight );

   bool sign;
   if( getLinePolygonIntersection( bottomRight, bottomLeft, poly, intersections ) == 2 )
   {
      if( !t2dPhysics::isPolyConvex( intersections.size(), intersections.address(), sign ) )
      {
         t2dVector pt1 = intersections[intersections.size() - 2];
         t2dVector pt2 = intersections[intersections.size() - 1];
         intersections[intersections.size() - 2] = pt2;
         intersections[intersections.size() - 1] = pt1;
      }
   }
   if( getPointInPolygon( bottomLeft, poly ) )
      intersections.push_back( bottomLeft);

   if( getLinePolygonIntersection( bottomLeft, topLeft, poly, intersections ) == 2 )
   {
      if( !t2dPhysics::isPolyConvex( intersections.size(), intersections.address(), sign ) )
      {
         t2dVector pt1 = intersections[intersections.size() - 2];
         t2dVector pt2 = intersections[intersections.size() - 1];
         intersections[intersections.size() - 2] = pt2;
         intersections[intersections.size() - 1] = pt1;
      }
   }
   if( getPointInPolygon( topLeft, poly ) )
      intersections.push_back( topLeft );

   // If any of the points are in the rect, we want to use them as well.
   for( U32 i = 0; i < poly.size(); i++ )
   {
      if( ( poly[i].mX >= topLeft.mX ) && ( poly[i].mX <= bottomRight.mX ) &&
         ( poly[i].mY >= topLeft.mY ) && ( poly[i].mY <= bottomRight.mY ) )
      {
         // Special case if all points are in the rect.
         if( intersections.empty() )
         {
            intersections.push_back( poly[i] );
            continue;
         }

         bool sign;
         Vector<t2dVector>::iterator it;
         for( it = intersections.begin(); it != intersections.end(); it++ )
         {
            intersections.insert( it, poly[i] );

            if( t2dPhysics::isPolyConvex( intersections.size(), intersections.address(), sign ) )
               break;
            else
               intersections.erase( it );
         }
      }
   }

   return intersections.size();
}

bool t2dTileLayer::setTileCollisionPolyCustomGlobal( const U32 tileX, const U32 tileY, const U32 polyVertexCount, const t2dVector *pCustomPolygon )
{
   Vector<t2dVector> intersections;

   Vector<t2dVector> poly;
   for( U32 i = 0; i < polyVertexCount; i++ )
   {
      t2dVector point = getTileLocalPoint( tileX, tileY, pCustomPolygon[i] );
      poly.push_back( point );
   }

   RectF tileRect = RectF( -1.0f, -1.0f, 2.0f, 2.0f );
   U32 count = getRectPolygonIntersection( tileRect, poly, intersections );

   if( count > 0 )
   {
      setTileCollisionActive( tileX, tileY, true );
      return setTileCollisionPolyCustom( tileX, tileY, intersections.size(), intersections.address() );
   }

   return false;
}

void t2dTileLayer::setCollisionPolyCustom( const U32 polyVertexCount, const t2dVector *pCustomPolygon )
{
   for( U32 x = 0; x < getTileCountX(); x++ )
   {
      for( U32 y = 0; y < getTileCountY(); y++ )
      {
         setTileCollisionPolyCustomGlobal( x, y, polyVertexCount, pCustomPolygon );
      }
   }
}

//-----------------------------------------------------------------------------
// Get Collision Area Physics.
//
// NOTE:-   Here we check to see which tiles overlap the specified world-area
//          but we need to ensure that each tile doesn't get its physics
//          updated repeatedly.  We do this by utilising the multi-purpose
//          "update-sequence" that the scene-graph maintains during the
//          scene update-phase.  A tiles' physics is only updated if the
//          update-sequence is out-of-sync.
//-----------------------------------------------------------------------------
t2dSceneObject::typePhysicsVector& t2dTileLayer::getCollisionAreaPhysics( const RectF& worldArea )
{
   // Choose Appropriate Render Method.
   if ( mIsZero( getRotation() ) )
      // No Rotation.
      return getCollisionAreaPhysics_ZeroAngle( worldArea );
   else
      // Arbitrary Angle.
      return getCollisionAreaPhysics_ArbitraryAngle( worldArea );
}


//-----------------------------------------------------------------------------
// Get Collision Area Physics (Zero Angle).
//-----------------------------------------------------------------------------
t2dSceneObject::typePhysicsVector& t2dTileLayer::getCollisionAreaPhysics_ZeroAngle( const RectF& worldArea )
{
   // Clear Collision Physics List.
   mCollisionPhysicsList.clear();

   // Cannot do anything without tile-object array!
   if ( !mppTileObjectArray )
      return mCollisionPhysicsList;

   // Calculate Clipped World Area.
   RectF clippedWorldArea = worldArea;
   clippedWorldArea.intersect( getWorldClipRectangle() );

   // Fetch Scene Update Sequence Key.
   U32 sceneUpdateSequenceKey = getSceneGraph()->getUpdateSequenceKey();

   // Calculate Logical Fractional-Tile at Intersection.
   const F32 logFracTileX = (clippedWorldArea.point.x - mWorldClipBoundary[0].mX + mPanPositionX) / mTileSizeX;
   const F32 logFracTileY = (clippedWorldArea.point.y - mWorldClipBoundary[0].mY + mPanPositionY) / mTileSizeY;

   F32 logTileOffsetX;
   F32 logTileOffsetY;
   F32 phyTileOffsetX;
   F32 phyTileOffsetY;
   S32 logTileX;
   S32 logTileY;

   // Deal with Pan-X Direction Appropriately.
   if ( logFracTileX < 0.0f )
   {
      logTileOffsetX = 1.0f + (logFracTileX - S32(logFracTileX));
      phyTileOffsetX = mFabs(logTileOffsetX) * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(logFracTileX - logTileOffsetX + 0.5f) - 1;
   }
   else
   {
      logTileOffsetX = logFracTileX - mFloor(logFracTileX);
      phyTileOffsetX = logTileOffsetX * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(mFloor(logFracTileX - logTileOffsetX));
   }

   // Deal with Pan-Y Direction Appropriately.
   if ( logFracTileY < 0.0f )
   {
      logTileOffsetY = 1.0f + (logFracTileY - S32(logFracTileY));
      phyTileOffsetY = mFabs(logTileOffsetY) * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(logFracTileY - logTileOffsetY + 0.5f) - 1;
   }
   else
   {
      logTileOffsetY = logFracTileY - mFloor(logFracTileY);
      phyTileOffsetY = logTileOffsetY * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(mFloor(logFracTileY - logTileOffsetY));
   }

   // Calculate Physical Tile.
   S32 phyTileX = logTileX;
   S32 phyTileY = logTileY;
   // Wrap Tile-X.
   if ( phyTileX < 0 )
      phyTileX = (phyTileX+mTileCountX) % mTileCountX;
   else if ( phyTileX >= mTileCountX )
      phyTileX %= mTileCountX;
   // Wrap Tile-Y.
   if ( phyTileY < 0 )
      phyTileY = (phyTileY+mTileCountY) % mTileCountY;
   else if ( phyTileY >= mTileCountY )
      phyTileY %= mTileCountY;

   // Calculate Tile Offset.
   t2dVector tileOffset( phyTileOffsetX, phyTileOffsetY );

   // Calculate Tiles in area.
   const U32 tilesViewedX = U32( mCeil( (clippedWorldArea.len_x() + phyTileOffsetX) / mTileSizeX ) );
   const U32 tilesViewedY = U32( mCeil( (clippedWorldArea.len_y() + phyTileOffsetY) / mTileSizeY ) );

   // Calculate Renormalised Tile Clip Steps.
   t2dVector tileClipStep[3];
   tileClipStep[0] = t2dVector(mTileSizeX, 0);
   tileClipStep[2] = t2dVector(0, mTileSizeY);
   tileClipStep[1] = tileClipStep[0] + tileClipStep[2];

   // Calculate Center Offset.
   const t2dVector tileCenter = tileClipStep[1] * 0.5f;


   // ******************************************************************
   //
   //  To save some time, we're going to be doing some extrapolated
   //  tile-movement in world-space.  World tile-steps are calculated
   //  and used to step through the tiles prior to rendering.
   //
   // ******************************************************************

   // Initialise Base/Current Tile World Clip Boundary.
   t2dVector baseTileWorldClipBoundary[4];
   t2dVector CurrentTileWorldClipBoundary[4];

   // Calculate Base/Current Tile Clip.
   baseTileWorldClipBoundary[0] = clippedWorldArea.point - tileOffset;
   baseTileWorldClipBoundary[1] = clippedWorldArea.point - tileOffset + tileClipStep[0];
   baseTileWorldClipBoundary[2] = clippedWorldArea.point - tileOffset + tileClipStep[1];
   baseTileWorldClipBoundary[3] = clippedWorldArea.point - tileOffset + tileClipStep[2];

   // Reset Tile Positions Y.
   S32 logTilePositionY = logTileY;
   U32 phyTilePositionY = phyTileY;

   // Render all the Y-tiles.
   for ( U32 y = 0; y < tilesViewedY; y++ )
   {
      // Are we rendering a tile here?
      if (    mWrapY ||   // Wrap Mode-Y On.
         ( !mWrapY && (logTilePositionY >= 0 && logTilePositionY < mTileCountY ) ) ) // Wrap Mode-Y Off but within tile-boundary.
      {
         // Transfer base clip into current.
         CurrentTileWorldClipBoundary[0] = baseTileWorldClipBoundary[0];
         CurrentTileWorldClipBoundary[1] = baseTileWorldClipBoundary[1];
         CurrentTileWorldClipBoundary[2] = baseTileWorldClipBoundary[2];
         CurrentTileWorldClipBoundary[3] = baseTileWorldClipBoundary[3];

         // Reset Tile Positions X.
         S32 logTilePositionX = logTileX;
         U32 phyTilePositionX = phyTileX;

         // Render all the X-tiles.
         for ( U32 x = 0; x < tilesViewedX; x++ )
         {
            // Are we rendering a tile here?
            if (    mWrapX ||   // Wrap Mode-X On.
               ( !mWrapX && (logTilePositionX >= 0 && logTilePositionX < mTileCountX ) ) )// Wrap Mode-X Off but within tile-boundary.
            {
               // Yes, so fetch Tile Object.
               tTileObject* pTileObject = mppTileObjectArray[phyTilePositionX + ( phyTilePositionY * mTileCountX )];

               // Is there a tile object here and available for receive collision?
               if ( pTileObject && pTileObject->mReceiveCollisions )
               {
                  // Yes, so has this tile been updated this sequence?
                  if ( pTileObject->mUpdateSequenceKey != sceneUpdateSequenceKey )
                  {
                     // No, so set sequence.
                     pTileObject->mUpdateSequenceKey = sceneUpdateSequenceKey;

                     // Update Tiles Physics.
                     pTileObject->mpPhysics->setPosition( CurrentTileWorldClipBoundary[0] + tileCenter );                            
                     pTileObject->mpPhysics->setRotation( 0.0f );
                  }

                  // Add Physics to Collision List.
                  mCollisionPhysicsList.push_back( pTileObject->mpPhysics );
               }
            }

            // Move Current Clip Boundary in X.
            CurrentTileWorldClipBoundary[0] += tileClipStep[0];
            CurrentTileWorldClipBoundary[1] += tileClipStep[0];
            CurrentTileWorldClipBoundary[2] += tileClipStep[0];
            CurrentTileWorldClipBoundary[3] += tileClipStep[0];

            // Move to next Tile-X Position.
            logTilePositionX++;
            phyTilePositionX++;

            // Wrap Physical Tile Position.
            if ( phyTilePositionX >= mTileCountX )
               phyTilePositionX = 0;
         }
      }

      // Move Base Clip Boundary in Y.
      baseTileWorldClipBoundary[0] += tileClipStep[2];
      baseTileWorldClipBoundary[1] += tileClipStep[2];
      baseTileWorldClipBoundary[2] += tileClipStep[2];
      baseTileWorldClipBoundary[3] += tileClipStep[2];

      // Move to next Tile-Y Position.
      logTilePositionY++;
      phyTilePositionY++;

      // Wrap Physical Tile Position.
      if ( phyTilePositionY >= mTileCountY )
         phyTilePositionY = 0;
   }

   // Return Collision List.
   return mCollisionPhysicsList;
}


//-----------------------------------------------------------------------------
// Get Collision Area Physics (Arbitrary Angle).
//-----------------------------------------------------------------------------
t2dSceneObject::typePhysicsVector& t2dTileLayer::getCollisionAreaPhysics_ArbitraryAngle( const RectF& worldArea )
{
   // Clear Collision Physics List.
   mCollisionPhysicsList.clear();

   // Cannot do anything without tile-object array!
   if ( !mppTileObjectArray )
      return mCollisionPhysicsList;

   // Calculate Clipped World Area.
   RectF clippedWorldArea = worldArea;
   clippedWorldArea.intersect( getWorldClipRectangle() );

   // Fetch Scene Update Sequence Key.
   const U32 sceneUpdateSequenceKey = getSceneGraph()->getUpdateSequenceKey();

   // Calculate Logical Fractional-Tile at Intersection.
   const F32 logFracTileX = mPanPositionX / mTileSizeX;
   const F32 logFracTileY = mPanPositionY / mTileSizeY;

   F32 logTileOffsetX;
   F32 logTileOffsetY;
   F32 phyTileOffsetX;
   F32 phyTileOffsetY;
   S32 logTileX;
   S32 logTileY;

   // Deal with Pan-X Direction Appropriately.
   if ( logFracTileX < 0.0f )
   {
      logTileOffsetX = 1.0f + (logFracTileX - S32(logFracTileX));
      phyTileOffsetX = mFabs(logTileOffsetX) * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(logFracTileX - logTileOffsetX + 0.5f) - 1;
   }
   else
   {
      logTileOffsetX = logFracTileX - mFloor(logFracTileX);
      phyTileOffsetX = logTileOffsetX * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(mFloor(logFracTileX - logTileOffsetX));
   }

   // Deal with Pan-Y Direction Appropriately.
   if ( logFracTileY < 0.0f )
   {
      logTileOffsetY = 1.0f + (logFracTileY - S32(logFracTileY));
      phyTileOffsetY = mFabs(logTileOffsetY) * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(logFracTileY - logTileOffsetY + 0.5f) - 1;
   }
   else
   {
      logTileOffsetY = logFracTileY - mFloor(logFracTileY);
      phyTileOffsetY = logTileOffsetY * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(mFloor(logFracTileY - logTileOffsetY));
   }

   // Calculate Physical Tile.
   S32 phyTileX = logTileX;
   S32 phyTileY = logTileY;
   // Wrap Tile-X.
   if ( phyTileX < 0 )
      phyTileX = (phyTileX+mTileCountX) % mTileCountX;
   else if ( phyTileX >= mTileCountX )
      phyTileX %= mTileCountX;
   // Wrap Tile-Y.
   if ( phyTileY < 0 )
      phyTileY = (phyTileY+mTileCountY) % mTileCountY;
   else if ( phyTileY >= mTileCountY )
      phyTileY %= mTileCountY;

   // Calculate Tile Clip Region.
   t2dVector tileClipStep[3];
   tileClipStep[0] = (mWorldClipBoundary[1] - mWorldClipBoundary[0]);
   tileClipStep[2] = (mWorldClipBoundary[3] - mWorldClipBoundary[0]);

   // Calculate Tiles Viewed.
   const U32 tilesViewedX = U32( mCeil( (tileClipStep[0].len() + phyTileOffsetX + 0.5f) / mTileSizeX ) ) - (mIsZero(phyTileOffsetX)?1:0);
   const U32 tilesViewedY = U32( mCeil( (tileClipStep[2].len() + phyTileOffsetY + 0.5f) / mTileSizeY ) ) - (mIsZero(phyTileOffsetY)?1:0);

   // Renormalise to Calculate Tile-Sized Clip Step.
   tileClipStep[0].normalise(mTileSizeX);
   tileClipStep[2].normalise(mTileSizeY);
   tileClipStep[1] = tileClipStep[0] + tileClipStep[2];

   // Calculate Center Offset.
   const t2dVector tileCenter = tileClipStep[1] * 0.5f;

   // Calculate Tile Unit Steps.
   // NOTE:-   We're using logical tile offset here for the renormalisation.
   t2dVector tileOffset[3];
   tileOffset[0] = tileClipStep[0] * logTileOffsetX;
   tileOffset[2] = tileClipStep[2] * logTileOffsetY;
   tileOffset[1] = tileOffset[0] + tileOffset[2];

   // Fetch Min/Max View Intersection Points.
   t2dVector areaMin;
   t2dVector areaMax;
   areaMin = clippedWorldArea.point;
   areaMax = (clippedWorldArea.point + clippedWorldArea.extent);

   // Origin.
   static t2dVector vecOrigin(0,0);

   // Get Source Inverse Matrix.
   const t2dMatrix& srcInverseRotationMatrix = getInverseRotationMatrix();


   // ******************************************************************
   // Calculate area as a Local-Boundary!
   //
   // NOTE:-   We only use this as the final check to see if the view is
   //          totally ENCLOSED by the tile which is the most expensive
   //          test here.
   // ******************************************************************
   t2dVector areaLocal[4];

   // Calculate Non-Transformed Position.
   areaLocal[0] = areaMin;
   areaLocal[1] = t2dVector(areaMax.mX, areaMin.mY);
   areaLocal[2] = areaMax;
   areaLocal[3] = t2dVector(areaMin.mX, areaMax.mY);

   // We don't need to inverse rotate if rotation is at zero!
   if ( mNotZero(getRotation()) )
   {
      // Transform Points into Local-Boundary.
      for ( U32 n = 0; n < 4; n++ )
         inverseTransformPoint2D( srcInverseRotationMatrix, areaLocal[n], vecOrigin, areaLocal[n] );
   }


   // ******************************************************************
   //
   //  To save some time, we're going to be doing some extrapolated
   //  tile-movement in world-space.  World tile-steps are calculated
   //  and used to step through the tiles prior to rendering.
   //
   // ******************************************************************

   // Initialise Base/Current Tile World Clip Boundary.
   t2dVector baseTileWorldClipBoundary[4];
   t2dVector CurrentTileWorldClipBoundary[4];

   // Calculate Base/Current Tile Clip.
   baseTileWorldClipBoundary[0] = mWorldClipBoundary[0] - tileOffset[1];
   baseTileWorldClipBoundary[1] = mWorldClipBoundary[0] - tileOffset[1] + tileClipStep[0];
   baseTileWorldClipBoundary[2] = mWorldClipBoundary[0] - tileOffset[1] + tileClipStep[1];
   baseTileWorldClipBoundary[3] = mWorldClipBoundary[0] - tileOffset[1] + tileClipStep[2];

   // Reset Tile Positions Y.
   S32 logTilePositionY = logTileY;
   U32 phyTilePositionY = phyTileY;

   // Tile Object.
   tTileObject* pTileObject;

   // Render all the Y-tiles.
   for ( U32 y = 0; y < tilesViewedY; y++ )
   {
      // Are we rendering a tile here?
      if (    mWrapY ||   // Wrap Mode-Y On.
         ( !mWrapY && (logTilePositionY >= 0 || logTilePositionY < mTileCountY ) ) ) // Wrap Mode-Y Off but within tile-boundary.
      {
         // Transfer base clip into current.
         CurrentTileWorldClipBoundary[0] = baseTileWorldClipBoundary[0];
         CurrentTileWorldClipBoundary[1] = baseTileWorldClipBoundary[1];
         CurrentTileWorldClipBoundary[2] = baseTileWorldClipBoundary[2];
         CurrentTileWorldClipBoundary[3] = baseTileWorldClipBoundary[3];

         // Reset Tile Positions X.
         S32 logTilePositionX = logTileX;
         U32 phyTilePositionX = phyTileX;

         // Render all the X-tiles.
         for ( U32 x = 0; x < tilesViewedX; x++ )
         {
            // Are we rendering a tile here?
            if (    mWrapX ||   // Wrap Mode-X On.
               ( !mWrapX && (logTilePositionX >= 0 || logTilePositionX < mTileCountX ) ) )// Wrap Mode-X Off but within tile-boundary.
            {
               // Yes, so fetch Tile Object.
               pTileObject = mppTileObjectArray[phyTilePositionX + ( phyTilePositionY * mTileCountX )];

               // Is there a tile object here and available for receive collision?
               if ( pTileObject && pTileObject->mReceiveCollisions )
               {
                  // ****************************************************************
                  // Yes, so check if tile is in the area...
                  // ****************************************************************

                  // Reset Collidable Flag.
                  bool collidable = false;

                  // ****************************************************************
                  // Check to see if tile is WITHIN the area...
                  // ****************************************************************
                  for ( U32 n = 0; n < 4; n++ )
                  {
                     // Check if tile is within the area.
                     if ( pointInRectangle( CurrentTileWorldClipBoundary[n], areaMin, areaMax ) )
                     {
                        // Collidable!
                        collidable = true;
                        break;
                     }
                  }

                  // Do we need to check further?
                  if ( !collidable )
                  {
                     // ****************************************************************
                     // Yes, so check to see if tile INTERSECTS the area...
                     // ****************************************************************
                     if (    lineRectangleIntersect( CurrentTileWorldClipBoundary[0], CurrentTileWorldClipBoundary[1], areaMin, areaMax ) ||
                        lineRectangleIntersect( CurrentTileWorldClipBoundary[1], CurrentTileWorldClipBoundary[2], areaMin, areaMax ) ||
                        lineRectangleIntersect( CurrentTileWorldClipBoundary[2], CurrentTileWorldClipBoundary[3], areaMin, areaMax ) ||
                        lineRectangleIntersect( CurrentTileWorldClipBoundary[3], CurrentTileWorldClipBoundary[0], areaMin, areaMax ) )
                     {
                        // Collidable!
                        collidable = true;
                     }
                  }

                  // Do we need to check further?
                  if ( !collidable )
                  {
                     // ****************************************************************
                     // Yes, so check to see if tile ENCLOSES the area...
                     // ****************************************************************

                     // Transform Current Tile into Local-Boundary.
                     static t2dVector invCurrentTileWorldClipBoundaryMin;
                     static t2dVector invCurrentTileWorldClipBoundaryMax;

                     // Zero Angle?
                     if ( mIsZero( getRotation() ) )
                     {
                        // Yes, so simply use tile-boundary.
                        invCurrentTileWorldClipBoundaryMin = CurrentTileWorldClipBoundary[0];
                        invCurrentTileWorldClipBoundaryMax = CurrentTileWorldClipBoundary[2];
                     }
                     else
                     {
                        // No, so transform tile-boundary.
                        inverseTransformPoint2D( srcInverseRotationMatrix, CurrentTileWorldClipBoundary[0], vecOrigin, invCurrentTileWorldClipBoundaryMin );
                        inverseTransformPoint2D( srcInverseRotationMatrix, CurrentTileWorldClipBoundary[2], vecOrigin, invCurrentTileWorldClipBoundaryMax );
                     }

                     for ( U32 n = 0; n < 4; n++ )
                     {
                        // Check if tile is within the area.
                        if ( pointInRectangle( areaLocal[n], invCurrentTileWorldClipBoundaryMin, invCurrentTileWorldClipBoundaryMax ) )
                        {
                           // Collidable!
                           collidable = true;
                           break;
                        }
                     }
                  }

                  // Is the tile Collidable?
                  if ( collidable )
                  {
                     // Yes, so has this tile been updated this sequence?
                     if ( pTileObject->mUpdateSequenceKey != sceneUpdateSequenceKey )
                     {
                        // No, so set sequence.
                        pTileObject->mUpdateSequenceKey = sceneUpdateSequenceKey;

                        // Update Tiles Physics.
                        pTileObject->mpPhysics->setPosition( CurrentTileWorldClipBoundary[0] + tileCenter );                            
                        pTileObject->mpPhysics->setRotation( getRotation() );
                     }

                     // Add Physics to Collision List.
                     mCollisionPhysicsList.push_back( pTileObject->mpPhysics );
                  }
               }
            }

            // Move Current Clip Boundary in X.
            CurrentTileWorldClipBoundary[0] += tileClipStep[0];
            CurrentTileWorldClipBoundary[1] += tileClipStep[0];
            CurrentTileWorldClipBoundary[2] += tileClipStep[0];
            CurrentTileWorldClipBoundary[3] += tileClipStep[0];

            // Move to next Tile-X Position.
            logTilePositionX++;
            phyTilePositionX++;

            // Wrap Physical Tile Position.
            if ( phyTilePositionX >= mTileCountX )
               phyTilePositionX = 0;
         }

         // Move Base Clip Boundary in Y.
         baseTileWorldClipBoundary[0] += tileClipStep[2];
         baseTileWorldClipBoundary[1] += tileClipStep[2];
         baseTileWorldClipBoundary[2] += tileClipStep[2];
         baseTileWorldClipBoundary[3] += tileClipStep[2];

         // Move to next Tile-Y Position.
         logTilePositionY++;
         phyTilePositionY++;

         // Wrap Physical Tile Position.
         if ( phyTilePositionY >= mTileCountY )
            phyTilePositionY = 0;
      }
   }

   // Return Collision List.
   return mCollisionPhysicsList;
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dTileLayer )
REGISTER_SERIALISE_VERSION( t2dTileLayer, 2, true )
REGISTER_SERIALISE_VERSION( t2dTileLayer, 3, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dTileLayer, 3 )


//-----------------------------------------------------------------------------
// Load v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dTileLayer, 2 )
{
   U32                 tileCountX;
   U32                 tileCountY;
   F32                 tileSizeX;
   F32                 tileSizeY;
   bool                wrapModeX;
   bool                wrapModeY;
   F32                 panPositionX;
   F32                 panPositionY;
   F32                 autoPanX;
   F32                 autoPanY;

   // Object Info.
   if  (   !stream.read( &tileCountX ) ||
      !stream.read( &tileCountY ) ||
      !stream.read( &tileSizeX ) ||
      !stream.read( &tileSizeY ) ||
      !stream.read( &wrapModeX ) ||
      !stream.read( &wrapModeY ) ||
      !stream.read( &panPositionX ) ||
      !stream.read( &panPositionY ) ||
      !stream.read( &autoPanX ) ||
      !stream.read( &autoPanY ) )
      return false;

   // Create Layer.
	//printf( " === IN IMPLEMENT_T2D_LOAD_METHOD( t2dTileLayer, 2 ), calling createLayer() \n" ); // RKS: Checking for the createLayer() memory leak
	object->deleteLayer(); // RKS NOTE:  This is to ensure that the memory is deallocated before creating a new layer.
   object->createLayer( tileCountX, tileCountY, tileSizeX, tileSizeY );

   // Set Wrap Options.
   object->setWrap( wrapModeX, wrapModeY );

   // Set Pan Position,
   object->setPanPosition( panPositionX, panPositionY );

   // Set Auto Pan.
   object->setAutoPan( autoPanX, autoPanY );


   bool tileAvailable;
   t2dTileMap::eTileType tileType;
   // UNUSED: JOSEPH THOMAS -> U32 tileOptions;

   // Tile Object.
   tTileObject* pTileObject;

   // Tile-Details.
   for ( U32 tileX = 0; tileX < object->mTileCountX; tileX++ )
   {
      for ( U32 tileY = 0; tileY < object->mTileCountY; tileY++ )
      {
         // Read Tile Available Flag.
         stream.read( &tileAvailable );

         // Do we have a tile here?
         if ( tileAvailable )
         {
            // Read Tile Type.
            if  (!stream.read( (S32*)&tileType ) )
               return false;

            // Serialise each tile type appropriately.
            switch( tileType )
            {
               // Static Tile.
            case t2dTileMap::STATIC_TILE:
               {
                  // Read ImageMap Name.
                  StringTableEntry imageMapName = stream.readSTString();

                  // Read Frame.
                  U32 frame;
                  if ( !stream.read( &frame ) )
                     return false;

                  // Set Static Tile.
                  if ( !object->setStaticTile( tileX, tileY, imageMapName, frame ) )
                     return false;

               } break;

               // Animation Tile.
            case t2dTileMap::ANIMATED_TILE:
               {
                  // Read Animation Name.
                  StringTableEntry animationName = stream.readSTString();

                  // Read 'Force Unique' Flag.
                  bool forceUnique;
                  if ( !stream.read( &forceUnique ) )
                     return false;

                  // Set Animated Tile.
                  if ( !object->setAnimatedTile( tileX, tileY, animationName, forceUnique ) )
                     return false;

               } break;

               // Active Tile.
            case t2dTileMap::ACTIVE_TILE:
               {
                  // Read Active-Tile Class/Datablock Names.
                  StringTableEntry activeTileClassName        = stream.readSTString();
                  StringTableEntry activeTileDataBlockName    = stream.readSTString();

                  // Set Active Tile.
                  if ( !object->setActiveTile( tileX, tileY, activeTileClassName, activeTileDataBlockName ) )
                     return false;

                  // Fetch Active Tile Node for this position.
                  // NOTE:-   We're dereferencing getTileNode which normally would be dangerous!
                  object->getTileObject( tileX, tileY, &pTileObject );
                  t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pTileObject->mpTileNode;

                  // Do Active-Tile Self-Serialisation.
                  if ( !pTileNode->pActiveTile2D->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
                     return false;

               } break;
            }

            // Fetch Root Tile Node.
            // NOTE:-   We're dereferencing getTileNode which normally would be dangerous!
            object->getTileObject( tileX, tileY, &pTileObject );
            // UNUSED: JOSEPH THOMAS -> t2dTileMap::cRootTileNode* pRootTileNode = pTileObject->mpTileNode;

            // Physics.
            if ( !pTileObject->mpPhysics->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
               // Error.
               return false;

            // Read Tile-Script Parameters.
            //pTileObject->mCustomData = stream.readSTString(); // Will be added in when new file-format is ready.
            pTileObject->mShowScript = stream.readSTString();
            if ( !stream.read( &pTileObject->mScriptActioned ) )
               return false;

            // Read Misc Tile Parameters.
            if (    !stream.read( &pTileObject->mReceiveCollisions ) ||
               !stream.read( &pTileObject->mFlipHorizontal ) ||
               !stream.read( &pTileObject->mFlipVertical ) )
               return false;
         }
      }
   }

   // Return Okay.
   return true;
}

//-----------------------------------------------------------------------------
// Save v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dTileLayer, 2 )
{
   // Tile Object.
   tTileObject* pTileObject;

   // Object Info.
   if  (   !stream.write( object->mTileCountX ) ||
      !stream.write( object->mTileCountY ) ||
      !stream.write( object->mTileSizeX ) ||
      !stream.write( object->mTileSizeY ) ||
      !stream.write( object->mWrapX ) ||
      !stream.write( object->mWrapY ) ||
      !stream.write( object->mPanPositionX ) ||
      !stream.write( object->mPanPositionY ) ||
      !stream.write( object->mAutoPanX ) ||
      !stream.write( object->mAutoPanY ) )
      return false;

   // Tile-Details.
   for ( U32 x = 0; x < object->mTileCountX; x++ )
   {
      for ( U32 y = 0; y < object->mTileCountY; y++ )
      {
         // Fetch Root Tile Node for this position.
         // NOTE:-   We're dereferencing getTileNode which normally would be dangerous!
         object->getTileObject( x, y, &pTileObject );

         // Write Tile Available Flag.
         stream.write( pTileObject != NULL );

         // Do we have a tile here?
         if ( pTileObject )
         {
            // Fetch Tile Type.
            t2dTileMap::cRootTileNode* pRootTileNode = pTileObject->mpTileNode;
            t2dTileMap::eTileType tileType = pRootTileNode->mTileType;

            // Write Tile Type.
            if  ( !stream.write( (S32)tileType ) )
               return false;

            // Serialise each tile type appropriately.
            switch( tileType )
            {
               // Static Tile.
            case t2dTileMap::STATIC_TILE:
               {
                  // Cast Static Tile.
                  t2dTileMap::cStaticTileNode* pTileNode = (t2dTileMap::cStaticTileNode*)pRootTileNode;

                  if( !pTileNode || pTileNode->pImageMapDataBlock2D.isNull() )
                     break;

                  // *****************************
                  // Write Tile Info.
                  // *****************************

                  // Write ImageMap Name.
                  stream.writeString( pTileNode->pImageMapDataBlock2D->getName() );

                  // Write Frame.
                  if ( !stream.write( pTileNode->mFrame ) )
                     return false;

               } break;

               // Animation Tile.
            case t2dTileMap::ANIMATED_TILE:
               {
                  // Cast Animation Tile.
                  t2dTileMap::cAnimationTileNode* pTileNode = (t2dTileMap::cAnimationTileNode*)pRootTileNode;

                  // *****************************
                  // Write Tile Info.
                  // *****************************

                  // Write Animation Name.
                  if ( pTileNode->pAnimationController2D->getCurrentDataBlock() )
                  {
                     stream.writeString( pTileNode->pAnimationController2D->getCurrentDataBlock()->getName() );
                  }
                  else
                  {
                     stream.writeString( StringTable->insert("") );
                  }

                  // Write 'Force Unique' Flag.
                  //
                  // NOTE:-   The idea here is that if the reference count for the root tile node is greater
                  //          than 1, then we're not sharing the tile and the assumption is to not use
                  //          the 'forceUnique' flag.  We'll invert this logic so it represents the flag itself.
                  if ( !stream.write( (pTileNode->mReferenceCount <= 1) ) )
                     return false;

               } break;

               // Active Tile.
            case t2dTileMap::ACTIVE_TILE:
               {
                  // Cast Active Tile.
                  t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pRootTileNode;

                  // Write the Active-Tile Class/Datablock Names.
                  stream.writeString( pTileNode->pActiveTile2D->getClassName() );
                  stream.writeString( pTileNode->pActiveTile2D->mConfigDataBlock->getName() );

                  // Do Active-Tile Self-Serialisation.
                  if ( !pTileNode->pActiveTile2D->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
                     return false;

               } break;
            }

            // Physics.
            if ( !pTileObject->mpPhysics->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
               // Error.
               return false;

            // Write Tile-Script Parameters.
            //stream.writeString( pTileObject->mCustomData );       // Will be added in when new file-format is ready.
            stream.writeString( pTileObject->mShowScript );
            if ( !stream.write( pTileObject->mScriptActioned ) )
               return false;



            // Write Misc Tile Parameters.
            if (    !stream.write( pTileObject->mReceiveCollisions ) ||
               !stream.write( pTileObject->mFlipHorizontal ) ||
               !stream.write( pTileObject->mFlipVertical ) )
               return false;
         }
      }
   }

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Load v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dTileLayer, 3 )
{
   U32                 tileCountX;
   U32                 tileCountY;
   F32                 tileSizeX;
   F32                 tileSizeY;
   bool                wrapModeX;
   bool                wrapModeY;
   F32                 panPositionX;
   F32                 panPositionY;
   F32                 autoPanX;
   F32                 autoPanY;

   // Object Info.
   if  (   !stream.read( &tileCountX ) ||
      !stream.read( &tileCountY ) ||
      !stream.read( &tileSizeX ) ||
      !stream.read( &tileSizeY ) ||
      !stream.read( &wrapModeX ) ||
      !stream.read( &wrapModeY ) ||
      !stream.read( &panPositionX ) ||
      !stream.read( &panPositionY ) ||
      !stream.read( &autoPanX ) ||
      !stream.read( &autoPanY ) )
      return false;

   // Create Layer.
	//printf( " === IN IMPLEMENT_T2D_LOAD_METHOD( t2dTileLayer, 3 ), calling createLayer() \n" ); // RKS: Checking for the createLayer() memory leak
   object->createLayer( tileCountX, tileCountY, tileSizeX, tileSizeY );

   // Set Wrap Options.
   object->setWrap( wrapModeX, wrapModeY );

   // Set Pan Position,
   object->setPanPosition( panPositionX, panPositionY );

   // Set Auto Pan.
   object->setAutoPan( autoPanX, autoPanY );


   bool tileAvailable;
   t2dTileMap::eTileType tileType;
   // UNUSED: JOSEPH THOMAS -> U32 tileOptions;

   // Tile Object.
   tTileObject* pTileObject;

   // Tile-Details.
   for ( U32 tileX = 0; tileX < object->mTileCountX; tileX++ )
   {
      for ( U32 tileY = 0; tileY < object->mTileCountY; tileY++ )
      {
         // Read Tile Available Flag.
         stream.read( &tileAvailable );

         // Do we have a tile here?
         if ( tileAvailable )
         {
            // Read Tile Type.
            if  (!stream.read( (S32*)&tileType ) )
               return false;

            // Serialise each tile type appropriately.
            switch( tileType )
            {
               // Static Tile.
            case t2dTileMap::STATIC_TILE:
               {
                  // Read ImageMap Name.
                  StringTableEntry imageMapName = stream.readSTString();

                  // Read Frame.
                  U32 frame;
                  if ( !stream.read( &frame ) )
                     return false;

                  // Set Static Tile.
                  if ( !object->setStaticTile( tileX, tileY, imageMapName, frame ) )
                     return false;

               } break;

               // Animation Tile.
            case t2dTileMap::ANIMATED_TILE:
               {
                  // Read Animation Name.
                  StringTableEntry animationName = stream.readSTString();

                  // Read 'Force Unique' Flag.
                  bool forceUnique;
                  if ( !stream.read( &forceUnique ) )
                     return false;

                  // Set Animated Tile.
                  if ( !object->setAnimatedTile( tileX, tileY, animationName, forceUnique ) )
                     return false;

               } break;

               // Active Tile.
            case t2dTileMap::ACTIVE_TILE:
               {
                  // Read Active-Tile Class/Datablock Names.
                  StringTableEntry activeTileClassName        = stream.readSTString();
                  StringTableEntry activeTileDataBlockName    = stream.readSTString();

                  // Set Active Tile.
                  if ( !object->setActiveTile( tileX, tileY, activeTileClassName, activeTileDataBlockName ) )
                     return false;

                  // Fetch Active Tile Node for this position.
                  // NOTE:-   We're dereferencing getTileNode which normally would be dangerous!
                  object->getTileObject( tileX, tileY, &pTileObject );
                  t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pTileObject->mpTileNode;

                  // Do Active-Tile Self-Serialisation.
                  if ( !pTileNode->pActiveTile2D->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
                     return false;

               } break;
            }

            // Fetch Root Tile Node.
            // NOTE:-   We're dereferencing getTileNode which normally would be dangerous!
            object->getTileObject( tileX, tileY, &pTileObject );
            // UNUSED: JOSEPH THOMAS -> t2dTileMap::cRootTileNode* pRootTileNode = pTileObject->mpTileNode;

            // Physics.
            if ( !pTileObject->mpPhysics->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
               // Error.
               return false;

            // Read Tile-Script Parameters.
            pTileObject->mCustomData = stream.readSTString();
            pTileObject->mShowScript = stream.readSTString();
            if ( !stream.read( &pTileObject->mScriptActioned ) )
               return false;

            // Read Misc Tile Parameters.
            if (    !stream.read( &pTileObject->mReceiveCollisions ) ||
               !stream.read( &pTileObject->mFlipHorizontal ) ||
               !stream.read( &pTileObject->mFlipVertical ) )
               return false;
         }
      }
   }

   // Return Okay.
   return true;
}

//-----------------------------------------------------------------------------
// Save v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dTileLayer, 3 )
{
   // Tile Object.
   tTileObject* pTileObject;

   // Object Info.
   if  (   !stream.write( object->mTileCountX ) ||
      !stream.write( object->mTileCountY ) ||
      !stream.write( object->mTileSizeX ) ||
      !stream.write( object->mTileSizeY ) ||
      !stream.write( object->mWrapX ) ||
      !stream.write( object->mWrapY ) ||
      !stream.write( object->mPanPositionX ) ||
      !stream.write( object->mPanPositionY ) ||
      !stream.write( object->mAutoPanX ) ||
      !stream.write( object->mAutoPanY ) )
      return false;

   // Tile-Details.
   for ( U32 x = 0; x < object->mTileCountX; x++ )
   {
      for ( U32 y = 0; y < object->mTileCountY; y++ )
      {
         // Fetch Root Tile Node for this position.
         // NOTE:-   We're dereferencing getTileNode which normally would be dangerous!
         object->getTileObject( x, y, &pTileObject );

         // Write Tile Available Flag.
         stream.write( pTileObject != NULL );

         // Do we have a tile here?
         if ( pTileObject )
         {
            // Fetch Tile Type.
            t2dTileMap::cRootTileNode* pRootTileNode = pTileObject->mpTileNode;
            t2dTileMap::eTileType tileType = pRootTileNode->mTileType;

            // Write Tile Type.
            if  ( !stream.write( (S32)tileType ) )
               return false;

            // Serialise each tile type appropriately.
            switch( tileType )
            {
               // Static Tile.
            case t2dTileMap::STATIC_TILE:
               {
                  // Cast Static Tile.
                  t2dTileMap::cStaticTileNode* pTileNode = (t2dTileMap::cStaticTileNode*)pRootTileNode;

                  if( !pTileNode || pTileNode->pImageMapDataBlock2D.isNull() )
                     break;

                  // *****************************
                  // Write Tile Info.
                  // *****************************

                  // Write ImageMap Name.
                  stream.writeString( pTileNode->pImageMapDataBlock2D->getName() );

                  // Write Frame.
                  if ( !stream.write( pTileNode->mFrame ) )
                     return false;

               } break;

               // Animation Tile.
            case t2dTileMap::ANIMATED_TILE:
               {
                  // Cast Animation Tile.
                  t2dTileMap::cAnimationTileNode* pTileNode = (t2dTileMap::cAnimationTileNode*)pRootTileNode;

                  // *****************************
                  // Write Tile Info.
                  // *****************************

                  // Write Animation Name.
                  if ( pTileNode->pAnimationController2D->getCurrentDataBlock() )
                  {
                     stream.writeString( pTileNode->pAnimationController2D->getCurrentDataBlock()->getName() );
                  }
                  else
                  {
                     stream.writeString( StringTable->insert("") );
                  }

                  // Write 'Force Unique' Flag.
                  //
                  // NOTE:-   The idea here is that if the reference count for the root tile node is greater
                  //          than 1, then we're not sharing the tile and the assumption is to not use
                  //          the 'forceUnique' flag.  We'll invert this logic so it represents the flag itself.
                  if ( !stream.write( (pTileNode->mReferenceCount <= 1) ) )
                     return false;

               } break;

               // Active Tile.
            case t2dTileMap::ACTIVE_TILE:
               {
                  // Cast Active Tile.
                  t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pRootTileNode;

                  // Write the Active-Tile Class/Datablock Names.
                  stream.writeString( pTileNode->pActiveTile2D->getClassName() );
                  stream.writeString( pTileNode->pActiveTile2D->mConfigDataBlock->getName() );

                  // Do Active-Tile Self-Serialisation.
                  if ( !pTileNode->pActiveTile2D->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
                     return false;

               } break;
            }

            // Physics.
            if ( !pTileObject->mpPhysics->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
               // Error.
               return false;

            // Write Tile-Script Parameters.
            stream.writeString( pTileObject->mCustomData );
            stream.writeString( pTileObject->mShowScript );
            if ( !stream.write( pTileObject->mScriptActioned ) )
               return false;



            // Write Misc Tile Parameters.
            if (    !stream.write( pTileObject->mReceiveCollisions ) ||
               !stream.write( pTileObject->mFlipHorizontal ) ||
               !stream.write( pTileObject->mFlipVertical ) )
               return false;
         }
      }
   }

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dTileLayer::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
   // Move Pan Position X.
   if ( mNotZero(mAutoPanX) )
      mPanPositionX += (mAutoPanX * elapsedTime);

   // Move Pan Position Y.
   if ( mNotZero(mAutoPanY) )
      mPanPositionY += mAutoPanY * elapsedTime;

   // Is the Cursor Icon Active?
   if ( mCursorIconActive )
   {
      // Yes, so adjust blink-time.
      mCurrentCursorBlinkTime -= elapsedTime;

      // Blink Time Change?
      if ( mCurrentCursorBlinkTime <= 0.0f )
      {
         // Yes, so flip state.
         mCurrentCursorBlinkState = !mCurrentCursorBlinkState;

         // Reset Blink Time.
         mCurrentCursorBlinkTime = mCursorBlinkTime;
      }
   }

   // Update Tick Scroll Position.
   updateTickScrollPosition();

   // Call Parent.
   Parent::integrateObject( sceneTime, elapsedTime, pDebugStats );
}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dTileLayer::interpolateTick( const F32 timeDelta )
{
    // Calculate Render Tick Position.
    mRenderTickOffset = (mPreTickeOffset * timeDelta) + ((1.0f-timeDelta) * mPostTickOffset);

    // Base Tick Interpolation.
    Parent::interpolateTick( timeDelta );
}


//-----------------------------------------------------------------------------
// Reset Tick Scroll Positions.
//-----------------------------------------------------------------------------
void t2dTileLayer::resetTickScrollPositions( void )
{
    // Reset Scroll Positions.
    mRenderTickOffset.set( mPanPositionX, mPanPositionY );
    mPreTickeOffset = mPostTickOffset = mRenderTickOffset;
}


//-----------------------------------------------------------------------------
// Update Tick Scroll Position.
//-----------------------------------------------------------------------------
void t2dTileLayer::updateTickScrollPosition( void )
{
    // Store Pre Tick Scroll Position.
    mPreTickeOffset = mPostTickOffset;

    // Store Current Tick Scroll Position.
    mPostTickOffset.set( mPanPositionX, mPanPositionY );

    // Render Tick Position is at Pre-Tick Scroll Position.
    mRenderTickOffset = mPreTickeOffset;
};


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dTileLayer::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
   // Ignore if not part of a scene.
   if ( !getSceneGraph() )
      return;

   // Choose Appropriate Render Method.
   if ( mIsZero( getRenderRotation() ) )
      // No Rotation.
      renderZeroAngleObject( viewPort, viewIntersection );
   else
      // Arbitrary Angle.
      renderArbitraryAngleObject( viewPort, viewIntersection );
}


//-----------------------------------------------------------------------------
// Render Object at Zero Angle ( No Rotation ).
//-----------------------------------------------------------------------------
void t2dTileLayer::renderZeroAngleObject( const RectF& viewPort, const RectF& viewIntersection )
{
   // We cannot render if we've not got a tile array!
   // Also : we can't render without a valid scenegraph
   if ( !mppTileObjectArray || !getSceneGraph() ) {
      return;
   }
		
   // Note old Viewport.
   RectI oldViewport;
   dglGetViewport(&oldViewport);

   // Fetch Scene Window.
   t2dSceneWindow* pSceneWindow = getSceneGraph()->getCurrentRenderWindow();

   // If we don't have a window (like when being rendered by a guit2dObjectCtrl), then just render
   // to fill the current viewport.
   if (!pSceneWindow)
   {
      t2dVector tilePosition = mWorldClipBoundary[0];
      t2dVector tileSize = mWorldClipBoundary[2] - mWorldClipBoundary[0];
      tileSize.mX /= (F32)getTileCountX();
      tileSize.mY /= (F32)getTileCountY();

      // Bind Texture.
      glEnable        ( GL_TEXTURE_2D );
      glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

      // Set Blend Options.
      setBlendOptions();

      for (S32 y = 0, index = 0; y < getTileCountY(); y++, tilePosition.mY += tileSize.mY)
      {
         tilePosition.mX = mWorldClipBoundary[0].mX;
         for (S32 x = 0; x < getTileCountX(); x++, index++, tilePosition.mX += tileSize.mX)
         {
            tTileObject* tileObject = mppTileObjectArray[index];
            t2dVector CurrentTileWorldClipBoundary[4] = { tilePosition, tilePosition + t2dVector(tileSize.mX, 0.0f),
               tilePosition + tileSize, tilePosition + t2dVector(0.0f, tileSize.mY) };
            if (tileObject)
            {
               t2dTileMap::cRootTileNode* rootTile = tileObject->mpTileNode;
               switch (rootTile->mTileType)
               {
               case t2dTileMap::STATIC_TILE:
                  {
                     t2dTileMap::cStaticTileNode* pTileNode = (t2dTileMap::cStaticTileNode*)rootTile;

                     if( !pTileNode || pTileNode->pImageMapDataBlock2D.isNull() )
                        break;

                     // Fetch frame area and bind appropriate texture page.
                     const t2dImageMapDatablock::cFrameTexelArea& frameArea = pTileNode->pImageMapDataBlock2D->getImageMapFrameAreaBind( pTileNode->mFrame );

                     // Fetch Positions.
                     F32 minX = frameArea.mX;
                     F32 minY = frameArea.mY;
                     F32 maxX = frameArea.mX2;
                     F32 maxY = frameArea.mY2;

                     // Handle Horizontal Flip.
                     if ( tileObject->mFlipHorizontal )
                     {
                        F32 tempX = minX;
                        minX = maxX;
                        maxX = tempX;
                     }

                     // Handle Vertical Flip.
                     if ( tileObject->mFlipVertical )
                     {
                        F32 tempY = minY;
                        minY = maxY;
                        maxY = tempY;
                     }
#ifdef TORQUE_OS_IPHONE
					  //2 for 2-point coordinates
					  // Uses memory rather than heap memory. PUAP optimization.
					  dglDrawTextureQuadiPhone(
											   CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
											   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
											   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
											   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
											   
											   minX, minY,
											   maxX, minY,
											   minX, maxY,
											   maxX, maxY
					  );
#else

                     // Draw Static Tile.
                     glBegin(GL_QUADS);
                     glTexCoord2f( minX, minY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                     glTexCoord2f( maxX, minY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                     glTexCoord2f( maxX, maxY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                     glTexCoord2f( minX, maxY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                     glEnd();
#endif
                     break;
                  }

               case t2dTileMap::ANIMATED_TILE:
                  {
                     t2dTileMap::cAnimationTileNode* tile = (t2dTileMap::cAnimationTileNode*)rootTile;

                     // Bind with Animated Texture.
                     glBindTexture( GL_TEXTURE_2D, tile->pAnimationController2D->getCurrentFrameTexture().getGLName() );

                     // Fetch Current Frame Region.
                     const t2dImageMapDatablock::cFrameTexelArea& frameArea = tile->pAnimationController2D->getCurrentFrameArea();

                     // Fetch Positions.
                     F32 minX = frameArea.mX;
                     F32 minY = frameArea.mY;
                     F32 maxX = frameArea.mX2;
                     F32 maxY = frameArea.mY2;

                     // Handle Horizontal Flip.
                     if ( tileObject->mFlipHorizontal )
                     {
                        F32 tempX = minX;
                        minX = maxX;
                        maxX = tempX;
                     }

                     // Handle Vertical Flip.
                     if ( tileObject->mFlipVertical )
                     {
                        F32 tempY = minY;
                        minY = maxY;
                        maxY = tempY;
                     }
#ifdef TORQUE_OS_IPHONE
					  //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
					  dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
											   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
											   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
											   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
											   minX, minY,
											   maxX, minY,
											   minX, maxY,
											   maxX, maxY);
#else

                     // Draw Static Tile.
                     glBegin(GL_QUADS);
                     glTexCoord2f( minX, minY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                     glTexCoord2f( maxX, minY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                     glTexCoord2f( maxX, maxY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                     glTexCoord2f( minX, maxY );
                     glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                     glEnd();
#endif
                     break;
                  }

               case t2dTileMap::ACTIVE_TILE:
                  {
                     t2dTileMap::cActiveTileNode* tile = (t2dTileMap::cActiveTileNode*)rootTile;

                     // Render the Active Tile.
                     tile->pActiveTile2D->renderObject( viewPort, viewIntersection, CurrentTileWorldClipBoundary );
                     break;
                  }			   
               }
            }
         }
      }
      return;
   }

   // Fetch Current Camera.
   t2dSceneWindow::tCameraView currentCamera = pSceneWindow->getCurrentCamera();

   // Calculate Window Boundary Coordinates.
   t2dVector windowBoundaryTopLeft;
   t2dVector windowBoundaryBottomRight;
	
	// RKS:  THIS SHIT IS HERE TO DEBUG WHY THE WORLD CLIP BOUNDARY IS MESSED UP
	//if( dStricmp( "PathGrid", getName() ) == 0 )
	//{
	//	dPrintf( "World Clip Boundary: (%f, %f)\n", mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY );
	//}
	
   pSceneWindow->sceneToWindowCoord( mWorldClipBoundary[0], windowBoundaryTopLeft );
   pSceneWindow->sceneToWindowCoord( mWorldClipBoundary[2], windowBoundaryBottomRight );

   // Calculate Window Clip Coordinates.
   t2dVector windowClipTopLeft;
   t2dVector windowClipBottomRight;
   t2dVector viewIntersectionTopLeft( viewIntersection.point.x, viewIntersection.point.y );
   t2dVector viewIntersectionBottomRight( viewIntersection.point.x+viewIntersection.len_x(), viewIntersection.point.y+viewIntersection.len_y() );
   pSceneWindow->sceneToWindowCoord( viewIntersectionTopLeft, windowClipTopLeft );
   pSceneWindow->sceneToWindowCoord( viewIntersectionBottomRight, windowClipBottomRight );

   // Calculate Boundary/Clip Intersection.
   RectF viewportBoundary(windowBoundaryTopLeft.mX, windowBoundaryTopLeft.mY, windowBoundaryBottomRight.mX-windowBoundaryTopLeft.mX, windowBoundaryBottomRight.mY-windowBoundaryTopLeft.mY );
   RectF viewportClip(windowClipTopLeft.mX, windowClipTopLeft.mY, windowClipBottomRight.mX-windowClipTopLeft.mX, windowClipBottomRight.mY-windowClipTopLeft.mY );
   viewportBoundary.intersect( viewportClip );

   // Calculate new Window Clip.
   RectI windowRectClip( S32(viewportBoundary.point.x), S32(viewportBoundary.point.y), S32(viewportBoundary.len_x()), S32(viewportBoundary.len_y()) );

   // Calculate Old Window.
   Point2I oldWindowTopLeft = pSceneWindow->globalToLocalCoord( oldViewport.point );
   Point2I oldWindowBottomRight = pSceneWindow->globalToLocalCoord( oldViewport.point+oldViewport.extent );

   // Clip new Window by Old Viewport.
   windowRectClip.intersect( RectI( oldWindowTopLeft.x, oldWindowTopLeft.y, oldWindowBottomRight.x - oldWindowTopLeft.x, oldWindowBottomRight.y - oldWindowTopLeft.y ) );

   // Calculate Window Points.
   Point2I windowPointTopLeft = windowRectClip.point;
   Point2I windowPointBottomRight = windowPointTopLeft + windowRectClip.extent;

   // Calculate Canvas Points.
   Point2I canvasTopLeft = pSceneWindow->localToGlobalCoord( windowPointTopLeft );
   Point2I canvasBottomRight = pSceneWindow->localToGlobalCoord( windowPointBottomRight );

   // Calculate Scene Points / Clip Rectangle.
   t2dVector sceneTopLeft;
   t2dVector sceneBottomRight;
   pSceneWindow->windowToSceneCoord( t2dVector(F32(windowPointTopLeft.x),F32(windowPointTopLeft.y)), sceneTopLeft );
   pSceneWindow->windowToSceneCoord( t2dVector(F32(windowPointBottomRight.x),F32(windowPointBottomRight.y)), sceneBottomRight );
   RectF objectClip( sceneTopLeft.mX, sceneTopLeft.mY, sceneBottomRight.mX-sceneTopLeft.mX, sceneBottomRight.mY-sceneTopLeft.mY ); 

   // Get Camera Shake offset.
   const t2dVector& cameraShake = pSceneWindow->getCameraShake();

   // Setup new Clipping Projection (including camera shake).
//IPHONE WARNING: tilelayer 2 deep pushes in the projection matrix is all that is allowed
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
#ifdef TORQUE_OS_IPHONE
   glOrthof( objectClip.point.x + cameraShake.mX, objectClip.point.x + objectClip.len_x() + cameraShake.mX, objectClip.point.y + objectClip.len_y() + cameraShake.mY, objectClip.point.y + cameraShake.mY, 0.0f, t2dSceneGraph::maxLayersSupported );
#else
   glOrtho( objectClip.point.x + cameraShake.mX, objectClip.point.x + objectClip.len_x() + cameraShake.mX, objectClip.point.y + objectClip.len_y() + cameraShake.mY, objectClip.point.y + cameraShake.mY, 0.0f, t2dSceneGraph::maxLayersSupported );
#endif
   // Setup new viewport.
   RectI newViewport( canvasTopLeft.x, canvasTopLeft.y, canvasBottomRight.x - canvasTopLeft.x, canvasBottomRight.y - canvasTopLeft.y );
   dglSetViewport(newViewport);

   // Set ModelView.
   glMatrixMode(GL_MODELVIEW);

   // Set Blend Options.
   setBlendOptions();

   // Enable Texturing.
   glEnable        ( GL_TEXTURE_2D );
   glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

   // Get merged Local/Scene Debug Mask.
   U32 debugMask = getDebugMask() | mpTileMap2DManager->getSceneGraph()->getDebugMask();

   // Clear Debug Render Vector.
   mDebugRenderPhysics.clear();

   // Fetch NULL String.
   StringTableEntry nullString = StringTable->insert(""); // RKS: THIS COULD BE CAUSING SOME PERFORMING PROBLEMS

   // Calculate Logical Fractional-Tile at Intersection.
   F32 logFracTileX = (viewIntersection.point.x - mWorldClipBoundary[0].mX + mRenderTickOffset.mX) / mTileSizeX;
   F32 logFracTileY = (viewIntersection.point.y - mWorldClipBoundary[0].mY + mRenderTickOffset.mY) / mTileSizeY;

   F32 logTileOffsetX;
   F32 logTileOffsetY;
   F32 phyTileOffsetX;
   F32 phyTileOffsetY;
   S32 logTileX;
   S32 logTileY;

   // Deal with Pan-X Direction Appropriately.
   if ( logFracTileX < 0.0f )
   {
      logTileOffsetX = 1.0f + (logFracTileX - S32(logFracTileX));
      phyTileOffsetX = mFabs(logTileOffsetX) * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(logFracTileX - logTileOffsetX + 0.5f) - 1;
   }
   else
   {
      logTileOffsetX = logFracTileX - mFloor(logFracTileX);
      phyTileOffsetX = logTileOffsetX * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(mFloor(logFracTileX - logTileOffsetX));
   }

   // Deal with Pan-Y Direction Appropriately.
   if ( logFracTileY < 0.0f )
   {
      logTileOffsetY = 1.0f + (logFracTileY - S32(logFracTileY));
      phyTileOffsetY = mFabs(logTileOffsetY) * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(logFracTileY - logTileOffsetY + 0.5f) - 1;
   }
   else
   {
      logTileOffsetY = logFracTileY - mFloor(logFracTileY);
      phyTileOffsetY = logTileOffsetY * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(mFloor(logFracTileY - logTileOffsetY));
   }

   // Calculate Physical Tile.
   S32 phyTileX = logTileX;
   S32 phyTileY = logTileY;
   // Wrap Tile-X.
   if ( phyTileX < 0 )
      phyTileX = (phyTileX+mTileCountX) % mTileCountX;
   else if ( phyTileX >= mTileCountX )
      phyTileX %= mTileCountX;
   // Wrap Tile-Y.
   if ( phyTileY < 0 )
      phyTileY = (phyTileY+mTileCountY) % mTileCountY;
   else if ( phyTileY >= mTileCountY )
      phyTileY %= mTileCountY;

   // Calculate Tile Offset.
   t2dVector tileOffset( phyTileOffsetX, phyTileOffsetY );

   // Calculate Tiles Viewed.
   const U32 tilesViewedX = U32( mCeil( (viewIntersection.len_x() + phyTileOffsetX) / mTileSizeX ) );
   const U32 tilesViewedY = U32( mCeil( (viewIntersection.len_y() + phyTileOffsetY) / mTileSizeY ) );

   // Calculate Renormalised Tile Clip Steps.
   t2dVector tileClipStep[3];
   tileClipStep[0].set(mTileSizeX, 0);
   tileClipStep[2].set(0, mTileSizeY);
   tileClipStep[1] = tileClipStep[0] + tileClipStep[2];

   // Calculate Renormalised Half-Tile Clip Steps.
   t2dVector tileHalfClipStep[3];
   tileHalfClipStep[0] = tileClipStep[0] * 0.5f;
   tileHalfClipStep[2] = tileClipStep[2] * 0.5f;
   tileHalfClipStep[1] = tileHalfClipStep[0] + tileHalfClipStep[2];

   // Reference Important Half-Tile Clip Step.
   const t2dVector& tileCenter = tileHalfClipStep[1];


   // ******************************************************************
   //
   //  To save some time, we're going to be doing some extrapolated
   //  tile-movement in world-space.  World tile-steps are calculated
   //  and used to step through the tiles prior to rendering.
   //
   // ******************************************************************

   // Initialise Base/Current Tile World Clip Boundary.
   t2dVector baseTileWorldClipBoundary[4];
   t2dVector CurrentTileWorldClipBoundary[4];

   // Calculate Base/Current Tile Clip.
   baseTileWorldClipBoundary[0] = viewIntersection.point - tileOffset;
   baseTileWorldClipBoundary[1] = viewIntersection.point - tileOffset + tileClipStep[0];
   baseTileWorldClipBoundary[2] = viewIntersection.point - tileOffset + tileClipStep[1];
   baseTileWorldClipBoundary[3] = viewIntersection.point - tileOffset + tileClipStep[2];

   // Reset Tile Positions Y.
   S32 logTilePositionY = logTileY;
   U32 phyTilePositionY = phyTileY;

   // Tile-Texture Region.
   F32 minX, minY, maxX, maxY;

   // Argument Buffer.
   char argBuffer[2][64];

   // Render all the Y-tiles.
   for ( U32 y = 0; y < tilesViewedY; y++ )
   {
      // Are we rendering a tile here?
      if (    mWrapY ||   // Wrap Mode-Y On.
         ( !mWrapY && (logTilePositionY >= 0 && logTilePositionY < mTileCountY ) ) ) // Wrap Mode-Y Off but within tile-boundary.
      {
         // Transfer base clip into current.
         CurrentTileWorldClipBoundary[0] = baseTileWorldClipBoundary[0];
         CurrentTileWorldClipBoundary[1] = baseTileWorldClipBoundary[1];
         CurrentTileWorldClipBoundary[2] = baseTileWorldClipBoundary[2];
         CurrentTileWorldClipBoundary[3] = baseTileWorldClipBoundary[3];

         // Reset Tile Positions X.
         S32 logTilePositionX = logTileX;
         U32 phyTilePositionX = phyTileX;

         // Render all the X-tiles.
         for ( U32 x = 0; x < tilesViewedX; x++ )
         {
            // Are we rendering a tile here?
            if (    mWrapX ||   // Wrap Mode-X On.
               ( !mWrapX && (logTilePositionX >= 0 && logTilePositionX < mTileCountX ) ) )// Wrap Mode-X Off but within tile-boundary.
            {
               // Yes, so fetch Tile Object.
               tTileObject* pTileObject = mppTileObjectArray[phyTilePositionX + ( phyTilePositionY * mTileCountX )];

               // Do we have a tile object here?
               if ( pTileObject && pTileObject->mpTileNode )
               {
                  // Yes, so fetch Root Tile Node for this position.
                  t2dTileMap::cRootTileNode* pRootTileNode = pTileObject->mpTileNode;

                  // Render each tile type appropriately.
                  switch( pRootTileNode->mTileType )
                  {
                     // Static Tile.
                  case t2dTileMap::STATIC_TILE:
                     {
                        // Cast Static Tile.
                        t2dTileMap::cStaticTileNode* pTileNode = dynamic_cast<t2dTileMap::cStaticTileNode*>(pRootTileNode);

                        if( !pTileNode || pTileNode->pImageMapDataBlock2D.isNull() )
                           break;

						 // Fetch frame area and bind appropriate texture page.
						 const t2dImageMapDatablock::cFrameTexelArea& frameArea = pTileNode->pImageMapDataBlock2D->getImageMapFrameAreaBind( pTileNode->mFrame );
						 
						 // Fetch Positions.
						 minX = frameArea.mX;
						 minY = frameArea.mY;
						 maxX = frameArea.mX2;
						 maxY = frameArea.mY2;
						 
						 // Handle Horizontal Flip.
						 if ( pTileObject->mFlipHorizontal )
						 {
							 F32 tempX = minX;
							 minX = maxX;
							 maxX = tempX;
						 }
						 
						 // Handle Vertical Flip.
						 if ( pTileObject->mFlipVertical )
						 {
							 F32 tempY = minY;
							 minY = maxY;
							 maxY = tempY;
						 }
#ifdef TORQUE_OS_IPHONE
					  // Uses memory rather than heap memory. PUAP optimization.
  					  dglDrawTextureQuadiPhone(
												  CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
												  CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
												  CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
												  CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
												  
												  minX, minY,
												  maxX, minY,
												  minX, maxY,
												  maxX, maxY
											  );
#else

                        // Draw Static Tile.
                        glBegin(GL_QUADS);
                        glTexCoord2f( minX, minY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                        glTexCoord2f( maxX, minY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                        glTexCoord2f( maxX, maxY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                        glTexCoord2f( minX, maxY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                        glEnd();
#endif
                     } break;

                     // Animation Tile.
                  case t2dTileMap::ANIMATED_TILE:
                     {
                        // Cast Animation Tile.
                        t2dTileMap::cAnimationTileNode* pTileNode = (t2dTileMap::cAnimationTileNode*)pRootTileNode;

                        if( !pTileNode || !pTileNode->pAnimationController2D )
                           break;

                        // Bind with Animated Texture.
                        glBindTexture( GL_TEXTURE_2D, pTileNode->pAnimationController2D->getCurrentFrameTexture().getGLName() );

                        // Fetch Current Frame Region.
                        const t2dImageMapDatablock::cFrameTexelArea& frameArea = pTileNode->pAnimationController2D->getCurrentFrameArea();
                        // Fetch Positions.
                        minX = frameArea.mX;
                        minY = frameArea.mY;
                        maxX = frameArea.mX2;
                        maxY = frameArea.mY2;

                        // Handle Horizontal Flip.
                        if ( pTileObject->mFlipHorizontal )
                        {
                           F32 tempX = minX;
                           minX = maxX;
                           maxX = tempX;
                        }

                        // Handle Vertical Flip.
                        if ( pTileObject->mFlipVertical )
                        {
                           F32 tempY = minY;
                           minY = maxY;
                           maxY = tempY;
                        }
#ifdef TORQUE_OS_IPHONE
					  //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
					  dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
											   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
											   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
											   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
											   minX, minY,
											   maxX, minY,
											   minX, maxY,
											   maxX, maxY);
#else

                        // Draw Animated Tile.
                        glBegin(GL_QUADS);
                        glTexCoord2f( minX, minY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                        glTexCoord2f( maxX, minY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                        glTexCoord2f( maxX, maxY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                        glTexCoord2f( minX, maxY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                        glEnd();
#endif
                     } break;

                     // Active Tile.
                  case t2dTileMap::ACTIVE_TILE:
                     {
                        // Cast Active Tile.
                        t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pRootTileNode;

                        if( !pTileNode || pTileNode->pActiveTile2D.isNull() )
                           break;

                        // Render the Active Tile.
                        pTileNode->pActiveTile2D->renderObject( viewPort, viewIntersection, CurrentTileWorldClipBoundary );

                     } break;
                  }

                  // Custom-Data available?
                  if ( pTileObject->mCustomData != nullString )
                  {
                     // Draw Custom-Icon (if active)
                     if ( mCustomIconActive && mTileIconImageMapDataBlock )
                     {
                        // Fetch frame area and bind appropriate texture page.
                        const t2dImageMapDatablock::cFrameTexelArea& frameArea = mTileIconImageMapDataBlock->getImageMapFrameAreaBind( 0 );
                        // Fetch Positions.
                        minX = frameArea.mX;
                        minY = frameArea.mY;
                        maxX = frameArea.mX2;
                        maxY = frameArea.mY2;

                        // Calculate Icon Region.
                        const t2dVector tl = CurrentTileWorldClipBoundary[0] + tileHalfClipStep[0];
                        const t2dVector br = CurrentTileWorldClipBoundary[1] + tileHalfClipStep[2];
                        const t2dVector bl = CurrentTileWorldClipBoundary[0] + tileCenter;

#ifdef TORQUE_OS_IPHONE
					  //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
					  dglDrawTextureQuadiPhone(tl.mX, tl.mY,
											   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
											   bl.mX, bl.mY,
											   br.mX, br.mY,
											   minX, minY,
											   maxX, minY,
											   minX, maxY,
											   maxX, maxY);
#else
                        // Draw Animated Tile.
                        glBegin(GL_QUADS);
                        glTexCoord2f( minX, minY );
                        glVertex2fv ( (GLfloat*)&tl );
                        glTexCoord2f( maxX, minY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                        glTexCoord2f( maxX, maxY );
                        glVertex2fv ( (GLfloat*)&br );
                        glTexCoord2f( minX, maxY );
                        glVertex2fv ( (GLfloat*)&bl );
                        glEnd();
#endif
                     }
                  }

                  // Script available?
                  if ( pTileObject->mShowScript != nullString )
                  {
                     // Yes, so script unactioned?
                     if ( !pTileObject->mScriptActioned )
                     {
                        // Yes, so flag Script Actioned.
                        // NOTE:-   We do this here otherwise if the player resets the layer in
                        //          the callback, we'd reflag this tile as actioned!
                        pTileObject->mScriptActioned = true;

                        // Format layer-id buffer.
                        dSprintf( argBuffer[0], 64, "%d", getId() );
                        // Format tile-position buffer.
                        dSprintf( argBuffer[1], 64, "%d %d", phyTilePositionX, phyTilePositionY );
                        // Do script callback.
                        Con::executef( mpTileMap2DManager, 4, "onTileScript", argBuffer[0], argBuffer[1], pTileObject->mShowScript );
                     }

                     // Draw Script-Icon (if active)
                     if ( mScriptIconActive && mTileIconImageMapDataBlock )
                     {
                        // Fetch frame area and bind appropriate texture page.
                        const t2dImageMapDatablock::cFrameTexelArea& frameArea = mTileIconImageMapDataBlock->getImageMapFrameAreaBind( 1 );
                        // Fetch Positions.
                        minX = frameArea.mX;
                        minY = frameArea.mY;
                        maxX = frameArea.mX2;
                        maxY = frameArea.mY2;

                        // Calculate Top-Left Sub-tile of tile.
                        const t2dVector tr = CurrentTileWorldClipBoundary[0] + tileHalfClipStep[0];
                        const t2dVector br = CurrentTileWorldClipBoundary[0] + tileCenter;
                        const t2dVector bl = CurrentTileWorldClipBoundary[0] + tileHalfClipStep[2];
#ifdef TORQUE_OS_IPHONE
						//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
						dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
											   tr.mX, tr.mY,
											   bl.mX, bl.mY,
											   br.mX, br.mY,
											   minX, minY,
											   maxX, minY,
											   minX, maxY,
											   maxX, maxY);
#else

                        // Draw Animated Tile.
                        glBegin(GL_QUADS);
                        glTexCoord2f( minX, minY );
                        glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                        glTexCoord2f( maxX, minY );
                        glVertex2fv ( (GLfloat*)&tr );
                        glTexCoord2f( maxX, maxY );
                        glVertex2fv ( (GLfloat*)&br );
                        glTexCoord2f( minX, maxY );
                        glVertex2fv ( (GLfloat*)&bl );
                        glEnd();
#endif
                     }
                  }

                  // Calculate Debug Info.
                  if ( (pTileObject->mReceiveCollisions && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS)) )
                  {
                     // Update Tiles Physics.
                     // NOTE:-   We wouldn't normally do this here.  We only update the physics when
                     //          a search query is made.
                     pTileObject->mpPhysics->setPosition( CurrentTileWorldClipBoundary[0] + tileCenter );                            
                     pTileObject->mpPhysics->setRotation( getRenderRotation() );

                     // Reference Physics for Debug Rendering.
                     mDebugRenderPhysics.push_back( pTileObject->mpPhysics );
                  }
               }

               // Draw Cursor-Icon (if active)
               if (    mCursorIconActive &&
                  mCurrentCursorBlinkState &&
                  logTilePositionX == mCurrentCursorX &&
                  logTilePositionY == mCurrentCursorY )
               {
                  // Fetch frame area and bind appropriate texture page.
                  const t2dImageMapDatablock::cFrameTexelArea& frameArea = mTileIconImageMapDataBlock->getImageMapFrameAreaBind( 3 );
                  // Fetch Positions.
                  minX = frameArea.mX;
                  minY = frameArea.mY;
                  maxX = frameArea.mX2;
                  maxY = frameArea.mY2;

#ifdef TORQUE_OS_IPHONE
				  //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
				  dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
										   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
										   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
										   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
										   minX, minY,
										   maxX, minY,
										   minX, maxY,
										   maxX, maxY);
#else

                  // Draw Cursor.
                  glBegin(GL_QUADS);
                  glTexCoord2f( minX, minY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                  glTexCoord2f( maxX, minY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                  glTexCoord2f( maxX, maxY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                  glTexCoord2f( minX, maxY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                  glEnd();
#endif
			   }
            }

            // Move Current Clip Boundary in X.
            CurrentTileWorldClipBoundary[0] += tileClipStep[0];
            CurrentTileWorldClipBoundary[1] += tileClipStep[0];
            CurrentTileWorldClipBoundary[2] += tileClipStep[0];
            CurrentTileWorldClipBoundary[3] += tileClipStep[0];

            // Move to next Tile-X Position.
            logTilePositionX++;
            phyTilePositionX++;

            // Wrap Physical Tile Position.
            if ( phyTilePositionX >= mTileCountX )
               phyTilePositionX = 0;
         }
      }

      // Move Base Clip Boundary in Y.
      baseTileWorldClipBoundary[0] += tileClipStep[2];
      baseTileWorldClipBoundary[1] += tileClipStep[2];
      baseTileWorldClipBoundary[2] += tileClipStep[2];
      baseTileWorldClipBoundary[3] += tileClipStep[2];

      // Move to next Tile-Y Position.
      logTilePositionY++;
      phyTilePositionY++;

      // Wrap Physical Tile Position.
      if ( phyTilePositionY >= mTileCountY )
         phyTilePositionY = 0;
   }

   // Draw Grid (if active).
   if ( mGridActive )
   {
      // Disable Texturing.
      glDisable( GL_TEXTURE_2D );
      // Set Colour.
      glColor4f(0,0,0.5f,1);
      // Disable Blending.
      glDisable(GL_BLEND);

      // Calculate Edge Strides.
      const t2dVector& edgeStrideX = tileClipStep[0];
      const t2dVector& edgeStrideY = tileClipStep[2];

      // Calculate Edge Top/Left.
      t2dVector tileTopLeft = t2dVector(viewIntersection.point.x,viewIntersection.point.y)  - tileOffset;

      // Calculate Left-Edge.
      t2dVector edgeLeft = tileTopLeft;
      // Calculate Right-Edge.
      t2dVector edgeRight = tileTopLeft + (edgeStrideX*F32(tilesViewedX));

      // Calculate Top Edge.
      t2dVector edgeTop = tileTopLeft;
      // Calculate Bottom-Edge.
      t2dVector edgeBottom = tileTopLeft + (edgeStrideY*F32(tilesViewedY));
#ifdef TORQUE_OS_IPHONE
	   // Render all the X tile-edges.
	   for ( U32 x = 0; x < tilesViewedX; x++ )
	   {
		   GLfloat verts[] = {
			   (GLfloat)(edgeTop.mX), (GLfloat)(edgeTop.mY),
			   (GLfloat)(edgeBottom.mX), (GLfloat)(edgeBottom.mY),
		   };
		   
		   glVertexPointer(2, GL_FLOAT, 0, verts );
		   
		   glDrawArrays(GL_LINES, 0, 2);//draw last two
		   // Move to next edge.
		   edgeTop += edgeStrideX;
		   edgeBottom += edgeStrideX;
	   }

	   // Render all the Y tile-edges.
	   for ( U32 y = 0; y < tilesViewedY; y++ )
	   {
		   // Render Edge.
		   GLfloat verts[] = {
			   (GLfloat)(edgeLeft.mX), (GLfloat)(edgeLeft.mY),
			   (GLfloat)(edgeRight.mX), (GLfloat)(edgeRight.mY),
		   };
		   
		   glVertexPointer(2, GL_FLOAT, 0, verts );
		   
		   glDrawArrays(GL_LINES, 0, 2);//draw last two
		   // Move to next edge.
		   edgeLeft += edgeStrideY;
		   edgeRight += edgeStrideY;
	   }
#else

      // Begin Line Batch.
      glBegin(GL_LINES);

      // Render all the X tile-edges.
      for ( U32 x = 0; x < tilesViewedX; x++ )
      {
         // Render Edge.
         glVertex2fv( (GLfloat*)&edgeTop );
         glVertex2fv( (GLfloat*)&edgeBottom );

         // Move to next edge.
         edgeTop += edgeStrideX;
         edgeBottom += edgeStrideX;
      }

      // Render all the Y tile-edges.
      for ( U32 y = 0; y < tilesViewedY; y++ )
      {
         // Render Edge.
         glVertex2fv( (GLfloat*)&edgeLeft );
         glVertex2fv( (GLfloat*)&edgeRight );

         // Move to next edge.
         edgeLeft += edgeStrideY;
         edgeRight += edgeStrideY;
      }

      // End Line Batch.
      glEnd();
#endif
   }
	
   // Render Debug Info.
   if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS )
   {
      // Disable Texturing.
      glDisable( GL_TEXTURE_2D );
      // Set Colour.
      glColor4f(0,1,0,1);
      // Disable Blending.
      glDisable(GL_BLEND);

      // Render Collision Bounds.
      for ( U32 n = 0; n < mDebugRenderPhysics.size(); n++ )
         mDebugRenderPhysics[n]->renderCollisionBounds();

      // Clear Debug Render Vector.
      mDebugRenderPhysics.clear();
   }
   // Set Colour.
   glColor4f(1,1,1,1);

   // Disable Texturing.
   glDisable( GL_TEXTURE_2D );

//IPHONE WARNING: tilelayer 2 deep in the projection matrix is all that is allowed
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode( GL_MODELVIEW );
   // Restore old viewport.
   dglSetViewport( oldViewport );

   // Call Parent.
   Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}


//-----------------------------------------------------------------------------
// Render Object at an Arbitrary Angle.
//-----------------------------------------------------------------------------
void t2dTileLayer::renderArbitraryAngleObject( const RectF& viewPort, const RectF& viewIntersection )
{
   // We cannot render if we've not got a tile array!
   if ( !mppTileObjectArray )
      return;

   // Set Blend Options.
   setBlendOptions();

   // Enable Texturing.
   glEnable        ( GL_TEXTURE_2D );
   glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

   // Get merged Local/Scene Debug Mask.
   U32 debugMask = getDebugMask() | mpTileMap2DManager->getSceneGraph()->getDebugMask();

   // Clear Debug Render Vector.
   mDebugRenderPhysics.clear();

   // Fetch NULL String.
   StringTableEntry nullString = StringTable->insert("");

   // Calculate Logical Fractional-Tile at Intersection.
   F32 logFracTileX = mRenderTickOffset.mX / mTileSizeX;
   F32 logFracTileY = mRenderTickOffset.mY / mTileSizeY;

   F32 logTileOffsetX;
   F32 logTileOffsetY;
   F32 phyTileOffsetX;
   F32 phyTileOffsetY;
   S32 logTileX;
   S32 logTileY;

   // Deal with Pan-X Direction Appropriately.
   if ( logFracTileX < 0.0f )
   {
      logTileOffsetX = 1.0f + (logFracTileX - S32(logFracTileX));
      phyTileOffsetX = mFabs(logTileOffsetX) * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(logFracTileX - logTileOffsetX + 0.5f) - 1;
   }
   else
   {
      logTileOffsetX = logFracTileX - mFloor(logFracTileX);
      phyTileOffsetX = logTileOffsetX * mTileSizeX;
      // Calculate Logical Tile.
      logTileX = S32(mFloor(logFracTileX - logTileOffsetX));
   }

   // Deal with Pan-Y Direction Appropriately.
   if ( logFracTileY < 0.0f )
   {
      logTileOffsetY = 1.0f + (logFracTileY - S32(logFracTileY));
      phyTileOffsetY = mFabs(logTileOffsetY) * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(logFracTileY - logTileOffsetY + 0.5f) - 1;
   }
   else
   {
      logTileOffsetY = logFracTileY - mFloor(logFracTileY);
      phyTileOffsetY = logTileOffsetY * mTileSizeY;
      // Calculate Logical Tile.
      logTileY = S32(mFloor(logFracTileY - logTileOffsetY));
   }

   // Calculate Physical Tile.
   S32 phyTileX = logTileX;
   S32 phyTileY = logTileY;
   // Wrap Tile-X.
   if ( phyTileX < 0 )
      phyTileX = (phyTileX+mTileCountX) % mTileCountX;
   else if ( phyTileX >= mTileCountX )
      phyTileX %= mTileCountX;
   // Wrap Tile-Y.
   if ( phyTileY < 0 )
      phyTileY = (phyTileY+mTileCountY) % mTileCountY;
   else if ( phyTileY >= mTileCountY )
      phyTileY %= mTileCountY;

   // Calculate Tile Clip Region.
   t2dVector tileClipStep[3];
   tileClipStep[0] = (mWorldClipBoundary[1] - mWorldClipBoundary[0]);
   tileClipStep[2] = (mWorldClipBoundary[3] - mWorldClipBoundary[0]);

   // Calculate Tiles Viewed.
   const U32 tilesViewedX = U32( mCeil( (tileClipStep[0].len() + phyTileOffsetX + 0.5f) / mTileSizeX ) ) - (mIsZero(phyTileOffsetX)?1:0);
   const U32 tilesViewedY = U32( mCeil( (tileClipStep[2].len() + phyTileOffsetY + 0.5f) / mTileSizeY ) ) - (mIsZero(phyTileOffsetY)?1:0);

   // Renormalise to Calculate Tile-Sized Clip Step.
   tileClipStep[0].normalise(mTileSizeX);
   tileClipStep[2].normalise(mTileSizeY);
   tileClipStep[1] = tileClipStep[0] + tileClipStep[2];

   // Calculate Renormalised Half-Tile Clip Steps.
   t2dVector tileHalfClipStep[3];
   tileHalfClipStep[0] = tileClipStep[0] * 0.5f;
   tileHalfClipStep[2] = tileClipStep[2] * 0.5f;
   tileHalfClipStep[1] = tileHalfClipStep[0] + tileHalfClipStep[2];

   // Reference Important Half-Tile Clip Step.
   const t2dVector& tileCenter = tileHalfClipStep[1];

   // Calculate Tile Unit Steps.
   // NOTE:-   We're using logical tile offset here for the renormalisation.
   t2dVector tileOffset[3];
   tileOffset[0] = tileClipStep[0] * logTileOffsetX;
   tileOffset[2] = tileClipStep[2] * logTileOffsetY;
   tileOffset[1] = tileOffset[0] + tileOffset[2];

   // Fetch Min/Max View Intersection Points.
   t2dVector viewPortMin;
   t2dVector viewPortMax;
   viewPortMin = viewPort.point;
   viewPortMax = (viewPort.point + viewPort.extent);

   // ******************************************************************
   // Calculate viewPort as a Local-Boundary!
   //
   // NOTE:-   We only use this as the final check to see if the view is
   //          totally ENCLOSED by the tile which is the most expensive
   //          test here.
   // ******************************************************************
   t2dVector viewPortLocal[4];

   // Calculate Non-Transformed Position.
   viewPortLocal[0] = viewPortMin;
   viewPortLocal[1] = t2dVector(viewPortMax.mX, viewPortMin.mY);
   viewPortLocal[2] = viewPortMax;
   viewPortLocal[3] = t2dVector(viewPortMin.mX, viewPortMax.mY);

   // Origin.
   static t2dVector vecOrigin(0,0);

   // Get Source Inverse Matrix.
   const t2dMatrix& srcInverseRotationMatrix = getInverseRotationMatrix();

   // We don't need to invert rotate if rotation is at zero!
   if ( mNotZero(getRenderRotation()) )
   {
      // Transform Points into Local-Boundary.
      for ( U32 n = 0; n < 4; n++ )
         inverseTransformPoint2D( srcInverseRotationMatrix, viewPortLocal[n], vecOrigin, viewPortLocal[n] );
   }


   // ******************************************************************
   //
   //  To save some time, we're going to be doing some extrapolated
   //  tile-movement in world-space.  World tile-steps are calculated
   //  and used to step through the tiles prior to rendering.
   //
   // ******************************************************************

   // Initialise Base/Current Tile World Clip Boundary.
   t2dVector baseTileWorldClipBoundary[4];
   t2dVector CurrentTileWorldClipBoundary[4];

   // Calculate Base/Current Tile Clip.
   baseTileWorldClipBoundary[0] = mWorldClipBoundary[0] - tileOffset[1];
   baseTileWorldClipBoundary[1] = mWorldClipBoundary[0] - tileOffset[1] + tileClipStep[0];
   baseTileWorldClipBoundary[2] = mWorldClipBoundary[0] - tileOffset[1] + tileClipStep[1];
   baseTileWorldClipBoundary[3] = mWorldClipBoundary[0] - tileOffset[1] + tileClipStep[2];

   // Reset Tile Positions Y.
   S32 logTilePositionY = logTileY;
   U32 phyTilePositionY = phyTileY;

   // Tile Object.
   tTileObject* pTileObject;

   // Tile-Texture Region.
   F32 minX, minY, maxX, maxY;

   // Argument Buffer.
   static char argBuffer[2][64];

   // Render all the Y-tiles.
   for ( U32 y = 0; y < tilesViewedY; y++ )
   {
      // Are we rendering a tile here?
      if (    mWrapY ||   // Wrap Mode-Y On.
         ( !mWrapY && (logTilePositionY >= 0 || logTilePositionY < mTileCountY ) ) ) // Wrap Mode-Y Off but within tile-boundary.
      {
         // Transfer base clip into current.
         CurrentTileWorldClipBoundary[0] = baseTileWorldClipBoundary[0];
         CurrentTileWorldClipBoundary[1] = baseTileWorldClipBoundary[1];
         CurrentTileWorldClipBoundary[2] = baseTileWorldClipBoundary[2];
         CurrentTileWorldClipBoundary[3] = baseTileWorldClipBoundary[3];

         // Reset Tile Positions X.
         S32 logTilePositionX = logTileX;
         U32 phyTilePositionX = phyTileX;

         // Render all the X-tiles.
         for ( U32 x = 0; x < tilesViewedX; x++ )
         {
            // Are we rendering a tile here?
            if (    mWrapX ||   // Wrap Mode-X On.
               ( !mWrapX && (logTilePositionX >= 0 || logTilePositionX < mTileCountX ) ) )// Wrap Mode-X Off but within tile-boundary.
            {
               // Yes, so fetch Tile Object.
               pTileObject = mppTileObjectArray[phyTilePositionX + ( phyTilePositionY * mTileCountX )];

               // Is there a tile object here?
               if ( pTileObject )
               {
                  // ****************************************************************
                  // Yes, so check if tile is in the area...
                  // ****************************************************************

                  // Reset Viewable Flag.
                  bool viewable = false;

                  // ****************************************************************
                  // Check to see if tile is WITHIN the view...
                  // ****************************************************************
                  for ( U32 n = 0; n < 4; n++ )
                  {
                     // Check if tile is within the view.
                     if ( pointInRectangle( CurrentTileWorldClipBoundary[n], viewPortMin, viewPortMax ) )
                     {
                        // Viewable!
                        viewable = true;
                        break;
                     }
                  }

                  // Do we need to check further?
                  if ( !viewable )
                  {
                     // ****************************************************************
                     // Yes, so check to see if tile INTERSECTS the view...
                     // ****************************************************************
                     if (    lineRectangleIntersect( CurrentTileWorldClipBoundary[0], CurrentTileWorldClipBoundary[1], viewPortMin, viewPortMax ) ||
                        lineRectangleIntersect( CurrentTileWorldClipBoundary[1], CurrentTileWorldClipBoundary[2], viewPortMin, viewPortMax ) ||
                        lineRectangleIntersect( CurrentTileWorldClipBoundary[2], CurrentTileWorldClipBoundary[3], viewPortMin, viewPortMax ) ||
                        lineRectangleIntersect( CurrentTileWorldClipBoundary[3], CurrentTileWorldClipBoundary[0], viewPortMin, viewPortMax ) )
                     {
                        // Viewable!
                        viewable = true;
                     }
                  }

                  // Do we need to check further?
                  if ( !viewable )
                  {
                     // ****************************************************************
                     // Yes, so check to see if tile ENCLOSES the view...
                     // ****************************************************************

                     // Transform Current Tile into Local-Boundary.
                     t2dVector invCurrentTileWorldClipBoundaryMin;
                     t2dVector invCurrentTileWorldClipBoundaryMax;
                     inverseTransformPoint2D( srcInverseRotationMatrix, CurrentTileWorldClipBoundary[0], vecOrigin, invCurrentTileWorldClipBoundaryMin );
                     inverseTransformPoint2D( srcInverseRotationMatrix, CurrentTileWorldClipBoundary[2], vecOrigin, invCurrentTileWorldClipBoundaryMax );

                     for ( U32 n = 0; n < 4; n++ )
                     {
                        // Check if tile is within the view.
                        if ( pointInRectangle( viewPortLocal[n], invCurrentTileWorldClipBoundaryMin, invCurrentTileWorldClipBoundaryMax ) )
                        {
                           // Viewable!
                           viewable = true;
                           break;
                        }
                     }
                  }

                  // Is the tile Viewable?
                  if ( viewable )
                  {
                     // Yes, so fetch Root Tile Node for this position.
                     t2dTileMap::cRootTileNode* pRootTileNode = pTileObject->mpTileNode;

                     // Render each tile type appropriately.
                     switch( pRootTileNode->mTileType )
                     {
                        // Static Tile.
                     case t2dTileMap::STATIC_TILE:
                        {
                           // Cast Static Tile.
                           t2dTileMap::cStaticTileNode* pTileNode = (t2dTileMap::cStaticTileNode*)pRootTileNode;


                           if( !pTileNode || pTileNode->pImageMapDataBlock2D.isNull() )
                              break;

                           // Fetch frame area and bind appropriate texture page.
                           const t2dImageMapDatablock::cFrameTexelArea& frameArea = pTileNode->pImageMapDataBlock2D->getImageMapFrameAreaBind( pTileNode->mFrame );

                           // Fetch Positions.
                           minX = frameArea.mX;
                           minY = frameArea.mY;
                           maxX = frameArea.mX2;
                           maxY = frameArea.mY2;

                           // Handle Horizontal Flip.
                           if ( pTileObject->mFlipHorizontal )
                           {
                              F32 tempX = minX;
                              minX = maxX;
                              maxX = tempX;
                           }

                           // Handle Vertical Flip.
                           if ( pTileObject->mFlipVertical )
                           {
                              F32 tempY = minY;
                              minY = maxY;
                              maxY = tempY;
                           }

#ifdef TORQUE_OS_IPHONE
						   //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
							dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
												   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
												   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
												   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
												   minX, minY,
												   maxX, minY,
												   minX, maxY,
												   maxX, maxY);
#else
                           // Draw Static Tile.
                           glBegin(GL_QUADS);
                           glTexCoord2f( minX, minY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                           glTexCoord2f( maxX, minY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                           glTexCoord2f( maxX, maxY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                           glTexCoord2f( minX, maxY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                           glEnd();

#endif		
                        } break;

                        // Animation Tile.
                     case t2dTileMap::ANIMATED_TILE:
                        {
                           // Cast Animation Tile.
                           t2dTileMap::cAnimationTileNode* pTileNode = (t2dTileMap::cAnimationTileNode*)pRootTileNode;

                           // Bind with Animated Texture.
                           glBindTexture( GL_TEXTURE_2D, pTileNode->pAnimationController2D->getCurrentFrameTexture().getGLName() );

                           // Fetch Current Frame Region.
                           const t2dImageMapDatablock::cFrameTexelArea& frameArea = pTileNode->pAnimationController2D->getCurrentFrameArea();
                           // Fetch Positions.
                           minX = frameArea.mX;
                           minY = frameArea.mY;
                           maxX = frameArea.mX2;
                           maxY = frameArea.mY2;

                           // Handle Horizontal Flip.
                           if ( pTileObject->mFlipHorizontal )
                           {
                              F32 tempX = minX;
                              minX = maxX;
                              maxX = tempX;
                           }

                           // Handle Vertical Flip.
                           if ( pTileObject->mFlipVertical )
                           {
                              F32 tempY = minY;
                              minY = maxY;
                              maxY = tempY;
                           }

#ifdef TORQUE_OS_IPHONE
						   //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
							dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
												   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
												   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
												   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
												   minX, minY,
												   maxX, minY,
												   minX, maxY,
												   maxX, maxY);
#else
                           // Draw Animated Tile.
                           glBegin(GL_QUADS);
                           glTexCoord2f( minX, minY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                           glTexCoord2f( maxX, minY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                           glTexCoord2f( maxX, maxY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                           glTexCoord2f( minX, maxY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                           glEnd();

#endif
                        } break;

                        // Active Tile.
                     case t2dTileMap::ACTIVE_TILE:
                        {
                           // Cast Active Tile.
                           t2dTileMap::cActiveTileNode* pTileNode = (t2dTileMap::cActiveTileNode*)pRootTileNode;

                           // Render the Active Tile.
                           pTileNode->pActiveTile2D->renderObject( viewPort, viewIntersection, CurrentTileWorldClipBoundary );

                        } break;
                     }

                     // Custom-Data available?
                     if ( pTileObject->mCustomData != nullString )
                     {
                        // Draw Custom-Icon (if active)
                        if ( mCustomIconActive && mTileIconImageMapDataBlock )
                        {
                           // Fetch frame area and bind appropriate texture page.
                           const t2dImageMapDatablock::cFrameTexelArea& frameArea = mTileIconImageMapDataBlock->getImageMapFrameAreaBind( 0 );
                           // Fetch Positions.
                           minX = frameArea.mX;
                           minY = frameArea.mY;
                           maxX = frameArea.mX2;
                           maxY = frameArea.mY2;

                           // Calculate Icon Region.
                           const t2dVector tl = CurrentTileWorldClipBoundary[0] + tileHalfClipStep[0];
                           const t2dVector br = CurrentTileWorldClipBoundary[1] + tileHalfClipStep[2];
                           const t2dVector bl = CurrentTileWorldClipBoundary[0] + tileCenter;

#ifdef TORQUE_OS_IPHONE
							dglDrawTextureQuadiPhone(tl.mX, tl.mY,
												   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
												   bl.mX, bl.mY,
												   br.mX, br.mY,
												   minX, minY,
												   maxX, minY,
												   minX, maxY,
												   maxX, maxY);
#else
                           // Draw Animated Tile.
                           glBegin(GL_QUADS);
                           glTexCoord2f( minX, minY );
                           glVertex2fv ( (GLfloat*)&tl );
                           glTexCoord2f( maxX, minY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                           glTexCoord2f( maxX, maxY );
                           glVertex2fv ( (GLfloat*)&br );
                           glTexCoord2f( minX, maxY );
                           glVertex2fv ( (GLfloat*)&bl );
                           glEnd();
#endif
                        }
                     }

                     // Script available?
                     if ( pTileObject->mShowScript != nullString )
                     {
                        // Yes, so script unactioned?
                        if ( !pTileObject->mScriptActioned )
                        {
                           // Yes, so flag Script Actioned.
                           // NOTE:-   We do this here otherwise if the player resets the layer in
                           //          the callback, we'd reflag this tile as actioned!
                           pTileObject->mScriptActioned = true;

                           // Format layer-id buffer.
                           dSprintf( argBuffer[0], 64, "%d", getId() );
                           // Format tile-position buffer.
                           dSprintf( argBuffer[1], 64, "%d %d", phyTilePositionX, phyTilePositionY );
                           // Do script callback.
                           Con::executef( mpTileMap2DManager, 4, "onTileScript", argBuffer[0], argBuffer[1], pTileObject->mShowScript );
                        }

                        // Draw Script-Icon (if active)
                        if ( mScriptIconActive && mTileIconImageMapDataBlock )
                        {
                           // Fetch frame area and bind appropriate texture page.
                           const t2dImageMapDatablock::cFrameTexelArea& frameArea = mTileIconImageMapDataBlock->getImageMapFrameAreaBind( 1 );
                           // Fetch Positions.
                           minX = frameArea.mX;
                           minY = frameArea.mY;
                           maxX = frameArea.mX2;
                           maxY = frameArea.mY2;

                           // Calculate Top-Left Sub-tile of tile.
                           const t2dVector tr = CurrentTileWorldClipBoundary[0] + tileHalfClipStep[0];
                           const t2dVector br = CurrentTileWorldClipBoundary[0] + tileCenter;
                           const t2dVector bl = CurrentTileWorldClipBoundary[0] + tileHalfClipStep[2];

#ifdef TORQUE_OS_IPHONE
	 					   //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
							dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
												   tr.mX, tr.mY,
												   bl.mX, bl.mY,
												   br.mX, br.mY,
												   minX, minY,
												   maxX, minY,
												   minX, maxY,
												   maxX, maxY);
#else
                           // Draw Animated Tile.
                           glBegin(GL_QUADS);
                           glTexCoord2f( minX, minY );
                           glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                           glTexCoord2f( maxX, minY );
                           glVertex2fv ( (GLfloat*)&tr );
                           glTexCoord2f( maxX, maxY );
                           glVertex2fv ( (GLfloat*)&br );
                           glTexCoord2f( minX, maxY );
                           glVertex2fv ( (GLfloat*)&bl );
                           glEnd();
#endif
                        }
                     }

                     // Calculate Debug Info.
                     if ( (pTileObject->mReceiveCollisions && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS)) )
                     {
                        // Update Tiles Physics.
                        // NOTE:-   We wouldn't normally do this here.  We only update the physics when
                        //          a search query is made.
                        pTileObject->mpPhysics->setPosition( CurrentTileWorldClipBoundary[0] + tileCenter );                            
                        pTileObject->mpPhysics->setRotation( getRenderRotation() );

                        // Reference Physics for Debug Rendering.
                        mDebugRenderPhysics.push_back( pTileObject->mpPhysics );
                     }
                  }
               }

               // Draw Cursor-Icon (if active)
               if (    mCursorIconActive &&
                  mCurrentCursorBlinkState &&
                  logTilePositionX == mCurrentCursorX &&
                  logTilePositionY == mCurrentCursorY )
               {
                  // Fetch frame area and bind appropriate texture page.
                  const t2dImageMapDatablock::cFrameTexelArea& frameArea = mTileIconImageMapDataBlock->getImageMapFrameAreaBind( 3 );
                  // Fetch Positions.
                  minX = frameArea.mX;
                  minY = frameArea.mY;
                  maxX = frameArea.mX2;
                  maxY = frameArea.mY2;
				   
#ifdef TORQUE_OS_IPHONE
				  //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
					dglDrawTextureQuadiPhone(CurrentTileWorldClipBoundary[0].mX, CurrentTileWorldClipBoundary[0].mY,
										   CurrentTileWorldClipBoundary[1].mX, CurrentTileWorldClipBoundary[1].mY,
										   CurrentTileWorldClipBoundary[3].mX, CurrentTileWorldClipBoundary[3].mY,
										   CurrentTileWorldClipBoundary[2].mX, CurrentTileWorldClipBoundary[2].mY,
										   minX, minY,
										   maxX, minY,
										   minX, maxY,
										   maxX, maxY);
#else
                  // Draw Cursor.
                  glBegin(GL_QUADS);
                  glTexCoord2f( minX, minY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[0]) );
                  glTexCoord2f( maxX, minY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[1]) );
                  glTexCoord2f( maxX, maxY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[2]) );
                  glTexCoord2f( minX, maxY );
                  glVertex2fv ( (GLfloat*)&(CurrentTileWorldClipBoundary[3]) );
                  glEnd();
#endif
               }
            }

            // Move Current Clip Boundary in X.
            CurrentTileWorldClipBoundary[0] += tileClipStep[0];
            CurrentTileWorldClipBoundary[1] += tileClipStep[0];
            CurrentTileWorldClipBoundary[2] += tileClipStep[0];
            CurrentTileWorldClipBoundary[3] += tileClipStep[0];

            // Move to next Tile-X Position.
            logTilePositionX++;
            phyTilePositionX++;

            // Wrap Physical Tile Position.
            if ( phyTilePositionX >= mTileCountX )
               phyTilePositionX = 0;
         }

         // Move Base Clip Boundary in Y.
         baseTileWorldClipBoundary[0] += tileClipStep[2];
         baseTileWorldClipBoundary[1] += tileClipStep[2];
         baseTileWorldClipBoundary[2] += tileClipStep[2];
         baseTileWorldClipBoundary[3] += tileClipStep[2];

         // Move to next Tile-Y Position.
         logTilePositionY++;
         phyTilePositionY++;

         // Wrap Physical Tile Position.
         if ( phyTilePositionY >= mTileCountY )
            phyTilePositionY = 0;
      }
   }

   // Draw Grid (if active).
   if ( mGridActive )
   {
      // Disable Texturing.
      glDisable( GL_TEXTURE_2D );
      // Set Colour.
      glColor4f(0,0,0.5f,1);
      // Disable Blending.
      glDisable(GL_BLEND);

      // Calculate Edge Strides.
      const t2dVector& edgeStrideX = tileClipStep[0];
      const t2dVector& edgeStrideY = tileClipStep[2];

      // Calculate Edge Top/Left.
      const t2dVector tileTopLeft = mWorldClipBoundary[0] - tileOffset[1];

      // Calculate Left-Edge.
      t2dVector edgeLeft = tileTopLeft;
      // Calculate Right-Edge.
      t2dVector edgeRight = tileTopLeft + (edgeStrideX*F32(tilesViewedX));

      // Calculate Top Edge.
      t2dVector edgeTop = tileTopLeft;
      // Calculate Bottom-Edge.
      t2dVector edgeBottom = tileTopLeft + (edgeStrideY*F32(tilesViewedY));

// PUAP -Mat untested
#ifdef TORQUE_OS_IPHONE
	   // Render all the X tile-edges.
	   for ( U32 x = 0; x < tilesViewedX; x++ )
	   {
		   // Render Edge.
		   GLfloat verts[] = {
			   (GLfloat)(edgeTop.mX), (GLfloat)(edgeTop.mY),
			   (GLfloat)(edgeBottom.mX), (GLfloat)(edgeBottom.mY),
		   };
		   
		   glVertexPointer(2, GL_FLOAT, 0, verts );
		   
		   glDrawArrays(GL_LINES, 0, 2);//draw last two
		   
		   // Move to next edge.
		   edgeTop += edgeStrideX;
		   edgeBottom += edgeStrideX;
	   }
	   
	   // Render all the Y tile-edges.
	   for ( U32 y = 0; y < tilesViewedY; y++ )
	   {
		   // Render Edge.
		   GLfloat verts[] = {
			   (GLfloat)(edgeLeft.mX), (GLfloat)(edgeLeft.mY),
			   (GLfloat)(edgeRight.mX), (GLfloat)(edgeRight.mY),
		   };
		   
		   glVertexPointer(2, GL_FLOAT, 0, verts );
		   
		   glDrawArrays(GL_LINES, 0, 2);//draw last two
		   
		   // Move to next edge.
		   edgeLeft += edgeStrideY;
		   edgeRight += edgeStrideY;
	   }
#else
      // Begin Line Batch.
      glBegin(GL_LINES);

      // Render all the X tile-edges.
      for ( U32 x = 0; x < tilesViewedX; x++ )
      {
         // Render Edge.
         glVertex2fv( (GLfloat*)&edgeTop );
         glVertex2fv( (GLfloat*)&edgeBottom );

         // Move to next edge.
         edgeTop += edgeStrideX;
         edgeBottom += edgeStrideX;
      }

      // Render all the Y tile-edges.
      for ( U32 y = 0; y < tilesViewedY; y++ )
      {
         // Render Edge.
         glVertex2fv( (GLfloat*)&edgeLeft );
         glVertex2fv( (GLfloat*)&edgeRight );

         // Move to next edge.
         edgeLeft += edgeStrideY;
         edgeRight += edgeStrideY;
      }

      // End Line Batch.
      glEnd();
#endif
   }

   // Render Debug Info.
   if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS )
   {
      // Disable Texturing.
      glDisable( GL_TEXTURE_2D );
      // Set Colour.
      glColor4f(0,255,0,255);
      // Disable Blending.
      glDisable(GL_BLEND);

      // Render Collision Bounds.
      for ( U32 n = 0; n < mDebugRenderPhysics.size(); n++ )
         mDebugRenderPhysics[n]->renderCollisionBounds();

      // Clear Debug Render Vector.
      mDebugRenderPhysics.clear();
   }

   // Set Colour.
   glColor4f(1,1,1,1);
   // Disable Texturing.
   glDisable( GL_TEXTURE_2D );

   // Restore Textire Matrix.
   glLoadIdentity();
   glMatrixMode( GL_MODELVIEW );

   // Call Parent.
   Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}
