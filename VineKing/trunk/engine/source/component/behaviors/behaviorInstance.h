//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _BEHAVIORINSTANCE_H_
#define _BEHAVIORINSTANCE_H_

#include "console/simBase.h"
#include "component/simComponent.h"

// Forward refs
class BehaviorTemplate;

//////////////////////////////////////////////////////////////////////////
/// An Instance of a given Object Behavior
///
/// A BehaviorInstance object is created from a BehaviorTemplate object 
/// that defines it's Default Values, 
class BehaviorInstance : public SimObject
{
   typedef SimObject Parent;

protected:
   BehaviorTemplate *mTemplate;
   SimComponent     *mBehaviorOwner;

public:
   BehaviorInstance(BehaviorTemplate *btemplate = NULL);
   virtual ~BehaviorInstance();
   DECLARE_CONOBJECT(BehaviorInstance);

   virtual bool onAdd();
   virtual void onRemove();
   static void initPersistFields();

   // This must be called before the object is registered with the sim
   void setBehaviorClass(StringTableEntry className, StringTableEntry superClass)
   {
      mClassName = className;
      mSuperClassName = superClass;
   }

   void setBehaviorOwner( SimComponent*pOwner ) { mBehaviorOwner = pOwner; };
   inline SimComponent *getBehaviorOwner() { return mBehaviorOwner ? mBehaviorOwner : NULL; };

   // Read-Only field accessor (never allows write)
   static bool setOwner( void* obj, const char* data ) { return true; };

   BehaviorTemplate *getTemplate()        { return mTemplate; }
   const char *getTemplateName();

   virtual void packToStream( Stream &stream, U32 tabStop, U32 flags = 0 );
};

#endif // _BEHAVIORINSTANCE_H_
