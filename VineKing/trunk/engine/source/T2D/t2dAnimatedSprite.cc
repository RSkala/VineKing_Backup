//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Animated Sprite.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dAnimatedSprite.h"


//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dAnimatedSprite);

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dAnimatedSprite::t2dAnimatedSprite() :    T2D_Stream_HeaderID(makeFourCCTag('2','D','A','S')),
                                            mAnimationCallbackComplete(true),
                                            mFrameChangeCallback(false)
{
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dAnimatedSprite::~t2dAnimatedSprite()
{
}

void t2dAnimatedSprite::initPersistFields()
{
   addProtectedField("animationName", TypeString, Offset(mAnimationController.mCurrentAnimationName, t2dAnimatedSprite), &setAnimation, &defaultProtectedGetFn, "");

   Parent::initPersistFields();
}

void t2dAnimatedSprite::copyTo(SimObject* object)
{
   Parent::copyTo(object);

   AssertFatal(dynamic_cast<t2dAnimatedSprite*>(object), "t2dAnimatedSprite::copyTo - Copy object is not a t2dAnimatedSprite!");
   t2dAnimatedSprite* animatedSprite = static_cast<t2dAnimatedSprite*>(object);

   animatedSprite->playAnimation(mAnimationController.getCurrentAnimation(), false);
}

//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dAnimatedSprite::onAdd()
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
void t2dAnimatedSprite::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Handle Pausing
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, pauseAnimation, void, 3, 3, "(bool enable) - Pause the current animation\n"
															 "@param enable If true, pause the animation. If false, continue animating\n")
{
	object->mAnimationController.pauseAnimation(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------
// Set the frame-change callback status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, setFrameChangeCallback, void, 3, 3, "(bool enabled) - Sets the frame-change callback status.\n"
																	 "@param enabled Whether or not to call the onFrameChange callback\n"
																	 "@return No return value.")
{
    // Return Animation Finished Status.
    object->mFrameChangeCallback = dAtob(argv[2]);
}


//-----------------------------------------------------------------------------
// Set the current frame-index.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, setAnimationFrame, void, 3, 3, "(int frame) - Sets the current animation frame.\n"
																"@param frame Which frame of the animation to display\n"
																"@return No return value.")
{
    // Set Animation Frame
    object->mAnimationController.setAnimationFrame( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------
// Is Animation Finished?
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, getIsAnimationFinished, bool, 2, 2, "() - Checks animation status.\n"
																	 "@return (bool finished) Whether or not the animation is finished")
{
    // Return Animation Finished Status.
    return object->mAnimationController.isAnimationFinished();
}


//-----------------------------------------------------------------------------
// Get Animation Name.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, getAnimationName, const char*, 2, 2, "() - Gets current animation.\n"
																	  "@return (string Animation) The current animation")
{
    // Get Current Animation.
    return object->mAnimationController.getCurrentAnimation();
}

ConsoleMethod(t2dAnimatedSprite, getAnimation, const char*, 2, 2, "() - Gets current animation.\n"
																  "@return (string Animation) The current animation")
{
    // Get Current Animation.
    return object->mAnimationController.getCurrentAnimation();
}


//-----------------------------------------------------------------------------
// Get Animation Frame.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, getAnimationFrame, S32, 2, 2, "() - Gets current animation frame.\n"
												    		   "@return (int frame) The current animation frame")
{
    // Get Animation Frame.
    return object->mAnimationController.getCurrentFrame();
}


//-----------------------------------------------------------------------------
// Get Animation Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, getAnimationTime, F32, 2, 2, "() - Gets current animation time.\n"
															  "@return (float time) The current animation time")
{
    // Get Animation Time.
    return object->mAnimationController.getCurrentTime();
}


//-----------------------------------------------------------------------------
// Play Animation.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dAnimatedSprite, setAnimation, void, 3, 3, "(string animationName) - Sets the current animation.\n"
														   "@param animationName The animation to play\n"
															"@return No return value.")
{
   object->playAnimation(argv[2], false, -1, false);
}

ConsoleMethod(t2dAnimatedSprite, playAnimation, bool, 3, 6, "(string animationName, [bool autoRestore], [int frameIndex], [bool mergeTime]) - Plays an animation.\n"
															"@param animationName The animation to play\n"
															"@param autoRestore If true, the previous animation will be played when this new animation finishes.\n"
															"@param frameIndex The animation frame to start playing from.\n"
															"@param mergeTime Where to merge the previous animation with the new one\n"
															"@return Returns true on success.")
{
    // Fetch Auto-Restore Flag.
    const bool autoRestore = (argc >= 4) ? dAtob(argv[3]) : false;
    // Fetch Frame Index.
    const S32 frameIndex = (argc >= 5) ? dAtoi(argv[4]) : -1;
    // Fetch Merge-Time Flag.
    const bool mergeTime = (argc >= 6) ? dAtob(argv[5]) : false;

    // Play Animation.
    return object->playAnimation( argv[2], autoRestore, frameIndex, mergeTime );
}   
// Play Animation.
bool t2dAnimatedSprite::playAnimation( const char* animationName, const bool autoRestore, const S32 frameIndex, const bool mergeTime )
{
    // Reset Animation Callback.
    mAnimationCallbackComplete = false;

    // Play Animation; okay?
    if ( mAnimationController.playAnimation( animationName, autoRestore, frameIndex, mergeTime ) )
    {
        // Yes, so do script callback.
       if( isMethod( "onAnimationStart" ) )
         Con::executef( this, 1, "onAnimationStart" );
        // Return Okay.
        return true;
    }
    else
    {
        // No, so return error.
        return false;
    }  
}

//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dAnimatedSprite::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( sceneTime, elapsedTime, pDebugStats );

    // Has the Animation Finished?
    if ( mAnimationController.isAnimationFinished() )
    {
        // Yes, so is the animation callback complete?
        if ( !mAnimationCallbackComplete )
        {
            // No, so set Animation Callback Complete.
            mAnimationCallbackComplete = true;
            // Do script callback.
            //Con::executef( this, 1, "onAnimationEnd" ); // RKS: I disabled this script call
        }
    }
    else
    {
        // Is animation controller valid?
        if ( !mAnimationController.getIsValid() )
        {
            // No, so finish here.
            return;
        }

        // No, so update Animation.
        const bool frameChanged = mAnimationController.updateAnimation( elapsedTime );

        // Are we using frame-change callback and the frame has changed?
        if ( mFrameChangeCallback && frameChanged )
        {
            // Yes ...

            // Argument Buffer.
            static char argBuffer[16];
            // Format Event-Modifier Buffer.
            dSprintf(argBuffer, 16, "%d", mAnimationController.getCurrentFrame() );
            // Call Scripts.
            if( isMethod( "onFrameChange" ) )
               Con::executef(this, 2, "onFrameChange", argBuffer);
        }
    }
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dAnimatedSprite::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Cannot render without valid animation-controller image-map!
    if ( !mAnimationController.getIsImageMapValid() )
        return;

    // Setup Texturing.
    glEnable        ( GL_TEXTURE_2D );

    // Fetch frame area and bind appropriate texture page.
    const t2dImageMapDatablock::cFrameTexelArea& frameArea = mAnimationController.getCurrentFrameAreaBind();
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Fetch Positions.
    const F32& minX = frameArea.mX;
    const F32& minY = frameArea.mY;
    const F32& maxX = frameArea.mX2;
    const F32& maxY = frameArea.mY2;

    // Set Blend Options.
    setBlendOptions();
#ifdef TORQUE_OS_IPHONE
	//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
	dglDrawTextureQuadiPhone(mWorldClipBoundary[0].mX, mWorldClipBoundary[0].mY, mWorldClipBoundary[1].mX, mWorldClipBoundary[1].mY, mWorldClipBoundary[3].mX, mWorldClipBoundary[3].mY, mWorldClipBoundary[2].mX, mWorldClipBoundary[2].mY, 
								minX, minY, maxX, minY, minX, maxY, maxX, maxY );
#else

    // Draw Object.
    glBegin(GL_QUADS);
        glTexCoord2f( minX, minY );
        glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[0]) );
        glTexCoord2f( maxX, minY );
        glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[1]) );
        glTexCoord2f( maxX, maxY );
        glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[2]) );
        glTexCoord2f( minX, maxY );
        glVertex2fv ( (GLfloat*)&(mWorldClipBoundary[3]) );
    glEnd();
#endif
    // Disable Texturing.
    glDisable       ( GL_TEXTURE_2D );

    // Call Parent.
    Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dAnimatedSprite )
    REGISTER_SERIALISE_VERSION( t2dAnimatedSprite, 4, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dAnimatedSprite, 4 )


//-----------------------------------------------------------------------------
// Load v4
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dAnimatedSprite, 4 )
{
    // Read Misc Data.
    if ( !stream.read( &object->mAnimationCallbackComplete ) )
        return false;

    // Read Animation Controller.
    return object->mAnimationController.loadStream( T2D_SERIALISE_LOAD_ARGS_PASS );
}

//-----------------------------------------------------------------------------
// Save v4
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dAnimatedSprite, 4 )
{
    // Write Misc Data.
    if ( !stream.write( object->mAnimationCallbackComplete ) )
        return false;

    // Write Animation Controller.
    return object->mAnimationController.saveStream( T2D_SERIALISE_SAVE_ARGS_PASS );
}
