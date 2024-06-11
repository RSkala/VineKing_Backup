//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// The intent of this object is that it will allow iPhone features to be used
// by various parts of the torque iPhone platform, without having to include
// the <UIKit/UIKit.h> header(s), or any extra Torque headers.

#import <UIKit/UIKit.h>

#import "platformiPhone/Facebook/FBConnect.h" // RKS NOTE: I added this for Facebook integration - 2011/07/2

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@interface TGBAppDelegate : NSObject <UIApplicationDelegate, FBSessionDelegate, FBRequestDelegate, FBDialogDelegate/*, SA_OAuthTwitterControllerDelegate*/> 
{
	IBOutlet UIWindow *window;
	
	Facebook* m_pFacebook;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) Facebook* m_pFacebook;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Inherited from NSObject
- (id)init; // RKS NOTE: I added this for getting the delegate - 2011/08/05

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// RKS NOTE: I added this for Facebook integration - 2011/07/31; Function mention here: https://developers.facebook.com/docs/guides/mobile/
- (BOOL) application:	(UIApplication *)application 
	     handleOpenURL:	(NSURL *)url;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//- (void) doSomeTwitterShit;
//- (bool) isTwitterEngineInitialized;
//- (void) initializeTwitterEngine;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@end // @interface TGBAppDelegate
