//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Graph Object.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "core/color.h"
#include "math/mMathFn.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dSceneGraph.h"
#include "./t2dUtility.h"
#include "./t2dSceneObject.h"
#include "console/simBase.h"
#include "component/behaviors/behaviorTemplate.h"

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif

#include "component/ComponentGlobals.h" // RKS: I added this


//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------
static const float maxWorldLimitSize    = 1e+4f;

//-----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(t2dSceneObjectDatablock);
IMPLEMENT_CONOBJECT(t2dSceneObject);

t2dSceneObjectDatablock* t2dSceneObject::mDefaultConfig = NULL;

ConsoleFunction(setDefaultSceneObjectDatablock, void, 2, 2, "(obj) Set the default configuration"
				"@return No return value.")
{
   t2dSceneObjectDatablock* db = dynamic_cast<t2dSceneObjectDatablock*>(Sim::findObject(argv[1]));
   if (db)
      t2dSceneObject::setDefaultConfig(db);
}

//-----------------------------------------------------------------------------
// World Limit Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums worldLimitLookup[] =
                {
                { t2dSceneObject::T2D_LIMIT_OFF,    "OFF" },
                { t2dSceneObject::T2D_LIMIT_NULL,   "NULL" },
                { t2dSceneObject::T2D_LIMIT_RIGID,  "RIGID" },
                { t2dSceneObject::T2D_LIMIT_BOUNCE, "BOUNCE" },
                { t2dSceneObject::T2D_LIMIT_CLAMP,  "CLAMP" },
                { t2dSceneObject::T2D_LIMIT_STICKY, "STICKY" },
                { t2dSceneObject::T2D_LIMIT_KILL,   "KILL" },
                };

static EnumTable worldLimitTable(sizeof(worldLimitLookup) /  sizeof(EnumTable::Enums), &worldLimitLookup[0]);

//-----------------------------------------------------------------------------
// World Limit Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dSceneObject::eWorldLimit getWorldLimitEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(worldLimitLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(worldLimitLookup[i].label, label) == 0)
            return((t2dSceneObject::eWorldLimit)worldLimitLookup[i].index);

    // Invalid World Limit!
    AssertFatal(false, "t2dSceneObject::getWorldLimit() - Invalid World Limit!");
    // Bah!
    return t2dSceneObject::T2D_LIMIT_INVALID;
}

//-----------------------------------------------------------------------------
// World Limit Description Script-Enumerator.
//-----------------------------------------------------------------------------
static const char* getWorldLimitDescription(const t2dSceneObject::eWorldLimit limit)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(worldLimitLookup) / sizeof(EnumTable::Enums)); i++)
        if( worldLimitLookup[i].index == limit )
            return worldLimitLookup[i].label;

    // Invalid World Limit!
    AssertFatal(false, "t2dSceneObject::getWorldLimitDescription() - Invalid World Limit!");
    // Bah!
    return StringTable->insert("");
}


//-----------------------------------------------------------------------------
// Source Blend Factor Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums srcBlendFactorLookup[] =
                {
                { GL_ZERO,                  "ZERO"                  },
                { GL_ONE,                   "ONE"                   },
                { GL_DST_COLOR,             "DST_COLOR"             },
                { GL_ONE_MINUS_DST_COLOR,   "ONE_MINUS_DST_COLOR"   },
                { GL_SRC_ALPHA,             "SRC_ALPHA"             },
                { GL_ONE_MINUS_SRC_ALPHA,   "ONE_MINUS_SRC_ALPHA"   },
                { GL_DST_ALPHA,             "DST_ALPHA"             },
                { GL_ONE_MINUS_DST_ALPHA,   "ONE_MINUS_DST_ALPHA"   },
                { GL_SRC_ALPHA_SATURATE,    "SRC_ALPHA_SATURATE"    },
                };

EnumTable srcBlendFactorTable(sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums), &srcBlendFactorLookup[0]);

//-----------------------------------------------------------------------------
// Source Blend Factor Script-Enumerator.
//-----------------------------------------------------------------------------
S32 getSrcBlendFactorEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(srcBlendFactorLookup[i].label, label) == 0)
            return(srcBlendFactorLookup[i].index);

    // Invalid World Limit!
    AssertFatal(false, "t2dSceneObject::getSrcBlendFactor() - Invalid Source Blend Factor!");
    // Bah!
    return GL_INVALID_BLEND_FACTOR;
}


//-----------------------------------------------------------------------------
// Source Blend Factor Description Script-Enumerator.
//-----------------------------------------------------------------------------
const char* getSrcBlendFactorDescription(const GLenum factor)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
        if( srcBlendFactorLookup[i].index == factor )
            return srcBlendFactorLookup[i].label;

    // Invalid World Limit!
    AssertFatal(false, "t2dSceneObject::getSrcBlendFactorDescription() - Invalid Src Factor!");
    // Bah!
    return StringTable->insert("");
}


//-----------------------------------------------------------------------------
// Destination Blend Factor Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums dstBlendFactorLookup[] =
                {
                { GL_ZERO,                  "ZERO" },
                { GL_ONE,                   "ONE" },
                { GL_SRC_COLOR,             "SRC_COLOR" },
                { GL_ONE_MINUS_SRC_COLOR,   "ONE_MINUS_SRC_COLOR" },
                { GL_SRC_ALPHA,             "SRC_ALPHA" },
                { GL_ONE_MINUS_SRC_ALPHA,   "ONE_MINUS_SRC_ALPHA" },
                { GL_DST_ALPHA,             "DST_ALPHA" },
                { GL_ONE_MINUS_DST_ALPHA,   "ONE_MINUS_DST_ALPHA" },
                };

EnumTable dstBlendFactorTable(sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums), &dstBlendFactorLookup[0]);

//-----------------------------------------------------------------------------
// Destination Blend Factor Script-Enumerator.
//-----------------------------------------------------------------------------
S32 getDstBlendFactorEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(dstBlendFactorLookup[i].label, label) == 0)
            return(dstBlendFactorLookup[i].index);

    // Invalid World Limit!
    AssertFatal(false, "t2dSceneObject::getDstBlendFactor() - Invalid Destination Blend Factor!");
    // Bah!
    return GL_INVALID_BLEND_FACTOR;
}


//-----------------------------------------------------------------------------
// Destination Blend Factor Description Script-Enumerator.
//-----------------------------------------------------------------------------
const char* getDstBlendFactorDescription(const GLenum factor)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
        if( dstBlendFactorLookup[i].index == factor )
            return dstBlendFactorLookup[i].label;

    // Invalid World Limit!
    AssertFatal(false, "t2dSceneObject::getDstBlendFactorDescription() - Invalid Dst Factor!");
    // Bah!
    return StringTable->insert("");
}



//-----------------------------------------------------------------------------
// Collision-Status Sort (Collision Time Real).
//
// Sort by the Collision-Time-Real Key in a collision-status list.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK collisionStatusListCollisionTimeRealSort(const void* a, const void* b)
{
    // Fetch Keys.
    const F32 keyA = ((t2dPhysics::cCollisionStatus*)a)->mCollisionTimeReal;
    const F32 keyB = ((t2dPhysics::cCollisionStatus*)b)->mCollisionTimeReal;

    // Return Sort Order.
    if ( keyA < keyB )
        return -1;
    else if ( keyA > keyB )
        return 1;
    else
        return 0;
}

//------------------------------------------------------------------------------

t2dSceneObjectDatablock::t2dSceneObjectDatablock()
{
   // Namespace
   mSuperclass = StringTable->insert("");
   mClass = StringTable->insert("");

   // Random
   mUseMouseEvents = false;
   mEnabled = true;
   mPaused = false;
   mVisible = true;
   mLifetime = 0.0f;

   // Spatial
   mPosition = t2dVector::getZero();
   mSize = t2dVector(10.0f, 10.0f);
   mRotation = 0.0f;
   mAutoRotation = 0.0f;
   mFlipX = false;
   mFlipY = false;
   mLayer = 0;
   mGraphGroup = 0;
   mMountRotation = 0.0f;
   mAutoMountRotation = 0.0f;
   mSortPoint = t2dVector::getZero();

   // World Limits
   mWorldLimitMode = t2dSceneObject::T2D_LIMIT_OFF;
   mWorldLimitMin = t2dVector::getZero();
   mWorldLimitMax = t2dVector::getZero();
   mWorldLimitCallback = false;

   // Collision
   mCollisionActiveSend = false;
   mCollisionActiveReceive = false;
   mCollisionPhysicsSend = true;
   mCollisionPhysicsReceive = true;
   mCollisionGroups = T2D_MASK_ALL;
   mCollisionLayers = T2D_MASK_ALL;
   mCollisionCallback = false;
   mCollisionPolyScale = t2dVector(1.0f, 1.0f);
   mCollisionCircleScale = 1.0f;
   mCollisionMaxIterations = 1;
   mCollisionDetectionMode = t2dPhysics::T2D_DETECTION_POLY_ONLY;
   mCollisionResponseMode = t2dPhysics::T2D_RESPONSE_CLAMP;
   mCollisionCircleSuperscribed = true;
   VECTOR_SET_ASSOCIATION(mCollisionPolyCustom);
   mCollisionPolyCustom.push_back(t2dVector(-1.0f, -1.0f));
   mCollisionPolyCustom.push_back(t2dVector(1.0f, -1.0f));
   mCollisionPolyCustom.push_back(t2dVector(1.0f, 1.0f));
   mCollisionPolyCustom.push_back(t2dVector(-1.0f, 1.0f));

   // Blending
   mBlendingEnabled = true;
   mSrcBlendFactor = GL_SRC_ALPHA;
   mDstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
   mBlendColor = ColorF(1.0f, 1.0f, 1.0f, 1.0f);

   // Physics
   mUsesPhysics = false;
   mConstantForce = t2dVector::getZero();
   mConstantForceGravitic = false;
   mForceScale = 1.0f;
   mImmovable = false;
   mForwardOnly = false;
   mAutoMassInertia = true;
   mMass = 1.0f;
   mInertia = 16.6f;
   mDensity = 0.01f;
   mFriction = 0.3f;
   mRestitution = 1.0f;
   mDamping = 0.0f;
   mLinearVelocity = t2dVector::getZero();
   mAngularVelocity = 0.0f;
   mMinLinearVelocity  = 0.0f;
   mMinAngularVelocity = 0.0f;
   mMaxLinearVelocity  = 10.0e+3f;
   mMaxAngularVelocity = 10.0e+3f;
   VECTOR_SET_ASSOCIATION(mLinkPoints);

   // Mounting
   mMountOffset = t2dVector::getZero();
   mMountForce = 0.0f;
   mMountTrackRotation = true;
   mMountOwned = true;
   mMountInheritAttributes = true;
}

//------------------------------------------------------------------------------

t2dSceneObjectDatablock::~t2dSceneObjectDatablock()
{
}
//------------------------------------------------------------------------------

bool t2dSceneObjectDatablock::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

void t2dSceneObjectDatablock::onRemove()
{
   // Clean up the reference set
   if( mSceneObjects.isProperlyAdded() )
      mSceneObjects.unregisterObject();

   Parent::onRemove();
}


void t2dSceneObjectDatablock::onStaticModified(const char* slotName)
{
   // Update all scene objects referencing this config.
   for (S32 i = 0; i < mSceneObjects.size(); i++)
   {
      t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(mSceneObjects[i]);
      if (!sceneObject)
         continue;

      const char* newValue = getDataField(slotName, NULL);

      // Since the scene object's fields are all protected, the appropriate
      // setter function will be called.
      if (newValue)
         sceneObject->setDataField(slotName, NULL, newValue);
      else
         sceneObject->setDataField(slotName, NULL, "");
   }
}

void t2dSceneObjectDatablock::initPersistFields()
{
   Parent::initPersistFields();

   // Namespace
   addField("SuperClass", TypeString, Offset(mSuperclass, t2dSceneObjectDatablock));
   addField("Class", TypeString, Offset(mClass, t2dSceneObjectDatablock));

   // Random
   addField("UseMouseEvents", TypeBool, Offset(mUseMouseEvents, t2dSceneObjectDatablock));
   addField("Enabled", TypeBool, Offset(mEnabled, t2dSceneObjectDatablock));
   addField("Paused", TypeBool, Offset(mPaused, t2dSceneObjectDatablock));
   addField("Visible", TypeBool, Offset(mVisible, t2dSceneObjectDatablock));
   addField("Lifetime", TypeF32, Offset(mLifetime, t2dSceneObjectDatablock));

   // Spatial
   addField("Position", TypePoint2F, Offset(mPosition, t2dSceneObjectDatablock));
   addField("Size", TypePoint2F, Offset(mSize, t2dSceneObjectDatablock));
   addField("Rotation", TypeF32, Offset(mRotation, t2dSceneObjectDatablock));
   addField("AutoRotation", TypeF32, Offset(mAutoRotation, t2dSceneObjectDatablock));
   addField("FlipX", TypeBool, Offset(mFlipX, t2dSceneObjectDatablock));
   addField("FlipY", TypeBool, Offset(mFlipY, t2dSceneObjectDatablock));
   addField("SortPoint", TypePoint2F, Offset(mSortPoint, t2dSceneObjectDatablock));
   addField("Layer", TypeS32, Offset(mLayer, t2dSceneObjectDatablock));
   addField("GraphGroup", TypeS32, Offset(mGraphGroup, t2dSceneObjectDatablock));
   addField("MountRotation", TypeF32, Offset(mMountRotation, t2dSceneObjectDatablock));
   addField("AutoMountRotation", TypeF32, Offset(mAutoMountRotation, t2dSceneObjectDatablock));

   // World Limits
   addField("WorldLimitMode", TypeEnum, Offset(mWorldLimitMode, t2dSceneObjectDatablock), 1, &worldLimitTable);
   addField("WorldLimitMin", TypePoint2F, Offset(mWorldLimitMin, t2dSceneObjectDatablock));
   addField("WorldLimitMax", TypePoint2F, Offset(mWorldLimitMax, t2dSceneObjectDatablock));
   addField("WorldLimitCallback", TypeBool, Offset(mWorldLimitCallback, t2dSceneObjectDatablock));

   // Collision
   addField("CollisionActiveSend", TypeBool, Offset(mCollisionActiveSend, t2dSceneObjectDatablock));
   addField("CollisionActiveReceive", TypeBool, Offset(mCollisionActiveReceive, t2dSceneObjectDatablock));
   addField("CollisionPhysicsSend", TypeBool, Offset(mCollisionPhysicsSend, t2dSceneObjectDatablock));
   addField("CollisionPhysicsReceive", TypeBool, Offset(mCollisionPhysicsReceive, t2dSceneObjectDatablock));
   addField("CollisionGroups", TypeS32, Offset(mCollisionGroups, t2dSceneObjectDatablock));
   addField("CollisionLayers", TypeS32, Offset(mCollisionLayers, t2dSceneObjectDatablock));
   addField("CollisionCallback", TypeBool, Offset(mCollisionCallback, t2dSceneObjectDatablock));
   addField("CollisionPolyScale", TypePoint2F, Offset(mCollisionPolyScale, t2dSceneObjectDatablock));
   addField("CollisionCircleScale", TypeF32, Offset(mCollisionCircleScale, t2dSceneObjectDatablock));
   addField("CollisionMaxIterations", TypeS32, Offset(mCollisionMaxIterations, t2dSceneObjectDatablock));
   addField("CollisionDetectionMode", TypeEnum, Offset(mCollisionDetectionMode, t2dSceneObjectDatablock), 1, &collisionDetectionTable);
   addField("CollisionResponseMode", TypeEnum, Offset(mCollisionResponseMode, t2dSceneObjectDatablock), 1, &collisionResponseTable);
   addField("CollisionCircleSuperscribed", TypeBool, Offset(mCollisionCircleSuperscribed, t2dSceneObjectDatablock));
   addField("CollisionPolyList", TypePoint2FVector, Offset(mCollisionPolyCustom, t2dSceneObjectDatablock));

   // Blending
   addField("BlendingEnabled", TypeBool, Offset(mBlendingEnabled, t2dSceneObjectDatablock));
   addField("SrcBlendFactor", TypeEnum, Offset(mSrcBlendFactor, t2dSceneObjectDatablock), 1, &srcBlendFactorTable);
   addField("DstBlendFactor", TypeEnum, Offset(mDstBlendFactor, t2dSceneObjectDatablock), 1, &dstBlendFactorTable);
   addField("BlendColor", TypeColorF, Offset(mBlendColor, t2dSceneObjectDatablock));

   // Physics
   addField("UsesPhysics", TypeBool, Offset(mUsesPhysics, t2dSceneObjectDatablock));
   addField("ConstantForce", TypePoint2F, Offset(mConstantForce, t2dSceneObjectDatablock));
   addField("ConstantForceGravitic", TypeBool, Offset(mConstantForceGravitic, t2dSceneObjectDatablock));
   addField("ForceScale", TypeF32, Offset(mForceScale, t2dSceneObjectDatablock));
   addField("Immovable", TypeBool, Offset(mImmovable, t2dSceneObjectDatablock));
   addField("ForwardOnly", TypeBool, Offset(mForwardOnly, t2dSceneObjectDatablock));
   addField("AutoMassInertia", TypeBool, Offset(mAutoMassInertia, t2dSceneObjectDatablock));
   addField("Mass", TypeF32, Offset(mMass, t2dSceneObjectDatablock));
   addField("Inertia", TypeF32, Offset(mInertia, t2dSceneObjectDatablock));
   addField("Density", TypeF32, Offset(mDensity, t2dSceneObjectDatablock));
   addField("Friction", TypeF32, Offset(mFriction, t2dSceneObjectDatablock));
   addField("Restitution", TypeF32, Offset(mRestitution, t2dSceneObjectDatablock));
   addField("Damping", TypeF32, Offset(mDamping, t2dSceneObjectDatablock));
   addField("LinearVelocity", TypePoint2F, Offset(mLinearVelocity, t2dSceneObjectDatablock));
   addField("AngularVelocity", TypeF32, Offset(mAngularVelocity, t2dSceneObjectDatablock));
   addField("MinLinearVelocity", TypeF32, Offset(mMinLinearVelocity, t2dSceneObjectDatablock));
   addField("MaxLinearVelocity", TypeF32, Offset(mMaxLinearVelocity, t2dSceneObjectDatablock));
   addField("MinAngularVelocity", TypeF32, Offset(mMinAngularVelocity, t2dSceneObjectDatablock));
   addField("MaxAngularVelocity", TypeF32, Offset(mMaxAngularVelocity, t2dSceneObjectDatablock));
   addField("LinkPoints", TypePoint2FVector, Offset(mLinkPoints, t2dSceneObjectDatablock));

   // Mounting
   addField("MountOffset", TypePoint2F, Offset(mMountOffset, t2dSceneObjectDatablock));
   addField("MountForce", TypeF32, Offset(mMountForce, t2dSceneObjectDatablock));
   addField("MountTrackRotation", TypeBool, Offset(mMountTrackRotation, t2dSceneObjectDatablock));
   addField("MountOwned", TypeBool, Offset(mMountOwned, t2dSceneObjectDatablock));
   addField("MountInheritAttributes", TypeBool, Offset(mMountInheritAttributes, t2dSceneObjectDatablock));
}

void t2dSceneObjectDatablock::addSceneObjectReference(t2dSceneObject* object)
{
   mSceneObjects.addObject(object);
}

void t2dSceneObjectDatablock::removeSceneObjectReference(t2dSceneObject* object)
{
   mSceneObjects.removeObject(object);
}

//------------------------------------------------------------------------------

void t2dSceneObjectDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);
}

//------------------------------------------------------------------------------

void t2dSceneObjectDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);
}

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dSceneObject::t2dSceneObject() :      T2D_Stream_HeaderID(makeFourCCTag('2','D','S','O')),
                                        mConfigDataBlock(NULL),
                                        mpAttachedCamera(NULL),
                                        mVisible(true),
                                        mPaused(false),
                                        mProcessTick(true),
                                        mMountRotation(0.0f),
                                        mFlipX(false),
                                        mFlipY(false),
                                        mSortPoint(0.0f,0.0f),
                                        mAutoMountRotation(0.0f),
                                        mAutoMountRotationActive(false),
                                        mLayer(0),
                                        mGraphGroup(0),
                                        mLayerMask(BIT(mLayer)),
                                        mGraphGroupMask(BIT(mGraphGroup)),
                                        mWorldLimitMode(T2D_LIMIT_OFF),
                                        mWorldLimitMin(0.0f, 0.0f),
                                        mWorldLimitMax(0.0f, 0.0f),
                                        mWorldLimitCallback(false),
                                        mLifetime(0.0f),
                                        mLifetimeActive(false),
                                        mpSceneGraph(NULL),
                                        mpSceneObjectGroup(NULL),
                                        mpBinReferenceChain(NULL),
                                        mCollisionActiveSend(false),
                                        mCollisionActiveReceive(false),
                                        mCollisionPhysicsSend(true),
                                        mCollisionPhysicsReceive(true),
                                        mCollisionLayerMask(T2D_MASK_ALL),
                                        mCollisionGroupMask(T2D_MASK_ALL),
                                        mCollisionCallback(false),
                                        mCollisionSuppress(false),
                                        mContainerSequenceKey(0),
                                        mUpdateSequenceKey(0),
                                        mBlending(true),
                                        mSrcBlendFactor(GL_SRC_ALPHA),
                                        mDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA),
                                        mBlendColour(ColorF(1.0f,1.0f,1.0f,1.0f)),
                                        mpMountedTo(NULL),
                                        mMountPreRotation(0.0f),
                                        mEnableChild(false),
                                        mLocalSerialiseID(1),
                                        mSerialiseKey(0),
                                        mPickingAllowed(true),
                                        mEnableAlwaysScope(false),
                                        mSafeDeleteReady(true),
                                        mBeingSafeDeleted(false),
                                        mDebugMask(0X00000000),
                                        mUseMouseEvents(false),
                                        mMouseLocked(false),
                                        mPeriodicTimerID(0),
                                        mpAttachedGui(NULL),
                                        mpAttachedGuiSceneWindow(NULL),
                                        mAttachedGuiSizeControl(false),
                                        mInitialUpdate(false),
                                        mMountToID(0),
                                        mMountOffset(0.0f, 0.0f),
                                        mMountForce(0.0f),
                                        mMountTrackRotation(true),
                                        mMountOwned(true),
                                        mMountInheritAttributes(true),
                                        mNeverSolvePhysics(false),
                                        mFieldSelectedSceneGraph(NULL),
                                        mUpdateCallback(false),
										mUsesPhysics(false)
{
    // Initialise Process Link.
    processInitialiseLink();

    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mDestroyNotifyList );
    VECTOR_SET_ASSOCIATION( mMountNodes );
    VECTOR_SET_ASSOCIATION( mPotentialCollisionObjectList );
    VECTOR_SET_ASSOCIATION( mPotentialCollisionStatusList );
    VECTOR_SET_ASSOCIATION( mCollisionPhysicsList );

    // Set Default Allowances.
    mCollisionSendAllowed = true;
    mCollisionReceiveAllowed = true;

    // Reset Mount Rotation Matrix.
    mMountRotationMatrix.orientate( mMountRotation );

    // Initialise Physics.
    initializePhysics();
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dSceneObject::~t2dSceneObject()
{
    // Are we in a Scene Graph?
    if ( mpSceneGraph )
    {
        // Yes, so remove from Scene Graph.
        mpSceneGraph->removeFromScene( this );
    }

    // Check Object.
    AssertFatal( mpBinReferenceChain == NULL, "t2dSceneObject::~t2dSceneObject() - Object is still referenced in Container System!" );
}

void t2dSceneObject::initPersistFields()
{
   Parent::initPersistFields();

   addProtectedField("scenegraph", TypeSimObjectPtr, Offset(mpSceneGraph, t2dSceneObject), &setSceneGraph, &defaultProtectedGetFn, "");
   addProtectedField("config", TypeSimObjectName, Offset(mConfigDataBlock, t2dSceneObject), &setConfigDatablock, &defaultProtectedGetFn, "");

   // Random
   addProtectedField("UseMouseEvents", TypeBool, Offset(mUseMouseEvents, t2dSceneObject), &setUseMouseEvents, &defaultProtectedGetFn, "");
   addProtectedField("Paused", TypeBool, Offset(mPaused, t2dSceneObject), &setPaused, &defaultProtectedGetFn, "");
   addProtectedField("Visible", TypeBool, Offset(mVisible, t2dSceneObject), &setVisible, &defaultProtectedGetFn, "");
   addProtectedField("Lifetime", TypeF32, Offset(mLifetime, t2dSceneObject), &setLifetime, &defaultProtectedGetFn, "");

   // Spatial
   // Physics properties are stored in a separate t2dPhysics class, but the offset macro should still work in
   // getting its memory location.
   addProtectedField("Position", TypePoint2F, Offset(mParentPhysics.mPosition, t2dSceneObject), &setPosition, &defaultProtectedGetFn, "");
   addProtectedField("Size", TypePoint2F, Offset(mParentPhysics.mSize, t2dSceneObject), &setSize, &defaultProtectedGetFn, "");
   addProtectedField("Rotation", TypeF32, Offset(mParentPhysics.mRotation, t2dSceneObject), &setRotation, &defaultProtectedGetFn, "");
   addProtectedField("AutoRotation", TypeF32, Offset(mParentPhysics.mAutoRotation, t2dSceneObject), &setAutoRotation, &defaultProtectedGetFn, "");
   addProtectedField("FlipX", TypeBool, Offset(mFlipX, t2dSceneObject), &setFlipX, &defaultProtectedGetFn, "");
   addProtectedField("FlipY", TypeBool, Offset(mFlipY, t2dSceneObject), &setFlipY, &defaultProtectedGetFn, "");
   addProtectedField("SortPoint", TypePoint2F, Offset(mSortPoint, t2dSceneObject), &setSortPoint, &defaultProtectedGetFn, "");
   addProtectedField("Layer", TypeS32, Offset(mLayer, t2dSceneObject), &setLayer, &defaultProtectedGetFn, "");
   addProtectedField("GraphGroup", TypeS32, Offset(mGraphGroup, t2dSceneObject), &setGraphGroup, &defaultProtectedGetFn, "");
   addProtectedField("MountRotation", TypeF32, Offset(mMountRotation, t2dSceneObject), &setMountRotation, &defaultProtectedGetFn, "");
   addProtectedField("AutoMountRotation", TypeF32, Offset(mAutoMountRotation, t2dSceneObject), &setAutoMountRotation, &defaultProtectedGetFn, "");

   // World Limits
   addProtectedField("WorldLimitMode", TypeEnum, Offset(mWorldLimitMode, t2dSceneObject), &setWorldLimitMode, &defaultProtectedGetFn, 1, &worldLimitTable);
   addProtectedField("WorldLimitMin", TypePoint2F, Offset(mWorldLimitMin, t2dSceneObject), &setWorldLimitMin, &defaultProtectedGetFn, "");
   addProtectedField("WorldLimitMax", TypePoint2F, Offset(mWorldLimitMax, t2dSceneObject), &setWorldLimitMax, &defaultProtectedGetFn, "");
   addProtectedField("WorldLimitCallback", TypeBool, Offset(mWorldLimitCallback, t2dSceneObject), &setWorldLimitCallback, &defaultProtectedGetFn, "");

   // Collision
   addProtectedField("CollisionActiveSend", TypeBool, Offset(mCollisionActiveSend, t2dSceneObject), &setCollisionActiveSend, &defaultProtectedGetFn, "");
   addProtectedField("CollisionActiveReceive", TypeBool, Offset(mCollisionActiveReceive, t2dSceneObject), &setCollisionActiveReceive, &defaultProtectedGetFn, "");
   addProtectedField("CollisionPhysicsSend", TypeBool, Offset(mCollisionPhysicsSend, t2dSceneObject), &setCollisionPhysicsSend, &defaultProtectedGetFn, "");
   addProtectedField("CollisionPhysicsReceive", TypeBool, Offset(mCollisionPhysicsReceive, t2dSceneObject), &setCollisionPhysicsReceive, &defaultProtectedGetFn, "");
   addProtectedField("CollisionGroups", TypeS32, Offset(mCollisionGroupMask, t2dSceneObject), &setCollisionGroups, &defaultProtectedGetFn, "");
   addProtectedField("CollisionLayers", TypeS32, Offset(mCollisionLayerMask, t2dSceneObject), &setCollisionLayers, &defaultProtectedGetFn, "");
   addProtectedField("CollisionCallback", TypeBool, Offset(mCollisionCallback, t2dSceneObject), &setCollisionCallback, &defaultProtectedGetFn, "");
   addProtectedField("CollisionPolyScale", TypePoint2F, Offset(mParentPhysics.mCollisionPolyScale, t2dSceneObject), &setCollisionPolyScale, &defaultProtectedGetFn, "");
   addProtectedField("CollisionCircleScale", TypeF32, Offset(mParentPhysics.mCollisionCircleScale, t2dSceneObject), &setCollisionCircleScale, &defaultProtectedGetFn, "");
   addProtectedField("CollisionMaxIterations", TypeS32, Offset(mParentPhysics.mCollisionMaxIterations, t2dSceneObject), &setCollisionMaxIterations, &defaultProtectedGetFn, "");
   addProtectedField("CollisionDetectionMode", TypeEnum, Offset(mParentPhysics.mCollisionDetectionMode, t2dSceneObject), &setCollisionDetectionMode, &defaultProtectedGetFn, 1, &collisionDetectionTable);
   addProtectedField("CollisionResponseMode", TypeEnum, Offset(mParentPhysics.mCollisionResponseMode, t2dSceneObject), &setCollisionResponseMode, &defaultProtectedGetFn, 1, &collisionResponseTable);
   addProtectedField("CollisionCircleSuperscribed", TypeBool, Offset(mParentPhysics.mCollisionCircleSuperscribed, t2dSceneObject), &setCollisionCircleSuperscribed, &defaultProtectedGetFn, "");
   addProtectedField("CollisionPolyList", TypePoint2FVector, Offset(mParentPhysics.mCollisionPolyBasisList, t2dSceneObject), &setCollisionPolyCustom, &defaultProtectedGetFn, "");

   // Blending
   addProtectedField("BlendingEnabled", TypeBool, Offset(mBlending, t2dSceneObject), &setBlendingEnabled, &defaultProtectedGetFn, "");
   addProtectedField("SrcBlendFactor", TypeEnum, Offset(mSrcBlendFactor, t2dSceneObject), &setSrcBlendFactor, &defaultProtectedGetFn, 1, &srcBlendFactorTable);
   addProtectedField("DstBlendFactor", TypeEnum, Offset(mDstBlendFactor, t2dSceneObject), &setDstBlendFactor, &defaultProtectedGetFn, 1, &dstBlendFactorTable);
   addProtectedField("BlendColor", TypeColorF, Offset(mBlendColour, t2dSceneObject), &setBlendColor, &defaultProtectedGetFn, "");

   // Physics
   addProtectedField("UsesPhysics", TypeBool, Offset(mUsesPhysics, t2dSceneObject), &setUsesPhysics, &defaultProtectedGetFn, "");
   addProtectedField("ConstantForce", TypePoint2F, Offset(mParentPhysics.mConstantForce, t2dSceneObject), &setConstantForce, &defaultProtectedGetFn, "");
   addProtectedField("ConstantForceGravitic", TypeBool, Offset(mParentPhysics.mGraviticConstantForce, t2dSceneObject), &setConstantForceGravitic, &defaultProtectedGetFn, "");
   addProtectedField("ForceScale", TypeF32, Offset(mParentPhysics.mForceScale, t2dSceneObject), &setForceScale, &defaultProtectedGetFn, "");
   addProtectedField("Immovable", TypeBool, Offset(mParentPhysics.mImmovable, t2dSceneObject), &setImmovable, &defaultProtectedGetFn, "");
   addProtectedField("ForwardOnly", TypeBool, Offset(mParentPhysics.mForwardMovementOnly, t2dSceneObject), &setForwardOnly, &defaultProtectedGetFn, "");
   addProtectedField("AutoMassInertia", TypeBool, Offset(mParentPhysics.mAutoMassInertia, t2dSceneObject), &setAutoMassInertia, &defaultProtectedGetFn, "");
   addProtectedField("Mass", TypeF32, Offset(mParentPhysics.mMass, t2dSceneObject), &setMass, &defaultProtectedGetFn, "");
   addProtectedField("Inertia", TypeF32, Offset(mParentPhysics.mInertialMoment, t2dSceneObject), &setInertia, &defaultProtectedGetFn, "");
   addProtectedField("Density", TypeF32, Offset(mParentPhysics.mDensity, t2dSceneObject), &setDensity, &defaultProtectedGetFn, "");
   addProtectedField("Friction", TypeF32, Offset(mParentPhysics.mCoefDynamicFriction, t2dSceneObject), &setFriction, &defaultProtectedGetFn, "");
   addProtectedField("Restitution", TypeF32, Offset(mParentPhysics.mCoefRestitution, t2dSceneObject), &setRestitution, &defaultProtectedGetFn, "");
   addProtectedField("Damping", TypeF32, Offset(mParentPhysics.mDamping, t2dSceneObject), &setDamping, &defaultProtectedGetFn, "");
   addProtectedField("LinearVelocity", TypePoint2F, Offset(mParentPhysics.mGrossLinearVelocity, t2dSceneObject), &setLinearVelocity, &defaultProtectedGetFn, "");
   addProtectedField("AngularVelocity", TypeF32, Offset(mParentPhysics.mGrossAngularVelocity, t2dSceneObject), &setAngularVelocity, &defaultProtectedGetFn, "");
   addProtectedField("MinLinearVelocity", TypeF32, Offset(mParentPhysics.mMinLinearVelocity, t2dSceneObject), &setMinLinearVelocity, &defaultProtectedGetFn, "");
   addProtectedField("MaxLinearVelocity", TypeF32, Offset(mParentPhysics.mMaxLinearVelocity, t2dSceneObject), &setMaxLinearVelocity, &defaultProtectedGetFn, "");
   addProtectedField("MinAngularVelocity", TypeF32, Offset(mParentPhysics.mMinAngularVelocity, t2dSceneObject), &setMinAngularVelocity, &defaultProtectedGetFn, "");
   addProtectedField("MaxAngularVelocity", TypeF32, Offset(mParentPhysics.mMaxAngularVelocity, t2dSceneObject), &setMaxAngularVelocity, &defaultProtectedGetFn, "");
   addProtectedField("LinkPoints", TypePoint2FVector, Offset(mMountNodesList, t2dSceneObject), &setLinkPoints, &getLinkPoints, "");

   // Mounting
   addProtectedField("MountOffset", TypePoint2F, Offset(mMountOffset, t2dSceneObject), &setMountOffset, &defaultProtectedGetFn, "");
   addProtectedField("MountForce", TypeF32, Offset(mMountForce, t2dSceneObject), &setMountForce, &defaultProtectedGetFn, "");
   addProtectedField("MountTrackRotation", TypeBool, Offset(mMountTrackRotation, t2dSceneObject), &setMountTrackRotation, &defaultProtectedGetFn, "");
   addProtectedField("MountOwned", TypeBool, Offset(mMountOwned, t2dSceneObject), &setMountOwned, &defaultProtectedGetFn, "");
   addProtectedField("MountInheritAttributes", TypeBool, Offset(mMountInheritAttributes, t2dSceneObject), &setMountInheritAttributes, &defaultProtectedGetFn, "");
}

bool t2dSceneObject::setWorldLimitMode(void* obj, const char* data)
{
   static_cast<t2dSceneObject*>(obj)->setWorldLimitMode(getWorldLimitEnum(data));
   return false;
};

//------------------------------------------------------------------------------
// Get First Non-Whitespace.
//------------------------------------------------------------------------------
const char* t2dSceneObject::getFirstNonWhitespace( const char* inString )
{
    // Search for first non-whitespace.
   while(*inString == ' ' || *inString == '\n' || *inString == '\t')
      inString++;

   // Return found point.
   return inString;
}


//------------------------------------------------------------------------------
// Get String Element Vector.
// NOTE:-   You must verify that elements (index/index+1) are valid first!
//------------------------------------------------------------------------------
t2dVector t2dSceneObject::getStringElementVector( const char* inString, const U32 index )
{
    if ((index + 1) >= getStringElementCount(inString))
       return t2dVector::getZero();

    // Get String Element Vector.
    return t2dVector( dAtof(getStringElement(inString,index)), dAtof(getStringElement(inString,index+1)) );
}


//------------------------------------------------------------------------------
// Get String Element Vector 3D.
// NOTE:-   You must verify that elements (index/index+1/index+2) are valid first!
//------------------------------------------------------------------------------
VectorF t2dSceneObject::getStringElementVector3D( const char* inString, const U32 index )
{
    if ((index + 2) >= getStringElementCount(inString))
       return VectorF(0.0f, 0.0f, 0.0f);

    // Get String Element Vector.
    return VectorF( dAtof(getStringElement(inString,index)), dAtof(getStringElement(inString,index+1)), dAtof(getStringElement(inString,index+2)) );
}



//------------------------------------------------------------------------------
// Get String Element.
//------------------------------------------------------------------------------
const char* t2dSceneObject::getStringElement( const char* inString, const U32 index )
{
    // Non-whitespace chars.
    static const char* set = " \t\n";

    U32 wordCount = 0;
    U8 search = 0;
    const char* pWordStart = NULL;

    // End of string?
    if ( *inString != 0 )
    {
        // No, so search string.
        while( *inString )
        {
            // Get string element.
            search = *inString;

            // End of string?
            if ( search == 0 )
                break;

            // Move to next element.
            inString++;

            // Search for seperators.
            for( U32 i = 0; set[i]; i++ )
            {
                // Found one?
                if( search == set[i] )
                {
                    // Yes...
                    search = 0;
                    break;
                }   
            }

            // Found a seperator?
            if ( search == 0 )
                continue;

            // Found are word?
            if ( wordCount == index )
            {
                // Yes, so mark it.
                pWordStart = inString-1;
            }

            // We've found a non-seperator.
            wordCount++;

            // Search for end of non-seperator.
            while( 1 )
            {
                // Get string element.
                search = *inString;

                // End of string?
                if ( search == 0 )
                    break;

                // Move to next element.
                inString++;

                // Search for seperators.
                for( U32 i = 0; set[i]; i++ )
                {
                    // Found one?
                    if( search == set[i] )
                    {
                        // Yes...
                        search = 0;
                        break;
                    }   
                }

                // Found Seperator?
                if ( search == 0 )
                    break;
            }

            // Have we found our word?
            if ( pWordStart )
            {
                // Yes, so we've got our word...

                // Result Buffer.
                static char buffer[4096];

                // Calculate word length.
                const U32 length = inString - pWordStart - ((*inString)?1:0);

                // Copy Word.
                dStrncpy( buffer, pWordStart, length);
                buffer[length] = '\0';

                // Return Word.
                return buffer;
            }

            // End of string?
            if ( *inString == 0 )
            {
                // Bah!
                break;
            }
        }
    }

    // Sanity!
    AssertFatal( false, "t2dSceneObject::getStringElement() - Couldn't find specified string element!" );
    // Didn't find it
    return " ";
}   


//------------------------------------------------------------------------------
// Get String Element Count.
//------------------------------------------------------------------------------
U32 t2dSceneObject::getStringElementCount( const char* inString )
{
    // Non-whitespace chars.
    static const char* set = " \t\n";

    // End of string.
    if ( *inString == 0 )
        return 0;

    U32 wordCount = 0;
    U8 search = 0;

    // Search String.
    while( *inString )
    {
        // Get string element.
        search = *inString;

        // End of string?
        if ( search == 0 )
            break;

        // Move to next element.
        inString++;

        // Search for seperators.
        for( U32 i = 0; set[i]; i++ )
        {
            // Found one?
            if( search == set[i] )
            {
                // Yes...
                search = 0;
                break;
            }   
        }

        // Found a seperator?
        if ( search == 0 )
            continue;

        // We've found a non-seperator.
        wordCount++;

        // Search for end of non-seperator.
        while( 1 )
        {
            // Get string element.
            search = *inString;

            // End of string?
            if ( search == 0 )
                break;

            // Move to next element.
            inString++;

            // Search for seperators.
            for( U32 i = 0; set[i]; i++ )
            {
                // Found one?
                if( search == set[i] )
                {
                    // Yes...
                    search = 0;
                    break;
                }   
            }

            // Found Seperator?
            if ( search == 0 )
                break;
        }

        // End of string?
        if ( *inString == 0 )
        {
            // Bah!
            break;
        }
    }

    // We've finished.
    return wordCount;
}


//-----------------------------------------------------------------------------
// Add to Scene.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, addToScene, void, 3, 3, "(t2dSceneGraph scenegraph) Add the object to a scenegraph.\n"
                                                      "@param scenegraph the scenegraph you wish to add this object to."
													  "@return No return value.")
{
    // Find t2dSceneGraph Object.
    t2dSceneGraph* pT2DSceneGraph = dynamic_cast<t2dSceneGraph*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pT2DSceneGraph )
    {
        Con::warnf("t2dSceneObject::addToScene() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Add to Scene.
    pT2DSceneGraph->addToScene( object );
}   


//-----------------------------------------------------------------------------
// Remove from Scene.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, removeFromScene, void, 2, 2, "() Remove the object from the scenegraph."
			  "@return No return value.")
{
    // Check we're in a SceneGraph.
    if ( !object->getSceneGraph() )
    {
        Con::warnf("t2dSceneObject::removeFromScene() - Object is not in Scene! (%s)", object->getIdString());
        return;
    }

    // Remove from Scene.
    object->getSceneGraph()->removeFromScene( object );
}  

ConsoleMethod( t2dSceneObject, getSceneGraph, S32, 2, 2, "() Get the scenegraph the object is in.\n"
                                                         "@return (t2dSceneGraph scenegraph) The scenegraph this object is currently in.")
{
   return object->getSceneGraph() ? object->getSceneGraph()->getId() : 0;
}

//-----------------------------------------------------------------------------
// Set Enabled.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setEnabled, void, 3, 3, "(bool status) Enables or disables the object.\n"
                                                      "@param status Whether to enable or disable the object."
													  "@return No return value.")
{
    // Set Enabled.
    object->setEnabled( dAtob(argv[2]) );

    // Update Container Configuration.
    object->updateSpatialConfig();
}

//-----------------------------------------------------------------------------
// Set Visible.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setVisible, void, 3, 3, "(bool status) - Show or hide the object.\n"
                                                      "@param status Whether to enable or disable visiblity on the object."
													  "@return No return value.")
{
    // Set Visible.
    object->setVisible( dAtob(argv[2]) );
}   
// Set Visible.
void t2dSceneObject::setVisible( const bool status )
{
    // Set Visible Status.
    mVisible = status;
}


//-----------------------------------------------------------------------------
// Get Visible Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getVisible, bool, 2, 2, "() - Gets the object's visible status.\n"
                                                      "@return (bool status) Whether or not the object is visible.")
{
    // Get Visible Status.
    return object->getVisible();
}   


//-----------------------------------------------------------------------------
// Set Paused.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPaused, void, 3, 3, "(bool status) - Pauses or unpauses the object.\n"
                                                     "@param status Whether to pause or unpause the object."
													  "@return No return value.")
{
    // Set Paused.
    object->setPaused( dAtob(argv[2]) );
}   
// Set Paused.
void t2dSceneObject::setPaused( const bool status )
{
    // Set Paused Status.
    mPaused = status;
}


//-----------------------------------------------------------------------------
// Get Paused Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getPaused, bool, 2, 2, "() Gets the object's paused status.\n"
                                                     "@return (bool status) Whether or not the object is currently paused.")
{
    // Get Paused Status.
    return object->getPaused();
}   

void t2dSceneObject::setMouseLocked(bool locked)
{
   if (!getSceneGraph())
   {
      Con::errorf("t2dSceneObject::setMouseLocked - Object is not in a scenegraph!");
      return;
   }

   if (locked && !mMouseLocked)
      getSceneGraph()->setObjectMouseLock(this);
   else if (!locked && mMouseLocked)
      getSceneGraph()->clearObjectMouseLock(this);

   mMouseLocked = locked;
}

ConsoleMethod(t2dSceneObject, setMouseLocked, void, 3, 3, "(bool status) Allows the object to receive all mouse events, regardless of whether it is under the mouse.\n"
                                                          "@param setMouseLocked Whether to lock or unlock the mouse."
														"@return No return value.")
{
   object->setMouseLocked(dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getMouseLocked, bool, 2, 2, "() Gets the mouse locked status of the object\n"
                                                          "@return (bool status) Whether the mouse is locked or not.")
{
   return object->getMouseLocked();
}


//-----------------------------------------------------------------------------
// Get Child Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getIsChild, bool, 2, 2, "() Gets the objects Child status.\n"
                                                      "@return (bool isChild) Whether or not the object is a child.")
{
    // Get Child Status.
    return object->getIsChild();
}  


//-----------------------------------------------------------------------------
// Set Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setArea, void, 3, 6, "(float x1, float y1, float x2, float y2) Sets the objects area.\n"
                                                   "@param x1 The upper left corner x position.\n"
                                                   "@param y1 The upper left corner y position.\n"
                                                   "@param x2 The lower right corner x position.\n"
                                                   "@param y2 The lower right corner y position.\n"
												   "@return No return value.")
{
   // Upper left and lower right bound.
   t2dVector v1, v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = t2dSceneObject::getStringElementVector(argv[2]);
       v2 = t2dSceneObject::getStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = t2dSceneObject::getStringElementVector(argv[2]);
      v2 = t2dSceneObject::getStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       v2 = t2dVector(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setArea() - Invalid number of parameters!");
      return;
   }

   // Set Area.
   object->setArea( v1, v2 );
}
// Set Area.
void t2dSceneObject::setArea( const t2dVector& corner1, const t2dVector& corner2 )
{
   // Calculate Normalised Rectangle.
   const t2dVector topLeft((corner1.mX <= corner2.mX) ? corner1.mX : corner2.mX, (corner1.mY <= corner2.mY) ? corner1.mY : corner2.mY);
   const t2dVector bottomRight((corner1.mX > corner2.mX) ? corner1.mX : corner2.mX, (corner1.mY > corner2.mY) ? corner1.mY : corner2.mY);

   // Calculate New Size.
   const t2dVector size = bottomRight - topLeft;
   // Calculate New Position.
   const t2dVector position = topLeft + (size * 0.5f);

   // Reset Rotation.
   setRotation( 0.0f) ;

   // Set Position/Size.
   setPosition( position );
   setSize( size );
}


//-----------------------------------------------------------------------------
// Get Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getArea, const char*, 2, 2, "() Gets the object's area.\n"
                                                          "@return (float x1/float y1/float x2/float y2) A space seperated list of the upper left corner x and y position and the lower right corner x and y positions.")
{
    // Get Clip Rectange.
    RectF clipRectangle = object->getWorldClipRectangle();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f %f %f", clipRectangle.point.x, clipRectangle.point.y, clipRectangle.point.x+clipRectangle.len_x(), clipRectangle.point.y+clipRectangle.len_y());
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Min Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAreaMin, const char*, 2, 2, "() Gets the upper left point of the object.\n"
                                                             "@return (float x1/float y1) The upper left corner x and y position of the object.")
{
    // Get Clip Rectange.
    RectF clipRectangle = object->getWorldClipRectangle();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 256, "%f %f", clipRectangle.point.x, clipRectangle.point.y);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Max Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAreaMax, const char*, 2, 2, "Gets the lower right point of the object.\n"
                                                             "@return (float x2/float y2) The lower right corner x and y position of the object.")
{
    // Get Clip Rectange.
    RectF clipRectangle = object->getWorldClipRectangle();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", clipRectangle.point.x+clipRectangle.len_x(), clipRectangle.point.y+clipRectangle.len_y());
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPosition, void, 3, 4, "(float x, float y) - Sets the objects position.\n"
                                                       "@param x The position of the object along the horizontal axis.\n"
                                                       "@param y The position of the object along the vertical axis.\n"
													  "@return No return value.")
{
   // The new position.
   t2dVector position;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      position = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      position = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setPosition() - Invalid number of parameters!");
      return;
   }

   // Set Position.
   object->setPosition(position);

   // Update Container Configuration.
   object->updateSpatialConfig();
}   
// Set Position.
void t2dSceneObject::setPosition( const t2dVector& position )
{
    // Set Position.
    getParentPhysics().setPosition( position );

    // Reset Tick Positions.
    getParentPhysics().resetTickPosition();
}


//-----------------------------------------------------------------------------
// Get Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getPosition, const char*, 2, 2, "() Gets the object's position.\n"
                                                              "@return (float x/float y) The x and y (horizontal and vertical) position of the object.")
{
    // Get Position.
    t2dVector position = object->getPosition();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", position.mX, position.mY);
    // Return Velocity.
    return pBuffer;
}

ConsoleMethod(t2dSceneObject, getRenderPosition, const char*, 2, 2, "() Gets the current render position.\n"
                                                                    "@return (float x/float y) The x and y (horizontal and vertical) render position of the object.")
{
   // Get Position.
   t2dVector position = object->getRenderPosition();

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);
   // Format Buffer.
   dSprintf(pBuffer, 32, "%f %f", position.mX, position.mY);
   // Return Velocity.
   return pBuffer;
}

//-----------------------------------------------------------------------------
// Set Position X-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPositionX, void, 3, 3, "(float x) Sets the objects x position.\n"
                                                        "@param x The horizontal position of the object."
													  "@return No return value.")
{
    // Set Position X-Component.
    object->setPosition( t2dVector( dAtof(argv[2]), object->getPosition().mY ) );

    object->updateSpatialConfig();
}


//-----------------------------------------------------------------------------
// Set Position Y-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPositionY, void, 3, 3, "(float y) Sets the objects y position.\n"
                                                        "@param y The vertical position of the object."
													  "@return No return value.")
{
    // Set Position Y-Component.
    object->setPosition( t2dVector( object->getPosition().mX, dAtof(argv[2]) ) );
    
    object->updateSpatialConfig();
}


//-----------------------------------------------------------------------------
// Get Position X-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getPositionX, F32, 2, 2, "() Gets the object's x position.\n"
                                                       "@return (float x) The horizontal position of the object")
{
    // Get Position X-Component.
    return object->getPosition().mX;
}


//-----------------------------------------------------------------------------
// Get Position Y-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getPositionY, F32, 2, 2, "() Gets the object's y position.\n"
                                                       "@return (float y) The vertical position of the object")
{
    // Get Position Y-Component.
    return object->getPosition().mY;
}


//-----------------------------------------------------------------------------
// Set Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setRotation, void, 3, 3, "(float rotation) - Sets the objects rotation.\n"
                                                       "0 is the default object rotation.\n"
                                                       "@param rotation The rotation (angle) of the object."
													  "@return No return value.")
{
    // Set Rotation.
    object->setRotation( dAtof(argv[2]) );

    // Update Container Configuration.
    object->updateSpatialConfig();
}   
// Set Rotation.
void t2dSceneObject::setRotation( const F32 rotation )
{
    // Set Rotation.
    getParentPhysics().setRotation( rotation ) ;

    // Reset Rotation Tick Rotations.
    getParentPhysics().resetTickRotation();
}


//-----------------------------------------------------------------------------
// Get Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getRotation, F32, 2, 2, "() Gets the object's rotation.\n"
                                                      "0 is the default object rotation.\n"
                                                      "@return (float rotation) The object's current rotation (angle).")
{
    // Return Rotation.
    return object->getRotation();
}

//-----------------------------------------------------------------------------
// Get Render Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getRenderRotation, F32, 2, 2, "() Gets the object's render rotation.\n"
                                                            "0 is the default object rotation.\n"
                                                            "@return (float rotation) The object's current render rotation (angle).")
{
    // Return Rotation.
    return object->getRenderRotation();
}


//-----------------------------------------------------------------------------
// Set Mount Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setMountRotation, void, 3, 3, "(float mountRotation) Sets the objects Mount rotation.\n"
                                                            "@param mountRotation The mount rotation."
															"@return No return value.")
{
    // Set Mount Rotation.
    object->setMountRotation( dAtof(argv[2]) );

    // Update Container Configuration.
    object->updateSpatialConfig();
}   
// Set Mount Rotation.
void t2dSceneObject::setMountRotation( const F32 mountRotation )
{
    // Set Mount Rotation.
    mMountRotation = mFmod(mountRotation, 360.0f);
}


//-----------------------------------------------------------------------------
// Get Mount Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMountRotation, F32, 2, 2, "() Gets the object's mount rotation.\n"
                                                           "@return (float mountRotation) The object's current mount rotation.")
{
    // Return Mount Rotation.
    return object->getMountRotation();
}


//-----------------------------------------------------------------------------
// Set Flip.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setFlipX, void, 3, 3, "(bool flipX) Sets whether or not the object is flipped horizontally.\n"
                                                    "@param flipX Whether or not to flip the object along the x (horizontal) axis."
													  "@return No return value.")
{
    // Set Flip.
    object->setFlipX( dAtob(argv[2]) );

    // Update Container Configuration.
    object->updateSpatialConfig();
}

ConsoleMethod(t2dSceneObject, setFlipY, void, 3, 3, "(bool flipY) Sets whether or not the object is flipped vertically.\n"
                                                    "@param flipY Whether or not to flip the object along the y (vertical) axis."
													  "@return No return value.")
{
    // Set Flip.
    object->setFlipY( dAtob(argv[2]) );

    // Update Container Configuration.
    object->updateSpatialConfig();
}
ConsoleMethod(t2dSceneObject, getFlipX, bool, 2, 2, "() Gets whether or not the object is flipped horizontally.\n"
                                                    "@return (bool flipX) Whether or not the object is flipped along the x axis.")
{
   return object->getFlipX();
}

ConsoleMethod(t2dSceneObject, getFlipY, bool, 2, 2, "() Gets whether or not the object is flipped vertically."
                                                    "@return (bool flipY) Whether or not the object is flipped along the y axis.")
{
   return object->getFlipY();
}

ConsoleMethod(t2dSceneObject, setFlip, void, 4, 4, "(bool flipX, bool flipY) Sets flipping for each axis.\n"
                                                   "@param flipX Whether or not to flip the object along the x (horizontal) axis.\n"
                                                   "@param flipY Whether or not to flip the object along the y (vertical) axis.\n"
													  "@return No return value.")
{
    // Set Flip.
    object->setFlip( dAtob(argv[2]), dAtob(argv[3]) );

    // Update Container Configuration.
    object->updateSpatialConfig();
}
// Set Flip.
void t2dSceneObject::setFlip( const bool flipX, const bool flipY )
{
   // If nothing's changed, we don't update anything. (JDD)
   if( flipX == mFlipX && flipY == mFlipY )
      return;

   // Set Flip.
   mFlipX = flipX;
   mFlipY = flipY;

   // Generate Collision Polygon.
   getParentPhysics().generateCollisionPoly();

   // Set Spatial Dirty.
   getParentPhysics().setSpatialDirty();

}
void t2dSceneObject::setFlipX( const bool flipX )
{
   setFlip( flipX, mFlipY );
}
void t2dSceneObject::setFlipY( const bool flipY )
{
   setFlip( mFlipX, flipY );
}


//-----------------------------------------------------------------------------
// Get Flip.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getFlip, const char*, 2, 2, "() Gets the flip for each axis.\n"
                                                          "@return (bool flipX/bool flipY) Whether or not the object is flipped along the x and y axis.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getFlipX(), object->getFlipY());
    // Return Buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Sort Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setSortPoint, void, 3, 4, "(float x, float y) Sets the layer draw order sorting point.\n"
                                                        "@param x The x position local to the object of the sort point.\n"
                                                        "@param y The y position local to the object of the sort point.\n"
													  "@return No return value.")
{
   // The new sort point.
   t2dVector sortPt;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      sortPt = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      sortPt = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setSortPoint() - Invalid number of parameters!");
      return;
   }

   // Set Position.
   object->setSortPoint(sortPt);
}   
// Set Sort Point.
void t2dSceneObject::setSortPoint( const t2dVector& pt )
{
    mSortPoint = pt;
}
ConsoleMethod(t2dSceneObject, setSortPointX, void, 3, 3, "(float x) Sets the x-component of the layer draw order sorting point.\n"
                                                         "@param x The x position local to the object of the sort point.\n"
														"@return No return value.")
{
    // Set sort point X-component.
    object->setSortPoint( t2dVector( dAtof(argv[2]), object->getSortPoint().mY ) );
}
ConsoleMethod(t2dSceneObject, setSortPointY, void, 3, 3, "(float y) Sets the y-component of the layer draw order sorting point.\n"
                                                         "@param y The y position local to the object of the sort point.\n"
														 "@return No return value.")
{
    // Set sort point Y-component.
    object->setSortPoint( t2dVector( object->getSortPoint().mX, dAtof(argv[2]) ) );
}


//-----------------------------------------------------------------------------
// Get Sort Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getSortPoint, const char*, 2, 2, "() Gets the layer draw order sorting point.\n"
                                                               "@return (float x/float y) The local x and y position of the sort point.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", object->getSortPoint().mX, object->getSortPoint().mY);
    // Return Buffer.
    return pBuffer;
}
ConsoleMethod(t2dSceneObject, getSortPointX, F32, 2, 2, "() Gets the x component of the layer draw order sorting point.\n"
                                                        "@return (float x) The local x position of the sort point.")
{
    // Get sort point X-component.
    return object->getSortPoint().mX;
}
ConsoleMethod(t2dSceneObject, getSortPointY, F32, 2, 2, "() Gets the y component of the layer draw order sorting point.\n"
                                                        "@return (float y) The local y position of the sort point.")
{
    // Get sort point Y-component.
    return object->getSortPoint().mY;
}


//-----------------------------------------------------------------------------
// Set Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setSize, void, 3, 4, "(float width, float height) Sets the objects size.\n"
                                                   "@param width The width of the object.\n"
                                                   "@param height The height of the object.\n"
													 "@return No return value.")
{
   F32 width, height;

   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("width height")
   if ((elementCount == 2) && (argc == 3))
   {
      width = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      height = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (width, [height])
   else if (elementCount == 1)
   {
      width = dAtof(argv[2]);

      if (argc > 3)
         height = dAtof(argv[3]);
      else
         height = width;
   }

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setSize() - Invalid number of parameters!");
      return;
   }

   // Set Size.
   object->setSize(t2dVector(width, height));

   // Update Container Configuration.
   object->updateSpatialConfig();
}   
// Set Size.
void t2dSceneObject::setSize( const t2dVector& size )
{
    // Set Collision Poly.
    getParentPhysics().setSize( size );
}


//-----------------------------------------------------------------------------
// Get Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getSize, const char*, 2, 2, "() Gets the objects size.\n"
                                                          "@return (float width/float height) The width and height of the object.")
{
    // Get Size.
    t2dVector size = object->getSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", size.mX, size.mY);
    // Return buffer.
    return pBuffer;
} 


//-----------------------------------------------------------------------------
// Set Size X-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setSizeX, void, 3, 3, "(float width) Sets the width of the object.\n"
                                                    "@param width The width of the object."
													 "@return No return value.")
{
    // Set Size X-Component.
    object->setSize( t2dVector( dAtof(argv[2]), object->getSize().mY ) );
}


//-----------------------------------------------------------------------------
// Set Size Y-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setSizeY, void, 3, 3, "(float height) Sets the height of the object.\n"
                                                    "@param height The height of the object."
													  "@return No return value.")
{
    // Set Size Y-Component.
    object->setSize( t2dVector( object->getSize().mX, dAtof(argv[2]) ) );
}


//-----------------------------------------------------------------------------
// Get Size X-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getSizeX, F32, 2, 2, "() Gets the width of the object.\n"
                                                   "@return (float width) The width of the object.")
{
    // Get Size X-Component.
    return object->getSize().mX;
}


//-----------------------------------------------------------------------------
// Get Size Y-Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getSizeY, F32, 2, 2, "() Gets the height of the object.\n"
                                                   "@return (float height) The height of the object.")
{
    // Get Size Y-Component.
    return object->getSize().mY;
}


//-----------------------------------------------------------------------------
// Set Size Width Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setWidth, void, 3, 3, "(float width) Sets the width of the object.\n"
                                                    "@param width The width of the object."
													  "@return No return value.")
{
    // Set Size Width Component.
    object->setSize( t2dVector( dAtof(argv[2]), object->getSize().mY ) );
    object->updateSpatialConfig();
}


//-----------------------------------------------------------------------------
// Set Size Height Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setHeight, void, 3, 3, "(float height) Sets the height of the object."
                                                     "@param height The height of the object."
													  "@return No return value.")
{
    // Set Size Height Component.
    object->setSize( t2dVector( object->getSize().mX, dAtof(argv[2]) ) );
    object->updateSpatialConfig();
}


//-----------------------------------------------------------------------------
// Get Size Width Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getWidth, F32, 2, 2, "() Gets the width of the object.\n"
                                                   "@return (float width) The width of the object.")
{
    // Get Size Width Component.
    return object->getSize().mX;
}


//-----------------------------------------------------------------------------
// Get Size Height Component.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getHeight, F32, 2, 2, "() Gets the height of the object.\n"
                                                    "@return (float height) The height of the object.")
{
    // Get Size Height Component.
    return object->getSize().mY;
}


//-----------------------------------------------------------------------------
// Set Auto Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setAutoRotation, void, 3, 3, "(float autoRotationSpeed) Sets the objects auto rotation speed.\n"
                                                           "@param autoRotationSpeed the speed of the auto rotation."
														 "@return No return value.")
{
    // Set Auto Rotation.
    object->setAutoRotation( dAtof(argv[2]) );
}   
// Set Auto Rotation.
void t2dSceneObject::setAutoRotation( const F32 rotationSpeed )
{
    // Set Auto Rotation.
    getParentPhysics().setAutoRotation( rotationSpeed );
}


//-----------------------------------------------------------------------------
// Get Auto Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAutoRotation, F32, 2, 2, "() Gets the objects auto rotation.\n"
                                                          "@return (float autoRotationSpeed) The object's auto rotation speed.")
{
    // Return Auto Rotation.
    return object->getAutoRotation();
}   


//-----------------------------------------------------------------------------
// Set Auto Mount-Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setAutoMountRotation, void, 3, 3, "(float autoMountRotationSpeed) Sets the objects auto mount rotation speed.\n"
                                                                "@param autoMountRotationSpeed The auto mount rotation speed."
															  "@return No return value.")
{
    // Set Auto Mount-Rotation.
    object->setAutoMountRotation( dAtof(argv[2]) );
}   
// Set Auto Mount-Rotation.
void t2dSceneObject::setAutoMountRotation( const F32 autoMountRotationSpeed )
{
    // Set Auto Mount-Rotation.
    mAutoMountRotation = autoMountRotationSpeed;

    // Usage Flag.
    mAutoMountRotationActive = mNotZero( mAutoMountRotation );
}


//-----------------------------------------------------------------------------
// Get Auto Mount Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAutoMountRotation, F32, 2, 2, "() Gets the objects auto mount rotation.\n"
                                                               "@return float (autoMountRotationSpeed) The auto mount rotation speed.")
{
    // Return Auto-Mount Rotation.
    return object->getAutoMountRotation();
}   


//-----------------------------------------------------------------------------
// Set Forward Movement Only.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setForwardMovementOnly, void, 3, 3, "(bool forwardMovementOnly) Sets the object to only move forward.\n"
                                                                  "@param forwardMovementOnly Whether or not the object should only move forward."
																	"@return No return value.")
{
    // Set Forward Movement Only.
    object->setForwardMovementOnly( dAtob(argv[2]) );
}
// Set Forward Movement Only.
void t2dSceneObject::setForwardMovementOnly( const bool forwardMovementOnly )
{
    // Set Forward Movement Only.
    getParentPhysics().setForwardMovementOnly( forwardMovementOnly );
}


//-----------------------------------------------------------------------------
// Get Forward Movement Only.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getForwardMovementOnly, bool, 2, 2, "() Gets the object forward movement only status.\n"
                                                                  "@return (bool forwardMovementOnly) Whether or not the object should only move forward.")
{
    // Get Forward Movement Only.
    return object->getForwardMovementOnly();
}


//-----------------------------------------------------------------------------
// Set Forward Movement Only.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setForwardSpeed, void, 3, 3, "(float forwardSpeed) - Sets the forward speed of the object.\n"
                                                           "@param forwardSpeed The foward speed of the object."
														 "@return No return value.")
{
    // Set Forward Movement Only.
    object->setForwardSpeed( dAtof(argv[2]) );
}
// Set Forward Movement Only.
void t2dSceneObject::setForwardSpeed( const F32 forwardSpeed )
{
    // Validate Speed.
    if ( mLessThan(forwardSpeed, 0.0f) )
    {
        Con::warnf("t2dSceneObject::setForwardSpeed() - Negative speed is not allowed!");
        return;
    }

    // Ignore if forward-movement is disabled.
    if ( !getForwardMovementOnly() ) return;

    // Set Forward Movement Only.
    setLinearVelocity( t2dVector( 0.0f, forwardSpeed ) );
}


//-----------------------------------------------------------------------------
// Set Position Target.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPositionTarget, void, 3, 8, "(t2dVector positionTarget, [bool autoStop = true], [bool callback = false], [bool snap = true], [float targetMargin = 0.1]) Sets a position target for the object.\n"
                                                             "This function does not actually move the object. Instead, it allows an object to be stopped or execute a callback upon reaching a destination position. To have an object automatically move to a target position, use moveTo().\n"
                                                             "@param positionTarget The x and y position of the position target.\n"
                                                             "@param autoStop Whether or not to autoStop upon reaching the taret.\n"
                                                             "@param callback Whether or not to perform a script callback (onPositionTarget()) upon reaching the designated target.\n"
                                                             "@param snap Whether or not to snap the object to the target when it is within margin.\n"
                                                             "@param targetMargin The proximity to the target necessary to qualify as reaching the target.\n"
															"@return No return value.")
{
   // The target position.
   t2dVector positionTarget;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("targetX targetY")
   if ((elementCount == 1) && (argc > 3))
      positionTarget = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // (targetX, targetY)
   else if ((elementCount == 2) && (argc < 8))
      positionTarget = t2dSceneObject::getStringElementVector(argv[2]);

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setPositionTarget() - Invalid number of parameters!");
      return;
   }

   // The index of the first argument after positionTarget (either 3 or 4).
   U32 firstArg = 5 - elementCount;

   // Grab the autoStop flag - if it's available.
   bool autoStop = true;
   if (argc > firstArg)
      autoStop = dAtob(argv[firstArg]);

   // Grab the callback flag.
   bool callback = false;
   if (argc > (firstArg + 1))
      callback = dAtob(argv[firstArg + 1]);

   // Grab the snap flag.
   bool snap = true;
   if (argc > (firstArg + 2))
      snap = dAtob(argv[firstArg + 2]);

   // Grab the target margin.
   F32 targetMargin = 0.1f;
   if (argc > (firstArg + 3))
      targetMargin = dAtof(argv[firstArg + 3]);

   // Set Position Target.
   object->setPositionTarget(positionTarget, autoStop, callback, snap, targetMargin);
}
// Set Position Target.
void t2dSceneObject::setPositionTarget( const t2dVector positionTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin )
{
    // Set Position Target.
    getParentPhysics().setPositionTarget( positionTarget, autoStop, callback, snap, targetMargin );
}


//-----------------------------------------------------------------------------
// Set Position Target Off.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPositionTargetOff, void, 2, 2, "() Cancels the objects position target."
																"@return No return value.")
{
    // Set Position Target Off.
    object->setPositionTargetOff();
}
// Set Position Target Off.
void t2dSceneObject::setPositionTargetOff( void )
{
    // Set Position Target Off.
    getParentPhysics().setPositionTargetOff();
}


//-----------------------------------------------------------------------------
// Set Rotation Target.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setRotationTarget, void, 3, 7, "(float rotationTarget, [bool autoStop = true], [bool callback = false], [bool snap = true], [float targetMargin = 0.1]) Sets a rotation target for the object.\n"
                                                             "This function does not actually rotate the object. It merely provides a target rotation for the object. To actually rotate an object to a target use rotateTo().\n"
                                                             "@param rotationTarget The rotation target.\n"
                                                             "@param autoStop Whether or not to autoStop upon reaching the taret.\n"
                                                             "@param callback Whether or not to perform a script callback (onRotationTarget()) upon reaching the designated target rotation.\n"
                                                             "@param snap Whether or not to snap the object to the target when it is within margin.\n"
                                                             "@param targetMargin The proximity to the target necessary to qualify as reaching the target.\n"
															"@return No return value.")
{
    // Fetch Rotation Target.
    F32 rotationTarget = dAtof(argv[2]);

    // Fetch AutoStop Flag.
    bool autoStop = true;
    if ( argc > 3 ) autoStop = dAtob(argv[3]);

    // Fetch Callback Flag.
    bool callback = false;
    if ( argc > 4 ) callback = dAtob(argv[4]);

    // Fetch Snap.
    bool snap = true;
    if ( argc > 5 ) snap = dAtob(argv[5]);

    // Fetch Target Margin.
    F32 targetMargin = 0.1f;
    if ( argc > 6 ) targetMargin = dAtof(argv[6]);

    // Set Rotation Target.
    object->setRotationTarget( rotationTarget, autoStop, callback, snap, targetMargin );
}
// Set Rotation Target.
void t2dSceneObject::setRotationTarget( const F32 rotationTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin )
{
    // Set Rotation Target.
    getParentPhysics().setRotationTarget( rotationTarget, autoStop, callback, snap, targetMargin );
}


//-----------------------------------------------------------------------------
// Set Rotation Target Off.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setRotationTargetOff, void, 2, 2, "() Removes the objects rotation target."
													  "@return No return value.")
{
    // Set Rotation Target Off.
    object->setRotationTargetOff();
}
// Set Rotation Target Off.
void t2dSceneObject::setRotationTargetOff( void )
{
    // Set Rotation Target Off.
    getParentPhysics().setRotationTargetOff();
}


//-----------------------------------------------------------------------------
// Move-To Target Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, moveTo, void, 4, 9, "(t2dVector positionTarget, float linearSpeed, [bool autoStop = true], [bool callback = false], [bool snap = true], [float targetMargin = 0.1]) Sets a position target for the object and moves toward it.\n"
                                                  "The object is not actually forced to the target position, it is merely sent there. So, if a collision or some other outside force interferes with the object, it may not make it to the target position. However, the target is still set, so the object may still reach the target in the future.\n"
                                                  "@param positionTarget The x and y position of the position target.\n"
                                                  "@param linearSpeed The speed at which the object is setn to the target.\n"
                                                  "@param autoStop Whether or not to autoStop upon reaching the taret.\n"
                                                  "@param callback Whether or not to perform a script callback (onPositionTarget()) upon reaching the designated target.\n"
                                                  "@param snap Whether or not to snap the object to the target when it is within margin.\n"
                                                  "@param targetMargin The proximity to the target necessary to qualify as reaching the target.\n"
												  "@return No return value.")              
{
   // The target position and linear speed.
   t2dVector positionTarget;
   F32 linearSpeed;

   // Calculate Element Count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("targetX targetY", speed, ...)
   if ((elementCount == 1) && (argc > 4))
   {
      positionTarget = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
      linearSpeed = dAtof(argv[4]);
   }

   // (targetX, targetY, speed, ...)
   else if ((elementCount == 2) && (argc < 9))
   {
      positionTarget = t2dSceneObject::getStringElementVector(argv[2]);
      linearSpeed = dAtof(argv[3]);
   }

   // Invalid
   else
   {
       Con::warnf("t2dSceneObject::moveTo() - Invalid number of parameters!");
       return;
   }

   // The index of the first argument after positionTarget (either 4 or 5).
   U32 firstArg = 6 - elementCount;

   // Grab the autoStop flag - if it's available.
   bool autoStop = true;
   if (argc > firstArg)
      autoStop = dAtob(argv[firstArg]);

   // Grab the callback flag.
   bool callback = false;
   if (argc > (firstArg + 1))
      callback = dAtob(argv[firstArg + 1]);

   // Grab the snap flag.
   bool snap = true;
   if (argc > (firstArg + 2))
      snap = dAtob(argv[firstArg + 2]);

   // Grab the target margin.
   F32 targetMargin = 0.1f;
   if (argc > (firstArg + 3))
      targetMargin = dAtof(argv[firstArg + 3]);

   // Move-To Target Position.
   object->moveTo(positionTarget, linearSpeed, autoStop, callback, snap, targetMargin);
}
// Move-To Target Position.
void t2dSceneObject::moveTo( const t2dVector positionTarget, const F32 linearSpeed, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin )
{
	if(!mUsesPhysics)
	{
		Con::printf("Object %s is trying to use moveTo without usesPhysics, nothing will happen here.", getName());
		return;
	}

    // Setup Target.
    setPositionTarget( positionTarget, autoStop, callback, snap, targetMargin );

    // Reset Linear Velocity.
    getParentPhysics().resetLinearVelocity();
    // Calculate New Target Velocity.
    t2dVector targetVelocity = positionTarget-getPosition();
    // Renormalise to selected speed.
    targetVelocity.normalise( linearSpeed );
    // Set new linear velocity.
    setLinearVelocity( targetVelocity );
}


//-----------------------------------------------------------------------------
// Rotate-To Rotation Target.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, rotateTo, void, 4, 8, "(float rotationTarget, float angularSpeed, [bool autoStop = true], [bool callback = false], [bool snap = true], [float targetMargin = 0.1]) Sets a rotation target for the object and rotates toward it.\n"
                                                    "The object is not forced to the target rotation. It just has an angular velocity set that sends it toward the target. If it is interrupted by a collision or some other means, the target will be set, but the object may not reach it.\n"
                                                    "@param rotationTarget The rotation target.\n"
                                                    "@param angularSpeed The speed at which you want the object to rotate to the target.\n"
                                                    "@param autoStop Whether or not to autoStop upon reaching the taret.\n"
                                                    "@param callback Whether or not to perform a script callback (onRotationTarget()) upon reaching the designated target rotation.\n"
                                                    "@param snap Whether or not to snap the object to the target when it is within margin.\n"
                                                    "@param targetMargin The proximity to the target necessary to qualify as reaching the target.\n"
													  "@return No return value.")
{
    // Fetch Rotation Target.
    const F32 rotationTarget = dAtof(argv[2]);

    // Fetch Angular Speed.
    const F32 angularSpeed = mFabs(dAtof(argv[3]));

    // Fetch AutoStop Flag.
    bool autoStop = true;
    if ( argc >= 5 ) autoStop = dAtob(argv[4]);

    // Fetch Callback Flag.
    bool callback = false;
    if ( argc >= 6 ) callback = dAtob(argv[5]);

    // Fetch Snap.
    bool snap = true;
    if ( argc >= 7 ) snap = dAtob(argv[6]);

    // Fetch Target Margin.
    F32 targetMargin = 0.1f;
    if ( argc >= 8 ) targetMargin = dAtof(argv[7]);

    // Rotate-To Rotation Target.
    object->rotateTo( rotationTarget, angularSpeed, autoStop, callback, snap, targetMargin );
}
// Rotate-To Rotation Target.
void t2dSceneObject::rotateTo( const F32 rotationTarget, const F32 angularSpeed, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin )
{
    // Check Angular Speed.
    if ( angularSpeed <= 0.0f )
    {
        // Warn.
        Con::warnf("t2dSceneObject::rotateTo() - 'angularSpeed' must be greater than zero!" );
    }

    // Set Rotation Target.
    setRotationTarget( rotationTarget, autoStop, callback, snap, targetMargin );

    // Reset Auto-Rotation.
    setAutoRotation( 0.0f );
    // Reset Angular Velocity.
    getParentPhysics().resetAngularVelocity();

    // Calculate Source/Destination Rotations.
    F32 srcRotation = mFmod(getRotation(), 360.0f);
    F32 dstRotation = mFmod(rotationTarget, 360.0f);
    // Ensure Rotations are positive.
    if ( srcRotation < 0.0f ) srcRotation += 360.0f;
    if ( dstRotation < 0.0f ) dstRotation += 360.0f;

    // Finish if we're already at the destination.
    if ( srcRotation == dstRotation )
        return;

    // Calculate current 'side' of destination rotation.
    F32 rotationSign;
    if ( srcRotation < dstRotation )
    {
        // Calculate shortest Angular Rotation.
        rotationSign = ( (dstRotation-srcRotation) < (360.0f-dstRotation+srcRotation) ) ? 1.0f : -1.0f;
    }
    else
    {
        // Calculate shortest Angular Rotation.
        rotationSign = ( (srcRotation-dstRotation) < (360.0f-srcRotation+dstRotation) ) ? -1.0f : 1.0f;
    }

    // Set Appropriate Angular Velocity.
    setAngularVelocity( rotationSign * mFabs(angularSpeed) );
}


//-----------------------------------------------------------------------------
// Set Collision Active.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionActive, void, 4, 4, "(bool send, bool receive) - Sets the collision check directions.\n"
                                                              "Object's can both collide with other objects and be collided with by other objects. Setting an object to send collisions makes it able to collide, and setting an object to receive collisions makes it able to be collided with. So, an object that is sending collisions will only be able to collide with objects that are receiving collisions and vice versa.\n"
                                                              "A single object can be set to both send and receive collisions.\n"
                                                              "@param send Whether or not the object can collide with other objects.\n"
                                                              "@param receive Whether or not the object can be collided with by other objects.\n"
																"@return No return value.")
{
    // Set Collision Active.
    object->setCollisionActive( dAtob(argv[2]), dAtob(argv[3]) );
}
// Set Collision Active.
void t2dSceneObject::setCollisionActive( bool send, bool receive )
{
    // Check we're allowed to send collisions!
    if ( send && !mCollisionSendAllowed )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setCollisionActive() - Send collision not allowed for this object! (%d).", getId() );
        // Turn Send Collision Off.
        send = false;
    }

    // Check we're allowed to receive collisions!
    if ( receive && !mCollisionReceiveAllowed )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setCollisionActive() - Receive collision not allowed for this object! (%d).", getId() );
        // Turn Send Collision Off.
        receive = false;
    }

    // Set Send/Receive Collision Active.
    mCollisionActiveSend = send;
    mCollisionActiveReceive = receive;
}

void t2dSceneObject::setCollisionActiveSend( bool send )
{
    // Check we're allowed to send collisions!
    if ( send && !mCollisionSendAllowed )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setCollisionActive() - Send collision not allowed for this object! (%d).", getId() );
        // Turn Send Collision Off.
        send = false;
    }

    mCollisionActiveSend = send;
}

void t2dSceneObject::setCollisionActiveReceive( bool receive )
{
    // Check we're allowed to send collisions!
    if ( receive && !mCollisionReceiveAllowed )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setCollisionActive() - Receive collision not allowed for this object! (%d).", getId() );
        // Turn Send Collision Off.
        receive = false;
    }

    mCollisionActiveReceive = receive;
}

ConsoleMethod(t2dSceneObject, setCollisionActiveSend, void, 3, 3, "(bool send) - Sets the collision active send status.\n"
                                                                  "@param send Whether or not the object can collide with other objects.\n"
													  "@return No return value.")
{
   object->setCollisionActiveSend(dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getCollisionActiveSend, bool, 2, 2, "Gets the collision active send status.\n"
                                                                  "@return (bool send) The send collision active status.")
{
   return object->getCollisionActiveSend();
}

ConsoleMethod(t2dSceneObject, setCollisionActiveReceive, void, 3, 3, "(bool receive) - Sets the collision active receive status.\n"
                                                                     "@param receive Whether or not the object can be collided with by other objects."
													  "@return No return value.")
{
   object->setCollisionActiveReceive(dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getCollisionActiveReceive, bool, 2, 2, "Gets the collision active receive status.\n"
                                                                     "@return (bool recieve) The receive collision active status.")
{
   return object->getCollisionActiveReceive();
}

ConsoleMethod(t2dSceneObject, setCollisionPhysicsSend, void, 3, 3, "(bool send) - Gets the collision physics send status.\n"
                                                                   "@param send Whether or not to use physics in sent collisions.\n"
													  "@return No return value.")
{
   object->setCollisionPhysics(dAtob(argv[2]), object->getCollisionPhysicsReceive());
}

ConsoleMethod(t2dSceneObject, getCollisionPhysicsSend, bool, 2, 2, "() Gets the collision physics send status.\n"
                                                                   "@return (bool send) The send physics status.")
{
   return object->getCollisionPhysicsSend();
}

ConsoleMethod(t2dSceneObject, setCollisionPhysicsReceive, void, 3, 3, "(bool receive) - Sets the collision physics receive status.\n"
                                                                      "@param receive Whether or not to use physics in received collisions."
													  "@return No return value.")
{
   object->setCollisionPhysics(object->getCollisionPhysicsSend(), dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getCollisionPhysicsReceive, bool, 2, 2, "() Gets the collision physics receive status.\n"
                                                                      "@return (bool recieve) The receive physics status.")
{
   return object->getCollisionPhysicsReceive();
}

//-----------------------------------------------------------------------------
// Get Collision Physics.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionActive, const char*, 2, 2, "() Gets the collision active directions.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getCollisionActiveSend()?1:0, (U32)object->getCollisionActiveReceive()?1:0);
    // Return Buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Collision Physics.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionPhysics, void, 4, 4, "(bool send, bool receive) - Sets the collision physics directions.\n"
                                                               "@param send Whether or not to use physics in sent collisions.\n"
                                                               "@param receive Whether or not to use physics in received collisions."
													  "@return No return value.")
{
    // Set Collision Physics.
    object->setCollisionPhysics( dAtob(argv[2]), dAtob(argv[3]) );
}
// Set Collision Physics.
void t2dSceneObject::setCollisionPhysics( bool send, bool receive )
{
    // Set Send/Receive Collision Physics.
    mCollisionPhysicsSend = send;
    mCollisionPhysicsReceive = receive;
}
// Set Collision Physics.
void t2dSceneObject::setCollisionPhysicsSend( bool send )
{
    // Set Send/Receive Collision Physics.
    mCollisionPhysicsSend = send;
}
// Set Collision Physics.
void t2dSceneObject::setCollisionPhysicsReceive( bool receive )
{
    // Set Send/Receive Collision Physics.
    mCollisionPhysicsReceive = receive;
}


//-----------------------------------------------------------------------------
// Get Collision Physics.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionPhysics, const char*, 2, 2, "() Gets the collision physics directions.\n"
                                                                      "@return (bool send/bool recieve) The send and receive physics status.\n")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getCollisionPhysicsSend()?1:0, (U32)object->getCollisionPhysicsReceive()?1:0);
    // Return Buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Collision Masks.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionMasks, void, 3, 4, "(groupMask, [layerMask]) - Sets the collision masks.\n"
                                                             "Collision masks limit the objects that are collided with based on their Graph Group and Rendering Layer. In order for two objects to collide, the sending object must have the receiving object's Graph Group and Rendering Layer in its masks. The receiving object's masks, however, do not have to contain the sending object's Graph Group and Rendering Layer.\n"
                                                             "@param groupMask The Graph Groups to enable collision with.\n"
                                                             "@param layerMask The Rendering Layers to enable collision with. If layerMask is not specified all Rendering Layers will be included."
													  "@return No return value.")
{
    // Calculate Group-Mask.
    const U32 groupMask = dAtoi(argv[2]);
    // Calculate Layer-Mask.
    const U32 layerMask = (argc > 3) ? dAtoi(argv[3]) : T2D_MASK_ALL;

    // Set Collision Masks.
    object->setCollisionMasks( groupMask, layerMask );
}
// Set Collision Masks.
void t2dSceneObject::setCollisionMasks( const U32 groupMask, const U32 layerMask )
{
    // Set Group/Layer Collision Masks.
    mCollisionGroupMask = groupMask;
    mCollisionLayerMask = layerMask;
}


//-----------------------------------------------------------------------------
// Get Collision Masks.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionMasks, const char*, 2, 2, "Gets the collision group/layer masks.\n"
                                                                    "@return (groupMask/layerMask) The Graph Groups and Rendering Layers the object is enabled to collide with.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getCollisionGroupMask(), (U32)object->getCollisionLayerMask());
    // Return Buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Collision Against.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionAgainst, void, 3, 4, "(t2dSceneObject object, [clearMasks? = false]) - Sets the collision masks against a specific object.\n"
                                                               "@param object The t2dSceneobject to set collision masks against\n"
                                                               "@param clearMasks Whether or not to clear the collision masks before setting them against the object."
													  "@return No return value.")
{
    // Find t2dSceneObject Object.
    const t2dSceneObject* pSceneObject = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject )
    {
        Con::warnf("t2dSceneObject::setCollisionAgainst() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Fetch ClearMasks Option.
    bool clearMasks = argc >= 4 ? dAtob(argv[3]) : false;

    // Set Collision Against.
    object->setCollisionAgainst( pSceneObject, clearMasks );
}
// Set Collision Against.
void t2dSceneObject::setCollisionAgainst( const t2dSceneObject* pSceneObject, const bool clearMasks )
{
    // Do we need to clear existing masks?
    if ( clearMasks )
    {
        // Yes, so just set the masks to the referenced-objects' masks.
        setCollisionMasks( pSceneObject->getGraphGroupMask(), pSceneObject->getLayerMask() );
    }
    else
    {
        // No, so merge with existing masks.
        setCollisionMasks( getCollisionGroupMask() | pSceneObject->getGraphGroupMask(), getCollisionLayerMask() | pSceneObject->getLayerMask() );
    }
}


//-----------------------------------------------------------------------------
// Set Collision Groups.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionGroups, void, 3, 2 + T2D_MASK_BITCOUNT, "(groups$) - Sets the collision group(s).\n"
                                                                                  "@param groups A list of collision group numbers to collide with.\n"
																				"@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      object->setCollisionGroups(0);
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneObject::setCollisionGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneObject::setCollisionGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setCollisionGroups(mask);
}
// Set Collision Groups.
void t2dSceneObject::setCollisionGroups( const U32 groupMask )
{
   // Set Collision Masks.
   setCollisionMasks(groupMask, getCollisionLayerMask());
}


//-----------------------------------------------------------------------------
// Set Collision Layers.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionLayers, void, 3, 2 + T2D_MASK_BITCOUNT, "(layers$) - Sets the collision layers(s).\n"
                                                                                  "@param layers A list of layers numbers to collide with.\n"
													  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      object->setCollisionLayers(0);
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneObject::setCollisionLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneObject::setCollisionLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setCollisionLayers(mask);
}
// Set Collision Layers.
void t2dSceneObject::setCollisionLayers( const U32 layerMask )
{
   // Set Collision Masks.
   setCollisionMasks(getCollisionGroupMask(), layerMask);
}


ConsoleMethod(t2dSceneObject, getCollisionLayers, const char*, 2, 2, "() - Gets the collision layers.\n"
                                                                     "@return (collisionLayers) A list of collision layers.")
{
   U32 mask = object->getCollisionLayerMask();

   bool first = true;
   char* bits = Con::getReturnBuffer(128);
   bits[0] = '\0';
   for (S32 i = 0; i < T2D_MASK_BITCOUNT; i++)
   {
      if (mask & BIT(i))
      {
         char bit[4];
         dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
         first = false;
         dStrcat(bits, bit);
      }
   }

   return bits;
}

ConsoleMethod(t2dSceneObject, getCollisionGroups, const char*, 2, 2, "() - Gets the collision groups.\n"
                                                                     "@return (collisionGroups) A list of collision groups.")
{
   U32 mask = object->getCollisionGroupMask();

   bool first = true;
   char* bits = Con::getReturnBuffer(128);
   bits[0] = '\0';
   for (S32 i = 0; i < T2D_MASK_BITCOUNT; i++)
   {
      if (mask & BIT(i))
      {
         char bit[4];
         dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
         first = false;
         dStrcat(bits, bit);
      }
   }

   return bits;
}

//-----------------------------------------------------------------------------
// Set Collision Circle Superscribed Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionCircleSuperscribed, void, 3, 3, "(superscribeMode?) - Sets the Collision-Circle superscribed mode.\n"
                                                                          "@param superscribeMode Whether to enable or disable the superscribed mode.\n"
													  "@return No return value.")
{
    // Set Collision Circle Superscribed Mode.
    object->setCollisionCircleSuperscribed( dAtob(argv[2]) );
}
// Set Collision Circle Superscribed Mode.
void t2dSceneObject::setCollisionCircleSuperscribed( const bool superscribeMode )
{
    // Set Collision Circle Superscribed Mode.
    getParentPhysics().setCollisionCircleSuperscribed( superscribeMode );
}

ConsoleMethod(t2dSceneObject, getCollisionCircleSuperscribed, bool, 2, 2, "() Gets the current setting for the superscript mode."
			  "@return Returns true if the mode if active, false if not.")
{
   return object->getCollisionCircleSuperscribed();
}


//-----------------------------------------------------------------------------
// Set Collision Detection Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionDetection, void, 3, 3, "(detectionMode) - Sets the Collision-Detection mode.\n"
                                                                 "@param detectionMode <br>Possible Values<br>"
                                                                 " -FULL: A superscribed circle is used first to filter objects that are obviously not colliding. If the bounding circles of objects are colliding, a further polygon check is done to see if a collision actually occurred.<br>"
                                                                 " -CIRCLE: Only the collision circle is used in determining collisions.<br>"
                                                                 " -POLYGON: Only the collision polygon is used in determining collisions.<br>"
                                                                 " -CUSTOM: A custom function is called to detect whether or not collision has occurred between two objects. This function is defined by the user in the engine.\n"
																 "@return No return Value.")
{
    // Fetch Collision Detection Mode.
    t2dPhysics::eCollisionDetection detectionMode = getCollisionDetectionEnum(argv[2]);

    // Set Collision Detection Mode.
    object->setCollisionDetection( detectionMode );
}
// Set Collision Detection Mode.
void t2dSceneObject::setCollisionDetection( const t2dPhysics::eCollisionDetection detectionMode )
{
    // Set Collision Detection Mode.
    getParentPhysics().setCollisionDetectionMode( detectionMode );
}



//-----------------------------------------------------------------------------
// Get Collision Detection Description.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionDetection, const char*, 2, 2, "() Get the Collision-Detection mode.\n"
                                                                        "@return (string detectionMode) The current collision detection mode, see setCollisionDetection() for the modes.")
{
    // Return Collision Detection Description.
    return getCollisionDetectionDescription(object->getCollisionDetectionMode());
}


//-----------------------------------------------------------------------------
// Set Collision Response Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionResponse, void, 3, 3, "(responseMode) - Sets the Collision-Response mode.\n"
                                                                "The collision response is only used if collision physics are active for the collision.\n"
                                                                "@param responseMode <br>Possible values<br>"
                                                                " -RIGID: The object uses rigid body physics.<br>"
                                                                " -BOUNCE: The object bounces off the object it collided with.<br>"
                                                                " -CLAMP: The object is kept from penetrating the object it collided with.<br>"
                                                                " -STICKY: The object sticks to the collision point.<br>"
                                                                " -KILL: The object is deleted.<br>"
                                                                " -CUSTOM: A custom function is called to resolve collisions between two objects. This function is defined by the user in the engine.\n"
																 "@return No return Value.")
{
    // Fetch Collision Response.
    t2dPhysics::eCollisionResponse responseMode = getCollisionResponseEnum(argv[2]);

    // Set Collision Response Mode.
    object->setCollisionResponse( responseMode );
}
// Set Collision Response Mode.
void t2dSceneObject::setCollisionResponse( const t2dPhysics::eCollisionResponse responseMode )
{
    // Set Collision Response Mode.
    getParentPhysics().setCollisionResponseMode( responseMode );
}


//-----------------------------------------------------------------------------
// Get Collision Response Description.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionResponse, const char*, 2, 2, "() Get the Collision-Response mode.\n"
                                                                       "@return (responseMode) The current collision response mode, see setCollisionResponse for the modes.")
{
    // Return Collision Response Description.
    return getCollisionResponseDescription(object->getCollisionResponseMode());
}


//-----------------------------------------------------------------------------
// Set Collision Maximum Iterations.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionMaxIterations, void, 3, 3, "(iterationCount) - Sets the Objects Maximum Collision Iterations.\n"
                                                                     "Sets the maximum number of iterations that will be taken through the collision detection and resolution systems in a single frame.\n"
                                                                     "@param iterationCount the max number of iterations.\n"
																 "@return No return Value.")
{
    // Set Collision Maximum Iterations.
    object->getParentPhysics().setCollisionMaxIterations( dAtoi(argv[2]) );
}


//-----------------------------------------------------------------------------
// Get Collision MaximumIterations.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionMaxIterations, S32, 2, 2, "() Gets the Objects Maximum Collision Iterations.\n"
                                                                    "Gets the maximum number of iterations that will be taken through the collision detection and resolution systems in a single frame.\n"
                                                                    "@return (integer iterationCount) the max number of iterations.")
{
    // Get Collision MaximumIterations.
    return S32(object->getParentPhysics().getCollisionMaxIterations());
}


//-----------------------------------------------------------------------------
// Set Collision Callback.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionCallback, void, 3, 3, "(bool status?) - Sets the collision callback status.\n"
                                                                "Sets whether or not the onCollision() callback will be called for this object when it is part of a collision.\n"
                                                                "@param status Whether or not to call a collision callback.\n"
																 "@return No return Value.")
{
    // Set Collision Callback.
    object->setCollisionCallback( dAtob(argv[2]) );
}
// Set Collision Callback.
void t2dSceneObject::setCollisionCallback( const bool status )
{
    // Collision Callback.
    mCollisionCallback = status;
}


//-----------------------------------------------------------------------------
// Get Collision Callback.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionCallback, bool, 2, 2, "() Gets the collision callback status.\n"
                                                                "@return (bool status) Whether or not the collision callback will be called.")
{
    // Get Collision Callback.
    return object->getCollisionCallback();
}


//-----------------------------------------------------------------------------
// Set Collision Suppress.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionSuppress, void, 3, 3, "(bool status?) - Sets the collision suppress status.\n"
                                                                "@param status Whether to suppress the collision or not.\n"
																 "@return No return Value.")
{
    // Set Collision Suppress.
    object->setCollisionSuppress( dAtob(argv[2]) );
}
// Set Collision Suppress.
void t2dSceneObject::setCollisionSuppress( const bool status )
{
    // Collision Callback.
    mCollisionSuppress = status;
}


//-----------------------------------------------------------------------------
// Get Collision Suppress.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionSuppress, bool, 2, 2, "() Gets the collision suppress status.\n"
                                                                "@return (bool status) Whether to supress the collision or not.")
{
    // Get Collision Callback.
    return object->getCollisionSuppress();
}


//-----------------------------------------------------------------------------
// Set Collision Polygon Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionPolyScale, void, 3, 4, "(float widthScale / [float heightScale]) - Sets the collision-polygon scale.\n"
                                                                 "This value is not cumulative. So, if a scale factor of 0.5 is applied twice, the second one will have no affect on the collision polygon.\n"
                                                                 "@param widthScale The scale factor for the collision polygon width.\n"
                                                                 "@param heightScale The scale factor for the collision polygon height.\n"
																 "@return No return Value.")
{
   // The scale.
   t2dVector scale;

   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("width height")
   if ((elementCount == 2) && (argc == 3))
      scale = t2dSceneObject::getStringElementVector(argv[2]);

   // (width, [height])
   else if (elementCount == 1)
   {
      scale.mX = dAtof(argv[2]);

      if (argc == 4)
         scale.mY = dAtof(argv[3]);
      else
         scale.mY = scale.mX;
   }

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setCollisionPolyScale() - Invalid number of parameters!");
      return;
   }

   // Set Collision Polygon Scale.
   object->setCollisionPolyScale(scale);
}   
// Set Collision Polygon Scale.
void t2dSceneObject::setCollisionPolyScale( const t2dVector& scale )
{
    // Check Scales.
    if ( scale.mX <= 0.0f || scale.mY <= 0.0f )
    {
        Con::warnf("t2dSceneObject::setCollisionPolyScale() - Collision Scales must be greater than zero! '%f,%f'.", scale.mX, scale.mY);
        return;
    }
    // Check Scales.
    if ( scale.mX > 1.0f || scale.mY > 1.0f )
    {
        Con::warnf("t2dSceneObject::setCollisionPolyScale() - Collision Scales cannot be greater than one! '%f,%f'.", scale.mX, scale.mY);
        return;
    }

    // Set Collision Polygon Scale.
    getParentPhysics().setCollisionPolyScale( scale ); 
}


//-----------------------------------------------------------------------------
// Get Collision Polygon Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionPolyScale, const char*, 2, 2, "() Gets the collision-polygon Scale.\n"
                                                                        "@return (float widthScale/float heightScale) The width and height poly scale values.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", object->getCollisionPolyScale().mX, object->getCollisionPolyScale().mY);
    // Return Buffer.
    return pBuffer;
}



//-----------------------------------------------------------------------------
// Set Collision Circle Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionCircleScale, void, 3, 3, "(float scale) - Sets the collision-circle scale.\n"
                                                                   "This value is not cumulative. So, if a scale factor of 0.5 is applied twice, the second one will have no affect on the collision circle.\n"
                                                                   "@param scale Float in the range (0.0, 1.0]\n"
																 "@return No return Value.")
{
    // Set Collision Circle Scale.
    object->setCollisionCircleScale( dAtof(argv[2]) );
}   
// Set Collision Circle Scale.
void t2dSceneObject::setCollisionCircleScale( const F32 scale )
{
    // Check Scales.
    if ( mLessThanOrEqual( scale, 0.0f ) )
    {
        Con::warnf("t2dSceneObject::setCollisionCircleScale() - Collision scale must be greater than zero! '%f'.", scale);
        return;
    }

    // Set Collision Circle Scale.
    getParentPhysics().setCollisionCircleScale( scale ); 
}


//-----------------------------------------------------------------------------
// Get Collision Circle Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionCircleScale, F32, 2, 2, "Gets the collision-circle Scale.\n"
                                                                  "@return (float scale) The circle scale factor.")
{
    // Return Circle Scale.
    return object->getCollisionCircleScale();
}



//-----------------------------------------------------------------------------
// Set Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLayer, void, 3, 3, "(integer layer) Sets the objects layer (0-31).\n"
                                                    "The layer on which to place the object.\n"
                                                    "@param layer Integer in the range [0-31].\n"
													"@return No return Value.")
{
    // Set Layer.
    object->setLayer( dAtoi(argv[2]) );
}   
// Set Layer.
void t2dSceneObject::setLayer( const U32 layer )
{
    // Check Layer.
    if ( layer > (t2dSceneGraph::maxLayersSupported-1) )
    {
        Con::warnf("t2dSceneObject::setLayer() - Invalid Layer '%d' (0-31).", layer);
        return;
    }

    // Set Layer.
    mLayer = layer;
    // Set Layer Mask.
    mLayerMask = BIT( mLayer );
}


//-----------------------------------------------------------------------------
// Get Layer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLayer, S32, 2, 2, "() Gets the Objects Layer.\n"
                                                   "@return (integer layer) The layer the object is on.")
{
    // Return Layer.
    return object->getLayer();
}


//-----------------------------------------------------------------------------
// Set Group.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setGraphGroup, void, 3, 3, "(integer group) - Sets the objects group (0-31).\n"
                                                         "The graph group to place the object in.\n"
                                                         "@param group Integer in the range [0-31].\n"
														 "@return No return Value.")
{
    // Set Group.
    object->setGraphGroup( dAtoi(argv[2]) );
}   
// Set Group.
void t2dSceneObject::setGraphGroup( const U32 graphGroup )
{
    // Check Group.
    if ( graphGroup > 31 )
    {
        Con::warnf("t2dSceneObject::setGroup() - Invalid Group '%d' (0-31).", graphGroup);
        return;
    }

    // Set Group.
    mGraphGroup = graphGroup;
    // Set Group Mask.
    mGraphGroupMask = BIT( mGraphGroup );
}


//-----------------------------------------------------------------------------
// Get Group.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getGraphGroup, S32, 2, 2, "() Gets the Objects Group.\n"
                                                        "@return (integer group) The group the object is on.")
{
    // Return Group.
    return object->getGraphGroup();
}


//-----------------------------------------------------------------------------
// Set Collision Poly Primitive.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionPolyPrimitive, void, 3, 3, "(integer poly-count) - Sets Primitive Collision Polygon.\n"
                                                                     "While 1 or 2 edges doesn't make a polygon, it is perfectly valid to use these numbers for %edgeCount in order to specify a single point or a line for collision detection."
                                                                     "The polygon is subscribed within the bounding box of the object."
                                                                     "The higher the number of edges in the collision polygon, the longer collision detection will take. The polygon should contain the fewest edges possible while still accurately representing the desired collision area.\n"
                                                                     "@param poly-count Integer in the range [1, 64].\n"
																	 "@return No return Value.")
{
    // Set Collision Poly Primitive.
    object->setCollisionPolyPrimitive( dAtoi(argv[2]) );
}
// Set Collision Poly Primitive.
void t2dSceneObject::setCollisionPolyPrimitive( const U32 polyVertexCount )
{
    // Validate Polygon.
    if ( polyVertexCount < 1 )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setCollisionPolyPrimitive() - Invalid Polygon Edge Count '%d' (>1).", polyVertexCount);
        return;
    }
    // Check for Maximum Polygon Edges.
    else if ( polyVertexCount > t2dPhysics::MAX_COLLISION_POLY_VERTEX )
    {
        Con::warnf("t2dSceneObject::setCollisionPolyPrimitive() - Cannot generate a %d edged collision polygon.  Maximum is %d!", polyVertexCount, t2dPhysics::MAX_COLLISION_POLY_VERTEX);
        return;
    }

    // Set Collision Poly Primitive.
    getParentPhysics().setCollisionPolyPrimitive( polyVertexCount );
}


//-----------------------------------------------------------------------------
// Set Collision Poly Custom.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionPolyCustom, void, 4, 3 + t2dPhysics::MAX_COLLISION_POLY_VERTEX, "(integer poly-count, poly-Definition$) - Sets Custom Collision Polygon.\n"
                                                                                                          "The collision polygon must be convex. While 1 or 2 edges doesn't make a polygon, it is perfectly valid to use these numbers for %edgeCount in order to specify a single point or a line for collision detection."
                                                                                                          "The higher the number of edges in the collision polygon, the longer collision detection will take. The polygon should contain the fewest edges possible while still accurately representing the desired collision area.\n"
                                                                                                          "@param poly-count Integer in the range [1, 64]. The number of edges in the polygon.\n"
                                                                                                          "@param poly-Definition Each of these parameters is a vertex of the collision polygon in object space. \n"
																											"@return No return Value.")
{
   // Grab the poly count.
   U32 vertexCount = dAtoi(argv[2]);
   // Vertex storage.
   t2dVector poly[t2dPhysics::MAX_COLLISION_POLY_VERTEX];

   // Validate Polygon.
   if (vertexCount > t2dPhysics::MAX_COLLISION_POLY_VERTEX)
   {
      Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Cannot generate a %d edged collision polygon.  Maximum is %d!", vertexCount, t2dPhysics::MAX_COLLISION_POLY_VERTEX);
      return;
   }

   if (vertexCount < 1)
   {
      // Warn.
      Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Polygon Edge Count '%d' (>0).", vertexCount);
      return;
   }

   U32 elementCount = t2dSceneObject::getStringElementCount(argv[3]);

   // Comma separated list.
   if (elementCount == 1)
   {
      U32 actualCount = vertexCount * 2;
      if (argc != (actualCount + 3))
      {
         // Warn.
         Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Custom Polygon Items '%d'; expected '%d'!", argc - 3, actualCount);
         return;
      }

      // Grab the poly's
      for (U32 i = 0, elementIndex = 3; i < vertexCount; i++, elementIndex += 2)
      {
         poly[i] = t2dVector(dAtof(argv[elementIndex]), dAtof(argv[elementIndex + 1]));
      }
   }

   // Comma separated list of space separated points.
   else if (elementCount == 2)
   {
      if (argc != (vertexCount + 3))
      {
         // Warn.
         Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Custom Polygon '%d'; expected '%d'!", argc - 3, vertexCount );
         return;
      }

      for (U32 i = 0; i < vertexCount; i++)
      {
         if (t2dSceneObject::getStringElementCount(argv[i + 3]) != 2)
         {
            Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid vertex found at vertex '%d'", i);
            return;
         }
         poly[i] = t2dSceneObject::getStringElementVector(argv[i + 3]);
      }
   }

   // Space separated list.
   else if (elementCount >= 2)
   {
      // Validate Polygon Custom Length.
      if (elementCount != (vertexCount * 2))
      {
         // Warn.
         Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Custom Polygon Items '%d'; expected '%d'!", elementCount, vertexCount * 2 );
         return;
      }

      for (U32 i = 0; i < vertexCount; i++)
      {
         poly[i] = t2dSceneObject::getStringElementVector(argv[3], i * 2);
      }
   }

   else
   {
      // Warn.
      Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Polygon Edge Count '%d' (>0).", elementCount);
      return;
   }

    // Set Collision Poly Custom.
    object->setCollisionPolyCustom( vertexCount, poly );
}
// Set Collision Poly Custom.
void t2dSceneObject::setCollisionPolyCustom( const U32 polyVertexCount, const t2dVector* pCustomPolygon )
{
    // Validate Polygon Vertices.
    for ( U32 n = 0; n < polyVertexCount; n+=2 )
    {
        // Fetch Coordinate.
        const t2dVector& coord = pCustomPolygon[n];
        // Check Range.
        if ( coord.mX < -1.0f || coord.mX > 1.0f || coord.mY < -1.0f || coord.mY > 1.0f )
        {
            // Warn.
            Con::warnf("t2dSceneObject::setCollisionPolyCustom() - Invalid Polygon Coordinate range; Must be -1 to +1! '(%f,%f)'", coord.mX, coord.mY );
            return;
        }
    }

    // Set Collision Poly Custom.
    getParentPhysics().setCollisionPolyCustom( polyVertexCount, pCustomPolygon );
}


//-----------------------------------------------------------------------------
// Get Collision Poly Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionPolyCount, S32, 2, 2, "Gets Collision Poly Edge Count.\n"
                                                                "@return (integer poly-count) The edge count of the collision polygon.")
{
    // Get Collision Poly Count.
    return object->getCollisionPolyCount();
}


//-----------------------------------------------------------------------------
// Get Collision Poly.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getCollisionPoly, const char*, 2, 2, "Gets Collision Poly.\n"
                                                                   "@return (poly-Definition) The vertices of the collision polygon in object space.")
{
   // Get Collision Poly Count.
   return object->getCollisionPoly();
}
// Get Collision Poly.
const char* t2dSceneObject::getCollisionPoly( void )
{
    // Get Collision Polygon.
    const t2dVector* pCollisionPoly = getParentPhysics().getCollisionPolyBasis();

    // Set Max Buffer Size.
    const U32 maxBufferSize = getCollisionPolyCount() * 18 + 1;

    // Get Return Buffer.
    char* pReturnBuffer = Con::getReturnBuffer( maxBufferSize );

    // Check Buffer.
    if( !pReturnBuffer )
    {
        // Warn.
        Con::printf("t2dSceneObject::getCollisionPoly() - Unable to allocate buffer!");
        // Exit.
        return NULL;
    }

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Polygon Edges.
    for ( U32 n = 0; n < getCollisionPolyCount(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pReturnBuffer + bufferCount, maxBufferSize-bufferCount, "%0.5f %0.5f ", pCollisionPoly[n].mX, pCollisionPoly[n].mY );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneObject::getCollisionPoly() - Error writing to buffer!");
            break;
        }
    }

    // Return Buffer.
    return pReturnBuffer;
}


//-----------------------------------------------------------------------------
// Set Collision Material.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setCollisionMaterial, void, 3, 3, "(collisionMaterial) - Sets Collision Material.\n"
                                                                "@param collisionMateral\n"
																 "@return No return Value.")
{
    // Set Collision Material.
    object->setCollisionMaterial( argv[2] );
}
// Set Collision Material.
void t2dSceneObject::setCollisionMaterial( const char* pCollisionMaterialName )
{
    // Check Collision Name.
    if ( dStrlen( pCollisionMaterialName ) == 0 )
        return;

    // Find Animation Datablock.
    t2dCollisionMaterialDatablock* pCollisionMaterialDataBlock = dynamic_cast<t2dCollisionMaterialDatablock*>(Sim::findObject( pCollisionMaterialName ));

    // Check Datablock (if we've got one).
    if ( !t2dCheckDatablock( pCollisionMaterialDataBlock ) )
    {
        Con::warnf("t2dSceneObject::setCollisionMaterial() - Invalid t2dCollisionMaterialDatablock datablock (%s)", pCollisionMaterialName);
        return;
    }

    // Set Collision Material Properties.
    setDynamicFriction( pCollisionMaterialDataBlock->mCoefDynamicFriction );
    setRestitution( pCollisionMaterialDataBlock->mCoefRestitution );
    setAutoMassInertia( pCollisionMaterialDataBlock->mAutoMassInertia );
    setMass( pCollisionMaterialDataBlock->mMass );
    setInertialMoment( pCollisionMaterialDataBlock->mInertialMoment );
    setDensity( pCollisionMaterialDataBlock->mDensity );
    setForceScale( pCollisionMaterialDataBlock->mForceScale );
    setDamping( pCollisionMaterialDataBlock->mDamping );
    setImmovable( pCollisionMaterialDataBlock->mImmovable );
}


//-----------------------------------------------------------------------------
// Set World Limit.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setWorldLimit, void, 3, 8, "(limitMode, [float minX / float minY / float maxX / float maxY], [bool callback?]) - Set World Limit.\n"
                                                         "@param limitMode <br>Possible Values:<br>"
                                                         " -OFF: Turns the limit off.<br>"
                                                         " -NULL: Nothing is done, but the callback is still called.<br>"
                                                         " -RIGID: The objects responds with a rigid collision response.<br>"
                                                         " -BOUNCE: The object bounces off the limit bounds.<br>"
                                                         " -CLAMP: The object is clamped to the edge of the limit bounds.<br>"
                                                         " -STICKY: The object sticks to the spot it hit the limit bounds.<br>"
                                                         " -KILL: The object is deleted when it collides with any limit bound.<br>"
                                                         "@param minX The top left x position."
                                                         "@param minY The top left y position."
                                                         "@param maxX The bottom right x position."
                                                         "@param maxY The bottom right y position."
                                                         "@param callback Whether or not to trigger the onWorldLimit() callback.\n"
																 "@return No return Value.")
{
   // limitMode. Always specified.
   t2dSceneObject::eWorldLimit worldLimit = getWorldLimitEnum(argv[2]);
   // The bounds.
   t2dVector min, max;
   // Index of the optional callback parameter in argv.
   U32 callbackIndex;

   // Early out if the world limit is being turned off.
   if ((worldLimit == t2dSceneObject::T2D_LIMIT_OFF) || (worldLimit == t2dSceneObject::T2D_LIMIT_INVALID))
   {
      object->setWorldLimit(worldLimit, t2dVector::getZero(), t2dVector::getZero(), false);
      return;
   }

   // If the limit is not being turned off, the bounds of the limit must be specified.
   else if (argc == 3)
   {
      Con::warnf("t2dSceneObject::setWorldLimit() - Must specify bounds with this limit mode!");
      return;
   }

   // Number of elements for the 2nd and 3rd parameters.
   U32 elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);
   U32 elementCount3 = 1;
   if (argc > 4)
      elementCount3 = t2dSceneObject::getStringElementCount(argv[4]);

   // ("minX minY maxX maxY", [callback])
   if ((elementCount2 == 4) && (argc < 6))
   {
      min = t2dSceneObject::getStringElementVector(argv[3]);
      max = t2dSceneObject::getStringElementVector(argv[3], 2);
      callbackIndex = 4;
   }

   // ("minX minY", "maxX maxY", [callback])
   else if ((elementCount2 == 2) && (elementCount3 == 2) && (argc < 7))
   {
      min = t2dSceneObject::getStringElementVector(argv[3]);
      max = t2dSceneObject::getStringElementVector(argv[4]);
      callbackIndex = 5;
   }

   // (minX, minY, maxX, maxY, [callback])
   else if (argc > 6)
   {
      min = t2dVector(dAtof(argv[3]), dAtof(argv[4]));
      max = t2dVector(dAtof(argv[5]), dAtof(argv[6]));
      callbackIndex = 7;
   }

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setWorldLimit() - Invalid number of parameters!");
      return;
   }

   // Grab callback if it's specified.
   bool callback = false;
   if (argc > callbackIndex)
      callback = dAtob(argv[callbackIndex]);

   // Set the World Limit.
   object->setWorldLimit(worldLimit, min, max, callback);
}
// Set World Limit.
void t2dSceneObject::setWorldLimit( const eWorldLimit limitMode, const t2dVector& limitMin, const t2dVector& limitMax, const bool limitCallback )
{
    // Check for Error.
    if ( limitMode == T2D_LIMIT_INVALID )
    {
        Con::warnf("t2dSceneObject::setWorldLimit - Invalid World Limit!");
        return;
    }

    // Check that world-limit is not too large!
    if ( limitMode != T2D_LIMIT_OFF && ((mFabs(limitMax.mX - limitMin.mX)) > maxWorldLimitSize || (mFabs(limitMax.mY - limitMin.mY)) > maxWorldLimitSize) )
    {
        // Too large!
        Con::warnf("t2dSceneObject::setWorldLimit - World Limit Area is too large!  Maximum width/height is %f!", maxWorldLimitSize);
        return;
    }

    // Set World Limit.
    mWorldLimitMode = limitMode;

    // Set World Limit Min/Max.
    mWorldLimitMin = limitMin;
    mWorldLimitMax = limitMax;

    // Set World Limit Callback.
    mWorldLimitCallback = limitCallback;

    // Fetch World Limit Offset.
    // NOTE:-   We assume the world-limit physics are all the same
    //          size and square and so now we need to calculate the center
    //          position that would coincide with the edges of the
    //          physics polygon aligning with the specified
    //          world-limit 'area' planes.
    const F32 limitOffset = mWorldLimitPhysics[0].getSize().mX * 0.5f;

    // Calculate Center Position for world-limit area.
    const t2dVector worldLimitCenter( (limitMin.mX + limitMax.mX) * 0.5f, (limitMin.mY + limitMax.mY) * 0.5f );

    // Calculate World Limit Physics Positions.
    // NOTE:-   0 = Left
    //          1 = Right
    //          2 = Top
    //          3 = Bottom
    //
    mWorldLimitPhysics[0].setPosition( t2dVector(limitMin.mX - limitOffset, worldLimitCenter.mY) );
    mWorldLimitPhysics[1].setPosition( t2dVector(limitMax.mX + limitOffset, worldLimitCenter.mY) );
    mWorldLimitPhysics[2].setPosition( t2dVector(worldLimitCenter.mX, limitMin.mY - limitOffset) );
    mWorldLimitPhysics[3].setPosition( t2dVector(worldLimitCenter.mX, limitMax.mY + limitOffset) );
}

void t2dSceneObject::setWorldLimitMode( const eWorldLimit mode )
{
   mWorldLimitMode = mode;
}

void t2dSceneObject::setWorldLimitCallback( const bool callback )
{
   mWorldLimitCallback = callback;
}

void t2dSceneObject::setWorldLimitMin( const t2dVector& limitMin)
{
    // Set World Limit Min/Max.
    mWorldLimitMin = limitMin;
    t2dVector limitMax = mWorldLimitMax;

    // Fetch World Limit Offset.
    // NOTE:-   We assume the world-limit physics are all the same
    //          size and square and so now we need to calculate the center
    //          position that would coincide with the edges of the
    //          physics polygon aligning with the specified
    //          world-limit 'area' planes.
    const F32 limitOffset = mWorldLimitPhysics[0].getSize().mX * 0.5f;

    // Calculate Center Position for world-limit area.
    const t2dVector worldLimitCenter( (limitMin.mX + limitMax.mX) * 0.5f, (limitMin.mY + limitMax.mY) * 0.5f );

    // Calculate World Limit Physics Positions.
    // NOTE:-   0 = Left
    //          1 = Right
    //          2 = Top
    //          3 = Bottom
    //
    mWorldLimitPhysics[0].setPosition( t2dVector(limitMin.mX - limitOffset, worldLimitCenter.mY) );
    mWorldLimitPhysics[1].setPosition( t2dVector(limitMax.mX + limitOffset, worldLimitCenter.mY) );
    mWorldLimitPhysics[2].setPosition( t2dVector(worldLimitCenter.mX, limitMin.mY - limitOffset) );
    mWorldLimitPhysics[3].setPosition( t2dVector(worldLimitCenter.mX, limitMax.mY + limitOffset) );
}

void t2dSceneObject::setWorldLimitMax( const t2dVector& limitMax)
{
    // Set World Limit Min/Max.
    mWorldLimitMax = limitMax;
    t2dVector limitMin = mWorldLimitMin;

    // Fetch World Limit Offset.
    // NOTE:-   We assume the world-limit physics are all the same
    //          size and square and so now we need to calculate the center
    //          position that would coincide with the edges of the
    //          physics polygon aligning with the specified
    //          world-limit 'area' planes.
    const F32 limitOffset = mWorldLimitPhysics[0].getSize().mX * 0.5f;

    // Calculate Center Position for world-limit area.
    const t2dVector worldLimitCenter( (limitMin.mX + limitMax.mX) * 0.5f, (limitMin.mY + limitMax.mY) * 0.5f );

    // Calculate World Limit Physics Positions.
    // NOTE:-   0 = Left
    //          1 = Right
    //          2 = Top
    //          3 = Bottom
    //
    mWorldLimitPhysics[0].setPosition( t2dVector(limitMin.mX - limitOffset, worldLimitCenter.mY) );
    mWorldLimitPhysics[1].setPosition( t2dVector(limitMax.mX + limitOffset, worldLimitCenter.mY) );
    mWorldLimitPhysics[2].setPosition( t2dVector(worldLimitCenter.mX, limitMin.mY - limitOffset) );
    mWorldLimitPhysics[3].setPosition( t2dVector(worldLimitCenter.mX, limitMax.mY + limitOffset) );
}


//-----------------------------------------------------------------------------
// Get World Limit.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getWorldLimit, const char*, 2, 2, "() Get the World Limit.\n"
                                                                "@return (limitMode / float minX / float minY / float maxX / float maxY / bool callback) The return is a space seperated string containing the mode, the top left corner x and y position, the bottom right x and y position, and then whether or not the callback is enabled.")
{
    // Get World Limit.
    return object->getWorldLimit();
}
// Get World Limit.
const char* t2dSceneObject::getWorldLimit(void)
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(256);
    // Format Buffer.
    dSprintf(pBuffer, 256, "%s %f %f %f %f %s", getWorldLimitDescription(mWorldLimitMode),
                                                mWorldLimitMin.mX, mWorldLimitMin.mY,
                                                mWorldLimitMax.mX, mWorldLimitMax.mY,
                                                mWorldLimitCallback ? "true" : "false" );
    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------
// Set Lifetime.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLifetime, void, 3, 3, "(float lifetime) - Set Objects' Lifetime.\n"
                                                       "@param lifetime The amount of time, in seconds, before the object is automatically deleted.\n"
																 "@return No return Value.")
{
    // Set Lifetime.
    object->setLifetime( dAtof(argv[2]) );
}
// Set Lifetime.
void t2dSceneObject::setLifetime( const F32 lifetime )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_setLifetime);
#endif

    // Usage Flag.
    mLifetimeActive = mGreaterThanZero( lifetime );

    // Is life active?
    if ( mLifetimeActive )
    {
        // Yes, so set to incoming lifetime.
        mLifetime = lifetime;
    }
    else
    {
        // No, so reset it to be safe.
        mLifetime = 0.0f;
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_setLifetime
#endif
}


//-----------------------------------------------------------------------------
// Get Lifetime.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLifetime, F32, 2, 2, "() Gets the Objects Lifetime.\n"
                                                      "@return (float lifetime) The objects lifetime left before it is automatically deleted.")
{
    // Return Lifetime.
    return object->getLifetime();
}

//-----------------------------------------------------------------------------
// Set Impulse Force.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setImpulseForce, void, 3, 5, "(float impulseForceX / float impulseForceY, [bool gravitic? = false])\n"
              "Applies a one time force to the object.\n"
              "@param impulseForceX The x component of the impulse force\n"
              "@param impulseForceY The y component of the impulse force\n"
              "@param gravitic Whether or not the force should be gravitic.\n"
			"@return No return Value.")
{
   // The force.
   t2dVector force;
   // Gravitic flag.
   bool gravitic = false;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("forceX forceY", [gravitic])
   if ((elementCount == 2) && (argc < 5))
   {
      force = t2dSceneObject::getStringElementVector(argv[2]);

      if (argc > 3)
         gravitic = dAtob(argv[3]);
   }

   // (forceX, forceY, [gravitic])
   else if ((elementCount == 1) && (argc > 3))
   {
      force = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

      if (argc > 4)
         gravitic = dAtob(argv[4]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setImpulseForce() - Invalid number of parameters!");
      return;
   }

   // Set Impulse Force.
   object->setImpulseForce(force, gravitic);
}
// Set Impulse Force.
void t2dSceneObject::setImpulseForce( const t2dVector& impulseForce, const bool gravitic )
{
    // Add Gross Linear Force to Physics (act on a gravitic force appropriately).
    getParentPhysics().addGrossLinearForce( gravitic?impulseForce*getParentPhysics().getMass():impulseForce );
}


//-----------------------------------------------------------------------------
// Set Impulse Force Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setImpulseForcePolar, void, 4, 5, "(float angle, float force, [bool gravitic? = false]) - Apply an instantaneous polar force.\n"
                                                                "@param angle The angle of direction you want to apply the force to.\n"
                                                                "@param force The force value you want to apply\n"
                                                                "@param gravitic Whether or not the force should be gravitic.\n"
																 "@return No return Value.")
{
    // Renormalise Angle.
    F32 angle = mFmod(dAtof(argv[2]), 360.0f);
    // Fetch Force.
    F32 force = dAtof(argv[3]);

    // Fetch Gravitic Flag.
    bool gravitic = false;
    if ( argc >= 5 )
        gravitic = dAtob(argv[4]);

    // Set Impulse Force.
    object->setImpulseForce( t2dVector( mSin(mDegToRad(angle))*force, -mCos(mDegToRad(angle))*force ), gravitic );
}

ConsoleMethod(t2dSceneObject, setConstantForceX, void, 3, 3, "(float forceX)\n"
                                                             "@param forceX The x component of the constant force.\n"
																 "@return No return Value.")
{
   t2dVector constantForce = object->getConstantForce();
   object->setConstantForce(t2dVector(dAtof(argv[2]), constantForce.mY), object->getGraviticConstantForce());
}

ConsoleMethod(t2dSceneObject, setConstantForceY, void, 3, 3, "(float forceY)\n"
                                                             "@param forceY The y component of the constant force.\n"
																 "@return No return Value.")
{
   t2dVector constantForce = object->getConstantForce();
   object->setConstantForce(t2dVector(constantForce.mX, dAtof(argv[2])), object->getGraviticConstantForce());
}

ConsoleMethod(t2dSceneObject, setGraviticConstantForce, void, 3, 3, "(bool gravitic)\n"
                                                                    "@param gravitic Whether to set the constant force as gravitic.\n"
																 "@return No return Value.")
{
   t2dVector constantForce = object->getConstantForce();
   object->setConstantForce(t2dVector(constantForce.mX, constantForce.mY), dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getConstantForceX, F32, 2, 2, "()\n"
                                                            "@return (float forceX) The x component of the constant force.")
{
   return object->getConstantForce().mX;
}

ConsoleMethod(t2dSceneObject, getConstantForceY, F32, 2, 2, "()\n"
                                                            "@return (float forceY) The y component of the constant force.")
{
   return object->getConstantForce().mY;
}

ConsoleMethod(t2dSceneObject, getGraviticConstantForce, bool, 2, 2, "()\n"
                                                                    "@return (bool gravitic) Whether the constant force is gravitic or not.")
{
   return object->getGraviticConstantForce();
}

//-----------------------------------------------------------------------------
// Set Constant Force.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setConstantForce, void, 3, 5, "(float forceX / float forceY, [gravitic? = false]) - Apply a continuous force.\n"
                                                            "Sets a constant force to be applied to the object.\n"
                                                            "@param forceX The x component of the constant force.\n"
                                                            "@param forceY The y component of the constant force.\n"
                                                            "@param gravitic Whether to set the constant force as gravitic.\n"
																 "@return No return Value.")
{
   // The force.
   t2dVector force;
   // Gravitic flag.
   bool gravitic = false;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("forceX forceY", [gravitic])
   if ((elementCount == 2) && (argc < 5))
   {
      force = t2dSceneObject::getStringElementVector(argv[2]);

      if (argc > 3)
         gravitic = dAtob(argv[3]);
   }

   // (forceX, forceY, [gravitic])
   else if ((elementCount == 1) && (argc > 3))
   {
      force = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

      if (argc > 4)
         gravitic = dAtob(argv[4]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setConstantForce() - Invalid number of parameters!");
      return;
   }

   // Set Constant Force.
   object->setConstantForce(force, gravitic);
}
// Set Constant Force.
void t2dSceneObject::setConstantForce( const t2dVector& force, const bool gravitic )
{
    // Add Constant-Force to Physics.
    getParentPhysics().setConstantForce( force, gravitic );
}
// Set Constant Force.
void t2dSceneObject::setConstantForce( const t2dVector& force )
{
    // Add Constant-Force to Physics.
    getParentPhysics().setConstantForce( force, getGraviticConstantForce() );
}
// Set Constant Force.
void t2dSceneObject::setConstantForceGravitic( const bool gravitic )
{
    // Add Constant-Force to Physics.
    getParentPhysics().setConstantForce( getConstantForce(), gravitic );
}


//-----------------------------------------------------------------------------
// Set Constant Force Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setConstantForcePolar, void, 4, 5, "(float angle, float force, [bool gravitic? = false]) - Apply a continuous polar force.\n"
                                                                 "Sets a constant force to be applied to the object in polar form.\n"
                                                                 "@param angle The direction of the force.\n"
                                                                 "@param force The magnitude of the force.\n"
                                                                 "@param gravitic Whether to set the constant force as gravitic.\n"
																 "@return No return Value.")
{
    // Renormalise Angle.
    F32 angle = mFmod(dAtof(argv[2]), 360.0f);
    // Fetch Force.
    F32 force = dAtof(argv[3]);

    // Fetch Gravitic Flag.
    bool gravitic = false;
    if ( argc >= 5 )
        gravitic = dAtob(argv[4]);

    // Set Constant Force Polar.
    object->setConstantForce( t2dVector( mSin(mDegToRad(angle))*force, -mCos(mDegToRad(angle))*force ), gravitic );
}


//-----------------------------------------------------------------------------
// Stop Constant Force Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, stopConstantForce, void, 2, 2, "() - Stops any continuous force.\n"
																 "@return No return Value.")
{
    // Reset Constant Force.
    object->setConstantForce( t2dVector::getZero(), false );
}


//-----------------------------------------------------------------------------
// Get Constant Force.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getConstantForce, const char*, 2, 2, "() - Gets the objects constant-force.\n"
                                                                   "@return (float forceX / float forceY) The x and y components of the constant force.")
{
    // Get Constant Force.
    t2dVector constantForce = object->getConstantForce();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", constantForce.mX, constantForce.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Force Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setForceScale, void, 3, 3, "(float scale) - Sets the scale factor for all forces applied to this object.\n"
                                                         "@param scale The scale factor.\n"
																 "@return No return Value.")
{
    // Set Force Scale.
    object->setForceScale( dAtof(argv[2]) );
}
// Set Force Scale.
void t2dSceneObject::setForceScale( const F32 forceScale )
{
    // Set Force Scale for Physics.
    getParentPhysics().setForceScale( forceScale );
}


//-----------------------------------------------------------------------------
// Get Force Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getForceScale, F32, 2, 2, "() - Gets the objects force-scale.\n"
                                                        "@return (float scale) The scale factor.")
{
    // Get Force Scale.
    return object->getForceScale();
}


//-----------------------------------------------------------------------------
// Set at Rest.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setAtRest, void, 2, 2, "() - Set Object at Rest.\n"
                                                     "Both linear velocities and angular velocities are set to 0 so the object will be at rest right after the function is called. However, if any forces are applied to the object after it is set at rest, it will move again.\n\n"
																 "@return No return Value.")
{
    // Set at Rest.
    object->setAtRest();
}
// Set at Rest.
void t2dSceneObject::setAtRest( void )
{
    // Set at Rest.
    getParentPhysics().setAtRest();
}


//-----------------------------------------------------------------------------
// Get at Rest.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAtRest, bool, 2, 2, "() - Gets whether the Object is at Rest.\n"
                                                     "@return (bool atRest) Whether or not the object is at rest.")
{
    // Get at Rest.
    return object->getAtRest();
}


//-----------------------------------------------------------------------------
// Set Dynamic Friction.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setFriction, void, 3, 3, "(float friction) - Set Object Friction.\n"
                                                       "This is kinetic friction, which is the force that resists the movement of the object along the surface of another object. Larger numbers will slow down the object more.\n"
                                                       "@param friction The kinetic friction.\n"
																 "@return No return Value.")
{
    // Set Dynamic Friction.
    object->setDynamicFriction( dAtof(argv[2]) );
}
// Set Dynamic Friction.
void t2dSceneObject::setDynamicFriction( const F32 dynamicfriction )
{
    // Check for negative dynamic friction.
    if ( dynamicfriction < 0.0f )
    {
        Con::warnf("t2dSceneObject::setDynamicFriction() - Cannot have negative friction! (%f)", dynamicfriction);
        return;
    }

    // Set Dynamic Friction.
    getParentPhysics().setDynamicFriction( dynamicfriction );
}


//-----------------------------------------------------------------------------
// Set Restitution.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setRestitution, void, 3, 3, "(float restitution) - Set Object Restitution.\n"
                                                          "A restitution of 1.0 will cause the object to bounce with perfect elasticity, while a value 0.0 will cause the object to not bounce at all.\n"
                                                          "@param restitution Float in the range [0.0, 1.0], the restitution.\n"
																 "@return No return Value.")
{
    // Set Restitution.
    object->setRestitution( dAtof(argv[2]) );
}
// Set Restitution.
void t2dSceneObject::setRestitution( const F32 restitution )
{
    // Check for invalid restitution.
    if ( restitution < 0.0f || restitution > 1.0f )
    {
        Con::warnf("t2dSceneObject::setRestitution() - Restitution must be in the range (0->1)! (%f)", restitution);
        return;
    }

    // Set Restitution.
    getParentPhysics().setRestitution( restitution );
}


//-----------------------------------------------------------------------------
// Set Density.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setDensity, void, 3, 3, "(float density) - Set Object Density.\n"
                                                      "Density is only used when auto mass inertia is active. It is used in calculating the mass and inertia of the object. The higher the density, the larger the mass and inertia. Very low values, in the vicinity of 0.01, are recommended.\n"
                                                      "@param density The density of the object.\n"
																 "@return No return Value.")
{
    // Set Density.
    object->setDensity( dAtof(argv[2]) );
}
// Set Density.
void t2dSceneObject::setDensity( const F32 density )
{
    // Check for negative density.
    if ( density < 0.0f )
    {
        Con::warnf("t2dSceneObject::setDensity() - Cannot have negative density! (%f)", density);
        return;
    }

    // Set Density.
    getParentPhysics().setDensity( density );
}


//-----------------------------------------------------------------------------
// Set Auto Mass/Inertia.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setAutoMassInertia, void, 3, 3, "(bool status?) - Sets Auto Mass/Inertia Calculations.\n"
                                                              "Sets whether or not mass and inertia should be automatically calculated based on the size and density of the object.\n"
                                                              "@param status Whether or not to auto calculate the mass and inertia.\n"
																 "@return No return Value.")
{
    // Set Auto Mass/Inertia.
    object->setAutoMassInertia( dAtob(argv[2]) );
}
// Set Auto Mass/Inertia.
void t2dSceneObject::setAutoMassInertia( const bool status )
{
    // Set Auto Mass/Inertia.
    getParentPhysics().setAutoMassInertia( status );
}


//-----------------------------------------------------------------------------
// Set Mass.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setMass, void, 3, 3, "(float mass) - Set Object Mass.\n"
                                                   "The mass is used in determining the effect of linear forces applied to the object.\n"
                                                   "@param mass The mass of the object.\n"
																 "@return No return Value.")
{
    // Set Mass.
    object->setMass( dAtof(argv[2]) );
}
// Set Mass.
void t2dSceneObject::setMass( const F32 mass )
{
    // Set Mass.
    getParentPhysics().setMass( mass );
}


//-----------------------------------------------------------------------------
// Set Inertial Moment.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setInertialMoment, void, 3, 3, "(float inertia) - Set Object Inertial Moment.\n"
                                                             "The inertia is used in determining the effect of angular forces applied to the object.\n"
                                                             "@param inertia The inertial moment.\n"
																 "@return No return Value.")
{
    // Set Inertial Moment.
    object->setInertialMoment( dAtof(argv[2]) );
}
// Set Inertial Moment.
void t2dSceneObject::setInertialMoment( const F32 inertialMoment )
{
    // Set Inertial Moment.
    getParentPhysics().setInertialMoment( inertialMoment );
}


//-----------------------------------------------------------------------------
// Set Damping.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setDamping, void, 3, 3, "(float damping) - Set Objects Damping.\n"
                                                      "Damping is a percentage value of the amount of linear and angular velocity the object loses per second. A damping value of 1.0 will cause the object to be completely still after 1 second.\n"
                                                      "@param damping The damping value for the object.\n"
																 "@return No return Value.")
{
    // Set Damping.
    object->setDamping( dAtof(argv[2]) );
}
// Set Damping.
void t2dSceneObject::setDamping( const F32 damping )
{
    // Validate Damping.
    if ( damping < 0.0f )
    {
        Con::warnf("t2dSceneObject::setDamping - Invalid Damping! (%f)", damping);
        return;
    }

    // Set Damping.
    getParentPhysics().setDamping( damping );
}


//-----------------------------------------------------------------------------
// Set Immovable.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setImmovable, void, 2, 3, "(bool status) - Set the Object Immovable.\n"
                                                        "Completely stops the object and makes it immovable. An immovable object will not react to any forces or velocities applied to it.\n"
                                                        "The object can still be moved or rotated directly with setPosition() and setRotation().\n"
                                                        "@param status Whether or not the object should be immovable.\n"
																 "@return No return Value.")
{
    // Set Immovable.
    object->setImmovable( (argc == 2) ? true : dAtob(argv[2]) );
}
// Set Immovable.
void t2dSceneObject::setImmovable( const bool immovable )
{
    // Set Immovable.
    getParentPhysics().setImmovable( immovable );
}

ConsoleMethod(t2dSceneObject, getImmovable, bool, 2, 2, "() - Gets the immovable status.\n"
                                                        "@return (bool status) Whether or not the object is immovable.")
{
   return object->getImmovable();
}

//-----------------------------------------------------------------------------
// Get Dynamic Friction.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getFriction, F32, 2, 2, "() - Gets Object Friction.\n"
                                                      "@return (float friction) The friction value of the object.")
{
    // Get Dynamic Friction.
    return object->getDynamicFriction();
}


//-----------------------------------------------------------------------------
// Get Restitution.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getRestitution, F32, 2, 2, "() - Gets Object Restitution.\n"
                                                         "@return (float restitution) The restitution value of the object.")
{
    // Get Restitution.
    return object->getRestitution();
}


//-----------------------------------------------------------------------------
// Get Density.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getDensity, F32, 2, 2, "() - Gets Object Density.\n"
                                                     "@return (float density) The density value of the object.")
{
    // Get Density.
    return object->getDensity();
}


//-----------------------------------------------------------------------------
// Get Auto Mass/Inertia Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAutoMassInertia, bool, 2, 2, "() - Gets Object Auto Mass/Inertia Mode.\n"
                                                              "@return (bool status) Whether or not to auto calculate the mass and inertia.")
{
    // Get Auto Mass/Inertia Mode.
    return object->getAutoMassInertia();
}


//-----------------------------------------------------------------------------
// Get Mass.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMass, F32, 2, 2, "() - Gets Object Mass.\n"
                                                  "@return (float mass) The mass value of the object.")
{
    // Get Mass.
    return object->getMass();
}


//-----------------------------------------------------------------------------
// Get Inertial Moment.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getInertialMoment, F32, 2, 2, "() - Gets Object Inertial Moment.\n"
                                                            "@return (float intertia) The intertia value of the object.")
{
    // Get Inertial Moment.
    return object->getInertialMoment();
}



//-----------------------------------------------------------------------------
// Get Damping.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getDamping, F32, 2, 2, "() - Gets Object Damping.\n"
                                                     "@return (float damping) The damping value of the object.")
{
    // Get Damping.
    return object->getDamping();
}


//-----------------------------------------------------------------------------
// Set Linear Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLinearVelocity, void, 3, 4, "(float velocityX, float velocityY) - Sets Objects Linear Velocity.\n"
                                                             "Sets the velocity at which the object will move.\n"
                                                             "@param velocityX The x component of the velocity.\n"
                                                             "@param velocityY The y component of the velocity.\n\n"
																 "@return No return Value.")
{
   // The velocity.
   t2dVector velocity;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // (x, y)
   if ((elementCount == 1) && (argc > 3))
      velocity = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // ("x y")
   else if ((elementCount == 2) && (argc < 4))
      velocity = t2dSceneObject::getStringElementVector(argv[2]);

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setLinearVelocity - Invalid number of parameters!");
      return;
   }

   // Set Linear Velocity.
   object->setLinearVelocity(velocity);
}
// Set Linear Velocity.
void t2dSceneObject::setLinearVelocity( const t2dVector& linearVelocity )
{
    // Set Gross Linear Velocity.
    getParentPhysics().setGrossLinearVelocity( linearVelocity );
}


//-----------------------------------------------------------------------------
// Set Linear Velocity Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLinearVelocityPolar, void, 4, 4, "(float angle, float speed) - Sets Objects Linear Velocity using Polar-speed.\n"
                                                                  "Sets the speed and direction at which the object will move.\n"
                                                                  "This has the same effect as setLinearVelocity(), it just sets the velocity in a different way.\n"
                                                                  "@param angle The angle of the direction at which the object to move.\n"
                                                                  "@param speed The speed at which the object will move.\n"
																 "@return No return Value.")
{
    // Renormalise Angle.
    F32 angle = mDegToRad(mFmod(dAtof(argv[2]), 360.0f));
    // Fetch Speed.
    F32 speed = dAtof(argv[3]);

    // Calculate Angle.
    F32 sin, cos;
    mSinCos( angle, sin, cos );

    // Set Gross Linear Velocity.
    object->setLinearVelocity( t2dVector( sin*speed, -cos*speed ) );
}


//-----------------------------------------------------------------------------
// Get Linear Velocity Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLinearVelocityPolar, const char*, 2, 2, "() - Gets Objects Linear Velocity using Polar angle/speed.\n"
                                                                         "@return (float angle, float speed) The angle and speed the object is moving at.")
{
    // Get Linear Velocity.
    t2dVector linearVelocity = object->getLinearVelocity();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", mRadToDeg(mAtan(linearVelocity.mX, -linearVelocity.mY)), linearVelocity.len() );
    // Return Velocity.
    return pBuffer;

}


//-----------------------------------------------------------------------------
// Get Linear Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLinearVelocity, const char*, 2, 2, "() - Gets Object Linear Velocity.\n"
                                                                    "@return (float velocityX, float velocityY) The x and y velocities of the object.")
{
    // Get Linear Velocity.
    t2dVector linearVelocity = object->getLinearVelocity();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", linearVelocity.mX, linearVelocity.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Linear Velocity X.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLinearVelocityX, F32, 2, 2, "() - Gets Object Linear Velocity X.\n"
                                                             "@return (float velocityX) The object's velocity along the x axis.")
{
    // Get Linear Velocity X.
    return object->getLinearVelocity().mX;
}


//-----------------------------------------------------------------------------
// Get Linear Velocity Y.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLinearVelocityY, F32, 2, 2, "() - Gets Object Linear Velocity Y.\n"
                                                             "@return (float velocityY) The object's velocity along the y axis.")
{
    // Get Linear Velocity Y.
    return object->getLinearVelocity().mY;
}


//-----------------------------------------------------------------------------
// Set Linear Velocity X-Component
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLinearVelocityX, void, 3, 3, "(float velocityX) - Sets Objects Linear Velocity X-Component.\n"
                                                              "@param velocityX The x component of the velocity.\n"
																 "@return No return Value.")
{
    // Set Linear Velocity X-Component.
    object->setLinearVelocity( t2dVector( dAtof(argv[2]), object->getLinearVelocity().mY ) );
}


//-----------------------------------------------------------------------------
// Set Linear Velocity Y-Component
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLinearVelocityY, void, 3, 3, "(float velocityY) - Sets Objects Linear Velocity Y-Component.\n"
                                                              "@param velocityY The y component of the velocity.\n"
																 "@return No return Value.")
{
    // Set Linear Velocity Y-Component.
    object->setLinearVelocity( t2dVector( object->getLinearVelocity().mX, dAtof(argv[2]) ) );
}



//-----------------------------------------------------------------------------
// Set Angular Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setAngularVelocity, void, 3, 3, "(float velocity) - Sets Objects Angular Velocity.\n"
                                                              "Auto rotation and angular velocity are two different values, though both of them affect the speed at which the object rotates. Auto rotation is not affected by any of the object's physics, whereas angular velocity is. If an auto rotation is set, it will be used instead of angular velocity.\n"
                                                              "@param velocity The speed at which the object will rotate.\n"
																 "@return No return Value.")
{
    // Set Angular Velocity.
    object->setAngularVelocity( dAtof(argv[2]) );
}
void t2dSceneObject::setAngularVelocity( const F32 angularVelocity )
{
    // Set Gross Angular Velocity.
    getParentPhysics().setGrossAngularVelocity( angularVelocity );
}


//-----------------------------------------------------------------------------
// Get Angular Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAngularVelocity, F32, 2, 2, "() - Gets Object Angular Velocity.\n"
                                                             "@return (float velocity) The speed at which the object is rotating.")
{
    // Get Angular Velocity.
    return object->getAngularVelocity();
}


//-----------------------------------------------------------------------------
// Set Min Linear Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setMinLinearVelocity, void, 3, 3, "(float minLinearVelocity) - Set Object Min Linear Velocity.\n"
                                                                "Sets the minimum velocity at which the object is allowed to move.\n"
                                                                "@param minLinearVelocity The minimum velocity for the object.\n"
																 "@return No return Value.")
{
    // Set Min Linear Velocity.
    object->setMinLinearVelocity( dAtof(argv[2]) );
}
// Set Min Linear Velocity.
void t2dSceneObject::setMinLinearVelocity( const F32 velocity )
{
    // Set Min Linear Velocity.
    getParentPhysics().setMinLinearVelocity( velocity );
}


//-----------------------------------------------------------------------------
// Get Min Linear Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMinLinearVelocity, F32, 2, 2, "() - Gets the Objects Min Linear Velocity.\n"
                                                               "@return (float minLinearVelocity) The minimum velocity the object is allowed to move.")
{
    // Get Min Linear Velocity.
    return object->getMinLinearVelocity();
}


//-----------------------------------------------------------------------------
// Set Min Angular Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setMinAngularVelocity, void, 3, 3, "(float minAngularVelocity) - Set Object Min Angular Velocity.\n"
                                                                 "@param minAngularVelocity The minimum angular velocity the object is allowed to move.\n"
																 "@return No return Value.")
{
    // Set Min Angular Velocity.
    object->setMinAngularVelocity( dAtof(argv[2]) );
}
// Set Min Angular Velocity.
void t2dSceneObject::setMinAngularVelocity( const F32 velocity )
{
    // Set Max Angular Velocity.
    getParentPhysics().setMinAngularVelocity( velocity );
}


//-----------------------------------------------------------------------------
// Get Min Angular Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMinAngularVelocity, F32, 2, 2, "() - Gets the Objects Min Angular Velocity.\n"
                                                                "@return (float minAngularVelocity) The minimum angular velocity the object is allowed to move.")
{
    // Get Min Angular Velocity.
    return object->getMinAngularVelocity();
}


//-----------------------------------------------------------------------------
// Set Max Linear Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setMaxLinearVelocity, void, 3, 3, "(float maxLinearVelocity) - Set Object Max Linear Velocity.\n"
                                                                "@param maxLinearVelocity The maximum velocity the object is allowed to move.\n"
																 "@return No return Value.")
{
    // Set Max Linear Velocity.
    object->setMaxLinearVelocity( dAtof(argv[2]) );
}
// Set Max Linear Velocity.
void t2dSceneObject::setMaxLinearVelocity( const F32 velocity )
{
    // Set Max Linear Velocity.
    getParentPhysics().setMaxLinearVelocity( velocity );
}


//-----------------------------------------------------------------------------
// Get Max Linear Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMaxLinearVelocity, F32, 2, 2, "() - Gets the Objects Max Linear Velocity.\n"
                                                               "@return (float maxLinearVelocity) The maximum velocity the object is allowed to move.")
{
    // Get Max Linear Velocity.
    return object->getMaxLinearVelocity();
}


//-----------------------------------------------------------------------------
// Set Max Angular Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setMaxAngularVelocity, void, 3, 3, "(float maxAngularVelocity) - Set Object Max Angular Velocity.\n"
                                                                 "@param maxAngularVelocity The maximum angular velocity the object is allowed to move.\n"
																 "@return No return Value.")
{
    // Set Max Angular Velocity.
    object->setMaxAngularVelocity( dAtof(argv[2]) );
}
// Set Max Angular Velocity.
void t2dSceneObject::setMaxAngularVelocity( const F32 velocity )
{
    // Set Max Angular Velocity.
    getParentPhysics().setMaxAngularVelocity( velocity );
}


//-----------------------------------------------------------------------------
// Get Max Angular Velocity.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMaxAngularVelocity, F32, 2, 2, "() - Gets the Objects Max Angular Velocity.\n"
                                                                "@return (float maxAngularVelocity) The maximum angular velocity the object is allowed to move.")
{
    // Get Max Angular Velocity.
    return object->getMaxAngularVelocity();
}


//-----------------------------------------------------------------------------
// Set Physics Suppress.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setPhysicsSuppress, void, 3, 3, "(bool status?) - Sets the Objects Physics Suppress Status.\n"
                                                              "@param status True to disable physics, false to enable them.\n"
																 "@return No return Value.")
{
    // Set Physics Suppress.
    object->getParentPhysics().setPhysicsSuppress( dAtob(argv[2]) );
}


//-----------------------------------------------------------------------------
// Get Physics Suppress.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getPhysicsSuppress, bool, 2, 2, "() - Gets the Objects Physics Suppress Status.\n"
                                                              "@return (bool status) Whether or not the physics for this object are supressed.")
{
    // Get Physics Suppress.
    return object->getParentPhysics().getPhysicsSuppress();
}

ConsoleMethod(t2dSceneObject, setBlendingStatus, void, 3, 3, "(bool blendStatus) - Enables blending.\n"
                                                             "@param blendStatus Whether or not the object should use blending.\n"
																 "@return No return Value.")
{
   object->setBlending(dAtob(argv[2]), object->getSrcBlendFactor(), object->getDstBlendFactor());
}

ConsoleMethod(t2dSceneObject, setSrcBlendFactor, void, 3, 3, "(srcBlend) - Sets the blending properties of the object.\n"
                                                             "@param srcBlend The source blend factor.\n"
																 "@return No return Value.")
{
   object->setBlending(object->getBlendingStatus(), getSrcBlendFactorEnum(argv[2]), object->getDstBlendFactor());
}

ConsoleMethod(t2dSceneObject, setDstBlendFactor, void, 3, 3, "(dstBlend) - Sets the blending properties of the object.\n"
                                                             "@param dstBlend The source blend factor.\n"
																 "@return No return Value.")
{
   object->setBlending(object->getBlendingStatus(), object->getSrcBlendFactor(), getDstBlendFactorEnum(argv[2]));
}

ConsoleMethod(t2dSceneObject, getBlendingStatus, bool, 2, 2, "() - Returns the blending status.\n"
                                                             "@return (bool status) Whether or not the object is using blending.")
{
   return object->getBlendingStatus();
}

ConsoleMethod(t2dSceneObject, getSrcBlendFactor, const char*, 2, 2, "() - Returns the src blend factor.\n"
                                                                    "@return (srcBlend) The source blend factor.")
{
   return getSrcBlendFactorDescription(object->getSrcBlendFactor());
}

ConsoleMethod(t2dSceneObject, getDstBlendFactor, const char*, 2, 2, "() - Returns the dst blend factor.\n"
                                                                    "@return (dstBlend) The destination blend factor.")
{
   return getDstBlendFactorDescription(object->getDstBlendFactor());
}

//-----------------------------------------------------------------------------
// Set Blending.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setBlending, void, 3, 5, "(bool blendStatus?, [srcBlendFactor = GL_SRC_ALPHA], [dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA]) - Sets the Rendering Blend Options.\n"
                                                       "Blending controls how colors will be blended when they are overlapping in the scene. setBlending(%status) should only be used to turn blending off with setBlending(false). A subsequent call with setBlending(true) will reset srcFactor to SRC_ALPHA and dstFactor to ONE_MINUS_SRC_ALPHA.\n"
                                                       "@param blendStatus Enables or disables blending.\n"
                                                       "@param srcBlendFactor The source blend factor.\n"
                                                       "@param dstBlendFactor The destination blend factor. \n"
														"@return No return Value.")
{
    // Calculate Blending Factors.
    S32 srcBlendFactor = argc >= 4 ? getSrcBlendFactorEnum(argv[3]) : GL_SRC_ALPHA;
    S32 dstBlendFactor = argc >= 5 ? getDstBlendFactorEnum(argv[4]) : GL_ONE_MINUS_SRC_ALPHA;

    // Check Source Factor.
    if ( srcBlendFactor == GL_INVALID_BLEND_FACTOR )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setBlending() - Invalid srcBlendFactor '%s', defaulting to SRC_ALPHA!", argv[3]);
        // USe Default.
        srcBlendFactor = GL_SRC_ALPHA;
    }

    // Check Destination Factor.
    if ( dstBlendFactor == GL_INVALID_BLEND_FACTOR )
    {
        // Warn.
        Con::warnf("t2dSceneObject::setBlending() - Invalid dstBlendFactor '%s', defaulting to ONE_MINUS_SRC_ALPHA!", argv[4]);
        // USe Default.
        dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
    }

    // Set Blending.
    object->setBlending( dAtob(argv[2]), srcBlendFactor, dstBlendFactor );
}
// Set Blending.
void t2dSceneObject::setBlending( const bool status, const S32 srcBlendFactor, const S32 dstBlendFactor )
{
    // Set Blending Flag.
    mBlending = status;

    // Set Blending Factors.
    mSrcBlendFactor = srcBlendFactor;
    mDstBlendFactor = dstBlendFactor;
}

void t2dSceneObject::setBlendStatus( const bool status )
{
   mBlending = status;
}

void t2dSceneObject::setSrcBlendFactor( const S32 srcBlendFactor )
{
   mSrcBlendFactor = srcBlendFactor;
}

void t2dSceneObject::setDstBlendFactor( const S32 dstBlendFactor )
{
   mDstBlendFactor = dstBlendFactor;
}


//-----------------------------------------------------------------------------
// Get Blending.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getBlending, const char*, 2, 2, "() - Gets the Rendering Blend Options.\n"
                                                              "@return (bool blendStatus / srcBlendFactor / dstBlendFactor) Whether or not the blending is enabled as well as the source and destination blending factors.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(128);
    // Format Buffer.
    dSprintf(pBuffer, 128, "%d %s %s", S32(object->getBlendingStatus()), getSrcBlendFactorDescription((GLenum)object->getSrcBlendFactor()), getDstBlendFactorDescription((GLenum)object->getDstBlendFactor()) );
    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Set Blend Colour.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setBlendColour, void, 3, 6, "(float red, float green, float blue, [float alpha = 1.0]) - Sets the Rendering Blend Colour."
                                                          "The blend color specifies how much of each color in the original image to show. A red value of 0.5 will cut the amount of red shown in half. A blend color of (1.0, 1.0, 1.0) will not affect the image and a blend color of (0.0, 0.0, 0.0) will make the image black.\n"
                                                          "The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent."
                                                          "Blending must be enabled (as set in setBlending) for this to have any effect. It is enabled by default.\n"
                                                          "@param red The red value.\n"
                                                          "@param green The green value.\n"
                                                          "@param blue The blue value.\n"
                                                          "@param alpha The alpha value.\n"
																 "@return No return Value.")
{
    // Set Blend Colour.
    object->setBlendColourString(argc, argv);
}
// Alias.
ConsoleMethod(t2dSceneObject, setBlendColor, void, 3, 6, "(float red, float green, float blue, [float alpha = 1.0]) - Sets the Rendering Blend Color."
                                                          "The blend color specifies how much of each color in the original image to show. A red value of 0.5 will cut the amount of red shown in half. A blend color of (1.0, 1.0, 1.0) will not affect the image and a blend color of (0.0, 0.0, 0.0) will make the image black.\n"
                                                          "The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent."
                                                          "Blending must be enabled (as set in setBlending) for this to have any effect. It is enabled by default.\n"
                                                          "@param red The red value.\n"
                                                          "@param green The green value.\n"
                                                          "@param blue The blue value.\n"
                                                          "@param alpha The alpha value.\n"
																 "@return No return Value.")
{
    // Set Blend Colour.
    object->setBlendColourString(argc, argv);
}

// Set Blend Colour String.
void t2dSceneObject::setBlendColourString( S32 argc, const char** argv )
{
   // The colors.
   F32 red;
   F32 green;
   F32 blue;
   F32 alpha = 1.0f;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Space separated.
   if (argc < 4)
   {
      // ("R G B [A]")
      if ((elementCount == 3) || (elementCount == 4))
      {
         // Extract the color.
         red = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
         green = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
         blue = dAtof(t2dSceneObject::getStringElement(argv[2], 2));

         // Grab the alpha if it's there.
         if (elementCount > 3)
            alpha = dAtof(t2dSceneObject::getStringElement(argv[2], 3));
      }

      // Invalid.
      else
      {
         Con::warnf("t2dSceneObject::setBlendColour() - Invalid Number of parameters!");
         return;
      }
   }

   // (R, G, B)
   else if (argc >= 5)
   {
      red = dAtof(argv[2]);
      green = dAtof(argv[3]);
      blue = dAtof(argv[4]);

      // Grab the alpha if it's there.
      if (argc > 5)
         alpha = dAtof(argv[5]);
   }

   // Invalid.
   else
   {
      Con::warnf("t2dSceneObject::setBlendColour() - Invalid Number of parameters!");
      return;
   }

   // Set Blend Colour.
   setBlendColour(ColorF(red, green, blue, alpha));
}

void t2dSceneObject::setBlendColourString(const char* color)
{
   U32 elementCount = getStringElementCount(color);
   
   // ("R G B [A]")
   if ((elementCount == 3) || (elementCount == 4))
   {
      // Extract the color.
      F32 red = dAtof(t2dSceneObject::getStringElement(color, 0));
      F32 green = dAtof(t2dSceneObject::getStringElement(color, 1));
      F32 blue = dAtof(t2dSceneObject::getStringElement(color, 2));
      F32 alpha = 1.0f;

      // Grab the alpha if it's there.
      if (elementCount > 3)
         alpha = dAtof(t2dSceneObject::getStringElement(color, 3));
      

      setBlendColour(ColorF(red, green, blue, alpha));
   }
}

// Set Blend Colour.
void t2dSceneObject::setBlendColour( const ColorF& blendColour )
{
    // Set Blend Colour.
    mBlendColour = blendColour;
}


//-----------------------------------------------------------------------------
// Set Blend Alpha.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setBlendAlpha, void, 3, 3, "(float alpha) - Sets the Rendering Alpha (transparency).\n"
                                                         "The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent.\n"
                                                         "@param alpha The alpha value.\n"
																 "@return No return Value.")
{
    // Set Blend Alpha.
    object->setBlendAlpha( dAtof(argv[2]) );
}
// Set Blend Alpha.
void t2dSceneObject::setBlendAlpha( const F32 alpha )
{
    // Set Blend Alpha.
    mBlendColour.alpha = alpha;
}


//-----------------------------------------------------------------------------
// Get Blend Colour.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getBlendColour, const char*, 2, 2, "() - Gets the Rendering Blend Colour.\n"
                                                                 "@return (float red / float green / float blue / float alpha) The red, green, blue, and alpha values of this object's blending color (a range from 0.0 to 1.0).")
{
    // Get Blend Colour.
    ColorF blendColour = object->getBlendColour();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f %f %f", blendColour.red, blendColour.green, blendColour.blue, blendColour.alpha );
    // Return buffer.
    return pBuffer;
}
// Alias.
ConsoleMethod(t2dSceneObject, getBlendColor, const char*, 2, 2, "Gets the Rendering Blend Colour.\n"
                                                                "@return (float red / float green / float blue / float alpha) The red, green, blue, and alpha values of this object's blending color (a range from 0.0 to 1.0).")
{
    // Get Blend Colour.
    ColorF blendColour = object->getBlendColour();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f %f %f", blendColour.red, blendColour.green, blendColour.blue, blendColour.alpha );
    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Blend Alpha.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getBlendAlpha, F32, 2, 2, "() - Gets the Rendering Alpha (transparency).\n"
                                                        "@return (float alpha) The alpha value, a range from 0.0 to 1.0.")
{
    // Get Blend Alpha.
    return object->getBlendAlpha();
}


//-----------------------------------------------------------------------------
// Get Local Coordinate.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLocalPoint, const char*, 3, 4, "(float worldX, float worldY) - Returns local-point for object.\n"
                                                                "Converts a point in world space to object space.\n"
                                                                "@param worldX The world position x you want to convert into local x."
                                                                "@param worldY The world position y you want to convert into local y."
                                                                "@return (float localX / float local Y) The local position that was converted from the world position passed.")
{
   // The new position.
   t2dVector worldPoint;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      worldPoint = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      worldPoint = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::getLocalPoint() - Invalid number of parameters!");
      return NULL;
   }

    // Calculate Local Coordindate.
    t2dVector localPoint = object->getLocalPoint(worldPoint);

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", localPoint.mX, localPoint.mY);
    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get World Coordinate.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getWorldPoint, const char*, 3, 4, "(float localX, float localY) - Returns world-point from object.\n"
                                                                "Converts a point in object space to world space.\n"
                                                                "@param localX The local x position you want to convert into world x."
                                                                "@param localY The local y position you want to convert into world y."
                                                                "@return (float worldX / float worldY) The world position that was converted from the local position passed.")
{
   // The new position.
   t2dVector localPoint;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      localPoint = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      localPoint = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::getWorldPoint() - Invalid number of parameters!");
      return false;
   }

    // Calculate World Coordindate.
    t2dVector worldPoint = object->getWorldPoint(localPoint);

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", worldPoint.mX, worldPoint.mY);
    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Check World-Point against Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getIsPointInObject, bool, 3, 4, "(worldX, worldY) - Returns whether point intersects with object.\n"
                                                              "@param worldX The x component of the world position to check.\n"
                                                              "@param worldY The y component of the world position to check.\n"
                                                              "@return (bool isInside) True if the point is inside the object, false otherwise.")
{
   t2dVector point;

   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("X Y")
   if ((elementCount == 2) && (argc == 3))
      point = t2dSceneObject::getStringElementVector(argv[2]);

   // (X, Y)
   else if ((elementCount == 1) && (argc == 4))
      point = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::getIsPointInObject() - Invalid number of parameters!");
      return false;
   }

    // Calculate if point intersects the object.
    return object->getIsPointInObject( point );
}
// Check World-Point against Object.
bool t2dSceneObject::getIsPointInObject( const t2dVector& worldPoint )
{
    // Collision Details.
    t2dVector collisionNormal;
    F32 collisionTime;

    // Do collision check.
    // NOTE:-   This is a singularity-line (point) test.  We should really use a much more
    //          efficient point-test really.
    return getParentPhysics().castLine( worldPoint, worldPoint, collisionNormal, collisionTime );
}


//-----------------------------------------------------------------------------
// Set Debug On Mode(s).
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setDebugOn, void, 3, 2 + T2D_DEBUGMODE_COUNT, "(debugMode) - Sets Debug On Mode(s).\n"
                                                                            "The debug banner (debug mode 0) has no effect on t2dSceneObjects. It is only relevant to t2dSceneGraphs.\n"
                                                                            "@param debugMode The debug modes to enable, this can be one or all in a comma seperated list.  <br>Possible Values<br>"
                                                                            " -1. Bounding, Collision, and Clipping Boxes: Displays the axis aligned bounding box in dark cyan, the axis aligned collision bounding box in light cyan, and various clipping rectangles.<br>"
                                                                            " -2. Mount Nodes: Shows, as yellow crosses, all the mount nodes.<br>"
                                                                            " -3. Mount Force Lines: Shows, in red, the force lines for mounted objects.<br>"
                                                                            " -4. World Limit: Shows, in white, the world limit rectangle.<br>"
                                                                            " -5. Collision Bounds: Shows, in green, the object collision polygon or circle.\n"
																			"@return No return Value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if ((elementCount < 1) || (elementCount > T2D_DEBUGMODE_COUNT))
   {
      Con::warnf("t2dSceneObject::setDebugOn() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneObject::setDebugOn() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneObject::setDebugOn() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Debug Mode.
   object->setDebugOn(mask);
}   
// Set Debug On Mode(s).
void t2dSceneObject::setDebugOn( const U32 debugMask )
{
    // Set Debug On Mask.
    mDebugMask |= debugMask;
}


//-----------------------------------------------------------------------------
// Set Debug Off Mode(s).
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setDebugOff, void, 3, 2 + T2D_DEBUGMODE_COUNT, "(debugMask) - Sets Debug Off Mask.\n"
                                                                             "Disables debug modes.\n"
                                                                             "@param debugMask A list of debug modes to disable, see setDebugOn() for a list of modes you can pass.\n"
																			 "@return No return Value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneObject::setDebugOff() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneObject::setDebugOff() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneObject::setDebugOff() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   object->setDebugOff(mask);
}
// Set Debug Off Mode(s).
void t2dSceneObject::setDebugOff( const U32 debugMask )
{
    // Set Debug Off Mask.
    mDebugMask &= ~debugMask;
}

ConsoleMethod(t2dSceneObject, getAttachedToPath, S32, 2, 2, "() - Gets the t2dPath that this object is attached to.\n"
                                                            "@return (t2dPath path) The path that this object is attached to, or 0 if it is not attached to a path.")
{
   t2dSceneObject* path = object->getAttachedToPath();
   if (path)
      return path->getId();
   
   return NULL;
}

//-----------------------------------------------------------------------------
// Mount Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, mount, S32, 3, 10, "(t2dSceneObject object, [float offsetX = 0], [float offsetY = 0], [float mountForce = 0], [bool trackRotation? = true], [bool sendToMount? = true], [bool ownedByMount? = true], [bool inheritAttributes? = true] ) - Mounts onto a specified object.\n"
                                                 "Object's can be mounted to any other object regardless of whether they have other mounts, or are mounted themselves. The only restriction is that there cannot be any cyclical mounting (two objects mounted to each other, for example).\n"
                                                 "The functions listed other than mount only work if the object is already mounted to another object. \n"
                                                 "@param object The object to mount to.\n"
                                                 "@param offsetX The x position to mount to in the object space of %object.\n"
                                                 "@param offsetY The y position to mount to in the object space of %object.\n"
                                                 "@param mountForce The magnitude of the force constantly applied to the object to keep it at the mount point. A value of 0 makes the mount rigid, and thus stuck to the object it is mounted to.\n"
                                                 "@param trackRotation Whether or not to track the rotation of the object this is being mounted to.\n"
                                                 "@param sendToMount Whether or not to send the object directly to the mount point.\n"
                                                 "@param ownedByMount Whether or not the object is deleted when the object it is mounted to is deleted.\n"
                                                 "@param inheritAttributes Whether or not to inherit certain attributes from the object this is being mounted to. The inherited attributes are enabled, visible, paused, and flip.\n"
                                                 "@return (integer mountID) The mount ID if successful or -1 if the mount fails.")
{
    // Grab the object. Always specified.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if (!pSceneObject2D)
    {
        Con::warnf("t2dSceneObject::mount() - Couldn't find/Invalid object '%s'.", argv[2]);
        return -1;
    }

    // Reset Element Count.
    U32 elementCount = 2;
    // Calculate Mount-Offset.
    t2dVector mountOffset(0.0f, 0.0f);

    if (argc > 3)
    {
        // Fetch Element Count.
        elementCount = t2dSceneObject::getStringElementCount(argv[3]);

        // (object, "offsetX offsetY", ...)
        if ((elementCount == 2) && (argc < 10))
            mountOffset = t2dSceneObject::getStringElementVector(argv[3]);

        // (object, offsetX, offsetY, ...)
        else if ((elementCount == 1) && (argc > 4))
            mountOffset = t2dVector(dAtof(argv[3]), dAtof(argv[4]));

        // Invalid.
        else
        {
            Con::warnf("t2dSceneObject::mount() - Invalid number of parameters!");
            return -1;
        }
    }

    // Set the next arg index.
    // The argv index of the first parameter after the offset.
    U32 firstArg = 6 - elementCount;

    // Grab the mount force - if it's specified.
    F32 mountForce = 0.0f;
    if (argc > firstArg)
        mountForce = dAtof(argv[firstArg]);

    // Grab the track rotation flag.
    bool trackRotation = true;
    if (argc > (firstArg + 1))
        trackRotation = dAtob(argv[firstArg + 1]);

    // Grab the send to mount flag.
    bool sendToMount = true;
    if (argc > (firstArg + 2))
        sendToMount = dAtob(argv[firstArg + 2]);

    // Grab the owned by mount flag.
    bool ownedByMount = true;
    if (argc > (firstArg + 3))
        ownedByMount = dAtob(argv[firstArg + 3]);

    // Grab the inherit attributes flag.
    bool inheritAttributes = true;
    if (argc > (firstArg + 4))
        inheritAttributes = dAtob(argv[firstArg + 4]);

    // Perform the mounting.
    const S32 mountID = object->mount( pSceneObject2D, mountOffset, mountForce, trackRotation, sendToMount, ownedByMount, inheritAttributes );

    // Update Container Configuration (if we actual got an index).
    if (mountID != -1)
    {
        // Update Parent Mount.
        pSceneObject2D->updateSpatialConfig();
        // Update our Mount.
        object->updateSpatialConfig();
    }

    // Return Mount-ID.
    return mountID;
}
// Mount Object.
S32 t2dSceneObject::mount( t2dSceneObject* pSceneObject2D, const t2dVector& mountOffset, const F32 mountForce, const bool trackRotation, const bool sendToMount, const bool ownedByMount, const bool inheritAttributes )
{
   // Check object validity
   if( !pSceneObject2D )
   {
      Con::warnf("t2dSceneObject::mount() - invalid object pointer provided!");
      return -1;
   }

    S32 count = pSceneObject2D->getMountNodeCount();
    S32 mountID = pSceneObject2D->addLinkPoint( mountOffset );
    S32 ret = mount( pSceneObject2D, mountID, mountForce, trackRotation, sendToMount, ownedByMount, inheritAttributes );

    // Don't want to add the link point if the mount failed, or increment the ref count if the link point existed.
    if( ( ret == -1 ) || ( count == pSceneObject2D->getMountNodeCount() ) )
       pSceneObject2D->removeLinkPoint( mountID );

    return ret;
}


// Mount Object to an already existing link point.
ConsoleMethod(t2dSceneObject, mountToLinkpoint, S32, 3, 10, "(t2dSceneObject object, int linkpointID, [float mountForce = 0], [bool trackRotation? = true], [bool sendToMount? = true], [bool ownedByMount? = true], [bool inheritAttributes? = true] ) - Mounts onto a specified object at the linkpoint specified.\n"
                                                 "Object's can be mounted to any other object regardless of whether they have other mounts, or are mounted themselves. The only restriction is that there cannot be any cyclical mounting (two objects mounted to each other, for example).\n"
                                                 "The functions listed other than mount only work if the object is already mounted to another object. \n"
                                                 "@param object The object to mount to.\n"
												 "@param linkpointID the linkpoint on that object that we're mounting to.\n"
												 "@param mountForce The magnitude of the force constantly applied to the object to keep it at the mount point. A value of 0 makes the mount rigid, and thus stuck to the object it is mounted to.\n"
                                                 "@param trackRotation Whether or not to track the rotation of the object this is being mounted to.\n"
                                                 "@param sendToMount Whether or not to send the object directly to the mount point.\n"
                                                 "@param ownedByMount Whether or not the object is deleted when the object it is mounted to is deleted.\n"
                                                 "@param inheritAttributes Whether or not to inherit certain attributes from the object this is being mounted to. The inherited attributes are enabled, visible, paused, and flip.\n"
                                                 "@return (integer mountID) The mount ID if successful or -1 if the mount fails.")
{
    // Grab the object. Always specified.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if (!pSceneObject2D)
    {
        Con::warnf("t2dSceneObject::mountToLinkpoint() - Couldn't find/Invalid object '%s'.", argv[2]);
        return -1;
    }

    // Reset Element Count.
    // UNUSED: JOSEPH THOMAS -> U32 elementCount = 2;

	U32 linkpointID = 0;

    if (argc > 3)
    {
		linkpointID = dAtoi(argv[3]);
    }

	if (pSceneObject2D->getMountNode(linkpointID) == NULL)
	{
		Con::warnf("t2dSceneObject::mountToLinkpoint() - Couldn't find linkpointID %d on object %s", linkpointID, argv[2]);
		return -1;
	}

	if ((object->mpMountedTo == pSceneObject2D) && (object->mMountToID == linkpointID))
	{
		Con::warnf("t2dSceneObject::mountToLinkpoint() - Already mounted to %s at linkpoint %d", argv[2], linkpointID);
		return linkpointID;
	}

    // Set the next arg index.
    // The argv index of the first parameter after the linkpoint.
    U32 firstArg = 4;

    // Grab the mount force - if it's specified.
    F32 mountForce = 0.0f;
    if (argc > firstArg)
        mountForce = dAtof(argv[firstArg]);

    // Grab the track rotation flag.
    bool trackRotation = true;
    if (argc > (firstArg + 1))
        trackRotation = dAtob(argv[firstArg + 1]);

    // Grab the send to mount flag.
    bool sendToMount = true;
    if (argc > (firstArg + 2))
        sendToMount = dAtob(argv[firstArg + 2]);

    // Grab the owned by mount flag.
    bool ownedByMount = true;
    if (argc > (firstArg + 3))
        ownedByMount = dAtob(argv[firstArg + 3]);

    // Grab the inherit attributes flag.
    bool inheritAttributes = true;
    if (argc > (firstArg + 4))
        inheritAttributes = dAtob(argv[firstArg + 4]);

    // Perform the mounting.
	const S32 mountID = object->mount( pSceneObject2D, linkpointID, mountForce, trackRotation, sendToMount, ownedByMount, inheritAttributes );

    // Update Container Configuration (if we actual got an index).
    if (mountID != -1)
    {
        // Update Parent Mount.
        pSceneObject2D->updateSpatialConfig();
        // Update our Mount.
        object->updateSpatialConfig();
    }

    // Return Mount-ID.
    return mountID;
}


S32 t2dSceneObject::mount( t2dSceneObject* pSceneObject2D, const U32& mountID, const F32 mountForce, const bool trackRotation, const bool sendToMount, const bool ownedByMount, const bool inheritAttributes )
{
   // Check object validity
   if( !pSceneObject2D )
   {
      Con::warnf("t2dSceneObject::mount() - invalid object pointer provided!");
      return -1;
   }

    // Check for a valid mount id.
    if (!pSceneObject2D->getMountNode(mountID))
    {
       Con::warnf("t2dSceneObject::mount() - '%d' is not in a valid mount node!", mountID);
       return -1;
    }

    // Check if we're actually in a scene graph.
    if ( !mpSceneGraph )
    {
        Con::warnf("t2dSceneObject::mount() - Object '%s' is not in a SceneGraph!", getIdString());
        return -1;
    }
    // Check we're not mounting to ourself!
    if ( pSceneObject2D == this )
    {
        Con::warnf("t2dSceneObject::mount() - Cannot mount to myself! ('%s')", pSceneObject2D->getIdString());
        return -1;
    }
    // Check if object is actually in a scene graph.
    if ( !pSceneObject2D->mpSceneGraph )
    {
        Con::warnf("t2dSceneObject::mount() - Object '%s' is not in a SceneGraph!", pSceneObject2D->getIdString());
        return -1;
    }
    // Check if object is in the same scene graph.
    if ( pSceneObject2D->mpSceneGraph != mpSceneGraph )
    {
        Con::warnf("t2dSceneObject::mount() - Object '%s' is not in the same SceneGraph!", pSceneObject2D->getIdString());
        return -1;
    }

    // Are we already mounted?
    if ( processIsMounted() )
    {
        // Yes, so dismount.
        dismount();
    }

    // Fetch start of mounting chain.
    t2dSceneObject* pSceneObject2DRef = pSceneObject2D;
    // Check for cyclic mountings...
    while ( pSceneObject2DRef->processIsMounted() )
    {
        // Move to mount.
        pSceneObject2DRef = pSceneObject2DRef->mpMountedTo;

        // Is it mounted to us?
        if ( pSceneObject2DRef == this )
        {
            // Yes, so warn!
            Con::warnf("t2dSceneObject::mount() - Cannot mount to object that's mounted to me! ('%s')", getIdString());
            return -1;
        }
    };

    // Unlink from Process.
    processUnLink();
    // Change Process Order.
    processLinkAfter( pSceneObject2D );
    // Set Mount Object Reference.
    mpMountedTo = pSceneObject2D;
    // Store Mount Offset.
    mMountOffset = mpMountedTo->getMountNode(mountID)->mLocalMountPosition;
    // Set Mount Force.
    mMountForce = mountForce;
    // Set Track Rotation.
    mMountTrackRotation = trackRotation;
    // Set Pre-Mount Rotation.
    mMountPreRotation = getRotation();
    // Set Mount Ownership.
    mMountOwned = ownedByMount;
    // Set Inherit Attributes.
    mMountInheritAttributes = inheritAttributes;
    // Create a Mount Node.
    // NOTE:-   We add ourself if we're owned by the mount object.
    mMountToID = mountID;

	//if our mount parent has physics, make sure we do too. If they don't and we do,
	//we may have had that set previously for a different purpose so leave it
	if( mpMountedTo->getUsesPhysics() ) {
		setUsesPhysics( true );
	}

    // Add a destroy notification.
    mpMountedTo->addDestroyNotification( this );

    // Increment reference count.
    mpMountedTo->getMountNode( mMountToID )->mMountReferenceCount++;

    // Send to Mount ( if selected ).
    if ( sendToMount )
    {
        // Set to Mount Point.
        setPosition( mpMountedTo->getMountNode(mMountToID)->mWorldMountPosition );
    }

    // Return Mount ID.
    return mMountToID;
}

ConsoleMethod(t2dSceneObject, getMountForce, F32, 2, 2, "() - Returns the mount force.\n"
                                                        "@return (float mountForce The mountForce value.")
{
   if (object->processIsMounted())
      return object->getMountForce();

   return 0.0f;
}

ConsoleMethod(t2dSceneObject, setMountForce, void, 3, 3, "(float mountForce) - Sets the mount force.\n"
                                                         "@param mountForce The magnitude of the force constantly applied to the object to keep it at the mount point. A value of 0 makes the mount rigid, and thus stuck to the object it is mounted to.\n"
																 "@return No return Value.")
{
   if (object->processIsMounted())
      object->setMountForce(dAtof(argv[2]));
}

ConsoleMethod(t2dSceneObject, getMountTrackRotation, bool, 2, 2, "() - Returns the track rotation status.\n"
                                                                 "@return (bool trackRotation) Whether or not the mount is set to track rotation.")
{
   if (object->processIsMounted())
      return object->getMountTrackRotation();

   return false;
}

ConsoleMethod(t2dSceneObject, setMountTrackRotation, void, 3, 3, "(bool trackRotation) - Sets the track rotation status.\n"
                                                                 "@param trackRotation Whether or not to track the rotation of the object this is being mounted to.\n"
																 "@return No return Value.")
{
   if (object->processIsMounted())
      object->setMountTrackRotation(dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getMountOwned, bool, 2, 2, "() - Returns the mount owned status.\n"
                                                         "@return (bool ownedByMount) Whether or not the object is deleted when the object it is mounted to is deleted.")
{
   if (object->processIsMounted())
      return object->getMountOwned();

   return false;
}

ConsoleMethod(t2dSceneObject, setMountOwned, void, 3, 3, "(bool ownedByMount) - Sets the mount owned status.\n"
                                                         "@param ownedByMount Whether or not the object is deleted when the object it is mounted to is deleted.\n"
																 "@return No return Value.")
{
   if (object->processIsMounted())
      object->setMountOwned(dAtob(argv[2]));
}

ConsoleMethod(t2dSceneObject, getMountInheritAttributes, bool, 2, 2, "() - Returns the inherit attributes status.\n"
                                                                     "@return (bool inheritAttributes) Whether or not this object is inheriting attributes from the object it is mounted to.")
{
   if (object->processIsMounted())
      return object->getMountInheritAttributes();

   return false;
}

ConsoleMethod(t2dSceneObject, setMountInheritAttributes, void, 3, 3, "(bool inheritAttributes) - Sets the mount inherit attributes status.\n"
                                                                     "@param inheritAttributes Whether or not to inherit certain attributes from the object this is being mounted to. The inherited attributes are enabled, visible, paused, and flip.\n"
																 "@return No return Value.")
{
   if (object->processIsMounted())
      object->setMountInheritAttributes(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------
// Dismount Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, dismount, void, 2, 2, "() - Dismounts object.\n"
                                                    "@return (bool dismounted) Returns true if it properly dismounted, false if it didn't.")
{
    // Dismount Object.
    object->dismount();
}
// Dismount Object.
bool t2dSceneObject::dismount( void )
{
    // Check if object is actually in a scene graph.
    if ( !mpSceneGraph )
    {
        Con::warnf("t2dSceneObject::dismount - Object '%s' is not in a SceneGraph!.", getIdString());
        return false;
    }

    if ( !processIsMounted() )
    {
        //Con::warnf("t2dSceneObject::dismount - Object '%s' is not mounted!", getIdString());
        return false;
    }

    // Remove Mount Node.
    mpMountedTo->removeLinkPoint( mMountToID );

    // Remove destroy notification.
    mpMountedTo->removeDestroyNotification( this );

    // Reset Mount.
    mpMountedTo = NULL;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Get Is Mounted?
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getIsMounted, bool, 2, 2, "() - Get 'Is Mounted' Status.\n"
                                                        "@return (bool isMounted) Whether or not the object is mounted.")
{
    // Get Is Mounted?
    return object->getIsMounted();
}

//-----------------------------------------------------------------------------
// Get Mounted Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getMountedParent, S32, 2, 2, "() - Get Parent we're mounted-to.\n"
                                                           "@return (integer parentID) The parent's object ID if it is mounted, otherwise 0.")
{
    // Return nothing if we're not mounted.
    if ( !object->processIsMounted() )
        return 0;

    // Return Parent Mount.
    return object->getProcessMount()->getId();
}


//-----------------------------------------------------------------------------
// Add Link Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, addLinkPoint, S32, 2, 4, "([float offsetX / float offsetY]) - Adds a Link-Point.\n"
                                                       "@param offsetX the local x offset to create the point at.\n"
                                                       "@param offsetY the local y offset to create the point at.\n"
                                                       "@return (integer linkPointID) The link point ID if created properly, otherwise -1.")
{
    t2dVector nodeOffset(0,0);

    if (argc > 2)
    {
        // Check Parameters.
        U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

        // (offsetX, offsetY)
        if ((elementCount == 1) && (argc == 4))
        {
           nodeOffset = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
        }

        // ("offsetX offsetY")
        else if ((elementCount == 2) && (argc == 3))
        {
           // Fetch Node Offset.
           nodeOffset = t2dSceneObject::getStringElementVector(argv[2]);
        }

        // Invalid
        else
        {
            Con::warnf("t2dSceneObject::addLinkPoint() - Invalid number of parameters!");
            return -1;
        }
    }

    // Add Link Point.
    return S32(object->addLinkPoint(nodeOffset));
}

// Add Link Point.
U32 t2dSceneObject::addLinkPoint( const t2dVector& nodeOffset )
{
   // Link points are now being reference counted as well so as to match the functionality of
   // mount nodes. This is necessary for proper working with level loading. ADL.

   // Check if the node exists.
   for ( U32 nodeIndex = 0; nodeIndex < mMountNodes.size(); nodeIndex++ )
   {
      // Fetch Node Reference.
      tMountNode& mountNode = mMountNodes[nodeIndex];

      // Yes, so does this nodes' local mount position match?
      if ( mountNode.mLocalMountPosition.isEqual(nodeOffset) )
      {
         // Yes, so increase reference count.
         mountNode.mMountReferenceCount++;

         // Return Node Index.
         return mountNode.mMountID;
      }
   }

   // We are going to find the lowest available mount node id so they don't continually getting larger.
   // Internally this is no big deal, but it makes things nicer for the editor.
   // First we have to find the highest existing mount node id...
   S32 highestNode = 0;
   for (S32 i = 0; i < mMountNodes.size(); i++)
   {
      if (mMountNodes[i].mMountID > highestNode)
         highestNode = mMountNodes[i].mMountID;
   }

   // Now we loop through all the indexes from 0 to the highest node, breaking on the first available one.
   S32 lowestID = highestNode + 1;
   for (S32 i = 1; i <= highestNode; i++)
   {
      bool found = false;
      for (S32 j = 0; j < mMountNodes.size(); j++)
      {
         if (mMountNodes[j].mMountID == i)
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         lowestID = i;
         break;
      }
   }

    // Node doesn't exist so add it.
    tMountNode newNode;
    newNode.mLocalMountPosition = nodeOffset;
    newNode.mMountID = lowestID;
    newNode.mMountReferenceCount = 1;

    // Calculate Mount Node: Transform Point to World-Space...

    // Get Object Half-Size.
    const t2dVector& halfSize = getParentPhysics().getHalfSize();
    // Transform Sized Local-Space into World-Space.
    transformPoint2D( mMountRotationMatrix, t2dVector(newNode.mLocalMountPosition.mX * halfSize.mX, newNode.mLocalMountPosition.mY * halfSize.mY), getPosition(), newNode.mWorldMountPosition );

    // Add Mount Node.
    mMountNodes.push_back( newNode );

    // Return Mount-ID.
    return newNode.mMountID;
}


//-----------------------------------------------------------------------------
// Remove Link Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, removeLinkPoint, void, 3, 3, "(integer mountID) - Removes a Link-Point.\n"
                                                           "@param mountID The id of the link point to remove.\n"
																 "@return No return Value.")
{
    // Remove Link Point.
    object->removeLinkPoint( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------
// Remove Mount Node.
//-----------------------------------------------------------------------------
void t2dSceneObject::removeLinkPoint( const U32 mountID )
{
    // Scan for Mount-ID.
    for ( U32 node = 0; node < mMountNodes.size(); ++node )
    {
        // Fetch Mount Node.
        tMountNode& mountNode = mMountNodes[node];

        // Is this the selected Mount-ID?
        if ( mountNode.mMountID == mountID )
        {
            // Sanity.
            AssertFatal( mountNode.mMountReferenceCount != 0, "t2dSceneObject::removeMountNode() - Mounted-Reference is already zero!" );

            // Yes, so decease reference count.
            mountNode.mMountReferenceCount--;

            // Have we any mount-references?
            if ( mountNode.mMountReferenceCount < 1 )
            {
               // Remove Mount Node Safely.
               mMountNodes.erase_fast( node );
            }

            // Finish Here.
            return;
        }
    }

    // Warn.
    Con::warnf("t2dSceneObject::removeMountNode() - Couldn't find mount-ID (%d)", mountID);
}


//-----------------------------------------------------------------------------
// Remove All Link Points.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, removeAllLinkPoints, void, 2, 2, "() - Removes all Link-Points.\n"
																 "@return No return Value.")
{
    // Remove All Link Points.
    object->removeAllLinkPoints();
}
// Remove All Link Points.
void t2dSceneObject::removeAllLinkPoints( void )
{
    // Scan for Mount-ID.
    for ( U32 node = 0; node < mMountNodes.size(); )
    {
        // Fetch Mount Node.
        tMountNode& mountNode = mMountNodes[node];

        mountNode.mMountReferenceCount--;

        // Have we any mount-references?
        if ( mountNode.mMountReferenceCount < 1 )
        {
            // No, so remove node.
            mMountNodes.erase_fast( node );
            // Continue.
            continue;
        }

        // Next Node.
        ++node;
    }

    mMountNodes.clear();
}

//-----------------------------------------------------------------------------
// Set Link Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setLinkPoint, void, 4, 5, "(integer mountID, float offsetX, float offsetY) - Sets a Link-Point Position.\n"
                                                        "@param mountID The id of the link point.\n"
                                                        "@param offsetX the local x offset to move the point to.\n"
                                                        "@param offsetY the local y offset to move the point to.\n"
																 "@return No return Value.")
{
   t2dVector offset;

   U32 elementCount = t2dSceneObject::getStringElementCount(argv[3]);

   // (mountID, "offsetX offsetY")
   if ((elementCount == 2) && (argc == 4))
      offset = t2dSceneObject::getStringElementVector(argv[3]);

   // (mountID, offsetX, offsetY)
   else if ((elementCount == 1) && (argc == 5))
      offset = t2dVector(dAtof(argv[3]), dAtof(argv[4]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setLinkPoint() - Invalid number of parameters!");
      return;
   }

    // Set Link Point.
    object->setLinkPoint( dAtoi(argv[2]), offset );
}
// Set Link Point.
bool t2dSceneObject::setLinkPoint( const U32 mountID, const t2dVector& nodeOffset )
{
    // Fetch Mount Position.
    tMountNode* pMountNode = getMountNode(mountID);

    // Finish if the mount-node is invalid.
     if ( !pMountNode ) return false;

    // Set Mount Node.
    pMountNode->mLocalMountPosition = nodeOffset;

    // Update Mount Node.
    const t2dVector& halfSize = getParentPhysics().getHalfSize();
    // Transform Sized Local-Space into World-Space.
    transformPoint2D( mMountRotationMatrix, t2dVector(pMountNode->mLocalMountPosition.mX * halfSize.mX,pMountNode->mLocalMountPosition.mY * halfSize.mY), getPosition(), pMountNode->mWorldMountPosition );

    return true;
}


//-----------------------------------------------------------------------------
// Get Mount Node.
//-----------------------------------------------------------------------------
t2dSceneObject::tMountNode* t2dSceneObject::getMountNode( const U32 mountID )
{
    // Scan for Mount-ID.
    for ( U32 node = 0; node < mMountNodes.size(); ++node )
    {
        // Fetch Mount Node.
        tMountNode* pMountNode = &(mMountNodes[node]);

        // Is this the selected Mount-ID?
        if ( pMountNode->mMountID == mountID )
        {
            // Yes, so return Mount Node.
            return pMountNode;
        }
    }

    // Warn. The editor mount tool makes use of this to check if nodes are valid so this
    // needs to be commented out to avoid console spam.
    //Con::warnf("t2dSceneObject::getMountNode() - Couldn't find mount-ID (%d)", mountID);
    // Return Invalid Node.
    return NULL;
}


//-----------------------------------------------------------------------------
// Calculate Mount Nodes.
//-----------------------------------------------------------------------------
void t2dSceneObject::calculateMountNodes( const t2dVector& position )
{
    // Finish if we've not got any mount nodes.
    if ( mMountNodes.size() == 0 )
        return;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_calculateMountNodes);
#endif

    // Calculate Mount Rotation Matrix.
    mMountRotationMatrix.orientate( getRotation()+getMountRotation() );

    // Are we flipped?
    if ( mFlipX || mFlipY )
    {
        // Yes, so get Object Half-Size.
        const t2dVector halfSize = getHalfSize();

        t2dVector localPosition;

        // Step through Flipped Mount Nodes.
        for ( U32 node = 0; node < mMountNodes.size(); node++ )
        {
            // Fetch Node Reference.
            tMountNode& nodeRef = mMountNodes[node];

            // Fetch Local Mount Position.
            localPosition = nodeRef.mLocalMountPosition;

            // Flip X?
            if ( mFlipX )
                localPosition.mX = -localPosition.mX;

            // Flip Y?
            if ( mFlipY )
                localPosition.mY = -localPosition.mY;

            // Transform Sized Local-Space into World-Space.
            transformPoint2D( mMountRotationMatrix, localPosition.mult( halfSize ), position, nodeRef.mWorldMountPosition );
        }
    }
    else
    {
        // No, so get Object Half-Size.
        const t2dVector halfSize = getHalfSize();

        t2dVector localPosition;

        // No, so step through Mount Nodes.
        for ( U32 node = 0; node < mMountNodes.size(); node++ )
        {
            // Fetch Node Reference.
            tMountNode& nodeRef = mMountNodes[node];

            // Fetch Local Mount Position.
            localPosition = nodeRef.mLocalMountPosition;

            // Transform Sized Local-Space into World-Space.
            transformPoint2D( mMountRotationMatrix, localPosition.mult( halfSize ), position, nodeRef.mWorldMountPosition );
        }
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_calculateMountNodes
#endif
}


//-----------------------------------------------------------------------------
// Update Mount.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateMount( const F32 elapsedTime )
{
    // Ignore if not mounted.
    if ( !processIsMounted() )
        return;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_updateMount);
#endif

    // Fetch Mount Position.
    const t2dVector& mountPos = mpMountedTo->getMountNode(mMountToID)->mWorldMountPosition;

    // Update mount offset.
    mMountOffset = mpMountedTo->getMountNode(mMountToID)->mLocalMountPosition;

    // Fetch Current Position.
    const t2dVector currentPos = getPosition();

    // Are we fractionally close to the mount position?
    if ( currentPos.notEqual(mountPos) && (mountPos-currentPos).lenSquared() < T2D_CONST_EPSILON )
    {
        // Yes, so set to mount position.
        getParentPhysics().updateTickPosition( mountPos );
    }
    else
    {
        // No, so are we mounted rigidly?
        if ( mNotZero(mMountForce) )
        {
            // No, so calculate direction to mount position.
            t2dVector direction = mountPos - currentPos;
            // Fetch Direction Magnitude.
            F32 magnitude = direction.len();

            // Skip normalisation if at mount position.
            if ( mNotZero(magnitude) )
            {
                // Renomalise to mount-force magnitude.
                direction.normalise( magnitude * mMountForce * elapsedTime );
            }

            // Have we exceeded our step!
            if ( mLessThan(direction.len(), magnitude) )
            {
                // No, so set new position.
                getParentPhysics().updateTickPosition( currentPos + direction );
            }
            else
            {
                // Yes, so set to mount position.
                getParentPhysics().updateTickPosition( mountPos );
            }
        }
        else
        {
            // Yes, so set new position.
            getParentPhysics().updateTickPosition( mountPos );
        }
    }

    // Track rotation ( if selected ).
    if ( mMountTrackRotation )
    {
        // Calculate Mount Rotation.
        // NOTE:-   We're using the parents mount-rotation, rotation and this objects pre-rotation value.
        const F32 mountRotation = mpMountedTo->getMountRotation() + mpMountedTo->getRotation() + mMountPreRotation;

        // Update Tick Rotation.
        getParentPhysics().updateTickRotation( mountRotation );

        // Are we at the target mount rotation?
        if ( mNotEqual(mountRotation, getRotation()) )
        {
            // No, so set rotation.
            setRotation( mountRotation);
        }
    }
    else
    {
        // Non-Tracked Rotation...

        // Update Tick Rotation.
        getParentPhysics().updateTickRotation();
    }

    // Are we inheriting attributes?
    if ( mMountInheritAttributes )
    {
        // Yes, so we share common attributes...

        // Enabled.
        setEnabled( mpMountedTo->isEnabled() );

        // Visibility.
        setVisible( mpMountedTo->getVisible() );

        // Flip.
        setFlip( mpMountedTo->getFlipX(), mpMountedTo->getFlipY() );

        // Paused.
        setPaused( mpMountedTo->getPaused() );
    }

    // Update Container Configuration (not mount-nodes).
    updateSpatialConfig( false );

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateMount
#endif
}


//-----------------------------------------------------------------------------
// Update Spatial Configuration.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateSpatialConfig( const bool updateMountNodes )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_updateSpatialConfig);
#endif

    // Calculate Object Mount Nodes (if selected)
    if ( updateMountNodes )
    {
        calculateMountNodes( getPosition() );
    }

    // Ignore if not in scenegraph or disabled!
    if ( !getSpatialDirty() || !isEnabled() || !getSceneGraph()  )
    {
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateSpatialConfig
#endif
        // Finish Here.
        return;
    }

    // Calculate Pivot Offset.
    const t2dVector size = getSize();
    const t2dVector halfSize = getHalfSize();
    const t2dVector pivotOffset = -halfSize;

    // Calculate Local Boundary.
    mLocalClipBoundary[0] = pivotOffset;
    mLocalClipBoundary[1] = pivotOffset + t2dVector(size.mX, 0);
    mLocalClipBoundary[2] = pivotOffset + size;
    mLocalClipBoundary[3] = pivotOffset + t2dVector(0, size.mY);

    // Flip X?
    if ( mFlipX )
    {
        mLocalClipBoundary[0].swap( mLocalClipBoundary[1] );
        mLocalClipBoundary[2].swap( mLocalClipBoundary[3] );
    }

    // Flip Y?
    if ( mFlipY )
    {
        mLocalClipBoundary[0].swap( mLocalClipBoundary[3] );
        mLocalClipBoundary[1].swap( mLocalClipBoundary[2] );
    }

    // Calculate Local Clip Rectangles.
    mLocalClipRectangle.point.set( mLocalClipBoundary[0].mX, mLocalClipBoundary[0].mY );
    mLocalClipRectangle.extent.set( mLocalClipBoundary[2].mX-mLocalClipBoundary[0].mX, mLocalClipBoundary[2].mY-mLocalClipBoundary[0].mY );

    // Update World Clip.
    updateWorldClip( getPosition() );

    // Calculate World Collision Clip Rectangle.
    mWorldCollisionClipRectangle = mWorldClipRectangle;

    // Is the object using a superscribed circle?
    t2dPhysics::eCollisionDetection collisionMode = getCollisionDetectionMode();
    if ( (collisionMode == t2dPhysics::T2D_DETECTION_FULL || collisionMode == t2dPhysics::T2D_DETECTION_CIRCLE_ONLY) )
    {
        // Fetch Collision Circle Diameter.
        const F32 circleDiameter = getCollisionCircleRadii() * 2.0f;
        
        // Adjust Search Rectangle.
        if ( mWorldCollisionClipRectangle.extent.x < circleDiameter )
        {
            // Calculate Difference.
            F32 deltaX = circleDiameter - mWorldCollisionClipRectangle.extent.x;
            // Expand collision clip-rectangle to be this size.
            mWorldCollisionClipRectangle.point.x -= deltaX*0.5f;
            mWorldCollisionClipRectangle.extent.x += deltaX;
        }
        if ( mWorldCollisionClipRectangle.extent.y < circleDiameter )
        {
            // Calculate Difference.
            F32 deltaY = circleDiameter - mWorldCollisionClipRectangle.extent.y;
            // Expand collision clip-rectangle to be this size.
            mWorldCollisionClipRectangle.point.y -= deltaY*0.5f;
            mWorldCollisionClipRectangle.extent.y += deltaY;
        }
    }

    // Check Scene Object Bins.
    getSceneGraph()->checkSceneObjectBins( this );

    // Reset Spatial Dirty.
    resetSpatialDirty();

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateSpatialConfig
#endif
}


//-----------------------------------------------------------------------------
// Update World Clip.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateWorldClip( const t2dVector& position )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_updateWorldClip);
#endif

	// RKS: THIS IS HERE TO TEST WHY THE WORLD CLIP FOR THE PATH GRID IS MESSED UP!
	//if( getName() != 0 && strcmp( "PathGrid", getName() ) == 0 )
	//{
	//	printf( "---------------\n" );
	//	printf( "BEFORE - mWorldClipBoundary[0]: (%f, %f)\n", mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY );
	//	printf( " - Position: (%f, %f)\n", getPosition().mX, getPosition().mY );
	//}
	
    // Calculate World Boundary.
    transformRectangle2D( getRotationMatrix(), mLocalClipBoundary, position, mWorldClipBoundary );
	
	// RKS: THIS IS HERE TO TEST WHY THE WORLD CLIP FOR THE PATH GRID IS MESSED UP!
	//if( getName() != 0 && strcmp( "PathGrid", getName() ) == 0 )
	//{
	//	printf( "AFTER -  mWorldClipBoundary[0]: (%f, %f)\n", mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY );
	//	printf( " - Position: (%f, %f)\n", getPosition().mX, getPosition().mY );
	//	printf( "---------------\n" );
	//	
	//	if( (S32)(mWorldClipBoundary[0].mX) == -160 )
	//	{
	//		int aaaa = 0;
	//		aaaa++;
	//	}
	//}

    // Calculate World Clip Rectangle Extents.
    mWorldClipLeft      = getMin( getMin( mWorldClipBoundary[0].mX, mWorldClipBoundary[1].mX ), getMin( mWorldClipBoundary[2].mX, mWorldClipBoundary[3].mX ) );
    mWorldClipRight     = getMax( getMax( mWorldClipBoundary[0].mX, mWorldClipBoundary[1].mX ), getMax( mWorldClipBoundary[2].mX, mWorldClipBoundary[3].mX ) );
    mWorldClipTop       = getMin( getMin( mWorldClipBoundary[0].mY, mWorldClipBoundary[1].mY ), getMin( mWorldClipBoundary[2].mY, mWorldClipBoundary[3].mY ) );
    mWorldClipBottom    = getMax( getMax( mWorldClipBoundary[0].mY, mWorldClipBoundary[1].mY ), getMax( mWorldClipBoundary[2].mY, mWorldClipBoundary[3].mY ) );

    // Calculate World Clip Rectangle ( this'll be non-axis-aligned ).
    mWorldClipRectangle.point.set( mWorldClipLeft, mWorldClipTop );
    mWorldClipRectangle.extent.set( mWorldClipRight-mWorldClipLeft, mWorldClipBottom-mWorldClipTop );

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateWorldClip
#endif
}



//-----------------------------------------------------------------------------
// Link Before Selected Object.
//-----------------------------------------------------------------------------
void t2dSceneObject::processLinkBefore( t2dSceneObject* pSceneObject2D )
{
    mpNextProcess       = pSceneObject2D;
    mpPreviousProcess   = pSceneObject2D->mpPreviousProcess;
    pSceneObject2D->mpPreviousProcess = this;
    mpPreviousProcess->mpNextProcess = this;
}


//-----------------------------------------------------------------------------
// Link After Selected Object.
//-----------------------------------------------------------------------------
void t2dSceneObject::processLinkAfter( t2dSceneObject* pSceneObject2D )
{
    mpNextProcess       = pSceneObject2D->mpNextProcess;
    mpPreviousProcess   = pSceneObject2D;
    pSceneObject2D->mpNextProcess = this;
    mpNextProcess->mpPreviousProcess = this;
}


//-----------------------------------------------------------------------------
// Unlink Selected Object.
//-----------------------------------------------------------------------------
void t2dSceneObject::processUnLink( void )
{
    mpNextProcess->mpPreviousProcess = mpPreviousProcess;
    mpPreviousProcess->mpNextProcess = mpNextProcess;
    processInitialiseLink();
}


//-----------------------------------------------------------------------------
// Add Destroy Notification.
//-----------------------------------------------------------------------------
void t2dSceneObject::addDestroyNotification( t2dSceneObject* pSceneObject2D )
{
    // Search list to see if we're already in it (finish if we are).
    for ( U32 n = 0; n < mDestroyNotifyList.size(); n++ )
    {
        // In the list already?
        if ( mDestroyNotifyList[n].mpSceneObject == pSceneObject2D )
        {
            // Yes, so just bump-up the reference count.
            mDestroyNotifyList[n].mRefCount++;
            // Finish here.
            return;
        }
    }

    // Add Destroy Notification.
    tDestroyNotification notification;
    notification.mpSceneObject = pSceneObject2D;
    notification.mRefCount = 1;

    // Add Notification.
    mDestroyNotifyList.push_back( notification );
}


//-----------------------------------------------------------------------------
// Remove Destroy Notification.
//-----------------------------------------------------------------------------
void t2dSceneObject::removeDestroyNotification( t2dSceneObject* pSceneObject2D )
{
    // Find object in notification list.
    for ( U32 n = 0; n < mDestroyNotifyList.size(); n++ )
    {
        // Our object?
        if ( mDestroyNotifyList[n].mpSceneObject == pSceneObject2D )
        {
            // Yes, so reduce reference count.
            mDestroyNotifyList[n].mRefCount--;
            // Finish Here.
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Process Destroy Notifications.
//-----------------------------------------------------------------------------
void t2dSceneObject::processDestroyNotifications( void )
{
    // Find object in notification list.
    while( mDestroyNotifyList.size() )
    {
        // Fetch Notification Item.
        tDestroyNotification notification = mDestroyNotifyList.first();
        // Only action if we've got a reference active.
        if ( notification.mRefCount > 0 )
            // Call Destroy Notification.
            notification.mpSceneObject->onDestroyNotify( this );

        // Remove it.
        mDestroyNotifyList.pop_front();
    }

    // Sanity!
    AssertFatal( mDestroyNotifyList.size() == 0, "t2dSceneObject::processDestroyNotifications() - Notifications still pending!" );
}


//-----------------------------------------------------------------------------
// Add Camera Mount Reference.
//-----------------------------------------------------------------------------
void t2dSceneObject::addCameraMountReference( t2dSceneWindow* pAttachedCamera )
{
    // Attach Camera.
    mpAttachedCamera = pAttachedCamera;
}


//-----------------------------------------------------------------------------
// Remove Camera Mount Reference.
//-----------------------------------------------------------------------------
void t2dSceneObject::removeCameraMountReference( void )
{
    // Detach Camera.
    mpAttachedCamera = NULL;
}


//-----------------------------------------------------------------------------
// Dismount Specific Object.
//-----------------------------------------------------------------------------
void t2dSceneObject::dismountCamera( void )
{
    // Are we attached to a camera?
    if ( mpAttachedCamera )
    {
        // Yes, so get camera to dismount.
        mpAttachedCamera->dismountMe( this );
    }
}


//-----------------------------------------------------------------------------
// Safe Delete.
//
// NOTE:-   This is safe delete meaning that this will defer the deletion of
//          the object until such time that it's safe.  You can safely use
//          this during normal script operation as well as in the sensitive
//          script callbacks.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, safeDelete, void, 2, 2, "() - Safely deletes object.\n"
																 "@return No return Value.")
{
    // Script Delete.
    object->safeDelete();
}
// Safe Delete.
void t2dSceneObject::safeDelete()
{
    // We cannot delete child objects here.
    if ( getIsChild() )
        return;

    // Are we in a scenegraph?
    if ( getSceneGraph() )
    {
        // Yes, so add a delete-request to the scenegraph.
        getSceneGraph()->addDeleteRequest( this );
    }
    else
    {
		//dPrintf( "safeDeleting: %s, Address: 0x%x\n", getName(), this ); // RKS: Trying to figure out the crash between levels
		
        // No, so use standard SimObject helper.
        deleteObject();
    }
}


//-----------------------------------------------------------------------------
// Update Lifetime.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateLifetime( const F32 elapsedTime )
{
    // Update Lifetime.
    if ( mLifetimeActive )
    {
        // Reduce Lifetime.
        setLifetime( getLifetime() - elapsedTime );

        // Are we now dead?
        if ( mLessThanOrEqual( getLifetime(), 0.0f) )
        {
            // Yes, so reset lifetime.
            setLifetime( 0.0f );

            // Initiate Death!
            safeDelete();
        }
    }
}


//-----------------------------------------------------------------------------
// Get Link Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLinkPoint, const char*, 3, 3, "(integer mountID) - Gets a Link-Point.\n"
                                                               "@return (float worldX / float worldY) The world position at which the link point is.")
{
    // Fetch Mount Node.
    const t2dSceneObject::tMountNode* pMountNode = object->getMountNode( dAtoi(argv[2]) );

    // Reset Link-Point.
    t2dVector linkPoint(0,0);

    // Fetch Link-Point (if the node was valid).
    if ( pMountNode )
    {
        linkPoint = pMountNode->mWorldMountPosition;
    }

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", linkPoint.mX, linkPoint.mY);
    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Link Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getLinkCount, S32, 2, 2, "() - Gets the Link-Point Count.\n"
                                                       "@return (integer linkPointCount) The count of link points.")
{
    // Return Link Count.
    return object->getMountNodeCount();
}

//-----------------------------------------------------------------------------
// Get All Linkpoint IDs
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAllLinkpointIDs, const char*, 2, 2, "() - Gets the linkpoint IDs of the linkpoints on this object.\n"
			  "@return (list mountedChildren) The space-separated list of LinkpointIDs this has")
{
	return object->getAllLinkpointIDs();
}

char* t2dSceneObject::getAllLinkpointIDs()
{
	// Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

	if (mMountNodes.size() == 0)
	{
		bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "");
	}
	else
	{
		for (U32 node = 0; node < mMountNodes.size(); node++)
		{
			bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", mMountNodes[node].mMountID );

			// Finish early if we run out of buffer space.
			if ( bufferCount >= maxBufferSize )
			{
				// Warn.
				Con::warnf("t2dSceneObject::getAllLinkpointIDs() - Too many linkpoint IDs to return to scripts!");
				break;
			}
		}
	}

	return pBuffer;
}

//-----------------------------------------------------------------------------
// Get All Mounted Children.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getAllMountedChildren, const char*, 2, 2, "() - Gets the objects mounted to this object.\n"
			  "@return (list mountedChildren) The space-separated list of objects mounted to this object")
{
	// Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

	Vector<t2dSceneObject *> children;
	object->getAllMountedChildren(&children);

	if (children.size() == 0)
	{
		bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "");
	}
	else
	{
		for (U32 n = 0; n < children.size(); n++)
		{
			bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", children[n]->getId() );

			// Finish early if we run out of buffer space.
			if ( bufferCount >= maxBufferSize )
			{
				// Warn.
				Con::warnf("t2dSceneObject::getAllMountedChildren() - Too many children to return to scripts!");
				break;
			}
		}
	}

	children.clear();

	return pBuffer;
}

void t2dSceneObject::getAllMountedChildren(Vector<t2dSceneObject *> *children)
{
	children->clear();

	// Search list to see if we're already in it (finish if we are).
    for ( U32 n = 0; n < mDestroyNotifyList.size(); n++ )
    {
		//if (mDestroyNotifyList[n].mpSceneObject->mpMountedTo->getId() == this->getId())
		if (mDestroyNotifyList[n].mpSceneObject->mpMountedTo == this)
		{
			children->push_back(mDestroyNotifyList[n].mpSceneObject);
		}
	}
}



//-----------------------------------------------------------------------------
// Get Children of Linkpoint.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getChildrenOfLinkpoint, const char*, 3, 3, "(int linkpoint) - Gets the objects mounted to this object on the specified linkpoint.\n"
			  "@return (list mountedChildren) The space-separated list of objects mounted to this object at that linkpoint")
{
	S32 lp = dAtoi(argv[2]);

	// Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

	Vector<t2dSceneObject *> children;
	object->getAllMountedChildren(&children);

	for (U32 n = 0; n < children.size(); n++)
	{
		// mMountToID seems to be 1 higher than the actual mount-point it's mounted to.
		if ((children[n]->mMountToID) == lp)
		{
			bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", children[n]->getId() );

			// Finish early if we run out of buffer space.
			if ( bufferCount >= maxBufferSize )
			{
				// Warn.
				Con::warnf("t2dSceneObject::getAllMountedChildren() - Too many children to return to scripts!");
				break;
			}
		}
	}
	children.clear();

	if (bufferCount == 0)
	{
		bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "");
	}

	return pBuffer;
}



ConsoleMethod(t2dSceneObject, cloneWithBehaviors, S32, 2, 3,"(bool disable? = false) - Clones the object and its behaviors.\n"
                                                "@param disable Optional argument that controls if the newly created object is enabled. Defaults to true." 
                                                             "@return (newObjectID) The newly cloned object's id if successful, otherwise a 0.")
{
   t2dSceneObject* newObject = object->clone();
   bool disable = false;
    if( argc >= 3 ) 
	{
		disable = dAtob( argv[2] );
	}
	if (newObject && (!disable) )
	{
		newObject->setEnabled(true);
	}
   return newObject ? newObject->getId() : 0;
}

t2dSceneObject* t2dSceneObject::clone()
{
   const char* className = getClassName();
   t2dSceneObject* newObject = static_cast<t2dSceneObject*>(ConsoleObject::create(className));
   if (!newObject)
   {
      Con::errorf("t2dSceneObject::cloneWithBehaviors - Unable to create cloned object.");
      return 0;
   }

   newObject->setModStaticFields(true);
   newObject->setModDynamicFields(true);

   // SRZ 04/08/08: we are not getting the namespaces linked properly at this point.
   // Theoretically, everything should be set by setModStaticFields and setModDynamicFields?
   // if we register right now, the class and superclass fields have not been set properly,
   // therefore the object registers with no additional namespaces.
   // lets's try assigning fields from:
   newObject->assignFieldsFrom(this);

   if (!newObject->registerObject())
   {
      Con::warnf("t2dSceneObject::cloneWithBehaviors() - Unable to register cloned object.");
      delete newObject;
      return 0;
   }

   copyTo(newObject);

   return newObject;
}

void t2dSceneObject::copyTo(SimObject* obj)
{
   Parent::copyTo(obj);

   AssertFatal(dynamic_cast<t2dSceneObject*>(obj), "t2dSceneObject::copyTo - Copy object is not a t2dSceneObject!");
   t2dSceneObject* object = static_cast<t2dSceneObject*>(obj);

   // [neo, 5/11/2007 - #2990]
   // ConfigDatablock
   if( mConfigDataBlock )
   {
      object->mConfigDataBlock = mConfigDataBlock;
      object->mConfigDataBlock->addSceneObjectReference( object );
   }

   // Random
   object->mUseMouseEvents = mUseMouseEvents;
   object->mPaused = mPaused;
   object->mVisible = mVisible;
   object->mLifetime = mLifetime;

   // Spatial
   object->setPosition(getPosition());
   object->setSize(getSize());
   object->setRotation(getRotation());
   object->setAutoRotation(getAutoRotation());
   object->setFlip(getFlipX(), getFlipY());

   object->mSortPoint = mSortPoint;
   object->mLayer = mLayer;
   object->mGraphGroup = mGraphGroup;
   object->mGraphGroupMask = BIT( mGraphGroup );
   object->mMountRotation = mMountRotation;
   object->mAutoMountRotation = mAutoMountRotation;

   // World Limits
   object->setWorldLimit(mWorldLimitMode, mWorldLimitMin, mWorldLimitMax, mWorldLimitCallback);

   // Collision
   object->mCollisionActiveSend = mCollisionActiveSend;
   object->mCollisionActiveReceive = mCollisionActiveReceive;
   object->mCollisionPhysicsSend = mCollisionPhysicsSend;
   object->mCollisionPhysicsReceive = mCollisionPhysicsReceive;
   object->mCollisionGroupMask = mCollisionGroupMask;
   object->mCollisionLayerMask = mCollisionLayerMask;
   object->mCollisionCallback = mCollisionCallback;
   object->mParentPhysics.mCollisionPolyScale = mParentPhysics.mCollisionPolyScale;
   object->mParentPhysics.mCollisionCircleScale = mParentPhysics.mCollisionCircleScale;
   object->mParentPhysics.mCollisionMaxIterations = mParentPhysics.mCollisionMaxIterations;
   object->mParentPhysics.mCollisionDetectionMode = mParentPhysics.mCollisionDetectionMode;
   object->mParentPhysics.mCollisionResponseMode = mParentPhysics.mCollisionResponseMode;
   object->mParentPhysics.mCollisionCircleSuperscribed = mParentPhysics.mCollisionCircleSuperscribed;

   // Blending
   object->mBlending = mBlending;
   object->mSrcBlendFactor = mSrcBlendFactor;
   object->mDstBlendFactor = mDstBlendFactor;
   object->mBlendColour = mBlendColour;

   // Physics
   object->mUsesPhysics = mUsesPhysics;
   object->mParentPhysics.mConstantForce = mParentPhysics.mConstantForce;
   object->mParentPhysics.mGraviticConstantForce = mParentPhysics.mGraviticConstantForce;
   object->mParentPhysics.mForceScale = mParentPhysics.mForceScale;
   object->mParentPhysics.mImmovable = mParentPhysics.mImmovable;
   object->mParentPhysics.mForwardMovementOnly = mParentPhysics.mForwardMovementOnly;
   object->mParentPhysics.mAutoMassInertia = mParentPhysics.mAutoMassInertia;
   object->mParentPhysics.mMass = mParentPhysics.mMass;
   object->mParentPhysics.mInertialMoment = mParentPhysics.mInertialMoment;
   object->mParentPhysics.mDensity = mParentPhysics.mDensity;
   object->mParentPhysics.mCoefDynamicFriction = mParentPhysics.mCoefDynamicFriction;
   object->mParentPhysics.mCoefRestitution = mParentPhysics.mCoefRestitution;
   object->mParentPhysics.mDamping = mParentPhysics.mDamping;
   object->mParentPhysics.mGrossLinearVelocity = mParentPhysics.mGrossLinearVelocity;
   object->mParentPhysics.mGrossAngularVelocity = mParentPhysics.mGrossAngularVelocity;
   object->mParentPhysics.mMinLinearVelocity = mParentPhysics.mMinLinearVelocity;
   object->mParentPhysics.mMaxLinearVelocity = mParentPhysics.mMaxLinearVelocity;
   object->mParentPhysics.mMinAngularVelocity = mParentPhysics.mMinAngularVelocity;
   object->mParentPhysics.mMaxAngularVelocity = mParentPhysics.mMaxAngularVelocity;

   object->mUsesPhysics = mUsesPhysics;

   // Linkpoints
   for ( U32 node = 0; node < mMountNodes.size(); ++node )
   {
	   // Fetch Mount Node.
       const tMountNode& mountNode = mMountNodes[node];
       object->addLinkPoint(mountNode.mLocalMountPosition);
   }

   // Mounting
   object->mMountOffset = mMountOffset;
   object->mMountForce = mMountForce;
   object->mMountTrackRotation = mMountTrackRotation;
   object->mMountOwned = mMountOwned;
   object->mMountInheritAttributes = mMountInheritAttributes;

	if(object->mpSceneGraph) object->mpSceneGraph->removeFromScene(object);
  object->assignDynamicFieldsFrom(this);

   // Behaviors
   S32 count = getBehaviorCount();
   for (S32 i = 0; i < count; i++)
   {
      BehaviorInstance* bInstance = getBehavior(i);
      BehaviorTemplate* bTemplate = bInstance->getTemplate();
      BehaviorInstance* newInstance = bTemplate->createInstance();
      newInstance->assignDynamicFieldsFrom(bInstance);
      object->addBehavior(newInstance, 1);//delay the onBehaviorAdd callback by 1 tick
   }

   // Moved this back after the Behaviors being added, otherwise the onAddToScene callback is never called
   // on the various behaviors since the behaviors didn't exist on the object before they are added
   // we're currently getting multiple adds to scene, it doesn't really make sense to try to add it again
   // need a better way to handle how we trigger two-step adds.
   // can we refactor the addToScene callbacks for behaviors directly?
   // Mark this as WNF in 1.7.3, refactor in later releases SRZ 04/08/08
   // hack is to set the pref to not warn on adds to scene

   t2dSceneGraph* scenegraph = getSceneGraph();
//-Mat may cause double adds
	object->mpSceneGraph = NULL;
	if (scenegraph)
      scenegraph->addToScene(object, false);

   object->updateSpatialConfig();
   object->setCollisionPolyCustom(mParentPhysics.mCollisionPolyBasisList.size(), mParentPhysics.mCollisionPolyBasisList.address());

   // Make sure we trigger the onAdd callback
   //Con::executef(object, 1, "onAdd"); // RKS: I disabled this script call

   // If we're in a scenegraph already, then the addToScene method wouldn't
   // have been able to call this.
   #ifndef TORQUE_TOOLS
      //if (scenegraph)
      //   Con::executef(object, 2, "onAddToScene", scenegraph->getIdString()); // RKS: I disabled this script call
   #endif
}

//-----------------------------------------------------------------------------
// Clone.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, clone, S32, 2, 3, "(bool copyFields? = false) - Clone Object.\n"
                                                "@param copyFields Whether or not to copy the dynamic fields of the object to the newly cloned object."
                                                "@return (integer newObjectID) The newly cloned object's id if successful, otherwise a 0.")
{
    t2dSceneObject* pSceneObject;

    // Fetch "Copy Fields" Flag.
    bool copyFields = false;
    if( argc >= 3 ) copyFields = dAtob( argv[2] );
    
    // Clone Object.
    if ( object->clone( &pSceneObject, copyFields ) )
    {
        // Okay, so return clone.
        return pSceneObject->getId();
    }
    else
    {
        // Error, so return nothing!
        return 0;
    }
}
// Clone.
bool t2dSceneObject::clone( t2dSceneObject** ppSceneObject, const bool bCopyFields )
{
    // Fetch Scenegraph.
    t2dSceneGraph* pSceneGraph = getSceneGraph();

    // Check we're in a SceneGraph.
    if ( !pSceneGraph )
    {
        // Warn.
        Con::warnf("t2dSceneObject::clone() - Cannot Clone Object (%d); not in a scene!", getIdString());
        return false;
    }

    // Get Object Classname.
    const char* className = getClassName();

    // Create Scene Object.
    t2dSceneObject* pSceneObject2D = (t2dSceneObject*)ConsoleObject::create( className );
    // Check Object.
    if ( !pSceneObject2D )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::clone() - Could not create Classname '%s'.", className);
        // Return Error.
        return false;
    }

    // Flag Object as Modifiable 
    pSceneObject2D->setModStaticFields(true);
    pSceneObject2D->setModDynamicFields(true);

    // Add it to Scene Graph ( if not already added ).
    if ( !pSceneObject2D->getSceneGraph() )
    {
        // Add it to the scene.
        pSceneGraph->addToScene( pSceneObject2D );
    }

    // Create our resiable memory stream.
    InfiniteBitStream memStream;

    // Write-out the original objects stream.
    if ( !saveStream( memStream, pSceneGraph->getNextSerialiseID(), 1 ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::clone() - Error saving Clone-stream!");
        // Destroy Object.
        pSceneObject2D->deleteObject();
        // Return Error.
        return false;
    }

    // Scene Objects.
    Vector<t2dSceneObject*> ObjReferenceList;
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( ObjReferenceList );

    // Start Stream from beginning.
    memStream.setPosition( 0 );

    // Read-in the new objects stream.
    if ( !pSceneObject2D->loadStream( memStream, pSceneGraph, ObjReferenceList, true  ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::clone() - Error loading Clone-stream!");
        // Destroy Object.
        pSceneObject2D->deleteObject();
        // Return Error.
        return false;
    }

    // Copy over fields if requested.
    if ( bCopyFields )
       pSceneObject2D->assignFieldsFrom( this );

    // Register Object.
    if ( !pSceneObject2D->registerObject() )
    {
       // Warn.
       Con::warnf("t2dSceneGraph::clone() - Could not register object '%s'", className);
       // Destroy Object.
       delete pSceneObject2D;
       // Return Error.
       return false;
    }

    // Reference new Object.
    *ppSceneObject = pSceneObject2D;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Copy.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, copy, bool, 3, 4, "(t2dSceneObject object, [copyFields? = false]) - Copies one object from another.\n"
                                                "The object being copied to needs to be of the same class as the object being copied from.\n"
                                                "@param object The t2dSceneObject to copy this object to.\n"
                                                "@param copyFields Specifies whether or not the object's dynamic fields should be copied to the new object.\n"
                                                "@return (bool) Whether or not the copy was successful.")
{
    // Find t2dSceneObject Object.
    t2dSceneObject* pSceneObject = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject )
    {
        Con::warnf("t2dSceneObject::copy() - Couldn't find/Invalid object '%s'.", argv[2]);
        return false;
    }

    // Fetch "Copy Fields" Flag.
    bool copyFields = false;
    if( argc >= 3 ) copyFields = dAtob( argv[2] );

    // Return Copy Status.
    return object->copy( pSceneObject, copyFields );
}
// Copy.
bool t2dSceneObject::copy( t2dSceneObject* pSceneObject, const bool bCopyFields )
{
    // Are the objects the same class!
    if ( StringTable->insert(getClassName()) != StringTable->insert(pSceneObject->getClassName()) )
    {
        // No, so Warn.
        Con::warnf("t2dSceneObject::copy() - Cannot Copy; Objects are not the same type! (%s)->(%s)", pSceneObject->getClassName(), getClassName());
        // Return Error.
        return false;
    }

    // Fetch Source Scenegraph.
    t2dSceneGraph* pSrcSceneGraph = pSceneObject->getSceneGraph();
    // Check it's valid.
    if ( !pSrcSceneGraph )
    {
        // No, so Warn.
        Con::warnf("t2dSceneObject::copy() - Cannot Copy Source Object (%d); not in a scene!", pSceneObject->getIdString());
        // Return Error.
        return false;
    }

    // Is destination in a SceneGraph (and it's not the same one as the source?
    if ( getSceneGraph() && getSceneGraph() != pSrcSceneGraph )
        // Yes, so remove from scene.
        getSceneGraph()->removeFromScene( this );

    // Add to the source scene (if it's not the same as the source).
    if ( getSceneGraph() != pSrcSceneGraph )
        pSrcSceneGraph->addToScene( this );

    // Create our resiable memory stream.
    InfiniteBitStream memStream;

    // Write-out the original objects stream.
    if ( !pSceneObject->saveStream( memStream, pSrcSceneGraph->getNextSerialiseID(), 1 ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::copy() - Error saving Copy-stream!");
        // Return Error.
        return false;
    }

    // Scene Objects.
    Vector<t2dSceneObject*> ObjReferenceList;
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( ObjReferenceList );

    // Start Stream from beginning.
    memStream.setPosition( 0 );

    // Read-in the new objects stream.
    if ( !loadStream( memStream, pSrcSceneGraph, ObjReferenceList, true  ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::copy() - Error loading Copy-stream!");
        // Return Error.
        return false;
    }

    // Copy over fields if requested.
    if ( bCopyFields )
       assignFieldsFrom( pSceneObject );

    // Dirty the spatial so this object will be enabled.
    pSceneObject->getParentPhysics().setSpatialDirty();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load Copy.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, loadCopy, bool, 3, 3, "(objectFile$) - Loads a copy of the object from disk.\n"
                                                    "Loads a previously saved object into this one.\n"
                                                    "@param objectFile The filename to load from."
                                                    "@return (bool) Whether or not the loading was successful.")
{
    // Return LoadCopy Status.
    return object->loadCopy( argv[2] );
}
// Load Copy.
bool t2dSceneObject::loadCopy( const char* objectFile )
{
    // Are we in a Scene?
    if ( getSceneGraph() == NULL )
    {
        // No, so warn.
        Con::warnf("t2dSceneObject::loadCopy() - Cannot Load a copy; not in a scene!");
        // Return Error.
        return false;
    }

    // Expand relative paths.
    char buffer[1024];
    if ( objectFile )
        if ( Con::expandScriptFilename( buffer, sizeof( buffer ), objectFile ) )
            objectFile = buffer;

    // Open Copy File.
    Stream* pStream = ResourceManager->openStream( objectFile );
    // Check Stream.
    if ( !pStream )
    {
        // Warn.
        Con::warnf("t2dSceneObject::loadCopy() - Could not Open File '%s' for Copy.", objectFile);
        // Return Error.
        return false;
    }

    // Scene Objects.
    Vector<t2dSceneObject*> ObjReferenceList;

    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( ObjReferenceList );

    // Load Stream.
    if ( !loadStream( *pStream, getSceneGraph(), ObjReferenceList, true  ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::loadCopy() - Error Loading Copy!");
        // Return Error.
        return false;
    }

    // Close Stream.
    ResourceManager->closeStream( pStream );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save Copy.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, saveCopy, bool, 3, 3, "(objectFile$) - Saves a copy of the object to disk.\n"
                                                    "@param objectFile The filename to load from."
                                                    "@return (bool) Whether or not the saving was successful.")
{
    // Return SaveCopy Status.
    return object->saveCopy( argv[2] );
}
// Save Copy.
bool t2dSceneObject::saveCopy( const char* objectFile )
{
    // Are we in a Scene?
    if ( getSceneGraph() == NULL )
    {
        // No, so warn.
        Con::warnf("t2dSceneObject::saveCopy() - Cannot Save a Copy; not in a scene!");
        // Return Error.
        return false;
    }

    // Expand relative paths.
    char buffer[1024];
    if ( objectFile )
        if ( Con::expandToolScriptFilename( buffer, sizeof( buffer ), objectFile ) )
            objectFile = buffer;

    // Open Copy File.
    FileStream fileStream;
    if ( !ResourceManager->openFileForWrite( fileStream, objectFile, FileStream::Write ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::saveCopy() - Could not open File '%s' for Copy.", objectFile);
        // Return Error.
        return false;
    }

    // Save Stream.
    if ( !saveStream( fileStream, getSceneGraph()->getNextSerialiseID(), 1 ) )
    {
        // Warn.
        Con::warnf("t2dSceneObject::saveCopy() - Error Saving Copy!");
        // Return Error.
        return false;
    }

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Start Timer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setTimerOn, void, 3, 3, "(float timePeriod) - Starts a periodic timer for this object.\n"
                                                      "Sets a timer on the object that, when it expires, will cause the object to execute the onTimer() callback.\n"
                                                      "The timer event will continue to occur at regular intervals until setTimerOff() is called.\n"
                                                      "@param timePeriod The period of time, in milliseconds, between each callback.\n"
													"@return No return Value.")
{
    // Is the periodic timer running?
    if ( object->mPeriodicTimerID != 0 )
    {
        // Yes, so cancel it.
        Sim::cancelEvent( object->mPeriodicTimerID );

        // Reset Timer ID.
        object->mPeriodicTimerID = 0;
    }

    // Fetch Time-Delta.
    U32 timeDelta = U32(dAtof(argv[2]));
    // Create Timer Event.
    t2dSceneObjectTimerEvent* pEvent = new t2dSceneObjectTimerEvent( timeDelta );
    // Post Event.
    object->mPeriodicTimerID = Sim::postEvent( object, pEvent, Sim::getCurrentTime() + timeDelta );
}

//-----------------------------------------------------------------------------
// Stop Timer.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setTimerOff, void, 2, 2, "() - Stops the periodic timer for this object.\n"
																 "@return No return Value.")
{
    // Finish if the periodic timer isn't running.
    if ( object->mPeriodicTimerID == 0 )
        return;

    // Cancel It.
    Sim::cancelEvent( object->mPeriodicTimerID );

    // Reset Timer ID.
    object->mPeriodicTimerID = 0;
}

ConsoleMethod(t2dSceneObject, enableUpdateCallback, void, 2, 2, "() - Enables calling of the onUpdate callback\n"
																 "@return No return Value.")
{
   object->enableUpdateCallback();
}

ConsoleMethod(t2dSceneObject, disableUpdateCallback, void, 2, 2, "() - Disables calling of the onUpdate callback\n"
																 "@return No return Value.")
{
   object->disableUpdateCallback();
}


//-----------------------------------------------------------------------------
// Set Use Mouse Events.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setUseMouseEvents, void, 3, 3, "(bool mouseStatus) - Sets whether mouses-events are monitored by this object.\n"
                                                             "The t2dSceneWindow that is attached to the t2dSceneGraph this object is in must have 'use object mouse events' enabled as set in setUseObjectMouseEvents.\n"
                                                             "@param mouseStatus Whether or not to monitor mouse events.\n"
																 "@return No return Value.")
{
    // Set Use Mouse Events.
    object->setUseMouseEvents( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------
// Get Use Mouse Events.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getUseMouseEvents, bool, 2, 2, "() - Gets whether mouses-events are monitored by this object.\n"
                                                             "@return (bool mouseStatus) Whether mouse-events are monitored by this object.")
{
    // Get Use Window Mouse Events.
    return object->getUseMouseEvents();
} 


//-----------------------------------------------------------------------------
// Attach Gui Control.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, attachGui, void, 4, 5, "(guiControl guiObject, t2dSceneWindow window, [sizeControl? = false]) - Attach a GUI Control to the object.\n"
                                                     "@param guiObject The GuiControl to attach.\n"
                                                     "@param window The t2dSceneWindow to bind the GuiControl to.\n"
                                                     "@param sizeControl Whether or not to size the GuiControl to the size of this object.\n"
																 "@return No return Value.")
{
    // Find GuiControl Object.
    GuiControl* pGuiControl = dynamic_cast<GuiControl*>(Sim::findObject(argv[2]));

    // Check for invalid Gui Control.
    if ( !pGuiControl )
    {
        Con::warnf("t2dSceneObject::attachGui() - Couldn't find GuiControl %s!", argv[2]);
        return;
    }

    // Find t2dSceneWindow Object.
    t2dSceneWindow* pSceneWindow = dynamic_cast<t2dSceneWindow*>(Sim::findObject(argv[3]));

    // Check for invalid t2dSceneWindow Object.
    if ( !pSceneWindow )
    {
        Con::warnf("t2dSceneObject::attachGui() - Couldn't find t2dSceneWindow %s!", argv[3]);
        return;
    }

    // Calculate Send to Mount.
    const bool sizeControl = argc >= 5 ? dAtob(argv[4]) : false;

    // Attach Gui Control.
    object->attachGui( pGuiControl, pSceneWindow, sizeControl );
}
// Attach Gui Control.
void t2dSceneObject::attachGui( GuiControl* pGuiControl, t2dSceneWindow* pSceneWindow, const bool sizeControl )
{
    // Attach Gui Control.
    mpAttachedGui = pGuiControl;
    // Attach t2dSceneWindow.
    mpAttachedGuiSceneWindow = pSceneWindow;
    // Set Size Gui Flag.
    mAttachedGuiSizeControl = sizeControl;

    // Register Gui Control/Window References.
    mpAttachedGui->registerReference( (SimObject**)&mpAttachedGui );
    mpAttachedGuiSceneWindow->registerReference( (SimObject**)&mpAttachedGuiSceneWindow );

    // Check/Adjust Parentage.
    if ( mpAttachedGui->getParent() != mpAttachedGuiSceneWindow )
    {
        // Warn.
        //Con::warnf("t2dSceneObject::attachGui() - GuiControl is not a direct-child of t2dSceneGraph; adjusting!");
        // Remove GuiControl from existing parent (if it has one).
        if ( mpAttachedGui->getParent() )
        {
            mpAttachedGui->getParent()->removeObject( mpAttachedGui );
        }

        // Add it to the scene-window.
        mpAttachedGuiSceneWindow->addObject( mpAttachedGui );
    }
    
}


//-----------------------------------------------------------------------------
// Detach Gui Control.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, detachGui, void, 2, 2, "() - Detach any GUI Control.\n"
																 "@return No return Value.")
{
    // Detach Gui Control.
    object->detachGui();
}
// Detach Gui Control.
void t2dSceneObject::detachGui( void )
{
    // Unregister Gui Control Reference.
    if ( mpAttachedGui )
    {
       // [neo, 5/7/2007 - #2997]
       // Changed to UNregisterReference was registerReference which would crash later
       mpAttachedGui->unregisterReference( (SimObject**)&mpAttachedGui );
        mpAttachedGui = NULL;
    }

    // Unregister Gui Control Reference.
    if ( mpAttachedGuiSceneWindow )
    {
        mpAttachedGuiSceneWindow->registerReference( (SimObject**)&mpAttachedGuiSceneWindow );
        mpAttachedGuiSceneWindow = NULL;
    }
}


//-----------------------------------------------------------------------------
// Update Attached GUI.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateAttachedGui( void )
{
    // Finish if either Gui Control or Window is invalid.
    if ( !mpAttachedGui || !mpAttachedGuiSceneWindow )
        return;

    // Ignore if we're not in the scene that the scene-window is attached to.
    if ( getSceneGraph() != mpAttachedGuiSceneWindow->getSceneGraph() )
    {
        // Warn.
        Con::warnf("t2dSceneObject::updateAttachedGui() - t2dSceneWindow is not attached to my t2dSceneGraph!");
        // Detach from GUI Control.
        detachGui();
        // Finish Here.
        return;
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_updateAttachedGui);
#endif

    // Calculate the GUI Controls' dimensions.
    Point2I topLeftI, extentI;

    // Size Control?
    if ( mAttachedGuiSizeControl )
    {
        // Yes, so fetch Clip Rectangle; this forms the area we want to fix the Gui-Control to.
        const RectF clipRectangle = getWorldClipRectangle();
        // Fetch Top-Left.
        t2dVector topLeft(clipRectangle.point);
        t2dVector bottomRight( clipRectangle.point.x + clipRectangle.extent.x, clipRectangle.point.y + clipRectangle.extent.y );
        // Convert Scene to Window Coordinates.
        mpAttachedGuiSceneWindow->sceneToWindowCoord( topLeft, topLeft );
        mpAttachedGuiSceneWindow->sceneToWindowCoord( bottomRight, bottomRight );
        // Convert Control Dimensions.
        topLeftI.set( S32(topLeft.mX), S32(topLeft.mY) );
        extentI.set( S32(bottomRight.mX-topLeft.mX), S32(bottomRight.mY-topLeft.mY) );
    }
    else
    {
        // No, so center Gui-Control on objects position but don't resize it.

        // Calculate Position from World Clip.
        const RectF clipRectangle = getWorldClipRectangle();
        // Calculate Clip Center.
        const t2dVector centerPosition = clipRectangle.point + t2dVector(clipRectangle.len_x()*0.5f, clipRectangle.len_y()*0.5f);

        // Convert Scene to Window Coordinates.
        t2dVector positionI;
        mpAttachedGuiSceneWindow->sceneToWindowCoord( centerPosition, positionI );
        // Fetch Control Extents (which don't change here).
        extentI = mpAttachedGui->getExtent();
        // Calculate new top-left.
        topLeftI.set( S32(positionI.mX-extentI.x/2), S32(positionI.mY-extentI.y/2) );
    }

    // Set Control Dimensions.
    mpAttachedGui->resize( topLeftI, extentI );

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateAttachedGui
#endif
}

//-----------------------------------------------------------------------------
// Specify Configuration Datablocks.
//-----------------------------------------------------------------------------
void t2dSceneObject::setConfigDatablock( const char* datablockName )
{
   if (mConfigDataBlock)
      mConfigDataBlock->removeSceneObjectReference(this);

   if (!datablockName || !*datablockName)
   {
      mConfigDataBlock = NULL;
      return;
   }

   t2dSceneObjectDatablock *datablock = dynamic_cast<t2dSceneObjectDatablock*>( Sim::findObject( datablockName ) );
   if (datablock != NULL)
   {
      // Transfer the fields.
      // First the static fields.
      const AbstractClassRep::FieldList &list = getFieldList();
      for (S32 i = 0; i < list.size(); i++)
      {
         const AbstractClassRep::Field* f = &list[i];

         // Ignore non data fields.
         if ((f->type == AbstractClassRep::DepricatedFieldType) ||
		       (f->type == AbstractClassRep::StartGroupFieldType) ||
		       (f->type == AbstractClassRep::EndGroupFieldType)) continue;

         // Grab each field in the array
         for(S32 j = 0; j < f->elementCount; j++)
         {
            char index[8];
#ifdef PUAP_OPTIMIZE
			dItoa( j, index );//-Mat put itoa instead of dsprintf 
#else
			dSprintf(index, 8, "%d", j);
#endif
            const char* array = NULL;
            if(f->elementCount > 1)
               array = index;
            
            if (dStricmp(f->pFieldname, StringTable->insert("CollisionPolyList")) == 0)
               S32 y = 0;

            // Only copy over fields different from the default.
            const char* tempValue = datablock->getDataField(f->pFieldname, array);
            if (!tempValue || !*tempValue)
               continue;

            // Need to make a copy of getValue since getDataField uses the same buffer for all its returns.
            char* value = new char[dStrlen(tempValue) + 1];
            dStrcpy(value, tempValue);

            if (!getDefaultConfig() || (dStricmp(getDefaultConfig()->getDataField(f->pFieldname, array), value) != 0))
               setDataField(f->pFieldname, array, value);

            delete [] value;
         }
      }

      // And now copy over dynamic fields.
      SimFieldDictionary* fieldDictionary = datablock->getFieldDictionary();
      if (fieldDictionary)
      {
         for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
         {
            SimFieldDictionary::Entry* entry = *itr;
            setDataField(entry->slotName, NULL, entry->value);
         }
      }

      datablock->addSceneObjectReference(this);
      mConfigDataBlock = datablock;
   }
}

ConsoleMethod( t2dSceneObject, getConfigDatablock, const char*, 2, 2, "() - Gets the configuration datablock this object is using.\n"
                                                                      "@return (configDatablock) The configuration datablock name.")
{
   t2dSceneObjectDatablock *datablock = object->getConfigDatablock();
   if( datablock == NULL )
      return "";

   return ( datablock->getName() != NULL ) ? datablock->getName() : "";
}

ConsoleMethod( t2dSceneObject, setConfigDatablock, void, 2, 3, "(configDatablock) - Applies a configuration datablock to this object.\n"
                                                               "@param configDatablock The configuration datablock name.\n"
																 "@return No return Value.")
{
   if( argc == 2 )
      object->setConfigDatablock( NULL );
   else
   {
      if( dStricmp( argv[2], "NONE" ) == 0 )
         object->setConfigDatablock( NULL );
      else
         object->setConfigDatablock( argv[2] );
   }


}
//-----------------------------------------------------------------------------
// OnAdd.
//-----------------------------------------------------------------------------
bool t2dSceneObject::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // [neo, 5/11/2007]
    // We defer adding of behaviors until after we've been added to the scene graph!
    
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Add to the default scenegraph if it's not already in a scene.
    if (!getSceneGraph() && gDefaultSceneGraph && !getIsChild())
       gDefaultSceneGraph->addToScene(this);

    // [neo, 5/11/2007]
    // Now we can add behaviors
    addBehaviors();

    // Moved physics initialization to constructor. Persistent fields were being overwritten
    // since they are set before the call to onAdd. (ADL)

    // tell the scripts
    //Con::executef(this, 1, "onAdd"); // RKS: I disabled this script call


    // If we're in a scenegraph already, then the addToScene method wouldn't
    // have been able to call this.
#ifndef TORQUE_TOOLS
	// RKS: I disabled this script call
    //if (getSceneGraph())
    //   Con::executef(this, 2, "onAddToScene", getSceneGraph()->getIdString());
#endif
    
    // Return Okay.
    return true;
}

void t2dSceneObject::initializePhysics()
{
    // Initialise Primary Physics.
    getParentPhysics().initialise(this, NULL);

    // Initialise World Limit Physics.
    for ( U32 n = 0; n < 4; n++ )
    {
        // Configure World-Limit Physics.
        mWorldLimitPhysics[n].initialise(this, NULL);
        mWorldLimitPhysics[n].setDensity(0);
        mWorldLimitPhysics[n].setDynamicFriction(0.0f);
        mWorldLimitPhysics[n].setCollisionPolyPrimitive(4);
        mWorldLimitPhysics[n].setSize(t2dVector(maxWorldLimitSize,maxWorldLimitSize));
        mWorldLimitPhysics[n].setCollisionDetectionMode( t2dPhysics::T2D_DETECTION_POLY_ONLY ); // Polygon Only!
        mWorldLimitPhysics[n].setCollisionResponseMode( t2dPhysics::T2D_RESPONSE_OFF ); // No Response for World-Limit!
        
        // Configure Associated World-Limit Collision Status.
        mWorldLimitCollisionStatus[n].mpSrcPhysics = &getParentPhysics();
        mWorldLimitCollisionStatus[n].mpDstPhysics = mWorldLimitPhysics+n;
        mWorldLimitCollisionStatus[n].mSrcObject = NULL;
        mWorldLimitCollisionStatus[n].mDstObject = NULL;
        mWorldLimitCollisionStatus[n].mSrcSolve = true;     // Solve Colliding Object.
        mWorldLimitCollisionStatus[n].mDstSolve = false;    // Don't Solve for World-Limit.
    }

    // Setup World Limit Descriptions.
    mWorldLimitDescription[0] = StringTable->insert("left");
    mWorldLimitDescription[1] = StringTable->insert("right");
    mWorldLimitDescription[2] = StringTable->insert("top");
    mWorldLimitDescription[3] = StringTable->insert("bottom");

    // Clear Potential Collision Object/Status Lists.
    mPotentialCollisionObjectList.clear();
    mPotentialCollisionStatusList.clear();

    // Clear Collision Physics List.
    mCollisionPhysicsList.clear();
    // Default to Global Physics Mode.
    // NOTE:- This list can be overriden by custom objects.
    mCollisionPhysicsList.push_back( &getParentPhysics() );
}

//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dSceneObject::onRemove()
{
    // tell the scripts
    //Con::executef(this, 1, "onRemove"); // RKS: I disabled this script call
	
    // Detach Any GUI Control.
    detachGui();

    // Remove from Scene.
    if ( getSceneGraph() )
        getSceneGraph()->removeFromScene( this );

    // Remove All Link Points.
    removeAllLinkPoints();

    // Sanity.
    AssertFatal( mMountNodes.size() == 0, "t2dSceneObject::onRemove() - Mount-Nodes still exist!" );

    // Call Parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------
// On Destroy Notification.
//-----------------------------------------------------------------------------
void t2dSceneObject::onDestroyNotify( t2dSceneObject* pSceneObject2D )
{
    // Are we mounted to this object?
    if ( processIsMounted() && mpMountedTo == pSceneObject2D )
    {
        // Yes, so schedule a delete if we're owned by the mount.
        if ( mMountOwned )
            safeDelete();

        // Dismount the object for now.
        dismount();
    }
}


//-----------------------------------------------------------------------------
// On Mouse Event.
//-----------------------------------------------------------------------------
void t2dSceneObject::onMouseEvent( const char* name, const GuiEvent& event, const t2dVector& worldMousePosition, const EMouseEvent _eMouseEvent /*= MOUSE_EVENT_NONE*/ )
{
	//if( mMouseLocked == false )
	//{
	//	printf( "t2dSceneObject::onMouseEvent: %s\n", name ); // RKS: See what the mouse event is
	//}
//	if( strcmp( name, "onMouseEnter" ) == 0 )
//	{
//		printf( "t2dSceneObject::onMouseEvent - %s\n", name );
//	}
//	else if( strcmp( name, "onMouseLeave" ) == 0 )
//	{
//		printf( "t2dSceneObject::onMouseEvent - %s\n", name );
//	}
	
//	if( strcmp( name, "onMouseEnter" ) == 0 )
//	{
//		int breakhere = 0;
//		breakhere++;
//	}
//	
//	if( strcmp( name, "onMouseLeave" ) == 0 )
//	{
//		int breakhere = 0;
//		breakhere++;
//	}
	
//	printf( "t2dSceneObject::onMouseEvent; Event: %s;  Object: %s; locked: %i\n", name, getName(), mMouseLocked );
//	if( mMouseLocked == false )
//	{
//		int breakhere = 0;
//		breakhere++;
//	}
	
//	// RKS: I added this so the components can handle mouse events
//	EMouseEvent eMouseEvent;
//	if( strcmp( name, "onMouseDown" ) == 0 )
//	{
//		eMouseEvent = MOUSE_EVENT_DOWN;   
//	}
//	else if (strcmp( name, "onMouseUp" ) == 0 )
//	{
//		eMouseEvent = MOUSE_EVENT_UP;
//	}
//	else if( strcmp( name, "onMouseEnter" ) == 0 )
//	{
//		eMouseEvent = MOUSE_EVENT_ENTER;
//	}
//	else if( strcmp( name, "onMouseLeave" ) == 0 )
//	{
//		eMouseEvent = MOUSE_EVENT_LEAVE;
//	}
//	else
//	{
//		printf( "t2dSceneObject::onMouseEvent - Unhandled mouse event: %s\n", name );
//	}
	
//	void SetLastMouseDownObject( t2dSceneObject* const _pObject ) { m_pLastMouseDownObject = _pObject; }
//	void ClearLastMouseDownObject() { m_pLastMouseDownObject = NULL; }
//	t2dSceneObject* GetLastMouseDownObject() const { return m_pLastMouseDownObject; }
//	
//	void SetLastMouseEnterObject( t2dSceneObject* const _pObject ) { m_pLastMouseEnterObject = _pObject; }
//	void ClearLastMouseEnterObject() { m_pLastMouseEnterObject = NULL; }
//	t2dSceneObject* GetLastMouseEnterObject() const { return m_pLastMouseEnterObject; }
	
	if( mMouseLocked == false )
	{
		static int iCount = 0;
		
		if( _eMouseEvent == MOUSE_EVENT_DOWN )
		{
			// RKS COMPONENTS!
			//printf( "onMouseEvent -- Object: %s;  Event: %s;  TouchingDown: %d\n", getName(), "\tMOUSE_EVENT_DOWN\t", CComponentGlobals::GetInstance().IsTouchingDown() );
			//CComponentGlobals::GetInstance().SetLastMouseDownObject( this );
		}
		else if( _eMouseEvent == MOUSE_EVENT_UP )
		{
			// RKS COMPONENTS!
			//printf( "onMouseEvent -- Object: %s;  Event: %s;  TouchingDown: %d\n", getName(), "\tMOUSE_EVENT_UP\t", CComponentGlobals::GetInstance().IsTouchingDown() );
		}
		else if( _eMouseEvent == MOUSE_EVENT_ENTER )
		{
			// RKS COMPONENTS!
			//CComponentGlobals::GetInstance().SetLastMouseEnterObject( this );
		}
		else if( _eMouseEvent == MOUSE_EVENT_LEAVE )
		{
			// RKS COMPONENTS!
			//CComponentGlobals::GetInstance().ClearLastMouseEnterObject();
		}
		else 
		{
			printf( "--------------------\n" );
		}

		iCount++;
		
		SimComponent* pComponent = NULL;
		U32 iNumComponents = getComponentCount();
		for( S32 i = 0; i < iNumComponents; ++i ) // RKS TODO: ONLY DO THIS FOR NON-MOUSE LOCKED OBJECTS!
		{
			pComponent = getComponent( i );
			if( pComponent )
			{
				pComponent->OnMouseEvent( _eMouseEvent, worldMousePosition );
			}
		}
	}
	
    // Argument Buffers.
    char argBuffer[3][32];

     // Format Event-Modifier Buffer.
    dSprintf(argBuffer[0], 32, "%d", event.modifier);

    // Format Mouse-Position Buffer.
    dSprintf(argBuffer[1], 32, "%f %f", worldMousePosition.mX, worldMousePosition.mY);

    // Format Mouse-Click Count Buffer.
    dSprintf(argBuffer[2], 32, "%d", event.mouseClickCount);

    // Call Scripts.
    //Con::executef(this, 4, name, argBuffer[0], argBuffer[1], argBuffer[2]); // RKS: Comment this out to skip the script onMouseEvent calls
}

//---------------------------------------------------------------------------------------------
// 2D Point Transformation.
//
// Alright, so I could've used a Homegenous coordinate system but I didn't, get over it!
//---------------------------------------------------------------------------------------------
void t2dSceneObject::transformPoint2D( const t2dMatrix& xform, const t2dVector& srcPoint2D, const t2dVector& srcTranslation, t2dVector& dstPoint2D )
{
    dstPoint2D = (srcPoint2D * xform) + srcTranslation;
}

//-----------------------------------------------------------------------------
// 2D Point Inverse Transformation.
//-----------------------------------------------------------------------------
void t2dSceneObject::inverseTransformPoint2D( const t2dMatrix& xform, const t2dVector& srcPoint2D, const t2dVector& srcTranslation, t2dVector& dstPoint2D )
{
    dstPoint2D = ((srcPoint2D - srcTranslation) * xform);
}


//---------------------------------------------------------------------------------------------
// 2D Point Transformation about Arbitrary Point.
//
// Alright, so I could've used a Homegenous coordinate system but I didn't, get over it!
//---------------------------------------------------------------------------------------------
void t2dSceneObject::pivotTransformPoint2D( const t2dMatrix& xform, const t2dVector& srcPoint2D, const t2dVector& pivotPoint, t2dVector& dstPoint2D )
{
    dstPoint2D = ((srcPoint2D - pivotPoint) * xform) + pivotPoint;
}


//---------------------------------------------------------------------------------------------------
// 2D Rectangle Transformation.
//
// The assumption here is that we've got 4 src/destination points (single translation point)
//---------------------------------------------------------------------------------------------------
void t2dSceneObject::transformRectangle2D( const t2dMatrix& xform, const t2dVector* srcPoint2D, const t2dVector& srcTranslation, t2dVector* dstPoint2D )
{
    // Transform Rectangle.
    transformPoint2D( xform, srcPoint2D[0], srcTranslation, dstPoint2D[0] );
    transformPoint2D( xform, srcPoint2D[1], srcTranslation, dstPoint2D[1] );
    transformPoint2D( xform, srcPoint2D[2], srcTranslation, dstPoint2D[2] );
    transformPoint2D( xform, srcPoint2D[3], srcTranslation, dstPoint2D[3] );
}

//---------------------------------------------------------------------------------------------------
// 2D Rectangle Inverse-Transformation.
//
// The assumption here is that we've got 4 src/destination points (single translation point)
//---------------------------------------------------------------------------------------------------
void t2dSceneObject::inverseTransformRectangle2D( const t2dMatrix& xform, const t2dVector* srcPoint2D, const t2dVector& srcTranslation, t2dVector* dstPoint2D )
{
    // Transform Rectangle.
    inverseTransformPoint2D( xform, srcPoint2D[0],  srcTranslation, dstPoint2D[0] );
    inverseTransformPoint2D( xform, srcPoint2D[1],  srcTranslation, dstPoint2D[1] );
    inverseTransformPoint2D( xform, srcPoint2D[2],  srcTranslation, dstPoint2D[2] );
    inverseTransformPoint2D( xform, srcPoint2D[3],  srcTranslation, dstPoint2D[3] );
}


//---------------------------------------------------------------------------------------------------
// Line/Rectangle Intersection Test.
//---------------------------------------------------------------------------------------------------
bool t2dSceneObject::pointInRectangle( const t2dVector& point, const t2dVector& rectMin, const t2dVector& rectMax )
{
    // Do the trivial check to see if point is within the rectangle.
    if (    point.mX < getMin(rectMin.mX,rectMax.mX) || point.mX > getMax(rectMin.mX,rectMax.mX) ||
            point.mY < getMin(rectMin.mY,rectMax.mY) || point.mY > getMax(rectMin.mY,rectMax.mY) )
                // We possibly haven't a collision!
                return false;
            else
                // We *must* have a collision!
                return true;
}

//---------------------------------------------------------------------------------------------------
// Line/Rectangle Intersection Test.
//---------------------------------------------------------------------------------------------------
bool t2dSceneObject::lineRectangleIntersect( const t2dVector& startPoint, const t2dVector& endPoint, const t2dVector& rectMin, const t2dVector& rectMax, F32* pTime )
{
    // Reset Times.
    F32 startTime, endTime = 0;
    F32 finishStartTime = 0.0f;
    F32 finishEndTime = 1.0f;

    // Initialise Element Pointers.
    F32 const* pRectMin     = &(rectMin.mX);
    F32 const* pRectMax     = &(rectMax.mX);
    F32 const* pStartPoint  = &(startPoint.mX);
    F32 const* pEndPoint    = &(endPoint.mX);

    // Step through elements.
    for (int i = 0; i < 2; i++)
    {
        if (*pStartPoint < *pEndPoint)
        {
            // Are we outside the bounds?
            if (*pStartPoint > *pRectMax || *pEndPoint < *pRectMin)
                // Yes, so return 'No Collision'.
                return false;

            // Calculate Intercept Times.
            F32 deltaPoint = *pEndPoint - *pStartPoint;
            startTime = (*pStartPoint < *pRectMin) ? (*pRectMin - *pStartPoint) / deltaPoint : 0.0f;
            endTime = (*pEndPoint > *pRectMax) ? (*pRectMax - *pStartPoint) / deltaPoint : 1.0f;
        }
        else
        {
            // Are we outside the bounds?
            if (*pEndPoint > *pRectMax || *pStartPoint < *pRectMin)
                // Yes, so return 'No Collision'.
                return false;

            // Calculate Intercept Times.
            F32 deltaPoint = *pEndPoint - *pStartPoint;
            startTime = (*pStartPoint > *pRectMax) ? (*pRectMax - *pStartPoint) / deltaPoint : 0.0f;
            endTime = (*pEndPoint < *pRectMin) ? (*pRectMin - *pStartPoint) / deltaPoint : 1.0f;
        }

        // Bound our times.
        if (startTime > finishStartTime) finishStartTime = startTime;
        if (endTime < finishEndTime) finishEndTime = endTime;

            // Are we outside the bounds?
        if (finishEndTime < finishStartTime)
            // Yes, so return 'No Collision'.
            return false;

        // Next Element.
        pRectMin++; pRectMax++;
        pStartPoint++; pEndPoint++;
    }

    // Store Finish Start Time ( if requested )
    if ( pTime )
        *pTime = finishStartTime;

    // Return 'Collision'.
    return true;
}


//-----------------------------------------------------------------------------
// Cast Collision.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, castCollision, const char*, 3, 3, "(float elapsedTime) - Returns collision details for object over elapsed-time.\n"
                                                                "@param elapsedTime The number of seconds into the future to check for collisions. If this number is 0.0, the collision will be done based on the current position of all objects. If it is greater than 0.0, the engine will perform the collision detection across a simulated time period of length %time and return the first collision detected.\n"
                                                                "@return (space seperated list) If no collision occurs, the return value is an empty string. Otherwise, the list is in the form 'object time contactX contactY normalX normalY'. 'object' is the object collided with, 'time' is the number of seconds in the future the collision took place as a float (this number will always be less than the %time parameter), 'contact' is the position the objects collided at as a vector, and 'normal' is the normal of the collision as a vector.")
{
    // Send Collision Status.
    t2dPhysics::cCollisionStatus sendCollisionStatus;

    // Collision?
    if ( !object->castCollision( dAtof(argv[2]), &sendCollisionStatus ) )
        // No, so finish.
        return NULL;
        
    // Calculate our Collision Position...
    t2dVector collisionPosition;

    // Collision in forward-time?
    if ( mGreaterThanZero(sendCollisionStatus.mCollisionTimeReal) )
    {
        // Yes, so extrapolate position.
        // [neo, 5/7/2007 - #2980]
        // We want real time not normalized time so we get total distance to collision point
        // Changed sendCollisionStatus.mCollisionTimeNorm to mCollisionTimeReal
        collisionPosition = object->getPosition() + object->getLinearVelocity() * sendCollisionStatus.mCollisionTimeReal;
    }
    else
    {
        // No, so for now, let's just pass the current position.
        collisionPosition = object->getPosition();
    }

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(128);
    // Format Buffer.
    dSprintf(pBuffer, 128, "%d %f %f %f %f %f", sendCollisionStatus.mDstObject->getId(),
                                                sendCollisionStatus.mCollisionTimeReal,
                                                collisionPosition.mX, collisionPosition.mY,
                                                sendCollisionStatus.mCollisionNormal.mX, sendCollisionStatus.mCollisionNormal.mY );

    // Return buffer.
    return pBuffer;
}
// Cast Collision.
bool t2dSceneObject::castCollision( const F32 elapsedTime, t2dPhysics::cCollisionStatus* pCollisionStatus )
{
	if (!mpSceneGraph) return false;

    // Reset Collision Validity.
    pCollisionStatus->mValidCollision = false;

    // Clear Potential Collision Object Lists.
    mPotentialCollisionObjectList.clear();

    // Find Potential Collision Lists for Collisions.
    RectF rectangleSearched;
    if ( mpSceneGraph->getSceneContainer().findPotentialCollisions( this, elapsedTime, findObjectsCallback, &mPotentialCollisionObjectList, rectangleSearched ) == 0 )
        return false;

    // Reset Collision Detected Flag.
    bool collisionDetected = false;

    // Clear Potential Collision Status List.
    mPotentialCollisionStatusList.clear();

    // Send Collision Status.
    t2dPhysics::cCollisionStatus sendCollisionStatus;

    // Set Source Object.
    sendCollisionStatus.mSrcObject = this;
    // Set Source Physics.
    sendCollisionStatus.mpSrcPhysics = &getParentPhysics();

    // Step through Potential Collision Objects.
    for (   typeSceneObjectVector::iterator potentialCollisionItr = mPotentialCollisionObjectList.begin();
            potentialCollisionItr < mPotentialCollisionObjectList.end();
            potentialCollisionItr++ )
    {
        // Fetch Destination Object.
        t2dSceneObject* pDstObject = *potentialCollisionItr;
        // Set Destination Object.
        sendCollisionStatus.mDstObject = pDstObject;

        // Skip if we are being deleted
        if( pDstObject->isBeingDeleted() )
           continue;

        // Check Object unless it has its collisions suppressed and isn't receiving collisions.
        if ( !pDstObject->getCollisionSuppress() && pDstObject->getCollisionActiveReceive() )
        {
            // Fetch Collision Physics for Destination Object.
            typePhysicsVector& dstPhysicsList = pDstObject->getCollisionAreaPhysics( rectangleSearched );

            // Did we get any physics to potentially collide with?
            if ( dstPhysicsList.size() > 0 )
            {
                // Yes, so iterate Destination Physics.
                for (   typePhysicsVector::iterator dstPhysicsItr = dstPhysicsList.begin();
                        dstPhysicsItr < dstPhysicsList.end();
                        dstPhysicsItr++ )
                {
                    // Set Destination Physics.
                    sendCollisionStatus.mpDstPhysics = *dstPhysicsItr;
                    
                    // Potential collision so do a more comprehensive check.
                    if ( t2dPhysics::calculateCollision( elapsedTime, &sendCollisionStatus ) )
                        mPotentialCollisionStatusList.push_back( sendCollisionStatus );
                }
            }
        }
    }

    // Fetch Collision Count.
    U32 collisionCount = mPotentialCollisionStatusList.size();

    // Were there any collisions?
    if ( collisionCount > 0 )
    {
        // Yes, so note first one.
        F32 firstTime = mPotentialCollisionStatusList[0].mCollisionTimeReal;
        U32 collisionIndex = 0;

        // Do we need to check for quicker collisions?
        if ( collisionCount > 1 )
        {
            // Yes, so find any quicker collisions.
            for ( U32 n = 1; n < collisionCount; n++ )
            {
                // Quicker time?
                if ( mPotentialCollisionStatusList[n].mCollisionTimeReal <= firstTime )
                {
                    // Yes, so note it.
                    firstTime = mPotentialCollisionStatusList[n].mCollisionTimeReal;
                    collisionIndex = n;
                }
            }
        }

        // Copy collision status.
        *pCollisionStatus = mPotentialCollisionStatusList[collisionIndex];

        // Flag Collision Detected.
        collisionDetected = true;
    }

    // Clear Potential Collision Object/Status Lists.
    mPotentialCollisionObjectList.clear();
    mPotentialCollisionStatusList.clear();

    // Return Collision-Detected Status.
    return collisionDetected;
}


//-----------------------------------------------------------------------------
// Cast Collision List.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, castCollisionList, const char*, 3, 3, "(float elapsedTime) - Returns list of objects collided with over elapsed-time.\n"
                                                                    "@param elapstedTime The number of seconds into the future to check for collisions. If this number is 0.0, the collision will be done based on the current position of all objects. If it is greater than 0.0, the engine will perform the collision detection across a simulated time period of length %time and return the first collision detected.\n"
                                                                    "@return (space seperated list) The objects that would be collided with.")
{
    // Send Collision Status.
    t2dPhysics::cCollisionStatus sendCollisionStatus;

    // Collision?
    if ( !object->castCollisionList( dAtof(argv[2]) ) )
        // No, so finish.
        return NULL;
        

    // Fetch Collision Count.
    U32 collisionCount = object->mPotentialCollisionStatusList.size();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < collisionCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", object->mPotentialCollisionStatusList[n].mDstObject->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneObject::castCollisionList() - Too many collisions to return to scripts!");
            break;
        }
    }

    // Clear Potential Collision Status List.
    object->mPotentialCollisionStatusList.clear();

    // Return Buffer.
    return pBuffer;
}
// Cast Collision.
bool t2dSceneObject::castCollisionList( const F32 elapsedTime )
{
    // Clear Potential Collision Object Lists.
    mPotentialCollisionObjectList.clear();

    // Find Potential Collision Lists for Collisions.
    RectF rectangleSearched;
    if ( mpSceneGraph->getSceneContainer().findPotentialCollisions( this, elapsedTime, findObjectsCallback, &mPotentialCollisionObjectList, rectangleSearched ) == 0 )
        return false;

    // Reset Collision Detected Flag.
    // UNUSED: JOSEPH THOMAS -> bool collisionDetected = false;

    // Clear Potential Collision Status List.
    mPotentialCollisionStatusList.clear();

    // Send Collision Status.
    t2dPhysics::cCollisionStatus sendCollisionStatus;

    // Set Source Object.
    sendCollisionStatus.mSrcObject = this;
    // Set Source Physics.
    sendCollisionStatus.mpSrcPhysics = &getParentPhysics();

    // Step through Potential Collision Objects.
    for (   typeSceneObjectVector::iterator potentialCollisionItr = mPotentialCollisionObjectList.begin();
            potentialCollisionItr < mPotentialCollisionObjectList.end();
            potentialCollisionItr++ )
    {
        // Fetch Destination Object.
        t2dSceneObject* pDstObject = *potentialCollisionItr;
        // Set Destination Object.
        sendCollisionStatus.mDstObject = pDstObject;

        // Check Object unless it has its collisions suppressed, isn't receiving collisions or is being deleted.
        if ( !pDstObject->getCollisionSuppress() && pDstObject->getCollisionActiveReceive() )
        {
            // Fetch Collision Physics for Destination Object.
            typePhysicsVector& dstPhysicsList = pDstObject->getCollisionAreaPhysics( rectangleSearched );

            // Did we get any physics to potentially collide with?
            if ( dstPhysicsList.size() > 0 )
            {
                // Yes, so iterate Destination Physics.
                for (   typePhysicsVector::iterator dstPhysicsItr = dstPhysicsList.begin();
                        dstPhysicsItr < dstPhysicsList.end();
                        dstPhysicsItr++ )
                {
                    // Set Destination Physics.
                    sendCollisionStatus.mpDstPhysics = *dstPhysicsItr;
                    
                    // Potential collision so do a more comprehensive check.
                    if ( t2dPhysics::calculateCollision( elapsedTime, &sendCollisionStatus ) )
                    {
                        // Just add it to the list.
                        mPotentialCollisionStatusList.push_back( sendCollisionStatus );
                    }
                }
            }
        }
    }

    // Quick-Sort by the Collision Time (if more than one status entry).
    if ( mPotentialCollisionStatusList.size() > 1 )
    {
        dQsort(mPotentialCollisionStatusList.address(), mPotentialCollisionStatusList.size(), sizeof(t2dPhysics::cCollisionStatus), collisionStatusListCollisionTimeRealSort );
    }

    // Clear Potential Collision Object Lists.
    mPotentialCollisionObjectList.clear();

    // Return Collision-Detected Status.
    return mPotentialCollisionStatusList.size() > 0;
}


//-----------------------------------------------------------------------------
// Check Collision Send.
//-----------------------------------------------------------------------------
bool t2dSceneObject::checkCollisionSend( const F32 simTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_checkCollisionSend);
#endif

    // Collision Detected Flag.
    bool collisionDetected;

    // Set Macro Collision Retry.
    U32 macroCollisionRetry = 2;

    do
    {
        // Initially Invalidate Collision.
        sendCollisionStatus.mValidCollision = false;

        // Reset Handled.
        sendCollisionStatus.mHandled = false;

        // Clear Potential Collision Object Lists.
        mPotentialCollisionObjectList.clear();

        // Find Potential Collision Lists for Collisions.
        RectF rectangleSearched;
        if ( mpSceneGraph->getSceneContainer().findPotentialCollisions( this, simTime, findObjectsCallback, &mPotentialCollisionObjectList, rectangleSearched ) == 0 )
        {
            // T2D Debug Profiling.
            #ifdef TORQUE_ENABLE_PROFILER
                PROFILE_END();   // T2D_t2dSceneObject_checkCollisionSend
            #endif
            // No Potential Collisions!
            return false;
        }

        // Reset Collision Detected Flag.
        collisionDetected = false;

        // Clear Potential Collision Status List.
        mPotentialCollisionStatusList.clear();

        // Set Source Object.
        sendCollisionStatus.mSrcObject = this;
        // Set Source Physics.
        sendCollisionStatus.mpSrcPhysics = &getParentPhysics();

        // Sum Potential Collisions.
        pDebugStats->objectPotentialCollisions += mPotentialCollisionObjectList.size();

        // Reset Collision Index.
        S32 collisionIndex = -1;

        // Step through Potential Collision Objects.
        for (   typeSceneObjectVector::iterator potentialCollisionItr = mPotentialCollisionObjectList.begin();
                potentialCollisionItr < mPotentialCollisionObjectList.end();
                potentialCollisionItr++ )
        {
            // Fetch Destination Object.
            t2dSceneObject* pDstObject = *potentialCollisionItr;
            // Set Destination Object.
            sendCollisionStatus.mDstObject = pDstObject;

            // Check Object unless it has its collisions suppressed, isn't receiving collisions or is being deleted.
            if ( !pDstObject->getCollisionSuppress() && pDstObject->getCollisionActiveReceive() )
            {
                // Fetch Collision Physics for Destination Object.
                typePhysicsVector& dstPhysicsList = pDstObject->getCollisionAreaPhysics( rectangleSearched );

                // Did we get any physics to potentially collide with?
                if ( dstPhysicsList.size() > 0 )
                {
                    // Yes, so iterate Destination Physics.
                    for (   typePhysicsVector::iterator dstPhysicsItr = dstPhysicsList.begin();
                            dstPhysicsItr < dstPhysicsList.end();
                            dstPhysicsItr++ )
                    {
                        // Set Destination Physics.
                        sendCollisionStatus.mpDstPhysics = *dstPhysicsItr;
                    
                        // Potential collision so do a more comprehensive check.
                        if ( t2dPhysics::calculateCollision( simTime, &sendCollisionStatus ) )
					    {
                            // Collision in forward-time so store it.
                            mPotentialCollisionStatusList.push_back( sendCollisionStatus );
					    }
                    }
                }
            }
        }

        // Are we responding to sending physics or disabling physics?
        if ( !getCollisionPhysicsSend() || getNeverSolvePhysics() )
        {
            // Yes, so were there any collisions?
            if ( mPotentialCollisionStatusList.size() > 0 )
            {
                // Yes, so process All Collision Status.
                processAllCollisionStatus( mPotentialCollisionStatusList, pDebugStats );
            }
            // Flag no collisions so we simply do a full time-step.
            collisionDetected = false;
            break;
        }

        // Got a collision selected?
        if ( collisionIndex == -1 )
        {
            // No, so fetch Collision Count.
            const U32 collisionCount = mPotentialCollisionStatusList.size();

            // Were there any collisions?
            if ( collisionCount == 0 )
            {
                // No, so finish.
                break;
            }
            // Yes, so note first one.
            F32 firstTime = mPotentialCollisionStatusList[0].mCollisionTimeReal;
            // Set Collision Index.
            collisionIndex = 0;

            // Do we need to check for quicker collisions?
            if ( collisionCount > 1 )
            {
                // Yes, so find any quicker collisions.
                for ( U32 n = 1; n < collisionCount; n++ )
                {
                    // Quicker time?
                    if ( mPotentialCollisionStatusList[n].mCollisionTimeReal < firstTime )
                    {
                        // Yes, so note it.
                        firstTime = mPotentialCollisionStatusList[n].mCollisionTimeReal;
                        // Set Collision Index.
                        collisionIndex = n;
                    }
                }
            }
        }

        // Flag Collision Detected.
        collisionDetected = true;

        // Set Collision Status.
        sendCollisionStatus = mPotentialCollisionStatusList[collisionIndex];

        // Fetch Source/Destination Objects.
        t2dSceneObject& srcObject = *sendCollisionStatus.mSrcObject;
        t2dSceneObject& dstObject = *sendCollisionStatus.mDstObject;

        // We only need to solve the contacts if we've got physics enabled,
        // we're not mounted rigidly and we're not immovable.
        sendCollisionStatus.mSrcSolve = srcObject.getCollisionPhysicsSend() && !srcObject.processIsMountedRigid() && !srcObject.getImmovable();
        sendCollisionStatus.mDstSolve = dstObject.getCollisionPhysicsReceive() && !dstObject.processIsMountedRigid() && !dstObject.getImmovable();

         //Special-Case here.
         //NOTE:-   If we're using rigid-body then remove macro-collisions by clamping the velocity along the collision normal.
         //         We're only doing this for extremely small collision times!  This is quite obviously a hack as the solution
         //         is to implement a robust contact solver.
        const bool rigidBodySrcSolve = sendCollisionStatus.mSrcSolve && srcObject.getCollisionResponseMode() == t2dPhysics::T2D_RESPONSE_RIGID;
        const bool rigidBodyDstSolve = sendCollisionStatus.mDstSolve && dstObject.getCollisionResponseMode() == t2dPhysics::T2D_RESPONSE_RIGID;

        if (    (rigidBodySrcSolve || rigidBodyDstSolve) &&
                !sendCollisionStatus.mOverlapped &&
                sendCollisionStatus.mCollisionTimeReal == 0.0f )
        {
                // Yes, so clamp gross linear velocity.
                t2dPhysics::resolveClampCollision( rigidBodySrcSolve, rigidBodyDstSolve, &sendCollisionStatus, 0 );
                // Back Away from Collision.
                t2dPhysics::backAway( &sendCollisionStatus );
                // Update Container Configuration (not mount-nodes).
                updateSpatialConfig( false );
                // Retry collision detection.
                continue;
        }

        // Process Collision.
        processCollisionStatus( &sendCollisionStatus, pDebugStats );

        // Collision Processed.
        break;

    } while( --macroCollisionRetry > 0 );

    // Clear Potential Collision Object/Status Lists.
    mPotentialCollisionObjectList.clear();
    mPotentialCollisionStatusList.clear();

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_checkCollisionSend
#endif

    // Return Collision-Detected Status.
    return collisionDetected;
}


//-----------------------------------------------------------------------------
// Process Collision Status.
//-----------------------------------------------------------------------------
void t2dSceneObject::processCollisionStatus( t2dPhysics::cCollisionStatus* pCollisionStatus, CDebugStats* pDebugStats )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_processCollisionStatus);
#endif

    // Monitor Collision/Contact Pairs.
    pDebugStats->objectContactPairs += pCollisionStatus->mContactCount;

    // Fetch Destination Object.
    t2dSceneObject* pSrcObject = pCollisionStatus->mSrcObject;
    t2dSceneObject* pDstObject = pCollisionStatus->mDstObject;

    // Adding Unsolved Contact History Only?
    if ( pCollisionStatus->mpSrcPhysics->mCollectCollisionHistory &&
         pCollisionStatus->mValidCollision &&
         !Con::getBoolVariable( "$pref::T2D::contactHistorySolvedOnly", true ) )
    {
        // Yes, so add Collision History.
        pCollisionStatus->mpSrcPhysics->addCollisionHistory( pCollisionStatus );
    }

    // Solve?
    if ( pCollisionStatus->mSrcSolve || pCollisionStatus->mDstSolve )
    {
        // Yes, so solve collision.
        if (!pCollisionStatus->mSrcObject->getNeverSolvePhysics() && !pCollisionStatus->mDstObject->getNeverSolvePhysics())
        {
            // Increase Actual Collision Debug Count.
            pDebugStats->objectActualCollisions++;
            // Solve Collision.
            t2dPhysics::solveCollision( pCollisionStatus );
        }
    }

    // Check for Script Collision-Callbacks for both Source/Destination.
    // Source...
    if ( pSrcObject->getCollisionCallback() )
    {
        pSrcObject->onScriptCollision( pCollisionStatus );
    }

    // Destination (only if receive collisions are active).
    if ( pDstObject->getCollisionCallback() && Con::getBoolVariable( "$pref::T2D::dualCollisionCallbacks", true ) )
    {
        // Receive Collision Status.
        t2dPhysics::cCollisionStatus receiveCollisionStatus;
        // Reverse Collision Status.
        t2dPhysics::reverseCollisionStatus( pCollisionStatus, &receiveCollisionStatus );

        // Script callback.
        pDstObject->onScriptCollision( &receiveCollisionStatus );
    }

    pSrcObject->onCollision(pCollisionStatus);
    pDstObject->onCollision(pCollisionStatus);

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();  // T2D_t2dSceneObject_processCollisionStatus
#endif
}


//-----------------------------------------------------------------------------
// Process All Collision Status.
//-----------------------------------------------------------------------------
void t2dSceneObject::processAllCollisionStatus( Vector<t2dPhysics::cCollisionStatus>& collisionStatusList, CDebugStats* pDebugStats )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_processAllCollisionStatus);
#endif

    // Quick-Sort by the Collision Time (if more than one status entry).
    if ( collisionStatusList.size() > 1 )
    {
        dQsort(collisionStatusList.address(), collisionStatusList.size(), sizeof(t2dPhysics::cCollisionStatus), collisionStatusListCollisionTimeRealSort );
    }

    // Fetch Collision Count.
    const U32 collisionCount = collisionStatusList.size();

    // Process all collisions.
    for ( int n = 0; n < collisionCount; ++n )
    {
        // Fetch Collision Status.
        t2dPhysics::cCollisionStatus* pCollisionStatus = &(collisionStatusList[n]);

        // Fetch Destination Object.
        t2dSceneObject* pSrcObject = pCollisionStatus->mSrcObject;
        t2dSceneObject* pDstObject = pCollisionStatus->mDstObject;

        // Source is never solved here.
        pCollisionStatus->mSrcSolve = false;

        // We only need to solve the contacts if we've got physics enabled,
        // we're not mounted rigidly and we're not immovable.
        pCollisionStatus->mDstSolve = pDstObject->getCollisionPhysicsReceive() && !pDstObject->processIsMountedRigid() && !pDstObject->getImmovable();

        // Monitor Collision/Contact Pairs.
        pDebugStats->objectContactPairs += pCollisionStatus->mContactCount;

        // Adding Unsolved Contact History Only?
        if ( pCollisionStatus->mpSrcPhysics->mCollectCollisionHistory &&
             pCollisionStatus->mValidCollision &&
             !Con::getBoolVariable( "$pref::T2D::contactHistorySolvedOnly", true ) )
        {
            // Yes, so add Collision History.
            pCollisionStatus->mpSrcPhysics->addCollisionHistory( pCollisionStatus );
        }

        // Solve?
        if ( pCollisionStatus->mDstSolve )
        {
            // Yes, so solve collision.
            if (!pCollisionStatus->mSrcObject->getNeverSolvePhysics() && !pCollisionStatus->mDstObject->getNeverSolvePhysics())
            {
                // Increase Actual Collision Debug Count.
                pDebugStats->objectActualCollisions += collisionCount;
                // Solve Collision.
                t2dPhysics::solveCollision( pCollisionStatus );
            }
        }

        // Check for Script Collision-Callbacks for both Source/Destination.
        // Source...
        if ( pSrcObject->getCollisionCallback() )
        {
            pSrcObject->onScriptCollision( pCollisionStatus );
        }

        // Destination (only if receive collisions are active).
        if ( pDstObject->getCollisionCallback() && Con::getBoolVariable( "$pref::T2D::dualCollisionCallbacks", true ) )
        {
            // Receive Collision Status.
            t2dPhysics::cCollisionStatus receiveCollisionStatus;
            // Reverse Collision Status.
            t2dPhysics::reverseCollisionStatus( pCollisionStatus, &receiveCollisionStatus );

            // Script callback.
            pDstObject->onScriptCollision( &receiveCollisionStatus );
        }

        pSrcObject->onCollision(pCollisionStatus);
        pDstObject->onCollision(pCollisionStatus);
    }

    // T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();  // T2D_t2dSceneObject_processAllCollisionStatus
#endif
}


//-----------------------------------------------------------------------------
// Custom Collision Detection Callback.
//
// Use this to do custom collision-detection.
//
// NOTE:-   The collision-detection mode must be set to "CUSTOM" for this
//          callback to occur.  See fixed-detection modes for procedure to
//          populate the collision-status.
//-----------------------------------------------------------------------------
bool t2dSceneObject::onCustomCollisionDetection( const F32 elapsedTime, t2dPhysics::cCollisionStatus* pCollisionStatus )
{
    // Default custom-detection detects nothing!
    return false;
}


//-----------------------------------------------------------------------------
// Custom Collision Response Callback.
//
// Use this to do custom response when a collision occurs.
//
// NOTE:-   The collision-response mode must be set to "CUSTOM" for this
//          callback to occur.
//-----------------------------------------------------------------------------
void t2dSceneObject::onCustomCollisionResponse( t2dPhysics::cCollisionStatus* pCollisionStatus )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;
}


//-----------------------------------------------------------------------------
// On Script Collision.
//
// The default collision callback returns the following information:-
//
// - Source Object ID
// - Destination Object ID
// - Collision Normal
// - Collision Time
// - Contact Count
// - Contact Points as string ( x Contact Count Pairs )
//-----------------------------------------------------------------------------
void t2dSceneObject::onScriptCollision( const t2dPhysics::cCollisionStatus* pCollisionStatus )
{
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneObject_onScriptCollision);
#endif
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( 1 ) // RKS - Script Collision Handled here
	{
		HandleScriptCollision( const_cast<t2dPhysics::cCollisionStatus*>(pCollisionStatus) ); // RKS:  This is here so we can bypass the "onCollision" script call
		return;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

    // Argument Buffers.
    char argBuffer[5][64];

     // Format Parent ID Buffer.
    dSprintf(argBuffer[0], 64, "%d", pCollisionStatus->mpDstPhysics->getRefParentId());

    // Format Collision-Time Buffer.
    dSprintf(argBuffer[1], 64, "%f", pCollisionStatus->mCollisionTimeReal);

    // Format Collision Normal Buffer.
    dSprintf(argBuffer[2], 64, "%f %f", pCollisionStatus->mCollisionNormal.mX, pCollisionStatus->mCollisionNormal.mY);

    // Format Contact Count Buffer.
    dSprintf(argBuffer[3], 64, "%d", pCollisionStatus->mContactCount);

    // Format Contact Points.
    // NOTE:-   Let's rig this to only return the currently supported two contact points to make this
    //          easier; god knows something needs to be!
    if ( pCollisionStatus->mContactCount == 1 )
        dSprintf(argBuffer[4], 64, "%f %f", pCollisionStatus->mSrcContacts[0].mX, pCollisionStatus->mSrcContacts[0].mY);
    else if ( pCollisionStatus->mContactCount == 2 )
        dSprintf(argBuffer[4], 64, "%f %f %f %f", pCollisionStatus->mSrcContacts[0].mX, pCollisionStatus->mSrcContacts[0].mY, pCollisionStatus->mSrcContacts[1].mX, pCollisionStatus->mSrcContacts[1].mY);

    // Report Collision.
    // NOTE: The source object-id is not passed as this is already taken care of by the engine implicitly.
    Con::executef( this, 8, "onCollision",  argBuffer[0],
                                            pCollisionStatus->mpSrcPhysics->getRefMetaString(),
                                            pCollisionStatus->mpDstPhysics->getRefMetaString(),
                                            argBuffer[1],
                                            argBuffer[2],
                                            argBuffer[3],
                                            argBuffer[4] );
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneObject_onScriptCollision
#endif
}


//-----------------------------------------------------------------------------
// Find Serialise Key.
//-----------------------------------------------------------------------------
t2dSceneObject* t2dSceneObject::findSerialiseKey( U32 serialiseKey, Vector<t2dSceneObject*>& ObjReferenceList )
{
    // Search Vector for Serialise Key and return Object.
    for ( U32 n = 0; n < ObjReferenceList.size(); n++ )
        if ( ObjReferenceList[n]->mSerialiseKey == serialiseKey )
            return ObjReferenceList[n];

    // Object not found.
    return NULL;
}


//-----------------------------------------------------------------------------
// Update World Limit.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateWorldLimit( F32 elapsedTime )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_updateWorldLimit);
#endif

    // Check for Limit Violations ( if limits are on ).
    if ( getIsWorldLimitActive() )
    {
        // Reset Collision Status.
        bool collision = false;

        // Check Collision Left.
        if ( mLessThan(mWorldClipLeft, mWorldLimitMin.mX) && t2dPhysics::calculateCollision( elapsedTime, mWorldLimitCollisionStatus ) )
        {
            // Process Collision.
            processWorldLimitCollision( mWorldLimitCollisionStatus, mWorldLimitDescription[0] );
            // Flag Collision.
            collision = true;
        }

        // Check Collision Right.
        if ( mGreaterThan(mWorldClipRight, mWorldLimitMax.mX) && t2dPhysics::calculateCollision( elapsedTime, mWorldLimitCollisionStatus+1 ) )
        {
            // Process Collision.
            processWorldLimitCollision( mWorldLimitCollisionStatus+1, mWorldLimitDescription[1] );
            // Flag Collision.
            collision = true;
        }

        // Check Collision Top.
        if ( mLessThan(mWorldClipTop, mWorldLimitMin.mY) && t2dPhysics::calculateCollision( elapsedTime, mWorldLimitCollisionStatus+2 ) )
        {
            // Process Collision.
            processWorldLimitCollision( mWorldLimitCollisionStatus+2, mWorldLimitDescription[2] );
            // Flag Collision.
            collision = true;
        }

        // Check Collision Bottom.
        if ( mGreaterThan(mWorldClipBottom, mWorldLimitMax.mY) && t2dPhysics::calculateCollision( elapsedTime, mWorldLimitCollisionStatus+3 ) )
        {
            // Process Collision.
            processWorldLimitCollision( mWorldLimitCollisionStatus+3, mWorldLimitDescription[3] );
            // Flag Collision.
            collision = true;
        }

        // Update Container Config if there was a collision.
        if ( collision )
            // Update Container Configuration (not mount-nodes).
            updateSpatialConfig( false );
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateWorldLimit
#endif
}


//-----------------------------------------------------------------------------
// Process World Limit Collision.
//-----------------------------------------------------------------------------
void t2dSceneObject::processWorldLimitCollision(t2dPhysics::cCollisionStatus* pCollisionStatus, StringTableEntry limitDescription )
{
    // We're only interested in overlaps!
    if ( !pCollisionStatus->mOverlapped && mWorldLimitMode != T2D_LIMIT_RIGID )
        return;

    // Execute Callback (if selected).
    if ( mWorldLimitCallback )
    {
        Con::executef( this, 3, "onWorldLimit",
                                getWorldLimitDescription(mWorldLimitMode),
                                limitDescription );
    }

    // Average Contact Points.
    t2dPhysics::averageContactPoints( pCollisionStatus );

    // Action World Limit Violations.
    switch( mWorldLimitMode )
    {
        // Rigid-Body.
        case T2D_LIMIT_RIGID:   
        {
            // Save Collision Response.
            t2dPhysics::eCollisionResponse oldCollisionResponse = getCollisionResponseMode();
            // Set Response to World-Limit Response.
            setCollisionResponse( t2dPhysics::T2D_RESPONSE_RIGID );
            // Solve Overlap (fully).
            t2dPhysics::resolveOverlap( pCollisionStatus );
            // Resolve using selected Collision-Response.
            t2dPhysics::resolveForwardCollision( pCollisionStatus );
            // Restore Collision Response.
            setCollisionResponse( oldCollisionResponse );

        } break;

        // Bounce off world limit.
        case T2D_LIMIT_BOUNCE:
        {
            // Save Collision Response.
            t2dPhysics::eCollisionResponse oldCollisionResponse = getCollisionResponseMode();
            // Set Response to World-Limit Response.
            setCollisionResponse( t2dPhysics::T2D_RESPONSE_BOUNCE );
            // Solve Overlap (fully).
            t2dPhysics::resolveOverlap( pCollisionStatus );
            // Resolve using selected Collision-Response.
            t2dPhysics::resolveForwardCollision( pCollisionStatus );
            // Restore Collision Response.
            setCollisionResponse( oldCollisionResponse );

        } break;

        // Clamp to world limit.
        case T2D_LIMIT_CLAMP:
        {
            // Save Collision Response.
            t2dPhysics::eCollisionResponse oldCollisionResponse = getCollisionResponseMode();
            // Set Response to World-Limit Response.
            setCollisionResponse( t2dPhysics::T2D_RESPONSE_CLAMP );
            // Solve Overlap (fully).
            t2dPhysics::resolveOverlap( pCollisionStatus );
            // Resolve using selected Collision-Response.
            t2dPhysics::resolveForwardCollision( pCollisionStatus );
            // Restore Collision Response.
            setCollisionResponse( oldCollisionResponse );

        } break;

        // Clamp to world limit (kill velocity).
        case T2D_LIMIT_STICKY:
        {
            // Solve Overlap (fully).
            t2dPhysics::resolveOverlap( pCollisionStatus );
            // Stop Object.
            setAtRest();

        } break;

        // Destroy object.
        case T2D_LIMIT_KILL:
        {
            // Do a safe deletion.
            safeDelete();

        } break;
    };
}

//-----------------------------------------------------------------------------
// Set Default World Limit.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, setDefaultWorldLimit, void, 2, 2, "() - Set to the world limit to default values.\n"
																 "@return No return Value.")
{
    object->setDefaultWorldLimit();
}

void t2dSceneObject::setDefaultWorldLimit()
{
    const t2dVector defaultWorldLimitSize = getSize() * 25.0f;
    setWorldLimit( mWorldLimitMode, getPosition() - defaultWorldLimitSize * 0.5f, getPosition() + defaultWorldLimitSize * 0.5f, mWorldLimitCallback);
}


//-----------------------------------------------------------------------------
// Update Constant Force.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateConstantForce( const F32 elapsedTime )
{
    // Only process Constant-Forces if not mounted rigid!
    if ( !processIsMountedRigid() ) 
    {
        // Fetch Objects' Physics.
        t2dPhysics& objPhysics = getParentPhysics();
        // Get SceneGraph.
        t2dSceneGraph* pSceneGraph = getSceneGraph();

        // Add External Forces ( if applicable ).
        if (    (objPhysics.getIsConstantForceActive() || pSceneGraph->getIsConstantForceActive()) &&
                !objPhysics.getImmovable() &&
                !objPhysics.getPhysicsSuppress() )
        {
            // Fetch Scene Constant Force.
            t2dVector sceneConstantForce = pSceneGraph->getGraviticConstantForceFlag() ? pSceneGraph->getConstantForce()*objPhysics.getMass() : pSceneGraph->getConstantForce();
            // Fetch Object Constant Force.
            t2dVector objectConstantForce = objPhysics.getGraviticConstantForceFlag() ? objPhysics.getConstantForce()*objPhysics.getMass() : objPhysics.getConstantForce();

            // Add Linear Force.
            objPhysics.addNetLinearForce( sceneConstantForce+objectConstantForce );
        }

        // Update Net Velocity.
        objPhysics.updateNetVelocity( elapsedTime );
    }
}


//-----------------------------------------------------------------------------
// Pre-Integrate.
//-----------------------------------------------------------------------------
void t2dSceneObject::preIntegrate( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{

    // ****************************************************
    // Update Objects and prepare for next stage.
    // ****************************************************

    // Only process Constant-Forces if not mounted rigid!
    if ( processIsMountedRigid() ) 
    {
        // Reset Net Velocity.
        getParentPhysics().resetNetVelocity();
    }
}


//-----------------------------------------------------------------------------
// Update Object.
//-----------------------------------------------------------------------------
void t2dSceneObject::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
    // ****************************************************
    // Check and Handle Collisions and finally integrate.
    // ****************************************************

    // Editor Scene?
    const bool editting = getSceneGraph()->getIsEditorScene();

    // Has it been initialised?
    if ( getInitialUpdate() && !editting)
    {
        // Yes, so update Physics.
        updateLocalPhysics( elapsedTime, pDebugStats );
    }

    // Integrate Auto Mount-Rotation.
    if ( mAutoMountRotationActive )
    {
        setMountRotation( getMountRotation() + (getAutoMountRotation() * elapsedTime) );
    }

    // Mounted?
    if ( processIsMounted() )
    {
        // Yes, so update mount.
        // NOTE:-   We'll update the tick position based upon the mount!
        updateMount( elapsedTime );
    }
    else
    {
        // Update Tick Position.
        getParentPhysics().updateTickPosition();
        // Update Tick Rotation.
        getParentPhysics().updateTickRotation();
    }

    // Update World Limit ( if required ).
    if ( getIsWorldLimitActive() && !editting)
    {
        updateWorldLimit( elapsedTime );
    }

    // Flag Initial Update.
    setInitialUpdate();

    // Update Lifetime.
    if ( mLifetimeActive && !editting)
    {
        updateLifetime( elapsedTime );
    }

    // Update Any Attached GUI.
    if ( mpAttachedGui && mpAttachedGuiSceneWindow )
    {
        updateAttachedGui();
    }

    // Update Container Configuration.
    updateSpatialConfig();

    // Are we attached to a camera?
    if ( mpAttachedCamera )
    {
        // Yes, so calculate camera mount.
        mpAttachedCamera->calculateCameraMount( elapsedTime );
    }
}


//-----------------------------------------------------------------------------
// Pre-Integrate.
//-----------------------------------------------------------------------------
void t2dSceneObject::postIntegrate(const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats) // RKS: I commented out all potential calls to the 'onUpdate' script
{
   // [neo, 5/17/2007]
   // Don't want to call it unless a scene graph has been set
#ifdef USE_COMPONENTS
	if( getSceneGraph() )
	{
		//-Mat PUAP adding C++ onUpdate function
		//NOTE: may need to set another option to call component onUpdate but not script
		onUpdate();

		//if( mUpdateCallback ) // RKS: SCRIPT DISABLED
		//{
		//	Con::executef(this, 1, "onUpdate");	
		//}
   }
#else // USE_COMPONENTS
   if( getSceneGraph() && mUpdateCallback )
   {
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneObject_onUpdate);
#endif // TORQUE_ENABLE_PROFILER

		Con::executef(this, 1, "onUpdate");

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneObject_onUpdate
#endif // TORQUE_ENABLE_PROFILER
	}
#endif // USE_COMPONENTS

}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dSceneObject::interpolateTick( const F32 timeDelta )
{
	//%PUAP%   optimizer   only do physics on objects that have physics
	// 1.4.1 - Objects without physics enabled are not initially
	// using correct spatial information. This resulted in new
	// objects being positioned at "0 0" during the first frame.
	// Added getSpatialDirty() to the check
	if( !mUsesPhysics && !getSpatialDirty()) {
		return;
	}

    // Interpolate Tick Position.
    getParentPhysics().interpolateTickPosition( timeDelta );
    // Interpolate Tick Rotation.
    getParentPhysics().interpolateTickRotation( timeDelta );

    // Update World Clip.
    updateWorldClip( getRenderPosition() );

    // Update Any Attached GUI.
    if ( mpAttachedGui && mpAttachedGuiSceneWindow )
    {
        updateAttachedGui();
    }

    // Are we attached to a camera?
    if ( mpAttachedCamera )
    {
        // Yes, so interpolate camera mount.
        mpAttachedCamera->interpolateCameraMount( timeDelta );
    }
};


//-----------------------------------------------------------------------------
// Update Local Physics.
//-----------------------------------------------------------------------------
void t2dSceneObject::updateLocalPhysics( const F32 elapsedTime, CDebugStats* pDebugStats )
{
	//%PUAP%
	if( !mUsesPhysics ) {
		return;
	}

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneObject_updateLocalPhysics);
#endif
   
    // Fetch Object Physics.
    t2dPhysics& objectPhysics = getParentPhysics();

    // Update Constant Force.
    updateConstantForce( elapsedTime );
    // Damp Gross Velocity.
    objectPhysics.dampGrossVelocity( elapsedTime );

    // Set Simulation Time.
    F32 simTime = elapsedTime;

    // Can it actively send collisions?
    if ( getCollisionActiveSend() )
    {
        // Send Collision Status.
        t2dPhysics::cCollisionStatus sendCollisionStatus;

        // Get quick references to collision times.
        const F32& collisionTimeReal = sendCollisionStatus.mCollisionTimeReal = 0.0f;

        // Get Iteration Count.
        U32 maxIterations = getCollisionMaxIterations();
        U32 iterationCount = 0;

        // Rigid-Body Source Object?
        if ( objectPhysics.getCollisionResponseMode() == t2dPhysics::T2D_RESPONSE_RIGID )
        {
            // Yes, so let's clamp the minimum iteration to 3!
            maxIterations = getMax( U32(3), maxIterations );
        }

        // Start Collision-Detection Stepping.
        do
        {
            // Update Container Configuration (not mount-nodes).
            updateSpatialConfig( false );

            // Calculate Sweep time.
            const F32 sweepTime = objectPhysics.checkPositionTarget( simTime );

            // Allow object to send collisions.
            const bool collisionFound = checkCollisionSend( sweepTime, sendCollisionStatus, pDebugStats );

            // Finish if we didn't handle a collision.
            // NOTE:-   If we're immovable, we get to do at least a single collision iteration.
            if ( !collisionFound || !sendCollisionStatus.mHandled || getImmovable() )
            {
                // Update Spatials.
                objectPhysics.updateSpatials( sweepTime );

                // Reset Sim Time.
                simTime = 0.0f;

                break;
            }

            // Process Sim-Time if a disjoint-collision in forward-time.
            if ( !sendCollisionStatus.mOverlapped )
            {
                // Update Spatials.
                objectPhysics.updateSpatials( collisionTimeReal, true );

                // Reduce Sim-Time.
                simTime -= collisionTimeReal;
            }
            else
            {
                // We've just tried to solve an overlap.  If we've been told to do a single iteration only then
                // we're only going to solve the overlap and do nothing else, wasting the intended integration
                // period for this object.  To counteract this, let's ignore our masters this cycle and and try
                // at least one more iteration.
                if ( maxIterations == 1 )
                {
                    maxIterations++;
                }
            }

            // Handle Iteration Counter.
            if ( ++iterationCount == maxIterations )
                break;

        // Finished Sim-Tick?
        } while ( mGreaterThanZero(simTime) );

        // Update Spatials.
        objectPhysics.updateSpatials( simTime, true );
    }
    else
    {
        // Update Spatials.
        objectPhysics.updateSpatials( elapsedTime );
    }

    // Update Container Configuration (not mount-nodes).
    updateSpatialConfig( false );

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneObject_updateLocalPhysics
#endif
}


//-----------------------------------------------------------------------------
// Get Collision Area Physics.
//-----------------------------------------------------------------------------
t2dSceneObject::typePhysicsVector& t2dSceneObject::getCollisionAreaPhysics( const RectF& worldArea )
{
    // Default is to return the parent physics.
    // NOTE:-   We don't need to do any fancy udpate-sequencing here as the parent physics is
    //          kept up to date without the need to check for updation repetition.
    return mCollisionPhysicsList;
}

#ifdef USE_COMPONENTS
void t2dSceneObject::onUpdate() {

	//call onUpdate for each component
	SimComponent *pComponent = NULL;
	for( U32 i = 0; i < this->getComponentCount(); i++ ) {
		pComponent = this->getComponent( i );
		if( pComponent == NULL ) {
			break;
		}
		pComponent->onUpdate();
	}
}

void t2dSceneObject::onAddToScene() {

	//call onUpdate for each component
	SimComponent *pComponent = NULL;
	for( U32 i = 0; i < this->getComponentCount(); i++ ) {
		pComponent = this->getComponent( i );
		if( pComponent == NULL ) {
			break;
		}
		pComponent->onAddToScene();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//virtual void OnPositionTargetReached(); // RKS: I added this. This will be called in t2dPhysics::processPositionTarget to skip the script call with the same name.
void t2dSceneObject::OnPositionTargetReached()
{
	//Con::executef( this, 1, "onPositionTarget" ); // RKS TODO: This should be removed, as it is not necessary because I do not use the scripts for this.
	
	SimComponent* pComponent = NULL;
	S32 iNumComponents = getComponentCount();
	for( S32 i = 0; i < iNumComponents; ++i )
	{
		pComponent = getComponent( i );
		if( pComponent )
		{
			pComponent->HandleOwnerPositionTargetReached();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void t2dSceneObject::OnPostInit()
{
	SimComponent* pComponent = NULL;
	S32 iNumComponents = getComponentCount();
	for( S32 i = 0; i < iNumComponents; ++i )
	{
		pComponent = getComponent( i );
		if( pComponent )
			pComponent->OnPostInit();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // USE_COMPONENTS


//----------------------------------------------------------------------------------------------
// Set Blend Options.
//----------------------------------------------------------------------------------------------
void t2dSceneObject::setBlendOptions( void )
{
    // Set Blend Status.
    if ( mBlending )
    {
        // Enable Blending.
        glEnable( GL_BLEND );
        // Set Blend Function.
        glBlendFunc( mSrcBlendFactor, mDstBlendFactor );
        // Set Colour.
#ifdef TORQUE_OS_IPHONE
        glColor4f(mBlendColour.red,mBlendColour.green,mBlendColour.blue,mBlendColour.alpha );
#else
        glColor4fv( (GLfloat*)&mBlendColour );
#endif
	}
    else
    {
        // Disable Blending.
        glDisable( GL_BLEND );
        // Reset Colour.
        glColor4f(1,1,1,1);
    }
}


//----------------------------------------------------------------------------------------------
// Reset Blend Options.
//----------------------------------------------------------------------------------------------
void t2dSceneObject::resetBlendOptions( void )
{
    // Disable Blending.
    glDisable( GL_BLEND );
    // Reset Colour.
    glColor4f(1,1,1,1);
}


//----------------------------------------------------------------------------------------------
// Render Object.
//
// Only call this if you want the object to be *able* to display the Debug Bounding-Box.
//----------------------------------------------------------------------------------------------

#ifdef TORQUE_OS_IPHONE
void t2dSceneObject::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Get Scene Graph.
    t2dSceneGraph*  pT2DSceneGraph = getSceneGraph();

    // Cannot do anything without SceneGraph!
    if ( !pT2DSceneGraph ) {
        return;
	}

    // Get merged Local/Scene Debug Mask.
    U32 debugMask = getDebugMask() | pT2DSceneGraph->getDebugMask();

    // Finish here if we're not rendering any debug info.
    // NOTE:-   We might as well quit if only BIT#0 is on because this is the debug-info pane that's
    //          only rendered by the scenegraph anyway.
    if ( !debugMask || ( debugMask == BIT(0) ) ) {
        return;
	}

    // Clear blending.
    glDisable( GL_BLEND );

    // Bounding Box Mode.
    if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_BOUNDING_BOXES )
    {
        // Set Colour.
        glColor4f(0,255,255,255);

        // Draw World Boundary.
		
		GLfloat vertexHorizontal[] = {
			(GLfloat)(mWorldClipBoundary[0].mX), (GLfloat)(mWorldClipBoundary[0].mY),
			(GLfloat)(mWorldClipBoundary[1].mX), (GLfloat)(mWorldClipBoundary[1].mY),
			(GLfloat)(mWorldClipBoundary[2].mX), (GLfloat)(mWorldClipBoundary[2].mY),
			(GLfloat)(mWorldClipBoundary[3].mX), (GLfloat)(mWorldClipBoundary[3].mY),
		};
		
		glVertexPointer(2, GL_FLOAT, 0, vertexHorizontal );
		glDrawArrays(GL_LINE_LOOP, 0, 4 );//draw last two
		
		
        // Set Colour.
        glColor4f(0,128,128,255);

        // Calculate Clip Rectangle.
        t2dVector clipRectangle[4];
        clipRectangle[0] = mWorldClipRectangle.point;
        clipRectangle[1] = mWorldClipRectangle.point + t2dVector(mWorldClipRectangle.extent.x, 0);
        clipRectangle[2] = mWorldClipRectangle.point + mWorldClipRectangle.extent;
        clipRectangle[3] = mWorldClipRectangle.point + t2dVector(0, mWorldClipRectangle.extent.y);

        // Draw World Boundary.
		GLfloat vertexVertical[] = {
			(GLfloat)(clipRectangle[0].mX), (GLfloat)(clipRectangle[0].mY),
			(GLfloat)(clipRectangle[1].mX), (GLfloat)(clipRectangle[1].mY),
			(GLfloat)(clipRectangle[2].mX), (GLfloat)(clipRectangle[2].mY),
			(GLfloat)(clipRectangle[3].mX), (GLfloat)(clipRectangle[3].mY),
		};
		
		glVertexPointer(2, GL_FLOAT, 0, vertexVertical );
		glDrawArrays(GL_LINE_LOOP, 0, 4 );//draw last two
    }
	

    // Sort Point Mode.
    if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_SORT_POINTS )
    {
        // Calculate sort point marker size based upon the object size.
        const t2dVector objectSize = getSize() * 0.2f;
        const F32 markerSize = getMin(objectSize.mX, objectSize.mY);

        // Calculate The Sort Point "X". 
        t2dVector sortX[4];
        t2dVector worldPt = getPosition() + t2dVector( mSortPoint.mX  * getHalfSize().mX, mSortPoint.mY  * getHalfSize().mY );
        sortX[0] = worldPt - t2dVector( markerSize, markerSize );
        sortX[1] = worldPt + t2dVector( markerSize, markerSize );
        sortX[2] = worldPt - t2dVector( markerSize, -markerSize );
        sortX[3] = worldPt + t2dVector( markerSize, -markerSize );

        // Set Colour.
        glColor4f(128,0,128,255);

        // Draw Sort Point.
		GLfloat verts[] = {
            (GLfloat)(sortX[0].mX), (GLfloat)(sortX[0].mY),
            (GLfloat)(sortX[1].mX), (GLfloat)(sortX[1].mY),
            (GLfloat)(sortX[2].mX), (GLfloat)(sortX[2].mY),
            (GLfloat)(sortX[3].mX), (GLfloat)(sortX[3].mY),
		};
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, verts);		
		glDrawArrays(GL_LINES, 0, 4);
    }

    // Collision Poly Mode.
    if ( (getCollisionActiveSend() || getCollisionActiveReceive()) && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS) )
    {
        // Set Colour.
        glColor4f(0,255,0, 255);

        // Render Collision Bounds.
        getParentPhysics().renderCollisionBounds();
    }

    // Collision History.
    if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_HISTORY )
    {
        // Ensure we're collecting collision history.
        getParentPhysics().mCollectCollisionHistory = true;

        // Render Collision History.
        getParentPhysics().renderCollisionHistory();
    }
    else
    {
        // Were we collecting collision history?
        if ( getParentPhysics().mCollectCollisionHistory == true )
        {
            // Yes, so ensure we're now *not* collecting collision history.
            getParentPhysics().mCollectCollisionHistory = false;
            // Clear the list.
            getParentPhysics().mCollisionHistory.clear();
        }
    }
	
	
    // Mount Nodes Mode.
    if ( mMountNodes.size() > 0 && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_MOUNT_NODES) )
    {
        // Set Colour.
        glColor4f(255,255,0,255);

        // Calculate Mount-Node Marker size based upon Object Size.
        const t2dVector objectSize = getSize() * 0.2f;
        // Use smallest axi.
        const F32 markerSize = getMin(objectSize.mX, objectSize.mY);

        // Step through Mount Nodes.
        for ( U32 node = 0; node < mMountNodes.size(); ++node )
        {
            // Fetch Mount Node.
            const tMountNode& mountNode = mMountNodes[node];

            // Fetch Mount Node.
            const t2dVector& mountPosition = mountNode.mWorldMountPosition;

            // Draw Collision World Boundary.
			GLfloat verts[] = {
                (GLfloat)( mountPosition.mX - markerSize), (GLfloat)(mountPosition.mY ),
                (GLfloat)( mountPosition.mX + markerSize), (GLfloat)(mountPosition.mY ),
                (GLfloat)( mountPosition.mX), (GLfloat)(mountPosition.mY - markerSize ),
                (GLfloat)( mountPosition.mX), (GLfloat)(mountPosition.mY + markerSize ),
			};
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, verts);		
			glDrawArrays(GL_LINES, 0, 4);
        }
    }


    // Mount Link Mode.
    if (processIsMounted() && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_MOUNT_LINK) )
    {
        // Set Colour.
        glColor4f(255,0,0,255);

        // Fetch Object Position.
        t2dVector pivotPos = getPosition();
        // Fetch Mount Position.
        const t2dVector& mountPos = mpMountedTo->getMountNode(mMountToID)->mWorldMountPosition;

        // Draw Mount Link.
        GLfloat verts[] = {
            (GLfloat)pivotPos.mX, (GLfloat)pivotPos.mY,
            (GLfloat)mountPos.mX, (GLfloat)mountPos.mY,
		};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, verts);		
		glDrawArrays(GL_LINES, 0, 2);	
        // Set Colour.
        glColor4f(255,0,128,255);

        // Fetch Object Position.
        pivotPos = mpMountedTo->getPosition();

        // Draw Mount Link.
        GLfloat verts2[] = {
            (GLfloat)pivotPos.mX, (GLfloat)pivotPos.mY,
            (GLfloat)mountPos.mX, (GLfloat)mountPos.mY,
		};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, verts2);		
		glDrawArrays(GL_LINES, 0, 2);	
    }

    // World Limit Mode.
    if ( mWorldLimitMode != T2D_LIMIT_OFF && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_WORLD_LIMITS) )
    {
        // Set Colour.
        glColor4f(255,255,255,255);

        // Draw World Limit.
		GLfloat verts[] = {
            (GLfloat)( mWorldLimitMin.mX), (GLfloat)(mWorldLimitMin.mY ),
            (GLfloat)( mWorldLimitMax.mX), (GLfloat)(mWorldLimitMin.mY ),
            (GLfloat)( mWorldLimitMax.mX), (GLfloat)(mWorldLimitMax.mY ),
            (GLfloat)( mWorldLimitMin.mX), (GLfloat)(mWorldLimitMax.mY ),
		};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, verts);		
		glDrawArrays(GL_LINE_LOOP, 0, 4);		
    }
}

#else

void t2dSceneObject::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Get Scene Graph.
    t2dSceneGraph*  pT2DSceneGraph = getSceneGraph();

    // Cannot do anything without SceneGraph!
    if ( !pT2DSceneGraph )
        return;

    // Get merged Local/Scene Debug Mask.
    U32 debugMask = getDebugMask() | pT2DSceneGraph->getDebugMask();

    // Finish here if we're not rendering any debug info.
    // NOTE:-   We might as well quit if only BIT#0 is on because this is the debug-info pane that's
    //          only rendered by the scenegraph anyway.
    if ( !debugMask || ( debugMask == BIT(0) ) )
        return;

    // Clear blending.
    glDisable( GL_BLEND );

    // Bounding Box Mode.
    if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_BOUNDING_BOXES )
    {
        // Set Colour.
        glColor3f(0,255,255);

        // Draw World Boundary.
        glBegin(GL_LINE_LOOP);
            glVertex2fv( (GLfloat*)&(mWorldClipBoundary[0]) );
            glVertex2fv( (GLfloat*)&(mWorldClipBoundary[1]) );
            glVertex2fv( (GLfloat*)&(mWorldClipBoundary[2]) );
            glVertex2fv( (GLfloat*)&(mWorldClipBoundary[3]) );
        glEnd();

        // Set Colour.
        glColor3f(0,128,128);

        // Calculate Clip Rectangle.
        t2dVector clipRectangle[4];
        clipRectangle[0] = mWorldClipRectangle.point;
        clipRectangle[1] = mWorldClipRectangle.point + t2dVector(mWorldClipRectangle.extent.x, 0);
        clipRectangle[2] = mWorldClipRectangle.point + mWorldClipRectangle.extent;
        clipRectangle[3] = mWorldClipRectangle.point + t2dVector(0, mWorldClipRectangle.extent.y);

        // Draw World Boundary.
        glBegin(GL_LINE_LOOP);
            glVertex2fv( (GLfloat*)&(clipRectangle[0]) );
            glVertex2fv( (GLfloat*)&(clipRectangle[1]) );
            glVertex2fv( (GLfloat*)&(clipRectangle[2]) );
            glVertex2fv( (GLfloat*)&(clipRectangle[3]) );
        glEnd();
    }

    // Sort Point Mode.
    if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_SORT_POINTS )
    {
        // Calculate sort point marker size based upon the object size.
        const t2dVector objectSize = getSize() * 0.2f;
        const F32 markerSize = getMin(objectSize.mX, objectSize.mY);

        // Calculate The Sort Point "X". 
        t2dVector sortX[4];
        t2dVector worldPt = getPosition() + t2dVector( mSortPoint.mX  * getHalfSize().mX, mSortPoint.mY  * getHalfSize().mY );
        sortX[0] = worldPt - t2dVector( markerSize, markerSize );
        sortX[1] = worldPt + t2dVector( markerSize, markerSize );
        sortX[2] = worldPt - t2dVector( markerSize, -markerSize );
        sortX[3] = worldPt + t2dVector( markerSize, -markerSize );

        // Set Colour.
        glColor3f(128,0,128);

        // Draw Sort Point.
        glBegin(GL_LINES);
            glVertex2fv( (GLfloat*)&(sortX[0]) );
            glVertex2fv( (GLfloat*)&(sortX[1]) );
            glVertex2fv( (GLfloat*)&(sortX[2]) );
            glVertex2fv( (GLfloat*)&(sortX[3]) );
        glEnd();
    }

    // Collision Poly Mode.
    if ( (getCollisionActiveSend() || getCollisionActiveReceive()) && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS) )
    {
        // Set Colour.
        glColor3f(0,255,0);

        // Render Collision Bounds.
        getParentPhysics().renderCollisionBounds();
    }

    // Collision History.
    if ( debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_COLLISION_HISTORY )
    {
        // Ensure we're collecting collision history.
        getParentPhysics().mCollectCollisionHistory = true;

        // Render Collision History.
        getParentPhysics().renderCollisionHistory();
    }
    else
    {
        // Were we collecting collision history?
        if ( getParentPhysics().mCollectCollisionHistory == true )
        {
            // Yes, so ensure we're now *not* collecting collision history.
            getParentPhysics().mCollectCollisionHistory = false;
            // Clear the list.
            getParentPhysics().mCollisionHistory.clear();
        }
    }

    // Mount Nodes Mode.
    if ( mMountNodes.size() > 0 && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_MOUNT_NODES) )
    {
        // Set Colour.
        glColor3f(255,255,0);

        // Calculate Mount-Node Marker size based upon Object Size.
        const t2dVector objectSize = getSize() * 0.2f;
        // Use smallest axi.
        const F32 markerSize = getMin(objectSize.mX, objectSize.mY);

        // Step through Mount Nodes.
        for ( U32 node = 0; node < mMountNodes.size(); ++node )
        {
            // Fetch Mount Node.
            const tMountNode& mountNode = mMountNodes[node];

            // Fetch Mount Node.
            const t2dVector& mountPosition = mountNode.mWorldMountPosition;

            // Draw Collision World Boundary.
            glBegin(GL_LINES);
                glVertex2f( mountPosition.mX - markerSize, mountPosition.mY );
                glVertex2f( mountPosition.mX + markerSize, mountPosition.mY );
                glVertex2f( mountPosition.mX, mountPosition.mY - markerSize );
                glVertex2f( mountPosition.mX, mountPosition.mY + markerSize );
            glEnd();
        }
    }

    // Mount Link Mode.
    if ( processIsMounted() && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_MOUNT_LINK) )
    {
        // Set Colour.
        glColor3f(255,0,0);

        // Fetch Object Position.
        t2dVector pivotPos = getPosition();
        // Fetch Mount Position.
        const t2dVector& mountPos = mpMountedTo->getMountNode(mMountToID)->mWorldMountPosition;

        // Draw Mount Link.
        glBegin(GL_LINES);
            glVertex2fv( (GLfloat*)&pivotPos );
            glVertex2fv( (GLfloat*)&mountPos );
        glEnd();

        // Set Colour.
        glColor3f(255,0,128);

        // Fetch Object Position.
        pivotPos = mpMountedTo->getPosition();

        // Draw Mount Link.
        glBegin(GL_LINES);
            glVertex2fv( (GLfloat*)&pivotPos );
            glVertex2fv( (GLfloat*)&mountPos );
        glEnd();
    }

    // World Limit Mode.
    if ( mWorldLimitMode != T2D_LIMIT_OFF && (debugMask & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_WORLD_LIMITS) )
    {
        // Set Colour.
        glColor3f(255,255,255);

        // Draw World Limit.
        glBegin(GL_LINE_LOOP);
            glVertex2f( mWorldLimitMin.mX, mWorldLimitMin.mY );
            glVertex2f( mWorldLimitMax.mX, mWorldLimitMin.mY );
            glVertex2f( mWorldLimitMax.mX, mWorldLimitMax.mY );
            glVertex2f( mWorldLimitMin.mX, mWorldLimitMax.mY );
        glEnd();
    }
}
#endif


//----------------------------------------------------------------------------------------------
// Network Send.
//
// TO BE IMPLEMENTED!
//----------------------------------------------------------------------------------------------
U32 t2dSceneObject::packUpdate(NetConnection * conn, U32 mask, BitStream *stream)
{
    return 0;
}


//----------------------------------------------------------------------------------------------
// Network Receive.
//
// TO BE IMPLEMENTED!
//----------------------------------------------------------------------------------------------
void t2dSceneObject::unpackUpdate(NetConnection * conn, BitStream *stream)
{
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dSceneObject )
    REGISTER_SERIALISE_VERSION( t2dSceneObject, 8, true )
    REGISTER_SERIALISE_VERSION( t2dSceneObject, 9, false )
    REGISTER_SERIALISE_VERSION( t2dSceneObject, 10, false )
    REGISTER_SERIALISE_VERSION( t2dSceneObject, 11, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_BASE( t2dSceneObject, 11 )


//-----------------------------------------------------------------------------
// Load v8
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dSceneObject, 8 )
{
    bool                    initialUpdate;
    bool                    enabled;
    bool                    visible;
    F32                     mountRotation;
    bool                    flipX;
    bool                    flipY;
    U32                     layer;
    U32                     group;
    F32                     lifetime;
    eWorldLimit             worldLimitMode;
    t2dVector               worldLimitMin;
    t2dVector               worldLimitMax;
    bool                    worldLimitCallback;
    F32                     autoMountRotation;
    bool                    collisionActiveSend;
    bool                    collisionActiveReceive;
    bool                    collisionPhysicsSend;
    bool                    collisionPhysicsReceive;
    U32                     collisionLayerMask;
    U32                     collisionGroupMask;
    bool                    collisionCallback;
    bool                    collisionSuppress;
    bool                    blending;
    U32                     srcBlendFactor;
    U32                     dstBlendFactor;
    ColorF                  blendColour;
    S32                     layerOrder;

    // Core Info.
    if  (   !stream.read( &initialUpdate ) ||
            !stream.read( &enabled ) ||
            !stream.read( &visible ) ||
            !stream.read( &mountRotation ) ||
            !stream.read( &flipX ) ||
            !stream.read( &flipY ) ||
            !stream.read( &layer ) ||
            !stream.read( &group ) ||
            !stream.read( &lifetime ) ||
            !stream.read( (S32*)&worldLimitMode ) ||
            !stream.read( &worldLimitMin.mX ) ||
            !stream.read( &worldLimitMin.mY ) ||
            !stream.read( &worldLimitMax.mX ) ||
            !stream.read( &worldLimitMax.mY ) ||
            !stream.read( &worldLimitCallback ) ||
            !stream.read( &autoMountRotation ) ||
            !stream.read( &collisionActiveSend ) ||
            !stream.read( &collisionActiveReceive ) ||
            !stream.read( &collisionPhysicsSend ) ||
            !stream.read( &collisionPhysicsReceive ) ||
            !stream.read( &collisionLayerMask ) ||
            !stream.read( &collisionGroupMask ) ||
            !stream.read( &collisionCallback ) ||
            !stream.read( &collisionSuppress ) ||
            !stream.read( &blending ) ||
            !stream.read( &srcBlendFactor ) ||
            !stream.read( &dstBlendFactor ) ||
            !stream.read( &blendColour ) ||
            !stream.read( &layerOrder ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
        // Error.
        return false;

    // Set Initial Update Status.
    object->mInitialUpdate = initialUpdate;

    // Set Enabled Status.
    object->setEnabled( enabled );
    // Set Visible Status.
    object->setVisible( visible );
    // Set Mount Rotation.
    object->setMountRotation( mountRotation );
    // Set Flip.
    object->setFlip( flipX, flipY );
    // Layer/Group.
    object->setLayer( layer );
    object->setGraphGroup( group );
    // Set Lifetime.
    object->setLifetime( lifetime );
    // Set World Limit.
    object->setWorldLimit( worldLimitMode, worldLimitMin, worldLimitMax, worldLimitCallback );
    // Set Auto Mount-Rotation.
    object->setAutoMountRotation( autoMountRotation );
    // Set Collisions.
    object->setCollisionActive( collisionActiveSend, collisionActiveReceive );
    object->setCollisionPhysics( collisionPhysicsSend, collisionPhysicsReceive );
    object->setCollisionMasks( collisionGroupMask, collisionLayerMask );
    object->setCollisionCallback( collisionCallback );
    object->setCollisionSuppress( collisionSuppress );

    // Set Blending.
    object->setBlending( blending, srcBlendFactor, dstBlendFactor );
    // Set Blend Colour.
    object->setBlendColour( blendColour );

    // Set Layer Order.
    //
    // NOTE:-   we ignore the layer-order if requested.  This is typically done
    //          so that objects that have their own loading/saving support, don't
    //          have their already-set layer-order changed by the load.  This
    //          may not always be the case but the object in question can select
    //          whether it's used or not with this flag.
    if ( !ignoreLayerOrder )
        object->setLayerOrder( layerOrder );


    // Mounting.
    bool                    mountFlag;
    U32                     mountObjectKey;
    t2dVector               mountOffset;
    F32                     mountForce;
    bool                    mountTrackRotation;
    bool                    mountOwned;
    bool                    mountInheritAttributes;

    // Read Mount Flag.
    if ( !stream.read( &mountFlag ) )
        // Return Error.
        return false;

    // Mount Object?
    if ( mountFlag )
    {
        // Read Mount Object Key Key.
        if (    !stream.read( &mountObjectKey ) ||
                !stream.read( &mountOffset.mX ) ||
                !stream.read( &mountOffset.mY ) ||
                !stream.read( &mountForce ) ||
                !stream.read( &mountTrackRotation ) ||
                !stream.read( &mountOwned ) ||
                !stream.read( &mountInheritAttributes ) )
            // Return Error.
            return false;

        // Find Mount Object.
        t2dSceneObject* pMountObject2D = object->findSerialiseKey( mountObjectKey, ObjReferenceList );
        // Check Object.
        if ( !pMountObject2D )
        {
            // Warn.
            // NOTE:-   We're removing this because per object saves cannot work with this.
            //Con::warnf("t2dSceneObject::loadStream - Object was mounted when saved but mount now not available!");
            // Should be Okay!
            return true;
        }

        // Mount Object.
        if ( object->mount( pMountObject2D, mountOffset, mountForce, mountTrackRotation, false, mountOwned, mountInheritAttributes ) < 0 )
            // Error.
            return false;
    }

    // Update Spatial Config.
    object->updateSpatialConfig();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Save v8
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dSceneObject, 8 )
{
    // Core Info.
    if  (   !stream.write( object->mInitialUpdate ) ||
            !stream.write( object->mEnabled ) ||
            !stream.write( object->mVisible ) ||
            !stream.write( object->mMountRotation ) ||
            !stream.write( object->mFlipX ) ||
            !stream.write( object->mFlipY ) ||
            !stream.write( object->mLayer ) ||
            !stream.write( object->mGraphGroup ) ||
            !stream.write( object->mLifetime ) ||
            !stream.write( (S32)object->mWorldLimitMode ) ||
            !stream.write( object->mWorldLimitMin.mX ) ||
            !stream.write( object->mWorldLimitMin.mY ) ||
            !stream.write( object->mWorldLimitMax.mX ) ||
            !stream.write( object->mWorldLimitMax.mY ) ||
            !stream.write( object->mWorldLimitCallback ) ||
            !stream.write( object->mAutoMountRotation ) ||
            !stream.write( object->mCollisionActiveSend ) ||
            !stream.write( object->mCollisionActiveReceive ) ||
            !stream.write( object->mCollisionPhysicsSend ) ||
            !stream.write( object->mCollisionPhysicsReceive ) ||
            !stream.write( object->mCollisionLayerMask ) ||
            !stream.write( object->mCollisionGroupMask ) ||
            !stream.write( object->mCollisionCallback ) ||
            !stream.write( object->mCollisionSuppress ) ||
            !stream.write( object->mBlending ) ||
            !stream.write( object->mSrcBlendFactor ) ||
            !stream.write( object->mDstBlendFactor ) ||
            !stream.write( object->mBlendColour ) ||
            !stream.write( object->mLayerOrder ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
        // Error.
        return false;


    // Is the Object Mounted?
    if ( object->processIsMounted() )
    {
        // Yes, so write Mount Object Details.
        if (    !stream.write( true ) ||
                !stream.write( object->getProcessMount()->mSerialiseKey ) ||
                !stream.write( object->mMountOffset.mX ) ||
                !stream.write( object->mMountOffset.mY ) ||
                !stream.write( object->mMountForce ) ||
                !stream.write( object->mMountTrackRotation ) ||
                !stream.write( object->mMountOwned ) ||
                !stream.write( object->mMountInheritAttributes ) )
            // Error.
            return false;
    }
    else
    {
        // No, so write No Mount Flag.
        if ( !stream.write( false ) )
            // Error.
            return false;
    }

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v9
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dSceneObject, 9 )
{
    bool                    initialUpdate;
    bool                    enabled;
    bool                    visible;
    F32                     mountRotation;
    bool                    flipX;
    bool                    flipY;
    U32                     layer;
    U32                     group;
    F32                     lifetime;
    eWorldLimit             worldLimitMode;
    t2dVector               worldLimitMin;
    t2dVector               worldLimitMax;
    bool                    worldLimitCallback;
    F32                     autoMountRotation;
    bool                    collisionActiveSend;
    bool                    collisionActiveReceive;
    bool                    collisionPhysicsSend;
    bool                    collisionPhysicsReceive;
    U32                     collisionLayerMask;
    U32                     collisionGroupMask;
    bool                    collisionCallback;
    bool                    collisionSuppress;
    bool                    blending;
    U32                     srcBlendFactor;
    U32                     dstBlendFactor;
    ColorF                  blendColour;
    S32                     layerOrder;
    bool                    useMouseEvents;

    // Core Info.
    if  (   !stream.read( &initialUpdate ) ||
            !stream.read( &enabled ) ||
            !stream.read( &visible ) ||
            !stream.read( &mountRotation ) ||
            !stream.read( &flipX ) ||
            !stream.read( &flipY ) ||
            !stream.read( &layer ) ||
            !stream.read( &group ) ||
            !stream.read( &lifetime ) ||
            !stream.read( (S32*)&worldLimitMode ) ||
            !stream.read( &worldLimitMin.mX ) ||
            !stream.read( &worldLimitMin.mY ) ||
            !stream.read( &worldLimitMax.mX ) ||
            !stream.read( &worldLimitMax.mY ) ||
            !stream.read( &worldLimitCallback ) ||
            !stream.read( &autoMountRotation ) ||
            !stream.read( &collisionActiveSend ) ||
            !stream.read( &collisionActiveReceive ) ||
            !stream.read( &collisionPhysicsSend ) ||
            !stream.read( &collisionPhysicsReceive ) ||
            !stream.read( &collisionLayerMask ) ||
            !stream.read( &collisionGroupMask ) ||
            !stream.read( &collisionCallback ) ||
            !stream.read( &collisionSuppress ) ||
            !stream.read( &blending ) ||
            !stream.read( &srcBlendFactor ) ||
            !stream.read( &dstBlendFactor ) ||
            !stream.read( &blendColour ) ||
            !stream.read( &layerOrder ) ||
            !stream.read( &useMouseEvents ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
        // Error.
        return false;

    // Set Initial Update Status.
    object->mInitialUpdate = initialUpdate;

    // Set Enabled Status.
    object->setEnabled( enabled );
    // Set Visible Status.
    object->setVisible( visible );
    // Set Mount Rotation.
    object->setMountRotation( mountRotation );
    // Set Flip.
    object->setFlip( flipX, flipY );
    // Layer/Group.
    object->setLayer( layer );
    object->setGraphGroup( group );
    // Set Lifetime.
    object->setLifetime( lifetime );
    // Set World Limit.
    object->setWorldLimit( worldLimitMode, worldLimitMin, worldLimitMax, worldLimitCallback );
    // Set Auto Mount-Rotation.
    object->setAutoMountRotation( autoMountRotation );
    // Set Collisions.
    object->setCollisionActive( collisionActiveSend, collisionActiveReceive );
    object->setCollisionPhysics( collisionPhysicsSend, collisionPhysicsReceive );
    object->setCollisionMasks( collisionGroupMask, collisionLayerMask );
    object->setCollisionCallback( collisionCallback );
    object->setCollisionSuppress( collisionSuppress );

    // Set Blending.
    object->setBlending( blending, srcBlendFactor, dstBlendFactor );
    // Set Blend Colour.
    object->setBlendColour( blendColour );

    // Set Layer Order.
    //
    // NOTE:-   we ignore the layer-order if requested.  This is typically done
    //          so that objects that have their own loading/saving support, don't
    //          have their already-set layer-order changed by the load.  This
    //          may not always be the case but the object in question can select
    //          whether it's used or not with this flag.
    if ( !ignoreLayerOrder )
        object->setLayerOrder( layerOrder );

    // Set Use Mouse Events.
    object->setUseMouseEvents( useMouseEvents );


    // Mounting.
    bool                    mountFlag;
    U32                     mountObjectKey;
    t2dVector               mountOffset;
    F32                     mountForce;
    bool                    mountTrackRotation;
    F32                     mountPreRotation;
    bool                    mountOwned;
    bool                    mountInheritAttributes;

    // Read Mount Flag.
    if ( !stream.read( &mountFlag ) )
        // Return Error.
        return false;

    // Mount Object?
    if ( mountFlag )
    {
        // Read Mount Object Key Key.
        if (    !stream.read( &mountObjectKey ) ||
                !stream.read( &mountOffset.mX ) ||
                !stream.read( &mountOffset.mY ) ||
                !stream.read( &mountForce ) ||
                !stream.read( &mountTrackRotation ) ||
                !stream.read( &mountPreRotation ) ||
                !stream.read( &mountOwned ) ||
                !stream.read( &mountInheritAttributes ) )
            // Return Error.
            return false;

        // Find Mount Object.
        t2dSceneObject* pMountObject2D = object->findSerialiseKey( mountObjectKey, ObjReferenceList );
        // Check Object.
        if ( !pMountObject2D )
        {
            // Warn.
            // NOTE:-   We're removing this because per object saves cannot work with this.
            //Con::warnf("t2dSceneObject::loadStream - Object was mounted when saved but mount now not available!");
            // Should be Okay!
            return true;
        }

        // Set Rotation to the pre-rotation state.
        object->setRotation( mountPreRotation );

        // Mount Object.
        if ( object->mount( pMountObject2D, mountOffset, mountForce, mountTrackRotation, false, mountOwned, mountInheritAttributes ) < 0 )
            // Error.
            return false;
    }

    // Update Spatial Config.
    object->updateSpatialConfig();

    // Return Okay.
    return true;
}



//-----------------------------------------------------------------------------
// Save v9
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dSceneObject, 9 )
{
    // Core Info.
    if  (   !stream.write( object->mInitialUpdate ) ||
            !stream.write( object->mEnabled ) ||
            !stream.write( object->mVisible ) ||
            !stream.write( object->mMountRotation ) ||
            !stream.write( object->mFlipX ) ||
            !stream.write( object->mFlipY ) ||
            !stream.write( object->mLayer ) ||
            !stream.write( object->mGraphGroup ) ||
            !stream.write( object->mLifetime ) ||
            !stream.write( (S32)object->mWorldLimitMode ) ||
            !stream.write( object->mWorldLimitMin.mX ) ||
            !stream.write( object->mWorldLimitMin.mY ) ||
            !stream.write( object->mWorldLimitMax.mX ) ||
            !stream.write( object->mWorldLimitMax.mY ) ||
            !stream.write( object->mWorldLimitCallback ) ||
            !stream.write( object->mAutoMountRotation ) ||
            !stream.write( object->mCollisionActiveSend ) ||
            !stream.write( object->mCollisionActiveReceive ) ||
            !stream.write( object->mCollisionPhysicsSend ) ||
            !stream.write( object->mCollisionPhysicsReceive ) ||
            !stream.write( object->mCollisionLayerMask ) ||
            !stream.write( object->mCollisionGroupMask ) ||
            !stream.write( object->mCollisionCallback ) ||
            !stream.write( object->mCollisionSuppress ) ||
            !stream.write( object->mBlending ) ||
            !stream.write( object->mSrcBlendFactor ) ||
            !stream.write( object->mDstBlendFactor ) ||
            !stream.write( object->mBlendColour ) ||
            !stream.write( object->mLayerOrder ) ||
            !stream.write( object->mUseMouseEvents ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
        // Error.
        return false;


    // Is the Object Mounted?
    if ( object->processIsMounted() )
    {
        // Yes, so write Mount Object Details.
        if (    !stream.write( true ) ||
                !stream.write( object->getProcessMount()->mSerialiseKey ) ||
                !stream.write( object->mMountOffset.mX ) ||
                !stream.write( object->mMountOffset.mY ) ||
                !stream.write( object->mMountForce ) ||
                !stream.write( object->mMountTrackRotation ) ||
                !stream.write( object->mMountPreRotation ) ||
                !stream.write( object->mMountOwned ) ||
                !stream.write( object->mMountInheritAttributes ) )
            // Error.
            return false;
    }
    else
    {
        // No, so write No Mount Flag.
        if ( !stream.write( false ) )
            // Error.
            return false;
    }

    // Return Okay.
    return true;
}



//-----------------------------------------------------------------------------
// Load v10
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dSceneObject, 10 )
{
    bool                    initialUpdate;
    bool                    enabled;
    bool                    paused;
    bool                    visible;
    F32                     mountRotation;
    bool                    flipX;
    bool                    flipY;
    U32                     layer;
    U32                     group;
    F32                     lifetime;
    eWorldLimit             worldLimitMode;
    t2dVector               worldLimitMin;
    t2dVector               worldLimitMax;
    bool                    worldLimitCallback;
    F32                     autoMountRotation;
    bool                    collisionActiveSend;
    bool                    collisionActiveReceive;
    bool                    collisionPhysicsSend;
    bool                    collisionPhysicsReceive;
    U32                     collisionLayerMask;
    U32                     collisionGroupMask;
    bool                    collisionCallback;
    bool                    collisionSuppress;
    bool                    blending;
    U32                     srcBlendFactor;
    U32                     dstBlendFactor;
    ColorF                  blendColour;
    S32                     layerOrder;
    bool                    useMouseEvents;

    // Core Info.
    if  (   !stream.read( &initialUpdate ) ||
            !stream.read( &enabled ) ||
            !stream.read( &paused ) ||
            !stream.read( &visible ) ||
            !stream.read( &mountRotation ) ||
            !stream.read( &flipX ) ||
            !stream.read( &flipY ) ||
            !stream.read( &layer ) ||
            !stream.read( &group ) ||
            !stream.read( &lifetime ) ||
            !stream.read( (S32*)&worldLimitMode ) ||
            !stream.read( &worldLimitMin.mX ) ||
            !stream.read( &worldLimitMin.mY ) ||
            !stream.read( &worldLimitMax.mX ) ||
            !stream.read( &worldLimitMax.mY ) ||
            !stream.read( &worldLimitCallback ) ||
            !stream.read( &autoMountRotation ) ||
            !stream.read( &collisionActiveSend ) ||
            !stream.read( &collisionActiveReceive ) ||
            !stream.read( &collisionPhysicsSend ) ||
            !stream.read( &collisionPhysicsReceive ) ||
            !stream.read( &collisionLayerMask ) ||
            !stream.read( &collisionGroupMask ) ||
            !stream.read( &collisionCallback ) ||
            !stream.read( &collisionSuppress ) ||
            !stream.read( &blending ) ||
            !stream.read( &srcBlendFactor ) ||
            !stream.read( &dstBlendFactor ) ||
            !stream.read( &blendColour ) ||
            !stream.read( &layerOrder ) ||
            !stream.read( &useMouseEvents ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
        // Error.
        return false;

    // Set Initial Update Status.
    object->mInitialUpdate = initialUpdate;

    // Set Enabled Status.
    object->setEnabled( enabled );
    // Set Paused Status.
    object->setPaused( paused );
    // Set Visible Status.
    object->setVisible( visible );
    // Set Mount Rotation.
    object->setMountRotation( mountRotation );
    // Set Flip.
    object->setFlip( flipX, flipY );
    // Layer/Group.
    object->setLayer( layer );
    object->setGraphGroup( group );
    // Set Lifetime.
    object->setLifetime( lifetime );
    // Set World Limit.
    object->setWorldLimit( worldLimitMode, worldLimitMin, worldLimitMax, worldLimitCallback );
    // Set Auto Mount-Rotation.
    object->setAutoMountRotation( autoMountRotation );
    // Set Collisions.
    object->setCollisionActive( collisionActiveSend, collisionActiveReceive );
    object->setCollisionPhysics( collisionPhysicsSend, collisionPhysicsReceive );
    object->setCollisionMasks( collisionGroupMask, collisionLayerMask );
    object->setCollisionCallback( collisionCallback );
    object->setCollisionSuppress( collisionSuppress );

    // Set Blending.
    object->setBlending( blending, srcBlendFactor, dstBlendFactor );
    // Set Blend Colour.
    object->setBlendColour( blendColour );

    // Set Layer Order.
    //
    // NOTE:-   we ignore the layer-order if requested.  This is typically done
    //          so that objects that have their own loading/saving support, don't
    //          have their already-set layer-order changed by the load.  This
    //          may not always be the case but the object in question can select
    //          whether it's used or not with this flag.
    if ( !ignoreLayerOrder )
        object->setLayerOrder( layerOrder );

    // Set Use Mouse Events.
    object->setUseMouseEvents( useMouseEvents );


    // Mounting.
    bool                    mountFlag;
    U32                     mountObjectKey;
    t2dVector               mountOffset;
    F32                     mountForce;
    bool                    mountTrackRotation;
    F32                     mountPreRotation;
    bool                    mountOwned;
    bool                    mountInheritAttributes;

    // Read Mount Flag.
    if ( !stream.read( &mountFlag ) )
        // Return Error.
        return false;

    // Mount Object?
    if ( mountFlag )
    {
        // Read Mount Object Key Key.
        if (    !stream.read( &mountObjectKey ) ||
                !stream.read( &mountOffset.mX ) ||
                !stream.read( &mountOffset.mY ) ||
                !stream.read( &mountForce ) ||
                !stream.read( &mountTrackRotation ) ||
                !stream.read( &mountPreRotation ) ||
                !stream.read( &mountOwned ) ||
                !stream.read( &mountInheritAttributes ) )
            // Return Error.
            return false;

        // Find Mount Object.
        t2dSceneObject* pMountObject2D = object->findSerialiseKey( mountObjectKey, ObjReferenceList );
        // Check Object.
        if ( !pMountObject2D )
        {
            // Warn.
            // NOTE:-   We're removing this because per object saves cannot work with this.
            //Con::warnf("t2dSceneObject::loadStream - Object was mounted when saved but mount now not available!");
            // Should be Okay!
            return true;
        }

        // Set Rotation to the pre-rotation state.
        object->setRotation( mountPreRotation );

        // Mount Object.
        if ( object->mount( pMountObject2D, mountOffset, mountForce, mountTrackRotation, false, mountOwned, mountInheritAttributes ) < 0 )
            // Error.
            return false;
    }

    // Update Spatial Config.
    object->updateSpatialConfig();

    // Return Okay.
    return true;
}



//-----------------------------------------------------------------------------
// Save v10
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dSceneObject, 10 )
{
    // Core Info.
    if  (   !stream.write( object->mInitialUpdate ) ||
            !stream.write( object->mEnabled ) ||
            !stream.write( object->mPaused ) ||
            !stream.write( object->mVisible ) ||
            !stream.write( object->mMountRotation ) ||
            !stream.write( object->mFlipX ) ||
            !stream.write( object->mFlipY ) ||
            !stream.write( object->mLayer ) ||
            !stream.write( object->mGraphGroup ) ||
            !stream.write( object->mLifetime ) ||
            !stream.write( (S32)object->mWorldLimitMode ) ||
            !stream.write( object->mWorldLimitMin.mX ) ||
            !stream.write( object->mWorldLimitMin.mY ) ||
            !stream.write( object->mWorldLimitMax.mX ) ||
            !stream.write( object->mWorldLimitMax.mY ) ||
            !stream.write( object->mWorldLimitCallback ) ||
            !stream.write( object->mAutoMountRotation ) ||
            !stream.write( object->mCollisionActiveSend ) ||
            !stream.write( object->mCollisionActiveReceive ) ||
            !stream.write( object->mCollisionPhysicsSend ) ||
            !stream.write( object->mCollisionPhysicsReceive ) ||
            !stream.write( object->mCollisionLayerMask ) ||
            !stream.write( object->mCollisionGroupMask ) ||
            !stream.write( object->mCollisionCallback ) ||
            !stream.write( object->mCollisionSuppress ) ||
            !stream.write( object->mBlending ) ||
            !stream.write( object->mSrcBlendFactor ) ||
            !stream.write( object->mDstBlendFactor ) ||
            !stream.write( object->mBlendColour ) ||
            !stream.write( object->mLayerOrder ) ||
            !stream.write( object->mUseMouseEvents ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
        // Error.
        return false;


    // Is the Object Mounted?
    if ( object->processIsMounted() )
    {
        // Yes, so write Mount Object Details.
        if (    !stream.write( true ) ||
                !stream.write( object->getProcessMount()->mSerialiseKey ) ||
                !stream.write( object->mMountOffset.mX ) ||
                !stream.write( object->mMountOffset.mY ) ||
                !stream.write( object->mMountForce ) ||
                !stream.write( object->mMountTrackRotation ) ||
                !stream.write( object->mMountPreRotation ) ||
                !stream.write( object->mMountOwned ) ||
                !stream.write( object->mMountInheritAttributes ) )
            // Error.
            return false;
    }
    else
    {
        // No, so write No Mount Flag.
        if ( !stream.write( false ) )
            // Error.
            return false;
    }

    // Return Okay.
    return true;
}



//-----------------------------------------------------------------------------
// Load v11
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dSceneObject, 11 )
{
    bool                    initialUpdate;
    bool                    enabled;
    bool                    paused;
    bool                    visible;
    F32                     mountRotation;
    bool                    flipX;
    bool                    flipY;
    t2dVector               sortPoint;
    U32                     layer;
    U32                     group;
    F32                     lifetime;
    eWorldLimit             worldLimitMode;
    t2dVector               worldLimitMin;
    t2dVector               worldLimitMax;
    bool                    worldLimitCallback;
    F32                     autoMountRotation;
    bool                    collisionActiveSend;
    bool                    collisionActiveReceive;
    bool                    collisionPhysicsSend;
    bool                    collisionPhysicsReceive;
    U32                     collisionLayerMask;
    U32                     collisionGroupMask;
    bool                    collisionCallback;
    bool                    collisionSuppress;
    bool                    blending;
    U32                     srcBlendFactor;
    U32                     dstBlendFactor;
    ColorF                  blendColour;
    S32                     layerOrder;
    bool                    useMouseEvents;
	bool					usesPhysics;

    // Core Info.
    if  (   !stream.read( &initialUpdate ) ||
            !stream.read( &enabled ) ||
            !stream.read( &paused ) ||
            !stream.read( &visible ) ||
            !stream.read( &mountRotation ) ||
            !stream.read( &flipX ) ||
            !stream.read( &flipY ) ||
            !stream.read( &sortPoint.mX ) ||
            !stream.read( &sortPoint.mY ) ||
            !stream.read( &layer ) ||
            !stream.read( &group ) ||
            !stream.read( &lifetime ) ||
            !stream.read( (S32*)&worldLimitMode ) ||
            !stream.read( &worldLimitMin.mX ) ||
            !stream.read( &worldLimitMin.mY ) ||
            !stream.read( &worldLimitMax.mX ) ||
            !stream.read( &worldLimitMax.mY ) ||
            !stream.read( &worldLimitCallback ) ||
            !stream.read( &autoMountRotation ) ||
            !stream.read( &usesPhysics ) ||
			!stream.read( &collisionActiveSend ) ||
            !stream.read( &collisionActiveReceive ) ||
            !stream.read( &collisionPhysicsSend ) ||
            !stream.read( &collisionPhysicsReceive ) ||
            !stream.read( &collisionLayerMask ) ||
            !stream.read( &collisionGroupMask ) ||
            !stream.read( &collisionCallback ) ||
            !stream.read( &collisionSuppress ) ||
            !stream.read( &blending ) ||
            !stream.read( &srcBlendFactor ) ||
            !stream.read( &dstBlendFactor ) ||
            !stream.read( &blendColour ) ||
            !stream.read( &layerOrder ) ||
            !stream.read( &useMouseEvents ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false; 

    // Set Initial Update Status.
    object->mInitialUpdate = initialUpdate;

    // Set Enabled Status.
    object->setEnabled( enabled );
    // Set Paused Status.
    object->setPaused( paused );
    // Set Visible Status.
    object->setVisible( visible );
    // Set Mount Rotation.
    object->setMountRotation( mountRotation );
    // Set Flip.
    object->setFlip( flipX, flipY );
    // Set Sort Point.
    object->setSortPoint( sortPoint );
    // Layer/Group.
    object->setLayer( layer );
    object->setGraphGroup( group );
    // Set Lifetime.
    object->setLifetime( lifetime );
    // Set World Limit.
    object->setWorldLimit( worldLimitMode, worldLimitMin, worldLimitMax, worldLimitCallback );
    // Set Auto Mount-Rotation.
    object->setAutoMountRotation( autoMountRotation );
    // Set Collisions.
	object->setUsesPhysics( usesPhysics );
    object->setCollisionActive( collisionActiveSend, collisionActiveReceive );
    object->setCollisionPhysics( collisionPhysicsSend, collisionPhysicsReceive );
    object->setCollisionMasks( collisionGroupMask, collisionLayerMask );
    object->setCollisionCallback( collisionCallback );
    object->setCollisionSuppress( collisionSuppress );

    // Set Blending.
    object->setBlending( blending, srcBlendFactor, dstBlendFactor );
    // Set Blend Colour.
    object->setBlendColour( blendColour );

    // Set Layer Order.
    //
    // NOTE:-   we ignore the layer-order if requested.  This is typically done
    //          so that objects that have their own loading/saving support, don't
    //          have their already-set layer-order changed by the load.  This
    //          may not always be the case but the object in question can select
    //          whether it's used or not with this flag.
    if ( !ignoreLayerOrder )
        object->setLayerOrder( layerOrder );

    // Set Use Mouse Events.
    object->setUseMouseEvents( useMouseEvents );


    // Mounting.
    bool                    mountFlag;
    U32                     mountObjectKey;
    t2dVector               mountOffset;
    F32                     mountForce;
    bool                    mountTrackRotation;
    F32                     mountPreRotation;
    bool                    mountOwned;
    bool                    mountInheritAttributes;

    // Read Mount Flag.
    if ( !stream.read( &mountFlag ) )
        // Return Error.
        return false;

    // Mount Object?
    if ( mountFlag )
    {
        // Read Mount Object Key Key.
        if (    !stream.read( &mountObjectKey ) ||
                !stream.read( &mountOffset.mX ) ||
                !stream.read( &mountOffset.mY ) ||
                !stream.read( &mountForce ) ||
                !stream.read( &mountTrackRotation ) ||
                !stream.read( &mountPreRotation ) ||
                !stream.read( &mountOwned ) ||
                !stream.read( &mountInheritAttributes ) )
            // Return Error.
            return false;

        // Find Mount Object.
        t2dSceneObject* pMountObject2D = object->findSerialiseKey( mountObjectKey, ObjReferenceList );
        // Check Object.
        if ( !pMountObject2D )
        {
            // Warn.
            // NOTE:-   We're removing this because per object saves cannot work with this.
            //Con::warnf("t2dSceneObject::loadStream - Object was mounted when saved but mount now not available!");
            // Should be Okay!
            return true;
        }

        // Set Rotation to the pre-rotation state.
        object->setRotation( mountPreRotation );

        // Mount Object.
        if ( object->mount( pMountObject2D, mountOffset, mountForce, mountTrackRotation, false, mountOwned, mountInheritAttributes ) < 0 )
            // Error.
            return false;
    }

    // Update Spatial Config.
    object->updateSpatialConfig();

    // Return Okay.
    return true;
}



//-----------------------------------------------------------------------------
// Save v11
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dSceneObject, 11 )
{
    // Core Info.
    if  (   !stream.write( object->mInitialUpdate ) ||
            !stream.write( object->mEnabled ) ||
            !stream.write( object->mPaused ) ||
            !stream.write( object->mVisible ) ||
            !stream.write( object->mMountRotation ) ||
            !stream.write( object->mFlipX ) ||
            !stream.write( object->mFlipY ) ||
            !stream.write( object->mSortPoint.mX ) ||
            !stream.write( object->mSortPoint.mY ) ||
            !stream.write( object->mLayer ) ||
            !stream.write( object->mGraphGroup ) ||
            !stream.write( object->mLifetime ) ||
            !stream.write( (S32)object->mWorldLimitMode ) ||
            !stream.write( object->mWorldLimitMin.mX ) ||
            !stream.write( object->mWorldLimitMin.mY ) ||
            !stream.write( object->mWorldLimitMax.mX ) ||
            !stream.write( object->mWorldLimitMax.mY ) ||
            !stream.write( object->mWorldLimitCallback ) ||
            !stream.write( object->mAutoMountRotation ) ||
			!stream.write( object->mUsesPhysics) ||
            !stream.write( object->mCollisionActiveSend ) ||
            !stream.write( object->mCollisionActiveReceive ) ||
            !stream.write( object->mCollisionPhysicsSend ) ||
            !stream.write( object->mCollisionPhysicsReceive ) ||
            !stream.write( object->mCollisionLayerMask ) ||
            !stream.write( object->mCollisionGroupMask ) ||
            !stream.write( object->mCollisionCallback ) ||
            !stream.write( object->mCollisionSuppress ) ||
            !stream.write( object->mBlending ) ||
            !stream.write( object->mSrcBlendFactor ) ||
            !stream.write( object->mDstBlendFactor ) ||
            !stream.write( object->mBlendColour ) ||
            !stream.write( object->mLayerOrder ) ||
            !stream.write( object->mUseMouseEvents ) )
        // Error.
        return false;

    // Physics.
    if ( !object->getParentPhysics().saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
        return false;

    // Is the Object Mounted?
    if ( object->processIsMounted() )
    {
        // Yes, so write Mount Object Details.
        if (    !stream.write( true ) ||
                !stream.write( object->getProcessMount()->mSerialiseKey ) ||
                !stream.write( object->mMountOffset.mX ) ||
                !stream.write( object->mMountOffset.mY ) ||
                !stream.write( object->mMountForce ) ||
                !stream.write( object->mMountTrackRotation ) ||
                !stream.write( object->mMountPreRotation ) ||
                !stream.write( object->mMountOwned ) ||
                !stream.write( object->mMountInheritAttributes ) )
            // Error.
            return false;
    }
    else
    {
        // No, so write No Mount Flag.
        if ( !stream.write( false ) )
            // Error.
            return false;
    }

    // Return Okay.
    return true;
}


bool t2dSceneObject::writeField(StringTableEntry fieldname, const char* value)
{
   if (!Parent::writeField(fieldname, value))
      return false;

   // If value is the same as the corresponding value in the config datablock or the default
   // field table, don't write the field.
   if (mConfigDataBlock && (dStricmp(mConfigDataBlock->getDataField(fieldname, NULL), value) == 0))
      return false;

   // If we don't have a config datablock then check the default datablock for changes.
   if (!mConfigDataBlock && getDefaultConfig() && (dStricmp(getDefaultConfig()->getDataField(fieldname, NULL), value) == 0))
      return false;

   // Never save the scenegraph field.
   if (dStricmp(fieldname, "scenegraph") == 0)
      return false;

   // Don't save mount fields if it's not mounted.
   if (!processIsMounted())
   {
      if (dStricmp(fieldname, "mountOffset") == 0) return false;
      if (dStricmp(fieldname, "mountForce") == 0) return false;
      if (dStricmp(fieldname, "mountTrackRotation") == 0) return false;
      if (dStricmp(fieldname, "mountOwned") == 0) return false;
      if (dStricmp(fieldname, "mountInheritAttributes") == 0) return false;
   }

   // Don't save mass and inertia if they're auto calculated.
   if (getAutoMassInertia())
   {
      if (dStricmp(fieldname, "mass") == 0) return false;
      if (dStricmp(fieldname, "inertia") == 0) return false;
   }

   return true;
}

void t2dSceneObject::writeFields(Stream& stream, U32 tabStop)
{
   if (mpMountedTo)
   {
      if( getMountTrackRotation() )
         setRotation(getRotation() - mpMountedTo->getMountRotation() - mpMountedTo->getRotation());

      setDataField(StringTable->insert("mountToID"), NULL, mpMountedTo->getDataField(StringTable->insert("mountID"), NULL));
   }

   Parent::writeFields(stream, tabStop);
}

void t2dSceneObject::readFieldsPostLoad()
{
   if (!getSceneGraph() || !getFieldDictionary())
      return;

   const char* stringMountID = getDataField(StringTable->insert("mountToID"), NULL);
   U32 mountToID = stringMountID && *stringMountID ? dAtoi(stringMountID) : 0;

   // No sense searching for nothing.
   if (mountToID < 1)
      return;

   t2dSceneObject* pSceneObject2D = getSceneGraph()->getProcessHead()->getProcessNext();
   while ( pSceneObject2D != getSceneGraph()->getProcessHead() )
   {
      if (dAtoi(pSceneObject2D->getDataField(StringTable->insert("mountID"), NULL)) == mountToID)
         mount(pSceneObject2D, mMountOffset, mMountForce, mMountTrackRotation, true, mMountOwned, mMountInheritAttributes);

      pSceneObject2D = pSceneObject2D->getProcessNext();
   }
}

//---------------------------------------------------------------------------------------------------------------------------------------------
// RKS: I created this function to replace the 'OnScriptCollision' because scripts suck balls
void t2dSceneObject::HandleScriptCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( _pCollisionStatus == NULL )
		return;
	
	SimComponent* pComponent = NULL;
	S32 iNumComponents = getComponentCount();
	for( S32 i = 0; i < iNumComponents; ++i )
	{
		pComponent = getComponent( i );
		if( pComponent )
		{
			pComponent->HandleOwnerCollision( _pCollisionStatus );
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------


BehaviorInstance * t2dSceneObject::behavior(const char *name)
{
   StringTableEntry stName = StringTable->insert(name);
   VectorPtr<SimComponent *>&componentList = lockComponentList();

   for( SimComponentIterator nItr = componentList.begin(); nItr != componentList.end(); nItr++ )
   {
      BehaviorInstance *pComponent = dynamic_cast<BehaviorInstance*>(*nItr);
      if( pComponent && StringTable->insert(pComponent->getTemplateName()) == stName )
      {
         unlockComponentList();
         return pComponent;
      }
   }

   unlockComponentList();

   return NULL;
}

ConsoleMethod(t2dSceneObject, behavior, S32, 3, 3, "(string behaviorName) - Gets the behavior instance ID off of the object based on the behavior name passed.\n"
                                                   "@param behaviorName The name of the behavior you want to get the instance ID of.\n"
                                                   "@return (integer behaviorID) The id of the behavior instance.")
{
   BehaviorInstance *inst = object->behavior(argv[2]);
   return inst ? inst->getId() : 0;
}

//-----------------------------------------------------------------------------
// Set UsesPhysics.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneObject, getUsesPhysics, bool, 2, 2, "() - Gets whether physics are used by this object.\n"
                                                             "@return (bool usesPhysics) Whether physics are used by this object.")
{
    // Get Use Window Mouse Events.
    return object->getUsesPhysics();
}

ConsoleMethod(t2dSceneObject, setUsesPhysics, void, 3, 3, "(bool update) Enables or disables the object's physics.\n"
                                                      "@param update Whether to enable or disable the object."
													  "@return No return value.")
{
    // Set Enabled.
    object->setUsesPhysics( dAtob(argv[2]) );
}

