#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Window Edit Control.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERSCENEWINDOW_H_
#define _LEVELBUILDERSCENEWINDOW_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

class LevelBuilderSceneEdit;
class LevelBuilderBaseTool;

//-----------------------------------------------------------------------------
// t2dEditMouseStatus
// Stores mouse status information for the scene window.
//-----------------------------------------------------------------------------
struct t2dEditMouseStatus
{
   // Mouse Dispatcher Event Types
   enum MouseEventType
   {
      TYPE_NONE = 0,
      TYPE_DOWN,
      TYPE_UP,
      TYPE_DRAGGED,
      TYPE_MOVE,
      TYPE_RIGHT_DOWN,
      TYPE_RIGHT_UP,
      TYPE_RIGHT_DRAGGED,
      TYPE_RIGHT_MOVE,
      TYPE_MIDDLE_DOWN,
      TYPE_MIDDLE_UP,
      TYPE_MIDDLE_DRAGGED,
      TYPE_MIDDLE_MOVE,
      TYPE_WHEEL_UP,
      TYPE_WHEEL_DOWN
   };

   MouseEventType          type;            ///< The Mouse Event Type, Specified in Enums above.
   GuiEvent                event;           ///< The GuiEvent that generated this dispatch
   typeSceneObjectVector   pickList;        ///< Scene window pick list for current mouse event at the point
   Point2I                 lastMousePoint;  ///< Last Mouse Position
   t2dVector               mousePoint2D;    ///< Mouse point in t2d scene coordinates
   t2dVector               lastMousePoint2D;///< Last Mouse point in t2d scene coordinates
   bool                    dragging;        ///< Specifies if we're currently dragging
   RectF                   dragRect2D;      ///< 2D Vector Drag Rect
   RectF                   dragRectNormal2D;///< 2D Vector Drag Rect Normalised
   RectI                   dragRect;        ///< Specifies the drag rect (Anchor + Extent) if we're dragging
   RectI                   dragRectNormal;  ///< Specifies the drag rect (Anchor + Extent) if we're dragging (with always positive rect)
   typeSceneObjectVector   dragPickList;    ///< Drag pick list (Pick rect at Anchor point with Extent extent)
   t2dSceneObject*         firstPickPoint;  ///< First pick point object (if any) for quick referencing
   t2dSceneObject*         firstPickRect;   ///< First pick rect object (if any) for quick referencing

   t2dEditMouseStatus() : event()
   {
      // Default to nothing.
      type = TYPE_NONE;
      
      // Clear Pick Lists
      pickList.clear();
      dragPickList.clear();

      // Clear Drag Rect
      dragging = false;
      dragRect.set( 0,0,0,0 );

      // No 2D Mouse Point
      mousePoint2D.zero();

      // Clear First Picks
      firstPickRect  = NULL;
      firstPickPoint = NULL;
   };
};

//-----------------------------------------------------------------------------
// LevelBuilderSceneWindow
// The scene window's job is to package up input events and dispatch them to
// its editing context. It can also specify a tool override which basically
// overrides the editing context's active tool for this window only.
//-----------------------------------------------------------------------------
class LevelBuilderSceneWindow : public t2dSceneWindow
{
private:
   typedef t2dSceneWindow Parent;

   // Mouse Drag Handling
   Point2I            mDragPoint;
   Point2I            mDragCurrent;
   t2dEditMouseStatus mLastMouseStatus;
   // Picking masks
   U32                mLayerMask;
   U32                mGroupMask;

   // We can get mouse up events even if the mouse wasn't clicked on this control (like after
   // menu selections for instance). This will keep any actions from happening unless the
   // mouse was pressed on this control.
   bool               mMouseDown;

   // Input Handling Helper Functions
   void setDragging(const GuiEvent& event, bool initial = false);
   inline void resetDragging() { mDragPoint.set(-1, -1); mDragCurrent.set(-1, -1); };
   inline bool isDragging() const { return (mDragPoint != Point2I(-1, -1)); };

protected:
   // This is the current editing context that all events are dispatched to.
   LevelBuilderSceneEdit* mSceneEdit;
   // A tool override can be set which tells the editing context to disregard
   // the active tool and instead use this one.
   LevelBuilderBaseTool* mToolOverride;

public:
   LevelBuilderSceneWindow();
   virtual ~LevelBuilderSceneWindow();

   // Helper Functions
   RectI getObjectBoundsWindow(const t2dSceneObject* obj);

   // Mouse Event Dispatching
   bool mouseDispatcher(const GuiEvent& event, const t2dEditMouseStatus::MouseEventType eventType);

   // Mouse handling
   void onMouseUp(const GuiEvent& event);
   void onMouseDown(const GuiEvent& event);
   void onMouseMove(const GuiEvent& event);
   void onMouseDragged(const GuiEvent& event);
   bool onMouseWheelUp(const GuiEvent& event);
   bool onMouseWheelDown(const GuiEvent& event);
   void onRightMouseDown(const GuiEvent& event);
   void onRightMouseUp(const GuiEvent& event);
   void onRightMouseDragged(const GuiEvent& event);
   void onMiddleMouseDown(const GuiEvent& event);
   void onMiddleMouseUp(const GuiEvent& event);
   void onMiddleMouseDragged(const GuiEvent& event);

   // Key Handling
   bool onKeyDown(const GuiEvent& event);
   bool onKeyUp(const GuiEvent& event);
   bool onKeyRepeat(const GuiEvent& event);

   // Tool Management
   void setSceneEdit(LevelBuilderSceneEdit* sceneEdit);
   LevelBuilderSceneEdit* getSceneEdit()               { return mSceneEdit; };
   bool setToolOverride(LevelBuilderBaseTool* tool);
   LevelBuilderBaseTool* getToolOverride()             { return mToolOverride; };

   // t2dSceneWindow Overrides
   virtual void setSceneGraph(t2dSceneGraph* pT2DSceneGraph);
   virtual void resetSceneGraph(void);
   virtual void setTargetCameraArea(const RectF& cameraWindow);
   virtual void setTargetCameraPosition(t2dVector centerPosition, F32 width, F32 height);
   virtual void setCurrentCameraArea(const RectF& cameraWindow);
   virtual void setCurrentCameraPosition(t2dVector centerPosition, F32 width, F32 height);
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);

   // Pick Mask Handling
   void setLayerMask(const U32 layerMask);
   void setGroupMask(const U32 groupMask);
   U32 getLayerMask();
   U32 getGroupMask();

   // Render
   virtual void onRender(Point2I offset, const RectI &updateRect);
   
   DECLARE_CONOBJECT(LevelBuilderSceneWindow);
};

#endif


#endif // TORQUE_TOOLS
