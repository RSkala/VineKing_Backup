#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level builder editing context.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERSCENEEDIT_H_
#define _LEVELBUILDERSCENEEDIT_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#ifndef _T2DSCENEOBJECTSET_H_
#include "T2D/t2dSceneObjectSet.h"
#endif

#ifndef _T2DSCENEOBJECTGROUP_H_
#include "T2D/t2dSceneObjectGroup.h"
#endif

#ifndef _LEVELBUILDERBASETOOL_H_
#include "TGB/levelBuilderBaseTool.h"
#endif

#ifndef _LEVELBUILDERSCENEWINDOW_H_
#include "TGB/levelBuilderSceneWindow.h"
#endif

#ifndef _UNDO_H_
#include "util/undo.h"
#endif

typedef SimObjectPtr<LevelBuilderBaseTool> ToolPtr;

//-----------------------------------------------------------------------------
// LevelBuilderSceneEdit
//-----------------------------------------------------------------------------
class LevelBuilderSceneEdit : public SimObject
{
   typedef SimObject Parent;

private:
   // Tool Info
   ToolPtr mActiveTool;
   ToolPtr mDefaultTool;
   SimSet mTools;

   // Properties
   F32                     mGridSnapX;
   F32                     mGridSnapY;
   bool                    mGridVisible;
   bool                    mSnapToGridX;
   bool                    mSnapToGridY;
   ColorF                  mGridColor;
   ColorF                  mFillColor;
   F32                     mSnapThreshold;
   bool                    mRotationSnap;
   F32                     mRotationSnapThreshold;
   F32                     mRotationSnapAngle;
   bool                    mGuidesVisible;
   bool                    mCameraVisible;

   // Guides
   Vector<F32>             mXGuides;
   Vector<F32>             mYGuides;

   Point2I                 mDesignResolution;

   // State Info
   t2dVector               mCameraPosition;
   t2dVector               mMousePosition;
   F32                     mCameraZoom;

   // The Undo Manager
   UndoManager             mUndoManager;

   // This holds all deleted objects so the deletion can be undone.
   struct RecycledObject
   {
      RecycledObject(SimObject* _object, SimSet* _group) { object = _object; group = _group; }
      SimSet* group;
      SimObject* object;
   };

protected:
   // Acquired Objects
   t2dSceneObjectSet       mAcquiredObjects;
   t2dSceneObjectGroup*    mAcquiredGroup;

   // The Last Window Events Were Received From
   LevelBuilderSceneWindow* mLastWindow;
  
public:
   LevelBuilderSceneEdit();
   virtual ~LevelBuilderSceneEdit();

   Vector<RecycledObject>  mRecycleBin;

   // SimObject Overrides
   virtual bool onAdd();
   virtual void onRemove();

   // Scene Window Management
   void setLastWindow(LevelBuilderSceneWindow* sceneWindow) { mLastWindow = sceneWindow; };
   inline LevelBuilderSceneWindow* getLastWindow() { return mLastWindow; };

   // Undo Manager
   UndoManager& getUndoManager() { return mUndoManager; };
   void moveToRecycleBin(SimObject* object);
   void moveFromRecycleBin(SimObject* object);
   void onDeleteNotify( SimObject* object );
   bool isRecycled(SimObject* object);
   void undo();
   void redo();

   // Tool Management
   ToolPtr getActiveTool() const { return mActiveTool; };
   bool    setActiveTool(ToolPtr tool);
   bool    addTool(ToolPtr tool, bool setDefault = false);
   bool    removeTool(ToolPtr tool);

   // Default Tool
   ToolPtr getDefaultTool() const { return mDefaultTool; };
   void    setDefaultTool(ToolPtr tool);
   bool    setDefaultToolActive();

   // Object Acquisition
   void acquireObject(t2dSceneObject* object);
   void acquireObject(t2dSceneObjectGroup* pGroup);
   void requestAcquisition(t2dSceneObject* obj);
   void requestAcquisition(t2dSceneObjectGroup* pGroup);
   void clearAcquisition(t2dSceneObject* object = NULL);

   // Acquired Object Management
   inline t2dSceneObject*      getAcquiredObject(S32 index = 0) const { return (t2dSceneObject*)mAcquiredObjects.at(index); };
   inline t2dSceneObjectSet&   getAcquiredObjects()                   { return mAcquiredObjects; }
   inline t2dSceneObjectGroup* getAcquiredGroup() const               { return mAcquiredGroup; };
   inline S32                  getAcquiredObjectCount() const         { return mAcquiredObjects.size(); }
   inline bool                 hasAcquiredObjects() const             { return mAcquiredObjects.size() > 0; }
   bool                        isAcquired(const t2dSceneObject* object) const;
   bool                        isAcquired(const t2dSceneObjectGroup* group) const;
   bool                        isOnlyAcquired(const t2dSceneObjectGroup* group) const;
   bool                        containsAllAcquiredObjects(const t2dSceneObjectGroup* group) const;
   void                        deleteAcquiredObjects();
   void                        groupAcquiredObjects();
   void                        addObjectsToGroup(t2dSceneObjectGroup* checkGroup, Vector<SimObject*>& objectGroup);
   void                        breakApartAcquiredObjects();
   void                        updateAcquiredObjects();
	void                        onObjectChanged();
	void                        onObjectSpatialChanged();
   void                        onObjectChanged(t2dSceneObject* object);
   void                        onObjectSpatialChanged(t2dSceneObject* object);

   // Scene Window Events
   virtual bool onMouseEvent(LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus& mouseStatus);
   virtual bool onKeyDown(LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event);
   virtual bool onKeyRepeat(LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event);
   virtual bool onKeyUp(LevelBuilderSceneWindow* sceneWindow, const GuiEvent& event);
   virtual void onRenderBackground(LevelBuilderSceneWindow* sceneWindow);
   virtual void onRenderForeground(LevelBuilderSceneWindow* sceneWindow);

   // Design Resolution
   inline void    setDesignResolution(Point2I res) { mDesignResolution = res; };
   inline Point2I getDesignResolution()            { return mDesignResolution; };

   // Property Accessors
   inline ColorI getGridColor() const             { return mGridColor; };
   inline ColorI getFillColor() const             { return mFillColor; };
   inline bool   getGridVisibility() const        { return mGridVisible; }
   inline bool   getSnapToGridX() const           { return mSnapToGridX; };
   inline bool   getSnapToGridY() const           { return mSnapToGridY; };
   inline bool   isGridSnapX() const              { return (mGridSnapX != 0.0f); }
   inline bool   isGridSnapY() const              { return (mGridSnapY != 0.0f); }
   inline F32    getGridSnapX() const             { return mGridSnapX; }
   inline F32    getGridSnapY() const             { return mGridSnapY; }
   inline F32    getSnapThreshold() const         { return mSnapThreshold; };
   inline F32    getRotationSnapThreshold() const { return mRotationSnapThreshold; };
   inline bool   getRotationSnap() const          { return mRotationSnap; };
   inline F32    getRotationSnapAngle() const     { return mRotationSnapAngle; };
   inline bool   getCameraVisibility() const      { return mCameraVisible; };
   inline bool   getGuidesVisibility() const      { return mGuidesVisible; };

   // Property Setting
   inline void setGridColor(ColorI gridColor)      { mGridColor = gridColor; };
   inline void setFillColor(ColorI fillColor)      { mFillColor = fillColor; };
   inline void setGridVisibility(bool bVisible)    { mGridVisible = bVisible; }
   inline void setSnapToGridX(bool bSnap)          { mSnapToGridX = bSnap; };
   inline void setSnapToGridY(bool bSnap)          { mSnapToGridY = bSnap; };
   inline void setGridSnapX(F32 fSnap)             { mGridSnapX = fSnap;}
   inline void setGridSnapY(F32 fSnap)             { mGridSnapY = fSnap;}
   inline void setSnapThreshold(F32 value)         { mSnapThreshold = value; };
   inline void setRotationSnap(bool value)         { mRotationSnap = value; };
   inline void setRotationSnapAngle(F32 value)     { mRotationSnapAngle = value; };
   inline void setRotationSnapThreshold(F32 value) { mRotationSnapThreshold = value; };
   inline void setCameraVisibility(bool visible)   { mCameraVisible = visible; };
   inline void setGuidesVisibility(bool visible)   { mGuidesVisible = visible; };

   // Guide Management
   void addXGuide( F32 x ) { mXGuides.push_back( x ); };
   void addYGuide( F32 y ) { mYGuides.push_back( y ); };
   void removeXGuide( F32 x );
   void removeYGuide( F32 y );
   bool hasXGuides() { return !mXGuides.empty(); };
   bool hasYGuides() { return !mYGuides.empty(); };
   F32 getClosestXGuide( F32 x );
   F32 getClosestYGuide( F32 y );

   // State Accessors
   inline t2dVector getMousePosition()          { return mMousePosition; };
   inline t2dVector getCameraPosition()         { return mCameraPosition; };
   inline F32       getCameraZoom()             { return mCameraZoom; };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderSceneEdit);
};

// Undo Action Types
class UndoDeleteAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   struct UndoObject
   {
      UndoObject(t2dSceneObject* _object, bool _wasAcquired) { object = _object; wasAcquired = _wasAcquired; group = _object->getSceneObjectGroup(); };
      t2dSceneObject* object;
      t2dSceneObjectGroup* group;
      bool wasAcquired;
   };

   Vector<UndoObject> mObjects;

   // We need this so we can send notifications of objects changing.
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoDeleteAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };

   void addObject(t2dSceneObject* object, bool wasAcquired) { mObjects.push_back(UndoObject(object, wasAcquired)); deleteNotify(object); };

   virtual void onDeleteNotify(SimObject* object)
   {
      Vector<UndoObject>::iterator itr;
      for (itr = mObjects.begin(); itr != mObjects.end(); itr++)
      {
         if (itr->object == object)
         {
            mObjects.erase_fast(itr);
            break;
         }
      }

      if (mObjects.empty() && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         t2dSceneObject* object = mObjects[i].object;
         if (mSceneEdit->isRecycled(mObjects[i].group))
            mSceneEdit->moveFromRecycleBin(mObjects[i].group);

         mSceneEdit->moveFromRecycleBin(object);
         if (mObjects[i].wasAcquired)
            mSceneEdit->acquireObject(object);
      }

      Con::executef(1, "refreshTreeView");
   };

   virtual void redo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         t2dSceneObject* object = mObjects[i].object;
         if (mSceneEdit->isAcquired(object))
            mSceneEdit->clearAcquisition(object);

         mSceneEdit->moveToRecycleBin(object);

         if (mObjects[i].group && mObjects[i].group->empty())
            mSceneEdit->moveToRecycleBin(mObjects[i].group);
      }

      Con::executef(1, "refreshTreeView");
   }
};

class UndoGroupAction : public UndoAction
{
   struct GroupedObject
   {
      GroupedObject(SimSet* _oldGroup, SimObject* _object) { oldGroup = _oldGroup; object = _object; };
      SimSet* oldGroup;
      SimObject* object;
   };

   Vector<GroupedObject> mObjects;
   SimSet* mOldGroup;
   t2dSceneObjectGroup* mGroup;

   LevelBuilderSceneEdit* mSceneEdit;

public:

   UndoGroupAction(LevelBuilderSceneEdit* sceneEdit, t2dSceneObjectGroup* group, UTF8* actionName) : UndoAction(actionName)
   {
      mSceneEdit = sceneEdit;
      mGroup = group;
   };

   void addObject(SimObject* object)
   {
      SimSet* set = t2dSceneObjectGroup::getSceneObjectGroup(object);
      if (!set) set = t2dSceneObjectGroup::getSceneGraph(object);
      mObjects.push_back(GroupedObject(set, object));

      deleteNotify(object);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      Vector<GroupedObject>::iterator itr;
      for (itr = mObjects.begin(); itr != mObjects.end(); itr++)
      {
         if (itr->object == object)
         {
            mObjects.erase_fast(itr);
            break;
         }
      }

      if (mObjects.empty() && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         mGroup->removeObject(mObjects[i].object);
         mObjects[i].oldGroup->addObject(mObjects[i].object);
      }
      mSceneEdit->moveToRecycleBin(mGroup);

      Con::executef(1, "refreshTreeView");
   };

   virtual void redo()
   {
      mSceneEdit->moveFromRecycleBin(mGroup);
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         mObjects[i].oldGroup->removeObject(mObjects[i].object);
         mGroup->addObject(mObjects[i].object);
      }

      Con::executef(1, "refreshTreeView");
   };
};

class UndoBreakApartAction : public UndoAction
{
   struct GroupedObject
   {
      GroupedObject(SimSet* _oldGroup, SimSet* _newGroup, SimObject* _object) { oldGroup = _oldGroup; newGroup = _newGroup; object = _object; };
      SimSet* oldGroup;
      SimSet* newGroup;
      SimObject* object;
   };

   Vector<GroupedObject> mObjects;
   Vector<t2dSceneObjectGroup*> mRecycledGroups;
   LevelBuilderSceneEdit* mSceneEdit;

public:

   UndoBreakApartAction(LevelBuilderSceneEdit* sceneEdit, UTF8* actionName) : UndoAction(actionName)
   {
      mSceneEdit = sceneEdit;
   };

   void addObject(SimObject* object, SimSet* oldGroup, SimSet* newGroup)
   {
      mObjects.push_back(GroupedObject(oldGroup, newGroup, object));
      deleteNotify(object);
   };

   void addRecycledGroup(t2dSceneObjectGroup* group)
   {
      mRecycledGroups.push_back(group);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      Vector<GroupedObject>::iterator itr;
      for (itr = mObjects.begin(); itr != mObjects.end(); itr++)
      {
         if (itr->object == object)
         {
            mObjects.erase_fast(itr);
            break;
         }
      }

      if (mObjects.empty() && mUndoManager)
         mUndoManager->removeAction(this);
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mRecycledGroups.size(); i++)
         mSceneEdit->moveFromRecycleBin(mRecycledGroups[i]);

      for (S32 i = 0; i < mObjects.size(); i++)
      {
         mObjects[i].newGroup->removeObject(mObjects[i].object);
         mObjects[i].oldGroup->addObject(mObjects[i].object);
      }

      Con::executef(1, "refreshTreeView");
   };

   virtual void redo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         mObjects[i].oldGroup->removeObject(mObjects[i].object);
         mObjects[i].newGroup->addObject(mObjects[i].object);
      }

      for (S32 i = 0; i < mRecycledGroups.size(); i++)
         mSceneEdit->moveToRecycleBin(mRecycledGroups[i]);

      Con::executef(1, "refreshTreeView");
   };
};

#endif


#endif // TORQUE_TOOLS
