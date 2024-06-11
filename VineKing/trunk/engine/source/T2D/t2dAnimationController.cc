//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Animation Controller/Datablock Object.
//-----------------------------------------------------------------------------

#include "dgl/gBitmap.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dSceneObject.h"
#include "./t2dAnimationController.h"

//------------------------------------------------------------------------------


IMPLEMENT_CO_DATABLOCK_V1(t2dAnimationDatablock);


//------------------------------------------------------------------------------

t2dAnimationDatablock::t2dAnimationDatablock() :    mImageMapDataBlock(NULL),
                                                    mImageMapName(StringTable->insert("")),
                                                    mAnimationFramesString(StringTable->insert("")),
                                                    mAnimationTime(1.0f),
                                                    mAnimationCycle(true),
                                                    mRandomStart(false),
                                                    mStartFrame(0),
													mPingPong(false),
													mPlayForward(true)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mFramesList );
}

//------------------------------------------------------------------------------

t2dAnimationDatablock::~t2dAnimationDatablock()
{
}

//------------------------------------------------------------------------------

void t2dAnimationDatablock::initPersistFields()
{
    Parent::initPersistFields();

    // Datablock.
    addField("imageMap",            TypeString, Offset(mImageMapName,           t2dAnimationDatablock));

    // Fields.
    addField("animationFrames",     TypeString, Offset(mAnimationFramesString,  t2dAnimationDatablock));
    addField("animationTime",       TypeF32,    Offset(mAnimationTime,          t2dAnimationDatablock));
    addField("animationCycle",      TypeBool,   Offset(mAnimationCycle,         t2dAnimationDatablock));
    addField("randomStart",         TypeBool,   Offset(mRandomStart,            t2dAnimationDatablock));
    addField("startFrame",          TypeS32,    Offset(mStartFrame,             t2dAnimationDatablock));

	//Luma: animation feature additions
	addField("pingPong",			TypeBool,   Offset(mPingPong,				t2dAnimationDatablock));
	addField("playForward",         TypeBool,   Offset(mPlayForward,            t2dAnimationDatablock));
}

//------------------------------------------------------------------------------

bool t2dAnimationDatablock::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Invalidate Datablock.
    setIsValid(false);

    // Cast the Datablock.
    mImageMapDataBlock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(mImageMapName));

    // Transfer Datablock (if we've got one).
    if ( !t2dCheckDatablock( mImageMapDataBlock ) )
    {
        // Error.
        Con::warnf("t2dAnimationDatablock::onAdd() - ImageMap Datablock is invalid, cannot create animation! (%s)", mImageMapDataBlock?mImageMapDataBlock->getName():"UNKNOWN!");
        // Reset ImageMap Datablock.
        mImageMapDataBlock = NULL;
        // Return Error.
        return false;
    }
    
    // Calculate Animation.
    calculateAnimation();

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

void t2dAnimationDatablock::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void t2dAnimationDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
}

//------------------------------------------------------------------------------

void t2dAnimationDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);

    // Read Datablock.
}

//------------------------------------------------------------------------------

void t2dAnimationDatablock::calculateAnimation(void)
{
    // ***********************************************************
    // Setup Frames List.
    // ***********************************************************

    // Clear Animation Frames.
    mFramesList.clear();

    // Do we have a valid ImageMap?
    if ( !mImageMapDataBlock )
        // No, finish here.
        return;

    // Fetch Image-Map Area Count.
    const U32 imageMapAreaCount = mImageMapDataBlock->getImageMapFrameCount();

    // Fetch Animation Frame Count.
    const U32 animationFrameCount = t2dSceneObject::getStringElementCount( mAnimationFramesString );

    // Any Frames Specified?
    if ( animationFrameCount > 0 )
    {
        // Yes, so check each frame.
        for ( U32 i = 0; i < animationFrameCount; i++ )
        {
            // Fetch Frame Index.
            U32 frameIndex = dAtoi( t2dSceneObject::getStringElement( mAnimationFramesString, i ));
            // Valid Frame?
            if ( frameIndex >= 0 && frameIndex < imageMapAreaCount )
            {
                // Yes, so add frame to list.
                mFramesList.push_back( frameIndex );
            }
            else
            {
                // No, so skip frame and warn!
                Con::printf("t2dAnimationDatablock::calculateAnimation() - Skipping invalid frame #%d for animation (%s) using imageMap (%s) which only has frame(s) #0 to #%d!", frameIndex, getName(), mImageMapDataBlock->getName(), mImageMapDataBlock->getImageMapFrameCount()-1);
            }
        }

        // Have we got some valid frames?
        if ( mFramesList.size() == 0 )
        {
            // No, so warn.
            Con::warnf("t2dAnimationController::calculateAnimation() - Invalid t2dAnimationDatablock datablock (%s) - Animation has no valid frames!", getName());
            // Finish here.
            return;
        }
    }
    else
    {
        // No, so use all the frames from the imageMap.
        for ( U32 i = 0; i < imageMapAreaCount; i++ )
            mFramesList.push_back( i );
    }

    // Calculate Animation Integration.
    mAnimationIntegration = mAnimationTime / F32(mFramesList.size());

    // Validate Datablock.
    setIsValid(true);
}

ConsoleMethod( t2dAnimationDatablock, calculateAnimation, void, 2, 2, "() Calculates the animation based on the number of frames and the timesteps."
			  "@return No return value.")
{
   object->calculateAnimation();
}


//-----------------------------------------------------------------------------
// Animation Controller 2D.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dAnimationController::t2dAnimationController() :  T2D_Stream_HeaderID(makeFourCCTag('2','D','A','C')),
                                                    mConfigDataBlock(NULL),
                                                    mLocalSerialiseID(1),
                                                    mLastAnimationName(StringTable->insert("")),
                                                    mCurrentAnimationName(StringTable->insert("")),
                                                    mCurrentImageMapDataBlock(NULL),
                                                    mCurrentFrameTextureHandle(NULL),
                                                    mCurrentFrameIndex(0),
                                                    mLastFrameIndex(0),
                                                    mMaxFrameIndex(0),
                                                    mCurrentTime(0.0f),
                                                    mTotalIntegrationTime(0.0f),
                                                    mFrameIntegrationTime(0.0f),
                                                    mAutoRestoreAnimation(false),
                                                    mAnimationFinished(true),
                                                    mLastFrameError(0),
													mAnimationPaused(false)
{
	m_bAnimationPaused = false; // RKS: I added this
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dAnimationController::~t2dAnimationController()
{
}



//-----------------------------------------------------------------------------
// Play Animation.
//-----------------------------------------------------------------------------
bool t2dAnimationController::playAnimation( const char* animationName, const bool autoRestore, S32 startFrame, const bool mergeTime )
{
    // Cannot play empty animation name!
    if ( dStrlen( animationName ) == 0 )
        return false;

    // Find Animation Datablock.
    t2dAnimationDatablock* pAnimationDataBlock = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject( animationName ));

    // Check Datablock (if we've got one).
    if ( !t2dCheckDatablock( pAnimationDataBlock ) )
    {
        Con::warnf("t2dAnimationController::playAnimation() - Invalid t2dAnimationDatablock datablock (%s)", animationName);
        return false;
    }

    // Check we've got some frames.
    if ( pAnimationDataBlock->mFramesList.size() == 0 )
    {
        Con::warnf("t2dAnimationController::playAnimation() - Cannot play t2dAnimationDatablock datablock (%s) - Animation has no frames!", animationName);
        return false;
    }

    // Check ImageMap in Datablock.
    if ( !t2dCheckDatablock(pAnimationDataBlock->mImageMapDataBlock ) )
    {
        Con::warnf("t2dAnimationController::playAnimation() - Animation doesn't have a valid imageMap datablock! (%s)", animationName);
        return false;
    }

    // Merge Start-Time.
    F32 mergeNormalisedStartTime = -1.0f;

    // Do we already have an animation selected?
    if ( t2dCheckDatablock( mConfigDataBlock ) )
    {
        // Yes, so are we merging time?
        if ( mergeTime )
        {
            // Yes, so calculate merge start-time...
            mergeNormalisedStartTime = mCurrentTime / mTotalIntegrationTime;
        }

		//if( dStricmp( mCurrentAnimationName, "BossEye_HIT" ) == 0 ) // RKS: I am trying to see if doing the StringTable->insert is a WASTE (should do it on load or something)
		//{
		//	int aaaa = 0;
		//	aaaa++;
		//}
        // Note last animation.
        mLastAnimationName = StringTable->insert(mCurrentAnimationName);

        // Reset Last Image-Map Datablock (unlock).
        mCurrentImageMapDataBlock = NULL;
    }

    // Store Current Animation Name.
    mCurrentAnimationName = StringTable->insert(animationName);

    // Set Config Animation Datablock.
    mConfigDataBlock = pAnimationDataBlock;

    // Set Current Image-Map Datablock (locked).
    mCurrentImageMapDataBlock = mConfigDataBlock->mImageMapDataBlock;

    // Set Maximum Frame Index.
    mMaxFrameIndex = mConfigDataBlock->mFramesList.size()-1;

    // Calculate Frame Integration Time.
    mFrameIntegrationTime = mConfigDataBlock->mAnimationTime / mConfigDataBlock->mFramesList.size();
    // Calculate Total Integration Time.
    mTotalIntegrationTime = mConfigDataBlock->mAnimationTime;

    // Are we merging time and we calculated one?
    if ( mergeTime && mGreaterThanOrEqual(mergeNormalisedStartTime,0.0f) )
    {
        // Yes, so calculate merged time.
        mCurrentTime = mTotalIntegrationTime * mergeNormalisedStartTime;
    }
    else
    {
        // No, so are we using a function-Specified start-frame?
        if ( startFrame != -1 )
        {
            // Yes, so valid start frame?
            if ( startFrame < 0 || startFrame > mMaxFrameIndex )
            {
                // No, so warn.
                Con::warnf("t2dAnimationController::playAnimation() - Specified Start-Frame Invalid! (frame#%d of %d in %s)", startFrame, mMaxFrameIndex, mCurrentAnimationName);
                // Reset Start Frame.
                startFrame = 0;
            }

            // Calculate start-frame time.
            mCurrentTime = startFrame * mFrameIntegrationTime;
        }
        else
        {
            // No, so random Start?
            if ( pAnimationDataBlock->mRandomStart )
            {
                // Yes, so calculate start time.
                mCurrentTime = mGetT2DRandomF(0.0f, mTotalIntegrationTime*0.999f);
            }
            else
            {
                // No, so valid Start Frame?
                if ( mConfigDataBlock->mStartFrame < 0 || mConfigDataBlock->mStartFrame > mMaxFrameIndex )
                {
                    // No, so warn.
                    Con::warnf("t2dAnimationController::playAnimation() - Animation Start-Frame Invalid; Starting at frame#0! (frame#%d of %d in %s)", mConfigDataBlock->mStartFrame, mMaxFrameIndex, mCurrentAnimationName);
                    // Reset Start Frame.
                    mConfigDataBlock->mStartFrame = 0;
                }

                // Calculate start-frame time.
                mCurrentTime = mConfigDataBlock->mStartFrame * mFrameIntegrationTime;
            }
        }
    }

    // Reset Last Frame Error.
    mLastFrameError = 0;

    // Reset Last Frame Index.
    mLastFrameIndex = -1;

    // Set Auto Restore Animation Flag.
    mAutoRestoreAnimation = autoRestore;

    // Reset Animation Finishd Flag.
    mAnimationFinished = false;

    // Do an initial animation update.
    updateAnimation(0.0f);

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Update Animation.
//-----------------------------------------------------------------------------
bool t2dAnimationController::updateAnimation( const F32 elapsedTime )
{
    // We *must* have a valid datablock and be still animating!
    if ( !getIsValid() )
        return false;
	
	//if( m_bAnimationPaused == false ) // RKS: I added this so we can pause animations! (NOTE: This was causing animations to play at double speed, since it's handled below
	//{
	//	// Update Current Time.
	//	mCurrentTime += elapsedTime;
	//}

	//Luma: animation feature additions
	if ( mConfigDataBlock->mPingPong )
	{
		if ( mConfigDataBlock->mPlayForward )
		{
			// Update Current Time.
			mCurrentTime += elapsedTime;

			if ( mCurrentTime > mTotalIntegrationTime)
			{
				mCurrentTime = mTotalIntegrationTime;
			}
		}
		else
		{
			// Update Current Time.
			mCurrentTime -= elapsedTime;
		}
	}
	else if( m_bAnimationPaused == false ) // RKS: I added this so we can pause animations!
	{
		//if( m_bAnimationPaused == false ) // RKS: I added this so we can pause animations!
		{
			// Update Current Time.
			mCurrentTime += elapsedTime; // RKS:  MOVE THE ANIMATION PAUSED THING TO HERE!!
		}
    }
	else if(!mAnimationPaused) // Added pausing capability -mperry
	{
		// Update Current Time.
		mCurrentTime += elapsedTime;
	}


    // Check if the animation has finished.
    if ( !mConfigDataBlock->mAnimationCycle && mGreaterThanOrEqual(mCurrentTime, mTotalIntegrationTime) )
    {
        // Animation has finished.
        mAnimationFinished = true;

        // Are we restoring the animation?
        if ( mAutoRestoreAnimation )
        {
            // Yes, so play last animation.
            playAnimation( mLastAnimationName, false );
        }
        else
        {
            // No, so fix Animation at end of frames.
            mCurrentTime = mTotalIntegrationTime - (mFrameIntegrationTime * 0.5);
        }
    }

    // Update Current Mod Time.
    mCurrentModTime = mFmod( mCurrentTime, mTotalIntegrationTime );

    // Calculate Current Frame.
	//Luma: animation feature additions
	if ( mGreaterThanOrEqual(mCurrentTime, mTotalIntegrationTime) && mConfigDataBlock->mPingPong )
	{	
		mCurrentFrameIndex = mMaxFrameIndex;
	}	
	else
	{
		mCurrentFrameIndex = (S32)(mCurrentModTime / mFrameIntegrationTime);
	}

    // Check Current Frame Index.
    if ( mCurrentFrameIndex < 0 || mCurrentFrameIndex > mMaxFrameIndex )
    {
		//Luma: animation feature additions
		if ( mConfigDataBlock->mPingPong )
		{
			if ( mCurrentFrameIndex < 0 )
			{
				mConfigDataBlock->mPlayForward = true;
			}
			else if ( mCurrentFrameIndex > mMaxFrameIndex )
			{
				mConfigDataBlock->mPlayForward = false;
			}
		}
		else
		{
			// Have we already warned of this frame?
			if ( mCurrentFrameIndex != mLastFrameError )
			{
				// No, so warn.
				Con::warnf("t2dAnimationController::updateAnimation() - Invalid Frame#%d Selected for Animation (%s)", mCurrentFrameIndex, mConfigDataBlock->getName());
				// Note this frame error.
				mLastFrameError = mCurrentFrameIndex;
			}
			// Finish here without updating.
			return false;
		}
    }

    // Fetch Current Frame Texture Handle.
    mCurrentFrameTextureHandle = mConfigDataBlock->mImageMapDataBlock->getImageMapFrameTexture( mConfigDataBlock->mFramesList[mCurrentFrameIndex] );

    // Calculate Current Frame Area.
    mCurrentFrameArea = mConfigDataBlock->mImageMapDataBlock->getImageMapFrameArea( mConfigDataBlock->mFramesList[mCurrentFrameIndex] );
    mCurrentFramePixelArea = mConfigDataBlock->mImageMapDataBlock->getImageMapFramePixelArea( mConfigDataBlock->mFramesList[mCurrentFrameIndex] );

    // Calculate if frame has changed.
    bool frameChanged = (mCurrentFrameIndex != mLastFrameIndex);

    // Reset Last Frame.
    mLastFrameIndex = mCurrentFrameIndex;

	//Luma: animation feature additions
	if ( mGreaterThanOrEqual(mCurrentTime, mTotalIntegrationTime) || mLessThan( mCurrentTime, 0 ) )
	{
		if (  mConfigDataBlock->mPingPong )
		{
			if ( mConfigDataBlock->mPlayForward )
			{
				mConfigDataBlock->mPlayForward = false;
			}
			else
			{
				mConfigDataBlock->mPlayForward = true;
				mCurrentTime = 0.0f;
			}
		}
	}

    // Return Frame-Changed Flag.
    return frameChanged;
}


//-----------------------------------------------------------------------------
// Reset Time.
//-----------------------------------------------------------------------------
void t2dAnimationController::resetTime( void )
{
    // Rest Time.
    mCurrentTime = 0.0f;
}

//======================================================================================================================================================
// RKS -- This is to force

void t2dAnimationController::SetLastAnimationName( const char* pszAnimationName )
{
	if( pszAnimationName == NULL )
		return;
	
	//strcpy( mLastAnimationName, pszAnimationName );
	//dStrcpy( const_cast<char*>( mLastAnimationName ), pszAnimationName );
	mLastAnimationName = StringTable->insert( pszAnimationName );
}

//======================================================================================================================================================


//-----------------------------------------------------------------------------
// Set Animation Frame.
//-----------------------------------------------------------------------------
void t2dAnimationController::setAnimationFrame( const U32 frameIndex )
{
    // We *must* have a valid datablock and be still animating!
    if ( !getIsValid() )
    {
        // Warn.
        Con::warnf("t2dAnimationController::setAnimationFrame() - Cannot set frame; animation is finished or is invalid!");
        return;
    }

    // Validate Frame Index?
    if ( frameIndex < 0 || frameIndex > mMaxFrameIndex )
    {
        // No, so warn.
        Con::warnf("t2dAnimationController::setAnimationFrame() - Animation Frame-Index Invalid (frame#%d of %d in %s)", frameIndex, mMaxFrameIndex, mCurrentAnimationName);
        // Finish here.
        return;
    }

    // Calculate current time.
    mCurrentTime = frameIndex*mFrameIntegrationTime;

    // Do an immediate animation update.
    updateAnimation(0.0f);
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dAnimationController )
    REGISTER_SERIALISE_VERSION( t2dAnimationController, 1, false )
REGISTER_SERIALISE_END()

// Implement Leaf Serialisation.
IMPLEMENT_T2D_SERIALISE_LEAF( t2dAnimationController, 1 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dAnimationController, 1 )
{
    StringTableEntry                    currentAnimationName;
    bool                                autoRestoreAnimation;

    // Read Animation Finished Flag.
    if ( !stream.read( &object->mAnimationFinished ) )
        return false;

    // Is the Animation Finished?
    if ( !object->mAnimationFinished )
    {
        // UNUSED: JOSEPH THOMAS -> char    animationName[256];

        // No, so read Auto-Restore Animation Flag.
        if  ( !stream.read( &autoRestoreAnimation ) )
            return false;

        // Are we using Auto-Restore Animation?
        if ( autoRestoreAnimation )
            // Yes, so read Last Animation Name.
            object->mLastAnimationName = stream.readSTString();

        // Read Current Animation Name.
        currentAnimationName = stream.readSTString();

        // Play Animation.
        object->playAnimation( currentAnimationName, autoRestoreAnimation );
    }

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dAnimationController, 1 )
{
    // Write Animation Finished Flag.
    if ( !stream.write( object->mAnimationFinished ) )
        return false;

    // Animation Finished?
    if ( !object->mAnimationFinished )
    {
        // No, so write Auto-Restore Animation Flag.
        if ( !stream.write( object->mAutoRestoreAnimation ) )
            return false;

        // write Last Animation Name if we're using Auto-Restore?
        if ( object->mAutoRestoreAnimation )
            stream.writeString( object->mLastAnimationName );

        // Write Current Animation Name.
        stream.writeString( object->mCurrentAnimationName );
    }

    // Return Okay.
    return true;
}
