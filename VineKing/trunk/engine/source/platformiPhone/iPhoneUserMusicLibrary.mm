//
//  iPhoneUserMusicLibrary.mm
//  iTorque2D
//
//  Created by Sven Bergstrom on 2010/05/10.
//  Copyright 2010 Luma Arcade. All rights reserved.
//

#ifdef TORQUE_ALLOW_MUSICPLAYER

#import "iPhoneUserMusicLibrary.h"
#import "MediaPlayer/MPMusicPlayerController.h"

#include "console/console.h"
#include "console/consoleTypes.h"

// Global master volume for stock music player
static F32 gMusicVolume = 0.5; 

@interface iPhoneUserMusicLibrary : NSObject {
	
	MPMusicPlayerController* musicPlayer;
	
}

@property (nonatomic, retain) MPMusicPlayerController *musicPlayer;

@end


@implementation iPhoneUserMusicLibrary

	@synthesize musicPlayer;

@end


static iPhoneUserMusicLibrary* iT2dMusicManager;
static bool iPhoneMusicManagerIsActive = false;

void createMusicPlayer()
{
	// Specify a media query; this one matches the entire iPod library because it
	// does not contain a media property predicate
	MPMediaQuery *everything = [[MPMediaQuery alloc] init];
	
	// Configure the media query to group its media items; here, grouped by artist
	[everything setGroupingType: MPMediaGroupingArtist];
	
	iT2dMusicManager = [[[iPhoneUserMusicLibrary alloc] init] retain]; 
	iT2dMusicManager.musicPlayer = [MPMusicPlayerController iPodMusicPlayer];
	
	[iT2dMusicManager.musicPlayer setQueueWithQuery: everything];
	
	Con::printf("iPhoneCreateMusicPlayer success, music player is enabled.");
	
	iPhoneMusicManagerIsActive = true;
	iT2dMusicManager.musicPlayer.volume = gMusicVolume;
	
	Con::addVariable("iPodMusicVolume", TypeF32, &gMusicVolume);
	Con::setVariable("$iPhoneMusicTrack", "Now Playing: ");
}

void destroyMusicPlayer()
{
    iT2dMusicManager.musicPlayer = nil;
    [iT2dMusicManager release];
    iT2dMusicManager = nil;
    
    iPhoneMusicManagerIsActive = false;
}

void updateVolume()
{
	if(iPhoneMusicManagerIsActive)
	{
		// Set the iT2DMusicManager.musicPlayer volume property
		iT2dMusicManager.musicPlayer.volume = gMusicVolume;
	}
}

ConsoleFunction(iPhoneCreateMusicPlayer, void, 1, 1, "")
{
	if(!iPhoneMusicManagerIsActive)
	{
		createMusicPlayer();
	}
	else
	{
		Con::printf("iPhoneCreateMusicPlayer was already called.");
	}

}

ConsoleFunction(iPhoneMusicPlay, void, 1, 1, "iPhoneMusicPlay()")
{
	if(iPhoneMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer play];
		
		MPMediaItem *currentItem = [iT2dMusicManager.musicPlayer nowPlayingItem];
		
		// Display the artist and song name for the now-playing media item
		NSString* output = [
								   NSString stringWithFormat: @"%@ %@ %@ %@",
								   NSLocalizedString (@"Now Playing:", @"Label for introducing the now-playing song title and artist"),
								   [currentItem valueForProperty: MPMediaItemPropertyTitle],
								   NSLocalizedString (@"by", @"Article between song name and artist name"),
								   [currentItem valueForProperty: MPMediaItemPropertyArtist]];
		//Hand it off to the compass variable in torque.
		const char* tOutput = [output UTF8String];
		Con::printf(tOutput);
		Con::setVariable("$iPhoneMusicTrack", tOutput );
	}
	else 
	{
		Con::printf("iPhoneMusicPlayer is not active, did you call iPhoneCreateMusicPlayer(); ? ");
	}

}

ConsoleFunction(iPhoneMusicPause, void, 1, 1, "iPhoneMusicPause()")
{
	if(iPhoneMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer pause];		
	}
	else 
	{
		Con::printf("iPhoneMusicPlayer is not active, did you call iPhoneCreateMusicPlayer(); ? ");
	}
	
}

ConsoleFunction(iPhoneMusicStop, void, 1, 1, "iPhoneMusicStop()")
{
	if(iPhoneMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer stop];
	}
	else 
	{
		Con::printf("iPhoneMusicPlayer is not active, did you call iPhoneCreateMusicPlayer(); ? ");
	}
	
}

ConsoleFunction(iPhoneMusicNext, void, 1, 1, "iPhoneMusicNext()")
{
	if(iPhoneMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer skipToNextItem];
		MPMediaItem *currentItem = [iT2dMusicManager.musicPlayer nowPlayingItem];
		
		// Display the artist and song name for the now-playing media item
		NSString* output = [
							NSString stringWithFormat: @"%@ %@ %@ %@",
							NSLocalizedString (@"Now Playing:", @"Label for introducing the now-playing song title and artist"),
							[currentItem valueForProperty: MPMediaItemPropertyTitle],
							NSLocalizedString (@"by", @"Article between song name and artist name"),
							[currentItem valueForProperty: MPMediaItemPropertyArtist]];
		//Hand it off to the compass variable in torque.
		const char* tOutput = [output UTF8String];
		Con::printf(tOutput);
		Con::setVariable("$iPhoneMusicTrack", tOutput );
	}
	else 
	{
		Con::printf("iPhoneMusicPlayer is not active, did you call iPhoneCreateMusicPlayer(); ? ");
	}
	
}

ConsoleFunction(iPhoneMusicPrevious, void, 1, 1, "iPhoneMusicPrevious()")
{
	if(iPhoneMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer skipToPreviousItem];
		MPMediaItem *currentItem = [iT2dMusicManager.musicPlayer nowPlayingItem];
		
		// Display the artist and song name for the now-playing media item
		NSString* output = [
							NSString stringWithFormat: @"%@ %@ %@ %@",
							NSLocalizedString (@"Now Playing:", @"Label for introducing the now-playing song title and artist"),
							[currentItem valueForProperty: MPMediaItemPropertyTitle],
							NSLocalizedString (@"by", @"Article between song name and artist name"),
							[currentItem valueForProperty: MPMediaItemPropertyArtist]];
		//Hand it off to the compass variable in torque.
		const char* tOutput = [output UTF8String];
		Con::printf(tOutput);
		Con::setVariable("$iPhoneMusicTrack", tOutput );
	}
	else 
	{
		Con::printf("iPhoneMusicPlayer is not active, did you call iPhoneCreateMusicPlayer(); ? ");
	}
	
}

#endif //TORQUE_ALLOW_MUSICPLAYER
