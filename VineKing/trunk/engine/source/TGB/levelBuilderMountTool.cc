#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// SceneObject Mount Tool.
//-----------------------------------------------------------------------------
#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderMountTool.h"
#include "TGB/levelBuilderSceneEdit.h"
#include "T2D/t2dPath.h"

IMPLEMENT_CONOBJECT(LevelBuilderMountTool);

LevelBuilderMountTool::LevelBuilderMountTool() : LevelBuilderBaseTool(),
                                                 mSceneWindow(NULL),
                                                 mSceneObject(NULL),
                                                 mBlendAlpha(1.0f)
{
   mNutSize = 8;
   // Set our tool name
   mToolName = StringTable->insert("Mount Tool");
}

LevelBuilderMountTool::~LevelBuilderMountTool()
{
}

bool LevelBuilderMountTool::onActivate(LevelBuilderSceneWindow* sceneWindow)
{
   if (!Parent::onActivate(sceneWindow))
      return false;

   sceneWindow->getSceneGraph()->setDebugOn(4);

   mSceneObject = NULL;
   mSceneWindow = sceneWindow;

   return true;
}

void LevelBuilderMountTool::onDeactivate()
{
   finishEdit();

   if (mSceneWindow)
      mSceneWindow->getSceneGraph()->setDebugOff(4);

   mSceneObject = NULL;
   mSceneWindow = NULL;
   Parent::onDeactivate();
}

bool LevelBuilderMountTool::onAcquireObject( t2dSceneObject *object )
{
   if(!isEditable(object) || !mSceneWindow)
      return false;

   // Parent handling 
   if(!Parent::onAcquireObject(object)) 
      return false;

   if (!mSceneObject || (mSceneWindow->getToolOverride() == this))
   {
      finishEdit();
      editObject(object);
   }
   
   return true;
}

void LevelBuilderMountTool::onRelinquishObject( t2dSceneObject *object )
{
   if(!mSceneWindow || !mSceneObject)
      return Parent::onRelinquishObject(object);

   if (object == mSceneObject)
   {
      finishEdit();
   }

   // Do parent cleanup
   Parent::onRelinquishObject(object);
}

void LevelBuilderMountTool::editObject(t2dSceneObject* object)
{
   if (!isEditable(object))
      return;

   mBlendAlpha = object->getBlendAlpha();
   object->setBlendAlpha(0.5f);
   mObjectStartPosition = object->getPosition();

   mSceneObject = object;
}

ConsoleMethod(LevelBuilderMountTool, editObject, void, 3, 3, "Selects an object for editing.")
{
   t2dSceneObject* obj = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));
   if (obj)
      object->editObject(obj);
   else
      Con::warnf("Invalid object past to LevelBuilderMountTool::editObject");
}

void LevelBuilderMountTool::finishEdit()
{
   if (!mSceneObject || !mSceneWindow)
      return;

   mSceneObject->setBlendAlpha(mBlendAlpha);
   mSceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);
   Con::executef(this, 2, "onFinishEdit", mSceneObject);

   mSceneObject = NULL;
}

ConsoleMethod(LevelBuilderMountTool, finishEdit, void, 2, 2, "Applies changes and ends editing of an object.")
{
   object->finishEdit();
}

bool LevelBuilderMountTool::isEditable(t2dSceneObject* obj)
{
   return true;
}

ConsoleMethod(LevelBuilderMountTool, dismountObject, void, 4, 4, "Dismounts an object.")
{
   LevelBuilderSceneEdit* sceneEdit = dynamic_cast<LevelBuilderSceneEdit*>(Sim::findObject(argv[2]));
   t2dSceneObject* obj = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[3]));

   if (sceneEdit && obj)
      object->dismountObject(sceneEdit, obj);
}

ConsoleMethod(LevelBuilderMountTool, clearAllMounts, void, 4, 4, "Clears all mounts.")
{
   LevelBuilderSceneEdit* sceneEdit = dynamic_cast<LevelBuilderSceneEdit*>(Sim::findObject(argv[2]));
   t2dSceneObject* obj = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[3]));

   if (sceneEdit && obj)
      object->clearAllMounts(sceneEdit, obj);
}

void LevelBuilderMountTool::dismountObject(LevelBuilderSceneEdit* sceneEdit, t2dSceneObject* object)
{
   if (object->getIsMounted())
   {
      t2dSceneObject* mountedTo = object->getMountedParent();

      UndoDismountAction* undo = new UndoDismountAction(sceneEdit, "Dismount Object");
      undo->setMountInfo(object, mountedTo, object->getMountOffset(), object->getMountForce(), object->getMountTrackRotation(),
                         true, object->getMountOwned(), object->getMountInheritAttributes());

      object->dismount();
      t2dVector position = object->getPosition() + t2dVector(10.0f, 10.0f);
      object->setPosition(position);
      undo->setStartPosition(position);
      undo->addToManager(&sceneEdit->getUndoManager());

      sceneEdit->onObjectChanged(object);
      sceneEdit->onObjectChanged(mountedTo);
      object->updateSpatialConfig();
      sceneEdit->updateAcquiredObjects();
   }

   else if (object->getAttachedToPath())
   {
      t2dPath* path = dynamic_cast<t2dPath*>(object->getAttachedToPath());
      if (path)
      {
         UndoPathDetachAction* undo = new UndoPathDetachAction(sceneEdit, "Detach From Path");
         undo->setMountInfo(object, path);
         path->detachObject(object);
         t2dVector position = object->getPosition() + t2dVector(10.0f, 10.0f);
         object->setPosition(position);
         undo->setStartPosition(position);
         undo->addToManager(&sceneEdit->getUndoManager());
         
         sceneEdit->onObjectChanged(object);
         sceneEdit->onObjectChanged(path);
         object->updateSpatialConfig();
         sceneEdit->updateAcquiredObjects();
      }
   }
}

void LevelBuilderMountTool::clearAllMounts(LevelBuilderSceneEdit* sceneEdit, t2dSceneObject* object)
{
   if (object->getIsMounted())
   {
      t2dSceneObject* mountedTo = object->getMountedParent();

      UndoDismountAction* undo = new UndoDismountAction(sceneEdit, "Dismount Object");
      undo->setMountInfo(object, mountedTo, object->getMountOffset(), object->getMountForce(), object->getMountTrackRotation(),
         true, object->getMountOwned(), object->getMountInheritAttributes());

      object->dismount();
      t2dVector position = object->getPosition() + t2dVector(10.0f, 10.0f);
      object->setPosition(position);
      undo->setStartPosition(position);
      undo->addToManager(&sceneEdit->getUndoManager());

      sceneEdit->onObjectChanged(object);
      sceneEdit->onObjectChanged(mountedTo);
      object->updateSpatialConfig();
      sceneEdit->updateAcquiredObjects();
   }

   else if (object->getAttachedToPath())
   {
      t2dPath* path = dynamic_cast<t2dPath*>(object->getAttachedToPath());
      if (path)
      {
         UndoPathDetachAction* undo = new UndoPathDetachAction(sceneEdit, "Detach From Path");
         undo->setMountInfo(object, path);
         path->detachObject(object);
         t2dVector position = object->getPosition() + t2dVector(10.0f, 10.0f);
         object->setPosition(position);
         undo->setStartPosition(position);
         undo->addToManager(&sceneEdit->getUndoManager());

         sceneEdit->onObjectChanged(object);
         sceneEdit->onObjectChanged(path);
         object->updateSpatialConfig();
         sceneEdit->updateAcquiredObjects();
      }
   }
   else
   {
      // check to see if this is a t2dPath
      t2dPath* path = dynamic_cast<t2dPath*>(object);
      if (path)
      {
         // for each pathed object, create an undo action and dismount
         while (path->getPathedObjectCount() > 0)
         {
            t2dSceneObject* pathedObject = path->getPathedObject((U32)0);
            if (pathedObject)
            {
               UndoPathDetachAction* undo = new UndoPathDetachAction(sceneEdit, "Detach From Path");
               undo->setMountInfo(pathedObject, path);
               path->detachObject(pathedObject);

               t2dVector position = pathedObject->getPosition() + t2dVector(10.0f, 10.0f);
               pathedObject->setPosition(position);

               undo->setStartPosition(position);
               undo->addToManager(&sceneEdit->getUndoManager());

               sceneEdit->onObjectChanged(pathedObject);
               sceneEdit->onObjectChanged(path);
               pathedObject->updateSpatialConfig();
               sceneEdit->updateAcquiredObjects();
            }
         }
      }
   }

   // lets grab this object's scenegraph, and make sure no objects are mounted to it.. UGH!
   t2dSceneGraph* sceneGraph = object->getSceneGraph();
   if (sceneGraph)
   {
      for (U32 i = 0; i < sceneGraph->size(); i++)
      {
         t2dSceneObject* sceneObject = sceneGraph->getSceneObject(i);
         if (sceneObject && (sceneObject->getMountedParent() == object))
         {
            UndoDismountAction* undo = new UndoDismountAction(sceneEdit, "Dismount Object");

            undo->setMountInfo(sceneObject, object, sceneObject->getMountOffset(), sceneObject->getMountForce(), sceneObject->getMountTrackRotation(),
               true, sceneObject->getMountOwned(), sceneObject->getMountInheritAttributes());

            sceneObject->dismount();
            t2dVector position = sceneObject->getPosition() + t2dVector(10.0f, 10.0f);
            sceneObject->setPosition(position);
            undo->setStartPosition(position);
            undo->addToManager(&sceneEdit->getUndoManager());

            sceneEdit->onObjectChanged(sceneObject);
            sceneEdit->onObjectChanged(object);
            sceneObject->updateSpatialConfig();
            sceneEdit->updateAcquiredObjects();
         }
      }
   }
}

void LevelBuilderMountTool::onRenderGraph(LevelBuilderSceneWindow* sceneWindow )
{
   Parent::onRenderGraph( sceneWindow );

   if ((mSceneWindow != sceneWindow) || !mSceneObject)
      return;
}

bool LevelBuilderMountTool::onMouseMove( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow))
      return false;

   F32 snapDistance = 2.5f;
   t2dVector objPosition = mouseStatus.mousePoint2D;
   objPosition.mX += snapDistance;

   bool found = false;

   // Find a mount point near the mouse unless shift is pressed.
   if (!(mouseStatus.event.modifier & SI_SHIFT))
   {
      for (S32 i = 0; i < mouseStatus.pickList.size(); i++)
      {
         t2dSceneObject* object = mouseStatus.pickList[i];

         // Skip this object.
         if (object == mSceneObject)
            continue;

         // If it's a path, we'll try to mount to a path node.
         t2dPath* path = dynamic_cast<t2dPath*>(object);
         if (path)
         {
            for (S32 i = 0; i < path->getNodeCount(); i++)
            {
               t2dPath::PathNode& node = path->getNode(i);
               if ((node.position - mouseStatus.mousePoint2D).len() < (objPosition - mouseStatus.mousePoint2D).len())
               {
                  found = true;
                  objPosition = node.position;
               }
            }
            continue;
         }

         // Otherwise, try to find a mount node.
         for (S32 j = 0; j < object->getMountNodeCount(); j++)
         {
            t2dVector nodePosition = object->getMountNodeByIndex(j).mWorldMountPosition;
            if ((nodePosition - mouseStatus.mousePoint2D).len() < (objPosition - mouseStatus.mousePoint2D).len())
            {
               found = true;
               objPosition = nodePosition;
            }
         }

         // And if nothing else, snap to the middle of the object.
         if (!found)
         {
            t2dVector nodePosition = object->getPosition();
            if ((nodePosition - mouseStatus.mousePoint2D).len() < (objPosition - mouseStatus.mousePoint2D).len())
            {
               found = true;
               objPosition = nodePosition;
            }
         }
      }
   }

   if (!found)
      objPosition = mouseStatus.mousePoint2D;

   mSceneObject->setPosition(objPosition);
   sceneWindow->getSceneEdit()->onObjectSpatialChanged(mSceneObject);

   return true;
}

bool LevelBuilderMountTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow))
      return false;

   bool found = false;

	// First, scan all objects with linkpoints, since they may have mountpoints outside their bounds.
	F32 snapDistance = 2.5f;
	t2dVector objPosition = mouseStatus.mousePoint2D;
	objPosition.mX += snapDistance;

	Vector<t2dSceneObject *> objList;
	U32 objCount;
	objCount = sceneWindow->getSceneGraph()->getSceneObjectList(objList);
	for (S32 i = 0; i < objCount; i++)
	{
		t2dSceneObject* object = objList[i];

		// Skip this object.
		if (object == mSceneObject)
			continue;

		// Try to find a mount node.
		for (S32 j = 0; j < object->getMountNodeCount(); j++)
		{
			t2dVector nodePosition = object->getMountNodeByIndex(j).mWorldMountPosition;
			if ((nodePosition - mouseStatus.mousePoint2D).len() < (objPosition - mouseStatus.mousePoint2D).len())
			{
				t2dVector offset = object->getLocalPoint(nodePosition);
				UndoMountAction* undo = new UndoMountAction(sceneWindow->getSceneEdit(), "Mount Object");
				undo->setStartPosition(mObjectStartPosition);
				undo->setMountInfo(mSceneObject, object, offset, 0.0f, true, true, false, false);
				if (object->getFlipX()) offset.mX = -offset.mX;
				if (object->getFlipY()) offset.mY = -offset.mY;
				mSceneObject->mount(object, offset, 0.0f, true, true, false, false);
				undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());

				sceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);
				sceneWindow->getSceneEdit()->onObjectChanged(object);
				found = true;
				break;
			}
		}
	}

	
	if (!found)
	{
		for (S32 i = 0; i < mouseStatus.pickList.size(); i++)
		{
			t2dSceneObject* mountToObject = mouseStatus.pickList[i];

			// Can't mount to self.
			if (mountToObject == mSceneObject)
				continue;

			// Check if it's a path.
			t2dPath* path = dynamic_cast<t2dPath*>(mountToObject);
			if (path)
			{
				for (S32 i = 0; i < path->getNodeCount(); i++)
				{
					t2dPath::PathNode& node = path->getNode(i);
					if (mIsZero((mSceneObject->getPosition() - node.position).len()))
					{
						UndoPathAttachAction* undo = new UndoPathAttachAction(sceneWindow->getSceneEdit(), "Attach to Path");
						undo->setStartPosition(mObjectStartPosition);
						path->attachObject(mSceneObject, 10, 1, true, i, i - 1, T2D_PATH_WRAP, -1, false);
						undo->setMountInfo(mSceneObject, path);
						undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
               
						sceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);
						sceneWindow->getSceneEdit()->onObjectChanged(path);

						found = true;
						break;
					}
				}
				if (found)
					break;
			}

			else
			{
				t2dVector offset = mountToObject->getLocalPoint(mSceneObject->getPosition());
				UndoMountAction* undo = new UndoMountAction(sceneWindow->getSceneEdit(), "Mount Object");
				undo->setStartPosition(mObjectStartPosition);
				undo->setMountInfo(mSceneObject, mountToObject, offset, 0.0f, true, true, false, false);
				if (mountToObject->getFlipX()) offset.mX = -offset.mX;
				if (mountToObject->getFlipY()) offset.mY = -offset.mY;
				mSceneObject->mount(mountToObject, offset, 0.0f, true, true, false, false);
				undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());

				sceneWindow->getSceneEdit()->onObjectChanged(mSceneObject);
				sceneWindow->getSceneEdit()->onObjectChanged(mountToObject);
				found = true;
				break;
			}
		}
	}

   if (!found)
   {
      UndoMountMoveAction* undo = new UndoMountMoveAction(sceneWindow->getSceneEdit(), "Move Object");
      undo->setStartPosition(mSceneObject, mObjectStartPosition);
      undo->setEndPosition(mSceneObject->getPosition());
      undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
   }

   finishEdit();

   return true;
}

bool LevelBuilderMountTool::onRightMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mSceneObject || (mSceneWindow != sceneWindow))
      return false;

   finishEdit();

   return true;
}


#endif // TORQUE_TOOLS
