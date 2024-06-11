#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level Object Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERCREATETOOL_H_
#define _LEVELBUILDERCREATETOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#include "TGB/levelBuilderBaseEditTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderCreateTool
// Provides standard functionality for object creation.
//-----------------------------------------------------------------------------
class LevelBuilderCreateTool : public LevelBuilderBaseEditTool
{
private:
   bool              mObjectHidden;
   ColorI            mOutlineColor;

   // The mouse drag start position after snapping.
   t2dVector mDragStart;
   F32 mMouseDownAR;

protected:
   typedef LevelBuilderBaseEditTool Parent;
   
   t2dSceneObject*   mCreatedObject;
   StringTableEntry  mScriptClassName;
   StringTableEntry  mScriptSuperClassName;
   StringTableEntry  mConfigDatablockName;

   // Must be defined by the derived class though they can't be pure virtual since this
   // is a console object.
   virtual t2dSceneObject* createObject() { return NULL; };
   virtual void showObject() { if (mCreatedObject) mCreatedObject->setVisible(true); };
   virtual t2dVector getDefaultSize(LevelBuilderSceneWindow* sceneWindow);
   virtual Point2I getPixelSize() { return Point2I(128, 128); };

   // Properties
   bool mAcquireCreatedObjects;
  
public:
   LevelBuilderCreateTool();
   ~LevelBuilderCreateTool();
   
	virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );

   void onRenderGraph( LevelBuilderSceneWindow* sceneWindow );

   bool create(LevelBuilderSceneWindow* sceneWindow);
   t2dSceneObject* createFull(LevelBuilderSceneWindow* sceneWindow, t2dVector position);

   virtual void onObjectCreated();

   // Property Accessors
   void setAcquireCreatedObjects(bool val) { mAcquireCreatedObjects = val; };
   bool getAcquireCreatedObjects()         { return mAcquireCreatedObjects; };

   void setConfigDatablock( const char* datablockName );
   void setClassNamespace( const char* classNamespace );
   void setSuperClassNamespace( const char* superClassNamespace );

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderCreateTool);
};

// Undo Action
class UndoCreateAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   t2dSceneObject* mObject;
   bool mWasAcquired;
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoCreateAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };
   void addObject(t2dSceneObject* object)
   {
      mObject = object;
      deleteNotify(object);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      if (mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      if (!mObject) return;
      if (mSceneEdit->isAcquired(mObject))
      {
         mWasAcquired = true;
         mSceneEdit->clearAcquisition(mObject);
      }
      else
         mWasAcquired = false;

      mSceneEdit->moveToRecycleBin(mObject);
   }

   virtual void redo()
   {
      if (!mObject) return;
      mSceneEdit->moveFromRecycleBin(mObject);
      if (mWasAcquired)
         mSceneEdit->requestAcquisition(mObject);
   }
};

#endif


#endif // TORQUE_TOOLS
