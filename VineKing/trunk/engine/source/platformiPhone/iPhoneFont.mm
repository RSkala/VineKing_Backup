//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


#include "platformiPhone/iPhoneFont.h"
#include "platformiPhone/platformiPhone.h"
#include "dgl/gFont.h"
#include "dgl/gBitmap.h"
#include "Math/mRect.h"
#include "console/console.h"
#include "core/unicode.h"
#include "core/frameAllocator.h"





//------------------------------------------------------------------------------
// New Unicode capable font class.
PlatformFont *createPlatformFont(const char *name, U32 size, U32 charset /* = TGE_ANSI_CHARSET */)
{
    PlatformFont *retFont = new iPhoneFont;

    if(retFont->create(name, size, charset))
        return retFont;

    delete retFont;
    return NULL;
}

//------------------------------------------------------------------------------
iPhoneFont::iPhoneFont()
{
   //mStyle   = NULL;
   //mLayout  = NULL;
   mColorSpace = NULL;
}

iPhoneFont::~iPhoneFont()
{
   // apple docs say we should dispose the layout first.
   //ATSUDisposeTextLayout(mLayout);
   //ATSUDisposeStyle(mStyle);
   CGColorSpaceRelease(mColorSpace);
}


//------------------------------------------------------------------------------
bool iPhoneFont::create( const char *name, U32 size, U32 charset)
{
	//make a new UIFont
	UIFont *font = NULL;
	CGFloat num = (CGFloat) size;

	NSString *string = [[NSString alloc] initWithUTF8String:name];
	
	font = [UIFont fontWithName: string size: num];
	
	mColorSpace = CGColorSpaceCreateDeviceGray();

	mUIFont = font;
	
	CGFloat width = size * strlen( name );
	CGFloat height = size;
	mBaseline = width;
	mHeight = height;
	
	// adjust the size. win dpi = 96, mac dpi = 72. 72/96 = .75
	// Interestingly enough, 0.75 is not what makes things the right size.
	U32 scaledSize = size - 2 - (int)((float)size * 0.1);
	mSize = scaledSize;
	
	
	// and finally cache the font's name. We use this to cheat some antialiasing options below.
	mName = StringTable->insert(name);
	
	//end of font creation
	[string release];	
	return true;
}

//------------------------------------------------------------------------------
bool iPhoneFont::isValidChar(const UTF8 *str) const
{
   // since only low order characters are invalid, and since those characters
   // are single codeunits in UTF8, we can safely cast here.
   return isValidChar((UTF16)*str);  
}

bool iPhoneFont::isValidChar( const UTF16 ch) const
{
   // We cut out the ASCII control chars here. Only printable characters are valid.
   // 0x20 == 32 == space
   if( ch < 0x20 )
      return false;

   return true;
}

PlatformFont::CharInfo& iPhoneFont::getCharInfo(const UTF8 *str) const
{
   return getCharInfo(oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)));
}

PlatformFont::CharInfo& iPhoneFont::getCharInfo(const UTF16 ch) const
{
	Con::warnf("iPhoneFont::getCharInfo() is being used to get char info, values will almost certainly be wrong(generate better UFTs)");	
	// Declare and clear out the CharInfo that will be returned.
	static PlatformFont::CharInfo c;
	dMemset(&c, 0, sizeof(c));
	
	// prep values for GFont::addBitmap()
	c.bitmapIndex = 0;
	c.xOffset = 0;
	c.yOffset = 0;

	return c;
}

void PlatformFont::enumeratePlatformFonts( Vector<StringTableEntry>& fonts, UTF16* fontFamily )
{}

GOldFont* createFont(const char *name, dsize_t size, U32 charset) 
{
	return NULL;
}
