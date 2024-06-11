#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// SceneObject Link Point Tool.
//-----------------------------------------------------------------------------
#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderLinkPointTool.h"
#include "TGB/levelBuilderSceneEdit.h"
#include "TGB/levelBuilderMountTool.h"

IMPLEMENT_CONOBJECT(LevelBuilderLinkPointTool);

LevelBuilderLinkPointTool::LevelBuilderLinkPointTool() : LevelBuilderBaseTool(),
                                                         mCameraArea(0.0f, 0.0f, 0.0f, 0.0f),
                                                         mRotation(0.0f),
                                                         mMountRotation(0.0f),
                                                         mAutoMountRotation(0.0f),
                                                         mSceneWindow(NULL),
                                                         mSceneObject(NULL)
{
   mNutSize = 8;
   // Set our tool name
   mToolName = StringTable->insert("Mount Tool");
}

LevelBuilderLinkPointTool::~LevelBuilderLinkPointTool()
{
}

bool LevelBuilderLinkPointTool::onActivate(LevelBuilderSceneWindow* sceneWindow)
{
   if (!Parent::onActivate(sceneWindow))
      return false;

   mSceneObject = NULL;
   mSceneWindow = sceneWindow;

   return true;
}

void LevelBuilderLinkPointTool::onDeactivate()
{
   finishEdit();

   mSceneObject = NULL;
   mSceneWindow = NULL;
   Parent::onDeactivate();
}

bool LevelBuilderLinkPointTool::onAcquireObject( t2dSceneObject *object )
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

void LevelBuilderLinkPointTool::onRelinquishObject( t2dSceneObject *object )
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

void LevelBuilderLinkPointTool::editObject(t2dSceneObject* object)
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

ConsoleMethod(LevelBuilderLinkPointTool, editObject, void, 3, 3, "Selects an object for editing.")
{
   t2dSceneObject* obj = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));
   if (obj)
      object->editObject(obj);
   else
      Con::warnf("Invalid object passed to LevelBuilderLinkPointTool::editObject");
}

void LevelBuilderLinkPointTool::finishEdit()
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

ConsoleMethod(LevelBuilderLinkPointTool, finishEdit, void, 2, 2, "Applies changes and ends editing of an object.")
{
   object->finishEdit();
}

bool LevelBuilderLinkPointTool::isEditable(t2dSceneObject* obj)
{
   return true;
}

void LevelBuilderLinkPointTool::onRenderGraph(LevelBuilderSceneWindow* sceneWindow )
{
   Parent::onRenderGraph( sceneWindow );

   if ((mSceneWindow != sceneWindow) || !mSceneObject)
      return;

   t2dSceneObject::typeMountNodeVector& mountNodes = mSceneObject->mMountNodes;

   GuiControlProfile* profile = dynamic_cast<GuiControlProfile*>(Sim::findObject("GuiDefaultProfile"));
   AssertFatal(profile, "GuiDefaultProfile not found");

   Resource<GFont> font = profile->mFont;

   // Draw the bounding rect.
   RectI bounds = mSceneWindow->getObjectBoundsWindow(mSceneObject);
   bounds.point = mSceneWindow->localToGlobalCoord(bounds.point);
   dglDrawRect(bounds, ColorI(255, 255, 255));
   
   if( !mountNodes.empty() )
   {
      // Generate Draw Points 
      static Vector<Point2I> drawPoints;
      drawPoints.clear();
      drawPoints.reserve( mountNodes.size() + 1 );

      for ( U32 j = 0; j < mountNodes.size(); j++ )
         drawPoints.push_back( getMountPointWorld( mSceneWindow, mSceneObject, mountNodes[j].mLocalMountPosition ) );

      // Render the mount nodes.
      Vector<Point2I>::iterator i = drawPoints.begin();
      for ( U32 i = 0; i < drawPoints.size(); i++ )
      {
         Point2I pt = drawPoints[i];
         S32 size = 8;

         char number[8];
         dSprintf(number, 8, "%d", mountNodes[i].mMountID);

         drawNut( pt );
         dglSetBitmapModulation(mNutOutlineColor);
         dglDrawText(font, pt - Point2I(size >> 1, size), number);
      }
   }

   // If this is a window's tool override, we need the window to follow the object
   // being edited.
   if ((mSceneWindow->getToolOverride() == this) && !mSceneWindow->isCameraMoving())
   {
      RectF newArea = mSceneObject->getWorldClipRectangle();
      newArea.inset(-1, -1);
      mSceneWindow->setCurrentCameraArea(newArea);
   }
}

bool LevelBuilderLinkPointTool::onMouseMove( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (sceneWindow != mSceneWindow))
      return Parent::onMouseMove(sceneWindow, mouseStatus);

   return true;
}

bool LevelBuilderLinkPointTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   mAddUndo = false;
   mUndoAction = NULL;

   if (!mSceneWindow)
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

   t2dSceneObject::typeMountNodeVector& mountNodes = mSceneObject->mMountNodes;

   RectI bounds = mSceneWindow->getObjectBoundsWindow(mSceneObject);
   S32 hitNode = findMountNode( mouseStatus.event.mousePoint );
   if( hitNode != -1 )
   {
      mDraggingMountNode = hitNode;
      UndoLinkPointMoveAction* undo = new UndoLinkPointMoveAction(mSceneWindow->getSceneEdit(), "Move Link Point");
      undo->setStartPosition(mSceneObject, mSceneObject->mMountNodes[mDraggingMountNode].mMountID, mSceneObject->mMountNodes[mDraggingMountNode].mLocalMountPosition);
      mUndoAction = (UndoAction*)undo;
   }
   else if( bounds.pointInRect( mouseStatus.event.mousePoint ) )
   {
      mAddUndo = true;
      mDraggingMountNode = mountNodes.size();
      t2dVector position = getMountPointObject( mSceneWindow, mSceneObject, mSceneWindow->localToGlobalCoord(mouseStatus.event.mousePoint));
      U32 node = mSceneObject->addLinkPoint(position);
      mSceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);

      UndoLinkPointAddAction* undo = new UndoLinkPointAddAction(mSceneWindow->getSceneEdit(), "Add Link Point");
      undo->setNode(mSceneObject, node, position);
      mUndoAction = (UndoAction*)undo;
   }
   else if ((mouseStatus.event.mouseClickCount >= 2) && mSceneObject)
      finishEdit();

   return true;
}

bool LevelBuilderLinkPointTool::onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow))
      return false;

   RectI bounds = sceneWindow->getObjectBoundsWindow(mSceneObject);

   if ((mDraggingMountNode >= 0) && (mDraggingMountNode < mSceneObject->mMountNodes.size()))
   {
      if (bounds.pointInRect(mouseStatus.event.mousePoint))
      {
         mAddUndo = true;
         mSceneObject->setLinkPoint(mSceneObject->mMountNodes[mDraggingMountNode].mMountID,
                  getMountPointObject( mSceneWindow, mSceneObject, sceneWindow->localToGlobalCoord(mouseStatus.event.mousePoint) ) );
         
         mSceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);
      }
   }

   return true;
}

bool LevelBuilderLinkPointTool::onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow))
      return false;

   if (mAddUndo)
   {
      UndoLinkPointMoveAction* undoMove = dynamic_cast<UndoLinkPointMoveAction*>(mUndoAction);
      UndoLinkPointAddAction* undoAdd = dynamic_cast<UndoLinkPointAddAction*>(mUndoAction);
      if (undoMove)
      {
         undoMove->setEndPosition(mSceneObject->mMountNodes[mDraggingMountNode].mLocalMountPosition);
         undoMove->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
      }
      else if (undoAdd)
      {
         undoAdd->setNode(mSceneObject, mSceneObject->mMountNodes[mDraggingMountNode].mMountID, mSceneObject->mMountNodes[mDraggingMountNode].mLocalMountPosition);
         undoAdd->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
      }
      else if (mUndoAction)
         delete mUndoAction;
   }
   else if (mUndoAction)
      delete mUndoAction;

   mAddUndo = false;
   mUndoAction = NULL;
   mDraggingMountNode = -1;

   return true;
}

bool LevelBuilderLinkPointTool::onRightMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow))
      return false;

   if (mouseStatus.pickList.size() > 1)
   {
      t2dSceneObject* obj = mouseStatus.pickList[0];
      if (obj == mSceneObject)
         obj = mouseStatus.pickList[1];

      if (obj->getIsMounted())
      {
         t2dSceneObject* mountedTo = obj->getMountedParent();

         UndoDismountAction* undo = new UndoDismountAction(mSceneWindow->getSceneEdit(), "Dismount Object");
         undo->setMountInfo(obj, mountedTo, obj->getMountOffset(), obj->getMountForce(), obj->getMountTrackRotation(),
                            true, obj->getMountOwned(), obj->getMountInheritAttributes());

         obj->dismount();
         t2dVector position = obj->getPosition() + t2dVector(10.0f, 10.0f);
         obj->setPosition(position);
         undo->setStartPosition(position);
         undo->addToManager(&mSceneWindow->getSceneEdit()->getUndoManager());

         mSceneWindow->getSceneEdit()->onObjectChanged(obj);
         mSceneWindow->getSceneEdit()->onObjectChanged(mountedTo);
      }
   }

   else
   {
      S32 hitNut = findMountNode( mouseStatus.event.mousePoint );
      if (hitNut != -1)
      {
         UndoLinkPointRemoveAction* undo = new UndoLinkPointRemoveAction(sceneWindow->getSceneEdit(), "Remove Link Point");
         undo->setNode(mSceneObject, mSceneObject->mMountNodes[hitNut].mMountID, mSceneObject->mMountNodes[hitNut].mLocalMountPosition);

         mSceneObject->removeLinkPoint(mSceneObject->mMountNodes[hitNut].mMountID);
         mSceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);

         undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
      }
   }

   return true;
}

S32 LevelBuilderLinkPointTool::findMountNode( Point2I hitPoint )
{
   if ( !mSceneObject )
      return -1;
   
   t2dSceneObject::typeMountNodeVector& mountNodes = mSceneObject->mMountNodes;

   // Generate Draw Points 
   static Vector<Point2I> drawPoints;
   drawPoints.clear();
   drawPoints.reserve( mountNodes.size() + 1 );

   t2dSceneObject::typeMountNodeVector::iterator j = mountNodes.begin();
   for ( ; j != mountNodes.end(); j++ )
      drawPoints.push_back( mSceneWindow->globalToLocalCoord( getMountPointWorld( mSceneWindow, mSceneObject, (*j).mLocalMountPosition ) ) );

   for (S32 i = 0 ; i < drawPoints.size(); i++ )
      if( inNut (drawPoints[i], hitPoint.x, hitPoint.y) )
         return i;

   return -1;
}

Point2F LevelBuilderLinkPointTool::getMountPointObject(LevelBuilderSceneWindow* sceneWindow, const t2dSceneObject* obj, const Point2I& worldPoint) const 
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

Point2I LevelBuilderLinkPointTool::getMountPointWorld(LevelBuilderSceneWindow* sceneWindow, const t2dSceneObject *obj, Point2F oneToOnePoint) const 
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
