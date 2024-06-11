//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


#include "platformiPhone/platformiPhone.h"
#include "platform/platformSemaphore.h"
#include "platform/platformVideo.h"
#include "platform/threads/thread.h"
#include "console/console.h"
#include "platformiPhone/iPhoneEvents.h"
#include "platform/nativeDialogs/msgBox.h"

#include "platformiPhone/iPhoneAlerts.h"


@implementation iPhoneAlertDelegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	buttonNumber = buttonIndex;
}

//Luma: Added delegate for dismissed call by system
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
	buttonNumber = buttonIndex;
}


- (void)didPresentAlertView:(UIAlertView *)alertView {
	
}

@end



bool iPhoneButtonBox(const char *windowTitle, const char *message, int numButtons = 0, NSString *buttons[] = nil, iPhoneAlertDelegate *delegate = nil) 
{

	UIAlertView *Alert =  [[UIAlertView alloc] initWithTitle: [NSString stringWithUTF8String: windowTitle]
													 message: [NSString stringWithUTF8String: message] 
													delegate: delegate 
										   cancelButtonTitle: nil
										   otherButtonTitles: nil ];
	
	if(numButtons > 0)
	{
		NSString *current = nil;
		for( int i = 1;  i < numButtons ; i++ ) 
		{
			current = buttons[i];
			[Alert addButtonWithTitle: current ];
		}
	}
	else 
	{
		[Alert addButtonWithTitle: @"OK" ];
	}

	
	[Alert show];
	
	// PUAP -Mat NOTE: NSRunLoop is not Thread-Safe, see documentation

	while (Alert.visible) 
	{
		[[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: [NSDate dateWithTimeIntervalSinceNow: 0.100]];
	}
	
	[Alert release];
	
	return true;
}



//-----------------------------------------------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
	iPhoneAlertDelegate *delegate = [[iPhoneAlertDelegate alloc] init];
	
	iPhoneButtonBox( windowTitle, message, 0, nil, delegate );
	
	[delegate release];
}
//-----------------------------------------------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{	
	iPhoneAlertDelegate *delegate = [[iPhoneAlertDelegate alloc] init];
	

	NSString *buttons[] = { @"OK", @"Cancel" };
	
	//Luma:	Need to pass the delegate in as well
	iPhoneButtonBox( windowTitle, message, 2, buttons, delegate );	
	
	//Luma: Zero is NOT the cancel button index... it is based on the order of the buttons in the above array
	bool returnValue = (delegate->buttonNumber != 1 );
	[delegate release];
	return returnValue;
}

//-----------------------------------------------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{//retry/cancel
	iPhoneAlertDelegate *delegate = [[iPhoneAlertDelegate alloc] init];
	
	//Luma:	Should be Retry / Cancel, not Cancel / Retry
	NSString *buttons[] = { @"Retry",
							@"Cancel",
						  };

	//Luma:	Need to pass the delegate in as well
	iPhoneButtonBox( windowTitle, message, 2, buttons, delegate );	
	
	//Luma: Zero is NOT the cancel button index... it is based on the order of the buttons in the above array
	bool returnValue = (delegate->buttonNumber != 1 );
	[delegate release];
	return returnValue;
}


bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{	
	iPhoneAlertDelegate *delegate = [[iPhoneAlertDelegate alloc] init];
	
	NSString *buttons[] = { @"Yes", @"No" };
	
	iPhoneButtonBox( windowTitle, message, 2, buttons, delegate );	
	bool returnValue = (delegate->buttonNumber != 1 );
	[delegate release];
	
	return returnValue;
}
