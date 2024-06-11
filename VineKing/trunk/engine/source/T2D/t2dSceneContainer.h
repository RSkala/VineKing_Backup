//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Container.
//-----------------------------------------------------------------------------

#ifndef _T2DSCENECONTAINER_H_
#define _T2DSCENECONTAINER_H_

#ifndef _T2DVECTOR_H_
#include "t2dVector.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _MRECT_H_
#include "math/mRect.h"
#endif

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif


///-----------------------------------------------------------------------------
/// Forward Declarations.
///-----------------------------------------------------------------------------
class t2dSceneObject;


///-----------------------------------------------------------------------------
/// Scene Bin Object.
///-----------------------------------------------------------------------------
class t2dSceneBinReference
{
public:
   t2dSceneObject*          mpSceneObject2D;
   t2dSceneBinReference*    mpNextBinReference;
   t2dSceneBinReference*    mpPreviousBinReference;
   t2dSceneBinReference*    mpObjectLink;
};


/// This doesn't really belong here but I don't want to orphan it!
class CDebugStats
{
public:
    U32     objectsPotentialRender;
    U32     objectsActualRendered;
    F32     objectsHitPercRendered;
    U32     objectsLayerSorted;
    U32     objectsUpdated;
    U32     objectPotentialCollisions;
    U32     objectActualCollisions;
    F32     objectHitPercCollisions;
    U32     objectContactPairs;
    U32     objectParticlesAvailable;
    U32     objectParticlesActive;
    U32     objectBinRelocations;
    U32     objectBinSearches;              ///< Not Displayed Yet!
    U32     objectBinCollisions;

    void reset(void)
    {
        objectsPotentialRender = 0;
        objectsActualRendered = 0;
        objectsHitPercRendered = 0.0f;
        objectsLayerSorted = 0;
        objectsUpdated = 0;
        objectPotentialCollisions = 0;
        objectActualCollisions = 0;
        objectHitPercCollisions = 0.0f;
        objectContactPairs = 0;
        objectParticlesAvailable = 0;
        objectParticlesActive = 0;
        objectBinRelocations = 0;
        objectBinSearches = 0;
        objectBinCollisions = 0;
    };

    CDebugStats() { reset(); };
};

///-----------------------------------------------------------------------------
/// Scene Container Object.
///-----------------------------------------------------------------------------
class t2dSceneContainer 
{
    friend class t2dSceneGraph;

private:
    t2dSceneGraph*                  mpParentSceneGraph2D;
    F32                             mBinSize;
    U32                             mBinCount;
    F32                             mTotalBinSize;
    U32                             mSceneReferenceBlockSize;
    Vector<t2dSceneBinReference*>   mSceneBinReferenceBlocks;
    t2dSceneBinReference*           mFreeSceneBinReferences;
    t2dSceneBinReference*           mSceneBinArray;
    t2dSceneBinReference            mSceneOverflowBin;
    Vector<t2dSceneObject*>         mSceneAlwaysScope;
    Vector<t2dSceneObject*>         mSceneDisabledBin;
    t2dSceneBinReference            mAlwaysScopeDummy;      ///< Dummy used for Always-Scope Objects.
    U32                             mCurrentSequenceKey;

    void initialise( t2dSceneGraph* pT2DSceneGraph, F32 containerBinSize, U32 containerBinCount );
    void addSceneObject( t2dSceneObject* pSceneObject2D );
    void addSceneObjectDirect( t2dSceneObject* pSceneObject2D, U32 minX, U32 minY, U32 maxX, U32 maxY );
    void removeSceneObject( t2dSceneObject* pSceneObject2D );

    void createContainerBins( void );
    void destroyContainerBins( void );
    void addSceneReferenceBlock( void );
    inline t2dSceneBinReference* allocateSceneBinReference();
    inline void freeSceneBinReference(t2dSceneBinReference* pSceneBinRef );
    void getBinRectangle( const RectF& rectangle, U32& minBinX, U32& minBinY, U32& maxBinX, U32& maxBinY );
    void getBinRange( const F32 min, const F32 max, U32& minBin, U32& maxBin );
    void checkSceneObjectBins( t2dSceneObject* pSceneObject2D );

public:
    t2dSceneContainer();
    virtual ~t2dSceneContainer();

    typedef void (*t2dFindCallback)( t2dSceneObject*, void* storage );

    void destroyContainerSystem( void );    ///< Careful!!!

    /// Accessors.
    U32 getCurrentSequenceKey(void) const       { return mCurrentSequenceKey; };
    void nextSequenceKey( void )                { mCurrentSequenceKey++; };
    F32 getBinSize( void ) const                { return mBinSize; };
    U32 getBinCount( void ) const               { return mBinCount; };
    F32 getTotalBinSize( void ) const           { return mTotalBinSize; };

    /// Container Search/Picking.
    U32 pickPoint( const t2dVector& pickPoint, const U32 layerMask, const U32 groupMask, const bool showInvisible, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject, bool ignorePhysics = false );
    U32 pickArea( const RectF& pickArea, const U32 layerMask, const U32 groupMask, const bool showInvisible, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject, bool ignorePhysics = false );
    bool pickLine( const t2dVector& lineStart, const t2dVector& lineEnd, const U32 layerMask, const U32 groupMask, const bool showInvisible, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject );
    U32 findObjects( const RectF& rectangle, const U32 layerMask, const U32 groupMask, const bool showInvisible, const bool picking, t2dFindCallback pFnCallback, void* storage, const t2dSceneObject* pIgnoreObject, bool ignorePhysics = false );
    U32 findPotentialCollisions( t2dSceneObject* pSceneObject, const F32 elapsedTime, t2dFindCallback pFnCallback, void* storage, RectF& srcRectangleSearched );
};

#endif // _T2DSCENECONTAINER_H_
