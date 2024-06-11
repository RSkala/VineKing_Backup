#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Object Selection/Movement Tool
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERSELECTIONTOOL_H_
#define _LEVELBUILDERSELECTIONTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#include "TGB/levelBuilderBaseEditTool.h"
#endif

//-----------------------------------------------------------------------------
// SelectionToolWidget
//-----------------------------------------------------------------------------
class SelectionToolWidget : public SimObject
{
	typedef SimObject Parent;

public:
   enum eDisplayRules
   {
      UnMountedOnly = 1,
      MountedOnly  = 2,
      UnPathedOnly = 4,
      PathedOnly = 8,

      NoDisplayRules = 0
   };

   // The texture to draw for this widget.
   TextureHandle mTexture;

   // The tooltip to render when this widget is hovered over.
   StringTableEntry mToolTip;

   // Since different classes may have more than 4 widgets, the four with the highest
   // priority will be displayed.
   S32 mPriority;

   // This is the position this widget prefers to be drawn at (0 - upper left, 1 - upper right,
   // 2 - lower left, 3 - lower right, -1 - no preference).
   S32 mPosition;

   // These are the classes this widget should be or should not be shown for.
   Vector<StringTableEntry> mClasses;

   // Determines whether the previous list contains classes to show, or not show.
   bool mShowClasses;

   // Display rule flags.
   U32 mDisplayRules;

   // The script method to call when this is pressed.
   StringTableEntry mCallback;

   SelectionToolWidget();

   static void initPersistFields();

   // Checks if an object (based on its class) should show this widget.
   bool isDisplayed(const t2dSceneObject* object)
   {
      if ((object->getIsMounted() && (mDisplayRules & UnMountedOnly)) ||
          (!object->getIsMounted() && (mDisplayRules & MountedOnly)))
      {
         return false;
      }

      if ((object->getAttachedToPath() && (mDisplayRules & UnPathedOnly)) ||
          (!object->getAttachedToPath() && (mDisplayRules & PathedOnly)))
      {
         return false;
      }

      StringTableEntry className = object->getClassName();
      for (S32 i = 0; i < mClasses.size(); i++)
      {
         if (dStricmp(className, mClasses[i]) == 0)
            return mShowClasses;
      }
      return !mShowClasses;
   };

   void addClass(const char* className)
   {
      mClasses.push_back(StringTable->insert(className));
   };

   void setTexture(const char* textureName)
   {
      mTexture = TextureHandle(textureName, BitmapTexture);
   };

   void setDisplayRule(eDisplayRules displayRule)
   {
      mDisplayRules |= displayRule;
   };

   // Calls the script function defined in 'mCallback'.
   void doCallback(LevelBuilderSceneEdit* sceneEdit, const t2dSceneObject* object)
   {
      Con::executef(sceneEdit, 2, mCallback, Con::getIntArg(object->getId()));
   };

	DECLARE_CONOBJECT( SelectionToolWidget );
};

//-----------------------------------------------------------------------------
// LevelBuilderSelectionTool
//-----------------------------------------------------------------------------
class LevelBuilderSelectionTool : public LevelBuilderBaseEditTool
{
	typedef LevelBuilderBaseEditTool Parent;

private:
   UndoAction* mCurrentUndo;
   bool        mAddUndo;

   // True when the left mouse button is down
   bool        mMouseDown;
   F32         mMouseDownAR;
   // True if trying to drag a mounted object without its mount.
   bool        mCantMove;
   bool        mForcingUniform;
   bool        mForcingShift;
   Point2I     mTooltipPosition;

   TextureHandle mTexture;
   Point2I     mWidgetSize;
   Point2I     mWidgetBufferSize;
   S32         mWidgetSelectBuffer;

   Vector<SelectionToolWidget*> mWidgets;
   SelectionToolWidget *mHoverWidget;

protected:
   // Mouse States.
   enum objectMouseModes
   {
      Selecting,
      DragSelecting,
      MovingSelection,
      SizingSelection,
      RotatingSelection,
      SelectingWidget
   };
   
   U32                      mMouseState;

   // The hover object is the object that would be selected if the mouse were
   // clicked at its current point.
	const t2dSceneObject*    mHoverObj;

   // The selected hover object is the object for which the widgets are being shown.
   const t2dSceneObject*    mSelectedHoverObj;

   static const S32 mMaxWidgets = 8;
   // These are the widgets to be rendered for the current selected hover obj.
   SelectionToolWidget* mCurrentWidgets[mMaxWidgets];
   S32 mNumberOfWidgets;

   // The colors with which to draw the hover object rect.
	ColorI                   mHoverOutlineColor;
	ColorI                   mHoverFillColor;

   // The window space rectangle of the current drag rect.
   RectI                    mDragRect;

   // Properties
   bool                     mFullContainSelect;
   bool                     mUndoSelections;

   // This is the offset from an object's center that the mouse was clicked.
   t2dVector mMouseOffset;
   // These store info about the rotation and mouse status at the start of a rotation.
   F32 mStartRotation;
   t2dVector mRotationVector;

   // This function finds the selected hover object based on a mouse status.
   void checkSelectedHoverObj(LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus& mouseStatus);
   void setSelectedHoverObj(const t2dSceneObject* object);

public:
	LevelBuilderSelectionTool();
	~LevelBuilderSelectionTool();

   void refreshSelectedHoverObj();

	// Acquired Mouse Events
   virtual bool onMouseMove( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus );
	virtual bool onKeyDown(LevelBuilderSceneWindow* sceneWindow, const GuiEvent &event);
	virtual bool onKeyRepeat(LevelBuilderSceneWindow* sceneWindow, const GuiEvent &event);
   virtual bool onKeyUp(LevelBuilderSceneWindow* sceneWindow, const GuiEvent &event);

   // Add Widgets
   void addWidget(SelectionToolWidget* widget);

	// Tool Rendering.
	void onRenderGraph(LevelBuilderSceneWindow* sceneWindow);

   // Property Accessors
   inline void setFullContainSelect(bool set) { mFullContainSelect = set; };
   inline bool getFullContainSelect() const   { return mFullContainSelect; };
   
   inline void setUndoSelections(bool set) { mUndoSelections = set; };
   inline bool getUndoSelections() const   { return mUndoSelections; };

	// Declare our Console Object
	DECLARE_CONOBJECT( LevelBuilderSelectionTool );
};

// Undo Action Types
class UndoMoveAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   struct UndoObject
   {
      UndoObject(t2dSceneObject* _object, t2dVector _oldPosition) { object = _object; oldPosition = _oldPosition; };
      t2dSceneObject* object;
      t2dVector oldPosition;
      t2dVector newPosition;
   };

   Vector<UndoObject> mObjects;

   // We need this so we can send notifications of objects changing.
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoMoveAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };

   void addObject(t2dSceneObject* object)
   {
      mObjects.push_back(UndoObject(object, object->getPosition()));
      deleteNotify(object);
   };

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
   }

   void setNewPosition(t2dSceneObject* object)
   {
      Vector<UndoObject>::iterator itr;
      for (itr = mObjects.begin(); itr != mObjects.end(); itr++)
      {
         if ((*itr).object == object)
         {
            (*itr).newPosition = object->getPosition();
            return;
         }
      }
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         if (!mObjects[i].object) continue;
         t2dSceneObject* object = mObjects[i].object;
         object->setPosition(mObjects[i].oldPosition);
         mSceneEdit->onObjectSpatialChanged(object);
      }
   };

   virtual void redo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         if (!mObjects[i].object) continue;
         t2dSceneObject* object = mObjects[i].object;
         object->setPosition(mObjects[i].newPosition);
         mSceneEdit->onObjectSpatialChanged(object);
      }
   }
};

// Undo Action Types
class UndoScaleAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   struct UndoObject
   {
      UndoObject(t2dSceneObject* _object, t2dVector _oldSize, t2dVector _oldPosition, bool _oldFlipX, bool _oldFlipY)
	  {
		  object = _object;
		  oldSize = _oldSize;
		  oldPosition = _oldPosition;
		  oldFlipX = _oldFlipX;
		  oldFlipY = _oldFlipY;
	  };
      t2dSceneObject* object;
      t2dVector oldSize;
      t2dVector oldPosition;
	  bool oldFlipX;
	  bool oldFlipY;
      t2dVector newSize;
      t2dVector newPosition;
	  bool newFlipX;
	  bool newFlipY;
   };

   Vector<UndoObject> mObjects;

   // We need this so we can send notifications of objects changing.
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoScaleAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };

   void addObject(t2dSceneObject* object)
   {
      mObjects.push_back(UndoObject(object, object->getSize(), object->getPosition(), object->getFlipX(), object->getFlipY()));
      deleteNotify(object);
   };

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
   }

   void setNewSize(t2dSceneObject* object)
   {
      Vector<UndoObject>::iterator itr;
      for (itr = mObjects.begin(); itr != mObjects.end(); itr++)
      {
         if ((*itr).object == object)
         {
            (*itr).newSize = object->getSize();
            (*itr).newPosition = object->getPosition();
			(*itr).newFlipX = object->getFlipX();
			(*itr).newFlipY = object->getFlipY();
            return;
         }
      }
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         if (!mObjects[i].object) continue;
         t2dSceneObject* object = mObjects[i].object;
         object->setSize(mObjects[i].oldSize);
         object->setPosition(mObjects[i].oldPosition);
		 object->setFlipX(mObjects[i].oldFlipX);
		 object->setFlipY(mObjects[i].oldFlipY);
         mSceneEdit->onObjectSpatialChanged(object);
      }
   };

   virtual void redo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         if (!mObjects[i].object) continue;
         t2dSceneObject* object = mObjects[i].object;
         object->setSize(mObjects[i].newSize);
         object->setPosition(mObjects[i].newPosition);
		 object->setFlipX(mObjects[i].newFlipX);
		 object->setFlipY(mObjects[i].newFlipY);
         mSceneEdit->onObjectSpatialChanged(object);
      }
   }
};

// Undo Action Types
class UndoSelectAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   Vector<t2dSceneObject*> mOldObjects;
   Vector<t2dSceneObject*> mNewObjects;

   // We need this so we can send notifications of objects changing.
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoSelectAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };

   void addOldObject(t2dSceneObject* object)
   {
      mOldObjects.push_back(object);
      deleteNotify(object);
   };
   void addNewObject(t2dSceneObject* object)
   {
      mNewObjects.push_back(object);
      deleteNotify(object);
   };

   virtual void onDeleteNotify(SimObject* object)
   {
      Vector<t2dSceneObject*>::iterator itr;
      for (itr = mOldObjects.begin(); itr != mOldObjects.end(); itr++)
      {
         if (*itr == object)
         {
            mOldObjects.erase_fast(itr);
            break;
         }
      }

      Vector<t2dSceneObject*>::iterator itr2;
      for (itr2 = mNewObjects.begin(); itr2 != mNewObjects.end(); itr2++)
      {
         if (*itr2 == object)
         {
            mNewObjects.erase_fast(itr2);
            break;
         }
      }

      if (mOldObjects.empty() && mNewObjects.empty() && mUndoManager)
         mUndoManager->removeAction(this);
   }

   virtual void undo()
   {
      mSceneEdit->clearAcquisition();
      for (S32 i = 0; i < mOldObjects.size(); i++)
      {
         if (!mOldObjects[i]) continue;
         mSceneEdit->acquireObject(mOldObjects[i]);
      }
   };

   virtual void redo()
   {
      mSceneEdit->clearAcquisition();
      for (S32 i = 0; i < mNewObjects.size(); i++)
      {
         SimObjectPtr<t2dSceneObject> object = mNewObjects[i];
         if (!object) continue;
         mSceneEdit->acquireObject(mNewObjects[i]);
      }
   }
};

// Undo Action Types
class UndoRotateAction : public UndoAction
{
   typedef UndoAction Parent;

private:
   struct UndoObject
   {
      UndoObject(t2dSceneObject* _object, F32 _oldRotation) { object = _object; oldRotation = _oldRotation; };
      t2dSceneObject* object;
      F32 oldRotation;
      F32 newRotation;
   };

   Vector<UndoObject> mObjects;

   // We need this so we can send notifications of objects changing.
   LevelBuilderSceneEdit* mSceneEdit;

public:
   UndoRotateAction(LevelBuilderSceneEdit* sceneEdit, UTF8* name) : UndoAction(name) { mSceneEdit = sceneEdit; };

   void addObject(t2dSceneObject* object)
   {
      mObjects.push_back(UndoObject(object, object->getRotation()));
      deleteNotify(object);
   };

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

   void setNewRotation(t2dSceneObject* object)
   {
      Vector<UndoObject>::iterator itr;
      for (itr = mObjects.begin(); itr != mObjects.end(); itr++)
      {
         if ((*itr).object == object)
         {
            (*itr).newRotation = object->getRotation();
            return;
         }
      }
   };

   virtual void undo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         if (!mObjects[i].object) continue;
         t2dSceneObject* object = mObjects[i].object;
         object->setRotation(mObjects[i].oldRotation);
         mSceneEdit->onObjectSpatialChanged(object);
      }
   };

   virtual void redo()
   {
      for (S32 i = 0; i < mObjects.size(); i++)
      {
         if (!mObjects[i].object) continue;
         t2dSceneObject* object = mObjects[i].object;
         object->setRotation(mObjects[i].newRotation);
         mSceneEdit->onObjectSpatialChanged(object);
      }
   }
};

#endif


#endif // TORQUE_TOOLS
