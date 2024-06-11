//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Particle Emitter Object.
//-----------------------------------------------------------------------------

#include "./t2dParticleEffect.h"
#include "./t2dParticleEmitter.h"

#include "dgl/dgl.h"

//-----------------------------------------------------------------------------


IMPLEMENT_CONOBJECT(t2dParticleEmitter);


//-----------------------------------------------------------------------------
// Particle Orientation Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums particleOrientationLookup[] =
                {
                { t2dParticleEmitter::ALIGNED,  "ALIGNED" },
                { t2dParticleEmitter::FIXED,    "FIXED" },
                { t2dParticleEmitter::RANDOM,   "RANDOM" },
                };

//-----------------------------------------------------------------------------
// Particle-Orientation Mode Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dParticleEmitter::tEmitterOrientationMode getParticleOrientationMode(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(particleOrientationLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(particleOrientationLookup[i].label, label) == 0)
            return((t2dParticleEmitter::tEmitterOrientationMode)particleOrientationLookup[i].index);

    // Invalid Orientation!
    AssertFatal(false, "t2dParticleEmitter::getParticleOrientationMode() - Invalid Orientation Mode!");
    // Bah!
    return t2dParticleEmitter::FIXED;
}


//-----------------------------------------------------------------------------
// Emitter Type Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums emitterTypeLookup[] =
                {
                { t2dParticleEmitter::POINT,    "POINT" },
                { t2dParticleEmitter::LINEX,    "LINEX" },
                { t2dParticleEmitter::LINEY,    "LINEY" },
                { t2dParticleEmitter::AREA,     "AREA" },
                };

//-----------------------------------------------------------------------------
// Emitter Type Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dParticleEmitter::tEmitterType getEmitterType(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(emitterTypeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(emitterTypeLookup[i].label, label) == 0)
            return((t2dParticleEmitter::tEmitterType)emitterTypeLookup[i].index);

    // Invalid Emitter-Type!
    AssertFatal(false, "t2dParticleEmitter::getEmitterType() - Invalid Emitter-Type!");
    // Bah!
    return t2dParticleEmitter::POINT;
}



// **************************************************************************************************
//
// NOTE:-   We're using a system where we have zero-degrees vertically which, using the current
//          generation of the camera system, is the negative Y-axis.
//
//          We rotate the orientation angles by 180-deg and use XSin/YCos rather than XCos/YSin
//          so that this equates to a correct spatial orientation for the particles.
//
// **************************************************************************************************


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dParticleEmitter::t2dParticleEmitter( ) : T2D_Stream_HeaderID(makeFourCCTag('2','D','P','E')),
                                            mLocalSerialiseID(1),
                                            mImageMapDataBlock(NULL),
                                            mAnimationSelected(false),
                                            mEmitterName(StringTable->insert("")),
                                            mAnimationName(StringTable->insert("")),
                                            mImageMapName(StringTable->insert("")),
                                            mpFreeParticleNodes(NULL),
                                            mpCurrentGraph(NULL),
                                            mCurrentGraphName(StringTable->insert("")),
                                            mParticlePoolBlockSize(16),
                                            mActiveParticles(0),
                                            mPauseEmitter(false),
                                            mVisible(true),
                                            mUseEmitterCollisions(true)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mParticlePool );

    // Reset Particle Node Head.
    mParticleNodeHead.mNextNode = mParticleNodeHead.mPreviousNode = &mParticleNodeHead;
	mParticlePref = Con::getFloatVariable("$pref::T2D::particleEngineQuantityScale", 1.0f);
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dParticleEmitter::~t2dParticleEmitter()
{
    // Destroy Particle Pool.
    destroyParticlePool();

    // Clear Graph Selections.
    clearGraphSelections();
}

void t2dParticleEmitter::copyTo(SimObject* object)
{
   AssertFatal(dynamic_cast<t2dParticleEmitter*>(object), "t2dParticleEmitter::copyTo - Copy object is not a t2dParticleEmitter!");
   t2dParticleEmitter* emitter = static_cast<t2dParticleEmitter*>(object);

   emitter->mFixedAspect = mFixedAspect;
   emitter->mFixedForceDirection = mFixedForceDirection;
   emitter->mFixedForceAngle = mFixedForceAngle;
   emitter->mParticleOrientationMode = mParticleOrientationMode;
   emitter->mAlign_AngleOffset = mAlign_AngleOffset;
   emitter->mAlign_KeepAligned = mAlign_KeepAligned;
   emitter->mRandom_AngleOffset = mRandom_AngleOffset;
   emitter->mRandom_Arc = mRandom_Arc;
   emitter->mFixed_AngleOffset = mFixed_AngleOffset;
   emitter->mEmitterType = mEmitterType;

   if (mImageMapName && *mImageMapName)
      emitter->setImageMap(mImageMapName, mImageMapFrame);

   if (mAnimationName && *mAnimationName)
      emitter->setAnimationName(mAnimationName);

   emitter->mAnimationSelected = mAnimationSelected;
   emitter->mPivotPoint = mPivotPoint;
   emitter->mUseEffectEmission = mUseEffectEmission;
   emitter->mLinkEmissionRotation = mLinkEmissionRotation;
   emitter->mIntenseParticles = mIntenseParticles;
   emitter->mSingleParticle = mSingleParticle;
   emitter->mAttachPositionToEmitter = mAttachPositionToEmitter;
   emitter->mAttachRotationToEmitter = mAttachRotationToEmitter;
   emitter->mFirstInFrontOrder = mFirstInFrontOrder;

   /// Render Options.
   emitter->mBlending = mBlending;
   emitter->mSrcBlendFactor = mSrcBlendFactor;
   emitter->mDstBlendFactor = mDstBlendFactor;

   /// Collisions.
   emitter->mUseEmitterCollisions = mUseEmitterCollisions;

   mParticleLife.copyTo(emitter->mParticleLife);
   mQuantity.copyTo(emitter->mQuantity);
   mSizeX.copyTo(emitter->mSizeX);
   mSizeY.copyTo(emitter->mSizeY);
   mSpeed.copyTo(emitter->mSpeed);
   mSpin.copyTo(emitter->mSpin);
   mFixedForce.copyTo(emitter->mFixedForce);
   mRandomMotion.copyTo(emitter->mRandomMotion);
   mEmissionForce.copyTo(emitter->mEmissionForce);
   mEmissionAngle.copyTo(emitter->mEmissionAngle);
   mEmissionArc.copyTo(emitter->mEmissionArc);
   mColourRed.copyTo(emitter->mColourRed);
   mColourGreen.copyTo(emitter->mColourGreen);
   mColourBlue.copyTo(emitter->mColourBlue);
   mVisibility.copyTo(emitter->mVisibility);
}

//-----------------------------------------------------------------------------
// Clear Graph Selection.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::clearGraphSelections( void )
{
    // Destroy Graph Selections.
    for ( U32 n = 0; n < mGraphSelectionList.size(); n++ )
        delete mGraphSelectionList[n];

    // Clear List.
    mGraphSelectionList.clear();
}


//-----------------------------------------------------------------------------
// Add Graph Selection.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::addGraphSelection( const char* graphName, t2dGraphField* pGraphObject )
{
    // Generate new Graph Selection.
    tGraphSelection* pGraphSelection = new tGraphSelection;

    // Populate Graph Selection.
    pGraphSelection->mGraphName = StringTable->insert( graphName );
    pGraphSelection->mpGraphObject = pGraphObject;

    // Put into Graph Selection List.
    mGraphSelectionList.push_back( pGraphSelection );
}


//-----------------------------------------------------------------------------
// Find Graph Selection.
//-----------------------------------------------------------------------------
t2dGraphField* t2dParticleEmitter::findGraphSelection( const char* graphName ) const
{
    // Search For Selected Graph and return if found.
    for ( U32 n = 0; n < mGraphSelectionList.size(); n++ )
        if ( mGraphSelectionList[n]->mGraphName == StringTable->insert(graphName) )
            return mGraphSelectionList[n]->mpGraphObject;

    // Return "Not Found".
    return NULL;
}

ConsoleMethod(t2dParticleEmitter, getParentEffect, S32, 2, 2, "()\n @return Returns the effect containing this emitter.")
{
   t2dParticleEffect* effect = object->getParentEffect();
   if (effect)
      return effect->getId();

   return 0;
}

//-----------------------------------------------------------------------------
// Initialise.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::initialise( t2dParticleEffect* pParentEffect )
{
    // Set Parent Effect.
    pParentEffectObject = pParentEffect;

    // ****************************************************************************
    // Initialise Graph Selections.
    // ****************************************************************************
    addGraphSelection( "particlelife_base", &mParticleLife.t2dGraphField_Base );
    addGraphSelection( "particlelife_var", &mParticleLife.t2dGraphField_Variation );

    addGraphSelection( "quantity_base", &mQuantity.t2dGraphField_Base );
    addGraphSelection( "quantity_var", &mQuantity.t2dGraphField_Variation );

    addGraphSelection( "sizex_base", &mSizeX.t2dGraphField_Base );
    addGraphSelection( "sizex_var", &mSizeX.t2dGraphField_Variation );
    addGraphSelection( "sizex_life", &mSizeX.t2dGraphField_OverLife );

    addGraphSelection( "sizey_base", &mSizeY.t2dGraphField_Base );
    addGraphSelection( "sizey_var", &mSizeY.t2dGraphField_Variation );
    addGraphSelection( "sizey_life", &mSizeY.t2dGraphField_OverLife );

    addGraphSelection( "speed_base", &mSpeed.t2dGraphField_Base );
    addGraphSelection( "speed_var", &mSpeed.t2dGraphField_Variation );
    addGraphSelection( "speed_life", &mSpeed.t2dGraphField_OverLife );

    addGraphSelection( "spin_base", &mSpin.t2dGraphField_Base );
    addGraphSelection( "spin_var", &mSpin.t2dGraphField_Variation );
    addGraphSelection( "spin_life", &mSpin.t2dGraphField_OverLife );

    addGraphSelection( "fixedforce_base", &mFixedForce.t2dGraphField_Base );
    addGraphSelection( "fixedforce_var", &mFixedForce.t2dGraphField_Variation );
    addGraphSelection( "fixedforce_life", &mFixedForce.t2dGraphField_OverLife );

    addGraphSelection( "randommotion_base", &mRandomMotion.t2dGraphField_Base );
    addGraphSelection( "randommotion_var", &mRandomMotion.t2dGraphField_Variation );
    addGraphSelection( "randommotion_life", &mRandomMotion.t2dGraphField_OverLife );

    addGraphSelection( "emissionforce_base", &mEmissionForce.t2dGraphField_Base );
    addGraphSelection( "emissionforce_var", &mEmissionForce.t2dGraphField_Variation );

    addGraphSelection( "emissionangle_base", &mEmissionAngle.t2dGraphField_Base );
    addGraphSelection( "emissionangle_var", &mEmissionAngle.t2dGraphField_Variation );

    addGraphSelection( "emissionarc_base", &mEmissionArc.t2dGraphField_Base );
    addGraphSelection( "emissionarc_var", &mEmissionArc.t2dGraphField_Variation );

    addGraphSelection( "red_life", &mColourRed.t2dGraphField_OverLife );
    addGraphSelection( "green_life", &mColourGreen.t2dGraphField_OverLife );
    addGraphSelection( "blue_life", &mColourBlue.t2dGraphField_OverLife );
    addGraphSelection( "visibility_life", &mVisibility.t2dGraphField_OverLife );



    // ****************************************************************************
    // Initialise Graphs.
    // ****************************************************************************
    mParticleLife.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 1000.0f, 2.0f );
    mParticleLife.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );

    mQuantity.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 1000.0f, 10.0f );
    mQuantity.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );

    mSizeX.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 1000.0f, 2.0f );
    mSizeX.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
    mSizeX.t2dGraphField_OverLife.setValueBounds( 1.0f, -100.0f, 100.0f, 1.0f );

    mSizeY.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 1000.0f, 2.0f );
    mSizeY.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
    mSizeY.t2dGraphField_OverLife.setValueBounds( 1.0f, -100.0f, 100.0f, 1.0f );

    mSpeed.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 1000.0f, 10.0f );
    mSpeed.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
    mSpeed.t2dGraphField_OverLife.setValueBounds( 1.0f, -100.0f, 100.0f, 1.0f );

    mSpin.t2dGraphField_Base.setValueBounds( 1000.0f, -3600.0f, 3600.0f, 0.0f );
    mSpin.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
    mSpin.t2dGraphField_OverLife.setValueBounds( 1.0f, -100.0f, 100.0f, 1.0f );

    mFixedForce.t2dGraphField_Base.setValueBounds( 1000.0f, -1000.0f, 1000.0f, 0.0f );
    mFixedForce.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
    mFixedForce.t2dGraphField_OverLife.setValueBounds( 1.0f, -100.0f, 100.0f, 1.0f );

    mRandomMotion.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 1000.0f, 0.0f );
    mRandomMotion.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
    mRandomMotion.t2dGraphField_OverLife.setValueBounds( 1.0f, -100.0f, 100.0f, 1.0f );

    mEmissionForce.t2dGraphField_Base.setValueBounds( 1000.0f, -1000.0f, 1000.0f, 5.0f );
    mEmissionForce.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );

    mEmissionAngle.t2dGraphField_Base.setValueBounds( 1000.0f, -3600.0f, 3600.0f, 0.0f );
    mEmissionAngle.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 720.0f, 0.0f );

    mEmissionArc.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 360.0f, 360.0f );
    mEmissionArc.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 720.0f, 0.0f );

    mColourRed.t2dGraphField_OverLife.setValueBounds( 1.0f, 0.0f, 1.0f, 1.0f );
    mColourGreen.t2dGraphField_OverLife.setValueBounds( 1.0f, 0.0f, 1.0f, 1.0f );
    mColourBlue.t2dGraphField_OverLife.setValueBounds( 1.0f, 0.0f, 1.0f, 1.0f );
    mVisibility.t2dGraphField_OverLife.setValueBounds( 1.0f, 0.0f, 1.0f, 1.0f );


    // Set Other Properties.
    mFixedAspect = true;
    setFixedForceAngle(0.0f);
    mParticleOrientationMode = FIXED;
    mFixed_AngleOffset = 0.0f;
    mAlign_AngleOffset = 0.0f;
    mAlign_KeepAligned = false;
    mRandom_AngleOffset = 0.0f;
    mRandom_Arc = 360.0f;
    mEmitterType = POINT;

    // Other Properties.
    setPivotPoint( t2dVector(0.5f, 0.5f) );
    mAnimationSelected = false;
    mImageMapDataBlock = NULL;
    mUseEffectEmission = true;
    mLinkEmissionRotation = false;
    mIntenseParticles = false;
    mSingleParticle = false;
    mAttachPositionToEmitter = false;
    mAttachRotationToEmitter = false;
    mFirstInFrontOrder = false;

    // Rendering Options.
    mBlending = true;
    mSrcBlendFactor = GL_SRC_ALPHA;
    mDstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
}




//-----------------------------------------------------------------------------
// Select Graph.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, selectGraph, void, 3, 3, "(graphName) - Select Graph Name.")
{
    // Select Graph.
    object->selectGraph( argv[2] );
}
// Select Graph.
void t2dParticleEmitter::selectGraph( const char* graphName )
{
    // Find and Selected Graph.
    mpCurrentGraph = findGraphSelection( graphName );

    // Was it a registered graph?
    if ( mpCurrentGraph )
    {
        // Yes, so store name.
        mCurrentGraphName = StringTable->insert( graphName );
    }
    else
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::selectGraph() - Invalid Graph Selected! (%s)", graphName );
        return;
    }
}


//-----------------------------------------------------------------------------
// Add Data Key.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, addDataKey, S32, 4, 4, "(time, value) - Add Data-Key to Graph.")
{
    // Add Data Key.
    return object->addDataKey( dAtof(argv[2]), dAtof(argv[3]) );
}
// Add Data Key.
S32 t2dParticleEmitter::addDataKey( F32 time, F32 value )
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::addDataKey() - No Graph Selected!" );
        return -1;
    }

    // Add Data Key.
    return mpCurrentGraph->addDataKey( time, value );
}


//-----------------------------------------------------------------------------
// Remove Data Key.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, removeDataKey, bool, 3, 3, "(keyIndex) - Remove Data-Key from Graph.")
{
    // Remove Data Key.
    return object->removeDataKey( dAtoi(argv[2]) );
}
// Remove Data Key.
bool t2dParticleEmitter::removeDataKey( S32 index )
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::removeDataKey() - No Graph Selected!" );
        return false;
    }

    // Remove Data Key.
    return mpCurrentGraph->removeDataKey( index );
}


//-----------------------------------------------------------------------------
// Clear Data Keys.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, clearDataKeys, bool, 2, 2, "Clear Data-Key(s) from Graph.")
{
    // Clear Data Keys
    return object->clearDataKeys();
}
// Clear Data Keys
bool t2dParticleEmitter::clearDataKeys( void )
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEffect::clearDataKeys() - No Graph Selected!" );
        return false;
    }

    // Clear Data Keys
    mpCurrentGraph->clearDataKeys();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Set Data Key Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setDataKeyValue, bool, 4, 4, "(keyIndex, value) - Set Data-Key Value in Graph.")
{
    // Set Data Key Value.
    return object->setDataKeyValue( dAtoi(argv[2]), dAtof(argv[3]) );
}
// Set Data Key Value.
bool t2dParticleEmitter::setDataKeyValue( S32 index, F32 value )
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::setDataKeyValue() - No Graph Selected!" );
        return false;
    }

    // Set Data Key Value.
    return mpCurrentGraph->setDataKeyValue( index, value );
}



//-----------------------------------------------------------------------------
// Get Data Key
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getDataKey, const char*, 3, 3, "(keyIndex) - Get Data-Key Time/Value from Graph.")
{
    // Fetch Key Index.
    S32 keyIndex = dAtoi(argv[2]);

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", object->getDataKeyTime( keyIndex ), object->getDataKeyValue( keyIndex ) );
    // Return buffer.
    return pBuffer;
}
// Get Data Key Value.
F32 t2dParticleEmitter::getDataKeyValue( S32 index ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getDataKeyValue() - No Graph Selected!" );
        return false;
    }

    // Get Data Key Value.
    return mpCurrentGraph->getDataKeyValue( index );
}
// Get Data Key Time.
F32 t2dParticleEmitter::getDataKeyTime( S32 index ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getDataKeyTime() - No Graph Selected!" );
        return false;
    }

    // Get Data Key Time.
    return mpCurrentGraph->getDataKeyTime( index );
}


//-----------------------------------------------------------------------------
// Get Data Key Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getDataKeyCount, S32, 2, 2, "Get Data-Key Count from Graph.")
{
    // Get Data Key Count.
    return object->getDataKeyCount();
}
// Get Data Key Count.
U32 t2dParticleEmitter::getDataKeyCount( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getDataKeyCount() - No Graph Selected!" );
        return false;
    }

    // Get Data Key Count.
    return mpCurrentGraph->getDataKeyCount();
}


//-----------------------------------------------------------------------------
// Get Min Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getMinValue, F32, 2, 2, "Get Min-Value from Graph.")
{
    // Get Min Value.
    return object->getMinValue();
}
// Get Min Value.
F32 t2dParticleEmitter::getMinValue( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getMinValue() - No Graph Selected!" );
        return false;
    }

    // Get Min Value.
    return mpCurrentGraph->getMinValue();
}


//-----------------------------------------------------------------------------
// Get Max Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getMaxValue, F32, 2, 2, "Get Max-Value from Graph.")
{
    // Get Max Value.
    return object->getMaxValue();
}
// Get Max Value.
F32 t2dParticleEmitter::getMaxValue( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getMaxValue() - No Graph Selected!" );
        return false;
    }

    // Get Max Value.
    return mpCurrentGraph->getMaxValue();
}


//-----------------------------------------------------------------------------
// Get Min Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getMinTime, F32, 2, 2, "Get Min-Time from Graph.")
{
    // Get Min Time.
    return object->getMinTime();
}
// Get Min Time.
F32 t2dParticleEmitter::getMinTime( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getMinTime() - No Graph Selected!" );
        return false;
    }

    // Get Min Time.
    return mpCurrentGraph->getMinTime();
}


//-----------------------------------------------------------------------------
// Get Max Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getMaxTime, F32, 2, 2, "Get Max-Value from Graph.")
{
    // Get Max Time.
    return object->getMaxTime();
}
// Get Max Time.
F32 t2dParticleEmitter::getMaxTime( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getMaxTime() - No Graph Selected!" );
        return false;
    }

    // Get Max Time.
    return mpCurrentGraph->getMaxTime();
}


//-----------------------------------------------------------------------------
// Get Graph Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getGraphValue, F32, 3, 3, "(time) - Get Value from Graph.")
{
    // Get Graph Value.
    return object->getGraphValue( dAtof(argv[2]) );
}
// Get Graph Value.
F32 t2dParticleEmitter::getGraphValue( F32 time ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getGraphValue() - No Graph Selected!" );
        return false;
    }

    // Get Graph Value.
    return mpCurrentGraph->getGraphValue( time );
}



//-----------------------------------------------------------------------------
// Set Time Repeat.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setTimeRepeat, bool, 3, 3, "(timeRepeat) - Set Time-Repeat for Graph.")
{
    // Set Time Repeat.
    return object->setTimeRepeat( dAtof(argv[2]) );
}
// Set Time Repeat.
bool t2dParticleEmitter::setTimeRepeat( F32 timeRepeat )
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::setTimeRepeat() - No Graph Selected!" );
        return false;
    }

    // Set Time Repeat.
    return mpCurrentGraph->setTimeRepeat( timeRepeat );
}


//-----------------------------------------------------------------------------
// Get Time Repeat.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getTimeRepeat, F32, 2, 2, "Get Time-Repeat for Graph.")
{
    // Get Time Repeat.
    return object->getTimeRepeat();
}
// Get Time Repeat.
F32 t2dParticleEmitter::getTimeRepeat( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getTimeRepeat() - No Graph Selected!" );
        return false;
    }

    // Get Time Repeat.
    return mpCurrentGraph->getTimeRepeat();
}


//-----------------------------------------------------------------------------
// Set Value Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setValueScale, bool, 3, 3, "(valueScale) - Set Value-Scale For Graph.")
{
    // Set Value Scale.
    return object->setValueScale( dAtof(argv[2]) );
}
// Set Value Scale.
bool t2dParticleEmitter::setValueScale( const F32 valueScale )
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::setValueScale() - No Graph Selected!" );
        return false;
    }

    // Set Value Scale.
    return mpCurrentGraph->setValueScale( valueScale );
}


//-----------------------------------------------------------------------------
// Get Value Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getValueScale, F32, 2, 2, "Get Value-Scale for Graph.")
{
    // Get Value Scale.
    return object->getValueScale();
}
// Get Value Scale.
F32 t2dParticleEmitter::getValueScale( void ) const
{
    // Have we got a valid Graph Selected?
    if ( !mpCurrentGraph )
    {
        // No, so warn.
        Con::warnf( "t2dParticleEmitter::getValueScale() - No Graph Selected!" );
        return false;
    }

    // Get Value Scale.
    return mpCurrentGraph->getValueScale();
}


//-----------------------------------------------------------------------------
// Set Emitter Visibility.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setVisible, void, 3, 3, "(status?) - Set the Emitters Visibility.")
{
    // Set Emitter Visibility.
    object->setVisible( dAtob(argv[2]) );
}
// Set Emitter Visibility.
void t2dParticleEmitter::setVisible( bool status )
{
    // Set Emitter Visibility.
    mVisible = status;

    // Free All Particles ( if making invisible ).
    if ( !mVisible )
        freeAllParticles();
}


//-----------------------------------------------------------------------------
// Set Emitter Name.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setEmitterName, void, 3, 3, "(emitterName$) - Set the Emitters Name.")
{
    // Set Emitter Name.
    object->setEmitterName( argv[2] );
}
// Set Emitter Name.
void t2dParticleEmitter::setEmitterName( const char* emitterName )
{
    // Set Emitter Name.
    mEmitterName = StringTable->insert(emitterName);
}



//-----------------------------------------------------------------------------
// Set Fixed Aspect.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setFixedAspect, void, 3, 3, "(fixedAspect) - Set Fixed-Aspect.")
{
    // Set Fixed Aspect.
    object->setFixedAspect( dAtob(argv[2]) );
}
// Set Fixed Aspect.
void t2dParticleEmitter::setFixedAspect( bool aspect )
{
    // Sets Fixed Aspect.
    mFixedAspect = aspect;
}


//-----------------------------------------------------------------------------
// Set Fixed-Force Angle.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setFixedForceAngle, void, 3, 3, "(fixedForceAngle) - Set Fixed-Force Angle.")
{
    // Set Fixed-Force Angle.
    object->setFixedForceAngle( dAtof(argv[2]) );
}
// Set Fixed-Force Angle.
void t2dParticleEmitter::setFixedForceAngle( F32 fixedForceAngle )
{
    // Set Fixed-Force Angle.
    mFixedForceAngle = fixedForceAngle;

    // Set Fixed-Force Direction.
    mFixedForceDirection.set( mSin(mDegToRad(mFixedForceAngle)), -mCos(mDegToRad(mFixedForceAngle)) );
}


//-----------------------------------------------------------------------------
// Set Particle Orientation Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setParticleOrientationMode, void, 3, 3, "(particleOrientationMode$) - Set Particle Orientation.")
{
    // Set Particle Orientation Mode.
    object->setParticleOrientationMode( getParticleOrientationMode(argv[2]) );
}
// Set Particle Orientation Mode.
void t2dParticleEmitter::setParticleOrientationMode( tEmitterOrientationMode particleOrientationMode )
{
    // Set Particle Orientation Mode.
    mParticleOrientationMode = particleOrientationMode;
}


//-----------------------------------------------------------------------------
// Set Align Angle Offset.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setAlignAngleOffset, void, 3, 3, "(alignAngleOffset) - Set Align-Orientation Angle Offset.")
{
    // Set Align Angle Offset.
    object->setAlignAngleOffset( dAtof(argv[2]) );
}
// Set Align Angle Offset.
void t2dParticleEmitter::setAlignAngleOffset( F32 angleOffset )
{
    // Set Align Angle Offset.
    mAlign_AngleOffset = angleOffset;
}


//-----------------------------------------------------------------------------
// Set Align Keep Aligned.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setAlignKeepAligned, void, 3, 3, "(keepAligned) - Set Align-Orientation Keep-Aligned Flag.")
{
    // Set Align Keep Aligned.
    object->setAlignKeepAligned( dAtob(argv[2]) );
}
// Set Align Keep Aligned.
void t2dParticleEmitter::setAlignKeepAligned( bool keepAligned )
{
    // Set Align Keep Aligned.
    mAlign_KeepAligned = keepAligned;
}


//-----------------------------------------------------------------------------
// Set Random Angle Offset.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setRandomAngleOffset, void, 3, 3, "(randomAngle) - Set Random-Orientation Angle-Offset.")
{
    // Set Random Angle Offset.
    object->setRandomAngleOffset( dAtof(argv[2]) );
}
// Set Random Angle Offset.
void t2dParticleEmitter::setRandomAngleOffset( F32 angleOffset )
{
    // Set Random Angle.
    mRandom_AngleOffset = angleOffset;
}


//-----------------------------------------------------------------------------
// Set Random Arc.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setRandomArc, void, 3, 3, "(randomArc) - Set Random-Orientation Arc.")
{
    // Set Random Arc.
    object->setRandomArc( dAtof(argv[2]) );
}
// Set Random Arc.
void t2dParticleEmitter::setRandomArc( F32 arc )
{
    // Set Random Arc.
    mRandom_Arc = arc;
}


//-----------------------------------------------------------------------------
// Set Fixed Angle Offset.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setFixedAngleOffset, void, 3, 3, "(randomAngle) - Set Fixed-Orientation Angle-Offset.")
{
    // Set Fixed Angle Offset.
    object->setFixedAngleOffset( dAtof(argv[2]) );
}
// Set Fixed Angle Offset.
void t2dParticleEmitter::setFixedAngleOffset( F32 angleOffset )
{
    // Set Fixed Angle Offset.
    mFixed_AngleOffset = angleOffset;
}


//-----------------------------------------------------------------------------
// Set Emitter Type.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setEmitterType, void, 3, 3, "(emitterType$) - Set Emitter Type.")
{
    // Set Emitter Type.
    object->setEmitterType( getEmitterType(argv[2]) );
}
// Set Emitter Type.
void t2dParticleEmitter::setEmitterType( tEmitterType emitterType )
{
    // Set Emitter Type.
    mEmitterType = emitterType;
}


//-----------------------------------------------------------------------------
// Set ImageMap/Frame.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setImageMap, bool, 3, 4, "(imageMapName$, [int frame]) - Set the ImageMap/Frame.")
{
    // Calculate Frame.
    U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;

    // Set ImageMap/Frame.
    return object->setImageMap( argv[2], frame );
}
// Set ImageMap/Frame.
bool t2dParticleEmitter::setImageMap( const char* imageMapName, U32 frame )
{
   // Invalid ImageMap Name.
   if ( !imageMapName || imageMapName == StringTable->insert("") )
      return false;

   // Find ImageMap Datablock.
    t2dImageMapDatablock* pImageMapDataBlock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject( imageMapName ));

    // Set Datablock.
    if ( !t2dCheckDatablock( pImageMapDataBlock ) )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::setImageMap() - t2dImageMapDatablock Datablock is invalid! (%s)", imageMapName);
        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= pImageMapDataBlock->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::setImageMap() - Invalid Frame #%d for t2dImageMapDatablock Datablock! (%s)", frame, imageMapName);
        // Return Here.
        return false;
    }

    // Flag Animation Not Selected.
    mAnimationSelected = false;

    // Set ImageMap Name.
    mImageMapName = StringTable->insert(imageMapName);

    // Set ImageMap Datablock.
    mImageMapDataBlock = pImageMapDataBlock;

    // Set Frame.
    mImageMapFrame = frame;

    // Free All Particles.
    freeAllParticles();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Set Animation Name.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setAnimationName, bool, 3, 3, "(animationName$) - Set the Animation.")
{
    // Set Animation Name.
    return object->setAnimationName( argv[2] );
}
// Set Animation Name.
bool t2dParticleEmitter::setAnimationName( const char* animationName )
{
    // Attempt to Play Animation.
    // NOTE:-   This animation controller is used as a proxy simply to test
    //          validity of the animationName that's passed.  Other than that,
    //          it's not used for rendering particles.
    bool validAnimation = mAnimationControllerProxy.playAnimation( animationName, false );

    // Did we get a valid animation?
    if ( validAnimation )
    {
        // Yes, so flag Animation Selected.
        mAnimationSelected = true;
        // Store Animation Name.
        mAnimationName = StringTable->insert( animationName );
    }

    // Return Animation Status.
    return validAnimation;
}


//-----------------------------------------------------------------------------
// Set Pivot Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setPivotPoint, void, 3, 4, "(pivotX / pivotY) - Set the Pivot-Point.")
{
   // The pivot point.
   F32 pivotX, pivotY;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("pivotX pivotY")
   if ((elementCount == 2) && (argc < 4))
   {
      pivotX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      pivotY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (pivotX, pivotY)
   else if ((elementCount == 1) && (argc > 3))
   {
      pivotX = dAtof(argv[2]);
      pivotY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dTileLayer::setPivotPoint() - Invalid number of parameters!");
      return;
   }

    // Set Pivot Point.
    object->setPivotPoint(t2dVector(pivotX, pivotY));
}
// Set Pivot Point.
void t2dParticleEmitter::setPivotPoint( t2dVector pivotPoint )
{
    // Set Pivot-Point.
    mPivotPoint.mX = pivotPoint.mX;
    mPivotPoint.mY = pivotPoint.mY;

    // Calculate Global Clip Boundary.
    mGlobalClipBoundary[0].set( -mPivotPoint.mX, -mPivotPoint.mY );
    mGlobalClipBoundary[1].set( 1.0f-mPivotPoint.mX, -mPivotPoint.mY );
    mGlobalClipBoundary[2].set( 1.0f-mPivotPoint.mX, 1.0f-mPivotPoint.mY );
    mGlobalClipBoundary[3].set( -mPivotPoint.mX, 1.0f-mPivotPoint.mY );
}


//-----------------------------------------------------------------------------
// Set Use Effect Emission.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setUseEffectEmission, void, 3, 3, "(useEffectEmission) - Set Use-Effect-Emission Flag.")
{
    // Set Use Effect Emission.
    object->setUseEffectEmission( dAtob(argv[2]) );
}
// Set Use Effect Emission.
void t2dParticleEmitter::setUseEffectEmission( bool useEffectEmission )
{
    // Set Use Effect Emission.
    mUseEffectEmission = useEffectEmission;
}


//-----------------------------------------------------------------------------
// Set Link Emission Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setLinkEmissionRotation, void, 3, 3, "(linkEmissionRotation) - Set Link-Emission-Rotation Flag.")
{
    // Set Link Emission Rotation.
    object->setLinkEmissionRotation( dAtob(argv[2]) );
}
// Set Link Emission Rotation.
void t2dParticleEmitter::setLinkEmissionRotation( bool linkEmissionRotation )
{
    // Set Link Emission Rotation.
    mLinkEmissionRotation = linkEmissionRotation;
}


//-----------------------------------------------------------------------------
// Set Intense Particles.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setIntenseParticles, void, 3, 3, "(intenseParticles) - Set Intense-Particles Flag.")
{
    // Set Intense Particles.
    object->setIntenseParticles( dAtob(argv[2]) );
}
// Set Intense Particles.
void t2dParticleEmitter::setIntenseParticles( bool intenseParticles )
{
    // Set Intense Particles.
    mIntenseParticles = intenseParticles;
}


//-----------------------------------------------------------------------------
// Set Single Particle.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setSingleParticle, void, 3, 3, "(singleParticle) - Set Single-Particle Flag.")
{
    // Set Single Particle.
    object->setSingleParticle( dAtob(argv[2]) );
}
// Set Single Particle.
void t2dParticleEmitter::setSingleParticle( bool singleParticle )
{
    // Set Single Particle.
    mSingleParticle = singleParticle;

    // Free All Particles.
    freeAllParticles();
}


//-----------------------------------------------------------------------------
// Set Attach Position To Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setAttachPositionToEmitter, void, 3, 3, "(attachPositionToEmitter) - Set Attach-Position-To-Emitter Flag.")
{
    // Set Attach Position To Emitter.
    object->setAttachPositionToEmitter( dAtob(argv[2]) );
}
// Set Attach Position To Emitter.
void t2dParticleEmitter::setAttachPositionToEmitter( bool attachPositionToEmitter )
{
    // Do we need to attach to the emitter?
    if ( attachPositionToEmitter && !mAttachPositionToEmitter )
    {
        // Yes, so we need to translate the position into Local Effect-Space...

        // Get Parent Effect Position.
        const t2dVector effectPos = pParentEffectObject->getPosition();

        // Fetch First Particle.
        tParticleNode* pParticleNode = mParticleNodeHead.mNextNode;

        // Process All particle nodes.
        while ( pParticleNode != &mParticleNodeHead )
        {
            // Move into Local Effect-Space
            pParticleNode->mPosition -= effectPos;

            // Move to next Particle.
            pParticleNode = pParticleNode->mNextNode;
        };

    }
    else if ( !attachPositionToEmitter && mAttachPositionToEmitter )
    {
        // Yes, so we need to translate the positions permanently into World-Space...

        // Get Parent Effect Position.
        const t2dVector effectPos = pParentEffectObject->getPosition();

        // Fetch First Particle.
        tParticleNode* pParticleNode = mParticleNodeHead.mNextNode;

        // Process All particle nodes.
        while ( pParticleNode != &mParticleNodeHead )
        {
            // Move into World-Space.
            pParticleNode->mPosition += effectPos;

            // Move to next Particle.
            pParticleNode = pParticleNode->mNextNode;
        };
    }

    // Set Attach Position To Emitter.
    mAttachPositionToEmitter = attachPositionToEmitter;
}


//-----------------------------------------------------------------------------
// Set Attach Rotation To Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setAttachRotationToEmitter, void, 3, 3, "(attachRotationToEmitter) - Set Attach-Rotation-To-Emitter Flag.")
{
    // Set Attach Rotation To Emitter.
    object->setAttachRotationToEmitter( dAtob(argv[2]) );
}
// Set Attach Rotation To Emitter.
void t2dParticleEmitter::setAttachRotationToEmitter( bool attachRotationToEmitter )
{
    // Set Attach Rotation To Emitter.
    mAttachRotationToEmitter = attachRotationToEmitter;
}


//-----------------------------------------------------------------------------
// Set First In Front Order.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setFirstInFrontOrder, void, 3, 3, "(firstInFrontOrder) - Set First-InFront-Order Flag.")
{
    // Set First In Front Order.
    object->setFirstInFrontOrder( dAtob(argv[2]) );
}
// Set First In Front Order.
void t2dParticleEmitter::setFirstInFrontOrder( bool firstInFrontOrder )
{
    // Set First In Front Order.
    mFirstInFrontOrder = firstInFrontOrder;
}


//-----------------------------------------------------------------------------
// Set Blending.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setBlending, void, 3, 5, "(blendStatus?, [srcBlendFactor], [dstBlendFactor]) - Sets the Rendering Blend Options.")
{
    // Calculate Blending Factors.
    S32 srcBlendFactor = argc >= 4 ? getSrcBlendFactorEnum(argv[3]) : GL_SRC_ALPHA;
    S32 dstBlendFactor = argc >= 5 ? getDstBlendFactorEnum(argv[4]) : GL_ONE_MINUS_SRC_ALPHA;

    // Check Source Factor.
    if ( srcBlendFactor == GL_INVALID_BLEND_FACTOR )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::setBlending() - Invalid srcBlendFactor '%s', defaulting to SRC_ALPHA!", argv[3]);
        // USe Default.
        srcBlendFactor = GL_SRC_ALPHA;
    }

    // Check Destination Factor.
    if ( dstBlendFactor == GL_INVALID_BLEND_FACTOR )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::setBlending() - Invalid dstBlendFactor '%s', defaulting to ONE_MINUS_SRC_ALPHA!", argv[4]);
        // USe Default.
        dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
    }

    // Set Blending.
    object->setBlending( dAtob(argv[2]), srcBlendFactor, dstBlendFactor );
}
// Set Blending.
void t2dParticleEmitter::setBlending( bool status, S32 srcBlendFactor, S32 dstBlendFactor )
{
    // Set Blending Flag.
    mBlending = status;

    // Set Blending Factors.
    mSrcBlendFactor = srcBlendFactor;
    mDstBlendFactor = dstBlendFactor;
}


//-----------------------------------------------------------------------------
// Get Blending.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getBlending, const char*, 2, 2, "Gets the Rendering Blend Options.")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(128);
    // Format Buffer.
    dSprintf(pBuffer, 128, "%d %s %s", S32(object->getBlendingStatus()), getSrcBlendFactorDescription((GLenum)object->getSrcBlendFactor()), getDstBlendFactorDescription((GLenum)object->getDstBlendFactor()) );
    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------
// Get Emitter Visibility.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getVisible, bool, 2, 2, "Get the Emitters Visibility.")
{
    // Get Emitter Visibility.
    return object->getVisible();
}
// Get Emitter Visibility.
bool t2dParticleEmitter::getVisible( void ) const
{
    // Get Emitter Visibility.
    return mVisible;
}


//-----------------------------------------------------------------------------
// Get Emitter Name.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getEmitterName, const char*, 2, 2, "Get the Emitters Name.")
{
    // Get Emitter Name.
    return object->getEmitterName();
}
// Get Emitter Name.
const char* t2dParticleEmitter::getEmitterName(void) const
{
    // Return Emitter Name.
    return mEmitterName;
}


//-----------------------------------------------------------------------------
// Get Fixed Aspect.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getFixedAspect, bool, 2, 2, "Get Fixed-Aspect.")
{
    // Get Fixed Aspect.
    return object->getFixedAspect();
}
// Get Fixed Aspect.
bool t2dParticleEmitter::getFixedAspect( void ) const
{
    // Get Fixed Aspect.
    return mFixedAspect;
}



//-----------------------------------------------------------------------------
// Get Fixed-Force Angle.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getFixedForceAngle, F32, 2, 2, "Get Fixed-Force-Direction.")
{
    // Get Fixed-Force Angle.
    return object->getFixedForceAngle();
}
// Get Fixed-Force Angle.
F32 t2dParticleEmitter::getFixedForceAngle( void ) const
{
    // Return Fixed-Angle Direction.
    return mFixedForceAngle;
}


//-----------------------------------------------------------------------------
// Get Particle Orientation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getParticleOrientation, const char*, 2, 2, "Get Particle Orientation.")
{
    // Get Particle Orientation.
    return object->getParticleOrientation();
}
ConsoleMethod(t2dParticleEmitter, getParticleOrientationMode, const char*, 2, 2, "Get Particle Orientation.")
{
    // Get Particle Orientation.
    return object->getParticleOrientation();
}
// Get Particle Orientation.
const char* t2dParticleEmitter::getParticleOrientation( void ) const
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(particleOrientationLookup) / sizeof(particleOrientationLookup[0])); i++)
    {
        if( (t2dParticleEmitter::tEmitterOrientationMode)particleOrientationLookup[i].index == mParticleOrientationMode )
        {
            // Return Particle Orientation Description.
            return particleOrientationLookup[i].label;
        }
    }

    // Bah!
    return NULL;
}



//-----------------------------------------------------------------------------
// Get Align Angle Offset.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getAlignAngleOffset, F32, 2, 2, "Get Align-Orientation Angle-Offset.")
{
    // Get Align Angle Offset.
    return object->getAlignAngleOffset();
}
// Get Align Angle Offset.
F32 t2dParticleEmitter::getAlignAngleOffset( void ) const
{
    // Get Align Angle Offset.
    return mAlign_AngleOffset;
}


//-----------------------------------------------------------------------------
// Get Align Keep Aligned.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getAlignKeepAligned, F32, 2, 2, "Get Align-Orientation Keep-Aligned Flag.")
{
    // Get Align Keep Aligned.
    return object->getAlignKeepAligned();
}
// Get Align Keep Aligned.
bool t2dParticleEmitter::getAlignKeepAligned( void ) const
{
    // Get Align Keep Aligned.
    return mAlign_KeepAligned;
}


//-----------------------------------------------------------------------------
// Get Random Angle Offset.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getRandomAngleOffset, F32, 2, 2, "Get Random-Orientation Angle-Offset.")
{
    // Get Random Angle Offset.
    return object->getRandomAngleOffset();
}
// Get Random Angle Offset.
F32 t2dParticleEmitter::getRandomAngleOffset( void ) const
{
    // Get Random Angle Offset.
    return mRandom_AngleOffset;
}


//-----------------------------------------------------------------------------
// Get Random Arc.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getRandomArc, F32, 2, 2, "Get Random-Orientation Arc.")
{
    // Get Random Arc.
    return object->getRandomArc();
}
// Get Random Arc.
F32 t2dParticleEmitter::getRandomArc( void ) const
{
    // Get Random Arc.
    return mRandom_Arc;
}


//-----------------------------------------------------------------------------
// Get Fixed Angle Offset.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getFixedAngleOffset, F32, 2, 2, "Get Fixed-Orientation Angle-Offset.")
{
    // Get Fixed Angle Offset.
    return object->getFixedAngleOffset();
}
// Get Fixed Angle Offset.
F32 t2dParticleEmitter::getFixedAngleOffset( void ) const
{
    // Get Fixed Angle Offset.
    return mFixed_AngleOffset;
}


//-----------------------------------------------------------------------------
// Get Emitter Type.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getEmitterType, const char*, 2, 2, "Get Emitter Type.")
{
    // Get Emitter Type.
    return object->getEmitterType();
}
// Get Emitter Type.
const char* t2dParticleEmitter::getEmitterType( void ) const
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(emitterTypeLookup) / sizeof(emitterTypeLookup[0])); i++)
    {
        if( (t2dParticleEmitter::tEmitterType)emitterTypeLookup[i].index == mEmitterType )
        {
            // Return Emitter Type Description.
            return emitterTypeLookup[i].label;
        }
    }

    // Bah!
    return NULL;
}


//-----------------------------------------------------------------------------
// Get ImageMap Name/Frame.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getImageMapNameFrame, const char*, 2, 2, "Get ImageMap Name and Frame.")
{
    // Get ImageMap Name/Frame.
    return object->getImageMapNameFrame();
}
// Get ImageMap Name/Frame.
const char* t2dParticleEmitter::getImageMapNameFrame( void ) const
{
    // Nothing to return if an animation is selected!
    if ( mAnimationSelected )
        return NULL;

    // Get Console Buffer.
    char* pConBuffer = Con::getReturnBuffer(256);

    // Write ImageMap Name/Frame String.
    dSprintf( pConBuffer, 256, "%s %d", mImageMapName, mImageMapFrame );

    // Return Buffer.
    return pConBuffer;
}


//-----------------------------------------------------------------------------
// Get Animation Name.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getAnimationName, const char*, 2, 2, "Get Animation Name.")
{
    // Get Animation Name.
    return object->getAnimationName();
}
// Get Animation Name.
const char* t2dParticleEmitter::getAnimationName( void ) const
{
    // Nothing to return if an animation is NOT selected!
    if ( !mAnimationSelected )
        return NULL;

    // Return Animation Name.
    return mAnimationName;
}


//-----------------------------------------------------------------------------
// Get Pivot-Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getPivotPoint, const char*, 2, 2, "Get Pivot-Point.")
{
    // Get Pivot-Point.
    return object->getPivotPoint();
}
// Get Pivot-Point.
const char* t2dParticleEmitter::getPivotPoint( void ) const
{
    // Get Console Buffer.
    char* pConBuffer = Con::getReturnBuffer(32);

    // Write ImageMap Name/Frame String.
    dSprintf( pConBuffer, 32, "%f %f", mPivotPoint.mX, mPivotPoint.mY );

    // Return Buffer.
    return pConBuffer;
}


//-----------------------------------------------------------------------------
// Get Use Effect Emission.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getUseEffectEmission, bool, 2, 2, "Get Use-Effect-Emission Flag.")
{
    // Get Use Effect Emission.
    return object->getUseEffectEmission();
}
// Get Use Effect Emission.
bool t2dParticleEmitter::getUseEffectEmission( void ) const
{
    // Get Use Effect Emission.
    return mUseEffectEmission;
}


//-----------------------------------------------------------------------------
// Get Link Emission Rotation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getLinkEmissionRotation, bool, 2, 2, "Get Link-Emission-Rotation Flag.")
{
    // Get Link Emission Rotation.
    return object->getLinkEmissionRotation();
}
// Get Link Emission Rotation.
bool t2dParticleEmitter::getLinkEmissionRotation( void ) const
{
    // Get Link Emission Rotation.
    return mLinkEmissionRotation;
}


//-----------------------------------------------------------------------------
// Get Intense Particles.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getIntenseParticles, bool, 2, 2, "Get Intense-Particles Flag.")
{
    // Get Intense Particles.
    return object->getIntenseParticles();
}
// Get Intense Particles.
bool t2dParticleEmitter::getIntenseParticles( void ) const
{
    // Get Intense Particles.
    return mIntenseParticles;
}


//-----------------------------------------------------------------------------
// Get Single Particle.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getSingleParticle, bool, 2, 2, "Get Single-Particle Flag.")
{
    // Get Single Particle.
    return object->getSingleParticle();
}
// Get Single Particle.
bool t2dParticleEmitter::getSingleParticle( void ) const
{
    // Get Single Particle.
    return mSingleParticle;
}


//-----------------------------------------------------------------------------
// Get Attach Position To Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getAttachPositionToEmitter, bool, 2, 2, "Get Attach-Position-To-Emitter Flag.")
{
    // Get Attach Position To Emitter.
    return object->getAttachPositionToEmitter();
}
// Get Attach Position To Emitter.
bool t2dParticleEmitter::getAttachPositionToEmitter( void ) const
{
    // Get Attach Position To Emitter.
    return mAttachPositionToEmitter;
}


//-----------------------------------------------------------------------------
// Get Attach Rotation To Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getAttachRotationToEmitter, bool, 2, 2, "Get Attach-Rotation-To-Emitter Flag.")
{
    // Get Attach Rotation To Emitter.
    return object->getAttachRotationToEmitter();
}
// Get Attach Rotation To Emitter.
bool t2dParticleEmitter::getAttachRotationToEmitter( void ) const
{
    // Get Attach Rotation To Emitter.
    return mAttachRotationToEmitter;
}


//-----------------------------------------------------------------------------
// Get First In Front Order.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getFirstInFrontOrder, bool, 2, 2, "Get First-In-Front-Order Flag.")
{
    // Get First In Front Order.
    return object->getFirstInFrontOrder();
}
// Get First In Front Order.
bool t2dParticleEmitter::getFirstInFrontOrder( void ) const
{
    // Get First In Front Order.
    return mFirstInFrontOrder;
}


//-----------------------------------------------------------------------------
// Get 'Using Animation' Flag.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getUsingAnimation, bool, 2, 2, "Get Using Animation Flag.")
{
    // Get 'Using Animation' Flag.
    return object->getUsingAnimation();
}
// Get 'Using Animation' Flag.
bool t2dParticleEmitter::getUsingAnimation( void ) const
{
    // Get 'Using Animation' Flag.
    return mAnimationSelected;
}


//-----------------------------------------------------------------------------
// Play Emitter.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::playEmitter( bool resetParticles )
{
    // Reset Time Since Last Generation.
    mTimeSinceLastGeneration = 0.0f;

    // Kill all particles if selected.
    if ( resetParticles )
        // Free All Particles.
        freeAllParticles();

    // Reset Pause Emitter.
    mPauseEmitter = false;
}


//-----------------------------------------------------------------------------
// Stop Emitter.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::stopEmitter( void )
{
    // Free All Particles.
    freeAllParticles();
}


//-----------------------------------------------------------------------------
// Pause Emitter.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::pauseEmitter( void )
{
    // Pause Emitter.
    mPauseEmitter = true;

    // Are we in single particle mode?
    if ( mSingleParticle )
    {
        // Yes, so remove all particles now!
        freeAllParticles();
    }
}


//-----------------------------------------------------------------------------
// Create Particle.
//-----------------------------------------------------------------------------
t2dParticleEmitter::tParticleNode* t2dParticleEmitter::createParticle( void )
{
    // Have we got any free particle nodes?
    if ( mpFreeParticleNodes == NULL )
        // No, so create a new block.
        createParticlePoolBlock();

    // Fetch Free Node.
    tParticleNode* pFreeParticleNode = mpFreeParticleNodes;

    // Reposition Free Node Reference.
    mpFreeParticleNodes = mpFreeParticleNodes->mNextNode;

    // Insert Particle into Head Chain.
    pFreeParticleNode->mNextNode        = mParticleNodeHead.mNextNode;
    pFreeParticleNode->mPreviousNode    = &mParticleNodeHead;
    mParticleNodeHead.mNextNode         = pFreeParticleNode;
    pFreeParticleNode->mNextNode->mPreviousNode = pFreeParticleNode;

    // Increase Active Particle Count.
    mActiveParticles++;

    // Configure Particle.
    configureParticle( pFreeParticleNode );

    // Return New Particle.
    return pFreeParticleNode;
}


//-----------------------------------------------------------------------------
// Free Particle.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::freeParticle( tParticleNode* pParticleNode )
{
    // Remove Particle from Head Chain.
    pParticleNode->mPreviousNode->mNextNode = pParticleNode->mNextNode;
    pParticleNode->mNextNode->mPreviousNode = pParticleNode->mPreviousNode;

    // Reset Extraneous Data.
    pParticleNode->mPreviousNode = NULL;
    
    // Insert into Free Pool.
    pParticleNode->mNextNode = mpFreeParticleNodes;
    mpFreeParticleNodes = pParticleNode;

    // Decrease Active Particle Count.
    mActiveParticles--;
}



//-----------------------------------------------------------------------------
// Clear Particles.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::freeAllParticles( void )
{
    // Free All Allocated Particles.
    while( mParticleNodeHead.mNextNode != &mParticleNodeHead )
        freeParticle( mParticleNodeHead.mNextNode );
}


//-----------------------------------------------------------------------------
// Create Particle Pool Block.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::createParticlePoolBlock( void )
{
    // Generate Free Pool Block.
    tParticleNode* pFreePoolBlock = new tParticleNode[mParticlePoolBlockSize];

    // Generate/Add New Pool Block.
    mParticlePool.push_back( pFreePoolBlock );

    // Initialise Free Pool Block.
    for ( U32 n = 0; n < (mParticlePoolBlockSize-1); n++ )
    {
        pFreePoolBlock[n].mPreviousNode = NULL;
        pFreePoolBlock[n].mNextNode     = pFreePoolBlock+n+1;
    }

    // Insert Last Node Preceeding any existing free nodes.
    pFreePoolBlock[mParticlePoolBlockSize-1].mPreviousNode = NULL;
    pFreePoolBlock[mParticlePoolBlockSize-1].mNextNode = mpFreeParticleNodes;

    // Set Free References.
    mpFreeParticleNodes = pFreePoolBlock;
}


//-----------------------------------------------------------------------------
// Destroy Particle Pool.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::destroyParticlePool( void )
{
    // Free All Particles.
    freeAllParticles();

    // Destroy All Blocks.
    for ( U32 n = 0; n < mParticlePool.size(); n++ )
        delete [] mParticlePool[n];

    // Reset Free Particles.
    mpFreeParticleNodes = NULL;
}


//-----------------------------------------------------------------------------
// Configure Particle.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::configureParticle( tParticleNode* pParticleNode )
{
    // UNUSED: JOSEPH THOMAS -> F32 baseValue;
    // UNUSED: JOSEPH THOMAS -> F32 varValue;
    // UNUSED: JOSEPH THOMAS -> F32 lifeValue;
    // UNUSED: JOSEPH THOMAS -> F32 effectValue;
    // UNUSED: JOSEPH THOMAS -> F32 value;

    // Fetch Effect Age.
    const F32 effectAge = pParentEffectObject->mEffectAge;

    // Fetch Effect Position.
    const t2dVector effectPos = pParentEffectObject->getPosition();


    // Default to not suppressing movement.
    pParticleNode->mSuppressMovement = false;


    // **********************************************************************************************************************
    // Calculate Particle Position.
    // **********************************************************************************************************************

    // Are we using Single Particle?
    if ( mSingleParticle )
    {
        // Yes, so if use Effect Position ( or origin if attached to emitter).
        if ( mAttachPositionToEmitter )
            pParticleNode->mPosition.set(0.0f, 0.0f);
        else
            pParticleNode->mPosition = effectPos;
    }
    else
    {
        // No, so select Emitter-Type.
        switch( mEmitterType )
        {
            // Use Pivot-Point.
            case POINT:
            {
                // Yes, so if use Effect Position ( or origin if attached to emitter).
                if ( mAttachPositionToEmitter )
                    pParticleNode->mPosition.set(0.0f, 0.0f);
                else
                    pParticleNode->mPosition = effectPos;

            } break;

            // Use X-Size and Pivot-Y.
            case LINEX:
            {
                // Fetch Local Clip Boundary.
                const t2dVector* pLocalClipBoundary = pParentEffectObject->getLocalClipBoundary();

                // Choose Random Position along line within Boundary with @ Pivot-Y.
                F32 minX = pLocalClipBoundary[0].mX;
                F32 maxX = pLocalClipBoundary[1].mX;
                F32 midY = (pLocalClipBoundary[0].mY + pLocalClipBoundary[3].mY) * 0.5f;

                t2dVector tempPos;

                // Normalise.
                if ( minX > maxX )
                    mSwap( minX, maxX );

                // Potential Vertical Line.
                if ( minX == maxX )
                    tempPos.set( minX, midY );
                else
                    // Normalised.
                    tempPos.set( mGetT2DRandomF( minX, maxX ), midY );

                // Rotate into Emitter-Space.
                //
                // NOTE:-   If we're attached to the emitter, then we keep the particle coordinates
                //          in local-space and use the hardware to render them in emitter-space.
                t2dSceneObject::transformPoint2D( (mAttachPositionToEmitter && mAttachRotationToEmitter) ? t2dMatrix::getIdentity() : pParentEffectObject->getRotationMatrix(), tempPos, mAttachPositionToEmitter ? t2dVector::getZero() : effectPos, pParticleNode->mPosition );

            } break;

            // Use Y-Size and Pivot-X.
            case LINEY:
            {
                // Fetch Local Clip Boundary.
                const t2dVector* pLocalClipBoundary = pParentEffectObject->getLocalClipBoundary();

                // Choose Random Position along line within Boundary with @ Pivot-Y.
                F32 midX = (pLocalClipBoundary[0].mX + pLocalClipBoundary[1].mX) * 0.5f;
                F32 minY = pLocalClipBoundary[0].mY;
                F32 maxY = pLocalClipBoundary[3].mY;

                t2dVector tempPos;

                // Normalise.
                if ( minY > maxY )
                    mSwap( minY, maxY );

                // Potential Horizontal Line.
                if ( minY == maxY )
                    tempPos.set( midX, minY );
                else
                    // Normalised.
                    tempPos.set( midX, mGetT2DRandomF( minY, maxY ) );

                // Rotate into Emitter-Space.
                //
                // NOTE:-   If we're attached to the emitter, then we keep the particle coordinates
                //          in local-space and use the hardware to render them in emitter-space.
                t2dSceneObject::transformPoint2D( (mAttachPositionToEmitter && mAttachRotationToEmitter) ? t2dMatrix::getIdentity() : pParentEffectObject->getRotationMatrix(), tempPos, mAttachPositionToEmitter ? t2dVector::getZero() : effectPos, pParticleNode->mPosition );

            } break;

            // Use X/Y Sizes.
            case AREA:
            {
                // Fetch Local Clip Boundary.
                const t2dVector* pLocalClipBoundary = pParentEffectObject->getLocalClipBoundary();

                //
                F32 minX = pLocalClipBoundary[0].mX;
                F32 maxX = pLocalClipBoundary[1].mX;
                F32 minY = pLocalClipBoundary[0].mY;
                F32 maxY = pLocalClipBoundary[3].mY;

                t2dVector tempPos;

                // Normalise.
                if ( minX > maxX )
                    mSwap( minX, maxX );
                // Normalise.
                if ( minY > maxY )
                    mSwap( minY, maxY );

                // Normalised.
                tempPos.set( (minX == maxX) ? minX : mGetT2DRandomF( minX, maxX ), (minY == maxY) ? minY : mGetT2DRandomF( minY, maxY ) );

                // Rotate into Emitter-Space.
                //
                // NOTE:-   If we're attached to the emitter, then we keep the particle coordinates
                //          in local-space and use the hardware to render them in emitter-space.
                t2dSceneObject::transformPoint2D( (mAttachPositionToEmitter && mAttachRotationToEmitter) ? t2dMatrix::getIdentity() : pParentEffectObject->getRotationMatrix(), tempPos, mAttachPositionToEmitter ? t2dVector::getZero() : effectPos, pParticleNode->mPosition );

            } break;
        }
    }


    // **********************************************************************************************************************
    // Calculate Particle Lifetime.
    // **********************************************************************************************************************
    pParticleNode->mParticleAge = 0.0f;
    pParticleNode->mParticleLifetime = t2dGraphField::calcGraphBVE( mParticleLife.t2dGraphField_Base,
                                                                        mParticleLife.t2dGraphField_Variation,
                                                                        pParentEffectObject->mParticleLife.t2dGraphField_Base,
                                                                        effectAge );


    // **********************************************************************************************************************
    // Calculate Particle Size-X.
    // **********************************************************************************************************************
    pParticleNode->mSize.mX = t2dGraphField::calcGraphBVE(  mSizeX.t2dGraphField_Base,
                                                            mSizeX.t2dGraphField_Variation,
                                                            pParentEffectObject->mSizeX.t2dGraphField_Base,
                                                            effectAge );

    // Is the Particle Aspect-Locked?
    if ( mFixedAspect )
    {
        // Yes, so simply copy Size-X.
        pParticleNode->mSize.mY = pParticleNode->mSize.mX;
    }
    else
    {
        // No, so calculate Particle Size-Y.
        pParticleNode->mSize.mY = t2dGraphField::calcGraphBVE(  mSizeY.t2dGraphField_Base,
                                                                mSizeY.t2dGraphField_Variation,
                                                                pParentEffectObject->mSizeY.t2dGraphField_Base,
                                                                effectAge );
    }

    // Reset Render Size.
    pParticleNode->mRenderSize.set(-1,-1);


    // **********************************************************************************************************************
    // Calculate Speed.
    // **********************************************************************************************************************

    // Ignore if we're using Single-Particle.
    if ( !mSingleParticle )
    {
        pParticleNode->mSpeed = t2dGraphField::calcGraphBVE(    mSpeed.t2dGraphField_Base,
                                                                mSpeed.t2dGraphField_Variation,
                                                                pParentEffectObject->mSpeed.t2dGraphField_Base,
                                                                effectAge );
    }


    // **********************************************************************************************************************
    // Calculate Spin.
    // **********************************************************************************************************************
    pParticleNode->mSpin = t2dGraphField::calcGraphBVE( mSpin.t2dGraphField_Base,
                                                            mSpin.t2dGraphField_Variation,
                                                            pParentEffectObject->mSpin.t2dGraphField_Base,
                                                            effectAge );


    // **********************************************************************************************************************
    // Calculate Fixed-Force.
    // **********************************************************************************************************************
    pParticleNode->mFixedForce = t2dGraphField::calcGraphBVE(   mFixedForce.t2dGraphField_Base,
                                                                mFixedForce.t2dGraphField_Variation,
                                                                pParentEffectObject->mFixedForce.t2dGraphField_Base,
                                                                effectAge );


    // **********************************************************************************************************************
    // Calculate Random Motion..
    // **********************************************************************************************************************

    // Ignore if we're using Single-Particle.
    if ( !mSingleParticle )
    {
        pParticleNode->mRandomMotion = t2dGraphField::calcGraphBVE( mRandomMotion.t2dGraphField_Base,
                                                                        mRandomMotion.t2dGraphField_Variation,
                                                                        pParentEffectObject->mRandomMotion.t2dGraphField_Base,
                                                                        effectAge );
    }


    // **********************************************************************************************************************
    // Calculate Emission Angle.
    // **********************************************************************************************************************

    // Note:- We reset the emission angle/arc in-case we're using Single-Particle mode as this is the default.
    F32 emissionForce = 0;
    F32 emissionAngle = 0;
    F32 emissionArc = 0;

    // Ignore if we're using Single-Particle.
    if ( !mSingleParticle )
    {
        // Are we using the Effects Emission?
        if ( mUseEffectEmission )
        {
            // Yes, so calculate emission from effect...

            // Calculate Emission Force.
            emissionForce = t2dGraphField::calcGraphBV( pParentEffectObject->mEmissionForce.t2dGraphField_Base,
                                                            pParentEffectObject->mEmissionForce.t2dGraphField_Variation,
                                                            effectAge);

            // Calculate Emission Angle.
            emissionAngle = t2dGraphField::calcGraphBV( pParentEffectObject->mEmissionAngle.t2dGraphField_Base,
                                                            pParentEffectObject->mEmissionAngle.t2dGraphField_Variation,
                                                            effectAge);

            // Calculate Emission Arc.
            // NOTE:-   We're actually interested in half the emission arc!
            emissionArc = t2dGraphField::calcGraphBV(   pParentEffectObject->mEmissionArc.t2dGraphField_Base,
                                                        pParentEffectObject->mEmissionArc.t2dGraphField_Variation,
                                                        effectAge ) * 0.5f;
        }
        else
        {
            // No, so calculate emission from emitter...

            // Calculate Emission Force.
            emissionForce = t2dGraphField::calcGraphBV( mEmissionForce.t2dGraphField_Base,
                                                            mEmissionForce.t2dGraphField_Variation,
                                                            effectAge);

            // Calculate Emission Angle.
            emissionAngle = t2dGraphField::calcGraphBV( mEmissionAngle.t2dGraphField_Base,
                                                        mEmissionAngle.t2dGraphField_Variation,
                                                        effectAge );

            // Calculate Emission Arc.
            // NOTE:-   We're actually interested in half the emission arc!
            emissionArc = t2dGraphField::calcGraphBV(   mEmissionArc.t2dGraphField_Base,
                                                        mEmissionArc.t2dGraphField_Variation,
                                                        effectAge ) * 0.5f;
        }

        // Is the Emission Rotation linked?
        if ( mLinkEmissionRotation )
            // Yes, so add Effect Object-Rotation.
            emissionAngle += pParentEffectObject->getRotation();

        // Calculate Final Emission Angle by choosing random Arc.
        emissionAngle = mFmod( mGetT2DRandomF( emissionAngle-emissionArc, emissionAngle+emissionArc ), 360.0f ) ;

        // Calculate Normalised Velocity.
        pParticleNode->mVelocity.set( emissionForce * mSin( mDegToRad(emissionAngle) ), emissionForce * -mCos( mDegToRad(emissionAngle) ) );
    }

    // **********************************************************************************************************************
    // Calculate Orientation Angle.
    // **********************************************************************************************************************

    // Handle Particle Orientation Mode.
    switch( mParticleOrientationMode )
    {
        // Aligned to Initial Emission.
        case ALIGNED:
        {
            // Just use the emission angle plus offset.
            pParticleNode->mOrientationAngle = mFmod( emissionAngle - mAlign_AngleOffset, 360.0f );

        } break;

        // Fixed Orientation.
        case FIXED:
        {
            // Use Fixed Angle.
            pParticleNode->mOrientationAngle = mFmod( mFixed_AngleOffset, 360.0f );

        } break;

        // Random with Constraints.
        case RANDOM:
        {
            // Used Random Angle/Arc.
            F32 randomArc = mRandom_Arc * 0.5f;
            pParticleNode->mOrientationAngle = mFmod( mGetT2DRandomF( mRandom_AngleOffset - randomArc, mRandom_AngleOffset + randomArc ), 360.0f );

        } break;

    }

    // **********************************************************************************************************************
    // Calculate RGBA Components.
    // **********************************************************************************************************************

    pParticleNode->mColour.set( mClampF( mColourRed.t2dGraphField_OverLife.getGraphValue( 0.0f ), mColourRed.t2dGraphField_OverLife.getMinValue(), mColourRed.t2dGraphField_OverLife.getMaxValue() ),
                                mClampF( mColourGreen.t2dGraphField_OverLife.getGraphValue( 0.0f ), mColourGreen.t2dGraphField_OverLife.getMinValue(), mColourGreen.t2dGraphField_OverLife.getMaxValue() ),
                                mClampF( mColourBlue.t2dGraphField_OverLife.getGraphValue( 0.0f ), mColourBlue.t2dGraphField_OverLife.getMinValue(), mColourBlue.t2dGraphField_OverLife.getMaxValue() ),
                                mClampF( mVisibility.t2dGraphField_OverLife.getGraphValue( 0.0f ) * pParentEffectObject->mVisibility.t2dGraphField_Base.getGraphValue( 0.0f ), mVisibility.t2dGraphField_OverLife.getMinValue(), mVisibility.t2dGraphField_OverLife.getMaxValue() ) );


    // **********************************************************************************************************************
    // Animation Controller.
    // **********************************************************************************************************************

    // If an animation is selected, start it playing.
    if ( mAnimationSelected )
        pParticleNode->mAnimationController.playAnimation( mAnimationName, false );


    // Reset Last Render Size.
    pParticleNode->mLastRenderSize.set(-1, -1);


    // **********************************************************************************************************************
    // Reset Tick Position.
    // **********************************************************************************************************************
    pParticleNode->mPreTickPosition = pParticleNode->mPostTickPosition = pParticleNode->mRenderTickPosition = pParticleNode->mPosition;


    // **********************************************************************************************************************
    // Do a Single Particle Integration to get things going.
    // **********************************************************************************************************************
    integrateParticle( pParticleNode, 0.0f, 0.0f );
}


//-----------------------------------------------------------------------------
// Load Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, loadEmitter, bool, 3, 3, "(emitterFile$) - Loads a Particle Emitter.")
{
    // Load Emitter.
    return object->loadEmitter( argv[2] );
}
// Load Emitter.
bool t2dParticleEmitter::loadEmitter( const char* emitterFile )
{
    // Check we're part of a parent-effect.
    // NOTE:-   There's no need to suppose we're not as this should not happen but
    //          if it does then let's fail gracefully.
    if ( !pParentEffectObject )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::loadEmitter() - The emitter is not part of a Scene!");
        // Return Error.
        return false;
    }

    // Expand relative paths.
    char buffer[1024];
    if ( emitterFile )
        if ( Con::expandScriptFilename( buffer, sizeof( buffer ), emitterFile ) )
            emitterFile = buffer;

    // Open Emitter File.
    Stream* pStream = ResourceManager->openStream( emitterFile );
    // Check Stream.
    if ( !pStream )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::loadEmitter() - Could not Open File '%s' for Emitter Load.", emitterFile);
        // Return Error.
        return false;
    }

    // Scene Objects.
    Vector<t2dSceneObject*> ObjReferenceList;

    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( ObjReferenceList );

    // Load Stream.
    if ( !loadStream( *pStream, pParentEffectObject->getSceneGraph(), ObjReferenceList, true  ) )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::loadEmitter() - Error Loading Emitter!");
        // Return Error.
        return false;
    }

    // Close Stream.
    ResourceManager->closeStream( pStream );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, saveEmitter, bool, 3, 3, "(emitterFile$) - Save a Particle Emitter.")
{
    // Save Emitter.
    return object->saveEmitter( argv[2] );
}
// Save Emitter.
bool t2dParticleEmitter::saveEmitter( const char* emitterFile )
{
    // Check we're part of a parent-effect.
    // NOTE:-   There's no need to suppose we're not as this should not happen but
    //          if it does then let's fail gracefully.
    if ( !pParentEffectObject )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::saveEmitter() - The emitter is not part of a Scene!");
        // Return Error.
        return false;
    }

    // Expand relative paths.
    char buffer[1024];
    if ( emitterFile )
        if ( Con::expandToolScriptFilename( buffer, sizeof( buffer ), emitterFile ) )
            emitterFile = buffer;

    // Open Emitter File.
    FileStream fileStream;
    if ( !ResourceManager->openFileForWrite( fileStream, emitterFile, FileStream::Write ) )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::saveEmitter() - Could not open File '%s' for Emitter Save.", emitterFile);
        // Return Error.
        return false;
    }

    // Stop Parent Effect.
    // NOTE:-   We do this so that we don't save our active particles.
    pParentEffectObject->stopEffect(false, false);

    // Save Stream.
    if ( !saveStream( fileStream, pParentEffectObject->getSceneGraph()->getNextSerialiseID(), 1 ) )
    {
        // Warn.
        Con::warnf("t2dParticleEmitter::saveEmitter() - Error Saving Emitter!");
        // Return Error.
        return false;
    }

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Set Emitter Collision Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, setEmitterCollisionStatus, void, 3, 3, "Set the emitter collision status.")
{
    // Set Emitter Collision Status.
    object->setEmitterCollisionStatus( dAtob(argv[2]) );
}
// Set Emitter Collision Status.
void t2dParticleEmitter::setEmitterCollisionStatus( const bool status )
{
    // Set Emitter Collision Status.
    mUseEmitterCollisions = status;
}


//-----------------------------------------------------------------------------
// Get Emitter Collision Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEmitter, getEffectCollisionStatus, bool, 2, 2, "Get the emitter collision status.")
{
    // Get Emitter Collision Status.
    return object->getEmitterCollisionStatus();
}
// Get Emitter Collision Status.
bool t2dParticleEmitter::getEmitterCollisionStatus( void )
{
    // Get Emitter Collision Status.
    return mUseEmitterCollisions;
}


//-----------------------------------------------------------------------------
// Check Particle Collisions.
//-----------------------------------------------------------------------------
bool t2dParticleEmitter::checkParticleCollisions( const t2dParticleEffect* pParentEffect, const F32 elapsedTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats )
{
    // Reset Initial Collision Status.
    bool collisionStatus = false;

    // Fetch SceneGraph.
    t2dSceneGraph* pSceneGraph = pParentEffect->getSceneGraph();

    // Get Parent Collision Masks.
    const U32 groupMask = pParentEffect->getCollisionGroupMask();
    const U32 layerMask = pParentEffect->getCollisionLayerMask();

    // Get Parent Collision Response.
    const t2dPhysics::eCollisionResponse collisionResponse = pParentEffect->getCollisionResponseMode();

    // Get Parent Restitution.
    const F32 restitution = pParentEffect->getRestitution();

    // Fetch First Particle Node.
    tParticleNode* pParticleNode = mParticleNodeHead.mNextNode;

    // Next Particle Node.
    tParticleNode* pNextParticleNode;

    // Process All particle nodes.
    while ( pParticleNode != &mParticleNodeHead )
    {
        t2dVector newPosition;

        // Default to not suppressing movement.
        pParticleNode->mSuppressMovement = false;

        // Fetch Start Position.
        t2dVector& startPosition = pParticleNode->mPosition;
        // Calculate Projected Position.
        const t2dVector endPosition = pParticleNode->mPosition + (pParticleNode->mVelocity * pParticleNode->mRenderSpeed * elapsedTime);

        // Pick Objects along our velocity path.
        const U32 pickedObjects = pSceneGraph->pickLine( startPosition, endPosition, groupMask, layerMask, false, pParentEffect );

        // Reference next Particle Node.
        // NOTE:-   We do this here because we may destroy the particle if the collision-response is in "KILL" mode.
        pNextParticleNode = pParticleNode->mNextNode;

        // Did we collide?
        if ( pickedObjects > 0 )
        {
            // Flag Collision Occurred.
            collisionStatus = true;

            // Fetch Colliding Object.
            typeSceneObjectVectorConstRef pickVector = pSceneGraph->getPickList();
            // Fetch Scene Object.
            t2dSceneObject* pSceneObject = pickVector[0];
            // Fetch Collisoin Time.
            const F32& collisionTime = pSceneObject->mSortKeyCollisionTime;

            // Lerp to collision position.
            startPosition.lerp( endPosition, collisionTime, startPosition );

            // Reference Velocity.
            t2dVector& linearVelocity = pParticleNode->mVelocity;
            // Reference Collision Normal.
            t2dVector& collisionNormal = pSceneObject->mSortKeyCollisionNormal;

            switch( collisionResponse )
            {
                // Bounce.
                case t2dPhysics::T2D_RESPONSE_BOUNCE:
                {
                    // Calculate Dot Velocity Normal.
                    const F32 dotVelocityNormal = collisionNormal * linearVelocity;
                    // Any velocity to clamp?
                    if ( mNotZero(dotVelocityNormal) )
                     {
                        // Set new Linear Velocity.
                        linearVelocity -= (collisionNormal*(2.0f*dotVelocityNormal) * restitution);
                    }

                } break;

                // Clamp.
                case t2dPhysics::T2D_RESPONSE_CLAMP:
                {
                    // Calculate Dot Velocity Normal.
                    const F32 dotVelocityNormal = collisionNormal * linearVelocity;
                    // Any velocity to clamp?
                    if ( mNotZero(dotVelocityNormal) )
                    {
                        // Set new Linear Velocity.
                        linearVelocity -= collisionNormal*dotVelocityNormal;
                    }

                } break;

                // Bounce.
                case t2dPhysics::T2D_RESPONSE_STICKY:
                {
                    // Set at rest.
                    linearVelocity.set(0.0f,0.0f);

                } break;

                // Kill.
                case t2dPhysics::T2D_RESPONSE_KILL:
                {
                    // Free Particle.
                    freeParticle( pParticleNode );

                } break;
            };

            // Suppress Movment.
            pParticleNode->mSuppressMovement = true;
        }

        // Move to next Particle Node.
        pParticleNode = pNextParticleNode;
    };   

    // Return Collision Status.
    return collisionStatus;
}


//-----------------------------------------------------------------------------
// Integrate Particle.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::integrateParticle( tParticleNode* pParticleNode, F32 particleAge, F32 elapsedTime )
{
    // **********************************************************************************************************************
    // Copy Old Tick Position.
    // **********************************************************************************************************************
    pParticleNode->mRenderTickPosition = pParticleNode->mPreTickPosition = pParticleNode->mPostTickPosition;


    // **********************************************************************************************************************
    // Scale Size.
    // **********************************************************************************************************************

    // Scale Size-X.
    pParticleNode->mRenderSize.mX = mClampF(    pParticleNode->mSize.mX * mSizeX.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                                mSizeX.t2dGraphField_Base.getMinValue(),
                                                mSizeX.t2dGraphField_Base.getMaxValue() );

    // Is the Particle Aspect-Locked?
    if ( mFixedAspect )
    {
        // Yes, so simply copy Size-X.
        pParticleNode->mRenderSize.mY = pParticleNode->mRenderSize.mX;
    }
    else
    {
        // No, so Scale Size-Y.
        pParticleNode->mRenderSize.mY = mClampF(    pParticleNode->mSize.mY * mSizeY.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                                    mSizeY.t2dGraphField_Base.getMinValue(),
                                                    mSizeY.t2dGraphField_Base.getMaxValue() );
    }


    // **********************************************************************************************************************
    // Scale Speed.
    // **********************************************************************************************************************
    pParticleNode->mRenderSpeed = mClampF(  pParticleNode->mSpeed * mSpeed.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                            mSpeed.t2dGraphField_Base.getMinValue(),
                                            mSpeed.t2dGraphField_Base.getMaxValue() );


    // **********************************************************************************************************************
    // Scale Spin (if Keep Aligned is not selected)
    // **********************************************************************************************************************
    if ( !(mParticleOrientationMode == ALIGNED && mAlign_KeepAligned) )
        pParticleNode->mRenderSpin = pParticleNode->mSpin * mSpin.t2dGraphField_OverLife.getGraphValue( particleAge );



    // **********************************************************************************************************************
    // Scale Fixed-Force.
    // **********************************************************************************************************************
    pParticleNode->mRenderFixedForce = mClampF( pParticleNode->mFixedForce * mFixedForce.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                                mFixedForce.t2dGraphField_Base.getMinValue(),
                                                mFixedForce.t2dGraphField_Base.getMaxValue() );


    // **********************************************************************************************************************
    // Scale Random-Motion.
    // **********************************************************************************************************************
    pParticleNode->mRenderRandomMotion = mClampF(   pParticleNode->mRandomMotion * mRandomMotion.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                                    mRandomMotion.t2dGraphField_Base.getMinValue(),
                                                    mRandomMotion.t2dGraphField_Base.getMaxValue() );


    // **********************************************************************************************************************
    // Scale Colour.
    // **********************************************************************************************************************

    // Red.
    pParticleNode->mColour.red = mClampF(   mColourRed.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                            mColourRed.t2dGraphField_OverLife.getMinValue(),
                                            mColourRed.t2dGraphField_OverLife.getMaxValue() );

    // Green.
    pParticleNode->mColour.green = mClampF( mColourGreen.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                            mColourGreen.t2dGraphField_OverLife.getMinValue(),
                                            mColourGreen.t2dGraphField_OverLife.getMaxValue() );

    // Blue.
    pParticleNode->mColour.blue = mClampF(  mColourBlue.t2dGraphField_OverLife.getGraphValue( particleAge ),
                                            mColourBlue.t2dGraphField_OverLife.getMinValue(),
                                            mColourBlue.t2dGraphField_OverLife.getMaxValue() );

    // Alpha.
    pParticleNode->mColour.alpha = mClampF( mVisibility.t2dGraphField_OverLife.getGraphValue( particleAge ) * pParentEffectObject->mVisibility.t2dGraphField_Base.getGraphValue( particleAge ),
                                            mVisibility.t2dGraphField_OverLife.getMinValue(),
                                            mVisibility.t2dGraphField_OverLife.getMaxValue() );




    // **********************************************************************************************************************
    // Integrate Particle.
    // **********************************************************************************************************************


    // Update Animation Controller (if used).
    if ( mAnimationSelected )
        pParticleNode->mAnimationController.updateAnimation( elapsedTime );


    // **********************************************************************************************************************
    // Calculate New Velocity...
    // **********************************************************************************************************************

    // Only Calculate Velocity if not a Single Particle.
    if ( !mSingleParticle )
    {
        // Calculate Random Motion (if we've got any).
        if ( mNotZero( pParticleNode->mRenderRandomMotion ) )
        {
            // Fetch Random Motion.
            F32 randomMotion = pParticleNode->mRenderRandomMotion * 0.5f;
            // Add Time-Integrated Random-Motion into Velocity.
            pParticleNode->mVelocity += elapsedTime * t2dVector( mGetT2DRandomF(-randomMotion, randomMotion), mGetT2DRandomF(-randomMotion, randomMotion) );
        }

        // Add Time-Integrated Fixed-Force into Velocity ( if we've got any ).
        if ( mNotZero( pParticleNode->mRenderFixedForce ) )
            pParticleNode->mVelocity += (mFixedForceDirection * pParticleNode->mRenderFixedForce * elapsedTime);

        // Suppress Movement?
        if ( !pParticleNode->mSuppressMovement )
        {
            // No, so adjust Particle Position.
            pParticleNode->mPosition += (pParticleNode->mVelocity * pParticleNode->mRenderSpeed * elapsedTime);
        }

    }


    // **********************************************************************************************************************
    // Are we Aligning to motion?
    // **********************************************************************************************************************
    if ( mParticleOrientationMode == ALIGNED && mAlign_KeepAligned )
    {
        // Yes, so calculate last movement direction.
        F32 movementAngle = mRadToDeg( mAtan( pParticleNode->mVelocity.mX, -pParticleNode->mVelocity.mY ) );
        // Adjust for Negative ArcTan-Quadrants.
        if ( movementAngle < 0.0f )
            movementAngle += 360.0f;

        // Set new Orientation Angle.
        pParticleNode->mOrientationAngle = -movementAngle - mAlign_AngleOffset;


        // **********************************************************************************************************************
        // Calculate Local Clip-Boundary.
        // **********************************************************************************************************************

        // Calculate Rotation Matrix.
        // NOTE:-   We tranpose the matrix here so that we get a matrix where zero-angle is 'up' and positive
        //          rotation is clockwise.
        pParticleNode->mRotationMatrix = t2dMatrix( pParticleNode->mOrientationAngle ).transpose() * pParticleNode->mRenderSize;
    }
    else
    {
        // Have we got some Spin?
        if ( mNotZero(pParticleNode->mRenderSpin) )
        {
            // Yes, so add into Orientation.
            pParticleNode->mOrientationAngle += pParticleNode->mRenderSpin * elapsedTime;
            // Keep within range.
            pParticleNode->mOrientationAngle = mFmod( pParticleNode->mOrientationAngle, 360.0f );
        }


        // If the size has changed or we have some Spin then we need to recalculate the Local Clip-Boundary.
        if ( mNotZero(pParticleNode->mRenderSpin) || pParticleNode->mRenderSize != pParticleNode->mLastRenderSize )
        {
            // **********************************************************************************************************************
            // Calculate Local Clip-Boundary.
            // **********************************************************************************************************************

            // Calculate Rotation Matrix.
            // NOTE:-   We tranpose the matrix here so that we get a matrix where zero-angle is 'up' and positive
            //          rotation is clockwise.
            pParticleNode->mRotationMatrix = t2dMatrix( pParticleNode->mOrientationAngle ).transpose() * pParticleNode->mRenderSize;
        }

        // We've dealt with a potential Size change so store current size for next time.
        pParticleNode->mLastRenderSize = pParticleNode->mRenderSize;
    }

    // Calculate Clip Boundary.
    t2dSceneObject::transformRectangle2D( pParticleNode->mRotationMatrix, mGlobalClipBoundary, pParticleNode->mPosition, pParticleNode->mLocalClipBoundary );


    // **********************************************************************************************************************
    // Set Post Tick Position.
    // **********************************************************************************************************************
    pParticleNode->mPostTickPosition = pParticleNode->mPosition;
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
    // **********************************************************************************************************************
    //
    // Integrate Particles.
    //
    // **********************************************************************************************************************

    // Fetch First Particle Node.
    tParticleNode* pParticleNode = mParticleNodeHead.mNextNode;

    // Process All particle nodes.
    while ( pParticleNode != &mParticleNodeHead )
    {
        // Update Particle Life.
        pParticleNode->mParticleAge += elapsedTime;

        // Has the particle expired?
        // NOTE:-   If we're in Single-Particle mode then the particle
        //          lives as long as the emitter does!
        if ( (!mSingleParticle && pParticleNode->mParticleAge > pParticleNode->mParticleLifetime) || (pParticleNode->mParticleLifetime == 0.0f) )
        {
            // Yes, so fetch next particle before we kill it.
            pParticleNode = pParticleNode->mNextNode;

            // Kill Particle.
            // NOTE:-   Because we move to the next particle,
            //          the particle to kill is now the previous!
            freeParticle( pParticleNode->mPreviousNode );
        }
        else
        {
            // Integrate Particle.
            integrateParticle( pParticleNode, pParticleNode->mParticleAge / pParticleNode->mParticleLifetime, elapsedTime );

            // **********************************************************************************************************************
            // Move to next Particle Node.
            // **********************************************************************************************************************
            pParticleNode = pParticleNode->mNextNode;
        }
    };


    // **********************************************************************************************************************
    //
    // Generate New Particles.
    //
    // **********************************************************************************************************************

    // Only Generate particles if we're not pause.
    if ( !mPauseEmitter )
    {
        // Are we in Single-Particle Mode?
        if ( mSingleParticle )
        {
            // Yes, so do we have a single particle yet?
            if ( mParticleNodeHead.mNextNode == &mParticleNodeHead )
            {
                // No, so generate Single Particle.
                createParticle();
            }
        }
        else
        {
            // No, so fetch Effect Age.
            F32 effectAge = pParentEffectObject->mEffectAge;

            // Accumulate Last Generation Time as we need to handle very small time-integrations correctly.
            //
            // NOTE:-   We need to do this if there's an emission target but the
            //          time-integration is so small that rounding results in
            //          no emission.  Downside to good FPS!
            mTimeSinceLastGeneration += elapsedTime;

            // Calculate Local Emission Quantity.
            const F32 baseEmission = mQuantity.t2dGraphField_Base.getGraphValue( effectAge );
            const F32 varEmission = mQuantity.t2dGraphField_Variation.getGraphValue( effectAge ) * 0.5f;
            const F32 effectEmission = pParentEffectObject->mQuantity.t2dGraphField_Base.getGraphValue( effectAge ) * mParticlePref;

            const F32 localEmission = mClampF(    (baseEmission + mGetT2DRandomF(-varEmission, varEmission)) * effectEmission,
                                                  mQuantity.t2dGraphField_Base.getMinValue(),
                                                  mQuantity.t2dGraphField_Base.getMaxValue() );
            const U32 emission = U32(mFloor(localEmission * mTimeSinceLastGeneration));

            // Do we have an emission?
            if ( emission > 0 )
            {
                // Yes, so remove this emission from accumulated time.
                mTimeSinceLastGeneration = getMax(0.0f, mTimeSinceLastGeneration - (emission / localEmission));

                // Suppress Precision Errors.
                if ( mIsZero( mTimeSinceLastGeneration ) )
                    mTimeSinceLastGeneration = 0.0f;

                // Generate Emission.
                for ( U32 n = 0; n < emission; n++ )
                    createParticle();
            }
            // No, so was there a calculated emission?
            else if ( localEmission == 0 )
            {
                // No, so reset accumulated time.
                //mTimeSinceLastGeneration = 0.0f;
            }
        }
    }

    // Update Debug Stats.
    pDebugStats->objectParticlesAvailable += mParticlePool.size() * mParticlePoolBlockSize;
    pDebugStats->objectParticlesActive += mActiveParticles;
}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::interpolateTick( const F32 timeDelta )
{
    // **********************************************************************************************************************
    //
    // Interpolate Particles.
    //
    // **********************************************************************************************************************

    // Fetch First Particle Node.
    tParticleNode* pParticleNode = mParticleNodeHead.mNextNode;

    // Process All particle nodes.
    while ( pParticleNode != &mParticleNodeHead )
    {
        // Interpolate Particle.
        pParticleNode->mRenderTickPosition = (pParticleNode->mPreTickPosition * timeDelta) + ((1.0f-timeDelta) * pParticleNode->mPostTickPosition);

        // Calculate Clip Boundary.
        t2dSceneObject::transformRectangle2D( pParticleNode->mRotationMatrix, mGlobalClipBoundary, pParticleNode->mRenderTickPosition, pParticleNode->mLocalClipBoundary );

        // Move to next Particle Node.
        pParticleNode = pParticleNode->mNextNode;
    }
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dParticleEmitter::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // No point in going further if there's no particles active!
    if ( mActiveParticles == 0 )
        return;

    // Cannot Render without Animation/ImageMap.
    if ( !mAnimationSelected && !mImageMapDataBlock )
        return;

    // Cannot Render if we're using an animation and the controller doesn't have a valid datablock.
    if ( mAnimationSelected && !mAnimationControllerProxy.getIsImageMapValid() )
        return;

    // Save Modelview.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Intense Particles?
    if ( mIntenseParticles )
    {
        // Yes, so enable blending.
        glEnable( GL_BLEND );
        // Set Blend Function.
        glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    }
    else
    {
        // No, so set standard blend options.
        if ( mBlending )
        {
            // Enable Blending.
            glEnable( GL_BLEND );
            // Set Blend Function.
            glBlendFunc( mSrcBlendFactor, mDstBlendFactor );
        }
        else
        {
            // Disable Blending.
            glDisable( GL_BLEND );
        }
    }


    // Image Regions.
    F32 minX;
    F32 minY;
    F32 maxX;
    F32 maxY;

    // Enable Texturing.
    glEnable        ( GL_TEXTURE_2D );

    // Are we using an ImageMap?
    if ( !mAnimationSelected )
    {
        // Yes, so fetch frame area and bind appropriate texture page.
        const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameAreaBind( mImageMapFrame );

        // Fetch Positions.
        minX = frameArea.mX;
        minY = frameArea.mY;
        maxX = frameArea.mX2;
        maxY = frameArea.mY2;
    }

    // Is the Position attached to the emitter?
    if ( mAttachPositionToEmitter )
    {
        // Yes, so get Effect Position.
        const t2dVector effectPos = pParentEffectObject->getPosition();
        // Move into Emitter-Space.
        glTranslatef( effectPos.mX, effectPos.mY, 0.0f );

        // Is the Rotation attached to the emitter?
        if ( mAttachRotationToEmitter )
        {
            // Yes, so rotate into Emitter-Space.
            // NOTE:- We need clockwise rotation here.
            glRotatef( pParentEffectObject->getRenderRotation(), 0.0f, 0.0f, 1.0f );
        }
    }

    // Fetch First Particle ( using appropriate sort-order ).
    tParticleNode* pParticleNode = mFirstInFrontOrder ? mParticleNodeHead.mNextNode : mParticleNodeHead.mPreviousNode;

    // Last Colour.
    ColorF lastColour(-1,-1,-1);

    // Last Texture.
    S32 lastTexture = -1;
    S32 frameTextureName;

    // Process All particle nodes.
    while ( pParticleNode != &mParticleNodeHead )
    {
        // Are we using an Animation?
        if ( mAnimationSelected )
        {
            // Yes, so bind to current frame.
            frameTextureName = pParticleNode->mAnimationController.getCurrentFrameTexture().getGLName();

            // Bind Texture if it was bound last time.
            if ( frameTextureName != lastTexture )
            {
                // Bind Texture.
                glBindTexture( GL_TEXTURE_2D, frameTextureName );
                // Note the Texture.
                lastTexture = frameTextureName;
            }

            // Fetch current frame area.
            const t2dImageMapDatablock::cFrameTexelArea& frameArea = pParticleNode->mAnimationController.getCurrentFrameArea();

            // Fetch Positions.
            minX = frameArea.mX;
            minY = frameArea.mY;
            maxX = frameArea.mX2;
            maxY = frameArea.mY2;
        }
		
#ifdef TORQUE_OS_IPHONE	
		//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
		if ( pParticleNode->mColour != lastColour )
        {
            // Set Colour.
            glColor4f(pParticleNode->mColour.red, pParticleNode->mColour.green, pParticleNode->mColour.blue, pParticleNode->mColour.alpha);
			
            // Store Last Colour.
            lastColour = pParticleNode->mColour;
        }
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		dglDrawTextureQuadiPhone(pParticleNode->mLocalClipBoundary[0].mX, 
								 pParticleNode->mLocalClipBoundary[0].mY,
								 pParticleNode->mLocalClipBoundary[1].mX, 
								 pParticleNode->mLocalClipBoundary[1].mY, 
								 pParticleNode->mLocalClipBoundary[3].mX, 
								 pParticleNode->mLocalClipBoundary[3].mY, 
								 pParticleNode->mLocalClipBoundary[2].mX, 
								 pParticleNode->mLocalClipBoundary[2].mY, 
									minX, minY, maxX, minY, minX, maxY, maxX, maxY );

#else
        // Set Colour (if changed).
        if ( pParticleNode->mColour != lastColour )
        {
            // Set Colour.
            glColor4fv( (GLfloat*)&(pParticleNode->mColour) );

            // Store Last Colour.
            lastColour = pParticleNode->mColour;
        }

        // Draw Object.
        glBegin(GL_QUADS);
            glTexCoord2f( minX, minY );
            glVertex2fv ( (GLfloat*)&(pParticleNode->mLocalClipBoundary[0]) );
            glTexCoord2f( maxX, minY );
            glVertex2fv ( (GLfloat*)&(pParticleNode->mLocalClipBoundary[1]) );
            glTexCoord2f( maxX, maxY );
            glVertex2fv ( (GLfloat*)&(pParticleNode->mLocalClipBoundary[2]) );
            glTexCoord2f( minX, maxY );
            glVertex2fv ( (GLfloat*)&(pParticleNode->mLocalClipBoundary[3]) );
        glEnd();
#endif

        // Move to next Particle ( using appropriate sort-order ).
        pParticleNode = mFirstInFrontOrder ? pParticleNode->mNextNode : pParticleNode->mPreviousNode;
    };

    // Reset Colour.
    glColor4f(1,1,1,1);
    // Disable Texturing.
    glDisable( GL_TEXTURE_2D );

    // Restore Matrix.
    glPopMatrix();
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dParticleEmitter )
    REGISTER_SERIALISE_VERSION( t2dParticleEmitter, 1, true )
    REGISTER_SERIALISE_VERSION( t2dParticleEmitter, 2, false )
    REGISTER_SERIALISE_VERSION( t2dParticleEmitter, 3, false )
REGISTER_SERIALISE_END()

// Implement Leaf Serialisation.
IMPLEMENT_T2D_SERIALISE_LEAF( t2dParticleEmitter, 3 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dParticleEmitter, 1 )
{
    // Free All Particles.
    object->freeAllParticles();

    // *********************************************************
    // Read Object Information.
    // *********************************************************

    // Load Graph Count.
    S32 graphCount;
    if ( !stream.read( &graphCount ) )
        return false;

    // Load Graphs.
    for ( U32 n = 0; n < graphCount; n++ )
    {
        // Load/Find Graph Name.
        t2dGraphField* pGraphField = object->findGraphSelection( stream.readSTString() );
        // Check Graph Field.
        if ( !pGraphField )
            return false;

        // Load Graph Object.
        if ( !pGraphField->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false;
    }

    // Load Graph Selection Flag.
    bool graphSelection;
    if ( !stream.read( &graphSelection ) )
        return false;

    // Do we have a Graph Selection?
    if ( graphSelection )
        // Yes, so Read Graph Name and Select it.
        object->selectGraph( stream.readSTString() );


    // Load Non-Graph Entries.
    object->mEmitterName = stream.readSTString();

    if (    !stream.read( &(object->mGlobalClipBoundary[0].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[0].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[1].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[1].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[2].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[2].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[3].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[3].mY) ) ||
            !stream.read( &object->mTimeSinceLastGeneration ) ||
            !stream.read( &object->mPauseEmitter ) ||
            !stream.read( &object->mFixedAspect ) ||
            !stream.read( &object->mFixedForceDirection.mX ) ||
            !stream.read( &object->mFixedForceDirection.mY ) ||
            !stream.read( &object->mFixedForceAngle ) ||
            !stream.read( (S32*)&object->mParticleOrientationMode ) ||
            !stream.read( &object->mAlign_AngleOffset ) ||
            !stream.read( &object->mAlign_KeepAligned ) ||
            !stream.read( &object->mRandom_AngleOffset ) ||
            !stream.read( &object->mRandom_Arc ) ||
            !stream.read( &object->mFixed_AngleOffset ) ||
            !stream.read( (S32*)&object->mEmitterType ) ||
            !stream.read( &object->mPivotPoint.mX ) ||
            !stream.read( &object->mPivotPoint.mY ) ||
            !stream.read( &object->mUseEffectEmission ) ||
            !stream.read( &object->mLinkEmissionRotation ) ||
            !stream.read( &object->mIntenseParticles ) ||
            !stream.read( &object->mSingleParticle ) ||
            !stream.read( &object->mAttachPositionToEmitter ) ||
            !stream.read( &object->mAttachRotationToEmitter ) ||
            !stream.read( &object->mFirstInFrontOrder ),
            !stream.read( &object->mPauseEmitter ) )
        return false;

    // Load Animation Flag.
    if ( !stream.read( &object->mAnimationSelected ) )
        return false;

    // Do we have an animation selected?
    if ( object->mAnimationSelected )
    {
        // Yes, so load Animation Controller Proxy.
        if ( !object->mAnimationControllerProxy.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false;

        // LoadAnimation Name.
        object->mAnimationName = stream.readSTString();
    }
    else
    {
        // No, so Load ImageMap Name..
        object->mImageMapName = stream.readSTString();
        // Load ImageMap Frame.
        if ( !stream.read( &object->mImageMapFrame ) )
            return false;

        // Set ImageMap Name/Frame.
        object->setImageMap( object->mImageMapName, object->mImageMapFrame );
    }

    // Load Particle Count.
    U32 particleCount;
    if ( !stream.read( &particleCount ) )
        return false;

    // Load All particle nodes.
    // NOTE:-   We load all the particles which were saved in reverse order
    //          which means that they'll now be put into forward order.
    for ( U32 n = 0; n < particleCount; n++ )
    {
        // Create Particle.
        tParticleNode* pParticleNode = object->createParticle();

        // Load Particle Node.
        if (    !stream.read( &(pParticleNode->mParticleLifetime) ) ||
                !stream.read( &(pParticleNode->mParticleAge) ) ||
                !stream.read( &(pParticleNode->mPosition.mX) ) ||
                !stream.read( &(pParticleNode->mPosition.mY) ) ||
                !stream.read( &(pParticleNode->mVelocity.mX) ) ||
                !stream.read( &(pParticleNode->mVelocity.mY) ) ||
                !stream.read( &(pParticleNode->mOrientationAngle) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[0].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[0].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[1].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[1].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[2].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[2].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[3].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[3].mY) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE11) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE12) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE21) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE22) ) ||
                !stream.read( &(pParticleNode->mLastRenderSize.mX) ) ||
                !stream.read( &(pParticleNode->mLastRenderSize.mY) ) ||
                !stream.read( &(pParticleNode->mRenderSize.mX) ) ||
                !stream.read( &(pParticleNode->mRenderSize.mY) ) ||
                !stream.read( &(pParticleNode->mRenderSpeed) ) ||
                !stream.read( &(pParticleNode->mRenderSpin) ) ||
                !stream.read( &(pParticleNode->mRenderFixedForce) ) ||
                !stream.read( &(pParticleNode->mRenderRandomMotion) ) ||
                !stream.read( &(pParticleNode->mSize.mX) ) ||
                !stream.read( &(pParticleNode->mSize.mY) ) ||
                !stream.read( &(pParticleNode->mSpeed) ) ||
                !stream.read( &(pParticleNode->mSpin) ) ||
                !stream.read( &(pParticleNode->mFixedForce) ) ||
                !stream.read( &(pParticleNode->mRandomMotion) ) ||
                !stream.read( &(pParticleNode->mColour) ) )
            return false;

        // Load Animation Controller (if animation is selected).
        if ( object->mAnimationSelected )
            pParticleNode->mAnimationController.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS );
    };
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dParticleEmitter, 1 )
{
    // Save Graph Count.
    if ( !stream.write( object->mGraphSelectionList.size() ) )
        return false;

    // Save Graphs.
    for ( U32 n = 0; n < object->mGraphSelectionList.size(); n++ )
    {
        // Write Graph Name.
        stream.writeString( object->mGraphSelectionList[n]->mGraphName );
        // Write Graph Object.
        if ( !object->mGraphSelectionList[n]->mpGraphObject->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
            return false;
    }

    // Save Graph Selection Flag.
    if ( !stream.write( (object->mpCurrentGraph != NULL) ) )
        return false;

    // Do we have a Graph Selection?
    if ( object->mpCurrentGraph )
        // Yes, so save Graph Selection.
        stream.writeString( object->mCurrentGraphName );

    // Save Non-Graph Entries.
    stream.writeString( object->mEmitterName );

    if (    !stream.write( object->mGlobalClipBoundary[0].mX ) ||
            !stream.write( object->mGlobalClipBoundary[0].mY ) ||
            !stream.write( object->mGlobalClipBoundary[1].mX ) ||
            !stream.write( object->mGlobalClipBoundary[1].mY ) ||
            !stream.write( object->mGlobalClipBoundary[2].mX ) ||
            !stream.write( object->mGlobalClipBoundary[2].mY ) ||
            !stream.write( object->mGlobalClipBoundary[3].mX ) ||
            !stream.write( object->mGlobalClipBoundary[3].mY ) ||
            !stream.write( object->mTimeSinceLastGeneration ) ||
            !stream.write( object->mPauseEmitter ) ||
            !stream.write( object->mFixedAspect ) ||
            !stream.write( object->mFixedForceDirection.mX ) ||
            !stream.write( object->mFixedForceDirection.mY ) ||
            !stream.write( object->mFixedForceAngle ) ||
            !stream.write( (S32)object->mParticleOrientationMode ) ||
            !stream.write( object->mAlign_AngleOffset ) ||
            !stream.write( object->mAlign_KeepAligned ) ||
            !stream.write( object->mRandom_AngleOffset ) ||
            !stream.write( object->mRandom_Arc ) ||
            !stream.write( object->mFixed_AngleOffset ) ||
            !stream.write( (S32)object->mEmitterType ) ||
            !stream.write( object->mPivotPoint.mX ) ||
            !stream.write( object->mPivotPoint.mY ) ||
            !stream.write( object->mUseEffectEmission ) ||
            !stream.write( object->mLinkEmissionRotation ) ||
            !stream.write( object->mIntenseParticles ) ||
            !stream.write( object->mSingleParticle ) ||
            !stream.write( object->mAttachPositionToEmitter ) ||
            !stream.write( object->mAttachRotationToEmitter ) ||
            !stream.write( object->mFirstInFrontOrder ),
            !stream.write( object->mPauseEmitter ) )
        return false;

    // Save Animation Flag.
    if ( !stream.write( object->mAnimationSelected ) )
        return false;

    // Do we have an animation selected?
    if ( object->mAnimationSelected )
    {
        // Yes, so save Animation Controller Proxy.
        if ( !object->mAnimationControllerProxy.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
            return false;

        // Save Animation Name.
        stream.writeString( object->mAnimationName );
    }
    else
    {
        // No, so Save ImageMap Name..
        stream.writeString( object->mImageMapName );
        // Save ImageMap Frame.
        if ( !stream.write( object->mImageMapFrame ) )
            return false;
    }

    // Save Active Particle Count.
    if ( !stream.write( object->mActiveParticles ) )
        return false;

    // Fetch First Particle.
    tParticleNode* pParticleNode = object->mParticleNodeHead.mPreviousNode;

    // Save All particle nodes.
    // NOTE:-   We save all the particles in reverse order so that when we
    //          subsequently load them, they'll be put into forward order.
    while ( pParticleNode != &object->mParticleNodeHead )
    {
        // Write Particle Node.
        if (    !stream.write( pParticleNode->mParticleLifetime ) ||
                !stream.write( pParticleNode->mParticleAge ) ||
                !stream.write( pParticleNode->mPosition.mX ) ||
                !stream.write( pParticleNode->mPosition.mY ) ||
                !stream.write( pParticleNode->mVelocity.mX ) ||
                !stream.write( pParticleNode->mVelocity.mY ) ||
                !stream.write( pParticleNode->mOrientationAngle ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[0].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[0].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[1].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[1].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[2].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[2].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[3].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[3].mY ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE11 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE12 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE21 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE22 ) ||
                !stream.write( pParticleNode->mLastRenderSize.mX ) ||
                !stream.write( pParticleNode->mLastRenderSize.mY ) ||
                !stream.write( pParticleNode->mRenderSize.mX ) ||
                !stream.write( pParticleNode->mRenderSize.mY ) ||
                !stream.write( pParticleNode->mRenderSpeed ) ||
                !stream.write( pParticleNode->mRenderSpin ) ||
                !stream.write( pParticleNode->mRenderFixedForce ) ||
                !stream.write( pParticleNode->mRenderRandomMotion ) ||
                !stream.write( pParticleNode->mSize.mX ) ||
                !stream.write( pParticleNode->mSize.mY ) ||
                !stream.write( pParticleNode->mSpeed ) ||
                !stream.write( pParticleNode->mSpin ) ||
                !stream.write( pParticleNode->mFixedForce ) ||
                !stream.write( pParticleNode->mRandomMotion ) ||
                !stream.write( pParticleNode->mColour ) )
            return false;

        // Write Animation Controller (if animation is selected).
        if ( object->mAnimationSelected )
            pParticleNode->mAnimationController.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS );

        // Move to previous Particle.
        pParticleNode = pParticleNode->mPreviousNode;
    };

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dParticleEmitter, 2 )
{
    // Free All Particles.
    object->freeAllParticles();

    // *********************************************************
    // Read Object Information.
    // *********************************************************

    // Load Graph Count.
    S32 graphCount;
    if ( !stream.read( &graphCount ) )
        return false;

    // Load Graphs.
    for ( U32 n = 0; n < graphCount; n++ )
    {
        // Load/Find Graph Name.
        t2dGraphField* pGraphField = object->findGraphSelection( stream.readSTString() );
        // Check Graph Field.
        if ( !pGraphField )
            return false;

        // Load Graph Object.
        if ( !pGraphField->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false;
    }

    // Load Graph Selection Flag.
    bool graphSelection;
    if ( !stream.read( &graphSelection ) )
        return false;

    // Do we have a Graph Selection?
    if ( graphSelection )
        // Yes, so Read Graph Name and Select it.
        object->selectGraph( stream.readSTString() );


    // Load Non-Graph Entries.
    object->mEmitterName = stream.readSTString();

    bool    blending;
    U32     srcBlendFactor;
    U32     dstBlendFactor;

    if (    !stream.read( &(object->mGlobalClipBoundary[0].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[0].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[1].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[1].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[2].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[2].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[3].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[3].mY) ) ||
            !stream.read( &object->mTimeSinceLastGeneration ) ||
            !stream.read( &object->mPauseEmitter ) ||
            !stream.read( &object->mFixedAspect ) ||
            !stream.read( &object->mFixedForceDirection.mX ) ||
            !stream.read( &object->mFixedForceDirection.mY ) ||
            !stream.read( &object->mFixedForceAngle ) ||
            !stream.read( (S32*)&object->mParticleOrientationMode ) ||
            !stream.read( &object->mAlign_AngleOffset ) ||
            !stream.read( &object->mAlign_KeepAligned ) ||
            !stream.read( &object->mRandom_AngleOffset ) ||
            !stream.read( &object->mRandom_Arc ) ||
            !stream.read( &object->mFixed_AngleOffset ) ||
            !stream.read( (S32*)&object->mEmitterType ) ||
            !stream.read( &object->mPivotPoint.mX ) ||
            !stream.read( &object->mPivotPoint.mY ) ||
            !stream.read( &object->mUseEffectEmission ) ||
            !stream.read( &object->mLinkEmissionRotation ) ||
            !stream.read( &object->mIntenseParticles ) ||
            !stream.read( &object->mSingleParticle ) ||
            !stream.read( &object->mAttachPositionToEmitter ) ||
            !stream.read( &object->mAttachRotationToEmitter ) ||
            !stream.read( &object->mFirstInFrontOrder ) ||
            !stream.read( &object->mPauseEmitter ) ||
            !stream.read( &blending ) ||
            !stream.read( &srcBlendFactor ) ||
            !stream.read( &dstBlendFactor ) )
        return false;

    // Set Blending.
    object->setBlending( blending, srcBlendFactor, dstBlendFactor );

    // Load Animation Flag.
    if ( !stream.read( &object->mAnimationSelected ) )
        return false;

    // Do we have an animation selected?
    if ( object->mAnimationSelected )
    {
        // Yes, so load Animation Controller Proxy.
        if ( !object->mAnimationControllerProxy.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false;

        // LoadAnimation Name.
        object->mAnimationName = stream.readSTString();
    }
    else
    {
        // No, so Load ImageMap Name..
        object->mImageMapName = stream.readSTString();
        // Load ImageMap Frame.
        if ( !stream.read( &object->mImageMapFrame ) )
            return false;

        // Set ImageMap Name/Frame.
        object->setImageMap( object->mImageMapName, object->mImageMapFrame );
    }

    // Load Particle Count.
    U32 particleCount;
    if ( !stream.read( &particleCount ) )
        return false;

    // Load All particle nodes.
    // NOTE:-   We load all the particles which were saved in reverse order
    //          which means that they'll now be put into forward order.
    for ( U32 n = 0; n < particleCount; n++ )
    {
        // Create Particle.
        tParticleNode* pParticleNode = object->createParticle();

        // Load Particle Node.
        if (    !stream.read( &(pParticleNode->mParticleLifetime) ) ||
                !stream.read( &(pParticleNode->mParticleAge) ) ||
                !stream.read( &(pParticleNode->mPosition.mX) ) ||
                !stream.read( &(pParticleNode->mPosition.mY) ) ||
                !stream.read( &(pParticleNode->mVelocity.mX) ) ||
                !stream.read( &(pParticleNode->mVelocity.mY) ) ||
                !stream.read( &(pParticleNode->mOrientationAngle) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[0].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[0].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[1].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[1].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[2].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[2].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[3].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[3].mY) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE11) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE12) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE21) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE22) ) ||
                !stream.read( &(pParticleNode->mLastRenderSize.mX) ) ||
                !stream.read( &(pParticleNode->mLastRenderSize.mY) ) ||
                !stream.read( &(pParticleNode->mRenderSize.mX) ) ||
                !stream.read( &(pParticleNode->mRenderSize.mY) ) ||
                !stream.read( &(pParticleNode->mRenderSpeed) ) ||
                !stream.read( &(pParticleNode->mRenderSpin) ) ||
                !stream.read( &(pParticleNode->mRenderFixedForce) ) ||
                !stream.read( &(pParticleNode->mRenderRandomMotion) ) ||
                !stream.read( &(pParticleNode->mSize.mX) ) ||
                !stream.read( &(pParticleNode->mSize.mY) ) ||
                !stream.read( &(pParticleNode->mSpeed) ) ||
                !stream.read( &(pParticleNode->mSpin) ) ||
                !stream.read( &(pParticleNode->mFixedForce) ) ||
                !stream.read( &(pParticleNode->mRandomMotion) ) ||
                !stream.read( &(pParticleNode->mColour) ) )
            return false;

        // Load Animation Controller (if animation is selected).
        if ( object->mAnimationSelected )
            pParticleNode->mAnimationController.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS );
    };
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dParticleEmitter, 2 )
{
    // Save Graph Count.
    if ( !stream.write( object->mGraphSelectionList.size() ) )
        return false;

    // Save Graphs.
    for ( U32 n = 0; n < object->mGraphSelectionList.size(); n++ )
    {
        // Write Graph Name.
        stream.writeString( object->mGraphSelectionList[n]->mGraphName );
        // Write Graph Object.
        if ( !object->mGraphSelectionList[n]->mpGraphObject->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
            return false;
    }

    // Save Graph Selection Flag.
    if ( !stream.write( (object->mpCurrentGraph != NULL) ) )
        return false;

    // Do we have a Graph Selection?
    if ( object->mpCurrentGraph )
        // Yes, so save Graph Selection.
        stream.writeString( object->mCurrentGraphName );

    // Save Non-Graph Entries.
    stream.writeString( object->mEmitterName );

    if (    !stream.write( object->mGlobalClipBoundary[0].mX ) ||
            !stream.write( object->mGlobalClipBoundary[0].mY ) ||
            !stream.write( object->mGlobalClipBoundary[1].mX ) ||
            !stream.write( object->mGlobalClipBoundary[1].mY ) ||
            !stream.write( object->mGlobalClipBoundary[2].mX ) ||
            !stream.write( object->mGlobalClipBoundary[2].mY ) ||
            !stream.write( object->mGlobalClipBoundary[3].mX ) ||
            !stream.write( object->mGlobalClipBoundary[3].mY ) ||
            !stream.write( object->mTimeSinceLastGeneration ) ||
            !stream.write( object->mPauseEmitter ) ||
            !stream.write( object->mFixedAspect ) ||
            !stream.write( object->mFixedForceDirection.mX ) ||
            !stream.write( object->mFixedForceDirection.mY ) ||
            !stream.write( object->mFixedForceAngle ) ||
            !stream.write( (S32)object->mParticleOrientationMode ) ||
            !stream.write( object->mAlign_AngleOffset ) ||
            !stream.write( object->mAlign_KeepAligned ) ||
            !stream.write( object->mRandom_AngleOffset ) ||
            !stream.write( object->mRandom_Arc ) ||
            !stream.write( object->mFixed_AngleOffset ) ||
            !stream.write( (S32)object->mEmitterType ) ||
            !stream.write( object->mPivotPoint.mX ) ||
            !stream.write( object->mPivotPoint.mY ) ||
            !stream.write( object->mUseEffectEmission ) ||
            !stream.write( object->mLinkEmissionRotation ) ||
            !stream.write( object->mIntenseParticles ) ||
            !stream.write( object->mSingleParticle ) ||
            !stream.write( object->mAttachPositionToEmitter ) ||
            !stream.write( object->mAttachRotationToEmitter ) ||
            !stream.write( object->mFirstInFrontOrder ) ||
            !stream.write( object->mPauseEmitter ) ||
            !stream.write( object->mBlending ) ||
            !stream.write( object->mSrcBlendFactor ) ||
            !stream.write( object->mDstBlendFactor ) )
        return false;

    // Save Animation Flag.
    if ( !stream.write( object->mAnimationSelected ) )
        return false;

    // Do we have an animation selected?
    if ( object->mAnimationSelected )
    {
        // Yes, so save Animation Controller Proxy.
        if ( !object->mAnimationControllerProxy.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
            return false;

        // Save Animation Name.
        stream.writeString( object->mAnimationName );
    }
    else
    {
        // No, so Save ImageMap Name..
        stream.writeString( object->mImageMapName );
        // Save ImageMap Frame.
        if ( !stream.write( object->mImageMapFrame ) )
            return false;
    }

    // Save Active Particle Count.
    if ( !stream.write( object->mActiveParticles ) )
        return false;

    // Fetch First Particle.
    tParticleNode* pParticleNode = object->mParticleNodeHead.mPreviousNode;

    // Save All particle nodes.
    // NOTE:-   We save all the particles in reverse order so that when we
    //          subsequently load them, they'll be put into forward order.
    while ( pParticleNode != &object->mParticleNodeHead )
    {
        // Write Particle Node.
        if (    !stream.write( pParticleNode->mParticleLifetime ) ||
                !stream.write( pParticleNode->mParticleAge ) ||
                !stream.write( pParticleNode->mPosition.mX ) ||
                !stream.write( pParticleNode->mPosition.mY ) ||
                !stream.write( pParticleNode->mVelocity.mX ) ||
                !stream.write( pParticleNode->mVelocity.mY ) ||
                !stream.write( pParticleNode->mOrientationAngle ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[0].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[0].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[1].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[1].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[2].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[2].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[3].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[3].mY ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE11 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE12 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE21 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE22 ) ||
                !stream.write( pParticleNode->mLastRenderSize.mX ) ||
                !stream.write( pParticleNode->mLastRenderSize.mY ) ||
                !stream.write( pParticleNode->mRenderSize.mX ) ||
                !stream.write( pParticleNode->mRenderSize.mY ) ||
                !stream.write( pParticleNode->mRenderSpeed ) ||
                !stream.write( pParticleNode->mRenderSpin ) ||
                !stream.write( pParticleNode->mRenderFixedForce ) ||
                !stream.write( pParticleNode->mRenderRandomMotion ) ||
                !stream.write( pParticleNode->mSize.mX ) ||
                !stream.write( pParticleNode->mSize.mY ) ||
                !stream.write( pParticleNode->mSpeed ) ||
                !stream.write( pParticleNode->mSpin ) ||
                !stream.write( pParticleNode->mFixedForce ) ||
                !stream.write( pParticleNode->mRandomMotion ) ||
                !stream.write( pParticleNode->mColour ) )
            return false;

        // Write Animation Controller (if animation is selected).
        if ( object->mAnimationSelected )
            pParticleNode->mAnimationController.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS );

        // Move to previous Particle.
        pParticleNode = pParticleNode->mPreviousNode;
    };

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dParticleEmitter, 3 )
{
    // Free All Particles.
    object->freeAllParticles();

    // *********************************************************
    // Read Object Information.
    // *********************************************************

    // Load Graph Count.
    S32 graphCount;
    if ( !stream.read( &graphCount ) )
        return false;

    // Load Graphs.
    for ( U32 n = 0; n < graphCount; n++ )
    {
        // Load/Find Graph Name.
        t2dGraphField* pGraphField = object->findGraphSelection( stream.readSTString() );
        // Check Graph Field.
        if ( !pGraphField )
            return false;

        // Load Graph Object.
        if ( !pGraphField->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false;
    }

    // Load Graph Selection Flag.
    bool graphSelection;
    if ( !stream.read( &graphSelection ) )
        return false;

    // Do we have a Graph Selection?
    if ( graphSelection )
        // Yes, so Read Graph Name and Select it.
        object->selectGraph( stream.readSTString() );


    // Load Non-Graph Entries.
    object->mEmitterName = stream.readSTString();

    bool    emitterCollisionStatus;
    bool    blending;
    U32     srcBlendFactor;
    U32     dstBlendFactor;

    if (    !stream.read( &(object->mGlobalClipBoundary[0].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[0].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[1].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[1].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[2].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[2].mY) ) ||
            !stream.read( &(object->mGlobalClipBoundary[3].mX) ) ||
            !stream.read( &(object->mGlobalClipBoundary[3].mY) ) ||
            !stream.read( &object->mTimeSinceLastGeneration ) ||
            !stream.read( &object->mPauseEmitter ) ||
            !stream.read( &object->mFixedAspect ) ||
            !stream.read( &object->mFixedForceDirection.mX ) ||
            !stream.read( &object->mFixedForceDirection.mY ) ||
            !stream.read( &object->mFixedForceAngle ) ||
            !stream.read( (S32*)&object->mParticleOrientationMode ) ||
            !stream.read( &object->mAlign_AngleOffset ) ||
            !stream.read( &object->mAlign_KeepAligned ) ||
            !stream.read( &object->mRandom_AngleOffset ) ||
            !stream.read( &object->mRandom_Arc ) ||
            !stream.read( &object->mFixed_AngleOffset ) ||
            !stream.read( (S32*)&object->mEmitterType ) ||
            !stream.read( &object->mPivotPoint.mX ) ||
            !stream.read( &object->mPivotPoint.mY ) ||
            !stream.read( &object->mUseEffectEmission ) ||
            !stream.read( &object->mLinkEmissionRotation ) ||
            !stream.read( &object->mIntenseParticles ) ||
            !stream.read( &object->mSingleParticle ) ||
            !stream.read( &object->mAttachPositionToEmitter ) ||
            !stream.read( &object->mAttachRotationToEmitter ) ||
            !stream.read( &object->mFirstInFrontOrder ) ||
            !stream.read( &object->mPauseEmitter ) ||
            !stream.read( &blending ) ||
            !stream.read( &srcBlendFactor ) ||
            !stream.read( &dstBlendFactor ),
            !stream.read( &emitterCollisionStatus ) )
        return false;

    //Emitter Collision Status.
    object->setEmitterCollisionStatus( emitterCollisionStatus );

    // Set Blending.
    object->setBlending( blending, srcBlendFactor, dstBlendFactor );

    // Load Animation Flag.
    if ( !stream.read( &object->mAnimationSelected ) )
        return false;

    // Do we have an animation selected?
    if ( object->mAnimationSelected )
    {
        // Yes, so load Animation Controller Proxy.
        if ( !object->mAnimationControllerProxy.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
            return false;

        // LoadAnimation Name.
        object->mAnimationName = stream.readSTString();
    }
    else
    {
        // No, so Load ImageMap Name..
        object->mImageMapName = stream.readSTString();
        // Load ImageMap Frame.
        if ( !stream.read( &object->mImageMapFrame ) )
            return false;

        // Set ImageMap Name/Frame.
        object->setImageMap( object->mImageMapName, object->mImageMapFrame );
    }

    // Load Particle Count.
    U32 particleCount;
    if ( !stream.read( &particleCount ) )
        return false;

    // Load All particle nodes.
    // NOTE:-   We load all the particles which were saved in reverse order
    //          which means that they'll now be put into forward order.
    for ( U32 n = 0; n < particleCount; n++ )
    {
        // Create Particle.
        tParticleNode* pParticleNode = object->createParticle();

        // Load Particle Node.
        if (    !stream.read( &(pParticleNode->mParticleLifetime) ) ||
                !stream.read( &(pParticleNode->mParticleAge) ) ||
                !stream.read( &(pParticleNode->mPosition.mX) ) ||
                !stream.read( &(pParticleNode->mPosition.mY) ) ||
                !stream.read( &(pParticleNode->mVelocity.mX) ) ||
                !stream.read( &(pParticleNode->mVelocity.mY) ) ||
                !stream.read( &(pParticleNode->mOrientationAngle) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[0].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[0].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[1].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[1].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[2].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[2].mY) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[3].mX) ) ||
                !stream.read( &(pParticleNode->mLocalClipBoundary[3].mY) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE11) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE12) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE21) ) ||
                !stream.read( &(pParticleNode->mRotationMatrix.mE22) ) ||
                !stream.read( &(pParticleNode->mLastRenderSize.mX) ) ||
                !stream.read( &(pParticleNode->mLastRenderSize.mY) ) ||
                !stream.read( &(pParticleNode->mRenderSize.mX) ) ||
                !stream.read( &(pParticleNode->mRenderSize.mY) ) ||
                !stream.read( &(pParticleNode->mRenderSpeed) ) ||
                !stream.read( &(pParticleNode->mRenderSpin) ) ||
                !stream.read( &(pParticleNode->mRenderFixedForce) ) ||
                !stream.read( &(pParticleNode->mRenderRandomMotion) ) ||
                !stream.read( &(pParticleNode->mSize.mX) ) ||
                !stream.read( &(pParticleNode->mSize.mY) ) ||
                !stream.read( &(pParticleNode->mSpeed) ) ||
                !stream.read( &(pParticleNode->mSpin) ) ||
                !stream.read( &(pParticleNode->mFixedForce) ) ||
                !stream.read( &(pParticleNode->mRandomMotion) ) ||
                !stream.read( &(pParticleNode->mColour) ) )
            return false;

        // Load Animation Controller (if animation is selected).
        if ( object->mAnimationSelected )
            pParticleNode->mAnimationController.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS );
    };
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dParticleEmitter, 3 )
{
    // Save Graph Count.
    if ( !stream.write( object->mGraphSelectionList.size() ) )
        return false;

    // Save Graphs.
    for ( U32 n = 0; n < object->mGraphSelectionList.size(); n++ )
    {
        // Write Graph Name.
        stream.writeString( object->mGraphSelectionList[n]->mGraphName );
        // Write Graph Object.
        if ( !object->mGraphSelectionList[n]->mpGraphObject->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
            return false;
    }

    // Save Graph Selection Flag.
    if ( !stream.write( (object->mpCurrentGraph != NULL) ) )
        return false;

    // Do we have a Graph Selection?
    if ( object->mpCurrentGraph )
        // Yes, so save Graph Selection.
        stream.writeString( object->mCurrentGraphName );

    // Save Non-Graph Entries.
    stream.writeString( object->mEmitterName );

    if (    !stream.write( object->mGlobalClipBoundary[0].mX ) ||
            !stream.write( object->mGlobalClipBoundary[0].mY ) ||
            !stream.write( object->mGlobalClipBoundary[1].mX ) ||
            !stream.write( object->mGlobalClipBoundary[1].mY ) ||
            !stream.write( object->mGlobalClipBoundary[2].mX ) ||
            !stream.write( object->mGlobalClipBoundary[2].mY ) ||
            !stream.write( object->mGlobalClipBoundary[3].mX ) ||
            !stream.write( object->mGlobalClipBoundary[3].mY ) ||
            !stream.write( object->mTimeSinceLastGeneration ) ||
            !stream.write( object->mPauseEmitter ) ||
            !stream.write( object->mFixedAspect ) ||
            !stream.write( object->mFixedForceDirection.mX ) ||
            !stream.write( object->mFixedForceDirection.mY ) ||
            !stream.write( object->mFixedForceAngle ) ||
            !stream.write( (S32)object->mParticleOrientationMode ) ||
            !stream.write( object->mAlign_AngleOffset ) ||
            !stream.write( object->mAlign_KeepAligned ) ||
            !stream.write( object->mRandom_AngleOffset ) ||
            !stream.write( object->mRandom_Arc ) ||
            !stream.write( object->mFixed_AngleOffset ) ||
            !stream.write( (S32)object->mEmitterType ) ||
            !stream.write( object->mPivotPoint.mX ) ||
            !stream.write( object->mPivotPoint.mY ) ||
            !stream.write( object->mUseEffectEmission ) ||
            !stream.write( object->mLinkEmissionRotation ) ||
            !stream.write( object->mIntenseParticles ) ||
            !stream.write( object->mSingleParticle ) ||
            !stream.write( object->mAttachPositionToEmitter ) ||
            !stream.write( object->mAttachRotationToEmitter ) ||
            !stream.write( object->mFirstInFrontOrder ) ||
            !stream.write( object->mPauseEmitter ) ||
            !stream.write( object->mBlending ) ||
            !stream.write( object->mSrcBlendFactor ) ||
            !stream.write( object->mDstBlendFactor ),
            !stream.write( object->getEmitterCollisionStatus() ) )
        return false;

    // Save Animation Flag.
    if ( !stream.write( object->mAnimationSelected ) )
        return false;

    // Do we have an animation selected?
    if ( object->mAnimationSelected )
    {
        // Yes, so save Animation Controller Proxy.
        if ( !object->mAnimationControllerProxy.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
            return false;

        // Save Animation Name.
        stream.writeString( object->mAnimationName );
    }
    else
    {
        // No, so Save ImageMap Name..
        stream.writeString( object->mImageMapName );
        // Save ImageMap Frame.
        if ( !stream.write( object->mImageMapFrame ) )
            return false;
    }

    // Save Active Particle Count.
    if ( !stream.write( object->mActiveParticles ) )
        return false;

    // Fetch First Particle.
    tParticleNode* pParticleNode = object->mParticleNodeHead.mPreviousNode;

    // Save All particle nodes.
    // NOTE:-   We save all the particles in reverse order so that when we
    //          subsequently load them, they'll be put into forward order.
    while ( pParticleNode != &object->mParticleNodeHead )
    {
        // Write Particle Node.
        if (    !stream.write( pParticleNode->mParticleLifetime ) ||
                !stream.write( pParticleNode->mParticleAge ) ||
                !stream.write( pParticleNode->mPosition.mX ) ||
                !stream.write( pParticleNode->mPosition.mY ) ||
                !stream.write( pParticleNode->mVelocity.mX ) ||
                !stream.write( pParticleNode->mVelocity.mY ) ||
                !stream.write( pParticleNode->mOrientationAngle ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[0].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[0].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[1].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[1].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[2].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[2].mY ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[3].mX ) ||
                !stream.write( pParticleNode->mLocalClipBoundary[3].mY ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE11 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE12 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE21 ) ||
                !stream.write( pParticleNode->mRotationMatrix.mE22 ) ||
                !stream.write( pParticleNode->mLastRenderSize.mX ) ||
                !stream.write( pParticleNode->mLastRenderSize.mY ) ||
                !stream.write( pParticleNode->mRenderSize.mX ) ||
                !stream.write( pParticleNode->mRenderSize.mY ) ||
                !stream.write( pParticleNode->mRenderSpeed ) ||
                !stream.write( pParticleNode->mRenderSpin ) ||
                !stream.write( pParticleNode->mRenderFixedForce ) ||
                !stream.write( pParticleNode->mRenderRandomMotion ) ||
                !stream.write( pParticleNode->mSize.mX ) ||
                !stream.write( pParticleNode->mSize.mY ) ||
                !stream.write( pParticleNode->mSpeed ) ||
                !stream.write( pParticleNode->mSpin ) ||
                !stream.write( pParticleNode->mFixedForce ) ||
                !stream.write( pParticleNode->mRandomMotion ) ||
                !stream.write( pParticleNode->mColour ) )
            return false;

        // Write Animation Controller (if animation is selected).
        if ( object->mAnimationSelected )
            pParticleNode->mAnimationController.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS );

        // Move to previous Particle.
        pParticleNode = pParticleNode->mPreviousNode;
    };

    // Return Okay.
    return true;
}

