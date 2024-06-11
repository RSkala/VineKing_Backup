#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level builder camera placement tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERCAMERATOOL_H_
#define _LEVELBUILDERCAMERATOOL_H_

#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#include "TGB/levelBuilderBaseEditTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

class UndoCameraAction;
class UndoFullCameraAction;

//-----------------------------------------------------------------------------
// LevelBuilderCameraTool
// Provides editing functionality for the scenegraphs camera area.
//-----------------------------------------------------------------------------
class LevelBuilderCameraTool : public LevelBuilderBaseEditTool
{
private:
   UndoFullCameraAction* mUndoFullAction;
   UndoCameraAction* mUndoAction;

   F32 mMouseDownAR;

   t2dVector mStartPos;
   t2dVector mStartSize;

protected:
   // Whether or not the camera is being moved.
   bool mMoving;

   typedef LevelBuilderBaseEditTool Parent;

   // Current size and position of the camera being edited.
   t2dVector mCameraPosition;
   t2dVector mCameraSize;
   t2dVector mOffset;

   ColorI mCameraOutlineColor;
   ColorI mCameraFillColor;

   // The camera area of the editor scene window prior to editing.
   RectF mCameraArea;
   F32 mCameraZoom;

   // The scene window on which this tool is being used.
   LevelBuilderSceneWindow* mSceneWindow;
  
public:
   LevelBuilderCameraTool();
   ~LevelBuilderCameraTool();
   
   virtual bool onAdd();
   virtual void onRemove();

   virtual bool hasUndoManager() { return true; };

	virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   virtual bool onActivate(LevelBuilderSceneWindow* sceneWindow);
   virtual void onDeactivate();

   virtual bool undo() { mUndoManager.undo(); return true; };
   virtual bool redo() { mUndoManager.redo(); return true; };

   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   // State Info Accessors
   t2dVector getCameraPosition() { return mCameraPosition; };
   t2dVector getCameraSize() { return mCameraSize; };
   void setCameraPosition(t2dVector pos) { mCameraPosition = pos; };
   void setCameraSize(t2dVector size) { mCameraSize = size; };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderCameraTool);
};

// This undoes the entire camera change.
class UndoFullCameraAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   LevelBuilderCameraTool* mCameraTool;

   t2dVector mStartPosition;
   t2dVector mStartSize;
   t2dVector mFinishPosition;
   t2dVector mFinishSize;

   SimObjectPtr<t2dSceneGraph> mSceneGraph;

public:
   UndoFullCameraAction(LevelBuilderCameraTool* camera, t2dSceneGraph* sceneGraph, UTF8* actionName) : UndoAction(actionName) { mSceneGraph = sceneGraph; mCameraTool = camera; };

   void setStartBounds(t2dVector pos, t2dVector size) { mStartPosition = pos; mStartSize = size; };
   void setFinishBounds(t2dVector pos, t2dVector size) { mFinishPosition = pos; mFinishSize = size; };

   bool hasChanged() { return !((mStartPosition == mFinishPosition) && (mStartSize == mFinishSize)); };

   virtual void undo()
   {
      if (!mSceneGraph) return;
      char pos[32];
      dSprintf(pos, 32, "%f %f", mStartPosition.mX, mStartPosition.mY);
      mSceneGraph->setDataField(StringTable->insert("cameraPosition"), NULL, pos);

      char size[32];
      dSprintf(size, 32, "%f %f", mStartSize.mX, mStartSize.mY);
      mSceneGraph->setDataField(StringTable->insert("cameraSize"), NULL, size);

      Con::executef(mCameraTool, 3, "onCameraChanged", pos, size);
   };

   virtual void redo()
   {
      if (!mSceneGraph) return;
      char pos[32];
      dSprintf(pos, 32, "%f %f", mFinishPosition.mX, mFinishPosition.mY);
      mSceneGraph->setDataField(StringTable->insert("cameraPosition"), NULL, pos);

      char size[32];
      dSprintf(size, 32, "%f %f", mFinishSize.mX, mFinishSize.mY);
      mSceneGraph->setDataField(StringTable->insert("cameraSize"), NULL, size);

      Con::executef(mCameraTool, 3, "onCameraChanged", pos, size);
   };
};

// This undoes each incremental change by the camera tool.
class UndoCameraAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   LevelBuilderCameraTool* mCameraTool;
   
   t2dVector mStartPosition;
   t2dVector mStartSize;
   t2dVector mFinishPosition;
   t2dVector mFinishSize;

public:
   UndoCameraAction(LevelBuilderCameraTool* camera, UTF8* actionName) : UndoAction(actionName) { mCameraTool = camera; };

   bool hasChanged() { return !((mStartPosition == mFinishPosition) && (mStartSize == mFinishSize)); };

   void setStartBounds(t2dVector pos, t2dVector size) { mStartPosition = pos; mStartSize = size; };
   void setFinishBounds(t2dVector pos, t2dVector size) { mFinishPosition = pos; mFinishSize = size; };

   virtual void undo()
   {
      mCameraTool->setCameraPosition(mStartPosition);
      mCameraTool->setCameraSize(mStartSize);

      char position[64];
      dSprintf(position, 64, "%f %f", mStartPosition.mX, mStartPosition.mY);
      char size[64];
      dSprintf(size, 64, "%f %f", mStartSize.mX, mStartSize.mY);
      Con::executef(mCameraTool, 3, "onCameraChanged", position, size);
   };

   virtual void redo()
   {
      mCameraTool->setCameraPosition(mFinishPosition);
      mCameraTool->setCameraSize(mFinishSize);

      char position[64];
      dSprintf(position, 64, "%f %f", mFinishPosition.mX, mFinishPosition.mY);
      char size[64];
      dSprintf(size, 64, "%f %f", mFinishSize.mX, mFinishSize.mY);
      Con::executef(mCameraTool, 3, "onCameraChanged", position, size);
   };
};

#endif


#endif // TORQUE_TOOLS
