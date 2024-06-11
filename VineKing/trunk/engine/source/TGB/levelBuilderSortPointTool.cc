#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// SceneObject Link Point Tool.
//-----------------------------------------------------------------------------
#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderSortPointTool.h"
#include "TGB/levelBuilderSceneEdit.h"
#include "TGB/levelBuilderMountTool.h"

IMPLEMENT_CONOBJECT(LevelBuilderSortPointTool);

LevelBuilderSortPointTool::LevelBuilderSortPointTool() : LevelBuilderBaseTool(),
                                                         mCameraArea(0.0f, 0.0f, 0.0f, 0.0f),
                                                         mRotation(0.0f),
                                                         mSceneWindow(NULL),
                                                         mSceneObject(NULL)
{
   mNutSize = 8;
   // Set our tool name
   mToolName = StringTable->insert("Sort Point Tool");
}

LevelBuilderSortPointTool::~LevelBuilderSortPointTool()
{
}

bool LevelBuilderSortPointTool::onActivate(LevelBuilderSceneWindow* sceneWindow)
{
   if (!Parent::onActivate(sceneWindow))
      return false;

   mSceneObject = NULL;
   mSceneWindow = sceneWindow;

   return true;
}

void LevelBuilderSortPointTool::onDeactivate()
{
   finishEdit();

   mSceneObject = NULL;
   mSceneWindow = NULL;
   Parent::onDeactivate();
}

bool LevelBuilderSortPointTool::onAcquireObject( t2dSceneObject *object )
{
   if(!isEditable(object) || !mSceneWindow)
      return false;

   // Parent handling 
   if(!Parent::onAcquireObject(object)) 
      return false;
   
   if (!mSceneObject || (mSceneWindow->getToolOverride() == this))
   {
      finishEdit();
      editObject(object);
   }
   
   return true;
}

void LevelBuilderSortPointTool::onRelinquishObject( t2dSceneObject *object )
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

void LevelBuilderSortPointTool::editObject(t2dSceneObject* object)
{
   if (!mSceneWindow || !isEditable(object))
      return;

   mSceneObject = object;

   // We're going to modify some things so we can get a better view on this object
   // for poly creation stuff, so let's back up their current settings
   mCameraZoom = mSceneWindow->getCurrentCameraZoom();
   mCameraArea = mSceneWindow->getCurrentCameraArea();
   RectF newArea = object->getWorldClipRectangle();
   newArea.inset(-1, -1);
   mSceneWindow->setTargetCameraZoom(1.0f);
   mSceneWindow->setTargetCameraArea(newArea);
   mSceneWindow->startCameraMove(0.5f);

   mRotation = object->getRotation();
   object->setRotation(0.0f);
}

ConsoleMethod(LevelBuilderSortPointTool, editObject, void, 3, 3, "Selects an object for editing.")
{
   t2dSceneObject* obj = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));
   if (obj)
      object->editObject(obj);
   else
      Con::warnf("Invalid object passed to LevelBuilderSortPointTool::editObject");
}

void LevelBuilderSortPointTool::finishEdit()
{
   if (!mSceneObject || !mSceneWindow)
      return;

   mSceneObject->setRotation(mRotation);
   mSceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);

   // Reset the camera.
   mSceneWindow->setTargetCameraZoom( mCameraZoom );
   mSceneWindow->setTargetCameraArea( mCameraArea );
   mSceneWindow->startCameraMove( 0.5f );

   mSceneObject = NULL;
}

ConsoleMethod(LevelBuilderSortPointTool, finishEdit, void, 2, 2, "Applies changes and ends editing of an object.")
{
   object->finishEdit();
}

bool LevelBuilderSortPointTool::isEditable(t2dSceneObject* obj)
{
   return true;
}

void LevelBuilderSortPointTool::onRenderGraph(LevelBuilderSceneWindow* sceneWindow )
{
   Parent::onRenderGraph( sceneWindow );

   if ((mSceneWindow != sceneWindow) || !mSceneObject)
      return;

   // Draw the bounding rect.
   RectI bounds = mSceneWindow->getObjectBoundsWindow(mSceneObject);
   bounds.point = mSceneWindow->localToGlobalCoord(bounds.point);
   dglDrawRect(bounds, ColorI(255, 255, 255));

   Point2I pt = getMountPointWorld( mSceneWindow, mSceneObject, mSceneObject->getSortPoint() );
   S32 size = 8;
   drawNut( pt );

   // If this is a window's tool override, we need the window to follow the object
   // being edited.
   if ((mSceneWindow->getToolOverride() == this) && !mSceneWindow->isCameraMoving())
   {
      RectF newArea = mSceneObject->getWorldClipRectangle();
      newArea.inset(-1, -1);
      mSceneWindow->setCurrentCameraArea(newArea);
   }
}

bool LevelBuilderSortPointTool::onMouseMove( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (sceneWindow != mSceneWindow))
      return Parent::onMouseMove(sceneWindow, mouseStatus);

   return true;
}

bool LevelBuilderSortPointTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   mAddUndo = false;
   mUndoAction = NULL;

   if (!mSceneWindow || !mSceneWindow->getSceneEdit())
      return false;

   // Acquire Object
   if (!mSceneObject)
   {
      if (mouseStatus.pickList.size() == 0)
         return Parent::onMouseDown(sceneWindow, mouseStatus);

      t2dSceneObject* pObj = mouseStatus.pickList[0];

      if ((mouseStatus.event.mouseClickCount >= 2) && isEditable(pObj))
         sceneWindow->getSceneEdit()->requestAcquisition(pObj);

      return true;
   }

   mUndoAction = new UndoSortPointMoveAction(sceneWindow->getSceneEdit(), "Moved Sort Point");
   mUndoAction->setStartPosition(mSceneObject, mSceneObject->getSortPoint());
   mAddUndo = true;

   t2dVector position = getMountPointObject( mSceneWindow, mSceneObject, mSceneWindow->localToGlobalCoord(mouseStatus.event.mousePoint));
   mSceneObject->setSortPoint( position );
   
   mSceneWindow->getSceneEdit()->onObjectSpatialChanged(mSceneObject);

   return true;
}

bool LevelBuilderSortPointTool::onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow) || !mSceneWindow->getSceneEdit())
      return false;

   RectI bounds = sceneWindow->getObjectBoundsWindow(mSceneObject);

   if (bounds.pointInRect(mouseStatus.event.mousePoint))
   {
      t2dVector position = getMountPointObject( mSceneWindow, mSceneObject, mSceneWindow->localToGlobalCoord(mouseStatus.event.mousePoint));
      mSceneObject->setSortPoint( position );
      
      mSceneWindow->getSceneEdit()->onObjectSpatialChanged(mSceneObject);
   }

   return true;
}

bool LevelBuilderSortPointTool::onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow) || !mSceneWindow->getSceneEdit())
      return false;

   if (mAddUndo && mUndoAction->isChanged())
   {
      mUndoAction->setEndPosition(mSceneObject->getSortPoint());
      mUndoAction->addToManager( &mSceneWindow->getSceneEdit()->getUndoManager() );
   }
   else if (mUndoAction)
      delete mUndoAction;

   mAddUndo = false;
   mUndoAction = NULL;

   return true;
}

Point2F LevelBuilderSortPointTool::getMountPointObject(LevelBuilderSceneWindow* sceneWindow, const t2dSceneObject* obj, const Point2I& worldPoint) const 
{
   Point2I localPoint = sceneWindow->globalToLocalCoord( worldPoint );
   // Get our object's bounds window
   RectI objRect = sceneWindow->getObjectBoundsWindow( obj );

   F32 nWidthInverse  = 1.0f / (F32)objRect.extent.x;
   F32 nHeightInverse = 1.0f / (F32)objRect.extent.y;

   S32 positionY = localPoint.y - objRect.point.y;
   if( positionY < 0 || positionY > objRect.extent.y  )
   {
      return Point2F(0,0);
   }

   S32 positionX = localPoint.x - objRect.point.x;
   if( positionX < 0 || positionX > objRect.extent.x ) 
   {
      return Point2F(0,0);
   }

   return Point2F( ( (F32)positionX * nWidthInverse  * 2.0f - 1.0f ), 
                   ( (F32)positionY * nHeightInverse * 2.0f - 1.0f) );
}

Point2I LevelBuilderSortPointTool::getMountPointWorld(LevelBuilderSceneWindow* sceneWindow, const t2dSceneObject *obj, Point2F oneToOnePoint) const 
{
   // Get our object's bounds window
   RectI objRect = sceneWindow->getObjectBoundsWindow(obj);

   F32 nWidth  = (F32)objRect.extent.x;
   F32 nHeight = (F32)objRect.extent.y;

   // Validate Y
   if( oneToOnePoint.y < -1.0f || oneToOnePoint.y > 1.0f  )
   {
      return Point2I( 0, 0 );
   }

   // Validate X
   if( oneToOnePoint.x < -1.0f || oneToOnePoint.x > 1.0f ) 
   {
      return Point2I( 0, 0 );
   }

   // Calculate Local Point
   Point2I localPoint = Point2I( S32( ( ( oneToOnePoint.x + 1.0f ) * 0.5f ) * nWidth ),
                                 S32( ( ( oneToOnePoint.y + 1.0f ) * 0.5f ) * nHeight ) );

   // Have to make sure we're lined up with the object in world coordinates
   localPoint += objRect.point;

   // Convert to global and return
   return sceneWindow->localToGlobalCoord( localPoint );
}


#endif // TORQUE_TOOLS
