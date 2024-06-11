//
//  FacebookAPIAppDelegate.h
//  iTorque2D
//
//  Created by Richard Skala on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

// Used tutorial at: http://www.mobisoftinfotech.com/blog/iphone/iphone-fbconnect-facebook-connect-tutorial/

#import <UIKit/UIKit.h>
#import "FBConnect.h"
#import "FBSession.h"
//#import "FBConnect/FBConnect.h"
//#import "FBConnect/FBSession.h"

@class FacebookAPIViewController;

@interface FacebookAPIAppDelegate : NSObject  
{
	UIWindow *window;
	FacebookAPIViewController *viewController;
	FBSession *_session;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet
FacebookAPIViewController *viewController;
@property (nonatomic,retain) FBSession *_session;
@end
