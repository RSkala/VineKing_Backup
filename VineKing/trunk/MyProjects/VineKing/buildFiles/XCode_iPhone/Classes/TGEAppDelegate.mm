//
//  TGEAppDelegate.m
//  TGE
//
//  Created by Puap Puap on 7/28/08.
//  Copyright Pick Up And Play 2008. All rights reserved.
//

#import "TGEAppDelegate.h"
#import "platformIPhone/iPhoneOGLVideo.h"

extern int iPhoneMain(int argc, const char **argv);
extern bool iPhoneRunGameLoop(void);

static int sArgc;
static char ** sArgv;

// application entry point
int main(int argc, char *argv[])
{
	NSAutoreleasePool * pool;
	pool = [[NSAutoreleasePool alloc] init];
	
	sArgc = argc;
	sArgv = argv;
	
	UIApplicationMain(argc, argv, nil, @"TGEAppDelegate");
	
	[pool release];
	
	return 0;
}

@implementation TGEAppDelegate

@synthesize window;
@synthesize glView;
@synthesize alertResult;

// for script debugging, in gdb use:  print [TGEAppDelegate conEval: "string"]
+ (const char *) conEval: (char *)str
{
	return Con::evaluate( str );
}

- (void) update
{
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (!iPhoneRunGameLoop()) {
		// exit app
		exit(0);
	} else {
		// schedule a little time away so iPhone has a chance to do other things
		[NSTimer scheduledTimerWithTimeInterval:(0.005) target:self selector:@selector(update) userInfo:nil repeats:NO];
	}
}

- (void) applicationDidFinishLaunching: (UIApplication*)application
{
	CGRect	rect = [[UIScreen mainScreen] bounds];
	
	// create a full-screen window
	window = [[UIWindow alloc] initWithFrame: rect];
	
	// create the OpenGL view and add it to the window
	//_glView = [[EAGLView alloc] initWithFrame:rect];
	glView = [[EAGLView alloc] initWithFrame:rect pixelFormat:GL_RGB565_OES depthFormat:GL_DEPTH_COMPONENT16_OES preserveBackbuffer:NO];
	
	[window addSubview:glView];
	
	// show the window
	[window makeKeyAndVisible];
	
   // Disable the idle timer (dims the screen after a period of inactivity).
   application.idleTimerDisabled = YES;

	iPhoneMain( sArgc, (const char **)sArgv );
	
	// create our rendering timer
	[NSTimer scheduledTimerWithTimeInterval: 1 / 30.0 target: self selector: @selector(update) userInfo: nil repeats: NO];
}

- (void)applicationWillResignActive: (UIApplication*)application
{
   application.idleTimerDisabled = NO;
}

- (void)applicationDidBecomeActive:(UIApplication*)application
{
   application.idleTimerDisabled = YES;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	printf("Terminating application\n");

	[glView release];
	[window release];
	glView = nil;
	window = nil;
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
	printf("Received Memory Warning!\n");
}

- (void)applicationSignificantTimeChange:(UIApplication *)application {
}

/* --------------------------------------------------------------
  Common alertview code for popping up dialogbox
-------------------------------------------------------------- */

- (void) alert: (NSString*)titleStr msg: (NSString*)msgStr okBtn: (NSString*)okBtn cancelBtn: (NSString*)cancelBtn
{
	// Don't show the alert if the app is shutting down.
	if (window == nil) {
		alertResult = 0;
		return;
	}
	
	if ((cancelBtn == nil) || ([cancelBtn length] == 0)) {
		// open an alert with just an OK button
		alert = [[UIAlertView alloc] initWithTitle: titleStr message: msgStr
						delegate: self cancelButtonTitle: okBtn otherButtonTitles: nil];
	} else {
		// open an alert with just an OK button
		alert = [[UIAlertView alloc] initWithTitle: titleStr message: msgStr
						delegate: self cancelButtonTitle: cancelBtn otherButtonTitles: okBtn, nil];
	}

	[alert show];

	while (alert != nil) {
		[[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: [NSDate distantFuture]];
	}
}

- (void)alertView: (UIAlertView *)alertView willDismissWithButtonIndex: (NSInteger)buttonIndex
{
	alertResult = buttonIndex;
	[alert release];
	alert = nil;
}

- (void)setGlView: (EAGLView*)value
{
	[glView removeFromSuperview];
	[glView release];

	if (value != nil) {
		glView = [value retain];
		[window addSubview: glView];
	}
}

- (void)dealloc {
	[super dealloc];
}

@end
