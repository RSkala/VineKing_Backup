//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scroller Object.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "math/mMathFn.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dScroller.h"


//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dScroller);

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dScroller::t2dScroller() :    T2D_Stream_HeaderID(makeFourCCTag('2','D','S','C')),
                                mImageMapDataBlock(NULL),
                                mRepeatX(1.0f),
                                mRepeatY(1.0f),
                                mScrollX(0.0f),
                                mScrollY(0.0f),
                                mTextureOffsetX(0.0f),
                                mTextureOffsetY(0.0f)
{
   if (getDefaultConfig())
   {
      getDefaultConfig()->setDataField(StringTable->insert("repeatX"), NULL, "1");
      getDefaultConfig()->setDataField(StringTable->insert("repeatY"), NULL, "1");
      getDefaultConfig()->setDataField(StringTable->insert("scrollX"), NULL, "0");
      getDefaultConfig()->setDataField(StringTable->insert("scrollY"), NULL, "0");
      getDefaultConfig()->setDataField(StringTable->insert("scrollPositionY"), NULL, "0");
      getDefaultConfig()->setDataField(StringTable->insert("scrollPositionX"), NULL, "0");
   }

	mUseSourceRect = false;
	mSrcRect = RectF();
	mSrcTexCoords = RectF();
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dScroller::~t2dScroller()
{
}

void t2dScroller::initPersistFields()
{
   addProtectedField("imageMap", TypeString, Offset(mImageMapDataBlockName, t2dScroller), &setImageMap, &defaultProtectedGetFn, "");
   addProtectedField("repeatX", TypeF32, Offset(mRepeatX, t2dScroller), &setRepeatX, &defaultProtectedGetFn, "");
   addProtectedField("repeatY", TypeF32, Offset(mRepeatY, t2dScroller), &setRepeatY, &defaultProtectedGetFn, "");
   addProtectedField("scrollX", TypeF32, Offset(mScrollX, t2dScroller), &setScrollX, &defaultProtectedGetFn, "");
   addProtectedField("scrollY", TypeF32, Offset(mScrollY, t2dScroller), &setScrollY, &defaultProtectedGetFn, "");
   addProtectedField("scrollPositionX", TypeF32, Offset(mTextureOffsetX, t2dScroller), &setScrollPositionX, &defaultProtectedGetFn, "");
   addProtectedField("scrollPositionY", TypeF32, Offset(mTextureOffsetY, t2dScroller), &setScrollPositionY, &defaultProtectedGetFn, "");

   addField("mUseSourceRect", TypeBool, Offset(mUseSourceRect, t2dScroller));
   addField("sourceRect", TypeRectF, Offset(mSrcRect, t2dScroller));

   Parent::initPersistFields();
}

void t2dScroller::copyTo(SimObject* object)
{
   Parent::copyTo(object);

   AssertFatal(dynamic_cast<t2dScroller*>(object), "t2dScroller::copyTo - Copy object is not a t2dScroller!");
   t2dScroller* scroller = static_cast<t2dScroller*>(object);

   scroller->setImageMap(getImageMapName());
   scroller->setRepeat(getRepeatX(), getRepeatY());
   scroller->setScroll(getScrollX(), getScrollY());
   scroller->setScrollPosition(getScrollPositionX(), getScrollPositionY());
}

//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dScroller::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

	//Luma: consolidate this into a single location
	setSourceTextCoords();

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dScroller::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Set Image Map.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dScroller, setImageMap, void, 3, 3, "(imageMapName$) - Sets the imageMap Name."
			  "@return No return value.")
{
    // Set Image Map.
    object->setImageMap( argv[2] );
}

ConsoleMethod(t2dScroller, getImageMap, const char*, 2, 2, "()\n @return Returns the image map.")
{
   return object->getImageMapName();
}

// Set Image Map.
void t2dScroller::setImageMap( const char* imageMapName )
{
   imageMapName = StringTable->insert( imageMapName );
    // Find imageMap Datablock.
    t2dImageMapDatablock* pImageMapDatablock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject( imageMapName ));

    // Check Datablock (if we've got one).
    if ( !t2dCheckDatablock( pImageMapDatablock ) )
    {
        // Warn.
        Con::warnf("t2dScroller::setImageMap() - Invalid t2dImageMapDatablock datablock (%s)", imageMapName);
        return;
    }

    // Check that the datablock is in FULL mode.
    if ( pImageMapDatablock->getImageMapMode() != t2dImageMapDatablock::T2D_IMAGEMODE_FULL )
    {
        // Warn.
        Con::warnf("t2dScroller::setImageMap() - t2dImageMapDatablock datablock *must* be in FULL mode! (%s)", imageMapName);
        return;
    }

    // Set Datablock.
    mImageMapDataBlock = pImageMapDatablock;
    mImageMapDataBlockName = StringTable->insert(imageMapName);

	//Luma: reset the source rects for new image map
	setSourceTextCoords();
}

ConsoleMethod(t2dScroller, setRepeatX, void, 3, 3, "(repeatX) Sets the number of times to repeat the texture over x direction\n"
			  "@return No return value.")
{
   object->setRepeat(dAtof(argv[2]), object->getRepeatY());
}

ConsoleMethod(t2dScroller, setRepeatY, void, 3, 3, "(repeatY) Sets the number of timnes to repeat the texture in y direction.\n"
			  "@return No return value.")
{
   object->setRepeat(object->getRepeatX(), dAtof(argv[2]));
}

ConsoleMethod(t2dScroller, getRepeatX, F32, 2, 2, "() \n @return Returns repeat X value")
{
   return object->getRepeatX();
}

ConsoleMethod(t2dScroller, getRepeatY, F32, 2, 2, "() \n @return Returns repeat Y value")
{
   return object->getRepeatY();
}

ConsoleMethod(t2dScroller, setScrollX, void, 3, 3, "(ScrollX) Sets the scroll speed in x direction\n"
			  "@return No return value.")
{
   object->setScroll(dAtof(argv[2]), object->getScrollY());
}

ConsoleMethod(t2dScroller, setScrollY, void, 3, 3, "(ScrollY) Sets the scroll speed in the Y direction\n"
			  "@return No return value.")
{
   object->setScroll(object->getScrollX(), dAtof(argv[2]));
}

ConsoleMethod(t2dScroller, getScrollX, F32, 2, 2, "() \n @return Returns Scroll speed in x direction.")
{
   return object->getScrollX();
}

ConsoleMethod(t2dScroller, getScrollY, F32, 2, 2, "() \n @return Returns Scroll speed in y direction.")
{
   return object->getScrollY();
}

ConsoleMethod(t2dScroller, setScrollPositionX, void, 3, 3, "(ScrollPositionX) Set the texture's position in x direction\n"
			  "@return No return value.")
{
   object->setScrollPosition(dAtof(argv[2]), object->getScrollPositionY());
}

ConsoleMethod(t2dScroller, setScrollPositionY, void, 3, 3, "(ScrollPositionY) Set the texture's position in y direction\n"
			  "@return No return value.")
{
   object->setScrollPosition(object->getScrollPositionX(), dAtof(argv[2]));
}

ConsoleMethod(t2dScroller, getScrollPositionX, F32, 2, 2, "() \nReturns  texture's position in x direction")
{
   return object->getScrollPositionX();
}

ConsoleMethod(t2dScroller, getScrollPositionY, F32, 2, 2, "() \nReturns texture's position in y direction")
{
   return object->getScrollPositionY();
}

//-----------------------------------------------------------------------------
// Set Repeat.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dScroller, setRepeat, void, 3, 4, "(repeatX / repeatY) Sets the Repeat X/Y repetition in each direction.\n"
			  "@param repeatX/Y The number of times to repeat in each direction as either (\"x y\") or (x, y)\n"
			  "@return No return value.")
{
   // The new position.
   F32 repeatX;
   F32 repeatY;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("repeatX repeatY")
   if ((elementCount == 2) && (argc == 3))
   {
      repeatX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      repeatY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (repeatX, repeatY)
   else if ((elementCount == 1) && (argc == 4))
   {
      repeatX = dAtof(argv[2]);
      repeatY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dScroller::setRepeat() - Invalid number of parameters!");
      return;
   }

   // Set Repeat.
   object->setRepeat(repeatX, repeatY);
}   
// Set Repeat.
void t2dScroller::setRepeat( F32 repeatX, F32 repeatY )
{
    // Warn.
    if ( mLessThanOrEqual(repeatX, 0.0f) || mLessThanOrEqual(repeatY, 0.0f) )
    {
        Con::warnf("t2dScroller::setRepeat() - Repeats must be greater than zero!");
    }

    // Set Repeat X/Y.
    mRepeatX = getMax(repeatX, (F32)T2D_CONST_EPSILON);
    mRepeatY = getMax(repeatY, (F32)T2D_CONST_EPSILON);
}


//-----------------------------------------------------------------------------
// Set Scroll.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dScroller, setScroll, void, 3, 4, "(offsetX / offsetY) Sets the Scroll speed."
			  "@param offsetX/Y The scroll speed in each direction as either (\"x y\") or (x, y)\n"
			  "@return No return value.")
{
   // The new position.
   F32 scrollX;
   F32 scrollY;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("scrollX scrollY")
   if ((elementCount == 2) && (argc == 3))
   {
      scrollX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      scrollY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (scrollX, scrollY)
   else if ((elementCount == 1) && (argc == 4))
   {
      scrollX = dAtof(argv[2]);
      scrollY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dScroller::setScroll() - Invalid number of parameters!");
      return;
   }

   // Set Scroll.
   object->setScroll(scrollX, scrollY);
}   
// Set Scroll.
void t2dScroller::setScroll( F32 scrollX, F32 scrollY )
{
    // Set Scroll X/Y.
    mScrollX = scrollX;
    mScrollY = scrollY;

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();
}


//-----------------------------------------------------------------------------
// Set Scroll Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dScroller, setScrollPolar, void, 4, 4, "(angle, scrollSpeed) Sets Auto-Pan Polarwise.\n"
			  "@param angle Polar angle.\n"
			  "@param scrollSpeed Speed as polar magnitude\n"
			  "@return No return value.")
{
    // Renormalise Angle.
    F32 angle = mFmod(dAtof(argv[2]), 360.0f);
    // Fetch Speed.
    F32 scrollSpeed = dAtof(argv[3]);

    // Set Scroll.
    object->setScroll( mSin(mDegToRad(angle))*scrollSpeed, -mCos(mDegToRad(angle))*scrollSpeed );
}


//-----------------------------------------------------------------------------
// Set Scroll Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dScroller, setScrollPosition, void, 3, 4, "(positionX / positionY) Sets the Scroll position X/Y."
			  "@param positionX/Y The scroll texture position as either (\"x y\") or (x, y)\n"
			  "@return No return value.")
{
   // The new position.
   F32 scrollX;
   F32 scrollY;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("positionX positionY")
   if ((elementCount == 2) && (argc == 3))
   {
      scrollX = dAtof(t2dSceneObject::getStringElement(argv[2], 0));
      scrollY = dAtof(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (positionX, positionY)
   else if ((elementCount == 1) && (argc == 4))
   {
      scrollX = dAtof(argv[2]);
      scrollY = dAtof(argv[3]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dScroller::setScrollPosition() - Invalid number of parameters!");
      return;
   }

   // Set Scroll Position.
   object->setScrollPosition(scrollX, scrollY);
}
// Set Scroll Position.
void t2dScroller::setScrollPosition( F32 scrollX, F32 scrollY )
{
    // Yes, so calculate texel shift per world-unit.
    const F32 scrollTexelX = mRepeatX / getSize().mX;
    const F32 scrollTexelY = mRepeatY / getSize().mY;

    // Calculate new offset and clamp.
    mTextureOffsetX = mFmod( scrollTexelX * scrollX, 1.0f );
    mTextureOffsetY = mFmod( scrollTexelY * scrollY, 1.0f );

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dScroller::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( sceneTime, elapsedTime, pDebugStats );

    // Cannot update without imageMap.
    if ( !mImageMapDataBlock )
        return;

    // Calculate texel shift per world-unit.
    const F32 scrollTexelX = mRepeatX / getSize().mX;
    const F32 scrollTexelY = mRepeatY / getSize().mY;

    // Calculate Scrolling Offsets.
    const F32 scrollOffsetX = scrollTexelX * mScrollX * elapsedTime;
    const F32 scrollOffsetY = scrollTexelY * mScrollY * elapsedTime;

    // Calculate new offset.
    mTextureOffsetX += scrollOffsetX;
    mTextureOffsetY += scrollOffsetY;

    // Update Tick Scroll Position.
    // NOTE:-   We *must* do the tick update here!
    updateTickScrollPosition();

    // Make sure the offsets used don't under/overflow.
    // NOTE-    We could simply use 'mFmod' on the offsets but unfrotunately
    //          we need to ensure that we can do a modulo simultaneously on both
    //          the pre/post ticks values otherwuse the pre/post interpolation
    //          won't worked correctly resulting in a nasty wrap 'hitch'.

    // Calculate Renormalised Offsets.
    const F32 renormalizedPreOffsetX = mFmod( mPreTickTextureOffset.mX, 1.0f );
    const F32 renormalizedPreOffsetY = mFmod( mPreTickTextureOffset.mY, 1.0f );
    const F32 renormalizedPostOffsetX = mFmod( mPostTickTextureOffset.mX, 1.0f );
    const F32 renormalizedPostOffsetY = mFmod( mPostTickTextureOffset.mY, 1.0f );

    // Scrolling X Positive?
    if ( mGreaterThanZero(scrollOffsetX) )
    {
        // Yes, so old/new normalised simultaneously?
        if ( mLessThan(renormalizedPreOffsetX, renormalizedPostOffsetX) )
        {
            // Yes, so normalised offset.
            mTextureOffsetX = renormalizedPostOffsetX;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.mX = renormalizedPreOffsetX;
            mPostTickTextureOffset.mX = renormalizedPostOffsetX;
        }        
    }
    else
    {
        // No, so old/new normalised simultaneously?
        if ( mGreaterThan(renormalizedPreOffsetX, renormalizedPostOffsetX) )
        {
            // Yes, so normalised offset.
            mTextureOffsetX = renormalizedPostOffsetX;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.mX = renormalizedPreOffsetX;
            mPostTickTextureOffset.mX = renormalizedPostOffsetX;
        }        
    }

    // Scrolling Y Positive?
    if ( mGreaterThanZero(scrollOffsetY) )
    {
        // Yes, so old/new normalised propertionally?
        if ( mLessThan(renormalizedPreOffsetY, renormalizedPostOffsetY) )
        {
            // Yes, so normalised offset.
            mTextureOffsetY = renormalizedPostOffsetY;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.mY = renormalizedPreOffsetY;
            mPostTickTextureOffset.mY = renormalizedPostOffsetY;
        }        
    }
    else
    {
        // No, so old/new normalised propertionally?
        if ( mGreaterThan(renormalizedPreOffsetY, renormalizedPostOffsetY) )
        {
            // Yes, so normalised offset.
            mTextureOffsetY = renormalizedPostOffsetY;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.mY = renormalizedPreOffsetY;
            mPostTickTextureOffset.mY = renormalizedPostOffsetY;
        }        
    }
}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dScroller::interpolateTick( const F32 timeDelta )
{
    // Base Tick Interpolation.
    Parent::interpolateTick( timeDelta );

    // Calculate Render Tick Position.
    mRenderTickTextureOffset = (mPreTickTextureOffset * timeDelta) + ((1.0f-timeDelta) * mPostTickTextureOffset);
}


//-----------------------------------------------------------------------------
// Reset Tick Scroll Positions.
//-----------------------------------------------------------------------------
void t2dScroller::resetTickScrollPositions( void )
{
    // Reset Scroll Positions.
    mRenderTickTextureOffset.set( mTextureOffsetX, mTextureOffsetY );
    mPreTickTextureOffset = mPostTickTextureOffset = mRenderTickTextureOffset;
}


//-----------------------------------------------------------------------------
// Update Tick Scroll Position.
//-----------------------------------------------------------------------------
void t2dScroller::updateTickScrollPosition( void )
{
    // Store Pre Tick Scroll Position.
    mPreTickTextureOffset = mPostTickTextureOffset;

    // Store Current Tick Scroll Position.
    mPostTickTextureOffset.set( mTextureOffsetX, mTextureOffsetY );

    // Render Tick Position is at Pre-Tick Scroll Position.
    mRenderTickTextureOffset = mPreTickTextureOffset;
};


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dScroller::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Cannot render without imageMap.
    if ( !mImageMapDataBlock )
        return;

    // Bind Texture.
    glEnable        ( GL_TEXTURE_2D );

    // Clamp Texture Offsets.
    const F32 textureOffsetX = mFmod( mRenderTickTextureOffset.mX, 1.0f );
    const F32 textureOffsetY = mFmod( mRenderTickTextureOffset.mY, 1.0f );

	F32 minX = 0.0f; 
	F32 maxX = 0.0f;
	F32 minY = 0.0f;
	F32 maxY = 0.0f;
    
	if(!mUseSourceRect)
	{
		// Fetch first frame area and bind appropriate texture page.
		const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameAreaBind( 0 );
		// Fetch Positions.
		minX = frameArea.mX + textureOffsetX;
		minY = frameArea.mY + textureOffsetY;
		maxX = ( frameArea.mX2 * mRepeatX ) + textureOffsetX;
		maxY = ( frameArea.mY2 * mRepeatY ) + textureOffsetY;
	}
	else
	{
		minX = mSrcTexCoords.point.x + textureOffsetX;
		minY = mSrcTexCoords.point.y + textureOffsetY;
		maxX = mSrcTexCoords.extent.x + textureOffsetX;
		maxY = mSrcTexCoords.extent.y + textureOffsetY;
	}

	// Set Blend Options.
    setBlendOptions();
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
    // Adative Clamp for texture.
    // NOTE:- We do this so that we don't end up sampling repetitively when we're not scrolling.
    mImageMapDataBlock->setTextureClamp( mIsOne( mRepeatX ) && mIsZero( mScrollX ) && mIsZero(textureOffsetX), mIsOne( mRepeatY ) && mIsZero( mScrollY ) && mIsZero(textureOffsetY) );

    //// Texture Matrix.
    //glMatrixMode( GL_TEXTURE );
    //// Offset Texture.
    //glTranslatef( mTextureOffsetX, mTextureOffsetY, 0 );

#ifdef TORQUE_OS_IPHONE
	//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
	dglDrawTextureQuadiPhone(mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY, mWorldClipBoundary[1].mX, mWorldClipBoundary[1].mY, mWorldClipBoundary[3].mX, mWorldClipBoundary[3].mY, mWorldClipBoundary[2].mX, mWorldClipBoundary[2].mY, 
								minX, minY, maxX, minY, minX, maxY, maxX, maxY );
#else



	   //render with new method that takes source rect usage into account
       glBegin(GL_QUADS);
         glTexCoord2f( minX, minY );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[0]) );
         glTexCoord2f(maxX, minY );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[1]) );
         glTexCoord2f( maxX, maxY );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[2]) );
         glTexCoord2f( minX, maxY );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[3]) );
       glEnd();

    // Restore Texture Matrix.
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );

#endif
    // Disable Texturing.
    glDisable   ( GL_TEXTURE_2D );

    // Turn-on texture-clamp.
    mImageMapDataBlock->setTextureClamp( true, true );

    // Call Parent.
    Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dScroller )
    REGISTER_SERIALISE_VERSION( t2dScroller, 4, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dScroller, 4 )


//-----------------------------------------------------------------------------
// Load v4
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dScroller, 4 )
{
    F32                     repeatX;
    F32                     repeatY;
    F32                     offsetX;
    F32                     offsetY;
    bool                    imageMapFlag;
    char                    imageMapName[256];

    // Object Info.
    if  (   !stream.read( &repeatX ) ||
            !stream.read( &repeatY ) ||
            !stream.read( &offsetX ) ||
            !stream.read( &offsetY ) )
        return false;

    // Read Ad-Hoc Info.
    if ( !stream.read( &imageMapFlag ) )
        return false;

    // Do we have an imageMap?
    if ( imageMapFlag )
    {
        // Yes, so read ImageMap Name.
        stream.readString( imageMapName );

        // Set ImageMap.
        object->setImageMap( imageMapName );
    }

    // Set Repeat.
    object->setRepeat( repeatX, repeatY );

    // Set Scroll.
    object->setScroll( offsetX, offsetY );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v4
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dScroller, 4 )
{
    // Object Info.
    if  (   !stream.write( object->mRepeatX ) ||
            !stream.write( object->mRepeatY ) ||
            !stream.write( object->mScrollX ) ||
            !stream.write( object->mScrollY ) )
        return false;

    // Ad-Hoc Info.
    if ( object->mImageMapDataBlock )
    {
        // Write ImageMap Datablock Name.
        if ( !stream.write( true ) )
            return false;

        // Write ImageMap Datablock Name.
        stream.writeString( object->mImageMapDataBlock->getName() );
    }
    else
    {
        // Write "No ImageMap Datablock".
        if ( !stream.write( false ) )
            return false;
    }

    // Return Okay.
    return true;
}

//Luma: consolidate this into a single location
void t2dScroller::setSourceTextCoords(void)
{
	if(mUseSourceRect)
	{
		bool isValidRect = mSrcRect.isValidRect();
		//An invalid source rect will result in us ignoring it, meaning it doesnt get rendered.

		F32 minX = 0; F32 maxX = 0;
		F32 minY = 0; F32 maxY = 0;

		if(isValidRect)
		{
			F32	fTexelWidth = 1.0f / (F32)mImageMapDataBlock->getSrcBitmapWidth();
			F32	fTexelHeight = 1.0f / (F32)mImageMapDataBlock->getSrcBitmapHeight();
			F32	fAreaWidth = mSrcRect.extent.x * fTexelWidth;
			F32	fAreaHeight = mSrcRect.extent.y * fTexelHeight;
			
			minX = mSrcRect.point.x * fTexelWidth;
			minY = mSrcRect.point.y * fTexelHeight;
			maxX = minX + fAreaWidth;
			maxY = minY + fAreaHeight;
		}
		else
		{
			Con::warnf("t2dScroller :: Invalid Source Rect (%s) Ignoring (no rendering will happen!)",mImageMapDataBlockName);
		}

		mSrcTexCoords = RectF(minX,minY,maxX,maxY);
	}
	else
	{
		// Fetch Current Frame Area as normal
		// Luma: adding a check for null here. Causes TGB to crash if imagemap was null
		if(mImageMapDataBlock)
		{
			const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameArea( 0 );
			mSrcTexCoords = RectF(frameArea.mX,frameArea.mY,frameArea.mX2,frameArea.mY2);
		}
		else
		{
			//Con::errorf("t2dStaticSprite has NULL imagemap - SourceRects set to 0");
			mSrcTexCoords = RectF();
		}
	}
}

//Luma:	allow on the fly script access to source rect changing
ConsoleMethod( t2dScroller, setSourceRect, void, 6, 6, "(S32 x, S32 y, S32 w, S32 h)"
              "Set the bitmap displayed in the control. Note that it is limited in size, to 256x256.")
{
	if(argc == 6)
	{
		object->mUseSourceRect = true;
		object->mSrcRect.point.x = dAtoi(argv[2]);
		object->mSrcRect.point.y = dAtoi(argv[3]);
		object->mSrcRect.extent.x = dAtoi(argv[4]);
		object->mSrcRect.extent.y = dAtoi(argv[5]);
	}
	else
	{
		object->mUseSourceRect = false;
	}
	object->setSourceTextCoords();
}