#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level builder editing context.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderSceneEdit.h"
#include "T2D/t2dTileMap.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderSceneEdit);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LevelBuilderSceneEdit::LevelBuilderSceneEdit() : SimObject(),
                                                 mGridSnapX(5.0f),
                                                 mGridSnapY(5.0f),
                                                 mGridVisible(true),
                                                 mSnapToGridX(true),
                                                 mSnapToGridY(true),
                                                 mGridColor(0.0f, 0.0f, 0.5f),
                                                 mFillColor(0.3f, 0.3f, 0.3f),
                                                 mSnapThreshold(1.0f),
                                                 mActiveTool(NULL),
                                                 mAcquiredGroup(NULL),
                                                 mLastWindow(NULL),
                                                 mCameraPosition(0.0f, 0.0f),
                                                 mCameraZoom(0.0f),
                                                 mMousePosition(0.0f, 0.0f),
                                                 mDesignResolution(800, 600),
                                                 mCameraVisible(true),
                                                 mGuidesVisible(true)
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
LevelBuilderSceneEdit::~LevelBuilderSceneEdit()
{
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool LevelBuilderSceneEdit::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // Register the acquired object set.
   mAcquiredObjects.registerObject();
   mAcquiredObjects.setModDynamicFields(true);
   mAcquiredObjects.setModStaticFields(true);

   mUndoManager.registerObject("LevelBuilderUndoManager");

   return true;
}

//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::onRemove()
{
   mUndoManager.unregisterObject();
   mAcquiredObjects.unregisterObject();

   Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Input Events
//-----------------------------------------------------------------------------
bool LevelBuilderSceneEdit::onMouseEvent(LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus& mouseStatus)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onMouseEvent);

   mLastWindow = sceneWindow;

   // Set State Info
   mMousePosition = mouseStatus.mousePoint2D;
   mCameraPosition = sceneWindow->getCurrentCameraPosition();
   mCameraZoom = sceneWindow->getCurrentCameraZoom();

   // Check for a tool override from the window.
   LevelBuilderBaseTool* tool = sceneWindow->getToolOverride();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return false;

   // Did the tool handle the event? If so we'll not let the scenegraph edit act on it.
   bool bHandled = false;

   switch(mouseStatus.type)
   {
   case t2dEditMouseStatus::TYPE_DOWN:
      bHandled = tool->onMouseDown( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_UP:
      bHandled = tool->onMouseUp( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_DRAGGED:
      bHandled = tool->onMouseDragged( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_MOVE:
      bHandled = tool->onMouseMove( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_RIGHT_DOWN:
      bHandled = tool->onRightMouseDown( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_RIGHT_UP:
      bHandled = tool->onRightMouseUp( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_RIGHT_DRAGGED:
      bHandled = tool->onRightMouseDragged( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_MIDDLE_DOWN:
      bHandled = tool->onMiddleMouseDown( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_MIDDLE_UP:
      bHandled = tool->onMiddleMouseUp( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_MIDDLE_DRAGGED:
      bHandled = tool->onMiddleMouseDragged( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_WHEEL_UP:
      bHandled = tool->onMouseWheelUp( sceneWindow, mouseStatus );    
      break;
   case t2dEditMouseStatus::TYPE_WHEEL_DOWN:
      bHandled = tool->onMouseWheelDown( sceneWindow, mouseStatus );    
      break;
   }

   return bHandled;
}

bool LevelBuilderSceneEdit::onKeyDown(LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event)
{
   mLastWindow = sceneWindow;
   
   // Check for a tool override from the window.
   LevelBuilderBaseTool* tool = sceneWindow->getToolOverride();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return false;

   return tool->onKeyDown(sceneWindow, event);
}

bool LevelBuilderSceneEdit::onKeyRepeat(LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event)
{
   mLastWindow = sceneWindow;
   
   // Check for a tool override from the window.
   LevelBuilderBaseTool* tool = sceneWindow->getToolOverride();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return true;

   return tool->onKeyRepeat(sceneWindow, event);
}

bool LevelBuilderSceneEdit::onKeyUp(LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event)
{
   mLastWindow = sceneWindow;
   
   // Check for a tool override from the window.
   LevelBuilderBaseTool* tool = sceneWindow->getToolOverride();
   if (!tool)
      tool = mActiveTool;

   if (!tool)
      return true;
   
   return tool->onKeyUp(sceneWindow, event);
}

//-----------------------------------------------------------------------------
// onRenderBackground
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::onRenderBackground(LevelBuilderSceneWindow* sceneWindow)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onRenderBackground);

   // Draw Background
   dglDrawRectFill(RectI(sceneWindow->localToGlobalCoord(Point2I(0, 0)), sceneWindow->getExtent()), mFillColor);

   Point2F	cameraStart = sceneWindow->getCurrentCamera().mSceneMin;
   Point2F	cameraEnd   = sceneWindow->getCurrentCamera().mSceneMax;

   t2dVector windowWorldMin, windowWorldMax;
   sceneWindow->sceneToWindowCoord((t2dVector)cameraStart, windowWorldMin);
   sceneWindow->sceneToWindowCoord((t2dVector)cameraEnd, windowWorldMax);

   windowWorldMin	=	(t2dVector)sceneWindow->localToGlobalCoord(Point2I(S32(windowWorldMin.mX), S32(windowWorldMin.mY)));
   windowWorldMax	=	(t2dVector)sceneWindow->localToGlobalCoord(Point2I(S32(windowWorldMax.mX), S32(windowWorldMax.mY)));

   // Draw Grid Lines
   if(mGridVisible)
   {
      F32 xSnapDistance = getMax(mGridSnapX,T2D_CONST_EPSILON);
      F32 ySnapDistance = getMax(mGridSnapY,T2D_CONST_EPSILON);

      S32 maximumGridLines = 64;

      Point2F cameraExtents = cameraEnd - cameraStart;
      while((cameraExtents.x / xSnapDistance) > maximumGridLines)
      {
         xSnapDistance *= 2.0f;
         ySnapDistance *= 2.0f;
      }

      t2dVector currStep;
      // Draw Vertical Grid Lines
      if( isGridSnapX() )
      {
         cameraStart.x = xSnapDistance * ( (S32)( cameraStart.x / xSnapDistance ) );
         for(F32 nI = cameraStart.x; nI < cameraEnd.x; nI += xSnapDistance )
         {
            currStep.set( nI, 0 );
            sceneWindow->sceneToWindowCoord(currStep,currStep);
            currStep	= (t2dVector)sceneWindow->localToGlobalCoord( Point2I( S32(currStep.mX), S32(currStep.mY) ) );
            dglDrawLine( (S32)(currStep.mX), (S32)windowWorldMin.mY, (S32)(currStep.mX), (S32)windowWorldMax.mY, mGridColor );
         }
      }

      if( isGridSnapY() )
      {
         cameraStart.y = ySnapDistance * ( (S32)( cameraStart.y / ySnapDistance ) );
         for(F32 nI = cameraStart.y; nI < cameraEnd.y; nI += ySnapDistance )
         {
            currStep.set( 0, nI );
            sceneWindow->sceneToWindowCoord(currStep,currStep);
            currStep	=	(t2dVector)sceneWindow->localToGlobalCoord(Point2I(S32(currStep.mX),S32(currStep.mY)));
            dglDrawLine(S32(windowWorldMin.mX), S32(currStep.mY), S32(windowWorldMax.mX), S32(currStep.mY), mGridColor);
         }
      }
   }
}

//-----------------------------------------------------------------------------
// onRenderForeground
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::onRenderForeground(LevelBuilderSceneWindow* sceneWindow)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onRenderForeground);

   Point2F	cameraStart = sceneWindow->getCurrentCamera().mSceneMin;
   Point2F	cameraEnd   = sceneWindow->getCurrentCamera().mSceneMax;

   t2dVector windowWorldMin, windowWorldMax;
   sceneWindow->sceneToWindowCoord((t2dVector)cameraStart, windowWorldMin);
   sceneWindow->sceneToWindowCoord((t2dVector)cameraEnd, windowWorldMax);

   windowWorldMin	=	(t2dVector)sceneWindow->localToGlobalCoord(Point2I(S32(windowWorldMin.mX), S32(windowWorldMin.mY)));
   windowWorldMax	=	(t2dVector)sceneWindow->localToGlobalCoord(Point2I(S32(windowWorldMax.mX), S32(windowWorldMax.mY)));

   S32 lineThickness = 2;
   ColorF lineColor = mGridColor * 0.5f;
   lineColor.alpha = 0.8f;

   // Draw the camera outline if it is desired.
   if (mCameraVisible)
   {
      t2dVector cameraSize = t2dVector(100.0f, 75.0f);
      t2dVector cameraPosition = t2dVector(0.0f, 0.0f);

      // Always check return values of () before referencing them.
      if( sceneWindow->getSceneGraph() != NULL )
      {
         const char* pos = sceneWindow->getSceneGraph()->getDataField(StringTable->insert("cameraPosition"), NULL);
         if (t2dSceneObject::getStringElementCount(pos) == 2)
            cameraPosition = t2dSceneObject::getStringElementVector(pos);

         const char* size = sceneWindow->getSceneGraph()->getDataField(StringTable->insert("cameraSize"), NULL);
         if (t2dSceneObject::getStringElementCount(size) == 2)
            cameraSize = t2dSceneObject::getStringElementVector(size);
      }

      t2dVector upperLeft = cameraPosition - (cameraSize * 0.5f);
      t2dVector lowerRight = cameraPosition + (cameraSize * 0.5f);

      sceneWindow->sceneToWindowCoord(upperLeft, upperLeft);
      sceneWindow->sceneToWindowCoord(lowerRight, lowerRight);

      Point2I windowUpperLeft = sceneWindow->localToGlobalCoord(Point2I(upperLeft.mX, upperLeft.mY));
      Point2I windowLowerRight = sceneWindow->localToGlobalCoord(Point2I(lowerRight.mX, lowerRight.mY));
      S32 width = windowLowerRight.x - windowUpperLeft.x;
      S32 height = windowLowerRight.y - windowUpperLeft.y;

      dglDrawRectFill(windowUpperLeft, windowUpperLeft + Point2I(lineThickness, height), lineColor);
      dglDrawRectFill(windowUpperLeft, windowUpperLeft + Point2I(width, lineThickness), lineColor);
      dglDrawRectFill(windowLowerRight, windowLowerRight - Point2I(lineThickness, height), lineColor);
      dglDrawRectFill(windowLowerRight, windowLowerRight - Point2I(width, lineThickness), lineColor);

      // Draw a 'safe area' rectangle which is 90% of the camera view.
      windowUpperLeft.x += 0.1 * width;
      windowUpperLeft.y += 0.1 * height;
      windowLowerRight.x -= 0.1 * width;
      windowLowerRight.y -= 0.1 * height;
      dglDrawRect( windowUpperLeft, windowLowerRight, lineColor );
   }

   if (mGuidesVisible)
   {
      t2dVector zeroPoint;
      sceneWindow->sceneToWindowCoord(t2dVector(0.0f, 0.0f), zeroPoint);
      Point2I windowZero = sceneWindow->localToGlobalCoord(Point2I(zeroPoint.mX, zeroPoint.mY));

      S32 halfLineThickness = lineThickness >> 1;

      // Draw Y Axis
      if ((cameraStart.x < 0.0f) && (cameraEnd.x > 0.0f))
         dglDrawRectFill(Point2I(windowZero.x - halfLineThickness, windowWorldMin.mY), Point2I(windowZero.x + halfLineThickness, windowWorldMax.mY), lineColor);

      // Draw X Axis
      if ((cameraStart.y < 0.0f) && (cameraEnd.y > 0.0f))
         dglDrawRectFill(Point2I(windowWorldMin.mX, windowZero.y - halfLineThickness), Point2I(windowWorldMax.mX, windowZero.y + halfLineThickness), lineColor);
   }

   // Check for a tool override from the window.
   LevelBuilderBaseTool* tool = sceneWindow->getToolOverride();
   if (!tool)
      tool = mActiveTool;

   // Let the active tool render.
   if (tool)
      tool->onRenderGraph(sceneWindow);
}

//-----------------------------------------------------------------------------
// isAcquired
//-----------------------------------------------------------------------------
bool LevelBuilderSceneEdit::isAcquired(const t2dSceneObject* object) const
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_isAcquired);

   for (U32 i = 0; i < mAcquiredObjects.size(); i++)
   {
      if (mAcquiredObjects.at(i) == object)
         return true;
   }
   return false;
}

bool LevelBuilderSceneEdit::isAcquired(const t2dSceneObjectGroup *group) const
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_isAcquired);

   // Every child of this group must be acquired.
   Vector<t2dSceneObject*> objects;
   group->getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      if (!isAcquired(objects[i]))
         return false;
   }

   return true;
}

bool LevelBuilderSceneEdit::isOnlyAcquired(const t2dSceneObjectGroup* group) const
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_isOnlyAcquired);

   // Every child of this group must be acquired, and nothing else.
   if (!isAcquired(group))
      return false;

   for (S32 i = 0; i < mAcquiredObjects.size(); i++)
   {
      if (!group->findChildObject(mAcquiredObjects.at(i)))
         return false;
   }

   return true;
}

//-----------------------------------------------------------------------------
// requestAcquisition
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::requestAcquisition(t2dSceneObject* obj)
{
   if (mActiveTool)
      acquireObject(obj);
}

void LevelBuilderSceneEdit::requestAcquisition(t2dSceneObjectGroup* group)
{
   if (mActiveTool)
      acquireObject(group);
}

//-----------------------------------------------------------------------------
// onObjectChanged
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::onObjectChanged()
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onObjectChanged);
   if (getAcquiredGroup())
      Con::executef(this, 2, "onObjectChanged", Con::getIntArg(getAcquiredGroup()->getId()));         
   else if (getAcquiredObjectCount() > 1)
      Con::executef(this, 2, "onObjectChanged", Con::getIntArg(getAcquiredObjects().getId()));
   else if (getAcquiredObjectCount())
      Con::executef(this, 2, "onObjectChanged", Con::getIntArg(getAcquiredObject(0)->getId()));
}

void LevelBuilderSceneEdit::onObjectSpatialChanged()
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onObjectSpatialChanged);
   if (getAcquiredGroup())
      Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(getAcquiredGroup()->getId()));         
   else if (getAcquiredObjectCount() > 1)
      Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(getAcquiredObjects().getId()));
   else if (getAcquiredObjectCount())
      Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(getAcquiredObject(0)->getId()));
}

void LevelBuilderSceneEdit::onObjectChanged(t2dSceneObject* object)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onObjectChanged_Param);
   Con::executef(this, 2, "onObjectChanged", Con::getIntArg(object->getId()));
}

void LevelBuilderSceneEdit::onObjectSpatialChanged(t2dSceneObject* object)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_onObjectSpatialChanged_Param);
   object->updateSpatialConfig();
   Con::executef(this, 2, "onObjectSpatialChanged", Con::getIntArg(object->getId()));
}

//-----------------------------------------------------------------------------
// acquireObject
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::acquireObject(t2dSceneObject* object)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_acquireObject);

   // Acquire the object.
   mAcquiredObjects.addObject(object);

   // Since we acquired a new object that isn't a sceneObjectGroup, then we
   // don't have an acquired group.
   mAcquiredGroup = NULL;

   // Notify the active tool
   if (mActiveTool && mActiveTool->onAcquireObject(object))
   {
      // Notify the script. For a single object, we'll just pass back the object, otherwise
      // we'll pass back the whole group.
      t2dSceneObjectGroup* group = object->getSceneObjectGroup();
      if (group && isOnlyAcquired(group) && (group->size() > 1))
      {
         mAcquiredGroup = group;
         Con::executef(this, 2, "onAcquireObjectSet", Con::getIntArg(group->getId()));
      }
      else if (getAcquiredObjectCount() > 1)
         Con::executef(this, 2, "onAcquireObjectSet", Con::getIntArg(getAcquiredObjects().getId()));
      else if (hasAcquiredObjects())
         Con::executef(this, 2, "onAcquireObject", Con::getIntArg(getAcquiredObject(0)->getId()));

      // And notify all the other tools.
      for (S32 i = 0; i < mTools.size(); i++)
      {
         LevelBuilderBaseTool* tool = (LevelBuilderBaseTool*)mTools[i];
         if (tool && (tool != mActiveTool))
            tool->onAcquireObject(object);
      }
   }

   // Only acquire if the active tool accepts the object.
   else
      mAcquiredObjects.removeObject(object);
}

//-----------------------------------------------------------------------------
// acquireObject
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::acquireObject(t2dSceneObjectGroup* group)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_acquireObject_group);

   Vector<t2dSceneObject*> pObjects;
   group->getSceneObjects(pObjects);

   // Each object should only be acquired if the active tool wants it.
   if (mActiveTool)
   {
      for (S32 i = 0; i < pObjects.size(); i++)
      {
         t2dSceneObject* object = pObjects[i];
         if (mActiveTool->onAcquireObject(object))
            mAcquiredObjects.addObject(object);
      }

      // Only if the tool wanted the whole group did we actually acquire the whole group.
      if (isOnlyAcquired(group))
      {
         mAcquiredGroup = group;
         Con::executef(this, 2, "onAcquireObjectSet", Con::getIntArg(group->getId()));
      }
      else
      {
         // Notify the script. For a single object, we'll just pass back the object, otherwise
         // we'll pass back the whole group.
         if (getAcquiredObjectCount() > 1)
            Con::executef(this, 2, "onAcquireObjectSet", Con::getIntArg(mAcquiredObjects.getId()));
         else if (hasAcquiredObjects())
            Con::executef(this, 2, "onAcquireObject", Con::getIntArg(getAcquiredObject(0)->getId()));
      }
   }
}

ConsoleMethod(LevelBuilderSceneEdit, acquireObject, void, 3, 3, "Acquires an object.")
{
   SimObject* pSimObject = Sim::findObject(dAtoi(argv[2]));
   if (!pSimObject)
      return;

   t2dSceneObject* pSceneObj           = dynamic_cast<t2dSceneObject*>(pSimObject);
   t2dSceneObjectGroup* pSceneObjGroup = dynamic_cast<t2dSceneObjectGroup*>(pSimObject);

   if (pSceneObj)
      object->requestAcquisition(pSceneObj);
   else if (pSceneObjGroup)
      object->requestAcquisition(pSceneObjGroup);
}

//-----------------------------------------------------------------------------
// clearAcquisition
// If an object is passed in, it is unacquired, otherwise the entire selection
// set is cleared.
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::clearAcquisition(t2dSceneObject* object)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_clearAcquisition);
   if(object)
   {
      // Notify the tools of the release.
      for (S32 i = 0; i < mTools.size(); i++)
      {
         LevelBuilderBaseTool* tool = (LevelBuilderBaseTool*)mTools[i];
         tool->onRelinquishObject(object);
      }

      // Remove the object.
      mAcquiredObjects.removeObject(object);
      Con::executef(this, 2, "onRelinquishObject", Con::getIntArg(object->getId()));
   }
   else
   {
      // Clear the entire set.
      while(mAcquiredObjects.size())
         clearAcquisition((t2dSceneObject*)mAcquiredObjects.first());
   }
}

ConsoleMethod(LevelBuilderSceneEdit, clearAcquisition, void, 2, 3, "Relinquishes acquisition of an object.")
{
   t2dSceneObject* pObject = NULL;

   if (argc == 3)
   {
      pObject = dynamic_cast<t2dSceneObject*>(Sim::findObject(dAtoi(argv[2])));

      // If an object was specified but it's invalid, just don't do anything.
      if (!pObject)
      {
         Con::warnf("LevelBuilderSceneEdit::clearAcquisition - Invalid object specified.");
         return;
      }
   }

   object->clearAcquisition(pObject);
}

//-----------------------------------------------------------------------------
// deleteAcquiredObjects
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::deleteAcquiredObjects()
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_deleteAcquiredObjects);

   UndoDeleteAction* undo = NULL;
   bool doUndo = false;
   undo = new UndoDeleteAction(this, "Delete Objects");

   while (mAcquiredObjects.size())
   {
      t2dSceneObject* object = (t2dSceneObject*)mAcquiredObjects.first();
      clearAcquisition(object);

      undo->addObject(object, true);
      doUndo = true;

      Con::executef(this, 2, "onPreObjectDeleted", Con::getIntArg( object->getId() ));

      t2dSceneObjectGroup* group = object->getSceneObjectGroup();
      moveToRecycleBin(object);
      if (group && group->empty())
         moveToRecycleBin(group);

      Con::executef(this, 2, "onPostObjectDeleted", Con::getIntArg( object->getId() ));
   }

   if (doUndo)
      undo->addToManager(&getUndoManager());
   else
      delete undo;

   Con::executef(1, "refreshTreeView");
}

ConsoleMethod(LevelBuilderSceneEdit, deleteAcquiredObjects, void, 2, 2, "Deletes all acquired objects.")
{
   object->deleteAcquiredObjects();
}

void LevelBuilderSceneEdit::moveFromRecycleBin(SimObject* object)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_moveFromRecycleBin);

   for (Vector<RecycledObject>::iterator i = mRecycleBin.begin(); i != mRecycleBin.end(); i++)
   {
      if (i->object == object)
      {
         if (i->group)
         {
            // We check for t2dSceneObejct's so that we can tag their spatials as dirty.
            // This allows them to be properly instantiated back into the scene. -JDD
            t2dSceneObject *pObject = dynamic_cast<t2dSceneObject*>( i->object );
            if( pObject )
               pObject->getParentPhysics().setSpatialDirty();

            i->group->addObject(i->object);
            Con::executef(this, 2, "onObjectResurrected", Con::getIntArg(i->object->getId()));
         }

         removeNotify( object, SimObject::Notify::DeleteNotify );
         mRecycleBin.erase_fast(i);
         break;
      }
   }
}

ConsoleMethod( LevelBuilderSceneEdit, getRecycledObjectCount, S32, 2, 2, "() - Returns the number of objects in the recycle bin." )
{
   return object->mRecycleBin.size();
}

ConsoleMethod( LevelBuilderSceneEdit, getRecycledObject, S32, 3, 3, "(int index) - Returns the recycled object at the given index." )
{
   S32 index = dAtoi( argv[2] );
   if( index < object->mRecycleBin.size() )
      return object->mRecycleBin[index].object->getId();

   return 0;
}

ConsoleMethod(LevelBuilderSceneEdit, moveFromRecycleBin, void, 3, 3, "Move an object from the recycle bin back to the scene.")
{
   SimObject* obj = Sim::findObject(argv[2]);
   if (obj)
      object->moveFromRecycleBin(obj);
}

void LevelBuilderSceneEdit::onDeleteNotify( SimObject* object )
{
   if( !isRecycled( object ) )
      return;

   for (Vector<RecycledObject>::iterator i = mRecycleBin.begin(); i != mRecycleBin.end(); i++)
   {
      if (i->object == object)
      {
         mRecycleBin.erase_fast(i);
         break;
      }
   }
}

void LevelBuilderSceneEdit::moveToRecycleBin(SimObject* object)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_moveToRecycleBin);

   t2dSceneGraph* oldSceneGraph = NULL;
   SimSet* oldGroup = NULL;
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
   t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
   if (sceneObject)
   {
      oldGroup = sceneObject->getSceneObjectGroup();
      oldSceneGraph = sceneObject->getSceneGraph();
      
      if (oldSceneGraph)
         oldSceneGraph->removeFromScene(sceneObject);

      if (oldGroup)
         oldGroup->removeObject(sceneObject);
      else
         oldGroup = oldSceneGraph;
   }

   else if (group)
   {
      oldGroup = group->getSceneObjectGroup();
      oldSceneGraph = group->getSceneGraph();
      
      if (oldSceneGraph)
         oldSceneGraph->removeFromScene(group);

      if (oldGroup)
         oldGroup->removeObject(group);
      else
         oldGroup = oldSceneGraph;
   }
   deleteNotify( object );
   mRecycleBin.push_back(RecycledObject(object, oldGroup));
}

ConsoleMethod(LevelBuilderSceneEdit, moveToRecycleBin, void, 3, 3, "Move an object from scene to the recycle bin.")
{
   SimObject* obj = Sim::findObject(argv[2]);
   if (obj)
      object->moveToRecycleBin(obj);
}

bool LevelBuilderSceneEdit::isRecycled(SimObject* object)
{
   for (Vector<RecycledObject>::iterator i = mRecycleBin.begin(); i != mRecycleBin.end(); i++)
   {
      if (i->object == object)
         return true;
   }
   return false;
}

ConsoleMethod(LevelBuilderSceneEdit, undo, void, 2, 2, "Undoes the last action")
{
   object->undo();
}

ConsoleMethod(LevelBuilderSceneEdit, redo, void, 2, 2, "Redoes the last action")
{
   object->redo();
}

void LevelBuilderSceneEdit::undo()
{
   if (!getActiveTool() || !getActiveTool()->undo())
      mUndoManager.undo();
}

void LevelBuilderSceneEdit::redo()
{
   if (!getActiveTool() || !getActiveTool()->redo())
      mUndoManager.redo();
}

//-----------------------------------------------------------------------------
// Script accessors to the acquired objects.
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneEdit, getAcquiredObjects, S32, 2, 2, "Returns the acquired object set.")
{
   return object->getAcquiredObjects().getId();
}

ConsoleMethod(LevelBuilderSceneEdit, getAcquiredObjectCount, S32, 2, 2, "Returns the acquired object count.")
{
   return object->getAcquiredObjectCount();
}

ConsoleMethod(LevelBuilderSceneEdit, getAcquiredGroup, S32, 2, 2, "Returns the acquired object group.")
{
   t2dSceneObjectGroup* group = object->getAcquiredGroup();

   if (group)
      return group->getId();
   
   return 0;
}

ConsoleMethod(LevelBuilderSceneEdit, isAcquired, bool, 3, 3, "Returns whether or not an object is acquired.")
{
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));
   if (sceneObject)
      return object->isAcquired(sceneObject);

   return false;
}

// Recursive grouping helper function.
void LevelBuilderSceneEdit::addObjectsToGroup(t2dSceneObjectGroup* checkGroup, Vector<SimObject*>& objectGroup)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_addObjectsToGroup);

   for (S32 j = 0; j < checkGroup->size(); j++)
   {
      t2dSceneObject* checkObject = dynamic_cast<t2dSceneObject*>(checkGroup->at(j));
      if (checkObject)
      {
         if (isAcquired(checkObject))
            objectGroup.push_back(checkObject);
      }
      else
      {
         t2dSceneObjectGroup* checkSubGroup = dynamic_cast<t2dSceneObjectGroup*>(checkGroup->at(j));
         if (checkSubGroup)
         {
            if (isAcquired(checkSubGroup))
               objectGroup.push_back(checkSubGroup);
            else
               addObjectsToGroup(checkSubGroup, objectGroup);
         }
      }
   }
}

bool LevelBuilderSceneEdit::containsAllAcquiredObjects(const t2dSceneObjectGroup* group) const
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_containsAllAcquireObjects);

   for (S32 i = 0; i < mAcquiredObjects.size(); i++)
   {
      if (!group->findChildObject(mAcquiredObjects.at(i)))
         return false;
   }

   return true;
}

//-----------------------------------------------------------------------------
// groupAcquiredObjects
// Puts all acquired objects into 'group'.
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::groupAcquiredObjects()
{
   // Can't group nothing.
   if (!mAcquiredObjects.size())
      return;

   //PROFILE_SCOPE(LevelBuilderSceneEdit_groupAcquiredObjects);

   t2dSceneObjectGroup* group = new t2dSceneObjectGroup();
   if (!group->registerObject())
   {
      delete group;
      return;
   }

   group->setModDynamicFields(true);
   group->setModStaticFields(true);

   UndoGroupAction* undo = new UndoGroupAction(this, group, "Group Objects");

   // The objects to group.
   Vector<SimObject*> groupObjects;
   // The groups that have already been checked.
   Vector<t2dSceneObjectGroup*> checkedGroups;

   // If an entire group is selected, We need to group it instead of the individual objects.
   for (S32 i = 0; i < mAcquiredObjects.size(); i++)
   {
      // Grab the scene Object.
      t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(mAcquiredObjects[i]);
      if (sceneObject)
      {
         // Grab the object's group.
         t2dSceneObjectGroup* checkGroup = sceneObject->getSceneObjectGroup();
         if (checkGroup)
         {
            // Make sure we haven't checked this group yet.
            bool checked = false;
            for (S32 j = 0; j < checkedGroups.size(); j++)
            {
               if ((checkedGroups[j] == checkGroup) || (checkedGroups[j]->findChildObject(checkGroup)))
               {
                  checked = true;
                  break;
               }
            }

            // We haven't checked this group, so do it.
            if (!checked)
            {
               // Group the group.
               if (isAcquired(checkGroup))
                  groupObjects.push_back(checkGroup);

               // Group the objects.
               else
                  addObjectsToGroup(checkGroup, groupObjects);
               
               // Mark the group as checked.
               checkedGroups.push_back(checkGroup);
            }
         }

         // The object isn't in a group, so just add it to the new group.
         else
            groupObjects.push_back(sceneObject);
      }
   }

   // By default, add the new group to the scenegraph, but if all objects
   // are in the same group, add the group to that.
   t2dSceneObjectGroup* parentGroup = NULL;
   t2dSceneGraph* parentGraph = NULL;
   if (groupObjects.size() > 0)
   {
      parentGraph = t2dSceneObjectGroup::getSceneGraph(groupObjects[0]);
      parentGroup = t2dSceneObjectGroup::getSceneObjectGroup(groupObjects[0]);
   }
      
   else
   {
      Con::warnf("levelBuilderSceneEdit::groupAcquiredObjects - No Objects to Group!");
      group->deleteObject();
      delete undo;
      return;
   }

   SimSet* groupToUse = parentGraph;
   // The parent group will be the lowest group in the heirarchy containing all acquired objects.
   while (parentGroup)
   {
      if (containsAllAcquiredObjects(parentGroup))
      {
         groupToUse = parentGroup;
         break;
      }

      parentGroup = parentGroup->getSceneObjectGroup();
   }

   groupToUse->addObject(group);
   for (S32 i = 0; i < groupObjects.size(); i++)
   {
      undo->addObject(groupObjects[i]);
      group->addObject(groupObjects[i]);
   }

   undo->addToManager(&getUndoManager());

   Con::executef(1, "refreshTreeView");
}

ConsoleMethod(LevelBuilderSceneEdit, groupAcquiredObjects, void, 2, 2, "Puts all acquired objects in a t2dSceneObjectGroup.")
{
   object->groupAcquiredObjects();
}

//-----------------------------------------------------------------------------
// breakApartAcquiredObjects
// Removes all selected objects from their parent group.
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::breakApartAcquiredObjects()
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_breakApartAcquiredObjects);

   UndoBreakApartAction* undo = new UndoBreakApartAction(this, "Break Apart Objects");

   Vector<t2dSceneObject*> objects;
   mAcquiredObjects.getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];
      t2dSceneObjectGroup* group = object->getSceneObjectGroup();
      if (group)
      {
         SimSet* oldGroup = group;
         SimSet* newGroup = NULL;
         group->removeObject(object);
         if (group->getSceneObjectGroup())
         {
            newGroup = group->getSceneObjectGroup();
            newGroup->addObject(object);
         }
         else
         {
            newGroup = getLastWindow()->getSceneGraph();
            getLastWindow()->getSceneGraph()->addObject(object);
         }

         undo->addObject(object, oldGroup, newGroup);

         // Groups can't have one object, so if this one does, remove it. Unless, of course that object is acquired
         // in which case we'll just let further iterations handle it.
         if (oldGroup->size() == 1)
         {
            t2dSceneObject* object = dynamic_cast<t2dSceneObject*>(oldGroup->at(0));
            if (object && !isAcquired(object))
            {
               SimSet* newGroup = NULL;
               group->removeObject(object);
               if (group->getSceneObjectGroup())
               {
                  newGroup = group->getSceneObjectGroup();
                  newGroup->addObject(object);
               }
               else
               {
                  newGroup = getLastWindow()->getSceneGraph();
                  getLastWindow()->getSceneGraph()->addObject(object);
               }

               undo->addObject(object, oldGroup, newGroup);
            }
         }

         if (oldGroup->empty())
         {
            t2dSceneObjectGroup* recycledGroup = dynamic_cast<t2dSceneObjectGroup*>(oldGroup);
            if (recycledGroup)
            {
               moveToRecycleBin(recycledGroup);
               undo->addRecycledGroup(recycledGroup);
            }
         }
      }
   }

   undo->addToManager(&getUndoManager());
}

ConsoleMethod(LevelBuilderSceneEdit, breakApartAcquiredObjects, void, 2, 2, "Removes all acquired objects from there group.")
{
   object->breakApartAcquiredObjects();
}

//-----------------------------------------------------------------------------
// updateAcquiredObjects
//-----------------------------------------------------------------------------
void LevelBuilderSceneEdit::updateAcquiredObjects()
{
   mAcquiredObjects.calculateObjectRect();
}

ConsoleMethod(LevelBuilderSceneEdit, updateAcquiredObjectSet, void, 2, 2, "Updates the t2dSceneObjectSet of acquired objects.")
{
   object->updateAcquiredObjects();
}

//-----------------------------------------------------------------------------
// getActiveTool
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneEdit, getActiveTool, S32, 2, 2, "Returns the active tool.")
{
   ToolPtr tool = object->getActiveTool();
   if (tool)
      return tool->getId();

   return 0;
}

//-----------------------------------------------------------------------------
// setActiveTool
//-----------------------------------------------------------------------------
bool LevelBuilderSceneEdit::setActiveTool(ToolPtr tool)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_setActiveTool);

   for(SimSet::iterator i = mTools.begin(); i != mTools.end(); i++)
   {
      ToolPtr iter(dynamic_cast<LevelBuilderBaseTool*>(*i));
      if ((*i) == tool)
      {
         LevelBuilderBaseTool* newActiveTool = dynamic_cast<LevelBuilderBaseTool*>(*i);
         if (newActiveTool && newActiveTool->onActivate(mLastWindow))
         {
            // Let the active tool now it's being usurped.
            if (mActiveTool)
            {
               mActiveTool->onDeactivate();
               Con::executef(this, 2, "onToolDeactivate", Con::getIntArg(mActiveTool->getId()));
            }

            mActiveTool = newActiveTool;

            if (mActiveTool->hasUndoManager())
               Con::executef(this, 2, "setHistoryUndoManager", Con::getIntArg(mActiveTool->getUndoManager().getId()));
            else
               Con::executef(this, 2, "setHistoryUndoManager", Con::getIntArg(getUndoManager().getId()));
         }
         else
            return false;

         // Notify script.
         Con::executef(this, 2, "onToolActivate", Con::getIntArg(mActiveTool->getId()));
         return true;
      }
   }

   return false;
}

ConsoleMethod(LevelBuilderSceneEdit, setActiveTool, bool, 3, 3, "Sets the active tool.")
{
   LevelBuilderBaseTool* tool = dynamic_cast<LevelBuilderBaseTool*>(Sim::findObject(argv[2]));
   if (!tool)
   {
      Con::warnf("LevelBuilderSceneEdit::setActiveTool - Unable to find tool %s.", argv[2]);
      return false;
   }

   if (!object->setActiveTool(tool))
      return false;

   return true;
}

//-----------------------------------------------------------------------------
// addTool
//-----------------------------------------------------------------------------
bool LevelBuilderSceneEdit::addTool(ToolPtr tool, bool setDefault)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_addTool);

   // Make sure we don't re-add a tool.
   for (SimSet::iterator i = mTools.begin(); i != mTools.end(); i++)
   {
      ToolPtr tPtr(dynamic_cast<LevelBuilderBaseTool*>(*i));
      if (tPtr == tool)
         return false;
   }

   mTools.addObject(tool);

   if (setDefault)
      setDefaultTool(tool);

   return true;
}

ConsoleMethod(LevelBuilderSceneEdit, addTool, bool, 3, 4, "addTool(ToolObject, default)")
{
   LevelBuilderBaseTool* tool = dynamic_cast<LevelBuilderBaseTool*>(Sim::findObject(argv[2]));
   if (!tool)
   {
      Con::warnf("LevelBuilderSceneEdit::addTool - Unable to find tool %s.", argv[2]);
      return false;
   }

   bool setDefault = false;
   if (argc > 3)
      setDefault = dAtob(argv[3]);

   if (!object->addTool(tool, setDefault))
   {
      Con::warnf("LevelBuilderSceneEdit::addTool - The tool %s already exists in this tool manager.", argv[2]);
      return false;
   }

   return true;
}

void LevelBuilderSceneEdit::setDefaultTool(ToolPtr tool)
{
   mDefaultTool = tool;
}

bool LevelBuilderSceneEdit::setDefaultToolActive()
{
   if (mDefaultTool)
      return setActiveTool(mDefaultTool);

   return false;
}

//-----------------------------------------------------------------------------
// removeTool
//-----------------------------------------------------------------------------
bool LevelBuilderSceneEdit::removeTool(ToolPtr tool)
{
   //PROFILE_SCOPE(LevelBuilderSceneEdit_removeTool);

   for(SimSet::iterator i = mTools.begin(); i != mTools.end(); i++)
   {
      if((*i) == tool)
      {
         mTools.removeObject(*i);

         if ((*i) == mActiveTool)
         {
            mActiveTool->onDeactivate();
            mActiveTool = NULL;
         }

         return true;
      }
   }

   return false;
}

ConsoleMethod( LevelBuilderSceneEdit, removeTool, bool, 3, 3, "removeTool(ToolObject)" )
{
   LevelBuilderBaseTool* tool = dynamic_cast<LevelBuilderBaseTool*>(Sim::findObject(argv[2]));
   if (!tool)
   {
      Con::warnf("LevelBuilderSceneEdit::removeTool - Unable to find tool %s.", argv[2]);
      return false;
   }

   if (!object->removeTool(tool))
   {
      Con::warnf("LevelBuilderSceneEdit::removeTool - The tool %s does not exist in this tool manager.", argv[2]);
      return false;
   }

   return true;
}

ConsoleMethod(LevelBuilderSceneEdit, setDesignResolution, void, 4, 4, "Set the design resolution.")
{
   object->setDesignResolution(Point2I(dAtoi(argv[2]), dAtoi(argv[3])));
}

ConsoleMethod(LevelBuilderSceneEdit, getDesignResolution, const char*, 2, 2, "Returns the design resolution.")
{
   Point2I res = object->getDesignResolution();

   char* resString = Con::getReturnBuffer(16);
   dSprintf(resString, 16, "%d %d", res.x, res.y);
   return resString;
}

void LevelBuilderSceneEdit::removeXGuide( F32 x )
{
   if( !hasXGuides() )
      return;

   //S32 size = mXGuides.size();
   S32 cnt = 0;
   Vector<F32>::iterator i;
   i = mXGuides.begin();
   for(cnt = 0; cnt < mXGuides.size(); cnt++ )
   {
      F32 pos = *i;
      if( mIsEqual( pos, x ) )
         break;
	  i++;
   }

   if(cnt == mXGuides.size() && cnt > 0)
	   i--;

   mXGuides.erase_fast( i );
}

void LevelBuilderSceneEdit::removeYGuide( F32 y )
{
   if( !hasYGuides() )
      return;

   S32 cnt = 0;
   Vector<F32>::iterator i;
   i = mYGuides.begin();
   for(cnt = 0; cnt < mYGuides.size(); cnt++ )
   {
      F32 pos = *i;
      if( mIsEqual( pos, y ) )
         break;
	  i++;
   }

   if(cnt == mYGuides.size() && cnt > 0)
	   i--;

   mYGuides.erase_fast( i );
}

F32 LevelBuilderSceneEdit::getClosestXGuide( F32 x )
{
   if( !hasXGuides() )
      return 0.0f;

   //PROFILE_SCOPE(LevelBuilderSceneEdit_getClosestXGuide);

   F32 closest = mXGuides[0];
   Vector<F32>::const_iterator i;
   for( i = mXGuides.begin(); i != mXGuides.end(); i++ )
   {
      F32 pos = *i;
      if( mFabs( pos - x ) < mFabs( closest - x ) )
         closest = pos;
   }

   return closest;
}

//-----------------------------------------------------------------------------
// Guide Management
//-----------------------------------------------------------------------------
ConsoleMethod( LevelBuilderSceneEdit, addXGuide, void, 3, 3, "(float x) - Adds a snap guide at the specified x position." )
{
   object->addXGuide( dAtof( argv[2] ) );
}

ConsoleMethod( LevelBuilderSceneEdit, addYGuide, void, 3, 3, "(float y) - Adds a snap guide at the specified y position." )
{
   object->addYGuide( dAtof( argv[2] ) );
}

//-----------------------------------------------------------------------------
// Guide Management
//-----------------------------------------------------------------------------
ConsoleMethod( LevelBuilderSceneEdit, removeXGuide, void, 3, 3, "(float x) - Removes the snap guide at the specified x position." )
{
   object->removeXGuide( dAtof( argv[2] ) );
}

ConsoleMethod( LevelBuilderSceneEdit, removeYGuide, void, 3, 3, "(float y) - Removes the snap guide at the specified y position." )
{
   object->removeYGuide( dAtof( argv[2] ) );
}

F32 LevelBuilderSceneEdit::getClosestYGuide( F32 y )
{
   
   if( !hasYGuides() )
      return 0.0f;

   //PROFILE_SCOPE(LevelBuilderSceneEdit_getClosestYGuide);

   F32 closest = mYGuides[0];
   Vector<F32>::const_iterator i;
   for( i = mYGuides.begin(); i != mYGuides.end(); i++ )
   {
      F32 pos = *i;
      if( mFabs( pos - y ) < mFabs( closest - y ) )
         closest = pos;
   }

   return closest;
}

//-----------------------------------------------------------------------------
// Property Accessors
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneEdit, setGridSizeX, void,3,3," sets the X snap unit for our tool, in world units, set to 0 to turn of snapping")
{
   object->setGridSnapX(dAtof(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, setGridSizeY,void,3,3," sets the Y snap unit for our tool, in world units, set to 0 to turn of snapping")
{
   object->setGridSnapY(dAtof(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, getGridSnapX,F32,2,2," returns current X snap  grid. 0 = off")
{
   return object->getGridSnapX();
}

ConsoleMethod(LevelBuilderSceneEdit, getGridSnapY,F32,2,2," returns current Y snap  grid. 0 = off")
{
   return object->getGridSnapY();
}

ConsoleMethod(LevelBuilderSceneEdit, setSnapToGrid, void, 3, 3, "Toggles Snapping.")
{
   bool snap = dAtob(argv[2]);
   object->setSnapToGridX(snap);
   object->setSnapToGridY(snap);
}

ConsoleMethod(LevelBuilderSceneEdit, getSnapToGrid, bool, 2, 2, "Gets the snap status.")
{
   return object->getSnapToGridX() && object->getSnapToGridY();
}

ConsoleMethod(LevelBuilderSceneEdit, setSnapToGridX,void,3,3," sets whether we'll snap to X axis")
{
   object->setSnapToGridX(dAtob(argv[2]));
}


ConsoleMethod(LevelBuilderSceneEdit, setSnapToGridY,void,3,3," sets whether we'll snap to Y axis")
{
   object->setSnapToGridY(dAtob(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, getSnapToGridX,bool,2,2," returns current X snap")
{
   return object->getSnapToGridX();
}

ConsoleMethod(LevelBuilderSceneEdit, getSnapToGridY,bool,2,2," returns current Y snap")
{
   return object->getSnapToGridY();
}


ConsoleMethod(LevelBuilderSceneEdit,setGridVisibility,void, 3,3, " turns grid drawing on/off")
{
   object->setGridVisibility(dAtob(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit,setCameraVisibility,void, 3,3, " turns camera drawing on/off")
{
   object->setCameraVisibility(dAtob(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit,setGuidesVisibility,void, 3,3, " turns guides drawing on/off")
{
   object->setGuidesVisibility(dAtob(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit,getGridVisibility,bool, 2,2, " turns grid drawing on/off")
{
   return object->getGridVisibility();
}

ConsoleMethod(LevelBuilderSceneEdit,getCameraVisibility,bool, 2,2, " turns grid drawing on/off")
{
   return object->getCameraVisibility();
}

ConsoleMethod(LevelBuilderSceneEdit,getGuidesVisibility,bool, 2,2, " turns grid drawing on/off")
{
   return object->getGuidesVisibility();
}

ConsoleMethod(LevelBuilderSceneEdit, setGridColor,void, 5,6,"sets the grid color for the control")
{
   //get each color element
   ColorI gridColor;
   if(argc > 5)
   {
      gridColor.set(dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]));
   }
   else
   {
      gridColor.set(dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]));
   }

   object->setGridColor( gridColor );
}

ConsoleMethod(LevelBuilderSceneEdit, getGridColor,const char*, 2,2, " returns current grid color")
{
   ColorI gridColor = object->getGridColor();
   char* buff    = Con::getReturnBuffer(16);
   dSprintf(buff,16,"%d %d %d %d",gridColor.red,gridColor.green, gridColor.blue, gridColor.alpha);
   return buff;
}


ConsoleMethod(LevelBuilderSceneEdit, setFillColor,void, 5,6,"sets the fill color for the control")
{
   //get each color element
   ColorI gridColor;
   if(argc > 5)
   {
      gridColor.set(dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]),dAtoi(argv[5]));
   }
   else
   {
      gridColor.set(dAtoi(argv[2]),dAtoi(argv[3]),dAtoi(argv[4]));
   }

   object->setFillColor( gridColor );
}

ConsoleMethod(LevelBuilderSceneEdit, getFillColor,const char*, 2,2, " returns current fill color")
{
   ColorI gridColor = object->getFillColor();
   char* buff    = Con::getReturnBuffer(16);
   dSprintf(buff,16,"%d %d %d %d",gridColor.red,gridColor.green, gridColor.blue, gridColor.alpha);
   return buff;
}

ConsoleMethod(LevelBuilderSceneEdit, setSnapThreshold, void, 3, 3, "Sets the snap threshold for snap to grid.")
{
   object->setSnapThreshold(dAtof(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, getSnapThreshold, F32, 2, 2, "Gets the snap threshold for snap to grid.")
{
   return object->getSnapThreshold();
}

ConsoleMethod(LevelBuilderSceneEdit, setRotationSnap, void, 3, 3, "Sets the snap rotation flag.")
{
   object->setRotationSnap(dAtob(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, getRotationSnap, bool, 2, 2, "Gets the snap rotation flag.")
{
   return object->getRotationSnap();
}

ConsoleMethod(LevelBuilderSceneEdit, setRotationSnapAngle, void, 3, 3, "Sets the rotation snap angle.")
{
   object->setRotationSnapAngle(dAtof(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, getRotationSnapAngle, F32, 2, 2, "Gets the rotation snap angle.")
{
   return object->getRotationSnapAngle();
}

ConsoleMethod(LevelBuilderSceneEdit, setRotationSnapThreshold, void, 3, 3, "Sets the rotation snap threshold for snap to grid.")
{
   object->setRotationSnapThreshold(dAtof(argv[2]));
}

ConsoleMethod(LevelBuilderSceneEdit, getRotationSnapThreshold, F32, 2, 2, "Gets the rotation snap threshold for snap to grid.")
{
   return object->getRotationSnapThreshold();
}

//-----------------------------------------------------------------------------
// State Info Accessors
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneEdit, getMousePosition, const char*, 2, 2, "")
{
   char* ret = Con::getReturnBuffer(32);
   dSprintf(ret, 32, "%s %s", object->getMousePosition().mX, object->getMousePosition().mY);
   return ret;
}

ConsoleMethod(LevelBuilderSceneEdit, getCameraPosition, const char*, 2, 2, "")
{
   char* ret = Con::getReturnBuffer(32);
   dSprintf(ret, 32, "%s %s", object->getCameraPosition().mX, object->getCameraPosition().mY);
   return ret;
}

ConsoleMethod(LevelBuilderSceneEdit, getCameraZoom, F32, 2, 2, "")
{
   return object->getCameraZoom();
}


//-----------------------------------------------------------------------------
// Window Accessors
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneEdit, setLastWindow, void, 3, 3, "")
{
   LevelBuilderSceneWindow* sceneWindow = dynamic_cast<LevelBuilderSceneWindow*>( Sim::findObject( argv[2] ) );
   if( !sceneWindow )
   {
      Con::printf("LevelBuilderSceneEdit::setLastWindow - Invalid Scene Window!");
      return;
   }

   object->setLastWindow( sceneWindow );
}

ConsoleMethod(LevelBuilderSceneEdit, getLastWindow, S32, 2, 2, "")
{
   LevelBuilderSceneWindow* sceneWindow = object->getLastWindow();
   return sceneWindow != NULL ? sceneWindow->getId() : 0;

}

#endif // TORQUE_TOOLS
