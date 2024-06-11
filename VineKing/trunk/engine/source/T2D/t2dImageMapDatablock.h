//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D ImageMap Datablock Object.
//-----------------------------------------------------------------------------

#ifndef _T2DIMAGEMAPDATABLOCK_H_
#define _T2DIMAGEMAPDATABLOCK_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _T2DBASEDATABLOCK_H_
#include "./t2dBaseDatablock.h"
#endif

#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif



//-----------------------------------------------------------------------------
// ImageMap Datablock 2D.
//-----------------------------------------------------------------------------
class t2dImageMapDatablock : public t2dBaseDatablock
{
public:
    // --------------------------------------------------------------------------------------
    // QSorts.
    // --------------------------------------------------------------------------------------
    static S32 QSORT_CALLBACK frameOutFrameNumberSort(const void* a, const void* b);
    static S32 QSORT_CALLBACK frameInHeightSort(const void* a, const void* b);


    // --------------------------------------------------------------------------------------
    /// ImageMap Error.
    // --------------------------------------------------------------------------------------
    enum tImageMapError
    {
        T2D_IMAGEMAP_OK                                 = 0,
        T2D_IMAGEMAP_ERROR_INVALID_MODE                 = 1,
        T2D_IMAGEMAP_ERROR_INVALID_BITMAP               = 2,
        T2D_IMAGEMAP_ERROR_FRAME_TOO_WIDE_HARDWARE      = 3,
        T2D_IMAGEMAP_ERROR_FRAME_TOO_HIGH_HARDWARE      = 4,
        T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_WIDE_HARDWARE  = 5,
        T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_HIGH_HARDWARE  = 6,
        T2D_IMAGEMAP_ERROR_FRAME_TOO_WIDE_SOFTWARE      = 7,
        T2D_IMAGEMAP_ERROR_FRAME_TOO_HIGH_SOFTWARE      = 8,
        T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_WIDE_SOFTWARE  = 9,
        T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_HIGH_SOFTWARE  = 10,
        T2D_IMAGEMAP_ERROR_HARDWARE_MAX_TEXTURE         = 11,
        T2D_IMAGEMAP_ERROR_MAXIMUM_TEXTURE_TOO_BIG      = 12,
        T2D_IMAGEMAP_ERROR_INVALID_CELL_OFFSETX         = 13,
        T2D_IMAGEMAP_ERROR_INVALID_CELL_OFFSETY         = 14,
        T2D_IMAGEMAP_ERROR_INVALID_CELL_DIMENSION       = 15,
        T2D_IMAGEMAP_ERROR_INVALID_CELL_QUANTITY        = 16,
        T2D_IMAGEMAP_ERROR_INVALID_CELL_CAPTURE         = 17,
        T2D_IMAGEMAP_ERROR_FAIL_SEPERATOR_COLOR         = 18,
        T2D_IMAGEMAP_ERROR_NO_FRAMES                    = 19,
        T2D_IMAGEMAP_ERROR_INVALID_FRAME_COUNT          = 20,
        T2D_IMAGEMAP_ERROR_NO_LINKED_IMAGEMAPS          = 21,
        T2D_IMAGEMAP_ERROR_NOT_ENOUGH_LINKED_IMAGEMAPS  = 22,
        T2D_IMAGEMAP_ERROR_INVALID_LINKED_IMAGEMAP      = 23,
    };


    // --------------------------------------------------------------------------------------
    /// Texture Filter Mode.
    // --------------------------------------------------------------------------------------
    enum eTextureFilterMode
    {
        T2D_FILTER_NEAREST,
        T2D_FILTER_BILINEAR,

        T2D_FILTER_INVALID,                 ///< Error!
    };


    // --------------------------------------------------------------------------------------
    /// ImageMap Mode.
    // --------------------------------------------------------------------------------------
    enum eImageMapMode
    {
        T2D_IMAGEMODE_FULL,
        T2D_IMAGEMODE_KEY,
        T2D_IMAGEMODE_CELL,
        T2D_IMAGEMODE_LINK,

        T2D_IMAGEMODE_INVALID,
    };

    // --------------------------------------------------------------------------------------
    /// Frame Pixel Area.
    // --------------------------------------------------------------------------------------
    class cFramePixelArea
    {
    public:
        void setArea( const U32 x, const U32 y, const U32 width, const U32 height ) { mX = x; mY = y; mWidth = width; mHeight = height; };

        U32 mX;
        U32 mY;
        U32 mWidth;
        U32 mHeight;
    };


    // --------------------------------------------------------------------------------------
    /// Frame Texel Area.
    // --------------------------------------------------------------------------------------
    class cFrameTexelArea
    {
    public:
        void convertArea( const cFramePixelArea& pixelArea, const F32 texelWidth, const F32 texelHeight )
        {
            mX = pixelArea.mX * texelWidth;
            mY = pixelArea.mY * texelHeight;
            mWidth = pixelArea.mWidth * texelWidth;
            mHeight = pixelArea.mHeight * texelHeight;
            mX2 = mX + mWidth;
            mY2 = mY + mHeight;
        };

        F32 mX;
        F32 mY;
        F32 mX2;
        F32 mY2;
        F32 mWidth;
        F32 mHeight;
    };


    // --------------------------------------------------------------------------------------
    /// Frame Partition.
    // --------------------------------------------------------------------------------------
    class cFramePartition
    {
    public:
        cFramePartition(void) : mFirstPartition(-1), mSecondPartition(-1), mFilled(false), mIndex(0) {};

        S32                     insertPartition( const U32& width, const U32& height );
        S32                     createPartition( void );

        static void             clearPartitions();
        static void             resetPartitionTree( const U32 x, const U32 y, const U32 width, const U32 height );
        static S32              insertRootPartition( const U32 width, const U32 height );
        static cFramePartition& getPartition( const S32 partition ) { return *mVecFramePartition[partition]; };
        static bool             isEmpty( const S32 partition ) { return (partition == empty); };
        static bool             notEmpty( const S32 partition ) { return (partition != empty); };
        static void             setEmpty( S32& partitionRef ) { partitionRef = empty; };

        //static Vector<cFramePartition>  mVecFramePartition;
        // [neo, 5/17/2007 - #3126]
        static Vector<cFramePartition*> mVecFramePartition;
        static U32                      mCurrentFramePartition;
        static const S32                empty;
        U32                             mIndex;
        bool                            mFilled;
        cFramePixelArea                 mPartitionPixelArea;
        S32                             mFirstPartition;
        S32                             mSecondPartition;
    };


    // --------------------------------------------------------------------------------------
    /// Input Frames.
    // --------------------------------------------------------------------------------------
    class cFrameIn
    {
    public:
        U32             mFrame;
        cFramePixelArea mOriginalPixelArea;
        cFramePixelArea mTargetPixelArea;
    };


    // --------------------------------------------------------------------------------------
    /// Output Frames.
    // --------------------------------------------------------------------------------------
    class cFrameOut
    {
    public:
        U32             mFrame;
        U32             mTexturePage;
        cFramePixelArea mInputPixelArea;
        cFramePixelArea mOutputPixelArea;
        cFrameTexelArea mOutputTexelArea;
    };

    // --------------------------------------------------------------------------------------
    /// Texture Page.
    // --------------------------------------------------------------------------------------
    class cTexturePage
    {
    public:
        cTexturePage(void) : mpPageTextureHandle(NULL), mpPageBitmap(NULL) {};

        TextureHandle*  mpPageTextureHandle;
        GBitmap*        mpPageBitmap;
        U32             mTexturePage;
        U32             mFramesOnPage;
        U32             mWidth;
        U32             mHeight;
    };


    // --------------------------------------------------------------------------------------
    // Types.
    // --------------------------------------------------------------------------------------
    typedef Vector<cFramePartition*> typeFramePartition;
    typedef Vector<cFrameIn>        typeFrameIn;
    typedef Vector<cFrameOut>       typeFrameOut;
    typedef Vector<cTexturePage>    typeTexturePage;
    typedef Vector< SimObjectId >   typeDatablockVector;


private:
    typedef t2dBaseDatablock    Parent;

	/// Texture Resurrection Callback Key.
	U32							mTextureCallbackKey;

    /// Bitmap.
    GBitmap*                    mpSrcBitmap;
    StringTableEntry            mSrcBitmapName;
    S32                         mSrcBitmapWidth;
    S32                         mSrcBitmapHeight;

    /// Image Options.
    eImageMapMode               mImageMode;
    S32                         mExpectedFrameCount;
    S32                         mMaximumCurrentTextureSize;
    eTextureFilterMode          mFilterMode;
    bool                        mPreferPerf;
    bool                        mFilterPad;
    bool                        mCellRowOrder;
    S32                         mCellOffsetX;
    S32                         mCellOffsetY;
    S32                         mCellStrideX;
    S32                         mCellStrideY;
    S32                         mCellWidth;
    S32                         mCellHeight;
    S32                         mCellCountX;
    S32                         mCellCountY;
    StringTableEntry            mLinkedImageMaps;
    typeDatablockVector         mVecLinkedDatablocks;

    bool                        mPreload;
    bool                        mAllowUnload;
	bool						mCompressPVR;
    bool                        mTexturesLoaded;
    U32                         mLockReference;
	bool						mOptimisedImageMap;
	bool						mForce16bit;				//Luma: control 16bit settings on a per-bitmap basis, not a global engine basis!
	bool						mImageMapDumpComplete;		//Luma:	Melv May's Memory Reduction

    /// Frame Packing.
    static typeFrameIn          mVecFrameIn;
    typeFrameOut                mVecFrameOut;
    typeTexturePage             mVecTexturePage;
    static typeFrameIn          mVecFrameOverflow;
    U32                         mTotalFrames;
    U32                         mTotalWastedSpace;
    U32                         mTotalSpace;
    F32                         mTotalTimeTaken;
    U32                         mIterations;


    /// Calculate Image Map Frames.
    bool calculateFrames( void );
    bool calculateFullImageMap( void );
    bool calculateCellImageMap( void );
    bool calculateKeyImageMap( void );
    bool calculateLinkImageMap( void );
    bool calculateSrcFrames( void );
    bool validateFrames( void );
    bool calculateDstFrames( void );

    /// Load/Unlading.
    bool loadSrcBitmap( void );
    void unloadSrcBitmap( void );
    void unloadTextures( void );

    /// Frame Packing.
    void fitFrames( typeFrameIn& vecFrameIn, typeFrameOut& vecFrameOut, cTexturePage& texturePage, U32& totalWastedSpace );
    U32 getSumArea( const typeFrameIn& vecFrameIn );
    U32 getMaxWidth( const typeFrameIn& vecFrameIn );
    U32 getMinFeasibleWidth( const typeFrameIn& vecFrameIn, const U32 areaHeight );
    void calculatePackFrameWaste( const typeFrameIn& vecFrameIn, const U32 areaWidth, const U32 areaHeight, U32& wastedSpace, U32& fitOnPage );
    U32 packFrames( typeFrameIn& vecFrameIn, typeFrameOut& vecFrameOut, const cTexturePage& texturePage );


public:
    t2dImageMapDatablock();
    virtual ~t2dImageMapDatablock();

    /// Image-Map Compilation.
    bool compileImageMap( void );

	//Luma:	Melv May's Memory Reduction
    bool loadTextures( bool resurrection = false );
	/// Destroy Resources.
    void destroyResources( void );

    /// Set Texture Clamping.
    void                    setTextureClamp( const bool s, const bool t );
    void                    setTextureFilter( const eTextureFilterMode filterMode );

    /// Get Properties.
    void                    validateFrame( U32& frame ) const               { if ( frame >= mTotalFrames ) frame = (mTotalFrames == 0 ? 0 : mTotalFrames-1 ); };
    void                    validatePage( U32& page ) const                 { if ( page >= getImageMapTextureCount() ) page = getImageMapTextureCount()-1; };
    eImageMapMode           getImageMapMode( void ) const                   { return mImageMode; };
    U32                     getImageMapFrameCount( void ) const             { return mTotalFrames; };
    U32                     getImageMapTextureCount( void ) const           { return mVecTexturePage.size(); };
    const cFrameOut&        getImageMapFrame( const U32 frame ) const       { static U32 inFrame = frame; validateFrame(inFrame); return mVecFrameOut[frame]; };
    const cTexturePage&     getImageMapTexturePage( const U32 page ) const  { static U32 inPage = page; validatePage(inPage); return mVecTexturePage[page]; };

    const TextureHandle&    getImageMapFrameTexture( U32 frame ) const      { AssertFatal( getLockReference() > 0, "getImageMapFrameTexture() - Texture is unloaded!"); return *(mVecTexturePage[mVecFrameOut[frame].mTexturePage].mpPageTextureHandle); };
    const cFramePixelArea&  getImageMapFramePixelArea( U32 frame ) const    { validateFrame(frame); return mVecFrameOut[frame].mOutputPixelArea; };
    const cFrameTexelArea&  getImageMapFrameArea( U32 frame ) const         { validateFrame(frame); return mVecFrameOut[frame].mOutputTexelArea; };
    const cFrameTexelArea&  getImageMapFrameAreaBind( U32 frame ) const     { AssertFatal( getLockReference() > 0, "getImageMapFrameTexture() - Texture is unloaded!"); bindImageMapFrame( frame ); return mVecFrameOut[frame].mOutputTexelArea; };
    const void              bindImageMapFrame( U32 frame ) const            { AssertFatal( getLockReference() > 0, "getImageMapFrameTexture() - Texture is unloaded!"); validateFrame(frame); glBindTexture( GL_TEXTURE_2D, mVecTexturePage[mVecFrameOut[frame].mTexturePage].mpPageTextureHandle->getGLName() ); };

	//Luma:	some optimised functions
    const cFrameTexelArea&  getImageMapFrameAreaQuick( U32 frame ) const         { return mVecFrameOut[frame].mOutputTexelArea; };
    const cFramePixelArea&  getImageMapFramePixelAreaQuick( U32 frame ) const    { return mVecFrameOut[frame].mOutputPixelArea; };
    const U32 getImageMapFrameOpenGLTextureQuick( U32 frame ) const				 { return mVecTexturePage[mVecFrameOut[frame].mTexturePage].mpPageTextureHandle->getGLName(); };

    S32                     getSrcBitmapWidth( void ) const                 { return mSrcBitmapWidth; }
    S32                     getSrcBitmapHeight( void ) const                { return mSrcBitmapHeight; }
    StringTableEntry        getSrcBitmapName( void ) const                  { return mSrcBitmapName; };

    U32                     getTotalSpace( void ) const                     { return mTotalSpace; };
    U32                     getTotalWastedSpace( void ) const               { return mTotalWastedSpace; };
    F32                     getTimeTaken( void ) const                      { return mTotalTimeTaken; };

	//Luma:	Melv May's Memory Reduction
	bool					getTexturesLoaded( void ) const					{ return mTexturesLoaded; };
    eTextureFilterMode      getFilterMode( void ) const                     { return mFilterMode; };
    inline U32              getLockReference( void ) const                  { return mLockReference; }

    /// Locking.
    void lockImageMap( void );
    void unlockImageMap( void );

    /// Core.
    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void packData(BitStream* stream);
    virtual void unpackData(BitStream* stream);

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dImageMapDatablock);
};

// Image-Map Console Type.
DECLARE_CONSOLETYPE(t2dImageMapDatablock)


//-----------------------------------------------------------------------------
// ImageMap Smart-Pointer.
//-----------------------------------------------------------------------------
class t2dImageMapSmartPtr
{
private:
    t2dImageMapDatablock* mpImageMap;

public:
    // Constructor (default).
    t2dImageMapSmartPtr() : mpImageMap(NULL) {};

    // Constructor (pointer).
    t2dImageMapSmartPtr( t2dImageMapDatablock* pImageMap )
    {
        // Set Image-Map.
        mpImageMap = pImageMap;

        // Valid Image Map?
        if( mpImageMap )
        {
            // Yes, so register reference.
            mpImageMap->registerReference( (SimObject**)&mpImageMap );
            // Lock Image-Map.
            mpImageMap->lockImageMap();
        }
    }

    // Constructor (reference).
    t2dImageMapSmartPtr( const t2dImageMapSmartPtr& rhs )
    {
        // Set Image-Map.
        mpImageMap = const_cast<t2dImageMapDatablock*>(static_cast<const t2dImageMapDatablock*>(rhs));

        // Valid Image Map?
        if( mpImageMap )
        {
            // Yes, so register reference.
            mpImageMap->registerReference( (SimObject**)&mpImageMap );
            // Lock Image-Map.
            mpImageMap->lockImageMap();
        }
    }

    // Assignment (reference).
    t2dImageMapSmartPtr& operator=(const t2dImageMapSmartPtr& rhs)
    {
        // Already got the reference?
        if( this == &rhs )
        {
            // Yes, so just return it.
            return(*this);
        }

        // Valid Image Map Already?
        if( mpImageMap )
        {
            // Yes, so unregister reference.
            mpImageMap->unregisterReference( (SimObject**)&mpImageMap );
            // Unlock Image-Map.
            mpImageMap->unlockImageMap();
        }

        // Set Image-Map.
        mpImageMap = const_cast<t2dImageMapDatablock*>(static_cast<const t2dImageMapDatablock*>(rhs));

        // Valid Image Map?
        if( mpImageMap )
        {
            // Yes, so register reference.
            mpImageMap->registerReference( (SimObject**)&mpImageMap );
            // Lock Image-Map.
            mpImageMap->lockImageMap();
        }

        // Return Reference.
        return( *this );
    }

    // Assignment (pointer).
    t2dImageMapSmartPtr& operator= (t2dImageMapDatablock *ptr)
    {
        // Already got the reference?
        if( mpImageMap != (t2dImageMapDatablock*)ptr)
        {
            // Valid Image Map Already?
            if( mpImageMap )
            {
                // Yes, so unregister reference.
                mpImageMap->unregisterReference( (SimObject**)&mpImageMap );
                // Unlock Image-Map.
                mpImageMap->unlockImageMap();
            }

            // Set Image-Map.
            mpImageMap = (t2dImageMapDatablock*)ptr;

            // Valid Image Map?
            if( mpImageMap )
            {
                // Yes, so register reference.
                mpImageMap->registerReference( (SimObject**)&mpImageMap );
                // Lock Image-Map.
                mpImageMap->lockImageMap();
            }
        }

        // Return Reference.
        return( *this );
    }

    // Destructor.
    ~t2dImageMapSmartPtr()
    {
        // Valid Image Map Already?
        if( mpImageMap )
        {
            // Yes, so unregister reference.
            mpImageMap->unregisterReference( (SimObject**)&mpImageMap );
            // Unlock Image-Map.
            mpImageMap->unlockImageMap();
        }
    }

#if defined(__MWERKS__) && (__MWERKS__ < 0x2400)
    // CW 5.3 seems to get confused comparing t2dImageMapSmartPtrs...
    bool operator == (const t2dImageMapDatablock *ptr)  { return mpImageMap == ptr; }
    bool operator != (const t2dImageMapDatablock *ptr)  { return mpImageMap != ptr; }
#endif
    bool isNull() const                                 { return mpImageMap == NULL; }
    t2dImageMapDatablock* operator->() const            { return mpImageMap->getIsValid() ? static_cast<t2dImageMapDatablock*>(mpImageMap) : NULL; }
    t2dImageMapDatablock& operator*() const             { return *static_cast<t2dImageMapDatablock*>(mpImageMap); }
    operator t2dImageMapDatablock*() const              { return static_cast<t2dImageMapDatablock*>(mpImageMap)? (mpImageMap->getIsValid() ? static_cast<t2dImageMapDatablock*>(mpImageMap) : NULL) : NULL; }
};



#endif // _T2DIMAGEMAPDATABLOCK_H_
