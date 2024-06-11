//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Test Active-Tile Object.
//-----------------------------------------------------------------------------

#ifndef _T2DTESTTILE_H_
#define _T2DTESTTILE_H_

#ifndef _T2DVECTOR_H_
#include "../t2dVector.h"
#endif

#ifndef _T2DTILEMAP_H_
#include "../t2dTileMap.h"
#endif


//-----------------------------------------------------------------------------
// Test-Tile Datablock 2D.
//-----------------------------------------------------------------------------
class t2dTestTileDatablock : public t2dActiveTileDatablock
{
    typedef t2dActiveTileDatablock Parent;

public:
    t2dTestTileDatablock();

    static void  initPersistFields();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    // Declare Console Object.
    DECLARE_CONOBJECT(t2dTestTileDatablock);
};

//-----------------------------------------------------------------------------
// ActiveTile Object 2D.
//-----------------------------------------------------------------------------
class t2dTestTile : public t2dActiveTile
{
    typedef t2dActiveTile       Parent;

private:
    F32                         mIntensity;
    F32                         mPhase;
    ColorF                      mTileColour;


public:
    t2dActiveTileDatablock* mConfigDataBlock;

    t2dTestTile();
    ~t2dTestTile();

    virtual void resetTile();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection, const t2dVector tileWorldClipBoundary[4] );

    // Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dTestTile );
    // Declare Serialise Objects.
    DECLARE_T2D_LOAD_METHOD( t2dTestTile, 1 );
    DECLARE_T2D_SAVE_METHOD( t2dTestTile, 1 );

    // Declare Console Object.
    DECLARE_CONOBJECT(t2dTestTile);
};


#endif // _T2DTESTTILE_H_
