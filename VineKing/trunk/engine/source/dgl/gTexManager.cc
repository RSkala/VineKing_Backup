//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platformAssert.h"
#include "platform/platformGL.h"
#include "platform/platform.h"
#include "core/tVector.h"
#include "core/resManager.h"
#include "dgl/gBitmap.h"
#include "dgl/gPalette.h"
#include "dgl/gTexManager.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleTypes.h"
#include "dgl/gChunkedTexManager.h"
#include "util/safeDelete.h"

//------------------------------------------------------------------------------

bool gDGLRender = true;

bool sgResurrect = false;
bool sgForcePalettedTexture = false;
bool sgForce16BitTexture    = false;


#define  ENABLE_HOLDING    1

#ifdef TORQUE_GATHER_METRICS
U32 TextureManager::smTextureSpaceLoaded = 0;
U32 TextureManager::smTextureCacheMisses = 0;
#endif

bool TextureManager::smUseSmallTextures = false;
bool TextureManager::smIsZombie = false;
bool TextureManager::smTextureManagerActive = false;

//--------------------------------------------------------------------------
//-------------------------------------- Texture detailing control variables
//                                        0: Highest
//                                        1: ...
//                                        2: ...
//                                        3: Lowest

namespace {

	struct Forced16BitMapping
	{
		GLenum wanted;
		GLenum forced;
		bool   end;
	};

#ifdef TORQUE_OS_IPHONE
#define EXT_ARRAY_SIZE 5
	static const char* extArray[EXT_ARRAY_SIZE] = { "", ".pvr", ".jpg", ".png",  ".bmp" };
	static const char* extArray_8[EXT_ARRAY_SIZE] = { "", ".pvr", ".bmp", ".jpg", ".png" };
#else
	Forced16BitMapping sg16BitMappings[] =
	{
		{ GL_RGB,  GL_RGB5,  false },
		{ GL_RGBA, GL_RGBA4, false },
		{ 0, 0, true }
	};

#define EXT_ARRAY_SIZE 4
	static const char* extArray[EXT_ARRAY_SIZE] = { "", ".jpg", ".png",  ".bmp"};
	static const char* extArray_8[EXT_ARRAY_SIZE] = { "",  ".bmp", ".jpg", ".png" };
#endif

	U32    sgTextureDetailLevel         = 0;
	U32    sgSkyTextureDetailLevel      = 0;
	U32    sgInteriorTextureDetailLevel = 0;
	bool   sgAllowTexCompression  = false;
	GLenum sgCompressionHint      = GL_FASTEST;
	F32    sgTextureAnisotropy    = 0.0; // default aniso, when available.  not sure why prefs.cs isn't setting.
	bool   sgDisableSubImage      = false;
	bool   sgTextureTrilinear      = false;

	ConsoleFunctionGroupBegin( OpenGLTex, "Functions controlling OpenGL parameters.");

	ConsoleFunction(setOpenGLMipReduction, void, 2, 2, "( reductionVal ) Use the setOpenGLMipReduction function to control shape texture detail\n"
		"@param reductionVal An integer value between 0 and 5, with 0 being the lowest quality and 5 being the highest quality.\n"
		"@return No return value.\n"
		"@sa setOpenGLInteriorMipReduction, setOpenGLSkyMipReduction")
	{
		argc;
		S32 val = dAtoi(argv[1]);
		if (val < 0)
			val = 0;
		else if (val > 5)
			val = 5;

		sgTextureDetailLevel = val;
	}

	ConsoleFunction(setOpenGLSkyMipReduction, void, 2, 2, "( reductionVal ) Use the setOpenGLSkyMipReduction function to control texture detail for the skybox and clouds.\n"
		"@param reductionVal An integer value between 0 and 5, with 0 being the lowest quality and 5 being the highest quality.\n"
		"@return No return value.\n"
		"@sa setOpenGLInteriorMipReduction, setOpenGLMipReduction")
	{
		argc;
		S32 val = dAtoi(argv[1]);
		if (val < 0)
			val = 0;
		else if (val > 5)
			val = 5;

		sgSkyTextureDetailLevel = val;
	}

	ConsoleFunction(setOpenGLInteriorMipReduction, void, 2, 2, "( reductionVal ) Use the setOpenGLInteriorMipReduction function to set the texture quality for interiors.\n"
		"@param reductionVal An integer value between 0 and 5, with 0 being the lowest quality and 5 being the highest quality.\n"
		"@return No return value.\n"
		"@sa setOpenGLMipReduction ,setOpenGLSkyMipReduction")
	{
		argc;
		S32 val = dAtoi(argv[1]);
		if (val < 0)
			val = 0;
		else if (val > 5)
			val = 5;

		sgInteriorTextureDetailLevel = val;
	}

	ConsoleFunction(setOpenGLTextureCompressionHint, void, 2, 2, " ( hint ) Use the setOpenGLTextureCompressionHint function to select the OpenGL texture compression method.\n"
		"@param hint \"GL_DONT_CARE\", \"GL_FASTEST\", or \"GL_NICEST\". (Please refer to an OpenGL text for information on what these mean).\n"
		"@return No return value")
	{
		argc;

		GLenum newHint        = GL_DONT_CARE;
		const char* newString = "GL_DONT_CARE";

		if (!dStricmp(argv[1], "GL_FASTEST"))
		{
			newHint = GL_FASTEST;
			newString = "GL_FASTEST";
		}
		else if (!dStricmp(argv[1], "GL_NICEST"))
		{
			newHint = GL_NICEST;
			newString = "GL_NICEST";
		}

		sgCompressionHint = newHint;

#if !defined(TORQUE_OS_IPHONE)
		if (dglDoesSupportTextureCompression())
			glHint(GL_TEXTURE_COMPRESSION_HINT_ARB, sgCompressionHint);
#endif
	}

	ConsoleFunction(setOpenGLAnisotropy, void, 2, 2, "( 0.0 .. max.f ) Use the setOpenGLAnisotropy function to enable or disable anisotropic filtering.\n"
		"Anisotropic filtering is somewhat 'expensive' filtering technique that uses more texels than your average filtering technique (bilinear or trilinear) for determining the color of a pixel for cases where more than one texel may be responsible for that pixels color\n"
		"@param 0.0 . max.f - A value between 0.0 and the maximum anisotropic level supported by the current machine. Selecting a value higher than max.f results in max.f.\n"
		"@return No return value.")
	{
		argc;
		F32 val = dAtof(argv[1]);
		if (val < 0.0)
			val = 0.0;
		if (val > dglGetMaxAnisotropy())
			val = dglGetMaxAnisotropy();
		sgTextureAnisotropy = val;

		if(dglDoesSupportTexAnisotropy())
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, sgTextureAnisotropy * dglGetMaxAnisotropy());
	}

	ConsoleFunctionGroupEnd( OpenGLTex );

} // namespace {}


//--------------------------------------
struct TextureDictionary
{
	static TextureObject **smTable;
	static TextureObject *smTOList;
	static U32 smHashTableSize;

	static void create();
	static void preDestroy();
	static void destroy();

	static void insert(TextureObject *object);
	static TextureObject *find(StringTableEntry name, TextureHandleType type, bool clamp);
	static void remove(TextureObject *object);
	static S32 clearHolds();
};

TextureObject **TextureDictionary::smTable = NULL;
TextureObject *TextureDictionary::smTOList = NULL;
U32 TextureDictionary::smHashTableSize = 0;

//--------------------------------------
void TextureDictionary::create()
{
	smTOList = NULL;
	smHashTableSize = 1023;
	smTable = new TextureObject *[smHashTableSize];
	for(U32 i = 0; i < smHashTableSize; i++)
		smTable[i] = NULL;

	Con::addVariable("$pref::OpenGL::force16BitTexture",    TypeBool, &sgForce16BitTexture);
	Con::addVariable("$pref::OpenGL::forcePalettedTexture", TypeBool, &sgForcePalettedTexture);
	Con::addVariable("$pref::OpenGL::allowCompression",     TypeBool, &sgAllowTexCompression);
	Con::addVariable("$pref::OpenGL::disableSubImage",      TypeBool, &sgDisableSubImage);

	Con::addVariable("$pref::OpenGL::textureTrilinear",     TypeBool, &sgTextureTrilinear);
	Con::addVariable("$pref::OpenGL::textureAnisotropy",    TypeF32,  &sgTextureAnisotropy);
}


//--------------------------------------
TextureObject *TextureDictionary::find(StringTableEntry name, TextureHandleType type, bool clamp)
{
	U32 key = HashPointer(name) % smHashTableSize;
	TextureObject *walk = smTable[key];
	for(; walk; walk = walk->hashNext)
		if(walk->texFileName == name && walk->type == type && walk->clamp == clamp)
			break;
	return walk;
}


//--------------------------------------
void TextureDictionary::remove(TextureObject *object)
{
	if(object->next)
		object->next->prev = object->prev;

	if(object->prev)
		object->prev->next = object->next;
	else
		smTOList = object->next;

	if(!object->texFileName)
		return;

	U32 key = HashPointer(object->texFileName) % smHashTableSize;
	TextureObject **walk = &smTable[key];
	while(*walk)
	{
		if(*walk == object)
		{
			*walk = object->hashNext;
			break;
		}
		walk = &((*walk)->hashNext);
	}
}


//--------------------------------------
void TextureDictionary::insert(TextureObject *object)
{
	object->next = smTOList;
	object->prev = NULL;
	if(smTOList)
		smTOList->prev = object;
	smTOList = object;

	if(object->texFileName)
	{
		U32 key = HashPointer(object->texFileName) % smHashTableSize;

		object->hashNext = smTable[key];
		smTable[key] = object;
	}
}

//--------------------------------------
void TextureDictionary::preDestroy()
{
	// This is a horrid hack, but it will have to do for now.  (DMM, aided
	//  and abetted by MF.)
	TextureObject* walk = smTOList;
	while (walk)
	{
		if((gDGLRender || sgResurrect) && walk->texGLName)
			glDeleteTextures(1, (const GLuint*)&walk->texGLName);
		if((gDGLRender || sgResurrect) && walk->smallTexGLName)
			glDeleteTextures(1, (const GLuint*)&walk->smallTexGLName);
		//delete walk->bitmap;
		walk->texGLName = 0;
		walk->smallTexGLName = 0;
		//walk->bitmap = NULL;

		walk = walk->next;
	}
}

//--------------------------------------
void TextureDictionary::destroy()
{
	// This is a horrid hack, but it will have to do for now.  (DMM, aided
	//  and abetted by MF.)
	while(smTOList)
		TextureManager::freeTexture(smTOList);
	delete[] smTable;
}

//--------------------------------------
S32 TextureDictionary::clearHolds()
{
	Vector<TextureObject *>    holds;

	// Find held textures to delete.  Clear holding flag too so they're free
	// to go away.
	for (TextureObject * walk = smTOList; walk; walk = walk->next)
	{
		if (walk->holding)
		{
			if (!walk->refCount)
				holds.push_back(walk);
			else
				walk->holding = false;
		}
	}

	// Remove them-
	for (S32 i = 0; i < holds.size(); i++)
		TextureManager::freeTexture(holds[i]);

	return holds.size();
}

ConsoleFunction(clearTextureHolds, S32, 1, 1, "() Use the clearTextureHolds function to free and release any held textures, returning the size of the held textures free.\n"
				"As long as a texture is not currently in use, it will be released.\n"
				"@return Returns the space freed.\n"
				"@sa dumpTextureStats, flushTextureCache, purgeResources")
{
	argc; argv;
	return TextureDictionary::clearHolds();
}


//--------------------------------------------------------------------------
//--------------------------------------
//
struct EventCallbackEntry
{
	TextureEventCallback callback;
	void *               userData;
	U32                  key;
};
static U32                        sgCurrCallbackKey = 0;
static Vector<EventCallbackEntry> sgEventCallbacks(__FILE__, __LINE__);

U32  TextureManager::registerEventCallback(TextureEventCallback callback, void *userData)
{
	sgEventCallbacks.increment();
	sgEventCallbacks.last().callback = callback;
	sgEventCallbacks.last().userData = userData;
	sgEventCallbacks.last().key      = sgCurrCallbackKey++;

	return sgEventCallbacks.last().key;
}

void TextureManager::unregisterEventCallback(const U32 callbackKey)
{
	for (S32 i = 0; i < sgEventCallbacks.size(); i++)
		if (sgEventCallbacks[i].key == callbackKey) {
			sgEventCallbacks.erase(i);
			return;
		}
}

void TextureManager::postTextureEvent(const U32 eventCode)
{
	for (S32 i = 0; i < sgEventCallbacks.size(); i++)
		(sgEventCallbacks[i].callback)(eventCode, sgEventCallbacks[i].userData);
}


void TextureManager::create()
{
	AssertISV(!smTextureManagerActive, "TextureManager::create - already created!");

	TextureDictionary::create();
	smTextureManagerActive = true;
}

void TextureManager::preDestroy()
{
	AssertISV(smTextureManagerActive, "TextureManager::preDestroy - nothing to destroy!");

	TextureDictionary::preDestroy();
}

void TextureManager::destroy()
{
	AssertISV(smTextureManagerActive, "TextureManager::destroy - nothing to destroy!");

	TextureDictionary::destroy();

	AssertFatal(sgEventCallbacks.size() == 0,
		"Error, some object didn't unregister it's texture event callback function!");

	smTextureManagerActive = false;
}


//--------------------------------------
void TextureManager::makeZombie()
{
	if (smIsZombie == true)
		return;
	smIsZombie = true;

	postTextureEvent(BeginZombification);
	ChunkedTextureManager::makeZombie();

	// Publish flush event?

	Vector<GLuint> deleteNames(4096);

	TextureObject* probe = TextureDictionary::smTOList;
	while (probe) 
	{
		AssertFatal(probe->type != TerrainTexture, "Error, all the terrain textureobjects should be gone by now!");

		if (probe->type == BitmapNoDownloadTexture)
		{
			probe = probe->next;
			continue;
		}

		if (probe->texGLName != 0)
			deleteNames.push_back(probe->texGLName);
		if (probe->smallTexGLName != 0)
			deleteNames.push_back(probe->smallTexGLName);

#ifdef TORQUE_GATHER_METRICS
		AssertFatal(probe->textureSpace <= smTextureSpaceLoaded, "Error, that shouldn't happen!");
		smTextureSpaceLoaded -= probe->textureSpace;
		probe->textureSpace   = 0;
#endif

		probe->texGLName      = 0;
		probe->smallTexGLName = 0;

		probe = probe->next;
	}

	glDeleteTextures(deleteNames.size(), deleteNames.address());
}

void TextureManager::resurrect()
{
	if (smIsZombie == false)
		return;
	smIsZombie = false;

	sgResurrect = true;

	// Get rid of any chunked textures created while the app was inactive
	ChunkedTextureManager::makeZombie();

	//Luma:	Melv May's Memory Reduction
	// Emit Begin-Ressurrection Event.
	postTextureEvent(BeginResurrection);

	// Reload textures...
	TextureObject* probe = TextureDictionary::smTOList;
	while (probe) 
	{
		// reload texture...
		AssertFatal(probe->type != TerrainTexture, "Error, all the terrain textureobjects should be gone by now!");

		//Luma:	Melv May's Memory Reduction
		if ((probe->type == BitmapNoDownloadTexture) || (probe->type == ImageMap2DTexture) || (probe->type == ImageMap2DKeepTexture))
		{
			probe = probe->next;
			continue;
		}

		if (probe->bitmap != NULL) 
		{
			if(probe->type == BitmapKeepTexture)
			{
				delete probe->bitmap;
				probe->bitmap = NULL;
			}
			else
			{
				if (probe->type == RegisteredTexture) 
				{
					createGLName(probe->bitmap, probe->clamp, 0, probe->type, probe);
				}
				else 
				{
					TextureObject* refreshed = registerTexture(probe->texFileName, probe->bitmap,
						probe->type, probe->clamp);
					AssertFatal(refreshed == probe, "Error, new texture object returned.  This should not happen in resurrect");
				}

				probe = probe->next;
				continue;
			}
		}

		// Ok, what we have here is the object, with the right name, we need to load the
		// bitmap, and register the texture
		GBitmap *bmp = loadBitmapInstance(probe->texFileName);
		AssertISV(bmp != NULL, "Error resurrecting the texture cache.\n"
			"Possible cause: a bitmap was deleted during the course of gameplay.");

		TextureObject* refreshed = registerTexture(probe->texFileName, bmp,
			probe->type, probe->clamp);
		AssertFatal(refreshed == probe, "Error, new texture object returned.  This should not happen in resurrect");

		probe = probe->next;
	}

	ChunkedTextureManager::resurrect();
	postTextureEvent(CacheResurrected);

	sgResurrect = false;
}

void TextureManager::flush()
{
	makeZombie();
	resurrect();
}

ConsoleFunction( flushTextureCache, void, 1, 1, "() Use the flushTextureCache function to flush the texture cache.\n"
				"@return No return value.\n"
				"@sa clearTextureHolds, dumpTextureStats, purgeResources")
{
	TextureManager::flush();
}

#ifdef TORQUE_GATHER_METRICS
void TextureManager::dumpStats()
{
	TextureObject* probe = TextureDictionary::smTOList;

	Con::errorf("aaa Texture dump");
	while (probe)
	{
		Con::errorf("aaa    %d: (%d, %s) %d (%s)", probe->type, probe->refCount, probe->holding ? "yes" : "no", probe->textureSpace, probe->texFileName ? probe->texFileName : "nil");
		probe = probe->next;
	}
}

ConsoleFunction(dumpTextureStats, void, 1, 1, "() Use the dumpTextureStats function to dump information about each texture currently in use to the console. This information will be printed in this format:\naaa type: (refCount, holding) textureSpace (filename)\nOutput Syntax\ntype: - Type of this texture. See\n'Texture Types' list below.\nrefCount - Number of references to this texture.\nholding - Is this texture being held? \"yes\" or \"no\"\ntextureSpace - Bytes used by this texture.\nfilename - Full path to this texture.\n"
				"For this to work, the engine must have been compiled with TORQUE_DEBUG defined.\n"
				"@return No return value.\n"
				"@sa clearTextureHolds, flushTextureCache")
{
	TextureManager::dumpStats();
}
#endif


//------------------------------------------------------------------------------
GBitmap* TextureManager::createPaddedBitmap(GBitmap* pBitmap)
{
	if (isPow2(pBitmap->getWidth()) && isPow2(pBitmap->getHeight()))
		return pBitmap;

	AssertFatal(pBitmap->getNumMipLevels() == 1,
		"Cannot have non-pow2 bitmap with miplevels");

	U32 width = pBitmap->getWidth();
	U32 height = pBitmap->getHeight();

	U32 newWidth  = getNextPow2(pBitmap->getWidth());
	U32 newHeight = getNextPow2(pBitmap->getHeight());

	GBitmap* pReturn = new GBitmap(newWidth, newHeight, false, pBitmap->getFormat());

	for (U32 i = 0; i < height; i++) 
	{
		U8*       pDest = (U8*)pReturn->getAddress(0, i);
		const U8* pSrc  = (const U8*)pBitmap->getAddress(0, i);

		dMemcpy(pDest, pSrc, width * pBitmap->bytesPerPixel);

		pDest += width * pBitmap->bytesPerPixel;
		// set the src pixel to the last pixel in the row
		const U8 *pSrcPixel = pDest - pBitmap->bytesPerPixel;

		for(U32 j = width; j < newWidth; j++)
			for(U32 k = 0; k < pBitmap->bytesPerPixel; k++)
				*pDest++ = pSrcPixel[k];
	}

	for(U32 i = height; i < newHeight; i++)
	{
		U8* pDest = (U8*)pReturn->getAddress(0, i);
		U8* pSrc = (U8*)pReturn->getAddress(0, height-1);
		dMemcpy(pDest, pSrc, newWidth * pBitmap->bytesPerPixel);
	}

#if !defined(TORQUE_OS_IPHONE)
	if (pBitmap->getFormat() == GBitmap::Palettized)
	{
		pReturn->pPalette = new GPalette;
		dMemcpy(pReturn->pPalette->getColors(), pBitmap->pPalette->getColors(), sizeof(ColorI) * 256);
		pReturn->pPalette->setPaletteType(pBitmap->pPalette->getPaletteType());
	}
#endif
	return pReturn;
}


//------------------------------------------------------------------------------
GBitmap* TextureManager::createMipBitmap(const GBitmap* pBitmap)
{
	AssertFatal(pBitmap != NULL, "Error, no bitmap");
	AssertFatal(pBitmap->getNumMipLevels() != 1, "Error, no mips to maintain");

	GBitmap* pRetBitmap = new GBitmap(pBitmap->getWidth(1),
		pBitmap->getHeight(1),
		true,
		pBitmap->getFormat());

	for (U32 i = 1; i < pBitmap->getNumMipLevels(); i++)
	{
		void* pDest      = pRetBitmap->getWritableBits(i - 1);
		const void* pSrc = pBitmap->getBits(i);

		dMemcpy(pDest, pSrc, (pBitmap->getWidth(i)  *
			pBitmap->getHeight(i) *
			pBitmap->bytesPerPixel));
	}

	return pRetBitmap;
}


//------------------------------------------------------------------------------
void TextureManager::freeTexture(TextureObject *to)
{
#ifdef TORQUE_GATHER_METRICS
	AssertFatal(to->textureSpace <= smTextureSpaceLoaded, "Error, that shouldn't happen!");
	smTextureSpaceLoaded -= to->textureSpace;
#endif

	if((gDGLRender || sgResurrect) && to->texGLName)
		glDeleteTextures(1, (const GLuint*)&to->texGLName);
	if((gDGLRender || sgResurrect) && to->smallTexGLName)
		glDeleteTextures(1, (const GLuint*)&to->smallTexGLName);

	SAFE_DELETE( to->bitmap );
	TextureDictionary::remove(to);
	SAFE_DELETE( to );
}


//------------------------------------------------------------------------------
static void getSourceDestByteFormat(GBitmap *pBitmap, U32 *sourceFormat, U32 *destFormat, U32 *byteFormat)
{
	*byteFormat = GL_UNSIGNED_BYTE;
#if defined(TORQUE_OS_IPHONE)
	switch(pBitmap->getFormat()) 
	{
	case GBitmap::Intensity:
		AssertFatal( 0, "GBitmap::Intensity GL_INTENSITY format not supported" );
		break;
	case GBitmap::Palettized:
		AssertFatal( 0, "GBitmap::Palettized GL_COLOR_INDEX format not supported" );
		break;
	case GBitmap::Luminance:
		*sourceFormat = GL_LUMINANCE;
		break;
	case GBitmap::RGB:
		*sourceFormat = GL_RGB;
		break;
	case GBitmap::RGBA:
		*sourceFormat = GL_RGBA;
		break;
	case GBitmap::Alpha:
		*sourceFormat = GL_ALPHA;
		break;
	case GBitmap::RGB565:
		*sourceFormat = GL_RGB;
		*byteFormat   = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case GBitmap::RGB5551:
		*sourceFormat = GL_RGBA;
		*byteFormat   = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
	case GBitmap::PVR2:
		*sourceFormat = GL_RGB;
		*byteFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
		break;
	case GBitmap::PVR2A:
		*sourceFormat = GL_RGBA;
		*byteFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
		break;
	case GBitmap::PVR4:
		*sourceFormat = GL_RGB;
		*byteFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		break;
	case GBitmap::PVR4A:
		*sourceFormat = GL_RGBA;
		*byteFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		break;
	}
	*destFormat = *sourceFormat;
	return;
#else	
	switch(pBitmap->getFormat()) 
	{
	case GBitmap::Intensity:
		*sourceFormat = GL_INTENSITY;
		break; 

	case GBitmap::Palettized:
		*sourceFormat = GL_COLOR_INDEX;
		break;

	case GBitmap::Luminance:
		*sourceFormat = GL_LUMINANCE;
		break;
	case GBitmap::RGB:
		*sourceFormat = GL_RGB;
		break;
	case GBitmap::RGBA:
		*sourceFormat = GL_RGBA;
		break;
	case GBitmap::Alpha:
		*sourceFormat = GL_ALPHA;
		break;

	case GBitmap::RGB565:
	case GBitmap::RGB5551:
#if defined(TORQUE_BIG_ENDIAN)
		*sourceFormat = GL_BGRA_EXT;
		*byteFormat   = GL_UNSIGNED_SHORT_1_5_5_5_REV;
#else
		*sourceFormat = GL_RGBA;
		*byteFormat   = GL_UNSIGNED_SHORT_5_5_5_1;
#endif
		break;
	};

	if(*byteFormat == GL_UNSIGNED_BYTE)
	{
		if (*sourceFormat != GL_COLOR_INDEX)
			*destFormat = *sourceFormat;
		else
			*destFormat = GL_COLOR_INDEX8_EXT;

		if (pBitmap->getNumMipLevels() > 1 &&
			pBitmap->getFormat() != GBitmap::Palettized &&
			(sgAllowTexCompression && dglDoesSupportTextureCompression()))
		{
			if (*sourceFormat == GL_RGB)
				*destFormat = GL_COMPRESSED_RGB_ARB;
			else if (*sourceFormat == GL_RGBA)
				*destFormat = GL_COMPRESSED_RGBA_ARB;
		}
	}
	else
	{
		*destFormat = GL_RGB5_A1;
	}

	if (sgForce16BitTexture)
	{
		for (U32 i = 0; sg16BitMappings[i].end != true; i++)
		{
			if (*destFormat == sg16BitMappings[i].wanted)
			{
				*destFormat = sg16BitMappings[i].forced;
				return;
			}
		}
	}
	else
	{
		if(*destFormat == GL_RGB)
			*destFormat = GL_RGB8;
		else if(*destFormat == GL_RGBA)
			*destFormat = GL_RGBA8;
	}
#endif // defined(TORQUE_OS_IPHONE)
}


//--------------------------------------
void TextureManager::refresh(TextureObject *to)
{
	if (!(gDGLRender || sgResurrect))
		return;

	U32 sourceFormat, destFormat, byteFormat;
	GBitmap *pBitmap = to->bitmap;

	getSourceDestByteFormat(pBitmap, &sourceFormat, &destFormat, &byteFormat);

	if (!to->texGLName)
		glGenTextures(1,&to->texGLName);

	glBindTexture(GL_TEXTURE_2D, to->texGLName);
	GBitmap *pDL = createPaddedBitmap(pBitmap);

	U32 maxDownloadMip = pDL->getNumMipLevels();
	if (to->type == BitmapTexture ||
		to->type == BitmapKeepTexture ||
		//Luma:	Melv May's Memory Reduction
		to->type == ImageMap2DTexture ||
		to->type == ImageMap2DKeepTexture ||
		to->type == BitmapNoDownloadTexture)
	{
		maxDownloadMip = 1;
	}

#if !defined(TORQUE_OS_IPHONE)
	if (pDL->getFormat() == GBitmap::Palettized)
	{
		glColorTableEXT(GL_TEXTURE_2D,
			pDL->getPalette()->getPaletteType() == GPalette::RGB ? GL_RGB : GL_RGBA,
			256,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pDL->getPalette()->getColors());
	}
#endif

	if (sgDisableSubImage)
	{
		for (U32 i = 0; i < maxDownloadMip; i++)
		{
			glTexImage2D(GL_TEXTURE_2D,
				i,
				destFormat,
				pDL->getWidth(i), pDL->getHeight(i),
				0,
				sourceFormat,
				byteFormat,
				pDL->getBits(i));
		}
	}
	else
	{
		for (U32 i = 0; i < maxDownloadMip; i++)
		{
			glTexSubImage2D(GL_TEXTURE_2D,
				i,
				0, 0,
				pDL->getWidth(i), pDL->getHeight(i),
				sourceFormat,
				byteFormat,
				pDL->getBits(i));
		}
	}

	if ((to->type == InteriorTexture || to->type == MeshTexture) &&
		pDL->getNumMipLevels() > 4)
	{
		//
		if (!to->smallTexGLName)
			glGenTextures(1,&to->smallTexGLName);

		glBindTexture(GL_TEXTURE_2D, to->smallTexGLName);

#if !defined(TORQUE_OS_IPHONE)
		if (pDL->getFormat() == GBitmap::Palettized)
		{
			glColorTableEXT(GL_TEXTURE_2D,
				pDL->getPalette()->getPaletteType() == GPalette::RGB ? GL_RGB : GL_RGBA,
				256,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				pDL->getPalette()->getColors());
		}
#endif

		if (sgDisableSubImage)
		{
			for (U32 i = 4; i < maxDownloadMip; i++)
			{
				glTexImage2D(GL_TEXTURE_2D,
					i - 4,
					destFormat,
					pDL->getWidth(i), pDL->getHeight(i),
					0,
					sourceFormat,
					byteFormat,
					pDL->getBits(i));
			}
		}
		else
		{
			for (U32 i = 4; i < maxDownloadMip; i++)
			{
				glTexSubImage2D(GL_TEXTURE_2D,
					i - 4,
					0, 0,
					pDL->getWidth(i), pDL->getHeight(i),
					sourceFormat,
					byteFormat,
					pDL->getBits(i));
			}
		}
	}
	else
	{
		if (to->smallTexGLName != 0)
			glDeleteTextures(1, &to->smallTexGLName);
		to->smallTexGLName = 0;
	}

	if(pDL != pBitmap)
		delete pDL;
}

void TextureManager::refresh(TextureObject *to, GBitmap* bmp)
{
	if (!(gDGLRender || sgResurrect)) return;

	U32 sourceFormat, destFormat, byteFormat;
	GBitmap* pBitmap = bmp;

	getSourceDestByteFormat(pBitmap, &sourceFormat, &destFormat, &byteFormat);

	if (!to->texGLName)
		glGenTextures(1,&to->texGLName);

	glBindTexture(GL_TEXTURE_2D, to->texGLName);
	GBitmap* pDL = createPaddedBitmap(pBitmap);

	U32 maxDownloadMip = pDL->getNumMipLevels();
	if (to->type == BitmapTexture ||
		to->type == BitmapKeepTexture ||
		//Luma:	Melv May's Memory Reduction
		to->type == ImageMap2DTexture ||
		to->type == ImageMap2DKeepTexture ||
		to->type == BitmapNoDownloadTexture)
	{
		maxDownloadMip = 1;
	}

#if !defined(TORQUE_OS_IPHONE)
	if (pDL->getFormat() == GBitmap::Palettized)
	{
		glColorTableEXT(GL_TEXTURE_2D,
			pDL->getPalette()->getPaletteType() == GPalette::RGB ? GL_RGB : GL_RGBA,
			256,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pDL->getPalette()->getColors());
	}
#endif

	if (sgDisableSubImage)
	{
		for (U32 i = 0; i < maxDownloadMip; i++)
		{
			glTexImage2D(GL_TEXTURE_2D,
				i,
				destFormat,
				pDL->getWidth(i), pDL->getHeight(i),
				0,
				sourceFormat,
				byteFormat,
				pDL->getBits(i));
		}
	}
	else
	{
		for (U32 i = 0; i < maxDownloadMip; i++)
		{
			glTexSubImage2D(GL_TEXTURE_2D,
				i,
				0, 0,
				pDL->getWidth(i), pDL->getHeight(i),
				sourceFormat,
				byteFormat,
				pDL->getBits(i));
		}
	}

	if ((to->type == InteriorTexture || to->type == MeshTexture) &&
		pDL->getNumMipLevels() > 4)
	{
		//
		if (!to->smallTexGLName)
			glGenTextures(1,&to->smallTexGLName);

		glBindTexture(GL_TEXTURE_2D, to->smallTexGLName);
		glBindTexture(GL_TEXTURE_2D, to->smallTexGLName);

#if !defined(TORQUE_OS_IPHONE)
		if (pDL->getFormat() == GBitmap::Palettized)
		{
			glColorTableEXT(GL_TEXTURE_2D,
				pDL->getPalette()->getPaletteType() == GPalette::RGB ? GL_RGB : GL_RGBA,
				256,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				pDL->getPalette()->getColors());
		}
#endif

		if (sgDisableSubImage)
		{
			for (U32 i = 4; i < maxDownloadMip; i++)
			{
				glTexImage2D(GL_TEXTURE_2D,
					i - 4,
					destFormat,
					pDL->getWidth(i), pDL->getHeight(i),
					0,
					sourceFormat,
					byteFormat,
					pDL->getBits(i));
			}
		}
		else
		{
			for (U32 i = 4; i < maxDownloadMip; i++)
			{
				glTexSubImage2D(GL_TEXTURE_2D,
					i - 4,
					0, 0,
					pDL->getWidth(i), pDL->getHeight(i),
					sourceFormat,
					byteFormat,
					pDL->getBits(i));
			}
		}
	}
	else
	{
		if (to->smallTexGLName != 0)
			glDeleteTextures(1, &to->smallTexGLName);
		to->smallTexGLName = 0;
	}

	if(pDL != pBitmap)
		delete pDL;
}



//--------------------------------------
//make 16 bit texture data out of this GBitmap
U16 *makeBitmap16Bit( GBitmap *pDL, U8 *in_source8, GBitmap::BitmapFormat alpha_info, GLint *GLformat, GLint *GLdata_type, U32 width, U32 height ) {
	//PUAP -Mat make 16 bit
	U16 *texture_data = new U16[width * height];
	U16 *dest = texture_data;
	U32 *source = (U32*)in_source8;
	//since the pointer is 4 bytes, multiply by the number of bytes per pixel over 4
	U32 spanInBytes = (width * height) * (pDL->bytesPerPixel / 4.0);
	U32 *source_end = source + spanInBytes;

	// UNUSED: JOSEPH THOMAS -> bool dither = false;

	switch (alpha_info) {
		case GBitmap::Alpha: //ALPHA_TRANSPARENT:
			while (source != source_end) {
				U32 color = *source++;
				*dest++ = ((color & 0xF8) << 8) | ((color & 0xF800) >> 5) | ((color & 0xF80000) >> 18) | (color >> 31);
			}
			*GLformat = GL_RGBA;
			*GLdata_type = GL_UNSIGNED_SHORT_5_5_5_1;
			break;
			case GBitmap::RGBA://ALPHA_BLEND
			while (source != source_end) {
				U32 color = *source++;
				*dest++ = ((color & 0xF0) << 8) | ((color & 0xF000) >> 4) | ((color & 0xF00000) >> 16) | ((color & 0xF0000000) >> 28);
			}
			*GLformat = GL_RGBA;
			*GLdata_type = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

		default://ALPHA_NONE
			U8 *source8 = (U8*)source;
			//32 bytes per address, snce we are casting to U8 we need 4 times as many
			U8 *end8 = source8 + (U32)(spanInBytes*4);
			while (source8 < end8) {
				U16 red = (U16)*source8;
				source8++;
				U16 green = (U16)*source8;
				source8++;
				U16 blue = (U16)*source8;
				source8++;
				//now color should be == RR GG BB 00
				*dest = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
				dest++;
			}
			*GLformat = GL_RGB;
			*GLdata_type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	}
	return texture_data;
}

//--------------------------------------
bool TextureManager::createGLName(GBitmap*          pBitmap,
								  bool              clampToEdge,
								  U32               firstMip,
								  TextureHandleType type,
								  TextureObject*    to)
{
	if (!(gDGLRender || sgResurrect))
		return 0;

	glGenTextures(1, &to->texGLName);
	glBindTexture(GL_TEXTURE_2D, to->texGLName);

	U32 sourceFormat, destFormat, byteFormat;

	getSourceDestByteFormat(pBitmap, &sourceFormat, &destFormat, &byteFormat);

	GBitmap *pDL = createPaddedBitmap(pBitmap);

	U32 maxDownloadMip = pDL->getNumMipLevels();
	if (type == BitmapTexture ||
		type == BitmapKeepTexture ||
		//Luma:	Melv May's Memory Reduction
		to->type == ImageMap2DTexture ||
		to->type == ImageMap2DKeepTexture ||
		type == BitmapNoDownloadTexture)
	{
		maxDownloadMip = firstMip + 1;
	}

#if !defined(TORQUE_OS_IPHONE)
	if (pDL->getFormat() == GBitmap::Palettized)
	{
		glColorTableEXT(GL_TEXTURE_2D,
			pDL->getPalette()->getPaletteType() == GPalette::RGB ? GL_RGB : GL_RGBA,
			256,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pDL->getPalette()->getColors());
	}
#else
	bool isCompressed = (pBitmap->getFormat() >= GBitmap::PVR2) && (pBitmap->getFormat() <= GBitmap::PVR4A);
#endif

	for (U32 i = firstMip; i < maxDownloadMip; i++)
	{
		// %PUAP%
		if (glGetError() != GL_NO_ERROR) {
			Con::printf("GLError before creating texImage\n");
		}
		// --> %PUAP%
#if defined(TORQUE_OS_IPHONE)
		if (isCompressed) {
			switch (pBitmap->getFormat()) {
				case GBitmap::PVR2:
				case GBitmap::PVR2A:
					glCompressedTexImage2D(GL_TEXTURE_2D, i - firstMip, byteFormat,
						pBitmap->getWidth(), pBitmap->getHeight(), 0, (getMax((int)pBitmap->getWidth(),16) * getMax((int)pBitmap->getHeight(), 8) * 2 + 7) / 8, pBitmap->getBits(0) );
					break;
				case GBitmap::PVR4:
				case GBitmap::PVR4A:
					glCompressedTexImage2D(GL_TEXTURE_2D, i - firstMip, byteFormat,
						pBitmap->getWidth(), pBitmap->getHeight(), 0, (getMax((int)pBitmap->getWidth(),8) * getMax((int)pBitmap->getHeight(), 8) * 4 + 7) / 8, pBitmap->getBits(0) );
					break;
				default:
				// already tested for range of values, so default is just to keep the compiler happy!
				break;
			}
		} else 
#endif
			//-Mat why do we keep the GBitmap?
		if( pDL->mForce16Bit ) {
			GLint GLformat;
			GLint GLdata_type;
			// UNUSED: JOSEPH THOMAS -> U32 spanInBytes = (pDL->getWidth( i ) * pDL->getHeight( i )) * (pDL->bytesPerPixel / 4.0);

			U16 *texture_data = makeBitmap16Bit( pDL, pDL->getWritableBits(i), pBitmap->getFormat(), 
												 &GLformat, &GLdata_type,
												 pDL->getWidth( i ), pDL->getHeight( i ) );

			glTexImage2D(GL_TEXTURE_2D, 
							i - firstMip,
							GLformat,
							pDL->getWidth( i ), pDL->getHeight( i ), 
							0,
							GLformat, 
							GLdata_type,
							texture_data
						);

			//copy new texture_data into pBits
			delete [] texture_data;
		} else {
			glTexImage2D(GL_TEXTURE_2D,
				i - firstMip,
				destFormat,
				pDL->getWidth(i), pDL->getHeight(i),
				0,
				sourceFormat,
				byteFormat,
				pDL->getBits(i));
		}
		// %PUAP%
		if (glGetError() != GL_NO_ERROR) {
			Con::printf("Failed to create %s, w/h = %d/%d!\n", to->texFileName,pDL->getWidth(i), pDL->getHeight(i));
		}
		// --> %PUAP%
	}

	if(to->filterNearest)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(pBitmap->getNumMipLevels() != 1 &&
			type != BitmapTexture &&
			type != BitmapKeepTexture &&
			//Luma:	Melv May's Memory Reduction
			type != ImageMap2DTexture &&
			type != ImageMap2DKeepTexture &&
			type != BitmapNoDownloadTexture) 
		{            
			if (sgTextureTrilinear || type == BumpTexture || type == InvertedBumpTexture)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			if (dglDoesSupportTexAnisotropy()) 
			{
				F32 val = 1.0f + sgTextureAnisotropy * dglGetMaxAnisotropy();
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, val);
			}         
		} 
		else 
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
	}

	U32 clamp = GL_REPEAT;
	if (clampToEdge)
		clamp = dglDoesSupportEdgeClamp() ? GL_CLAMP_TO_EDGE : GL_CLAMP;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);

	if ((type == InteriorTexture || type == MeshTexture) &&
		(pDL->getNumMipLevels() - firstMip) > 4)
	{
		glGenTextures(1, &to->smallTexGLName);
		glBindTexture(GL_TEXTURE_2D, to->smallTexGLName);

#if !defined(TORQUE_OS_IPHONE)
		if (pDL->getFormat() == GBitmap::Palettized)
		{
			glColorTableEXT(GL_TEXTURE_2D,
				pDL->getPalette()->getPaletteType() == GPalette::RGB ? GL_RGB : GL_RGBA,
				256,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				pDL->getPalette()->getColors());
		}
#endif

		for (U32 i = firstMip + 4; i < maxDownloadMip; i++)
		{
			// %PUAP%
			if (glGetError() != GL_NO_ERROR) {
				Con::printf("GLError before creating texImage\n");
			}
			// --> %PUAP%
#if defined(TORQUE_OS_IPHONE)
			if (isCompressed) {
				switch (pBitmap->getFormat()) {
					case GBitmap::PVR2:
					case GBitmap::PVR2A:
						glCompressedTexImage2D(GL_TEXTURE_2D,  i - (firstMip + 4), byteFormat,
							pBitmap->getWidth(), pBitmap->getHeight(), 0, (getMax((int)pBitmap->getWidth(),16) * getMax((int)pBitmap->getHeight(), 8) * 2 + 7) / 8, pBitmap->getBits(0) );
						break;
					case GBitmap::PVR4:
					case GBitmap::PVR4A:
						glCompressedTexImage2D(GL_TEXTURE_2D,  i - (firstMip + 4), byteFormat,
							pBitmap->getWidth(), pBitmap->getHeight(), 0, (getMax((int)pBitmap->getWidth(),8) * getMax((int)pBitmap->getHeight(), 8) * 4 + 7) / 8, pBitmap->getBits(0) );
						break;
					default:
						// already tested for range of values, so default is just to keep the compiler happy!
						break;
				}
			}
			else 
#endif
				if( pDL->mForce16Bit ) {
					GLint GLformat;
					GLint GLdata_type;
					// UNUSED: JOSEPH THOMAS -> U32 spanInBytes = (pDL->getWidth( i ) * pDL->getHeight( i )) * (pDL->bytesPerPixel / 4.0);

					U16 *texture_data = makeBitmap16Bit( pDL, pDL->getWritableBits(i), pBitmap->getFormat(), 
														 &GLformat, &GLdata_type,
														 pDL->getWidth( i ), pDL->getHeight( i ) );

					glTexImage2D(GL_TEXTURE_2D, 
									i - (firstMip + 4),
									GLformat,
									pDL->getWidth( i ), pDL->getHeight( i ), 
									0,
									GLformat, 
									GLdata_type,
									texture_data
								);

					//copy new texture_data into pBits
					delete [] texture_data;
				} else {
					glTexImage2D(GL_TEXTURE_2D,
						i - (firstMip + 4),
						destFormat,
						pDL->getWidth(i), pDL->getHeight(i),
						0,
						sourceFormat,
						byteFormat,
						pDL->getBits(i));
				}
				// %PUAP%
				if (glGetError() != GL_NO_ERROR) {
					Con::printf("Failed to create %s, w/h = %d/%d!\n", to->texFileName,pDL->getWidth(i), pDL->getHeight(i));
				}
				// --> %PUAP%
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (sgTextureTrilinear)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		if (dglDoesSupportTexAnisotropy()) 
		{
			F32 val = 1.0f + sgTextureAnisotropy * dglGetMaxAnisotropy();
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, val);
		}

		U32 clamp = GL_REPEAT;
		if (clampToEdge)
			clamp = dglDoesSupportEdgeClamp() ? GL_CLAMP_TO_EDGE : GL_CLAMP;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
	}

	if(pDL != pBitmap)
		delete pDL;

	return to->texGLName != 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TextureObject* TextureManager::registerTexture(const char* textureName, const GBitmap* data, bool clampToEdge)
{
	//WARNING: since there's no texture type here, there's no way to tell
	//if it is an inverted bump texture, which would cause it NOT to invert!
	//but, this appears to only be used for RegisteredTextures
	//so it shouldn't make a difference.

	// if there is no textureName, it isn't inserted into the hash
	// table... merely tracked by the texture manager

	TextureObject *ret = NULL;

	if(textureName)
	{
		textureName = StringTable->insert(textureName);
		ret         = TextureDictionary::find(textureName, RegisteredTexture, clampToEdge);
	}

	if(ret)
	{
		// Crucial conditionals for the flush case...
		if (ret->bitmap != data)
			delete ret->bitmap;
		if (ret->texGLName)
			glDeleteTextures(1, (const GLuint*)&ret->texGLName);
		if (ret->smallTexGLName)
			glDeleteTextures(1, (const GLuint*)&ret->smallTexGLName);

		ret->texGLName      = 0;
		ret->smallTexGLName = 0;

#ifdef TORQUE_GATHER_METRICS
		AssertFatal(ret->textureSpace <= smTextureSpaceLoaded, "Error, that shouldn't happen!");
		smTextureSpaceLoaded -= ret->textureSpace;
		ret->textureSpace     = 0;
#endif
	}
	else
	{
		ret = new TextureObject;
		ret->texFileName    = textureName;
		ret->texGLName      = 0;
		ret->smallTexGLName = 0;
		ret->refCount       = 0;
		ret->type = RegisteredTexture;
		ret->holding        = false;
		ret->filterNearest  = false;

		TextureDictionary::insert(ret);
	}

	ret->bitmap           = (GBitmap *) data;
	ret->bitmapWidth      = data->getWidth();
	ret->bitmapHeight     = data->getHeight();
	ret->texWidth         = getNextPow2(ret->bitmapWidth);
	ret->texHeight        = getNextPow2(ret->bitmapHeight);
	ret->downloadedWidth  = ret->texWidth;
	ret->downloadedHeight = ret->texHeight;
	ret->clamp            = clampToEdge;

#ifdef TORQUE_GATHER_METRICS
	ret->textureSpace     = ret->downloadedWidth * ret->downloadedHeight;
	smTextureSpaceLoaded += ret->textureSpace;
#endif

	createGLName(ret->bitmap, clampToEdge, 0, ret->type, ret);

	return ret;
}

//--------------------------------------
TextureObject* TextureManager::registerTexture(const char* textureName, GBitmap* bmp, TextureHandleType type, bool clampToEdge)
{
	//Get this done and out of the way first - if it's an inverted texture,
	//then invert it!  Do it in this function because resurrect() calls this directly
	if( type == InvertedBumpTexture )
	{
		// Get pixel address (0,0) of bitmap data for mip level 0.
		U8* pImageBits = bmp->getAddress(0,0,0);

		// Fetch total pixel count for bitmap.
		U32 PixelCount = bmp->getWidth(0) * bmp->getHeight(0) * bmp->bytesPerPixel;

		// Invert only the RGB components of the bitmap (leave alpha alone).
		for (U32 index = 0; index < PixelCount; ++index)
			pImageBits[index] ^= 0x00ffffff;
	}

	// In order to get the right blending, all of the pixels have to be at half intensity.
	// Would normally do this using glPixelTransferf(), but that function is not
	// supported in the D3D wrapper.
	// BUG: detect the case where we're using the D3D wrapper, and use this slower code only in that case.
	if (type == BumpTexture || type == InvertedBumpTexture)
	{
		U8* pImageBits = bmp->getAddress(0,0,0);

		U32 PixelCount = bmp->getWidth(0) * bmp->getHeight(0) * bmp->bytesPerPixel;

		for (U32 index = 0; index < PixelCount; index++)
			pImageBits[index] /= 2;
	}

	TextureObject *ret = NULL;
	if(textureName)
	{
		textureName = StringTable->insert(textureName);
		ret = TextureDictionary::find(textureName, type, clampToEdge);
	}

	if(ret)
	{
		// Crucial conditionals for the flush case...
		if (ret->bitmap != bmp)
			delete ret->bitmap;
		if (ret->texGLName)
			glDeleteTextures(1, (const GLuint*)&ret->texGLName);
		if (ret->smallTexGLName)
			glDeleteTextures(1, (const GLuint*)&ret->smallTexGLName);
		ret->texGLName      = 0;
		ret->smallTexGLName = 0;

#ifdef TORQUE_GATHER_METRICS
		AssertFatal(ret->textureSpace <= smTextureSpaceLoaded, "Error, that shouldn't happen!");
		smTextureSpaceLoaded -= ret->textureSpace;
		ret->textureSpace     = 0;
#endif
	}
	else
	{
		ret = new TextureObject;
		ret->texFileName    = textureName;
		ret->texGLName      = 0;
		ret->smallTexGLName = 0;
		ret->refCount       = 0;
		ret->type           = type;
		ret->filterNearest= false;

		TextureDictionary::insert(ret);
	}

	ret->bitmap       = bmp;
	ret->bitmapWidth  = bmp->getWidth();
	ret->bitmapHeight = bmp->getHeight();
	ret->texWidth     = getNextPow2(ret->bitmapWidth);
	ret->texHeight    = getNextPow2(ret->bitmapHeight);
	ret->clamp        = clampToEdge;
	ret->holding      = (type == MeshTexture) && ENABLE_HOLDING;

	if ((ret->type == DetailTexture || ret->type == BumpTexture || ret->type == InvertedBumpTexture) &&
#if defined(TORQUE_OS_IPHONE)
		((bmp->getFormat() != GBitmap::PVR2) && (bmp->getFormat() != GBitmap::PVR2A) && (bmp->getFormat() != GBitmap::PVR4) && (bmp->getFormat() != GBitmap::PVR4A)) &&
#endif
		bmp->getFormat() != GBitmap::Palettized)
		bmp->extrudeMipLevels();
	else if (ret->type != TerrainTexture &&
		ret->type != BitmapTexture &&
		ret->type != BitmapKeepTexture &&
		//Luma:	Melv May's Memory Reduction
		ret->type != ImageMap2DTexture &&
		ret->type != ImageMap2DKeepTexture &&
		ret->type != BitmapNoDownloadTexture &&
#if defined(TORQUE_OS_IPHONE)
		((bmp->getFormat() != GBitmap::PVR2) && (bmp->getFormat() != GBitmap::PVR2A) && (bmp->getFormat() != GBitmap::PVR4) && (bmp->getFormat() != GBitmap::PVR4A)) &&
#endif
		bmp->getFormat() != GBitmap::Palettized)
		bmp->extrudeMipLevels(ret->type==ZeroBorderTexture);

	if(!ret->texGLName) 
	{
		U32 firstMip = 0;
		if (ret->bitmap->getNumMipLevels() > 1 &&
			type != DetailTexture &&
			type != BumpTexture &&
			type != InvertedBumpTexture &&
			type != TerrainTexture &&
			type != BitmapTexture &&
			type != BitmapKeepTexture &&
			//Luma:	Melv May's Memory Reduction
			type != ImageMap2DTexture &&
			type != ImageMap2DKeepTexture &&
			type != BitmapNoDownloadTexture)
		{
			if (type == SkyTexture)
			{
				firstMip = getMin(sgSkyTextureDetailLevel, ret->bitmap->getNumMipLevels() - 1);
			}
			else if (type == InteriorTexture)
			{
				firstMip = getMin(sgInteriorTextureDetailLevel, ret->bitmap->getNumMipLevels() - 1);
			}
			else
			{
				firstMip = getMin(sgTextureDetailLevel, ret->bitmap->getNumMipLevels() - 1);
			}
		}

		ret->downloadedWidth  = ret->bitmapWidth  >> firstMip;
		ret->downloadedHeight = ret->bitmapHeight >> firstMip;
		if (ret->downloadedWidth  == 0) ret->downloadedWidth  = 1;
		if (ret->downloadedHeight == 0) ret->downloadedHeight = 1;

#ifdef TORQUE_GATHER_METRICS
		ret->textureSpace = 0;
		for (U32 i = firstMip; i < ret->bitmap->getNumMipLevels(); i++)
			ret->textureSpace += ret->bitmap->getWidth(i) * ret->bitmap->getHeight(i);
		smTextureSpaceLoaded += ret->textureSpace;
#endif

		if(ret->type != BitmapNoDownloadTexture)
			createGLName(bmp, clampToEdge, firstMip, ret->type, ret);
	}

	if (ret->type == BitmapKeepTexture || ret->type == BitmapNoDownloadTexture || ret->type == ImageMap2DKeepTexture) 
	{
		// do nothing
	}
	else if (ret->type == TerrainTexture) 
	{
		// Don't delete the bitmap
		ret->bitmap = NULL;
	}
	else 
	{
		delete ret->bitmap;
		ret->bitmap = NULL;
	}

	return ret;
}


//--------------------------------------
GBitmap *TextureManager::loadBitmapInstance(const char *textureName, bool recurse /* = true */, bool nocompression /* = false */)
{
	char fileNameBuffer[512];
#ifdef TORQUE_TOOLS
	Platform::makeFullPathName( textureName, fileNameBuffer, 512 );
#else
	dStrcpy(fileNameBuffer, textureName);
#endif
	GBitmap *bmp = NULL;

	// Loop through the supported extensions to find the file.
	U32 len = dStrlen(fileNameBuffer);
	for (U32 i = 0; i < EXT_ARRAY_SIZE && bmp == NULL; i++)
	{
#if defined(TORQUE_OS_IPHONE)
		// check to see if requested no-compression...
		if (nocompression && (dStrncmp( extArray[i], ".pvr", 4 ) == 0)) {
			continue;
		}
#endif
		if (sgForcePalettedTexture == true && dglDoesSupportPalettedTexture())
			dStrcpy(fileNameBuffer + len, extArray_8[i]);
		else
			dStrcpy(fileNameBuffer + len, extArray[i]);

		bmp = (GBitmap*)ResourceManager->loadInstance(fileNameBuffer);

		// CAF: if a jpg, and RGB, look for file.alpha.jpg as alpha channel
		if ( (!sgForcePalettedTexture || !dglDoesSupportPalettedTexture()) && !dStricmp(extArray[i],".jpg") && bmp && bmp->getFormat()==GBitmap::RGB)
		{
			dStrcpy(fileNameBuffer + len, ".alpha.jpg");
			GBitmap * bmpAlpha = (GBitmap*)ResourceManager->loadInstance(fileNameBuffer);
			S32 w = bmp->getWidth();
			S32 h = bmp->getHeight();
			if (bmpAlpha && bmpAlpha->getWidth() == w && bmpAlpha->getHeight() == h && bmpAlpha->bytesPerPixel==1)
			{
				GBitmap * bmp2 = new GBitmap(w,h,false,GBitmap::RGBA);
				U8 * rgbBits = bmp->getWritableBits();
				U8 * alphaBits = bmpAlpha->getWritableBits();
				U8 * bmpBits = bmp2->getWritableBits();
				for (S32 wi=0; wi<w; wi++)
				{
					for (S32 hi=0; hi<h; hi++)
					{
						bmpBits[wi*4 + hi*4*w + 0] = rgbBits[wi*3 + hi*3*w + 0];
						bmpBits[wi*4 + hi*4*w + 1] = rgbBits[wi*3 + hi*3*w + 1];
						bmpBits[wi*4 + hi*4*w + 2] = rgbBits[wi*3 + hi*3*w + 2];
						bmpBits[wi*4 + hi*4*w + 3] = alphaBits[wi + hi*w];
					}
				}
				delete bmpAlpha;
				delete bmp;
				bmp = bmp2;
			}
		}
	}

	// If unable to load texture in current directory
	// look in the parent directory.  But never look in the root.
	fileNameBuffer[len] = 0;
	if (!bmp && recurse)
	{
		char *name = dStrrchr(fileNameBuffer, '/');
		if (name)
		{
			*name++ = 0;
			char *parent = dStrrchr(fileNameBuffer, '/');
			if (parent)
			{
				parent[1] = 0;
				dStrcat(fileNameBuffer, name);
				return loadBitmapInstance(fileNameBuffer);
			}
		}
	}
	return bmp;
}

//--------------------------------------

TextureObject *TextureManager::loadTexture(const char* textureName, TextureHandleType type, bool clampToEdge, bool checkOnly /* = false */)
{
	// Catch if we're trying to load a blank texture...
	if(!textureName || dStrlen(textureName) == 0)
		return NULL;

	textureName = StringTable->insert(textureName);

	TextureObject *ret = TextureDictionary::find(textureName, type, clampToEdge);

	GBitmap *bmp = NULL;

	if(!ret)
	{
		// Ok, no hit - is it in the current dir? If so then let's grab it
		// and use it.
		bmp = loadBitmapInstance(textureName, false);

		if(bmp)
			return registerTexture(textureName, bmp, type, clampToEdge);

		// Otherwise...
		// We want to check for previously loaded textures with the same
		// name in higher directories. loadBitmapInstance does this too
		// but we want to reuse the actual texture object, if possible,
		// not just load things many times into video memory!
		char fileNameBuffer[512];
		dStrcpy(fileNameBuffer, textureName);

		// If unable to load texture in current directory
		// look in the parent directory.  But never look in the root.
		char *name = dStrrchr(fileNameBuffer, '/');
		if (name)
		{
			*name++ = 0;
			char *parent = dStrrchr(fileNameBuffer, '/');
			if (parent)
			{
				parent[1] = 0;
				dStrcat(fileNameBuffer, name);
				ret = loadTexture(fileNameBuffer, type, clampToEdge, true);
			}
		}
	}

	if(ret)
		return ret;

	// If we're just checking, fail out so we eventually get around to
	// loading a real bitmap.
	if(checkOnly)
		return NULL;

	// Ok, no success so let's try actually loading a texture.
	bmp = loadBitmapInstance(textureName);

	if(!bmp)
	{
		Con::warnf("Could not locate texture: %s", textureName);
		return NULL;
	}

	return registerTexture(textureName, bmp, type, clampToEdge);
}


//--------------------------------------

void TextureHandle::setFilterNearest()
{
	if (object)
	{
		object->filterNearest = true;

		if(object->texGLName != 0)
		{
			glBindTexture(GL_TEXTURE_2D, object->texGLName);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}
}

void TextureHandle::setClamp(const bool c)
{
	if (object)
	{
		object->clamp = c;
		if (object->texGLName != 0)
		{
			glBindTexture(GL_TEXTURE_2D, object->texGLName);
			GLenum clamp;
			if (c)
				clamp = dglDoesSupportEdgeClamp() ? GL_CLAMP_TO_EDGE : GL_CLAMP;
			else
				clamp = GL_REPEAT;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
		}

		if (object->smallTexGLName != 0)
		{
			glBindTexture(GL_TEXTURE_2D, object->smallTexGLName);

			GLenum clamp;
			if (c)
				clamp = dglDoesSupportEdgeClamp() ? GL_CLAMP_TO_EDGE : GL_CLAMP;
			else
				clamp = GL_REPEAT;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
		}
	}
}

#if defined(TORQUE_DEBUG)
// The non-debug version of this is inlined in the header file

#define MAX_THOBJECT 1024
static TextureObject *thobject[MAX_THOBJECT];
static int htracked = 0;

void TextureHandle::lock()
{
	AssertFatal(TextureManager::isActive(), "TextureHandle::lock - TextureManager must be active to lock a texture.");

	if(object)
	{
		if (object->refCount>10000 || object->refCount<0) // some reasonable numbers.
		{
			for (int i=0; i<htracked; i++)
			{
				if (thobject[i]==object)
					return;
			}
			Con::warnf(ConsoleLogEntry::Assert, "Texture refcount out of range: tex<%p> cnt<%d>", object, object->refCount);
			if (htracked<MAX_THOBJECT)
			{
				thobject[htracked] = object;
				htracked++;
			}
		}
		else
		{
			object->refCount++;
		}
	}
}

void TextureHandle::unlock()
{
	// Do nothing if the manager isn't active.
	if(!TextureManager::isActive())
		return;

	if(object)
	{
		object->refCount--;
		if (object->holding == false)
		{
			if(!object->refCount)
				TextureManager::freeTexture(object);
		}
		else
		{
			// dc - try outputting this error to console instead of assert box, and keep safe to continue.
			if (object->refCount<0)
			{
				Con::warnf(ConsoleLogEntry::Assert, "Texture holding out of balance: %d (0x%x)",
					object->refCount, object->refCount);
				object->refCount++; // put back for safety, in case we did something we shouldn't have.
			}
		}

		object = NULL;
	}
}
#endif

#ifdef TORQUE_GATHER_METRICS
F32 TextureManager::getResidentFraction()
{
	U32 resident = 0;
	U32 total    = 0;

	Vector<GLuint> names;

	TextureObject* pProbe = TextureDictionary::smTOList;
	while (pProbe != NULL) 
	{
		if (pProbe->texGLName != 0) 
		{
			total++;
			names.push_back(pProbe->texGLName);
		}

		pProbe = pProbe->next;
	}

	if (total == 0)
		return 1.0f;

	Vector<GLboolean> isResident;
	isResident.setSize(names.size());

	glAreTexturesResident(names.size(), names.address(), isResident.address());
	for (U32 i = 0; i < names.size(); i++)
		if (isResident[i] == GL_TRUE)
			resident++;

	return (F32(resident) / F32(total));
}
#endif

//-------------------------------------------------------------------------

ChunkedTextureObject *gChunkedTextureList = NULL;

ChunkedTextureObject* ChunkedTextureManager::loadTexture(const char *textureName)
{
	if(!textureName)
		return NULL;
	StringTableEntry tName = StringTable->insert(textureName);

	for(ChunkedTextureObject *walk = gChunkedTextureList; walk; walk = walk->next)
		if(walk->texFileName == tName)
			return walk;
	GBitmap *bmp = TextureManager::loadBitmapInstance(textureName);
	if(!bmp)
		return NULL;
	return registerTexture(textureName, bmp, false);
}

ChunkedTextureObject* ChunkedTextureManager::registerTexture(const char *textureName, GBitmap *data, bool keep)
{
	ChunkedTextureObject *ret = NULL;
	StringTableEntry tName = NULL;

	if(textureName)
	{
		tName = StringTable->insert(textureName);
		for(ChunkedTextureObject *walk = gChunkedTextureList; walk; walk = walk->next)
		{
			if(walk->texFileName == tName)
			{
				ret = walk;
				break;
			}
		}
	}

	if(ret && ret->bitmap)
	{
		delete ret->bitmap;
		ret->bitmap = data;
	}
	else
	{
		ret = new ChunkedTextureObject;
		ret->bitmap = data;
		ret->texFileName = tName;
		ret->next = gChunkedTextureList;
		gChunkedTextureList = ret;
#if defined(TORQUE_OS_IPHONE)
		if ((data->getFormat() >= GBitmap::PVR2) && (data->getFormat() <= GBitmap::PVR4A)) {
			// if compressed, then can't use it as 'chunked', treat it is one whole image
			ret->texWidthCount = 1;
			ret->texHeightCount = 1;
		} else 
#endif
		{
			ret->texWidthCount = (data->getWidth() + 255) >> 8;
			ret->texHeightCount = (data->getHeight() + 255) >> 8;
		}
		ret->width = data->getWidth();
		ret->height = data->getHeight();
		ret->textureHandles = NULL;
		ret->refCount = 0;
	}

	refresh(ret);

	if(!keep)
	{
		delete ret->bitmap;
		ret->bitmap = NULL;
	}

	return ret;
}

void ChunkedTextureManager::freeTexture(ChunkedTextureObject *to)
{
	// remove it from the linked list

	for(ChunkedTextureObject **walk = &gChunkedTextureList; *walk; walk = &((*walk)->next))
	{
		if(*walk == to)
		{
			*walk = to->next;
			delete[] to->textureHandles;
			delete to->bitmap;
			return;
		}
	}
}

void ChunkedTextureManager::refresh(ChunkedTextureObject *to)
{
	if(!to->bitmap)
		return;

	if(to->textureHandles)
	{
		delete[] to->textureHandles;
		to->textureHandles = NULL;
	}

#if defined(TORQUE_OS_IPHONE)
	// if the texture is compressed, don't try to subdivide it
	if ((to->bitmap->getFormat() >= GBitmap::PVR2) && (to->bitmap->getFormat() <= GBitmap::PVR4A)) {
		to->textureHandles = new TextureHandle[1];
		// clone the bitmap
		GBitmap *tempBitmap = new GBitmap(*(to->bitmap));
		to->textureHandles[0] = TextureHandle(NULL, tempBitmap, BitmapTexture, true);
		return;
	} 
#endif
	
	to->textureHandles = new TextureHandle[to->texWidthCount * to->texHeightCount];

	for(U32 j = 0; j < to->texHeightCount; j++)
	{
		U32 y = j * 256;
		U32 height = getMin(to->bitmap->getHeight() - y, U32(256));

		for(U32 i = 0; i < to->texWidthCount; i++)
		{
			U32 index = j * to->texWidthCount + i;
			U32 x = i * 256;
			U32 width = getMin(to->bitmap->getWidth() - x, U32(256));

			GBitmap *tempBitmap = new GBitmap(width, height, false, to->bitmap->getFormat());
			for(U32 lp = 0; lp < height; lp++)
			{
				const U8 *src = to->bitmap->getAddress(x, y + lp);
				U8 *dest = tempBitmap->getAddress(0, lp);
				dMemcpy(dest, src, width * to->bitmap->bytesPerPixel);
			}
			to->textureHandles[index] = TextureHandle(NULL, tempBitmap, BitmapTexture, true);

		}
	}
}

void ChunkedTextureManager::makeZombie()
{
	for(ChunkedTextureObject *walk = gChunkedTextureList; walk; walk = walk->next)
	{
		delete[] walk->textureHandles;
		walk->textureHandles = NULL;
	}
}

void ChunkedTextureManager::resurrect()
{
	for(ChunkedTextureObject *walk = gChunkedTextureList; walk; walk = walk->next)
	{
		GBitmap *bmp = walk->bitmap;
		if(!bmp)
			walk->bitmap = TextureManager::loadBitmapInstance(walk->texFileName);

		refresh(walk);

		if(!bmp)
		{
			delete walk->bitmap;
			walk->bitmap = NULL;
		}
	}
}

TextureHandle ChunkedTextureHandle::getSubTexture(U32 x, U32 y)
{
	if(!object || !object->textureHandles)
		return NULL;
	return object->textureHandles[x + y * object->texWidthCount];
}


void ChunkedTextureHandle::lock()
{
	if(object)
		object->refCount++;
}

void ChunkedTextureHandle::unlock()
{
	if(object)
	{
		object->refCount--;
		if(object->refCount == 0)
			ChunkedTextureManager::freeTexture(object);
	}
}
