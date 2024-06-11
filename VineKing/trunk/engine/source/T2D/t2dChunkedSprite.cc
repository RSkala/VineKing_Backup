//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Chunked Sprite.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dChunkedSprite.h"


//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dChunkedSprite);

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dChunkedSprite::t2dChunkedSprite() :  T2D_Stream_HeaderID(makeFourCCTag('2','D','C','S')),
                                        mChunkedImageDataBlock(NULL),
                                        mRepeatX(1),
                                        mRepeatY(1)

{
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dChunkedSprite::~t2dChunkedSprite()
{
}


//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dChunkedSprite::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dChunkedSprite::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Set Chunked Image.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dChunkedSprite, setChunkedImage, bool, 3, 3, "(chunkedImageName$) - Sets Chunked-Image.\n"
			  "@return Returns true on success.")
{
    // Set Chunked Image.
    return object->setChunkedImage( argv[2] );
}   
// Set Chunked Image.
bool t2dChunkedSprite::setChunkedImage( const char* chunkedImageName )
{
    // Check Chunked-Image Name.
    if ( !chunkedImageName || chunkedImageName == StringTable->insert("") )
        // Invalid!
        return false;

    // Find Chunked-Image Datablock.
    t2dChunkedImageDatablock* pChunkedImageDataBlock = dynamic_cast<t2dChunkedImageDatablock*>(Sim::findObject( chunkedImageName ));

    // Set Datablock.
    if ( !t2dCheckDatablock( pChunkedImageDataBlock ) )
    {
        // Warn.
        Con::warnf("t2dChunkedSprite::setChunkedImage() - t2dChunkedImageDatablock Datablock is invalid! (%s)", mConfigDataBlock?mConfigDataBlock->getName():"UNKNOWN!");
        // Return Here.
        return false;
    }

    // Set Chunked-Image Datablock.
    mChunkedImageDataBlock = pChunkedImageDataBlock;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Set Repeat.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dChunkedSprite, setRepeat, void, 3, 4, "(repeatX / repeatY) - Sets the Repeat X/Y repetition.\n"
			  "@param x,y X and Y repeats values as either (\"x y\") or (x,y))\n"
			  "@return No return value.")
{
   // The new position.
   U32 repeatX;
   U32 repeatY;

   // Elements in the first argument.
   const U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("repeatX repeatY")
   if ((elementCount == 2) && (argc == 3))
   {
      repeatX = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      repeatY = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (repeatX, repeatY)
   else if ((elementCount == 1) && (argc == 4))
   {
      repeatX = dAtoi(argv[2]);
      repeatY = dAtoi(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dChunkedSprite::setRepeat() - Invalid number of parameters!");
      return;
   }

    // Set Repeat.
    object->setRepeat(repeatX, repeatY);
}   
// Set Repeat.
void t2dChunkedSprite::setRepeat( U32 repeatX, U32 repeatY )
{
    // Warn.
    if ( repeatX < 1 || repeatY < 1 )
        Con::warnf("t2dChunkedSprite::setRepeat() - Repeats cannot be less than one!");

    // Set Repeat X/Y.
    mRepeatX = getMax(repeatX, (U32)1);
    mRepeatY = getMax(repeatY, (U32)1);
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dChunkedSprite::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Cannot render without Chunked-Texture.
    if ( !mChunkedImageDataBlock || !mChunkedImageDataBlock->getChunkedImageTexture() )
        return;

    // Validate Chunked Texture.
    if( !mChunkedImageDataBlock->getChunkedImageTexture().getTextureCountWidth() ||
        !mChunkedImageDataBlock->getChunkedImageTexture().getTextureCountHeight() )
        // Problem!
        return;

    // Enable Texturing.
    glEnable( GL_TEXTURE_2D );

    // Set Blend Options.
    setBlendOptions();

    // Tiled?
    if ( mRepeatX > 1 || mRepeatY > 1 )
    {
        // Yes, so render tiled regions...

        // Fetch Texture Handle from Datablock.
        const ChunkedTextureHandle textureHandle = mChunkedImageDataBlock->getChunkedImageTexture();

        // Calculate Clip Width/Height.
        const t2dVector renderClipWidth     = mWorldClipBoundary[1] - mWorldClipBoundary[0];
        const t2dVector renderClipHeight        = mWorldClipBoundary[3] - mWorldClipBoundary[0];
        // Calculate Clip Step Width/Height.
        const t2dVector renderClipStepWidth = renderClipWidth / (F32)mRepeatX;
        const t2dVector renderClipStepHeight    = renderClipHeight / (F32)mRepeatY;

        // Render Base/Current Clip Region.
        t2dVector renderBaseClipRegion[4];
        t2dVector renderCurrentClipRegion[4];

        // Calculate Base Render Clip Region.
        renderBaseClipRegion[0] = mWorldClipBoundary[0];
        renderBaseClipRegion[1] = mWorldClipBoundary[0] + renderClipStepWidth;
        renderBaseClipRegion[2] = mWorldClipBoundary[0] + renderClipStepWidth + renderClipStepHeight;
        renderBaseClipRegion[3] = mWorldClipBoundary[0] + renderClipStepHeight;
    
        // Step through Width.
        for ( U32 y = 0; y < mRepeatY; y++ )
        {
            // Reset Current Render Clip Region.
            renderCurrentClipRegion[0] = renderBaseClipRegion[0];
            renderCurrentClipRegion[1] = renderBaseClipRegion[1];
            renderCurrentClipRegion[2] = renderBaseClipRegion[2];
            renderCurrentClipRegion[3] = renderBaseClipRegion[3];

            // Step through height.
            for ( U32 x = 0; x < mRepeatX; x++ )
            {
                // Render sub-region region.
                renderRegion( renderCurrentClipRegion );

                // Next Current Sub-Region.
                renderCurrentClipRegion[0] += renderClipStepWidth;
                renderCurrentClipRegion[1] += renderClipStepWidth;
                renderCurrentClipRegion[2] += renderClipStepWidth;
                renderCurrentClipRegion[3] += renderClipStepWidth;
            }
            
            // Next Base Sub-Region.
            renderBaseClipRegion[0] += renderClipStepHeight;
            renderBaseClipRegion[1] += renderClipStepHeight;
            renderBaseClipRegion[2] += renderClipStepHeight;
            renderBaseClipRegion[3] += renderClipStepHeight;
        }
    }
    else
    {
        // Render whole region.
        renderRegion( mWorldClipBoundary );
    }

    // Disable Texturing.
    glDisable( GL_TEXTURE_2D );

    // Call Parent.
    Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}


//-----------------------------------------------------------------------------
// Render Region.
//-----------------------------------------------------------------------------
void t2dChunkedSprite::renderRegion( t2dVector* pClipBoundary )
{
    // Set Large Chunk Size.
    const F32 largeChunkSize = 256.0f;

    // Fetch Texture Handle from Datablock.
    ChunkedTextureHandle chunkedTextureHandle = mChunkedImageDataBlock->getChunkedImageTexture();

    // Fetch Width/Height Count.
    const U32 widthCount                    = chunkedTextureHandle.getTextureCountWidth();
    const U32 heightCount                   = chunkedTextureHandle.getTextureCountHeight();

    // Fetch Total Texture Width/Height.
    const F32 totalTextureWidth             = (F32)chunkedTextureHandle.getWidth();
    const F32 totalTextureHeight            = (F32)chunkedTextureHandle.getHeight();

    TextureHandle textureOverlapWidth       = chunkedTextureHandle.getSubTexture(widthCount-1, 0);
    TextureHandle textureOverlapHeight      = chunkedTextureHandle.getSubTexture(0, heightCount-1);

    // Calculate Edge Texture Texel Width/Height.
    const F32 edgeTextureTexelWidth         = 1.0f / ((TextureObject*)textureOverlapWidth)->texWidth;
    const F32 edgeTextureTexelHeight        = 1.0f / ((TextureObject*)textureOverlapHeight)->texHeight;

    // Calculate Clip Width/Height.
    const t2dVector renderClipWidth     = pClipBoundary[1] - pClipBoundary[0];
    const t2dVector renderClipHeight        = pClipBoundary[3] - pClipBoundary[0];

    // Calculate Clip Texel Width/Height.
    const t2dVector renderClipTexelWidth    = renderClipWidth / totalTextureWidth;
    const t2dVector renderClipTexelHeight   = renderClipHeight / totalTextureHeight;

    // Calculate Overlap Width/Height.
    const t2dVector renderOverlapWidth      = renderClipTexelWidth * (F32)textureOverlapWidth.getWidth();
    const t2dVector renderOverlapHeight = renderClipTexelHeight * (F32)textureOverlapHeight.getHeight();
    // Calculate Overlap Texel Width/Height.
    const F32 renderTexelOverlapWidth       = edgeTextureTexelWidth * (totalTextureWidth - ((widthCount-1) * largeChunkSize));
    const F32 renderTexelOverlapHeight      = edgeTextureTexelHeight * (totalTextureHeight - ((heightCount-1) * largeChunkSize));


    // Render Base/Current Clip Region.
    t2dVector renderCurrentClipRegion[4];

    // Render Clip Step Width/Height.
    t2dVector renderClipStepWidth;
    t2dVector renderClipStepHeight;

    // Set Clip Step Height to large-chunk size.
    renderClipStepHeight = renderClipTexelHeight*largeChunkSize;
    // Set Max Y Texel.
    F32 maxY = 1.0f;

    // Step through height.
    for ( U32 y = 0; y <= heightCount-1; y++ )
    {
        // Calculate Clip Step Height.
        if ( y == heightCount-1 )
        {
            // Set Clip Step Height to edge-chunk size.
            renderClipStepHeight = renderOverlapHeight;
            // Set edge-chunk texel size.
            maxY = renderTexelOverlapHeight;
        }

        // Set Max XTexel.
        F32 maxX = 1.0f;

        // Set Clip Step Width to large-chunk size.
        renderClipStepWidth = renderClipTexelWidth*largeChunkSize;

        // Step through width.
        for ( U32 x = 0; x <= widthCount-1; x++ )
        {
            // Fetch Sub-Texture.
            TextureHandle textureHandle = chunkedTextureHandle.getSubTexture(x, y);

            // Bind Texture.
            glBindTexture   ( GL_TEXTURE_2D,textureHandle.getGLName() );
            glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

            // Calculate Render Region Start Point.
            renderCurrentClipRegion[0] = pClipBoundary[0] + (renderClipTexelWidth*largeChunkSize*F32(x)) + (renderClipTexelHeight*largeChunkSize*F32(y));

            // Calculate Clip Step Width.
            if ( x == widthCount-1 )
            {
                // Set Clip Step Width to edge-chunk size.
                renderClipStepWidth = renderOverlapWidth;
                // Set edge-chunk texel size.
                maxX = renderTexelOverlapWidth;
            }

            // Calculate Current Render Region.
            renderCurrentClipRegion[1] = renderCurrentClipRegion[0] + renderClipStepWidth;
            renderCurrentClipRegion[2] = renderCurrentClipRegion[1] + renderClipStepHeight;
            renderCurrentClipRegion[3] = renderCurrentClipRegion[0] + renderClipStepHeight;

#ifdef TORQUE_OS_IPHONE
			//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
			dglDrawTextureQuadiPhone(renderCurrentClipRegion[0].mX, renderCurrentClipRegion[0].mY, renderCurrentClipRegion[1].mX, renderCurrentClipRegion[1].mY, renderCurrentClipRegion[3].mX, renderCurrentClipRegion[3].mY, renderCurrentClipRegion[2].mX, renderCurrentClipRegion[2].mY, 
									0.0f, 0.0f, maxX, 0.0f, 0.0f, maxY, maxX, maxY );
#else
            // Render sub-chunk region.
            glBegin(GL_QUADS);
                glTexCoord2f( 0.0f, 0.0f );
                glVertex2fv ( (GLfloat*)renderCurrentClipRegion );
                glTexCoord2f( maxX, 0.0f );
                glVertex2fv ( (GLfloat*)(renderCurrentClipRegion+1) );
                glTexCoord2f( maxX, maxY );
                glVertex2fv ( (GLfloat*)(renderCurrentClipRegion+2) );
                glTexCoord2f( 0.0f, maxY );
                glVertex2fv ( (GLfloat*)(renderCurrentClipRegion+3) );
            glEnd();

#endif
			
        }
    }
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dChunkedSprite )
    REGISTER_SERIALISE_VERSION( t2dChunkedSprite, 1, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dChunkedSprite, 1 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dChunkedSprite, 1 )
{
    bool                    repeatX;
    bool                    repeatY;
    bool                    chunkedImageFlag;
    char                    chunkedImageName[256];

    // Read Ad-Hoc Info.
    if ( !stream.read( &chunkedImageFlag ) )
        return false;

    // Do we have an chunked-image?
    if ( chunkedImageFlag )
    {
        // Yes, so read Chunked-Image Name.
        stream.readString( chunkedImageName );

        // Set Chunked Image.
        object->setChunkedImage( chunkedImageName );
    }

    // Read Repeat.
    if (    !stream.read( &repeatX ) ||
            !stream.read( &repeatY ) )
        return false;

    // Set Repeat.
    object->setRepeat( repeatX, repeatY );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dChunkedSprite, 1 )
{
    // Ad-Hoc Info.
    if ( object->mChunkedImageDataBlock )
    {
        // Write Chunked Image Name.
        if ( !stream.write( true ) )
            return false;

        // Write Chunked-Image Datablock Name.
        stream.writeString( object->mChunkedImageDataBlock->getName() );
    }
    else
    {
        // Write "No Chunked-Image Datablock".
        if ( !stream.write( false ) )
            return false;
    }

    // Write Repeat.
    if (    !stream.write( object->mRepeatX ) ||
            !stream.write( object->mRepeatY ) )
        return false;


    // Return Okay.
    return true;
}