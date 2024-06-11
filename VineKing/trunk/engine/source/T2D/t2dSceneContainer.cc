//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Container.
//-----------------------------------------------------------------------------

#include "math/mMathFn.h"
#include "./t2dSceneObject.h"
#include "./t2dUtility.h"
#include "./t2dSceneContainer.h"

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif

//-Mat may not need to be so big
#define PUAP_DEFAULT_SCENE_REFENCE_BLOCK_SIZE	1024


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dSceneContainer::t2dSceneContainer() :    mCurrentSequenceKey(1),
                                            mSceneBinArray(NULL)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mSceneBinReferenceBlocks );
    VECTOR_SET_ASSOCIATION( mSceneAlwaysScope );
    VECTOR_SET_ASSOCIATION( mSceneDisabledBin );
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dSceneContainer::~t2dSceneContainer()
{
    // Destroy Container System.
    destroyContainerSystem();
}


//------------------------------------------------------------------------------
// Initialise Container System.
//-----------------------------------------------------------------------------
void t2dSceneContainer::initialise( t2dSceneGraph* pT2DSceneGraph, F32 containerBinSize, U32 containerBinCount )
{
    // Set Parent SceneGraph.
    mpParentSceneGraph2D = pT2DSceneGraph;

    // Set Container Bin Size/Count.
    mBinSize    = containerBinSize;
    mBinCount   = containerBinCount;

    // Calculate Total Bin Size.
    mTotalBinSize = mBinSize * mBinCount;

    // Set Scene Reference Block Size.
	mSceneReferenceBlockSize = PUAP_DEFAULT_SCENE_REFENCE_BLOCK_SIZE;

    // Create ContainerBins.
    createContainerBins();
}


//------------------------------------------------------------------------------
// Destroy Scene Container.
//
// NOTE:-   You *must* ensure that the scene has been cleared via the
//          parent scene-graph *before* you use this function.
//
//          The only reason this is here is so that the serialisation
//          system can customise the container system after it has
//          potentially already been initialised.
//
//          Avoid this call if you can!
//-----------------------------------------------------------------------------
void t2dSceneContainer::destroyContainerSystem( void )
{
    // Destroy Container Bins ( if we've got any ).
    destroyContainerBins();
}


//-----------------------------------------------------------------------------
// Create Container Bins.
//-----------------------------------------------------------------------------
void t2dSceneContainer::createContainerBins(void)
{
    // Create Scene Bin Array.
    mSceneBinArray = new t2dSceneBinReference[mBinCount*mBinCount];
    // Initialise Scene Bin Array.
    for ( U32 y = 0; y < mBinCount; y++ )
    {
        // Calculate Offset Y.
        U32 offsetY = y * mBinCount;

        // Step through X.
        for ( U32 x = 0; x < mBinCount; x++ )
        {
            mSceneBinArray[offsetY+x].mpSceneObject2D           = NULL;
            mSceneBinArray[offsetY+x].mpPreviousBinReference    = NULL;
            mSceneBinArray[offsetY+x].mpNextBinReference        = NULL;
            mSceneBinArray[offsetY+x].mpObjectLink              = NULL;
        }
    }

    // Initialise Scene Overflow Bin.
    mSceneOverflowBin.mpSceneObject2D           = NULL;
    mSceneOverflowBin.mpPreviousBinReference    = NULL;
    mSceneOverflowBin.mpNextBinReference        = NULL;
    mSceneOverflowBin.mpObjectLink              = NULL;

    // Reset Free Scene Bin References.
    mFreeSceneBinReferences = NULL;
}


//-----------------------------------------------------------------------------
// Destroy Container Bins.
//-----------------------------------------------------------------------------
void t2dSceneContainer::destroyContainerBins(void)
{
    // Finish if we've not got a bin-array!
    if ( !mSceneBinArray )
        return;

    // Remove Scene Bin Reference Pool Blocks.
    for ( U32 n = 0; n < mSceneBinReferenceBlocks.size(); n++ )
    {
        // Fetch Reference Block.
        t2dSceneBinReference* refBlock = mSceneBinReferenceBlocks[n];
        // Check the Block to ensure everything was removed!
        for ( U32 i = 0; i < mSceneReferenceBlockSize; i++ )
            AssertFatal(refBlock[i].mpSceneObject2D == NULL, " t2dSceneContainer::destroyContainerBins() - Object(s) were not removed from the container system correctly!");

        // Destroy Block.
        delete [] refBlock;
    }

    // Clear Bin References.
    mSceneBinReferenceBlocks.clear();

    // Reset Free Scene Bin References.
    mFreeSceneBinReferences = NULL;

    // Create Scene Bin Array.
    delete [] mSceneBinArray;

    // Just to be safe.
    mSceneBinArray = NULL;
}


//-----------------------------------------------------------------------------
// Add Scene Reference Block.
//-----------------------------------------------------------------------------
void t2dSceneContainer::addSceneReferenceBlock(void)
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(t2dSceneContainer_addSceneReferenceBlock);
#endif

    // Create new Scene Bin Reference Pool Block.
    mSceneBinReferenceBlocks.push_back( new t2dSceneBinReference[mSceneReferenceBlockSize] );

    // Initialise Reference Block.
    for ( U32 i = 0; i < (mSceneReferenceBlockSize-1); i++ )
    {
        // Fetch Block Entry Reference.
        t2dSceneBinReference& refBlock = mSceneBinReferenceBlocks.last()[i];
        // Initialise.
        refBlock.mpSceneObject2D        = NULL;
        refBlock.mpPreviousBinReference = NULL;
        refBlock.mpNextBinReference     = NULL;
        refBlock.mpObjectLink           = &(mSceneBinReferenceBlocks.last()[i+1]);
    }

    // Fetch Last Block Entry Reference.
    t2dSceneBinReference& refBlock = mSceneBinReferenceBlocks.last()[mSceneReferenceBlockSize-1];
    // Initialise.
    refBlock.mpSceneObject2D        = NULL;
    refBlock.mpPreviousBinReference = NULL;
    refBlock.mpNextBinReference     = NULL;
    refBlock.mpObjectLink           = mFreeSceneBinReferences;

    // Set Free References.
    mFreeSceneBinReferences = &(mSceneBinReferenceBlocks.last()[0]);

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();  // t2dSceneContainer_addSceneReferenceBlock
#endif
}


//-----------------------------------------------------------------------------
// Allocate Scene Bin Reference.
//-----------------------------------------------------------------------------
t2dSceneBinReference* t2dSceneContainer::allocateSceneBinReference()
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(t2dSceneContainer_allocateSceneBinReference);
#endif

    // Are there any Free Scene Bin References?
    if ( mFreeSceneBinReferences == NULL )
        // No, so add a block.
        addSceneReferenceBlock();

    // Sanity Check.
    AssertFatal( mFreeSceneBinReferences != NULL, "t2dSceneContainer::createSceneBinReference() - We should always have a free reference here!" );

    // Fetch Free Reference.
    t2dSceneBinReference* freeRef = mFreeSceneBinReferences;

    // Remove Reference from Free Pool.
    mFreeSceneBinReferences = mFreeSceneBinReferences->mpObjectLink;

    // Remove Reference Link.
    freeRef->mpObjectLink = NULL;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // t2dSceneContainer_allocateSceneBinReference
#endif

    // Return Reference.
    return freeRef;
}


//-----------------------------------------------------------------------------
// Free Scene Bin Reference.
//-----------------------------------------------------------------------------
void t2dSceneContainer::freeSceneBinReference(t2dSceneBinReference* pSceneBinRef )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(t2dSceneContainer_freeSceneBinReference);
#endif

    // Free Reference by binding it to the front of the free pool.
    pSceneBinRef->mpSceneObject2D = NULL;
    pSceneBinRef->mpNextBinReference = NULL;
    pSceneBinRef->mpPreviousBinReference = NULL;
    pSceneBinRef->mpObjectLink = mFreeSceneBinReferences;

    // Bind to front of the free pool.
    mFreeSceneBinReferences = pSceneBinRef;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // t2dSceneContainer_freeSceneBinReference
#endif
}

//-----------------------------------------------------------------------------
// Get Bin Range (1D).
//-----------------------------------------------------------------------------
void t2dSceneContainer::getBinRange( const F32 min, const F32 max, U32& minBin, U32& maxBin )
{
    // Check Range.
    AssertFatal( max >= min, "t2dSceneContainer::getBinRange() - Bad Range!" );

    // Is the range bigger than our total bin size?
    if ( (max-min) >= mTotalBinSize )
    {
        // Yes, so Calculate min-position.
        F32 minPosition = mFmod( min, mTotalBinSize );
        // We only want to deal with a positive position.
        if ( minPosition < 0.0f )
        {
            minPosition += mTotalBinSize;
            // Hmm, we'll let's be safe anyway!
            if ( minPosition == mTotalBinSize )
                minPosition = mTotalBinSize - 0.01f;
        }
        // Sanity!
        AssertFatal(minPosition >= 0.0 && minPosition < mTotalBinSize, "t2dSceneContainer::getBinRange() - Bad minPosition!");

        // Calculate Bins.
        minBin = U32(minPosition / mBinSize);
        maxBin = minBin + (mBinCount-1);

        // Sanity!
        AssertFatal(minBin < mBinCount, avar("t2dSceneContainer::getBinRange() - Bad clip! (%f, %d)", minPosition, minBin));

        // Finish here.
        return;
    }
    else
    {
        // Calculate min-position.
        F32 minPosition = mFmod( min, mTotalBinSize );
        // We only want to deal with a positive position.
        if ( minPosition < 0.0f )
        {
            minPosition += mTotalBinSize;
            // Hmm, we'll let's be safe anyway!
            if ( minPosition == mTotalBinSize )
                minPosition = mTotalBinSize - 0.01f;
        }
        // Sanity!
        AssertFatal(minPosition >= 0.0 && minPosition < mTotalBinSize, "t2dSceneContainer::getBinRange() - Bad minPosition!");

        // Calculate max-position.
        F32 maxPosition = mFmod( max, mTotalBinSize );
        // We only want to deal with a positive position.
        if ( maxPosition < 0.0f )
        {
            maxPosition += mTotalBinSize;
            // Hmm, we'll let's be safe anyway!
            if ( maxPosition == mTotalBinSize )
                maxPosition = mTotalBinSize - 0.01f;
        }
        // Sanity!
        AssertFatal(maxPosition >= 0.0 && maxPosition < mTotalBinSize, "t2dSceneContainer::getBinRange() - Bad maxPosition!");

        // Calculate Bins.
        minBin = U32(minPosition / mBinSize);
        maxBin = U32(maxPosition / mBinSize);

        // Sanity!
        AssertFatal(minBin < mBinCount, avar("t2dSceneContainer::getBinRange() - Bad clip! (%f, %d)", minPosition, minBin));
        AssertFatal(maxBin < mBinCount, avar("t2dSceneContainer::getBinRange() - Bad clip! (%f, %d)", maxPosition, maxBin));

        // Hmm, we'll let's be safe anyway!
        if ( min != max && minPosition > maxPosition )
            maxBin += mBinCount;

        // Sanity!
        AssertFatal(maxBin >= minBin, "t2dSceneContainer::getBinRange() - MinBin should always be less than MaxBin!");
    }
}


//-----------------------------------------------------------------------------
// Get Bin Range (2D).
//-----------------------------------------------------------------------------
void t2dSceneContainer::getBinRectangle( const RectF& rectangle, U32& minBinX, U32& minBinY, U32& maxBinX, U32& maxBinY )
{
    // Get Bin Range.
    getBinRange( rectangle.point.x, rectangle.point.x + rectangle.len_x(), minBinX, maxBinX );
    getBinRange( rectangle.point.y, rectangle.point.y + rectangle.len_y(), minBinY, maxBinY );
}


//-----------------------------------------------------------------------------
// Add Scene Object to Container.
//-----------------------------------------------------------------------------
void t2dSceneContainer::addSceneObject( t2dSceneObject* pSceneObject2D )
{
    // Check everything is fine!
    AssertFatal( pSceneObject2D != NULL, "t2dSceneContainer::addSceneObject() - Invalid Object" );
    AssertFatal( pSceneObject2D->mpBinReferenceChain == NULL, "t2dSceneContainer::addSceneObject() - Object is already within a Container System!" );

    // Find which bins we cover.
    U32 minX, minY, maxX, maxY;
    getBinRectangle( pSceneObject2D->getWorldCollisionClipRectangle(), minX, minY, maxX, maxY );

    // Add the Scene Object Directly.
    addSceneObjectDirect( pSceneObject2D, minX, minY, maxX, maxY );
}


//-----------------------------------------------------------------------------
// Add Scene Object to Container Directly (via Bin Bounds).
//-----------------------------------------------------------------------------
void t2dSceneContainer::addSceneObjectDirect( t2dSceneObject* pSceneObject2D, U32 minX, U32 minY, U32 maxX, U32 maxY )
{
    // Check everything is fine!
    AssertFatal( pSceneObject2D != NULL, "t2dSceneContainer::addSceneObject() - Invalid Object" );
    AssertFatal( pSceneObject2D->mpBinReferenceChain == NULL, "t2dSceneContainer::addSceneObject() - Object is already within a Container System!" );

    // Is the Object always scoped?
    if ( pSceneObject2D->getIsAlwaysScope() )
    {
        // Yes, so simply add to Always Scope List.
        mSceneAlwaysScope.push_back( pSceneObject2D );
        // Flag Object in Container.
        pSceneObject2D->mpBinReferenceChain = &mAlwaysScopeDummy;
        // Finish Here.
        return;
    }

    // Update Scene Object.
    pSceneObject2D->mMinBinX = minX;
    pSceneObject2D->mMinBinY = minY;
    pSceneObject2D->mMaxBinX = maxX;
    pSceneObject2D->mMaxBinY = maxY;

    // Very Large Objects got into the Scene Overflow bin.  Overflow bin?
    if ( (maxX-minX+1) < mBinCount || (maxY-minY+1) < mBinCount )
    {
        // No, so fetch Bin Object Reference.
        t2dSceneBinReference** ppCurrentObjectRef = &pSceneObject2D->mpBinReferenceChain;

        for ( U32 y = minY; y <= maxY; y++ )
        {
            // Calculate Bin Position.
            U32 offsetY = (y % mBinCount) * mBinCount;

            for ( U32 x = minX; x <= maxX; x++ )
            {
                // Calculate Bin Position.
                U32 arrayIndex = offsetY + (x % mBinCount);

                // Allocate Scene Bin Reference.
                t2dSceneBinReference* pFreeRef = allocateSceneBinReference();

                // Insert into Scene Bin Array.
                pFreeRef->mpSceneObject2D           = pSceneObject2D;
                pFreeRef->mpNextBinReference        = mSceneBinArray[arrayIndex].mpNextBinReference;
                pFreeRef->mpPreviousBinReference    = &mSceneBinArray[arrayIndex];
                pFreeRef->mpObjectLink              = NULL;

                // Finalise the link.
                if ( mSceneBinArray[arrayIndex].mpNextBinReference )
                    mSceneBinArray[arrayIndex].mpNextBinReference->mpPreviousBinReference = pFreeRef;
                mSceneBinArray[arrayIndex].mpNextBinReference = pFreeRef;

                // Insert Current Object Reference.
                *ppCurrentObjectRef = pFreeRef;
                // Move to next link.
                ppCurrentObjectRef = &pFreeRef->mpObjectLink;
            }
        }
    }
    else
    {
        // Yes, so allocate Scene Bin Reference.
        t2dSceneBinReference* pFreeRef = allocateSceneBinReference();

        // Insert into Scene Overflow Bin.
        pFreeRef->mpSceneObject2D           = pSceneObject2D;
        pFreeRef->mpNextBinReference        = mSceneOverflowBin.mpNextBinReference;
        pFreeRef->mpPreviousBinReference    = &mSceneOverflowBin;
        pFreeRef->mpObjectLink              = NULL;

        // Finalise the link.
        if ( mSceneOverflowBin.mpNextBinReference )
        {
            mSceneOverflowBin.mpNextBinReference->mpPreviousBinReference = pFreeRef;
        }
        mSceneOverflowBin.mpNextBinReference = pFreeRef;

        // Set Current Object Reference.
        pSceneObject2D->mpBinReferenceChain = pFreeRef;
    }

    // Debug Output.
    //Con::printf("Object %s added to Min(%d,%d):Max(%d,%d).", pSceneObject2D->getIdString(), pSceneObject2D->mMinBinX, pSceneObject2D->mMinBinY, pSceneObject2D->mMaxBinX, pSceneObject2D->mMaxBinY);
}


//-----------------------------------------------------------------------------
// Remove Scene Object from Container.
//-----------------------------------------------------------------------------
void t2dSceneContainer::removeSceneObject( t2dSceneObject* pSceneObject2D )
{
    // Check everything is fine!
    AssertFatal( pSceneObject2D != NULL, "t2dSceneContainer::removeSceneObject() - Invalid Object" );

    // Is the Object always scoped?
    if ( pSceneObject2D->getIsAlwaysScope() )
    {
        // Yes, so find Object in Always Scope List.
        for ( U32 n = 0; n < mSceneAlwaysScope.size(); n++ )
        {
            // Is this the Object?
            if ( mSceneAlwaysScope[n] == pSceneObject2D )
            {
                // Yes, so remove from Always Scope List.
                mSceneAlwaysScope.erase_fast(n);
                // Flag Object out of Container.
                pSceneObject2D->mpBinReferenceChain = NULL;
                // Finish Here.
                return;
            }
        }

        // Warn.
		//-Mat Particle effects are set to Always Scope need to fix
        //Con::warnf("t2dSceneContainer::removeSceneObject() - Object 'Always Scope' but not in Scope-List! (%s)", pSceneObject2D->getIdString());
        // Finish Here.
        return;
    }

    // Fetch Bin Chain Reference.
    t2dSceneBinReference* pRefChain = pSceneObject2D->mpBinReferenceChain;

    // Cut Chain from Object.
    pSceneObject2D->mpBinReferenceChain = NULL;

    // Free Chain Scene Bin References.
    while ( pRefChain )
    {
        // Fetch Free Reference.
        t2dSceneBinReference* pFreeRef = pRefChain;
        // Move to next link.
        pRefChain = pRefChain->mpObjectLink;
        // Remove Reference from Chain.
        if ( pFreeRef->mpNextBinReference )
            pFreeRef->mpNextBinReference->mpPreviousBinReference = pFreeRef->mpPreviousBinReference;
        pFreeRef->mpPreviousBinReference->mpNextBinReference = pFreeRef->mpNextBinReference;

        // Free Reference.
        freeSceneBinReference( pFreeRef );
    }
}


//-----------------------------------------------------------------------------
// Check SceneObject Bins.
//-----------------------------------------------------------------------------
void t2dSceneContainer::checkSceneObjectBins( t2dSceneObject* pSceneObject2D )
{
    // Check everything is fine!
    AssertFatal( pSceneObject2D != NULL, "t2dSceneContainer::checkSceneObjectBins() - Invalid Object" );

    // Ignore if disabled.
    if ( !pSceneObject2D->isEnabled() )
        return;

    // Is the Scene Object in the container?
    if (  pSceneObject2D->mpBinReferenceChain == NULL )
    {
        // No, so add to the Scene Container.
        addSceneObject( pSceneObject2D );
        // Finish here.
        return;
    }

    // Finish if object is always scoped.
    if ( pSceneObject2D->getIsAlwaysScope() )
        return;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(t2dSceneContainer_BinRelocation);
#endif

    // Check to see if the object has moved outside its allocated bins...

    // Find which bins we cover.
    U32 minBinX, minBinY, maxBinX, maxBinY;
    getBinRectangle( pSceneObject2D->getWorldCollisionClipRectangle(), minBinX, minBinY, maxBinX, maxBinY );

    // Should the bin allocation change?
    if (    minBinX != pSceneObject2D->mMinBinX ||
            minBinY != pSceneObject2D->mMinBinY ||
            maxBinX != pSceneObject2D->mMaxBinX ||
            maxBinY != pSceneObject2D->mMaxBinY )
    {
        // Yes, so remove scene object.
        removeSceneObject( pSceneObject2D );
        // Add the scene object back.
        addSceneObjectDirect( pSceneObject2D, minBinX, minBinY, maxBinX, maxBinY );

        // Increase Bin Relocations.
        mpParentSceneGraph2D->getDebugStats().objectBinRelocations++;

        // Debug Output.
        //Con::printf("Object changed position to (%d,%d):(%d,%d) = (%d,%d).", minBinX, minBinY, maxBinX, maxBinY, mAbs(maxBinX-minBinX+1), mAbs(maxBinY-minBinY+1));
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // t2dSceneContainer_BinRelocation
#endif
}


//--------------------------------------------------------------------------------
// Pick Line through SceneObject Bins.
//
// NOTE:-   It is worth noting that pick-line refines the search using the
//          objects' physics whereas the other 'pick' routines don't do this.
//--------------------------------------------------------------------------------
bool t2dSceneContainer::pickLine( const t2dVector& lineStart, const t2dVector& lineEnd, const U32 layerMask, const U32 graphGroupMask, const bool showInvisible, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject )
{
    // Change the Current Sequence Key.
    nextSequenceKey();

    // Reset Object Found Counter.
    U32 objectsFound = 0;

    // Fetch Overflow Bin Chain.
    t2dSceneBinReference* pBinChain = mSceneOverflowBin.mpNextBinReference;

    // Step through Chain.
    while ( pBinChain )
    {
        // Fetch Scene Object Reference.
        t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

        // Increase Bin Searches.
        mpParentSceneGraph2D->getDebugStats().objectBinSearches++;

        // Is the Object Not being ignored, Enabled, Picking Allowed and we've got Visibility?
        if ( pSceneObjectRef != pIgnoreObject && pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() && pSceneObjectRef->getIsPickingAllowed() && (pSceneObjectRef->getVisible() || showInvisible) )
        {
            // Yes, so have we dealt with this object already?
            if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
            {
                // No, so set the container sequence key to indicate so.
                pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                // Check if the Group/Layer masks match.
                if (pSceneObjectRef->getGraphGroupMask() & graphGroupMask &&
                    pSceneObjectRef->getLayerMask() & layerMask )
                {
                    // Yes, so perform line-cast.
                    if ( pSceneObjectRef->getParentPhysics().castLine( lineStart, lineEnd, pSceneObjectRef->mSortKeyCollisionNormal, pSceneObjectRef->mSortKeyCollisionTime ) )
                    {
                        // Collision so perform callback.
                        (*pFnCallback)(pSceneObjectRef, storage);
                        // Increase Objects Found Counter.
                        objectsFound++;
                    }
                }
            }
        }

        // Move to next bin reference.
        pBinChain = pBinChain->mpNextBinReference;
    }

    // Bin Rasterisation.
    t2dVector normalStart;
    t2dVector normalEnd;

    // Normalise Start/End Points.
    if ( lineStart.mX <= lineEnd.mX )
    {
        normalStart = lineStart;
        normalEnd   = lineEnd;
    }
    else
    {
        normalStart = lineEnd;
        normalEnd   = lineStart;
    }

    // Find which bins we cover.
    U32 minBinX, minBinY, maxBinX, maxBinY;
    getBinRange( normalStart.mX, normalEnd.mX, minBinX, maxBinX );
    getBinRange( getMin(normalStart.mY, normalEnd.mY), getMax(normalStart.mY, normalEnd.mY), minBinY, maxBinY );

    // Clipping Points.
    t2dVector clipMin;
    t2dVector clipMax;

    // Optimise the case that the line is contained in one bin row or column, which
    // will be quite a few lines...
    if ( (mFabs(normalStart.mX - normalEnd.mX) < mTotalBinSize && minBinX == maxBinX) ||
        (mFabs(normalStart.mY - normalEnd.mY) < mTotalBinSize && minBinY == maxBinY))
    {
        U32 count;
        U32 incX;
        U32 incY;

        // Line Stepping.
        if ( minBinX == maxBinX)
        {
            count = maxBinY - minBinY + 1;
            incX  = 0;
            incY  = 1;
        }
        else
        {
            count = maxBinX - minBinX + 1;
            incX  = 1;
            incY  = 0;
        }

        U32 xBin = minBinX;
        U32 yBin = minBinY;

        // Take appropriate Steps.
        for (U32 i = 0; i < count; i++)
        {
            U32 checkX = xBin % mBinCount;
            U32 checkY = yBin % mBinCount;

            // Fetch Bin Chain.
            t2dSceneBinReference* pBinChain = mSceneBinArray[(checkY*mBinCount)+checkX].mpNextBinReference;

            // Step through Chain.
            while ( pBinChain )
            {
                // Fetch Scene Object Reference.
                t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

                // Increase Bin Searches.
                mpParentSceneGraph2D->getDebugStats().objectBinSearches++;

                // Is the Object Not being ignored, Enabled, Picking Allowed and we've got Visibility?
                if ( pSceneObjectRef != pIgnoreObject && pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() && pSceneObjectRef->getIsPickingAllowed() && (pSceneObjectRef->getVisible() || showInvisible) )
                {
                    // Have we dealt with this object already?
                    if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
                    {
                        // No, so set the container sequence key to indicate so.
                        pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                        // Check if the Group/Layer masks match.
                        if (    pSceneObjectRef->getGraphGroupMask() & graphGroupMask &&
                            pSceneObjectRef->getLayerMask() & layerMask )
                        {
                            // Calculate World Collisiom Clipping Points.
                            RectF collisionClip = pSceneObjectRef->getWorldCollisionClipRectangle();
                            clipMin = collisionClip.point;
                            clipMax = clipMin + t2dVector(collisionClip.extent.x, collisionClip.extent.y);

                            // Do Line Intersection of the objects World-Clip Rectangle.
                            if ( pSceneObjectRef->lineRectangleIntersect( lineStart, lineEnd, clipMin, clipMax ) )
                            {
                                // Yes, so perform line-cast.
                                if ( pSceneObjectRef->getParentPhysics().castLine( lineStart, lineEnd, pSceneObjectRef->mSortKeyCollisionNormal, pSceneObjectRef->mSortKeyCollisionTime ) )
                                {
                                    // Collision so perform callback.
                                    (*pFnCallback)(pSceneObjectRef, storage);
                                    // Increase Objects Found Counter.
                                    objectsFound++;
                                }
                            }
                        }
                    }
                }

                // Move to next bin reference.
                pBinChain = pBinChain->mpNextBinReference;
            }

            // Next Bin.
            xBin += incX;
            yBin += incY;
        }
    }
    else
    {
        // We're going to cross at least one boundary...
        F32 currentStartX = normalStart.mX;

        // Sanity!
        AssertFatal(currentStartX != normalEnd.mX, "t2dSceneContainer::pickLine() - This'll mess things up big-style!");

        // Step through binx.
        while (currentStartX != normalEnd.mX)
        {
            // Calculate End-Bin & Start/End Times.
            F32 currentEndX         = getMin(currentStartX + mTotalBinSize, normalEnd.mX);
            F32 currentStartTime    = (currentStartX - normalStart.mX) / (normalEnd.mX - normalStart.mX);
            F32 currentEndTime      = (currentEndX   - normalStart.mX) / (normalEnd.mX - normalStart.mX);

            // Start/End Y-Position.
            F32 y1 = normalStart.mY + (normalEnd.mY - normalStart.mY) * currentStartTime;
            F32 y2 = normalStart.mY + (normalEnd.mY - normalStart.mY) * currentEndTime;

            // Find which bins we cover.
            U32 subMinBinX;
            U32 subMaxBinX;
            getBinRange( currentStartX, currentEndX, subMinBinX, subMaxBinX );

            // Calculate Sub Start/End Bins.
            F32 subStartX = currentStartX;
            F32 subEndX   = currentStartX;

            // Let's modulate the bin position against the container dimensions.
            if (currentStartX < 0.0f)
                subEndX -= mFmod(subEndX, mBinSize);
            else
                subEndX += (mBinSize - mFmod(subEndX, mBinSize));

            // Step through current bins.
            for (U32 currentXBin = subMinBinX; currentXBin <= subMaxBinX; currentXBin++)
            {
                // Calculate Current X Bin.
                U32 checkX = currentXBin % mBinCount;

                // Calculate Start/End Time.
                F32 subStartTime    = (subStartX - currentStartX) / (currentEndX - currentStartX);
                F32 subEndTime      = getMin(F32((subEndX - currentStartX) / (currentEndX - currentStartX)), 1.0f);

                // Start/End Y-Position.
                F32 subY1 = y1 + (y2 - y1) * subStartTime;
                F32 subY2 = y1 + (y2 - y1) * subEndTime;

                // Find which bins we cover.
                U32 newMinBinY;
                U32 newMaxBinY;
                getBinRange( getMin(subY1, subY2), getMax(subY1, subY2), newMinBinY, newMaxBinY );

                // Step through Y.
                for (U32 i = newMinBinY; i <= newMaxBinY; i++)
                {
                    // Calculate Current Y Bin.
                    U32 checkY = i % mBinCount;

                    // Fetch Bin Chain.
                    t2dSceneBinReference* pBinChain = mSceneBinArray[(checkY*mBinCount)+checkX].mpNextBinReference;

                    // Increase Bin Searches.
                    mpParentSceneGraph2D->getDebugStats().objectBinSearches++;

                    // Step through Chain.
                    while ( pBinChain )
                    {
                        // Fetch Scene Object Reference.
                        t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

                        // Is the Object Not being ignored, Enabled, Picking Allowed and we've got Visibility?
                        if ( pSceneObjectRef != pIgnoreObject && pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() && pSceneObjectRef->getIsPickingAllowed() && (pSceneObjectRef->getVisible() || showInvisible) )
                        {
                            // Yes, so have we dealt with this object already?
                            if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
                            {
                                // No, so set the container sequence key to indicate so.
                                pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                                // Check if the Group/Layer masks match.
                                if (    pSceneObjectRef->getGraphGroupMask() & graphGroupMask &&
                                    pSceneObjectRef->getLayerMask() & layerMask )
                                {
                                    // Calculate World Collision Clipping Points.
                                    RectF collisionClip = pSceneObjectRef->getWorldCollisionClipRectangle();
                                    clipMin = collisionClip.point;
                                    clipMax = clipMin + t2dVector(collisionClip.extent.x, collisionClip.extent.y);

                                    // Do Line Intersection of the objects World-Clip Rectangle.
                                    if ( pSceneObjectRef->lineRectangleIntersect( lineStart,lineEnd, clipMin, clipMax ) )
                                    {
                                        // Yes, so perform line-cast.
                                        if ( pSceneObjectRef->getParentPhysics().castLine( lineStart, lineEnd, pSceneObjectRef->mSortKeyCollisionNormal, pSceneObjectRef->mSortKeyCollisionTime ) )
                                        {
                                            // Collision so perform callback.
                                            (*pFnCallback)(pSceneObjectRef, storage);
                                            // Increase Objects Found Counter.
                                            objectsFound++;
                                        }
                                    }
                                }
                            }
                        }

                        // Move to next bin reference.
                        pBinChain = pBinChain->mpNextBinReference;
                    }
                }

                // Next Bin.
                subStartX = subEndX;
                subEndX   = getMin(subEndX + mBinSize, currentEndX);
            }

            // Reset Start.
            currentStartX = currentEndX;
        }
    }

    // Return Objects Found Count.
    return objectsFound;
}


//-----------------------------------------------------------------------------
// Find SceneObjects within Container Bin System at a specified point.
//
// NOTE:-   Unlike "pickLine", this is a container-level search only.
//-----------------------------------------------------------------------------
U32 t2dSceneContainer::pickPoint( const t2dVector& pickPoint, const U32 layerMask, const U32 groupMask, const bool showInvisible, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject, bool ignorePhysics )
{
    // Generate a rectangle of same dimensions as the point.
    RectF pointRect( pickPoint.mX, pickPoint.mY, 1, 1 );

    // Find Objects.
    return findObjects( pointRect, layerMask, groupMask, showInvisible, true, pFnCallback, storage, pIgnoreObject, ignorePhysics );
}


//-----------------------------------------------------------------------------
// Find SceneObjects within Container Bin System inside a specified area.
//
// NOTE:-   Unlike "pickLine", this is a container-level search only.
//-----------------------------------------------------------------------------
U32 t2dSceneContainer::pickArea( const RectF& pickArea, const U32 layerMask, const U32 groupMask, const bool showInvisible, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject, bool ignorePhysics )
{
    // Find Objects.
    return findObjects( pickArea, layerMask, groupMask, showInvisible, true, pFnCallback, storage, pIgnoreObject, ignorePhysics );
}


//-----------------------------------------------------------------------------
// Find SceneObjects within Container Bin System bounded by a rectangle.
//-----------------------------------------------------------------------------
U32 t2dSceneContainer::findObjects( const RectF& rectangle,const  U32 layerMask, const U32 graphGroupMask, const bool showInvisible, const bool picking, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject, bool ignorePhysics )
{
// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(t2dSceneContainer_findObjects);
#endif

    // Find which bins are covered.
    U32 minX, minY, maxX, maxY;
    getBinRectangle( rectangle, minX, minY, maxX, maxY );

    // Change the Current Sequence Key.
    nextSequenceKey();

    // Reset Object Found Counter.
    U32 objectsFound = 0;

    // *******************************************************************************
    // Fetch Standard Container Bins.
    // *******************************************************************************

    // Step through Bin ranges.
    for ( U32 y = minY; y <= maxY; y++ )
    {
        // Calculate Bin Position.
        U32 offsetY = (y % mBinCount) * mBinCount;

        for ( U32 x = minX; x <= maxX; x++ )
        {
            // Calculate Bin Position.
            U32 arrayIndex = offsetY + (x % mBinCount);

            // Fetch Bin Chain.
            t2dSceneBinReference* pBinChain = mSceneBinArray[arrayIndex].mpNextBinReference;

            // Increase Bin Searches.
            mpParentSceneGraph2D->getDebugStats().objectBinSearches++;

            // Step through Chain.
            while ( pBinChain )
            {
                // Fetch Scene Object Reference.
                t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

                // Is the Object Not being ignored, Enabled, Picking (Allowed/Used) and we've got Visibility?
                if ( pSceneObjectRef != pIgnoreObject && pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() && ((picking && pSceneObjectRef->getIsPickingAllowed())||!picking) && (pSceneObjectRef->getVisible() || showInvisible) )
                {
                    // Yes, so have we dealt with this object already?
                    if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
                    {
                        // No, so set the container sequence key to indicate so.
                        pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                        // Check if the Group/Layer masks match.
                        if (    pSceneObjectRef->getGraphGroupMask() & graphGroupMask &&
                                pSceneObjectRef->getLayerMask() & layerMask )
                        {
                            // Yes, so fetch Clip Rectangle.
                            RectF clipRectangle = rectangle;
                            RectF intersectRect = ignorePhysics ? pSceneObjectRef->getWorldClipRectangle() :
                                                                  pSceneObjectRef->getWorldCollisionClipRectangle();

                            // Do the collision clip rectangles intersect?

                            if ( clipRectangle.intersect( intersectRect ) )
                            {
                                // Yes, so perform callback.
                                (*pFnCallback)(pSceneObjectRef, storage);

                                // Increase Objects Found Counter.
                                objectsFound++;
                            }
                        }
                    }
                }

                // Move to next bin reference.
                pBinChain = pBinChain->mpNextBinReference;
            }
        }
    }


    // *******************************************************************************
    // Fetch Overflow Bin Chain.
    // *******************************************************************************
    t2dSceneBinReference* pBinChain = mSceneOverflowBin.mpNextBinReference;

    // Step through Chain.
    while ( pBinChain )
    {
        // Fetch Scene Object Reference.
        t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

        // Increase Bin Searches.
        mpParentSceneGraph2D->getDebugStats().objectBinSearches++;

        // Is the Object Not being ignored, Enabled, Picking Allowed and we've got Visibility?
        if ( pSceneObjectRef != pIgnoreObject && pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() && ((picking && pSceneObjectRef->getIsPickingAllowed())||!picking) && (pSceneObjectRef->getVisible() || showInvisible) )
        {
            // Yes, so have we dealt with this object already?
            if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
            {
                // No, so set the container sequence key to indicate so.
                pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                // Check if the Group/Layer masks match.
                if (    pSceneObjectRef->getGraphGroupMask() & graphGroupMask &&
                    pSceneObjectRef->getLayerMask() & layerMask )
                {
                    // Yes, so fetch Clip Rectangle.
                    RectF clipRectangle = rectangle;
                    RectF intersectRect = ignorePhysics ? pSceneObjectRef->getWorldClipRectangle() :
                                                          pSceneObjectRef->getWorldCollisionClipRectangle();

                    // Do the collision clip rectangles intersect?
                    if ( clipRectangle.intersect( intersectRect ) )
                    {
                        // Yes, so perform callback.
                        (*pFnCallback)(pSceneObjectRef, storage);

                        // Increase Objects Found Counter.
                        objectsFound++;
                    }
                }
            }
        }

        // Move to next bin reference.
        pBinChain = pBinChain->mpNextBinReference;
    }


    // *******************************************************************************
    // Check Always Scope Objects.
    // *******************************************************************************
    for ( U32 n = 0; n < mSceneAlwaysScope.size(); n++ )
    {
        // Fetch Scene Object Reference.
        t2dSceneObject* pSceneObjectRef = mSceneAlwaysScope[n];

        // Increase Bin Searches.
        mpParentSceneGraph2D->getDebugStats().objectBinSearches++;

        // Is the Object Not being ignored, Enabled, Picking (Allowed/Used) and we've got Visibility?
        if ( pSceneObjectRef != pIgnoreObject && pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() && ((picking && pSceneObjectRef->getIsPickingAllowed())||!picking) && (pSceneObjectRef->getVisible() || showInvisible) )
        {
            // Yes, so have we dealt with this object already?
            if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
            {
                // No, so set the container sequence key to indicate so.
                pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                // Check if the Group/Layer masks match.
                if (    pSceneObjectRef->getGraphGroupMask() & graphGroupMask &&
                        pSceneObjectRef->getLayerMask() & layerMask )
                {
                    // Is the object Always Scoped?
                    if ( pSceneObjectRef->getIsAlwaysScope() )
                    {
                        // Yes, so always perform callback!
                        (*pFnCallback)(pSceneObjectRef, storage);

                        // Increase Objects Found Counter.
                        objectsFound++;
                    }
                    else
                    {
                        // No, so fetch Clip Rectangle.
                        RectF clipRectangle = rectangle;
                        RectF intersectRect = ignorePhysics ? pSceneObjectRef->getWorldClipRectangle() :
                                                              pSceneObjectRef->getWorldCollisionClipRectangle();

                        // Do the collision clip rectangles intersect?
                        if ( clipRectangle.intersect( intersectRect ) )
                        {
                            // Yes, so perform callback.
                            (*pFnCallback)(pSceneObjectRef, storage);

                            // Increase Objects Found Counter.
                            objectsFound++;
                        }
                    }
                }
            }
        }
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // t2dSceneContainer_findObjects
#endif

    // Return Objects Found Count.
    return objectsFound;
}


//-----------------------------------------------------------------------------------
// Find Potential Collision(s) for specified SceneObject within Container Bin System.
//-----------------------------------------------------------------------------------
U32 t2dSceneContainer::findPotentialCollisions( t2dSceneObject* pSceneObject2D, const F32 elapsedTime, t2dFindCallback pFnCallback, void* storage, RectF& srcRectangleSearched )
{
    // Check everything is fine!
    AssertFatal( pSceneObject2D != NULL, "t2dSceneContainer::findPotentialCollisions() - Invalid Object" );

    // Finished if either send-collision/object are disabled.
    if ( !pSceneObject2D->getCollisionActiveSend() || !pSceneObject2D->isEnabled() )
        return 0;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(t2dSceneContainer_findPotentialCollisions);
#endif

    // Fetch World Collision Clip Rectangle.
    srcRectangleSearched = pSceneObject2D->getWorldCollisionClipRectangle();

    // Expand Search Rectangle to include area covered by current velocity over elapsed time plus the object size.
    t2dVector objVelocity;

    // Are we using forward-only movement?
    if ( pSceneObject2D->getForwardMovementOnly() )
    {
        // Yes, so calculate forward vector.
        const t2dVector forwardVec(0.0f, -pSceneObject2D->getLinearVelocity().len() * elapsedTime);
        // Orientate the velocity to the new vector.
        objVelocity = forwardVec * pSceneObject2D->getRotationMatrix();
    }
    else
    {
        // No, so get integratedc velocity.
        objVelocity = pSceneObject2D->getLinearVelocity() * elapsedTime;
    }

    // Deal with X-Axii.
    if ( mLessThanZero(objVelocity.mX) )
    {
        // Extend Left.
        srcRectangleSearched.point.x += objVelocity.mX;
        srcRectangleSearched.extent.x -= objVelocity.mX;
    }
    else
    {
        // Extend Right.
        srcRectangleSearched.extent.x += objVelocity.mX;
    }

    // Deal with Y-Axii.
    if ( mLessThanZero(objVelocity.mY) )
    {
        // Extend Up.
        srcRectangleSearched.point.y += objVelocity.mY;
        srcRectangleSearched.extent.y -= objVelocity.mY;
    }
    else
    {
        // Extend Down.
        srcRectangleSearched.extent.y += objVelocity.mY;
    }

    // Find which bins we need to search.
    U32 minX, minY, maxX, maxY;
    getBinRectangle( srcRectangleSearched, minX, minY, maxX, maxY );

    // Fetch Source Collision Group/Layer Masks.
    U32 srcCollisionGroupMask = pSceneObject2D->getCollisionGroupMask();
    U32 srcCollisionLayerMask = pSceneObject2D->getCollisionLayerMask();

    // Change the Current Sequence Key.
    nextSequenceKey();

    // Reset Object Found Counter.
    U32 objectsFound = 0;

    // Step through Bin ranges.
    for ( U32 y = minY; y <= maxY; y++ )
    {
        // Calculate Bin Position.
        U32 offsetY = (y % mBinCount) * mBinCount;

        for ( U32 x = minX; x <= maxX; x++ )
        {
            // Calculate Bin Position.
            U32 arrayIndex = offsetY + (x % mBinCount);

            // Fetch Bin Chain.
            t2dSceneBinReference* pBinChain = mSceneBinArray[arrayIndex].mpNextBinReference;

            // Increase Bin Collisions.
            mpParentSceneGraph2D->getDebugStats().objectBinCollisions++;

            // Step through Chain.
            while ( pBinChain )
            {
                // Fetch Scene Object Reference.
                t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

                // Make sure we're not colliding with ourself!
                if ( pSceneObjectRef != pSceneObject2D )
                {
                    // Is the Object Enabled?
                    if ( pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() )
                    {
                        // Yes, so have we dealt with this object already?
                        if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
                        {
                            // No, so set the container sequence key to indicate so.
                            pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                            // Check if there's a potential collision.
                            if (    pSceneObjectRef->getGraphGroupMask() & srcCollisionGroupMask &&
                                pSceneObjectRef->getLayerMask() & srcCollisionLayerMask )
                            {
                                // Fetch Clip Rectangle.
                                RectF clipRectangle = srcRectangleSearched;

                                // Do the collision clip rectangles intersect?
                                if ( clipRectangle.intersect( pSceneObjectRef->getWorldCollisionClipRectangle() ) )
                                {
                                    // Yes, so perform callback.
                                    (*pFnCallback)(pSceneObjectRef, storage);

                                    // Increase Objects Found Counter.
                                    objectsFound++;
                                }
                            }
                        }
                    }
                }

                // Move to next bin reference.
                pBinChain = pBinChain->mpNextBinReference;
            }
        }
    }

    // Fetch Overflow Bin Chain.
    t2dSceneBinReference* pBinChain = mSceneOverflowBin.mpNextBinReference;

    // Step through Chain.
    while ( pBinChain )
    {
        // Fetch Scene Object Reference.
        t2dSceneObject* pSceneObjectRef = pBinChain->mpSceneObject2D;

        // Increase Bin Collisions.
        mpParentSceneGraph2D->getDebugStats().objectBinCollisions++;

        // Make sure we're not colliding with ourself!
        if ( pSceneObjectRef != pSceneObject2D )
        {
            // Is the Object Enabled?
            if ( pSceneObjectRef->isEnabled() && !pSceneObjectRef->isBeingDeleted() )
            {
                // Yes, so have we dealt with this object already?
                if ( pSceneObjectRef->getContainerSequenceKey() != getCurrentSequenceKey() )
                {
                    // No, so set the container sequence key to indicate so.
                    pSceneObjectRef->setContainerSequenceKey( getCurrentSequenceKey() );

                    // Check if there's a potential collision.
                    if (    pSceneObjectRef->getGraphGroupMask() & srcCollisionGroupMask &&
                        pSceneObjectRef->getLayerMask() & srcCollisionLayerMask )
                    {
                        // Fetch Clip Rectangle.
                        RectF clipRectangle = srcRectangleSearched;

                        // Do the collision clip rectangles intersect?
                        if ( clipRectangle.intersect( pSceneObjectRef->getWorldCollisionClipRectangle() ) )
                        {
                            // Yes, so perform callback.
                            (*pFnCallback)(pSceneObjectRef, storage);

                            // Increase Objects Found Counter.
                            objectsFound++;
                        }
                    }
                }
            }
        }

        // Move to next bin reference.
        pBinChain = pBinChain->mpNextBinReference;
    }

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // t2dSceneContainer_findPotentialCollisions
#endif

    // Return Objects Found Count.
    return objectsFound;
}



