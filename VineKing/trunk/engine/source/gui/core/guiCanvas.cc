//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "core/torqueConfig.h"
#include "platform/profiler.h"
#include "dgl/dgl.h"
#include "platform/event.h"
#include "platform/platform.h"
#include "platform/platformVideo.h"
#include "gui/core/guiTypes.h"
#include "gui/core/guiControl.h"
#include "gui/core/guiCanvas.h"
#include "dgl/gDynamicTexture.h"
#include "platform/gameInterface.h"

#ifdef TGB_TRIAL
#include _WPB(INCLUDE1)
#include _WPB(INCLUDE2)
#endif

extern bool gDGLRender;

// We formerly kept all the GUI related IMPLEMENT_CONOBJECT macros here.
// Now they belong with their implementations. -- BJG

IMPLEMENT_CONOBJECT(GuiCanvas);

GuiCanvas *Canvas = NULL;

ConsoleMethod( GuiCanvas, getContent, S32, 2, 2, "() Use the getContent method to get the ID of the control which is being used as the current canvas content.\n"
																"@return Returns the ID of the current canvas content (a control), or 0 meaning the canvas is empty")
{
   GuiControl *ctrl = object->getContentControl();
   if(ctrl)
      return ctrl->getId();
   return -1;
}

ConsoleMethod( GuiCanvas, setContent, void, 3, 3, "( handle ) Use the setContent method to set the control identified by handle as the current canvas content.\n"
																"@param handle The numeric ID or name of the control to be made the canvas contents.\n"
																"@return No return value")
{
   object;
   argc;

   GuiControl *gui = NULL;
   if(argv[2][0])
   {
      if (!Sim::findObject(argv[2], gui))
      {
         Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
         return;
      }
   }

   //set the new content control
   Canvas->setContentControl(gui);
}

ConsoleMethod( GuiCanvas, pushDialog, void, 3, 4, "( handle [ , layer ] ) Use the pushDialog method to open a dialog on a specific canvas layer, or in the same layer the last openned dialog. Newly placed dialogs placed in a layer with another dialog(s) will overlap the prior dialog(s).\n"
																"@param handle The numeric ID or name of the dialog to be opened.\n"
																"@param layer A integer value in the range [ 0 , inf ) specifying the canvas layer to place the dialog in.\n"
																"@return No return value")
{
   object;

   GuiControl *gui;

   if (!	Sim::findObject(argv[2], gui))
   {
      Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
      return;
   }

   //find the layer
   S32 layer = 0;
   if (argc == 4)
      layer = dAtoi(argv[3]);

   //set the new content control
   Canvas->pushDialogControl(gui, layer);
}

ConsoleMethod( GuiCanvas, popDialog, void, 2, 3, "( handle ) Use the popDialog method to remove a currently showing dialog. If no handle is provided, the top most dialog is popped.\n"
																"@param handle The ID or a previously pushed dialog.\n"
																"@return No return value.\n"
																"@sa pushDialog, popLayer")
{
   object;

   GuiControl *gui;
   if (argc == 3)
   {
      if (!Sim::findObject(argv[2], gui))
      {
         Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
         return;
      }
   }

   if (gui)
      Canvas->popDialogControl(gui);
   else
      Canvas->popDialogControl();
}

ConsoleMethod( GuiCanvas, popLayer, void, 2, 3, "( layer ) Use the popLayer method to remove (close) all dialogs in the specified canvas �layer�.\n"
																"@param layer A integer value in the range [ 0 , inf ) specifying the canvas layer to clear.\n"
																"@return No return value.\n"
																"@sa pushDialog, popDialog")
{
   object;

   S32 layer = 0;
   if (argc == 3)
      layer = dAtoi(argv[2]);

   Canvas->popDialogControl(layer);
}

ConsoleMethod(GuiCanvas, cursorOn, void, 2, 2, "() Use the cursorOn method to enable the cursor.\n"
																"@return No return value")
{
   object;
   argc;
   argv;
   Canvas->setCursorON(true);
}

ConsoleMethod(GuiCanvas, cursorOff, void, 2, 2, "() Use the cursorOff method to disable the cursor.\n"
																"@return No return value")
{
   object;
   argc;
   argv;
   Canvas->setCursorON(false);
}

ConsoleMethod( GuiCanvas, setCursor, void, 3, 3, "( cursorHandle ) Use the setCursor method to select the current cursor.\n"
																"@param cursorHandle The ID of a previously defined GuiCursor object.\n"
																"@return No return value")
{
   object;
   argc;

   GuiCursor *curs = NULL;
   if(argv[2][0])
   {
      if(!Sim::findObject(argv[2], curs))
      {
         Con::printf("%s is not a valid cursor.", argv[2]);
         return;
      }
   }
   Canvas->setCursor(curs);
}

ConsoleMethod( GuiCanvas, renderFront, void, 3, 3, "(bool enable)")
{
   Canvas->setRenderFront(dAtob(argv[2]));
}

ConsoleMethod( GuiCanvas, showCursor, void, 2, 2, "")
{
   Canvas->showCursor(true);
}

ConsoleMethod( GuiCanvas, hideCursor, void, 2, 2, "")
{
   Canvas->showCursor(false);
}

ConsoleMethod( GuiCanvas, isCursorOn, bool, 2, 2, "")
{
   return Canvas->isCursorON();
}

ConsoleMethod( GuiCanvas, setDoubleClickDelay, void, 3, 3, "")
{
   Canvas->setDoubleClickTime(dAtoi(argv[2]));
}

ConsoleMethod( GuiCanvas, setDoubleClickMoveBuffer, void, 4, 4, "")
{
   Canvas->setDoubleClickWidth(dAtof(argv[2]));
   Canvas->setDoubleClickHeight(dAtof(argv[3]));
}

ConsoleMethod( GuiCanvas, repaint, void, 2, 2, "() Use the repaint method to force the canvas to redraw all elements.\n"
																"@return No return value")
{
   Canvas->paint();
}

ConsoleMethod( GuiCanvas, reset, void, 2, 2, "() Use the reset method to reset the current canvas update region.\n"
																"@return No return value")
{
   Canvas->resetUpdateRegions();
}

ConsoleMethod( GuiCanvas, getCursorPos, const char*, 2, 2, "() Use the getCursorPos method to retrieve the current position of the mouse pointer.\n"
																"@return Returns a vector containing the �x y� coordinates of the cursor in the canvas")
{
   Point2I pos = Canvas->getCursorPos();
   char * ret = Con::getReturnBuffer(32);
   dSprintf(ret, 32, "%d %d", pos.x, pos.y);
   return(ret);
}

ConsoleMethod( GuiCanvas, setCursorPos, void, 3, 4, "( ) Use the setCursorPos method to set the position of the cursor in the cavas.\n"
																"@param position An \"x y\" position vector specifying the new location of the cursor.\n"
																"@return No return value")
{
   Point2I pos(0,0);

   if(argc == 4)
      pos.set(dAtoi(argv[2]), dAtoi(argv[3]));
   else
      dSscanf(argv[2], "%d %d", &pos.x, &pos.y);

   Canvas->setCursorPos(pos);
}

ConsoleMethod( GuiCanvas, getMouseControl, S32, 2, 2, "Gets the gui control under the mouse.")
{
   GuiControl* control = object->getMouseControl();
   if (control)
      return control->getId();
   
   return NULL;
}

ConsoleFunction( createCanvas, bool, 2, 2, "( WindowTitle ) Use the createCanvas function to initialize the canvas.\n"
																"@return Returns true on success, false on failure.\n"
																"@sa createEffectCanvas")
{
   AssertISV(!Canvas, "CreateCanvas: canvas has already been instantiated");

#ifndef TORQUE_PLAYER
#if !defined(TORQUE_OS_MAC) // macs can only run one instance in general.
#if !defined(TORQUE_DEBUG) && !defined(INTERNAL_RELEASE)
   if(!Platform::excludeOtherInstances(TORQUE_INSTANCE_EXCLUSION))
      return false;
#endif
#endif
#endif
#ifdef TORQUE_OS_IPHONE
	
	//FFS, PUAP.
	//First fetch the values from the prefs.
	S32 iDeviceType = -1;
	S32 iDeviceRes = -1;
	S32 iDeviceOrientation = -1;
	
	iDeviceType = Con::getIntVariable("$pref::iDevice::DeviceType");
	iDeviceRes = Con::getIntVariable("$pref::iDevice::ScreenResolution");
	iDeviceOrientation = Con::getIntVariable("$pref::iDevice::ScreenOrientation");
	
	// UNUSED: JOSEPH THOMAS -> S32 i_IPHONE = 0;
	S32 i_IPAD = 1;
	S32 i_LANDSCAPE = 0;
	// UNUSED: JOSEPH THOMAS -> S32 i_PORTRAIT = 1;
	S32 i_FULLRES = 0;
	// UNUSED: JOSEPH THOMAS -> S32 i_SMALLRES = 1;
	
	S32 i_SMALLW = 480;
	S32 i_SMALLH = 320;
	S32 i_FULLW = 1024;
	S32 i_FULLH = 768;
	
	Point2I startRes = Point2I(IPHONE_DEFAULT_RESOLUTION_X, IPHONE_DEFAULT_RESOLUTION_Y);
	
	if(iDeviceType == i_IPAD)
	{
		if(iDeviceRes == i_FULLRES)
		{
			if (iDeviceOrientation == i_LANDSCAPE) 
			{
				startRes.x = i_FULLW;
				startRes.y = i_FULLH;
			}
			else 
			{
				//portrait, swap width height.
				startRes.x = i_FULLH;
				startRes.y = i_FULLW;
			}

		}
		else 
		{
			if (iDeviceOrientation == i_LANDSCAPE) 
			{
				startRes.x = i_SMALLW;
				startRes.y = i_SMALLH;
			}
			else 
			{
				//portrait, swap width height.
				startRes.x = i_SMALLH;
				startRes.y = i_SMALLW;
			}
		}

	}
	else
	{
		if (iDeviceOrientation == i_LANDSCAPE) 
		{
			startRes.x = i_SMALLW;
			startRes.y = i_SMALLH;
		}
		else 
		{
			//portrait, swap width height.
			startRes.x = i_SMALLH;
			startRes.y = i_SMALLW;
		}
	}

   Platform::initWindow(startRes, argv[1]);
#else
	Platform::initWindow(Point2I(MIN_RESOLUTION_X, MIN_RESOLUTION_Y), argv[1]);
#endif

   if (!Video::getResolutionList())
      return false;

   // create the canvas, and add it to the manager
   Canvas = new GuiCanvas();
   Canvas->registerObject("Canvas"); // automatically adds to GuiGroup
   return true;
}

ConsoleFunction( setCanvasTitle, void, 2, 2, "(string windowTitle) Sets the title to the provided string\n" 
				"@param windowTitle The desired title\n"
				"@return No Return Value")
{
   Platform::setWindowTitle( argv[1] );
}

GuiCanvas::GuiCanvas()
{
#ifdef TORQUE_OS_IPHONE
   mBounds.set(0, 0, IPHONE_DEFAULT_RESOLUTION_X, IPHONE_DEFAULT_RESOLUTION_Y);
#else
   mBounds.set(0, 0, MIN_RESOLUTION_X, MIN_RESOLUTION_Y);
#endif
	
   mAwake = true;
   mPixelsPerMickey = 1.0f;

#ifndef TORQUE_TOOLS
   cursorON    = true;
   mShowCursor = true;   
#else
   cursorON    = true;
   mShowCursor = false;
#endif
   lastCursorON = false;
   rLastFrameTime = 0.0f;

   mMouseCapturedControl = NULL;
   mMouseControl = NULL;
   mMouseControlClicked = false;
   mMouseButtonDown = false;
   mMouseRightButtonDown = false;
   mMouseMiddleButtonDown = false;

   lastCursor = NULL;
   lastCursorPt.set(0,0);
   cursorPt.set(0,0);

	mLastMouseClickCount = 0;
	mLastMouseDownTime = 0;
	mPrevMouseTime = 0;
	defaultCursor = NULL;

   mRenderFront = false;

   hoverControlStart = Platform::getRealMilliseconds();
   hoverControl = NULL;
   hoverPosition = getCursorPos();
   hoverPositionSet = false;
   hoverLeftControlTime = 0;

   mLeftMouseLast = false;
   mMiddleMouseLast = false;
   mRightMouseLast = false;

   mDoubleClickWidth = Input::getDoubleClickWidth();
   mDoubleClickHeight = Input::getDoubleClickHeight();
   mDoubleClickTime = Input::getDoubleClickTime();

   _WPB(INIT);
}

GuiCanvas::~GuiCanvas()
{
   _WPB(SHUTDOWN);

   if(Canvas == this)
      Canvas = 0;
}

//------------------------------------------------------------------------------
void GuiCanvas::setCursor(GuiCursor *curs)
{
   defaultCursor = curs;

   if(mShowCursor)
   {
       Input::setCursorState(false);
   }
}

void GuiCanvas::setCursorON(bool onOff)
{
   cursorON = onOff;
   if(!cursorON)
      mMouseControl = NULL;
}


void GuiCanvas::setCursorPos(const Point2I &pt)   
{ 
   cursorPt.x = F32(pt.x); 
   cursorPt.y = F32(pt.y); 

   Input::setCursorPos( pt.x, pt.y );
}

void GuiCanvas::addAcceleratorKey(GuiControl *ctrl, U32 index, U32 keyCode, U32 modifier)
{
   if (keyCode > 0 && ctrl)
   {
      AccKeyMap newMap;
      newMap.ctrl = ctrl;
      newMap.index = index;
      newMap.keyCode = keyCode;
      newMap.modifier = modifier;
      mAcceleratorMap.push_back(newMap);
   }
}

bool GuiCanvas::tabNext(void)
{
   GuiControl *ctrl = static_cast<GuiControl *>(last());
   if (ctrl)
   {
      //save the old
      GuiControl *oldResponder = mFirstResponder;

		GuiControl* newResponder = ctrl->findNextTabable(mFirstResponder);
      if ( !newResponder )
         newResponder = ctrl->findFirstTabable();

		if ( newResponder && newResponder != oldResponder )
		{
			newResponder->setFirstResponder();
//      	if ( oldResponder )
//         	oldResponder->onLoseFirstResponder();
         return true;
		}
   }
   return false;
}

bool GuiCanvas::tabPrev(void)
{
   GuiControl *ctrl = static_cast<GuiControl *>(last());
   if (ctrl)
   {
      //save the old
      GuiControl *oldResponder = mFirstResponder;

		GuiControl* newResponder = ctrl->findPrevTabable(mFirstResponder);
		if ( !newResponder )
         newResponder = ctrl->findLastTabable();

		if ( newResponder && newResponder != oldResponder )
		{
			newResponder->setFirstResponder();
	
	      if ( oldResponder )
	         oldResponder->onLoseFirstResponder();

         return true;
		}
   }
   return false;
}

void GuiCanvas::processScreenTouchEvent(const ScreenTouchEvent *event)
{
	//copy the cursor point into the event
	mLastEvent.mousePoint.x = S32(event->xPos);
	mLastEvent.mousePoint.y = S32(event->yPos);

	//see if button was pressed
	if (event->action == SI_MAKE)
	{
		U32 curTime = Platform::getVirtualMilliseconds();
		mNextMouseTime = curTime + mInitialMouseDelay;
		
		mLastMouseDownTime = curTime;
		mLastEvent.mouseClickCount = mLastMouseClickCount;
		
		rootScreenTouchDown(mLastEvent);
	}
	//else button was released
	else
	{
		mNextMouseTime = 0xFFFFFFFF;
		rootScreenTouchUp(mLastEvent);
	}
}

void GuiCanvas::processMouseMoveEvent(const MouseMoveEvent *event)
{
   if( cursorON )
   {
		//copy the modifier into the new event
		mLastEvent.modifier = event->modifier;

      cursorPt.x += ( F32(event->xPos - cursorPt.x) * mPixelsPerMickey);
      cursorPt.y += ( F32(event->yPos - cursorPt.y) * mPixelsPerMickey);

      // clamp the cursor to the window, or not
      if( ! Con::getBoolVariable( "$pref::Gui::noClampTorqueCursorToWindow", true ))
      {
         cursorPt.x = getMax(0, getMin((S32)cursorPt.x, mBounds.extent.x - 1));
         cursorPt.y = getMax(0, getMin((S32)cursorPt.y, mBounds.extent.y - 1));
      }
      
		mLastEvent.mousePoint.x = S32(cursorPt.x);
		mLastEvent.mousePoint.y = S32(cursorPt.y);

      Point2F movement = mMouseDownPoint - cursorPt;
      if ((mAbs((S32)movement.x) > mDoubleClickWidth) || (mAbs((S32)movement.y) > mDoubleClickHeight))
      {
         mLeftMouseLast = false;
         mMiddleMouseLast = false;
         mRightMouseLast = false;
      }

		if (mMouseButtonDown)
			rootMouseDragged(mLastEvent);
		else if (mMouseRightButtonDown)
			rootRightMouseDragged(mLastEvent);
		else if(mMouseMiddleButtonDown)
			rootMiddleMouseDragged(mLastEvent);
		else
			rootMouseMove(mLastEvent);
	}
}

bool GuiCanvas::processInputEvent(const InputEvent *event)
{
	// First call the general input handler (on the extremely off-chance that it will be handled):
	if ( mFirstResponder )
   {
      if ( mFirstResponder->onInputEvent( *event ) )
		   return( true );
   }

   if(event->deviceType == KeyboardDeviceType)
   {
      mLastEvent.ascii = event->ascii;
      mLastEvent.modifier = event->modifier;
      mLastEvent.keyCode = event->objInst;

      U32 eventModifier = event->modifier;
      if(eventModifier & SI_SHIFT)
         eventModifier |= SI_SHIFT;
      if(eventModifier & SI_CTRL)
         eventModifier |= SI_CTRL;
      if(eventModifier & SI_ALT)
         eventModifier |= SI_ALT;

      if (event->action == SI_MAKE)
      {
         //see if we should tab next/prev

         //see if we should now pass the event to the first responder
         if (mFirstResponder)
         {
            if(mFirstResponder->onKeyDown(mLastEvent))
               return true;
         }

         if ( isCursorON() && ( event->objInst == KEY_TAB ) )
         {
            if (size() > 0)
            {
               if (event->modifier & SI_SHIFT)
               {
                  if(tabPrev())
                     return true;
               }
               else if (event->modifier == 0)
               {
                  if(tabNext())
                     return true;
               }
            }
         }

         //if not handled, search for an accelerator
         for (U32 i = 0; i < mAcceleratorMap.size(); i++)
         {
            if ((U32)mAcceleratorMap[i].keyCode == (U32)event->objInst && (U32)mAcceleratorMap[i].modifier == eventModifier)
            {
               mAcceleratorMap[i].ctrl->acceleratorKeyPress(mAcceleratorMap[i].index);
               return true;
            }
         }
      }
      else if(event->action == SI_BREAK)
      {
         if(mFirstResponder)
            if(mFirstResponder->onKeyUp(mLastEvent))
               return true;

         //see if there's an accelerator
         for (U32 i = 0; i < mAcceleratorMap.size(); i++)
         {
            if ((U32)mAcceleratorMap[i].keyCode == (U32)event->objInst && (U32)mAcceleratorMap[i].modifier == eventModifier)
            {
               mAcceleratorMap[i].ctrl->acceleratorKeyRelease(mAcceleratorMap[i].index);
               return true;
            }
         }
      }
      else if(event->action == SI_REPEAT)
      {

         //if not handled, search for an accelerator
         for (U32 i = 0; i < mAcceleratorMap.size(); i++)
         {
            if ((U32)mAcceleratorMap[i].keyCode == (U32)event->objInst && (U32)mAcceleratorMap[i].modifier == eventModifier)
            {
               mAcceleratorMap[i].ctrl->acceleratorKeyPress(mAcceleratorMap[i].index);
               return true;
            }
         }

         if(mFirstResponder)
            mFirstResponder->onKeyRepeat(mLastEvent);
         return true;
      }
   }
   else if(event->deviceType == MouseDeviceType && cursorON)
   {
      //copy the modifier into the new event
      mLastEvent.modifier = event->modifier;

      if(event->objType == SI_XAXIS || event->objType == SI_YAXIS)
      {
         bool moved = false;
         Point2I oldpt((S32)cursorPt.x, (S32)cursorPt.y);
         Point2F pt(cursorPt.x, cursorPt.y);

         if (event->objType == SI_XAXIS)
         {
            pt.x += (event->fValue * mPixelsPerMickey);
            cursorPt.x = getMax(0, getMin((S32)pt.x, mBounds.extent.x - 1));
            if (oldpt.x != S32(cursorPt.x))
               moved = true;
         }
         else
         {
            pt.y += (event->fValue * mPixelsPerMickey);
            cursorPt.y = getMax(0, getMin((S32)pt.y, mBounds.extent.y - 1));
            if (oldpt.y != S32(cursorPt.y))
               moved = true;
         }
         if (moved)
         {
            mLastEvent.mousePoint.x = S32(cursorPt.x);
            mLastEvent.mousePoint.y = S32(cursorPt.y);

#ifdef	TORQUE_ALLOW_JOURNALING
            // [tom, 9/8/2006] If we're journaling, we need to update the plat cursor
            if(Game->isJournalReading())
               Input::setCursorPos(cursorPt.x, cursorPt.y);
#endif	//TORQUE_ALLOW_JOURNALING

            if (mMouseButtonDown)
               rootMouseDragged(mLastEvent);
            else if (mMouseRightButtonDown)
               rootRightMouseDragged(mLastEvent);
            else if(mMouseMiddleButtonDown)
               rootMiddleMouseDragged(mLastEvent);
            else
               rootMouseMove(mLastEvent);
         }
         return true;
      }
		else if ( event->objType == SI_ZAXIS )
		{
         mLastEvent.mousePoint.x = S32( cursorPt.x );
         mLastEvent.mousePoint.y = S32( cursorPt.y );

			if ( event->fValue < 0.0f )
            rootMouseWheelDown( mLastEvent );
			else
            rootMouseWheelUp( mLastEvent );
      }
      else if(event->objType == SI_BUTTON)
      {
         //copy the cursor point into the event
         mLastEvent.mousePoint.x = S32(cursorPt.x);
         mLastEvent.mousePoint.y = S32(cursorPt.y);
         mMouseDownPoint = cursorPt;

         if(event->objInst == KEY_BUTTON0) // left button
         {
            //see if button was pressed
            if (event->action == SI_MAKE)
            {
               U32 curTime = Platform::getVirtualMilliseconds();
               mNextMouseTime = curTime + mInitialMouseDelay;

               //if the last button pressed was the left...
               if (mLeftMouseLast)
               {
                  //if it was within the double click time count the clicks
                  if (curTime - mLastMouseDownTime <= mDoubleClickTime)
                     mLastMouseClickCount++;
                  else
                     mLastMouseClickCount = 1;
               }
               else
               {
                  mLeftMouseLast = true;
                  mLastMouseClickCount = 1;
               }

               mLastMouseDownTime = curTime;
               mLastEvent.mouseClickCount = mLastMouseClickCount;

               rootMouseDown(mLastEvent);
            }
            //else button was released
            else
            {
               mNextMouseTime = 0xFFFFFFFF;
               rootMouseUp(mLastEvent);
            }
            return true;
         }
         else if(event->objInst == KEY_BUTTON1) // right button
         {
            if(event->action == SI_MAKE)
            {
               U32 curTime = Platform::getVirtualMilliseconds();

               //if the last button pressed was the right...
               if (mRightMouseLast)
               {
                  //if it was within the double click time count the clicks
                  if (curTime - mLastMouseDownTime <= mDoubleClickTime)
                     mLastMouseClickCount++;
                  else
                     mLastMouseClickCount = 1;
               }
               else
               {
                  mRightMouseLast = true;
                  mLastMouseClickCount = 1;
               }

               mLastMouseDownTime = curTime;
               mLastEvent.mouseClickCount = mLastMouseClickCount;

               rootRightMouseDown(mLastEvent);
            }
            else // it was a mouse up
               rootRightMouseUp(mLastEvent);
            return true;
         }
         else if(event->objInst == KEY_BUTTON2) // middle button
         {
            if(event->action == SI_MAKE)
            {
               U32 curTime = Platform::getVirtualMilliseconds();

               //if the last button pressed was the right...
               if (mMiddleMouseLast)
               {
                  //if it was within the double click time count the clicks
                  if (curTime - mLastMouseDownTime <= mDoubleClickTime)
                     mLastMouseClickCount++;
                  else
                     mLastMouseClickCount = 1;
               }
               else
               {
                  mMiddleMouseLast = true;
                  mLastMouseClickCount = 1;
               }

               mLastMouseDownTime = curTime;
               mLastEvent.mouseClickCount = mLastMouseClickCount;

               rootMiddleMouseDown(mLastEvent);
            }
            else // it was a mouse up
               rootMiddleMouseUp(mLastEvent);
            return true;
         }
      }
   }
   return false;
}

void GuiCanvas::rootMouseDown(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();
   mMouseButtonDown = true;

   //pass the event to the mouse locked control
   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onMouseDown(event);

   //else pass it to whoever is underneath the cursor
   else
   {
      iterator i;
      i = end();
      while (i != begin())
      {
         i--;
         GuiControl *ctrl = static_cast<GuiControl *>(*i);
         GuiControl *controlHit = ctrl->findHitControl(event.mousePoint);

         //see if the controlHit is a modeless dialog...
         if ((! controlHit->mActive) && (! controlHit->mProfile->mModal))
            continue;
         else
         {
            controlHit->onMouseDown(event);
            break;
         }
      }
   }
   if (bool(mMouseControl))
      mMouseControlClicked = true;
}

void GuiCanvas::findMouseControl(const GuiEvent &event)
{
   if(size() == 0)
   {
      mMouseControl = NULL;
      return;
   }
   GuiControl *controlHit = findHitControl(event.mousePoint);
   if(controlHit != static_cast<GuiControl*>(mMouseControl))
   {
      if(bool(mMouseControl))
         mMouseControl->onMouseLeave(event);
      mMouseControl = controlHit;
      mMouseControl->onMouseEnter(event);
   }
}

//Luma: Some fixes from the forums, Dave Calabrese
//http://www.garagegames.com/community/forums/viewthread/93467/1#comment-669559
void GuiCanvas::rootScreenTouchDown(const GuiEvent &event)
{
    mPrevMouseTime = Platform::getVirtualMilliseconds();  
    mMouseButtonDown = true;  
  
        iterator i;  
        i = end();  
        while (i != begin())  
        {  
            i--;  
            GuiControl *ctrl = static_cast<GuiControl *>(*i);  
            GuiControl *controlHit = ctrl->findHitControl(event.mousePoint);  
              
            //If the control we hit is not the same one that is locked,  
            // then unlock the existing control.  
            if (bool(mMouseCapturedControl))  
            {  
                if(mMouseCapturedControl->isMouseLocked())  
                {  
                    if(mMouseCapturedControl != controlHit)  
                    {  
                        mMouseCapturedControl->onMouseLeave(event);  
                    }  
                }  
            }  
              
            //see if the controlHit is a modeless dialog...  
            if ((! controlHit->mActive) && (! controlHit->mProfile->mModal))  
                continue;  
            else  
            {  
                controlHit->onMouseDown(event);  
                break;  
            }  
        }  
      
    if (bool(mMouseControl))  
        mMouseControlClicked = true;  
}

void GuiCanvas::rootScreenTouchUp(const GuiEvent &event)
{
	mPrevMouseTime = Platform::getVirtualMilliseconds();
	mMouseButtonDown = false;

	iterator i;
	i = end();
	while (i != begin())
	{
		//dPrintf( "GuiCanvas::rootScreenTouchUp - event.mousePoint: (%d, %d)\n", event.mousePoint.x, event.mousePoint.y ); // RKS
		
		i--;
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		GuiControl *controlHit = ctrl->findHitControl(event.mousePoint); // RKS:  This is where the touch up fails!
		
		 // RKS: I added these for debugging why touchup doesnt happen when dragging finger off screen
		
		//dPrintf( " - ctrl:       %s\n", ctrl->getName() );
		//dPrintf( " - controlHit: %s\n", controlHit->getName() ); 
		
		//see if the controlHit is a modeless dialog...
		if ((! controlHit->mActive) && (! controlHit->mProfile->mModal))
		{	
			printf( "GuiCanvas::rootScreenTouchUp - the control hit is modeless dialog or something\n" );
			continue;
		}
		else
		{
			controlHit->onMouseUp(event);
			break;
		}
	}
}

void GuiCanvas::refreshMouseControl()
{
   GuiEvent evt;
   evt.mousePoint.x = S32(cursorPt.x);
   evt.mousePoint.y = S32(cursorPt.y);
   findMouseControl(evt);
}

void GuiCanvas::rootMouseUp(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();
   mMouseButtonDown = false;

   //pass the event to the mouse locked control
   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onMouseUp(event);
   else
   {
      findMouseControl(event);
      if(bool(mMouseControl))
         mMouseControl->onMouseUp(event);
   }
}

void GuiCanvas::checkLockMouseMove(const GuiEvent &event)
{
	GuiControl *controlHit = findHitControl(event.mousePoint);
   if(controlHit != mMouseControl)
   {
      if(mMouseControl == mMouseCapturedControl)
         mMouseCapturedControl->onMouseLeave(event);
      else if(controlHit == mMouseCapturedControl)
         mMouseCapturedControl->onMouseEnter(event);
      mMouseControl = controlHit;
   }
}

void GuiCanvas::rootMouseDragged(const GuiEvent &event)
{
   //pass the event to the mouse locked control
   if (bool(mMouseCapturedControl))
   {
      checkLockMouseMove(event);
	  if(!mMouseCapturedControl.isNull())
			mMouseCapturedControl->onMouseDragged(event);
	   //Luma: Mouse dragged calls mouse Moved on iPhone
#ifdef TORQUE_OS_IPHONE
	   mMouseCapturedControl->onMouseMove(event);
#endif //TORQUE_OS_IPHONE
   }
   else
   {
      findMouseControl(event);
      if(bool(mMouseControl))
	  {
		  mMouseControl->onMouseDragged(event);
#ifdef TORQUE_OS_IPHONE
		  mMouseControl->onMouseMove(event);
#endif //TORQUE_OS_IPHONE
		  
	  }
   }
}

void GuiCanvas::rootMouseMove(const GuiEvent &event)
{
   if (bool(mMouseCapturedControl))
   {
      checkLockMouseMove(event);
      mMouseCapturedControl->onMouseMove(event);
   }
   else
   {
      findMouseControl(event);
      if(bool(mMouseControl))
         mMouseControl->onMouseMove(event);
   }
}

void GuiCanvas::rootRightMouseDown(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();
   mMouseRightButtonDown = true;

   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onRightMouseDown(event);
   else
   {
      findMouseControl(event);

      if(bool(mMouseControl))
      {
         mMouseControl->onRightMouseDown(event);
      }
   }
}

void GuiCanvas::rootRightMouseUp(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();
   mMouseRightButtonDown = false;

   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onRightMouseUp(event);
   else
   {
      findMouseControl(event);

      if(bool(mMouseControl))
         mMouseControl->onRightMouseUp(event);
   }
}

void GuiCanvas::rootRightMouseDragged(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();

   if (bool(mMouseCapturedControl))
   {
      checkLockMouseMove(event);
      mMouseCapturedControl->onRightMouseDragged(event);
   }
   else
   {
      findMouseControl(event);

      if(bool(mMouseControl))
         mMouseControl->onRightMouseDragged(event);
   }
}

void GuiCanvas::rootMiddleMouseDown(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();
   mMouseMiddleButtonDown = true;

   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onMiddleMouseDown(event);
   else
   {
      findMouseControl(event);

      if(bool(mMouseControl))
      {
         mMouseControl->onMiddleMouseDown(event);
      }
   }
}

void GuiCanvas::rootMiddleMouseUp(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();
   mMouseMiddleButtonDown = false;

   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onMiddleMouseUp(event);
   else
   {
      findMouseControl(event);

      if(bool(mMouseControl))
         mMouseControl->onMiddleMouseUp(event);
   }
}

void GuiCanvas::rootMiddleMouseDragged(const GuiEvent &event)
{
   mPrevMouseTime = Platform::getVirtualMilliseconds();

   if (bool(mMouseCapturedControl))
   {
      checkLockMouseMove(event);
      mMouseCapturedControl->onMiddleMouseDragged(event);
   }
   else
   {
      findMouseControl(event);

      if(bool(mMouseControl))
         mMouseControl->onMiddleMouseDragged(event);
   }
}

void GuiCanvas::rootMouseWheelUp(const GuiEvent &event)
{
   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onMouseWheelUp(event);
   else
   {
      findMouseControl(event);

      if (bool(mMouseControl))
         mMouseControl->onMouseWheelUp(event);
   }
}

void GuiCanvas::rootMouseWheelDown(const GuiEvent &event)
{
   if (bool(mMouseCapturedControl))
      mMouseCapturedControl->onMouseWheelDown(event);
   else
   {
      findMouseControl(event);

      if (bool(mMouseControl))
         mMouseControl->onMouseWheelDown(event);
   }
}

void GuiCanvas::setContentControl(GuiControl *gui)
{
   if(!gui)
      return;

   // If we're setting the same content, don't do anything
   if( gui == at(0) )
      return;

   //remove all dialogs on layer 0
   U32 index = 0;
   while (size() > index)
   {
      GuiControl *ctrl = static_cast<GuiControl*>((*this)[index]);
      if (ctrl == gui || ctrl->mLayer != 0)
         index++;

      removeObject(ctrl);
      Sim::getGuiGroup()->addObject(ctrl);
   }

   // lose the first responder from the old GUI
   GuiControl* responder = gui->findFirstTabable();
   if(responder)
      responder->setFirstResponder();

   //add the gui to the front
   if(!size() || gui != (*this)[0])
   {
      // automatically wakes objects in GuiControl::onWake
      addObject(gui);
      if (size() >= 2)
         reOrder(gui, *begin());
   }
   //refresh the entire gui
   resetUpdateRegions();

   //rebuild the accelerator map
   mAcceleratorMap.clear();

   for(iterator i = end(); i != begin() ; )
   {
      i--;
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->buildAcceleratorMap();

      if (ctrl->mProfile->mModal)
         break;
   }
   refreshMouseControl();

   // Force the canvas to update the sizing of the new content control
   maintainSizing();


   _WPB(PROCESS);
}

GuiControl *GuiCanvas::getContentControl()
{
   if(size() > 0)
      return (GuiControl *) first();
   return NULL;
}

void GuiCanvas::pushDialogControl(GuiControl *gui, S32 layer)
{
   //add the gui
   gui->mLayer = layer;

   // GuiControl::addObject wakes the object
   // UNUSED: JOSEPH THOMAS -> bool wakedGui = !gui->isAwake();
   addObject(gui);

   //reorder it to the correct layer
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl*>(*i);
      if (ctrl->mLayer > gui->mLayer)
      {
         reOrder(gui, ctrl);
         break;
      }
   }

   //call the dialog push method
   gui->onDialogPush();

   //find the top most dialog
   // UNUSED: JOSEPH THOMAS -> GuiControl *topCtrl = static_cast<GuiControl*>(last());

   //find the first responder
   GuiControl* responder = gui->findFirstTabable();
   if(responder)
      responder->setFirstResponder();

   // call the 'onWake' method?
   //if(wakedGui)
   //   Con::executef(gui, 1, "onWake");

   //refresh the entire gui
   resetUpdateRegions();

   //rebuild the accelerator map
   mAcceleratorMap.clear();
   if (size() > 0)
   {
      GuiControl *ctrl = static_cast<GuiControl*>(last());
      ctrl->buildAcceleratorMap();
   }
   refreshMouseControl();
   if(gui->mProfile && gui->mProfile->mModal)
   {
      Input::pushCursor(CursorManager::curArrow);

   }
}

void GuiCanvas::popDialogControl(GuiControl *gui)
{
   if (size() < 1)
      return;

   //first, find the dialog, and call the "onDialogPop()" method
   GuiControl *ctrl = NULL;
   if (gui)
   {
      //*** DAW: For modal dialogs, reset the mouse cursor and enable the appropriate platform menus
      if(gui->mProfile && gui->mProfile->mModal)
      {
         Input::popCursor();

      }

      //make sure the gui really exists on the stack
      iterator i;
      bool found = false;
      for(i = begin(); i != end(); i++)
      {
         GuiControl *check = static_cast<GuiControl *>(*i);
         if (check == gui)
         {
            ctrl = check;
            found = true;
         }
      }
      if (! found)
         return;
   }
   else
      ctrl = static_cast<GuiControl*>(last());

   //call the "on pop" function
   ctrl->onDialogPop();

   // sleep the object
   // UNUSED: JOSEPH THOMAS -> bool didSleep = ctrl->isAwake();

   //now pop the last child (will sleep if awake)
   removeObject(ctrl);

   // Save the old responder:
   // UNUSED: JOSEPH THOMAS -> GuiControl *oldResponder = mFirstResponder;

   Sim::getGuiGroup()->addObject(ctrl);

   if (size() > 0)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(last());
      if(ctrl->mFirstResponder)
         ctrl->mFirstResponder->setFirstResponder();
   }
   else
   {
      setFirstResponder(NULL);
   }

   //refresh the entire gui
   resetUpdateRegions();

   //rebuild the accelerator map
   mAcceleratorMap.clear();
   if (size() > 0)
   {
      GuiControl *ctrl = static_cast<GuiControl*>(last());
      ctrl->buildAcceleratorMap();
   }
   refreshMouseControl();
}

void GuiCanvas::popDialogControl(S32 layer)
{
   if (size() < 1)
      return;

   GuiControl *ctrl = NULL;
   iterator i = end(); // find in z order (last to first)
   while (i != begin())
   {
      i--;
      ctrl = static_cast<GuiControl*>(*i);
      if (ctrl->mLayer == layer)
         break;
   }
   if (ctrl)
      popDialogControl(ctrl);
}

void GuiCanvas::mouseLock(GuiControl *lockingControl)
{
   if (bool(mMouseCapturedControl))
      return;
   mMouseCapturedControl = lockingControl;
   if(mMouseControl && mMouseControl != mMouseCapturedControl)
   {
      GuiEvent evt;
      evt.mousePoint.x = S32(cursorPt.x);
      evt.mousePoint.y = S32(cursorPt.y);

      mMouseControl->onMouseLeave(evt);
   }
}

void GuiCanvas::mouseUnlock(GuiControl *lockingControl)
{
   if (static_cast<GuiControl*>(mMouseCapturedControl) != lockingControl)
      return;

   GuiEvent evt;
   evt.mousePoint.x = S32(cursorPt.x);
   evt.mousePoint.y = S32(cursorPt.y);

   GuiControl * controlHit = findHitControl(evt.mousePoint);
   if(controlHit != mMouseCapturedControl)
   {
      mMouseControl = controlHit;
      mMouseControlClicked = false;
      if(bool(mMouseControl))
         mMouseControl->onMouseEnter(evt);
   }
   mMouseCapturedControl = NULL;
}

void GuiCanvas::paint()
{
   resetUpdateRegions();

   // inhibit explicit refreshes in the case we're swapped out
   if (gDGLRender)
      renderFrame(false);
}

void GuiCanvas::maintainSizing()
{
   Point2I size = Platform::getWindowSize();

   if(size.x == 0 || size.y == 0)
      return;

   RectI screenRect(0, 0, size.x, size.y);
   mBounds = screenRect;

   //all bottom level controls should be the same dimensions as the canvas
   //this is necessary for passing mouse events accurately
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      AssertFatal(static_cast<GuiControl*>((*i))->isAwake(), "GuiCanvas::renderFrame: ctrl is not awake");
      GuiControl *ctrl = static_cast<GuiControl*>(*i);
      Point2I ext = ctrl->getExtent();
      Point2I pos = ctrl->getPosition();

      if(pos != screenRect.point || ext != screenRect.extent)
      {
         ctrl->resize(screenRect.point, screenRect.extent);
         resetUpdateRegions();
      }
   }

}

void GuiCanvas::renderFrame(bool preRenderOnly, bool bufferSwap /* = true */)
{
   PROFILE_START(CanvasPreRender);
	
#ifndef TORQUE_OS_IPHONE
//PUAP: THIS IS NOT NEEDED for iPhone
	
   if(mRenderFront)
      glDrawBuffer(GL_FRONT);
   else
      glDrawBuffer(GL_BACK);
#endif

   // Render all RTT Gui's HERE
   //DynamicTexture::updateGuiTextures();

   // Make sure the root control is the size of the canvas.
   Point2I size = Platform::getWindowSize();

   if(size.x == 0 || size.y == 0)
   {
	   //Luma: Fixed missing PROFILE_END()
	   PROFILE_END();
       return;
   }

   RectI screenRect(0, 0, size.x, size.y);

   maintainSizing();

   
#if PUAP_OPTIMIZE
	//-Mat since we remove this from GUIControls we want to make sure leftovers get cleared here
	if( mProfile && mProfile->mFillColor ) {
		F32 mul = 1.0f / 255.0f;
		F32 red = mProfile->mFillColor.red * mul;
		F32 green = mProfile->mFillColor.green * mul;
		F32 blue = mProfile->mFillColor.blue * mul;
		F32 alpha = mProfile->mFillColor.alpha * mul;
		glClearColor(red, blue, green, alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	} else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//ColorI blank( 0, 0, 0, 255 );
		//dglDrawRectFill(screenRect, blank);
	}
#endif


   //preRender (recursive) all controls
   preRender();
   PROFILE_END();
   if(preRenderOnly)
      return;

   // for now, just always reset the update regions - this is a
   // fix for FSAA on ATI cards
   resetUpdateRegions();

// Moved this below object integration for performance reasons. -JDD
//   // finish the gl render so we don't get too far ahead of ourselves
//#if defined(TORQUE_OS_WIN32)
//   PROFILE_START(glFinish);
//   glFinish();
//   PROFILE_END();
//#endif
   //draw the mouse, but not using tags...
   PROFILE_START(CanvasRenderControls);

   GuiCursor *mouseCursor = NULL;
   bool cursorVisible = true;

   if(bool(mMouseCapturedControl))
      mMouseCapturedControl->getCursor(mouseCursor, cursorVisible, mLastEvent);
   else if(bool(mMouseControl))
      mMouseControl->getCursor(mouseCursor, cursorVisible, mLastEvent);

   Point2I cursorPos((S32)cursorPt.x, (S32)cursorPt.y);
   if(!mouseCursor)
      mouseCursor = defaultCursor;

   if(lastCursorON && lastCursor)
   {
      Point2I spot = lastCursor->getHotSpot();
      Point2I cext = lastCursor->getExtent();
      Point2I pos = lastCursorPt - spot;
      addUpdateRegion(pos - Point2I(2, 2), Point2I(cext.x + 4, cext.y + 4));
   }
   if(cursorVisible && mouseCursor)
   {
      Point2I spot = mouseCursor->getHotSpot();
      Point2I cext = mouseCursor->getExtent();
      Point2I pos = cursorPos - spot;

      addUpdateRegion(pos - Point2I(2, 2), Point2I(cext.x + 4, cext.y + 4));
   }

	lastCursorON = cursorVisible;
	lastCursor = mouseCursor;
	lastCursorPt = cursorPos;

   RectI updateUnion;
   buildUpdateUnion(&updateUnion);
   if (updateUnion.intersect(screenRect))
   {
      //fill in with black first
      //glClearColor(0, 0, 0, 0);
      //glClear(GL_COLOR_BUFFER_BIT);

      //render the dialogs
      iterator i;
      for(i = begin(); i != end(); i++)
      {
         GuiControl *contentCtrl = static_cast<GuiControl*>(*i);
         dglSetClipRect(updateUnion);
         glDisable( GL_CULL_FACE );
         contentCtrl->onRender(contentCtrl->getPosition(), updateUnion);
      }

	  // Tooltip resource
	  if(bool(mMouseControl))
	  {
         U32 curTime = Platform::getRealMilliseconds();
         if(hoverControl == mMouseControl)
		 {
            if(hoverPositionSet || (curTime - hoverControlStart) >= hoverControl->mTipHoverTime || (curTime - hoverLeftControlTime) <= hoverControl->mTipHoverTime)
			{
               if(!hoverPositionSet)
			   {
                  hoverPosition = cursorPos;
			   }
               hoverPositionSet = mMouseControl->renderTooltip(hoverPosition);
			}

         } else
		 {
            if(hoverPositionSet)
			{
               hoverLeftControlTime = curTime;
			   hoverPositionSet = false;
			}
            hoverControl = mMouseControl;
			hoverControlStart = curTime;
		 }
	  }
	  //end tooltip

      dglSetClipRect(updateUnion);

      //temp draw the mouse
      if (cursorON && mShowCursor && !mouseCursor)
      {
#ifdef TORQUE_OS_IPHONE
         glColor4ub(255, 0, 0, 255);
		 GLfloat vertices[] = {
			  (GLfloat)(cursorPt.x),(GLfloat)(cursorPt.y),
			  (GLfloat)(cursorPt.x + 2),(GLfloat)(cursorPt.y),
			  (GLfloat)(cursorPt.x + 2),(GLfloat)(cursorPt.y + 2),
			  (GLfloat)(cursorPt.x),(GLfloat)(cursorPt.y + 2),
		  };
		  glEnableClientState(GL_VERTEX_ARRAY);
		  glVertexPointer(2, GL_FLOAT, 0, vertices);
		  glDrawArrays(GL_LINE_LOOP, 0, 4);
#else
         glColor4ub(255, 0, 0, 255);
         glRecti((S32)cursorPt.x, (S32)cursorPt.y, (S32)(cursorPt.x + 2), (S32)(cursorPt.y + 2));
#endif
      }
	   
      //DEBUG
      //draw the help ctrl
      //if (helpCtrl)
      //{
      //   helpCtrl->render(srf);
      //}

      if (cursorON && mouseCursor && mShowCursor)
      {
         Point2I pos((S32)cursorPt.x, (S32)cursorPt.y);
         Point2I spot = mouseCursor->getHotSpot();

         pos -= spot;
         mouseCursor->render(pos);
      }
   }

   _WPB(SHOW);

   PROFILE_END();

   

   // Render all RTT end of frame updates HERE
   //DynamicTexture::updateScreenTextures();
   //DynamicTexture::updateEndOfFrameTextures();

   if( bufferSwap )
      swapBuffers();
	
#if defined(TORQUE_OS_WIN32)
   PROFILE_START(glFinish);
   glFinish(); // This was changed to work with the D3D layer -pw
   PROFILE_END();
#endif

}

void GuiCanvas::swapBuffers()
{
   PROFILE_START(SwapBuffers);
   //flip the surface
   if(!mRenderFront)
      Video::swapBuffers();
   PROFILE_END();
}

void GuiCanvas::buildUpdateUnion(RectI *updateUnion)
{
   *updateUnion = mOldUpdateRects[0];

   //the update region should encompass the oldUpdateRects, and the curUpdateRect
   Point2I upperL;
   Point2I lowerR;

   upperL.x = getMin(mOldUpdateRects[0].point.x, mOldUpdateRects[1].point.x);
   upperL.x = getMin(upperL.x, mCurUpdateRect.point.x);

   upperL.y = getMin(mOldUpdateRects[0].point.y, mOldUpdateRects[1].point.y);
   upperL.y = getMin(upperL.y, mCurUpdateRect.point.y);

   lowerR.x = getMax(mOldUpdateRects[0].point.x + mOldUpdateRects[0].extent.x, mOldUpdateRects[1].point.x + mOldUpdateRects[1].extent.x);
   lowerR.x = getMax(lowerR.x, mCurUpdateRect.point.x + mCurUpdateRect.extent.x);

   lowerR.y = getMax(mOldUpdateRects[0].point.y + mOldUpdateRects[0].extent.y, mOldUpdateRects[1].point.y + mOldUpdateRects[1].extent.y);
   lowerR.y = getMax(lowerR.y, mCurUpdateRect.point.y + mCurUpdateRect.extent.y);

   updateUnion->point = upperL;
   updateUnion->extent = lowerR - upperL;

   //shift the oldUpdateRects
   mOldUpdateRects[0] = mOldUpdateRects[1];
   mOldUpdateRects[1] = mCurUpdateRect;

   mCurUpdateRect.point.set(0,0);
   mCurUpdateRect.extent.set(0,0);
}

void GuiCanvas::addUpdateRegion(Point2I pos, Point2I ext)
{
   if(mCurUpdateRect.extent.x == 0)
   {
      mCurUpdateRect.point = pos;
      mCurUpdateRect.extent = ext;
   }
   else
   {
      Point2I upperL;
      upperL.x = getMin(mCurUpdateRect.point.x, pos.x);
      upperL.y = getMin(mCurUpdateRect.point.y, pos.y);
      Point2I lowerR;
      lowerR.x = getMax(mCurUpdateRect.point.x + mCurUpdateRect.extent.x, pos.x + ext.x);
      lowerR.y = getMax(mCurUpdateRect.point.y + mCurUpdateRect.extent.y, pos.y + ext.y);
      mCurUpdateRect.point = upperL;
      mCurUpdateRect.extent = lowerR - upperL;
   }
}

void GuiCanvas::resetUpdateRegions()
{
   //DEBUG - get surface width and height
   mOldUpdateRects[0].set(mBounds.point, mBounds.extent);
   mOldUpdateRects[1] = mOldUpdateRects[0];
   mCurUpdateRect = mOldUpdateRects[0];
}

void GuiCanvas::setFirstResponder( GuiControl* newResponder )
{
	GuiControl* oldResponder = mFirstResponder;
	Parent::setFirstResponder( newResponder );

	if ( oldResponder && ( oldResponder != mFirstResponder ) )
		oldResponder->onLoseFirstResponder();
}