//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D ImageMap Datablock Object.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleTypes.h"
#include "platform/platform.h"
#include "dgl/gBitmap.h"
#include "core/bitStream.h"
#include "core/resManager.h"
#include "./t2dUtility.h"
#include "./t2dSceneObject.h"
#include "./t2dImageMapDatablock.h"


//-Mat use these instead of grabbing the script variable every time
bool gT2dImageMapDatablockPreload = false;
bool gT2dImageMapDatablockAllowUnload = false;
bool gT2dImageMapDatablockVarsLoaded = false;

//Luma: Optimse getting these from the script too
bool gT2dImageMapDatablockCompressPVR = false;
bool gT2dImageMapDatablockDumpTextures = false;

//------------------------------------------------------------------------------


IMPLEMENT_CO_DATABLOCK_V1(t2dImageMapDatablock);


//------------------------------------------------------------------------------
// ImageMap Error Helpers.
//------------------------------------------------------------------------------
static t2dImageMapDatablock::tImageMapError imageMapError;
#define CHECK_IMAGEMAP_ERROR( function )     if ( !(function) ) return false;
#define THROW_IMAGEMAP_ERROR( error )        imageMapError = (error); return false;


//------------------------------------------------------------------------------
// Static ImageMap/Sub-Class Members.
//------------------------------------------------------------------------------
t2dImageMapDatablock::typeFrameIn  t2dImageMapDatablock::mVecFrameIn;
t2dImageMapDatablock::typeFrameIn  t2dImageMapDatablock::mVecFrameOverflow;
t2dImageMapDatablock::typeFramePartition t2dImageMapDatablock::cFramePartition::mVecFramePartition;
U32 t2dImageMapDatablock::cFramePartition::mCurrentFramePartition;
const S32 t2dImageMapDatablock::cFramePartition::empty = -1;


//-----------------------------------------------------------------------------
// ImageMap Error Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums imageMapErrorLookup[] =
                {
                { t2dImageMapDatablock::T2D_IMAGEMAP_OK,                                "OK" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_MODE,                "Invalid Mode; must be FULL, KEY, CELL or LINK!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_BITMAP,              "Invalid Bitmap File!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_FRAME_TOO_WIDE_HARDWARE,     "Frame Width Too Big; Unsupported by current hardware!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_FRAME_TOO_HIGH_HARDWARE,     "Frame Height Too Big; Unsupported by current hardware!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_WIDE_HARDWARE, "Filter-Padded Frame Width Too Big; Unsupported by current hardware!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_HIGH_HARDWARE, "Filter-Padded Frame Height Too Big; Unsupported by current hardware!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_FRAME_TOO_WIDE_SOFTWARE,     "Frame Width Too Big; Larger than selected limit!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_FRAME_TOO_HIGH_SOFTWARE,     "Frame Height Too Big; Larger than selected limit!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_WIDE_SOFTWARE, "Filter-Padded Frame Width Too Big; Larger than selected limit!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_HIGH_SOFTWARE, "Filter-Padded Frame Height Too Big; Larger than selected limit!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_HARDWARE_MAX_TEXTURE,        "Reported Maximum Hardware Texture Size is invalid!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_MAXIMUM_TEXTURE_TOO_BIG,     "Selected Maximum Texture-Size is larger than the current hardware limit!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_CELL_OFFSETX,        "Invalid CELL Offset X!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_CELL_OFFSETY,        "Invalid CELL Offset Y!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_CELL_DIMENSION,      "Invalid CELL Dimension!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_CELL_QUANTITY,       "Invalid CELL Quantity!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_CELL_CAPTURE,        "Invalid CELL Capture!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_FAIL_SEPERATOR_COLOR,        "Failed to get separator color!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_NO_FRAMES,                   "No frames have been acquired!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_FRAME_COUNT,         "Number of frames acquired differs from that specified!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_NO_LINKED_IMAGEMAPS,         "No LINK mode image-maps selected!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_NOT_ENOUGH_LINKED_IMAGEMAPS, "Not enough LINK mode image-maps selected; Must be at least two!" },
                { t2dImageMapDatablock::T2D_IMAGEMAP_ERROR_INVALID_LINKED_IMAGEMAP,     "Invalid LINK mode image-map selected!" },
                };



//-----------------------------------------------------------------------------
// ImageMap Error Description Script-Enumerator.
//-----------------------------------------------------------------------------
static const char* getImageMapErrorDescription( t2dImageMapDatablock::tImageMapError imageMapError )
{
    // Search for Error.
    for(U32 i = 0; i < (sizeof(imageMapErrorLookup) / sizeof(EnumTable::Enums)); i++)
        if( imageMapErrorLookup[i].index == imageMapError )
            return imageMapErrorLookup[i].label;

    // Invalid Image Mode!
    AssertFatal(false, "t2dImageMapDatablock::getImageMapErrorDescription() - Invalid ImageMap Error");
    // Bah!
    return NULL;
}


//-----------------------------------------------------------------------------
// Image Mode Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums imageModeLookup[] =
                {
                { t2dImageMapDatablock::T2D_IMAGEMODE_FULL, "FULL" },
                { t2dImageMapDatablock::T2D_IMAGEMODE_KEY,  "KEY" },
                { t2dImageMapDatablock::T2D_IMAGEMODE_CELL, "CELL" },
                { t2dImageMapDatablock::T2D_IMAGEMODE_LINK, "LINK" },
                };

static EnumTable imageModeTable(sizeof(imageModeLookup) / sizeof(EnumTable::Enums), &imageModeLookup[0]);

//-----------------------------------------------------------------------------
// Image Mode Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dImageMapDatablock::eImageMapMode getImageMode(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(imageModeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(imageModeLookup[i].label, label) == 0)
            return((t2dImageMapDatablock::eImageMapMode)imageModeLookup[i].index);

    // Invalid Image Mode!
    AssertFatal(false, "t2dImageMapDatablock::getImageMode() - Invalid Image Mode!");
    // Bah!
    return t2dImageMapDatablock::T2D_IMAGEMODE_INVALID;
}

//------------------------------------------------------------------------------
// Image Mode Description Script-Enumerator.
//------------------------------------------------------------------------------
static const char* getImageModeDescription( t2dImageMapDatablock::eImageMapMode imageMapMode )
{
    // Search for Mode.
    for(U32 i = 0; i < (sizeof(imageModeLookup) / sizeof(EnumTable::Enums)); i++)
        if( imageModeLookup[i].index == imageMapMode )
            return imageModeLookup[i].label;

    // Invalid Image Mode!
    AssertFatal(false, "t2dImageMapDatablock::getImageModeDescription() - Invalid Image Mode!");
    // Bah!
    return "Undefined ImageMap Error; please report this problem!";
}


//-----------------------------------------------------------------------------
// Texture Filter Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums textureFilterLookup[] =
                {
                { t2dImageMapDatablock::T2D_FILTER_NEAREST,     "NONE"      },
                { t2dImageMapDatablock::T2D_FILTER_BILINEAR,    "SMOOTH"    },
                };

EnumTable textureFilterTable(sizeof(textureFilterLookup) / sizeof(EnumTable::Enums), &textureFilterLookup[0]);


//-----------------------------------------------------------------------------
// Texture Filter Mode Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dImageMapDatablock::eTextureFilterMode getFilterMode(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(textureFilterLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(textureFilterLookup[i].label, label) == 0)
            return((t2dImageMapDatablock::eTextureFilterMode)textureFilterLookup[i].index);

    // Invalid Filter Mode!
    AssertFatal(false, "t2dImageMapDatablock::getFilterMode() - Invalid Filter Mode!");
    // Bah!
    return t2dImageMapDatablock::T2D_FILTER_INVALID;
}

//------------------------------------------------------------------------------
// Texture Filter Mode Description Script-Enumerator.
//------------------------------------------------------------------------------
static const char* getFilterModeDescription( t2dImageMapDatablock::eTextureFilterMode filterMode )
{
    // Search for Mode.
    for(U32 i = 0; i < (sizeof(textureFilterLookup) / sizeof(EnumTable::Enums)); i++)
        if( textureFilterLookup[i].index == filterMode )
            return textureFilterLookup[i].label;

    // Invalid Filter Mode!
    AssertFatal(false, "t2dImageMapDatablock::getFilterModeDescription() - Invalid Filter Mode!");
    // Bah!
    return "Undefined ImageMap Error; please report this problem!";
}

//-----------------------------------------------------------------------------
// Frame-Out Frame-Number Sort.
//
// Sort the output-frames by ascending frame number.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK t2dImageMapDatablock::frameOutFrameNumberSort(const void* a, const void* b)
{
    // Ascending frame-number sort.
    return ((*((cFrameOut*)a)).mFrame) - ((*((cFrameOut*)b)).mFrame);
}


//-----------------------------------------------------------------------------
// Frame-In Height Sort.
//
// Sort the input-frames by decending height.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK t2dImageMapDatablock::frameInHeightSort(const void* a, const void* b)
{
    // Descending height sort.
    return ((*((cFrameIn*)b)).mTargetPixelArea.mHeight) - ((*((cFrameIn*)a)).mTargetPixelArea.mHeight);
}

//-----------------------------------------------------------------------------
// Recompile All Image Maps.
//-----------------------------------------------------------------------------
ConsoleFunction(recompileAllImageMaps, void, 1, 1, "() Recompiles all image map datablocks\n"
				"@return No return value.")
{
   SimSet* dbSet = t2dBaseDatablock::get2DDatablockSet();
   for (SimSet::iterator i = dbSet->begin(); i != dbSet->end(); i++)
   {
      t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(*i);
      if (imageMap)
         imageMap->compileImageMap();
   }
}


//------------------------------------------------------------------------------

t2dImageMapDatablock::t2dImageMapDatablock() :  mSrcBitmapName(StringTable->insert("")),
                                                mImageMode(T2D_IMAGEMODE_CELL),
                                                mFilterMode(T2D_FILTER_BILINEAR),
                                                mExpectedFrameCount(-1),
                                                mTotalFrames(0),
                                                mTotalWastedSpace(0),
                                                mTotalSpace(0),
                                                mIterations(0),
                                                mTotalTimeTaken(0.0f),
                                                mCellRowOrder(true),
                                                mPreferPerf(true),
                                                mCellOffsetX(0),
                                                mCellOffsetY(0),
                                                mCellStrideX(0),
                                                mCellStrideY(0),
                                                mCellWidth(0),
                                                mCellHeight(0),
                                                mCellCountX(-1),
                                                mCellCountY(-1),
                                                mLinkedImageMaps(StringTable->insert("")),
                                                mpSrcBitmap(NULL),
                                                mSrcBitmapWidth(0),
                                                mSrcBitmapHeight(0),
                                                mMaximumCurrentTextureSize(0),
                                                mLockReference(0),
												mOptimisedImageMap(0),
												mForce16bit(0),
												mImageMapDumpComplete(false),
                                                mTexturesLoaded(false)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mVecFrameIn );
    VECTOR_SET_ASSOCIATION( mVecFrameOut );
    VECTOR_SET_ASSOCIATION( mVecTexturePage );
    VECTOR_SET_ASSOCIATION( mVecFrameOverflow );
    VECTOR_SET_ASSOCIATION( mVecLinkedDatablocks );

    // Set Pre/Unload Defaults.
	if( gT2dImageMapDatablockVarsLoaded == false ) {
		gT2dImageMapDatablockPreload = Con::getBoolVariable( "$pref::T2D::imageMapPreloadDefault", true );
		gT2dImageMapDatablockAllowUnload = Con::getBoolVariable( "$pref::T2D::imageMapAllowUnloadDefault", false );

		//Luma: Optimse getting these from the script too
		gT2dImageMapDatablockCompressPVR = Con::getBoolVariable( "$pref::T2D::imageMapCompressPVRDefault", false );
		gT2dImageMapDatablockDumpTextures = Con::getBoolVariable( "$pref::T2D::imageMapDumpTextures", false );
		gT2dImageMapDatablockVarsLoaded = true;
	}
    mCompressPVR = gT2dImageMapDatablockCompressPVR;
    mPreload = gT2dImageMapDatablockPreload;
    mAllowUnload = gT2dImageMapDatablockAllowUnload;

    // Filter Padding is on by default if filter mode is bilinear.
    mFilterPad = (mFilterMode == T2D_FILTER_BILINEAR);
}


//------------------------------------------------------------------------------

t2dImageMapDatablock::~t2dImageMapDatablock()
{
}

//------------------------------------------------------------------------------

void t2dImageMapDatablock::initPersistFields()
{
    Parent::initPersistFields();

    // Fields.
    addField("imageName",       TypeFilename,   Offset(mSrcBitmapName,              t2dImageMapDatablock));
    addField("imageMode",       TypeEnum,       Offset(mImageMode,                  t2dImageMapDatablock), 1, &imageModeTable);

    addField("frameCount",      TypeS32,        Offset(mExpectedFrameCount,         t2dImageMapDatablock));

    addField("filterMode",      TypeEnum,       Offset(mFilterMode,                 t2dImageMapDatablock), 1, &textureFilterTable);
    addField("filterPad",       TypeBool,       Offset(mFilterPad,                  t2dImageMapDatablock));

    addField("preferPerf",      TypeBool,       Offset(mPreferPerf,                 t2dImageMapDatablock));
    
    addField("cellRowOrder",    TypeBool,       Offset(mCellRowOrder,               t2dImageMapDatablock));
    addField("cellOffsetX",     TypeS32,        Offset(mCellOffsetX,                t2dImageMapDatablock));
    addField("cellOffsetY",     TypeS32,        Offset(mCellOffsetY,                t2dImageMapDatablock));
    addField("cellStrideX",     TypeS32,        Offset(mCellStrideX,                t2dImageMapDatablock));
    addField("cellStrideY",     TypeS32,        Offset(mCellStrideY,                t2dImageMapDatablock));
    addField("cellCountX",      TypeS32,        Offset(mCellCountX,                 t2dImageMapDatablock));
    addField("cellCountY",      TypeS32,        Offset(mCellCountY,                 t2dImageMapDatablock));
    addField("cellWidth",       TypeS32,        Offset(mCellWidth,                  t2dImageMapDatablock));
    addField("cellHeight",      TypeS32,        Offset(mCellHeight,                 t2dImageMapDatablock));

    addField("linkImageMaps", TypeString,       Offset(mLinkedImageMaps,            t2dImageMapDatablock));

    addField("preload",         TypeBool,       Offset(mPreload,                    t2dImageMapDatablock));
    addField("allowUnload",     TypeBool,       Offset(mAllowUnload,                t2dImageMapDatablock));
	addField("compressPVR",       TypeBool,       Offset(mCompressPVR,                  t2dImageMapDatablock));
    addField("optimised",      TypeBool,       Offset(mOptimisedImageMap,                t2dImageMapDatablock));

	//Luma: control 16bit settings on a per-bitmap basis, not a global engine basis!
	addField("force16bit",      TypeBool,       Offset(mForce16bit,                 t2dImageMapDatablock));
}

//------------------------------------------------------------------------------

void textureEventCallback( const U32 eventCode, void *userData )
{
	//Luma:	Melv May's Memory Reduction
	// Cache Resurrecting?
	if( eventCode == TextureManager::BeginResurrection )
	{
		// Cast ImageMap Datablock.
		t2dImageMapDatablock* pImageMapDatablock = reinterpret_cast<t2dImageMapDatablock*>(userData);

		// Linked Mode Image-Map?
		if ( pImageMapDatablock->getImageMapMode() == t2dImageMapDatablock::T2D_IMAGEMODE_LINK )
		{
			// Yes, so we can ignore these as they hold no textures directly.
			return;
		}

		// Were textures loaded?
		if ( pImageMapDatablock->getTexturesLoaded() )
		{
        // Yes, so load Textures.
        if ( !pImageMapDatablock->loadTextures() )
		  {
			  // Error so warn!
			  Con::warnf( "There was a problem loading the texture for %s during texture resurrection!", pImageMapDatablock->getName() );
		  }
		}
	}
}

//------------------------------------------------------------------------------

bool t2dImageMapDatablock::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Preload?
    if ( mPreload )
    {
        // Yes, so set lock-reference so that we're always active!
        mLockReference = 1;
    }

	//do some initial setup for Optimised ImageMaps
	if(mOptimisedImageMap)
	{
		//cannot support optimsied image maps with the LINK type... so we spit out a warning and remove the optimised setting
		if(mImageMode == T2D_IMAGEMODE_LINK)
		{
			Con::warnf( "t2dImageMapDatablock::onAdd(%s) - Cannot have LINK images using the Optimised setting... removing Optimised)", getName());
			mOptimisedImageMap = false;
		}
		else
		{
			//Optimsied overrides some other settings that make it incompatible!
			mFilterPad = false;
		}
	}

    // Compile ImageMap.
    compileImageMap();

    bool valid = getIsValid();

    // Call Parent.
    if (!Parent::onAdd())
       return false;

    setIsValid(valid);

	//Luma:	Melv May's Memory Reduction
    // Linked Mode Image-Map?
	if ( getImageMapMode() != t2dImageMapDatablock::T2D_IMAGEMODE_LINK )
 	{
  		 // Register for texture callback.
  		 mTextureCallbackKey = TextureManager::registerEventCallback(textureEventCallback, this);
 	}

    return true;
}

//------------------------------------------------------------------------------

void t2dImageMapDatablock::onRemove()
{
#ifdef TORQUE_DEBUG
    // Any References?
    if ( !(getLockReference() == 1 && mPreload) && getLockReference() > 0 )
    {
        // Yes, so Warn.
        Con::warnf( "t2dImageMapDatablock::onRemove( %s ) - Reference Count isn't zero but image-map is being destroyed! (%d ref)", getSrcBitmapName(), getLockReference() );
    }
#endif

	//Luma:	Melv May's Memory Reduction
    // Linked Mode Image-Map?
 	if ( getImageMapMode() != t2dImageMapDatablock::T2D_IMAGEMODE_LINK )
 	{
		// Unregister for texture callback.
		TextureManager::unregisterEventCallback( mTextureCallbackKey );
 	}

    // Destroy Resources.
    destroyResources();

    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void t2dImageMapDatablock::lockImageMap( void )
{
    // Increase Lock Reference.
    ++mLockReference;

    // Do we need to load?
    if ( !mTexturesLoaded && mLockReference == 1 )
    {
        // Yes, so load textures.
        loadTextures();
    }
}

//------------------------------------------------------------------------------

void t2dImageMapDatablock::unlockImageMap( void )
{
    // Sanity.
    AssertFatal( getLockReference() > 0, "t2dImageMapDatablock::unlock() - Already released locks!" );

    // Decrease Lock Reference.
    --mLockReference;

    // Do we need to unload?
    if ( getLockReference() == 0 && mAllowUnload )
    {
        // Yes, so unload textures.
        unloadTextures();
    }
}


//------------------------------------------------------------------------------

#ifdef PUAP_NAMESPACE_CHANGE

bool t2dImageMapDatablock::compileImageMap( void )
{
    // Invalidate Datablock.
    setIsValid(false);

    // Is there a global pack-start callback?
    if ( Namespace::globalFunctionNamespace()->lookup(StringTable->insert("onImageMapPackStart")) )
    {
        // Yes, so call it.
        Con::executef(2, "onImageMapPackStart", getName() );
    }

    // Calculate Frames.
    calculateFrames();

    // Is there a global pack-end callback?
    if ( Namespace::globalFunctionNamespace()->lookup(StringTable->insert("onImageMapPackEnd")) )
    {
        // Yes, so call it.
        Con::executef(2, "onImageMapPackEnd", getName() );
    }

    // Did we get an error?
    if ( imageMapError != T2D_IMAGEMAP_OK )
    {
        // Yes, so destroy Resources.
        destroyResources();

        // Fetch Error Description.
        const char* pErrorDescription = getImageMapErrorDescription(imageMapError);

        // Is there a global error callback?
        if ( Namespace::globalFunctionNamespace()->lookup(StringTable->insert("onImageMapError")) )
        {
            // Yes, so format output.

            // Argument Buffer.
            char argBuffer[16];

            // Format Image-Map Error Number Buffer.
            dSprintf(argBuffer, 16, "%d", S32(imageMapError));

            // Do Callback.
            Con::executef(4, "onImageMapError", getName(), argBuffer, pErrorDescription);
        }

        // Are we echoing imaegMap errors?
        if ( Con::getBoolVariable( "$pref::T2D::imageMapEchoErrors", true ) )
        {
            // Yes, so echo error.
           Con::errorf("ImageMap(%s)SrcBitmap(%s) Error: '%s' (%d)", getName(), mSrcBitmapName ? mSrcBitmapName : "" , pErrorDescription, imageMapError );
        }

        // Return Error.
        return false;
    }

    // Validate Datablock.
    setIsValid( true );

    // Return No Error.
    return true;
}

#else //normal TGB

bool t2dImageMapDatablock::compileImageMap( void )
{
    // Invalidate Datablock.
    setIsValid(false);
	//-Mat do not use these callbacks, they are very slow
#ifndef  PUAP_OPTIMIZE
    // Is there a global pack-start callback?
    if ( Namespace::global()->lookup(StringTable->insert("onImageMapPackStart")) )
    {
        // Yes, so call it.
        Con::executef(2, "onImageMapPackStart", getName() );
    }
#endif
    // Calculate Frames.
    calculateFrames();
#ifndef  PUAP_OPTIMIZE
    // Is there a global pack-end callback?
    if ( Namespace::global()->lookup(StringTable->insert("onImageMapPackEnd")) )
    {
        // Yes, so call it.
        Con::executef(2, "onImageMapPackEnd", getName() );
    }
#endif
    // Did we get an error?
    if ( imageMapError != T2D_IMAGEMAP_OK )
    {
        // Yes, so destroy Resources.
        destroyResources();

        // Fetch Error Description.
        const char* pErrorDescription = getImageMapErrorDescription(imageMapError);

        // Is there a global error callback?
        if ( Namespace::global()->lookup(StringTable->insert("onImageMapError")) )
        {
            // Yes, so format output.

            // Argument Buffer.
            char argBuffer[16];

            // Format Image-Map Error Number Buffer.
            dSprintf(argBuffer, 16, "%d", S32(imageMapError));

            // Do Callback.
            Con::executef(4, "onImageMapError", getName(), argBuffer, pErrorDescription);
        }

        // Are we echoing imaegMap errors?
        if ( Con::getBoolVariable( "$pref::T2D::imageMapEchoErrors", true ) )
        {
            // Yes, so echo error.
           Con::errorf("ImageMap(%s)SrcBitmap(%s) Error: '%s' (%d)", getName(), mSrcBitmapName ? mSrcBitmapName : "" , pErrorDescription, imageMapError );
        }

        // Return Error.
        return false;
    }

    // Validate Datablock.
    setIsValid( true );

    // Return No Error.
    return true;
}

#endif //PUAP_NAMESPACE_CHANGE

//------------------------------------------------------------------------------

void t2dImageMapDatablock::destroyResources( void )
{
    // Unload Textures.
    unloadTextures();

    // Clear Frame Packing.
    mVecFrameIn.clear();
    mVecFrameOut.clear();
    mVecFrameOverflow.clear();

    // Clear Texture Pages.
    mVecTexturePage.clear();

    // Clear Existing Partitions.
    // [neo, 5/17/2007 - #3126]
    cFramePartition::clearPartitions();

    // Clear Linked Datablocks.
    mVecLinkedDatablocks.clear();

    // Reset Source Bitmap Dimensions.
    mSrcBitmapWidth = mSrcBitmapHeight = 0;

	//destroy the bitmap too just in case!
	unloadSrcBitmap();

    // mIterations Iteration Count.
    mIterations = 0;
    // Reset Wasted Space.
    mTotalWastedSpace = 0;
    // Reset Total Space.
    mTotalSpace = 0;
    // Reset Total Frames.
    mTotalFrames = 0;
}

//------------------------------------------------------------------------------

bool t2dImageMapDatablock::calculateFrames( void )
{
    // Destroy Resources.
    destroyResources();

    // Reset ImageMap Error.
    imageMapError = T2D_IMAGEMAP_OK;

    // Valid ImageMode?
    if ( mImageMode == T2D_IMAGEMODE_INVALID )
    {
        // No, so Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_MODE );
    }

    // Link Mode?
    if ( mImageMode == T2D_IMAGEMODE_LINK )
    {
        // Yes, so calculate Linked Map(s).
        CHECK_IMAGEMAP_ERROR( calculateLinkImageMap() );
        // Return No Error.
        return true;
    }

    // Timestamp Start.
    const U32 startTime = Platform::getRealMilliseconds();

    // Calculate Source Frames.
    CHECK_IMAGEMAP_ERROR( calculateSrcFrames() );


    // ******************************************************************************
    // Are we showing imageMap packing details?
    // ******************************************************************************
    if ( Con::getBoolVariable( "$pref::T2D::imageMapShowPacking", false ) )
    {
        // Yes, so output ImageMap Name.
        Con::printf("\n********* Packing ImageMap: '%s' *********", getName());
        // Output Frame Details.
        for ( U32 n = 0; n < mTotalFrames; n++ )
        {
            // Fetch Frame Reference.
            const cFrameIn& frameIn = mVecFrameIn[n];

            // Filter Pad?
            if ( mFilterPad )
            {
                // Yes, so output pad details.
                Con::printf("Frame Input: %0.4d at (%0.4d,%0.4d) sized %0.4d(w) x %0.4d(h) padded to size %0.4d(w) x %0.4d(h)", frameIn.mFrame, frameIn.mOriginalPixelArea.mX, frameIn.mOriginalPixelArea.mY,
                                                                                                                                frameIn.mOriginalPixelArea.mWidth, frameIn.mOriginalPixelArea.mHeight,
                                                                                                                                frameIn.mTargetPixelArea.mWidth, frameIn.mTargetPixelArea.mHeight );
            }
            else
            {
                // No, so output non-padded details.
                Con::printf("Frame Input: %0.4d is %0.4d(w) x %0.4d(h)", frameIn.mFrame, frameIn.mOriginalPixelArea.mWidth, frameIn.mOriginalPixelArea.mHeight);
            }
        }
    }

    // Validate Frames.
    CHECK_IMAGEMAP_ERROR( validateFrames() );

    // Calculate Destination Frames.
    CHECK_IMAGEMAP_ERROR( calculateDstFrames() );

    // Are we preloading?
    if ( getLockReference() > 0 )
    {
        // Yes, so are textures loaded?
        if ( mTexturesLoaded )
        {
            // Yes, so unload textures.
            unloadTextures();
        }

        // Yes, so load Textures.
        CHECK_IMAGEMAP_ERROR( loadTextures() );
    }

    // Calculate Total Time Taken.
    mTotalTimeTaken = (Platform::getRealMilliseconds() - startTime) / 1000.0f;

    // ******************************************************************************
    // Are we showing imageMap packing details?
    // ******************************************************************************
    if ( Con::getBoolVariable( "$pref::T2D::imageMapShowPacking", false ) )
    {
        // Yes, so output Frame Details.
        for ( U32 n = 0; n < mVecFrameOut.size(); n++ )
        {
            // Fetch Frame Reference.
            const cFrameOut& frameOut = mVecFrameOut[n];
            // Dump Frame Output.
            Con::printf("Frame Output: %0.4d is %0.4d(w) x %0.4d(h) - Relocated Src:(%0.4d,%0.4d) to Dst:(%0.4d,%0.4d) on Page %0.4d)", frameOut.mFrame, frameOut.mOutputPixelArea.mWidth, frameOut.mOutputPixelArea.mHeight,
                                                                                                                                        frameOut.mInputPixelArea.mX, frameOut.mInputPixelArea.mY, frameOut.mOutputPixelArea.mX, frameOut.mOutputPixelArea.mY, frameOut.mTexturePage);
        }

        // Output Texture Details.
        for ( U32 n = 0; n < mVecTexturePage.size(); n++ )
        {
            // Fetch Texture-Page Reference.
            const cTexturePage& texturePage = mVecTexturePage[n];
            // Dump Texture-Page.
            Con::printf("Texture Page: %0.4d is %0.4d(w) x %0.4d(h) and contains %0.4d frames", n, texturePage.mWidth, texturePage.mHeight, texturePage.mFramesOnPage);
        }

        // Output ImageMap Summary.
        Con::printf("Fitted in %0.4d texture(s) in %0.4d iteration(s) occupying %d texel(s) with %d unused texel(s).", S32(mVecTexturePage.size()), mIterations, getTotalSpace(), getTotalWastedSpace());
        Con::printf("Fit Percentage is therefore %0.2f%%", 100.0f-((100.0f / F32(getTotalSpace())) * F32(getTotalWastedSpace())));
        Con::printf("Percentage change from original is %0.2f%%", (100.0f / F32(getNextPow2(getSrcBitmapWidth())*getNextPow2(getSrcBitmapHeight())))*F32(getTotalSpace()));
        Con::printf("Total Time Taken: %0.3f sec(s)\n", getTimeTaken());
    }

    // Return No Error.
    return true;
}

static const char* g_pszCurrentLevelName = NULL; // RKS NOTE: I added this, so we can limit the number of level datablock files

static char g_szNewWaterBitmapName[64] = { 0 };
void _HACK_GetNewWaterBitmapName( const char* );

static char g_szNewScoreScreenBitmapName[64] = { 0 };
void _HACK_GetNewScoreScreenBitmapName( const char* );

//------------------------------------------------------------------------------
// Load Src Bitmap.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::loadSrcBitmap( void )
{
    // Have we a source bitmap?
    if ( !mpSrcBitmap )
    {
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// RKS NOTE:  This is a hack I added in order to limit the number of level datablock files (since the only reason to have more than one is because of the Water images)
		// RKS NOTE:  This hack was further extended to allow the Score Screen images to be changed in the same manner. This also avoids having a large number of datablock files.
		g_pszCurrentLevelName = Con::getVariable( "$CurrentLevelFileName" );
		if( g_pszCurrentLevelName )
		{
			// Check if this is a gameplay level (i.e. Level_XX_XX or Boss_XX_XX)
			if( strstr( g_pszCurrentLevelName, "Level_" ) != NULL || strstr( g_pszCurrentLevelName, "Boss_" ) != NULL )
			{
				if( strcmp( mSrcBitmapName, "game/data/images/water_01_01.png" ) == 0 )
				{
					_HACK_GetNewWaterBitmapName( g_pszCurrentLevelName );
					
					if( g_szNewWaterBitmapName[0] != '\0' )
					{
						//printf( "New Water Bitmap Name:  %s\n", g_szNewWaterBitmapName );
						strcpy( const_cast<char*>( mSrcBitmapName ), g_szNewWaterBitmapName );
					}
				}
				else if( strcmp( mSrcBitmapName, "game/data/images/score_screen_01.png" ) == 0 )
				{
					_HACK_GetNewScoreScreenBitmapName( g_pszCurrentLevelName );
					
					if( g_szNewScoreScreenBitmapName[0] != '\0' )
					{
						//printf( "New Score Screen Bitmap Name: %s\n", g_szNewScoreScreenBitmapName );
						strcpy( const_cast<char*>( mSrcBitmapName ), g_szNewScoreScreenBitmapName );
					}
				}
			}
		}
		
		g_pszCurrentLevelName = NULL;
		g_szNewWaterBitmapName[0] = '\0';
		g_szNewScoreScreenBitmapName[0] = '\0';
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
       char szFullPathBuffer[1024];
       Con::expandScriptFilename( szFullPathBuffer, sizeof(szFullPathBuffer), mSrcBitmapName );

        // No, so attempt to fetch Bitmap Resource.
        mpSrcBitmap = TextureManager::loadBitmapInstance( StringTable->insert( szFullPathBuffer ) );

        // Does texture-file exist?
        if ( !mpSrcBitmap )
        {
            // No, so Error!
            THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_BITMAP );
        }

		//Luma: control 16bit settings on a per-bitmap basis, not a global engine basis!
		mpSrcBitmap->mForce16Bit = mForce16bit;

        // Fetch Source Bitmap Dimensions.
        mSrcBitmapWidth = S32( mpSrcBitmap->getWidth() );
        mSrcBitmapHeight = S32( mpSrcBitmap->getHeight() );
    }

    // Return No Error.
    return true;
}

//------------------------------------------------------------------------------
// Unload Src Bitmap.
//------------------------------------------------------------------------------
void t2dImageMapDatablock::unloadSrcBitmap( void )
{
    // Destroy Bitmap.
    if ( mpSrcBitmap )
    {
        delete mpSrcBitmap;
        mpSrcBitmap = NULL;
    }
}

//------------------------------------------------------------------------------
// Calculate Source Frames.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::calculateSrcFrames( void )
{
    // Load Src Bitmap.
    CHECK_IMAGEMAP_ERROR( loadSrcBitmap() );

    // Handle Mode Appropriately.
    switch( mImageMode )
    {
        // Full-Frame Mode.
        case T2D_IMAGEMODE_FULL:
        {
            // Calculate Map.
            CHECK_IMAGEMAP_ERROR( calculateFullImageMap() );

        } break;

        // Cell-Frame Mode.
        case T2D_IMAGEMODE_CELL:
        {
            // Calculate Map.
            CHECK_IMAGEMAP_ERROR( calculateCellImageMap() );

        } break;

        // Colour-Keyed Mode.
        case T2D_IMAGEMODE_KEY:
        {
            // Calculate Map.
            CHECK_IMAGEMAP_ERROR( calculateKeyImageMap() );

        } break;

    };

    // Unload Src Bitmap.
	//Luma: don't unload the bitmap if we are also preloading... otherwise we end up loading twice (when we load textures for preloading)... SLOW!!!!!!!!!
	if ( getLockReference() == 0 )
	{
		unloadSrcBitmap();
	}

    // Set Frame Count.
    mTotalFrames = mVecFrameIn.size();

    // Pad frames if selected.
    for ( U32 n = 0; n < mTotalFrames; n++ )
    {
        // Get Frame Reference.
        cFrameIn& frameIn = mVecFrameIn[n];
        // Copy to Target Pixel Area.
        // NOTE:-   We're actually not interested in the position in the Target pixel area!
        frameIn.mTargetPixelArea = frameIn.mOriginalPixelArea;
        // Padding Frames?
        if ( mFilterPad )
        {
            // Yes, so pad by two pixels.
            frameIn.mTargetPixelArea.mWidth += 2;
            frameIn.mTargetPixelArea.mHeight += 2;
        }
    }

    // Return No Error.
    return true;
}


//------------------------------------------------------------------------------
// Full ImageMap.
//
// Use whole image to acquire single-frame.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::calculateFullImageMap()
{
    // Calculate Full-Frame Pixel Frame.
    cFrameIn frameIn;
    // Set Frame Number.
    frameIn.mFrame = 0;
    // Setup Full-Frame Area.
    frameIn.mOriginalPixelArea.setArea( 0, 0, mSrcBitmapWidth, mSrcBitmapHeight );

    // Add to input frames.
    mVecFrameIn.push_back( frameIn );

    // Is filter-pad on and the frame is a POT in dimension?
    if ( mFilterPad && getNextPow2(mSrcBitmapWidth) == mSrcBitmapWidth && getNextPow2(mSrcBitmapHeight) == mSrcBitmapHeight )
    {
        // Yes, so let's disabled filter-padding.

		//Luma : allow control here, just make a note in the console. This used to be forced false        
		//mFilterPad = false;
		Con::printf("Luma :: t2dImageMapDatablock - ImageMap (%s) disabled 'filter-padding' as it was unnecessary!", getName());

#ifdef TORQUE_DEBUG
        // Issue a warning.
        Con::printf("t2dImageMapDatablock::calculateFullImageMap() - ImageMap (%s) disabled 'filter-padding' as it was unnecessary!", getName());
#endif
    }

    // Return No Error.
    return true;
}


//------------------------------------------------------------------------------
// Cell ImageMap.
//
// Use a fixed cell-size to acquire frames.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::calculateCellImageMap( void )
{
    // The cell width needs to be maximum the bitmap width!
    if ( mCellWidth < 1 || mCellWidth > mSrcBitmapWidth )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell Width of %d.", mCellWidth );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_DIMENSION );
    }

    // The cell height needs to be maximum the bitmap height!
    if ( mCellHeight < 1 || mCellHeight > mSrcBitmapHeight )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell Height of %d.", mCellHeight );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_DIMENSION );
    }

    // The Cell Offset X needs to be within the bitmap.
    if ( mCellOffsetX < 0 || mCellOffsetX >= mSrcBitmapWidth )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell OffsetX of %d.", mCellOffsetX );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_OFFSETX );
    }

    // The Cell Offset Y needs to be within the bitmap.
    if ( mCellOffsetY < 0 || mCellOffsetY >= mSrcBitmapHeight )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell OffsetY of %d.", mCellOffsetY );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_OFFSETY );
    }


    // Are we using Cell-StrideX?
    S32 cellStepX;
    if ( mCellStrideX != 0 )
    {
        // Yes, so set stepX to be StrideX.
        cellStepX = mCellStrideX;
    }
    else
    {
        // No, so set stepY to be Cell Width.
        cellStepX = mCellWidth;
    }

    // Are we using Cell-StrideY?
    S32 cellStepY;
    if ( mCellStrideY != 0 )
    {
        // Yes, so set stepY to be StrideY.
        cellStepY = mCellStrideY;
    }
    else
    {
        // No, so set stepY to be Cell Height.
        cellStepY = mCellHeight;
    }


    // Are we in Auto-CountX Mode?
    S32 cellCountX;
    if ( mCellCountX == -1 )
    {
        // Yes, so positive stepX?
        if ( cellStepX > 0 )
        {
            // Yes, so calculate cells to right-hand side.
            cellCountX = (mSrcBitmapWidth-mCellOffsetX) / cellStepX;
        }
        else
        {
            // No, so calculate cells to left-hand side.
            cellCountX = (mCellOffsetX / mAbs(cellStepX)) + 1;
        }  
    }
    else
    {
        cellCountX = mCellCountX;
    }

    // Are we in Auto-CountY Mode?
    S32 cellCountY;
    if ( mCellCountY == -1 )
    {
        // Yes, so positive stepY?
        if ( cellStepY > 0 )
        {
            // Yes, so calculate cells to bottom side.
            cellCountY = (mSrcBitmapHeight-mCellOffsetY) / cellStepY;
        }
        else
        {
            // No, so calculate cells to top side.
            cellCountY = (mCellOffsetY / mAbs(cellStepY))+1;
        }  
    }
    else
    {
        cellCountY = mCellCountY;
    }

    // Check Cell Count X.
    if ( cellCountX <= 0 || cellCountY <= 0 )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell CountX/Y of (%d,%d).", cellCountX, cellCountY );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_QUANTITY );
    }


    // Calculate Final Cell Position X.
    S32 cellFinalPositionX = mCellOffsetX + ((cellCountX-((cellStepX<0)?1:0))*cellStepX);
    // Off Left?
    if ( cellFinalPositionX < 0 )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell OffsetX(%d)/Width(%d)/CountX(%d); off bitmap left-hand-side.", mCellOffsetX, mCellWidth, cellCountX );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_CAPTURE );
    }
    // Off Right?
    else if ( cellFinalPositionX > mSrcBitmapWidth )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell OffsetX(%d)/Width(%d)/CountX(%d); off bitmap right-hand-side.", mCellOffsetX, mCellWidth, cellCountX );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_CAPTURE );
    }

    // Calculate Final Cell Position Y.
    S32 cellFinalPositionY = mCellOffsetY + ((cellCountY-((cellStepY<0)?1:0))*cellStepY);
    // Off Top?
    if ( cellFinalPositionY < 0 )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell OffsetY(%d)/Height(%d)/CountY(%d); off bitmap top-side.", mCellOffsetY, mCellHeight, cellCountY );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_CAPTURE );
    }
    // Off Bottom?
    else if ( cellFinalPositionY > mSrcBitmapHeight )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::calculateCellImageMap() - Invalid Cell OffsetY(%d)/Height(%d)/CountY(%d); off bitmap bottom-side.", mCellOffsetY, mCellHeight, cellCountY );
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_CELL_CAPTURE );
    }


    // Calculate Cell-Frames.
    cFrameIn frameIn;
    U32 frame = 0;

    // Cell Row Order?
    if ( mCellRowOrder )
    {
        // Yes, so RowRow Order.
        for ( S32 y = 0, cellPositionY = mCellOffsetY; y < cellCountY; y++, cellPositionY+=cellStepY )
        {
            for ( S32 x = 0, cellPositionX = mCellOffsetX; x < cellCountX; x++, cellPositionX+=cellStepX )
            {
                // Setup Cell-Frame Area.
                frameIn.mOriginalPixelArea.setArea( cellPositionX, cellPositionY, mCellWidth, mCellHeight );
                // Set Frame Number.
                frameIn.mFrame = frame++;
                // Add to input frames.
                mVecFrameIn.push_back( frameIn );
            }
        }
    }
    else
    {
        // No, so Column Order.
        for ( S32 x = 0, cellPositionX = mCellOffsetX; x < cellCountX; x++, cellPositionX+=cellStepX )
        {
            for ( S32 y = 0, cellPositionY = mCellOffsetY; y < cellCountY; y++, cellPositionY+=cellStepY )
            {
                // Setup Cell-Frame Area.
                frameIn.mOriginalPixelArea.setArea( cellPositionX, cellPositionY, mCellWidth, mCellHeight );
                // Set Frame Number.
                frameIn.mFrame = frame++;
                // Add to input frames.
                mVecFrameIn.push_back( frameIn );
            }
        }
    }

    // Return No Error.
    return true;
}


//------------------------------------------------------------------------------
// Key ImageMap.
//
// Use a colour-key to acquire frames.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::calculateKeyImageMap( void )
{
    // Initialise Frame Number.
    U32 frame = 0;

    // Fetch the Seperator Colour.
    ColorI seperatorColour;
    if ( !mpSrcBitmap->getColor( 0, 0, seperatorColour ) )
    {
        // Throw Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_FAIL_SEPERATOR_COLOR );
    }

    // Start at top of imageMap.
    S32 positionY = 0;

    // Search Colour.
    ColorI searchColour;

    // Frame Region.
    cFrameIn frameIn;

    // Find this Row Height.
    while ( positionY < mSrcBitmapHeight )
    {
        // Fetch ImageMap Colour.
        mpSrcBitmap->getColor( 0, positionY, searchColour);
        // Skip Seperator Colours.
        if(searchColour == seperatorColour)
        {
            positionY++;
            continue;
        }

        // Start at left.
        S32 positionX = 0;

        // Find regions from left to right ...
        while ( positionX < mSrcBitmapWidth )
        {
            // Fetch ImageMap Colour.
            mpSrcBitmap->getColor( positionX, positionY, searchColour);
            // Skip Seperator Colours.
            if(searchColour == seperatorColour)
            {
                positionX++;
                continue;
            }

            // Frame starts at current position X.
            S32 framePosX = positionX;

            // Found a region so calculate width ...
            while ( positionX < mSrcBitmapWidth )
            {
                // Fetch ImageMap Colour.
                mpSrcBitmap->getColor( positionX, positionY, searchColour);
                // Finish if Seperator Colour.
                if(searchColour == seperatorColour)
                    break;

                // Next Position Along.
                positionX++;
            }

            // Frame starts at current position Y.
            S32 framePosY = positionY;

            // Find the region height ...
            while ( framePosY < mSrcBitmapHeight )
            {
                // Fetch ImageMap Colour.
                mpSrcBitmap->getColor( framePosX, framePosY, searchColour);
                // Finish if Seperator Colour.
                if(searchColour == seperatorColour)
                    break;

                // Next Position Down.
                framePosY++;
            }

            // Set Frame Number.
            frameIn.mFrame = frame++;
            // Setup Key-Frame Area.
            frameIn.mOriginalPixelArea.setArea( framePosX, positionY, positionX-framePosX, framePosY-positionY );
            // Add to input frames.
            mVecFrameIn.push_back( frameIn );
        }

        // Find next seperator colour at left.
        while ( positionY < mSrcBitmapHeight )
        {
            // Fetch ImageMap Colour.
            mpSrcBitmap->getColor( 0, positionY, searchColour);
            // Finish if Seperator Colour.
            if(searchColour == seperatorColour)
                break;

            // Next Position Down.
            positionY++;
        }
    }

    // Return No Error.
    return true;
}


//------------------------------------------------------------------------------
// Link ImageMap.
//
// Link multiple image-maps to gather frames.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::calculateLinkImageMap( void )
{
    // Do we have any linked image-maps?
    if ( !mLinkedImageMaps || mLinkedImageMaps == StringTable->insert("") )
    {
        // No, so Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_NO_LINKED_IMAGEMAPS );
    }

    // Do we have at least two image-maps?
    U32 linkedImageMapCount = t2dSceneObject::getStringElementCount(mLinkedImageMaps);
    if ( linkedImageMapCount < 2 )
    {
        // No, so Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_NOT_ENOUGH_LINKED_IMAGEMAPS ); 
    }

    // Extract the Image-Maps.
    for ( U32 n = 0; n < linkedImageMapCount; n++ )
    {
        // Fetch Image-Map Name.
        const char* pImageMapName = t2dSceneObject::getStringElement(mLinkedImageMaps, n);

        // Find t2dImageMapDatablock Object.
        const t2dImageMapDatablock* pLinkedImageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(pImageMapName));

        // Valid ImageMap?
        if ( !pLinkedImageMap )
        {
            // No, so Error!
            THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_LINKED_IMAGEMAP ); 
        }

        // Default to not linked.
        bool alreadyLinked = false;

        // Check that we've not already got a request for this object.
        for ( U32 linkIndex = 0; linkIndex < mVecLinkedDatablocks.size(); )
        {
            // Fetch ImageMap-Datablock.
            // NOTE:- Let's be safer and check that it's definately an imageMap-datablock.
            const t2dImageMapDatablock* pImageMap = dynamic_cast<t2dImageMapDatablock*>( Sim::findObject( mVecLinkedDatablocks[linkIndex] ) );
            // Valid ImageMap?
            if ( pImageMap )
            {
                // Yes, so is it the same?
                if ( pImageMap == pLinkedImageMap )
                {
                    // Yes, so issue warning.
                    Con::warnf("t2dImageMapDatablock::calculateLinkImageMap() - Duplicate Linked ImageMap '%s' (index %d) in imageMap '%s'.", pImageMapName, n, getName());
                    // Flag as already linked.
                    alreadyLinked = true;
                    // Finish here.
                    break;
                }
            }
            else
            {
                // No, so it looks like the object got deleted prematurely; let's just remove it instead.
                mVecLinkedDatablocks.erase_fast( linkIndex );
                
                // Repeat this item.
                continue;
            }

            // Move to next link.
            ++linkIndex;
        }

        // Not linked yet?
        if ( !alreadyLinked )
        {
            // Yes, so let's link it now.
            mVecLinkedDatablocks.push_back( pLinkedImageMap->getId() );
        }
    }

    // Fetch Actual Linked Image-Map Count.
    linkedImageMapCount = mVecLinkedDatablocks.size();

    // Again, check we've got enough.
    if ( linkedImageMapCount < 2 )
    {
        // No, so Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_NOT_ENOUGH_LINKED_IMAGEMAPS ); 
    }

    U32 currentTexturePage = 0;
    cTexturePage texturePage;
    cFrameOut frameOut;

    // Compile Texture Pages / Frame-Outs from Linked Datablocks.
    for ( U32 n = 0; n < linkedImageMapCount; ++n )
    {
        // Fetch ImageMap-Datablock.
        // NOTE:- Let's be safer and check that it's definately an imageMap-datablock.
        const t2dImageMapDatablock* pLinkedImageMap = dynamic_cast<t2dImageMapDatablock*>( Sim::findObject( mVecLinkedDatablocks[n] ) );
        // Valid ImageMap?
        if ( pLinkedImageMap )
        {
            // Yes, so fetch Texture Page Count.
            const U32 pageCount = pLinkedImageMap->getImageMapTextureCount();
            // Add Pages.
            for ( U32 page = 0; page < pageCount; page++ )
            {
                // Fetch Texture Page.
                texturePage = pLinkedImageMap->mVecTexturePage[page];
                // Adjust Texture Page Reference.
                texturePage.mTexturePage += currentTexturePage;
                // Add Texture Page.
                mVecTexturePage.push_back( texturePage );
            }

            // Fetch Frame Count.
            U32 frameCount = pLinkedImageMap->getImageMapFrameCount();
            // Add Frames.
            for ( U32 frame = 0; frame < frameCount; frame++ )
            {
                // Fetch Frame.
                frameOut = pLinkedImageMap->mVecFrameOut[frame];
                // Adjust Texture Page Reference.
                frameOut.mTexturePage += currentTexturePage;
                // Add Frame.
                mVecFrameOut.push_back( frameOut );
            }

            // Increase Current Texture Page.
            currentTexturePage += pageCount;
        }
        else
        {
            // We should never-ever get here unless we've got some fancy multi-threading working...
            AssertFatal( false, "t2dImageMapDatablock::calculateLinkImageMap() - ImageMap is not available from with the same function; multithreading?" );
        }
    }

    // Calculate Total Frames.
    mTotalFrames = mVecFrameOut.size();

    // Ignore manual frame-count expectations.
    if ( mExpectedFrameCount != -1 )
    {
        // Have we acquired the number frames we expected?
        if ( mTotalFrames != mExpectedFrameCount )
        {
            // No, so warn.
            Con::warnf("t2dImageMapDatablock::calculateLinkImageMap() - Failed to find the specified number of imageMap frames in LINK mode imageMap! (Expected:%d / Found:%d / ImageMaps:%d", mExpectedFrameCount, mTotalFrames, linkedImageMapCount );
            // Error!
            THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_FRAME_COUNT );
        }
    }

    // Return No Error.
    return true;
}


//------------------------------------------------------------------------------
// Validate Frames.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::validateFrames( void )
{
    // Check we've got some frames.
    if ( mTotalFrames == 0 )
    {
        // No, so Error!
        THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_NO_FRAMES );
    }

    // Ignore manual frame-count expectations.
    if ( mExpectedFrameCount != -1 )
    {
        // Have we acquired the number frames we expected?
        if ( mTotalFrames != mExpectedFrameCount )
        {
            // No, so warn.
            Con::warnf("t2dImageMapDatablock::validateFrames() - Failed to find the specified number of imageMap frames! (Expected:%d / Found:%d)", mExpectedFrameCount, mTotalFrames );
            // Error!
            THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_INVALID_FRAME_COUNT );
        }
    }

    // Fetch Maximum Selected Texture Size.
    const S32 maximumSelectedTextureSize = Con::getIntVariable( "$pref::T2D::imageMapFixedMaxTextureSize", 0 );

     // Fetch whether we are using a hardware or software texture-size limit.
    const bool hardwareLimit = (maximumSelectedTextureSize == 0);
    // Hardware Limit?
    if ( hardwareLimit )
    {
        // Yes, so fetch maximum supported size.
        mMaximumCurrentTextureSize = mGetT2DMaxTextureSize();
        // Is the hardware limit valid?
        if ( mMaximumCurrentTextureSize < 1 )
        {
            // No, so Error!
            THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_HARDWARE_MAX_TEXTURE );
        }
    }
    else
    {
        // No, so use software selected limit.
        mMaximumCurrentTextureSize = maximumSelectedTextureSize;
        // Is the Software limit bigger than the hardware one?
        if ( mMaximumCurrentTextureSize > mGetT2DMaxTextureSize() )
        {
            // Yes, so is this an error?
            if ( Con::getIntVariable( "$pref::T2D::imageMapFixedMaxTextureError", true ) )
            {
                // Yes, so Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_MAXIMUM_TEXTURE_TOO_BIG );
            }
            else
            {
                // No, so warn only.
                Con::warnf("t2dImageMapDatablock::validateFrames() - Maximum Fixed Texture Size is greater than current hardware limit! (%s)", getName());
                Con::warnf("Using Hardware Size Limit of %d instead of specified size: %d", mGetT2DMaxTextureSize(), mMaximumCurrentTextureSize );

                // Use hardware limit.
                mMaximumCurrentTextureSize = mGetT2DMaxTextureSize();
            }
        }
    }

    // Check Acquired Frame Sizes.
    for ( U32 n = 0; n < mTotalFrames; n++ )
    {
        // Fetch Frame.
        const cFrameIn& frameIn = mVecFrameIn[n];

        // Check Frame Width.
        if ( frameIn.mOriginalPixelArea.mWidth > mMaximumCurrentTextureSize )
        {
            if ( hardwareLimit )
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_FRAME_TOO_WIDE_HARDWARE );
            }
            else
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_FRAME_TOO_WIDE_SOFTWARE );
            }
        }

        // Check Frame Height.
        if ( frameIn.mOriginalPixelArea.mHeight > mMaximumCurrentTextureSize )
        {
            if ( hardwareLimit )
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_FRAME_TOO_HIGH_HARDWARE );
            }
            else
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_FRAME_TOO_HIGH_SOFTWARE );
            }
        }

        // Check Filter-Padded Frame Width.
        if ( (frameIn.mTargetPixelArea.mWidth) > mMaximumCurrentTextureSize )
        {
            if ( hardwareLimit )
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_WIDE_HARDWARE );
            }
            else
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_WIDE_SOFTWARE );
            }
        }

        // Check Filter-Padded Frame Height.
        if ( (frameIn.mTargetPixelArea.mHeight) > mMaximumCurrentTextureSize )
        {
            if ( hardwareLimit )
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_HIGH_HARDWARE );
            }
            else
            {
                // Error!
                THROW_IMAGEMAP_ERROR( T2D_IMAGEMAP_ERROR_PAD_FRAME_TOO_HIGH_SOFTWARE );
            }
        }
    }

    // Return No Error.
    return true;
}


//------------------------------------------------------------------------------
// Calculate Destination Frames.
//------------------------------------------------------------------------------
bool t2dImageMapDatablock::calculateDstFrames( void )
{
    // Reset Texture Page.
    cTexturePage newTexturePage;
    U32 texturePageIndex = 0;

    // Fit all frames...
    do
    {
        // Set Texture Page Index.
        newTexturePage.mTexturePage = texturePageIndex;

        // Fit most frames with least wasted space as possible...
        fitFrames( mVecFrameIn, mVecFrameOut, newTexturePage, mTotalWastedSpace );

        // Sum Total Space.
        mTotalSpace += newTexturePage.mWidth * newTexturePage.mHeight;

        // Ready a new Texture Page.
        mVecTexturePage.push_back( newTexturePage );
        // Next Texture Page Index.
        texturePageIndex++;

    // Repeat if we've still got frames left to fit.
    } while( mVecFrameIn.size() > 0 );

    // Sort output-frames by ascending frame-number.
    if ( mVecFrameOut.size() > 1 )
    {
        dQsort( mVecFrameOut.address(), mVecFrameOut.size(), sizeof(cFrameOut), frameOutFrameNumberSort );
    }

    // Return No Error.
    return true;
}


// --------------------------------------------------------------------------------------
// Load Textures.
// --------------------------------------------------------------------------------------
//Luma:	Melv May's Memory Reduction
bool t2dImageMapDatablock::loadTextures( bool resurrection )
{
    // Load Src Bitmap.
    CHECK_IMAGEMAP_ERROR( loadSrcBitmap() );

#ifdef TORQUE_OS_IPHONE
	bool pvr = ((mpSrcBitmap->getFormat() >= GBitmap::PVR2)  && (mpSrcBitmap->getFormat() <= GBitmap::PVR4A));
#endif

    // Generate Bitmap Pages.
    for ( U32 n = 0; n < mVecTexturePage.size(); n++ )
    {
        // Fetch Texture-Page Reference.
        cTexturePage& texturePage = mVecTexturePage[n];

		if(mOptimisedImageMap)
		{
			//only need to allocate new bitmap and copy it if the sizes are different (ie. to make pow2 texture)
			if((mSrcBitmapWidth != texturePage.mWidth) || (mSrcBitmapHeight != texturePage.mHeight))
			{
				//create new sized bitmap and copy the rect across quickly... line by line memcpy == fail
				texturePage.mpPageBitmap = new GBitmap(texturePage.mWidth, texturePage.mHeight, false, mpSrcBitmap->getFormat());
				texturePage.mpPageBitmap->copyRect(mpSrcBitmap, RectI(0, 0, mSrcBitmapWidth, mSrcBitmapHeight), Point2I(0, 0));
				texturePage.mpPageBitmap->mForce16Bit = mpSrcBitmap->mForce16Bit;
			}
			else
			{
				//don't need to make a new bitmap... we'll just use the mpSrcBitmap below during OGL texture creation
				texturePage.mpPageBitmap = NULL;
			}
		}
		else
		{
#ifdef TORQUE_OS_IPHONE
			if(pvr)
			{
				texturePage.mpPageBitmap = mpSrcBitmap;
			}
			else
#endif	//TORQUE_OS_IPHONE
			{
	        	// Allocate Bitmap Page of same format as source bitmap.
				texturePage.mpPageBitmap = new GBitmap( texturePage.mWidth, texturePage.mHeight, false, mpSrcBitmap->getFormat() );
				texturePage.mpPageBitmap->mForce16Bit = mpSrcBitmap->mForce16Bit;
			}
		}
	}


	//if using optimised images, we can skip the whole crazy copy business here
	if(!mOptimisedImageMap)
	{
#ifdef 	TORQUE_OS_IPHONE
		//if iPhone and using pvrs, then we don't want to do this copy
		if(!pvr)
#endif	//TORQUE_OS_IPHONE
		{
			// Copy Frames to Bitmap Pages.
			for ( U32 n = 0; n < mTotalFrames; n++ )
			{
				// Fetch Frame Output Reference.
				const cFrameOut& frameOut = mVecFrameOut[n];
				// Fetch Frame Texture Page Reference.
				cTexturePage& texturePage = mVecTexturePage[frameOut.mTexturePage];
				// Fetch Destination Bitmap.
				GBitmap* pDstBitmap = texturePage.mpPageBitmap;
				// Fetch Frame Input Area Reference.
				const cFramePixelArea& inputPixelArea = frameOut.mInputPixelArea;
				// Fetch Frame Output Area Reference.
				const cFramePixelArea& outputPixelArea = frameOut.mOutputPixelArea;

				// Fetch Source/Destination Positions.
				const U32 srcColumn = inputPixelArea.mX;
				const U32 srcRow = inputPixelArea.mY;
				const U32 dstColumn = outputPixelArea.mX;
				const U32 dstRow = outputPixelArea.mY;

				// Fetch Frame Width/Height.
				const U32 frameWidth = inputPixelArea.mWidth;
				const U32 frameHeight = inputPixelArea.mHeight;

				// Fetch Texel Size.
				const U32 texelSize = mpSrcBitmap->bytesPerPixel;
				// Fetch Row Size.
				const U32 rowSize = texelSize * frameWidth;

				// Copy Bitmap Data.
				for ( U32 row = 0; row < frameHeight; row++ )
				{
					// Copy Row.
					dMemcpy( pDstBitmap->getAddress(dstColumn,dstRow+row), mpSrcBitmap->getAddress(srcColumn,srcRow+row), rowSize );

					// Filter Pad?
					if ( mFilterPad )
					{
						// Yes, so copy left/right column texels.
						dMemcpy( pDstBitmap->getAddress(dstColumn-1,dstRow+row), mpSrcBitmap->getAddress(srcColumn,srcRow+row), texelSize );
						dMemcpy( pDstBitmap->getAddress(dstColumn+frameWidth,dstRow+row), mpSrcBitmap->getAddress(srcColumn+frameWidth-1,srcRow+row), texelSize );
					}
				}

				// Filter Pad?
				if ( mFilterPad )
				{
					// Yes, so Top Row.
					dMemcpy( pDstBitmap->getAddress(dstColumn,dstRow-1), mpSrcBitmap->getAddress(srcColumn,srcRow), rowSize );
					// Bottom Row.
					dMemcpy( pDstBitmap->getAddress(dstColumn,dstRow+frameHeight), mpSrcBitmap->getAddress(srcColumn,srcRow+frameHeight-1), rowSize );

					// Top/Left Texel.
					dMemcpy( pDstBitmap->getAddress(dstColumn-1,dstRow-1), pDstBitmap->getAddress(dstColumn-1,dstRow), texelSize );
					// Top/Right Texel.
					dMemcpy( pDstBitmap->getAddress(dstColumn+frameWidth,dstRow-1), pDstBitmap->getAddress(dstColumn+frameWidth,dstRow), texelSize );

					// Bottom/Left Texel.
					dMemcpy( pDstBitmap->getAddress(dstColumn-1,dstRow+frameHeight), pDstBitmap->getAddress(dstColumn-1,dstRow+frameHeight-1), texelSize );
					// Bottom/Right Texel.
					dMemcpy( pDstBitmap->getAddress(dstColumn+frameWidth,dstRow+frameHeight), pDstBitmap->getAddress(dstColumn+frameWidth,dstRow+frameHeight-1), texelSize );
				}
			}

			// Unload Src Bitmap.
			unloadSrcBitmap();
		}
	}


    // Fetch Image-Map Dump Flag.
	//Luma:	Melv May's Memory Reduction
	const bool imageMapDump = (resurrection || mImageMapDumpComplete) ? false : gT2dImageMapDatablockDumpTextures;

    // Generate Texture Pages.
    for ( U32 n = 0; n < mVecTexturePage.size(); n++ )
    {
        // Fetch Texture-Page Reference.
        cTexturePage& texturePage = mVecTexturePage[n];

        // Are we dumping the imageMap textures?
        if ( imageMapDump )
        {
            // Dump packed textures to disk.
            static char buffer[256];
            dSprintf( buffer, 256, "imageMapDump/%s_Page_%d.png", getName(), n );
            static char packFile[1024];
            Con::expandToolScriptFilename(packFile, sizeof(packFile), buffer);
            static FileStream stream;
            const bool opened = ResourceManager->openFileForWrite( stream, packFile, FileStream::Write );
            if ( opened )
            {
				if(mOptimisedImageMap && !texturePage.mpPageBitmap)
				{
					//optimised images will use the mpSrcBitmap if they are already valid image sizes
					mpSrcBitmap->writePNG(stream);
				}
				else
				{
					texturePage.mpPageBitmap->writePNG( stream );
				}
                stream.close();
            }
        }

		//Luma:	Melv May's Memory Reduction
		// Any existing texture handle?
		if(!texturePage.mpPageTextureHandle)
        {
			// Generate Texture.
	        texturePage.mpPageTextureHandle = new TextureHandle();
		}

		//Luma:	Melv May's Memory Reduction
		// NOTE:-	We'll use the name of the image-map just for indentification.  If the texture-manager tries
		//				to use it as a filename there'll be trouble.
    	static char nameBuffer[256];
		dSprintf( nameBuffer, 256, "%s_%d", getName(), n );

		if(mOptimisedImageMap)
		{
			//optimised images will use the mpSrcBitmap if it is already a valid (ie. pow2) image
			GBitmap	*psTextureSource = (texturePage.mpPageBitmap) ? texturePage.mpPageBitmap : mpSrcBitmap;
			texturePage.mpPageTextureHandle->set( nameBuffer, psTextureSource, ImageMap2DKeepTexture, true );
			
			//need to manually delete the bitmap on the texture if it wasn't made from the mpSrcBitmap (that will be destroyed below as part of the standard code)
			TextureObject	*psTexObj = (TextureObject *)(*(texturePage.mpPageTextureHandle));
			if(texturePage.mpPageBitmap)
			{
				delete psTexObj->bitmap;
			}
			psTexObj->bitmap = NULL;
		}
		else
		{
			// Set Texture.
			texturePage.mpPageTextureHandle->set( nameBuffer, texturePage.mpPageBitmap, ImageMap2DTexture, true );
		}
		texturePage.mpPageBitmap = NULL;

        // Set Texture Environment.
#ifndef TORQUE_OS_IPHONE //-Mat untested
		// Set Texture Environment.
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
#endif	//TORQUE_OS_IPHONE
    }

    // Set Filter Mode.
	setTextureFilter( mFilterMode );

	if(mOptimisedImageMap)
	{
		//only unload the SrcBitmap here as we may have needed it for OGL texture creation for optimsied images
		unloadSrcBitmap();
	}

	//Luma:	Melv May's Memory Reduction
	// Flag Image-Map Dump Completed.
	mImageMapDumpComplete = true;

    // Flag Textures Loaded.
    mTexturesLoaded = true;

    // Return No Error.
    return true;
}


// --------------------------------------------------------------------------------------
// Unload Textures.
// --------------------------------------------------------------------------------------
void t2dImageMapDatablock::unloadTextures( void )
{
    if ( mImageMode != T2D_IMAGEMODE_LINK )
    {
        // No, so remove Texture Handles
        for ( S32 n = 0; n < mVecTexturePage.size(); n++ )
        {
            // Fetch Texture Page Reference.
            cTexturePage& texturePage = mVecTexturePage[n];

            // Texture Handle?
            if ( texturePage.mpPageTextureHandle != NULL )
            {
                // Yes, so destroy texture handle.
                delete texturePage.mpPageTextureHandle;
                // Remove handle reference.
                texturePage.mpPageTextureHandle = NULL;
            }
        }
    }

    // Flag Textures Unloaded.
    mTexturesLoaded = false;
}


// --------------------------------------------------------------------------------------
// Fit Frames.
// --------------------------------------------------------------------------------------
void t2dImageMapDatablock::fitFrames( typeFrameIn& vecFrameIn, typeFrameOut& vecFrameOut, cTexturePage& texturePage, U32& totalWastedSpace )
{
    // Sort input-frames by decending height.
    if ( vecFrameIn.size() > 1 )
    {
        dQsort( vecFrameIn.address(), vecFrameIn.size(), sizeof(cFrameIn), frameInHeightSort );
    }

	//assume that all frames are in the same texture... that's why we can't support LINK IMs
	U32 bestWidth;
	U32 bestHeight;
	U32	bestFitOnPage;

	if(mOptimisedImageMap)
	{
		//do quick method of finding the pow2 sizes
		bestWidth = getNextPow2(mSrcBitmapWidth);
		bestHeight = getNextPow2(mSrcBitmapHeight);
		bestFitOnPage = vecFrameIn.size();
	}
	else
	{
		// Calculate Start Dimensions.
		U32 currentHeight = getNextPow2(vecFrameIn[vecFrameIn.size()-1].mTargetPixelArea.mHeight);
		U32 currentWidth = mMaximumCurrentTextureSize;

		// Calculate Total Area.
		U32 totalArea = getSumArea(vecFrameIn);

		// Reset Best Fit.
		bestWidth = mMaximumCurrentTextureSize;
		bestHeight = mMaximumCurrentTextureSize;

		U32 bestArea = bestWidth*bestHeight;
		U32 bestWaste = mMaximumCurrentTextureSize*mMaximumCurrentTextureSize;
		U32 bestAxisBalance = mMaximumCurrentTextureSize;
	    bestFitOnPage = 0;

		// Calculate Lower Width.
		U32 lowerWidth = getMaxWidth( vecFrameIn );

		// Reset Last Height.
		// NOTE:-   This just has to be different than the current height!
		U32 lastHeight = currentHeight-1;

		// Reset Potential Fits.
		U32 potentialFits = 0;

		// Misc.
		U32 minFeasibleWidth;
		U32 wastedSpace;
		U32 fitOnPage;

		// Do the fit...
		while(1)
		{
			// Iteration Count.
			mIterations++;

			// Do we have enough area?
			if ( (currentWidth*currentHeight) >= totalArea )
			{
				// Yes, so height Changed?
				if ( lastHeight != currentHeight )
				{
					// Yes, so calculate Minimum Feasible Width.
					minFeasibleWidth = getMinFeasibleWidth( vecFrameIn, currentHeight );

					// Store 'Last' Height.
					lastHeight = currentHeight;
				}
			}

			// Minimum Area / Width-for-Height?
			if ( (currentWidth*currentHeight) < totalArea || currentWidth < minFeasibleWidth || minFeasibleWidth > mMaximumCurrentTextureSize )
			{
				// Increase Height.
				currentHeight <<= 1;
			}
			else
			{
				// Calculate Wasted Space for maximum frame packing.
				calculatePackFrameWaste( vecFrameIn, currentWidth, currentHeight, wastedSpace, fitOnPage );

				// Increase Potential Fit Counter.
				potentialFits++;

				// Use Packing Preference of Performance or Least-Waste.
				if ( mPreferPerf )
				{
					// Performance is the preference so is this the best packing yet?
					if ( potentialFits == 1 || fitOnPage > bestFitOnPage || (fitOnPage == bestFitOnPage && (U32(mAbs(S32(currentWidth-currentHeight))) < bestAxisBalance || wastedSpace < bestWaste) ) )

					{
						// Yes, so remember it.
						bestWidth = currentWidth;
						bestHeight = currentHeight;
						bestArea = bestWidth*bestHeight;
						bestFitOnPage = fitOnPage;
						bestAxisBalance = mAbs(S32(bestWidth-bestHeight));
					}
				}
				else
				{
					// Least-Waste is the preference so is this the best packing yet?
					if ( potentialFits == 1 || wastedSpace < bestWaste || (wastedSpace == bestWaste && (U32(mAbs(S32(currentWidth-currentHeight))) < bestAxisBalance || fitOnPage > bestFitOnPage) ) )
					{
						// Yes, so remember it.
						bestWidth = currentWidth;
						bestHeight = currentHeight;
						bestArea = bestWidth*bestHeight;
						bestWaste = wastedSpace;
						bestFitOnPage = fitOnPage;
						bestAxisBalance = mAbs(S32(bestWidth-bestHeight));
					}
				 }


			    // Decrease Width.
				currentWidth >>= 1;
			}

			// Finish if beyond max dimension.
			if ( currentHeight > mMaximumCurrentTextureSize )
				break;

			// Finish if limit-width reached.
			if ( currentWidth < lowerWidth )
				break;
		};
	}

    // Set Texture Page Dimensions.
    texturePage.mWidth = bestWidth;
    texturePage.mHeight = bestHeight;
    texturePage.mFramesOnPage = bestFitOnPage;

    // Pack Frames and update total waste.
    totalWastedSpace += packFrames( vecFrameIn, vecFrameOut, texturePage );
}


// --------------------------------------------------------------------------------------
// Summate Area.
// --------------------------------------------------------------------------------------
U32 t2dImageMapDatablock::getSumArea( const typeFrameIn& vecFrameIn )
{
    U32 sumArea = 0;
    for ( U32 n = 0; n < U32(vecFrameIn.size()); n++ )
    {
        sumArea += vecFrameIn[n].mTargetPixelArea.mWidth*vecFrameIn[n].mTargetPixelArea.mHeight;
    }
    return sumArea;
}


// --------------------------------------------------------------------------------------
// Maximum Width.
// --------------------------------------------------------------------------------------
U32 t2dImageMapDatablock::getMaxWidth( const typeFrameIn& vecFrameIn )
{
    U32 width;
    U32 maxWidth = 0;
    for ( U32 n = 0; n < U32(vecFrameIn.size()); n++ )
    {
        width = vecFrameIn[n].mTargetPixelArea.mWidth;
        if ( width > maxWidth )
            maxWidth = width;
    }
    return maxWidth;
}


// --------------------------------------------------------------------------------------
// Minimum Feasible Width.
// --------------------------------------------------------------------------------------
U32 t2dImageMapDatablock::getMinFeasibleWidth( const typeFrameIn& vecFrameIn, const U32 areaHeight )
{
    // Get Frame Count.
    S32 frameCount = S32(vecFrameIn.size());

    // Reset Minimum Feasible Width.
    U32 minFeasibleWidth = 0;

    U32 binWidth;
    U32 binHeight;

    for ( S32 frameIndex = frameCount-1; frameIndex >= 0; frameIndex-- )
    {
        // Fetch Base Bin-Width.
        binWidth = vecFrameIn[frameIndex].mTargetPixelArea.mWidth;
        // Fetch Base Bin-Height.
        binHeight = vecFrameIn[frameIndex].mTargetPixelArea.mHeight;

        // Increase Feasible Width.
        minFeasibleWidth += binWidth;

        // Finish if this is the last frame.
        if ( frameIndex == 0 )
            return minFeasibleWidth;

        if ( (binHeight+vecFrameIn[frameIndex-1].mTargetPixelArea.mHeight) <= areaHeight )
            return minFeasibleWidth;
    };

    return minFeasibleWidth;
}


// --------------------------------------------------------------------------------------
// Calculate Waste from packing as many frames as possible.
// --------------------------------------------------------------------------------------
void t2dImageMapDatablock::calculatePackFrameWaste( const typeFrameIn& vecFrameIn, const U32 areaWidth, const U32 areaHeight, U32& wastedSpace, U32& fitOnPage )
{
    // Reset Partition Tree.
    cFramePartition::resetPartitionTree( 0, 0, areaWidth, areaHeight );

    // Reset Wasted Space.
    wastedSpace = areaWidth*areaHeight;

    // Calculate Texel Ratios.
    // UNUSED: JOSEPH THOMAS -> F32 texelWidth = 1.0f / areaWidth;
    // UNUSED: JOSEPH THOMAS -> F32 texelHeight = 1.0f / areaHeight;

    U32 frameWidth;
    U32 frameHeight;

    // Get Frame Count.
    S32 frameCount = S32(vecFrameIn.size());

    // Reset Fit On Page.
    fitOnPage = 0;

    // Insert Frames.
    for ( S32 n = frameCount-1; n >= 0; n-- )
    {
        // Fetch Frame Dimensions.
        frameWidth = vecFrameIn[n].mTargetPixelArea.mWidth;
        frameHeight = vecFrameIn[n].mTargetPixelArea.mHeight;

        // Can we fit frame?
        if ( cFramePartition::notEmpty( cFramePartition::insertRootPartition( frameWidth, frameHeight ) ) )
        {
            // Yes, so reduce wasted space.
            wastedSpace -= frameWidth*frameHeight;

            // Increase Fit on Page.
            fitOnPage++;
        }
    }
}

// --------------------------------------------------------------------------------------
// Pack as many frames as possible.
// --------------------------------------------------------------------------------------
U32 t2dImageMapDatablock::packFrames( typeFrameIn& vecFrameIn, typeFrameOut& vecFrameOut, const cTexturePage& texturePage )
{
    // Clear Frame Overflow.
    mVecFrameOverflow.clear();

    // Fetch Texture-Page Width/Height.
    U32 pageWidth = texturePage.mWidth;
    U32 pageHeight = texturePage.mHeight;

    // Calculate Page Texel Ratios.
    F32 texelWidth = 1.0f / pageWidth;
    F32 texelHeight = 1.0f / pageHeight;

    // Reset Partition Tree.
    cFramePartition::resetPartitionTree( 0, 0, pageWidth, pageHeight );

    // Reset Wasted Space.
    U32 wastedSpace = pageWidth*pageHeight;

    U32 frameWidth;
    U32 frameHeight;
    cFrameOut frameOut;
    S32 framePartition;

    // Get Frame Count.
    S32 frameCount = S32(vecFrameIn.size());

    // Insert Frames.
    for ( S32 n = frameCount-1; n >= 0; n-- )
    {
        // Fetch Reference.
        cFrameIn& frameIn = vecFrameIn[n];

        // Fetch Frame Dimensions.
        frameWidth = frameIn.mTargetPixelArea.mWidth;
        frameHeight = frameIn.mTargetPixelArea.mHeight;

        // Attempt to insert frame.
        framePartition = cFramePartition::insertRootPartition( frameWidth, frameHeight );

        // Did it fit?
        if ( cFramePartition::notEmpty(framePartition) )
        {
            // Yes, so reduce wasted space.
            wastedSpace -= frameWidth*frameHeight;

            // Transfer to output frame.
            frameOut.mFrame = frameIn.mFrame;
            frameOut.mTexturePage = texturePage.mTexturePage;
            frameOut.mInputPixelArea = frameIn.mOriginalPixelArea;

			if(mOptimisedImageMap)
			{
				//for optimised images, output == input
				frameOut.mOutputPixelArea = frameOut.mInputPixelArea;
			}
			else
			{
				// Padding?
				if ( mFilterPad )
				{
					// Yes, so fetch partition pixel area.
					cFramePixelArea& partitionPixelArea = cFramePartition::getPartition(framePartition).mPartitionPixelArea;

					// Insert output pixel area.
					frameOut.mOutputPixelArea.setArea( partitionPixelArea.mX+1, partitionPixelArea.mY+1, partitionPixelArea.mWidth-2, partitionPixelArea.mHeight-2 );
				}
				else
				{
					// No, so simply use partition area.
					frameOut.mOutputPixelArea = cFramePartition::getPartition(framePartition).mPartitionPixelArea;
				}
			}

            // Calculate Texel Area.
            frameOut.mOutputTexelArea.convertArea( frameOut.mOutputPixelArea, texelWidth, texelHeight );

            // Store Output frame.
            vecFrameOut.push_back( frameOut );

            // Remove Last Frame.
            vecFrameIn.pop_back();
        }
        else
        {
            // No, so move into overflow.
            mVecFrameOverflow.push_back( frameIn );

            // Remove Last Input-Frame.
            vecFrameIn.pop_back();
        }
    }

    // Do we have any overflow frames?
    frameCount = S32(mVecFrameOverflow.size());
    if ( frameCount > 0 )
    {
        // Yes, so transfer back to input frames.
        for ( S32 n = frameCount-1; n >= 0; n-- )
        {
            // Move into input-frames.
            vecFrameIn.push_back( mVecFrameOverflow[n] );
            // Remove Last Overflow-frame.
            mVecFrameOverflow.pop_back();
        }
    }

    // Return Wasted Space.
    return wastedSpace;
}


// --------------------------------------------------------------------------------------
// Insert Partition.
// --------------------------------------------------------------------------------------
S32 t2dImageMapDatablock::cFramePartition::insertPartition( const U32& width, const U32& height )
{
    // Is Partition a leaf?
    if ( isEmpty(mFirstPartition) && isEmpty(mSecondPartition) )
    {
        // Yes, so we can't fit if it's filled!
        if ( mFilled ) return cFramePartition::empty;

        // Calculate Correct Area Dimensions.
        U32 areaWidth = mPartitionPixelArea.mWidth;
        U32 areaHeight = mPartitionPixelArea.mHeight;

        // Is Area too small?
        if ( areaWidth < width || areaHeight < height )
        {
            // Yes!
            return cFramePartition::empty;
        }

        // Is Area perfect?
        if ( areaWidth == width && areaHeight == height )
        {
            // Yes, so flag as filled.
            mFilled = true;

            // Return this partition.
            return mIndex;
        }

        // Create Some Children for the split.
        mFirstPartition = cFramePartition::createPartition();
        mSecondPartition = cFramePartition::createPartition();

        // Calculate the split dimensions.
        U32 deltaWidth = areaWidth - width;
        U32 deltaHeight = areaHeight - height;

        // Get Area Position.
        U32 areaX = mPartitionPixelArea.mX;
        U32 areaY = mPartitionPixelArea.mY;

        // Decide on the split axis.
        if ( deltaWidth > deltaHeight )
        {
            // Split Vertically...
            getPartition(mFirstPartition).mPartitionPixelArea.setArea( areaX, areaY, width, areaHeight) ;
            getPartition(mSecondPartition).mPartitionPixelArea.setArea( areaX+width, areaY, deltaWidth, areaHeight );
        }
        else
        {
            // Split Horizontally...
            getPartition(mFirstPartition).mPartitionPixelArea.setArea( areaX, areaY, areaWidth, height );
            getPartition(mSecondPartition).mPartitionPixelArea.setArea( areaX, areaY+height, areaWidth, deltaHeight);
        }

        // Return result of inserting into left-child.
        return getPartition(mFirstPartition).insertPartition( width, height );
    }
    else
    {
        // No, so ...
        
        // Try inserting into first-child.
        S32 newPartition = getPartition(mFirstPartition).insertPartition( width, height );
        // Return partition (if inserted).
        if ( notEmpty(newPartition) )
            return newPartition;

        // Return result of inserting into second-child.
        return getPartition(mSecondPartition).insertPartition( width, height );
    }
}


// --------------------------------------------------------------------------------------
// Create Partition.
// --------------------------------------------------------------------------------------
S32 t2dImageMapDatablock::cFramePartition::createPartition( void )
{
    //static cFramePartition blankPartition;

    // All Partitions allocated?
    if ( mCurrentFramePartition >= U32(mVecFramePartition.size()) )
    {
        // Yes, so create a couple of partitions.
        //blankPartition.mIndex = mCurrentFramePartition;
        //mVecFramePartition.push_back( blankPartition );
        //blankPartition.mIndex = mCurrentFramePartition+1;
        //mVecFramePartition.push_back( blankPartition );       

        // [neo, 5/17/2007 - #2361]
        // Partitions can't be stored by value as the vector will call realloc
        // when it needs to resize and pull the rug out from under us here by
        // moving everything that "this" points to while we are executing here.
        cFramePartition *p = new cFramePartition();
        p->mIndex          = mCurrentFramePartition;

        mVecFramePartition.push_back( p );

        p         = new cFramePartition();
        p->mIndex = mCurrentFramePartition + 1;

        mVecFramePartition.push_back( p );
       
        // Allocate the next free partition.
        return mVecFramePartition[mCurrentFramePartition++]->mIndex;
    }
    else
    {
        // No, so allocate the next free partition.
        return mVecFramePartition[mCurrentFramePartition++]->mIndex;
    }
}

// [neo, 5/17/2007 - #3126]
// 
void t2dImageMapDatablock::cFramePartition::clearPartitions()
{
   // Clear Existing Partitions.   
   for( S32 i = 0; i < mVecFramePartition.size(); i++ )
       delete cFramePartition::mVecFramePartition[ i ];

   mVecFramePartition.clear();
}

// --------------------------------------------------------------------------------------
// Reset Partition Tree.
// --------------------------------------------------------------------------------------
void t2dImageMapDatablock::cFramePartition::resetPartitionTree( const U32 x, const U32 y, const U32 width, const U32 height )
{
    //static cFramePartition blankPartition;

    // Clear Existing Partitions.
    // [neo, 5/17/2007 - #3126]
    // 
    clearPartitions();

    // Set Partition Area.
    //blankPartition.mPartitionPixelArea.setArea(x, y, width, height);
    // Store Root Partition.
    //mVecFramePartition.push_back(blankPartition);

    cFramePartition *p = new cFramePartition();
       
    p->mPartitionPixelArea.setArea(x, y, width, height);
    
    // Store Root Partition.
    mVecFramePartition.push_back(p);

    // Next Partition.
    mCurrentFramePartition = 1;
}


// --------------------------------------------------------------------------------------
// Inset Root Partition.
// --------------------------------------------------------------------------------------
S32 t2dImageMapDatablock::cFramePartition::insertRootPartition( const U32 width, const U32 height )
{
    // Sanity!
    AssertFatal( mVecFramePartition.size() > 0, "cFramePartition::insertRootPartition() - Cannot insert into root; partition doesn't exist!" );

    // Insert Partition into the Root.
    return mVecFramePartition[0]->insertPartition( width, height );
}


//------------------------------------------------------------------------------

void t2dImageMapDatablock::setTextureClamp( const bool s, const bool t )
{
    // Process All Texture Pages.
    for ( S32 n = 0; n < mVecTexturePage.size(); n++ )
    {
        // Bind Texture.
        glBindTexture( GL_TEXTURE_2D, mVecTexturePage[n].mpPageTextureHandle->getGLName() );

        // Set Clamp Mode.
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s ? (dglDoesSupportEdgeClamp() ? GL_CLAMP_TO_EDGE : GL_CLAMP) : GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t ? (dglDoesSupportEdgeClamp() ? GL_CLAMP_TO_EDGE : GL_CLAMP) : GL_REPEAT );
    }
}

//------------------------------------------------------------------------------

void t2dImageMapDatablock::setTextureFilter( const eTextureFilterMode filterMode )
{
    // Select Hardware Filter Mode.
    GLint glFilterMode;

    switch( filterMode )
    {
        // Nearest ("none").
        case T2D_FILTER_NEAREST:
        {
            glFilterMode = GL_NEAREST;

        } break;

        // Bilinear ("smooth").
        case T2D_FILTER_BILINEAR:
        {
            glFilterMode = GL_LINEAR;

        } break;

        // Huh?
        default:
            // Oh well...
            glFilterMode = GL_LINEAR;
    };

    // Process All Texture Pages.
    for ( S32 n = 0; n < mVecTexturePage.size(); n++ )
    {
        // Set the texture objects filter mode.
        //
        // NOTE:-   When the TextureManager resurrects itself, it sets filtering based on the 
        //          TextureObjects filterNearest member.
        TextureObject* pTextureObject = (TextureObject*)(*mVecTexturePage[n].mpPageTextureHandle);

        // Set Filter Mode.
        pTextureObject->filterNearest = ( glFilterMode == GL_NEAREST );

        // Bind Texture.
        glBindTexture( GL_TEXTURE_2D, mVecTexturePage[n].mpPageTextureHandle->getGLName() );

        // Set Filter Mode.
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMode );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMode );         
    }
}

//-----------------------------------------------------------------------------
// Set Filter Mode Description.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, compile, bool, 2, 2, "() Compile Image-Map.\n"
			  "@return Returns true on success, false otherwise.")
{
    // Compile ImageMap.
    return object->compileImageMap();
}


//-----------------------------------------------------------------------------
// Set Filter Mode Description.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, setFilterMode, void, 3, 3, "(mode) Set Filter Mode.\n"
			  "@return No return value.")
{
    // Fetch Texture Filter Mode.
    const t2dImageMapDatablock::eTextureFilterMode filterMode = getFilterMode( argv[2] );

    // Valid Filter?
    if ( filterMode == t2dImageMapDatablock::T2D_FILTER_INVALID )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::setFilterMode() - Invalid Filter Mode Specified! (%s)", argv[2] );
        // Finish Here.
        return;
    }

    // Set Filter Mode.
    object->setTextureFilter( filterMode );
}


//-----------------------------------------------------------------------------
// Get Filter Mode Description.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getFilterMode, const char*, 2, 2, "() Get Filter Mode.\n"
			  "@return Returns the filter mode as a string")
{
    // Get Filter Mode Description.
    return getFilterModeDescription( object->getFilterMode() );
}


//-----------------------------------------------------------------------------
// Get ImageMap Mode Description.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getImageMapMode, const char*, 2, 2, "() Get ImageMap Mode.\n"
			  "@return Returns the mode as a string.")
{
    // Get ImageMap Mode Description.
    return getImageModeDescription( object->getImageMapMode() );
}


//-----------------------------------------------------------------------------
// Get Source Bitmap Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getSrcBitmapName, const char*, 2, 2, "() Get Source Bitmap Name.\n"
			  "@return Returns the bitmap name as a string.")
{
    // Return Bitmap Name.
    return object->getSrcBitmapName();
}


//-----------------------------------------------------------------------------
// Get Source Bitmap Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getSrcBitmapSize, const char*, 2, 2, "() Get Source Bitmap Size.\n"
			  "@return Returns the size as a string formatted as \"width height\"")
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getSrcBitmapWidth(), object->getSrcBitmapHeight());
    // Return Buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get ImageMap Frame Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getFrameCount, S32, 2, 2, "() Get ImageMap Frame Count.\n"
			  "@return Returns the count as an integer")
{
    // Get ImageMap Frame Count.
    return object->getImageMapFrameCount(); 
}


//-----------------------------------------------------------------------------
// Get ImageMap Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getFrameSize, const char*, 3, 3, "(int frame) - Get ImageMap Frame Size.\n"
			  "@param frame The number of the frame to check\n"
			  "@return Returns the frame's size as a string formatted as \"width height\"")
{
    // Fetch Frame.
    S32 frame = dAtoi(argv[2]);

    // Check Frame.
    if ( frame < 0 || frame >= object->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::getImageMapFrameSize() - Invalid Frame; Allowed range is 0 to %d", object->getImageMapFrameCount()-1 );
        // Finish Here.
        return NULL;
    }

    // Fetch Selected Frame Pixel Area.
    const t2dImageMapDatablock::cFramePixelArea& framePixelArea = object->getImageMapFramePixelArea( frame );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", framePixelArea.mWidth, framePixelArea.mHeight );
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get ImageMap Frame Texture Page.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getFrameTexturePage, S32, 3, 3, "(int frame) - Get ImageMap Frame Texture Page.\n"
			  "@param frame The frame number\n"
			  "@return The image mapo texture page index.")
{
    // Fetch Frame.
    S32 frame = dAtoi(argv[2]);

    // Check Frame.
    if ( frame < 0 || frame >= object->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::getImageMapFrameSize() - Invalid Frame; Allowed range is 0 to %d", object->getImageMapFrameCount()-1 );
        // Finish Here.
        return NULL;
    }

    // Return Texture Page.
    return object->getImageMapFrame( frame ).mTexturePage;
}


//-----------------------------------------------------------------------------
// Get ImageMap Texture Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getTexturePageCount, S32, 2, 2, "() Get ImageMap Texture-Page Count.\n"
			  "@return The number of texture-pages")
{
    // Get ImageMap Texture Count.
    return object->getImageMapTextureCount();   
}


//-----------------------------------------------------------------------------
// Get ImageMap Texture Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getTexturePageSize, const char*, 3, 3, "(int texturePage) - Get ImageMap Texture-Page Size.\n"
			  "@param texturePage The index of texture-page.\n"
			  "@return Returns the size of the object as a string formatted as \"width height\"")
{
    // Fetch Texture Page.
    S32 page = dAtoi(argv[2]);

    // Check Texture Page.
    if ( page < 0 || page >= object->getImageMapTextureCount() )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::getImageMapTextureSize() - Invalid Texture Page; Allowed range is 0 to %d", object->getImageMapTextureCount()-1 );
        // Finish Here.
        return NULL;
    }

    // Fetch Selected Frame Pixel Area.
    const t2dImageMapDatablock::cTexturePage& texturePage = object->getImageMapTexturePage( page );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", texturePage.mWidth, texturePage.mHeight );
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get ImageMap Texture Frame Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dImageMapDatablock, getTexturePageFrameCount, S32, 3, 3, "(int texturePage) - Get ImageMap Texture-Page Frame Count.\n"
			  "@param texturePage The index of the texturePage\n"
			  "@return Returns the numbers of frames in the page.")
{
    // Fetch Texture Page.
    S32 page = dAtoi(argv[2]);

    // Check Texture Page.
    if ( page < 0 || page >= object->getImageMapTextureCount() )
    {
        // Warn.
        Con::warnf("t2dImageMapDatablock::getTexturePageFrameCount() - Invalid Texture Page; Allowed range is 0 to %d", object->getImageMapTextureCount()-1 );
        // Finish Here.
        return NULL;
    }

    // Return Frames on Page.
    return object->getImageMapTexturePage( page ).mFramesOnPage;
}




//------------------------------------------------------------------------------

void t2dImageMapDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
    //stream->writeString( mSrcBitmapName );
    //stream->write( mImageCount );
}

//------------------------------------------------------------------------------

void t2dImageMapDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);

    // Read Datablock.
    //mSrcBitmapName = stream->readSTString();
    //stream->read( &mImageCount );
}


//------------------------------------------------------------------------------
// Register t2d imageMap datablock type
//------------------------------------------------------------------------------
IMPLEMENT_CONSOLETYPE(t2dImageMapDatablock)
IMPLEMENT_GETDATATYPE(t2dImageMapDatablock)
IMPLEMENT_SETDATATYPE(t2dImageMapDatablock)


//======================================================================================================================================================================
// RKS NOTE: I ADDED THIS HACK FOR WATER BITMAP STUFF
void _HACK_GetNewWaterBitmapName( const char* _pszLevelName )
{
	if( _pszLevelName == NULL )
		return;
	
	if( strcmp( _pszLevelName, "Level_01_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_01_01.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_02_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_02_01.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_03_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_03_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_03_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_03_02.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_04_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_04_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_04_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_04_02.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_05_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_05_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_05_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_05_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_05_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_05_03.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_06_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_06_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_06_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_06_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_06_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_06_03.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_07_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_07_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_07_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_07_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_07_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_07_03.png" );
	}
	else if( strcmp( _pszLevelName, "Level_07_04.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_07_04.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_08_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_08_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_08_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_08_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_08_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_08_03.png" );
	}
	else if( strcmp( _pszLevelName, "Level_08_04.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_08_04.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_09_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_09_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_09_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_09_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_09_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_09_03.png" );
	}
	else if( strcmp( _pszLevelName, "Level_09_04.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_09_04.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Level_10_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_10_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_10_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_10_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_10_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_10_03.png" );
	}
	else if( strcmp( _pszLevelName, "Level_10_04.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_10_04.png" );
	}
	
	
	
	else if( strcmp( _pszLevelName, "Boss_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_boss_1.png" );
	}
	else if( strcmp( _pszLevelName, "Boss_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_boss_2.png" );
	}
	else if( strcmp( _pszLevelName, "Boss_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_boss_3.png" );
	}
	
	// DLC Levels
	else if( strcmp( _pszLevelName, "Level_11_01.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_11_01.png" );
	}
	else if( strcmp( _pszLevelName, "Level_11_02.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_11_02.png" );
	}
	else if( strcmp( _pszLevelName, "Level_11_03.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_11_03.png" );
	}
	else if( strcmp( _pszLevelName, "Level_11_04.t2d" ) == 0 )
	{
		strcpy( g_szNewWaterBitmapName, "game/data/images/water_11_04.png" );
	}
}

//======================================================================================================================================================================

// RKS NOTE: I ADDED THIS HACK FOR SCORE SCREEN BITMAP STUFF
void _HACK_GetNewScoreScreenBitmapName( const char* _pszLevelName )
{
	if( _pszLevelName == NULL )
		return;
	
	//score_screen_01
	//score_screen_02
	//score_screen_03
	//score_screen_04
	
	// Ignore boss levels
	if( strstr( _pszLevelName, "Boss_" ) != NULL )
		return;
	
	// Score Screen 1
	if( strcmp( _pszLevelName, "Level_01_01.t2d" ) == 0 ||
	    strcmp( _pszLevelName, "Level_02_01.t2d" ) == 0 )
	{
		strcpy( g_szNewScoreScreenBitmapName, "game/data/images/score_screen_01.png" );
		return;
	}
	
	// Score Screen 2
	if( strcmp( _pszLevelName, "Level_03_01.t2d" ) == 0 )
	{
		strcpy( g_szNewScoreScreenBitmapName, "game/data/images/score_screen_02.png" );
		return;
	}
	
	// Score Screen 3
	if( strcmp( _pszLevelName, "Level_03_02.t2d" ) == 0 || 
	    strcmp( _pszLevelName, "Level_04_01.t2d" ) == 0 )
	{
		strcpy( g_szNewScoreScreenBitmapName, "game/data/images/score_screen_03.png" );
		return;
	}
	
	// The remainder of the levels (4.2 - 10.4) use Score Screen 4  (06/17/2011 - Also the extra DLC levels)
	strcpy( g_szNewScoreScreenBitmapName, "game/data/images/score_screen_04.png" );
}
