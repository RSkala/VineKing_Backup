//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Test Active-Tile Object.
//-----------------------------------------------------------------------------

#ifndef _T2DGUNTURRET_H_
#define _T2DGUNTURRET_H_

#ifndef _T2DVECTOR_H_
#include "../t2dVector.h"
#endif

#ifndef _T2DTILEMAP_H_
#include "../t2dTileMap.h"
#endif


//-----------------------------------------------------------------------------
// ActiveTile Object 2D.
//-----------------------------------------------------------------------------
class t2dGunTurret : public t2dActiveTile
{
    typedef t2dActiveTile       Parent;

private:
    t2dImageMapDatablock*       mImageMapDataBlock;
    U32                         mFrame;
    t2dSceneObject*             mpSceneObject2D;


public:
    t2dGunTurret();
    ~t2dGunTurret();

    bool setImageMap( const char* imageMapName, U32 frame );
    bool setFrame( U32 frame );
    bool setTrackObject( t2dSceneObject* pSceneObject2D );

    virtual void resetTile();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection, const t2dVector tileWorldClipBoundary[4] );

    // Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dGunTurret );
    // Declare Serialise Objects.
    DECLARE_T2D_LOAD_METHOD( t2dGunTurret, 1 );
    DECLARE_T2D_SAVE_METHOD( t2dGunTurret, 1 );

    // Declare Console Object.
    DECLARE_CONOBJECT(t2dGunTurret);
};


#endif // _T2DGUNTURRET_H_
