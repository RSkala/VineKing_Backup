//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

#include "T2D/t2dSceneObjectGroup.h"
#include "T2D/t2dSceneObject.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(t2dSceneObjectGroup);

//---------------------------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------------------------
t2dSceneObjectGroup::t2dSceneObjectGroup() : mSceneObjectGroup(NULL),
                                             mSceneGraph(NULL)
{
   mNSLinkMask = LinkSuperClassName | LinkClassName;
}

//---------------------------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------------------------
t2dSceneObjectGroup::~t2dSceneObjectGroup()
{
}

bool t2dSceneObjectGroup::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Synchronize Namespace's
    linkNamespaces();

    // tell the scripts
    Con::executef(this, 1, "onAdd");

    return true;
}

void t2dSceneObjectGroup::onRemove()
{
    // tell the scripts
    Con::executef(this, 1, "onRemove");

    // Restore NameSpace's
    unlinkNamespaces();

    // Call Parent.
    Parent::onRemove();
}

// Get a scene object or scene group's parent group.
t2dSceneObjectGroup* t2dSceneObjectGroup::getSceneObjectGroup(SimObject* object)
{
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
   if (sceneObject)
      return sceneObject->getSceneObjectGroup();

   else
   {
      t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (group)
         return group->getSceneObjectGroup();
   }

   return NULL;
}

// Set a scene object or scene group's parent group.
void t2dSceneObjectGroup::setSceneObjectGroup(SimObject* object, t2dSceneObjectGroup* group)
{
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
   if (sceneObject)
      sceneObject->mpSceneObjectGroup = group;

   else
   {
      t2dSceneObjectGroup* sceneGroup = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (sceneGroup)
         sceneGroup->mSceneObjectGroup = group;
   }
}

// Get a scene object or scene group's scenegraph.
t2dSceneGraph* t2dSceneObjectGroup::getSceneGraph(SimObject* object)
{
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
   if (sceneObject)
      return sceneObject->getSceneGraph();

   else
   {
      t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (group)
         return group->getSceneGraph();
   }

   return NULL;
}

void t2dSceneObjectGroup::addObject(SimObject* object)
{
   if (object == this)
   {
      Con::errorf("t2dSceneObjectGroup::addObject - (%d) can't add self!", getId());
      return;
   }

   t2dSceneObjectGroup* testGroup = dynamic_cast<t2dSceneObjectGroup*>(object);
   if (testGroup)
   {
      if (testGroup->findChildObject(this))
      {
         Con::errorf("t2dSceneObjectGroup::addObject - (%d) can't add parent!", object->getId());
         return;
      }
   }

   t2dSceneObjectGroup* parentGroup = t2dSceneObjectGroup::getSceneObjectGroup(object);
   t2dSceneGraph* parentGraph = t2dSceneObjectGroup::getSceneGraph(object);

   if (parentGroup != this)
   {
      if (parentGroup)
      {
         parentGroup->removeObject(object);
      }
      else if (parentGraph)
      {
         parentGraph->removeObject(object);
      }

      t2dSceneObjectGroup::setSceneObjectGroup(object, this);
      Parent::addObject(object);

      if (mSceneGraph && (parentGraph != mSceneGraph))
         mSceneGraph->addToScene(object);
   }
}

void t2dSceneObjectGroup::removeObject(SimObject* object)
{
   if (t2dSceneObjectGroup::getSceneObjectGroup(object) == this)
   {
      t2dSceneObjectGroup::setSceneObjectGroup(object, NULL);
      Parent::removeObject(object);
   }
}

bool t2dSceneObjectGroup::findChildObject(SimObject* searchObject) const
{
   for (S32 i = 0; i < size(); i++)
   {
      SimObject* object = at(i);

      if (object == searchObject)
         return true;

      t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (group)
      {
         if (group->findChildObject(searchObject))
            return true;
      }
   }

   // If we make it here, no.
   return false;
}

void t2dSceneObjectGroup::write(Stream &stream, U32 tabStop, U32 flags)
{
   // Set a mount id on all objects.
   U32 mountID = 1;
   Vector<t2dSceneObject*> sceneObjects;
   getSceneObjects(sceneObjects);
   for (S32 i = 0; i < sceneObjects.size(); i++)
   {
      t2dSceneObject* pSceneObject2D = sceneObjects[i];

      // Don't reassign.
      if (!*pSceneObject2D->getDataField(StringTable->insert("mountID"), NULL))
      {
         char idString[8];
#ifdef PUAP_OPTIMIZE
         dItoa( mountID, idString);//-Mat put itoa instead of dsprintf 
#else
		 dSprintf(idString, 8, "%d", mountID);
#endif
		 pSceneObject2D->setDataField(StringTable->insert("mountID"), NULL, idString);
      }
      mountID++;
   }
   return Parent::write(stream, tabStop, flags);
}
