#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level Object Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderWorldLimitTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderWorldLimitTool);

void LevelBuilderWorldLimitTool::updateSceneObject()
{
   // Do nothing if there is no valid scene object
   if(!mSceneObject || !mSceneWindow )
      return;
   
   const t2dVector min = mWorldLimitPosition - (mWorldLimitSize * 0.5);
   const t2dVector max = mWorldLimitPosition + (mWorldLimitSize * 0.5);

   /* This part is copied from former finishEdit() - Why aren't those modes allowed?
   // Set a default limit mode if necessary.
   t2dSceneObject::eWorldLimit limit = mSceneObject->getWorldLimitMode();
   if ((limit == t2dSceneObject::T2D_LIMIT_OFF) ||
       (limit == t2dSceneObject::T2D_LIMIT_INVALID) ||
       (limit == t2dSceneObject::T2D_LIMIT_NULL))
   {
      limit = t2dSceneObject::T2D_LIMIT_BOUNCE;
   }
   */

   t2dSceneObject::eWorldLimit limit = mSceneObject->getWorldLimitMode();
   if( limit == t2dSceneObject::T2D_LIMIT_INVALID )
      limit = t2dSceneObject::T2D_LIMIT_BOUNCE;

   mSceneObject->setWorldLimit(limit, min, max, mSceneObject->getWorldLimitCallback());

   if( mSceneWindow->getSceneEdit() )
      mSceneWindow->getSceneEdit()->onObjectSpatialChanged(mSceneObject);
}

LevelBuilderWorldLimitTool::LevelBuilderWorldLimitTool() : LevelBuilderBaseEditTool(),
                                                           mWorldLimitPosition(0.0f, 0.0f),
                                                           mWorldLimitSize(100.0f, 75.0f),
                                                           mCameraOutlineColor( 255, 255, 255, 200 ),
                                                           mCameraFillColor( 40, 40, 40, 150),
                                                           mSceneObject(NULL),
                                                           mSceneWindow(NULL),
                                                           mUndoFullAction(NULL),
                                                           mUndoAction(NULL),
                                                           mMouseDownAR(1.0f)
{
   // Set our tool name
   mToolName = StringTable->insert("World Limit Tool");
}

LevelBuilderWorldLimitTool::~LevelBuilderWorldLimitTool()
{
}

bool LevelBuilderWorldLimitTool::onAdd()
{
   if (!Parent::onAdd())
      return false;

   if (!mUndoManager.registerObject())
      return false;

   mUndoManager.setModDynamicFields(true);
   mUndoManager.setModStaticFields(true);

   return true;
}

void LevelBuilderWorldLimitTool::onRemove()
{
   mUndoManager.unregisterObject();

   Parent::onRemove();
}

bool LevelBuilderWorldLimitTool::onActivate(LevelBuilderSceneWindow* sceneWindow)
{
   if (!Parent::onActivate(sceneWindow))
      return false;

   mSceneWindow = sceneWindow;
   mSceneObject = NULL;

   return true;
}

void LevelBuilderWorldLimitTool::onDeactivate()
{
   finishEdit();

   mUndoFullAction = NULL;
   mUndoAction = NULL;

   mSceneObject = NULL;
   mSceneWindow = NULL;
   Parent::onDeactivate();
}

bool LevelBuilderWorldLimitTool::onAcquireObject(t2dSceneObject* object)
{
   if(!isEditable(object) || !mSceneWindow)
      return false;

   if (!Parent::onAcquireObject(object))
      return false;

   if (!mSceneObject || (mSceneWindow->getToolOverride() == this))
   {
      finishEdit();
      editObject(object);
   }

   return true;
}

void LevelBuilderWorldLimitTool::onRelinquishObject(t2dSceneObject* object)
{
   if(!mSceneWindow || !mSceneObject)
      return Parent::onRelinquishObject(object);

   if (object == mSceneObject)
   {
      finishEdit();

      if (mSceneWindow->getToolOverride() == this)
      {
         bool foundNewObject = false;
         // Since we're a tool override, we should try to edit any object we can.
         for (S32 i = 0; i < mSceneWindow->getSceneEdit()->getAcquiredObjectCount(); i++)
         {
            t2dSceneObject* newObject = mSceneWindow->getSceneEdit()->getAcquiredObject(i);
            if ((newObject != mSceneObject) && isEditable(newObject))
            {
               foundNewObject = true;
               editObject(newObject);
               break;
            }
         }

         if (!foundNewObject)
         {
            // Grab the size and position of the camera from the scenegraph.
            t2dVector cameraPosition = t2dVector(0.0f, 0.0f);
            t2dVector cameraSize = t2dVector(100.0f, 75.0f);
            if (mSceneWindow->getSceneGraph())
            {
               const char* pos = mSceneWindow->getSceneGraph()->getDataField(StringTable->insert("cameraPosition"), NULL);
               if (t2dSceneObject::getStringElementCount(pos) == 2)
                  cameraPosition = t2dSceneObject::getStringElementVector(pos);
               
               const char* size = mSceneWindow->getSceneGraph()->getDataField(StringTable->insert("cameraSize"), NULL);
               if (t2dSceneObject::getStringElementCount(size) == 2)
                  cameraSize = t2dSceneObject::getStringElementVector(size);
            }

            // And update the camera.
            mSceneWindow->setTargetCameraZoom( 1.0f );
            mSceneWindow->setTargetCameraPosition(cameraPosition, cameraSize.mX, cameraSize.mY);
            mSceneWindow->startCameraMove( 0.5f );
            mSceneObject = NULL;
         }
      }
   }

   // Do parent cleanup
   Parent::onRelinquishObject(object);
}

void LevelBuilderWorldLimitTool::editObject(t2dSceneObject* object)
{
   if (!mSceneWindow || !isEditable(object))
      return;

   mSceneObject = object;

   t2dSceneObject::eWorldLimit mode = object->getWorldLimitMode();
   t2dVector min = object->getWorldLimitMin();
   t2dVector max = object->getWorldLimitMax();
   bool callback = object->getWorldLimitCallback();

   // Store entry values
   mWorldLimitMinBackup = min;
   mWorldLimitMinBackup = max;

   if (RectF(min.mX, min.mY, max.mX - min.mX, max.mY - min.mY).isValidRect())
   {
      mWorldLimitPosition = min + ((max - min) * 0.5);
      mWorldLimitSize = max - min;
   }
   else
   {
      // Set to default
      mSceneObject->setDefaultWorldLimit();
      editObject( mSceneObject );
      return;
   }

   mCameraArea = mSceneWindow->getCurrentCameraArea();
   mCameraZoom = mSceneWindow->getCurrentCameraZoom();

   mSceneWindow->setTargetCameraZoom(1.0f);
   mSceneWindow->setTargetCameraPosition(mWorldLimitPosition, mWorldLimitSize.mX * 2.0f, mWorldLimitSize.mY * 2.0f);
   mSceneWindow->startCameraMove( 0.5f );

   mUndoFullAction = new UndoFullWorldLimitAction(mSceneObject, "World Limit Change");
   mUndoFullAction->setStart(mode, min, max, callback);
}

ConsoleMethod(LevelBuilderWorldLimitTool, editObject, void, 3, 3, "Selects an object for editing.")
{
   t2dSceneObject* obj = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));
   if (obj)
      object->editObject(obj);
   else
      Con::warnf("Invalid object past to LevelBuilderWorldLimitTool::editObject");
}

void LevelBuilderWorldLimitTool::cancelEdit()
{
   if (!mSceneObject || !mSceneWindow)
      return;

   // Restore world limit
   mSceneObject->setWorldLimitMin( mWorldLimitMinBackup );
   mSceneObject->setWorldLimitMax( mWorldLimitMaxBackup );

   // Reset the camera.
   mSceneWindow->setTargetCameraZoom( mCameraZoom );
   mSceneWindow->setTargetCameraArea( mCameraArea );
   mSceneWindow->startCameraMove( 0.5f );

   // Cancel the undo.
   if (mUndoFullAction)
   {
      delete mUndoFullAction;
      mUndoFullAction = NULL;
   }
   
   mUndoManager.clearAll();

   mSceneObject = NULL;
}

ConsoleMethod(LevelBuilderWorldLimitTool, cancelEdit, void, 2, 2, "Cancels editing of an object.")
{
   object->cancelEdit();
}

void LevelBuilderWorldLimitTool::finishEdit()
{
   if (!mSceneObject || !mSceneWindow)
      return;

   // Update the SceneObject
   updateSceneObject();

   // Reset the camera.
   mSceneWindow->setTargetCameraZoom( mCameraZoom );
   mSceneWindow->setTargetCameraArea( mCameraArea );
   mSceneWindow->startCameraMove( 0.5f );

   // Set the undo.
   if (mUndoFullAction)
   {
      mUndoFullAction->setFinish( mSceneObject->getWorldLimitMode(), 
                                  mSceneObject->getWorldLimitMin(), 
                                  mSceneObject->getWorldLimitMax(),
                                  mSceneObject->getWorldLimitCallback());

      if (mUndoFullAction->hasChanged())
         mUndoFullAction->addToManager(&mSceneWindow->getSceneEdit()->getUndoManager());
      else
         delete mUndoFullAction;

      mUndoFullAction = NULL;
   }
   
   mUndoManager.clearAll();

   mSceneObject = NULL;
}

ConsoleMethod(LevelBuilderWorldLimitTool, finishEdit, void, 2, 2, "Applies changes and ends editing of an object.")
{
   object->finishEdit();
}

bool LevelBuilderWorldLimitTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (sceneWindow != mSceneWindow)
      return Parent::onMouseDragged(sceneWindow, mouseStatus);

   // Acquire Object
   if (!mSceneObject)
   {
      if (mouseStatus.pickList.size() == 0)
         return Parent::onMouseDown(sceneWindow, mouseStatus);

      t2dSceneObject* pObj = mouseStatus.pickList[0];

      if (mouseStatus.event.mouseClickCount >= 2)
         sceneWindow->getSceneEdit()->requestAcquisition(pObj);

      return true;
   }

   mMouseDownAR = mWorldLimitSize.mX / mWorldLimitSize.mY;

   t2dVector upperLeft = mWorldLimitPosition - (mWorldLimitSize * 0.5);
   t2dVector lowerRight = mWorldLimitPosition + (mWorldLimitSize * 0.5);

   mSizingState = getSizingState( sceneWindow, mouseStatus.event.mousePoint, RectF(upperLeft, mWorldLimitSize));

   mMoving = false;
   if (!mSizingState)
   {
      if ((mouseStatus.mousePoint2D.mX > upperLeft.mX) && (mouseStatus.mousePoint2D.mX < lowerRight.mX) &&
          (mouseStatus.mousePoint2D.mY > upperLeft.mY) && (mouseStatus.mousePoint2D.mY < lowerRight.mY))
      {
          mMoving = true;
          mOffset = mouseStatus.mousePoint2D - (upperLeft + ((lowerRight - upperLeft) * 0.5));
      }
   }

   if ((mouseStatus.event.mouseClickCount >= 2) && !mMoving && !mSizingState)
      finishEdit();

   mUndoAction = new UndoWorldLimitAction(this, "World Limit Change");
   mUndoAction->setStartBounds(mWorldLimitPosition, mWorldLimitSize);

   return true;
}

bool LevelBuilderWorldLimitTool::onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (sceneWindow != mSceneWindow))
      return Parent::onMouseDragged(sceneWindow, mouseStatus);

   bool flipX, flipY;
   if (mSizingState)
      scale(sceneWindow->getSceneEdit(), mWorldLimitSize, mWorldLimitPosition, mouseStatus.mousePoint2D, mouseStatus.event.modifier & SI_CTRL,
            mouseStatus.event.modifier & SI_SHIFT, mMouseDownAR, mWorldLimitSize, mWorldLimitPosition, flipX, flipY);
   else if (mMoving)
      move(sceneWindow->getSceneEdit(), mWorldLimitSize, mouseStatus.mousePoint2D - mOffset, mWorldLimitPosition);

   updateSceneObject();

   return true;
}

bool LevelBuilderWorldLimitTool::onMouseUp(LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus& mouseStatus)
{
   if (!mSceneObject || (sceneWindow != mSceneWindow) || !mUndoAction )
      return Parent::onMouseUp(sceneWindow, mouseStatus);

   mUndoAction->setFinishBounds(mWorldLimitPosition, mWorldLimitSize);
   if (mUndoAction->hasChanged())
      mUndoAction->addToManager(&mUndoManager);
   else
      delete mUndoAction;

   mUndoAction = NULL;

   return false;
}

void LevelBuilderWorldLimitTool::onRenderGraph(LevelBuilderSceneWindow* sceneWindow)
{ 
   // Render Parent first
   Parent::onRenderGraph( sceneWindow );

   if (!mSceneObject || (sceneWindow != mSceneWindow))
      return;

   t2dVector upperLeft = mWorldLimitPosition - (mWorldLimitSize * 0.5);
   t2dVector lowerRight = mWorldLimitPosition + (mWorldLimitSize * 0.5);
   t2dVector windowUpperLeft, windowLowerRight;
   sceneWindow->sceneToWindowCoord(upperLeft, windowUpperLeft);
   sceneWindow->sceneToWindowCoord(lowerRight, windowLowerRight);

   Point2I offsetUpperLeft = sceneWindow->localToGlobalCoord(Point2I(S32(windowUpperLeft.mX), S32(windowUpperLeft.mY)));
   Point2I offsetLowerRight = sceneWindow->localToGlobalCoord(Point2I(S32(windowLowerRight.mX), S32(windowLowerRight.mY)));

   RectI cameraRect = RectI(offsetUpperLeft, offsetLowerRight - offsetUpperLeft);

   dglDrawRect( cameraRect, mCameraOutlineColor );
   dglDrawRectFill( cameraRect, mCameraFillColor );

   drawSizingNuts(sceneWindow, RectF(upperLeft, mWorldLimitSize));
}


#endif // TORQUE_TOOLS
