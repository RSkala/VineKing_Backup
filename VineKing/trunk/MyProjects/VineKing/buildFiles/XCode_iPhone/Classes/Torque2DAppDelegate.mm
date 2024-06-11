//
//  Torque2DAppDelegate.m
//  Torque2D
//
//  Created by puap on 7/28/08.
//  Copyright  PUAP 2008. All rights reserved.
//

#import "Torque2DAppDelegate.h"
#import "platformiPhone/iPhoneWindow.h"

iPhoneGLWindow *glView;

@implementation Torque2DAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(UIApplication *)application {

	glView = [[iPhoneGLWindow alloc] initWithFrame: window.bounds];
	[window addSubview: glView];
	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 60.0;
}

- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
