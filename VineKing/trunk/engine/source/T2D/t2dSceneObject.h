//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Graph Object.
//-----------------------------------------------------------------------------

#ifndef _T2DSCENEOBJECT_H_
#define _T2DSCENEOBJECT_H_

#ifndef _T2DSCENEGRAPH_H_
#include "./t2dSceneGraph.h"
#endif

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _T2DBASEDATABLOCK_H_
#include "./t2dBaseDatablock.h"
#endif

#ifndef _T2DSCENEWINDOW_H_
#include "./t2dSceneWindow.h"
#endif

#ifndef _T2DPHYSICS_H_
#include "./t2dPhysics.h"
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

#ifndef _T2DSCENEOBJECTGROUP_H_
#include "./t2dSceneObjectGroup.h"
#endif

#include "component/behaviors/behaviorComponent.h"
#include "component/behaviors/behaviorInstance.h"

#include "component/MouseEventEnums.h" // RKS: I added this


///-----------------------------------------------------------------------------
/// Scene Datablock Object 2D.
///-----------------------------------------------------------------------------
class t2dSceneObjectDatablock : public t2dBaseDatablock
{
public:
    typedef t2dBaseDatablock Parent;

    t2dSceneObjectDatablock();
    virtual ~t2dSceneObjectDatablock();

    static void initPersistFields();
    void onStaticModified(const char* slotName);

    virtual bool onAdd();
    virtual void onRemove();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    void addSceneObjectReference(t2dSceneObject* object);
    void removeSceneObjectReference(t2dSceneObject* object);

   // Namespace
   StringTableEntry mSuperclass;
   StringTableEntry mClass;

   // Random
   bool mUseMouseEvents;
   bool mEnabled;
   bool mPaused;
   bool mVisible;
   F32 mLifetime;

   // Spatial
   t2dVector mPosition;
   t2dVector mSize;
   F32 mRotation;
   F32 mAutoRotation;
   bool mFlipX;
   bool mFlipY;
   t2dVector mSortPoint;
   S32 mLayer;
   S32 mGraphGroup;
   F32 mMountRotation;
   F32 mAutoMountRotation;

   // World Limits
   S32 mWorldLimitMode;
   t2dVector mWorldLimitMin;
   t2dVector mWorldLimitMax;
   bool mWorldLimitCallback;

   // Collision
   bool mCollisionActiveSend;
   bool mCollisionActiveReceive;
   bool mCollisionPhysicsSend;
   bool mCollisionPhysicsReceive;
   S32 mCollisionGroups;
   S32 mCollisionLayers;
   bool mCollisionCallback;
   t2dVector mCollisionPolyScale;
   F32 mCollisionCircleScale;
   S32 mCollisionMaxIterations;
   t2dPhysics::eCollisionDetection mCollisionDetectionMode;
   t2dPhysics::eCollisionResponse mCollisionResponseMode;
   bool mCollisionCircleSuperscribed;
   Vector<t2dVector> mCollisionPolyCustom;

   // Blending
   bool mBlendingEnabled;
   S32 mSrcBlendFactor;
   S32 mDstBlendFactor;
   ColorF mBlendColor;

   // Physics
   bool mUsesPhysics;
   t2dVector mConstantForce;
   bool mConstantForceGravitic;
   F32 mForceScale;
   bool mImmovable;
   bool mForwardOnly;
   bool mAutoMassInertia;
   F32 mMass;
   F32 mInertia;
   F32 mDensity;
   F32 mFriction;
   F32 mRestitution;
   F32 mDamping;
   t2dVector mLinearVelocity;
   F32 mAngularVelocity;
   F32 mMinLinearVelocity;
   F32 mMaxLinearVelocity;
   F32 mMinAngularVelocity;
   F32 mMaxAngularVelocity;
   Vector<t2dVector> mLinkPoints;

   // Mounting
   t2dVector mMountOffset;
   F32 mMountForce;
   bool mMountTrackRotation;
   bool mMountOwned;
   bool mMountInheritAttributes;

public:
    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dSceneObjectDatablock);

    SimSet mSceneObjects;
};


///-----------------------------------------------------------------------------
/// Note that the t2dParticleEmitter class uses these calls, typically they'd
//  be for direct t2dSceneObject derived objects only.
///-----------------------------------------------------------------------------
extern S32 getSrcBlendFactorEnum(const char* label);
extern S32 getDstBlendFactorEnum(const char* label);
extern const char* getSrcBlendFactorDescription(const GLenum factor);
extern const char* getDstBlendFactorDescription(const GLenum factor);

// Invalid Blend Factor.
const S32 GL_INVALID_BLEND_FACTOR = -1;


/// Sort Routines.
static S32 QSORT_CALLBACK collisionStatusListCollisionTimeRealSort(const void* a, const void* b);

///-----------------------------------------------------------------------------
/// T2D Scene Object.
///
/// @note   We can promote the class to inherit from NetObject to expand
///         the network functionality further later-on.
///
///         We'll need to implement the pack/unpack functionality as well as
///         maintaining the container system on both sides.  We'll also need
///         to mark a line for the integration/collision routines.
///-----------------------------------------------------------------------------
class t2dSceneObject : public BehaviorComponent
{
private:
    typedef BehaviorComponent Parent;

   static t2dSceneObjectDatablock* mDefaultConfig;
protected:
   // [neo, 5/11/2007]
   /// Allow deferring of addBehaviors() until after we are added to the scene graph, 
   /// call addBehaviors() directly when needed.
   virtual bool deferAddingBehaviors() const { return true; }

public:
    friend class t2dSceneGraph;
    friend class t2dSceneContainer;
    friend class t2dSceneWindow;
    friend class t2dSceneObjectGroup;

    /// World Limit Mode.
    enum eWorldLimit
    {
       T2D_LIMIT_OFF,
       T2D_LIMIT_NULL,
       T2D_LIMIT_RIGID,
       T2D_LIMIT_BOUNCE,
       T2D_LIMIT_CLAMP,
       T2D_LIMIT_STICKY,
       T2D_LIMIT_KILL,

       T2D_LIMIT_INVALID,
    };
  
    // Config Datablock
    t2dSceneObjectDatablock* mConfigDataBlock;

    // If a config datablock is not specified, fields are compared against the corresponding
    // field in this datablock to determine if they should be saved.
    static t2dSceneObjectDatablock* getDefaultConfig() { return mDefaultConfig; };
    static void setDefaultConfig(t2dSceneObjectDatablock* config) { mDefaultConfig = config; };

    /// Potential Collision Object List.
    typeSceneObjectVector   mPotentialCollisionObjectList;

    /// Potential Collision Status List.
    Vector<t2dPhysics::cCollisionStatus> mPotentialCollisionStatusList;

    /// Physics Vector.
    typedef Vector<t2dPhysics*> typePhysicsVector;

    /// Special System Options.
    bool                        mEnableChild;
    bool                        mPickingAllowed;
    bool                        mCollisionSendAllowed;
    bool                        mCollisionReceiveAllowed;
    bool                        mEnableAlwaysScope;
    U32                         mDebugMask;
    U32                         mSerialiseKey;

    /// Safe Delete.
    bool                        mBeingSafeDeleted;

    /// Sort Key.
    F32                         mSortKeyCollisionTime;
    t2dVector                   mSortKeyCollisionNormal;

    /// Timer.
    S32                         mPeriodicTimerID;

    /// Process-Ordering.
    t2dSceneObject*             mpNextProcess;
    t2dSceneObject*             mpPreviousProcess;      ///< Serves as Mounted Target.

    /// Camera Attachment.
    t2dSceneWindow*             mpAttachedCamera;

    /// Update Sequencing.
    U32                         mUpdateSequenceKey;


    /// Mounting.
    struct tMountNode
    {
        U32                     mMountID;
        U32                     mMountReferenceCount;
        t2dVector               mLocalMountPosition;
        t2dVector               mWorldMountPosition;
    };
    
    t2dSceneObject*             mpMountedTo;
    t2dVector                   mMountOffset;
    U32                         mMountToID;
    F32                         mMountForce;
    bool                        mMountTrackRotation;
    F32                         mMountPreRotation;
    bool                        mMountOwned;
    bool                        mMountInheritAttributes;
    typedef Vector<tMountNode>  typeMountNodeVector;
    typeMountNodeVector         mMountNodes;
    Vector<t2dVector>           mMountNodesList;

    SimObjectPtr<t2dSceneObject> mAttachedToPath;

    /// Destroy Notification.
    struct tDestroyNotification
    {
        t2dSceneObject*         mpSceneObject;
        U32                     mRefCount;
    };
    typedef Vector<tDestroyNotification>    typeDestroyNotificationVector;

    /// Utility.
    static bool pointInRectangle( const t2dVector& point, const t2dVector& rectMin, const t2dVector& rectMax );
    static bool lineRectangleIntersect( const t2dVector& startPoint, const t2dVector& endPoint, const t2dVector& rectMin, const t2dVector& rectMax, F32* pTime = NULL );
    static void transformPoint2D( const t2dMatrix& xform, const t2dVector& srcPoint2D, const t2dVector& srcTranslation, t2dVector& dstPoint2D );
    static void inverseTransformPoint2D( const t2dMatrix& xform, const t2dVector& srcPoint2D, const t2dVector& srcTranslation, t2dVector& dstPoint2D );
    static void pivotTransformPoint2D( const t2dMatrix& xform, const t2dVector& srcPoint2D, const t2dVector& pivotPoint, t2dVector& dstPoint2D );
    static void transformRectangle2D( const t2dMatrix& xform, const t2dVector* srcPoint2D, const t2dVector& srcTranslation, t2dVector* dstPoint2D );
    static void inverseTransformRectangle2D( const t2dMatrix& xform, const t2dVector* srcPoint2D, const t2dVector& srcTranslation, t2dVector* dstPoint2D );
    static const char* getFirstNonWhitespace( const char* inString );
    static t2dVector getStringElementVector( const char* inString, const U32 index = 0 );
    static VectorF getStringElementVector3D( const char* inString, const U32 index = 0 );
    static const char* getStringElement( const char* inString, const U32 index );
    static U32 getStringElementCount( const char *string );
    static t2dSceneObject* findSerialiseKey( U32 serialiseKey, Vector<t2dSceneObject*>& ObjReferenceList );

    /// Process-Ordering.
    void processLinkBefore( t2dSceneObject* pSceneObject2D );
    void processLinkAfter( t2dSceneObject* pSceneObject2D );
    void processUnLink( void );
    void processInitialiseLink( void )                      { mpNextProcess = mpPreviousProcess = this; mpMountedTo = NULL; };
    bool processIsLinked( void ) const                      { return ( mpNextProcess != this && mpPreviousProcess != this ); };
    bool processIsMounted( void ) const                     { return (mpMountedTo != NULL); };
    bool processIsMountedRigid( void ) const                { return processIsMounted() && mIsZero(mMountForce); };
    t2dSceneObject* getProcessMount( void ) const           { return mpMountedTo; };
    t2dSceneObject* getProcessNext( void ) const            { return mpNextProcess; };
    t2dSceneObject* getProcessPrevious( void ) const        { return mpPreviousProcess; };

    /// Mounting.
    U32 addLinkPoint( const t2dVector& nodeOffset );
    void removeLinkPoint( const U32 mountID );
    bool setLinkPoint( const U32 mountID, const t2dVector& nodeOffset );
    tMountNode* getMountNode( const U32 mountID );
    S32 getMountNodeCount() const { return mMountNodes.size(); };
	 const tMountNode& getMountNodeByIndex(S32 index) const { return mMountNodes[index]; };
    void removeAllLinkPoints( void );
	char* getAllLinkpointIDs( void );
	void getAllMountedChildren(Vector<t2dSceneObject *> *);


    /// Pathing.
    void setAttachedToPath(t2dSceneObject* path)            { mAttachedToPath = path; };
    t2dSceneObject* getAttachedToPath() const               { return mAttachedToPath; };

    // Mouse-Events.
    void setUseMouseEvents( bool mouseStatus )              { mUseMouseEvents = mouseStatus; };
    bool getUseMouseEvents( void ) const                    { return mUseMouseEvents; };
    void setMouseLocked(bool locked);
    bool getMouseLocked()                                     { return mMouseLocked; };

    /// Update Sequencing.
    inline void setUpdateSequenceKey( const U32 seqKey )    { mUpdateSequenceKey = seqKey; };
    inline U32  getUpdateSequenceKey( void ) const          { return mUpdateSequenceKey; };

    /// Initial Update.
    inline void setInitialUpdate( void )                    { mInitialUpdate = true; }
    inline bool getInitialUpdate( void ) const              { return mInitialUpdate; }

    void enableUpdateCallback()                             { mUpdateCallback = true; }
    void disableUpdateCallback()                            { mUpdateCallback = false; }

    /// Safe Deletion.
    inline void setSafeDelete( const bool status )          { mSafeDeleteReady = status; };
    inline bool getSafeDelete( void ) const                 { return mSafeDeleteReady; };
    inline bool isBeingDeleted( void ) const                { return mBeingSafeDeleted; };

public:
   void setConfigDatablock( const char* datablockName );
   t2dSceneObjectDatablock *getConfigDatablock() { return mConfigDataBlock; };

   virtual bool writeField(StringTableEntry fieldname, const char* value);
   virtual void writeFields(Stream& stream, U32 tabStop);
   virtual void readFieldsPostLoad();

   void setNeverSolvePhysics(bool neverSolvePhysics) { mNeverSolvePhysics = neverSolvePhysics; };
   bool getNeverSolvePhysics() { return mNeverSolvePhysics; };

   virtual void onCollision(t2dPhysics::cCollisionStatus* collisionStatus) { };
	
	// RKS: I add this to supplant the script call to "onCollision" which is called from onScriptCollision()
	virtual void HandleScriptCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus );

   BehaviorInstance *behavior(const char *name);

private:
   bool mNeverSolvePhysics;

protected:

    /// Properties.
    bool                    mVisible;
    bool                    mPaused;
    F32                     mMountRotation;
    bool                    mFlipX;
    bool                    mFlipY;
    t2dVector               mSortPoint;
    /// mSize (Physics).
    U32                     mLayer;
    U32                     mGraphGroup;
    F32                     mLifetime;
    bool                    mLifetimeActive;
    eWorldLimit             mWorldLimitMode;
    t2dVector               mWorldLimitMin;
    t2dVector               mWorldLimitMax;
    bool                    mWorldLimitCallback;
    F32                     mAutoMountRotation;
    bool                    mAutoMountRotationActive;

    /// Collisions.
	bool					mUsesPhysics;
    bool                    mCollisionActiveSend;
    bool                    mCollisionActiveReceive;
    bool                    mCollisionPhysicsSend;
    bool                    mCollisionPhysicsReceive;
    U32                     mCollisionLayerMask;
    U32                     mCollisionGroupMask;
    bool                    mCollisionCallback;
    bool                    mCollisionSuppress;

    /// Render Options.
    bool                    mBlending;
    S32                     mSrcBlendFactor;
    S32                     mDstBlendFactor;
    ColorF                  mBlendColour;

    // Mouse.
    bool                    mUseMouseEvents;
    bool                    mMouseLocked;

    // Attached Gui Control.
    GuiControl*             mpAttachedGui;
    t2dSceneWindow*         mpAttachedGuiSceneWindow;
    bool                    mAttachedGuiSizeControl;


    // Misc.
    S32                     mLayerOrder;


    /// Derivatives.
    U32                     mLayerMask;
    U32                     mGraphGroupMask;

    /// Clipping/Collision.
    t2dVector               mLocalClipBoundary[4];
    t2dVector               mWorldClipBoundary[4];
    RectF                   mLocalClipRectangle;
    RectF                   mWorldClipRectangle;
    RectF                   mWorldCollisionClipRectangle;
    F32                     mWorldClipLeft;
    F32                     mWorldClipRight;
    F32                     mWorldClipTop;
    F32                     mWorldClipBottom;
    t2dMatrix               mMountRotationMatrix;

    /// Container System.
    SimObjectPtr<t2dSceneGraph> mpSceneGraph;
    t2dSceneObjectGroup*    mpSceneObjectGroup;
    t2dSceneBinReference*   mpBinReferenceChain;
    U32                     mContainerSequenceKey;
    U32                     mMinBinX;
    U32                     mMinBinY;
    U32                     mMaxBinX;
    U32                     mMaxBinY;

    bool                    mInitialUpdate;

    /// Safe Delete Flag.
    bool                    mSafeDeleteReady;

    // Container Enabled Status.
    //bool                  mContainerEnabled;

    // Tick Processing.
    bool                    mProcessTick;
    bool                    mUpdateCallback;


    /// Parent Physics.
    // @note    This physics is parent in the sense that the object can
    //          maintain child physics.
    t2dPhysics                      mParentPhysics;

    /// World Limit.
    t2dPhysics                      mWorldLimitPhysics[4];
    t2dPhysics::cCollisionStatus    mWorldLimitCollisionStatus[4];
    StringTableEntry                mWorldLimitDescription[4];

    /// Collision List.
    typePhysicsVector       mCollisionPhysicsList;

    t2dSceneGraph* mFieldSelectedSceneGraph;

    /// Notification List.
    typeDestroyNotificationVector   mDestroyNotifyList;

public:
    t2dSceneObject();
    virtual ~t2dSceneObject();

    static void initPersistFields();

    void            setContainerSequenceKey( U32 seqKey )   { mContainerSequenceKey = seqKey; };
    void            setChild( bool status )                 { mEnableChild = status; };

    void            setVisible( const bool status );
    void            setPaused( const bool status );
    virtual void    setArea( const t2dVector& corner1, const t2dVector& corner2 );
    virtual void    setPosition( const t2dVector& position );
    virtual void    setRotation( const F32 rotation );
    void            setMountRotation( const F32 mountRotation );
    // Made flip methods virtual so tile layer can ignore.
    virtual void    setFlip( const bool flipX, bool flipY );
    virtual void    setFlipX( const bool flipX );
    virtual void    setFlipY( bool flipY );
    virtual void    setSize( const t2dVector& size );
    void            setLayer( const U32 layer );
    void            setGraphGroup( const U32 group );
    void            setWorldLimit( const eWorldLimit limitMode, const t2dVector& limitMin, const t2dVector& limitMax, const bool limitCallback );
    void            setWorldLimitMode( const eWorldLimit limitMode );
    void            setWorldLimitMin( const t2dVector& limitMin );
    void            setWorldLimitMax( const t2dVector& limitMin );
    void            setWorldLimitCallback( const bool callback );
    void            setLifetime( const F32 lifetime );
    void            setAutoRotation( const F32 rotationSpeed );
    void            setAutoMountRotation( const F32 mountRotationSpeed );
    void            setSortPoint( const t2dVector& pt );

    void            setForwardMovementOnly( const bool forwardMovementOnly );
    void            setForwardSpeed( const F32 forwardSpeed );
    void            setPositionTarget( const t2dVector positionTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin );
    void            setPositionTargetOff( void );
    void            setRotationTarget( const F32 rotationTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin );
    void            setRotationTargetOff( void );
    void            moveTo( const t2dVector positionTarget, const F32 linearSpeed, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin );
    void            rotateTo( const F32 rotationTarget, const F32 angularSpeed, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin );

    void            setCollisionActive( bool send, bool receive );
    void            setCollisionActiveSend( bool send );
    void            setCollisionActiveReceive( bool receive );
    void            setCollisionPhysics( bool send, bool receive );
    void            setCollisionPhysicsSend( bool send );
    void            setCollisionPhysicsReceive( bool receive );
    void            setCollisionMasks( const U32 groupMask, const U32 layerMask = T2D_MASK_ALL );
    void            setCollisionAgainst( const t2dSceneObject* pSceneObject, const bool clearMasks );
    void            setCollisionGroups( const U32 groupMask );
    void            setCollisionLayers( const U32 layerMask );
    void            setCollisionCircleSuperscribed( const bool superscribeMode ); 
    void            setCollisionMaxIterations( const U32 maxIterations ) { getParentPhysics().setCollisionMaxIterations(maxIterations); };
    void            setCollisionDetection( const t2dPhysics::eCollisionDetection detectionMode );
    void            setCollisionResponse( const t2dPhysics::eCollisionResponse responseMode );
    void            setCollisionCallback( const bool status );
    void            setCollisionPolyScale( const t2dVector& scale );
    void            setCollisionCircleScale( const F32 scale );
    void            setCollisionSuppress( const bool status );

    void            setCollisionPolyPrimitive( const U32 polyVertexCount );
    virtual void    setCollisionPolyCustom( const U32 polyVertexCount, const t2dVector* pCustomPolygon );
    void            setCollisionMaterial( const char* pCollisionMaterialName );

    void            setLayerOrder( const U32 order ) { mLayerOrder = order; };
    void            setImpulseForce( const t2dVector& impulseForce, const bool gravitic );
    void            setConstantForce( const t2dVector& force, const bool gravitic );
    void            setConstantForce( const t2dVector& force );
    void            setConstantForceGravitic( const bool gravitic );
    void            setForceScale( const F32 forceScale );
    void            setAtRest( void );

    void            setDynamicFriction( const F32 dynamicfriction );
    void            setRestitution( const F32 restitution );
    void            setAutoMassInertia( const bool status );
    void            setDensity( const F32 density );
    void            setMass( const F32 mass );
    void            setInertialMoment( const F32 inertialMoment );
    void            setDamping( const F32 damping );
    void            setImmovable( const bool immovable );

    void            setLinearVelocity( const t2dVector& linearVelocity );
    void            setAngularVelocity( const F32 angularVelocity );
    void            setMinLinearVelocity( const F32 velocity );
    void            setMinAngularVelocity( const F32 velocity );
    void            setMaxLinearVelocity( const F32 velocity );
    void            setMaxAngularVelocity( const F32 velocity );

    void            setMountOffset(t2dVector offset)        { if(mpMountedTo) {mpMountedTo->setLinkPoint(mMountToID, offset);} else {mMountOffset = offset;} };
    void            setMountForce(F32 force)                { mMountForce = force; };
    void            setMountTrackRotation(bool track)       { mMountTrackRotation = track; };
    void            setMountOwned(bool owned)               { mMountOwned = owned; };
    void            setMountInheritAttributes(bool inherit) { mMountInheritAttributes = inherit; };

    void            setBlending( const bool status, const S32 srcBlendFactor, const S32 dstBlendFactor );
    void            setBlendStatus( const bool status );
    void            setSrcBlendFactor( const S32 srcBlendFactor );
    void            setDstBlendFactor( const S32 dstBlendFactor );
    void            setBlendColourString( S32 argc, const char** argv );
    void            setBlendColourString( const char* string );
    void            setBlendColour( const ColorF& blendColour );
    void            setBlendAlpha( const F32 alpha );
    void            setSerialiseState( const U32 serialiseID, const U32 serialiseKey ) { mLocalSerialiseID = serialiseID; mSerialiseKey = serialiseKey; };

    /// Debug Mode.
    void            setDebugOn( const U32 debugMask );
    void            setDebugOff( const U32 debugMask );
    inline U32      getDebugMask( void ) const                          { return mDebugMask; };


    inline bool                 getVisible(void) const                  { return mVisible; };
    inline bool                 getPaused(void) const                   { return mPaused; };
    inline bool                 getIsProcessingTicks() const            { return mProcessTick; };

    inline t2dVector            getPosition(void) const                 { return getParentPhysicsConst().getPosition(); };
    inline F32                  getRotation(void) const                 { return getParentPhysicsConst().getRotation(); };
    inline t2dVector            getRenderPosition(void) const           { return getParentPhysicsConst().getRenderTickPosition(); };
    inline F32                  getRenderRotation(void) const           { return getParentPhysicsConst().getRenderTickRotation(); };
    inline t2dVector            getLinearVelocity(void) const           { return getParentPhysicsConst().getGrossLinearVelocity(); };
    inline F32                  getAngularVelocity(void) const          { return getParentPhysicsConst().getGrossAngularVelocity(); };
    inline F32                  getMountRotation(void) const            { return mMountRotation; };
    inline bool                 getFlipX(void) const                    { return mFlipX; };
    inline bool                 getFlipY(void) const                    { return mFlipY; };
    inline t2dVector            getSortPoint(void) const                { return mSortPoint; };
    
    inline t2dVector            getSize(void) const                     { return getParentPhysicsConst().getSize(); };
    inline t2dVector            getHalfSize(void) const                 { return getParentPhysicsConst().getHalfSize(); };
    inline U32                  getLayer(void) const                    { return mLayer; };
    inline U32                  getGraphGroup(void) const               { return mGraphGroup; };
    inline const char*          getWorldLimit(void);
    inline eWorldLimit          getWorldLimitMode(void) const           { return mWorldLimitMode; };
    inline t2dVector            getWorldLimitMin(void) const            { return mWorldLimitMin; };
    inline t2dVector            getWorldLimitMax(void) const            { return mWorldLimitMax; };
    inline bool                 getWorldLimitCallback(void) const       { return mWorldLimitCallback; };
    inline F32                  getAutoRotation(void) const             { return getParentPhysicsConst().getAutoRotation(); };
    inline F32                  getAutoMountRotation(void) const        { return mAutoMountRotation; };
    inline bool                 getForwardMovementOnly(void) const      { return getParentPhysicsConst().getForwardMovementOnly(); };

    inline F32                  getLifetime(void) const                 { return mLifetime; };
    inline F32                  getCollisionCircleRadii(void) const     { return getParentPhysicsConst().getCollisionCircleRadii(); };
    inline bool                 getCollisionCircleSuperscribed(void) const { return getParentPhysicsConst().getCollisionCircleSuperscribed(); };
    inline F32                  getCollisionCircleScale(void) const     { return getParentPhysicsConst().getCollisionCircleScale(); };
    inline t2dVector            getCollisionPolyScale(void) const       { return getParentPhysicsConst().getCollisionPolyScale(); };
    inline U32                  getCollisionPolyCount(void) const       { return getParentPhysicsConst().getCollisionPolyCount(); };
    const char*                 getCollisionPoly(void);
    const t2dVector*            getCollisionPolyArray(void)             { return getParentPhysics().getCollisionPolyBasis(); };

    inline S32                  getLayerOrder( void ) const             { return mLayerOrder; };
    inline U32                  getLayerMask(void) const                { return mLayerMask; };
    inline U32                  getGraphGroupMask(void) const                { return mGraphGroupMask; };

    inline bool                 getCollisionActiveSend(void) const      { return mCollisionActiveSend && !mCollisionSuppress; };
    inline bool                 getCollisionActiveReceive(void) const   { return mCollisionActiveReceive && !mCollisionSuppress; };
    inline bool                 getCollisionPhysicsSend(void)           { return mCollisionPhysicsSend && !getParentPhysics().getPhysicsSuppress(); };
    inline bool                 getCollisionPhysicsReceive(void)        { return mCollisionPhysicsReceive && !getParentPhysics().getPhysicsSuppress(); };
    inline U32                  getCollisionGroupMask(void) const       { return mCollisionGroupMask; };
    inline U32                  getCollisionLayerMask(void) const       { return mCollisionLayerMask; };
    inline bool                 getCollisionCallback(void) const        { return mCollisionCallback; };
    inline bool                 getCollisionSuppress(void) const        { return mCollisionSuppress; };
    inline U32                  getCollisionMaxIterations(void)         { return getParentPhysicsConst().getCollisionMaxIterations(); };
    t2dPhysics::eCollisionDetection getCollisionDetectionMode(void) const { return getParentPhysicsConst().getCollisionDetectionMode(); };
    t2dPhysics::eCollisionResponse getCollisionResponseMode(void) const { return getParentPhysicsConst().getCollisionResponseMode(); };

    inline F32                  getMinLinearVelocity(void) const        { return getParentPhysicsConst().getMinLinearVelocity(); };
    inline F32                  getMinAngularVelocity(void) const       { return getParentPhysicsConst().getMinAngularVelocity(); };
    inline F32                  getMaxLinearVelocity(void) const        { return getParentPhysicsConst().getMaxLinearVelocity(); };
    inline F32                  getMaxAngularVelocity(void) const       { return getParentPhysicsConst().getMaxAngularVelocity(); };
    inline t2dVector            getConstantForce( void ) const          { return getParentPhysicsConst().getConstantForce(); };
    inline bool                 getGraviticConstantForce( void ) const  { return getParentPhysicsConst().getGraviticConstantForceFlag(); };

    inline F32                  getDynamicFriction( void ) const        { return getParentPhysicsConst().getDynamicFriction(); };
    inline F32                  getRestitution( void ) const            { return getParentPhysicsConst().getRestitution(); };
    inline bool                 getAutoMassInertia( void ) const        { return getParentPhysicsConst().getAutoMassInertia(); };
    inline F32                  getDensity( void ) const                { return getParentPhysicsConst().getDensity(); };
    inline F32                  getMass( void ) const                   { return getParentPhysicsConst().getMass(); };
    inline F32                  getInertialMoment( void ) const         { return getParentPhysicsConst().getInertialMoment(); };
    inline F32                  getForceScale( void ) const             { return getParentPhysicsConst().getForceScale(); };
    inline F32                  getDamping( void ) const                { return getParentPhysicsConst().getDamping(); };
    inline bool                 getImmovable( void ) const              { return getParentPhysicsConst().getImmovable(); };

    inline bool                 getBlendingStatus( void ) const         { return mBlending; };
    inline S32                  getSrcBlendFactor( void ) const         { return mSrcBlendFactor; };
    inline S32                  getDstBlendFactor( void ) const         { return mDstBlendFactor; };
    inline ColorF               getBlendColour( void ) const            { return mBlendColour; };
    inline F32                  getBlendAlpha( void ) const             { return mBlendColour.alpha; };

    inline const t2dVector*     getLocalClipBoundary(void) const        { return mLocalClipBoundary; };
    inline const t2dVector*     getWorldClipBoundary(void) const        { return mWorldClipBoundary; };
    inline RectF                getLocalRect(void) const                { return mLocalClipRectangle; }; 
    inline RectF                getWorldClipRectangle(void) const       { return mWorldClipRectangle; };
    inline F32                  getWorldClipLeft(void) const            { return mWorldClipLeft; };
    inline F32                  getWorldClipRight(void) const           { return mWorldClipRight; };
    inline F32                  getWorldClipTop(void) const             { return mWorldClipTop; };
    inline F32                  getWorldClipBottom(void) const          { return mWorldClipBottom; };
    inline const RectF&         getWorldCollisionClipRectangle(void) const { return mWorldCollisionClipRectangle; };
    inline const t2dMatrix&     getRotationMatrix(void)                 { return getParentPhysics().getRotationMatrix(); };
    inline const t2dMatrix&     getInverseRotationMatrix(void)          { return getParentPhysics().getInverseRotationMatrix(); };
    inline U32                  getContainerSequenceKey(void) const     { return mContainerSequenceKey; };
    inline t2dSceneGraph* const getSceneGraph(void) const               { return mpSceneGraph; };
    inline t2dSceneObjectGroup* const getSceneObjectGroup(void) const   { return mpSceneObjectGroup; };
    inline F32                  getSceneTime(void) const                { if ( mpSceneGraph ) return mpSceneGraph->getSceneTime(); else return 0.0f; }
    inline bool                 getAtRest( void ) const                 { return getParentPhysicsConst().getAtRest(); };
    inline bool                 getSpatialDirty( void ) const           { return getParentPhysicsConst().getSpatialDirty(); };
    inline void                 resetSpatialDirty( void )               { getParentPhysics().resetSpatialDirty(); };
    inline bool                 getIsMounted(void) const                { return processIsMounted(); }
    inline t2dVector            getLocalPoint( const t2dVector& worldPoint );
    inline t2dVector            getWorldPoint( const t2dVector& localPoint );

    /// Misc Accessors.
    inline const char*          scriptThis(void) const                  { static char argBuffer[8]; dSprintf(argBuffer, 8, "%d", getId()); return argBuffer; };
    inline bool                 getIsWorldLimitActive(void) const       { return mWorldLimitMode != T2D_LIMIT_OFF; };
    inline bool                 getIsChild(void) const                  { return mEnableChild; };
    inline bool                 getIsPickingAllowed(void) const         { return mPickingAllowed; };
    inline bool                 getIsAlwaysScope(void) const            { return mEnableAlwaysScope; };
    inline bool                 getIsInViewport( RectF viewWindow, RectF& intersection ) const  { intersection = viewWindow; return intersection.intersect( mWorldClipRectangle ); };

    /// Fetch Physics.
    inline t2dPhysics&          getParentPhysics( void )                { return mParentPhysics; };
    inline const t2dPhysics&    getParentPhysicsConst( void ) const     { return mParentPhysics; };
    inline t2dPhysics* const    getWorldLimitPhysics( void )            { return mWorldLimitPhysics; };

    /// Mounting.
    S32             mount( t2dSceneObject* pSceneObject2D, const t2dVector& mountOffset, const F32 mountForce, const bool trackRotation, const bool sendToMount, const bool ownedByMount, const bool inheritAttributes );
    S32             mount( t2dSceneObject* pSceneObject2D, const U32& mountID, const F32 mountForce, const bool trackRotation, const bool sendToMount, const bool ownedByMount, const bool inheritAttributes );
    bool            dismount( void );
    void            updateMount( const F32 elapsedTime );

    t2dSceneObject* getMountedParent()          { return mpMountedTo; };
    t2dVector       getMountOffset()            { return mMountOffset; };
    F32             getMountForce()             { return mMountForce; };
    bool            getMountTrackRotation()     { return mMountTrackRotation; };
    bool            getMountOwned()             { return mMountOwned; };
    bool            getMountInheritAttributes() { return mMountInheritAttributes; };

    /// Notification.
    void            addDestroyNotification( t2dSceneObject* pSceneObject2D );
    void            removeDestroyNotification( t2dSceneObject* pSceneObject2D );
    void            processDestroyNotifications( void );

    // Container Enable/Disable Status.
    //void          setContainerEnabledStatus( bool status )    { mContainerEnabled = status; };
    //bool          getContainerEnabledStatus( void )           { return mContainerEnabled; };

    /// Camera Mount.
    void            addCameraMountReference( t2dSceneWindow* pAttachedCamera );
    void            removeCameraMountReference( void );
    void            dismountCamera( void );

    /// World Limit.
    void            updateWorldLimit( F32 elapsedTime );
    void            processWorldLimitCollision(t2dPhysics::cCollisionStatus* pCollisionStatus, StringTableEntry limitDescription );
    void            setDefaultWorldLimit();

    /// Blending.
    void setBlendOptions( void );
    static void resetBlendOptions( void );

    /// Safe Deletion.
    virtual void safeDelete();
    void updateLifetime( const F32 elapsedTime );

    /// Cloning.
    t2dSceneObject* clone();
    virtual void copyTo(SimObject* object);
    bool clone( t2dSceneObject** ppSceneObject, const bool bCopyFields );
    virtual bool copy( t2dSceneObject* pSceneObject, const bool bCopyFields );
    /// Load/Save Copy.
    bool loadCopy( const char* objectFile );
    bool saveCopy( const char* objectFile );

    // GUI Attachment.
    void attachGui( GuiControl* pGuiControl, t2dSceneWindow* pSceneWindow, const bool sizeControl );
    void detachGui( void );
    inline void updateAttachedGui( void );

    /// Internal Crunchers.
    void calculateMountNodes( const t2dVector& position );

    /// Is Point in Object?
    virtual bool getIsPointInObject( const t2dVector& worldPoint );

    virtual bool onAdd();
    virtual void onRemove();
    virtual U32  packUpdate(NetConnection * conn, U32 mask, BitStream *stream);
    virtual void unpackUpdate(NetConnection * conn, BitStream *stream);
    virtual void onDestroyNotify( t2dSceneObject* pSceneObject2D );

    void initializePhysics();

    /// Namespace Linking.
    void linkNamespaces();
    void unLinkNamespaces();

    /// Spatial Attribute Calculation.
    virtual void updateSpatialConfig( const bool updateMountNodes = true );
    void updateWorldClip( const t2dVector& position );

    /// Mouse-Events.
    virtual void onMouseEvent( const char* name, const GuiEvent& event, const t2dVector& worldMousePoint, const EMouseEvent _eMouseEvent = MOUSE_EVENT_NONE );

    /// Collisions.
    virtual typePhysicsVector& getCollisionAreaPhysics( const RectF& worldArea );
    virtual bool checkCollisionSend( const F32 elapsedTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats );
    virtual void processCollisionStatus( t2dPhysics::cCollisionStatus* pCollisionStatus, CDebugStats* pDebugStats );
    virtual void processAllCollisionStatus( Vector<t2dPhysics::cCollisionStatus>& collisionStatusList, CDebugStats* pDebugStats );
    virtual bool onCustomCollisionDetection( const F32 elapsedTime, t2dPhysics::cCollisionStatus* pCollisionStatus );
    virtual void onCustomCollisionResponse( t2dPhysics::cCollisionStatus* pCollisionStatus );
    virtual void onScriptCollision( const t2dPhysics::cCollisionStatus* pCollisionStatus );
    virtual bool castCollision( const F32 elapsedTime, t2dPhysics::cCollisionStatus* pCollisionStatus );
    virtual bool castCollisionList( const F32 elapsedTime );

    /// Integration.
    virtual void interpolateTick( const F32 timeDelta );
    virtual void preIntegrate( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );          ///< Always call this as PARENT!
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void postIntegrate(const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats);
    virtual void updateLocalPhysics( const F32 elapsedTime, CDebugStats* pDebugStats );
    void updateConstantForce( const F32 elapsedTime );

    // Rendering.
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dSceneObject );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dSceneObject, 8 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dSceneObject, 9 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dSceneObject, 10 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dSceneObject, 11 );

    /// Declare Console Object.
    DECLARE_CONOBJECT( t2dSceneObject );

    // Accessors for protected persistent fields. Most of the protected fields can just use the
    // default get function.
    // Config
    static bool setConfigDatablock(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setConfigDatablock(data); return false; };
    static bool setSceneGraph(void* obj, const char* data)
    {
       t2dSceneGraph* scenegraph = dynamic_cast<t2dSceneGraph*>(Sim::findObject(data));
       t2dSceneObject* object = static_cast<t2dSceneObject*>(obj);
       if (scenegraph)
       {
          if (object->getSceneGraph()) object->getSceneGraph()->removeFromScene(object);
          scenegraph->addToScene(object);
       }
       return false;
    };

    // Random
    static bool setUseMouseEvents(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setUseMouseEvents(dAtob(data)); return false; };
    static bool setPaused(void* obj, const char* data)         { static_cast<t2dSceneObject*>(obj)->setPaused(dAtob(data)); return false; };
    static bool setVisible(void* obj, const char* data)        { static_cast<t2dSceneObject*>(obj)->setVisible(dAtob(data)); return false; };
    static bool setLifetime(void* obj, const char* data)       { static_cast<t2dSceneObject*>(obj)->setLifetime(dAtof(data)); return false; };

    // Spatial
    static bool setPosition(void* obj, const char* data)       { static_cast<t2dSceneObject*>(obj)->setPosition(getStringElementVector(data)); static_cast<t2dSceneObject*>(obj)->updateSpatialConfig(); return false; };
    static bool setSize(void* obj, const char* data)           { static_cast<t2dSceneObject*>(obj)->setSize(getStringElementVector(data)); static_cast<t2dSceneObject*>(obj)->updateSpatialConfig(); return false; };
    static bool setRotation(void* obj, const char* data)       { static_cast<t2dSceneObject*>(obj)->setRotation(dAtof(data)); static_cast<t2dSceneObject*>(obj)->updateSpatialConfig(); return false; };
    static bool setAutoRotation(void* obj, const char* data)   { static_cast<t2dSceneObject*>(obj)->setAutoRotation(dAtof(data)); return false; };
    static bool setFlipX(void* obj, const char* data)          { static_cast<t2dSceneObject*>(obj)->setFlipX(dAtob(data)); static_cast<t2dSceneObject*>(obj)->updateSpatialConfig(); return false; };
    static bool setFlipY(void* obj, const char* data)          { static_cast<t2dSceneObject*>(obj)->setFlipY(dAtob(data)); static_cast<t2dSceneObject*>(obj)->updateSpatialConfig(); return false; };
    static bool setSortPoint(void* obj, const char* data)      { static_cast<t2dSceneObject*>(obj)->setSortPoint(getStringElementVector(data)); return false; };
    static bool setLayer(void* obj, const char* data)          { static_cast<t2dSceneObject*>(obj)->setLayer(dAtoi(data)); return false; };
    static bool setGraphGroup(void* obj, const char* data)     { static_cast<t2dSceneObject*>(obj)->setGraphGroup(dAtoi(data)); return false; };
    static bool setMountRotation(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setMountRotation(dAtof(data)); return false; };
    static bool setAutoMountRotation(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setAutoMountRotation(dAtof(data)); return false; };

    // World Limits
    static bool setWorldLimitMode(void* obj, const char* data);
    static bool setWorldLimitMin(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setWorldLimitMin(getStringElementVector(data)); return false; };
    static bool setWorldLimitMax(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setWorldLimitMax(getStringElementVector(data)); return false; };
    static bool setWorldLimitCallback(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setWorldLimitCallback(dAtob(data)); return false; };

    // Collision
    static bool setCollisionActiveSend(void* obj, const char* data)     { static_cast<t2dSceneObject*>(obj)->setCollisionActiveSend(dAtob(data)); return false; };
    static bool setCollisionActiveReceive(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setCollisionActiveReceive(dAtob(data)); return false; };
    static bool setCollisionPhysicsSend(void* obj, const char* data)    { static_cast<t2dSceneObject*>(obj)->setCollisionPhysicsSend(dAtob(data)); return false; };
    static bool setCollisionPhysicsReceive(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setCollisionPhysicsReceive(dAtob(data)); return false; };
    static bool setCollisionGroups(void* obj, const char* data)         { static_cast<t2dSceneObject*>(obj)->setCollisionGroups(dAtoi(data)); return false; };
    static bool setCollisionLayers(void* obj, const char* data)         { static_cast<t2dSceneObject*>(obj)->setCollisionLayers(dAtoi(data)); return false; };
    static bool setCollisionCallback(void* obj, const char* data)       { static_cast<t2dSceneObject*>(obj)->setCollisionCallback(dAtob(data)); return false; };
    static bool setCollisionPolyScale(void* obj, const char* data)      { static_cast<t2dSceneObject*>(obj)->setCollisionPolyScale(getStringElementVector(data)); return false; };
    static bool setCollisionCircleScale(void* obj, const char* data)    { static_cast<t2dSceneObject*>(obj)->setCollisionCircleScale(dAtof(data)); return false; };
    static bool setCollisionMaxIterations(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setCollisionMaxIterations(dAtoi(data)); return false; };
    static bool setCollisionDetectionMode(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setCollisionDetection(getCollisionDetectionEnum(data)); return false; };
    static bool setCollisionResponseMode(void* obj, const char* data)   { static_cast<t2dSceneObject*>(obj)->setCollisionResponse(getCollisionResponseEnum(data)); return false; };
    static bool setCollisionCircleSuperscribed(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setCollisionCircleSuperscribed(dAtob(data)); return false; };
    static bool setCollisionPolyCustom(void* obj, const char* data)
    {
       t2dVector poly[t2dPhysics::MAX_COLLISION_POLY_VERTEX];
       U32 count = getStringElementCount(data) >> 1;
       for (U32 i = 0; i < count; i++)
          poly[i] = getStringElementVector(data, i * 2);

       static_cast<t2dSceneObject*>(obj)->setCollisionPolyCustom(count, poly);
       return false;
    };

    // Blending
    static bool setBlendingEnabled(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setBlendStatus(dAtob(data)); return false; };
    static bool setSrcBlendFactor(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setSrcBlendFactor(getSrcBlendFactorEnum(data)); return false; };
    static bool setDstBlendFactor(void* obj, const char* data)  { static_cast<t2dSceneObject*>(obj)->setDstBlendFactor(getDstBlendFactorEnum(data)); return false; };
    static bool setBlendColor(void* obj, const char* data)      { static_cast<t2dSceneObject*>(obj)->setBlendColourString(data); return false; };

    // Physics
	static bool setUsesPhysics(void* obj, const char* data)			  { static_cast<t2dSceneObject*>(obj)->setUsesPhysics(dAtob(data)); return false; };
    static bool setConstantForce(void* obj, const char* data)         { static_cast<t2dSceneObject*>(obj)->setConstantForce(getStringElementVector(data)); return false; };
    static bool setConstantForceGravitic(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setConstantForceGravitic(dAtob(data)); return false; };
    static bool setForceScale(void* obj, const char* data)            { static_cast<t2dSceneObject*>(obj)->setForceScale(dAtof(data)); return false; };
    static bool setImmovable(void* obj, const char* data)             { static_cast<t2dSceneObject*>(obj)->setImmovable(dAtob(data)); return false; };
    static bool setForwardOnly(void* obj, const char* data)           { static_cast<t2dSceneObject*>(obj)->setForwardMovementOnly(dAtob(data)); return false; };
    static bool setAutoMassInertia(void* obj, const char* data)       { static_cast<t2dSceneObject*>(obj)->setAutoMassInertia(dAtob(data)); return false; };
    static bool setMass(void* obj, const char* data)                  { static_cast<t2dSceneObject*>(obj)->setMass(dAtof(data)); return false; };
    static bool setInertia(void* obj, const char* data)               { static_cast<t2dSceneObject*>(obj)->setInertialMoment(dAtof(data)); return false; };
    static bool setDensity(void* obj, const char* data)               { static_cast<t2dSceneObject*>(obj)->setDensity(dAtof(data)); return false; };
    static bool setFriction(void* obj, const char* data)              { static_cast<t2dSceneObject*>(obj)->setDynamicFriction(dAtof(data)); return false; };
    static bool setRestitution(void* obj, const char* data)           { static_cast<t2dSceneObject*>(obj)->setRestitution(dAtof(data)); return false; };
    static bool setDamping(void* obj, const char* data)               { static_cast<t2dSceneObject*>(obj)->setDamping(dAtof(data)); return false; };
    static bool setLinearVelocity(void* obj, const char* data)        { static_cast<t2dSceneObject*>(obj)->setLinearVelocity(getStringElementVector(data)); return false; };
    static bool setAngularVelocity(void* obj, const char* data)       { static_cast<t2dSceneObject*>(obj)->setAngularVelocity(dAtof(data)); return false; };
    static bool setMinLinearVelocity(void* obj, const char* data)     { static_cast<t2dSceneObject*>(obj)->setMinLinearVelocity(dAtof(data)); return false; };
    static bool setMaxLinearVelocity(void* obj, const char* data)     { static_cast<t2dSceneObject*>(obj)->setMaxLinearVelocity(dAtof(data)); return false; };
    static bool setMinAngularVelocity(void* obj, const char* data)    { static_cast<t2dSceneObject*>(obj)->setMinAngularVelocity(dAtof(data)); return false; };
    static bool setMaxAngularVelocity(void* obj, const char* data)    { static_cast<t2dSceneObject*>(obj)->setMaxAngularVelocity(dAtof(data)); return false; };
    static bool setLinkPoints(void* obj, const char* data)
    {
       static_cast<t2dSceneObject*>(obj)->removeAllLinkPoints();
       U32 count = getStringElementCount(data) >> 1;
       for (U32 i = 0; i < count; i++)
          static_cast<t2dSceneObject*>(obj)->addLinkPoint(getStringElementVector(data, i * 2));

       return false;
    };

    static const char *getLinkPoints( void *obj, const char *data )
    {
       t2dSceneObject* object = static_cast<t2dSceneObject*>( obj );
       U32 count = object->mMountNodes.size();
       S32 maxReturn = count * 64;
       char* returnBuffer = Con::getReturnBuffer( maxReturn );
       returnBuffer[0] = '\0';
       S32 returnLeng = 0;
       for( Vector<tMountNode>::iterator itr = object->mMountNodes.begin(); itr != object->mMountNodes.end(); itr++ )
       {
          // concatenate the next value onto the return string
          dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%.3f %.3f ", (*itr).mLocalMountPosition.mX, (*itr).mLocalMountPosition.mY);
          // update the length of the return string (so far)
          returnLeng = dStrlen(returnBuffer);
       }
       // trim off that last extra space
       if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
          returnBuffer[returnLeng - 1] = '\0';
       return returnBuffer;
    }; 

    // Mounting
    static bool setMountOffset(void* obj, const char* data)            { static_cast<t2dSceneObject*>(obj)->setMountOffset(getStringElementVector(data)); return false; };
    static bool setMountForce(void* obj, const char* data)             { static_cast<t2dSceneObject*>(obj)->setMountForce(dAtof(data)); return false; };
    static bool setMountTrackRotation(void* obj, const char* data)     { static_cast<t2dSceneObject*>(obj)->setMountTrackRotation(dAtob(data)); return false; };
    static bool setMountOwned(void* obj, const char* data)             { static_cast<t2dSceneObject*>(obj)->setMountOwned(dAtob(data)); return false; };
    static bool setMountInheritAttributes(void* obj, const char* data) { static_cast<t2dSceneObject*>(obj)->setMountInheritAttributes(dAtob(data)); return false; };

	//%PUAP%
	void setUsesPhysics( bool val )			{ mUsesPhysics = val; }
	bool getUsesPhysics(void) const			{ return mUsesPhysics; }


#ifdef USE_COMPONENTS
	void onUpdate(); //-Mat to replace script onUpdate
	void onAddToScene(); //-Mat to replace script onAddToScene
	virtual void OnPositionTargetReached(); // RKS: I added this. This will be called in t2dPhysics::processPositionTarget to skip the script call with the same name.
	virtual void OnPostInit(); // RKS: I added this.
#endif
};


///-----------------------------------------------------------------------------
///
/// T2D Scene Object Timer Event.
///
///-----------------------------------------------------------------------------
class t2dSceneObjectTimerEvent : public SimEvent
{
public:
    t2dSceneObjectTimerEvent( U32 timerPeriod ) : mTimerPeriod(timerPeriod) {};
    ~t2dSceneObjectTimerEvent() {};

    virtual void process(SimObject *object)
    {
        /// Create new Timer Event.
        t2dSceneObjectTimerEvent* pEvent = new t2dSceneObjectTimerEvent( mTimerPeriod );
        /// Post Event.
        (dynamic_cast<t2dSceneObject*>(object))->mPeriodicTimerID = Sim::postEvent( object, pEvent, Sim::getCurrentTime() + mTimerPeriod );

        /// Do script callback.
        /// @note   This *must* be done here in-case the user turns off the timer which would be the one above!
        Con::executef( object, 1, "onTimer" );
    }

private:
    U32 mTimerPeriod;
};

//------------------------------------------------------------------------------
// inlines implementation
//------------------------------------------------------------------------------
// Get World Point.
t2dVector t2dSceneObject::getWorldPoint( const t2dVector &localPoint )
{
    // Transfer local-point.
    t2dVector worldPoint = localPoint;

    // Get Object Half-Size.
    const t2dVector& halfSize = getParentPhysics().getHalfSize();
    // Scale World-Point by Size.
    worldPoint.set( worldPoint.mX * halfSize.mX, worldPoint.mY * halfSize.mY );

    // Transform into object-space.
    transformPoint2D( getRotationMatrix(), worldPoint, getPosition(), worldPoint );
    // Return Local-Point.
    return worldPoint;
}
//------------------------------------------------------------------------------
// Get Local Point.
t2dVector t2dSceneObject::getLocalPoint( const t2dVector &worldPoint )
{
    t2dVector localPoint;

    // Transform into object-space.
    inverseTransformPoint2D( getInverseRotationMatrix(), worldPoint, getPosition(), localPoint );
    // Get Object Half-Size.
    const t2dVector& halfSize = getParentPhysics().getHalfSize();
    // Scale Local Point by Size.
    localPoint.set( localPoint.mX / halfSize.mX, localPoint.mY / halfSize.mY );
    // Return Local-Point.
    return localPoint;
}
//------------------------------------------------------------------------------




#endif // _T2DSCENEOBJECT_H_
