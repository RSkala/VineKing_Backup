#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// TileMap Editing Tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERTILEMAPEDITTOOL_H_
#define _LEVELBUILDERTILEMAPEDITTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DTILEMAP_H_
#include "T2D/t2dTileMap.h"
#endif

#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#include "TGB/levelBuilderBaseEditTool.h"
#endif

class UndoTileEditAction;


///-----------------------------------------------------------------------------
/// Structures.
///-----------------------------------------------------------------------------
struct tLayerAlphaHandle
{
   SimObjectId     mObjectId;
   F32             mAlphaValue;
};

///-----------------------------------------------------------------------------
/// Types.
///-----------------------------------------------------------------------------
typedef Vector<tLayerAlphaHandle> typeLayerAlphaList;


//-----------------------------------------------------------------------------
// LevelBuilderTileMapEditTool
// Provides tilemap editing functionality.
//-----------------------------------------------------------------------------
class LevelBuilderTileMapEditTool : public LevelBuilderBaseEditTool
{
public:
   enum ToolType
   {
      SELECT_TOOL,
      PAINT_TOOL,
      EYE_TOOL,
      FLOOD_TOOL,
      ERASER_TOOL,

      INVALID_TOOL
   };

private:
   ToolType mTool;

   bool mMouseDown;

   StringTableEntry mBitmapName;
   TextureHandle mTextureHandle;

   F32 mPreviousAutoPanX;
   F32 mPreviousAutoPanY;
   F32 mPreviousPanPositionX;
   F32 mPreviousPanPositionY;

   bool mAddUndo;
   UndoTileEditAction* mUndoAction;

   StringTableEntry mImageMap;
   StringTableEntry mTileScript;
   StringTableEntry mCustomData;
   U32 mFrame;
   bool mFlipX;
   bool mFlipY;
   bool mCollision;
   Vector<t2dVector> mCollisionPoly;

   bool mDragSelect;
   RectI mDragRect;

   bool mUpdateImageMap;
   bool mUpdateTileScript;
   bool mUpdateCustomData;
   bool mUpdateFlipX;
   bool mUpdateFlipY;
   bool mUpdateCollision;
   bool mUpdateCollisionPoly;

   typeLayerAlphaList mLayerList;
   bool           mGridWasActive;

   void eraseSelected( UndoTileEditAction* undo );

   Vector<Point2I> mSelectedTiles;
   void selectTile(Point2I tilePosition);
   void deselectTile(Point2I tilePosition);
   void toggleTileSelection(Point2I tilePosition);
   void clearSelections();
   bool isSelected(Point2I tilePosition);
   void floodSelect(Point2I tilePosition, bool start = 0);

   void selectRect(RectF rect);
   void paintRect(RectF rect, UndoTileEditAction* undo);
   void eraseRect(RectF rect, UndoTileEditAction* undo);

   void drawTile(Point2I tilePosition, UndoTileEditAction* undo);
   void floodFill(Point2I tilePosition, UndoTileEditAction* undo, bool start = 0);
   void eraseTile(Point2I tilePosition, UndoTileEditAction* undo);
   void addUndoData(Point2I tilePosition, UndoTileEditAction* undo, bool old);

protected:
   typedef LevelBuilderBaseEditTool Parent;

   LevelBuilderSceneWindow* mSceneWindow;
   t2dTileLayer*            mTileLayer;
  
public:
   LevelBuilderTileMapEditTool();
   ~LevelBuilderTileMapEditTool();
   
   virtual bool onActivate(LevelBuilderSceneWindow* sceneWindow);
   virtual void onDeactivate();

   void setTool(ToolType tool);
   t2dVector getTileSize();
   void applyToSelection();

   virtual bool onKeyDown( LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event );
	virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   void resetAutoPanSettings();
   void setAutoPanSettings();

   // Object Editing
   void editObject(t2dTileLayer* object);
   // This cancels an edit, applying changes.
   void finishEdit();

   void setIconBitmap( const char* bitmapName );

   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   void setImageMap(const char* imageMap, bool useImageMap);
   void setFrame(U32 frame);
   void setTileScript(const char* script, bool useScript);
   void setCustomData(const char* data, bool useData);
   void setFlipX(bool flip, bool useFlip);
   void setFlipY(bool flip, bool useFlip);
   void setCollision(bool enabled, bool useCollision);
   void setCollisionPoly(S32 count, t2dVector* poly, bool usePoly);

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderTileMapEditTool);
};

class UndoTileEditAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   t2dTileLayer* mLayer;

   struct TileChange
   {
      Point2I tile;
      StringTableEntry image;
      U32 frame;
      StringTableEntry customData;
      StringTableEntry tileScript;
      bool flipX;
      bool flipY;
      bool collision;
      Vector<t2dVector> collisionPoly;
      
      StringTableEntry oldImage;
      U32 oldFrame;
      StringTableEntry oldCustomData;
      StringTableEntry oldTileScript;
      bool oldFlipX;
      bool oldFlipY;
      bool oldCollision;
      Vector<t2dVector> oldCollisionPoly;
   };

   Vector<TileChange> mTiles;

public:
   UndoTileEditAction(t2dTileLayer* layer, UTF8* actionName) : UndoAction(actionName) { mLayer = layer; deleteNotify(layer); };

   virtual void onDeleteNotify(SimObject* object)
   {
      if (mUndoManager)
         mUndoManager->removeAction(this);
   };

   bool hasChanged()
   {
      for (S32 i = 0; i < mTiles.size(); i++)
      {
         TileChange& tile = mTiles[i];
         if ((tile.image != tile.oldImage) ||
             (tile.frame != tile.oldFrame) ||
             (tile.customData != tile.oldCustomData) ||
             (tile.tileScript != tile.oldTileScript) ||
             (tile.flipX != tile.oldFlipX) ||
             (tile.flipY != tile.oldFlipY) ||
             (tile.collision != tile.oldCollision) ||
             (tile.collisionPoly.size() != tile.oldCollisionPoly.size()))
            return true;

         for (S32 j = 0; j < tile.collisionPoly.size(); j++)
         {
            if (tile.collisionPoly[j] != tile.oldCollisionPoly[j])
               return true;
         }
      }

      return false;
   };

   S32 findTile(Point2I tilePosition)
   {
      for (U32 i = 0; i < mTiles.size(); i++)
      {
         if (mTiles[i].tile == tilePosition)
            return i;
      }
      return -1;
   }

   void addOldTile(Point2I tilePosition, StringTableEntry image, U32 frame, StringTableEntry customData,
                   StringTableEntry tileScript, bool flipX, bool flipY, bool collision, S32 polyCount, const t2dVector* poly)
   {
      S32 tileIndex = findTile(tilePosition);
      if (tileIndex != -1)
         return;

      mTiles.increment();
      TileChange& tile = mTiles.last();
      constructInPlace(&tile);
      tile.tile = tilePosition;
      tile.image = image;
      tile.frame = frame;
      tile.customData = customData;
      tile.tileScript = tileScript;
      tile.flipX = flipX;
      tile.flipY = flipY;
      tile.collision = collision;
      for (S32 i = 0; i < polyCount; i++)
         tile.collisionPoly.push_back(poly[i]);
   }

   void addNewTile(Point2I tilePosition, StringTableEntry image, U32 frame, StringTableEntry customData,
                   StringTableEntry tileScript, bool flipX, bool flipY, bool collision, S32 polyCount, const t2dVector* poly)
   {
      S32 tileIndex = findTile(tilePosition);
      if (tileIndex == -1)
         return;

      TileChange& tile = mTiles[tileIndex];
      tile.oldImage = image;
      tile.oldFrame = frame;
      tile.oldCustomData = customData;
      tile.oldTileScript = tileScript;
      tile.oldFlipX = flipX;
      tile.oldFlipY = flipY;
      tile.oldCollision = collision;
      for (S32 i = 0; i < polyCount; i++)
         tile.oldCollisionPoly.push_back(poly[i]);
   }

   virtual void redo()
   {
      for (U32 i = 0; i < mTiles.size(); i++)
      {
         TileChange& tile = mTiles[i];
         S32 x = tile.tile.x;
         S32 y = tile.tile.y;

         if (!tile.oldImage || !*tile.oldImage)
            mLayer->clearTile(x, y);

         else
         {
            t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(tile.oldImage));
            t2dAnimationDatablock* animation = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject(tile.oldImage));
            if (imageMap)
               mLayer->setStaticTile(x, y, tile.oldImage, tile.oldFrame);
            else if (animation)
               mLayer->setAnimatedTile(x, y, tile.oldImage, false);
            else
               return;
            mLayer->setTileCustomData(x, y, tile.oldCustomData);
            mLayer->setTileScript(x, y, tile.oldTileScript);
            mLayer->setTileFlip(x, y, tile.oldFlipX, tile.oldFlipY);
            mLayer->setTileCollisionActive(x, y, tile.oldCollision);
            if (tile.oldCollision)
               mLayer->setTileCollisionPolyCustom(x, y, tile.oldCollisionPoly.size(), tile.oldCollisionPoly.address());
         }
      }

      Con::executef( mLayer, 1, "onChanged" );
   };
   virtual void undo()
   {
      for (U32 i = 0; i < mTiles.size(); i++)
      {
         TileChange& tile = mTiles[i];
         S32 x = tile.tile.x;
         S32 y = tile.tile.y;

         if (!tile.image || !*tile.image)
            mLayer->clearTile(x, y);

         else
         {
            t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(tile.image));
            t2dAnimationDatablock* animation = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject(tile.image));
            if (imageMap)
               mLayer->setStaticTile(x, y, tile.image, tile.frame);
            else if (animation)
               mLayer->setAnimatedTile(x, y, tile.image, false);
            else
               return;

            mLayer->setTileCustomData(x, y, tile.customData);
            mLayer->setTileScript(x, y, tile.tileScript);
            mLayer->setTileFlip(x, y, tile.flipX, tile.flipY);
            mLayer->setTileCollisionActive(x, y, tile.collision);
            if (tile.collision)
               mLayer->setTileCollisionPolyCustom(x, y, tile.collisionPoly.size(), tile.collisionPoly.address());
         }
      }

      Con::executef( mLayer, 1, "onChanged" );
   };
};

#endif


#endif // TORQUE_TOOLS
