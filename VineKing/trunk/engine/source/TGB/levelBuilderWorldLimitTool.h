#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level Object Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERWORLDLIMITTOOL_H_
#define _LEVELBUILDERWORLDLIMITTOOL_H_

#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#include "TGB/levelBuilderBaseEditTool.h"
#endif
#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

class UndoFullWorldLimitAction;
class UndoWorldLimitAction;

//-----------------------------------------------------------------------------
// LevelBuilderWorldLimitTool
//-----------------------------------------------------------------------------
class LevelBuilderWorldLimitTool : public LevelBuilderBaseEditTool
{
   typedef LevelBuilderBaseEditTool Parent;
private:
   UndoFullWorldLimitAction* mUndoFullAction;
   UndoWorldLimitAction* mUndoAction;

   F32 mMouseDownAR;

protected:
   bool mMoving;

   LevelBuilderSceneWindow* mSceneWindow;
   t2dSceneObject* mSceneObject;

   t2dVector mWorldLimitPosition;
   t2dVector mWorldLimitSize;
   t2dVector mOffset;

   t2dVector mWorldLimitMinBackup;
   t2dVector mWorldLimitMaxBackup;

   ColorI mCameraOutlineColor;
   ColorI mCameraFillColor;

   RectF mCameraArea;
   F32 mCameraZoom;

   bool isEditable(t2dSceneObject* object) { return true; };

   void updateSceneObject();

public:
   LevelBuilderWorldLimitTool();
   ~LevelBuilderWorldLimitTool();
   
   virtual bool onAdd();
   virtual void onRemove();
   
   virtual bool hasUndoManager() { return true; };

	virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
   virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   // Base Tool Overrides
   virtual bool onActivate(LevelBuilderSceneWindow* sceneWindow);
   virtual void onDeactivate();
   virtual bool onAcquireObject(t2dSceneObject* object);
   virtual void onRelinquishObject(t2dSceneObject* object);

   // Object Editing
   void editObject(t2dSceneObject* object);
   // This cancels an edit, not applying any changes.
   void cancelEdit();
   // This cancels an edit, applying changes.
   void finishEdit();

   virtual bool undo() { mUndoManager.undo(); return true; };
   virtual bool redo() { mUndoManager.redo(); return true; };

   void setWorldLimitPosition(t2dVector pos) { mWorldLimitPosition = pos; };
   void setWorldLimitSize(t2dVector size) { mWorldLimitSize = size; };

   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderWorldLimitTool);
};

// This undoes the entire world limit change.
class UndoFullWorldLimitAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   t2dSceneObject::eWorldLimit mStartMode;
   t2dVector mStartMin;
   t2dVector mStartMax;
   bool mStartCallback;
   
   t2dSceneObject::eWorldLimit mEndMode;
   t2dVector mEndMin;
   t2dVector mEndMax;
   bool mEndCallback;

   t2dSceneObject* mSceneObject;

public:
   UndoFullWorldLimitAction(t2dSceneObject* sceneObject, UTF8* actionName) : UndoAction(actionName) { mSceneObject = sceneObject; deleteNotify(sceneObject); };

   void setStart(t2dSceneObject::eWorldLimit mode, t2dVector min, t2dVector max, bool callback)
   {
      mStartMode = mode;
      mStartMin = min;
      mStartMax = max;
      mStartCallback = callback;
   };

   void setFinish(t2dSceneObject::eWorldLimit mode, t2dVector min, t2dVector max, bool callback)
   {
      mEndMode = mode;
      mEndMin = min;
      mEndMax = max;
      mEndCallback = callback;
   };

   bool hasChanged()
   {
      if ((mStartMode != mEndMode) ||
          (mStartMin != mEndMin) ||
          (mStartMax != mEndMax) ||
          (mStartCallback != mEndCallback))
      {
         return true;
      }
      return false;
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mSceneObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mSceneObject->setWorldLimit(mStartMode, mStartMin, mStartMax, mStartCallback);
   };

   virtual void redo()
   {
      mSceneObject->setWorldLimit(mEndMode, mEndMin, mEndMax, mEndCallback);
   };
};

// This undoes each incremental change by the world limit tool.
class UndoWorldLimitAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   LevelBuilderWorldLimitTool* mWorldLimitTool;
   
   t2dVector mStartPosition;
   t2dVector mStartSize;
   t2dVector mFinishPosition;
   t2dVector mFinishSize;

public:
   UndoWorldLimitAction(LevelBuilderWorldLimitTool* tool, UTF8* actionName) : UndoAction(actionName) { mWorldLimitTool = tool; };

   bool hasChanged() { return !((mStartPosition == mFinishPosition) && (mStartSize == mFinishSize)); };

   void setStartBounds(t2dVector pos, t2dVector size) { mStartPosition = pos; mStartSize = size; };
   void setFinishBounds(t2dVector pos, t2dVector size) { mFinishPosition = pos; mFinishSize = size; };

   virtual void undo() { mWorldLimitTool->setWorldLimitPosition(mStartPosition); mWorldLimitTool->setWorldLimitSize(mStartSize); };
   virtual void redo() { mWorldLimitTool->setWorldLimitPosition(mFinishPosition); mWorldLimitTool->setWorldLimitSize(mFinishSize); };
};

#endif


#endif // TORQUE_TOOLS
