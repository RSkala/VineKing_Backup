//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "console/console.h"
#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/core/guiCanvas.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/controls/guiColorPicker.h"

/// @name Common colors we use
/// @{
ColorF colorWhite(1.,1.,1.);
ColorF colorWhiteBlend(1.,1.,1.,.75);
ColorF colorBlack(.0,.0,.0);
ColorF colorAlpha(0.0f, 0.0f, 0.0f, 0.0f);

ColorI GuiColorPickerCtrl::mColorRange[9] = {
   ColorI(255,255,255), // White
   ColorI(255,255,255), // White
	ColorI(255,0,255),   // Pink
	ColorI(0,0,255),     // Blue
	ColorI(0,255,255),   // Light blue
	ColorI(0,255,0),     // Green
	ColorI(255,255,0),   // Yellow
	ColorI(255,0,0),     // Red
   ColorI(0,0,0)        // Black
};
/// @}

IMPLEMENT_CONOBJECT(GuiColorPickerCtrl);

//--------------------------------------------------------------------------
GuiColorPickerCtrl::GuiColorPickerCtrl()
{
   mBounds.extent.set(140, 30);
   mDisplayMode = pPallet;
   mBaseColor = ColorF(1.,.0,1.);
   mPickColor = ColorF(.0,.0,.0);
   mSelectorPos = Point2I(0,0);
   mMouseDown = mMouseOver = false;
   mActive = true;
   mPositionChanged = false;
   mSelectorGap = 1;
   mActionOnMove = false;
}

//--------------------------------------------------------------------------
static EnumTable::Enums gColorPickerModeEnums[] =
{
   { GuiColorPickerCtrl::pPallet,		"Pallet"   },
   { GuiColorPickerCtrl::pHorizColorRange,	"HorizColor"},
   { GuiColorPickerCtrl::pVertColorRange,	"VertColor" },
   { GuiColorPickerCtrl::pHorizColorBrightnessRange,	"HorizBrightnessColor"},
   { GuiColorPickerCtrl::pVertColorBrightnessRange,	"VertBrightnessColor" },
   { GuiColorPickerCtrl::pBlendColorRange,	"BlendColor"},
   { GuiColorPickerCtrl::pHorizAlphaRange,	"HorizAlpha"},
   { GuiColorPickerCtrl::pVertAlphaRange,	"VertAlpha" },
   { GuiColorPickerCtrl::pDropperBackground,	"Dropper" },
};

static EnumTable gColorPickerModeTable( 9, gColorPickerModeEnums );

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::initPersistFields()
{
   Parent::initPersistFields();
   
   addGroup("ColorPicker");
   addField("BaseColor", TypeColorF, Offset(mBaseColor, GuiColorPickerCtrl));
   addField("PickColor", TypeColorF, Offset(mPickColor, GuiColorPickerCtrl));
   addField("SelectorGap", TypeS32,  Offset(mSelectorGap, GuiColorPickerCtrl)); 
   addField("DisplayMode", TypeEnum, Offset(mDisplayMode, GuiColorPickerCtrl), 1, &gColorPickerModeTable );
   addField("ActionOnMove", TypeBool,Offset(mActionOnMove, GuiColorPickerCtrl));
   endGroup("ColorPicker");
}

//--------------------------------------------------------------------------
// Function to draw a box which can have 4 different colors in each corner blended together
#if defined(TORQUE_OS_IPHONE)
void dglDrawBlendBox(RectI &bounds, ColorF &c1, ColorF &c2, ColorF &c3, ColorF &c4)
{
   S32 left = bounds.point.x, right = bounds.point.x + bounds.extent.x - 1;
   S32 top = bounds.point.y, bottom = bounds.point.y + bounds.extent.y - 1;
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);
	
	const GLfloat verts[] = {
		left, top,
		right,  top,
		left,  bottom,
		right,   bottom,
	};
	const GLubyte squareColors[] = {
		255 * c1.red, 255 * c1.green, 255 * c1.blue, 255 * c1.alpha,
		255 * c2.red, 255 * c2.green, 255 * c2.blue, 255 * c2.alpha,
		255 * c3.red, 255 * c3.green, 255 * c3.blue, 255 * c3.alpha,
		255 * c4.red, 255 * c4.green, 255 * c4.blue, 255 * c4.alpha,
	};
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

//--------------------------------------------------------------------------
/// Function to draw a set of boxes blending throughout an array of colors
void dglDrawBlendRangeBox(RectI &bounds, bool vertical, U8 numColors, ColorI *colors)
{
   S32 left = bounds.point.x, right = bounds.point.x + bounds.extent.x - 1;
   S32 top = bounds.point.y, bottom = bounds.point.y + bounds.extent.y - 1;

   // Calculate increment value
   S32 x_inc = int(mFloor((right - left) / (numColors-1)));
   S32 y_inc = int(mFloor((bottom - top) / (numColors-1)));
   

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

	GLfloat verts[] = {
		0,0,	0,0,	0,0,	0,0,
		0,0,	0,0,	0,0,	0,0,
	};	
	
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glEnableClientState(GL_VERTEX_ARRAY);

	for (U16 i=0;i<numColors-1;i++) 
	{
		// If we are at the end, x_inc and y_inc need to go to the end (otherwise there is a rendering bug)
		if (i == numColors-2) 
		{
            x_inc += right-left-1;
            y_inc += bottom-top-1;
		}
		
		if (vertical)  // Vertical (+y)		colors go up and down
		{
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
			verts[0] = left;
			verts[1] = top;
			verts[2] = left+x_inc;
			verts[3] = top;
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
			
			verts[4] = left;
			verts[5] = bottom;
			verts[6] = left+x_inc;
			verts[7] = bottom;			
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            left += x_inc;
		}
		else  // Horizontal (+x)	colors go lateral
		{
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
			verts[0] = left;
			verts[1] = top+y_inc;
			verts[2] = right;
			verts[3] = top+y_inc;
			
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
			verts[4] = left;
			verts[5] = top;
			verts[6] = right;
			verts[7] = top;			
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            top += y_inc;
        }
	}	
}
//--------------------------------------------------------------------------
/// Function to invoke calls to draw the picker box and selector
void GuiColorPickerCtrl::renderColorBox(RectI &bounds)
{
   RectI pickerBounds;
   pickerBounds.point.x = bounds.point.x+1;
   pickerBounds.point.y = bounds.point.y+1;
   pickerBounds.extent.x = bounds.extent.x-1;
   pickerBounds.extent.y = bounds.extent.y-1;
   
   if (mProfile->mBorder)
      dglDrawRect(bounds, mProfile->mBorderColor);
      
   Point2I selectorPos = Point2I(bounds.point.x+mSelectorPos.x+1, bounds.point.y+mSelectorPos.y+1);

   // Draw color box differently depending on mode
   RectI blendRect;
   switch (mDisplayMode)
   {
   case pHorizColorRange:
      dglDrawBlendRangeBox( pickerBounds, false, 7, mColorRange + 2);
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertColorRange:
      dglDrawBlendRangeBox( pickerBounds, true, 7, mColorRange + 2);
      drawSelector( pickerBounds, selectorPos, sHorizontal );
   break;
   case pHorizColorBrightnessRange:
      blendRect = pickerBounds;
      blendRect.point.y++;
      blendRect.extent.y += 2;
      dglDrawBlendRangeBox( pickerBounds, false, 9, mColorRange);
      // This is being drawn slightly offset from the larger rect so as to insure 255 and 0
      // can both be selected for every color.
      dglDrawBlendBox( blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack );
      blendRect.point.y += blendRect.extent.y - 1;
      //blendRect.extent.y = 2;
      dglDrawRect( blendRect, colorBlack);
      drawSelector( pickerBounds, selectorPos, sVertical );
	  drawSelector( pickerBounds, selectorPos, sHorizontal );
   break;
   case pVertColorBrightnessRange:
      dglDrawBlendRangeBox( pickerBounds, true, 9, mColorRange);
      dglDrawBlendBox( pickerBounds, colorAlpha, colorAlpha, colorBlack, colorBlack );
      drawSelector( pickerBounds, selectorPos, sHorizontal );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pHorizAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorWhite, colorBlack, colorWhite );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorBlack, colorWhite, colorWhite );
      drawSelector( pickerBounds, selectorPos, sHorizontal ); 
   break;
   case pBlendColorRange:
      dglDrawBlendBox( pickerBounds, colorWhite, mBaseColor, colorBlack, colorBlack );
      drawSelector( pickerBounds, selectorPos, sHorizontal );      
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pDropperBackground:
   break;
   case pPallet:
   default:
      dglDrawRectFill( pickerBounds, mBaseColor );
   break;
   }
}

#else

void dglDrawBlendBox(RectI &bounds, ColorF &c1, ColorF &c2, ColorF &c3, ColorF &c4)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
      // Make sure the top row of pixels are the right color.
      glColor4f(c1.red, c1.green, c1.blue, c1.alpha);
      glVertex2f(l, t);
      glColor4f(c2.red, c2.green, c2.blue, c2.alpha);
      glVertex2f(r, t);
      glColor4f(c2.red, c2.green, c2.blue, c2.alpha);
      glVertex2f(r, t + 1);
      glColor4f(c1.red, c1.green, c1.blue, c1.alpha);
      glVertex2f(l, t + 1);

      // The main blend box.
      glColor4f(c1.red, c1.green, c1.blue, c1.alpha);
      glVertex2f(l, t + 1);
      glColor4f(c2.red, c2.green, c2.blue, c2.alpha);
      glVertex2f(r, t + 1);
      glColor4f(c3.red, c3.green, c3.blue, c3.alpha);
      glVertex2f(r, b - 1);
      glColor4f(c4.red, c4.green, c4.blue, c4.alpha);
      glVertex2f(l, b - 1);

      // Make sure the bottom row of pixels are the right color.
      glColor4f(c4.red, c4.green, c4.blue, c4.alpha);
      glVertex2f(l, b - 1);
      glColor4f(c3.red, c3.green, c3.blue, c3.alpha);
      glVertex2f(r, b - 1);
      glColor4f(c3.red, c3.green, c3.blue, c3.alpha);
      glVertex2f(r, b);
      glColor4f(c4.red, c4.green, c4.blue, c4.alpha);
      glVertex2f(l, b);
   glEnd();
}

//--------------------------------------------------------------------------
/// Function to draw a set of boxes blending throughout an array of colors
void dglDrawBlendRangeBox(RectI &bounds, bool vertical, U8 numColors, ColorI *colors)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;
   
   // Calculate increment value
   S32 x_inc = int(mFloor((r - l) / (numColors-1)));
   S32 y_inc = int(mFloor((b - t) / (numColors-1)));
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
      for (U16 i=0;i<numColors-1;i++) 
      {
         // If we are at the end, x_inc and y_inc need to go to the end (otherwise there is a rendering bug)
         if (i == numColors-2) 
         {
            x_inc += r-l-1;
            y_inc += b-t-1;
         }
         if (!vertical)  // Horizontal (+x)
         {
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
            glVertex2f(l, t);
            glVertex2f(l, b);
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
            glVertex2f(l+x_inc, b);
            glVertex2f(l+x_inc, t);
            l += x_inc;
         }
         else  // Vertical (+y)
         {
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
            glVertex2f(l, t);
            glVertex2f(r, t);
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
            glVertex2f(r, t+y_inc);
            glVertex2f(l, t+y_inc);
            t += y_inc;
        }
     }
   glEnd();
}

//--------------------------------------------------------------------------
/// Function to invoke calls to draw the picker box and selector
void GuiColorPickerCtrl::renderColorBox(RectI &bounds)
{
   RectI pickerBounds;
   pickerBounds.point.x = bounds.point.x+1;
   pickerBounds.point.y = bounds.point.y+1;
   pickerBounds.extent.x = bounds.extent.x-1;
   pickerBounds.extent.y = bounds.extent.y-1;
   
   if (mProfile->mBorder)
      dglDrawRect(bounds, mProfile->mBorderColor);
      
   Point2I selectorPos = Point2I(bounds.point.x+mSelectorPos.x+1, bounds.point.y+mSelectorPos.y+1);

   // Draw color box differently depending on mode
   RectI blendRect;
   switch (mDisplayMode)
   {
   case pHorizColorRange:
      dglDrawBlendRangeBox( pickerBounds, false, 7, mColorRange + 2);
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertColorRange:
      dglDrawBlendRangeBox( pickerBounds, true, 7, mColorRange + 2);
      drawSelector( pickerBounds, selectorPos, sHorizontal );
   break;
   case pHorizColorBrightnessRange:
      blendRect = pickerBounds;
      blendRect.point.y++;
      blendRect.extent.y -= 2;
      dglDrawBlendRangeBox( pickerBounds, false, 9, mColorRange);
      // This is being drawn slightly offset from the larger rect so as to insure 255 and 0
      // can both be selected for every color.
      dglDrawBlendBox( blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack );
      blendRect.point.y += blendRect.extent.y - 1;
      blendRect.extent.y = 2;
      dglDrawRect( blendRect, colorBlack);
      drawSelector( pickerBounds, selectorPos, sHorizontal );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertColorBrightnessRange:
      dglDrawBlendRangeBox( pickerBounds, true, 9, mColorRange);
      dglDrawBlendBox( pickerBounds, colorAlpha, colorBlack, colorBlack, colorAlpha );
      drawSelector( pickerBounds, selectorPos, sHorizontal );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pHorizAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorWhite, colorWhite, colorBlack );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorBlack, colorWhite, colorWhite );
      drawSelector( pickerBounds, selectorPos, sHorizontal ); 
   break;
   case pBlendColorRange:
      dglDrawBlendBox( pickerBounds, colorWhite, mBaseColor, colorBlack, colorBlack );
      drawSelector( pickerBounds, selectorPos, sHorizontal );      
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pDropperBackground:
   break;
   case pPallet:
   default:
      dglDrawRectFill( pickerBounds, mBaseColor );
   break;
   }
}
#endif

void GuiColorPickerCtrl::drawSelector(RectI &bounds, Point2I &selectorPos, SelectorMode mode)
{
	U16 sMax = mSelectorGap*2;
	switch (mode)
	{
		case sVertical:
			// Now draw the vertical selector
			// Up -> Pos
			if (selectorPos.y != bounds.point.y+1)
				dglDrawLine(selectorPos.x, bounds.point.y, selectorPos.x, selectorPos.y-sMax-1, colorWhiteBlend);
			// Down -> Pos
			if (selectorPos.y != bounds.point.y+bounds.extent.y) 
				dglDrawLine(selectorPos.x,	selectorPos.y + sMax, selectorPos.x, bounds.point.y + bounds.extent.y, colorWhiteBlend);
		break;
		case sHorizontal:
			// Now draw the horizontal selector
			// Left -> Pos
			if (selectorPos.x != bounds.point.x) 
            dglDrawLine(bounds.point.x, selectorPos.y-1, selectorPos.x-sMax, selectorPos.y-1, colorWhiteBlend);
			// Right -> Pos
			if (selectorPos.x != bounds.point.x) 
            dglDrawLine(bounds.point.x+mSelectorPos.x+sMax, selectorPos.y-1, bounds.point.x + bounds.extent.x, selectorPos.y-1, colorWhiteBlend);
		break;
	}
}

void GuiColorPickerCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   RectI boundsRect(offset, mBounds.extent); 
   renderColorBox(boundsRect);

   if (mPositionChanged) 
   {
      mPositionChanged = false;
      Point2I extent = Canvas->getExtent();
      // If we are anything but a pallete, change the pick color
      if (mDisplayMode != pPallet) 
      {
         // To simplify things a bit, just read the color via glReadPixels()

         // [neo, 5/30/2007 - #3175]
         // glReadPixels does not even look at the format passed in, it just assumes 
         // GL_RGBA and a dword buffer, so we  can't use GL_FLOAT, etc. glReadPixels
         // was borked in D3D and not calculating the correct offset rectangle etc 
         // so I changed it to match the OGL version functionality exactly.
         // [see glReadPixels in OpenGL2D3C.cc]
         
         // glFloat rBuffer[4];
         //glReadPixels( buf_x, buf_y, 1, 1, GL_RGBA, GL_FLOAT, rBuffer);
         //mPickColor.red = rBuffer[0];
         //mPickColor.green = rBuffer[1];
         //mPickColor.blue = rBuffer[2];
         //mPickColor.alpha = rBuffer[3]; 

         GLubyte rBuffer[4] = { 255, 255, 255, 255 };

         glReadBuffer( GL_BACK );

         U32 buf_x = offset.x + mSelectorPos.x + 1;
         U32 buf_y = extent.y - ( offset.y + mSelectorPos.y + 1 );
                  
         glReadPixels( buf_x, buf_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rBuffer );
      
         mPickColor = ColorI( rBuffer[ 0 ], rBuffer[ 1 ], rBuffer[ 2 ], 255 );

         // Now do onAction() if we are allowed
         if (mActionOnMove) 
            onAction();
      }
   }
   
   //render the children
   renderChildControls( offset, updateRect);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::setSelectorPos(const Point2I &pos)
{
   Point2I extent = mBounds.extent;
   RectI rect;
   if (mDisplayMode != pDropperBackground) 
   {
      extent.x -= 3;
      extent.y -= 2;
      rect = RectI(Point2I(1,1), extent);
   }
   else
   {
      rect = RectI(Point2I(0,0), extent);
   }
   
   if (rect.pointInRect(pos)) 
   {
      mSelectorPos = pos;
      mPositionChanged = true;
      // We now need to update
      setUpdate();
   }

   else
   {
      if ((pos.x > rect.point.x) && (pos.x < (rect.point.x + rect.extent.x)))
         mSelectorPos.x = pos.x;
      else if (pos.x <= rect.point.x)
         mSelectorPos.x = rect.point.x;
      else if (pos.x >= (rect.point.x + rect.extent.x))
         mSelectorPos.x = rect.point.x + rect.extent.x - 1;

      if ((pos.y > rect.point.y) && (pos.y < (rect.point.y + rect.extent.y)))
         mSelectorPos.y = pos.y;
      else if (pos.y <= rect.point.y)
         mSelectorPos.y = rect.point.y;
      else if (pos.y >= (rect.point.y + rect.extent.y))
         mSelectorPos.y = rect.point.y + rect.extent.y - 1;

      mPositionChanged = true;
      setUpdate();
   }
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseDown(const GuiEvent &event)
{
   if (!mActive)
      return;
   
   if (mDisplayMode == pDropperBackground)
      return;

   mouseLock(this);
   
   if (mProfile->mCanKeyFocus)
      setFirstResponder();

   // Update the picker cross position
   if (mDisplayMode != pPallet)
      setSelectorPos(globalToLocalCoord(event.mousePoint)); 
   
   mMouseDown = true;
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseDragged(const GuiEvent &event)
{
   if ((mActive && mMouseDown) || (mActive && (mDisplayMode == pDropperBackground)))
   {
      // Update the picker cross position
      if (mDisplayMode != pPallet)
         setSelectorPos(globalToLocalCoord(event.mousePoint));
   }

   if (!mActionOnMove && mAltConsoleCommand[0] )
      Con::evaluate( mAltConsoleCommand, false );

}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseMove(const GuiEvent &event)
{
   // Only for dropper mode
   if (mActive && (mDisplayMode == pDropperBackground))
      setSelectorPos(globalToLocalCoord(event.mousePoint));
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseEnter(const GuiEvent &event)
{
   mMouseOver = true;
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseLeave(const GuiEvent &)
{
   // Reset state
   mMouseOver = false;
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onMouseUp(const GuiEvent &)
{
   //if we released the mouse within this control, perform the action
   if (mActive && mMouseDown && (mDisplayMode != pDropperBackground)) 
   {
      onAction();
      mMouseDown = false;
   }
   else if (mActive && (mDisplayMode == pDropperBackground)) 
   {
      // In a dropper, the alt command executes the mouse up action (to signal stopping)
      if ( mAltConsoleCommand[0] )
         Con::evaluate( mAltConsoleCommand, false );
   }
   
   mouseUnlock();
}

//--------------------------------------------------------------------------
const char *GuiColorPickerCtrl::getScriptValue()
{
   static char temp[256];
   ColorF color = getValue();
   dSprintf(temp,256,"%f %f %f %f",color.red, color.green, color.blue, color.alpha);
   return temp; 
}

//--------------------------------------------------------------------------    
void GuiColorPickerCtrl::setScriptValue(const char *value)
{
   ColorF newValue;
   dSscanf(value, "%f %f %f %f", &newValue.red, &newValue.green, &newValue.blue, &newValue.alpha);
   setValue(newValue);
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPos, const char*, 2, 2, "() Gets the current position of the selector\n"
			  "@return Returns the position of the selector as space-separted x,y coordinates.")
{
   char *temp = Con::getReturnBuffer(256);
   Point2I pos;
   pos = object->getSelectorPos();
   dSprintf(temp,256,"%d %d",pos.x, pos.y); 
   return temp;
}

ConsoleMethod(GuiColorPickerCtrl, setSelectorPos, void, 3, 3, "(\"x y\")Sets the current position of the selector"
			  "@param The coordinates with space-separated formating.\n"
			  "@return No return value.")
{
   Point2I newPos;
   dSscanf(argv[2], "%d %d", &newPos.x, &newPos.y);
   object->setSelectorPos(newPos);
}

ConsoleMethod(GuiColorPickerCtrl, updateColor, void, 2, 2, "() Forces update of pick color\n"
			  "@return No return value.")
{
	object->updateColor();
}
