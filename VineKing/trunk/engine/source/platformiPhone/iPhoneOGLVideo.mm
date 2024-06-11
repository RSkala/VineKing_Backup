//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//
// Portions taken from OpenGL Full Screen.c sample from Apple Computer, Inc.
// (that's where many of the lead helper functions originated from, but code
//  has been significantly changed & revised.)
//-----------------------------------------------------------------------------


#include "platformiPhone/platformiPhone.h"
#include "platformiPhone/platformGL.h"
#include "platformiPhone/iPhoneOGLVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "core/fileStream.h"
#include "platformiPhone/iPhoneUtil.h"
#include "platformiPhone/iPhoneEvents.h"
#include "dgl/dgl.h"
#include "platform/profiler.h"

extern bool createMouseMoveEvent( S32 i, S32 x, S32 y );
extern bool createMouseDownEvent( S32 touchNumber, S32 x, S32 y );
extern bool createMouseUpEvent( S32 touchNumber, S32 x, S32 y );

//Luma: Tap support
extern void createMouseTapEvent( S32 nbrTaps, S32 x, S32 y );

extern bool createAccelMoveEvent( UIAccelerationValue *accel );
extern bool setScreenOrientation( bool, bool );
// TODO: Card Profiling code isn't doing anything.
extern StringTableEntry gScreenOrientation;
extern bool gScreenUpsideDown;


//-Mat we should update the accelereometer once per frame
U32  AccelerometerUpdateMS =	sgTimeManagerProcessInterval; // defines period between accelerometer updates updates in Milliseconds


#pragma mark -

//-----------------------------------------------------------------------------------------
// Creates a dummy AGL context, so that naughty objects that call OpenGL before the window
//  exists will not crash the game.
//  If for some reason we fail to get a default contet, assert -- something's very wrong.
//-----------------------------------------------------------------------------------------
void initDummyAgl(void)
{
}

U32      nAllDevs;


#pragma mark -
//------------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
   // Set the device name:
   mDeviceName = "OpenGL";

   // macs games are not generally full screen only
   mFullScreenOnly = false;
}

//------------------------------------------------------------------------------
void OpenGLDevice::initDevice()
{
	//instead of caling enum monitors and enumdisplaymodes on that, we're just going to put in the ones that we know we have

	mResolutionList.push_back( Resolution( 320, 480, 16 ) );	
	mResolutionList.push_back( Resolution( 480, 320, 16 ) );	

	mResolutionList.push_back( Resolution( 320, 480, 32 ) );	
	mResolutionList.push_back( Resolution( 480, 320, 32 ) );	
	
	mResolutionList.push_back( Resolution( 768, 1024, 16 ) );	
	mResolutionList.push_back( Resolution( 1024, 768, 16 ) );	

	mResolutionList.push_back( Resolution( 768, 1024, 32 ) );	
	mResolutionList.push_back( Resolution( 1024, 768, 32 ) );	

}

//------------------------------------------------------------------------------
// Activate
//  this is called once, as a result of createCanvas() in scripts.
//  dumps OpenGL driver info for the current screen
//  creates an initial window via setScreenMode
bool OpenGLDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
	Con::printf( " OpenGLDevice activating..." );
	
	// gets opengl rendering capabilities of the screen pointed to by platState.hDisplay
	// sets up dgl with the capabilities info, & reports opengl status.
	
	getGLCapabilities();
	
	// Create the window or capture fullscreen
	if(!setScreenMode( width, height, bpp, fullScreen, true, false ))
		return false;
	
	// set the displayDevice pref to "OpenGL"
	Con::setVariable( "$pref::Video::displayDevice", mDeviceName );   
	
	// set vertical sync now because it doesnt need setting every time we setScreenMode()
	setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ));
	
	return true;
}

//------------------------------------------------------------------------------
// returns TRUE if textures need resurrecting in future...
//------------------------------------------------------------------------------
bool OpenGLDevice::cleanupContextAndWindow()
{
   Con::printf( "Cleaning up the display device..." );
	return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::shutdown()
{
   Con::printf( "Shutting down the OpenGL display device..." );
	[platState.ctx release];
}


//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool OpenGLDevice::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint )
{

	Con::printf(" set screen mode %i x %i x %i, %s, %s, %s",width, height, bpp,
				fullScreen  ? "fullscreen" : "windowed",
				forceIt     ? "force it" : "dont force it",
				repaint     ? "repaint"  : "dont repaint"
				);
	
	// validation, early outs --------------------------------------------------
	// sanity check. some scripts are liable to pass in bad values.
	if(!bpp)
		bpp = platState.desktopBitsPixel;
	
	Resolution newRes = Resolution(width, height, bpp);
	
	// if no values changing and we're not forcing a change, kick out. prevents thrashing.
	if(!forceIt && smIsFullScreen == fullScreen && smCurrentRes == newRes)
		return(true);
	
	// we have a new context, this is now safe to do:
	// delete any contexts or windows that exist, and kill the texture manager.
	bool needResurrect = cleanupContextAndWindow();
	
	Con::printf( ">> Attempting to change display settings to %s %dx%dx%d...", 
				fullScreen?"fullscreen":"windowed", newRes.w, newRes.h, newRes.bpp );
	
	// set torque variables ----------------------------------------------------
	// save window size for dgl
	Platform::setWindowSize( newRes.w, newRes.h );
	// update smIsFullScreen and pref
	smIsFullScreen = fullScreen;
	Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );
	// save resolution
	smCurrentRes = newRes;
	// save resolution to prefs
	char buf[32];
	if(fullScreen)
	{
		dSprintf( buf, sizeof(buf), "%d %d %d", newRes.w, newRes.h, newRes.bpp);
		Con::setVariable("$pref::Video::resolution", buf);
	}
	else
	{
		dSprintf( buf, sizeof(buf), "%d %d", newRes.w, newRes.h);
		Con::setVariable("$pref::Video::windowedRes", buf);
	}
	
	// fade the window into existance, asynchronously
	//if(platState.appWindow)
		//iPhoneFadeInWindow(platState.appWindow);
	
	
	// begin rendering again ----------------------------------------------------
	if( needResurrect )
	{
		// Reload the textures gl names
		Con::printf( "Resurrecting the texture manager..." );
		Game->textureResurrect();
	}
	
	// reattach the event handlers to the new window.
	//iPhoneRemoveCarbonEventHandlers();
	//iPhoneInstallCarbonEventHandlers();
	
	if( repaint )
		Con::evaluate( "resetCanvas();" );
	
	
	return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
	[platState.ctx swapBuffers];
}  


//------------------------------------------------------------------------------
const char* OpenGLDevice::getDriverInfo()
{
	return NULL;
}

typedef struct iPhoneGamma
{
   F32 r, g, b;
   F32 scale;
};

// UNUSED: JOSEPH THOMAS -> static iPhoneGamma _iPhoneGamma;
//------------------------------------------------------------------------------
bool OpenGLDevice::getGammaCorrection(F32 &g)
{
   return true;
}    

//------------------------------------------------------------------------------
bool OpenGLDevice::setGammaCorrection(F32 g)
{
   return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync( bool on )
{
   return true;
}



Resolution Video::getDesktopResolution()
{
	return Resolution( IPHONE_DEFAULT_RESOLUTION_X, IPHONE_DEFAULT_RESOLUTION_Y, IPHONE_DEFAULT_RESOLUTION_BIT_DEPTH );
}



//EAGLContext agl_ctx;


DisplayDevice* OpenGLDevice::create()
{
	// set up a dummy default agl context.
	// this will be replaced later with the window's context,
	// but we need agl_ctx to be valid at all times,
	// since some things try to make gl calls before the device is activated.
	
	// create the DisplayDevice
	OpenGLDevice* newOGLDevice = new OpenGLDevice();

	newOGLDevice->initDevice();
	
	return newOGLDevice;
}




#define USE_DEPTH_BUFFER 0

// A class extension to declare private methods
@interface iPhoneOGLVideo ()

@property (nonatomic, retain) EAGLContext *context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesChangedWithEvent:(UIEvent *)event;
- (void)touchesCancelled;
@end


@implementation iPhoneOGLVideo

@synthesize context;


// You must implement this
+ (Class)layerClass {
	return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithFrame: (CGRect) frame
{
	if ((self = [super initWithFrame: frame])) {
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
		
		eaglLayer.opaque = YES;
		//eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										//[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if (!context || ![EAGLContext setCurrentContext:context]) {
			[self release];
			return nil;
		} 
		
	}
	if( AccelerometerUpdateMS <= 0 ) {
	  //Luma:	This variable needs to be store MS value, not Seconds value
      AccelerometerUpdateMS = 33; // 33 ms
	}
	
	//Luma: Do division by 1000.0f here to get the seconds value that the UIAccelerometer needs
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(AccelerometerUpdateMS / 1000.0f)];//this value is in seconds
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	
	isLayedOut = false;
	
	//by default, we are in portrait(upright) mode
	currentAngle = (M_PI / 2.0);
	
	bool multiTouch = dAtob( Con::getVariable( "$pref::iDevice::UseMultitouch" ) );
	platState.multipleTouchesEnabled = multiTouch;
	if( multiTouch ) {
		[self setMultipleTouchEnabled:YES];
	} else {
		[self setMultipleTouchEnabled:NO];
	}
	
	return self;
}


-(void)swapBuffers {
	if( isLayedOut == true ) { 
		//-Mat moved to layoutSubviews
//		[EAGLContext setCurrentContext:context];
//		glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);			
//		glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);

		[context presentRenderbuffer:GL_RENDERBUFFER_OES];
		
		//<Mat> added ClearColor to swapBuffers to see what parts of the screen are not being drawn to
		//glClearColor(0.0f, 0.2f, 0.35f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

	}
}

 

- (void)layoutSubviews {
	[EAGLContext setCurrentContext:context];

	//-Mat was in swapBuffers
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);			
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	[self destroyFramebuffer];
	[self createFramebuffer];
	isLayedOut = true;
}

-(void)rotateByAngle:(CGFloat)angle {
	
	CGAffineTransform transform = self.transform;

	transform = CGAffineTransformRotate(transform, angle);
	self.transform = transform;
}

-(void)rotateToAngle:(CGFloat)angle {
	CGFloat rotateAmount = (angle - currentAngle);//need to make this work better
	if( rotateAmount == 0 ) {
		return;
	}
	currentAngle = angle;
	[self rotateByAngle:rotateAmount];
}

-(void)centerOnPoint:(CGPoint)point {
	self.center = point;
}


- (BOOL)createFramebuffer {
	
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if (USE_DEPTH_BUFFER) {
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}
	
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}


- (void)destroyFramebuffer {
	
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	if(depthRenderbuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}



- (void)dealloc {
		
	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];	
	[super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
	//printf( "touchesBegan\n" );
	
	NSUInteger touchCount = 0;
	// Enumerates through all touch objects
	for (UITouch *touch in touches)
	{
		CGPoint point = [touch locationInView:self];
		// PUAP -Mat platform::init sets the window to 480x480 for easy rotation, which means this needs to be done to
		//keep the curser at the right point, if we are in landscape
		if( platState.portrait == false ) 
		{
			//point.y -= (480 - 320);	
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		//printf( "touchesBegan point:  (%f, %f)\n", point.x, point.y ); // RKS: I've added these printf statements to see why the input is off
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		createMouseDownEvent( touchCount, point.x, point.y );
		touchCount++;
	}
}



extern Vector<Event*> TouchMoveEvents;



- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{

	NSUInteger touchCount = 0;
	// Enumerates through all touch objects
	for (UITouch *touch in touches)
	{
		CGPoint point = [touch locationInView:self];
		// PUAP -Mat platform::init sets the window to 480x480 for easy rotation, which means this needs to be done to
		//keep the curser at the right point, if we are in landscape
		if( platState.portrait == false ) 
		{
			//point.y -= (480 - 320);	
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		//printf( "touchesMoved point:  (%f, %f)\n", point.x, point.y ); // RKS: I've added these printf statements to see why the input is off
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		createMouseMoveEvent( touchCount, point.x, point.y );
		touchCount++;
	}
	
 }

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
	
	NSUInteger touchCount = 0;
	// Enumerates through all touch objects
	for (UITouch *touch in touches)
	{
		CGPoint point = [touch locationInView:self];
		// PUAP -Mat platform::init sets the window to 480x480 for easy rotation, which means this needs to be done to
		//keep the curser at the right point, if we are in landscape
		if( platState.portrait == false ) 
		{
			//point.y -= (480 - 320);	
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		//printf( "touchesEnded point:  (%f, %f)\n", point.x, point.y ); // RKS: I've added these printf statements to see why the input is off
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		
		createMouseUpEvent( touchCount, point.x, point.y );
		touchCount++;

		//Luma: Tap support
		int tc = [touch tapCount];
		if (tc > 0) 
		{
			// this was a tap, so create a tap event
			createMouseTapEvent( tc, (int)point.x, (int)point.y );
		}		
	}
	
}

- (void)touchesChangedWithEvent:(UIEvent *)event {
	Con::printf( "Touches Changed with Event" );
}

- (void)touchesCancelled {
	Con::printf( "Touches Cancelled" );
}

bool gScreenAutoRotate = false;
int currentRotate = 0;
#define ROTATE_LEFT		0
#define ROTATE_RIGHT	1
#define ROTATE_UP		2

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration {

	UIAccelerationValue accel[3];
	if(platState.portrait){
		accel[0] = acceleration.x;//switch axises when in landscape mode	-y
		accel[1] = acceleration.y;//										x
	}
	else {
		accel[0] = -acceleration.y;
		accel[1] = acceleration.x;

	}
	accel[2] = acceleration.z;
	
	
#if 0 // FIXME: PUAP -Mat	INCOMPLETE	Screen rotate
	
	gScreenAutoRotate = dAtob( Con::getVariable( "$pref::iPhone::ScreenAutoRotate" ) );
	
	//Now check if we should autoRotate
	if( gScreenAutoRotate ) {	
		//acceleration.x is the side to side movement
		if( acceleration.x < -0.7 ) {
			//autoRotateLandscapeLeft();
			if( currentRotate != ROTATE_LEFT ) {
				[self rotateToAngle: (M_PI / 1.0 ) ];
				printf( "Left: %f \n", currentAngle );
				currentRotate = ROTATE_LEFT;
			}
		}
		if( acceleration.x > 0.7 ) {
			//autoRotateLandscapeRight();
			if( currentRotate != ROTATE_RIGHT ) {
				[self rotateToAngle: 0 ];
				printf( "Right: %f \n", currentAngle );
				currentRotate = ROTATE_RIGHT;
			}
		}
		if( acceleration.x > -0.3 && acceleration.x < 0.3 ) {
			//autoRotatePortrait();
			if( currentRotate != ROTATE_UP ) {
				[self rotateToAngle: (M_PI / 2.0 ) ];
				printf( "Up: %f \n", currentAngle  );
				currentRotate = ROTATE_UP;
			}
		}
	}
#endif	
	createAccelMoveEvent( accel );
}


@end


