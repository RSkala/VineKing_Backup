#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// TileMap Editing Tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderTileMapEditTool.h"
#include "T2D/t2dStaticSprite.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderTileMapEditTool);

LevelBuilderTileMapEditTool::LevelBuilderTileMapEditTool() : LevelBuilderBaseEditTool(),
                                                             mTileLayer(NULL),
                                                             mSceneWindow(NULL),
                                                             mUndoAction(NULL),
                                                             mAddUndo(false),
                                                             mMouseDown(false),
                                                             mDragSelect(false),
                                                             mDragRect(0, 0, -1, -1),
                                                             mGridWasActive(true)
{
   // Set our tool name
   mToolName            = StringTable->insert("Tile Map Tool");
   mBitmapName = StringTable->insert("");

   mImageMap = StringTable->insert("");
   mCustomData = StringTable->insert("");
   mTileScript = StringTable->insert("");
   mFlipX = false;
   mFlipY = false;
   mCollision = false;

   mUpdateImageMap = false;
   mFrame = 0;
   mUpdateTileScript = false;
   mUpdateCustomData = false;
   mUpdateFlipX = false;
   mUpdateFlipY = false;
   mUpdateCollision = false;
   mUpdateCollisionPoly = false;

   mTool = SELECT_TOOL;

   mLayerList.clear();
}

LevelBuilderTileMapEditTool::~LevelBuilderTileMapEditTool()
{
}

ConsoleMethod(LevelBuilderTileMapEditTool, setIconBitmap, void, 3, 3, "(bitmapName)")
{
   object->setIconBitmap(argv[2]);
}

void LevelBuilderTileMapEditTool::setIconBitmap(const char* bitmapName)
{
   mBitmapName = StringTable->insert(bitmapName);
   mTextureHandle = TextureHandle(mBitmapName, BitmapTexture, true);
}

bool LevelBuilderTileMapEditTool::onActivate(LevelBuilderSceneWindow* sceneWindow)
{
   if (!Parent::onActivate(sceneWindow))
      return false;

   mSceneWindow = sceneWindow;
   mTileLayer = NULL;

   return true;
}

void LevelBuilderTileMapEditTool::onDeactivate()
{
   finishEdit();

   mUndoAction = NULL;

   mTileLayer = NULL;
   mSceneWindow = NULL;
   Parent::onDeactivate();
}

void LevelBuilderTileMapEditTool::editObject(t2dTileLayer* object)
{
   if (!mSceneWindow)
      return;
   LevelBuilderSceneEdit *pSceneEdit = mSceneWindow->getSceneEdit();

   if( pSceneEdit == NULL )
      return;

   // Make sure we're clean
   finishEdit();

   mTileLayer = object;

   mPreviousAutoPanX = mTileLayer->getAutoPanX();
   mPreviousAutoPanY = mTileLayer->getAutoPanY();
   mPreviousPanPositionX = mTileLayer->getPanPositionX();
   mPreviousPanPositionY = mTileLayer->getPanPositionY();

   mTileLayer->setAutoPan( 0.0f, 0.0f );
   mTileLayer->setPanPosition( 0.0f, 0.0f );

   mTileLayer->setGridActive(true);
   mTileLayer->setDebugOn(BIT(5));
   mTileLayer->setScriptIconActive(true);
   mTileLayer->setCustomIconActive(true);
   mTileLayer->setCursorPosition(1, 1);

   // Set all other layers to 0.25f alpha
   F32 layerAlpha = 0.25f;
   t2dSceneGraph *pScene = mSceneWindow->getSceneGraph();
   if( pScene != NULL )
   {
      for( S32 nI = 0; nI < pScene->size(); nI++ )
      {
         t2dTileLayer *pTile = dynamic_cast<t2dTileLayer*> ( pScene->at( nI ) );
         if( !pTile )
            continue;

         // Backup current alpha
         tLayerAlphaHandle layerEntry;
         layerEntry.mObjectId = pTile->getId();
         layerEntry.mAlphaValue = pTile->getBlendAlpha();

         // Layers ABOVE our layer get alpha 0.1 so as not to obstruct
         if( pTile == mTileLayer )
            layerAlpha = 0.10f;
         else
            pTile->setBlendAlpha( layerAlpha );

         // Push Backup
         mLayerList.push_back( layerEntry );
      }
   }

   mGridWasActive = pSceneEdit->getGridVisibility();

   pSceneEdit->setGridVisibility( false );
}

ConsoleMethod(LevelBuilderTileMapEditTool, editObject, void, 3, 3, "Selects an object for editing.")
{
   t2dTileLayer* obj = dynamic_cast<t2dTileLayer*>(Sim::findObject(argv[2]));
   if (obj)
      object->editObject(obj);
   else
      Con::warnf("Invalid object passed to LevelBuilderTileMapEditTool::editObject");
}

void LevelBuilderTileMapEditTool::finishEdit()
{
   if (!mTileLayer || !mSceneWindow)
      return;
   LevelBuilderSceneEdit *pSceneEdit = mSceneWindow->getSceneEdit();

   if( pSceneEdit == NULL )
      return;

   mTileLayer->setDebugOff(BIT(5));
   mTileLayer->setGridActive(false);
   mTileLayer->setScriptIconActive(false);
   mTileLayer->setCustomIconActive(false);

   mTileLayer->setAutoPan( mPreviousAutoPanX, mPreviousAutoPanY );
   mTileLayer->setPanPosition( mPreviousPanPositionX, mPreviousPanPositionY );


   // Restore other layers alpha.
   for ( U32 n = 0; n < mLayerList.size(); n++ )
   {
      t2dTileLayer *pLayer = dynamic_cast<t2dTileLayer*>( Sim::findObject( mLayerList[n].mObjectId ) );
      if( pLayer )
         pLayer->setBlendAlpha( mLayerList[n].mAlphaValue );
   }

   pSceneEdit->setGridVisibility( mGridWasActive );

   mTileLayer = NULL;
   mLayerList.clear();
}

ConsoleMethod(LevelBuilderTileMapEditTool, finishEdit, void, 2, 2, "Applies changes and ends editing of an object.")
{
   object->finishEdit();
}

ConsoleMethod( LevelBuilderTileMapEditTool, setAutoPanSettings, void, 2, 2, "() - Egregious hack for getting layers to save auto pan correctly.")
{
   object->setAutoPanSettings();
}

ConsoleMethod( LevelBuilderTileMapEditTool, resetAutoPanSettings, void, 2, 2, "() - Egregious hack for getting layers to save auto pan correctly.")
{
   object->resetAutoPanSettings();
}

void LevelBuilderTileMapEditTool::setAutoPanSettings()
{
   if( !mTileLayer )
      return;

   mTileLayer->setAutoPan( 0.0f, 0.0f );
   mTileLayer->setPanPosition( 0.0f, 0.0f );
}

void LevelBuilderTileMapEditTool::resetAutoPanSettings()
{
   if( !mTileLayer )
      return;

   mTileLayer->setAutoPan( mPreviousAutoPanX, mPreviousAutoPanY );
   mTileLayer->setPanPosition( mPreviousPanPositionX, mPreviousPanPositionY );
}

bool LevelBuilderTileMapEditTool::onKeyDown( LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event )
{
   if (!mTileLayer || (sceneWindow != mSceneWindow) || mMouseDown)
      return false;

   switch( event.keyCode )
   {
      case KEY_DELETE:
         mUndoAction = new UndoTileEditAction( mTileLayer, "Tile Edit" );
         eraseSelected( mUndoAction );
         mAddUndo = true;
   }


   if (mUndoAction)
   {
      if (mAddUndo && sceneWindow && sceneWindow->getSceneEdit() && mUndoAction->hasChanged())
      {
         mUndoAction->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
         Con::executef( mTileLayer, 1, "onChanged" );
      }

      else
         delete mUndoAction;
   }

   mAddUndo = false;
   mUndoAction = NULL;

   return false;
}

void LevelBuilderTileMapEditTool::eraseSelected( UndoTileEditAction* undo )
{
   for( U32 i = 0; i < mSelectedTiles.size(); i++ )
      eraseTile( mSelectedTiles[i], undo );
}

bool LevelBuilderTileMapEditTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mTileLayer || (sceneWindow != mSceneWindow))
      return Parent::onMouseDown(sceneWindow, mouseStatus);

   mAddUndo = false;
   mUndoAction = new UndoTileEditAction(mTileLayer, "Tile Edit");
   mMouseDown = true;

   Point2I tilePosition = Point2I();
   if (mTileLayer->pickTile(mouseStatus.mousePoint2D, tilePosition))
   {
      switch(mTool)
      {
      case SELECT_TOOL:
         if (mouseStatus.event.mouseClickCount == 2)
            floodSelect(tilePosition, true);

         else if (mouseStatus.event.modifier & SI_CTRL)
            toggleTileSelection(tilePosition);
         
         else
         {
            clearSelections();

            if( mouseStatus.event.modifier & SI_SHIFT )
               mDragSelect = true;
            else
               selectTile(tilePosition);
         }
         break;

      case PAINT_TOOL:
         if( mouseStatus.event.modifier & SI_SHIFT )
            mDragSelect = true;

         else
            drawTile(tilePosition, mUndoAction);

         break;

      case FLOOD_TOOL:
         floodFill(tilePosition, mUndoAction, true);
         break;

      case EYE_TOOL:
         char stringPos[32];
         dSprintf(stringPos, 32, "%d %d", tilePosition.x, tilePosition.y);
         Con::executef(this, 3, "onTilePicked", Con::getIntArg(mTileLayer->getId()), stringPos);
         break;

      case ERASER_TOOL:
         if( mouseStatus.event.modifier & SI_SHIFT )
            mDragSelect = true;

         else
            eraseTile(tilePosition, mUndoAction);

         break;
      }
   }

   return true;
}

bool LevelBuilderTileMapEditTool::onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mTileLayer || (sceneWindow != mSceneWindow))
      return Parent::onMouseDragged(sceneWindow, mouseStatus);

   if( mDragSelect )
      mDragRect = mouseStatus.dragRectNormal;

   Point2I tilePosition;
   if (mTileLayer->pickTile(mouseStatus.mousePoint2D, tilePosition))
   {
      switch(mTool)
      {
      case SELECT_TOOL:
         if( !mDragSelect)
            selectTile(tilePosition);

         break;

      case PAINT_TOOL:
         if( !mDragSelect)
            drawTile(tilePosition, mUndoAction);

         break;

      case FLOOD_TOOL:
         //floodFill(tilePosition);
         break;

      case EYE_TOOL:
         //char stringPos[32];
         //dSprintf(stringPos, 32, "%d %d", tilePosition.x, tilePosition.y);
         //Con::executef(this, 3, "onTilePicked", Con::getIntArg(mTileLayer->getId()), stringPos);
         break;

      case ERASER_TOOL:
         if( !mDragSelect )
            eraseTile(tilePosition, mUndoAction);

         break;
      }
   }

   return true;
}

void LevelBuilderTileMapEditTool::selectTile(Point2I tile)
{
   if (!isSelected(tile))
      mSelectedTiles.push_back(tile);
}

void LevelBuilderTileMapEditTool::clearSelections()
{
      mSelectedTiles.clear();
}

void LevelBuilderTileMapEditTool::deselectTile(Point2I tilePosition)
{
   Vector<Point2I>::iterator i;
   for (i = mSelectedTiles.begin(); i != mSelectedTiles.end(); i++)
   {
      if (*i == tilePosition)
      {
         mSelectedTiles.erase_fast(i);
         break;
      }
   }
}

void LevelBuilderTileMapEditTool::toggleTileSelection(Point2I tile)
{
   if (!isSelected(tile))
      selectTile(tile);
   else
      deselectTile(tile);
}

bool LevelBuilderTileMapEditTool::isSelected(Point2I tilePosition)
{
   Vector<Point2I>::iterator i;
   for (i = mSelectedTiles.begin(); i != mSelectedTiles.end(); i++)
   {
      if (*i == tilePosition)
         return true;
   }

   return false;
}

void LevelBuilderTileMapEditTool::floodSelect(Point2I tile, bool start)
{
   if (!mTileLayer)
      return;

   static S32 recursionCount = 0;
   if( start )
      recursionCount = 0;
   else
      recursionCount++;

   if( recursionCount > 1000 )
      return;

   t2dTileLayer::tTileObject* tileObject;
   t2dTileLayer::tTileObject* newTileObject;
   t2dTileLayer::tTileObject* upObject;
   t2dTileLayer::tTileObject* downObject;
   t2dTileLayer::tTileObject* leftObject;
   t2dTileLayer::tTileObject* rightObject;

   if (mTileLayer->getTileObject(tile.x, tile.y, &tileObject))
   {
      if (tileObject)
      {
         t2dTileMap::cRootTileNode* tileNode = tileObject->mpTileNode;
         selectTile(tile);
         if (mTileLayer->getTileObject(tile.x, tile.y - 1, &upObject) && upObject && (tileNode == upObject->mpTileNode) && !isSelected(Point2I(tile.x, tile.y - 1)))
            floodSelect(Point2I(tile.x, tile.y - 1));

         if (mTileLayer->getTileObject(tile.x, tile.y + 1, &downObject) && downObject && (tileNode == downObject->mpTileNode) && !isSelected(Point2I(tile.x, tile.y + 1)))
            floodSelect(Point2I(tile.x, tile.y + 1));

         if (mTileLayer->getTileObject(tile.x - 1, tile.y, &leftObject) && leftObject && (tileNode == leftObject->mpTileNode) && !isSelected(Point2I(tile.x - 1, tile.y)))
            floodSelect(Point2I(tile.x - 1, tile.y));

         if (mTileLayer->getTileObject(tile.x + 1, tile.y, &rightObject) && rightObject && (tileNode == rightObject->mpTileNode) && !isSelected(Point2I(tile.x + 1, tile.y)))
            floodSelect(Point2I(tile.x + 1, tile.y));
      }

      else
      {
         selectTile(tile);
         if (mTileLayer->getTileObject(tile.x, tile.y - 1, &upObject) && !upObject && !isSelected(Point2I(tile.x, tile.y - 1)))
            floodSelect(Point2I(tile.x, tile.y - 1));

         if (mTileLayer->getTileObject(tile.x, tile.y + 1, &downObject) && !downObject && !isSelected(Point2I(tile.x, tile.y + 1)))
            floodSelect(Point2I(tile.x, tile.y + 1));

         if (mTileLayer->getTileObject(tile.x - 1, tile.y, &leftObject) && !leftObject && !isSelected(Point2I(tile.x - 1, tile.y)))
            floodSelect(Point2I(tile.x - 1, tile.y));

         if (mTileLayer->getTileObject(tile.x + 1, tile.y, &rightObject) && !rightObject && !isSelected(Point2I(tile.x + 1, tile.y)))
            floodSelect(Point2I(tile.x + 1, tile.y));
      }
   }
}

void LevelBuilderTileMapEditTool::addUndoData(Point2I tile, UndoTileEditAction* undo, bool old)
{
   if (!undo)
      return;

   t2dTileLayer::tTileObject* object;
   bool gotObject = mTileLayer->getTileObject(tile.x, tile.y, &object);
   if (!gotObject || !object)
   {
      if (old)
         undo->addOldTile(tile, StringTable->insert(""), 0, StringTable->insert(""), StringTable->insert(""), false, false, false, 0, NULL);
      else
         undo->addNewTile(tile, StringTable->insert(""), 0, StringTable->insert(""), StringTable->insert(""), false, false, false, 0, NULL);
   }

   else
   {
      StringTableEntry image = StringTable->insert("");
      U32 frame = 0;
      t2dTileMap::cStaticTileNode* staticTile = dynamic_cast<t2dTileMap::cStaticTileNode*>(object->mpTileNode);
      t2dTileMap::cAnimationTileNode* animatedTile = dynamic_cast<t2dTileMap::cAnimationTileNode*>(object->mpTileNode);
      if (staticTile && staticTile->pImageMapDataBlock2D)
      {
         image = StringTable->insert(staticTile->pImageMapDataBlock2D->getName() ? staticTile->pImageMapDataBlock2D->getName() : "");
         frame = staticTile->mFrame;
      }
      else if (animatedTile && animatedTile->pAnimationController2D)
      {
         image = StringTable->insert(animatedTile->pAnimationController2D->getCurrentAnimation() ? animatedTile->pAnimationController2D->getCurrentAnimation() : "");
      }

      StringTableEntry customData = object->mCustomData;
      StringTableEntry tileScript = object->mShowScript;
      bool flipX = object->mFlipHorizontal;
      bool flipY = object->mFlipVertical;
      bool collision = object->mReceiveCollisions;
      S32 polyCount = 0;
      const t2dVector* poly = NULL;
      if (collision && object->mpPhysics)
      {
         polyCount = object->mpPhysics->getCollisionPolyCount();
         poly = object->mpPhysics->getCollisionPolyBasis();
      }

      if (old)
         undo->addOldTile(tile, image, frame, customData, tileScript, flipX, flipY, collision, polyCount, poly);
      else
         undo->addNewTile(tile, image, frame, customData, tileScript, flipX, flipY, collision, polyCount, poly);
   }
   mAddUndo = true;
}

void LevelBuilderTileMapEditTool::drawTile(Point2I tile, UndoTileEditAction* undo)
{
   if (!mTileLayer)
      return;

   addUndoData(tile, undo, true);

   if (mUpdateImageMap && mImageMap)
   {
      t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(mImageMap));
      t2dAnimationDatablock* animation = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject(mImageMap));
      if (imageMap)
      {
         if ((mFrame < 0) || (mFrame >= imageMap->getImageMapFrameCount()))
            mFrame = 0;

         mTileLayer->setStaticTile(tile.x, tile.y, mImageMap, mFrame);
      }
      else if (animation)
         mTileLayer->setAnimatedTile(tile.x, tile.y, mImageMap, false);
      else
      {
         mTileLayer->clearTile(tile.x, tile.y);
         addUndoData(tile, undo, false);
         return;
      }
   }

   t2dTileLayer::tTileObject* object;
   if (!mTileLayer->getTileObject(tile.x, tile.y, &object) || !object)
      return;

   if (mUpdateTileScript)
      mTileLayer->setTileScript(tile.x, tile.y, mTileScript);

   if (mUpdateCustomData)
      mTileLayer->setTileCustomData(tile.x, tile.y, mCustomData);

   bool flipX, flipY;
   mTileLayer->getTileFlip(tile.x, tile.y, flipX, flipY);
   if (!mUpdateFlipX)
      mFlipX = flipX;

   if (!mUpdateFlipY)
      mFlipY = flipY;

   if (mUpdateFlipX || mUpdateFlipY)
      mTileLayer->setTileFlip(tile.x, tile.y, mFlipX, mFlipY);

   if (mUpdateCollision)
      mTileLayer->setTileCollisionActive(tile.x, tile.y, mCollision);

   if (mUpdateCollisionPoly && (mCollisionPoly.size() > 0))
   {
      Vector<t2dVector> collisionPoly;
      collisionPoly.reserve( mCollisionPoly.size() );
      for( S32 i = 0; i < mCollisionPoly.size(); i++ )
      {
         t2dVector vertex = mCollisionPoly[i];
         if( mFlipX )
            vertex.mX = -vertex.mX;
         if( mFlipY )
            vertex.mY = -vertex.mY;

         collisionPoly.push_back( vertex );
      }

      mTileLayer->setTileCollisionPolyCustom(tile.x, tile.y, collisionPoly.size(), collisionPoly.address());
   }

   addUndoData(tile, undo, false);
}

void LevelBuilderTileMapEditTool::eraseTile(Point2I tile, UndoTileEditAction* undo)
{
   if (!mTileLayer)
      return;

   addUndoData(tile, undo, true);
   mTileLayer->clearTile(tile.x, tile.y);
   addUndoData(tile, undo, false);
}

void LevelBuilderTileMapEditTool::floodFill(Point2I tile, UndoTileEditAction* undo, bool start)
{
   if (!mTileLayer)
      return;

   static S32 recursionCount = 0;
   if( start )
      recursionCount = 0;
   else
      recursionCount++;

   if( recursionCount > 2000 )
      return;

   t2dTileLayer::tTileObject* tileObject;
   t2dTileLayer::tTileObject* newTileObject;
   t2dTileLayer::tTileObject* upObject;
   t2dTileLayer::tTileObject* downObject;
   t2dTileLayer::tTileObject* leftObject;
   t2dTileLayer::tTileObject* rightObject;

   if (mTileLayer->getTileObject(tile.x, tile.y, &tileObject))
   {
      if (tileObject)
      {
         t2dTileMap::cRootTileNode* tileNode = tileObject->mpTileNode;
         drawTile(tile, undo);
         if (mTileLayer->getTileObject(tile.x, tile.y, &newTileObject) && newTileObject)
         {
            if (tileNode == newTileObject->mpTileNode)
               return;
         }

         if (mTileLayer->getTileObject(tile.x, tile.y - 1, &upObject) && upObject && (tileNode == upObject->mpTileNode))
            floodFill(Point2I(tile.x, tile.y - 1), undo);

         if (mTileLayer->getTileObject(tile.x, tile.y + 1, &downObject) && downObject && (tileNode == downObject->mpTileNode))
            floodFill(Point2I(tile.x, tile.y + 1), undo);

         if (mTileLayer->getTileObject(tile.x - 1, tile.y, &leftObject) && leftObject && (tileNode == leftObject->mpTileNode))
            floodFill(Point2I(tile.x - 1, tile.y), undo);

         if (mTileLayer->getTileObject(tile.x + 1, tile.y, &rightObject) && rightObject && (tileNode == rightObject->mpTileNode))
            floodFill(Point2I(tile.x + 1, tile.y), undo);
      }

      else if( mImageMap && *mImageMap )
      {
         drawTile(tile, undo);
         if (mTileLayer->getTileObject(tile.x, tile.y - 1, &upObject) && !upObject)
            floodFill(Point2I(tile.x, tile.y - 1), undo);

         if (mTileLayer->getTileObject(tile.x, tile.y + 1, &downObject) && !downObject)
            floodFill(Point2I(tile.x, tile.y + 1), undo);

         if (mTileLayer->getTileObject(tile.x - 1, tile.y, &leftObject) && !leftObject)
            floodFill(Point2I(tile.x - 1, tile.y), undo);

         if (mTileLayer->getTileObject(tile.x + 1, tile.y, &rightObject) && !rightObject)
            floodFill(Point2I(tile.x + 1, tile.y), undo);
      }
   }
}

void LevelBuilderTileMapEditTool::selectRect( RectF rect )
{
   if( !mTileLayer )
      return;
   
   Point2I upperLeft, lowerRight;
   mTileLayer->pickTile( rect.point, upperLeft );
   mTileLayer->pickTile( rect.point + rect.extent, lowerRight );

   for( S32 x = upperLeft.x; x <= lowerRight.x; x++ )
   {
      for( S32 y = upperLeft.y; y <= lowerRight.y; y++ )
         selectTile( Point2I( x, y ) );
   }
}

void LevelBuilderTileMapEditTool::paintRect( RectF rect, UndoTileEditAction* undo )
{
   if( !mTileLayer )
      return;

   Point2I upperLeft, lowerRight;
   mTileLayer->pickTile( rect.point, upperLeft );
   mTileLayer->pickTile( rect.point + rect.extent, lowerRight );

   for( S32 x = upperLeft.x; x <= lowerRight.x; x++ )
   {
      for( S32 y = upperLeft.y; y <= lowerRight.y; y++ )
         drawTile( Point2I( x, y ), undo );
   }
}

void LevelBuilderTileMapEditTool::eraseRect( RectF rect, UndoTileEditAction* undo )
{
   if( !mTileLayer )
      return;

   Point2I upperLeft, lowerRight;
   mTileLayer->pickTile( rect.point, upperLeft );
   mTileLayer->pickTile( rect.point + rect.extent, lowerRight );

   for( S32 x = upperLeft.x; x <= lowerRight.x; x++ )
   {
      for( S32 y = upperLeft.y; y <= lowerRight.y; y++ )
         eraseTile( Point2I( x, y ), undo );
   }
}

bool LevelBuilderTileMapEditTool::onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mTileLayer || (sceneWindow != mSceneWindow))
      return Parent::onMouseUp(sceneWindow, mouseStatus);

   Point2I tilePosition;
   if (mTileLayer->pickTile(mouseStatus.mousePoint2D, tilePosition))
   {
      switch(mTool)
      {
      case SELECT_TOOL:
         if( mDragSelect )
            selectRect( mouseStatus.dragRectNormal2D );

         break;

      case PAINT_TOOL:
         if( mDragSelect )
            paintRect( mouseStatus.dragRectNormal2D, mUndoAction );

         break;

      case ERASER_TOOL:
         if( mDragSelect )
            eraseRect( mouseStatus.dragRectNormal2D, mUndoAction );

         break;
      }
   }

   if (mUndoAction)
   {
      if (mAddUndo && sceneWindow && sceneWindow->getSceneEdit() && mUndoAction->hasChanged())
      {
         mUndoAction->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
         Con::executef( mTileLayer, 1, "onChanged" );
      }

      else
         delete mUndoAction;
   }

   mMouseDown = false;
   mAddUndo = false;
   mUndoAction = NULL;

   mDragSelect = false;
   mDragRect = RectI(0, 0, -1, -1);

   return true;
}

void LevelBuilderTileMapEditTool::onRenderGraph( LevelBuilderSceneWindow* sceneWindow )
{
   // Render Parent
   Parent::onRenderGraph( sceneWindow );

   if (!mTileLayer || (sceneWindow != mSceneWindow))
      return;

   if (!mTileLayer->getSceneGraph())
   {
      finishEdit();

      if (sceneWindow->getSceneEdit())
         sceneWindow->getSceneEdit()->setDefaultToolActive();

      return;
   }

   RectF layerRect;
   layerRect.point = mTileLayer->getPosition() - mTileLayer->getHalfSize();
   layerRect.extent = Point2F(mTileLayer->getTileSizeX() * mTileLayer->getTileCountX(),
                              mTileLayer->getTileSizeY() * mTileLayer->getTileCountY());

   t2dVector upperLeft = t2dVector(layerRect.point.x, layerRect.point.y);
   t2dVector lowerRight = t2dVector(layerRect.extent.x, layerRect.extent.y) + upperLeft;
   t2dVector windowUpperLeft, windowLowerRight;

   sceneWindow->sceneToWindowCoord(upperLeft, windowUpperLeft);
   sceneWindow->sceneToWindowCoord(lowerRight, windowLowerRight);

   RectI objRect = RectI(S32(windowUpperLeft.mX), S32(windowUpperLeft.mY),
                         S32(windowLowerRight.mX - windowUpperLeft.mX), S32(windowLowerRight.mY - windowUpperLeft.mY));

   objRect.point = sceneWindow->localToGlobalCoord( objRect.point );

   RectI objBounds = sceneWindow->getObjectBoundsWindow( mTileLayer );
   objBounds.point = sceneWindow->localToGlobalCoord( objBounds.point );

   Point2F position = Point2F(objRect.point.x, objRect.point.y);
   Point2F size = Point2F((F32)objRect.extent.x / (F32)mTileLayer->getTileCountX(), (F32)objRect.extent.y / (F32)mTileLayer->getTileCountY());
   Point2F iconSize = size / 3;

   if (mTextureHandle)
   {
      dglClearBitmapModulation();

      for (S32 x = 0; x < mTileLayer->getTileCountX(); x++)
      {
         for (S32 y = 0; y < mTileLayer->getTileCountY(); y++)
         {
            if( !objRect.overlaps( RectI( position.x, position.y, size.x, size.y ) ) )
            {
               position.y += size.y;
               continue;
            }

            const char* data = mTileLayer->getTileCustomData(x, y);
            if (data && *data)
            {
               RectI rect1(Point2I(position.x, position.y), Point2I(iconSize.x, iconSize.y));
               dglDrawBitmapStretchSR(mTextureHandle, rect1, RectI(0, 0, 32, 32));
            }

            const char* script = mTileLayer->getTileScript(x, y);
            if (script && *script)
            {
               RectI rect2(Point2I(position.x + size.x - iconSize.x, position.y), Point2I(iconSize.x, iconSize.y));
               dglDrawBitmapStretchSR(mTextureHandle, rect2, RectI(32, 0, 32, 32));
            }

            if ((mTool == SELECT_TOOL) && isSelected(Point2I(x, y)))
            {
               RectI rect3(Point2I(position.x, position.y), Point2I(size.x, size.y));
               rect3.intersect(objBounds);
               if (rect3.isValidRect())
                  dglDrawRect(rect3, ColorI(255, 255, 255, 200));
            }

            position.y += size.y;
         }
         position.x += size.x;
         position.y = objRect.point.y;
      }
   }

   // Draw a translucent rect in the area of the object bigger than the tiles. This is always going
   // to be to the right and below the tile layer.
   RectI rectRight = RectI(objRect.point.x + objRect.extent.x, objRect.point.y, objBounds.extent.x - objRect.extent.x, objBounds.extent.y);
   RectI rectBottom = RectI(objRect.point.x, objRect.point.y + objRect.extent.y, objRect.extent.x, objBounds.extent.y - objRect.extent.y);
   if (rectRight.isValidRect())
      dglDrawRectFill(rectRight, ColorI(155, 100, 100, 128));
   
   if (rectBottom.isValidRect())
      dglDrawRectFill(rectBottom, ColorI(155, 100, 100, 128));

   // Draw a rect displaying the drag rect if drag selecting is active.
   if( mDragSelect && mDragRect.isValidRect() )
   {
      dglDrawRect(mDragRect, ColorI(255, 255, 255, 200));
      dglDrawRectFill(mDragRect, ColorI(128, 128, 128, 200));
   }
}

ConsoleMethod(LevelBuilderTileMapEditTool, setImage, void, 3, 3, "")
{
   bool use = true;
   if (dStricmp(argv[2], "-") == 0)
      use = false;

   object->setImageMap(argv[2], use);
}

void LevelBuilderTileMapEditTool::setImageMap(const char* imageMapName, bool useImageMap)
{
   mUpdateImageMap = useImageMap;

   if (useImageMap)
      mImageMap = StringTable->insert(imageMapName);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setFrame, void, 3, 3, "")
{
   object->setFrame(dAtoi(argv[2]));
}

void LevelBuilderTileMapEditTool::setFrame(U32 frame)
{
   mFrame = frame;
}

ConsoleMethod(LevelBuilderTileMapEditTool, setTileScript, void, 3, 3, "")
{
   bool use = true;
   if (dStricmp(argv[2], "-") == 0)
      use = false;

   object->setTileScript(argv[2], use);
}

void LevelBuilderTileMapEditTool::setTileScript(const char* script, bool useScript)
{
   mUpdateTileScript = useScript;
   if (useScript)
      mTileScript = StringTable->insert(script);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setCustomData, void, 3, 3, "")
{
   bool use = true;
   if (dStricmp(argv[2], "-") == 0)
      use = false;

   object->setCustomData(argv[2], use);
}

void LevelBuilderTileMapEditTool::setCustomData(const char* data, bool useData)
{
   mUpdateCustomData = useData;
   if (useData)
      mCustomData = StringTable->insert(data);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setFlipX, void, 3, 3, "")
{
   bool use = true;
   if (dAtoi(argv[2]) < 0)
      use = false;

   object->setFlipX(dAtob(argv[2]), use);
}

void LevelBuilderTileMapEditTool::setFlipX(bool flip, bool useFlip)
{
   mUpdateFlipX = useFlip;
   if (useFlip)
      mFlipX = flip;
}

ConsoleMethod(LevelBuilderTileMapEditTool, setFlipY, void, 3, 3, "")
{
   bool use = true;
   if (dAtoi(argv[2]) < 0)
      use = false;

   object->setFlipY(dAtob(argv[2]), use);
}

void LevelBuilderTileMapEditTool::setFlipY(bool flip, bool useFlip)
{
   mUpdateFlipY = useFlip;
   if (useFlip)
      mFlipY = flip;
}

ConsoleMethod(LevelBuilderTileMapEditTool, setCollision, void, 3, 3, "")
{
   bool use = true;
   if (dAtoi(argv[2]) < 0)
      use = false;

   object->setCollision(dAtob(argv[2]), use);
}

void LevelBuilderTileMapEditTool::setCollision(bool enabled, bool useCollision)
{
   mUpdateCollision = useCollision;
   if (useCollision)
      mCollision = enabled;
}

ConsoleMethod(LevelBuilderTileMapEditTool, setCollisionPoly, void, 3, 4, "")
{
   bool use = true;
   if (*argv[2] == 0)
      use = false;

   if (use)
   {
      S32 count = dAtoi(argv[2]);
      t2dVector* poly = new t2dVector[count];

      if ((argc < 4) || ((count << 1) != t2dSceneObject::getStringElementCount(argv[3])))
      {
         Con::warnf("Invalid Polygon specified in LevelBuilderTileMapEditTool::setCollisionPoly().");
         return;
      }

      for (S32 i = 0; i < count; i++)
      {
         poly[i] = t2dSceneObject::getStringElementVector(argv[3], i * 2);
      }

      object->setCollisionPoly(count, poly, true);
      delete [] poly;
   }
   else
      object->setCollisionPoly(-1, NULL, false);
}

void LevelBuilderTileMapEditTool::setCollisionPoly(S32 count, t2dVector* poly, bool usePoly)
{
   mUpdateCollisionPoly = usePoly;
   if (usePoly && poly)
   {
      // TODO: Check For Convex
      mCollisionPoly.clear();
      for (S32 i = 0; i < count; i++)
         mCollisionPoly.push_back(poly[i]);
   }
}

ConsoleMethod(LevelBuilderTileMapEditTool, applyToSelection, void, 2, 2, "")
{
   object->applyToSelection();
}

void LevelBuilderTileMapEditTool::applyToSelection()
{
   if (!mTileLayer)
      return;

   if (mTool == SELECT_TOOL)
   {
      mUndoAction = new UndoTileEditAction(mTileLayer, "Edit Tile");
      Vector<Point2I>::const_iterator i;
      for (i = mSelectedTiles.begin(); i != mSelectedTiles.end(); i++)
         drawTile(*i, mUndoAction);

      if (mSceneWindow && mSceneWindow->getSceneEdit() && mUndoAction->hasChanged())
         mUndoAction->addToManager(&mSceneWindow->getSceneEdit()->getUndoManager());
      else
         delete mUndoAction;

      mUndoAction = NULL;
   }
}

ConsoleMethod(LevelBuilderTileMapEditTool, getTileSize, const char*, 2, 2, "")
{
   t2dVector tileSize = object->getTileSize();
   char* stringSize = Con::getReturnBuffer(32);
   dSprintf(stringSize, 32, "%f %f", tileSize.mX, tileSize.mY);
   return stringSize;
}

t2dVector LevelBuilderTileMapEditTool::getTileSize()
{
   if (!mTileLayer)
      return t2dVector::getZero();

   t2dVector ret;
   ret.mX = mTileLayer->getTileSizeX();
   ret.mY = mTileLayer->getTileSizeY();
   return ret;
}

ConsoleMethod(LevelBuilderTileMapEditTool, setSelectTool, void, 2, 2, "")
{
   object->setTool(LevelBuilderTileMapEditTool::SELECT_TOOL);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setPaintTool, void, 2, 2, "")
{
   object->setTool(LevelBuilderTileMapEditTool::PAINT_TOOL);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setFloodTool, void, 2, 2, "")
{
   object->setTool(LevelBuilderTileMapEditTool::FLOOD_TOOL);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setEyeTool, void, 2, 2, "")
{
   object->setTool(LevelBuilderTileMapEditTool::EYE_TOOL);
}

ConsoleMethod(LevelBuilderTileMapEditTool, setEraserTool, void, 2, 2, "")
{
   object->setTool(LevelBuilderTileMapEditTool::ERASER_TOOL);
}

void LevelBuilderTileMapEditTool::setTool(ToolType tool)
{
   mTool = tool;
}

#endif // TORQUE_TOOLS
