//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Chunked Sprite.
//-----------------------------------------------------------------------------

#ifndef _T2DCHUNKEDSPRITE_H_
#define _T2DCHUNKEDSPRITE_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif

#ifndef _T2DCHUNKEDIMAGEDATABLOCK_H_
#include "./t2dChunkedImageDatablock.h"
#endif

///-----------------------------------------------------------------------------
/// Chunked Sprite Object 2D.
///-----------------------------------------------------------------------------
class t2dChunkedSprite : public t2dSceneObject
{
private:
    // Storage.
    typedef t2dSceneObject          Parent;
    t2dChunkedImageDatablock*       mChunkedImageDataBlock;
    U32                             mRepeatX;
    U32                             mRepeatY;

public:
    t2dChunkedSprite();
    virtual ~t2dChunkedSprite();

    // Accessors.
    bool setChunkedImage( const char* chunkedImageName );
    void setRepeat( U32 repeatX, U32 repeatY );
    void renderRegion( t2dVector* pClipBoundary );

    /// Core.
    virtual bool onAdd();
    virtual void onRemove();
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dChunkedSprite );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dChunkedSprite, 1 );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dChunkedSprite);
};

#endif // _T2DCHUNKEDSPRITE_H_
