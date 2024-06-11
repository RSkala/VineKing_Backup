//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMIPHONE_H_
#define _PLATFORMIPHONE_H_

/// NOTE: Placing system headers before Torque's platform.h will work around the Torque-Redefines-New problems.
//#include <CoreServices/CoreServices.h>
#include "platform/platform.h"
#include "math/mMath.h"
#include "platformiPhone/iPhoneEvents.h"

#include "platformiPhone/iPhoneOGLVideo.h"


class iPhonePlatState
{
public:
   bool              captureDisplay;
   bool              fadeWindows;

   id				 appID;
   UIView			 *Window;
   char              appWindowTitle[256];
   bool              quit;
   iPhoneOGLVideo        *ctx;
   bool              ctxNeedsUpdate;
	
	bool			portrait;

   S32               desktopBitsPixel;
   S32               desktopWidth;
   S32               desktopHeight;
   U32               currentTime;
   bool				 fullscreen;
   
   U32               osVersion;
   
   bool              tsmActive;
   
   U32               firstThreadId;
   U32               torqueThreadId;
   
   void*             alertSemaphore;
   S32               alertHit;
//   DialogRef         alertDlg;
//   EventQueueRef     mainEventQueue;
   
   MRandomLCG        platRandom;
   
   bool              mouseLocked;
   bool              backgrounded;
   bool              minimized;
   
   S32               sleepTicks;
   S32               lastTimeTick;
   
   Point2I           windowSize;
   
   U32               appReturn;
   
   U32               argc;
   const char**      argv;
   

   StringTableEntry  mainDotCsDir;
   
   NSTimer			 *mainLoopTimer;
   NSTimeInterval	 timerInterval;
	UIApplication	*application;
	//-Mat
	bool		multipleTouchesEnabled;
	
   iPhonePlatState();
};

/// Global singleton that encapsulates a lot of mac platform state & globals.
extern iPhonePlatState platState;

/// @name Misc Mac Plat Functions
/// Functions that are used by multiple files in the mac plat, but too trivial
/// to require their own header file.
/// @{
/// Fills gGLState with info about this gl renderer's capabilities.
void getGLCapabilities(void);


/// Display a file dialog.
/// calls FileDialog::Execute() on \p dialog
/// @param dialog The FileDialog object to Execute. A void* is used to cut down on header dependencies.
/// @see platform/nativeDialogs/fileDialog.h
void iPhoneShowDialog(void* dialog);

/// Translates a Mac keycode to a Torque keycode
U8 TranslateOSKeyCode(U8 vcode);
/// @}

/// @name Misc Mac Plat constants
/// @{

/// earlier versions of OSX don't have these convinience macros, so manually stick them here.
#ifndef IntToFixed
#define IntToFixed(a)    ((Fixed)(a) <<16)
#define FixedToInt(a)   ((short)(((Fixed)(a) + fixed1/2) >> 16))
#endif

/// mouse wheel sensitivity factor
const S32 kTMouseWheelMagnificationFactor = 25;

/// @}



#endif //_PLATFORMIPHONE_H_

