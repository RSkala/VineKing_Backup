//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Trigger Object.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dTrigger.h"

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dTrigger);

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dTrigger::t2dTrigger() : T2D_Stream_HeaderID(makeFourCCTag('2','D','T','R'))
{
    // Setup some debug vector associations.
    VECTOR_SET_ASSOCIATION(mEnterColliders);
    VECTOR_SET_ASSOCIATION(mStayColliders);
    VECTOR_SET_ASSOCIATION(mLeaveColliders);

    // Reset Collider Maps..
    mColliderDoubleBufferMap[0].clear();
    mColliderDoubleBufferMap[1].clear();
    mCurrentColliderMapIndex = 0;

    // Set default callbacks.
    mEnterCallback = true;
    mStayCallback = false;
    mLeaveCallback = true;

    // Add default values to the default config. It would be nice if this could be in initPersistFields,
    // but the default config isn't created when that is called. This will work, but it's less than ideal.
    if (getDefaultConfig())
    {
      getDefaultConfig()->setDataField(StringTable->insert("EnterCallback"), NULL, "1");
      getDefaultConfig()->setDataField(StringTable->insert("StayCallback"), NULL, "0");
      getDefaultConfig()->setDataField(StringTable->insert("LeaveCallback"), NULL, "1");
    }
}


//-----------------------------------------------------------------------------
// Initialise Persistent Fields.
//-----------------------------------------------------------------------------
void t2dTrigger::initPersistFields()
{
   addProtectedField("EnterCallback", TypeBool, Offset(mEnterCallback, t2dTrigger), &setEnterCallback, &defaultProtectedGetFn, "");
   addProtectedField("StayCallback", TypeBool, Offset(mStayCallback, t2dTrigger), &setStayCallback, &defaultProtectedGetFn, "");
   addProtectedField("LeaveCallback", TypeBool, Offset(mLeaveCallback, t2dTrigger), &setLeaveCallback, &defaultProtectedGetFn, "");

   Parent::initPersistFields();
}


//-----------------------------------------------------------------------------
// Copy To.
//-----------------------------------------------------------------------------
void t2dTrigger::copyTo(SimObject* object)
{
   Parent::copyTo(object);

   AssertFatal(dynamic_cast<t2dTrigger*>(object), "t2dTrigger::copyTo - Copy object is not a t2dTrigger!");
   t2dTrigger* trigger = static_cast<t2dTrigger*>(object);

   trigger->mEnterCallback = mEnterCallback;
   trigger->mStayCallback = mStayCallback;
   trigger->mLeaveCallback = mLeaveCallback;
}


//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dTrigger::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Enable collisions by default.
    setCollisionActive(true, true);
    setNeverSolvePhysics(true);

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// ConsoleMethods.
// Set the callback status of the trigger.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dTrigger, setEnterCallback, void, 2, 3, "([setting]) Set whether trigger checks onEnter events\n"
			  "@param setting Default is true.\n"
			  "@return No return value.")
{
   // If the value isn't specified, the default is true.
   bool callback = true;
   if (argc > 2)
      callback = dAtob(argv[2]);

   object->setEnterCallback(callback);
}

ConsoleMethod(t2dTrigger, setStayCallback, void, 2, 3, "([setting]) Set whether trigger checks onStay events\n"
			  "@param setting Default is true.\n"
			  "@return No return value.")
{
   // If the value isn't specified, the default is true.
   bool callback = true;
   if (argc > 2)
      callback = dAtob(argv[2]);

   object->setStayCallback(callback);
}

ConsoleMethod(t2dTrigger, setLeaveCallback, void, 2, 3, "([setting]) Set whether trigger checks onLeave events\n"
			  "@param setting Default is true.\n"
			  "@return No return value.")
{
   // If the value isn't specified, the default is true.
   bool callback = true;
   if (argc > 2)
      callback = dAtob(argv[2]);

   object->setLeaveCallback(callback);
}

ConsoleMethod(t2dTrigger, getEnterCallback, bool, 2, 2, "() \n @return Returns whether trigger checks onEnter events")
{
   return object->getEnterCallback();
}

ConsoleMethod(t2dTrigger, getStayCallback, bool, 2, 2, "() \n @return Returns whether trigger checks onStay events")
{
   return object->getStayCallback();
}

ConsoleMethod(t2dTrigger, getLeaveCallback, bool, 2, 2, "() \n @return Returns whether trigger checks onLeave events")
{
   return object->getLeaveCallback();
}


//-----------------------------------------------------------------------------
// Pre Integrate Object.
//-----------------------------------------------------------------------------
void t2dTrigger::preIntegrate(const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats)
{
    // Call Parent.
    Parent::preIntegrate(sceneTime, elapsedTime, pDebugStats);

    // Anything to do?
    if ( !mEnterCallback && !mStayCallback && !mLeaveCallback )
    {
        // No, so reset everything and finish here.
        mColliderDoubleBufferMap[0].clear();
        mColliderDoubleBufferMap[1].clear();
        mCurrentColliderMapIndex = 0;
        return;
    }

    // Clear Current Collider Map.
    mColliderDoubleBufferMap[mCurrentColliderMapIndex].clear();

    // Clear Collider Callback Lists.
    mEnterColliders.clear();
    mStayColliders.clear();
    mLeaveColliders.clear();
}


//-----------------------------------------------------------------------------
// Post Integrate Object.
//-----------------------------------------------------------------------------
void t2dTrigger::postIntegrate( const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats )
{
    // Update Trigger.
    updateTrigger();

    // Call Parent.
    Parent::postIntegrate(sceneTime, elapsedTime, pDebugStats);
}


//-----------------------------------------------------------------------------
// On Collision Handler.
//-----------------------------------------------------------------------------
void t2dTrigger::onCollision( t2dPhysics::cCollisionStatus* collisionStatus )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dTrigger_onCollision);
#endif
    
    // Fetch Collide Object.
    t2dSceneObject* pCollideObject = collisionStatus->mDstObject;

    // Are we the object?
    if ( pCollideObject == this )
    {
        // Yes, so use the source instead.
        pCollideObject = collisionStatus->mSrcObject;
    }

    // Push into current collider map.
    mColliderDoubleBufferMap[mCurrentColliderMapIndex].insert( pCollideObject->getId(),  pCollideObject );

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dTrigger_onCollision
#endif

}


//-----------------------------------------------------------------------------
// Update Trigger.
//-----------------------------------------------------------------------------
void t2dTrigger::updateTrigger( void )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dTrigger_checkCollisions);
#endif

    // Reference Collider Map Double-Buffer.
    colliderMapType& currentColliderMap = mColliderDoubleBufferMap[mCurrentColliderMapIndex];
    colliderMapType& previousColliderMap = mColliderDoubleBufferMap[1-mCurrentColliderMapIndex];

    // Any collisions found?
    if ( currentColliderMap.size() > 0 )
    {
        // Calculate current/previous collider map correspondence.
        for ( colliderMapType::Iterator previousItr = previousColliderMap.begin(); previousItr != previousColliderMap.end(); ++previousItr )
        {
            // Fetch Object.
            t2dSceneObject* const pCollideObject = (*previousItr).value;

            // Is the object in the current collider map?
            if ( currentColliderMap.contains( (*previousItr).key ) )
            {
                // Yes, so it's an "OnStay" object.
                if ( mStayCallback )
                {
                    mStayColliders.push_back( pCollideObject );
                }
            }
            else
            {
                // No, so it's an "OnLeave" object.
                if ( mLeaveCallback )
                {
                    mLeaveColliders.push_back( pCollideObject );
                }
            }
        }

        // Processing "OnEnter" callback?
        if ( mEnterCallback )
        {
            // Yes, so did we have any previous colliders?
            if ( previousColliderMap.isEmpty() )
            {
                // No, so everything is an "OnEnter" object.
                for ( colliderMapType::Iterator currentItr = currentColliderMap.begin(); currentItr != currentColliderMap.end(); ++currentItr )
                {
                    mEnterColliders.push_back( (*currentItr).value );
                }
            }
            else
            {
                // Yes, so calculate current/previous collider map correspondence.
                for ( colliderMapType::Iterator currentItr = currentColliderMap.begin(); currentItr != currentColliderMap.end(); ++currentItr )
                {
                    // Is the object in the previous collider map?
                    if ( !previousColliderMap.contains( (*currentItr).key ) )
                    {
                        // No, so it's an "OnEnter" object.
                        mEnterColliders.push_back( (*currentItr).value );
                    }
                }
            }
        }
    }
    // No, so this is a special case that nothing entered, nothing stays but some objects may have left...
    else if ( mLeaveCallback && previousColliderMap.size() > 0 )
    {
        // Calculate current/previous collider map correspondence.
        for ( colliderMapType::Iterator previousItr = previousColliderMap.begin(); previousItr != previousColliderMap.end(); ++previousItr )
        {
            // It's an "OnLeave" object.
            mLeaveColliders.push_back( (*previousItr).value );
        }
    }


    collideCallbackType::iterator iter;

    // Perform "OnEnter" callback.
    if ( mEnterCallback && mEnterColliders.size() > 0 )
    {
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dTrigger_onEnterCallback);
#endif

        for (iter = mEnterColliders.begin(); iter != mEnterColliders.end(); ++iter)
        {
            Con::executef(this, 2, "onEnter", (*iter)->getIdString());
        }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dTrigger_onEnterCallback
#endif
    }

    // Perform "OnStay" callback.
    if ( mStayCallback && mStayColliders.size() > 0 )
    {
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dTrigger_onStayCallback);
#endif

        for (iter = mStayColliders.begin(); iter != mStayColliders.end(); ++iter)
        {
            Con::executef(this, 2, "onStay", (*iter)->getIdString());
        }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dTrigger_onStayCallback
#endif
    }

    // Perform "OnLeave" callback.
    if ( mLeaveCallback && mLeaveColliders.size() > 0 )
    {
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dTrigger_onLeaveCallback);
#endif

        for (iter = mLeaveColliders.begin(); iter != mLeaveColliders.end(); ++iter)
        {
            Con::executef(this, 2, "onLeave", (*iter)->getIdString());
        }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dTrigger_onLeaveCallback
#endif
    }

    // Flip collider map double-buffer.
    mCurrentColliderMapIndex = 1 - mCurrentColliderMapIndex;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dTrigger_checkCollisions
#endif
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dTrigger )
    REGISTER_SERIALISE_VERSION( t2dTrigger, 3, false )
REGISTER_SERIALISE_END()

// Implement Parent  Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dTrigger, 3 )


//-----------------------------------------------------------------------------
// Load v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dTrigger, 3 )
{
    bool enter, stay, leave;
    if  ((!stream.read(&enter)) || (!stream.read(&stay)) || (!stream.read(&leave)))
    {
        return false;
    }

    object->setEnterCallback(enter);
    object->setStayCallback(stay);
    object->setLeaveCallback(leave);

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dTrigger, 3 )
{
   if ((!stream.write(object->mEnterCallback)) || (!stream.write(object->mStayCallback)) || (!stream.write(object->mLeaveCallback)))
   {
      return false;
   }

    // Return Okay.
    return true;
}
