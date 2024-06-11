//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Chunked Image Datablock Object.
//-----------------------------------------------------------------------------

#ifndef _T2DCHUNKEDIMAGEDATABLOCK_H_
#define _T2DCHUNKEDIMAGEDATABLOCK_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _T2DBASEDATABLOCK_H_
#include "./t2dBaseDatablock.h"
#endif

#ifndef _GCHUNKEDTEXMANAGER_H_
#include "dgl/gChunkedTexManager.h"
#endif


///-----------------------------------------------------------------------------
/// ChunkedImage Datablock 2D.
///-----------------------------------------------------------------------------
class t2dChunkedImageDatablock : public t2dBaseDatablock
{
private:
    typedef t2dBaseDatablock Parent;

    // Storage.
    ChunkedTextureHandle    mChunkedImageTextureHandle;
    StringTableEntry        mChunkedTextureName;

public:
    t2dChunkedImageDatablock();
    virtual ~t2dChunkedImageDatablock();

    void calculateChunkedImage(void);
    ChunkedTextureHandle getChunkedImageTexture(void)  const { return mChunkedImageTextureHandle; };

    /// Core.
    static void initPersistFields();
    bool onAdd();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dChunkedImageDatablock);
};


#endif // _T2DCHUNKEDIMAGEDATABLOCK_H_
