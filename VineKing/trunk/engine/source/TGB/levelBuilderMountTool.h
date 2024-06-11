#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// SceneObject Mount Tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERMOUNTTOOL_H_
#define _LEVELBUILDERMOUNTTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _T2DPATH_H_
#include "T2D/t2dPath.h"
#endif

#ifndef _LEVELBUILDERBASETOOL_H_
#include "TGB/levelBuilderBaseTool.h"
#endif

#ifndef _LEVELBUILDERSCENEEDIT_H_
#include "TGB/levelBuilderSceneEdit.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderMountTool
//-----------------------------------------------------------------------------
class LevelBuilderMountTool : public LevelBuilderBaseTool
{
   typedef LevelBuilderBaseTool Parent;

   F32 mBlendAlpha;
   t2dVector mObjectStartPosition;

protected:
   t2dSceneObject* mSceneObject;
   LevelBuilderSceneWindow* mSceneWindow;

   bool isEditable(t2dSceneObject* obj);

public:
   LevelBuilderMountTool();
   ~LevelBuilderMountTool();
   
   // Base Tool Overrides
   bool onActivate(LevelBuilderSceneWindow* sceneWindow);
   void onDeactivate();
   bool onAcquireObject(t2dSceneObject* object);
   void onRelinquishObject(t2dSceneObject* object);

   // Object Editing
   void editObject(t2dSceneObject* object);
   // This cancels an edit, applying changes.
   void finishEdit();
   
   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   // Dismounts An Object
   void dismountObject(LevelBuilderSceneEdit* sceneEdit, t2dSceneObject* object);
   void clearAllMounts(LevelBuilderSceneEdit* sceneEdit, t2dSceneObject* object);

   virtual bool onMouseMove( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
   virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
   virtual bool onRightMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   DECLARE_CONOBJECT(LevelBuilderMountTool);
};

class UndoMountAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;

   t2dVector mStartPosition;

   t2dSceneObject* mMounter;
   t2dSceneObject* mMountee;

   S32 mMounterLO;
   S32 mMounteeLO;

   t2dVector mOffset;
   F32 mForce;
   bool mTrack, mSend, mOwned, mInherit;

public:
   UndoMountAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dVector position) { mStartPosition = position; };
   void setMountInfo(t2dSceneObject* mounter, t2dSceneObject* mountee, t2dVector offset, F32 force, bool track, bool send, bool owned, bool inherit)
   {
      mMounter = mounter;
      mMountee = mountee;

      mMounterLO = mounter->getLayerOrder();
      mMounteeLO = mountee->getLayerOrder();

      mOffset = offset;
      mForce = force;
      mTrack = track;
      mSend = send;
      mOwned = owned;
      mInherit = inherit;

      deleteNotify(mounter);
      deleteNotify(mountee);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if (((object == mMounter) || (object == mMountee)) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mMounter->dismount();
      mMounter->setPosition(mStartPosition);
      mSceneEdit->onObjectChanged(mMountee);
      mSceneEdit->onObjectSpatialChanged(mMounter);
   };

   virtual void redo()
   {
      mMounter->mount(mMountee, mOffset, mForce, mTrack, mSend, mOwned, mInherit);

      // rdbnote: we need to reset these just in case either of the objects
      // was removed from the scenegraph, otherwise render order will be messed
      mMounter->setLayerOrder(mMounterLO);
      mMountee->setLayerOrder(mMounteeLO);

      mSceneEdit->onObjectChanged(mMountee);
      mSceneEdit->onObjectSpatialChanged(mMounter);
   };
};

class UndoDismountAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;

   t2dVector mStartPosition;

   t2dSceneObject* mMounter;
   t2dSceneObject* mMountee;

   S32 mMounterLO;
   S32 mMounteeLO;

   t2dVector mOffset;
   F32 mForce;
   bool mTrack, mSend, mOwned, mInherit;

public:
   UndoDismountAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dVector position) { mStartPosition = position; };
   void setMountInfo(t2dSceneObject* mounter, t2dSceneObject* mountee, t2dVector offset, F32 force, bool track, bool send, bool owned, bool inherit)
   {
      mMounter = mounter;
      mMountee = mountee;

      mMounterLO = mounter->getLayerOrder();
      mMounteeLO = mountee->getLayerOrder();

      mOffset = offset;
      mForce = force;
      mTrack = track;
      mSend = send;
      mOwned = owned;
      mInherit = inherit;

      deleteNotify(mounter);
      deleteNotify(mountee);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if (((object == mMounter) || (object == mMountee)) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mMounter->mount(mMountee, mOffset, mForce, mTrack, mSend, mOwned, mInherit);

      // rdbnote: we need to reset these just in case either of the objects
      // was removed from the scenegraph, otherwise render order will be messed
      mMounter->setLayerOrder(mMounterLO);
      mMountee->setLayerOrder(mMounteeLO);

      mSceneEdit->onObjectChanged(mMountee);
      mSceneEdit->onObjectSpatialChanged(mMounter);
   };

   virtual void redo()
   {
      mMounter->dismount();
      mMounter->setPosition(mStartPosition);
      mSceneEdit->onObjectChanged(mMountee);
      mSceneEdit->onObjectSpatialChanged(mMounter);
   };
};

// Undo Action Types
class UndoMountMoveAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   t2dSceneObject* mObject;
   t2dVector mStartPosition;
   t2dVector mEndPosition;

   // We need this so we can send notifications of objects changing.
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoMountMoveAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dSceneObject* object, t2dVector position) { mObject = object; mStartPosition = position; deleteNotify(object); };
   void setEndPosition(t2dVector position) { mEndPosition = position; };

   virtual void onDeleteNotify(SimObject* object)
   {
      if ((object == mObject) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mObject->setPosition(mStartPosition);
      mSceneEdit->onObjectSpatialChanged(mObject);
   };

   virtual void redo()
   {
      mObject->setPosition(mEndPosition);
      mSceneEdit->onObjectSpatialChanged(mObject);
   }
};

class UndoPathAttachAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;

   t2dVector mStartPosition;

   t2dSceneObject* mMounter;
   t2dPath* mPath;
   F32 mSpeed;
   S32 mDirection;
   bool mOrientToPath;
   S32 mStartNode, mEndNode;
   ePathMode mPathMode;
   S32 mLoops;

public:
   UndoPathAttachAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dVector position) { mStartPosition = position; };
   void setMountInfo(t2dSceneObject* mounter, t2dPath* path)
   {
      mMounter = mounter;
      mPath = path;

      mSpeed = mPath->getSpeed(mMounter);
      mDirection = mPath->getDirection(mMounter);
      mOrientToPath = mPath->getOrient(mMounter);
      mStartNode = mPath->getStartNode(mMounter);
      mEndNode = mPath->getEndNode(mMounter);
      mPathMode = mPath->getFollowMode(mMounter);
      mLoops = mPath->getLoops(mMounter);

      deleteNotify(mounter);
      deleteNotify(path);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if (((object == mMounter) || (object == mPath)) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mPath->detachObject(mMounter);
      mMounter->setPosition(mStartPosition);
      mSceneEdit->onObjectChanged(mMounter);
      mSceneEdit->onObjectChanged(mPath);
   };

   virtual void redo()
   {
      mPath->attachObject(mMounter, mSpeed, mDirection, mOrientToPath, mStartNode, mEndNode, mPathMode, mLoops, true);
      mSceneEdit->onObjectChanged(mPath);
      mSceneEdit->onObjectChanged(mMounter);
   };
};

class UndoPathDetachAction : public UndoAction
{
private:
   LevelBuilderSceneEdit* mSceneEdit;

   t2dVector mStartPosition;

   t2dSceneObject* mMounter;
   t2dPath* mPath;
   F32 mSpeed;
   S32 mDirection;
   bool mOrientToPath;
   S32 mStartNode, mEndNode;
   ePathMode mPathMode;
   S32 mLoops;

public:
   UndoPathDetachAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName) { mSceneEdit = sceneEdit; };

   void setStartPosition(t2dVector position) { mStartPosition = position; };
   void setMountInfo(t2dSceneObject* mounter, t2dPath* path)
   {
      mMounter = mounter;
      mPath = path;

      mSpeed = mPath->getSpeed(mMounter);
      mDirection = mPath->getDirection(mMounter);
      mOrientToPath = mPath->getOrient(mMounter);
      mStartNode = mPath->getStartNode(mMounter);
      mEndNode = mPath->getEndNode(mMounter);
      mPathMode = mPath->getFollowMode(mMounter);
      mLoops = mPath->getLoops(mMounter);

      deleteNotify(mounter);
      deleteNotify(path);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if (((object == mMounter) || (object == mPath)) && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      mPath->attachObject(mMounter, mSpeed, mDirection, mOrientToPath, mStartNode, mEndNode, mPathMode, mLoops, true);
      mSceneEdit->onObjectChanged(mPath);
      mSceneEdit->onObjectChanged(mMounter);
   };

   virtual void redo()
   {
      mPath->detachObject(mMounter);
      mMounter->setPosition(mStartPosition);
      mSceneEdit->onObjectChanged(mMounter);
      mSceneEdit->onObjectChanged(mPath);
   };
};

#endif


#endif // TORQUE_TOOLS
