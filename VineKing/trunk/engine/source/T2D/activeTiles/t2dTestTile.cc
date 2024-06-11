//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Tilemap Object.
//-----------------------------------------------------------------------------


#include "./t2dTestTile.h"


//------------------------------------------------------------------------------


IMPLEMENT_CO_DATABLOCK_V1(t2dTestTileDatablock);
IMPLEMENT_CONOBJECT(t2dTestTile);



//-----------------------------------------------------------------------------
//
//  t2dActiveTileDatablock.
//
//-----------------------------------------------------------------------------
t2dTestTileDatablock::t2dTestTileDatablock()
{
}

//------------------------------------------------------------------------------

void t2dTestTileDatablock::initPersistFields()
{
    // Call Parent.
    Parent::initPersistFields();
}

//------------------------------------------------------------------------------

void t2dTestTileDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
}

//------------------------------------------------------------------------------

void t2dTestTileDatablock::unpackData(BitStream* stream)
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
t2dTestTile::t2dTestTile() :        T2D_Stream_HeaderID(makeFourCCTag('2','D','A','T')),
                                    mConfigDataBlock(NULL)
{
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dTestTile::~t2dTestTile()
{
}


//-----------------------------------------------------------------------------
// OnAdd.
//-----------------------------------------------------------------------------
bool t2dTestTile::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Datablock Available?
    if ( mConfigDataBlock )
    {
        // Yes, so cast the Datablock.
        mConfigDataBlock = dynamic_cast<t2dTestTileDatablock*>(mConfigDataBlock);

        // Transfer Datablock (if we've got one).
        if ( !t2dCheckDatablock( mConfigDataBlock ) )
        {
            // Warn.
            Con::warnf("t2dTestTile::onAdd() - t2dTestTile Datablock is invalid! (%s)", mConfigDataBlock?mConfigDataBlock->getName():"UNKNOWN!");
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
void t2dTestTile::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Reset Tile.
// NOTE:-   This is called by T2D when a tile-layer is reset.
//          "t2dTileLayer::resetLayer()"
//-----------------------------------------------------------------------------
void t2dTestTile::resetTile()
{
    // Reset Intensity.
    mIntensity = 0.0f;

    // Choose Random Phase.
    mPhase = mGetT2DRandomF();

    // Choose Random Tile Colour.
    mTileColour.set( mGetT2DRandomF(), mGetT2DRandomF(), mGetT2DRandomF() );
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dTestTile )
    REGISTER_SERIALISE_VERSION( t2dTestTile, 1, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dTestTile, 1 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dTestTile, 1 )
{
    // Read Tile-State...
    if (    !stream.read( &object->mIntensity ) &&
            !stream.read( &object->mPhase ) &&
            !stream.read( &object->mTileColour ) )
        return false;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dTestTile, 1 )
{
    // Write Tile-State...
    if (    !stream.write( object->mIntensity ) &&
            !stream.write( object->mPhase ) &&
            !stream.write( object->mTileColour ) )
        return false;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dTestTile::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
    // Update Intensity (1 cycle/sec).
    mIntensity += elapsedTime;
    mIntensity = mFmod( mIntensity, 1.0f );
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
// PUAP -Mat untested
void t2dTestTile::renderObject( const RectF& viewPort, const RectF& viewIntersection, const t2dVector tileWorldClipBoundary[4] )
{
    // Disable Texturing.
    glDisable( GL_TEXTURE_2D );

    // Calculate Modulation.
    F32 modulation = mFmod( mIntensity+mPhase, 1.0f );
    // Set Tile Colour.
    glColor4f( mTileColour.red*modulation, mTileColour.green*modulation, mTileColour.blue*modulation, 1.0f );
	
#ifdef TORQUE_OS_IPHONE
	//NOTE: Torque expects to use quads, but we use a triangle strip, so the last two vertices are switched	
	GLfloat vertices[] = {
		(GLfloat)(tileWorldClipBoundary[0].mX),(GLfloat)(tileWorldClipBoundary[0].mY),
		(GLfloat)(tileWorldClipBoundary[1].mX),(GLfloat)(tileWorldClipBoundary[1].mY),
		(GLfloat)(tileWorldClipBoundary[3].mX),(GLfloat)(tileWorldClipBoundary[3].mY),
		(GLfloat)(tileWorldClipBoundary[2].mX),(GLfloat)(tileWorldClipBoundary[2].mY),
	};		
	//2 for 2-point coordinates
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
    // Draw Animated Tile.
    glBegin(GL_QUADS);
        glVertex2fv ( (GLfloat*)&(tileWorldClipBoundary[0]) );
        glVertex2fv ( (GLfloat*)&(tileWorldClipBoundary[1]) );
        glVertex2fv ( (GLfloat*)&(tileWorldClipBoundary[2]) );
        glVertex2fv ( (GLfloat*)&(tileWorldClipBoundary[3]) );
    glEnd();

#endif
    // Reset Colour.
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

    // Enable Texturing.
    glEnable( GL_TEXTURE_2D );
}

