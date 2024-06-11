//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "T2D/t2dTextObject.h"
#include "dgl/dgl.h"
#include "core/frameAllocator.h"
#include "core/unicode.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "util/safeDelete.h"

//-----------------------------------------------------------------------------
/// Table lookup for the text alignment.
//-----------------------------------------------------------------------------
static EnumTable::Enums textAlignLookup[] =
                {
                { t2dTextObject::LEFT,    "Left" },
                { t2dTextObject::CENTER,  "Center" },
                { t2dTextObject::RIGHT,   "Right" },
                { t2dTextObject::JUSTIFY, "Justify" },
                };

static EnumTable textAlignTable(sizeof(textAlignLookup) /  sizeof(EnumTable::Enums), &textAlignLookup[0]);

IMPLEMENT_CONOBJECT( t2dTextObject );

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------
t2dTextObject::t2dTextObject() : mConsoleBuffer( NULL ),
                                 mFontName( NULL ),
                                 mLineHeight( 12 ),
                                 mAspectRatio( 1.0f ),
                                 mWordWrap( false ),
                                 mClipText( true ),
                                 mLineSpacing( 0.0f ),
                                 mCharacterSpacing( 0.0f ),
                                 mAutoSize( true ),
                                 mRenderCursor( false ),
                                 mCharacterBounds( NULL ),
                                 mCharactersDirty( true ),
                                 mTextAlignment( LEFT ),
                                 mHighlightBlock( -1 ),
                                 mCursorPos( 0 ),
                                 mCursorColor( 0.0f, 0.0f, 0.0f, 1.0f ),
                                 mCursorFadeDelay( 0.0f ),
                                 mShowCursorAtEndOfLine( true ),
								 mAlpha(1.0f),
								 mFilter(false),
								 mIntegerPrecision(false),
								 mNoUnicode(false),
                                 mEditing( false )
{
   VECTOR_SET_ASSOCIATION( mFonts );
   setText( "" );
}

t2dTextObject::~t2dTextObject()
{
	//Luma:	make sure to kill the fonts on object destruction
	while(mFonts.size())
	{
		mFonts.last().font.purge();
		mFonts.decrement();
	}

   SAFE_DELETE_ARRAY( mCharacterBounds );
}

//-----------------------------------------------------------------------------
/// SimObject Overrides
//-----------------------------------------------------------------------------
void t2dTextObject::initPersistFields()
{
   Parent::initPersistFields();

   addProtectedField( "text", TypeString, Offset( mConsoleBuffer, t2dTextObject ), setText, getText,
      "The text that is displayed by the object." );
   addProtectedField( "font", TypeString, Offset( mFontName, t2dTextObject ), setFont, getFont,
      "The name of the font to render the text with." );
   addProtectedField( "wordWrap", TypeBool, Offset( mWordWrap, t2dTextObject ), setWordWrap, defaultProtectedGetFn,
      "Whether or not text should automatically wrap to the next line." );
   addProtectedField( "hideOverflow", TypeBool, Offset( mClipText, t2dTextObject ), defaultProtectedSetFn, defaultProtectedGetFn,
      "Whether or not text outside the object bounds should be hidden." );
   addProtectedField( "textAlign", TypeEnum, Offset( mTextAlignment, t2dTextObject ), setTextAlign, defaultProtectedGetFn, 1, &textAlignTable,
      "The alignment of the text within the object bounds.<br>"
      "Possible Values<br>"
      " -Left: Each line begins at the left bounds of the object.<br>"
      " -Right: Each line ends at the right bounds of the object.<br>"
      " -Center: Each line of text is centered within the object bounds.<br>"
      " -Justify: Text on complete lines is spaced out to fill the object bounds.<br>" );
   addProtectedField( "lineHeight", TypeF32, Offset( mLineHeight, t2dTextObject ), setLineHeight, defaultProtectedGetFn,
      "The height, in world units, of each line. Normally this would be specified by the font "
      "size, but the text object is designed to scale with resolution changes, so this parameter "
      "becomes necessary." );
   addProtectedField( "aspectRatio", TypeF32, Offset( mAspectRatio, t2dTextObject ), setAspectRatio, defaultProtectedGetFn,
      "The ratio of a characters width to its height." );
   addProtectedField( "lineSpacing", TypeF32, Offset( mLineSpacing, t2dTextObject ), setLineSpacing, defaultProtectedGetFn,
      "The amount of extra space to include between each line." );
   addProtectedField( "characterSpacing", TypeF32, Offset( mCharacterSpacing, t2dTextObject ), setCharacterSpacing, defaultProtectedGetFn,
      "The amount of extra space to include between each character." );
   addProtectedField( "autoSize", TypeBool, Offset( mAutoSize, t2dTextObject ), defaultProtectedSetFn, defaultProtectedGetFn,
      "Whether or not the object should be scaled when the text is changed." );
   addProtectedField( "fontSizes", TypeF32Vector, Offset( mFontSizes, t2dTextObject ), setFontSizes, getFontSizes,
      "Space separated list of font sizes the text object should use." );
   
   // The font color just links to the t2dSceneObject blend color.
   addProtectedField( "textColor", TypeColorF, Offset( mBlendColour, t2dTextObject ), t2dSceneObject::setBlendColor, defaultProtectedGetFn,
      "The color of the text." );

   addField( "filter", TypeBool, Offset( mFilter, t2dTextObject), "Whether or not the font is filtered" );
   addField( "integerPrecision", TypeBool, Offset( mIntegerPrecision, t2dTextObject), "Whether or not the font destination is clamped to integer values" );

   //Luma:	allow text to not use Unicode
   addField( "noUnicode", TypeBool, Offset( mNoUnicode, t2dTextObject), "Whether or not the text doesn't need to support Unicode conversions (ie. know that it is just numbers, etc.)" );
}

void t2dTextObject::copyTo(SimObject* object)
{
   Parent::copyTo(object);

   AssertFatal(dynamic_cast<t2dTextObject*>(object), "t2dTextObject::copyTo - Copy object is not a t2dTextObject!");
   t2dTextObject* textObject = static_cast<t2dTextObject*>(object);

   textObject->mAutoSize = false;
   textObject->setText(mText);
   textObject->setFont(mFontName);
   textObject->mWordWrap = mWordWrap;
   textObject->mClipText = mClipText;
   textObject->mTextAlignment = mTextAlignment;
   textObject->mLineHeight = mLineHeight;
   textObject->mAspectRatio = mAspectRatio;
   textObject->mLineSpacing = mLineSpacing;
   textObject->mCharacterSpacing = mCharacterSpacing;
   textObject->mAutoSize = mAutoSize;
   textObject->mFilter = mFilter;
   textObject->mIntegerPrecision = mIntegerPrecision;

   //Luma:	allow text to not use Unicode
   textObject->mNoUnicode = mNoUnicode;

   for (S32 i = 0; i < getFontSizeCount(); i++)
      textObject->addFontSize(getFontSize(i));

   textObject->setCharactersDirty();
}

//-----------------------------------------------------------------------------
// Text Management
//-----------------------------------------------------------------------------
void t2dTextObject::setText( const char* text )
{
   // Convert to a string buffer and set.

	//Luma:	allow text to not use Unicode
	setText(StringBuffer(text, mNoUnicode));
}

void t2dTextObject::setText( const StringBuffer& text )
{
   // Clean up the old character bounds.
   SAFE_DELETE_ARRAY( mCharacterBounds );

   // Allocate space for the new character bounds.
   if(mCharacterBounds)
      delete[] mCharacterBounds;

   S32 len = text.length();
   mCharacterBounds = new RectF[len + 2];

   // Set the text.
   mText.set( &text );

   // Force recalculation of character positions.
   setCharactersDirty();

   // Auto size, if applicable.
   if( hasFont() && !mWordWrap )
      autoSize();

   // Set the cursor position to the end of the string.
   setCursorPosition( len );
   showCursorAtEndOfLine( false );
}

const StringBuffer& t2dTextObject::getText() const
{
   return mText;
}

void t2dTextObject::insertText( UTF16* text )
{
   mText.insert( mCursorPos, text );

   // Save off the cursor pos because setText will change it.
   S32 oldCursorPos = mCursorPos;

   // Set the new text.
   setText( mText );

   // Move the cursor to the end of the inserted text.
   mCursorPos = oldCursorPos + dStrlen( text );
}

void t2dTextObject::deleteHighlighted()
{
   // Only if the highlight block is valid.
   if( ( mHighlightBlock < 0 ) || ( mHighlightBlock > mText.length() ) )
      return;

   // Cut the characters between the cursor pos and the highlight block.
   mText.cut( mCursorPos, mHighlightBlock - mCursorPos );

   // Save off the old cursor position. setText changes it.
   S32 oldCursorPos = mCursorPos;
   setText( mText );
   mCursorPos = oldCursorPos;

   // No more highlight block.
   mHighlightBlock = -1;
}

void t2dTextObject::backspace()
{
   // If a highlight block doesn't exist, set it to the character before the
   // cursor.
   if( mHighlightBlock < 0 )
      setHighlightBlock( mCursorPos - 1, mCursorPos );

   deleteHighlighted();
}

void t2dTextObject::deleteCharacter()
{
   // If a highlight block doesn't exist, set it to the character after the
   // cursor.
   if( mHighlightBlock < 0 )
      setHighlightBlock( mCursorPos, mCursorPos + 1 );

   deleteHighlighted();
}

void t2dTextObject::setLineHeight( F32 height )
{
   if( !mWordWrap && getInitialUpdate() )
   {
      F32 scale = height / mLineHeight;
      Parent::setSize( getSize() * scale );
      updateSpatialConfig();
   }

   mLineHeight = height;
   setCharactersDirty();
}

void t2dTextObject::setAspectRatio( F32 ar )
{
   if( !mWordWrap && getInitialUpdate() )
   {
      F32 scale = ar / mAspectRatio;
      Parent::setSize( t2dVector( getSize().mX * scale, getSize().mY ) );
      updateSpatialConfig();
   }

   mAspectRatio = ar;
   setCharactersDirty();
}

//-----------------------------------------------------------------------------
// Font Management
//-----------------------------------------------------------------------------
bool t2dTextObject::setFont( const char* fontName )
{
   // Set the font name.
   mFontName = StringTable->insert( fontName );

   // Grab a list of all the font heights.
   S32 sizeCount = mFonts.size();

   // If their are no sizes, we can't generate any fonts.
   if( sizeCount < 1 )
      return false;

   // Save off the old font sizes for recreation.
   FrameTemp<S32> fontSizes( sizeCount );
   for( S32 i = 0; i < sizeCount; i++ )
      fontSizes[i] = mFonts[i].height;

   // Delete the old font references.
   mFonts.clear();

   // And regenerate the new fonts at the same sizes.
   bool result = false;
   for( S32 i = 0; i < sizeCount; i++ )
   {
      // If we create even one successfully, return true.
      if( addFontSize( fontSizes[i] ) )
         result = true;
   }

   // Recalculate character and object bounds.
   setCharactersDirty();
   if( mAutoSize )
      autoSize();
   
   // Reset the font name if we failed.
   if( !result )
      mFontName = StringTable->insert( "" );

   return result;
}

StringTableEntry t2dTextObject::getFontName() const
{
   return mFontName;
}

bool t2dTextObject::createFont( const char* fontFace, S32 fontSize, Font& font )
{
   // Get the font cache directory.
   const char* fontCacheDirectory = Con::getVariable( "$GUI::fontCacheDirectory" );

   // Create the font.
   Resource<GFont> fontResource = GFont::create( fontFace, fontSize, fontCacheDirectory );

   // Check for success.
   if( fontResource.isNull() )
      return false;

   font.font = fontResource;
   font.height = fontSize;
   return true;
}

bool t2dTextObject::addFontSize( S32 fontSize )
{
   // Can't really do anything if the font name hasn't been set yet.
   if( !mFontName || !*mFontName )
   {
      Con::warnf( "t2dTextObject::addFontSize - Unable to create font at size %d. The font name hasn't been set yet.", fontSize );
      return false;
   }

   // Make sure it doesn't already exist.
   for( Vector<Font>::const_iterator iter = mFonts.begin(); iter != mFonts.end(); iter++ )
   {
      if( iter->height == fontSize )
      {
         Con::warnf( "t2dTextObject::addFontSize - Unable to create font at size %d. The font size already exists.", fontSize );
         return false;
      }
   }

   // Create the font.
   Font newFont;
   if( !createFont( mFontName, fontSize, newFont ) )
   {
      Con::warnf( "t2dTextObject::addFontSize - Unable to create font %s at size %d. Font generation failed.", mFontName, fontSize );
      return false;
   }

   // Make sure default characters are in the cache.
   for( UTF16 i = 32; i < 128; i++ )
      newFont.font->getCharInfo( i );

   // Allocate space, and construct the new object since Resources require
   // construction.
   mFonts.increment();
   constructInPlace( &mFonts.last() );
   mFonts.last() = newFont;

   return true;
}

void t2dTextObject::removeFontSize( S32 fontSize )
{
   // Find the font size.
   for( Vector<Font>::iterator iter = mFonts.begin(); iter != mFonts.end(); iter++ )
   {
      if( iter->height == fontSize )
      {
         // Erase. No need to keep the vector sorted.
         mFonts.erase_fast( iter );
         return;
      }
   }

   Con::warnf( "t2dTextObject::removeFontSize - Unable to remove font size %d. This size doesn't exist.", fontSize );
}

void t2dTextObject::removeAllFontSizes()
{
   mFonts.clear();
}

const t2dTextObject::Font& t2dTextObject::getFont( S32 pixelSize ) const
{
   AssertFatal( !mFonts.empty(), "t2dTextObject::getFont - The object has no fonts" );

   if( pixelSize <= 0 )
      return mFonts[0];

   // Find the smallest font that is greater than the pixel size.
   S32 index = 0;
   S32 size = mFonts[index].height;
   for( S32 i = 1; i < mFonts.size(); i++ )
   {
      S32 newSize = mFonts[i].height;

      // If the new size is between the pixel size and old size or
      // if the old size is smaller than the pixel size and the new size is
      // greater than the old size.
      if( ( ( newSize >= pixelSize ) && ( newSize < size ) ) ||
          ( ( size < pixelSize ) && ( newSize > size ) ) )
      {
         index = i;
         size = newSize;
      }
   }

   return mFonts[index];
}

//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------
void t2dTextObject::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
   // Nothing to do without a font or text.
   if( !hasFont() )
      return Parent::renderObject( viewPort, viewIntersection );

#ifdef PUAP_OPTIMIZE
//-Mat if we are invisible, don't render
   if( mBlendColour.alpha <= 0 ) {
      return Parent::renderObject( viewPort, viewIntersection );
   }
#endif

   // Calculate pixel size of the object.
   S32 pixelSize = 0;
   t2dSceneWindow* sceneWindow = getSceneGraph()->getCurrentRenderWindow();

   // This could happen if a guiT2DObjectCtrl is rendering this object. For
   // now, we'll just pick an arbitrary size.
   if( !sceneWindow )
      pixelSize = viewPort.extent.x;

   else
   {
      // This is the size of the world over the pixel size of the scene window.
      F32 ratioY = sceneWindow->getCurrentCameraWindowScale().mY;
      pixelSize = (S32)( ( mLineHeight / ratioY ) + 0.5f );
   }

   // Get the font to render with.
   GFont* font = getFont( pixelSize ).font;

   // Set blending.
   setBlendOptions();

   // Rotate the view based on the object's rotation.
//IPHONE WARNING: tilelayer 2 deep in the projection matrix is all that is allowed
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();

   // Translate to (0, 0), rotate, and translate back.
   t2dVector pos = getRenderPosition();
   glTranslatef( pos.mX, pos.mY, 0.0f );
   glRotatef( getRenderRotation(), 0.0f, 0.0f, 1.0f );
   glTranslatef( -pos.mX, -pos.mY, 0.0f );

   // Render the text.
   if( mText.length() > 0 )
	  renderText( font );

   // Render the cursor.
   if( mRenderCursor )
      renderCursor( font );

   glPopMatrix();
	
   // Call parent for debug rendering.
   Parent::renderObject( viewPort, viewIntersection );
}

void t2dTextObject::renderCursor( GFont* font )
{
   // Determine if we should render before or after the character at the
   // cursor position.
   S32 index = mCursorPos;

   // Get the character bounds at the cursor position.
   RectF bounds = getCharacterBounds( index );

   // Move the render point down a little bit, just because it looks nicer.
   F32 ySizeRatio = getSizeRatio( font ).mY;
   Point2F point = bounds.point;
   point.y += font->getDescent() * ySizeRatio * 0.5;

   // Determine size.
   F32 halfWidth = 0.25f;
   F32 height = font->getBaseline() * ySizeRatio;

   // Generate bounds.
   Point2F upperLeft  = Point2F( point.x - halfWidth, point.y );
   Point2F upperRight = Point2F( point.x + halfWidth, point.y );
   Point2F lowerLeft  = Point2F( point.x - halfWidth, point.y + height );
   Point2F lowerRight = Point2F( point.x + halfWidth, point.y + height );

#ifdef TORQUE_OS_IPHONE
	//now to draw to the screen

	//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
	glColor4f( mCursorColor.red, mCursorColor.green, mCursorColor.blue, mCursorColor.alpha );
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	dglDrawTextureQuadiPhone(lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y, upperLeft.x, upperLeft.y, upperRight.x, upperRight.y, 
								0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
#else
   // Render.
   glColor4fv( (GLfloat*)&mCursorColor );
   glBegin( GL_QUADS );
      glVertex2fv( (GLfloat*)(&upperLeft) );
      glVertex2fv( (GLfloat*)(&upperRight) );
      glVertex2fv( (GLfloat*)(&lowerRight) );
      glVertex2fv( (GLfloat*)(&lowerLeft) );
   glEnd();
#endif
}

void t2dTextObject::renderText( GFont* font )
{
   // Grab some object info.
   RectF worldRect = getWorldClipRectangle();
   t2dVector sizeRatio = getSizeRatio( font );
#ifndef TORQUE_OS_IPHONE

   // Get info for all the characters to make sure the necessary textures are
   // properly generated prior to the glBegin call.
   for( S32 i = 0; i < mText.length(); i++ )
   {
      const UTF16 c = mText.getChar( i );
      if(font->isValidChar( c ) )
         const PlatformFont::CharInfo& ci = font->getCharInfo( c );
   }
#endif
   // The last used texture is cached so we don't do unnecessary texture
   // swaps (Expensive).
   TextureObject* lastTexture = NULL;

   // Set up GL.
   glDisable(GL_LIGHTING);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
   // Render!
#ifndef TORQUE_OS_IPHONE
   glBegin(GL_QUADS);
#endif
   for( S32 i = 0; i < mText.length(); i++ )
   {
      const UTF16 c = mText.getChar( i );

      // Grab the character bounds.
      const RectF& charBounds = getCharacterBounds( i );

      // Draw a solid quad if the character is in the highlight block.
      if( ( i >= mCursorPos ) && ( i < mHighlightBlock ) && ( c != dT( '\n' ) ) )
      {
#ifndef TORQUE_OS_IPHONE
         // End the render batch since we need to disable texturing.
         glEnd();
#endif

         // Get the area.
         RectF bounds = charBounds;
         // Account for character spacing.
         bounds.extent.x += mCharacterSpacing * sizeRatio.mX;

         // Don't render outside the object bounds.
         if( mClipText )
            bounds.intersect( worldRect );

         if( bounds.isValidRect() )
         {
            // Grab rectangle corners.
            Point2F upperLeft = bounds.point;
            Point2F upperRight = Point2F( bounds.point.x + bounds.extent.x, bounds.point.y );
            Point2F lowerLeft = Point2F( bounds.point.x, bounds.point.y + bounds.extent.y );
            Point2F lowerRight = bounds.point + bounds.extent;

			if(mIntegerPrecision)
			{
				//round the locations of the text to be the nearest pixels
				upperLeft.round();
				upperRight.round();
				lowerLeft.round();
				lowerRight.round();
			}

            // Draw the black rect.
            glDisable(GL_TEXTURE_2D);

			//set alpha of the text
			glColor4f( 0.0f, 0.0f, 0.0f, mAlpha );

#ifdef TORQUE_OS_IPHONE
			//Luma: Use supposedly more optimal macro that renders from global instead of heap memory
			dglDrawTextureQuadiPhone(upperLeft.x, upperLeft.y, upperRight.x, upperRight.y, lowerRight.x, lowerRight.y, lowerLeft.x, lowerLeft.y, 
										0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            
			// Re-enable texturing, but set the blend color to white.
            glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
            glEnable(GL_TEXTURE_2D);
#else
            glBegin( GL_QUADS );
               glVertex2fv( (GLfloat*)(&upperLeft) );
               glVertex2fv( (GLfloat*)(&upperRight) );
               glVertex2fv( (GLfloat*)(&lowerRight) );
               glVertex2fv( (GLfloat*)(&lowerLeft) );
            glEnd();

            // Re-enable texturing, but set the blend color to white.
			//set the alpha of the text
			glColor4f( 0.0f, 0.0f, 0.0f, mAlpha );

            glEnable(GL_TEXTURE_2D);
            glBegin( GL_QUADS );
#endif
         }
      }
      else if( i == mHighlightBlock )
      {
         // End of the highlight block. Reset the blend color.
#ifdef TORQUE_OS_IPHONE
         glColor4f( mBlendColour.red, mBlendColour.green, mBlendColour.blue, mBlendColour.alpha );
#else
         glEnd();
         glColor4fv( (GLfloat*)&mBlendColour );
         glBegin( GL_QUADS );
#endif
      }

      // Make sure we can render this character.
      if( !isRenderable( c ) )
         continue;

      // Grab the character info.
      const PlatformFont::CharInfo& ci = font->getCharInfo( c );

      // Swap the texture if necessary.
      TextureObject* newTexture = font->getTextureHandle( ci.bitmapIndex );
      if( newTexture != lastTexture )
      {
#ifndef TORQUE_OS_IPHONE
         glEnd();
#endif
         glBindTexture(GL_TEXTURE_2D, newTexture->texGLName);

			//set proper texture filtering on the text object (linear or nearest)
		   if(mFilter)
		   {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   }
		   else
		   {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		   }

         lastTexture = newTexture;
#ifndef TORQUE_OS_IPHONE
         glBegin(GL_QUADS);
#endif
      }

      // The position of the character is the render position, plus various
      // offsets, depending on the character.
      t2dVector charPosition = charBounds.point;
      charPosition.mX += ci.xOrigin * sizeRatio.mX;
      charPosition.mY += ( (F32)font->getBaseline() - ci.yOrigin ) * sizeRatio.mY;

      // The size of the character is dependent on the previously computed
      // size ratio.
      t2dVector charSize;
      charSize.mX = ci.width * sizeRatio.mX;
      charSize.mY = ci.height * sizeRatio.mY;

      // Grab the bounding rectangle.
      RectF bounds = RectF( charPosition.mX, charPosition.mY, charSize.mX, charSize.mY );

      // Unless we are rendering beyond, make sure the character is within the
      // object bounds.
      if( mClipText )
      {
         bounds.intersect( worldRect );
         if( !bounds.isValidRect() )
            continue;
      }

      // The texture scale is based on the amount the character was clipped.
      // It keeps the textures from squishing in the case that the character
      // was clipped.
      t2dVector texScale;
      texScale.mX = bounds.extent.x / charSize.mX;
      texScale.mY = bounds.extent.y / charSize.mY;

      t2dVector texOffset = t2dVector::getZero();
      if( mIsEqual( worldRect.point.x, bounds.point.x ) )
      {
         texOffset.mX = 1.0f - texScale.mX;
         texScale.mX = 1.0f;
      }

      Point2F upperLeft = bounds.point;
      Point2F upperRight = Point2F( bounds.point.x + bounds.extent.x, bounds.point.y );
      Point2F lowerLeft = t2dVector( bounds.point.x, bounds.point.y + bounds.extent.y );
      Point2F lowerRight = bounds.point + bounds.extent;
      
	if(mIntegerPrecision)
	{
		//round the locations of the text to be at the nearest pixels
		upperLeft.round();
		upperRight.round();
		lowerLeft.round();
		lowerRight.round();
	}

      // Texture coords.
      F32 texLeft = ( F32( ci.xOffset ) + ( ci.width * texOffset.mX ) ) / F32( lastTexture->texWidth );
      F32 texRight = F32( ci.xOffset + ( ci.width * texScale.mX ) ) / F32( lastTexture->texWidth );
      F32 texTop = F32( ci.yOffset ) / F32( lastTexture->texHeight );
      F32 texBottom = F32( ci.yOffset + ( ci.height * texScale.mY ) ) / F32( lastTexture->texHeight );


#ifdef TORQUE_OS_IPHONE
	  //Luma: Use supposedly more optimal macro that renders from global instead of heap memory
  	  dglDrawTextureQuadiPhone(lowerLeft.x, lowerLeft.y, lowerRight.x, lowerRight.y, upperLeft.x, upperLeft.y, upperRight.x, upperRight.y, 
								texLeft, texBottom, texRight, texBottom, texLeft, texTop, texRight, texTop);
#else
      glTexCoord2f( texLeft, texTop );
      glVertex2fv( (GLfloat*)(&upperLeft) );

      glTexCoord2f( texRight, texTop );
      glVertex2fv( (GLfloat*)(&upperRight) );

      glTexCoord2f( texRight, texBottom );
      glVertex2fv( (GLfloat*)(&lowerRight) );

      glTexCoord2f( texLeft, texBottom );
      glVertex2fv( (GLfloat*)(&lowerLeft) );
#endif
   }
#ifndef TORQUE_OS_IPHONE
   glEnd();
   
#endif
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
}

//-----------------------------------------------------------------------------
// Text Field Info Gathering
//-----------------------------------------------------------------------------
F32 t2dTextObject::getLineWidth( GFont* font ) const
{
   U32 width = 0;
   U32 longest = 0;
   for( S32 i = 0; ; i++ )
   {
      // The end of a line is either the null terminator or the new line char.
      if( !mText.getChar( i ) || ( mText.getChar( i ) == dT( '\n' ) ) )
      {
         // Found a new longest line.
         if( width > longest )
            longest = width;

         // Reset the width.
         width = 0;

         // Break if this is the last line.
         if( !mText.getChar( i ) )
            break;

         continue;
      }

      UTF16 c = mText.getChar( i );
      if( c == '\t' )
         width += font->getCharXIncrement( ' ' ) * GFont::TabWidthInSpaces;
      else
         width += font->getCharXIncrement( c );

   }

   return (F32)longest;
}

void t2dTextObject::setWordWrap( bool wrap )
{
   mWordWrap = wrap;
   setCharactersDirty();

   if( wrap )
      mAutoSize = false;
   else
      autoSize();
}

bool t2dTextObject::isWhiteSpace( UTF16 c ) const
{
   // Whitespace is space, null terminator, tab, or new line.
   return !c || ( c == dT( ' ' ) ) || ( c == dT( '\t' ) ) || ( c == dT( '\n' ) );
}

bool t2dTextObject::isRenderable( UTF16 c ) const
{
   // Renderable characters are anything valid as determined by the font that
   // has a size and isn't tab or newline.

   GFont* font = getFont().font;

   if( !font || !font->isValidChar( c ) )
      return false;

   if( ( c == dT( '\t' ) ) || ( c == dT( '\n' ) ) )
      return false;

   const PlatformFont::CharInfo& ci = font->getCharInfo( c );
    if( ( ci.width == 0 ) || ( ci.height == 0 ) )
      return false;

   return true;
}

void t2dTextObject::autoSize()
{
   // Gotta have a font and auto sizing doesn't work with word wrapping.
   if( !hasFont() || mWordWrap )
      return;

   GFont* font = getFont().font;

   // Store the old size for positioning.
   t2dVector oldSize = getSize();

   // Calculate the size.
   t2dVector size;
   // The width is the width of the longest line, times the aspect ratio,
   // times the size ratio.
   size.mX = getLineWidth( font ) * mAspectRatio * getSizeRatio( font ).mY;

   // The height is the number of lines times the height of each line.
   size.mY = getLineCount() * ( mLineHeight + mLineSpacing );

   // Position based on alignment.
   t2dVector newPosition = getPosition();
   switch( mTextAlignment )
   {
   case LEFT:
   case JUSTIFY:
      // Move by half the amount the size changed down and to the right.
      newPosition += ( size - oldSize ) * 0.5f;
      break;

   case RIGHT:
      // Move by half the amount the size changed down and to the left.
      newPosition.mX -= ( size.mX - oldSize.mX ) * 0.5f;
      newPosition.mY += ( size.mY - oldSize.mY ) * 0.5f;
      break;

   case CENTER:
      // Move by half the amount the size changed down.
      newPosition.mY += ( size.mY - oldSize.mY ) * 0.5f;
      break;
   };

   // Update size.
   Parent::setSize( size );
   setPosition( newPosition );
   updateSpatialConfig();

   // Reset auto size flag. setSize turned it off.
   mAutoSize = true;
}

void t2dTextObject::autoSizeHeight()
{
   t2dVector newSize = getSize();
   t2dVector newPosition = getPosition();

   RectF bounds = getTextBounds();
   newSize.mY = bounds.extent.y;

   newPosition.mY = bounds.point.y + ( bounds.extent.y * 0.5f );

   Parent::setSize( newSize );
   Parent::setPosition( newPosition );
   setCharactersDirty();
}

F32 t2dTextObject::getNextWrapWidth( const StringBuffer& text ) const
{
   F32 width = 0.0f;
   S32 i = 0;

   GFont* font = getFont().font;

   // Nothing doing if the first character is a new line.
   if( text.getChar( 0 ) == dT( '\n' ) )
      return width;

   // Find the number of characters until the next whitespace.
   for( i = 0; !isWhiteSpace( text.getChar( i ) ); i++ );

   // Calculate the width.
   width = font->getStrNWidth( text.getPtr(), i - 1 );

   return width;
}

S32 t2dTextObject::getLineCount() const
{
   // Always have at least 1 line.
   S32 lineCount = 1;
   for( S32 i = 0; i < mText.length(); i++ )
   {
      // Increment the count on every new line.
      if( mText.getChar( i ) == dT( '\n' ) )
         lineCount++;
   }

   return lineCount;
}

S32 t2dTextObject::getLineNumber( S32 index )
{
   // Grab the y position of the first line.
   F32 yPos = getCharacterBounds( 0 ).point.y;

   // The first line is line 1.
   S32 lineCount = 1;

   // Iterate from the start of the text to the requested index.
   for( S32 i = 1; i <= index; i++ )
   {
      // Each time the y position of a character changes, we're at a new line.
      F32 newYPos = getCharacterBounds( i ).point.y;
      if( mNotEqual( newYPos, yPos ) )
      {
         yPos = newYPos;
         lineCount++;
      }
   }
   return lineCount;
}

S32 t2dTextObject::getLineIndex( S32 lineNumber )
{
   // Line number 1 starts at index 0.
   if( lineNumber <= 1 )
      return 0;

   // Grab the y position of the first line.
   F32 yPos = getCharacterBounds( 0 ).point.y;
   S32 lineCount = 1;
   for( S32 i = 1; i < mText.length(); i++ )
   {
      F32 newYPos = getCharacterBounds( i ).point.y;
      if( mNotEqual( newYPos, yPos ) )
      {
         yPos = newYPos;
         lineCount++;
      }

      // If we reached the requested line, then the current index is the
      // first character in that line.
      if( lineCount == lineNumber )
         return i;
   }

   // Only way to get here is if the requested line number is greater than
   // the number of lines in the string.
   return mText.length();
}

S32 t2dTextObject::getActualLineCount()
{
   // Grab the y position of the first line.
   F32 yPos = getCharacterBounds( 0 ).point.y;
   S32 lineCount = 1;
   for( S32 i = 1; i < mText.length(); i++ )
   {
      // Each time the y position of a character changes, we're at a new line.
      F32 newYPos = getCharacterBounds( i ).point.y;
      if( mNotEqual( newYPos, yPos ) )
      {
         yPos = newYPos;
         lineCount++;
      }
   }
   return lineCount;
}

F32 t2dTextObject::getLineHeight() const
{
   // Determine the line height. If mLineHeight is 0, then the line height is
   // whatever fills the object bounds.
   F32 lineHeight = mLineHeight;
   if( lineHeight <= 0 )
      lineHeight = getSize().mY / getLineCount();

   return lineHeight;
}

t2dVector t2dTextObject::getSizeRatio( GFont* font ) const
{
   F32 lineHeight = getLineHeight();

   // The size ratio stores the amount to scale the font in each direction.
   t2dVector sizeRatio;
   sizeRatio.mY = lineHeight / (F32)font->getHeight();
   sizeRatio.mX = mAspectRatio * sizeRatio.mY;

   return sizeRatio;
}

//-----------------------------------------------------------------------------
// Character Bounds
//-----------------------------------------------------------------------------
void t2dTextObject::alignText( S32 lineStart, S32 lineEnd )
{
   // The bounds are still dirty, since this is called from calculate bounds,
   // but these characters should have their bounds properly computed.
   RectF firstRect = mCharacterBounds[lineStart];
   RectF lastRect = mCharacterBounds[lineEnd - 1];

   F32 lineWidth = getSize().mX;
   F32 leftPoint = firstRect.point.x;
   F32 rightPoint = lastRect.point.x + lastRect.extent.x;
   F32 textWidth = rightPoint - leftPoint;

   F32 offset = 0.0f;
   switch( mTextAlignment )
   {
   // Set the start of the line to the left side of the object.
   case LEFT:
      offset = -getHalfSize().mX - leftPoint;
      break;

   // Set the end of the line to the right side of the object.
   case RIGHT:
      offset = getHalfSize().mX - rightPoint;
      break;

   // Center the line within the object bounds.
   case CENTER:
      offset = ( -getHalfSize().mX + ( ( lineWidth - textWidth ) / 2.0f ) ) - leftPoint;
      break;

   // Space out the characters so the line fills the object bounds.
   case JUSTIFY:
      // Don't justify lines with a line break at the end or the last line.
      if( ( lineEnd > mText.length() ) || !mText.getChar( lineEnd ) || ( mText.getChar( lineEnd ) == dT( '\n' ) ) )
         return;

      // Do nothing if the line is too long.
      if( ( lineWidth - textWidth ) < 0.0f )
         return;

      // Space out characters on full lines.
      {
         S32 spaces = 0;
         for( S32 i = lineStart; i < lineEnd; i++ )
         {
            if( mText.getChar( i ) == dT( ' ' ) )
               spaces++;
         }

         if( spaces < 1 )
         {
            F32 whiteSpace = ( lineWidth - textWidth ) / ( lineEnd - lineStart - 1 );
            F32 offset = whiteSpace;
            for( S32 i = lineStart + 1; i < lineEnd; i++ )
            {
               // Stretch the previous character to fill the empty space.
               mCharacterBounds[i - 1].extent.x += whiteSpace;
               // Move the current character.
               mCharacterBounds[i].point.x += offset;
               offset += whiteSpace;
            }
         }

         else
         {
            F32 whiteSpace = ( lineWidth - textWidth ) / spaces;
            F32 offset = 0.0f;
            for( S32 i = lineStart + 1; i < lineEnd; i++ )
            {
               // Move the current character.
               mCharacterBounds[i].point.x += offset;
               if( mText.getChar( i ) == dT( ' ' ) )
               {
                  // Stretch spaces.
                  offset += whiteSpace;
                  mCharacterBounds[i - 1].extent.x += offset;
               }
            }
         }
      }
      return;
   }

   // Move each character in the line based on the computed offset.
   for( S32 i = lineStart; i <= lineEnd; i++ )
      mCharacterBounds[i].point.x += offset;
}

void t2dTextObject::calculateCharacterBounds()
{
   // Keep track of whether or not this function is currently executing. Since
   // this gets called from several different places, incorrectly structured
   // code could cause infinite recursion. This assert should make that problem
   // easy to track down.
   static bool calculatingBounds = false;
   AssertFatal( !calculatingBounds, "t2dTextObject::calculateCharacterBounds - This function cannot be called recursively." );
   calculatingBounds = true;

   GFont* font = getFont().font;

   F32 lineHeight = getLineHeight();
   t2dVector sizeRatio = getSizeRatio( font );
   
   // The position at which rendering starts. This is saved so new lines know
   // the x position at which to start rendering.
   const t2dVector startPosition = -getHalfSize();

   // The end position is the bottom right of the bounds and the point outside
   // of which nothing should be rendered.
   const t2dVector endPosition = getHalfSize();

   // The upper left of the current render position.
   t2dVector renderPosition = startPosition;

   // Specifies whether or not we are at the first character after a line
   // wrap. In wrap modes, we don't want to render spaces at the beginning
   // of wrapped lines.
   bool wrappedLine = false;

   // The width of the next wrap object. In the case of character wrap, this
   // will be the next character width. In the case of word wrap, this will
   // be the next word width.
   F32 nextWrapWidth = 0.0f;

   S32 lineStart = 0;

   bool firstWord = true;

   // Loop through each character, calculating the position at each step.
   for( U32 i = 0; i < mText.length(); i++ )
   {
      const UTF16 c = mText.getChar( i );

      // Grab the bounding rectangle.
      mCharacterBounds[i] = RectF( renderPosition.mX, renderPosition.mY, 0, lineHeight );

      if( isWhiteSpace( c ) )
         firstWord = false;
      
      // Deal with special characters.
      // Tab.
      if ( c == dT('\t') ) 
      {
         const PlatformFont::CharInfo &space = font->getCharInfo( ' ' );
         renderPosition.mX += space.xIncrement * GFont::TabWidthInSpaces * sizeRatio.mX;
         mCharacterBounds[i].extent.x = renderPosition.mX;
         continue;
      }

      // New line.
      else if( c == dT('\n') )
      {
         alignText( lineStart, i );
         lineStart = i + 1;
         renderPosition.mY += lineHeight + ( mLineSpacing * sizeRatio.mY );
         renderPosition.mX = startPosition.mX;
         firstWord = true;
         continue;
      }
      
      // Grab the character info.
      const PlatformFont::CharInfo &ci = font->getCharInfo( c );

      // Update the bounding rectangle.
      mCharacterBounds[i].extent.x = ci.xIncrement * sizeRatio.mX ;
      

      if( mWordWrap && isWhiteSpace( mText.getChar( i ) ) )
      {
         StringBuffer text = mText.substring( i + 1, mText.length() - i - 1 );
         nextWrapWidth = ( getNextWrapWidth( text ) + font->getCharInfo( dT( ' ' ) ).xIncrement ) * sizeRatio.mX;
         if( ( renderPosition.mX + nextWrapWidth ) > endPosition.mX )
         {
            alignText( lineStart, i );
            lineStart = i + 1;

            renderPosition.mY += lineHeight + ( mLineSpacing * sizeRatio.mY );
            renderPosition.mX = startPosition.mX;
            wrappedLine = true;
            firstWord = true;
         }

         else
            renderPosition.mX += ( ci.xIncrement + mCharacterSpacing ) * sizeRatio.mX;
      }

      // Don't print spaces at the beginning of wrapped lines.
      else if( wrappedLine && ( c == dT( ' ' ) ) && mWordWrap )
      {
         // Don't change render position.
      }

      // If we're on the first word of a line and passing the end position, we
      // need to wrap in the middle of the word.
      else if( mWordWrap && firstWord && ( i + 1 < mText.length() ) && !isWhiteSpace( mText.getChar( i + 1 ) ) )
      {
         const PlatformFont::CharInfo &nextInfo = font->getCharInfo( mText.getChar( i + 1 ) );

         F32 charWidth = ci.xIncrement * sizeRatio.mX;
         F32 nextWidth = nextInfo.xIncrement * sizeRatio.mX;
         if( ( renderPosition.mX + charWidth + nextWidth ) > endPosition.mX )
         {
            alignText( lineStart, i );
            lineStart = i + 1;

            renderPosition.mY += lineHeight + ( mLineSpacing * sizeRatio.mY );
            renderPosition.mX = startPosition.mX;
            wrappedLine = true;
            firstWord = true;
         }

         else
            renderPosition.mX += ( ci.xIncrement + mCharacterSpacing ) * sizeRatio.mX;
      }

      else
      {
         wrappedLine = false;
         renderPosition.mX += ( ci.xIncrement + mCharacterSpacing ) * sizeRatio.mX;
      }
   }

   // Set the position of the last character.
   mCharacterBounds[mText.length()] = RectF( renderPosition.mX, renderPosition.mY, 0.0f, lineHeight );

   // Align the last line.
   alignText( lineStart, mText.length() + 1 );

   setCharactersDirty( false );
   calculatingBounds = false;
}

RectF t2dTextObject::getTextBounds()
{
   RectF upperLeft = getCharacterBounds( 0 );
   RectF lowerRight = getCharacterBounds( mText.length() );

   return RectF( upperLeft.point, ( lowerRight.point + lowerRight.extent ) - upperLeft.point );
}

RectF t2dTextObject::getCharacterBounds( S32 index )
{
   // If the character bounds are dirty, recompute.
   if( mCharactersDirty )
      calculateCharacterBounds();

   return RectF( mCharacterBounds[index].point + Point2F( getRenderPosition().mX, getRenderPosition().mY ),  mCharacterBounds[index].extent );
}

void t2dTextObject::setSize( const t2dVector& size )
{
   t2dVector sizeScale = size;
   sizeScale.div( getSize() );

   Parent::setSize( size );

   if( !mEditing )
   {
      mAspectRatio *= sizeScale.mX;
      mAspectRatio /= sizeScale.mY;
      mLineHeight *= sizeScale.mY;
   }

   setCharactersDirty();
}

//-----------------------------------------------------------------------------
// Integrate
//-----------------------------------------------------------------------------
void t2dTextObject::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( sceneTime, elapsedTime, pDebugStats );

   mCursorFadeDelay += elapsedTime;
   // Blink the cursor every 3/4 of a second.
   if( mCursorFadeDelay > 0.75f )
   {
      mCursorColor.alpha = 1.0f - mCursorColor.alpha;
      mCursorFadeDelay = 0.0f;
   }
}
//-----------------------------------------------------------------------------
// Cursor and Highlight Block
//-----------------------------------------------------------------------------
void t2dTextObject::setHighlightBlock( S32 startPos, S32 endPos )
{
   // The start position must be less than the end position. Fix if necessary.
   if( ( endPos >= 0 ) && ( endPos < startPos ) )
   {
      S32 temp = endPos;
      endPos = startPos;
      startPos = temp;
   }

   // Bind to the text bounds, just in case.
   if( startPos < 0 )
      startPos = 0;
   else if( startPos > mText.length() )
      startPos = mText.length();

   if( endPos > (S32)mText.length() )
      endPos = mText.length();

   // If they're the same, nothing should be highlighted.
   if( endPos == startPos )
   {
      mCursorPos = startPos;
      mHighlightBlock = -1;
   }
   else
   {
      mCursorPos = startPos;
      mHighlightBlock = endPos;
   }
}

void t2dTextObject::setCursorPosition( t2dVector worldPos )
{
   setCursorPosition( getCharacterPosition( worldPos ) );
}

void t2dTextObject::setCursorPosition( S32 pos )
{
   setHighlightBlock( pos, -1 );
}

S32 t2dTextObject::getCharacterPosition( t2dVector worldPos )
{
   // Rotate the world position relative to the object.
   worldPos.rotate( getRenderPosition(), -getRenderRotation() );

   // Iterate through the characters, and find the one at worldPos.
   for( S32 i = 0; i < mText.length(); i++ )
   {
      RectF bounds = getCharacterBounds( i );
      if( ( bounds.point.x <= worldPos.mX ) && ( worldPos.mX <= ( bounds.point.x + bounds.extent.x ) ) &&
          ( bounds.point.y <= worldPos.mY ) && ( worldPos.mY <= ( bounds.point.y + bounds.extent.y ) ) )
      {
         if( ( worldPos.mX - bounds.point.x ) > ( bounds.extent.x / 2.0f ) )
            return i + 1;
         else
            return i;
      }
   }

   // Well, the position is not in any character. What we want now is the
   // position at the beginning or end of the line at the y position.

   // Position is above the object.
   F32 top = getRenderPosition().mY - getHalfSize().mY;
   F32 bottom = getRenderPosition().mY + getHalfSize().mY;
   if( worldPos.mY <= top )
      return 0;
   
   // Position is below the object.
   if( worldPos.mY >= bottom )
      return mText.length();

   // Find the first character on the line.
   for( S32 i = 0; i < mText.length(); i++ )
   {
      const RectF cBounds = getCharacterBounds( i );
      if( ( cBounds.point.y <= worldPos.mY ) && ( worldPos.mY <= ( cBounds.point.y + cBounds.extent.y ) ) )
      {
         S32 index, line = getLineNumber(i);

         // if the position is to the right of the object, then we want the
         // first character on this line. Otherwise, we want the last one on
         // this line.
         if( worldPos.mX <= cBounds.point.x )
            index = getLineIndex(line);
         else
            index = getLineIndex(line + 1) - 1;

         return index;
      }
   }

   // The only time we should ever get here is if the position is within the
   // bounds of the object but past the last line.
   return mText.length();
}


//-----------------------------------------------------------------------------
// Protected Field Accessors
//-----------------------------------------------------------------------------
bool t2dTextObject::setFontSizes( void* obj, const char* data )
{
   t2dTextObject* textObject = static_cast<t2dTextObject*>( obj );
   textObject->removeAllFontSizes();

   U32 count = t2dSceneObject::getStringElementCount( data );
   for( U32 i = 0; i < count; i++ )
   {
      S32 size = dAtoi( t2dSceneObject::getStringElement( data, i ) );
      textObject->addFontSize( size );
   }

   return false;
}

const char* t2dTextObject::getFontSizes( void* obj, const char* data )
{
   t2dTextObject* textObject = static_cast<t2dTextObject*>( obj );

   S32 count = textObject->getFontSizeCount();
   char* sizes = Con::getReturnBuffer( count * 8 );
   sizes[0] = '\0';
   char sizeString[8];

   for( S32 i = 0; i < count; i++ )
   {
      S32 size = textObject->getFontSize( i );
      dSprintf( sizeString, 8, "%s%d", i > 0 ? " " : "", size );
      dStrcat( sizes, sizeString );
   }

   return sizes;
}

const char* t2dTextObject::getText( void* obj, const char* data )
{
   const StringBuffer& text = static_cast<t2dTextObject*>( obj )->getText();   
   // we don't need to copy the data to the con return buffer.
   // the purpose of the return buffer is to ensure that we have a buffer that
   // lives past the life of this function.
   
   // [neo, 5/7/2007]
   // Need this const_cast() nonsense as StringBuffer::getPtr8() is not const!
   //return text.getPtr8();
   return const_cast<StringBuffer&>(text).getPtr8();
}

//-----------------------------------------------------------------------------
// Console Methods
//-----------------------------------------------------------------------------
ConsoleMethod( t2dTextObject, addFontSize, bool, 3, 3, "(S32 fontSize)\n"
              "Adds a font size to the text object. Additional font sizes can make the font "
              "look better at more display sizes and resolutions."
              "@param fontSize The font size to add to the text object.\n"
              "@return Whether or not the creation of the font was successful." )
{
   return object->addFontSize( dAtoi( argv[2] ) );
}

ConsoleMethod( t2dTextObject, removeFontSize, void, 3, 3, "(S32 fontSize)\n"
              "Removes a font from the text object.\n"
              "@param fontSize The font size to remove from the text object.\n"
			  "@return No return value." )
{
   object->removeFontSize( dAtoi( argv[2] ) );
}

ConsoleMethod( t2dTextObject, removeAllFontSizes, void, 2, 2, "()\n"
              "Removes all of the font sizes from the text object. Effectively, "
              "all fonts will be removed, so, nothing will be rendered until a font size "
              "is added.\n"
			  "@return No return value." )
{
   object->removeAllFontSizes();
}

ConsoleMethod( t2dTextObject, addAutoFontSize, bool, 5, 5, "(S32 screenHeight, F32 cameraHeight, F32 characterHeight)\n"
              "Adds a font size based on a screen height, camera height, and character height. The resulting font "
              "size is automatically calculated.\n"
              "@param screenHeight The height of the screen to generate the font for.\n"
              "@param cameraHeight The height of the camera to generate the font for.\n"
              "@param characterHeight The height of the text in world units to generate the size for.\n"
              "@return Whether or not the font was created successfully." )
{
   S32 screenHeight = dAtoi( argv[2] );
   F32 cameraHeight = dAtof( argv[3] );
   F32 characterHeight = dAtof( argv[4] );

   // [neo, 5/7/2007 - #3001]
   // Make sure camera size, etc are valid or addFontSize() will do nasty things and crash
   // as well as the obv div by zero.
   if( cameraHeight > 0 && screenHeight > 0 && characterHeight > 0 )
      return object->addFontSize( S32( characterHeight / cameraHeight * screenHeight ) );

   Con::errorf( "t2dTextObject::addAutoFontSize() - invalid screenHeight, cameraHeight or characterHeight" );

   return false;
}

//ConsoleNamespace( t2dTextObject, "This object renders a string of text in a TGB scene. "
//                 "Instanciating a t2dTextObject is the same as for any other TGB object:\n"
//                 "@code\n"
//                 "new t2dTextObject()\n"
//                 "{\n"
//                 "   scenegraph = MyTGBScene;\n"
//                 "};\n"
//                 "@endcode\n\n"
//                 "A key property of the t2dTextObject is that, unlike the guiTextCtrls, the size "
//                 "of the text is independent of the screen resolution or scene window size. That "
//                 "means it will scale correctly when the size of the window changes. You should "
//                 "be aware, however, that rendering the text at a font size that is wildly different "
//                 "than the pixel size of the text on screen can adversely affect how well it looks. "
//                 "To counteract this, the t2dTextObject allows you to specify multiple font sizes "
//                 "to use. When rendering, the object will choose the font size that matches the "
//                 "pixel size the closest.\n"
//                 "The t2dTextObject also inherently supports unicode for localization to other regions.\n"
//                 "And of course, since the t2dTextObject is a subclass of the t2dSceneObject, it "
//                 "supports all of the usual scene object properties like collision, physics, mounting, "
//                 "rotation, etc."



// Sets the text alpha
void t2dTextObject::setAlpha(F32 alpha)
{
	if (alpha >= 0 && alpha <= 1)
	{
		mBlendColour.alpha = alpha;
	}
}

// Returns the alpha level
F32 t2dTextObject::getAlpha()
{
	return mBlendColour.alpha;
}

ConsoleMethod( t2dTextObject, setAlpha, void, 3, 3, "setAlpha(fAlpha)\n"
          "Sets the text alpha level between 0 and 1.\n"
		  "@return No return value." )
{
	object->setAlpha(dAtof(argv[2]));
}

ConsoleMethod( t2dTextObject, getAlpha, F32, 2, 2, "()\n"
          "Returns the alpha value." )
{
	return object->getAlpha();
}


//returns the actual size that the text takes up
void t2dTextObject::getTextSize(t2dVector *pSize)
{
   GFont* font = getFont().font;
   pSize->mX = getLineWidth( font ) * mAspectRatio * getSizeRatio( font ).mY;
   pSize->mY = getLineCount() * ( mLineHeight + mLineSpacing );
}

