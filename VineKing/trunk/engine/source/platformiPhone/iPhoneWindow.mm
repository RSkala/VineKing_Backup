//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


#include "platformiPhone/platformiPhone.h"
#include "platform/platformVideo.h"
#include "platformiPhone/iPhoneOGLVideo.h"
#include "platformiPhone/iPhoneConsole.h"
#include "platform/platformInput.h"
#include "platform/gameInterface.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "platformiPhone/iPhoneEvents.h"
#include "platform/threads/thread.h"

#include "platformiPhone/iPhoneWindow.h"
#import <OpenGLES/EAGLDrawable.h>

bool setScreenOrientation( bool, bool );
bool getStatusBarHidden();
bool setStatusBarHidden( bool );
void setStatusBarType( S32 );

//------------------------------------------------------------------------------
#pragma mark ---- PlatState ----
iPhonePlatState platState;

iPhonePlatState::iPhonePlatState()
{

   
   captureDisplay = true;
   fadeWindows    = true;
   backgrounded   = false;
   minimized      = false;
   
   quit           = false;
	
	portrait			= true;//-Mat iPhone is in portrait mode by default


   // start with something reasonable.
   desktopBitsPixel  = IPHONE_DEFAULT_RESOLUTION_BIT_DEPTH;
   desktopWidth      = IPHONE_DEFAULT_RESOLUTION_X;
   desktopHeight     = IPHONE_DEFAULT_RESOLUTION_Y;
   fullscreen		= true;

   osVersion = 0;
   
   dStrcpy(appWindowTitle, "iPhone Torque Game Engine");
   
   // Semaphore for alerts. We put the app in a modal state by blocking the main 
   alertSemaphore = Semaphore::createSemaphore(0);
   
   // directory that contains main.cs . This will help us detect whether we are 
   // running with the scripts in the bundle or not.
   mainDotCsDir = NULL;
   
   mainLoopTimer = NULL;
}


//------------------------------------------------------------------------------
// DGL, the Gui, and TS use this for various purposes.
const Point2I &Platform::getWindowSize()
{
   return platState.windowSize;
}


//------------------------------------------------------------------------------
// save the window size, for DGL's use
void Platform::setWindowSize( U32 newWidth, U32 newHeight )
{
   platState.windowSize.set( newWidth, newHeight );
}

//------------------------------------------------------------------------------
// Issue a minimize event. The standard handler will handle it.
void Platform::minimizeWindow()
{
	//no minimizing on iPhone
}

void Platform::restoreWindow()
{
	//no minimizing on iPhone
}

//------------------------------------------------------------------------------
void Platform::setWindowTitle(const char* title )
{
	//no window titles on iPhone
}



#pragma mark ---- Init funcs  ----
//------------------------------------------------------------------------------
void Platform::init()
{
   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "iphone" );

   iPhoneConsole::create();
   //if ( !iPhoneConsole::isEnabled() )
   Input::init();

   // allow users to specify whether to capture the display or not when going fullscreen
   Con::addVariable("pref::mac::captureDisplay", TypeBool, &platState.captureDisplay);
   Con::addVariable("pref::mac::fadeWindows", TypeBool, &platState.fadeWindows);
   
   // create the opengl display device
   DisplayDevice *dev = NULL;
   Con::printf( "Video Init:" );
   Video::init();
   dev = OpenGLDevice::create();
   if(dev)
      Con::printf( "   Accelerated OpenGL display device detected." );
   else
      Con::printf( "   Accelerated OpenGL display device not detected." );

   // and now we can install the device.
   Video::installDevice(dev);
   Con::printf( "" );
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
   setWindowLocked( false );
   Video::destroy();
   Input::destroy();
   iPhoneConsole::destroy();
}


//Hidden by Default. 1 Black Opaque, 2 Black Translucent
S32 gStatusBarType = 0; 
bool gStatusBarHidden = true;

//Landscape by default. 0 Landscape, 1 Portrait
S32 gScreenOrientation = 0;
bool gScreenUpsideDown = true;


//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
   dSprintf(platState.appWindowTitle, sizeof(platState.appWindowTitle), name);
	
	platState.windowSize.x = IPHONE_DEFAULT_RESOLUTION_X;
	platState.windowSize.y = IPHONE_DEFAULT_RESOLUTION_Y;
	
    //Get screen orientation prefs //Based on 0 Landscape, 1 Portrait
	gScreenOrientation = Con::getIntVariable( "$pref::iDevice::ScreenOrientation" );
	gScreenUpsideDown = Con::getBoolVariable( "$pref::iPhone::ScreenUpsideDown" );

    platState.windowSize = initialSize;
    
    // The following check seems irrelevant, since initialSize is always manually set
    // to a value supporting the iOS devices in createCanvas
    // See this thread for more information: http://www.garagegames.com/community/forums/viewthread/124734/1#comment-801182
    /*
	if( initialSize.x < MIN_RESOLUTION_X || initialSize.y < MIN_RESOLUTION_Y ) {
		Platform::AlertOK( "Invalid Initial window size", "initalSize is less than or equal to 0, using default resolution" );
	} else 	if( initialSize.x > IPHONE_MAX_RESOLUTION_X || initialSize.y > IPHONE_MAX_RESOLUTION_Y ) {
		Platform::AlertOK( "Invalid Initial window size", "initalSize is greater than max supported size, using default resolution." );
	} else {
		platState.windowSize = initialSize;
	}
	*/
	
	//Default to landscape, and run into portrait if requested.
	platState.portrait = false;
	
	if( gScreenOrientation != 0) //fuzzytodo :add a constant
	{
		//Could handle other options here, later.
		platState.portrait = true;
	}

	//We should now have a good windowSize, it will be default if initial size was bad
	iPhoneOGLVideo *glView;
	CGRect rect;

	rect.origin.x = 0;
	rect.origin.y = 0;
	
	rect.size.width = platState.windowSize.x;
	rect.size.height = platState.windowSize.y;
	
	glView = [[iPhoneOGLVideo alloc] initWithFrame: rect];
	
	//set this as our platfromState glView context
	[platState.Window addSubview: glView];
	
	platState.ctx = glView;

	//get status bar pref // 0 Hidden , 1 BlackOpaque , 2 BlackTranslucent
	
	S32 tempType = Con::getIntVariable( "$pref::iDevice::StatusBarType" );  
	setStatusBarType( tempType );
	
	//set screen orientation
	setScreenOrientation( platState.portrait, gScreenUpsideDown );
	
	bool fullScreen;
	U32 bpp = Con::getIntVariable( "$pref::iDevice::ScreenDepth" ); //IPHONE_DEFAULT_RESOLUTION_BIT_DEPTH;
	if(!bpp)
	{
		Con::printf("Default BPP Chosen , $pref::iDevice::ScreenDepth was not found.");
		bpp = IPHONE_DEFAULT_RESOLUTION_BIT_DEPTH;
	}
	
	fullScreen = true;
	//
	DisplayDevice::init();
	
	// this will create a rendering context & window
	bool ok = Video::setDevice( "OpenGL", platState.windowSize.x, platState.windowSize.y, bpp, fullScreen );
	if ( ! ok )
	{
        AssertFatal( false, "Could not find a compatible display device!" );
	}

	//Luma:	Clear frame buffer to BLACK to start with
	//NOTE:	This should probably be set by the user to be the color closest to Default.png in order to minimize any popping effect... $pref:: anyone? Are $pref::s even valid at this point in the Init process?
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	
}

//--------------------------------------
// run app function: not applicable to iPhone
//--------------------------------------
// run other apps
int runApp( const char* fileName, const char* batchArgs, bool blocking = false ) {
	return 0;
}

bool appIsRunning( int batchId ) {
	return false;
}

bool Platform::openWebBrowser( const char* webAddress )
{
	NSString *string = [[NSString alloc] initWithUTF8String: webAddress];
	NSURL *url = [[NSURL alloc] initWithString: string];
	bool ret = [platState.application openURL: url];
	[string release];
	[url release];
	
	return ret;// this bails on the application, switching to Safari
}

//dunno where this is supposed to be, but the windows version has it under WinWindow, so this seems a likely place
bool Platform::excludeOtherInstances(const char *string)
{
	return true;//only one instance on iPhone no matter what
}


bool isStatusBarHidden() 
{
	if( platState.application.statusBarHidden == YES ) 
	{
		return true;
	} 
	else 
	{
		return false;
	}
}

bool setStatusBarHidden( bool hidden )
{
	if( hidden ) 
	{
		platState.application.statusBarHidden = YES;
		gStatusBarHidden = true;
		
		return true;
	} 
	else 
	{
		platState.application.statusBarHidden = NO;
		gStatusBarHidden = false;
		
		return false;
	}
}

void setStatusBarType( S32 type ) 
{
	switch(type)
	{
		case 0: //Hidden
			setStatusBarHidden(true);
			break;
		case 1: //Black Opaque
			platState.application.statusBarStyle = UIStatusBarStyleBlackOpaque;
			setStatusBarHidden(false);
			break;
		case 2: //Black Transparent
			platState.application.statusBarStyle = UIStatusBarStyleBlackTranslucent;
			setStatusBarHidden(false);
			break;
		default:
			platState.application.statusBarStyle = UIStatusBarStyleDefault;
	}
	
	gStatusBarType = type;
}


bool setScreenOrientation( bool portrait, bool upsidedown ) 
{
	bool success = false;

	CGPoint point;
	if( platState.portrait ) 
	{
		point.x = platState.windowSize.x / 2; 
		point.y = platState.windowSize.y / 2;
	} 
	else
	{
		point.x = platState.windowSize.y / 2;
		point.y = platState.windowSize.x / 2; 
	}
	

	[platState.ctx centerOnPoint: point ];	
	
	if( portrait ) {//normal upright
		if( upsidedown ) {//button on top
			[platState.ctx rotateToAngle: M_PI + (M_PI / 2.0) ];//rotate to 90 degrees
			platState.application.statusBarOrientation = UIInterfaceOrientationPortraitUpsideDown ;
			success = true;
		} else {//button on bottom
			[platState.ctx rotateToAngle: (M_PI / 2.0) ];//rotate to 270 degrees
			platState.application.statusBarOrientation = UIInterfaceOrientationPortrait;
			success = true;
		}
	} else {//landscape/ sideways
		if( upsidedown ) {//button on left
			[platState.ctx rotateToAngle: 0 ];//rotate to -180 (0) degrees
			platState.application.statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
			success = true;
		} else {//button on right
			[platState.ctx rotateToAngle: (M_PI) ];//rotate to 180 degrees
			platState.application.statusBarOrientation = UIInterfaceOrientationLandscapeRight;
			success = true;
		}	
	}
	
	return success;
}

ConsoleFunction(setScreenOrientation, bool, 3, 3, "Sets the orientation of the screen ( portrait/landscape, upside down or right-side up )\n"
				"@(bool portrait, bool upside_down)")
{
	return setScreenOrientation( dAtob(argv[1]), dAtob(argv[2]) );
}


ConsoleFunction(getStatusBarHidden, bool, 1, 1, " Checks whether the status bar is hidden\n"
				"@return Returns true if hidden and false if not")
{
	return isStatusBarHidden();
}

ConsoleFunction(setStatusBarHidden, bool, 2, 2, " Hides/unhides the iPhone status bar \n"
				"@return true == status bar is hidden, false == status bar is visible")
{
	return setStatusBarHidden( dAtob(argv[1]) );
}

ConsoleFunction(setStatusBarType, void, 2, 2, " Set the status bar type. 0 hidden, 1 Black Opaque, 2 Black Translucent \n")
{
	return setStatusBarType( dAtoi(argv[1]) );
}