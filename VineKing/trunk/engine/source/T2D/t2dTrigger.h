//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Trigger Object.
//-----------------------------------------------------------------------------

#ifndef _T2DTRIGGER_H_
#define _T2DTRIGGER_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./T2D/t2dSceneObject.h"
#endif

#include "console/tHashTable.h"

///-----------------------------------------------------------------------------
/// Trigger 2D.
///-----------------------------------------------------------------------------
class t2dTrigger : public t2dSceneObject
{
   typedef t2dSceneObject Parent;

public:
    t2dTrigger();
    virtual ~t2dTrigger() {};

    /// Simulation Occupancy.
    virtual bool onAdd();

    /// Callback Management.
    inline void setEnterCallback(bool enter = true) { mEnterCallback = enter; };
    inline void setStayCallback(bool stay = true) { mStayCallback = stay; };
    inline void setLeaveCallback(bool leave = true) { mLeaveCallback = leave; };
    inline bool getEnterCallback() { return mEnterCallback; };
    inline bool getStayCallback() { return mStayCallback; };
    inline bool getLeaveCallback() { return mLeaveCallback; };

    /// Integration.
    virtual void preIntegrate( const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats );
    virtual void postIntegrate( const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats );

    /// Collision.
    virtual void onCollision( t2dPhysics::cCollisionStatus* collisionStatus );

    /// Object Cloning.
    virtual void copyTo(SimObject* object);

    /// Field Persistence.
    static void initPersistFields();

    /// Callback Management.
    static bool setEnterCallback(void* obj, const char* data) { static_cast<t2dTrigger*>(obj)->setEnterCallback(dAtob(data)); return false; };
    static bool setStayCallback(void* obj, const char* data)  { static_cast<t2dTrigger*>(obj)->setStayCallback(dAtob(data)); return false; };
    static bool setLeaveCallback(void* obj, const char* data) { static_cast<t2dTrigger*>(obj)->setLeaveCallback(dAtob(data)); return false; };

    /// Serialisation.
    DECLARE_T2D_SERIALISE( t2dTrigger );
    DECLARE_T2D_LOADSAVE_METHOD( t2dTrigger, 3 );
    DECLARE_CONOBJECT( t2dTrigger );


private:
    /// Update Trigger State.
    void updateTrigger( void );

    /// Callback Options.
    bool mEnterCallback;
    bool mStayCallback;
    bool mLeaveCallback;

    /// Object Mapping Database.
    typedef tHashMap<SimObjectId, t2dSceneObject*> colliderMapType;
    typedef VectorPtr<t2dSceneObject*> collideCallbackType;
    colliderMapType mColliderDoubleBufferMap[2];
    collideCallbackType mEnterColliders;
    collideCallbackType mStayColliders;
    collideCallbackType mLeaveColliders;
    S32 mCurrentColliderMapIndex;
};

#endif // _T2DTRIGGER_H_
