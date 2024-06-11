//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Actile-Tile Object.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "math/mMathFn.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dActiveTile.h"


//------------------------------------------------------------------------------


IMPLEMENT_CO_DATABLOCK_V1(t2dActiveTileDatablock);
IMPLEMENT_CONOBJECT(t2dActiveTile);


//-----------------------------------------------------------------------------
//
//  t2dActiveTileDatablock.
//
//-----------------------------------------------------------------------------
t2dActiveTileDatablock::t2dActiveTileDatablock()
{
}

//------------------------------------------------------------------------------

void t2dActiveTileDatablock::initPersistFields()
{
    // Call Parent.
    Parent::initPersistFields();
}

//------------------------------------------------------------------------------

void t2dActiveTileDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
}

//------------------------------------------------------------------------------

void t2dActiveTileDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);

    // Read Datablock.
}



//-----------------------------------------------------------------------------
//
//  t2dActiveTile.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dActiveTile::t2dActiveTile() :    T2D_Stream_HeaderID(makeFourCCTag('2','D','A','T')),
                                    mConfigDataBlock(NULL)
{
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dActiveTile::~t2dActiveTile()
{
}


//-----------------------------------------------------------------------------
// Set DataBlock.
//-----------------------------------------------------------------------------
void t2dActiveTile::setDataBlock( t2dActiveTileDatablock* pDataBlock )
{
    // Set DataBlock.
    mConfigDataBlock = pDataBlock;
}


//-----------------------------------------------------------------------------
// OnAdd.
//-----------------------------------------------------------------------------
bool t2dActiveTile::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Datablock Available?
    if ( mConfigDataBlock )
    {
        // Yes, so cast the Datablock.
        mConfigDataBlock = dynamic_cast<t2dActiveTileDatablock*>(mConfigDataBlock);

        // Transfer Datablock (if we've got one).
        if ( !t2dCheckDatablock( mConfigDataBlock ) )
        {
            // Warn.
            Con::warnf("t2dActiveTile::onAdd() - t2dActiveTile Datablock is invalid! (%s)", mConfigDataBlock?mConfigDataBlock->getName():"UNKNOWN!");
            // Return Here.
            return false;
        }
    }

    // No Default Parameters for now !!!!!
    return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dActiveTile::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dActiveTile )
    REGISTER_SERIALISE_VERSION( t2dActiveTile, 1, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_LEAF( t2dActiveTile, 1 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dActiveTile, 1 )
{
    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dActiveTile, 1 )
{
    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dActiveTile::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dActiveTile::renderObject( const RectF& viewPort, const RectF& viewIntersection, const t2dVector tileWorldClipBoundary[4] )
{
    // Call Parent.
    //Parent::renderObject( intersection ); // Always use for Debug Support!
}


