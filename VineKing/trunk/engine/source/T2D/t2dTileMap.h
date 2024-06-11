//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Tilemap Object.
//-----------------------------------------------------------------------------

#ifndef _T2DTILEMAP_H_
#define _T2DTILEMAP_H_

#ifndef _T2DACTIVETILE_H_
#include "./t2dActiveTile.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif

#ifndef _T2DIMAGEMAPDATABLOCK_H_
#include "./t2dImageMapDatablock.h"
#endif

#ifndef _T2DANIMATIONCONTROLLER_H_
#include "./t2dAnimationController.h"
#endif

#ifndef _GBITMAP_H_
#include "dgl/gBitmap.h"
#endif


///-----------------------------------------------------------------------------
/// Forward Declarations.
///-----------------------------------------------------------------------------
class t2dTileLayer;

///-----------------------------------------------------------------------------
/// Structures.
///-----------------------------------------------------------------------------
struct tLayerHandle
{
   SimObjectId     mObjectId;
   SimObject*      mpSceneObject;
};

///-----------------------------------------------------------------------------
/// Types.
///-----------------------------------------------------------------------------
typedef Vector<tLayerHandle> typeLayerVector;

///-----------------------------------------------------------------------------
/// TileMap Object 2D.
///-----------------------------------------------------------------------------
class t2dTileMap : public t2dSceneObject
{
    typedef t2dSceneObject      Parent;

public:

    /// Tile Types.
    enum eTileType
    {
        STATIC_TILE,
        ANIMATED_TILE,
        ACTIVE_TILE,

        INVALID_TILE
    };

    /// ***************************************************************
    /// Root TileNode.
    /// ***************************************************************
    class cRootTileNode
    {
    public:
        eTileType           mTileType;          /// Tile Type.
        U32                 mReferenceCount;    /// Reference Count.
        U32                 mUpdateSequenceKey; /// Update Sequencing.

        /// Initialse.
        cRootTileNode() : mReferenceCount(0), mUpdateSequenceKey(0) {};
        virtual ~cRootTileNode() {};
    };

    /// ***************************************************************
    /// Static TileNode.
    /// ***************************************************************
    class cStaticTileNode : public cRootTileNode
    {
    public:
        t2dImageMapSmartPtr     pImageMapDataBlock2D;
        U32                     mFrame;

        /// Initialse.
        cStaticTileNode() : pImageMapDataBlock2D(NULL), mFrame(0) { mTileType = STATIC_TILE; };
    };

    /// ***************************************************************
    /// Animation TileNode.
    /// ***************************************************************
    class cAnimationTileNode : public cRootTileNode
    {
    public:
        t2dAnimationController* pAnimationController2D;

        /// Initialse.
        cAnimationTileNode() : pAnimationController2D(NULL) { mTileType = ANIMATED_TILE; };
    };

    /// ***************************************************************
    /// Active TileNode.
    /// ***************************************************************
    class cActiveTileNode : public cRootTileNode
    {
    public:
        SimObjectPtr<t2dActiveTile>          pActiveTile2D;
    
        /// Initialse.
        cActiveTileNode() : pActiveTile2D(NULL) { mTileType = ACTIVE_TILE; };
    };

    // Tile Database.
    Vector<t2dTileLayer*>               mTileLayerList;
    Vector<cRootTileNode*>              mTileNodes;
    Vector<t2dAnimationController*>     mAnimationControllerList;
    Vector<t2dActiveTile*>              mActiveTileList;


    t2dTileMap();
    virtual ~t2dTileMap();

    /// Tile/Node Searchs.
    t2dTileLayer*       findTileLayer( U32 tileLayerID ) const;
    S32                 findTileLayerIndex( t2dTileLayer* pTileLayer ) const;
    S32                 findRootTileNodeIndex( cRootTileNode* pRootTileNode ) const;
    S32                 findAnimationControllerIndex( t2dAnimationController* pAnimationController ) const;
    S32                 findActiveTileIndex( t2dActiveTile* pActiveTile2D ) const;
    cStaticTileNode*    findStaticTileNode( t2dImageMapDatablock* pImageMapDataBlock, U32 frame ) const;
    cAnimationTileNode* findAnimationTileNode( t2dAnimationDatablock* pAnimationDataBlock ) const;

    /// TileNode Generation/Destruction.
    cStaticTileNode* createStaticTileNode( void );
    cAnimationTileNode* createAnimationTileNode( const char* animationName );
    cActiveTileNode* createActiveTileNode( const char* activeTileClassName, const char* activeTileDataBlockName );
    void deleteTileNode( cRootTileNode* pRootTileNode );
    virtual void onDeleteNotify(SimObject* object);


    /// TileMap Layers.
    SimObjectId createTileLayer( U32 tileCountX, U32 tileCountY, F32 tileSizeX, F32 tileSizeY );
    void addTileLayer(t2dTileLayer* pTileLayer);
    void removeTileLayer(t2dTileLayer* pTileLayer);
    void deleteTileLayer( U32 layerIndex );
    void deleteTileLayerId( SimObjectId tileLayerID ); 
    void deleteAllTileLayers(void);
    U32  getTileLayerCount(void);
    SimObjectId getTileLayer( U32 layerIndex );
    void setAllLayersToParent( void );

    /// Load/Save TileMaps.
    bool loadTileMapDataBlock( const char* tileMapDataBlockName );
    bool loadTileMap( const char* tilemapFile );
    bool saveTileMap( const char* tilemapFile );

    virtual bool onAdd();
    virtual void onRemove();
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dTileMap );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dTileMap, 2 );

    /// Declare Console Object.
    DECLARE_CONOBJECT( t2dTileMap );
};


///-----------------------------------------------------------------------------
/// TileLayer Object 2D.
///-----------------------------------------------------------------------------
class t2dTileLayer : public t2dSceneObject
{
    typedef t2dSceneObject      Parent;

    StringTableEntry mLayerFile;

public:
    struct tTileObject
    {
        U32                         mUpdateSequenceKey; ///< Update Sequence Key.
        t2dTileMap::cRootTileNode*  mpTileNode;         ///< Tile Node.
        t2dPhysics*                 mpPhysics;          ///< Physics.
        bool                        mReceiveCollisions; ///< Receive Collision Flag.
        bool                        mFlipHorizontal;    ///< Flip Horizontal Flag.
        bool                        mFlipVertical;      ///< Flip Vertical Flag.
        bool                        mScriptActioned;    ///< Script Actioned?
        StringTableEntry            mShowScript;        ///< Show-Script String.
        StringTableEntry            mCustomData;        ///< Custom Data.
    };

    /// Dimensions and Storage.
    U32                             mTileCountX;
    U32                             mTileCountY;
    F32                             mTileSizeX;
    F32                             mTileSizeY;
    tTileObject**                   mppTileObjectArray;
    t2dTileMap*                     mpTileMap2DManager;

    /// Dynamic Properties.
    bool                            mWrapX;
    bool                            mWrapY;
    F32                             mPanPositionX;
    F32                             mPanPositionY;
    F32                             mAutoPanX;
    F32                             mAutoPanY;

    /// Tick Properties.
    t2dVector                       mPreTickeOffset;
    t2dVector                       mPostTickOffset;
    t2dVector                       mRenderTickOffset;

    // Debug Properties.
    bool                            mGridActive;
    bool                            mScriptIconActive;
    bool                            mCustomIconActive;
    bool                            mCursorIconActive;
    F32                             mCursorBlinkTime;
    F32                             mCurrentCursorBlinkTime;
    bool                            mCurrentCursorBlinkState;
    U32                             mCurrentCursorX;
    U32                             mCurrentCursorY;
    t2dImageMapSmartPtr             mTileIconImageMapDataBlock;
    Vector<t2dPhysics*>             mDebugRenderPhysics;




    t2dTileLayer(t2dTileMap* pTileMap2D);
    t2dTileLayer();
    ~t2dTileLayer();

    static void initPersistFields();
    static bool setLayerFile(void* obj, const char* data);

    /// Layer/Tile Creation/Destruction.
    void createLayer( const U32 tileCountX, const U32 tileCountY, const F32 tileSizeX, const F32 tileSizeY );
    void deleteLayer( void );
    void resizeLayer( const U32 newTileCountX, const U32 newTileCountY );
    bool getTileObject( const U32 tileX, const U32 tileY, tTileObject** pTileObject ) const;
    void createTileObject( const U32 tileX, const U32 tileY, t2dTileMap::cRootTileNode* pRootTileNode );
    void clearTile( const U32 tileX, const U32 tile );
    void updateTileSizes( void );

    /// Accessors.
    F32 getTileSizeX( void ) const { return mTileSizeX; };
    F32 getTileSizeY( void ) const { return mTileSizeY; };
    U32 getTileCountX( void ) const { return mTileCountX; };
    U32 getTileCountY( void ) const { return mTileCountY; };
    bool getTileScriptActioned( const U32 tileX, const U32 tileY );
    const char* getTileScript( const U32 tileX, const U32 tileY );
    const char* getTileCustomData( const U32 tileX, const U32 tileY );
    bool getTileFlip( const U32 tileX, const U32 tileY, bool& flipX, bool& flipY ) const;
    t2dTileMap::eTileType getTileType( const U32 tileX, const U32 tileY ) const;
    
    /// Tile-Layer Admin.
    void clearLayer( void );
    void resetLayer( void );
    void deleteTile( const U32 tileX, const U32 tileY );
    bool setStaticTile( const U32 tileX, const U32 tileY, const char* imageMapName, const U32 frame );
    bool setAnimatedTile( const U32 tileX, const U32 tileY, const char* animationName, const bool forceUnique );
    S32 setActiveTile( const U32 tileX, const U32 tileY, const char* activeTileClassName, const char* activeTileDatablockName );
    bool setTileScript( const U32 tileX, const U32 tileY, const char* script );
    bool setTileCustomData( const U32 tileX, const U32 tileY, const char* customData );
    bool setTileFlip( const U32 tileX, const U32 tileY, bool flipHorz, bool flipVert );

    void setWrap( const bool wrapX, const bool wrapY );
    bool getWrapX() { return mWrapX; };
    bool getWrapY() { return mWrapY; };

    virtual void setFlip( const bool flipX, bool flipY ) { }; // Can't warn here because this is called when layers are loaded.
    virtual void setFlipX( const bool flipX ) { };
    virtual void setFlipY( bool flipY ) { };

    void setPanPosition( const F32 panX, const F32 panY );
    F32  getPanPositionX() { return mPanPositionX; };
    F32  getPanPositionY() { return mPanPositionY; };

    void setAutoPan( const F32 autoPanX, const F32 autoPanY );
    void setAutoPanPolar( const F32 angle, const F32 panSpeed );
    F32  getAutoPanX() { return mAutoPanX; };
    F32  getAutoPanY() { return mAutoPanY; };

    void setTileSize( const F32 tileSizeX, const F32 tileSizeY );

    bool pickTile( const t2dVector& worldPosition, Point2I& logicalTile );

    /// Debugging.
    void setGridActive( const bool status );
    void setCustomIconActive( const bool status );
    void setScriptIconActive( const bool status );
    void setCursorIconActive( const bool status, const F32 blinkTime = 0.25f );
    void setTileIcons( const char* imageMapName );
    void setCursorPosition( const U32 tileX, const U32 tileY );
    
    /// Collisions.
    bool setTileCollisionActive( const U32 tileX, const U32 tileY, const bool receiveCollisionActive );
    bool setTileCollisionPolyPrimitive( const U32 tileX, const U32 tileY, const U32 polyVertexCount );
    bool setTileCollisionPolyCustom( const U32 tileX, const U32 tileY, const U32 polyVertexCount, const t2dVector* pCustomPolygon );
    bool setTileCollisionPolyCustomGlobal( const U32 tileX, const U32 tileY, const U32 polyVertexCount, const t2dVector *pCustomPolygon );
    bool setTileCollisionScale( const U32 tileX, const U32 tileY, const t2dVector& scale );

	/// Clone support
	void copyTo(SimObject* obj);

    /// Serialisation.
    bool loadTileLayer( const char* tileLayerFile );
    bool saveTileLayer( const char* tileLayerFile );

private:
    bool getPointInPolygon( t2dVector point, Vector<t2dVector>& poly );
    U32 getLineLineIntersection( t2dVector a1, t2dVector a2, t2dVector b1, t2dVector b2, Vector<t2dVector>& intersections );
    U32 getLinePolygonIntersection( t2dVector a1, t2dVector a2, Vector<t2dVector>& poly, Vector<t2dVector>& intersections );
    U32 getRectPolygonIntersection( RectF rect, Vector<t2dVector>& poly, Vector<t2dVector>& intersections );
    t2dVector getTileLocalPoint( U32 tileX, U32 tileY, t2dVector layerLocalPoint );

public:
    virtual void setCollisionPolyCustom( const U32 polyVertexCount, const t2dVector *pCustomPolygon );
    virtual bool checkCollisionSend( const F32 simTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats ) { sendCollisionStatus.mHandled = false; return false; };  // No send collision.

    virtual typePhysicsVector& getCollisionAreaPhysics( const RectF& worldArea );
    t2dSceneObject::typePhysicsVector& getCollisionAreaPhysics_ZeroAngle( const RectF& worldArea );
    t2dSceneObject::typePhysicsVector& getCollisionAreaPhysics_ArbitraryAngle( const RectF& worldArea );

    /// Tick Processing.
    void resetTickScrollPositions( void );
    void updateTickScrollPosition( void );
    virtual void interpolateTick( const F32 timeDelta );

    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );
    void renderZeroAngleObject( const RectF& viewPort, const RectF& viewIntersection );
    void renderArbitraryAngleObject( const RectF& viewPort, const RectF& viewIntersection );

    virtual bool onAdd( void );
    virtual void onRemove( void );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dTileLayer );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dTileLayer, 2 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dTileLayer, 3 );

    /// Declare Console Object.
    DECLARE_CONOBJECT( t2dTileLayer );
};


#endif // _T2DTILEMAP_H_
