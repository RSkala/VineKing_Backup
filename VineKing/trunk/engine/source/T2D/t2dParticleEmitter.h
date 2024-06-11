//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Particle Emitter Object.
//-----------------------------------------------------------------------------

#ifndef _T2DPARTICLEEMITTER_H_
#define _T2DPARTICLEEMITTER_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif

#ifndef _T2DGRAPHFIELD_H_
#include "./t2dGraphField.h"
#endif

#ifndef _T2DIMAGEMAPDATABLOCK_H_
#include "./t2dImageMapDatablock.h"
#endif

#ifndef _T2DANIMATIONCONTROLLER_H_
#include "./t2dAnimationController.h"
#endif


///-----------------------------------------------------------------------------
/// Forward Declarations.
///-----------------------------------------------------------------------------
class t2dParticleEffect;


///-----------------------------------------------------------------------------
/// Particle Emitter Object 2D.
///-----------------------------------------------------------------------------
class t2dParticleEmitter : public SimObject
{
    typedef SimObject       Parent;

public:
    enum tEmitterOrientationMode { ALIGNED = 0, FIXED, RANDOM };
    enum tEmitterType { POINT = 0, LINEX, LINEY, AREA };

private:
    t2dParticleEffect*      pParentEffectObject;
    StringTableEntry        mEmitterName;
    t2dVector               mGlobalClipBoundary[4];
    F32                     mTimeSinceLastGeneration;
    bool                    mPauseEmitter;
    bool                    mVisible;
	F32						mParticlePref;

    /// Graph Selection.
    struct tGraphSelection
    {
        StringTableEntry    mGraphName;
        t2dGraphField*      mpGraphObject;
    };

    t2dGraphField*              mpCurrentGraph;
    StringTableEntry            mCurrentGraphName;
    Vector<tGraphSelection*>    mGraphSelectionList;


    /// Particle Node.
    struct tParticleNode
    {
        /// Particle Node Linkeage.
        tParticleNode*  mPreviousNode;
        tParticleNode*  mNextNode;

        /// Supress Movement.
        bool                    mSuppressMovement;

        /// Particle Components.
        F32                     mParticleLifetime;
        F32                     mParticleAge;
        t2dVector               mPosition;
        t2dVector               mVelocity;
        F32                     mOrientationAngle;
        t2dVector               mLocalClipBoundary[4];
        t2dMatrix               mRotationMatrix;
        t2dAnimationController  mAnimationController;

        /// Render Properties.
        t2dVector               mLastRenderSize;
        t2dVector               mRenderSize;
        F32                     mRenderSpeed;
        F32                     mRenderSpin;
        F32                     mRenderFixedForce;
        F32                     mRenderRandomMotion;

        /// Base Properties.
        t2dVector               mSize;
        F32                     mSpeed;
        F32                     mSpin;
        F32                     mFixedForce;
        F32                     mRandomMotion;
        ColorF                  mColour;    

        /// Interpolated Tick Position.
        t2dVector               mPreTickPosition;
        t2dVector               mPostTickPosition;
        t2dVector               mRenderTickPosition;
    };


    /// Particle Data.
    const U32               mParticlePoolBlockSize;
    Vector<tParticleNode*>  mParticlePool;
    tParticleNode*          mpFreeParticleNodes;
    tParticleNode           mParticleNodeHead;
    U32                     mActiveParticles;


    // Graph Properties.
    t2dGraphField_BV        mParticleLife;
    t2dGraphField_BV        mQuantity;
    t2dGraphField_BVL       mSizeX;
    t2dGraphField_BVL       mSizeY;
    t2dGraphField_BVL       mSpeed;
    t2dGraphField_BVL       mSpin;
    t2dGraphField_BVL       mFixedForce;
    t2dGraphField_BVL       mRandomMotion;
    t2dGraphField_BV        mEmissionForce;
    t2dGraphField_BV        mEmissionAngle;
    t2dGraphField_BV        mEmissionArc;
    t2dGraphField_L         mColourRed;
    t2dGraphField_L         mColourGreen;
    t2dGraphField_L         mColourBlue;
    t2dGraphField_L         mVisibility;

    /// Other Properties.
    bool                    mFixedAspect;
    t2dVector               mFixedForceDirection;
    F32                     mFixedForceAngle;
    tEmitterOrientationMode mParticleOrientationMode;
    F32                     mAlign_AngleOffset;
    bool                    mAlign_KeepAligned;
    F32                     mRandom_AngleOffset;
    F32                     mRandom_Arc;
    F32                     mFixed_AngleOffset;
    tEmitterType            mEmitterType;
    t2dAnimationController  mAnimationControllerProxy;
    t2dImageMapSmartPtr     mImageMapDataBlock;
    U32                     mImageMapFrame;
    StringTableEntry        mImageMapName;
    StringTableEntry        mAnimationName;
    bool                    mAnimationSelected;
    t2dVector               mPivotPoint;
    bool                    mUseEffectEmission;
    bool                    mLinkEmissionRotation;
    bool                    mIntenseParticles;
    bool                    mSingleParticle;
    bool                    mAttachPositionToEmitter;
    bool                    mAttachRotationToEmitter;
    bool                    mFirstInFrontOrder;

    /// Render Options.
    bool                    mBlending;
    S32                     mSrcBlendFactor;
    S32                     mDstBlendFactor;

    /// Collisions.
    bool                    mUseEmitterCollisions;


    void clearGraphSelections( void );
    void addGraphSelection( const char* graphName, t2dGraphField* pGraphObject );
    t2dGraphField* findGraphSelection( const char* graphName ) const;

public:
    t2dParticleEmitter();
    ~t2dParticleEmitter();

    /// Initialise.
    void initialise( t2dParticleEffect* pParentEffect );

    // Get Active Particle Count.
    U32 getActiveParticles( void ) const { return mActiveParticles; };

    void setParentEffect( t2dParticleEffect* parent ) { pParentEffectObject = parent; };
    t2dParticleEffect* getParentEffect() { return pParentEffectObject; };

    /// ***********************************************************
    /// Script Commands.
    /// ***********************************************************

    /// Graph Editing.
    void selectGraph( const char* graphName );
    S32 addDataKey( F32 time, F32 value );
    bool removeDataKey( S32 index );
    bool clearDataKeys( void );
    bool setDataKeyValue( S32 index, F32 value );
    F32 getDataKeyValue( S32 index ) const;
    F32 getDataKeyTime( S32 index ) const;
    U32 getDataKeyCount( void ) const;
    F32 getMinValue( void ) const;
    F32 getMaxValue( void ) const;
    F32 getMinTime( void ) const;
    F32 getMaxTime( void ) const;
    F32 getGraphValue( F32 time ) const;
    bool setTimeRepeat( F32 timeRepeat );
    F32 getTimeRepeat( void ) const;
    bool setValueScale( const F32 valueScale );
    F32 getValueScale( void ) const;

    /// Set Properties.
    void setVisible( bool status );
    void setEmitterName( const char* emitterName );
    void setEmitterType( tEmitterType emitterType );
    void setParticleOrientationMode( tEmitterOrientationMode particleOrientationMode );
    void setAlignAngleOffset( F32 angleOffset );
    void setAlignKeepAligned( bool keepAligned );
    void setFixedAngleOffset( F32 angleOffset );
    void setRandomAngleOffset( F32 angleOffset );
    void setRandomArc( F32 arc );
    void setPivotPoint( t2dVector pivotPoint );
    void setFixedForceAngle( F32 fixedForceAngle );
    bool setImageMap( const char* imageMapName, U32 frame );
    bool setAnimationName( const char* animationName );
    void setFixedAspect( bool aspect );
    void setIntenseParticles( bool intenseParticles );
    void setSingleParticle( bool singleParticle );
    void setAttachPositionToEmitter( bool attachPositionToEmitter );
    void setAttachRotationToEmitter( bool attachRotationToEmitter );
    void setUseEffectEmission( bool useEffectEmission );
    void setLinkEmissionRotation( bool linkEmissionRotation );
    void setFirstInFrontOrder( bool firstInFrontOrder );
    void setBlending( bool status, S32 srcBlendFactor, S32 dstBlendFactor );


    /// Get Properties.
    bool getVisible( void ) const;
    const char* getEmitterName( void ) const;
    F32 getFixedForceAngle( void ) const;
    const char* getParticleOrientation( void ) const;
    F32 getAlignAngleOffset( void ) const;
    bool getAlignKeepAligned( void ) const;
    F32 getFixedAngleOffset( void ) const;
    F32 getRandomAngleOffset( void ) const;
    F32 getRandomArc( void ) const;
    const char* getEmitterType( void ) const;
    const char* getImageMapNameFrame( void ) const;
    const char* getAnimationName( void ) const;
    const char* getPivotPoint( void ) const;
    bool getFixedAspect( void ) const;
    bool getIntenseParticles( void ) const;
    bool getSingleParticle( void ) const;
    bool getAttachPositionToEmitter( void ) const;
    bool getAttachRotationToEmitter( void ) const;
    bool getUseEffectEmission( void ) const;
    bool getLinkEmissionRotation( void ) const;
    bool getFirstInFrontOrder( void ) const;
    bool getUsingAnimation( void ) const;
    bool getBlendingStatus( void ) const { return mBlending; };
    S32 getSrcBlendFactor( void ) const { return mSrcBlendFactor; };
    S32 getDstBlendFactor( void ) const { return mDstBlendFactor; };

    /// Load/Save Emitter.
    bool loadEmitter( const char* emitterFile );
    bool saveEmitter( const char* emitterFile );

    /// ***********************************************************
    /// End of Script Commands.
    /// ***********************************************************


    /// Emitter Control.
    void playEmitter( bool resetParticles );
    void stopEmitter( void );
    void pauseEmitter( void );

    /// Remove All Particles.
    void freeAllParticles( void );

    /// Particle Block Control.
    tParticleNode* createParticle( void );
    void freeParticle( tParticleNode* pParticleNode );
    void createParticlePoolBlock( void );
    void destroyParticlePool( void );

    /// Particle Creation/Integration.
    inline void configureParticle( tParticleNode* pParticleNode );
    inline void integrateParticle( tParticleNode* pParticleNode, F32 particleAge, F32 elapsedTime );
    
    /// Collisions.
    void setEmitterCollisionStatus( const bool status );
    bool getEmitterCollisionStatus( void );

    /// Check Particle Collisions.
    bool checkParticleCollisions( const t2dParticleEffect* pParentEffect, const F32 elapsedTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats );

    /// Integration.
    void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    void interpolateTick( const F32 timeDelta );

    /// Object Rendering.
    void renderObject( const RectF& viewPort, const RectF& viewIntersection );

    virtual void copyTo(SimObject* object);

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dParticleEmitter );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dParticleEmitter, 1);
    DECLARE_T2D_LOADSAVE_METHOD( t2dParticleEmitter, 2);
    DECLARE_T2D_LOADSAVE_METHOD( t2dParticleEmitter, 3);

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dParticleEmitter);
};

#endif // _T2DPARTICLEEMITTER_H_
