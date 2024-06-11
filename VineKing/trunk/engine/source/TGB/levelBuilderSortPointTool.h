#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// SceneObject Link Point Tool.
//-----------------------------------------------------------------------------
#ifndef _LevelBuilderSortPointTool_H_
#define _LevelBuilderSortPointTool_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _LEVELBUILDERBASETOOL_H_
#include "TGB/levelBuilderBaseTool.h"
#endif

#ifndef _LEVELBUILDERSCENEEDIT_H_
#include "TGB/levelBuilderSceneEdit.h"
#endif

class UndoSortPointMoveAction;

//-----------------------------------------------------------------------------
// LevelBuilderSortPointTool
//-----------------------------------------------------------------------------
class LevelBuilderSortPointTool : public LevelBuilderBaseTool
{
   typedef LevelBuilderBaseTool Parent;

private:
   RectF mCameraArea;
   F32 mCameraZoom;
   F32 mRotation;

   UndoSortPointMoveAction* mUndoAction;
   bool mAddUndo;

protected:
   t2dSceneObject* mSceneObject;
   LevelBuilderSceneWindow* mSceneWindow;

   bool isEditable(t2dSceneObject* obj);
   Point2I getMountPointWorld(LevelBuilderSceneWindow* sceneWindow, const t2dSceneObject *obj, Point2F oneToOnePoint) const;
   Point2F getMountPointObject(LevelBuilderSceneWindow* sceneWindow, const t2dSceneObject *obj, const Point2I &worldPoint) const;

public:
   LevelBuilderSortPointTool();
   ~LevelBuilderSortPointTool();
   
   // Base Tool Overrides
   bool onActivate(LevelBuilderSceneWindow* sceneWindow);
   void onDeactivate();
   bool onAcquireObject(t2dSceneObject* object);
   void onRelinquishObject(t2dSceneObject* object);
   
   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   // Object Editing
   void editObject(t2dSceneObject* object);
   // This cancels an edit, applying changes.
   void finishEdit();

   virtual bool onMouseMove( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
   virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
   virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
   virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   DECLARE_CONOBJECT(LevelBuilderSortPointTool);
};

class UndoSortPointMoveAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;
   t2dSceneObject* mObject;
   
   t2dVector mStartPosition;
   t2dVector mEndPosition;

public:
   UndoSortPointMoveAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dSceneObject* object, t2dVector position) { mObject = object; mStartPosition = position; deleteNotify(object); };
   void setEndPosition(t2dVector position) { mEndPosition = position; };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   bool isChanged()
   {
      if( mEndPosition == mStartPosition )
         return false;
      
      return true;
   }

   virtual void undo()
   {
      mObject->setSortPoint(mStartPosition);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };
   virtual void redo()
   {
      mObject->setSortPoint(mEndPosition);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };
};

#endif


#endif // TORQUE_TOOLS
