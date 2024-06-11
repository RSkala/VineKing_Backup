//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#import "iPhoneInterfaceWrapper.h"

#include <sys/types.h>
#include <sys/sysctl.h>

#import <UIKit/UIActivityIndicatorView.h>
#import <UIKit/UIDevice.h>

#import "platformiPhone/Facebook/Facebook.h"

#import "GameCenterWrapper.h"

#import "TGBAppDelegate.h"

#include "component/ComponentMainMenuScreen.h"

//using namespace ActivityIndicatorWrapper;
//using namespace DeviceTypeWrapper;

UIActivityIndicatorView* g_pActivityIndicator = nil;

// Parameters for Facebook post
NSString* g_pFacebookPostLink				= @"http://pixelvandals.com/vineking";									// Link from 'name'
NSString* g_pFacebookPostAppImage			= @"http://pixelvandals.com/wp-content/uploads/2011/06/icon_72.png";	// Picture that appears
NSString* g_pFacebookPostName				= @"VineKing - for iPhone, iPod Touch, and iPad";						// Post Title
//NSString* g_pFacebookPostCaption			= @"for iPhone, iPod Touch, and iPad.";									// Caption underneath name
NSString* g_pFacebookPostDescription		= @"Don't have VineKing? Get it now on the App Store for Free!";		// Message underneath caption
//NSString* g_pFacebookPostDefaultMessage	= @"";																	// Default message in text edit box (user can edit this).

extern Facebook* g_pFacebook;
extern NSString* g_kFacebookAppId;
extern TGBAppDelegate* g_pTGBAppDelegate;
extern void _RequestFacebookUserName();


//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// ActivityIndicatorWrapper

void ActivityIndicatorWrapper::CreateIndicator()
{
	if( g_pActivityIndicator != nil )
	{
		return;
	}
	
	g_pActivityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleWhiteLarge];
	if( g_pActivityIndicator )
	{
		g_pActivityIndicator.center = CGPointMake( 160, 282 ); //( 288, 448 ); // ( 160, 282 ); //( 160, 240 ); // Position of Activity Icon
		g_pActivityIndicator.hidesWhenStopped = YES;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void ActivityIndicatorWrapper::DeleteIndicator()
{
	if( g_pActivityIndicator == nil )
		return;
	
	[g_pActivityIndicator release];
	g_pActivityIndicator = nil;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void ActivityIndicatorWrapper::ShowIndicator()
{
	if( g_pActivityIndicator == nil )
		return;
	
	UIWindow* pWindow = [UIApplication sharedApplication].keyWindow;
	if( pWindow )
	{
		[pWindow addSubview: g_pActivityIndicator];
		[g_pActivityIndicator startAnimating];
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void ActivityIndicatorWrapper::HideIndicator()
{
	if( g_pActivityIndicator == nil )
		return;
	
	[g_pActivityIndicator stopAnimating];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// DeviceTypeWrapper

DeviceTypeWrapper::EIOSDeviceType DeviceTypeWrapper::GetIOSDeviceType()
{
	// Set 'oldp' parameter to NULL to get the size of the data returned so we can allocate appropriate amount of space.
	size_t size;
	sysctlbyname( "hw.machine", NULL, &size, NULL, 0 );
	char* pszDeviceType = reinterpret_cast<char*>( malloc( size ) );
	   
	sysctlbyname( "hw.machine", pszDeviceType, &size, NULL, 0 );
	
	//printf( "Device Type: %s\n", pszDeviceType );
	
	DeviceTypeWrapper::EIOSDeviceType eDeviceType = ConvertStringToIOSDeviceTypeEnum( pszDeviceType );
	
	free( pszDeviceType );
	
	return eDeviceType;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

DeviceTypeWrapper::EIOSDeviceType DeviceTypeWrapper::ConvertStringToIOSDeviceTypeEnum( const char* _pszDeviceType )
{  
	// "i386"		- iPhone Simulator    
	// "iPhone1,1"	- iPhone Original
	// "iPhone1,2"	- iPhone 3G    
	// "iPhone2,1"	- iPhone 3GS
	// "iPhone3,1"	- iPhone 4
	// "iPod1,1"	- iPod Touch 1st Gen
	// "iPod2,1"	- iPod Touch 2nd Gen
	// "?"			- iPod Touch 3rd Gen
	// "?"			- iPad
	// "?"			- iPad 2
	
	if( _pszDeviceType == NULL )
		return DeviceTypeWrapper::DEVICE_TYPE_NONE;
	
	
	if( strcmp( _pszDeviceType, "i386" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_SIMULATOR; 
	}
	else if( strcmp( _pszDeviceType, "iPhone1,1" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_IPHONE_ORIGINAL;
	}
	else if( strcmp( _pszDeviceType, "iPhone1,2" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G;
	}
	else if( strcmp( _pszDeviceType, "iPhone2,1" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3GS;
	}
	else if( strcmp( _pszDeviceType, "iPhone3,1" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_IPHONE_4;
	}
	else if( strcmp( _pszDeviceType, "iPod1,1" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN;
	}
	else if( strcmp( _pszDeviceType, "iPod2,1" ) == 0 )
	{
		return DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN;
	}
	//else if( strcmp( _pszDeviceType, "?????" ) == 0 )
	//{
	//	return DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_3RD_GEN;
	//}
	//else if( strcmp( _pszDeviceType, "?????" ) == 0 )
	//{
	//	return DeviceTypeWrapper::DEVICE_TYPE_IPAD;
	//}
	//else if( strcmp( _pszDeviceType, "?????" ) == 0 )
	//{
	//	return DeviceTypeWrapper::DEVICE_TYPE_IPAD_2;
	//}
	
	return DeviceTypeWrapper::DEVICE_TYPE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void AppStoreLinkWrapper::SendUserToVineKingInAppStore()
{
	// Info from:	http://developer.apple.com/library/ios/#qa/qa1629/_index.html
	//  and:		http://stackoverflow.com/questions/433907/how-to-link-to-apps-on-the-app-store
	
	// NSString *iTunesLink = @"http://phobos.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=284417350&mt=8";
	// [[UIApplication sharedApplication] openURL:[NSURL URLWithString:iTunesLink]];
	
	//[[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"itms://itunes.com/apps/appname"]];
	//[[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"itms://itunes.com/apps/developername"]];
	
	//NSString* iTunesLink = @"itms://itunes.com/apps/vineking";
	//NSString* iTunesLink = @"itms://itunes.apple.com/us/app/vineking/id420324848?mt=8";
	NSString* iTunesLink = @"http://itunes.apple.com/us/app/vineking/id420324848?mt=8";
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:iTunesLink]];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void AppStoreLinkWrapper::SendUserToVineKingRatingsPage()
{
	// Info from: http://www.garagegames.com/community/forums/viewthread/126856/1#comment-811413
	
	NSString* iTunesRatingsLink = @"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=420324848";
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:iTunesRatingsLink]];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void AppStoreLinkWrapper::SendUserToVineKingWebSite()
{
	NSString* VineKingWebSiteLink = @"http://www.pixelvandals.com/vineking";
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:VineKingWebSiteLink]];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void AppStoreLinkWrapper::SendUserToVineKingFacebookPage()
{
	NSString* VineKingFaceBookLink = @"http://www.facebook.com/pages/Pixel-Vandals-Inc/148448825227646";
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:VineKingFaceBookLink]];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void AppStoreLinkWrapper::SendUserToVineKingTwitterPage()
{
	NSString* VineKingTwitterLink = @"http://www.twitter.com/pixelvandals";
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:VineKingTwitterLink]];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// FACEBOOK WRAPPER

void FacebookWrapper::OpenFeedDialog( const char* _pszMessage )
{
	if( _pszMessage == nil )
		return;
	
//	NSString* pPostDescription;
//	
//	if( _pszMessage != nil )
//		pPostDescription = [[NSString alloc] initWithCString: _pszMessage encoding: NSASCIIStringEncoding];
//	else
//		pPostDescription = [[NSString alloc] initWithString: g_pFacebookPostDefaultDescription];
	
	//g_pFacebook.name
	
	//[facebook requestWithGraphPath:@"me" andDelegate:self];             // get information about the currently logged in user
	//[facebook requestWithGraphPath:@"platform/posts" andDelegate:self]; // get the posts made by the "platform" page
	//[facebook requestWithGraphPath:@"me/friends" andDelegate:self];     // get the logged-in user's friends
	//[g_pFacebook requestWithGraphPath:@"me" andDelegate:[g_pFacebook self];
	//[facebook requestWithGraphPath:@"me?fields=id,name" andDelegate:self];
	
	//_RequestFacebookUserName();
	
	//NSString* pPostMessageString = [[NSString alloc] initWithCString:_pszMessage encoding:NSASCIIStringEncoding];
	NSString* pPostCaptionString = [[NSString alloc] initWithCString:_pszMessage encoding:NSASCIIStringEncoding];
	[pPostCaptionString autorelease];
	
	//NSLog( @"pPostMessageString: %@", pPostMessageString );
	
	// Dialog info from here: https://developers.facebook.com/docs/reference/dialogs/
	//						  https://developers.facebook.com/docs/reference/dialogs/feed/
	
	NSMutableDictionary* pParams = [NSMutableDictionary dictionaryWithObjectsAndKeys:
									g_kFacebookAppId,			@"app_id",		// Facebook App ID
									g_pFacebookPostAppImage,	@"picture",		// Picture that appears
									g_pFacebookPostName,		@"name",		// Post Title
									g_pFacebookPostLink,		@"link",		// Link from 'name'
									pPostCaptionString,			@"caption",		// Caption underneath name
									g_pFacebookPostDescription,	@"description",	// Message underneath caption
									//pPostMessageString,		@"message",		// Default words in text edit box (DEPRECATED - This field is now ignored by Facebook)
									nil];
	
	if( g_pFacebook )
	{
		[g_pFacebook dialog:@"feed" andParams:pParams andDelegate:g_pTGBAppDelegate];
	}
	
	//[pPostMessageString release];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool FacebookWrapper::IsLoggedIn()
{
	//printf( "FacebookWrapper::IsLoggedIn\n" );
	
	if( g_pFacebook == nil )
		return NO;
	
	return [g_pFacebook isSessionValid];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool FacebookWrapper::LogIn()
{
	//printf( "FacebookWrapper::LogIn\n" );
	
	if( g_pFacebook == nil )
		return NO;
	
    return NO;
	//[g_pFacebook authorize:nil delegate:[g_pFacebook self]];
	
	//return YES;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool FacebookWrapper::LogOut()
{
	//printf( "FacebookWrapper::LogOut\n" );
	
	if( g_pFacebook == nil )
		return NO;
	
	//[g_pFacebook logout:[g_pFacebook self]];
	FacebookWrapper::RequestFacebookUserName();
	
	return YES;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void FacebookWrapper::OnLogIn()
{
	//printf( "FacebookWrapper::OnLogIn\n" );
	
	CComponentMainMenuScreen::OnFacebookUserLoggedIn();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void FacebookWrapper::OnLogOut()
{
	//printf( "FacebookWrapper::OnLogOut\n" );
	
	CComponentMainMenuScreen::OnFacebookUserLoggedOut();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void FacebookWrapper::RequestFacebookUserName()
{
	//[facebook requestWithGraphPath:@"me?fields=id,name" andDelegate:self]; 
	//if( g_pFacebook == nil )
	//	return;
	
	//[g_pFacebook requestWithGraphPath:@"me?fields=id,name" andDelegate:[g_pFacebook self]];
	
	_RequestFacebookUserName();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void FacebookWrapper::OnSuccessfulFacebookPost()
{
	//printf( "FacebookWrapper::OnSuccessfulFacebookPost\n" );
	
	if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_FACEBOOK ) == false )
	{
		GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_FACEBOOK );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void TwitterWrapper::OpenTwitterDialog()
{
    //printf( "TwitterWrapper::OpenTwitterDialog\n" );
    
//    g_pTwitterViewController = [[VineKingTwitterViewController alloc] init];
//    if( g_pTwitterViewController != nil )
//    {
//        UIWindow* window = [UIApplication sharedApplication].keyWindow;
//        [window addSubview: g_pTwitterViewController.view];
//        [g_pTwitterViewController initializeTwitterEngine];
//    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

