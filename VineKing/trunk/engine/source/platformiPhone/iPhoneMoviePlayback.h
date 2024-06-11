//
//  iPhoneVideoPlayback.h
//  iTorque 2D
//
//  Created by Sven Bergstrom, improved by Michael Perry
//  GarageGames, LLC. All rights reserved.
//

//These correspond to the docs over on Apples site : 
#ifdef TORQUE_ALLOW_MOVIEPLAYER

#import <MediaPlayer/MPMoviePlayerController.h>
#import <UIKit/UIDevice.h>
bool playiPhoneMovie(const char* fileName, const char* extension, MPMovieScalingMode scalingMode, MPMovieControlStyle controlStyle);
#endif//TORQUE_ALLOW_MOVIEPLAYER