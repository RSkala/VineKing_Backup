//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Physics Object.
//-----------------------------------------------------------------------------

#include "math/mMath.h"
#include "t2dSceneObject.h"
#include "console/consoleTypes.h"
#include "./t2dUtility.h"
#include "./t2dPhysics.h"
#include "core/frameAllocator.h"

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif


//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------
static const float  defaultDynamicFriction          = 0.3f;
static const float  defaultRestitution              = 1.0f;
static const float  defaultDensity                  = 0.01f;
static const float  defaultMass                     = 1.0f;
static const float  defaultInertialMoment           = 16.6f;
static const float  defaultForceScale               = 1.0f;
static const float  defaultDamping                  = 0.0f;
static const bool   defaultAutoMassInertia          = true;
static const bool   defaultImmovable                = false;

// Collision Detection/Response.
const t2dPhysics::eCollisionDetection   defaultCollisionDetectionMode       = t2dPhysics::T2D_DETECTION_POLY_ONLY;
const t2dPhysics::eCollisionResponse    defaultCollisionResponseMode        = t2dPhysics::T2D_RESPONSE_CLAMP;
const bool                              defaultCollisionCircleSuperscribed  = true;
const U32                               defaultCollisionMaxIterations       = 3;

// Default Size.
const static t2dVector defaultObjectSize(10.0f, 10.0f); 


//-----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(t2dCollisionMaterialDatablock);

//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Collision Detection Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums collisionDetectionLookup[] =
                {
                { t2dPhysics::T2D_DETECTION_FULL,           "FULL"      },
                { t2dPhysics::T2D_DETECTION_CIRCLE_ONLY,    "CIRCLE"    },
                { t2dPhysics::T2D_DETECTION_POLY_ONLY,      "POLYGON"   },

                { t2dPhysics::T2D_DETECTION_CUSTOM,         "CUSTOM"    },
                };

EnumTable collisionDetectionTable(sizeof(collisionDetectionLookup) / sizeof(EnumTable::Enums), &collisionDetectionLookup[0]);

//-----------------------------------------------------------------------------
// Collision Detection Script-Enumerator.
//-----------------------------------------------------------------------------
t2dPhysics::eCollisionDetection getCollisionDetectionEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(collisionDetectionLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(collisionDetectionLookup[i].label, label) == 0)
            return((t2dPhysics::eCollisionDetection)collisionDetectionLookup[i].index);

    // Invalid Collision Response!
    AssertFatal(false, "t2dPhysics::getCollisionDetectionEnum() - Invalid Collision-Detection Mode!");
    // Bah!
    return t2dPhysics::T2D_DETECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Collision Detection Description Script-Enumerator.
//-----------------------------------------------------------------------------
const char* getCollisionDetectionDescription(t2dPhysics::eCollisionDetection detectionMode)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(collisionDetectionLookup) / sizeof(EnumTable::Enums)); i++)
        if( collisionDetectionLookup[i].index == detectionMode )
            return collisionDetectionLookup[i].label;

    // Invalid Collision Response!
    AssertFatal(false, "t2dPhysics::getCollisionDetectionDescription() - Invalid Collision-Detection Mode!");
    // Bah!
    return StringTable->insert("");
}



//-----------------------------------------------------------------------------
// Collision Response Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums collisionResponseLookup[] =
                {
                { t2dPhysics::T2D_RESPONSE_RIGID,   "RIGID"     },
                { t2dPhysics::T2D_RESPONSE_BOUNCE,  "BOUNCE"    },
                { t2dPhysics::T2D_RESPONSE_CLAMP,   "CLAMP"     },
                { t2dPhysics::T2D_RESPONSE_STICKY,  "STICKY"    },
                { t2dPhysics::T2D_RESPONSE_KILL,    "KILL"      },

                { t2dPhysics::T2D_RESPONSE_CUSTOM,  "CUSTOM"    },
                };

EnumTable collisionResponseTable(sizeof(collisionResponseLookup) / sizeof(EnumTable::Enums), &collisionResponseLookup[0]);

//-----------------------------------------------------------------------------
// Collision Response Script-Enumerator.
//-----------------------------------------------------------------------------
t2dPhysics::eCollisionResponse getCollisionResponseEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(collisionResponseLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(collisionResponseLookup[i].label, label) == 0)
            return((t2dPhysics::eCollisionResponse)collisionResponseLookup[i].index);

    // Invalid Collision Response!
    AssertFatal(false, "t2dPhysics::getCollisionResponseEnum() - Invalid Collision-Response Mode!");
    // Bah!
    return t2dPhysics::T2D_RESPONSE_INVALID;
}

//-----------------------------------------------------------------------------
// Collision Response Description Script-Enumerator.
//-----------------------------------------------------------------------------
const char* getCollisionResponseDescription(t2dPhysics::eCollisionResponse responseMode)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(collisionResponseLookup) / sizeof(EnumTable::Enums)); i++)
        if( collisionResponseLookup[i].index == responseMode )
            return collisionResponseLookup[i].label;

    // Invalid Collision Response!
    AssertFatal(false, "t2dPhysics::getCollisionResponseDescription() - Invalid Collision-Response Mode!");
    // Bah!
    return StringTable->insert("");
}

//------------------------------------------------------------------------------

t2dCollisionMaterialDatablock::t2dCollisionMaterialDatablock() :    mCoefDynamicFriction(defaultDynamicFriction),
                                                                    mCoefRestitution(defaultRestitution),
                                                                    mDensity(defaultDensity),
                                                                    mMass(defaultMass),
                                                                    mInertialMoment(defaultInertialMoment),
                                                                    mForceScale(defaultForceScale),
                                                                    mDamping(defaultDamping),
                                                                    mAutoMassInertia(defaultAutoMassInertia),
                                                                    mImmovable(defaultImmovable)
{
}

//------------------------------------------------------------------------------

t2dCollisionMaterialDatablock::~t2dCollisionMaterialDatablock()
{
}

//------------------------------------------------------------------------------

void t2dCollisionMaterialDatablock::initPersistFields()
{
    Parent::initPersistFields();

    // Fields.
    addField("friction",                TypeF32,                Offset(mCoefDynamicFriction,    t2dCollisionMaterialDatablock));
    addField("restitution",             TypeF32,                Offset(mCoefRestitution,        t2dCollisionMaterialDatablock));
    addField("density",                 TypeF32,                Offset(mDensity,                t2dCollisionMaterialDatablock));
    addField("mass",                    TypeF32,                Offset(mMass,                   t2dCollisionMaterialDatablock));
    addField("inertialmoment",          TypeF32,                Offset(mInertialMoment,         t2dCollisionMaterialDatablock));
    addField("forceScale",              TypeF32,                Offset(mForceScale,             t2dCollisionMaterialDatablock));
    addField("damping",                 TypeF32,                Offset(mDamping,                t2dCollisionMaterialDatablock));
    addField("automassinertia",         TypeBool,               Offset(mAutoMassInertia,        t2dCollisionMaterialDatablock));
    addField("immovable",               TypeBool,               Offset(mImmovable,              t2dCollisionMaterialDatablock));
}

//------------------------------------------------------------------------------

bool t2dCollisionMaterialDatablock::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void t2dCollisionMaterialDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
}

//------------------------------------------------------------------------------

void t2dCollisionMaterialDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);

    // Read Datablock.
}


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dPhysics::t2dPhysics() :  T2D_Stream_HeaderID(makeFourCCTag('2','D','P','M')),
                            mLocalSerialiseID(1),
                            mInitialised(false),
                            mSpatialDirty(true),
                            mNetLinearVelocityActive(false),
                            mNetAngularVelocityActive(false),
                            mGrossLinearVelocityActive(false),
                            mGrossAngularVelocityActive(false),
                            mAutoRotationActive(false),
                            mConstantForceActive(false),
                            mForwardMovementOnly(false),
                            mUpdatingPositionTarget(false),
                            mUpdatingRotationTarget(false),
                            mRefParentObject(NULL),
                            mRefMetaString(StringTable->insert("")),
                            mDensity(defaultDensity),
                            mMass(defaultMass),
                            mInertialMoment(defaultInertialMoment),
                            mAutoMassInertia(defaultAutoMassInertia),
                            mImmovable(defaultImmovable),
                            mCollisionMaxIterations( defaultCollisionMaxIterations ),
                            mCollisionPolyScale(1.0f, 1.0f),
                            mCollisionCircleScale(1.0f),
                            mCollisionDetectionMode(defaultCollisionDetectionMode),
                            mCollisionResponseMode(defaultCollisionResponseMode),
                            mCollisionCircleSuperscribed( defaultCollisionCircleSuperscribed ),
                            mCollectCollisionHistory(false)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mCollisionPolyBasisList );
    VECTOR_SET_ASSOCIATION( mCollisionPolyLocalList );
    VECTOR_SET_ASSOCIATION( mCollisionHistory );

    // Set Default Size.
    setSize( defaultObjectSize );
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dPhysics::~t2dPhysics()
{
	// RKS: This is an attempt to hunt down the crash that is happening between levels:
	//SimObject* mRefParentObject;
	//S32 iNumVectors1 = mCollisionPolyBasisList.size();
	//S32 iNumVectors2 = mCollisionPolyLocalList.size();
	//printf( "parent: %s, %d, %d\n", mRefParentObject? mRefParentObject->getName() : "", iNumVectors1, iNumVectors2 );
	//printf( "~t2dPhysics: mCollisionPolyBasisList.size() = %d\n", iNumVectors );
	//iNumVectors = mCollisionPolyLocalList.size();
	//printf( "~t2dPhysics: mCollisionPolyLocalList.size() = %d\n", iNumVectors );
	
	//printf( "%s, 0x%x, 0x%x\n", mRefParentObject? mRefParentObject->getName() : "", &mCollisionPolyBasisList., &mCollisionPolyLocalList );
	//printf( "%s, 0x%x, 0x%x\n", mRefParentObject? mRefParentObject->getName() : "", mCollisionPolyBasisList.GetArrayAddress(), mCollisionPolyLocalList.GetArrayAddress() );
	//printf( "%s, 0x%x, 0x%x\n", mRefParentObject? mRefParentObject->getName() : "", mCollisionPolyBasisList.address(), mCollisionPolyLocalList.address() );
	//if( mRefMetaString )
	//{
	//	printf( "%s - mRefMetaString: %s\n", mRefParentObject ? mRefParentObject->getName() : "", mRefMetaString );
	//}
}


//-----------------------------------------------------------------------------
// Initialise.
//-----------------------------------------------------------------------------
void t2dPhysics::initialise( SimObject* pParent, const char* pRefMetaString )
{

    // Set Reference Parent Object.
    mRefParentObject = pParent;

    // Set Reference Meta-String.
    if ( pRefMetaString )
	{
		// RKS:  I added this print statement
		//printf( "%s - pRefMetaString: %s; 0x%x\n", mRefParentObject ? mRefParentObject->getName() : "", pRefMetaString, &mRefMetaString );
        mRefMetaString = StringTable->insert( pRefMetaString );
	}
    else
	{
        mRefMetaString = StringTable->insert("");
	}

    // Physics Suppression Off.
    setPhysicsSuppress(false);

    // Set Targets.
    setPositionTargetOff();
    setRotationTargetOff();

    // Set Minimum/Maximum Linear/Angular Velocities.
    // NOTE:-   We do it without using the appropriate functions as they
    //          do specific checking which we don't want here.
    mMinLinearVelocity  = 0.0f;
    mMinAngularVelocity = 0.0f;
    mMaxLinearVelocity  = 10.0e+3f;
    mMaxAngularVelocity = 10.0e+3f;

    // Reset Gross Velocity.
    resetGrossVelocity();
    // Reset Net Velocity.
    resetNetVelocity();

    // Set Other Material Properties.
    setDynamicFriction( defaultDynamicFriction );
    setRestitution( defaultRestitution );
    setForceScale( defaultForceScale );
    setDamping( defaultDamping );

    // Set Default Physical Metrics.
    setPosition( t2dVector::getZero() );
    setAutoRotation( 0.0f );
    setRotation( 0.0f );

    // Reset Tick Position.
    resetTickPosition();
    // Reset Tick Rotation.
    resetTickRotation();

    // Reset Constant Force.
    setConstantForce( t2dVector::getZero(), false );

    // Setup Initial Polygon.
    setCollisionPolyPrimitive( 4 );

    // Solve Overlap defaults active.
    // NOTE:-   DO NOT TURN THIS OFF UNLESS YOU KNOW THE IMPACT IT HAS!!!
    mSolveOverlap = true;

    // Flag as Initialised.
    mInitialised = true;
}


//-----------------------------------------------------------------------------
// Set Collision Maximum Itertations.
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionMaxIterations( const U32 iterations )
{
    // Set Collision Maximum Itertations.
    mCollisionMaxIterations = iterations;
};


//-----------------------------------------------------------------------------
// Set Solve Overlap.
//-----------------------------------------------------------------------------
void t2dPhysics::setSolveOverlap( const bool status )
{
    // Set Solve Overlap.
    mSolveOverlap = status;
}


//-----------------------------------------------------------------------------
// Set Physics Suppress.
//-----------------------------------------------------------------------------
void t2dPhysics::setPhysicsSuppress( const bool status )
{
    // Set Physics Suppress.
    mPhysicsSuppress = status;

    // Set at rest!
    if ( mPhysicsSuppress )
        setAtRest();
}


//-----------------------------------------------------------------------------
// Set Position.
//-----------------------------------------------------------------------------
void t2dPhysics::setPosition( const t2dVector& position )
{
    // Set Position.
    mPosition = position;

    // Flag Spatial Dirty.
    setSpatialDirty();

    // Is Position Target is active and not being updated?
    if ( getPositionTargetActive() && !mUpdatingPositionTarget )
    {
        // Yes, so update position target.
        updatePositionTarget( 0.0f );
    }
}


//-----------------------------------------------------------------------------
// Set Size.
//-----------------------------------------------------------------------------
void t2dPhysics::setSize( const t2dVector& size )
{
    // Set Size.
    mSize = size;

    // Make sure size isn't zero in either axis.
    if ( mIsZero(mSize.mX) )
        mSize.mX = T2D_CONST_EPSILON * 2.0f;
    else if ( mSize.mX < 0.0f )
        mSize.mX = T2D_CONST_EPSILON * 2.0f;
    if ( mIsZero(mSize.mY) )
        mSize.mY = T2D_CONST_EPSILON * 2.0f;
    else if ( mSize.mY < 0.0f )
        mSize.mY = T2D_CONST_EPSILON * 2.0f;

    // Calculate Half Size.
    mHalfSize = mSize * 0.5f;

    // Flag Spatial Dirty.
    setSpatialDirty();

    // Generate Collision Circle.
    generateCollisionCircle();

    // Generate Collision Poly.
    generateCollisionPoly();
}


//-----------------------------------------------------------------------------
// Set Collision Polygon Scale.
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionPolyScale( const t2dVector& scale )
{
    // Set Collision Scale.
    mCollisionPolyScale = scale;

    // Generation Collision Poly.
    generateCollisionPoly();
}


//-----------------------------------------------------------------------------
// Set Collision Circle Scale.
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionCircleScale( const F32 scale )
{
    // Set Collision Scale.
    mCollisionCircleScale = scale;

    // Generate Collision Circle.
    generateCollisionCircle();
}


//-----------------------------------------------------------------------------
// Set Collision Circle Superscribed Mode.
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionCircleSuperscribed( bool superscribeMode )
{
    // Set Collision Circle Superscribed Mode.
    mCollisionCircleSuperscribed = superscribeMode;

    // Generate Collision Circle.
    generateCollisionCircle();

    // Flag Spatial Dirty.
    setSpatialDirty();
}


//-----------------------------------------------------------------------------
// Set Rotation.
//-----------------------------------------------------------------------------
void t2dPhysics::setRotation( const F32 rotation )
{
    // Set Rotation.
    mRotation = mFmod( rotation, 360.0f );

    // Update Rotation Matrix.
    updateRotationMatrix( mRotation );

    // Flag Spatial Dirty.
    setSpatialDirty();

    // Is Rotation Target is active and not being updated?
    if ( getRotationTargetActive() && !mUpdatingRotationTarget )
    {
        // Yes, so update rotation target.
        updateRotationTarget( 0.0f, 0.0f );
    }
}


//-----------------------------------------------------------------------------
// Set Auto Rotation.
//-----------------------------------------------------------------------------
void t2dPhysics::setAutoRotation( const F32 autoRotation )
{
    // Set Auto Rotation.
    mAutoRotation = autoRotation;

    // Usage Flag.
    mAutoRotationActive = mNotZero( mAutoRotation );
}


//-----------------------------------------------------------------------------
// Set Forward Movement Only.
//-----------------------------------------------------------------------------
void t2dPhysics::setForwardMovementOnly( const bool forwardMovementOnly )
{
    // Set Forward Movement Only.
    mForwardMovementOnly = forwardMovementOnly;
}


//-----------------------------------------------------------------------------
// Set Position Target.
//-----------------------------------------------------------------------------
void t2dPhysics::setPositionTarget( const t2dVector positionTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin )
{
    // Setup Position Target.
    mPositionTargetActive   = true;
    mPositionTarget         = positionTarget;
    mPositionTargetAutoStop = autoStop;
    mPositionTargetCallback = callback;
    mPositionTargetSnap     = snap;
    mPositionTargetMargin   = mFabs(targetMargin);
    // Cap the minimum target margin.
    if ( mLessThan(mPositionTargetMargin, 0.001f) ) { mPositionTargetMargin = 0.001f; };
}


//-----------------------------------------------------------------------------
// Set Position Target Off.
//-----------------------------------------------------------------------------
void t2dPhysics::setPositionTargetOff( void )
{
    // Reset Position Target.
    mPositionTargetActive = false;
}


//-----------------------------------------------------------------------------
// Set Rotation Target.
//-----------------------------------------------------------------------------
void t2dPhysics::setRotationTarget( const F32 rotationTarget, const bool autoStop, const bool callback, const bool snap, const F32 targetMargin )
{
    // Setup Rotation Target.
    mRotationTargetActive   = true;
    mRotationTarget         = mFmod(rotationTarget, 360.0f);
    // Remove any negative rotation.
    if ( mLessThanZero(mRotationTarget) ) { mRotationTarget += 360.0f; }
    mRotationTargetAutoStop = autoStop;
    mRotationTargetCallback = callback;
    mRotationTargetSnap     = snap;
    mRotationTargetMargin   = targetMargin;
}


//-----------------------------------------------------------------------------
// Set Rotation Target Off.
//-----------------------------------------------------------------------------
void t2dPhysics::setRotationTargetOff( void )
{
    // Reset Rotation Target.
    mRotationTargetActive = false;
}


//-----------------------------------------------------------------------------
// Set Minimum Linear Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setMinLinearVelocity( const F32 minVelocity )
{
    // Set Minimum Linear Velocity.
    mMinLinearVelocity = getMin(mMaxLinearVelocity, minVelocity);
}

//-----------------------------------------------------------------------------
// Set Maximum Linear Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setMaxLinearVelocity( const F32 maxVelocity )
{
    // Set Maximum Linear Velocity.
    mMaxLinearVelocity = getMax(0.0f, maxVelocity);;
}


//-----------------------------------------------------------------------------
// Set Minimum Angular Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setMinAngularVelocity( const F32 minVelocity )
{
    // Set Minimum Angular Velocity.
    mMinAngularVelocity = getMin(mMaxAngularVelocity, minVelocity);
}

//-----------------------------------------------------------------------------
// Set Maximum Angular Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setMaxAngularVelocity( const F32 maxVelocity )
{
    // Set Maximum Angular Velocity.
    mMaxAngularVelocity = getMax(0.0f, maxVelocity);
}


//-----------------------------------------------------------------------------
// Set Fixed Force.
//-----------------------------------------------------------------------------
void t2dPhysics::setConstantForce( const t2dVector& force, bool gravitic )
{
    // Set Constant Force.
    mConstantForce = force;

    // Set Gravitic Constant Force Flag.
    mGraviticConstantForce = gravitic;

    // Usage Flag.
    mConstantForceActive = mNotZero(mConstantForce.lenSquared());
}


//-----------------------------------------------------------------------------
// Set Force Scale.
//-----------------------------------------------------------------------------
void t2dPhysics::setForceScale( const F32 forceScale )
{
    // Set Force Scale.
    mForceScale = forceScale;
}


//-----------------------------------------------------------------------------
// Set Dynamic Friction ( >= 0 ) - Can arguably by greater than 1!
//-----------------------------------------------------------------------------
void t2dPhysics::setDynamicFriction( const F32 dynamicFriction )
{
    // Sanity!
    AssertFatal( dynamicFriction >= 0.0f, "t2dPhysics::setDynamicFriction() - Invalid Dynamic Friction!" );

    // Set Friction.
    mCoefDynamicFriction = dynamicFriction;
}


//-----------------------------------------------------------------------------
// Set Restitution.
//-----------------------------------------------------------------------------
void t2dPhysics::setRestitution( const F32 restitution )
{
    // Sanity!
    AssertFatal( restitution >= 0.0f && restitution <= 1.0f, "t2dPhysics::setRestitution() - Invalid Restitution!" );

    // Set Restitution.
    mCoefRestitution = restitution;
}


//-----------------------------------------------------------------------------
// Set Density. (Kg/World-Unit^2).
//-----------------------------------------------------------------------------
void t2dPhysics::setDensity( const F32 density )
{
    // Sanity!
    AssertFatal( density >= 0.0f, "t2dPhysics::setDensity() - Invalid Density!" );

    // Set Density.
    mDensity = density;

    // Calculate Mass / Inertia.
    calculateMassInertia();
}


//-----------------------------------------------------------------------------
// Set Damping.
//-----------------------------------------------------------------------------
void t2dPhysics::setDamping( const F32 damping )
{
    // Sanity!
    AssertFatal( damping >= 0.0f, "t2dPhysics::setDamping() - Invalid Damping!" );

    // Set Damping.
    mDamping = damping;

    // Usage Flags.
    mDampingActive = mNotZero(mDamping);
}


//-----------------------------------------------------------------------------
// Set Auto Mass/Inertia.
//-----------------------------------------------------------------------------
void t2dPhysics::setAutoMassInertia( const bool status )
{
    // Set Auto Mass/Inertia.
    mAutoMassInertia = status;
}


//-----------------------------------------------------------------------------
// Set Immovable.
//-----------------------------------------------------------------------------
void t2dPhysics::setImmovable( const bool status )
{
    // Set Immovable.
    mImmovable = status;

    // Immovable?
    if ( mImmovable )
    {
        // Yes, so set at rest.
        setAtRest();
    }
}


//-----------------------------------------------------------------------------
// Set Mass.
//-----------------------------------------------------------------------------
void t2dPhysics::setMass( const F32 mass )
{
    // Set Mass.
    mMass = mass;

    // Calculate Inverse Mass.
    mInverseMass = mNotZero(mMass) ? 1.0f/mMass : 0.0f;
}


//-----------------------------------------------------------------------------
// Set Collision Detection Mode.
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionDetectionMode( const eCollisionDetection detectionMode )
{
    // Is the detection-mode valid?
    if ( detectionMode == T2D_DETECTION_INVALID )
    {
        // No, so warn.
        Con::warnf("t2dPhysics::setCollisionDetection() - Invalid collision-detection mode!");
        return;
    }

    // Set Collision Detection Mode.
    mCollisionDetectionMode = detectionMode;

    // Flag Spatial Dirty.
    setSpatialDirty();
}


//-----------------------------------------------------------------------------
// Set Collision Response Mode.
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionResponseMode( const eCollisionResponse responseMode )
{
    // Is the response-mode valid?
    if ( responseMode == T2D_RESPONSE_INVALID )
    {
        // No, so warn.
        Con::warnf("t2dPhysics::setCollisionResponse() - Invalid collision-response mode!");
        return;
    }

    // Set Collision Response Mode.
    mCollisionResponseMode = responseMode;
}


//-----------------------------------------------------------------------------
// Set Inertial Moment.
//-----------------------------------------------------------------------------
void t2dPhysics::setInertialMoment( const F32 moment )
{
    // Set Inertial Moment.
    mInertialMoment = moment;

    // Calculate Inverse Inertial Moment.
    mInverseInertialMoment = mNotZero(mInertialMoment) ? 1.0f/mInertialMoment : 0.0f;
}


//-----------------------------------------------------------------------------
// Reset Tick Positions.
//-----------------------------------------------------------------------------
void t2dPhysics::resetTickPosition( void )
{
    // Reset Position.
    mRenderTickPosition = mPreTickPosition = mPostTickPosition = getPosition();
}


//-----------------------------------------------------------------------------
// Reset Tick Rotations.
//-----------------------------------------------------------------------------
void t2dPhysics::resetTickRotation( void )
{
    // Reset Rotation.
    mRenderTickRotation = mPreTickRotation = mPostTickRotation = getRotation();
}


//-----------------------------------------------------------------------------
// Update Tick Position.
//-----------------------------------------------------------------------------
void t2dPhysics::updateTickPosition( void )
{
    // Store Pre Tick Position.
    mPreTickPosition = mPostTickPosition;

    // Store Current Tick Position.
    mPostTickPosition = getPosition();

    // Render Tick Position is at Pre-Tick Position.
    mRenderTickPosition = mPreTickPosition;
};


//-----------------------------------------------------------------------------
// Update Tick Rotation.
//-----------------------------------------------------------------------------
void t2dPhysics::updateTickRotation( void )
{
    // Store Pre Tick Rotation.
    mPreTickRotation = mPostTickRotation;

    // Store Current Tick Rotation.
    mPostTickRotation = getRotation();

    // Render Tick Rotation is at Pre-Tick Rotation.
    mRenderTickRotation = mPreTickRotation;

   // Interpolate along the shortest arc.
   if ( mPreTickRotation - mPostTickRotation > 180.0f )
   {
        mPreTickRotation -= 360;
   }
   else if ( mPostTickRotation - mPreTickRotation > 180.0f )
   {
        mPreTickRotation += 360;
   }
};


//-----------------------------------------------------------------------------
// Update Tick Position.
//-----------------------------------------------------------------------------
void t2dPhysics::updateTickPosition( const t2dVector& position )
{
    // Store Pre Tick Position.
    mPreTickPosition = mPostTickPosition;

    // Store Current Tick Position.
    mPostTickPosition = position;

    // Render Tick Position is at Pre-Tick Position.
    mRenderTickPosition = mPreTickPosition;

    // Set Position.
    setPosition( position );
};


//-----------------------------------------------------------------------------
// Update Tick Rotations.
//-----------------------------------------------------------------------------
void t2dPhysics::updateTickRotation( const F32& rotation )
{
   // Store Pre Tick Rotation.
   mPreTickRotation = mPostTickRotation;
   
   // Store Current Tick Rotation.
   mPostTickRotation = rotation;
   
   // Render Tick Rotation is at Pre-Tick Rotation.
   mRenderTickRotation = mPreTickRotation;
 
   // Interpolate along the shortest arc.
   if ( mPreTickRotation - mPostTickRotation > 180.0f )
   {
        mPreTickRotation -= 360;
   }
   else if ( mPostTickRotation - mPreTickRotation > 180.0f )
   {
        mPreTickRotation += 360;
   }

   // Set Rotation.
   setRotation( rotation );
};


//-----------------------------------------------------------------------------
// Interpolate Tick Position.
//-----------------------------------------------------------------------------
void t2dPhysics::interpolateTickPosition( const F32 timeDelta )
{
    // Calculate Render Tick Position.
    mRenderTickPosition = (mPreTickPosition * timeDelta) + ((1.0f-timeDelta) * mPostTickPosition);
}


//-----------------------------------------------------------------------------
// Interpolate Tick Rotation.
//-----------------------------------------------------------------------------
void t2dPhysics::interpolateTickRotation( const F32 timeDelta )
{
    // Calculate Render Tick Rotation.
    mRenderTickRotation = mFmod( (mPreTickRotation * timeDelta) + ((1.0f-timeDelta) * mPostTickRotation), 360.0f );

    // Update Rotation Matrix.
    updateRotationMatrix( mRenderTickRotation );
}


//-----------------------------------------------------------------------------
// Update Rotation Matrix.
//-----------------------------------------------------------------------------
void t2dPhysics::updateRotationMatrix( F32 rotation )
{
    // Zero Rotation?
    if ( mIsZero(rotation) )
    {
        // Yes, so we can reset to identity.
        mMatrixRotation.identity();
        mInverseMatrixRotation.identity();
    }
    else
    {
        // No, so remove any negative rotation.
        if ( mLessThanZero(rotation) )
        {
            rotation += 360.0f;
        }

        // Calculate Rotation Matrix.
        mMatrixRotation.orientate( rotation );

        // Calculate Inverse Rotation Matrix.
        mInverseMatrixRotation = mMatrixRotation;
        mInverseMatrixRotation.transpose();
    }
}


//-----------------------------------------------------------------------------
// Set Collision Poly Primitive
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionPolyPrimitive( const U32 polyVertexCount )
{
    // Check for Maximum Polygon Edges.
    if ( polyVertexCount > MAX_COLLISION_POLY_VERTEX )
    {
        Con::warnf("t2dPhysics::setCollisionPolyPrimitive() - Cannot generate a %d edged collision polygon.  Maximum is %d!", polyVertexCount, MAX_COLLISION_POLY_VERTEX);
        return;
    }

    // Clear Polygon Basis List.
    mCollisionPolyBasisList.clear();
    mCollisionPolyBasisList.setSize( polyVertexCount );

    // Point?
    if ( polyVertexCount == 1 )
    {
        // Set Polygon Point.
        mCollisionPolyBasisList[0].set(0.0f, 0.0f);
    }
    // Special-Case Quad?
    else if ( polyVertexCount == 4 )
    {
        // Yes, so set Quad.
        mCollisionPolyBasisList[0].set(-1.0f, -1.0f);
        mCollisionPolyBasisList[1].set(+1.0f, -1.0f);
        mCollisionPolyBasisList[2].set(+1.0f, +1.0f);
        mCollisionPolyBasisList[3].set(-1.0f, +1.0f);
    }
    else
    {
        // No, so calculate Regular (Primitive) Polygon Stepping.
        //
        // NOTE:-   The polygon sits on an circle that subscribes the interior
        //          of the collision box.
        F32 angle = M_PI_F / polyVertexCount;
        F32 angleStep = M_2PI_F / polyVertexCount;

        // Calculate Polygon.
        for ( U32 n = 0; n < polyVertexCount; n++ )
        {
            // Calculate Angle.
            angle += angleStep;
            // Store Polygon Vertex.
            mCollisionPolyBasisList[n].set(mCos(angle), mSin(angle));
        }
    }

    // Generation Collision Poly.
    generateCollisionPoly();
}

bool t2dPhysics::isPolyConvex( const U32 polyVertexCount, const t2dVector* pPoly, bool& sign )
{
    // Reset Sign Flag.
    sign = false;
    // Assume the polygon is convex!
    bool convex = true;

    // Check that the polygon is convex.
    // NOTE:-   We'll iterate the polygon and check that each consecutive edge-pair
    //          maintains the same perp-dot-product sign; if not then it's not convex.
    for ( U32 n0 = 0; n0 < polyVertexCount; n0++ )
    {
        // Calculate next two vertex indices.
        U32 n1 = (n0+1)%polyVertexCount;
        U32 n2 = (n0+2)%polyVertexCount;
        // Calculate Edges.
        const t2dVector e0 = pPoly[n1] - pPoly[n0];
        const t2dVector e1 = pPoly[n2] - pPoly[n1];
        // Calculate Perpendicular Dot-Product for edges.
        F32 perpDotEdge = e0.getPerp() * e1;
        // Have we processed the first vertex?
        if ( n0 > 0 )
        {
            // Yes, so is the PDE the same sign?
            if ( sign != ( perpDotEdge > 0.0f ) )
            {
                // No, so polygon is *not* convex!
                convex = false;
                break;
            }
        }
        // No, so fetch sign.
        else 
        {
            // Calculate sign flag.
            sign = ( perpDotEdge > 0.0f );
        }
    }

    return convex;
}

//-----------------------------------------------------------------------------
// Set Collision Poly Primitive
//-----------------------------------------------------------------------------
void t2dPhysics::setCollisionPolyCustom( const U32 polyVertexCount, const t2dVector* pCustomPolygon )
{
    // Check for Maximum Polygon Edges.
    if ( polyVertexCount > MAX_COLLISION_POLY_VERTEX )
    {
        Con::warnf("t2dPhysics::setCollisionPolyCustom() - Cannot generate a %d edged collision polygon.  Maximum is %d!", polyVertexCount, MAX_COLLISION_POLY_VERTEX);
        return;
    }

    // Clear Polygon Basis List.
    mCollisionPolyBasisList.clear();
    mCollisionPolyBasisList.setSize( polyVertexCount );

    // Validate Polygon Vertices.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        // Store Polygon Vertex.
        mCollisionPolyBasisList[n] = pCustomPolygon[n];
    }

    bool sign;
    bool convex = isPolyConvex( polyVertexCount, mCollisionPolyBasisList.address(), sign );

    // Convex?
    if ( !convex )
    {
        // No, so warn.
        Con::warnf("t2dPhysics::setCollisionPolyCustom() - Polygon is not convex!  Defaulting to Quad! (%s)", pCustomPolygon);
        // Default to Quad!
        setCollisionPolyPrimitive( 4 );
        // Finish Here.
        return;
    }

    // Counter-clockwise?
    if ( !sign )
    {
       // Yes,so warn... Don't really think we need this. It spams the console when collision poly editing. ADL.
       //Con::warnf("t2dPhysics::setCollisionPolyCustom() - Polygon is defined counter-clockwise! Changing to clockwise!");
       // ...and revert the polygon
       FrameTemp<t2dVector> tempList( polyVertexCount );
       for( int i = 0; i < polyVertexCount; i++ )
          tempList[(polyVertexCount-1)-i] = mCollisionPolyBasisList[i];

       for( int i = 0; i < polyVertexCount; i++ )
          mCollisionPolyBasisList[i] = tempList[i];    
    }

    // Generation Collision Poly.
    generateCollisionPoly();
}


//-----------------------------------------------------------------------------
// Generate Collision Circle.
//-----------------------------------------------------------------------------
void t2dPhysics::generateCollisionCircle( void )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_generateCollisionCircle);
#endif

    // Get Half-Size Area.
    t2dVector circleArea = getHalfSize();

    // Superscribed Circle?
    if ( getCollisionCircleSuperscribed() )
    {
        // Yes, so calculate Superscribed Circle Collision Area.
        F32 minRadii = ((circleArea.mX < circleArea.mY) ? circleArea.mX/circleArea.mY : circleArea.mY/circleArea.mX);
        circleArea *= getCollisionCircleScale() * mSqrt(minRadii*minRadii + 1.0f);
    }
    else
    {
        // No, so calculate Normal Circle Collision Area.
        circleArea *= getCollisionCircleScale();
    }

    // Calculate Collision Circle Radii.
    mCollisionCircleRadii = ((circleArea.mX > circleArea.mY) ? circleArea.mX : circleArea.mY);

    // Flag Spatial Dirty.
    setSpatialDirty();

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();    // T2D_t2dPhysics_generateCollisionCircle
#endif
}



//-----------------------------------------------------------------------------
// Generate Collision Polygon.
//-----------------------------------------------------------------------------
void t2dPhysics::generateCollisionPoly( void )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_generateCollisionPoly);
#endif

    // Fetch Polygon Vertex Count.
    const U32 polyVertexCount = mCollisionPolyBasisList.size();

    // Fetch Flip Settings
    const bool flipX = dynamic_cast<t2dSceneObject*>(mRefParentObject) ? static_cast<t2dSceneObject*>(mRefParentObject)->getFlipX() : false;
    const bool flipY = dynamic_cast<t2dSceneObject*>(mRefParentObject) ? static_cast<t2dSceneObject*>(mRefParentObject)->getFlipY() : false;

    // Process Collision Polygon (if we've got one).
    if ( polyVertexCount > 0 )
    {
        // Sanity!
        AssertFatal( polyVertexCount <= MAX_COLLISION_POLY_VERTEX, "t2dPhysics::generateCollisionPoly() - Polygon Vertex count out of range!" );

//		S32 iSize = mCollisionPolyLocalList.size();
//		if( iSize > 4 )
//		{
//			// RKS:  I added this to test that memory crash that has been happening. "LEAKS" traced a memory leak here. (happens in VectorResize)
//			// setCollisionPolyPrimitive->generateCollisionPoly->setSize->resize->VectorResize
//			// I suspect that there is some shit allocated already, but "clear" below does not clear the memory.
//			//printf( "============ t2dPhysics::generateCollisionPoly - WARNING: CLEARING THE LOCAL COLLISION POLY WHILE IT STILL HAS ELEMENTS ================= \n" );
//			printf( "============ t2dPhysics::generateCollisionPoly - %s - Collision polygon size is: %d \n", mRefParentObject ? mRefParentObject->getName() : "", iSize );
//			
//			int stophere = 0;
//			stophere++;
//		}
		
		
        // Clear Polygon List.
        mCollisionPolyLocalList.clear();
		//mCollisionPolyLocalList.resize( 0 ); // RKS:  I replaced the clear() call with this resize() call because it COULD have been causing a serious memory leak.
		//mCollisionPolyLocalList.setSize( 0 ); // RKS:  I replaced the clear() call with this resize() call because it COULD have been causing a serious memory leak. (resize is protected)
		//mCollisionPolyLocalList.resize_public( 0 ); // RKS:  I replaced the clear() call with this resize() call because it COULD have been causing a serious memory leak. (resize is protected)
        mCollisionPolyLocalList.setSize( polyVertexCount );

        // Calculate Polygon Half-Size.
        t2dVector polyHalfSize( mHalfSize.mX * mCollisionPolyScale.mX * (flipX ? -1.0f : 1.0f),
                                mHalfSize.mY * mCollisionPolyScale.mY * (flipY ? -1.0f : 1.0f) );

        // Keep the Poly clock-wise
        const bool reversedOrder = (flipX != flipY);

        if( reversedOrder )
        {
           for (U32 i=0; i < polyVertexCount; i++)
           {
              mCollisionPolyLocalList[(polyVertexCount-1)-i].mX = mCollisionPolyBasisList[i].mX * polyHalfSize.mX;
              mCollisionPolyLocalList[(polyVertexCount-1)-i].mY = mCollisionPolyBasisList[i].mY * polyHalfSize.mY;
           }
        }
        else
        {
           for (U32 i=0; i < polyVertexCount; i++)
           {
              mCollisionPolyLocalList[i].mX = mCollisionPolyBasisList[i].mX * polyHalfSize.mX;
              mCollisionPolyLocalList[i].mY = mCollisionPolyBasisList[i].mY * polyHalfSize.mY;
           }
        }

        // Calculate Mass / Inertia.
        calculateMassInertia();
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();    // T2D_t2dPhysics_generateCollisionPoly
#endif
}



//-----------------------------------------------------------------------------
// Calculate Mass / Inertia.
//-----------------------------------------------------------------------------
void t2dPhysics::calculateMassInertia( void )
{
    // Ignore if we're not automatically calculating mass/inertia.
    if ( !mAutoMassInertia )
        return;

    // Have we got some density and a collision-poly?
    if ( mDensity > 0.0f && getCollisionPolyCount() ) 
    {
        // *****************************************************************************************************************
        // Calculate Mass.
        // Ref: http://www.physicsforums.com/showthread.php?s=e251fddad79b926d003e2d4154799c14&t=25293&page=2&pp=15
        // *****************************************************************************************************************

        // Is this a point?
        if ( getCollisionPolyCount() == 1 )
        {
            // Yes, so choose a default mass.
            setMass( 5.0f * mDensity ); 
        }
        else
        {
            // No, so is this a line?
            if ( getCollisionPolyCount() == 2 )
            {
                // Yes, so choose a default mass.
                setMass( 10.0f );
            }
            else
            {
                // No, so calculate polygon mass...

                // Reset Mass.
                mMass = 0.0f;

                // Get Collision Polygon.
                const t2dVector* pCollisionPoly = getCollisionPolyLocal();

                // Interate Polygon.
                for ( U32 j = getCollisionPolyCount()-1, i = 0; i < getCollisionPolyCount(); j = i, i++ )
                    // Summate Mass (with edge-points).
                    mMass += mFabs(pCollisionPoly[j] ^ pCollisionPoly[i]);
            }

            // Associate with Density.
            setMass( mMass * mDensity * 0.5f );
        }

        // *****************************************************************************************************************
        // Calculate Inertia..
        // Ref: http://www.physicsforums.com/showthread.php?s=e251fddad79b926d003e2d4154799c14&t=25293&page=2&pp=15
        // *****************************************************************************************************************

        // Is this a point?
        if ( getCollisionPolyCount() == 1 )
        {
            // Yes, so reset inertial moment.
            setInertialMoment( 0.0f );
        }
        else
        {
            // No, so calculate polygon inertia...

            // Reset Inertia Summates.
            F32 iDenom = 0.0f;
            F32 iNumer = 0.0f;

            // Get Collision Polygon.
            const t2dVector* pCollisionPoly = getCollisionPolyLocal();

            // Interate Polygon.
            for ( U32 j = getCollisionPolyCount()-1, i = 0; i < getCollisionPolyCount(); j = i, i++ )
            {
                // Fetch Edge Points.
                const t2dVector& p0 = pCollisionPoly[j];
                const t2dVector& p1 = pCollisionPoly[i];

                // Calculate Inertial Components.
                F32 c1 = mFabs(p0 ^ p1);
                F32 c2 = (p1*p1 + p1*p0 + p0*p0);

                // Summate Inertia.
                iDenom += (c1*c2);
                iNumer += c1;
            }

            // Associate with Mass.
            setInertialMoment( (mMass/6.0f) * (iDenom/iNumer) );
        }
    }
    else
    {
        // No, so reset Mass/Inertia.
        setMass( 0.0f );
        setInertialMoment( 0.0f );
    }
}


//-----------------------------------------------------------------------------
// Add Net Linear Force.
//-----------------------------------------------------------------------------
void t2dPhysics::addNetLinearForce( const t2dVector& force )
{
    // Ignore forces on immovable objects!
    if ( getImmovable() ) return;

    // Set Net-Linear Velocity.
    setNetLinearVelocity( getNetLinearVelocity() + force * mForceScale );
}


//-----------------------------------------------------------------------------
// Add Net Angular Force.
//-----------------------------------------------------------------------------
void t2dPhysics::addNetAngularForce( const F32& force )
{
    // Ignore forces on immovable objects!
    if ( getImmovable() ) return;

    // SetNet Angular Velocity.
    setNetAngularVelocity( getNetAngularVelocity() - (force * mForceScale) );
}

//-----------------------------------------------------------------------------
// Add Net Torque Force.
//-----------------------------------------------------------------------------
void t2dPhysics::addNetTorqueForce( const t2dVector& force, const t2dVector& forcePosition )
{
    // Ignore forces on immovable objects!
    if ( getImmovable() ) return;

    // Set Net Linear Velocity.
    setNetLinearVelocity( (getNetLinearVelocity() + force * mForceScale) );
    // Set Net Angular Velocity.
    setNetAngularVelocity( getNetAngularVelocity() - (( forcePosition - getPosition() ) ^ (force * mForceScale)) );
}


//-----------------------------------------------------------------------------
// Add Gross Linear Force.
//-----------------------------------------------------------------------------
void t2dPhysics::addGrossLinearForce( const t2dVector& force )
{
    // Ignore forces on immovable objects!
    if ( getImmovable() ) return;

    // Get Linear Velocity.
    t2dVector linearVelocity = getGrossLinearVelocity();
    // Add Force to Linear Velocity.
    linearVelocity += force * mForceScale;
    // Set Gross Linear Velocity.
    setGrossLinearVelocity( linearVelocity );
}


//-----------------------------------------------------------------------------
// Add Gross Angular Force.
//-----------------------------------------------------------------------------
void t2dPhysics::addGrossAngularForce( const F32& force )
{
    // Ignore forces on immovable objects!
    if ( getImmovable() ) return;

    // Add Point force to Gross Angular Velocity.
    setGrossAngularVelocity( getGrossAngularVelocity() - (force * mForceScale) );
}


//-----------------------------------------------------------------------------
// Add Gross Torque Force.
//-----------------------------------------------------------------------------
void t2dPhysics::addGrossTorqueForce( const t2dVector& force, const t2dVector& forcePosition )
{
    // Ignore forces on immovable objects!
    if ( getImmovable() ) return;

    // Set Gross Linear Velocity.
    setGrossLinearVelocity( getGrossLinearVelocity() + (force * mForceScale) );
    // Set Net Angular Velocity.
    setGrossAngularVelocity( getGrossAngularVelocity() - (( forcePosition - getPosition() ) ^ (force * mForceScale)) );
}


//-----------------------------------------------------------------------------
// Reset All Angular Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::resetAngularVelocity( void )
{
    // Reset Angular Velocity.
    mNetAngularVelocity = mGrossAngularVelocity = 0.0f;

    // Usage Flags.
    mNetAngularVelocityActive = mGrossAngularVelocityActive = false;
}


//-----------------------------------------------------------------------------
// Reset All Linear Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::resetLinearVelocity( void )
{
    // Reset Linear Velocity.
    mNetLinearVelocity.zero();
    mGrossLinearVelocity.zero();

    // Usage Flags.
    mNetLinearVelocityActive = mGrossLinearVelocityActive = false;
}


//-----------------------------------------------------------------------------
// Reset Net Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::resetNetVelocity( void )
{
    // Reset Net Velocities.
    mNetLinearVelocity.zero();
    mNetAngularVelocity = 0.0f;

    // Usage Flags.
    mNetLinearVelocityActive = mNetAngularVelocityActive = false;
}


//-----------------------------------------------------------------------------
// Reset Gross Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::resetGrossVelocity( void )
{
    // Reset Gross Velocities.
    mGrossLinearVelocity.zero();
    mGrossAngularVelocity = 0.0f;

    // Usage Flags.
    mGrossLinearVelocityActive = mGrossAngularVelocityActive = false;
}

//-----------------------------------------------------------------------------
// Set at Rest.
//-----------------------------------------------------------------------------
void t2dPhysics::setAtRest( void )
{
    // Reset Gross Velocity.
    resetGrossVelocity();

    // Reset Net Velocity.
    resetNetVelocity();
}


//-----------------------------------------------------------------------------
// Set Net Linear Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setNetLinearVelocity( const t2dVector& velocity )
{
    // Set Linear Velocity.
    mNetLinearVelocity = velocity;

    // Usage Flag.
    mNetLinearVelocityActive = (mNetLinearVelocity.len() > T2D_CONST_EPSILON);

    // Check for Zero-Clamp.
    if ( !mNetLinearVelocityActive )
        mNetLinearVelocity.zero();
}


//-----------------------------------------------------------------------------
// Set Net Angular Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setNetAngularVelocity( const F32& velocity )
{
    // Fetch Angular Magnitude.
    mNetAngularVelocity = velocity;

    // Usage Flag.
    mNetAngularVelocityActive = mNotZero( mNetAngularVelocity );

    // Check for Zero-Clamp.
    if ( !mNetAngularVelocityActive )
        // Reset Angular Velocity.
        mNetAngularVelocity = 0.0f;
}


//-----------------------------------------------------------------------------
// Set Gross Linear Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setGrossLinearVelocity( const t2dVector& velocity )
{
    // Set Linear Velocity.
    mGrossLinearVelocity = velocity;

    // Fetch Linear Magnitude.
    F32 linearMagnitude = mGrossLinearVelocity.len();

    // Clamp Linear Magnitude.
    if ( linearMagnitude > mMaxLinearVelocity )
        mGrossLinearVelocity.normalise( mMaxLinearVelocity );
    else if ( linearMagnitude < mMinLinearVelocity )
        mGrossLinearVelocity.zero();

    // Usage Flag.
    mGrossLinearVelocityActive = (mGrossLinearVelocity.len() > T2D_CONST_EPSILON);

    // Check for Zero-Clamp.
    if ( !mGrossLinearVelocityActive )
        mGrossLinearVelocity.zero();
}


//-----------------------------------------------------------------------------
// Set Gross Angular Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::setGrossAngularVelocity( const F32& velocity )
{
    // Set Gross Angular Velocity.
    mGrossAngularVelocity = velocity;

    // Fetch Angular Magnitude.
    F32 angularMagnitude = mFabs(mGrossAngularVelocity);

    // Clamp Angular Velocity.
    if ( angularMagnitude > mMaxAngularVelocity )
        mGrossAngularVelocity = (velocity < 0.0f) ? -mMaxAngularVelocity : mMaxAngularVelocity;
    else if ( angularMagnitude < mMinAngularVelocity )
        mGrossAngularVelocity = 0.0f;

    // Usage Flag.
    mGrossAngularVelocityActive = mNotZero( mGrossAngularVelocity );

    // Check for Zero-Clamp.
    if ( !mGrossAngularVelocityActive )
        // Reset Angular Velocity.
        mGrossAngularVelocity = 0.0f;
}


//-----------------------------------------------------------------------------
// Calculate Collision.
//
// NOTE:-
//
//  The cases for collision we're considering here are:-
//
//  1) Objects Overlap @ zero-time.
//  2) Objects are Disjoint but collide, forward in time.
//  3) Objects are Disjoint and do not collide, forward in time.
//
//-----------------------------------------------------------------------------
bool t2dPhysics::calculateCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus )
{
    // Initially Invalidate Collision.
    pCollisionStatus->mValidCollision = false;

    // Reset Handled.
    pCollisionStatus->mHandled = false;

    // Ignore invalid times.
    if ( elapsedTime < 0.0f )
    {
        // Warn.
        Con::warnf("t2dPhysics::calculateCollision() - Cannot process negative elapsed time! (%f)", elapsedTime);
        return false;
    }

    // Fetch Source/Destination Physics.
    t2dPhysics& srcPhysics = *pCollisionStatus->mpSrcPhysics;
    t2dPhysics& dstPhysics = *pCollisionStatus->mpDstPhysics;

    // Cannot proceed without parent initialisation.
    if ( !srcPhysics.isInitialised() || !dstPhysics.isInitialised()  )
    {
        // Warn.
        Con::warnf("t2dPhysics::calculateCollision() - Cannot proceed without initialisation!");
        return false;
    }

    // Handle Collision Detection Mode.
    switch( srcPhysics.getCollisionDetectionMode() )
    {
        // **************************************************
        // Swept-Circle->Swept-Polygon.
        // **************************************************
        case T2D_DETECTION_FULL:
        {
            // Handle Destination Detection Mode.
            switch( dstPhysics.getCollisionDetectionMode() )
            {
                // Swept-Circle->Swept-Polygon.
                case T2D_DETECTION_FULL:
                {
                    // Do Swept-Circle to Circle Collision.
                    if ( !sweptCircleToCircleCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Circle to Polygon Collision.
                    if ( !sweptCircleToPolyCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Poly to Circle Collision.
                    if ( !sweptPolyToCircleCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Poly to Polygon Collision.
                    return sweptPolyToPolyCollision( elapsedTime, pCollisionStatus );

                } break;

                // Swept-Circle Only.
                case T2D_DETECTION_CIRCLE_ONLY:
                {
                    // Do Swept-Circle to Circle Collision.
                    if ( !sweptCircleToCircleCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Poly to Circle Collision.
                    return sweptPolyToCircleCollision( elapsedTime, pCollisionStatus );

                } break;

                // Swept-Polygon Only.
                case T2D_DETECTION_POLY_ONLY:
                {
                    // Do Swept-Circle to Polygon Collision.
                    if ( !sweptCircleToPolyCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Poly to Polygon Collision.
                    return sweptPolyToPolyCollision( elapsedTime, pCollisionStatus );

                } break;
            };

        } break;

        // **************************************************
        // Swept-Circle Only.
        // **************************************************
        case T2D_DETECTION_CIRCLE_ONLY:
        {
            // Handle Destination Detection Mode.
            switch( dstPhysics.getCollisionDetectionMode() )
            {
                // Swept-Circle->Swept-Polygon.
                case T2D_DETECTION_FULL:
                {
                    // Do Swept-Circle to Circle Collision.
                    if ( !sweptCircleToCircleCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Circle to Polygon Collision.
                    return sweptCircleToPolyCollision( elapsedTime, pCollisionStatus );

                } break;

                // Swept-Circle Only.
                case T2D_DETECTION_CIRCLE_ONLY:
                {
                    // Do Swept-Circle to Circle Collision Only.
                    return sweptCircleToCircleCollision( elapsedTime, pCollisionStatus );

                } break;

                // Swept-Polygon Only.
                case T2D_DETECTION_POLY_ONLY:
                {
                    // Do Swept-Circle to Polygon Collision Only.
                    return sweptCircleToPolyCollision( elapsedTime, pCollisionStatus );

                } break;
            };

        } break;

        // **************************************************
        // Swept-Polygon Only.
        // **************************************************
        case T2D_DETECTION_POLY_ONLY:
        {
            // Handle Destination Detection Mode.
            switch( dstPhysics.getCollisionDetectionMode() )
            {
                // Swept-Circle->Swept-Polygon.
                case T2D_DETECTION_FULL:
                {
                    // Do Swept-Polygon to Circle Collision.
                    if ( !sweptPolyToCircleCollision( elapsedTime, pCollisionStatus ) )
                        // No collision!
                        return false;

                    // Do Swept-Polygon to Polygon Collision.
                    return sweptPolyToPolyCollision( elapsedTime, pCollisionStatus );

                } break;

                // Swept-Circle Only.
                case T2D_DETECTION_CIRCLE_ONLY:
                {
                    // Do Swept-Polygon to Circle Collision Only.
                    return sweptPolyToCircleCollision( elapsedTime, pCollisionStatus );

                } break;

                // Swept-Circle Only.
                case T2D_DETECTION_POLY_ONLY:
                {
                    // Do Swept-Polygon to Polygon Collision.
                    return sweptPolyToPolyCollision( elapsedTime, pCollisionStatus );

                } break;
            };

        } break;

        // **************************************************
        // Custom.
        //
        // NOTE:-   Custom collision detection methods should implement their own
        //          checking against stock detection methods for the destination
        //          object although this is not compulsary.
        //
        // **************************************************************************
        case T2D_DETECTION_CUSTOM:
        {
            // Sanity.
            AssertFatal(pCollisionStatus->mSrcObject, "t2dPhysics::calculateCollision() - No Source Object for 'T2D_DETECTION_CUSTOM'!");

            // Call Custom Collision Detection.
            return pCollisionStatus->mSrcObject->onCustomCollisionDetection( elapsedTime, pCollisionStatus );

        } break;
    };

    // Sanity.
    AssertFatal(false, "t2dPhysics::calculateCollision() - Invalid Collision Detection Mode!");

    // Return No Collision (shouldn't get here!).
    return false;
}


//-----------------------------------------------------------------------------
// Swept Polygon to Polygon Collision Check.
//-----------------------------------------------------------------------------
bool t2dPhysics::sweptPolyToPolyCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus )
{
    // Invalidate Collision.
    pCollisionStatus->mValidCollision = false;

    // Set Collision Integration Interval.
    pCollisionStatus->mFullTimeStep = elapsedTime;

    // Fetch Source/Destination Physics.
    const t2dPhysics& srcPhysics = *pCollisionStatus->mpSrcPhysics;
    const t2dPhysics& dstPhysics = *pCollisionStatus->mpDstPhysics;

    // Check for bad polygons.
    const t2dVector*    srcPoly         = srcPhysics.getCollisionPolyLocal();
    const t2dVector*    dstPoly         = dstPhysics.getCollisionPolyLocal();
    if ( !srcPoly || !dstPoly )
       return false;


    // Cannot collide points with points!
    U32                 srcPolyCount    = srcPhysics.getCollisionPolyCount();
    U32                 dstPolyCount    = dstPhysics.getCollisionPolyCount();
    if ( srcPolyCount < 1 && dstPolyCount < 1 )
       return false;

    // Pre-fetch both physics parameters.
    //
    // Source...    
    const t2dVector     srcPosition     = srcPhysics.getPosition();
    const t2dVector     srcVelocity     = srcPhysics.getGrossLinearVelocity();
    t2dMatrix           srcRotation     = srcPhysics.getRotationMatrix();
    //
    // Destination...
    const t2dVector     dstPosition     = dstPhysics.getPosition();
    const t2dVector     dstVelocity     = dstPhysics.getGrossLinearVelocity();
    t2dMatrix           dstRotation     = dstPhysics.getRotationMatrix();



    // Respaced Reference.
    t2dMatrix refLocalRotation  = srcRotation ^ dstRotation;
    t2dVector refLocalOffset    = (srcPosition - dstPosition) ^ dstRotation;
    t2dVector refLocalVelocity  = (srcVelocity - dstVelocity) ^ dstRotation;

    // Seperation Axii.
    static t2dVector vertexAxis[MAX_COLLISION_POLY_VERTEX*2];
    static F32  timeAxis[MAX_COLLISION_POLY_VERTEX*2];

    // Reset Axes Count.
    U32 axesCount = 0;

    // Square Velocity.
    const F32 refVelSqr = refLocalVelocity * refLocalVelocity;

    // Set Axis.
    vertexAxis[axesCount] = t2dVector( -refLocalVelocity.mY, refLocalVelocity.mX );

    // Ignore small velocities!
    if ( refVelSqr > T2D_CONST_EPSILON )
    {
        // Check Interval Intersection.
        if ( !checkIntervalIntersection(    srcPoly, srcPolyCount,
                                            dstPoly, dstPolyCount,
                                            vertexAxis[axesCount],
                                            refLocalOffset, refLocalVelocity, refLocalRotation,
                                            timeAxis[axesCount], pCollisionStatus->mFullTimeStep ) )
                // No Collision!
                return false;

        // Next Axes.
        axesCount++;
    }

    // Test Seperation Axes for Source Object.
    // NOTE:- We ignore if it's a point!
    if ( srcPolyCount > 1 )
    {
        // Interate Polygon.
        for ( U32 j = srcPolyCount-1, i = 0; i < srcPolyCount; j = i, i++ )
        {
            // Fetch Edge.
            t2dVector dP = srcPoly[i] - srcPoly[j];

            // Set Axis.
            vertexAxis[axesCount] = t2dVector( -dP.mY, dP.mX ) * refLocalRotation;

            // Check Interval Intersection.
            if ( !checkIntervalIntersection(    srcPoly, srcPolyCount,
                                                dstPoly, dstPolyCount,
                                                vertexAxis[axesCount],
                                                refLocalOffset, refLocalVelocity, refLocalRotation,
                                                timeAxis[axesCount], pCollisionStatus->mFullTimeStep ) )
                    // No Collision!
                    return false;

            // Next Axes.
            axesCount++;
        }
    }

    // Test Seperation Axes for Destination Object.
    // NOTE:- We ignore if it's a point!
    if ( dstPolyCount > 1 )
    {
        // Interate Polygon.
        for ( U32 j = dstPolyCount-1, i = 0; i < dstPolyCount; j = i, i++ )
        {
            // Fetch Edge.
            t2dVector dP = dstPoly[i] - dstPoly[j];

            // Set Axis.
            vertexAxis[axesCount] = t2dVector( -dP.mY, dP.mX );

            // Check Interval Intersection.
            if ( !checkIntervalIntersection(    srcPoly, srcPolyCount,
                                                dstPoly, dstPolyCount,
                                                vertexAxis[axesCount],
                                                refLocalOffset, refLocalVelocity, refLocalRotation,
                                                timeAxis[axesCount], pCollisionStatus->mFullTimeStep ) )
                    // No Collision!
                    return false;

            // Next Axes.
            axesCount++;
        }
    }

    // Test Special-Case for Segments for Destination Object.
    if ( dstPolyCount == 2 )
    {
        // Set Axis.
        vertexAxis[axesCount] = (dstPoly[1] - dstPoly[0]);

        // Check Interval Intersection.
        if ( !checkIntervalIntersection(    srcPoly, srcPolyCount,
                                            dstPoly, dstPolyCount,
                                            vertexAxis[axesCount],
                                            refLocalOffset, refLocalVelocity, refLocalRotation,
                                            timeAxis[axesCount], pCollisionStatus->mFullTimeStep ) )
                // No Collision!
                return false;

        // Next Axes.
        axesCount++;
    }

    // Test Special-Case for Segments for Source Object.
    if ( srcPolyCount == 2 )
    {
        // Set Axis.
        vertexAxis[axesCount] = (srcPoly[1] - srcPoly[0]) * refLocalRotation;

        // Check Interval Intersection.
        if ( !checkIntervalIntersection(    srcPoly, srcPolyCount,
                                            dstPoly, dstPolyCount,
                                            vertexAxis[axesCount],
                                            refLocalOffset, refLocalVelocity, refLocalRotation,
                                            timeAxis[axesCount], pCollisionStatus->mFullTimeStep ) )
                // No Collision!
                return false;

        // Next Axes.
        axesCount++;
    }

    // Find Minimum Seperation Distance.
    if ( !findMinimumSeperation( vertexAxis, timeAxis, axesCount, pCollisionStatus->mCollisionNormal, pCollisionStatus->mCollisionTimeReal ) )
        // No Collision!
        return false;

    // Clamp Collision Margin.
    if ( pCollisionStatus->mCollisionTimeReal >= 0.0f && pCollisionStatus->mCollisionTimeReal <= T2D_CONST_COLLISION_MARGIN )
    {
        pCollisionStatus->mCollisionTimeReal = 0.0f;
    }

    // Calculate Normalised Collision Time.
    pCollisionStatus->mCollisionTimeNorm = pCollisionStatus->mCollisionTimeReal / pCollisionStatus->mFullTimeStep;

    // Set Overlapped Status.
    pCollisionStatus->mOverlapped = (pCollisionStatus->mCollisionTimeReal < 0.0f);

    // Respace Normal.
    pCollisionStatus->mCollisionNormal *= dstRotation;
    // Renormalise.
    pCollisionStatus->mCollisionNormal.normalise();


    // We cannot assume that the collision polys contain the 0,0 origin point.
    // So compute the offset between the actual poly centers for testing the 
    // normal direction.
    S32 plyItr;
    t2dVector polyCenter(0, 0);
    for (plyItr = 0; plyItr < srcPolyCount; plyItr++)
       polyCenter += srcPoly[plyItr];
    polyCenter *= 1.0f / (F32)srcPolyCount;
    t2dVector polyOffset = srcPosition + (polyCenter ^ srcRotation);
    
    polyCenter.set(0, 0);
    for (plyItr = 0; plyItr < dstPolyCount; plyItr++)
       polyCenter += dstPoly[plyItr];
    polyCenter *= 1.0f / (F32)dstPolyCount;
    polyOffset -= dstPosition + (polyCenter ^ dstRotation);

    // Make sure the collision polygons are pushed away.
    // NOTE:- This is the overlap case.
    if ( (pCollisionStatus->mCollisionNormal * polyOffset) < 0.0f )
        pCollisionStatus->mCollisionNormal = -pCollisionStatus->mCollisionNormal;


// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_findContactPoints);
#endif
    // Find the Contact Point(s).
    if ( !findContactPoints(    srcPoly, srcPolyCount, srcPosition, srcVelocity, srcRotation,
                                dstPoly, dstPolyCount, dstPosition, dstVelocity, dstRotation,
                                pCollisionStatus ) )
    {
            // T2D Debug Profiling.
            #ifdef TORQUE_ENABLE_PROFILER
                    PROFILE_END();   // T2D_t2dPhysics_findContactPoints
            #endif
            // No Support Points!
            return false;
    }
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_findContactPoints
#endif

    // Validate Collision.
    pCollisionStatus->mValidCollision = true;

    // Successful Collision!
    return true;
}


//-----------------------------------------------------------------------------
// Check Interval Intersections.
//
// NOTE:-   This uses the "seperating axis" theorem.
//-----------------------------------------------------------------------------
bool t2dPhysics::checkIntervalIntersection(const t2dVector* srcPoly, U32 srcPolyCount,
                                            const t2dVector* dstPoly, U32 dstPolyCount,
                                            const t2dVector& vertexAxis,
                                            const t2dVector& refLocalOffset, const t2dVector& refLocalVelocity, const t2dMatrix& refLocalRotation,
                                            F32& timeAxis, const F32 collisionTime )
{
    // Projection Intervals for Source/Destination.
    F32 srcMinProj;
    F32 srcMaxProj;
    F32 dstMinProj;
    F32 dstMaxProj;

    // Calculate Intervals for Source/Destination.
    calculateInterval( srcPoly, srcPolyCount, vertexAxis ^ refLocalRotation, srcMinProj, srcMaxProj );
    calculateInterval( dstPoly, dstPolyCount, vertexAxis, dstMinProj, dstMaxProj );

    // Add Reference Offset.
    F32 srcOffset = refLocalOffset * vertexAxis;

    srcMinProj += srcOffset;
    srcMaxProj += srcOffset;

    // Calculate Intervals.
    const F32 delta0 = srcMinProj - dstMaxProj;
    const F32 delta1 = dstMinProj - srcMaxProj;

    // Are we seperated?
    if ( delta0 > -T2D_CONST_EPSILON || delta1 > -T2D_CONST_EPSILON )
    {
        // Yes, so test the dynamic intervals...

        // Calculate Speed.
        const F32 speed = refLocalVelocity * vertexAxis;

        // Ignore small speed.
        if ( mFabs(speed) < T2D_CONST_EPSILON )
            return false;

        // Calculate Time of impact.
        F32 startTime = -delta0 / speed;
        F32 endTime = +delta1 / speed;

        // Reorder time (if needed).
        if ( startTime > endTime )
        {
            // Swap Time Interval.
            mSwap( startTime, endTime );
        }

        // Calculate Time.
        timeAxis = ( startTime > 0.0f ) ? startTime : endTime;

        // Bad Time!
        if ( timeAxis < T2D_CONST_EPSILON || timeAxis > collisionTime )
            return false;

        // Successful Collision!
        return true;
    }
    else
    {
        // No, so we're overlapped.
        //
        // Let's get the interval of the smallest |delta0| and |delta1| and
        // encode it as a negative value to signify an overlap rather than
        // a disjoint collision in forward-time.
        timeAxis = ( delta0 > delta1 ) ? delta0 : delta1;

        // Successful Collision!
        return true;
    }
}


//-----------------------------------------------------------------------------
// Calculate the Projection of Polygon onto an Axii.
//-----------------------------------------------------------------------------
void t2dPhysics::calculateInterval( const t2dVector* pPoly, const U32 polyCount, const t2dVector& axii, F32& minProj, F32& maxProj )
{
    // Reset Projection Range.
    minProj = maxProj = ( pPoly[0] * axii );

    // Iterate Polygon.
    for ( U32 i = 0; i < polyCount; i++ )
    {
        // Calculate Projection.
        const F32 proj = pPoly[i] * axii;

        // Adjust Projection Interval.
        if ( proj < minProj )
            minProj = proj;
        else if ( proj > maxProj )
            maxProj = proj;
    }
}


//-----------------------------------------------------------------------------
// Find the Minimum Seperation.
//-----------------------------------------------------------------------------
bool t2dPhysics::findMinimumSeperation( t2dVector* pVertexAxis, F32* pTimeAxis, U32 axesCount, t2dVector& collisionNormal, F32& collisionTime )
{
    // Sanity!
    AssertFatal( axesCount > 0, "t2dPhysics::findMinimumSeperation() - No axes for object!" );

    // Reset Minimum Index.
    S32 minimumIndex = -1;

    // Reset Collision Time.
    collisionTime = 0.0f;

    // Iterate Axes.
    for ( U32 i = 0; i < axesCount; i++ )
    {
        // Check for Collisions.
        if ( pTimeAxis[i] > 0.0f && pTimeAxis[i] > collisionTime )
        {
            // Note Index.
            minimumIndex = i;
            // Note Time.
            collisionTime = pTimeAxis[i];
        }
    }

    // Found one!
    if ( minimumIndex != -1 )
    {
        // Note Normal.
        collisionNormal = pVertexAxis[minimumIndex];
        collisionNormal.normalise();
        return true;
    }

    // Nothing, so find overlaps...

    // Iterate Axes.
    minimumIndex = -1;
    for ( U32 i = 0; i < axesCount; i++ )
    {
        // Fetch Length/Normalise.
        const F32 length = pVertexAxis[i].normalise();
        // Renormalise Time.
        pTimeAxis[i] /= length;

        // Check for Collisions.
        if ( pTimeAxis[i] > collisionTime || minimumIndex == -1 )
        {
            // Note Index.
            minimumIndex = i;
            // Note Time.
            collisionTime = pTimeAxis[i];
            // Note Normal.
            collisionNormal = pVertexAxis[minimumIndex];
        }
    }

    // Return Status.
    return (minimumIndex != -1);
}

//-----------------------------------------------------------------------------
// Find Contacts.
//-----------------------------------------------------------------------------
bool t2dPhysics::findContactPoints( const t2dVector* srcPoly, U32 srcPolyCount, const t2dVector& srcPosition, const t2dVector& srcVelocity, const t2dMatrix& srcRotation,
                                    const t2dVector* dstPoly, U32 dstPolyCount, const t2dVector& dstPosition, const t2dVector& dstVelocity, const t2dMatrix& dstRotation,
                                    cCollisionStatus* pCollisionStatus )
{
    static t2dVector srcSupportPoints[MAX_COLLISION_CONTACTS];
    static t2dVector dstSupportPoints[MAX_COLLISION_CONTACTS];

    // Fetch References for speed and clarity.
    const t2dVector& collisionNormal    = pCollisionStatus->mCollisionNormal;
    const F32 collisionTime             = pCollisionStatus->mCollisionTimeReal;
    t2dVector* pSrcContacts             = pCollisionStatus->mSrcContacts;
    t2dVector* pDstContacts             = pCollisionStatus->mDstContacts;
    U32& contactCount                   = pCollisionStatus->mContactCount;

    // Find Source Support Points.
    const U32 srcSupportCount = findSupportPoints( srcPoly, srcPolyCount, srcPosition, srcVelocity, srcRotation, collisionNormal, collisionTime, srcSupportPoints );

    // No contacts without support-points!
    if ( srcSupportCount == 0 )
        return false;

    // Find Destination Support Points.
    const U32 dstSupportCount = findSupportPoints( dstPoly, dstPolyCount, dstPosition, dstVelocity, dstRotation, -collisionNormal, collisionTime, dstSupportPoints );

    // No contacts without support-points!
    if ( dstSupportCount == 0 )
        return false;

    // No Contacts without support-points!
    if ( srcSupportCount == 0 || dstSupportCount == 0 )
        return false;

    // Reset Contact Count.
    contactCount = 0;

    // Trivial Contact Check.
    if ( srcSupportCount == 1 && dstSupportCount == 1 )
    {
        // Simple Contact.
        pCollisionStatus->mSrcContacts[contactCount] = srcSupportPoints[0];
        pCollisionStatus->mDstContacts[contactCount] = dstSupportPoints[0];
        // Increase Contact Count.
        pCollisionStatus->mContactCount++;
        // Return Conversion.
        return true;
    }

    // Calculate Perpendicular Normal.
    const t2dVector perpNormal = collisionNormal.getPerp();

    // Calculate Source/Destination Points.
    F32 srcMin = srcSupportPoints[0] * perpNormal;
    F32 srcMax = srcMin;
    F32 dstMin = dstSupportPoints[0] * perpNormal;
    F32 dstMax = dstMin;

    // Check for Two support-points for source.
    if ( srcSupportCount == 2 )
    {
        // Set Max.
        srcMax = srcSupportPoints[1] * perpNormal;

        // Reoder (if needed).
        if ( srcMax < srcMin )
        {
            // Swap.
            mSwap( srcMin, srcMax );
            // Swap Support Points.
            srcSupportPoints[0].swap( srcSupportPoints[1] );
        }
    }

    // Check for Two support-points for destination.
    if ( dstSupportCount == 2 )
    {
        // Set Max.
        dstMax = dstSupportPoints[1] * perpNormal;

        // Reoder (if needed).
        if ( dstMax < dstMin )
        {
            // Swap.
            mSwap( dstMin, dstMax );
            // Swap Support Points.
            dstSupportPoints[0].swap( dstSupportPoints[1] );
        }
    }

    // Contacts?
    if ( srcMin > dstMax || dstMin > srcMax )
    {
       // if collision time is negative then we are fully overlapped, in which
       // case we don't do this test (this test is whether or not two oncoming edges
       // will pass through each other).
       if (collisionTime >= 0.0f)
       {
          // Nope!
          return false;
       }

    }

    // Projected Segment.
    t2dVector projSeg;

    if ( srcMin > dstMin )
    {
        // Project Src->Dst.
        if ( projectPointToSegment( srcSupportPoints[0], dstSupportPoints[0], dstSupportPoints[1], projSeg ) )
        {
            // Note Contacts.
            pSrcContacts[contactCount] = srcSupportPoints[0];
            pDstContacts[contactCount] = projSeg;
            // Increase Contact Count.
            contactCount++;
        }
    }
    else
    {
        // Project Dst->Src.
        if ( projectPointToSegment( dstSupportPoints[0], srcSupportPoints[0], srcSupportPoints[1], projSeg ) )
        {
            // Note Contacts.
            pSrcContacts[contactCount] = projSeg;
            pDstContacts[contactCount] = dstSupportPoints[0];
            // Increase Contact Count.
            contactCount++;
        }
    }

    // Other Variants.
    if ( srcMin != srcMax  && dstMin != dstMax )
    {
        if ( srcMax < dstMax )
        {
            // Project.
            if ( projectPointToSegment( srcSupportPoints[1], dstSupportPoints[0], dstSupportPoints[1], projSeg ) )
            {
                // Note Contacts.
                pSrcContacts[contactCount] = srcSupportPoints[1];
                pDstContacts[contactCount] = projSeg;
                // Increase Contact Count.
                contactCount++;
            }
        }
        else
        {
            // Project.
            if ( projectPointToSegment( dstSupportPoints[1], srcSupportPoints[0], srcSupportPoints[1], projSeg ) )
            {
                // Note Contacts.
                pSrcContacts[contactCount] = projSeg;
                pDstContacts[contactCount] = dstSupportPoints[1];
                // Increase Contact Count.
                contactCount++;
            }
        }
    }

    // Return Contacts.
    return true;
}


//----------------------------------------------------------------------------------------------- 
// Find Support Points.
// NOTE:-   This is a convex shape along a specified direction.
//----------------------------------------------------------------------------------------------- 
U32 t2dPhysics::findSupportPoints(  const t2dVector* pPoly, U32 polyCount,
                                    const t2dVector& position, const t2dVector& velocity, const t2dMatrix& rotation,
                                    const t2dVector& collisionNormal, F32 collisionTime,
                                    t2dVector* pSupportPoints )
{
    static F32 dir[MAX_COLLISION_POLY_VERTEX];

    // Calculate Normal.
    t2dVector normal = collisionNormal ^ rotation;

    // Reset Direction.
    F32 dirMin = dir[0] = pPoly[0] * normal;

    // Interate Polygon.
    for ( U32 i = 1; i < polyCount; i++ )
    {
        // Calculate.
        dir[i] = pPoly[i] * normal;

        // Check For Minimum.
        if ( dir[i] < dirMin )
            dirMin = dir[i];
    }

    // The limit here is two support-points only.
    // If we find more then we use the extremums.
    F32 supportDot[2];
    U32 supportCount = 0;

    // Reset Sign Flag.
    bool sign = false;

    // Calculate Perpendicular Normal.
    t2dVector perpNormal = collisionNormal.getPerp();

    // Interate Polygon.
    for ( U32 i = 0; i < polyCount; i++ )
    {
        // Check Contact.
        if ( dir[i] < (dirMin + T2D_CONST_COLLISION_MARGIN) )
        {
            // Transform Contact to World-Space.
            t2dVector contact;
            transformContact( pPoly[i], position, velocity, rotation, collisionTime, contact );
            // Contact Dot.
            F32 contactDot = contact * perpNormal;
            // Less than two supports?
            if ( supportCount < 2 )
            {
                // Yes, so note contact.
                supportDot[supportCount] = contactDot;
                pSupportPoints[supportCount] = contact;
                // Increase Support Count.
                supportCount++;

                // Note Sign for two contacts.
                if (supportCount > 1)
                    sign = ( supportDot[1] > supportDot[0] );
            }
            else
            {
                // No, so use extemums.
                F32& min0 = (sign) ? supportDot[0] : supportDot[1];
                F32& max0 = (sign) ? supportDot[1] : supportDot[0];
                t2dVector& min1 = (sign) ? pSupportPoints[0] : pSupportPoints[1];
                t2dVector& max1 = (sign) ? pSupportPoints[1] : pSupportPoints[0];
                // Choose.
                if ( contactDot < min0 )
                {
                    min0 = contactDot;
                    min1 = contact;

                }
                else if ( contactDot > max0 )
                {
                    max0 = contactDot;
                    max1 = contact;
                }
            }
        }
    }

    // Return Support Count.
    return supportCount;
}


//----------------------------------------------------------------------------------------------- 
// Transform Contact-Point into World-Space Collision.
//----------------------------------------------------------------------------------------------- 
void t2dPhysics::transformContact( const t2dVector& vertex, const t2dVector& position, const t2dVector& velocity, const t2dMatrix& rotation, F32 collisionTime, t2dVector& contact )
{
    // Do Transformation.
    contact = position + (vertex * rotation);

    // Check Time.
    if ( collisionTime > 0.0f )
        contact += velocity * collisionTime;
}


//----------------------------------------------------------------------------------------------- 
// Find closest point on a segment to a vertex
//----------------------------------------------------------------------------------------------- 
bool t2dPhysics::projectPointToSegment( const t2dVector& V, const t2dVector& A, const t2dVector& B, t2dVector& W, F32* pTime )
{
    // Deltas.
    t2dVector aV = V - A;
    t2dVector aB = B - A;

    // Time.
    F32 time = ( aV * aB ) / ( aB * aB );

    // Clamp Time.
    time = mClampF( time, 0.0f, 1.0f );

    // Pass-back time (if used).
    if ( pTime )
        *pTime = time;

    // Finish Projection.
    W = A + time * aB;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Swept-Circle to Circle Collision Check.
//-----------------------------------------------------------------------------
bool t2dPhysics::sweptCircleToCircleCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus )
{
    // Invalidate Collision.
    pCollisionStatus->mValidCollision = false;

    // Set Collision Integration Interval.
    pCollisionStatus->mFullTimeStep = elapsedTime;

    // Fetch Source/Destination Physics.
    const t2dPhysics& srcPhysics = *pCollisionStatus->mpSrcPhysics;
    const t2dPhysics& dstPhysics = *pCollisionStatus->mpDstPhysics;

    // Fetch Positions.
    const t2dVector& srcPosition = srcPhysics.getPosition();
    const t2dVector& dstPosition = dstPhysics.getPosition();

    // Calculate Relative Position.
    const t2dVector deltaPosition = dstPhysics.getPosition() - srcPhysics.getPosition();
    // Fetch Velocities.
    const t2dVector& srcGrossVelocity = srcPhysics.getGrossLinearVelocity();
    const t2dVector& dstGrossVelocity = dstPhysics.getGrossLinearVelocity();
    // Calculate Relative Velocity (for this frame).
    const t2dVector deltaVelocity = (srcGrossVelocity-dstGrossVelocity) * elapsedTime;
    // Calculate Minimal Distance.
    const F32 minimalDistance = srcPhysics.getCollisionCircleRadii() + dstPhysics.getCollisionCircleRadii();
    // Calculate Distance from centers.
    const F32 distance = deltaPosition.len();
    // Calculate Seperation.
    const F32 seperation = distance - minimalDistance;

    // Are we overlapped?
    if ( seperation < 0.0f )
    {
        // Yes, so are the objects positionally coincident?
        if ( deltaPosition.isLenZero() )
        {
            // MM: 18/04/07
            // Yes, so we have no choice but to make an educated guess on how to seperate these bodies.
            // We'll use source linear velocity as a hint and failing that, just seperate by moving one
            // object "UP" with respect to the other.  The magnitude of this response when both objects
            // are spatially coincident is the maximum radii of both objects.

            // Calculate the extended overlap magnitude.
            const F32& srcRadii = pCollisionStatus->mpSrcPhysics->getCollisionCircleRadii();
            const F32& dstRadii = pCollisionStatus->mpDstPhysics->getCollisionCircleRadii();
            const F32 overlapExtend = (srcRadii + dstRadii) * 0.5f;

            t2dVector overlapOffset;

            // Any useful linear velocity?
            if ( srcPhysics.mGrossLinearVelocityActive )
            {
                // Yes, so use the inverse linear velocity as the vector.
                overlapOffset = -srcPhysics.getGrossLinearVelocity();
                overlapOffset.normalise( overlapExtend );
            }
            else
            {
                // No, so use an arbitrary "UP" vector.
                overlapOffset.set(0, -overlapExtend);
            }

            // Set Source Contact Point.
            pCollisionStatus->mSrcContacts[0] = srcPosition - overlapOffset;
            // Set Destination Contact Point.
            pCollisionStatus->mDstContacts[0] = dstPosition + overlapOffset;
            // Single Contact-Point.
            pCollisionStatus->mContactCount = 1;
            // Set Collision Normal.
            pCollisionStatus->mCollisionNormal = overlapOffset;
            pCollisionStatus->mCollisionNormal.normalise();
        }
        else
        {
            // No, so calculate contact vectors...
            t2dVector contactVec1 = deltaPosition;
            contactVec1.normalise( srcPhysics.getCollisionCircleRadii() );
            t2dVector contactVec2 = -deltaPosition;
            contactVec2.normalise( dstPhysics.getCollisionCircleRadii() );

            // Set Source Contact Point.
            pCollisionStatus->mSrcContacts[0] = srcPosition + contactVec1;
            // Set Destination Contact Point.
            pCollisionStatus->mDstContacts[0] = dstPosition + contactVec2;
            // Single Contact-Point.
            pCollisionStatus->mContactCount = 1;
            // Set Collision Normal.
            pCollisionStatus->mCollisionNormal.set( -deltaPosition.mY, deltaPosition.mX );
            // Set Collision Normal.
            pCollisionStatus->mCollisionNormal = -deltaPosition;
            pCollisionStatus->mCollisionNormal.normalise();
            // Flag overlap.
            pCollisionStatus->mCollisionTimeNorm = 0.0f;
            pCollisionStatus->mCollisionTimeReal = -seperation;
        }

        // Set Overlap.
        pCollisionStatus->mOverlapped = true;

        // Validate Collision.
        pCollisionStatus->mValidCollision = true;

        // Successful Collision!
        return true;
    }

    // Calculate Sweep.
    const F32 sweep = deltaVelocity.len();

    // Will we cover the seperation.
    if( mLessThan(sweep, seperation) )
    {
        // No, so no collision!
        return false;
    }

    // This can happen!
    if ( srcGrossVelocity.isLenZero() )
    {
        // No, so no collision!
        return false;
    }

    // Calculate the unit direction.
    t2dVector deltaVelocityNorm = deltaVelocity;
    deltaVelocityNorm.normalise();

    // Project Velocity.
    const F32 projVelocity = deltaVelocityNorm.dot(deltaPosition);

    // Moving towards each other?
    if( mLessThanOrEqual(projVelocity, 0.0f) )
    {
        // No, so no collision!
        return false;
    }

    // Calculate Distance to center.
    const F32 projDistance = (distance * distance) - (projVelocity * projVelocity);

    // Calculate the minimal square approach required.
    const F32 minimalApproachSqr = minimalDistance * minimalDistance;

    // Will closet approach be close enough?
    if( mGreaterThanOrEqual(projDistance, minimalApproachSqr) )
    {
        // No, so no collision!
        return false;
    }

    // Calculate Approach.
    const F32 projApproach = minimalApproachSqr - projDistance;

    // Check it's valid.
    if( mLessThanZero(projApproach) )
    {
        // No, so no collision!
        return false;
    }

    // Calculate Distance to move for closet approach.
    const F32 distanceToMove = projVelocity - mSqrt(projApproach);

    // Are we moving this distance?
    if( mLessThan(sweep, distanceToMove) )
    {
        // No, so no collision!
        return false;
    }

    // Yes, we're going to hit so calculate the collision details...

    // Calculate our collision vector.
    t2dVector collisionVec = deltaVelocity;
    collisionVec.normalise(distanceToMove);

    // Calculate Collision time.
    const F32 collisionTime = distanceToMove / srcGrossVelocity.len();

    // Calculate positions at collision-time.
    const t2dVector srcPositionContact = srcPhysics.getPosition() + ( srcGrossVelocity * collisionTime );
    const t2dVector dstPositionContact = dstPhysics.getPosition() + ( dstGrossVelocity * collisionTime );

    // Calculate Contact Delta-Position at new time.
    const t2dVector contactDeltaPosition = dstPositionContact - srcPositionContact;

    // Calculate contact vectors...
    t2dVector contactVec1 = contactDeltaPosition;
    contactVec1.normalise( srcPhysics.getCollisionCircleRadii() );
    t2dVector contactVec2 = -contactDeltaPosition;
    contactVec2.normalise( dstPhysics.getCollisionCircleRadii() );

    // Set Source Contact Point.
    pCollisionStatus->mSrcContacts[0] = srcPositionContact + contactVec1;
    // Set Destination Contact Point.
    pCollisionStatus->mDstContacts[0] = dstPositionContact + contactVec2;
    // Single Contact-Point.
    pCollisionStatus->mContactCount = 1;
    // Set Collision Normal.
    pCollisionStatus->mCollisionNormal = contactVec2;
    pCollisionStatus->mCollisionNormal.normalise();

    // Clamp Collision Time?
    if ( collisionTime >= 0.0f && collisionTime < T2D_CONST_COLLISION_MARGIN )
    {
        // Yes, so clamp times.
        pCollisionStatus->mCollisionTimeNorm = pCollisionStatus->mCollisionTimeReal = 0.0f;
    }
    else
    {
        // No, so set Collision Times.
        pCollisionStatus->mCollisionTimeNorm = collisionTime / pCollisionStatus->mFullTimeStep;
        pCollisionStatus->mCollisionTimeReal = collisionTime;
    }

    // Not Overlapped.
    pCollisionStatus->mOverlapped = false;
    // Validate Collision.
    pCollisionStatus->mValidCollision = true;

    // Successful Collision!
    return true;
}


//-----------------------------------------------------------------------------
// Swept-Polygon to Circle Collision Check.
//-----------------------------------------------------------------------------
bool t2dPhysics::sweptPolyToCircleCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus )
{
    // Invalidate Collision.
    pCollisionStatus->mValidCollision = false;

    // Set Collision Integration Interval.
    pCollisionStatus->mFullTimeStep = elapsedTime;

    // Fetch Source/Destination Physics.
    t2dPhysics& srcPhysics = *pCollisionStatus->mpSrcPhysics;
    t2dPhysics& dstPhysics = *pCollisionStatus->mpDstPhysics;

    // Fetch Source Details.
    const t2dVector&    srcPosition     = srcPhysics.getPosition();
    const t2dVector*    srcPoly         = srcPhysics.getCollisionPolyLocal();
    const U32           srcPolyCount    = srcPhysics.getCollisionPolyCount();
    const t2dMatrix&    srcRotation     = srcPhysics.getRotationMatrix();

    // Fetch Destination Details.
    const t2dVector&    dstPosition     = dstPhysics.getPosition() - srcPosition;  // Destination-Space.
    const F32           dstRadii        = dstPhysics.getCollisionCircleRadii();

	// Calculate World-Space Transformed Collision-Polygon.
	static Vector<t2dVector> collisionPolyWorldList;
	// Set Poly List.
	collisionPolyWorldList.setSize( srcPolyCount );
	// Transform Polygon.
	for ( U32 n = 0; n < srcPolyCount; n++ )
    {
		collisionPolyWorldList[n] = srcPoly[n] * srcRotation;
    }
    // Fetch XFormed Polygon.
    const t2dVector* srcPolyXFormed = &(collisionPolyWorldList[0]);


    // Test if circle intersects at time-zero.
    if ( polygonCircleIntersect( srcPosition, srcPolyXFormed, srcPolyCount, dstPosition, dstRadii*dstRadii, pCollisionStatus ) )
    {
        // Because the test checks intersection of the circle against the polygon, which is opposite of what we're trying to achieve,
        // we need to swap the collision normal/contacts.
        pCollisionStatus->mCollisionNormal = -pCollisionStatus->mCollisionNormal;
        pCollisionStatus->mSrcContacts[0].swap( pCollisionStatus->mDstContacts[0] );

        // Successful Collision!
        return true;
    }

    // Calculate Destination Move (in source space).
    const t2dVector dstDelta = (dstPhysics.getGrossLinearVelocity()-srcPhysics.getGrossLinearVelocity()) * elapsedTime;

    // Test if circle intersects with one of the edges or vertices
    if ( sweptCircleEdgeVertexIntersect( srcPosition, srcPolyXFormed, srcPolyCount, dstPosition, dstDelta, 0, 0, dstRadii*dstRadii, pCollisionStatus ) )
    {
        // Because the test checks intersection of the circle against the polygon, which is opposite of what we're trying to achieve,
        // we need to swap the collision normal/contacts.
        pCollisionStatus->mCollisionNormal = -pCollisionStatus->mCollisionNormal;
        pCollisionStatus->mSrcContacts[0].swap( pCollisionStatus->mDstContacts[0] );
        // Successful Collision!
        return true;
    }

    // No collision.
    return false;
}


//-----------------------------------------------------------------------------
// Swept Circle to Polygon Collision.
//-----------------------------------------------------------------------------
bool t2dPhysics::sweptCircleToPolyCollision( F32 elapsedTime, cCollisionStatus* pCollisionStatus )
{
    // Invalidate Collision.
    pCollisionStatus->mValidCollision = false;

    // Set Collision Integration Interval.
    pCollisionStatus->mFullTimeStep = elapsedTime;

    // Fetch Source/Destination Physics.
    t2dPhysics& srcPhysics = *pCollisionStatus->mpSrcPhysics;
    t2dPhysics& dstPhysics = *pCollisionStatus->mpDstPhysics;

    // Fetch Destination Details.
    const t2dVector&    dstPosition     = dstPhysics.getPosition();
    const t2dVector*    dstPoly         = dstPhysics.getCollisionPolyLocal();
    const U32           dstPolyCount    = dstPhysics.getCollisionPolyCount();
    const t2dMatrix&    dstRotation     = dstPhysics.getRotationMatrix();

    // Fetch Source Details.
    const t2dVector&    srcPosition     = srcPhysics.getPosition() - dstPosition;  // Destination-Space.
    const F32           srcRadii        = srcPhysics.getCollisionCircleRadii();


	// Calculate World-Space Transformed Collision-Polygon.
	static Vector<t2dVector> collisionPolyWorldList;
	// Set Poly List.
	collisionPolyWorldList.setSize( dstPolyCount );
	// Transform Polygon.
	for ( U32 n = 0; n < dstPolyCount; n++ )
    {
		collisionPolyWorldList[n] = dstPoly[n] * dstRotation;
    }
    // Fetch XFormed Polygon.
    const t2dVector* dstPolyXFormed = &(collisionPolyWorldList[0]);

    // Test if circle intersects at time-zero.
    if ( polygonCircleIntersect( dstPosition, dstPolyXFormed, dstPolyCount, srcPosition, srcRadii*srcRadii, pCollisionStatus ) )
    {
        // Successful Collision!
        return true;
    }

    // Calculate Source Move.
    const t2dVector srcDelta = srcPhysics.getGrossLinearVelocity() * elapsedTime;

    // Test if circle intersects with one of the edges or vertices
    if ( sweptCircleEdgeVertexIntersect( dstPosition, dstPolyXFormed, dstPolyCount, srcPosition, srcDelta, 0, 0, srcRadii*srcRadii, pCollisionStatus ) )
    {
        // Successful Collision!
        return true;
    }

    // No collision.
    return false;
}


//-----------------------------------------------------------------------------
// Polygon Contains Point?
//-----------------------------------------------------------------------------
bool t2dPhysics::polygonContains( const t2dVector* pVertices, const U32 numVertices, const t2dVector& point )
{
    // Loop through edges
    for ( const t2dVector *pv1 = pVertices, *pv2 = pVertices + numVertices - 1; pv1 < pVertices + numVertices; pv2 = pv1, ++pv1 )
    {
        // If the point is outside this edge, the point is outside the polygon
        t2dVector v1_v2 = *pv2 - *pv1;
        t2dVector v1_point = point - *pv1;
        if ( (v1_v2.mX * v1_point.mY - v1_point.mX * v1_v2.mY) > 0.0f)
            return false;
    }

    // Point is Contained!
    return true;
}


//-----------------------------------------------------------------------------
// Get Smoothed Corner Normal.
//-----------------------------------------------------------------------------
void t2dPhysics::getSmoothedCornerNormal( const t2dVector* pVertices, const U32 numVertices, const U32 index, t2dVector& smoothedNormal )
{
    // Sanity!
    AssertFatal( index >= 0 && index < numVertices, "t2dPhysics::getSmoothedCornerNormal() - Invalid Vertex Index!" );

    // Is this a line?
    if ( numVertices == 2 )
    {
        // Yes, so return all we can; the corner.
        smoothedNormal = pVertices[index]; 
        // Finish Here.
        return;
    }

    // Fetch Indexed Vertex.
    const t2dVector vIndex = pVertices[index];

    t2dVector vBackEdge;
    t2dVector vForwardEdge;

    // Calculate Backwards Edge.
    if ( index == 0 )
    {
        vBackEdge = pVertices[numVertices-1] - vIndex;
    }
    else
    {
        vBackEdge = pVertices[index-1] - vIndex;
    }

    // Calculate Forwards Edge.
    if ( index == numVertices-1 )
    {
        vForwardEdge = vIndex - pVertices[0];
    }
    else
    {
        vForwardEdge = vIndex - pVertices[index+1];
    }

    t2dVector bePerp = vBackEdge.getPerp().getUnitDirection();
    t2dVector fePerp = vForwardEdge.getPerp().getUnitDirection();

    // Calculate Smoothed Normal.
    smoothedNormal = bePerp+fePerp;
}


//-----------------------------------------------------------------------------
// Polygon/Circle Intersection?
//-----------------------------------------------------------------------------
bool t2dPhysics::polygonCircleIntersect( const t2dVector& spaceXForm, const t2dVector* pVertices, const U32 numVertices, const t2dVector& center, const F32 radiusSqr, cCollisionStatus* pCollisionStatus )
{
    // No collision by default.
    bool collision = false;

    t2dVector colPoint;
    t2dVector colNormal;

    // First, check if polygon contains the circle center which we can use to adapt the partial-intersection
    // results by inverting the contacts appropriately.
    const bool polygonContained = polygonContains(pVertices, numVertices, center);

    // Now check for partial intersection...

    // Setup initial radius.
    F32 radiusSqrSweep = radiusSqr;

    t2dVector v1_v2;
    t2dVector v1_center;
    t2dVector point;
    F32 fraction;
    F32 distSqr;
    U32 vertex = 0;

    // Check all edges.
    for ( const t2dVector *pv1 = pVertices, *pv2 = pVertices + numVertices - 1; vertex < numVertices; pv2 = pv1, ++pv1, ++vertex )
    {
        // Find where closest point to this edge occurs...
        v1_v2 = *pv2 - *pv1;
        v1_center = center - *pv1;
        fraction = v1_center.dot(v1_v2);

        // Vertex (corner) closest?
        if (fraction < 0.0f)
        {
            // Yes, so within range?
            distSqr = v1_center.lenSquared();
            if ( mLessThan(distSqr, radiusSqrSweep) )
            {
                // Yes, so note Collision Point.
                colPoint = *pv1;
                // Set new Radius.
                radiusSqrSweep = distSqr;
                // Flag Collision.
                collision = true;
            }
        }
        else 
        {
            // No, so edge closest?
            const F32 v1_v2_len_sqr = v1_v2.lenSquared();
            if (mLessThanOrEqual(fraction, v1_v2_len_sqr))
            {
                // Yes, closest point is on edge.
                point = *pv1 + (v1_v2 * (fraction / v1_v2_len_sqr));
                distSqr = (point - center).lenSquared();
                // Within range?
                if ( mLessThan(distSqr,radiusSqrSweep) )
                {
                    // Yes, so note Collision Point.
                    colPoint = point;
                    // Set new Radius.
                    radiusSqrSweep = distSqr;
                    // Flag Collision.
                    collision = true;
                }
            }
        }
    }

    // Finish if no collision.    
    if (!collision) return false;

    // Calculate Source Collision Point.
    t2dVector contactNorm = colPoint - center;
    contactNorm.normalise(mSqrt(radiusSqr));

    if ( polygonContained )
    {
        // Set Contact Point.
        pCollisionStatus->mSrcContacts[0] = center - contactNorm + spaceXForm;
        pCollisionStatus->mDstContacts[0] = colPoint + spaceXForm;
    }
    else
    {
        // Set Contact Point.
        pCollisionStatus->mSrcContacts[0] = center + contactNorm + spaceXForm;
        pCollisionStatus->mDstContacts[0] = colPoint + spaceXForm;
    }
    // Single Contact-Point.
    pCollisionStatus->mContactCount = 1;
    // Reset Normalised Time.
    pCollisionStatus->mCollisionTimeNorm = 0.0f;

    // Normalise Collision Normal.
    pCollisionStatus->mCollisionNormal = pCollisionStatus->mDstContacts[0] - pCollisionStatus->mSrcContacts[0];
    pCollisionStatus->mCollisionNormal.normalise();

    // Calculate Contact Distance.
    const F32 contactDistance = (pCollisionStatus->mDstContacts[0] - pCollisionStatus->mSrcContacts[0]).len();

    // Small Overlap?
    if ( contactDistance < T2D_CONST_COLLISION_MARGIN )
    {
        // Yes, so reset to near-zero-time collision.
        pCollisionStatus->mCollisionTimeReal = 0.0f;
    }
    else
    {
        // Set Collision Time to be contact distance!
        pCollisionStatus->mCollisionTimeReal = -contactDistance;
        // Set Overlap.
        pCollisionStatus->mOverlapped = true;
    }

    // Validate Collision.
    pCollisionStatus->mValidCollision = true;

    // Collision!
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------
// Checks intersection between a polygon an moving circle at inBegin + t * inDelta with radius^2 = inA * t^2 + inB * t + inC, t in [0, 1]
// Returns true when it does and returns the intersection position in outPoint and the intersection fraction (value for t) in outFraction
//------------------------------------------------------------------------------------------------------------------------------------------------
bool t2dPhysics::sweptCircleEdgeVertexIntersect( const t2dVector& spaceXForm, const t2dVector* pVertices, U32 numVertices, const t2dVector& begin, const t2dVector& delta, F32 A, F32 B, F32 C, cCollisionStatus* pCollisionStatus )
{
    // No collision by default.
    bool collision = false;

    // Default upper-time.
    F32 upperTime = 1.0f;

    t2dVector colPoint;
    t2dVector colNormal;
    t2dVector v1_v2;
    U32 vertex = 0;

    // Check all edges.
    for ( const t2dVector *pv1 = pVertices, *pv2 = pVertices + numVertices - 1; vertex < numVertices; pv2 = pv1, ++pv1, ++vertex )
    {
        F32 time;
    
        // Check if circle hits the vertex (corner).
        v1_v2 = *pv2 - *pv1;
        t2dVector bv1 = *pv1 - begin;
        F32 a1 = A - delta.lenSquared();                
        F32 b1 = B + 2.0f * delta.dot(bv1);
        F32 c1 = C - bv1.lenSquared();
        if ( findLowestRootInInterval(a1, b1, c1, upperTime, time) )
        {
            // Yes, so note Collision Point.
            colPoint = *pv1;
            // Set Collision Normal.
            getSmoothedCornerNormal( pVertices, numVertices, vertex, colNormal );
            // Flag Collision.
            collision = true;
            // Set new upper-time.
            upperTime = time;
        }

        // Check if circle hits the edge
        F32 v1v2_dot_delta = v1_v2.dot(delta);
        F32 v1v2_dot_bv1 = v1_v2.dot(bv1);
        F32 v1v2_len_sqr = v1_v2.lenSquared();
        F32 a2 = v1v2_len_sqr * a1 + v1v2_dot_delta * v1v2_dot_delta;
        F32 b2 = v1v2_len_sqr * b1 - 2.0f * v1v2_dot_bv1 * v1v2_dot_delta;
        F32 c2 = v1v2_len_sqr * c1 + v1v2_dot_bv1 * v1v2_dot_bv1;
        if ( findLowestRootInInterval(a2, b2, c2, upperTime, time) )
        {
            // Check if the intersection point is on the edge
            F32 f = time * v1v2_dot_delta - v1v2_dot_bv1;
            if (f >= 0.0f && f <= v1v2_len_sqr)
            {
                // Yes, so note Collision Point.
                colPoint = *pv1 + v1_v2 * (f / v1v2_len_sqr);
                // Set Collision Normal.
                colNormal.set(-v1_v2.mY, v1_v2.mX);
                // Flag Collision.
                collision = true;
                // Set new upper-time.
                upperTime = time;
            }
        }
    }

    // Finish if no collision.
    if ( !collision ) return false;

    // Set Contact Points.
    pCollisionStatus->mSrcContacts[0] = pCollisionStatus->mDstContacts[0] = colPoint + spaceXForm;
    // Single Contact-Point.
    pCollisionStatus->mContactCount = 1;

    // Set Collision Normal.
    pCollisionStatus->mCollisionNormal = colNormal;
    pCollisionStatus->mCollisionNormal.normalise();

    const F32 collisionTime = pCollisionStatus->mFullTimeStep * upperTime;

    // Clamp Collision Time.
    if ( collisionTime < T2D_CONST_COLLISION_MARGIN )
    {
        // Zero-Time Collision.
        pCollisionStatus->mCollisionTimeReal = pCollisionStatus->mCollisionTimeNorm = 0.0f;
    }
    else
    {
        // Set Real Collision Time.
        pCollisionStatus->mCollisionTimeReal = collisionTime;
        // Set Normalised Collision Time.
        pCollisionStatus->mCollisionTimeNorm = upperTime;
    }

    // Not Overlapped.
    pCollisionStatus->mOverlapped = false;
    // Validate Collision.
    pCollisionStatus->mValidCollision = true;

    // Collision!
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------
// Solve the equation A * x^2 + B * x + C == 0 for the lowest x in [0, upperBound].
// Returns true if there is such a solution and returns the solution in X
//------------------------------------------------------------------------------------------------------------------------------------------------
bool t2dPhysics::findLowestRootInInterval( F32 A, F32 B, F32 C, F32 upperBound, F32& X )
{
    // Check if a solution exists
    F32 determinant = B * B - 4.0f * A * C;
    if (determinant < 0.0f)
        return false;

    // The standard way of doing this is by computing: x = (-b +/- Sqrt(b^2 - 4 a c)) / 2 a 
    // is not numerically stable when a is close to zero. 
    // Solve the equation according to "Numerical Recipies in C" paragraph 5.6
    F32 q = -0.5f * (B + (B < 0.0f? -1.0f : 1.0f) * mSqrt(determinant));

    // Both of these can return +INF, -INF or NAN that's why we test both solutions to be in the specified range below
    F32 x1 = q / A;
    F32 x2 = C / q;

    // Order the results
    if (x2 < x1)
        mSwap(x1, x2);

    // Check if x1 is a solution
    if ( x1 >= 0.0f && x1 <= upperBound )
    {
        X = x1;
        return true;
    }

    // Check if x2 is a solution
    if (x2 >= 0.0f && x2 <= upperBound)
    {
        X = x2;
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Reverse Collision Status.
//-----------------------------------------------------------------------------
void t2dPhysics::reverseCollisionStatus( const cCollisionStatus* pCollisionStatus, cCollisionStatus* pReverseCollisionStatus )
{
    // Generate Inverse Send Collision Status.
    pReverseCollisionStatus->mValidCollision    = pCollisionStatus->mValidCollision;
    pReverseCollisionStatus->mSrcObject         = pCollisionStatus->mDstObject;
    pReverseCollisionStatus->mDstObject         = pCollisionStatus->mSrcObject;
    pReverseCollisionStatus->mpSrcPhysics       = pCollisionStatus->mpDstPhysics;
    pReverseCollisionStatus->mpDstPhysics       = pCollisionStatus->mpSrcPhysics;
    pReverseCollisionStatus->mCollisionNormal   = pCollisionStatus->mCollisionNormal;
    pReverseCollisionStatus->mCollisionTimeNorm = pCollisionStatus->mCollisionTimeNorm;
    pReverseCollisionStatus->mCollisionTimeReal = pCollisionStatus->mCollisionTimeReal;
    pReverseCollisionStatus->mFullTimeStep      = pCollisionStatus->mFullTimeStep;
    pReverseCollisionStatus->mContactCount      = pCollisionStatus->mContactCount;

    // Reverse Contacts.
    for ( U32 n = 0; n < pCollisionStatus->mContactCount; n++ )
    {
        pReverseCollisionStatus->mSrcContacts[n] = pCollisionStatus->mDstContacts[n];
        pReverseCollisionStatus->mDstContacts[n] = pCollisionStatus->mSrcContacts[n];
    }
}


//-----------------------------------------------------------------------------
// Move to Collision Point.
//-----------------------------------------------------------------------------
void t2dPhysics::moveToCollisionPoint( cCollisionStatus* pCollisionStatus )
{
    // Fetch source physics.
    t2dPhysics* pSrcPhysics = pCollisionStatus->mpSrcPhysics;
    // Is the collision in forward-time and we're not immovable?
    if ( !pCollisionStatus->mOverlapped )
    {
        // Fetch Linear Velocity.
        const t2dVector linearVelocity = pSrcPhysics->getGrossLinearVelocity();

        // Calculate Spatial Step.
        t2dVector spatialStep = linearVelocity * pCollisionStatus->mCollisionTimeReal;

        // Calculate backing-off vector.
        t2dVector backOff = spatialStep;
        backOff.normalise(T2D_CONST_COLLISION_MARGIN);
        spatialStep -= backOff;

        // Set position at collision-time (minus a little to stop us colliding).
        pSrcPhysics->setPosition( pSrcPhysics->getPosition() + spatialStep );
    }
}


//-----------------------------------------------------------------------------
// Back Away.
//-----------------------------------------------------------------------------
void t2dPhysics::backAway( cCollisionStatus* pCollisionStatus )
{
    // Calculate Back Away Vector.
    t2dVector backOff = pCollisionStatus->mCollisionNormal;
    backOff.normalise(T2D_CONST_COLLISION_MARGIN);

    // Solve Source?
    if ( pCollisionStatus->mSrcSolve )
    {
        // Reposition Object.
        pCollisionStatus->mpSrcPhysics->setPosition( pCollisionStatus->mpSrcPhysics->getPosition() + backOff );
    }
}


//-----------------------------------------------------------------------------
// Average Contact Points.
//-----------------------------------------------------------------------------
void t2dPhysics::averageContactPoints( cCollisionStatus* pCollisionStatus )
{
    // Two Contact-Points?
    if ( pCollisionStatus->mContactCount == 2 )
    {
        // Yes, so average Source/Destination Contact-Points.
        pCollisionStatus->mSrcContacts[0] = (pCollisionStatus->mSrcContacts[0] + pCollisionStatus->mSrcContacts[1])*0.5f;
        pCollisionStatus->mDstContacts[0] = (pCollisionStatus->mDstContacts[0] + pCollisionStatus->mDstContacts[1])*0.5f;
        // Now we've only got one.
        pCollisionStatus->mContactCount = 1;
    }
}


//-----------------------------------------------------------------------------
// Add Collision History.
//-----------------------------------------------------------------------------
void t2dPhysics::addCollisionHistory( const cCollisionStatus* pCollisionStatus )
{
    // Ignore if we're not collecting collision history.
    if ( !mCollectCollisionHistory ) return;

    // Fetch Cross Size.
    const U32 contactMaximum = Con::getIntVariable( "$pref::T2D::contactHistoryMax", 1 );

    // Fetch Collision History Count.
    const U32 historyCount = pCollisionStatus->mpSrcPhysics->mCollisionHistory.size();

    // Have we reached the contact maximum?
    if ( historyCount >= contactMaximum )
    {
        // Yes, so remove one.
        // NOTE:- This is very slow so we need to be careful!
        pCollisionStatus->mpSrcPhysics->mCollisionHistory.pop_front();
    }

    // Store new Collision Status.
    pCollisionStatus->mpSrcPhysics->mCollisionHistory.push_back( *pCollisionStatus );
}


//-----------------------------------------------------------------------------
// Solve Collision.
//-----------------------------------------------------------------------------
void t2dPhysics::solveCollision( cCollisionStatus* pCollisionStatus )
{
    // Valid Collision?
    if ( !pCollisionStatus->mValidCollision )
    {
        // No!
        return;
    }

    // Is the source solved?
    if ( pCollisionStatus->mSrcSolve )
    {
        // Yes, so we need to set position/rotation target off.
        pCollisionStatus->mpSrcPhysics->setPositionTargetOff();
        pCollisionStatus->mpSrcPhysics->setRotationTargetOff();
    }

    // Adding Solved Contact History Only?
    if ( pCollisionStatus->mpSrcPhysics->mCollectCollisionHistory &&
         Con::getBoolVariable( "$pref::T2D::contactHistorySolvedOnly", true ) )
    {
        // Yes, so add Collision History.
        pCollisionStatus->mpSrcPhysics->addCollisionHistory( pCollisionStatus );
    }

    // Overlap?
    if ( pCollisionStatus->mOverlapped )
    {
        if ( pCollisionStatus->mpSrcPhysics->getSolveOverlap() )
        {
            // Yes, so fetch source object.
            t2dSceneObject* pSrcObj = pCollisionStatus->mSrcObject;

            // Solving physics?
            if ( pSrcObj != NULL && pSrcObj->getNeverSolvePhysics() )
            {
                // No, so finish.
                return;
            }
            
            // Yes, so resolve overlap.
            resolveOverlap( pCollisionStatus );
        }
    }
    // Have we definately got a forward-time collision?
    else if ( pCollisionStatus->mCollisionTimeReal > 0.0f )
    {
        // Yes, so move to collision point before we calculate the response.
        moveToCollisionPoint( pCollisionStatus );
    }

    // Handle any Disjoint Collision in forward-time.
    resolveForwardCollision( pCollisionStatus );
}


//-----------------------------------------------------------------------------
// Resolve Overlap Entry.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveOverlap( cCollisionStatus* pCollisionStatus )
{
    // Sanity!
    AssertFatal( pCollisionStatus->mContactCount == 1 || pCollisionStatus->mContactCount == 2, "[resolveOverlap] We can only have 1 or 2 contact pairs!" );

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_resolveOverlap);
#endif

    // Only resolve single contact.
    // NOTE:-   We need a much better contact solver but in the meantime we can't
    //          just solve each contact individually especially as this can in some
    //          cases double the contact seperation solution and in worst cases make
    //          overlaps worse.
    resolveOverlapPair( pCollisionStatus, 0 );

    // Flag as handled.
    pCollisionStatus->mHandled = true;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_resolveOverlap
#endif
}


//-----------------------------------------------------------------------------
// Resolve Overlap Pair.
//
// NOTE:-   For overlaps, we'll move the object directly which is a bad thing
//          to do as the downside is that we'll make the subsequent processing
//          invalid.
//
//          We could deal with this situation with an impulse-force but
//          choosing a magnitude for this force proves troublesome although
//          impulse or spring-based forces do provide the ability to
//          produce contact-style collisions when actually we're in an
//          invalid overlap condition meaning we can produce linear
//          velocities from overlapping the objects.
//
//-----------------------------------------------------------------------------
void t2dPhysics::resolveOverlapPair( cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Calculate Contact Direction.
    t2dVector contactDirection = pCollisionStatus->mDstContacts[contactIndex] - pCollisionStatus->mSrcContacts[contactIndex];

    // Increase the overlap backoff.
    t2dVector overlapExtend = contactDirection.isLenZero() ? pCollisionStatus->mCollisionNormal : contactDirection;
    overlapExtend.normalise(T2D_CONST_COLLISION_MARGIN);
    contactDirection += overlapExtend;

    // Adjust Source Position Appropriately.
    if ( pCollisionStatus->mSrcSolve )
    {
        // Source Position
        const t2dVector& srcPosFrom = pCollisionStatus->mpSrcPhysics->getPosition();
        // Adjust Source Position.
        pCollisionStatus->mpSrcPhysics->setPosition( srcPosFrom + contactDirection );
    }
}


//-----------------------------------------------------------------------------
// Resolve Forward-Collision Dispatcher.
//
// NOTE:-   It is extremely important to note that it is valid for the
//          collision status to contain references to physics that
//          do not reference any object in particular.  This means that the
//          physics can be used without the necessity to be bound to
//          any particular object.  For now, this is only being used for the
//          world-limits and there's a good chance that this feature will
//          become redundant in the future but for now, we'll support it.
//          
//-----------------------------------------------------------------------------
void t2dPhysics::resolveForwardCollision( cCollisionStatus* pCollisionStatus )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_resolveForwardCollision);
#endif

    // Average Contact Points.
    averageContactPoints( pCollisionStatus );

    // Remove any overlap-time.
    pCollisionStatus->mCollisionTimeReal = getMax( 0.0f, pCollisionStatus->mCollisionTimeReal );

    // Fetch Source/Destination Collision Responses.
    const t2dPhysics::eCollisionResponse& srcResponse = pCollisionStatus->mpSrcPhysics->getCollisionResponseMode();
    const t2dPhysics::eCollisionResponse& dstResponse = pCollisionStatus->mpDstPhysics->getCollisionResponseMode();

    // Fetch Source/Destination Solve Flag.s
    const bool srcSolve = pCollisionStatus->mSrcSolve;
    const bool dstSolve = pCollisionStatus->mDstSolve;

    // Any collision to solve?
    if ( srcSolve || dstSolve )
    {
        // Are both the collision-responses the same or we're only processing a single response?
        if ( srcResponse == dstResponse || srcSolve != dstSolve )
        {
            // Yes, so calculate which response to use.
            const t2dPhysics::eCollisionResponse& singleResponse = srcSolve ? srcResponse : dstResponse;

            // Iterate/resolve Contact Pairs.
            for ( U32 i = 0; i < pCollisionStatus->mContactCount; i++ )
            {
                // Dispatch same response for both objects.
                switch( singleResponse )
                {
                    // Rigid-Body.
                    case T2D_RESPONSE_RIGID:
                    {
                        // Rigid-Body Collision Response.
                        resolveRigidBodyCollision( srcSolve, dstSolve, pCollisionStatus, i );

                    } break;

                    // Bounce.
                    case T2D_RESPONSE_BOUNCE:
                    {
                        // Bounce Collision Response.
                        resolveBounceCollision( srcSolve, dstSolve, pCollisionStatus, i );

                    } break;

                    // Clamp.
                    case T2D_RESPONSE_CLAMP:
                    {
                        // Clamp Collision Response.
                        resolveClampCollision( srcSolve, dstSolve, pCollisionStatus, i );

                    } break;

                    // Sticky.
                    case T2D_RESPONSE_STICKY:
                    {
                        // Sticky Collision Response.
                        resolveStickyCollision( srcSolve, dstSolve, pCollisionStatus, i );

                    } break;

                    // Kill.
                    case T2D_RESPONSE_KILL:
                    {
                        // Kill Collision Response.
                        resolveKillCollision( srcSolve, dstSolve, pCollisionStatus, i );

                    } break;

                    // Custom.
                    case T2D_RESPONSE_CUSTOM:
                    {
                        // Custom Collision Response.
                        resolveCustomCollision( srcSolve, dstSolve, pCollisionStatus, i );

                    } break;
                };
            }
        }
        else
        {
            // Iterate/resolve Contact Pairs.
            for ( U32 i = 0; i < pCollisionStatus->mContactCount; i++ )
            {
                // ********************************************************************************************
                // Resolve Source Response.
                // ********************************************************************************************

                // Solve Source?
                if ( srcSolve )
                {
                    // Yes, so dispatch response.
                    switch( srcResponse )
                    {
                        // Rigid-Body.
                        case T2D_RESPONSE_RIGID:
                        {
                            // Rigid-Body Collision Response.
                            resolveRigidBodyCollision( true, false, pCollisionStatus, i );

                        } break;

                        // Bounce.
                        case T2D_RESPONSE_BOUNCE:
                        {
                            // Bounce Collision Response.
                            resolveBounceCollision( true, false, pCollisionStatus, i );

                        } break;

                        // Clamp.
                        case T2D_RESPONSE_CLAMP:
                        {
                            // Clamp Collision Response.
                            resolveClampCollision( true, false, pCollisionStatus, i );

                        } break;

                        // Sticky.
                        case T2D_RESPONSE_STICKY:
                        {
                            // Sticky Collision Response.
                            resolveStickyCollision( true, false, pCollisionStatus, i );

                        } break;

                        // Kill.
                        case T2D_RESPONSE_KILL:
                        {
                            // Kill Collision Response.
                            resolveKillCollision( true, false, pCollisionStatus, i );

                        } break;

                        // Custom.
                        case T2D_RESPONSE_CUSTOM:
                        {
                            // Custom Collision Response.
                            resolveCustomCollision( true, false, pCollisionStatus, i );

                        } break;
                    };
                }


                // ********************************************************************************************
                // Resolve Destination Response.
                // ********************************************************************************************

                // Destination Source?
                if ( dstSolve )
                {
                    // Yes, so dispatch response.
                    switch( dstResponse )
                    {
                        case T2D_RESPONSE_RIGID:
                        {
                            // Rigid-Body Collision Response.
                            resolveRigidBodyCollision( false, true, pCollisionStatus, i );

                        } break;

                        case T2D_RESPONSE_BOUNCE:
                        {
                            // Bounce Collision Response.
                            resolveBounceCollision( false, true, pCollisionStatus, i );

                        } break;

                        case T2D_RESPONSE_CLAMP:
                        {
                            // Clamp Collision Response.
                            resolveClampCollision( false, true, pCollisionStatus, i );

                        } break;

                        case T2D_RESPONSE_STICKY:
                        {
                            // Sticky Collision Response.
                            resolveStickyCollision( false, true, pCollisionStatus, i );

                        } break;

                        case T2D_RESPONSE_KILL:
                        {
                            // Kill Collision Response.
                            resolveKillCollision( false, true, pCollisionStatus, i );

                        } break;

                        case T2D_RESPONSE_CUSTOM:
                        {
                            // Custom Collision Response.
                            resolveCustomCollision( true, false, pCollisionStatus, i );

                        } break;
                    };
                }
            }
        }
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_resolveForwardCollision
#endif
}


//-----------------------------------------------------------------------------
// Resolve Rigid-Body Collision Basis.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveRigidBodyCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;

    // Src/Dst Physics.
    t2dPhysics* pSrcPhysics = pCollisionStatus->mpSrcPhysics;
    t2dPhysics* pDstPhysics = pCollisionStatus->mpDstPhysics;

    // Contact Normal.
    const t2dVector contactNormal = -pCollisionStatus->mCollisionNormal;

    // Contacts.
    const t2dVector srcContact = pCollisionStatus->mSrcContacts[contactIndex];
    const t2dVector dstContact = pCollisionStatus->mDstContacts[contactIndex];

    // Fetch Positions.  
    const t2dVector srcPosition = pSrcPhysics->getPosition();
    const t2dVector dstPosition = pDstPhysics->getPosition();

    // Fetch Linear Velocities.
    const t2dVector srcLinearVelocity = pSrcPhysics->getGrossLinearVelocity();
    const t2dVector dstLinearVelocity = pDstPhysics->getGrossLinearVelocity();
    
    // Fetch Angular Velocities.
    const F32 srcAngularVelocity = mDegToRad( -pSrcPhysics->getGrossAngularVelocity() );
    const F32 dstAngularVelocity = mDegToRad( -pDstPhysics->getGrossAngularVelocity() );

    // Contact Velocity.
    const t2dVector srcContactDelta     = srcContact - srcPosition;
    const t2dVector dstContactDelta     = dstContact - dstPosition;
    const t2dVector srcContactDeltaPerp = srcContactDelta.getPerp();
    const t2dVector dstContactDeltaPerp = dstContactDelta.getPerp();
    const t2dVector srcVP               = srcLinearVelocity - srcContactDeltaPerp * srcAngularVelocity;
    const t2dVector dstVP               = dstLinearVelocity - dstContactDeltaPerp * dstAngularVelocity;



    //------------------------------------------------------------------------------------------------------
    // Calculate Impact Velocity.
    //------------------------------------------------------------------------------------------------------
    const t2dVector deltaImpactVelocity = dstVP - srcVP;
    const F32 deltaVelocityDot = deltaImpactVelocity * contactNormal;

    // Are we seperated?
    if ( mGreaterThanZero(deltaVelocityDot) )
    {
        // Yes, so no interaction!
        return;
    }


    //------------------------------------------------------------------------------------------------------
    // Calculate Impulse ( Dynamic-Friction and Restitution )
    //------------------------------------------------------------------------------------------------------

    // Fetch Immovability.
    const bool srcImmovable = pSrcPhysics->getImmovable();
    const bool dstImmovable = pDstPhysics->getImmovable();

    // Source Friction/Restitution.
    const F32 srcDynamicFriction = pSrcPhysics->getDynamicFriction();
    const F32 srcRestitution = pSrcPhysics->getRestitution();

    // Fetch Inverse Masses.
    const F32 srcInverseMass = srcImmovable ? 0.0f : pSrcPhysics->getInverseMass();
    const F32 dstInverseMass = dstImmovable ? 0.0f : pDstPhysics->getInverseMass();

    // Fetch Inverse Inertial Moments.
    const F32 srcInverseInertialMoment = srcImmovable ? 0.0f : pSrcPhysics->getInverseInertialMoment();
    const F32 dstInverseInertialMoment = dstImmovable ? 0.0f : pDstPhysics->getInverseInertialMoment();

    // Normalise velocity.
    const t2dVector vt = deltaImpactVelocity - (contactNormal * deltaVelocityDot);

    // Break Impulse Function Down a little...
    const F32 srcRN = srcContactDelta ^ contactNormal;
    const F32 dstRN = dstContactDelta ^ contactNormal;
    const F32 t0 = srcRN * srcRN * srcInverseInertialMoment;
    const F32 t1 = dstRN * dstRN * dstInverseInertialMoment;
    const F32 jn = deltaVelocityDot / (srcInverseMass + dstInverseMass + t0 + t1);

    // Calculate Impulse (include restitution/dynamic friction).
    const t2dVector impulseForce = ((-(1.0f + srcRestitution) * jn) * contactNormal) + ((srcDynamicFriction * jn) * vt.getUnitDirection());


    //------------------------------------------------------------------------------------------------------
    // Changes in Momentum ( Linear and Angular ).
    //------------------------------------------------------------------------------------------------------

    // Calculate Linear Acceleration.
    t2dVector srcLinearDelta = -impulseForce * srcInverseMass;
    t2dVector dstLinearDelta = impulseForce * dstInverseMass;
    // Calculate Angular Acceleration.
    F32 srcAngularDelta = (srcContactDelta ^ impulseForce) * srcInverseInertialMoment;
    F32 dstAngularDelta = -(dstContactDelta ^ impulseForce) * dstInverseInertialMoment;



    //------------------------------------------------------------------------------------------------------
    // Finally, apply forces.
    //------------------------------------------------------------------------------------------------------

    // Only solve Source if needed.
    if ( solveSrc )
    {
        pSrcPhysics->setGrossLinearVelocity( srcLinearVelocity + srcLinearDelta );
        pSrcPhysics->setGrossAngularVelocity( -mRadToDeg(srcAngularVelocity + srcAngularDelta) );
    }

    // Only solve Destination if needed.
    if ( solveDst )
    {
        pDstPhysics->setGrossLinearVelocity( dstLinearVelocity + dstLinearDelta );
        pDstPhysics->setGrossAngularVelocity( -mRadToDeg(dstAngularVelocity + dstAngularDelta) );
    }
}


//-----------------------------------------------------------------------------
// Resolve Bounce Collisions.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveBounceCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;

    // Solve Source?
    if ( solveSrc )
    {
        // Fetch Linear Velocity / Collision Normal.
        const t2dVector linearVelocity = pCollisionStatus->mpSrcPhysics->getGrossLinearVelocity();
        t2dVector collisionNormal = pCollisionStatus->mCollisionNormal;
        // Normalise Collision Normal.
        collisionNormal.normalise();

        // Calculate Dot Velocity Normal.
        const F32 dotVelocityNormal = collisionNormal * linearVelocity;

        // Any velocity to clamp?
        if ( dotVelocityNormal < 0.0f )
        {
            // Calculate Velocity Delta.
            const t2dVector velocityDelta = collisionNormal*(2.0f*dotVelocityNormal);
            // Set new Linear Velocity.
            // NOTE:-   vO = vI - (2(N夫I)N) 
            pCollisionStatus->mpSrcPhysics->setGrossLinearVelocity( (linearVelocity-velocityDelta) * pCollisionStatus->mpSrcPhysics->getRestitution() );
        }
    }

    // Solve Destination?
    if ( solveDst )
    {
        // Fetch Linear Velocity / Collision Normal.
        const t2dVector linearVelocity = pCollisionStatus->mpDstPhysics->getGrossLinearVelocity();
        t2dVector collisionNormal = -pCollisionStatus->mCollisionNormal;
        // Normalise Collision Normal.
        collisionNormal.normalise();

        // Calculate Dot Velocity Normal.
        const F32 dotVelocityNormal = collisionNormal * linearVelocity;

        // Any velocity to clamp?
        if ( dotVelocityNormal < 0.0f )
        {
            // Calculate Velocity Delta.
            const t2dVector velocityDelta = collisionNormal*(2.0f*dotVelocityNormal);
            // Set new Linear Velocity.
            // NOTE:-   vO = vI - (2(N夫I)N) 
            pCollisionStatus->mpDstPhysics->setGrossLinearVelocity( (linearVelocity-velocityDelta) * pCollisionStatus->mpDstPhysics->getRestitution()  );
        }
    }
}


//-----------------------------------------------------------------------------
// Resolve Clamp Collisions.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveClampCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;

    // Solve Source?
    if ( solveSrc )
    {
        // Fetch Linear Velocity / Collision Normal.
        const t2dVector linearVelocity = pCollisionStatus->mpSrcPhysics->getGrossLinearVelocity();
        t2dVector collisionNormal = pCollisionStatus->mCollisionNormal;
        // Normalise Collision Normal.
        collisionNormal.normalise();

        // Calculate Dot Velocity Normal.
        const F32 dotVelocityNormal = collisionNormal * linearVelocity;

        // Any velocity to clamp?
        if ( dotVelocityNormal < 0.0f )
        {
            // Calculate Velocity Delta.
            const t2dVector velocityDelta = collisionNormal*dotVelocityNormal;
            // Set new Linear Velocity.
            // NOTE:-   vO = vI - ((N夫I)N) 
            pCollisionStatus->mpSrcPhysics->setGrossLinearVelocity( linearVelocity-velocityDelta );
        }
    }

    // Solve Destination?
    if ( solveDst )
    {
        // Fetch Linear Velocity / Collision Normal.
        const t2dVector linearVelocity = pCollisionStatus->mpDstPhysics->getGrossLinearVelocity();
        t2dVector collisionNormal = -pCollisionStatus->mCollisionNormal;
        // Normalise Collision Normal.
        collisionNormal.normalise();

        // Calculate Dot Velocity Normal.
        const F32 dotVelocityNormal = collisionNormal * linearVelocity;

        // Any velocity to clamp?
        if ( dotVelocityNormal < 0.0f )
        {
            // Calculate Velocity Delta.
            const t2dVector velocityDelta = collisionNormal*dotVelocityNormal;
            // Set new Linear Velocity.
            // NOTE:-   vO = vI - ((N夫I)N) 
            pCollisionStatus->mpDstPhysics->setGrossLinearVelocity( linearVelocity-velocityDelta );
        }
    }
}


//-----------------------------------------------------------------------------
// Resolve Sticky Collisions.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveStickyCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;

    // Solve Source?
    if ( solveSrc )
    {
        // Stop Object.
        pCollisionStatus->mpSrcPhysics->setAtRest();
    }

    // Solve Destination?
    if ( solveDst )
    {
        // Stop Object.
        pCollisionStatus->mpDstPhysics->setAtRest();
    }
}


//-----------------------------------------------------------------------------
// Resolve Kill Collisions.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveKillCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;

    // Solve Source?
    if ( solveSrc )
    {
        // Sanity.
        AssertFatal(pCollisionStatus->mSrcObject, "t2dPhysics::resolveKillCollision() - No Source Object for 'T2D_RESPONSE_KILL'!");
        // Do a safe deletion.
        pCollisionStatus->mSrcObject->safeDelete();
    }

    // Solve Destination?
    if ( solveDst )
    {
        // Sanity.
        AssertFatal(pCollisionStatus->mDstObject, "t2dPhysics::resolveKillCollision() - No Destination Object for 'T2D_RESPONSE_KILL'!");
        // Do a safe deletion.
        pCollisionStatus->mDstObject->safeDelete();
    }
}


//-----------------------------------------------------------------------------
// Resolve Custom Collisions.
//-----------------------------------------------------------------------------
void t2dPhysics::resolveCustomCollision( const bool solveSrc, const bool solveDst, cCollisionStatus* pCollisionStatus, const U32 contactIndex )
{
    // Flag as handled.
    pCollisionStatus->mHandled = true;

    // Solve Source?
    if ( solveSrc )
    {
        // Sanity.
        AssertFatal(pCollisionStatus->mSrcObject, "t2dPhysics::resolveCustomCollision() - No Source Object for 'T2D_RESPONSE_CUSTOM'!");
        // Perform Custom Collision.
        pCollisionStatus->mSrcObject->onCustomCollisionResponse( pCollisionStatus );
    }

    // Solve Destination?
    if ( solveDst )
    {
        // Sanity.
        AssertFatal(pCollisionStatus->mDstObject, "t2dPhysics::resolveCustomCollision() - No Destination Object for 'T2D_RESPONSE_CUSTOM'!");

        // Receive Collision Status.
        t2dPhysics::cCollisionStatus receiveCollisionStatus;
        // Reverse Collision Status.
        t2dPhysics::reverseCollisionStatus( pCollisionStatus, &receiveCollisionStatus );

        // Perform Custom Collision.
        pCollisionStatus->mDstObject->onCustomCollisionResponse( pCollisionStatus );
    }
}


//-----------------------------------------------------------------------------
// Cast Line against Collision Bounds.
//
// NOTE:-   This acts as a collision-bounds dispatcher for the line-test.
//-----------------------------------------------------------------------------
bool t2dPhysics::castLine( const t2dVector& startPos, const t2dVector& endPos, t2dVector& collisionNormal, F32& collisionTime )
{
    // Handle Collision Detection Mode.
    switch( getCollisionDetectionMode() )
    {
        // **************************************************
        // Line to Circle->Polygon.
        // **************************************************
        case T2D_DETECTION_FULL:
        {
            // Do Line/Circle.
            if ( !castLineToCircle( startPos, endPos, collisionNormal, collisionTime ) )
                // No collision!
                return false;

            // Do Line/Polygon.
            return castLineToPolygon( startPos, endPos, collisionNormal, collisionTime );

        } break;

        // **************************************************
        // Line to Circle Only.
        // **************************************************
        case T2D_DETECTION_CIRCLE_ONLY:
        {
            // Do Line/Circle Only.
            return castLineToCircle( startPos, endPos, collisionNormal, collisionTime );

        } break;

        // **************************************************
        // Line to Polygon Only.
        // **************************************************
        case T2D_DETECTION_POLY_ONLY:
        {
            // Do Line/Polygon Only.
            return castLineToPolygon( startPos, endPos, collisionNormal, collisionTime );

        } break;
    };

    // Sanity.
    AssertFatal(false, "t2dPhysics::castLine() - Invalid Collision Detection Mode!");

    // Return No Collision (shouldn't get here!).
    return false;
}


//-----------------------------------------------------------------------------
// Cast Line to Circle.
//-----------------------------------------------------------------------------
bool t2dPhysics::castLineToCircle( const t2dVector& startPos, const t2dVector& endPos, t2dVector& collisionNormal, F32& collisionTime )
{
    // Fetch Collision Circle Radii.
    F32 circleRadii = getCollisionCircleRadii();

    // Calculate Line-Segment Vector.
    const t2dVector lineDelta = endPos - startPos;

    // Calculate Line-Segment from Start to center.
    const t2dVector v1_center = getPosition() - startPos;

    // Is Start Position within circle?
    if ( mLessThanOrEqual( v1_center.len(), circleRadii ) )
    {
        // Yes, so collision normal.
        collisionNormal.set( -lineDelta.mY, lineDelta.mX );
        collisionNormal.normalise();
        // Set Collision Time.
        collisionTime = 0.0f;
        // Return Collision!
        return true;
    }

    // Check if line-segment intersects circle...

    // Calculate Sweep Time.
    const F32 sweepTime = v1_center.dot( lineDelta ) / lineDelta.lenSquared();

    // Is the sweep within the target zone?
    if ( mGreaterThanOrEqual(sweepTime,0.0f) && mLessThanOrEqual(sweepTime,1.0f) )
    {
        // Yes, so calculate closest approach. 
        const t2dVector intersection = startPos + (lineDelta*sweepTime);
        // Calculate target distance.
        const F32 targetDist = (intersection - getPosition()).len();

        // Are we within the specifed target margin?
        if ( mLessThanOrEqual(targetDist, circleRadii) )
        {
            // Yes, so collision normal.
            collisionNormal.set( -lineDelta.mY, lineDelta.mX );
            collisionNormal.normalise();
            // Set Collision Time.
            collisionTime = sweepTime;
            // Return Collision!
            return true;
        }
    }

    // No Collision!
    return false;
}

//-----------------------------------------------------------------------------
// Cast Line to Polygon.
//-----------------------------------------------------------------------------
bool t2dPhysics::castLineToPolygon( const t2dVector& startPos, const t2dVector& endPos, t2dVector& collisionNormal, F32& collisionTime )
{
    // Calculate Local-Space Position/Direction.
    const t2dVector localPos = (startPos - getPosition()) ^ getRotationMatrix();
    const t2dVector localDir = (endPos - startPos) ^ getRotationMatrix();

    // Set Near/Far Times.
    F32 timeNear = 0.0f;
    F32 timeFar = 1.0f;

    // Normals.
    t2dVector normalNear;
    t2dVector normalFar;

    // Get Collision Polygon.
    const t2dVector* pCollisionPoly = getCollisionPolyLocal();

    // Test Axes Seperation and iterate polygon.
    for ( U32 j = getCollisionPolyCount()-1, i = 0; i < getCollisionPolyCount(); j = i, i++ )
    {
        t2dVector edge0     = pCollisionPoly[j];
        t2dVector edge1     = pCollisionPoly[i];
        t2dVector edgeDelta = edge1 - edge0;
        t2dVector edgeNormal    ( edgeDelta.mY, -edgeDelta.mX );
        t2dVector edgeDistance = edge0 - localPos;

        // Calculate Clip.
        F32 iDenom = edgeDistance * edgeNormal;
        F32 iNumer = localDir * edgeNormal;

        // Is the ray parallel to the plane?
        if ( mIsZero(mFabs(iNumer)) )
        {
            // Yes, so are we outside of the plane?
            if ( iDenom < 0.0f )
                // Yes, no intersection!
                return false;
        }
        else
        {
            // Set Clip Time.
            F32 timeClip = iDenom / iNumer;

            // Near Intersection?
            if ( iNumer < 0.0f )
            {
                // Yes, so is time is too late?
                if ( timeClip > timeFar )
                    // Yes, no intersection!
                    return false;

                // Valid Intersection?
                if ( timeClip > timeNear )
                {
                    // Yes, so set Near Time.
                    timeNear = timeClip;
                    // Fetch Near-Edge Normal.
                    normalNear = edgeNormal;
                    // Reorientate.
                    normalNear *= getRotationMatrix();
                    // Normalise.
                    normalNear.normalise();
                }
            }
            // Far Intersection?
            else
            {
                // Yes, so is time is too early?
                if ( timeClip < timeNear )
                    // Yes, no intersection!
                    return false;

                // Valid Intersection?
                if ( timeClip < timeFar )
                {
                    // Yes, so set Far Time.
                    timeFar = timeClip;
                    // Fetch Far-Edge Normal.
                    normalFar = edgeNormal;
                    // Reorientate.
                    normalFar *= getRotationMatrix();
                    // Normalise.
                    normalFar.normalise();
                }
            }
        }
    }

    // Ignore far collisions for now!
    if ( timeNear >= 1.0f )
        return false;

    // Set Near Collision Details.
    collisionTime = timeNear;
    collisionNormal = normalNear;
    collisionNormal.normalise();

    // Collision!
    return true;
}


//-----------------------------------------------------------------------------
// Update Net Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::updateNetVelocity( const F32 elapsedTime )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_updateNetVelocity);
#endif

    // Ignore if no reason to be here!
    if ( !mNetLinearVelocityActive && !mNetAngularVelocityActive )
    {
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_updateNetForce
#endif
        // Finish Here.
        return;
    }

    // Cannot proceed without parent initialisation.
    AssertFatal( isInitialised(), "t2dPhysics::updateNetForce() - Cannot proceed without initialisation!" );

    // Update if not suppressed.
    if ( !getPhysicsSuppress() )
    {
        // Update Linear Velocity?
        if ( mNetLinearVelocityActive )
            setGrossLinearVelocity( getGrossLinearVelocity() + (getNetLinearVelocity() * (mInverseMass * elapsedTime)) );

        // Update Angular Velocity?
        if ( mNetAngularVelocityActive )
            setGrossAngularVelocity( getGrossAngularVelocity() + (getNetAngularVelocity() * (mInverseInertialMoment * elapsedTime)) );
    }

    // Reset Net Velocity.
    resetNetVelocity();

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_updateNetForce
#endif
}


//-----------------------------------------------------------------------------
// Damp Gross Velocity.
//-----------------------------------------------------------------------------
void t2dPhysics::dampGrossVelocity( const F32 elapsedTime )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_updateGrossVelocity);
#endif

    // Damping Velocities?
    if ( mDampingActive )
    {
        // Yes, so calculate integrated damping.
        F32 dampIntegral = mDamping * elapsedTime;

        // Any Linear Velocity?
        if ( mGrossLinearVelocityActive )
        {
            // Yes, so get Gross Linear Velocity.
            t2dVector linearVelocity = getGrossLinearVelocity();
            // Fetch Velocity Magnitude.
            F32 linVelMag = linearVelocity.len();
            // Calculate Renormalised Damped-Velocity.
            linearVelocity.normalise( getMax(linVelMag - (linVelMag * dampIntegral), 0.0f) );
            // Set Gross Linear Velocity.
            setGrossLinearVelocity( linearVelocity );
        }

        // Any Angular Velocity?
        if ( mGrossAngularVelocityActive )
        {
            // Yes, so get Angular Velocity.
            F32 angularVelocity = getGrossAngularVelocity();
            // Calculate damped-angular velocity delta.
            F32 deltaAngVelocity = angularVelocity * dampIntegral;
            // Ensure we don't get angular reversal!
            if ( mLessThan(angularVelocity,0.0f) )
                angularVelocity = mGetMin(angularVelocity-deltaAngVelocity, 0.0f);
            else
                angularVelocity = mGetMax(angularVelocity-deltaAngVelocity, 0.0f);

            // Set Gross Angular Velocity.
            setGrossAngularVelocity( angularVelocity );
        }
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_updateGrossVelocity
#endif
}


//-----------------------------------------------------------------------------
// Update Spatials.
//-----------------------------------------------------------------------------
void t2dPhysics::updateSpatials( const F32 elapsedTime, bool suppressLinear )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dPhysics_updateSpatials);
#endif

    // Only update spatials if we're not immovable.
    if ( !getImmovable() )
    {
        // Rotation Update?
        if ( mGrossAngularVelocityActive || mAutoRotationActive )
        {
            // Select the appropriate angular velocity.
            F32 angularVelocity;

            // Auto Rotation Active?
            if ( mAutoRotationActive )
            {
                // Yes, so set angular velocity to auto-rotation.
                angularVelocity = getAutoRotation();
            }
            else
            {
                // No, so set angular velocity to gross angular velocity.
                angularVelocity = getGrossAngularVelocity();
            }

            // Is Rotation Target Active?
            if ( getRotationTargetActive() )
            {
                // Yes, so update rotation target.
                updateRotationTarget( angularVelocity, elapsedTime );
            }
            else
            {
                // No, so do standard rotation integration.
                setRotation( getRotation() + (angularVelocity * elapsedTime) );
            }
        }


        // Position Update?
        if ( !suppressLinear && mGrossLinearVelocityActive )
        {
            // Are we doing forward movement only?
            if ( mForwardMovementOnly )
            {
                // Yes, so calculate forward vector.
                const t2dVector forwardVec(0.0f, -getGrossLinearVelocity().len());

                // Calculate new velocity vector.
                t2dVector directionVec = (forwardVec * getRotationMatrix());

                // Set new Gross Linear Velocity.
                setGrossLinearVelocity( directionVec );

                // Integrate new position.
                setPosition( getPosition() + (directionVec * elapsedTime) );
            }
            else
            {
                // Is Position Target Active?
                if ( getPositionTargetActive() )
                {
                    // Yes, so update position target.
                    updatePositionTarget( elapsedTime );
                }
                else
                {
                    // No, so do standard position integration.
                    setPosition( getPosition() + (getGrossLinearVelocity() * elapsedTime) );
                }
            }
        }
    }
    
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dPhysics_updateSpatials
#endif
}


//-----------------------------------------------------------------------------
// Check Position Target.
//-----------------------------------------------------------------------------
F32 t2dPhysics::checkPositionTarget( const F32 elapsedTime )
{
    // Ignore if Position Target is inactive or being updated.
    if ( !getPositionTargetActive() || mUpdatingPositionTarget ) return elapsedTime;

    // Any Sweeping?
    if ( mIsZero(elapsedTime) )
    {
        // No, so just do a distance calculation.  Within specified distance?
        if ( mLessThanOrEqual( (getPositionTarget()-getPosition()).len(), getPositionTargetMargin() ) )
        {
            // Yes, so time is valid.
            return elapsedTime;
        }
    }

    // Fetch From/To Positions.
    const t2dVector posFrom = getPosition();
    const t2dVector posTo = posFrom + (getGrossLinearVelocity() * elapsedTime);
    // Calculate some components.
    const t2dVector vecOffAxis = getPositionTarget() - posFrom;
    const t2dVector vecSweep = posTo - posFrom;
    // Calculate Sweep Time.
    const F32 sweepTime = vecOffAxis.dot( vecSweep ) / vecSweep.lenSquared();

    // Is the sweep within the target zone?
    if ( mGreaterThanOrEqual(sweepTime,0.0f) && mLessThanOrEqual(sweepTime,1.0f) )
    {
        // Yes, so calculate closest approach. 
        const t2dVector intersection = posFrom + (vecSweep*sweepTime);
        // Calculate target distance.
        const F32 targetDist = (intersection - getPositionTarget()).len();

        // Are we within the specifed target margin?
        if ( mLessThanOrEqual(targetDist, getPositionTargetMargin()) )
        {
            // Return Position Target Time.
            return sweepTime * elapsedTime;
        }
    }

    // Not hit target so full time.
    return elapsedTime;
}

//-----------------------------------------------------------------------------
// Update Position Target.
//-----------------------------------------------------------------------------
void t2dPhysics::updatePositionTarget( const F32 elapsedTime )
{
    // Ignore if Position Target is inactive or being updated.
    if ( !getPositionTargetActive() || mUpdatingPositionTarget ) return;

    // Set Updating Position Target.
    mUpdatingPositionTarget = true;

    // Any Sweeping?
    if ( mIsZero(elapsedTime) )
    {
        // No, so just do a distance calculation.  Within specified distance?
        if ( mLessThanOrEqual( (getPositionTarget()-getPosition()).len(), getPositionTargetMargin() ) )
        {
            // Yes, so process position target.
            processPositionTarget();
        }

        // Reset Updating Position Target.
        mUpdatingPositionTarget = false;
        // No position-integration so finish here.
        return;
    }

    // Fetch From/To Positions.
    const t2dVector posFrom = getPosition();
    const t2dVector posTo = posFrom + (getGrossLinearVelocity() * elapsedTime);
    // Calculate some components.
    const t2dVector vecOffAxis = getPositionTarget() - posFrom;
    const t2dVector vecSweep = posTo - posFrom;
    // Calculate Sweep Time.
    const F32 sweepTime = vecOffAxis.dot( vecSweep ) / vecSweep.lenSquared();

    // Let's move to the final position before we process.
    // NOTE:-   It's important that we process the final position
    //          BEFORE we allow a potential user-callback.  Also,
    //          this is the integration step we MUST do!
    setPosition( posTo );

    // Is the sweep within the target zone?
    if ( mGreaterThanOrEqual(sweepTime,0.0f) && mLessThanOrEqual(sweepTime,1.0f) )
    {
        // Yes, so calculate closest approach. 
        const t2dVector intersection = posFrom + (vecSweep*sweepTime);
        // Calculate target distance.
        const F32 targetDist = (intersection - getPositionTarget()).len();

        // Are we within the specifed target margin?
        if ( mLessThanOrEqual(targetDist, getPositionTargetMargin()) )
        {
            // Yes, so process Position Target.
            processPositionTarget();
        }
    }

    // Reset Updating Position Target.
    mUpdatingPositionTarget = false;
}

//-----------------------------------------------------------------------------
// Process Position Target.
//-----------------------------------------------------------------------------
void t2dPhysics::processPositionTarget( void )
{
	//printf( "t2dPhysics::processPositionTarget\n" ); // RKS -- This is here to see when this is called
	
    // Coincident, so reset Target.
    // NOTE:-   we want to do this here to allow the user to set another
    //          target in a potential callback.
    setPositionTargetOff();

    // Reset Gross Linear Velocity (if selected).
    if ( getPositionTargetAutoStop() ) 
	{
		setGrossLinearVelocity( t2dVector::getZero() );
	}
	
    // Snap Position (if selected).
    if ( getPositionTargetSnap() ) 
	{
		setPosition( getPositionTarget() );
	}
	
	// RKS: I commented this out to avoid the script call
    // Perform Callback (if selected).
    //if ( getPositionTargetCallback() && getRefParentObject() ) Con::executef( getRefParentObject(), 1, "onPositionTarget" );
	if( getPositionTargetCallback() && mRefParentObject )
	{
		// Con::executef( getRefParentObject(), 1, "onPositionTarget" );
		mRefParentObject->OnPositionTargetReached();
	}
}




//-----------------------------------------------------------------------------
// Update Rotation Target.
//-----------------------------------------------------------------------------
void t2dPhysics::updateRotationTarget( const F32 angularVelocity, const F32 elapsedTime )
{
   // Ignore if Rotation Target is inactive or being updated.
   if ( !getRotationTargetActive() || mUpdatingRotationTarget ) return;

   // Set Updating Rotation Target.
   mUpdatingRotationTarget = true;

   // Any Sweeping?
   if ( mIsZero(elapsedTime) || mIsZero(angularVelocity) )
   {
      // No, so just do a distance calculation.  Within specified distance?
      if ( mLessThanOrEqual( mFabs(getRotation()-getRotationTarget()), getRotationTargetMargin() ) )
      {
         // Yes, so process rotation target.
         processRotationTarget();
      }

      // Reset Updating Rotation Target.
      mUpdatingRotationTarget = false;
      // No rotation-integration so finish here.
      return;
   }

   // Fetch From/To Rotations.
   const F32 rotFrom = getRotation();
   const F32 sweep = angularVelocity * elapsedTime;
   const F32 rotTo = rotFrom + sweep;  

   // Fetch Rotation Target.
   F32 rotTarget = getRotationTarget();

   setRotation(rotTo);

   if( mGreaterThanOrEqual( mFabs(sweep + getRotationTargetMargin()),360.0f) )
   {
      processRotationTarget();
   }
   else
      if( mLessThanZero(rotTo) )
      {
         if( mLessThanOrEqual(rotTarget, rotFrom + getRotationTargetMargin() ) || mGreaterThanOrEqual(rotTarget, (rotTo - getRotationTargetMargin()) + 360.0f) )
            processRotationTarget();
      }
      else
         if( mGreaterThan(rotTo,360.0f) )
         {
            if( mGreaterThanOrEqual(rotTarget, rotFrom - getRotationTargetMargin() ) || mLessThanOrEqual(rotTarget, rotTo + getRotationTargetMargin() - 360.0f) )
               processRotationTarget();       
         }
         else
         {
            F32 rotMin, rotMax;      

            mGetMinMax(rotFrom,rotTo,rotMin,rotMax);

            if( mGreaterThanOrEqual(rotTarget, rotMin - getRotationTargetMargin()) && mLessThanOrEqual(rotTarget, rotMax + getRotationTargetMargin()))
               processRotationTarget();
         }

         // Reset Updating Rotation Target.
         mUpdatingRotationTarget = false;
}

//-----------------------------------------------------------------------------
// Process Rotation Target.
//-----------------------------------------------------------------------------
void t2dPhysics::processRotationTarget( void )
{
    // Coincident, so reset Target.
    // NOTE:-   we want to do this here to allow the user to set another
    //          target in a potential callback.
    setRotationTargetOff();

    // Reset Gross Angular / Auto Rotation Velocity (if selected).
    if ( getRotationTargetAutoStop() ) { setGrossAngularVelocity( 0.0f ); setAutoRotation( 0.0f ); };
    // Snap Rotation (if selected).
    if ( getRotationTargetSnap() ) setRotation( getRotationTarget() );
    // Perform Callback (if selected).
    if ( getRotationTargetCallback() && getRefParentObject() ) Con::executef( getRefParentObject(), 1, "onRotationTarget" );
}


//-----------------------------------------------------------------------------
// Output Collision Status.
//-----------------------------------------------------------------------------
void t2dPhysics::outputCollisionStatus( const char* pDescription, const cCollisionStatus* pCollisionStatus )
{
    // Prefetch some stuff.
    t2dPhysics* pSrcPhysics = pCollisionStatus->mpSrcPhysics;
    const t2dVector position = pSrcPhysics->getPosition();
    const t2dVector velocity = pSrcPhysics->getGrossLinearVelocity();

    // Console Output.
    Con::printf(    "%s - ColStat> SrcColPos(%f,%f) / Vel(%f,%f) / Norm(%f,%f) / Col-Time(%f) / Tick-Time(%f) / Contacts:(%d) / SrcContact#0(%f,%f) / DstContact#0(%f,%f) / SrcContact#1(%f,%f) / DstContact#1(%f,%f)",
                    pDescription,
                    position.mX, position.mY,
                    velocity.mX, velocity.mY,
                    pCollisionStatus->mCollisionNormal.mX, pCollisionStatus->mCollisionNormal.mY,
                    pCollisionStatus->mCollisionTimeReal, pCollisionStatus->mFullTimeStep,
                    pCollisionStatus->mContactCount,
                    pCollisionStatus->mSrcContacts[0].mX, pCollisionStatus->mSrcContacts[0].mY,
                    pCollisionStatus->mDstContacts[0].mX, pCollisionStatus->mDstContacts[0].mY,
                    pCollisionStatus->mSrcContacts[1].mX, pCollisionStatus->mSrcContacts[1].mY,
                    pCollisionStatus->mDstContacts[1].mX, pCollisionStatus->mDstContacts[1].mY
                );
}


//-----------------------------------------------------------------------------
// Render Collision Circle.
//-----------------------------------------------------------------------------
// -Mat untested
void t2dPhysics::renderCollisionCircle( void )
{
    // Circle Drawing Resolution.
    // UNUSED: JOSEPH THOMAS -> const U32 circleSteps = 32;

    // Fetch Collision Circle Radii.
    F32 circleRadii = getCollisionCircleRadii();

    // Fetch Center Position.
    const t2dVector center = getPosition();

#ifdef TORQUE_OS_IPHONE
    t2dVector previousPos = t2dVector( center.mX, center.mY + circleRadii );
    for ( F32 angle = 0; angle < M_2PI_F; angle += (M_2PI_F/360.0f) )
    {
        // Draw Point.
        t2dVector pos = t2dVector( center.mX + (circleRadii * -mSin(angle)), center.mY + (circleRadii * mCos(angle)) );
        GLfloat verts[] = {
			(GLfloat)previousPos.mX, (GLfloat)previousPos.mY,
        	(GLfloat)pos.mX, (GLfloat)pos.mY,
		};
        previousPos = pos;

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, verts);
		glDrawArrays(GL_LINES, 0, 4);
	}
#else
    // Draw Collision Polygon.
    glBegin(GL_LINES);

    // Draw Circle.
    t2dVector previousPos = t2dVector( center.mX, center.mY + circleRadii );
    for ( F32 angle = 0; angle < M_2PI_F; angle += (M_2PI_F/360.0f) )
    {
        // Draw Point.
        t2dVector pos = t2dVector( center.mX + (circleRadii * -mSin(angle)), center.mY + (circleRadii * mCos(angle)) );
        glVertex2fv( (GLfloat*)&previousPos );
        glVertex2fv( (GLfloat*)&pos );
        previousPos = pos;
    }
    // End Polygon.
    glEnd();
#endif
}


//-----------------------------------------------------------------------------
// Render Collision Polygon.
//-----------------------------------------------------------------------------
void t2dPhysics::renderCollisionPoly()
{
    // Ignore if we've not got a polygon.
    if ( getCollisionPolyCount() == 0 )
        return;

    // Get Collision Polygon.
    const t2dVector* pCollisionPoly = getCollisionPolyLocal();

    // Draw Collision Polygon.
#ifdef TORQUE_OS_IPHONE

    // Calculate Clip Rectangle.
    for ( U32 i = 1; i <= getCollisionPolyCount(); i++ )
    {
        // Calculate World-Space Point.
        t2dVector wsVertex1 = (pCollisionPoly[i - 1] * mMatrixRotation) + mPosition;
        t2dVector wsVertex2 = (pCollisionPoly[i == getCollisionPolyCount() ? 0 : i] * mMatrixRotation) + mPosition;
        // Draw Point.
		GLfloat verts[] = {
			(GLfloat)wsVertex1.mX, (GLfloat)wsVertex1.mY,
			(GLfloat)wsVertex2.mX, (GLfloat)wsVertex2.mY,
		};
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, verts);
		glDrawArrays(GL_LINES, 0, 4);
    }

    // End Polygon.
#else
    glBegin(GL_LINES);

    // Calculate Clip Rectangle.
    for ( U32 i = 1; i <= getCollisionPolyCount(); i++ )
    {
        // Calculate World-Space Point.
        t2dVector wsVertex1 = (pCollisionPoly[i - 1] * mMatrixRotation) + mPosition;
        t2dVector wsVertex2 = (pCollisionPoly[i == getCollisionPolyCount() ? 0 : i] * mMatrixRotation) + mPosition;
        // Draw Point.
        glVertex2fv( (GLfloat*)&wsVertex1 );
        glVertex2fv( (GLfloat*)&wsVertex2 );
    }

    // End Polygon.
    glEnd();
#endif
    bool drawNobs = true;
    if( drawNobs )
    {
       // Calculate Clip Rectangle.
       for ( U32 i = 0; i < getCollisionPolyCount(); i++ )
       {
           // Calculate World-Space Point.
           t2dVector vertex = (pCollisionPoly[i] * mMatrixRotation) + mPosition;

           F32 size = 0.4f;
#ifdef TORQUE_OS_IPHONE
           glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );
		   GLfloat verts[] = {
			   (GLfloat)vertex.mX - size, (GLfloat)vertex.mY - size,
			   (GLfloat)vertex.mX + size, (GLfloat)vertex.mY - size,
			   (GLfloat)vertex.mX + size, (GLfloat)vertex.mY + size,
			   (GLfloat)vertex.mX - size, (GLfloat)vertex.mY + size,
		   };
		   
		   glEnableClientState(GL_VERTEX_ARRAY);
		   glVertexPointer(2, GL_FLOAT, 0, verts);
		   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

           
		   // Draw Point.
           glColor4f( 0.0f, 1.0f, 0.0f, 1 );
		   GLfloat lines[] = {
				(GLfloat)vertex.mX - size, (GLfloat)vertex.mY - size,
				(GLfloat)vertex.mX + size, (GLfloat)vertex.mY - size,
				(GLfloat)vertex.mX + size, (GLfloat)vertex.mY + size,
				(GLfloat)vertex.mX - size, (GLfloat)vertex.mY + size,
		   };
		   
		   glEnableClientState(GL_VERTEX_ARRAY);
		   glVertexPointer(2, GL_FLOAT, 0, lines);
		   glDrawArrays(GL_LINE_LOOP, 0, 4);
#else

           glColor3f( 1.0f, 1.0f, 0.0f );
           glBegin(GL_QUADS);
              glVertex2f( vertex.mX - size, vertex.mY - size );
              glVertex2f( vertex.mX + size, vertex.mY - size );
              glVertex2f( vertex.mX + size, vertex.mY + size );
              glVertex2f( vertex.mX - size, vertex.mY + size );
           glEnd();

           // Draw Point.
           glColor3f( 0.0f, 1.0f, 0.0f );
           glBegin(GL_LINE_LOOP);
              glVertex2f( vertex.mX - size, vertex.mY - size );
              glVertex2f( vertex.mX + size, vertex.mY - size );
              glVertex2f( vertex.mX + size, vertex.mY + size );
              glVertex2f( vertex.mX - size, vertex.mY + size );
           glEnd();
#endif
       }
    }
}


//-----------------------------------------------------------------------------
// Render Collision History.
//-----------------------------------------------------------------------------
//-Mat untested
void t2dPhysics::renderCollisionHistory( void )
{
    // Fetch Collision History Count.
    const U32 historyCount = mCollisionHistory.size();
    
    // Ignore if we've no history.
    if ( historyCount == 0 )
        return;

    // Set Arrow Dimensions.
    const F32 arrowLength = getMax(Con::getFloatVariable( "$pref::T2D::contactHistoryArrowLength", 5.0f ), 1.0f);
    const F32 arrowWidth = arrowLength * 0.3f;
    const F32 contactCrossSize = getMax(arrowLength * 0.1f, 1.0f);

    // Draw Lines.
    glBegin(GL_LINES);

    // Iterate History Entries.
    for ( U32 entry = 0; entry < historyCount; entry++ )
    {
        // Fetch Collision Status.
        const cCollisionStatus& collisionStatus = mCollisionHistory[entry];

        // Was it an overlap?
        if ( collisionStatus.mOverlapped )
        {
            // Yes, so set overlap color.
            glColor3f(255,0,0);
        }
        else
        {
            // No, so set forward-time color.
            glColor3f(0,255,0);
        }

        // Fetch Contact Count.
        const U32 contactCount = collisionStatus.mContactCount;
        // Fetch Collision Normal.
        const t2dVector& collisionNormal = collisionStatus.mCollisionNormal;
        // Calculate Arrow Points.
        const t2dVector arrowDirection = (collisionNormal * arrowLength);
        const t2dVector arrowHeadLeft = (-collisionNormal * arrowWidth).rotate(-30.0f);
        const t2dVector arrowHeadRight = (-collisionNormal * arrowWidth).rotate(30.0f);

        // Iterate Contacts.
        for ( U32 index = 0; index < contactCount; index++ )
        {
            // Fetch Contacts.
            const t2dVector& srcContactPoint = collisionStatus.mSrcContacts[index];
            const t2dVector& dstContactPoint = collisionStatus.mDstContacts[index];

            // Calculate Arrow-Head points.
            const t2dVector contactArrowHead = srcContactPoint + arrowDirection;
            const t2dVector contactArrowHeadLeft = contactArrowHead + arrowHeadLeft;
            const t2dVector contactArrowHeadRight = contactArrowHead + arrowHeadRight;

            // Draw Arrow.
            glVertex2fv( (GLfloat*)&srcContactPoint );
            glVertex2fv( (GLfloat*)&contactArrowHead );
            glVertex2fv( (GLfloat*)&contactArrowHead );
            glVertex2fv( (GLfloat*)&contactArrowHeadLeft );
            glVertex2fv( (GLfloat*)&contactArrowHead );
            glVertex2fv( (GLfloat*)&contactArrowHeadRight );

            // Draw Source Contact (Square).
            glVertex2f( srcContactPoint.mX-contactCrossSize, srcContactPoint.mY-contactCrossSize );
            glVertex2f( srcContactPoint.mX+contactCrossSize, srcContactPoint.mY-contactCrossSize );
            glVertex2f( srcContactPoint.mX+contactCrossSize, srcContactPoint.mY-contactCrossSize );
            glVertex2f( srcContactPoint.mX+contactCrossSize, srcContactPoint.mY+contactCrossSize );
            glVertex2f( srcContactPoint.mX+contactCrossSize, srcContactPoint.mY+contactCrossSize );
            glVertex2f( srcContactPoint.mX-contactCrossSize, srcContactPoint.mY+contactCrossSize );
            glVertex2f( srcContactPoint.mX-contactCrossSize, srcContactPoint.mY+contactCrossSize );
            glVertex2f( srcContactPoint.mX-contactCrossSize, srcContactPoint.mY-contactCrossSize );

            // Draw Destination Contact (Cross).
            glVertex2f( dstContactPoint.mX-contactCrossSize, dstContactPoint.mY-contactCrossSize );
            glVertex2f( dstContactPoint.mX+contactCrossSize, dstContactPoint.mY+contactCrossSize );
            glVertex2f( dstContactPoint.mX-contactCrossSize, dstContactPoint.mY+contactCrossSize );
            glVertex2f( dstContactPoint.mX+contactCrossSize, dstContactPoint.mY-contactCrossSize );
        }
    }

    // End Lines.
    glEnd();
}



//-----------------------------------------------------------------------------
// Render Collision Bounds.
//-----------------------------------------------------------------------------
void t2dPhysics::renderCollisionBounds( void )
{
    // Render Detection Mode Appropriately.
    switch( getCollisionDetectionMode() )
    {
        case T2D_DETECTION_FULL:
        {
            // Render Collision Circle.
            renderCollisionCircle();

            // Render Collision Polygon.
            renderCollisionPoly();

        } break;

        case T2D_DETECTION_CIRCLE_ONLY:
        {
            // Render Collision Circle.
            renderCollisionCircle();

        } break;

        case T2D_DETECTION_POLY_ONLY:
        {
            // Render Collision Polygon.
            renderCollisionPoly();

        } break;
    };
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dPhysics )
    REGISTER_SERIALISE_VERSION( t2dPhysics, 2, true )
    REGISTER_SERIALISE_VERSION( t2dPhysics, 3, true )
    REGISTER_SERIALISE_VERSION( t2dPhysics, 4, true )
    REGISTER_SERIALISE_VERSION( t2dPhysics, 5, false )
REGISTER_SERIALISE_END()

// Implement Leaf Serialisation.
IMPLEMENT_T2D_SERIALISE_LEAF( t2dPhysics, 5 )


//-----------------------------------------------------------------------------
// Load v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dPhysics, 2 )
{
    U32                 polyVertexCount;
    t2dVector           polyVertex;
    t2dVector           collisionPolyScale;
    t2dVector           constantForce;
    F32                 forceScale;
    bool                graviticConstantForce;
    bool                physicsSuppress;
    t2dVector           position;
    t2dVector           size;
    F32                 rotation;
    F32                 autoRotation;
    F32                 density;
    F32                 coefDynamicFriction;
    F32                 coefRestitution;
    F32                 relaxation;
    t2dVector           linearVelocity;
    F32                 angularVelocity;
    F32                 minLinearVelocity;
    F32                 minAngularVelocity;
    F32                 maxLinearVelocity;
    F32                 maxAngularVelocity;

    // Core Info.
    if (    !stream.read( &polyVertexCount ) ||
            !stream.read( &collisionPolyScale.mX ) ||
            !stream.read( &collisionPolyScale.mY ) ||
            !stream.read( &constantForce.mX ) ||
            !stream.read( &constantForce.mY ) ||
            !stream.read( &forceScale ) ||
            !stream.read( &graviticConstantForce ) ||
            !stream.read( &physicsSuppress ) ||
            !stream.read( &position.mX ) ||
            !stream.read( &position.mY ) ||
            !stream.read( &size.mX ) ||
            !stream.read( &size.mY ) ||
            !stream.read( &rotation ) ||
            !stream.read( &autoRotation ) ||
            !stream.read( &density ) ||
            !stream.read( &coefDynamicFriction ) ||
            !stream.read( &coefRestitution ) ||
            !stream.read( &relaxation ) ||              // Read this in but ignore it!
            !stream.read( &linearVelocity.mX ) ||
            !stream.read( &linearVelocity.mY ) ||
            !stream.read( &angularVelocity ) ||
            !stream.read( &minLinearVelocity ) ||
            !stream.read( &minAngularVelocity ) ||
            !stream.read( &maxLinearVelocity ) ||
            !stream.read( &maxAngularVelocity ) )
        // Error.
        return false;

    // Clear Polygon Basis List.
    object->mCollisionPolyBasisList.clear();
    object->mCollisionPolyBasisList.setSize( polyVertexCount );

    // Read Collision Polygon.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        if (    !stream.read( &(polyVertex.mX) ) ||
                !stream.read( &(polyVertex.mY) ) )
            // Error.
            return false;

        // Set Polygon Basis.
        object->mCollisionPolyBasisList[n] = polyVertex;
    }

    // Set Collision Scale.
    // LEGACY NOTE: This entry was "setCollisionScale()" which meant polygon-scale!
    object->setCollisionPolyScale( collisionPolyScale );
    // Set Size.
    object->setSize( size );
    // Set Density.
    object->setDensity( density );

    // Generate Collision Poly.
    object->generateCollisionPoly();

    // Read Meta-String.
    object->mRefMetaString = stream.readSTString();

    // Set Physics Suppression.
    object->setPhysicsSuppress( physicsSuppress );

    // Set Gross Velocities.
    object->setGrossLinearVelocity( linearVelocity );
    object->setGrossAngularVelocity( angularVelocity );

    // Set Velocity Limits.
    object->setMinLinearVelocity( minLinearVelocity );
    object->setMinAngularVelocity( minAngularVelocity );
    object->setMaxLinearVelocity( maxLinearVelocity );
    object->setMaxAngularVelocity( maxAngularVelocity );

    // Set Constant Force.
    object->setConstantForce( constantForce, graviticConstantForce );

    // Set Other Material Properties.
    object->setDynamicFriction( coefDynamicFriction );
    object->setRestitution( coefRestitution );
    object->setForceScale( forceScale );

    // Set Position.
    object->setPosition( position );
    // Set Rotation.
    object->setRotation( rotation );
    // Set Auto Rotation.
    object->setAutoRotation( autoRotation );

    // Reset Net Velocity.
    object->resetNetVelocity();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Save v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dPhysics, 2 )
{
    if (    !stream.write( object->getCollisionPolyCount() ) ||
            !stream.write( object->getCollisionPolyScale().mX ) ||
            !stream.write( object->getCollisionPolyScale().mY ) ||
            !stream.write( object->getConstantForce().mX ) ||
            !stream.write( object->getConstantForce().mY ) ||
            !stream.write( object->getForceScale() ) ||
            !stream.write( object->getGraviticConstantForceFlag() ) ||
            !stream.write( object->getPhysicsSuppress() ) ||
            !stream.write( object->getPosition().mX ) ||
            !stream.write( object->getPosition().mY ) ||
            !stream.write( object->getSize().mX) ||
            !stream.write( object->getSize().mY) ||
            !stream.write( object->getRotation() ) ||
            !stream.write( object->getAutoRotation() ) ||
            !stream.write( object->getDensity() ) ||
            !stream.write( object->getDynamicFriction() ) ||
            !stream.write( object->getRestitution() ) ||
            !stream.write( 0.5f ) ||           // Deprecated!
            !stream.write( object->getGrossLinearVelocity().mX ) ||
            !stream.write( object->getGrossLinearVelocity().mY ) ||
            !stream.write( object->getGrossAngularVelocity() ) ||
            !stream.write( object->getMinLinearVelocity() ) ||
            !stream.write( object->getMinAngularVelocity() ) ||
            !stream.write( object->getMaxLinearVelocity() ) ||
            !stream.write( object->getMaxAngularVelocity() ) )
        // Error.
        return false;

    // Write Collision Polygon.
    for ( U32 n = 0; n < object->getCollisionPolyCount(); n++ )
    {
        if (    !stream.write( object->mCollisionPolyBasisList[n].mX ) ||
                !stream.write( object->mCollisionPolyBasisList[n].mY ) )
            // Error.
            return false;
    }

    // Write Meta-String.
    stream.writeString( object->getRefMetaString() );

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dPhysics, 3 )
{
    U32                 polyVertexCount;
    t2dVector           polyVertex;
    t2dVector           collisionPolyScale;
    t2dVector           constantForce;
    F32                 forceScale;
    bool                graviticConstantForce;
    bool                physicsSuppress;
    t2dVector           position;
    t2dVector           size;
    F32                 rotation;
    F32                 autoRotation;
    bool                autoMassInertia;
    F32                 mass;
    F32                 inertialMoment;
    F32                 density;
    F32                 coefDynamicFriction;
    F32                 coefRestitution;
    F32                 relaxation;
    t2dVector           linearVelocity;
    F32                 angularVelocity;
    F32                 minLinearVelocity;
    F32                 minAngularVelocity;
    F32                 maxLinearVelocity;
    F32                 maxAngularVelocity;
    eCollisionResponse  collisionResponseMode;

    // Core Info.
    if (    !stream.read( &polyVertexCount ) ||
            !stream.read( &collisionPolyScale.mX ) ||
            !stream.read( &collisionPolyScale.mY ) ||
            !stream.read( &constantForce.mX ) ||
            !stream.read( &constantForce.mY ) ||
            !stream.read( &forceScale ) ||
            !stream.read( &graviticConstantForce ) ||
            !stream.read( &physicsSuppress ) ||
            !stream.read( &position.mX ) ||
            !stream.read( &position.mY ) ||
            !stream.read( &size.mX ) ||
            !stream.read( &size.mY ) ||
            !stream.read( &rotation ) ||
            !stream.read( &autoRotation ) ||
            !stream.read( &autoMassInertia ) ||
            !stream.read( &mass ) ||
            !stream.read( &inertialMoment ) ||
            !stream.read( &density ) ||
            !stream.read( &coefDynamicFriction ) ||
            !stream.read( &coefRestitution ) ||
            !stream.read( &relaxation ) ||              // Read this in but ignore it!
            !stream.read( &linearVelocity.mX ) ||
            !stream.read( &linearVelocity.mY ) ||
            !stream.read( &angularVelocity ) ||
            !stream.read( &minLinearVelocity ) ||
            !stream.read( &minAngularVelocity ) ||
            !stream.read( &maxLinearVelocity ) ||
            !stream.read( &maxAngularVelocity ) ||
            !stream.read( (S32*)&collisionResponseMode ) )
        // Error.
        return false;

    // Clear Polygon Basis List.
    object->mCollisionPolyBasisList.clear();
    object->mCollisionPolyBasisList.setSize( polyVertexCount );

    // Read Collision Polygon.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        if (    !stream.read( &(polyVertex.mX) ) ||
                !stream.read( &(polyVertex.mY) ) )
            // Error.
            return false;

        // Set Polygon Basis.
        object->mCollisionPolyBasisList[n] = polyVertex;
    }

    // Set Auto Mass/Inertia.
    object->setAutoMassInertia( autoMassInertia );

    // Set Mass/Inertial-Moment.
    object->setMass( mass );
    object->setInertialMoment( inertialMoment );

    // Set Collision Scale.
    // LEGACY NOTE: This entry was "setCollisionScale()" which meant polygon-scale!
    object->setCollisionPolyScale( collisionPolyScale );
    // Set Size.
    object->setSize( size );
    // Set Density.
    object->setDensity( density );

    // Generate Collision Poly.
    object->generateCollisionPoly();

    // Set Collision Response Mode.
    object->setCollisionResponseMode( collisionResponseMode );

    // Read Meta-String.
    object->mRefMetaString = stream.readSTString();

    // Set Physics Suppression.
    object->setPhysicsSuppress( physicsSuppress );

    // Set Gross Velocities.
    object->setGrossLinearVelocity( linearVelocity );
    object->setGrossAngularVelocity( mRadToDeg(angularVelocity) );  // This version saved as radians!

    // Set Velocity Limits.
    object->setMinLinearVelocity( minLinearVelocity );
    object->setMinAngularVelocity( minAngularVelocity );
    object->setMaxLinearVelocity( maxLinearVelocity );
    object->setMaxAngularVelocity( maxAngularVelocity );

    // Set Constant Force.
    object->setConstantForce( constantForce, graviticConstantForce );

    // Set Other Material Properties.
    object->setDynamicFriction( coefDynamicFriction );
    object->setRestitution( coefRestitution );
    object->setForceScale( forceScale );

    // Set Position.
    object->setPosition( position );
    // Set Rotation.
    object->setRotation( rotation );
    // Set Auto Rotation.
    object->setAutoRotation( autoRotation );

    // Reset Net Velocity.
    object->resetNetVelocity();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dPhysics, 3 )
{
    if (    !stream.write( object->getCollisionPolyCount() ) ||
            !stream.write( object->getCollisionPolyScale().mX ) ||
            !stream.write( object->getCollisionPolyScale().mY ) ||
            !stream.write( object->getConstantForce().mX ) ||
            !stream.write( object->getConstantForce().mY ) ||
            !stream.write( object->getForceScale() ) ||
            !stream.write( object->getGraviticConstantForceFlag() ) ||
            !stream.write( object->getPhysicsSuppress() ) ||
            !stream.write( object->getPosition().mX ) ||
            !stream.write( object->getPosition().mY ) ||
            !stream.write( object->getSize().mX) ||
            !stream.write( object->getSize().mY) ||
            !stream.write( object->getRotation() ) ||
            !stream.write( object->getAutoRotation() ) ||
            !stream.write( object->getAutoMassInertia() ) ||
            !stream.write( object->getMass() ) ||
            !stream.write( object->getInertialMoment() ) ||
            !stream.write( object->getDensity() ) ||
            !stream.write( object->getDynamicFriction() ) ||
            !stream.write( object->getRestitution() ) ||
            !stream.write( 0.5f ) ||           // Deprecated!
            !stream.write( object->getGrossLinearVelocity().mX ) ||
            !stream.write( object->getGrossLinearVelocity().mY ) ||
            !stream.write( mDegToRad(object->getGrossAngularVelocity()) ) ||    // This version saved as radians!
            !stream.write( object->getMinLinearVelocity() ) ||
            !stream.write( object->getMinAngularVelocity() ) ||
            !stream.write( object->getMaxLinearVelocity() ) ||
            !stream.write( object->getMaxAngularVelocity() ) ||
            !stream.write( (S32)object->getCollisionResponseMode() ) )
        // Error.
        return false;

    // Write Collision Polygon.
    for ( U32 n = 0; n < object->getCollisionPolyCount(); n++ )
    {
        if (    !stream.write( object->mCollisionPolyBasisList[n].mX ) ||
                !stream.write( object->mCollisionPolyBasisList[n].mY ) )
            // Error.
            return false;
    }

    // Write Meta-String.
    stream.writeString( object->getRefMetaString() );

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v4
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dPhysics, 4 )
{
    U32                 polyVertexCount;
    t2dVector           polyVertex;
    t2dVector           collisionPolyScale;
    t2dVector           collisionCircleScale;
    t2dVector           constantForce;
    F32                 forceScale;
    bool                graviticConstantForce;
    bool                physicsSuppress;
    t2dVector           position;
    t2dVector           size;
    F32                 rotation;
    F32                 autoRotation;
    bool                autoMassInertia;
    F32                 mass;
    F32                 inertialMoment;
    F32                 density;
    F32                 coefDynamicFriction;
    F32                 coefRestitution;
    F32                 relaxation;
    t2dVector           linearVelocity;
    F32                 angularVelocity;
    F32                 minLinearVelocity;
    F32                 minAngularVelocity;
    F32                 maxLinearVelocity;
    F32                 maxAngularVelocity;
    U32                 collisionMaxIterations;
    eCollisionResponse  collisionResponseMode;
    eCollisionDetection collisionDetectionMode;
    bool                collisionCircleSuperscribed;
    bool                positionTargetActive;
    t2dVector           positionTarget;
    bool                positionTargetAutoStop;
    bool                positionTargetCallback;
    bool                positionTargetSnap;
    F32                 positionTargetMargin;
    bool                rotationTargetActive;
    F32                 rotationTarget;
    bool                rotationTargetAutoStop;
    bool                rotationTargetCallback;
    bool                rotationTargetSnap;
    F32                 rotationTargetMargin;

    // Core Info.
    if (    !stream.read( &polyVertexCount ) ||
            !stream.read( &collisionPolyScale.mX ) ||
            !stream.read( &collisionPolyScale.mY ) ||
            !stream.read( &collisionCircleScale.mX ) ||
            !stream.read( &collisionCircleScale.mY ) ||
            !stream.read( &constantForce.mX ) ||
            !stream.read( &constantForce.mY ) ||
            !stream.read( &forceScale ) ||
            !stream.read( &graviticConstantForce ) ||
            !stream.read( &physicsSuppress ) ||
            !stream.read( &position.mX ) ||
            !stream.read( &position.mY ) ||
            !stream.read( &size.mX ) ||
            !stream.read( &size.mY ) ||
            !stream.read( &rotation ) ||
            !stream.read( &autoRotation ) ||
            !stream.read( &positionTargetActive ) ||
            !stream.read( &positionTarget.mX ) ||
            !stream.read( &positionTarget.mY ) ||
            !stream.read( &positionTargetAutoStop ) ||
            !stream.read( &positionTargetCallback ) ||
            !stream.read( &positionTargetSnap ) ||
            !stream.read( &positionTargetMargin ) ||
            !stream.read( &rotationTargetActive ) ||
            !stream.read( &rotationTarget ) ||
            !stream.read( &rotationTargetAutoStop ) ||
            !stream.read( &rotationTargetCallback ) ||
            !stream.read( &rotationTargetSnap ) ||
            !stream.read( &rotationTargetMargin ) ||
            !stream.read( &autoMassInertia ) ||
            !stream.read( &mass ) ||
            !stream.read( &inertialMoment ) ||
            !stream.read( &density ) ||
            !stream.read( &coefDynamicFriction ) ||
            !stream.read( &coefRestitution ) ||
            !stream.read( &relaxation ) ||                  // Read this in but ignore it!
            !stream.read( &linearVelocity.mX ) ||
            !stream.read( &linearVelocity.mY ) ||
            !stream.read( &angularVelocity ) ||
            !stream.read( &minLinearVelocity ) ||
            !stream.read( &minAngularVelocity ) ||
            !stream.read( &maxLinearVelocity ) ||
            !stream.read( &maxAngularVelocity ) ||
            !stream.read( &collisionMaxIterations ) ||
            !stream.read( (S32*)&collisionResponseMode ) ||
            !stream.read( (S32*)&collisionDetectionMode ) ||
            !stream.read( &collisionCircleSuperscribed ) )
        // Error.
        return false;

    // Clear Polygon Basis List.
    object->mCollisionPolyBasisList.clear();
    object->mCollisionPolyBasisList.setSize( polyVertexCount );

    // Read Collision Polygon.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        if (    !stream.read( &(polyVertex.mX) ) ||
                !stream.read( &(polyVertex.mY) ) )
            // Error.
            return false;

        // Set Polygon Basis.
        object->mCollisionPolyBasisList[n] = polyVertex;
    }

    // Set Auto Mass/Inertia.
    object->setAutoMassInertia( autoMassInertia );

    // Set Mass/Inertial-Moment.
    object->setMass( mass );
    object->setInertialMoment( inertialMoment );

    // Set Collision Circle Superscribed Mode.
    object->setCollisionCircleSuperscribed( collisionCircleSuperscribed );

    // Set Collision Poly/Circle Scale.
    object->setCollisionPolyScale( collisionPolyScale );
    // NOTE:-   This changed from ellipse to circle so let's just used the maximum radius now.
    object->setCollisionCircleScale( (collisionCircleScale.mX > collisionCircleScale.mY) ? collisionCircleScale.mX : collisionCircleScale.mY  );
    // Set Size.
    object->setSize( size );
    // Set Density.
    object->setDensity( density );

    // Generate Collision Poly.
    object->generateCollisionPoly();

    // Set Collision Maximum Iterations.
    object->setCollisionMaxIterations( collisionMaxIterations );

    // Set Collision Detection Mode.
    object->setCollisionDetectionMode( collisionDetectionMode );

    // Set Collision Response Mode.
    object->setCollisionResponseMode( collisionResponseMode );

    // Read Meta-String.
    object->mRefMetaString = stream.readSTString();

    // Set Physics Suppression.
    object->setPhysicsSuppress( physicsSuppress );

    // Set Gross Velocities.
    object->setGrossLinearVelocity( linearVelocity );
    object->setGrossAngularVelocity( angularVelocity );

    // Set Velocity Limits.
    object->setMinLinearVelocity( minLinearVelocity );
    object->setMinAngularVelocity( minAngularVelocity );
    object->setMaxLinearVelocity( maxLinearVelocity );
    object->setMaxAngularVelocity( maxAngularVelocity );

    // Set Constant Force.
    object->setConstantForce( constantForce, graviticConstantForce );

    // Set Other Material Properties.
    object->setDynamicFriction( coefDynamicFriction );
    object->setRestitution( coefRestitution );
    object->setForceScale( forceScale );

    // Set Position.
    object->setPosition( position );
    // Set Rotation.
    object->setRotation( rotation );
    // Set Auto Rotation.
    object->setAutoRotation( autoRotation );


    // Set Position Target.
    if ( positionTargetActive )
    {
        // Target On.
        object->setPositionTarget( positionTarget, positionTargetAutoStop, positionTargetCallback, positionTargetSnap, positionTargetMargin );
    }
    else
    {
        // Target Off.
        object->setPositionTargetOff();
    }

    // Set Rotation Target.
    if ( rotationTargetActive )
    {
        // Target On.
        object->setRotationTarget( rotationTarget, rotationTargetAutoStop, rotationTargetCallback, rotationTargetSnap, rotationTargetMargin );
    }
    else
    {
        // Target Off.
        object->setRotationTargetOff();
    }


    // Reset Net Velocity.
    object->resetNetVelocity();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v4
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dPhysics, 4 )
{
    if (    !stream.write( object->getCollisionPolyCount() ) ||
            !stream.write( object->getCollisionPolyScale().mX ) ||
            !stream.write( object->getCollisionPolyScale().mY ) ||
            !stream.write( object->getCollisionCircleScale() ) ||
            // NOTE:- This changed from ellipse to circle so write out a dummy!
            !stream.write( T2D_CONST_EPSILON ) ||
            !stream.write( object->getConstantForce().mX ) ||
            !stream.write( object->getConstantForce().mY ) ||
            !stream.write( object->getForceScale() ) ||
            !stream.write( object->getGraviticConstantForceFlag() ) ||
            !stream.write( object->getPhysicsSuppress() ) ||
            !stream.write( object->getPosition().mX ) ||
            !stream.write( object->getPosition().mY ) ||
            !stream.write( object->getSize().mX) ||
            !stream.write( object->getSize().mY) ||
            !stream.write( object->getRotation() ) ||
            !stream.write( object->getAutoRotation() ) ||
            !stream.write( object->getPositionTargetActive() ) ||
            !stream.write( object->getPositionTarget().mX ) ||
            !stream.write( object->getPositionTarget().mY ) ||
            !stream.write( object->getPositionTargetAutoStop() ) ||
            !stream.write( object->getPositionTargetCallback() ) ||
            !stream.write( object->getPositionTargetSnap() ) ||
            !stream.write( object->getPositionTargetMargin() ) ||
            !stream.write( object->getRotationTargetActive() ) ||
            !stream.write( object->getRotationTarget() ) ||
            !stream.write( object->getRotationTargetAutoStop() ) ||
            !stream.write( object->getRotationTargetCallback() ) ||
            !stream.write( object->getRotationTargetSnap() ) ||
            !stream.write( object->getRotationTargetMargin() ) ||
            !stream.write( object->getAutoMassInertia() ) ||
            !stream.write( object->getMass() ) ||
            !stream.write( object->getInertialMoment() ) ||
            !stream.write( object->getDensity() ) ||
            !stream.write( object->getDynamicFriction() ) ||
            !stream.write( object->getRestitution() ) ||
            !stream.write( 0.5f ) ||           // Deprecated!
            !stream.write( object->getGrossLinearVelocity().mX ) ||
            !stream.write( object->getGrossLinearVelocity().mY ) ||
            !stream.write( object->getGrossAngularVelocity() ) ||
            !stream.write( object->getMinLinearVelocity() ) ||
            !stream.write( object->getMinAngularVelocity() ) ||
            !stream.write( object->getMaxLinearVelocity() ) ||
            !stream.write( object->getMaxAngularVelocity() ) ||
            !stream.write( object->getCollisionMaxIterations() ) ||
            !stream.write( (S32)object->getCollisionResponseMode() ) ||
            !stream.write( (S32)object->getCollisionDetectionMode() ) ||
            !stream.write( object->getCollisionCircleSuperscribed() ) )
        // Error.
        return false;

    // Write Collision Polygon.
    for ( U32 n = 0; n < object->getCollisionPolyCount(); n++ )
    {
        if (    !stream.write( object->mCollisionPolyBasisList[n].mX ) ||
                !stream.write( object->mCollisionPolyBasisList[n].mY ) )
            // Error.
            return false;
    }

    // Write Meta-String.
    stream.writeString( object->getRefMetaString() );

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v5
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dPhysics, 5 )
{
    U32                 polyVertexCount;
    t2dVector           polyVertex;
    t2dVector           collisionPolyScale;
    F32                 collisionCircleScale;
    t2dVector           constantForce;
    F32                 forceScale;
    bool                graviticConstantForce;
    bool                physicsSuppress;
    t2dVector           position;
    t2dVector           size;
    F32                 rotation;
    F32                 autoRotation;
    bool                immovable;
    bool                autoMassInertia;
    F32                 mass;
    F32                 inertialMoment;
    F32                 density;
    F32                 coefDynamicFriction;
    F32                 coefRestitution;
    t2dVector           linearVelocity;
    F32                 angularVelocity;
    F32                 minLinearVelocity;
    F32                 minAngularVelocity;
    F32                 maxLinearVelocity;
    F32                 maxAngularVelocity;
    U32                 collisionMaxIterations;
    eCollisionResponse  collisionResponseMode;
    eCollisionDetection collisionDetectionMode;
    bool                collisionCircleSuperscribed;
    bool                positionTargetActive;
    t2dVector           positionTarget;
    bool                positionTargetAutoStop;
    bool                positionTargetCallback;
    bool                positionTargetSnap;
    F32                 positionTargetMargin;
    bool                rotationTargetActive;
    F32                 rotationTarget;
    bool                rotationTargetAutoStop;
    bool                rotationTargetCallback;
    bool                rotationTargetSnap;
    F32                 rotationTargetMargin;

    // Core Info.
    if (    !stream.read( &polyVertexCount ) ||
            !stream.read( &collisionPolyScale.mX ) ||
            !stream.read( &collisionPolyScale.mY ) ||
            !stream.read( &collisionCircleScale ) ||
            !stream.read( &constantForce.mX ) ||
            !stream.read( &constantForce.mY ) ||
            !stream.read( &forceScale ) ||
            !stream.read( &graviticConstantForce ) ||
            !stream.read( &physicsSuppress ) ||
            !stream.read( &position.mX ) ||
            !stream.read( &position.mY ) ||
            !stream.read( &size.mX ) ||
            !stream.read( &size.mY ) ||
            !stream.read( &rotation ) ||
            !stream.read( &autoRotation ) ||
            !stream.read( &positionTargetActive ) ||
            !stream.read( &positionTarget.mX ) ||
            !stream.read( &positionTarget.mY ) ||
            !stream.read( &positionTargetAutoStop ) ||
            !stream.read( &positionTargetCallback ) ||
            !stream.read( &positionTargetSnap ) ||
            !stream.read( &positionTargetMargin ) ||
            !stream.read( &rotationTargetActive ) ||
            !stream.read( &rotationTarget ) ||
            !stream.read( &rotationTargetAutoStop ) ||
            !stream.read( &rotationTargetCallback ) ||
            !stream.read( &rotationTargetSnap ) ||
            !stream.read( &rotationTargetMargin ) ||
            !stream.read( &immovable ) ||
            !stream.read( &autoMassInertia ) ||
            !stream.read( &mass ) ||
            !stream.read( &inertialMoment ) ||
            !stream.read( &density ) ||
            !stream.read( &coefDynamicFriction ) ||
            !stream.read( &coefRestitution ) ||
            !stream.read( &linearVelocity.mX ) ||
            !stream.read( &linearVelocity.mY ) ||
            !stream.read( &angularVelocity ) ||
            !stream.read( &minLinearVelocity ) ||
            !stream.read( &minAngularVelocity ) ||
            !stream.read( &maxLinearVelocity ) ||
            !stream.read( &maxAngularVelocity ) ||
            !stream.read( &collisionMaxIterations ) ||
            !stream.read( (S32*)&collisionResponseMode ) ||
            !stream.read( (S32*)&collisionDetectionMode ) ||
            !stream.read( &collisionCircleSuperscribed ) )
        // Error.
        return false;

    // Clear Polygon Basis List.
    object->mCollisionPolyBasisList.clear();
    object->mCollisionPolyBasisList.setSize( polyVertexCount );

    // Read Collision Polygon.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        if (    !stream.read( &(polyVertex.mX) ) ||
                !stream.read( &(polyVertex.mY) ) )
            // Error.
            return false;

        // Set Polygon Basis.
        object->mCollisionPolyBasisList[n] = polyVertex;
    }

    // Set Immovable.
    object->setImmovable( immovable );
    // Set Auto Mass/Inertia.
    object->setAutoMassInertia( autoMassInertia );

    // Set Mass/Inertial-Moment.
    object->setMass( mass );
    object->setInertialMoment( inertialMoment );

    // Set Collision Circle Superscribed Mode.
    object->setCollisionCircleSuperscribed( collisionCircleSuperscribed );

    // Set Collision Poly/Circle Scales.
    object->setCollisionPolyScale( collisionPolyScale );
    object->setCollisionCircleScale( collisionCircleScale );
    // Set Size.
    object->setSize( size );
    // Set Density.
    object->setDensity( density );

    // Generate Collision Circle.
    object->generateCollisionCircle();
    // Generate Collision Poly.
    object->generateCollisionPoly();

    // Set Collision Maximum Iterations.
    object->setCollisionMaxIterations( collisionMaxIterations );

    // Set Collision Detection Mode.
    object->setCollisionDetectionMode( collisionDetectionMode );

    // Set Collision Response Mode.
    object->setCollisionResponseMode( collisionResponseMode );

    // Read Meta-String.
    object->mRefMetaString = stream.readSTString();

    // Set Physics Suppression.
    object->setPhysicsSuppress( physicsSuppress );

    // Set Gross Velocities.
    object->setGrossLinearVelocity( linearVelocity );
    object->setGrossAngularVelocity( angularVelocity );

    // Set Velocity Limits.
    object->setMinLinearVelocity( minLinearVelocity );
    object->setMinAngularVelocity( minAngularVelocity );
    object->setMaxLinearVelocity( maxLinearVelocity );
    object->setMaxAngularVelocity( maxAngularVelocity );

    // Set Constant Force.
    object->setConstantForce( constantForce, graviticConstantForce );

    // Set Other Material Properties.
    object->setDynamicFriction( coefDynamicFriction );
    object->setRestitution( coefRestitution );
    object->setForceScale( forceScale );

    // Set Position.
    object->setPosition( position );
    // Set Rotation.
    object->setRotation( rotation );
    // Set Auto Rotation.
    object->setAutoRotation( autoRotation );


    // Set Position Target.
    if ( positionTargetActive )
    {
        // Target On.
        object->setPositionTarget( positionTarget, positionTargetAutoStop, positionTargetCallback, positionTargetSnap, positionTargetMargin );
    }
    else
    {
        // Target Off.
        object->setPositionTargetOff();
    }

    // Set Rotation Target.
    if ( rotationTargetActive )
    {
        // Target On.
        object->setRotationTarget( rotationTarget, rotationTargetAutoStop, rotationTargetCallback, rotationTargetSnap, rotationTargetMargin );
    }
    else
    {
        // Target Off.
        object->setRotationTargetOff();
    }


    // Reset Net Velocity.
    object->resetNetVelocity();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v5
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dPhysics, 5 )
{
    if (    !stream.write( object->getCollisionPolyCount() ) ||
            !stream.write( object->getCollisionPolyScale().mX ) ||
            !stream.write( object->getCollisionPolyScale().mY ) ||
            !stream.write( object->getCollisionCircleScale() ) ||
            !stream.write( object->getConstantForce().mX ) ||
            !stream.write( object->getConstantForce().mY ) ||
            !stream.write( object->getForceScale() ) ||
            !stream.write( object->getGraviticConstantForceFlag() ) ||
            !stream.write( object->getPhysicsSuppress() ) ||
            !stream.write( object->getPosition().mX ) ||
            !stream.write( object->getPosition().mY ) ||
            !stream.write( object->getSize().mX) ||
            !stream.write( object->getSize().mY) ||
            !stream.write( object->getRotation() ) ||
            !stream.write( object->getAutoRotation() ) ||
            !stream.write( object->getPositionTargetActive() ) ||
            !stream.write( object->getPositionTarget().mX ) ||
            !stream.write( object->getPositionTarget().mY ) ||
            !stream.write( object->getPositionTargetAutoStop() ) ||
            !stream.write( object->getPositionTargetCallback() ) ||
            !stream.write( object->getPositionTargetSnap() ) ||
            !stream.write( object->getPositionTargetMargin() ) ||
            !stream.write( object->getRotationTargetActive() ) ||
            !stream.write( object->getRotationTarget() ) ||
            !stream.write( object->getRotationTargetAutoStop() ) ||
            !stream.write( object->getRotationTargetCallback() ) ||
            !stream.write( object->getRotationTargetSnap() ) ||
            !stream.write( object->getRotationTargetMargin() ) ||
            !stream.write( object->getImmovable() ) ||
            !stream.write( object->getAutoMassInertia() ) ||
            !stream.write( object->getMass() ) ||
            !stream.write( object->getInertialMoment() ) ||
            !stream.write( object->getDensity() ) ||
            !stream.write( object->getDynamicFriction() ) ||
            !stream.write( object->getRestitution() ) ||
            !stream.write( object->getGrossLinearVelocity().mX ) ||
            !stream.write( object->getGrossLinearVelocity().mY ) ||
            !stream.write( object->getGrossAngularVelocity() ) ||
            !stream.write( object->getMinLinearVelocity() ) ||
            !stream.write( object->getMinAngularVelocity() ) ||
            !stream.write( object->getMaxLinearVelocity() ) ||
            !stream.write( object->getMaxAngularVelocity() ) ||
            !stream.write( object->getCollisionMaxIterations() ) ||
            !stream.write( (S32)object->getCollisionResponseMode() ) ||
            !stream.write( (S32)object->getCollisionDetectionMode() ) ||
            !stream.write( object->getCollisionCircleSuperscribed() ) )
        // Error.
        return false;

    // Write Collision Polygon.
    for ( U32 n = 0; n < object->getCollisionPolyCount(); n++ )
    {
        if (    !stream.write( object->mCollisionPolyBasisList[n].mX ) ||
                !stream.write( object->mCollisionPolyBasisList[n].mY ) )
            // Error.
            return false;
    }

    // Write Meta-String.
    stream.writeString( object->getRefMetaString() );

    // Return Okay.
    return true;
}
