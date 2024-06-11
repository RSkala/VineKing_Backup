//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Chunked Image Datablock Object.
//-----------------------------------------------------------------------------

#include "dgl/gBitmap.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dChunkedImageDatablock.h"


//------------------------------------------------------------------------------

    
IMPLEMENT_CO_DATABLOCK_V1(t2dChunkedImageDatablock);


//------------------------------------------------------------------------------

t2dChunkedImageDatablock::t2dChunkedImageDatablock() : mChunkedTextureName(StringTable->insert(""))
{
}

//------------------------------------------------------------------------------

t2dChunkedImageDatablock::~t2dChunkedImageDatablock()
{
}

//------------------------------------------------------------------------------

void t2dChunkedImageDatablock::initPersistFields()
{
    Parent::initPersistFields();

    // Fields.
    addField("imageName", TypeFilename, Offset(mChunkedTextureName, t2dChunkedImageDatablock));
}

//------------------------------------------------------------------------------

bool t2dChunkedImageDatablock::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Calculate Chunked Image.
    calculateChunkedImage();

    // Finish here if invalid.
    if ( !getIsValid() )
        return false;

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void t2dChunkedImageDatablock::calculateChunkedImage(void)
{
    // Invalidate Datablock.
    setIsValid(false);

    // Load Texture.
    mChunkedImageTextureHandle = ChunkedTextureHandle( mChunkedTextureName );

    // Check for Error.
    if (!(bool)mChunkedImageTextureHandle)
    {
        Con::warnf("t2dChunkedImageDatablock::calculateChunkedImage() - Failed to load Chunked-Image!" );
        return;
    }

    // Validate Datablock.
    setIsValid(true);
}

//------------------------------------------------------------------------------

void t2dChunkedImageDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
    //stream->writeString( mTextureName );
}

//------------------------------------------------------------------------------

void t2dChunkedImageDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);

    // Read Datablock.
    //mTextureName = stream->readSTString();
}
