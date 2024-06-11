//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Animation Controller/Datablock Object.
//-----------------------------------------------------------------------------

#ifndef _T2DANIMATIONCONTROLLER_H_
#define _T2DANIMATIONCONTROLLER_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif

#ifndef _T2DIMAGEMAPDATABLOCK_H_
#include "./t2dImageMapDatablock.h"
#endif

#ifndef _T2DBASEDATABLOCK_H_
#include "./t2dBaseDatablock.h"
#endif

#ifndef _T2DSERIALISATION_H_
#include "./t2dSerialise.h"
#endif

///-----------------------------------------------------------------------------
/// Forward Declarations.
///-----------------------------------------------------------------------------
class t2dAnimationController;


///-----------------------------------------------------------------------------
/// Animation Datablock 2D.
///-----------------------------------------------------------------------------
class t2dAnimationDatablock : public t2dBaseDatablock
{
friend class t2dAnimationController;

private:
    typedef t2dBaseDatablock Parent;

    StringTableEntry        mImageMapName;
    SimObjectPtr<t2dImageMapDatablock> mImageMapDataBlock;

    StringTableEntry        mAnimationFramesString;
    bool                    mAnimationCycle;
    bool                    mRandomStart;
    S32                     mStartFrame;
    F32                     mAnimationTime;

    F32                     mAnimationIntegration;
    Vector<U32>             mFramesList;

	//Luma: animation feature additions
	bool					mPingPong;
	bool					mPlayForward;

public:
    t2dAnimationDatablock();
    virtual ~t2dAnimationDatablock();

    // [neo, 5/22/2007]
    // Added accessor methods so other classes can query info
    S32     getFrameCount() const { return mFramesList.size(); }
    S32     getFrame( S32 index ) const { return mFramesList[ index ]; }
    S32     getStartFrame() const { return mStartFrame; }
    F32     getAnimationTime() const { return mAnimationTime; }
    bool    getRandomStart() const { return mRandomStart; }
    bool    getAnimationCycle() const { return mAnimationCycle; }
	void	SetAnimationCycleOn() { mAnimationCycle = true; }	// RKS: I added this
	void	SetAnimationCycleOff() { mAnimationCycle = false; } // RKS: I added this
    const SimObjectPtr<t2dImageMapDatablock> getImageMap() const { return mImageMapDataBlock; }
    // [neo end]

    /// Calculate Animation.
    void calculateAnimation(void);

    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dAnimationDatablock);
};

///-----------------------------------------------------------------------------
/// Animation Datablock 2D.
///-----------------------------------------------------------------------------
class t2dAnimationController
{
public:
    StringTableEntry                        mCurrentAnimationName;
	bool									m_bAnimationPaused;		// RKS: I added this

private:
    SimObjectPtr<t2dAnimationDatablock>     mConfigDataBlock;

    StringTableEntry                        mLastAnimationName;
    S32                                     mLastFrameIndex;
    t2dImageMapSmartPtr                     mCurrentImageMapDataBlock;
    S32                                     mCurrentFrameIndex;
    t2dImageMapDatablock::cFramePixelArea   mCurrentFramePixelArea;
    t2dImageMapDatablock::cFrameTexelArea   mCurrentFrameArea;
    TextureHandle                           mCurrentFrameTextureHandle;
    U32                                     mMaxFrameIndex;
    F32                                     mCurrentTime;
    F32                                     mCurrentModTime;
    F32                                     mTotalIntegrationTime;
    F32                                     mFrameIntegrationTime;
    bool                                    mAutoRestoreAnimation;
    bool                                    mAnimationFinished;
    S32                                     mLastFrameError;

	// Added pausing capability -mperry
	bool									mAnimationPaused;


public:
    t2dAnimationController();
    virtual ~t2dAnimationController();

    const t2dImageMapDatablock::cFramePixelArea&    getCurrentFramePixelArea( void ) const  { return mCurrentFramePixelArea; };
    const t2dImageMapDatablock::cFrameTexelArea&    getCurrentFrameAreaBind( void ) const   { glBindTexture( GL_TEXTURE_2D, mCurrentFrameTextureHandle.getGLName() ); return mCurrentFrameArea; };
    TextureHandle                                   getCurrentFrameTexture( void ) const    { return mCurrentFrameTextureHandle; };
    const t2dImageMapDatablock::cFrameTexelArea&    getCurrentFrameArea( void ) const       { return mCurrentFrameArea; };
    const t2dAnimationDatablock*                    getCurrentDataBlock( void ) const       { return mConfigDataBlock; };
    const StringTableEntry                          getCurrentAnimation( void ) const       { return mCurrentAnimationName; };
    const U32                                       getCurrentFrame( void ) const           { return mCurrentFrameIndex; };
    const F32                                       getCurrentTime( void ) const            { return mCurrentTime; };
    bool                                            isAnimationFinished( void ) const       { return mAnimationFinished; };
    const t2dAnimationDatablock*                    getAnimationDatablock( void ) const     { return mConfigDataBlock; };
    const bool                                      getIsValid( void ) const                { return !( mAnimationFinished || !mConfigDataBlock || mConfigDataBlock->mImageMapDataBlock.isNull() ); };
    const bool                                      getIsImageMapValid( void ) const        { return !( !mConfigDataBlock || mConfigDataBlock->mImageMapDataBlock.isNull() ); };

    void                                            setAnimationFrame( const U32 frameIndex );

    bool playAnimation( const char* animationName, const bool autoRestore, S32 startFrame = -1, const bool mergeTime = false );
    bool updateAnimation( const F32 elapsedTime );
    void resetTime( void );

	// Added pausing capability -mperry
	void pauseAnimation( bool enabled) { mAnimationPaused = enabled; }

	void SetLastAnimationName( const char* );
	void PauseAnimation() { m_bAnimationPaused = true; }		// RKS: I added these functions so the animations can be paused
	void UnpauseAnimation() { m_bAnimationPaused = false; }

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dAnimationController );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dAnimationController, 1 );
};


#endif // _T2DANIMATIONCONTROLLER_H_
