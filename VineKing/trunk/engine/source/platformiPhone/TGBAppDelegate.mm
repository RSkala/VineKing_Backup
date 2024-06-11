//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//????

#import "TGBAppDelegate.h"
#include "platform/platformInput.h"

#if defined(_USE_SOCIAL_NETWORKING)
#include "platformiPhone/iPhoneSocialNetworkingManager.h"
#endif // _USE_SOCIAL_NETWORKING

#ifdef TORQUE_ALLOW_ORIENTATIONS
#include "platformiPhone/iPhoneUtil.h"
#endif //TORQUE_ALLOW_ORIENTATIONS

#include "iPhoneInterfaceWrapper.h" // RKS NOTE: This was added for FacebookWrapper
#include "ComponentGlobals.h" // RKS NOTE: This was added for getting the Device Type to disable Facebook on old devices

//-------------------------------------------------------------------------------------------------------------------------------------------------

extern int _iPhoneRunTorqueMain( id appID,  UIView *Window, UIApplication *app );
extern void _iPhoneGameInnerLoop();
extern void _iPhoneGameResignActive();
extern void _iPhoneGameBecomeActive();
extern void _iPhoneGameWillTerminate();

bool _iPhoneTorqueFatalError = false;

//// RKS NOTE: Keys for Twitter integration
//#define kOAuthConsumerKey				@"CpYgdUHWkDkQ9PnknhJGRg"
//#define kOAuthConsumerSecret			@"bbRqk7sdmc0hGr9vvqMoDs07WV2tiuXTOhnR70D1RHU"

//-------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef TORQUE_ALLOW_ORIENTATIONS
//Luma: Store current orientation for easy access
extern void _iPhoneGameChangeOrientation(S32 newOrientation);
#endif //TORQUE_ALLOW_ORIENTATION

//-------------------------------------------------------------------------------------------------------------------------------------------------

@implementation TGBAppDelegate

//-------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef TORQUE_ALLOW_ORIENTATIONS
UIDeviceOrientation currentOrientation;
#endif //TORQUE_ALLOW_ORIENTATIONS

//-------------------------------------------------------------------------------------------------------------------------------------------------

@synthesize window;
@synthesize m_pFacebook; // RKS NOTE: I added this on 2011/07/25 for Facebook integration (this creates 'getter' and 'setter' methods).

//-------------------------------------------------------------------------------------------------------------------------------------------------
// Facebook iOS tutorial: https://developers.facebook.com/docs/guides/mobile/#ios

NSString* g_kFacebookAppId = @"202917763091368"; // The Facebook AppID is created here: https://developers.facebook.com/apps

Facebook* g_pFacebook = nil; // RKS NOTE: This is used for externing the Facebook member

TGBAppDelegate* g_pTGBAppDelegate = nil; // RKS NOTE: This is for getting the delegate
NSString* g_pFacebookNameString = @"";
bool g_bFacebookNameSet = false;

//-------------------------------------------------------------------------------------------------------------------------------------------------
void _RequestFacebookUserName()
{
	printf( "_RequestFacebookUserName()\n" );
	
	if( g_pFacebook == nil || g_pTGBAppDelegate == nil )
		return;
	
	//[g_pFacebook requestWithGraphPath:@"me?fields=id,name" andDelegate:[g_pFacebook self]];
	[g_pFacebook requestWithGraphPath:@"me?fields=id,name" andDelegate:g_pTGBAppDelegate];
	//[g_pFacebook requestWithGraphPath:@"me?fields=name" andDelegate:g_pTGBAppDelegate];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

- (void)applicationDidFinishLaunching:(UIApplication *)application {

    //printf( "%s\n", __FUNCTION__ );
    
	_iPhoneTorqueFatalError = false;
	if(!_iPhoneRunTorqueMain( self, window, application ))
	{
		_iPhoneTorqueFatalError = true;
		return;
	};
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // 2014/02/13
    //[[UIApplication sharedApplication] setStatusBarHidden:YES];
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
    //[[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleBlackOpaque];
    
    UIWindow* frontWindow = [[UIApplication sharedApplication] keyWindow];
    NSLog( @"keyWindow SIZE: %f X %f",  frontWindow.frame.size.width, frontWindow.frame.size.height );
    NSLog( @"keyWindow POS:  (%f, %f)", frontWindow.frame.origin.x, frontWindow.frame.origin.y );
    //frontWindow.frame = CGRectMake( -100, 20, frontWindow.frame.size.width, frontWindow.frame.size.height );
    NSLog( @"Main Screen Bounds: %f X %f", [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height );
    
    CGFloat frontWinHeight = frontWindow.frame.size.height;
    CGFloat deviceHeight = [UIScreen mainScreen].bounds.size.height;
    
    NSLog( @"frontWinHeight: %f", frontWinHeight );
    NSLog( @"deviceHeight:   %f", deviceHeight );
    NSLog( @"DIFFERENCE:     %f", deviceHeight - frontWinHeight );
    CGFloat halfDist = fmaxf( ([UIScreen mainScreen].bounds.size.height - frontWindow.frame.size.height) / 2.0, 0 );
    NSLog( @"halfDist: %f", halfDist );
    
    frontWindow.frame = CGRectMake( 0, halfDist, frontWindow.frame.size.width, frontWindow.frame.size.height );
    //frontWindow.frame = CGRectMake( 0, )
            
//    UILabel* achievementDescription = [[UILabel alloc] initWithFrame:CGRectMake(0,0,widthConstraint,40)];
//    achievementDescription.frame = CGRectMake(achievementImage.frame.origin.x - widthConstraint,
//        achievementTitle.frame.origin.y + achievementTitle.frame.size.height,
//        achievementDescription.frame.size.width,
//        achievementDescription.frame.size.height);
    //////////////////////////////////////////////////////////////////////////////////////////

    if( CComponentGlobals::GetInstance().IsOldIOSDeviceType() == false )
    {
        // Create the Facebook instance
        m_pFacebook = [[Facebook alloc] initWithAppId: g_kFacebookAppId]; 
        if( m_pFacebook != nil )
        {
            // Check User Defaults
            NSUserDefaults* pUserDefaults = [NSUserDefaults standardUserDefaults];
            if( pUserDefaults )
            {
                if( [pUserDefaults objectForKey:@"FBAccessTokenKey"] && [pUserDefaults objectForKey:@"FBExpirationDateKey"] )
                {
                    m_pFacebook.accessToken		= [pUserDefaults objectForKey:@"FBAccessTokenKey"];
                    m_pFacebook.expirationDate	= [pUserDefaults objectForKey:@"FBExpirationDateKey"];
                    
                    NSLog( @"m_pFacebook.accessToken: %@", m_pFacebook.accessToken );
                }
            }
            
            // Check for a valid Facebook session
            //if( [m_pFacebook isSessionValid] == NO )
            //	[m_pFacebook authorize:nil delegate:self];
            
            g_pFacebook = m_pFacebook;
        }
        else
        {
            printf( "Failure to create Facebook instance.\n" );
        }
    }
	
#ifdef TORQUE_ALLOW_ORIENTATIONS
	
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	//Also we set the currentRotation up so its not invalid
	currentOrientation = [UIDevice currentDevice].orientation;
	//So we make a selector to handle that, called didRotate (lower down in the code)
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didRotate:)
												 name:UIDeviceOrientationDidChangeNotification
											   object:nil];	
#endif //TORQUE_ALLOW_ORIENTATIONS
	
#if defined(_USE_SOCIAL_NETWORKING)
    socialNetworkingInit(self);
#endif //_USE_SOCIAL_NETWORKING
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

- (void)applicationWillResignActive:(UIApplication *)application 
{
	_iPhoneGameResignActive();	
    
#if defined(_USE_SOCIAL_NETWORKING)
    socialNetworkingManager->socialNetworkingToResignActive();
#endif //_USE_SOCIAL_NETWORKING
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

- (void)applicationDidBecomeActive:(UIApplication *)application 
{
	if(!_iPhoneTorqueFatalError)
	{
		_iPhoneGameBecomeActive();
        
#if defined(_USE_SOCIAL_NETWORKING)
        socialNetworkingManager->socialNetworkingBecameActive();
#endif //_USE_SOCIAL_NETWORKING
	}
	else 
	{
		//Luma : Tell us why, and then quit
		//Note, engine has shutdown internally already!
		//Another note, Alerts get hidden/broken by default.png not being "finished" yet, causing a "freeze" in the alert run loop
		exit(0);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
// RKS NOTE: I added these methods to see if or when they are called. Supposedly, four functions are new to OS 4:
//				applicationWillResignActive:		(Handled above)
//				applicationDidBecomeActive:			(Handled above)
//				applicationDidEnterBackground:		(I handle below)
//				applicationWillEnterForeground:		(I handle below)
//-------------------------------------------------------------------------------------------------------------------------------------------------

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	printf( "applicationDidEnterBackground called\n" );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	printf( "applicationWillEnterForeground called\n" );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//Luma:	pick up memory warnings
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application 
{
	printf("\n\nMEMORY WARNING: Received Memory Warning from iPhone OS!\n");
}


- (void)applicationWillTerminate:(UIApplication *)application 
{
	printf( "applicationWillTerminate called\n" );
	_iPhoneGameWillTerminate();
	
#ifdef TORQUE_ALLOW_ORIENTATIONS
	[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
#endif //TORQUE_ALLOW_ORIENTATIONS

#if defined(_USE_SOCIAL_NETWORKING)
    socialNetworkingCleanup();
#endif //_USE_SOCIAL_NETWORKING
}

#ifdef TORQUE_ALLOW_ORIENTATIONS
- (void)didRotate:(NSNotification *)notification
{	
	//Default to landscape left
	S32 iPhoneGameOrientation = iPhoneOrientationLandscapeLeft;
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	if(currentOrientation != orientation)
	{
		switch(orientation)
		{
			case UIDeviceOrientationLandscapeLeft:		//back button on the right
				iPhoneGameOrientation = iPhoneOrientationLandscapeLeft;
				break;
			case UIDeviceOrientationLandscapeRight:		//back button to the left
				iPhoneGameOrientation = iPhoneOrientationLandscapeRight;
				break;
			case UIDeviceOrientationPortrait:			//back button at the bottom
				iPhoneGameOrientation = iPhoneOrientationPortrait;
				break;
			case UIDeviceOrientationPortraitUpsideDown: //back button at the top
				iPhoneGameOrientation = iPhoneOrientationPortraitUpsideDown;
				break;
		}
		
		//Change the orientation
		currentOrientation = orientation;
		//Tell the rest of the engine
		_iPhoneGameChangeOrientation(iPhoneGameOrientation);
	}
}
#endif //TORQUE_ALLOW_ORIENTATIONS

- (void) runMainLoop 
{
	_iPhoneGameInnerLoop();
}

- (void)dealloc 
{
	[window release];
	[super dealloc];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// RKS NOTE: I added this so we could get the delegate
- (id)init
{
	g_pTGBAppDelegate = self;
	
	return [super init];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// RKS NOTE: I added this for Facebook integration 2011/07/31

- (BOOL) application:	(UIApplication *)application 
		 handleOpenURL:	(NSURL *)url
{
	NSLog( @"application: handleOpenURL: %@", [url absoluteString] );
	
	if( m_pFacebook )
	{
		return [m_pFacebook handleOpenURL: url];
	}
	
	return NO;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Inherited from FBRequestDelegate - RKS NOTE: I added this for Facebook integration 2011/08/04

#pragma mark -
#pragma mark Methods from FBRequestDelegate (FaceBook)

- (void) request:	(FBRequest *)request 
		 didLoad:	(id)result
{
	NSLog( @"Result: %@", result );
	
	// http://developers.facebook.com/docs/reference/api/user/
	// http://stackoverflow.com/questions/6690999/getting-facebook-username-in-iphone-app
	// http://stackoverflow.com/questions/5170735/how-to-retrieve-facebook-response-using-facebook-iphone-sdk/5195951#5195951
	
	
	NSDictionary* pUserInfo = (NSDictionary*)result;
	
	NSString* pUserIDString		= [pUserInfo objectForKey:@"id"];
	NSString* pUserNameString	= [pUserInfo objectForKey:@"name"];
	
	
	int aaaa = 0;
	aaaa = 0;
	
	NSLog( @"Facebook user name: %@", pUserNameString );
	NSLog( @"Facebook user id:   %@", pUserIDString );
	
	
    if( [result isKindOfClass:[NSDictionary class]] )
	{
		NSLog( @" - result is NSDictionary" );
		
        //NSDictionary *hash = result;
        //NSLog( @"Birthday: %@", [result objectForKey:@"birthday"]	);
		NSLog( @"ID:   %@",	[result objectForKey:@"id"]		);
        NSLog( @"Name: %@",	[result objectForKey:@"name"]	);
		
		g_pFacebookNameString = [result objectForKey:@"name"];
		g_bFacebookNameSet = true;
		
		NSLog( @"g_pFacebookNameString: %@", g_pFacebookNameString );
		
		//NSString* g_pFacebookNameString = "";
		//bool g_bFacebookNameSet = false;
    }

	aaaa = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// RKS NOTE: I added these methods 2011/08/05 - INHERITED FROM FBDialogDelegate

#pragma mark -
#pragma mark Methods from FBDialogDelegate (FaceBook)

// Called when the dialog succeeds and is about to be dismissed.
- (void) dialogDidComplete: (FBDialog *)dialog
{
	NSLog( @"Facebook::dialogDidComplete:" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Called when the dialog succeeds with a returning url.
- (void) dialogCompleteWithUrl: (NSURL *)url
{
	NSLog( @"Facebook::dialogCompleteWithUrl:" );
	
	// RKS NOTE: I added this to check whether the user pressed "SKIP" (instead of "SHARE").
	// - Information from here: http://stackoverflow.com/questions/5296888/how-to-check-whether-user-has-click-on-skip-button-or-publish-button-in-fbconnect
	
	NSString* urlString = [[NSString alloc] initWithString: [url absoluteString]];
	if( [urlString isEqualToString:@"fbconnect://success"] == YES )
	{
		// If there is no post id, then the user pressed SKIP (Otherwise, the URL will look something like: fbconnect://success/?post_id=12345)
		NSLog( @"USER PRESSED SKIP" );
	}
	else
	{
		NSLog( @"USER PRESSED SHARE -- TODO -- CALL ONSUCCESSFULFACEBOOKPOST" );
		FacebookWrapper::OnSuccessfulFacebookPost();
	}
	
	[urlString release];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Called when the dialog get canceled by the user.

- (void) dialogDidNotCompleteWithUrl: (NSURL *)url
{
	NSLog( @"Facebook::dialogDidNotCompleteWithUrl:" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Called when the dialog is cancelled and is about to be dismissed.
- (void) dialogDidNotComplete: (FBDialog *)dialog
{
	NSLog( @"Facebook::dialogDidNotComplete:" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Called when dialog failed to load due to an error.
- (void) dialog:(FBDialog*)dialog didFailWithError:(NSError *)error
{
	NSLog( @"Facebook::dialog:didFailWithError:" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

@end

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
