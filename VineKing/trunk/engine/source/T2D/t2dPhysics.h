//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Physics Object.
//-----------------------------------------------------------------------------

#ifndef _T2DPHYSICS_H_
#define _T2DPHYSICS_H_

#ifndef _T2DSCENEGRAPH_H_
#include "./t2dSceneGraph.h"
#endif

#ifndef _FILESTREAM_H_
#include "core/fileStream.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _T2DMATRIX_H_
#include "t2dMatrix.h"
#endif

#ifndef _T2DUTILITY_H_
#include "./t2dUtility.h"
#endif

#ifndef _T2DSERIALISATION_H_
#include "./t2dSerialise.h"
#endif


///-----------------------------------------------------------------------------
/// Collision-Material Datablock 2D.
///-----------------------------------------------------------------------------
class t2dCollisionMaterialDatablock : public t2dBaseDatablock
{
public:
    typedef t2dBaseDatablock Parent;


    t2dCollisionMaterialDatablock();
    virtual ~t2dCollisionMaterialDatablock();

    static void  initPersistFields();
    virtual bool onAdd();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    F32                 mCoefDynamicFriction;       ///< Coefficient of Dynamic Friction.
    F32                 mCoefRestitution;           ///< Coefficient of Restitution.
    F32                 mDensity;                   ///< Unit Density.
    F32                 mMass;                      ///< Mass.
    F32                 mInertialMoment;            ///< Inertial Moment.
    F32                 mForceScale;                ///< Force Scale.
    F32                 mDamping;                   ///< Damping.
    bool                mAutoMassInertia;           ///< Auto Mass/Inertia.
    bool                mImmovable;                 ///< Immovable.

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dCollisionMaterialDatablock);
};



///-----------------------------------------------------------------------------
/// Physics Handler.
///
/// Many thanks to Olivier Renault and Kee-Tip Chan for your advice and guidance
/// through this minefield of simulation!
/// 
/// @note Some of the few references used to generate this code...
///
/// http://www.d6.com/users/checker/pdfs/gdmphys1.pdf
/// http://www.d6.com/users/checker/pdfs/gdmphys2.pdf
/// http://www.d6.com/users/checker/pdfs/gdmphys3.pdf
/// http://www.d6.com/users/checker/pdfs/gdmphys4.pdf
/// http://www.d6.com/users/checker/dynamics.htm (excellent!)
/// http://www-2.cs.cmu.edu/afs/cs.cmu.edu/user/baraff/www/sigcourse/index.html
/// http://www.magic-software.com/Documentation/MethodOfSeparatingAxes.pdf
/// http://www.cs.berkeley.edu/~jfc/papers/94/ibds94.pdf
/// http://portaldemo.narod.ru/physics.html
/// http://www.rowlhouse.co.uk/jiglib/
/// http://www.cs.unc.edu/~ehmann/RigidTutorial/
/// http://www.gdconf.com/archives/2004/baltman_rick.pdf
/// http://www.gamasutra.com/features/19991018/Gomez_1.htm
/// http://www.gamasutra.com/features/19990702/data_structures_01.htm
/// http://www.cs.cmu.edu/~baraff/papers/sig91.pdf
///
/// Restitution
/// http://carini.physics.indiana.edu/E105/cor.html
///
/// Static/Dynamic Friction + Tables:-
/// http://www.roymech.co.uk/Useful_Tables/Tribology/co_of_frict.htm#coef
///
/// Swept Circle/Ellipse...
/// http://www.gamedev.net/reference/articles/article1026.asp
/// http://www.opentnl.org/ specifically http://www.three14.demon.nl/sweptellipsoid/SweptEllipsoid.pdf
///
///-----------------------------------------------------------------------------

class t2dPhysics
{
public:
    enum { MAX_COLLISION_POLY_VERTEX = 64 };    ///< Maximum Collision Poly Vertices.
    enum { MAX_COLLISION_CONTACTS = 2 };        ///< Maximum Collision Contact-Points.

    enum eCollisionDetection
    {
        T2D_DETECTION_FULL,                     ///< Swept Circle + Poly.
        T2D_DETECTION_CIRCLE_ONLY,              ///< Swept Circle Only.
        T2D_DETECTION_POLY_ONLY,                ///< Swept Poly Only.
        T2D_DETECTION_CUSTOM,                   ///< Custom.

        T2D_DETECTION_INVALID,                  ///< Error!
    };

    /// Collision Responses.
    enum eCollisionResponse
    {
        T2D_RESPONSE_OFF,

        T2D_RESPONSE_RIGID,                     ///< Rigid-Body Collision.
        T2D_RESPONSE_BOUNCE,                    ///< Perfect Inelastic Collision.
        T2D_RESPONSE_CLAMP,                     ///< Perfect Elastic Collision.
        T2D_RESPONSE_STICKY,                    ///< Stop Object.
        T2D_RESPONSE_KILL,                      ///< Kill Object.
        T2D_RESPONSE_CUSTOM,                    ///< Customised Collision.

        T2D_RESPONSE_INVALID,                   ///< Error!
    };

    /// Collision Status.
    class cCollisionStatus
    {
    public:
       cCollisionStatus() : mHandled(false), mValidCollision(false), mOverlapped(false) {};

        bool                mHandled;
        bool                mValidCollision;
        bool                mOverlapped;
        bool                mSrcSolve;
        bool                mDstSolve;
        t2dSceneObject*     mSrcObject;
        t2dSceneObject*     mDstObject;
        t2dPhysics*         mpSrcPhysics;
        t2dPhysics*         mpDstPhysics;
        t2dVector           mCollisionNormal;
        F32                 mCollisionTimeNorm;
        F32                 mCollisionTimeReal;
        F32                 mFullTimeStep;
        U32                 mContactCount;
        t2dVector           mSrcContacts[MAX_COLLISION_CONTACTS];
        t2dVector           mDstContacts[MAX_COLLISION_CONTACTS];
    };


    t2dPhysics();
    virtual ~t2dPhysics();

    void initialise( SimObject* pParent, const char* pRefMetaString );

    /// Set Properties.
    static bool isPolyConvex( const U32 polyVertexCount, const t2dVector* pPoly, bool& sign );
    void setCollisionPolyPrimitive( const U32 polyVertexCount );
    void setCollisionPolyCustom( const U32 polyVertexCount, const t2dVector* pCustomPolygon );
    void setCollisionDetectionMode( const eCollisionDetection detectionMode );
    void setCollisionResponseMode( const eCollisionResponse responseMode );
    void setCollisionCircleScale( const F32 scale );
    void setCollisionPolyScale( const t2dVector& scale );
    void setCollisionCircleSuperscribed( bool superscribeMode ); 

    void setSolveOverlap( const bool status );
    void setPhysicsSuppress( const bool status );
    void setPosition( const t2dVector& position );
    void setSize( const t2dVector& size );
    void setRotation( const F32 rotation );
    void setAutoRotation( const F32 autoRotation );
    void setForwardMovementOnly( const bool forwardMovementOnly );
    void setPositionTarget( const t2dVector positionTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin );
    void setPositionTargetOff( void );
    void setRotationTarget( const F32 rotationTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin );
    void setRotationTargetOff( void );
    inline void setNetLinearVelocity( const t2dVector& velocity );
    inline void setNetAngularVelocity( const F32& velocity );
    void setGrossLinearVelocity( const t2dVector& velocity );
    void setGrossAngularVelocity( const F32& velocity );
    void setMinLinearVelocity( const F32 minVelocity );
    void setMinAngularVelocity( const F32 minVelocity );
    void setMaxLinearVelocity( const F32 maxVelocity );
    void setMaxAngularVelocity( const F32 maxVelocity );
    void setConstantForce( const t2dVector& force, bool gravitic );
    void setForceScale( const F32 forceScale );
    void setAtRest( void );
    void setCollisionMaxIterations( const U32 iterations );

    void setDensity( const F32 density );
    void setMass( const F32 mass );
    void setInertialMoment( const F32 moment );
    void setDynamicFriction( const F32 dynamicfriction );
    void setRestitution( const F32 restitution );
    void setDamping( const F32 damping );
    void setAutoMassInertia( const bool advancedMode );
    void setImmovable( const bool immovable );

    /// Tick Process Control.
    void resetTickPosition( void );
    void updateTickPosition( void );
    void updateTickPosition( const t2dVector& position );
    void interpolateTickPosition( const F32 timeDelta );
    void resetTickRotation( void );
    void updateTickRotation( void );
    void updateTickRotation( const F32& rotation );
    void interpolateTickRotation( const F32 timeDelta );
    void updateRotationMatrix( F32 rotation );

    /// Tick Processing.
    inline const t2dVector& getPreTickPosition( void ) const            { return mPreTickPosition; };
    inline const t2dVector& getPostTickPosition( void ) const           { return mPostTickPosition; };
    inline const t2dVector& getRenderTickPosition( void ) const         { return mRenderTickPosition; };
    inline const F32& getPreTickRotation( void ) const                  { return mPreTickRotation; };
    inline const F32& getPostTickRotation( void ) const                 { return mPostTickRotation; };
    inline const F32& getRenderTickRotation( void ) const               { return mRenderTickRotation; };

    /// Get Properties.
    inline const bool& getSolveOverlap( void ) const                    { return mSolveOverlap; };
    inline const bool& getPhysicsSuppress( void ) const                 { return mPhysicsSuppress; };
    inline const t2dVector& getPosition( void ) const                   { return mPosition; };
    inline const t2dVector& getSize( void ) const                       { return mSize; };
    inline const t2dVector& getHalfSize( void ) const                   { return mHalfSize; };
    inline const t2dVector& getCollisionPolyScale( void ) const         { return mCollisionPolyScale; };
    inline const F32& getCollisionCircleScale( void ) const             { return mCollisionCircleScale; };
    inline const bool& getCollisionCircleSuperscribed( void ) const     { return mCollisionCircleSuperscribed; };
    inline const F32& getCollisionCircleRadii( void ) const             { return mCollisionCircleRadii; };
    inline const F32& getRotation( void ) const                         { return mRotation; };
    inline const F32& getAutoRotation( void ) const                     { return mAutoRotation; };
    inline const bool& getForwardMovementOnly( void ) const             { return mForwardMovementOnly; };

    inline const bool& getPositionTargetActive( void ) const            { return mPositionTargetActive; };
    inline const t2dVector& getPositionTarget( void ) const             { return mPositionTarget; }
    inline const bool& getPositionTargetAutoStop( void ) const          { return mPositionTargetAutoStop; };
    inline const bool& getPositionTargetCallback( void ) const          { return mPositionTargetCallback; };
    inline const bool& getPositionTargetSnap( void ) const              { return mPositionTargetSnap; };
    inline const F32& getPositionTargetMargin( void ) const             { return mPositionTargetMargin; };

    inline const bool& getRotationTargetActive( void ) const            { return mRotationTargetActive; };
    inline const F32& getRotationTarget( void ) const                   { return mRotationTarget; }
    inline const bool& getRotationTargetAutoStop( void ) const          { return mRotationTargetAutoStop; };
    inline const bool& getRotationTargetCallback( void ) const          { return mRotationTargetCallback; };
    inline const bool& getRotationTargetSnap( void ) const              { return mRotationTargetSnap; };
    inline const F32& getRotationTargetMargin( void ) const             { return mRotationTargetMargin; };

    inline const t2dVector& getConstantForce( void ) const              { return mConstantForce; };
    inline const bool&  getIsConstantForceActive( void ) const          { return mConstantForceActive; };
    inline const bool& getGraviticConstantForceFlag( void ) const       { return mGraviticConstantForce; };
    inline const U32& getCollisionMaxIterations( void ) const           { return mCollisionMaxIterations; };

    inline const F32& getDynamicFriction( void ) const                  { return mCoefDynamicFriction; };
    inline const F32& getRestitution( void ) const                      { return mCoefRestitution; };
    inline const F32& getDensity( void ) const                          { return mDensity; };
    inline const F32& getMass( void ) const                             { return mMass; };
    inline const F32& getInertialMoment( void ) const                   { return mInertialMoment; };
    inline const F32& getForceScale( void ) const                       { return mForceScale; };
    inline const F32& getDamping( void ) const                          { return mDamping; };
    inline const bool& getAutoMassInertia( void ) const                 { return mAutoMassInertia; };
    inline const bool getImmovable( void ) const                        { return mImmovable || mIsZero(mMass); };   ///< We'll remove the mass-check eventually as it's deprecated.

    inline const F32& getInverseMass( void ) const                      { return mInverseMass; };
    inline const F32& getInverseInertialMoment( void ) const            { return mInverseInertialMoment; };

    inline const t2dVector& getNetLinearVelocity( void ) const          { return mNetLinearVelocity; };
    inline const F32& getNetAngularVelocity( void ) const               { return mNetAngularVelocity; };
    inline const t2dVector& getGrossLinearVelocity( void ) const        { return mGrossLinearVelocity; };
    inline const F32& getGrossAngularVelocity( void ) const             { return mGrossAngularVelocity; };
    inline const F32& getMinLinearVelocity( void ) const                { return mMinLinearVelocity; };
    inline const F32& getMinAngularVelocity( void ) const               { return mMinAngularVelocity; };
    inline const F32& getMaxLinearVelocity( void ) const                { return mMaxLinearVelocity; };
    inline const F32& getMaxAngularVelocity( void )  const              { return mMaxAngularVelocity; };

    inline const eCollisionDetection& getCollisionDetectionMode(void) const { return mCollisionDetectionMode; };
    inline const eCollisionResponse& getCollisionResponseMode(void) const   { return mCollisionResponseMode; };

    inline const t2dVector* getCollisionPolyBasis( void ) const         { return &(mCollisionPolyBasisList[0]); };
    inline const t2dVector* getCollisionPolyLocal( void ) const         { return &(mCollisionPolyLocalList[0]); };
    inline const U32 getCollisionPolyCount( void ) const                { return mCollisionPolyLocalList.size(); };
    inline const t2dMatrix& getRotationMatrix() const                   { return mMatrixRotation; };
    inline const t2dMatrix& getInverseRotationMatrix() const            { return mInverseMatrixRotation; };

    inline SimObject* getRefParentObject( void ) const                  { return mRefParentObject; };
    inline const SimObjectId getRefParentId( void ) const               { return mRefParentObject->getId(); };
    inline StringTableEntry getRefMetaString( void ) const              { return mRefMetaString; };

    inline const bool getAtRest( void ) const                           { return ( mIsZero(mGrossLinearVelocity.lenSquared()) && mIsZero(mFabs(mGrossAngularVelocity)) ); };
    inline const bool& isInitialised( void ) const                      { return mInitialised; };

    inline void generateCollisionCircle( void );
    void generateCollisionPoly( void );
    inline void calculateMassInertia( void );

    /// Forces.
    void addNetLinearForce( const t2dVector& force );
    void addNetAngularForce( const F32& force );
    void addNetTorqueForce( const t2dVector& force, const t2dVector& forcePosition );
    void addGrossLinearForce( const t2dVector& force );
    void addGrossAngularForce( const F32& force );
    void addGrossTorqueForce( const t2dVector& force, const t2dVector& forcePosition );
    void resetAngularVelocity( void );
    void resetLinearVelocity( void );
    void resetNetVelocity( void );
    void resetGrossVelocity( void );

    /// Spatial Dirty Control.
    void setSpatialDirty( void ) { mSpatialDirty = true; };
    void resetSpatialDirty( void ) { mSpatialDirty = false; };
    bool getSpatialDirty( void ) const { return mSpatialDirty; };

    /// Check Position/Rotation Targets.
    F32 checkPositionTarget( const F32 elapsedTime );
    inline void updatePositionTarget( const F32 elapsedTime );
    inline void updateRotationTarget( const F32 angularVelocity, const F32 elapsedTime );
    inline void processPositionTarget( void );
    inline void processRotationTarget( void );

    /// Collision Calculation.
    static bool calculateCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus );
    
    /// Swept-Circle to Circle Collision Check.
    static bool sweptCircleToCircleCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus );
    /// Swept-Circle to Polygon Collision Check.
    static bool sweptCircleToPolyCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus );
    /// Swept-Polygon to Circle Collision Check.
    static bool sweptPolyToCircleCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus );
    /// Swept-Polygon to Polygon Collision Check.
    static bool sweptPolyToPolyCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus );


    /// Check for Interval Intersections.
    static bool checkIntervalIntersection(  const t2dVector* srcPoly, U32 srcPolyCount,
                                            const t2dVector* dstPoly, U32 dstPolyCount,
                                            const t2dVector& vertexAxis,
                                            const t2dVector& refLocalOffset, const t2dVector& refLocalVelocity, const t2dMatrix& refLocalRotation,
                                            F32& timeAxis, const F32 collisionTime );

    /// Calculate Polygon Interval.
    static void calculateInterval( const t2dVector* pPoly, const U32 polyCount, const t2dVector& axii, F32& minProj, F32& maxProj );

    /// Find Minimum Seperating Distance.
    static bool findMinimumSeperation(t2dVector* pVertexAxis, F32* pTimeAxis, U32 axesCount, t2dVector& collisionNormal, F32& collisionTime );

    /// Find Collision Contacts.
    static bool findContactPoints(  const t2dVector* srcPoly, U32 srcPolyCount, const t2dVector& srcPosition, const t2dVector& srcVelocity, const t2dMatrix& srcRotation,
                                    const t2dVector* dstPoly, U32 dstPolyCount, const t2dVector& dstPosition, const t2dVector& dstVelocity, const t2dMatrix& dstRotation,
                                    cCollisionStatus* pCollisionStatus );

    /// Find Support Points.
    static U32 findSupportPoints(   const t2dVector* pPoly, U32 polyCount,
                                    const t2dVector& position, const t2dVector& velocity, const t2dMatrix& rotation,
                                    const t2dVector& collisionNormal, F32 collisionTime,
                                    t2dVector* pSupportPoints );

    /// Transform Contact.
    static void transformContact( const t2dVector& vertex, const t2dVector& position, const t2dVector& velocity, const t2dMatrix& rotation, F32 collisionTime, t2dVector& contact );

    /// Projection of Points to Segments.
    static bool projectPointToSegment( const t2dVector& V, const t2dVector& A, const t2dVector& B, t2dVector& W, F32* pTime = NULL );

    /// Reverse Collision Status.
    static void reverseCollisionStatus( const cCollisionStatus* pCollisionStatus, cCollisionStatus* pReverseCollisionStatus );

    /// Get Smoothed Corner Normal.
    static void getSmoothedCornerNormal( const t2dVector* pVertices, const U32 numVertices, const U32 index, t2dVector& smoothedNormal );

    /// Polygon Contains Point?
    static bool polygonContains( const t2dVector* pVertices, const U32 numVertices, const t2dVector& inPoint );

    /// Polygon/Circle Intersection?
    static bool polygonCircleIntersect( const t2dVector& spaceXForm, const t2dVector* pVertices, const U32 numVertices, const t2dVector& center, const F32 radiusSqr, cCollisionStatus* pCollisionStatus );

    // Find Lowest Root for Interval.
    static bool findLowestRootInInterval( F32 A, F32 B, F32 C, F32 upperBound, F32& X );

    /// Swept Circle/Edge-Vertex Intersection.
    static bool sweptCircleEdgeVertexIntersect( const t2dVector& spaceXForm, const t2dVector* pVertices, U32 numVertices, const t2dVector& begin, const t2dVector& delta, F32 A, F32 B, F32 C, cCollisionStatus* pCollisionStatus );

    /// Move to Collision Point.
    static void moveToCollisionPoint( cCollisionStatus* pCollisionStatus );

    /// Back Away.
    static void backAway( cCollisionStatus* pCollisionStatus );

    /// Average Contact Points.
    static void averageContactPoints( cCollisionStatus* pCollisionStatus );

    /// Solve Collision.
    static void solveCollision( cCollisionStatus* pCollisionStatus );

    /// Resolve Overlaps.
    static void resolveOverlap( cCollisionStatus* pCollisionStatus );
    static void resolveOverlapPair( cCollisionStatus* pCollisionStatus, const U32 contactIndex );

    /// Resolve Forward-Collision Dispatcher.
    static void resolveForwardCollision( cCollisionStatus* pCollisionStatus );

    /// Resolve Rigid-Body Collisions.
    static void resolveRigidBodyCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex );

    /// Resolve Bounce Collisions.
    static void resolveBounceCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex );
    /// Resolve Clamp Collisions.
    static void resolveClampCollision(  const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex );
    /// Resolve Sticky Collisions.
    static void resolveStickyCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex );
    /// Resolve Kill Collisions.
    static void resolveKillCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex );
    /// Resolve Custom Collisions.
    static void resolveCustomCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex );

    /// Output Collision Status.
    static void outputCollisionStatus( const char* pDescription, const cCollisionStatus* pCollisionStatus );

    /// Cast Line to Polygon.
    bool castLine( const t2dVector& startPos, const t2dVector& endPos, t2dVector& collisionNormal, F32& collisionTime );
    bool castLineToCircle( const t2dVector& startPos, const t2dVector& endPos, t2dVector& collisionNormal, F32& collisionTime );
    bool castLineToPolygon( const t2dVector& startPos, const t2dVector& endPos, t2dVector& collisionNormal, F32& collisionTime );

    /// Velocity Integration.
    void updateNetVelocity( const F32 elapsedTime );
    void dampGrossVelocity( const F32 elapsedTime );
    void updateSpatials( const F32 elapsedTime, bool suppressLinear = false );


    /// Collision History.
    void addCollisionHistory( const cCollisionStatus* pCollisionStatus );

    /// Collision-Bounds Rendering.
    void renderCollisionBounds( void );
    void renderCollisionCircle( void );
    void renderCollisionPoly( void );
    void renderCollisionHistory( void );


    /// Class Data.
    bool                mInitialised;                   ///< Initialised?
    bool                mSpatialDirty;                  ///< Spatial Chaneg Flag.       

    SimObject*          mRefParentObject;               ///< Reference Parent Object.
    StringTableEntry    mRefMetaString;                 ///< Reference Meta-String.

    Vector<t2dVector>   mCollisionPolyBasisList;        ///< Collision Polygon Basis List.
    Vector<t2dVector>   mCollisionPolyLocalList;        ///< Collision Polygon List.
    U32                 mCollisionMaxIterations;        ///< Maximum Collision Itertations.
    eCollisionDetection mCollisionDetectionMode;        ///< Collision Detection Mode.
    eCollisionResponse  mCollisionResponseMode;         ///< Collision Response Mode.
    bool                mCollisionCircleSuperscribed;   ///< Collision Circle Superscribed Mode?

    bool                mSolveOverlap;                  ///< Solve Overlaps?
    bool                mPhysicsSuppress;               ///< Physics Suppress.
    t2dVector           mPosition;                      ///< Absolute Position.
    t2dVector           mSize;                          ///< Size.
    F32                 mCollisionCircleScale;          ///< Collision Circle Scale.
    t2dVector           mCollisionPolyScale;            ///< Collision Polygon Scale.
    t2dVector           mConstantForce;                 ///< Constant Force.
    bool                mConstantForceActive;           ///< Flags if Constant Force is Used.
    bool                mGraviticConstantForce;         ///< Gravitic Constant Force?
    bool                mForwardMovementOnly;           ///< Forward Movement Only?
    F32                 mRotation;                      ///< Rotation.
    F32                 mAutoRotation;                  ///< Auto Rotation.
    bool                mAutoRotationActive;            ///< Auto Rotation?
    F32                 mDensity;                       ///< Unit Density.
    F32                 mMass;                          ///< Mass.
    F32                 mInertialMoment;                ///< Inertial Moment.
    F32                 mCoefDynamicFriction;           ///< Coefficient of Dynamic Friction.
    F32                 mCoefRestitution;               ///< Coefficient of Restitution.
    F32                 mForceScale;                    ///< Force Scale.
    F32                 mDamping;                       ///< Damping.
    bool                mDampingActive;                 ///< Damping Active?
    bool                mAutoMassInertia;               ///< Auto Mass/Inertia.
    bool                mImmovable;                     ///< Immovable.

    /// Interpolated Tick Position.
    t2dVector           mPreTickPosition;
    t2dVector           mPostTickPosition;
    t2dVector           mRenderTickPosition;

    /// Interpolated Tick Rotation.
    F32                 mPreTickRotation;
    F32                 mPostTickRotation;
    F32                 mRenderTickRotation;

    /// Position/Rotation Targets.
    bool                mPositionTargetActive;          ///< Position Target Active?
    t2dVector           mPositionTarget;                ///< Position Target.
    bool                mPositionTargetAutoStop;        ///< Position Target Auto-Stop?
    bool                mPositionTargetCallback;        ///< Position Target Callback?
    bool                mPositionTargetSnap;            ///< Position Target Snap?
    F32                 mPositionTargetMargin;          ///< Position Target Margin.
    bool                mRotationTargetActive;          ///< Rotation Target Active?
    F32                 mRotationTarget;                ///< Rotation Target.
    bool                mRotationTargetAutoStop;        ///< Rotation Target Auto-Stop?
    bool                mRotationTargetCallback;        ///< Rotation Target Callback?
    bool                mRotationTargetSnap;            ///< Rotation Target Snap?
    F32                 mRotationTargetMargin;          ///< Rotation Target Margin.

    /// Derivatives.
    t2dVector           mGrossLinearVelocity;           ///< Gross Linear Velocity.
    F32                 mGrossAngularVelocity;          ///< Gross Angular Velocity;
    F32                 mMinLinearVelocity;             ///< Minimum Linear Velocity.
    F32                 mMinAngularVelocity;            ///< Minimum Angular Velocity.
    F32                 mMaxLinearVelocity;             ///< Maximum Linear Velocity.
    F32                 mMaxAngularVelocity;            ///< Maximum Angular Velocity.
    t2dMatrix           mMatrixRotation;                ///< Rotation Matrix (Forward XForm).
    t2dMatrix           mInverseMatrixRotation;         ///< Inverse Rotation Matrix (Reverse XForm).
    t2dVector           mNetLinearVelocity;             ///< Net Linear Velocity.
    F32                 mNetAngularVelocity;            ///< Net Angular Velocity.
    bool                mNetLinearVelocityActive;       ///< Net Linear Force?
    bool                mNetAngularVelocityActive;      ///< Net Angular Force?
    bool                mGrossLinearVelocityActive;     ///< Gross Linear Force?
    bool                mGrossAngularVelocityActive;    ///< Gross Angular Force?
    bool                mUpdatingPositionTarget;        ///< Updating Position Target?
    bool                mUpdatingRotationTarget;        ///< Updating Position Target?

    /// Inverses.
    F32                 mInverseMass;                   ///< Inverse Mass.
    F32                 mInverseInertialMoment;         ///< Inverse Inertial Moment.
    t2dVector           mHalfSize;                      ///< Half Size.
    F32                 mCollisionCircleRadii;          ///< Collision Circle Radii.

    /// Collision History.
    Vector<cCollisionStatus> mCollisionHistory;         ///< Collision History.
    bool                mCollectCollisionHistory;       ///< Collision History Collection Flag.


    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dPhysics );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dPhysics, 2 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dPhysics, 3 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dPhysics, 4 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dPhysics, 5 );
};


///-----------------------------------------------------------------------------
/// The Collision Detection/Response Tables are only used in the
/// t2dSceneObjectDatablock fields but it is defined in the physics code purely
/// for code-cohesion.  Downside is that we need to use a nasty compiler extern,
/// oh well, worse things happen at sea.
///-----------------------------------------------------------------------------
extern EnumTable collisionDetectionTable;
extern EnumTable collisionResponseTable;
extern t2dPhysics::eCollisionDetection getCollisionDetectionEnum(const char* label);
extern t2dPhysics::eCollisionResponse getCollisionResponseEnum(const char* label);
extern const char* getCollisionDetectionDescription(t2dPhysics::eCollisionDetection detectionMode);
extern const char* getCollisionResponseDescription(t2dPhysics::eCollisionResponse responseMode);
extern const t2dPhysics::eCollisionDetection defaultCollisionDetectionMode;
extern const t2dPhysics::eCollisionResponse defaultCollisionResponseMode;


///-----------------------------------------------------------------------------

#endif // _T2DPHYSICS_H_
