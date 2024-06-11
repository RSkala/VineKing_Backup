//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Graph.
//-----------------------------------------------------------------------------

#ifndef _T2DSCENEGRAPH_H_
#define _T2DSCENEGRAPH_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _T2DSCENECONTAINER_H_
#include "./t2dSceneContainer.h"
#endif

#ifndef _T2DVECTOR_H_
#include "t2dVector.h"
#endif

#ifndef _T2DBASEDATABLOCK_H_
#include "./t2dBaseDatablock.h"
#endif

#ifndef _FILESTREAM_H_
#include "core/fileStream.h"
#endif

#ifndef _NETOBJECT_H_
#include "sim/netObject.h"
#endif

#ifndef _T2DSERIALISATION_H_
#include "./t2dSerialise.h"
#endif

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

#ifndef _T2DSCENEOBJECTGROUP_H_
#include "./t2dSceneObjectGroup.h"
#endif
///-----------------------------------------------------------------------------
/// Forward Declarations.
///-----------------------------------------------------------------------------
class t2dSceneObject;
class t2dTileMap;
class t2dSceneWindow;


///-----------------------------------------------------------------------------
/// Externs.
///-----------------------------------------------------------------------------


///-----------------------------------------------------------------------------
/// Structures.
///-----------------------------------------------------------------------------
struct tDeleteRequest
{
    SimObjectId     mObjectId;
    t2dSceneObject* mpSceneObject;
    bool            mSafeDeleteReady;
};


///-----------------------------------------------------------------------------
/// Types.
///-----------------------------------------------------------------------------
typedef Vector<t2dSceneObject*> typeSceneObjectVector;
typedef const Vector<t2dSceneObject*>& typeSceneObjectVectorConstRef;
typedef Vector<tDeleteRequest> typeDeleteVector;


///-----------------------------------------------------------------------------
/// Object Find Callbacks.
///-----------------------------------------------------------------------------
void findObjectsCallback(t2dSceneObject* pSceneObject, void* storage);
void findLayeredObjectsCallback(t2dSceneObject* pSceneObject, void* storage);


///-----------------------------------------------------------------------------
/// SceneGraph Datablock 2D.
///-----------------------------------------------------------------------------
class t2dSceneGraphDatablock : public t2dBaseDatablock
{
public:
    typedef t2dBaseDatablock Parent;


    t2dSceneGraphDatablock();
    virtual ~t2dSceneGraphDatablock();

    static void  initPersistFields();
    virtual bool onAdd();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    F32             mContainerBinSize;
    U32             mContainerBinCount;
    bool            mUseLayerSorting;
    bool            mLastInFrontSorting;

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dSceneGraphDatablock);
};

class t2dSceneObjectGroup;

///-----------------------------------------------------------------------------
/// SceneGraph Object.
///-----------------------------------------------------------------------------
class t2dSceneGraph : public SimSet, public virtual ITickable
{
private:
    typedef SimSet Parent;

    bool                        mSceneGraphInitialised;
    U32                         mSceneObjectCount;
    F32                         mSceneTime;
    bool                        mScenePause;
    F32                         mScenePhysicsLimitFPS;
    F32                         mScenePhysicsTargetFPS;
    U32                         mScenePhysicsMaxIterations;
    bool                        mScenePhysicsFPSActive;
    t2dVector                   mConstantForce;
    bool                        mConstantForceActive;                   ///< Flags if Constant Force is Used.
    bool                        mGraviticConstantForce;
    F32                         mMinFPS;
    F32                         mMaxFPS;
    U32                         mDebugMask;
    t2dSceneContainer           mSceneContainer;
    U32                         mMaxObjectBinRelocations;
    U32                         mMaxObjectBinCollisions;

    t2dSceneObject*             mpProcessHead;
    U32                         mUpdateSequenceKey;

    t2dTileMap*                 mGlobalTileMap;

    S32                         mLayerOrder;
    bool                        mUseLayerSorting;
    bool                        mLastInFrontSorting;

    U32                         mSceneSerialiseID;
    bool                        mSerialiseMode;

    t2dSceneWindow*             mpCurrentRenderWindow;
    SimSet                      mAttachedSceneWindows;

    SimSet                      mMouseLockedObjectSet;

    S32                         mIsEditorScene;
    /// Reset Debug Stats.
    CDebugStats                 mDebugStats;

	//-Mat don't call the update function unless we specify
	bool mUseUpdateCallback;

    class t2dSerialiseSection
    {
        bool* pSerialiseCriticalSection;
    public:
        t2dSerialiseSection( bool* pSection ) : pSerialiseCriticalSection(pSection) { AssertFatal( pSerialiseCriticalSection, "t2dSerialiseSection() - Incorrect Usage!" ); *pSerialiseCriticalSection = true; };
        ~t2dSerialiseSection() { *pSerialiseCriticalSection = false; };
    };

public:
    t2dSceneGraphDatablock* mConfigDataBlock;


    /// Maximum Layers Supported.
    enum
    {
        maxLayersSupported = 32,        ///< Wow; don't even be tempted to change this!
    };

    /// Scene-Graph Debug Bits.
    enum
    {
        T2D_SCENEGRAPH_DEBUG_STATISTICS         = BIT(0),
        T2D_SCENEGRAPH_DEBUG_BOUNDING_BOXES     = BIT(1),
        T2D_SCENEGRAPH_DEBUG_MOUNT_NODES        = BIT(2),
        T2D_SCENEGRAPH_DEBUG_MOUNT_LINK         = BIT(3),
        T2D_SCENEGRAPH_DEBUG_WORLD_LIMITS       = BIT(4),
        T2D_SCENEGRAPH_DEBUG_COLLISION_POLYS    = BIT(5),
        T2D_SCENEGRAPH_DEBUG_COLLISION_HISTORY  = BIT(6),
        T2D_SCENEGRAPH_DEBUG_SORT_POINTS        = BIT(7),
    };

    /// Scene/Layer Draw-Order Control.
    enum eDrawOrderCommand
    {
        T2D_ORDER_FRONT,
        T2D_ORDER_BACK,
        T2D_ORDER_FORWARD,
        T2D_ORDER_BACKWARD,
        T2D_ORDER_INVALID
    };

    // Scene-Object Vectors.
    typeDeleteVector                    mDeleteRequests;                                        ///< Delete Requests.
    typeDeleteVector                    mDeleteRequestsTemp;                                    ///< Delete Requests.
    typeSceneObjectVector               mLinearPickList;                                        ///< Non-Layered Pick-List.
    typeSceneObjectVector               mLayeredPickList[t2dSceneGraph::maxLayersSupported];    ///< Layered Pick-List.
    typeSceneObjectVector               mLayeredRenderList[t2dSceneGraph::maxLayersSupported];  ///< Layered Render-List.


    /// Scene-Render Sort Function Pointer.
    S32 (QSORT_CALLBACK *mRenderSortFn)(const void *, const void *);

    /// Sort Routines.
    static S32 QSORT_CALLBACK layeredRenderSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK layeredYSortPointSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK layeredXSortPointSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK layeredNegYSortPointSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK layeredNegXSortPointSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK layeredPickListSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK linearPickListSortKeyCollisionTime(const void* a, const void* b);

    enum eDrawOrderSort
    {
        T2D_SORT_NORMAL,
        T2D_SORT_YAXIS,
        T2D_SORT_XAXIS,
        T2D_SORT_NEGYAXIS,
        T2D_SORT_NEGXAXIS
    };

    eDrawOrderSort mLayerSortModes[t2dSceneGraph::maxLayersSupported];

    bool getIsEditorScene() { return ((mIsEditorScene > 0) ? true : false); }
    void setIsEditorScene(bool bIsEdit) { mIsEditorScene += (bIsEdit ? 1 : -1); }

    t2dSceneGraph();
    virtual ~t2dSceneGraph();

    static void initPersistFields();

    virtual bool onAdd();
    virtual void onRemove();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// RKS: I added these TEMPORARILY to figure out how to get a PROPER scene graph
	// ALSO: I want to figure out if the t2dSceneGraph objects in the level files are actually being deleted!!!!
	//bool registerObject();
	//bool registerObject(U32 id);
	//bool registerObject(const char *name);
	//bool registerObject(const char *name, U32 id);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline t2dSceneContainer& getSceneContainer( void )         { return mSceneContainer; };
    inline t2dSceneObject* getProcessHead( void ) const         { return mpProcessHead; };
    inline F32      getSceneTime( void ) const                  { return mSceneTime; };
    inline bool     getScenePause( void ) const                 { return mScenePause; };
    inline F32      getMinFPS( void ) const                     { return mMinFPS; };
    inline F32      getMaxFPS( void ) const                     { return mMaxFPS; };
    inline U32      getMaxBinRelocations( void ) const          { return mMaxObjectBinRelocations; };
    inline U32      getMaxBinCollisions( void ) const           { return mMaxObjectBinCollisions; };
    inline U32      getSceneObjectCount( void ) const           { return mSceneObjectCount; };
    t2dSceneObject* getSceneObject( const U32 objectIndex );
    U32             getSceneObjectList( Vector<t2dSceneObject*> &objectList );
    inline t2dVector getConstantForce( void ) const             { return mConstantForce; };
    inline bool     getIsConstantForceActive( void ) const      { return mConstantForceActive; };
    inline bool     getGraviticConstantForceFlag( void ) const  { return mGraviticConstantForce; };

    inline U32      getChildCount( void );
    inline U32      getDebugMask( void ) const                  { return mDebugMask; };
    inline bool     getUseLayeredSorting( void ) const          { return mUseLayerSorting; };
    inline bool     isInitialised( void ) const                 { return mSceneGraphInitialised; };
    inline U32      getNextSerialiseID( void )                  { return ++mSceneSerialiseID; };
    inline U32      getCurrentSerialiseID( void )               { return mSceneSerialiseID; };

    CDebugStats&    getDebugStats( void )                       { return mDebugStats; };
    void            resetDebugStats( void )                     { mDebugStats.reset(); };

    void            setGlobalTileMap(t2dTileMap* tileMap)       { mGlobalTileMap = tileMap; };
    t2dTileMap*     getGlobalTileMap() const                    { return mGlobalTileMap; };

    t2dSceneWindow* getCurrentRenderWindow( void )              { return mpCurrentRenderWindow; };
    void setCurrentRenderWindow( t2dSceneWindow* pSceneWindow ) { mpCurrentRenderWindow = pSceneWindow; };
    
    void    setScenePause( bool status )                        { mScenePause = status; };
    void    setConstantForce( t2dVector force, bool gravitic );

    /// Delete Requests.
    void    addDeleteRequest( t2dSceneObject* pSceneObject2D );
    void    processDeleteRequests( const bool forceImmediate );

    /// Scene Window.
    void    attachSceneWindow( t2dSceneWindow* pSceneWindow2D );
    void    detachSceneWindow( t2dSceneWindow* pSceneWindow2D );
    void    detachAllSceneWindows( void );
    bool    isSceneWindowAttached( t2dSceneWindow* pSceneWindow2D );

    const SimSet& getMouseLockedObjectSet() { return mMouseLockedObjectSet; };
    void setObjectMouseLock(t2dSceneObject* object) { mMouseLockedObjectSet.addObject((SimObject*)object); };
    void clearObjectMouseLock(t2dSceneObject* object) { mMouseLockedObjectSet.removeObject((SimObject*)object); };

    /// Enable/Disable.
    void    setEnableDisableStatus( t2dSceneObject* pSceneObject2D );

    /// Update Sequencing.
    inline U32  getUpdateSequenceKey( void ) const              { return mUpdateSequenceKey; };
    inline void setUpdateSequenceKey( U32 seqKey )              { mUpdateSequenceKey = seqKey; };
    inline void nextUpdateSequence( void )                      { mUpdateSequenceKey++; };

    /// Set Render-Sort Function.
    void setRenderSortFunction( S32 (QSORT_CALLBACK *sortFn)(const void *, const void *) ) { mRenderSortFn = sortFn; };

    /// Debug Stats/Mask.
    void    setDebugOn( U32 debugMask );
    bool    getDebugOn( U32 debugMask );
    void    setDebugOff( U32 debugMask );
    void    calculateDebugStats( void );

    ///  Container System.
    void    checkSceneObjectBins( t2dSceneObject* pSceneObject2D );

    /// Picking.
    void clearPickList( void );
    typeSceneObjectVectorConstRef getPickList( void ) const      { return mLinearPickList; };
    U32 pickPoint( const t2dVector& pickPoint, const U32 groupMask = T2D_MASK_ALL, const U32 layerMask = T2D_MASK_ALL, const bool showInvisible = false, const t2dSceneObject* pIgnoreObject = NULL, bool ignorePhysics = false );
    U32 pickLine( const t2dVector& startPickPoint, const t2dVector& endPickPoint, const U32 groupMask = T2D_MASK_ALL, const U32 layerMask = T2D_MASK_ALL, const bool showInvisible = false, const t2dSceneObject* pIgnoreObject = NULL );
    U32 pickRect( const t2dVector& startPickPoint, const t2dVector& endPickPoint, const U32 groupMask = T2D_MASK_ALL, const U32 layerMask = T2D_MASK_ALL, const bool showInvisible = false, const t2dSceneObject* pIgnoreObject = NULL, bool ignorePhysics = false );
    U32 pickRadius( const t2dVector& pickPoint, const F32 radius, const U32 groupMask = T2D_MASK_ALL, const U32 layerMask = T2D_MASK_ALL, const bool showInvisible = false, const t2dSceneObject* pIgnoreObject = NULL );

    bool    findChildObject( SimObject* searchObject );

    /// Scene Management.
    void    addToScene( t2dSceneObject* pSceneObject2D, bool triggerCallback = true);
    void    removeFromScene( t2dSceneObject* pSceneObject2D );
    void    addToScene( t2dSceneObjectGroup* pSceneObjectGroup );
    void    removeFromScene( t2dSceneObjectGroup* pSceneObjectGroup );
    void    addToScene( SimObject* object );
    void    removeFromScene( SimObject* object );
    void    addObject( SimObject* object );
    void    removeObject( SimObject* object );
    void    clearScene( bool deleteObjects = true );

    bool    setLayerDrawOrder( t2dSceneObject* pSceneObject2D, eDrawOrderCommand command );
    bool    setSceneDrawOrder( t2dSceneObject* pSceneObject2D, eDrawOrderCommand command );

    bool    loadScene( const char* sceneFile );
    bool    saveScene( const char* sceneFile );
    virtual void write(Stream &stream, U32 tabStop, U32 flags = 0);
    void    performPostInit();

    /// ITickable.
    virtual void interpolateTick( F32 delta );
    virtual void processTick();
    virtual void advanceTime( F32 timeDelta ) {};

    void    addToProcessList( t2dSceneObject* pSceneObject2D );
    void    removeFromProcessList( t2dSceneObject* pSceneObject2D );
    void    defaultInitialise( void );
    void    initialise( const F32 containerBinSize, const U32 containerBinCount, const bool useLayerSorting, const bool lastInFrontSorting );

    void    renderView( const RectF viewWindow, const U32 layerMask, const U32 groupMask, CDebugStats* pDebugStats );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dSceneGraph );

    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dSceneGraph, 2 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dSceneGraph, 3 );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dSceneGraph);
};

#endif // _T2DSCENEGRAPH_H_
