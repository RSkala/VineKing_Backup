#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Path editing tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERPATHEDITTOOL_H_
#define _LEVELBUILDERPATHDITTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DPATH_H_
#include "T2D/t2dPath.h"
#endif

#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#include "TGB/levelBuilderBaseEditTool.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderPathEditTool
// Provides path editing functionality.
//-----------------------------------------------------------------------------
class LevelBuilderPathEditTool : public LevelBuilderBaseEditTool
{
private:
   bool mAddUndo;
   UndoAction* mUndoAction;

protected:
   typedef LevelBuilderBaseEditTool Parent;

   LevelBuilderSceneWindow* mSceneWindow;
   t2dPath*                 mPath;
   S32                      mDraggingNode;
   S32                      mDraggingHandle;
   F32                      mStartRotation;
   t2dVector                mRotationVector;
  
public:
   LevelBuilderPathEditTool();
   ~LevelBuilderPathEditTool();
   
   virtual bool onActivate(LevelBuilderSceneWindow* sceneWindow);
   virtual void onDeactivate();

	virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onRightMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   // Object Editing
   void editObject(t2dPath* object);
   // This cancels an edit, applying changes.
   void finishEdit();

   S32 findPathNode(t2dPath* path, t2dVector position);
   S32 findClosestNode(t2dPath* path, t2dVector position);
   S32 findBezierHandle(t2dPath::PathNode& node, t2dVector position);

   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderPathEditTool);
};

class UndoPathNodeAddAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;
   t2dPath* mObject;
   U32 mNode;
   t2dVector mPosition;
   F32 mWeight;
   F32 mRotation;

public:
   UndoPathNodeAddAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setNode(t2dPath* object, U32 node, t2dVector position, F32 rotation, F32 weight)
   {
      mObject = object;
      mNode = node;
      mPosition = position;
      mRotation = rotation;
      mWeight = weight;
      deleteNotify(object);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mObject->removeNode(mNode);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };
   virtual void redo()
   {
      U32 node = mObject->addNode(mPosition, mRotation, mWeight, mNode);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };
};

class UndoPathNodeRemoveAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;
   t2dPath* mObject;
   U32 mNode;
   t2dVector mPosition;
   F32 mWeight;
   F32 mRotation;

public:
   UndoPathNodeRemoveAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setNode(t2dPath* object, U32 node, t2dVector position, F32 rotation, F32 weight)
   {
      mObject = object;
      mNode = node;
      mPosition = position;
      mRotation = rotation;
      mWeight = weight;
      deleteNotify(object);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void redo()
   {
      mObject->removeNode(mNode);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };
   virtual void undo()
   {
      U32 node = mObject->addNode(mPosition, mRotation, mWeight, mNode);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };
};

class UndoPathNodeMoveAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;
   t2dPath* mObject;
   
   U32 mNode;
   t2dVector mStartPosition;
   t2dVector mEndPosition;

public:
   UndoPathNodeMoveAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dPath* object, U32 node, t2dVector position) { mObject = object; mNode = node; mStartPosition = position; deleteNotify(object); };
   void setEndPosition(t2dVector position) { mEndPosition = position; };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      if (mObject->isValidNode(mNode))
      {
         mObject->getNode(mNode).position = mStartPosition;
         mSceneEdit->onObjectSpatialChanged(mObject);
      }
   };
   virtual void redo()
   {
      if (mObject->isValidNode(mNode))
      {
         mObject->getNode(mNode).position = mEndPosition;
         mSceneEdit->onObjectSpatialChanged(mObject);
      }
   };
};

class UndoPathNodeRotateAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;
   t2dPath* mObject;
   
   U32 mNode;
   F32 mStartRotation;
   F32 mStartWeight;
   F32 mEndRotation;
   F32 mEndWeight;

public:
   UndoPathNodeRotateAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartRotation(t2dPath* object, U32 node, F32 rotation, F32 weight) { mObject = object; mNode = node; mStartRotation = rotation; mStartWeight = weight; deleteNotify(object); };
   void setEndRotation(F32 rotation, F32 weight) { mEndRotation = rotation; mEndWeight = weight; };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      if (mObject->isValidNode(mNode))
      {
         t2dPath::PathNode& node = mObject->getNode(mNode);
         node.rotation = mStartRotation;
         node.weight = mStartWeight;
         mSceneEdit->onObjectSpatialChanged(mObject);
      }
   };
   virtual void redo()
   {
      if (mObject->isValidNode(mNode))
      {
         t2dPath::PathNode& node = mObject->getNode(mNode);
         node.rotation = mEndRotation;
         node.weight = mEndWeight;
         mSceneEdit->onObjectSpatialChanged(mObject);
      }
   };
};

#endif


#endif // TORQUE_TOOLS
