//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

#ifndef  _T2DSCENEOBJECTGROUP_H_
#define  _T2DSCENEOBJECTGROUP_H_

#include "T2D/t2dSceneObjectSet.h"
#include "core/tVector.h"
#include "T2D/t2dVector.h"
#include "T2D/t2dPhysics.h"
#include "core/color.h"

class t2dSceneGraph;

class t2dSceneObjectGroup: public t2dSceneObjectSet
{
private:
   typedef t2dSceneObjectSet Parent;

   t2dSceneGraph* mSceneGraph;
   t2dSceneObjectGroup* mSceneObjectGroup;

   static void setSceneObjectGroup(SimObject* object, t2dSceneObjectGroup* group);

public:
   DECLARE_CONOBJECT(t2dSceneObjectGroup);

   friend class t2dSceneGraph;

   t2dSceneObjectGroup();
   ~t2dSceneObjectGroup();

   virtual bool onAdd();
   virtual void onRemove();

   static t2dSceneObjectGroup* getSceneObjectGroup(SimObject* object);
   static t2dSceneGraph* getSceneGraph(SimObject* object);

   void addObject(SimObject* object);
   void removeObject(SimObject* object);
   bool findChildObject(SimObject* object) const;

   virtual void write(Stream &stream, U32 tabStop, U32 flags = 0);
   
   t2dSceneGraph* getSceneGraph() const { return mSceneGraph; };
   t2dSceneObjectGroup* getSceneObjectGroup() const { return mSceneObjectGroup; };
};

#endif
