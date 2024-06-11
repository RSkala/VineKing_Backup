#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Window Edit Control.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderSceneWindow.h"
#include "TGB/levelBuilderSceneEdit.h"
#include "platform/event.h"

IMPLEMENT_CONOBJECT(LevelBuilderSceneWindow);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LevelBuilderSceneWindow::LevelBuilderSceneWindow() : t2dSceneWindow(),
                                                     mDragCurrent(-1, -1),
                                                     mDragPoint(-1, -1),
                                                     mSceneEdit(NULL),
                                                     mToolOverride(NULL),
                                                     mMouseDown(false),
                                                     mLayerMask(T2D_MASK_ALL), 
                                                     mGroupMask(T2D_MASK_ALL)
{
   // Default Active State.
   mActive = true;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
LevelBuilderSceneWindow::~LevelBuilderSceneWindow()
{
}

//-----------------------------------------------------------------------------
// setSceneGraph
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::setSceneGraph(t2dSceneGraph* pT2DSceneGraph)
{
   Parent::setSceneGraph(pT2DSceneGraph);

   if(getSceneGraph())
      getSceneGraph()->setIsEditorScene(true);
}

//-----------------------------------------------------------------------------
// resetSceneGraph
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::resetSceneGraph()
{
   if (getSceneGraph())
      getSceneGraph()->setIsEditorScene(false);

   Parent::resetSceneGraph();
}

//-----------------------------------------------------------------------------
// getObjectBoundsWindow
// Gets the bounding rectangle of a scene object in window space.
//-----------------------------------------------------------------------------
RectI LevelBuilderSceneWindow::getObjectBoundsWindow(const t2dSceneObject* object)
{
   RectF objLocalRect = object->getWorldClipRectangle();

   t2dVector upperLeft = t2dVector(objLocalRect.point.x, objLocalRect.point.y);
   t2dVector lowerRight = t2dVector(objLocalRect.extent.x, objLocalRect.extent.y) + upperLeft;
   t2dVector windowUpperLeft, windowLowerRight;

   sceneToWindowCoord(upperLeft, windowUpperLeft);
   sceneToWindowCoord(lowerRight, windowLowerRight);

   return RectI(S32(windowUpperLeft.mX), S32(windowUpperLeft.mY),
                S32(windowLowerRight.mX - windowUpperLeft.mX), S32(windowLowerRight.mY - windowUpperLeft.mY));
}

//-----------------------------------------------------------------------------
// setDragging
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::setDragging( const GuiEvent &event, bool initial )
{
   mDragCurrent = event.mousePoint;

   if(initial)
      mDragPoint = event.mousePoint;
}

//-----------------------------------------------------------------------------
// mouseDispatcher
// Responds to mouse events and builds a structure containing useful
// information about the mouse and the scene being edited before dispatching
// the event to the editing context.
//-----------------------------------------------------------------------------
bool LevelBuilderSceneWindow::mouseDispatcher( const GuiEvent &event, const t2dEditMouseStatus::MouseEventType eventType )
{
   // Have to have an editing context and a scene graph to be useful.
   if (!mSceneEdit || !getSceneGraph())
      return false;

   // Copy Event.
   GuiEvent localEvent = event;
   localEvent.mousePoint = globalToLocalCoord( localEvent.mousePoint );

   // Initialize Mouse Status Structure
   t2dEditMouseStatus mouseStatus;

   // Fill Generic Info.
   mouseStatus.event             = localEvent;
   mouseStatus.type              = eventType;

   // Last Mouse Point Info
   mouseStatus.lastMousePoint    = mLastMouseStatus.event.mousePoint;
   mouseStatus.lastMousePoint2D  = mLastMouseStatus.mousePoint2D;

   // Mouse Point 2D
   t2dVector scenePoint;
   windowToSceneCoord( t2dVector( F32(localEvent.mousePoint.x), F32(localEvent.mousePoint.y) ), scenePoint );
   mouseStatus.mousePoint2D = scenePoint;

   // Don't pick invisible objects.
   U32 renderLayers = getRenderLayerMask();
   U32 renderGroups = getRenderGroupMask();

   // Generate T2D Pick List and Quick Reference Object
   if ( getSceneGraph()->pickPoint( scenePoint, mGroupMask & renderGroups, mLayerMask & renderLayers, false, NULL, true ) > 0 )
   {
      mouseStatus.pickList       = getSceneGraph()->getPickList();
      mouseStatus.firstPickPoint = mouseStatus.pickList[0];
   }

   // Generate Mouse Dragging Information
   if( isDragging() )
   {

      //-----------------------------------------------------------------------------
      // Generate GUI Coordinate Mouse Information
      //-----------------------------------------------------------------------------

      // Set Dragging State
      mouseStatus.dragging = true;

      // Generate Drag Rect
      mouseStatus.dragRect = RectI( mDragPoint, (mDragCurrent - mDragPoint) );

      // Generate Drag Rect that is Always Valid
      mouseStatus.dragRectNormal = mouseStatus.dragRect;
      if( mouseStatus.dragRectNormal.extent.x < 0 )
      {
         mouseStatus.dragRectNormal.extent.x = mAbs( mouseStatus.dragRectNormal.extent.x );
         mouseStatus.dragRectNormal.point.x -= mouseStatus.dragRectNormal.extent.x;
      }
      if( mouseStatus.dragRectNormal.extent.y < 0 )
      {
         mouseStatus.dragRectNormal.extent.y = mAbs( mouseStatus.dragRectNormal.extent.y );
         mouseStatus.dragRectNormal.point.y -= mouseStatus.dragRectNormal.extent.y;
      }

      //-----------------------------------------------------------------------------
      // Generate T2D Drag Rect PickList and Quick Reference Object
      //-----------------------------------------------------------------------------

      // Generate Drag Rect and Normalized Version
      t2dVector sceneDragPoint;
      t2dVector sceneCurrentPoint;
      windowToSceneCoord( globalToLocalCoord(mouseStatus.dragRect.point), sceneDragPoint );
      windowToSceneCoord( globalToLocalCoord(mouseStatus.dragRect.point + mouseStatus.dragRect.extent), sceneCurrentPoint );

      // Store Rect's
      mouseStatus.dragRect2D = RectF( sceneDragPoint.mX, sceneDragPoint.mY, sceneCurrentPoint.mX - sceneDragPoint.mX, sceneCurrentPoint.mY - sceneDragPoint.mY );
      mouseStatus.dragRectNormal2D = RectF( getMin(sceneDragPoint.mX, sceneCurrentPoint.mX), getMin(sceneDragPoint.mY, sceneCurrentPoint.mY),
                                            mFabs(sceneDragPoint.mX - sceneCurrentPoint.mX), mFabs(sceneDragPoint.mY - sceneCurrentPoint.mY));

      if (getSceneGraph()->pickRect(sceneDragPoint, sceneCurrentPoint, mGroupMask & renderGroups, mLayerMask & renderLayers, false, NULL, true ) > 0)
      {
         mouseStatus.dragPickList  = getSceneGraph()->getPickList();
         mouseStatus.firstPickRect = mouseStatus.dragPickList[0];
      }

   }

   // Save Last Mouse Status
   mLastMouseStatus = mouseStatus;

   // Dispatch to the edit context.
   if( mSceneEdit != NULL )
      return mSceneEdit->onMouseEvent(this, mouseStatus);
   
   return false;
}

//-----------------------------------------------------------------------------
// Mouse Events
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::onMouseMove(const GuiEvent &event)
{
   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_MOVE );
   if( !bHandled )
      Parent::onMouseMove( event );
}

void LevelBuilderSceneWindow::onMouseDown(const GuiEvent &event)
{
   mMouseDown = true;
   // Unlock Mouse.
   mouseLock(this);

   //make us first responder
   setFirstResponder();

   // Set Drag Points
   setDragging( event, true );

   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_DOWN );
   if( !bHandled )
      Parent::onMouseDown( event );
}

void LevelBuilderSceneWindow::onMouseDragged(const GuiEvent &event)
{
   if (!mMouseDown)
      return;

   // Set Drag Points
   setDragging( event );

   if( !mouseDispatcher( event, t2dEditMouseStatus::TYPE_DRAGGED ) )
      Parent::onMouseDragged( event );
}

void LevelBuilderSceneWindow::onMouseUp(const GuiEvent &event)
{
   if (!mMouseDown)
      return;

   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_UP );
   if( !bHandled )
      Parent::onMouseUp( event );

   // Reset Drag Points
   resetDragging();

   // Lock Mouse.
   mouseUnlock();
   mMouseDown = false;
}

void LevelBuilderSceneWindow::onRightMouseDown(const GuiEvent &event)
{
   //make us first responder
   setFirstResponder();

   // Set Drag Points
   setDragging( event, true );

   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_RIGHT_DOWN );
   if( !bHandled )
      Parent::onRightMouseDown( event );
}

void LevelBuilderSceneWindow::onRightMouseDragged(const GuiEvent &event)
{
   // Set Drag Points
   setDragging( event );

   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_RIGHT_DRAGGED );
   if( !bHandled )
      Parent::onRightMouseDragged( event );
}

void LevelBuilderSceneWindow::onRightMouseUp(const GuiEvent &event)
{
   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_RIGHT_UP );
   if( !bHandled )
      Parent::onRightMouseUp( event );

   // Reset Drag Points
   resetDragging();
}

void LevelBuilderSceneWindow::onMiddleMouseDown(const GuiEvent &event)
{
   //make us first responder
   setFirstResponder();

   // Set Drag Points
   setDragging( event, true );

   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_MIDDLE_DOWN );
   if( !bHandled )
      Parent::onMiddleMouseDown( event );
}

void LevelBuilderSceneWindow::onMiddleMouseDragged(const GuiEvent &event)
{
   // Set Drag Points
   setDragging( event );

   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_MIDDLE_DRAGGED );
   if( !bHandled )
      Parent::onMiddleMouseDragged( event );
}

void LevelBuilderSceneWindow::onMiddleMouseUp(const GuiEvent &event)
{
   if( !mouseDispatcher( event, t2dEditMouseStatus::TYPE_MIDDLE_UP ) )
      Parent::onMiddleMouseUp( event );

   // Reset Drag Points
   resetDragging();
}

bool LevelBuilderSceneWindow::onMouseWheelUp(const GuiEvent &event)
{
   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_WHEEL_UP );
   if( !bHandled )
      Parent::onMouseWheelUp( event );

   return true;
}

bool LevelBuilderSceneWindow::onMouseWheelDown(const GuiEvent &event)
{
   bool bHandled = mouseDispatcher( event, t2dEditMouseStatus::TYPE_WHEEL_DOWN );
   if( !bHandled )
      Parent::onMouseWheelDown( event );

   return true;
}

//-----------------------------------------------------------------------------
// Key Events
//-----------------------------------------------------------------------------
bool LevelBuilderSceneWindow::onKeyDown(const GuiEvent &event)
{
   GuiEvent localEvent = event;
   localEvent.mousePoint = globalToLocalCoord( localEvent.mousePoint );

   bool ret = false;
   if (mSceneEdit)
      ret = mSceneEdit->onKeyDown(this, localEvent);

   if( !ret )
      ret = Parent::onKeyDown( event );

   return ret;
}

bool LevelBuilderSceneWindow::onKeyRepeat(const GuiEvent &event)
{
   GuiEvent localEvent = event;
   localEvent.mousePoint = globalToLocalCoord( localEvent.mousePoint );

   if (mSceneEdit)
      mSceneEdit->onKeyRepeat(this, localEvent);

   // Don't call parent! We don't want more key down messages.
   return false;
}

bool LevelBuilderSceneWindow::onKeyUp(const GuiEvent &event)
{
   GuiEvent localEvent = event;
   localEvent.mousePoint = globalToLocalCoord( localEvent.mousePoint );

   bool ret = false;
   if (mSceneEdit)
      ret = mSceneEdit->onKeyUp(this, localEvent);

   if( !ret )
   {
      // Escape and Return cancel a tool
      if( ( event.keyCode == KEY_ESCAPE || event.keyCode == KEY_RETURN ) && mSceneEdit && isMethod( "onEscapeTool" ) )
         Con::executef( this, 2, "onEscapeTool" );

      ret = Parent::onKeyUp(event);
   }

   return ret;
}

//-----------------------------------------------------------------------------
// Camera Positioning and Sizing
// These are redefined so the aspect ratio of the camera is maintained despite
// any odd-sizing the actual gui control may have.
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::setTargetCameraArea(const RectF& cameraWindow)
{
   Point2F centerPosition = cameraWindow.point + (cameraWindow.extent * 0.5);
   F32 width = cameraWindow.extent.x;
   F32 height = cameraWindow.extent.y;

   setTargetCameraPosition(centerPosition, width, height);
}

void LevelBuilderSceneWindow::setTargetCameraPosition(t2dVector centerPosition, F32 width, F32 height)
{
   F32 windowAR = (F32)getExtent().x / (F32)getExtent().y;
   F32 sceneAR = width / height;
   
   if (sceneAR > windowAR)
      height *= (sceneAR / windowAR);
   else
      width *= (windowAR / sceneAR);
      
   Parent::setTargetCameraPosition(centerPosition, width, height);
}

void LevelBuilderSceneWindow::setCurrentCameraArea(const RectF& cameraWindow)
{
   Point2F centerPosition = cameraWindow.point + (cameraWindow.extent * 0.5);
   F32 width = cameraWindow.extent.x;
   F32 height = cameraWindow.extent.y;

   setCurrentCameraPosition(centerPosition, width, height);
}

void LevelBuilderSceneWindow::setCurrentCameraPosition(t2dVector centerPosition, F32 width, F32 height)
{
   F32 windowAR = (F32)getExtent().x / (F32)getExtent().y;
   F32 sceneAR = width / height;
   
   if (sceneAR > windowAR)
      height *= (sceneAR / windowAR);
   else
      width *= (windowAR / sceneAR);
      
   Parent::setCurrentCameraPosition(centerPosition, width, height);
}

void LevelBuilderSceneWindow::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   F32 width = getCurrentCameraArea().extent.x;
   F32 height = getCurrentCameraArea().extent.y;

   F32 windowAR = (F32)newExtent.x / (F32)newExtent.y;
   F32 sceneAR = width / height;

   Point2I oldExtent = getExtent();

   // If width and height are changing, then this is a res change.
   if ((oldExtent.x != newExtent.x) && (oldExtent.y != newExtent.y))
   {
      width *= (sceneAR = windowAR);
      height *= (windowAR / sceneAR);
   }

   else if (oldExtent.x != newExtent.x)
      width *= (windowAR / sceneAR);

   else if (oldExtent.y != newExtent.y)
      height *= (sceneAR / windowAR);

   Parent::resize(newPosition, newExtent);
   Parent::setCurrentCameraPosition(getCurrentCameraPosition(), width, height);
}

//-----------------------------------------------------------------------------
// onRender
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::onRender(Point2I offset, const RectI &updateRect)
{
   RectI oldViewport;
   dglGetViewport(&oldViewport);
   dglSetViewport(updateRect);

   // Just pass everything off to the editing context.
   if (mSceneEdit)
      mSceneEdit->onRenderBackground(this);

   // Let the parent do its stuff.
   Parent::onRender(offset, updateRect);

   if (mSceneEdit)
      mSceneEdit->onRenderForeground(this);

   dglSetViewport(oldViewport);
}

//-----------------------------------------------------------------------------
// setSceneEdit
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneWindow, setSceneEdit, void, 3, 3, "(toolManager) - Sets the tool manager for this edit window)")
{
   object->setSceneEdit(dynamic_cast<LevelBuilderSceneEdit*>(Sim::findObject(argv[2])));
}

void LevelBuilderSceneWindow::setSceneEdit(LevelBuilderSceneEdit* sceneEdit)
{
   mSceneEdit = sceneEdit;
   mSceneEdit->setLastWindow(this);
}

//-----------------------------------------------------------------------------
// getSceneEdit
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneWindow, getSceneEdit, S32, 2, 2, "() - Gets the tool manager for this edit window)")
{
   LevelBuilderSceneEdit* sceneEdit = object->getSceneEdit();
   return sceneEdit != NULL ? sceneEdit->getId() : 0;
}

//-----------------------------------------------------------------------------
// setToolOverride
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderSceneWindow, setToolOverride, bool, 3, 3, "() - Sets a tool override for this edit window)")
{
   return object->setToolOverride(dynamic_cast<LevelBuilderBaseTool*>(Sim::findObject(argv[2])));
}

bool LevelBuilderSceneWindow::setToolOverride(LevelBuilderBaseTool* tool)
{
   if (mToolOverride)
      mToolOverride->onDeactivate();

   mToolOverride = tool;
   if (tool)
   {
      if (!tool->onActivate(this))
      {
         mToolOverride = NULL;
         return false;
      }
   }

   return true;
}

ConsoleMethod(LevelBuilderSceneWindow, clearToolOverride, void, 2, 2, "() - Clears the tool override on this window")
{
   object->setToolOverride(NULL);
}

//-----------------------------------------------------------------------------
// Layer/Group Picking
//-----------------------------------------------------------------------------
void LevelBuilderSceneWindow::setLayerMask(const U32 layerMask)
{
   mLayerMask = layerMask;
}

void LevelBuilderSceneWindow::setGroupMask(const U32 groupMask)
{
   mGroupMask = groupMask;
}

U32 LevelBuilderSceneWindow::getLayerMask()
{
   return mLayerMask;
}

U32 LevelBuilderSceneWindow::getGroupMask()
{
   return mGroupMask;
}

ConsoleMethod(LevelBuilderSceneWindow, setLayerMask, void, 3, 3, "() - Sets the layer mask for picking")
{
   object->setLayerMask(dAtoi(argv[2]));
}

ConsoleMethod(LevelBuilderSceneWindow, setGroupMask, void, 3, 3, "() - Sets the group mask for picking")
{
   object->setGroupMask(dAtoi(argv[2]));
}

ConsoleMethod(LevelBuilderSceneWindow, getLayerMask, S32, 2, 2, "() - Gets the layer mask for picking")
{
   return object->getLayerMask();
}

ConsoleMethod(LevelBuilderSceneWindow, getGroupMask, S32, 2, 2, "() - Gets the group mask for picking")
{
   return object->getGroupMask();
}

#endif // TORQUE_TOOLS
