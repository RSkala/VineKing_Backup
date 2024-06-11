//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Particle Effect Object.
//-----------------------------------------------------------------------------

#ifndef _T2DPARTICLEEFFECT_H_
#define _T2DPARTICLEEFFECT_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif

#ifndef _T2DGRAPHFIELD_H_
#include "./t2dGraphField.h"
#endif

#ifndef _T2DPARTICLEEMITTER_H_
#include "./t2dParticleEmitter.h"
#endif

///-----------------------------------------------------------------------------
/// Structures.
///-----------------------------------------------------------------------------
struct tEmitterHandle
{
   SimObjectId     mObjectId;
   SimObject*      mpSceneObject;
};

///-----------------------------------------------------------------------------
/// Types.
///-----------------------------------------------------------------------------
typedef Vector<tEmitterHandle> typeEmitterVector;


///-----------------------------------------------------------------------------
/// Particle Effect Object 2D.
///-----------------------------------------------------------------------------
class t2dParticleEffect : public t2dSceneObject
{
friend class t2dParticleEmitter;

    typedef t2dSceneObject      Parent;

private:
    StringTableEntry mEffectFile;

    typeEmitterVector                mParticleEmitterList;                                   ///< Emitter List.

    bool                            mEffectPlaying;
    F32                             mEffectAge;
    F32                             mEffectLifetime;

    bool                            mInitialised;
    bool                            mWaitingForParticles;
    bool                            mWaitingForDelete;
    bool                            mUseEffectCollisions;

    U32                             mEmitterSerial;

	bool							mDisableParticleInterpolation;
	bool							mWasPlaying;//%PUAP% used to stop playing when off-screen

    /// Graph Selection.
    struct tGraphSelection
    {
        StringTableEntry        mGraphName;
        t2dGraphField*          mpGraphObject;
    };

    t2dGraphField*              mpCurrentGraph;
    StringTableEntry            mCurrentGraphName;
    Vector<tGraphSelection*>    mGraphSelectionList;

    /// Graph Selection.
    void clearGraphSelections( void );
    void addGraphSelection( const char* graphName, t2dGraphField* pGraphObject );
    t2dGraphField* findGraphSelection( const char* graphName ) const;


public:
    /// Graph Properties.
    t2dGraphField_B     mParticleLife;
    t2dGraphField_B     mQuantity;
    t2dGraphField_B     mSizeX;
    t2dGraphField_B     mSizeY;
    t2dGraphField_B     mSpeed;
    t2dGraphField_B     mSpin;
    t2dGraphField_B     mFixedForce;
    t2dGraphField_B     mRandomMotion;
    t2dGraphField_BV    mEmissionForce;
    t2dGraphField_BV    mEmissionAngle;
    t2dGraphField_BV    mEmissionArc;
    t2dGraphField_B     mVisibility;

    /// Effect Life Mode.
    enum eEffectLifeMode
    {
        INFINITE,
        CYCLE,
        KILL,
        STOP

    } mEffectLifeMode;

    t2dParticleEffect();
    virtual ~t2dParticleEffect();

    static void initPersistFields();
    static bool setEffectFile(void* obj, const char* data);
    static bool setUseEffectCollisions(void* obj, const char* data) { static_cast<t2dParticleEffect*>(obj)->setEffectCollisionStatus(dAtob(data)); return false; };

    virtual void readFieldsPostLoad();

    // Initialise.
    void initialise( void );

    // Emitter Indices.
    bool isEmitterValid( SimObjectId emitterID ) const;
    S32 findEmitterIndex( SimObjectId emitterID ) const;


    /// ***********************************************************
    /// Script Commands.
    /// ***********************************************************

    /// Graph Editing.
    void selectGraph( const char* graphName );
    S32 addDataKey( F32 time, F32 value );
    bool removeDataKey( S32 index );
    bool clearDataKeys( void );
    bool setDataKey( S32 index, F32 value );
    F32 getDataKeyValue( S32 index ) const;
    F32 getDataKeyTime( S32 index ) const;
    U32 getDataKeyCount( void ) const;
    F32 getMinValue( void ) const;
    F32 getMaxValue( void ) const;
    F32 getMinTime( void ) const;
    F32 getMaxTime( void ) const;
    F32 getGraphValue( F32 time ) const;
    bool setTimeRepeat( const F32 timeRepeat );
    F32 getTimeRepeat( void ) const;
    bool setValueScale( const F32 valueScale );
    F32 getValueScale( void ) const;

    /// Emitter Objects.
    t2dParticleEmitter* addEmitter( t2dParticleEmitter* pEmitter = NULL );
    void removeEmitter( SimObjectId emitterID, bool deleteEmitter = true );
    void clearEmitters( void );
    S32 getEmitterCount( void ) const;
    SimObjectId findEmitterObject( const char* emitterName ) const;
    SimObjectId getEmitterObject( S32 index ) const;
    void moveEmitter( S32 fromIndex, S32 toIndex );

    /// Effect Control.
    bool playEffect( bool resetParticles );
    void stopEffect( bool waitForParticles, bool killEffect );
    bool getIsEffectPlaying() { return mEffectPlaying; };
    bool moveEffectTo( const F32 moveTime, const F32 timeStep, U32& peakCount, F32& peakTime );
    bool findParticlePeak( const F32 searchTime, const F32 timeStep, const U32 peakLimit, U32& peakCount, F32& peakTime );


    // Effect Life Mode.
    void setEffectLifeMode( eEffectLifeMode lifeMode, F32 time );
    eEffectLifeMode getEffectLifeMode( void ) const { return mEffectLifeMode; };
    F32 getEffectLifeTime( void ) const { return mEffectLifetime; };
    void setEffectLifeTime( F32 time ) { mEffectLifetime = getMax( time, 0.f ); };
    // Properties
    static bool setEffectMode(void* obj, const char* data);
    static bool setEffectModeTime(void* obj, const char* data)  { static_cast<t2dParticleEffect*>(obj)->setEffectLifeTime(dAtof(data)); return false; };


    /// Load/Save Effect.
    bool loadEffect( const char* effectFile );
    bool saveEffect( const char* effectFile );

    /// Collisions.
    void setEffectCollisionStatus( const bool status );
    bool getEffectCollisionStatus( void );


    /// ***********************************************************
    /// End of Script Commands.
    /// ***********************************************************

    virtual bool checkCollisionSend( const F32 elapsedTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats );

    /// Integration.
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void interpolateTick( const F32 timeDelta );

    virtual void safeDelete();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );

    virtual void copyTo(SimObject* object);

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dParticleEffect );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dParticleEffect, 1 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dParticleEffect, 2 );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dParticleEffect);
};

#endif // _T2DPARTICLEEFFECT_H_
