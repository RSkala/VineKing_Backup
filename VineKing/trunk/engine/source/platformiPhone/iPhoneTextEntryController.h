//
//  iPhoneTextEntryController
//  iTorque 2D
//
//  Created by Sven Bergstrom 
//  GarageGames, LLC. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface iPhoneTextEntryController : UIViewController
{
	UITextField* textField;
	bool finished;
	bool userCanceled;
}

@property(nonatomic, readonly) IBOutlet UITextField* textField;
@property(nonatomic, readonly) bool finished;
@property(nonatomic, readonly) bool userCanceled;

- (IBAction)onCancelClicked: (id)sender;
- (IBAction)onCommitClicked: (id)sender;
- (IBAction)onEditingChanged: (id)sender;

- (void)onFinished: (id)sender;

@end
