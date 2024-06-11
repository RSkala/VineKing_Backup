//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Actile-Tile Object.
//-----------------------------------------------------------------------------

#ifndef _T2DACTIVETILE_H_
#define _T2DACTIVETILE_H_


#ifndef _T2DBASEDATABLOCK_H_
#include "./t2dBaseDatablock.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif


///-----------------------------------------------------------------------------
/// ActiveTile Datablock 2D.
///-----------------------------------------------------------------------------
class t2dActiveTileDatablock : public t2dBaseDatablock
{
    typedef t2dBaseDatablock Parent;

public:
    t2dActiveTileDatablock();

    static void  initPersistFields();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dActiveTileDatablock);
};


///-----------------------------------------------------------------------------
/// ActiveTile Object 2D.
///-----------------------------------------------------------------------------
class t2dActiveTile : public SimObject
{
    typedef SimObject       Parent;

public:
    t2dActiveTileDatablock* mConfigDataBlock;

    t2dActiveTile();
    ~t2dActiveTile();

    void setDataBlock( t2dActiveTileDatablock* pDataBlock );

    virtual void resetTile() {};
    virtual bool onAdd();
    virtual void onRemove();
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection, const t2dVector tileWorldClipBoundary[4] );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dActiveTile );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dActiveTile, 1 );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dActiveTile);
};


#endif // _T2DACTIVETILE_H_
