//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Static Sprite.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dStaticSprite.h"
#include "core/stringBuffer.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dStaticSprite);

// - Mat I think this is unused 
//t2dQuadBatch t2dStaticSprite::sStaticSpriteBatch;


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dStaticSprite::t2dStaticSprite() :    T2D_Stream_HeaderID(makeFourCCTag('2','D','S','S')),
                                        mImageMapDataBlock(NULL),
                                        mImageMapDataBlockName(NULL),
                                        mFrame(0)
{
	mUseSourceRect = false;
	mSrcRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
	mSrcTexCoords = RectF(0.0f, 0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dStaticSprite::~t2dStaticSprite()
{
}

void t2dStaticSprite::initPersistFields()
{
   addProtectedField("imageMap", TypeString, Offset(mImageMapDataBlockName, t2dStaticSprite), &setImageMap, &defaultProtectedGetFn, "");
   addProtectedField("frame", TypeS32, Offset(mFrame, t2dStaticSprite), &setFrame, &defaultProtectedGetFn, "");

   addField("mUseSourceRect", TypeBool, Offset(mUseSourceRect, t2dStaticSprite));
   addField("sourceRect", TypeRectF, Offset(mSrcRect, t2dStaticSprite));

   Parent::initPersistFields();

   // Bad place for this, but submit the static sprite quad batch to the batcher
   // - Mat I think this is unused 
   //t2dQuadBatch::submitBatch( &sStaticSpriteBatch );
}

//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dStaticSprite::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!


    // Call Parent.
    if(!Parent::onAdd())
        return false;

	//Luma: consolidate this into a single location
	setSourceTextCoords();
   
    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dStaticSprite::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Set ImageMap.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dStaticSprite, setImageMap, bool, 3, 4, "(string imageMapName, [int frame]) - Sets imageMap/Frame.\n"
														"@param imageMapName The imagemap to display\n"
														"@param frame The frame of the imagemap to display\n"
														"@return Returns true on success.")
{
    // Calculate Frame.
    U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;

    // Set ImageMap.
    return object->setImageMap( argv[2], frame );
}   
// Set ImageMap/Frame.
bool t2dStaticSprite::setImageMap( const char* imageMapName, U32 frame )
{
    // Invalid ImageMap Name.
    if ( !imageMapName || imageMapName == StringTable->insert("") )
        return false;
	
	// RKS:  I had to add this hack because after removing the Level-specific datablocks, I can no longer have unique Water imagemaps

    // Find ImageMap Datablock.
    t2dImageMapDatablock* pImageMapDataBlock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject( imageMapName ));

    // Set Datablock.
    if ( !t2dCheckDatablock( pImageMapDataBlock ) )
    {
        // Warn.
        Con::warnf("t2dStaticSprite::setImageMap() - t2dImageMapDatablock Datablock is invalid! (%s)", imageMapName);
        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= pImageMapDataBlock->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dStaticSprite::setImageMap() - Invalid Frame #%d for t2dImageMapDatablock Datablock! (%s)", frame, imageMapName);
        // Return Here.
        return false;
    }

    // Set ImageMap Datablock.
    mImageMapDataBlockName = StringTable->insert(imageMapName);
    mImageMapDataBlock = pImageMapDataBlock;
    
	//Luma: reset the source rects for new image map
	setSourceTextCoords();

    // Set Frame.
    mFrame = frame;

    // Return Okay.
    return true;
}




//-----------------------------------------------------------------------------
// Set ImageMap Frame.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dStaticSprite, setFrame, bool, 3, 3, "(int frame) - Sets imageMap frame.\n"
													 "@param frame The frame to display\n"
													 "@return Returns true on success.")
{
    // Set ImageMap Frame.
    return object->setFrame( dAtoi(argv[2]) );
}   
// Set ImageMap/Frame.
bool t2dStaticSprite::setFrame( U32 frame )
{
    // Check Existing ImageMap.
    if ( !mImageMapDataBlock )
    {
        // Warn.
        Con::warnf("t2dStaticSprite::setFrame() - Cannot set Frame without existing t2dImageMapDatablock Datablock!");
        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= mImageMapDataBlock->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dStaticSprite::setFrame() - Invalid Frame #%d for t2dImageMapDatablock Datablock! (%s)", frame, getIdString());
        // Return Here.
        return false;
    }

    // Set Frame.
    mFrame = frame;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Get ImageMap Name.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dStaticSprite, getImageMap, const char*, 2, 2, "() - Gets current imageMap name.\n"
															   "@return (string imageMapName) The imagemap being displayed")
{
    // Get ImageMap Name.
    return object->getImageMapName();
}   


ConsoleMethod(t2dStaticSprite, getUseSourceRect, const char*, 2, 2, "() - Gets whether the source rects are used in this sprite.\n"
															   "@return (bool) YES or NO")
{
    //Return correct values
	if(object->mUseSourceRect)
	{
		return "YES";
	}
	else
	{
		return "NO";
	}
}   

ConsoleMethod(t2dStaticSprite, setUseSourceRect, void, 3, 3, "() - Sets whether the source rects are used in this sprite.\n"
			  "@return (void))")
{
	if(dStrcmp("YES", argv[2]) == 0)
	{
		object->mUseSourceRect = true;
		object->setSourceTextCoords();
	}
	else
	{
		object->mUseSourceRect = false;
	}
}   


ConsoleMethod(t2dStaticSprite, getSpriteSourceRect, const char*, 2, 2, "() - Gets the source rect of this sprite.\n"
															   "@return (four component rect) X Y Width Height")
{
	RectF source = object->mSrcRect;
    //Return correct values
	StringBuffer value;
	char topleft[4];
	char topright[4];
	char width[4];
	char height[4];

	dItoa(source.point.x, topleft);
	dItoa(source.point.y, topright);
	dItoa(source.extent.x, width);
	dItoa(source.extent.y, height);

	value.append(topleft);
	value.append(" ");
	value.append(topright);
	value.append(" ");
	value.append(width);
	value.append(" ");
	value.append(height);
	
	const char* values = value.createCopy8();

	return values;
}   

ConsoleMethod(t2dStaticSprite, setSpriteSourceRect, void, 3, 3, "() - Sets the source rect used in this sprite.\n"
			  "@return (void))")
{
	if(dStrcmp("", argv[2]) != 0)
	{
		//if we have a source rect (must be 4)
		 F32 x,y,w,h;
		S32 args = dSscanf(argv[2], "%g %g %g %g", &x, &y, &w, &h);
		if(args == 4)
		{
			RectF tmp = RectF(x, y, w, h);
			if(!tmp.isValidRect())
			{
				Con::warnf("t2dStaticSprite - Not a valid source rect! Try x y width height (must be valid aswell)");
			}
			else
			{
				//we have a cool rect, 
				object->mSrcRect = tmp;
				object->setSourceTextCoords();
			}
		}
		else
		{
			Con::warnf("t2dStaticSprite - Not a valid source rect! Try x y width height");
		}
	}
	else
	{
		Con::warnf("t2dStaticSprite - Not a valid source rect! Try x y width height");
	}
}   

//-----------------------------------------------------------------------------
// Get ImageMap Frame.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dStaticSprite, getFrame, S32, 2, 2, "() - Gets current imageMap Frame.\n"
													"@return (int frame) The frame currently being displayed")
{
    // Get ImageMap Frame.
    return object->getFrame();
}   

void t2dStaticSprite::copyTo(SimObject* object)
{
   AssertFatal(dynamic_cast<t2dStaticSprite*>(object), "t2dStaticSprite::copyTo - Copy object is not a t2dStaticSprite!");
   t2dStaticSprite* staticSprite = static_cast<t2dStaticSprite*>(object);
   staticSprite->setImageMap(getImageMapName(), getFrame());

   Parent::copyTo(object);
}

void renderQuadAsStrip();

#ifdef TORQUE_OS_IPHONE
//-Mat for speedy Quad rendering
extern t2dSceneWindow *gCurrentSceneWindow;
//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dStaticSprite::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Cannot render without Texture.
    if ( !mImageMapDataBlock ) {
		return;
	}

    // Insert this sprite into the render list instead
	//-Mat looks like this is unused see comment from -pw
    if( 0) { //false && getSceneGraph() && getSceneGraph()->getCurrentRenderWindow() ) // Disable this for now -pw
      //sStaticSpriteBatch.submitQuad( &mWorldClipBoundary[0], mImageMapDataBlock, mFrame );
    }
	else
    {
		// Bind Texture.
       glEnable        ( GL_TEXTURE_2D );
       mImageMapDataBlock->bindImageMapFrame( mFrame );
	//PUAP -Mat use GL_MODULATE to tint, GL_REPLACE for texture only
       glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

       // Set Blend Options.
       setBlendOptions();

		//handle non-source rect rendering
		if(!mUseSourceRect)
		{
		   // Fetch Current Frame Area as normal
		   const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameArea( mFrame );
		   mSrcTexCoords = RectF(frameArea.mX,frameArea.mY,frameArea.mX2,frameArea.mY2);
		}

		//make sure we are on the device and NOT rotating
		//-Mat removedif( TARGET_IPHONE_SIMULATOR || !( mIsZero(getParentPhysics().mRotation) )  ) {
		//now to draw to the screen

		//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
 	    dglDrawTextureQuadiPhone(mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY, mWorldClipBoundary[1].mX, mWorldClipBoundary[1].mY, mWorldClipBoundary[3].mX, mWorldClipBoundary[3].mY, mWorldClipBoundary[2].mX, mWorldClipBoundary[2].mY, 
									mSrcTexCoords.point.x, mSrcTexCoords.point.y, mSrcTexCoords.extent.x, mSrcTexCoords.point.y, mSrcTexCoords.point.x, mSrcTexCoords.extent.y, mSrcTexCoords.extent.x, mSrcTexCoords.extent.y );
        // Disable Texturing.
        glDisable       ( GL_TEXTURE_2D );
    }

    glDisable       ( GL_BLEND );
    // Call Parent.
    Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}

#else

void t2dStaticSprite::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Cannot render without Texture.
    if ( !mImageMapDataBlock )
        return;

    // Insert this sprite into the render list instead
// - Mat I think this is unused 
//    if( false && getSceneGraph() && getSceneGraph()->getCurrentRenderWindow() ) // Disable this for now -pw
//		sStaticSpriteBatch.submitQuad( &mWorldClipBoundary[0], mImageMapDataBlock, mFrame );
//    else
     {
       // Bind Texture.
       glEnable        ( GL_TEXTURE_2D );
       mImageMapDataBlock->bindImageMapFrame( mFrame );
       glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

       // Set Blend Options.
       setBlendOptions();

		//handle non-source rect rendering
		if(!mUseSourceRect)
		{
		   // Fetch Current Frame Area as normal
		   const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameArea( mFrame );
		   mSrcTexCoords = RectF(frameArea.mX,frameArea.mY,frameArea.mX2,frameArea.mY2);
		}

	   //render with new method that takes source rect usage into account
       glBegin(GL_QUADS);
         glTexCoord2f( mSrcTexCoords.point.x, mSrcTexCoords.point.y );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[0]) );
         glTexCoord2f( mSrcTexCoords.extent.x, mSrcTexCoords.point.y );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[1]) );
         glTexCoord2f( mSrcTexCoords.extent.x, mSrcTexCoords.extent.y );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[2]) );
         glTexCoord2f( mSrcTexCoords.point.x, mSrcTexCoords.extent.y );
         glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[3]) );
       glEnd();

       // Disable Texturing.
       glDisable       ( GL_TEXTURE_2D );
    }

    // Call Parent.
    Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}
#endif

//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dStaticSprite )
    REGISTER_SERIALISE_VERSION( t2dStaticSprite, 3, false )
REGISTER_SERIALISE_END()

// Implement Parent  Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dStaticSprite, 3 )


//-----------------------------------------------------------------------------
// Load v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dStaticSprite, 3 )
{
    U32                     frame;
    bool                    imageMapFlag;
    char                    imageMapName[256];

    // Read Ad-Hoc Info.
    if ( !stream.read( &imageMapFlag ) )
        return false;

    // Do we have an imageMap?
    if ( imageMapFlag )
    {
        // Yes, so read ImageMap Name.
        stream.readString( imageMapName );

        // Read Frame.
        if  ( !stream.read( &frame ) )
            return false;

        // Set ImageMap/Frame.
        object->setImageMap( imageMapName, frame );
    }

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dStaticSprite, 3 )
{
    // Ad-Hoc Info.
    if ( object->mImageMapDataBlock )
    {
        // Write ImageMap Datablock Name.
        if ( !stream.write( true ) )
            return false;

        // Write ImageMap Datablock Name.
        stream.writeString( object->mImageMapDataBlock->getName() );

        // Write Frame.
        if  ( !stream.write( object->mFrame ) )
            return false;
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
void t2dStaticSprite::setSourceTextCoords(void)
{
   
   // Luma: adding a check for null here. Causes TGB to crash if imagemap was null
   if(!mImageMapDataBlock)
   {
      //Con::errorf("t2dStaticSprite has NULL imagemap - SourceRects set to 0");
	   mSrcRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
       mSrcTexCoords = RectF(0.0f, 0.0f, 0.0f, 0.0f);
      return;
   }
   
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
			Con::warnf("t2dStaticSprite :: Invalid Source Rect (%s) Ignoring (no rendering will happen!)",mImageMapDataBlockName);
		}

		mSrcTexCoords = RectF(minX,minY,maxX,maxY);
	}
	else
	{
		// Fetch Current Frame Area as normal
      const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameArea( 0 );
      mSrcTexCoords = RectF(frameArea.mX,frameArea.mY,frameArea.mX2,frameArea.mY2);
	}
}

//Luma:	allow on the fly script access to source rect changing
ConsoleMethod( t2dStaticSprite, setSourceRect, void, 6, 6, "(S32 x, S32 y, S32 w, S32 h)"
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
