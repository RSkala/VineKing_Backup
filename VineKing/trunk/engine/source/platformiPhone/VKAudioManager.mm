//
//  VKAudioManager.m
//  iTorque2D
//
//  Created by Richard Skala on 2/12/14.
//
//

#import "VKAudioManager.h"
//#include <cstdio.h>
//#include <stdio.h>

//---------------------------------------------------------------------------------------------------------------------

//const struct AudioManager AudioManager = {
//	.musicKey = @"OptionsScreen.musicKey",
//	.voiceKey = @"OptionsScreen.voiceKey",
//	.effectsKey = @"OptionsScreen.effectsKey",
//};
// USED AS SUCH:
//NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
//
//id value = [defaults valueForKey:AudioManager.effectsKey];
//NSLog(@"effectsKey: %@", value);
//if (!value) {
//    [defaults setBool:YES forKey:AudioManager.effectsKey];
//}
//
//const struct AudioKeys AudioKeys = {
//    .key1 = @"key1",
//    .key2 = @"key2",
//    .key3 = @"key3",
//};

//---------------------------------------------------------------------------------------------------------------------

static const CGFloat DEFAULT_BGM_VOLUME = 0.8f;
static const CGFloat DEFAULT_SFX_VOLUME = 0.8f;

static const NSUInteger MAX_PLAY_QUEUES = 10;
//static const NSString* kPlayQueueString = @"com.pixelvandals.vineking";
static const char kPlayQueueString[32] = "com.pixelvandals.vineking.sound";

static dispatch_queue_t dispatchQueues[MAX_PLAY_QUEUES];

//static NSString* const AUDIO_TYPE_WAV = @"wav";
static NSString* const AUDIO_TYPE_CAF = @"caf";
static NSString* const VK_AUDIO_TYPE = AUDIO_TYPE_CAF;

//static NSString* const AUDIO_DIR_WAV = @"game/data/audio";
static NSString* const AUDIO_DIR_CAF = @"audio_caf";
static NSString* const VK_AUDIO_DIR = AUDIO_DIR_CAF;

//---------------------------------------------------------------------------------------------------------------------

@implementation VKAudioManager

//---------------------------------------------------------------------------------------------------------------------

@synthesize isAudioInitialized;
@synthesize isSoundMuted;

@synthesize playQueue;
@synthesize playQueue01;
@synthesize playQueue02;
@synthesize playQueue03;
@synthesize playQueue04;
@synthesize playQueue05;
@synthesize playQueue06;
@synthesize playQueue07;
@synthesize playQueue08;
@synthesize playQueue09;
@synthesize playQueue10;

@synthesize bgmMainMenuRPG;
@synthesize bgmMapScreen;
@synthesize bgmInGame;
@synthesize bgmBoss;
@synthesize bgmYouWin;
@synthesize bgmYouLose;
@synthesize bgmBossWin;
@synthesize bgmOpeningStory;
@synthesize bgmCurrent;
@synthesize sfxButtonClickBack;
@synthesize sfxButtonClickForward;
@synthesize sfxBrickBreak;
@synthesize sfxCrystalBreak;
@synthesize sfxPlayerTakeDamage;
@synthesize sfxPlayerTap;
@synthesize sfxPlayerEat;
@synthesize sfxPlayerChanting;
@synthesize sfxBeamOfLight;
@synthesize sfxVineGrow;
@synthesize sfxCheering;
@synthesize sfxHomeBaseDeath;
@synthesize sfxOutOfMana;
@synthesize sfxVortex;
@synthesize sfxGenericEnemySpawn;
@synthesize sfxBlobAttackPlayer;
@synthesize sfxBlobAttackTile;
@synthesize sfxBlobDeath;
@synthesize sfxSerpentAttack;
@synthesize sfxSerpentDeath;
@synthesize sfxSmokeSpawn;
@synthesize sfxSmokeAttack;
@synthesize sfxSmokeDeath;
@synthesize sfxBossDamage;
@synthesize sfxBossDeath;
@synthesize sfxCash;
@synthesize sfxIntroFall;
@synthesize sfxThud;
@synthesize sfxWinGrunt;
@synthesize sfxFinalGrow;
@synthesize bgmStoryEnding;
@synthesize bgmEclipse;
@synthesize bgmCredits;
@synthesize sfxTutorialStinger;

//---------------------------------------------------------------------------------------------------------------------
// Singleton

+ (VKAudioManager*) sharedInstance {
    
    __strong static VKAudioManager* _sharedInstance = nil;
    static dispatch_once_t oncePredicate = 0;
    dispatch_once(&oncePredicate, ^{_sharedInstance = [[VKAudioManager alloc] init];});
    return _sharedInstance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//- (void) dealloc {
    
//    self.menuMusicAudioPlayer = nil;
//    self.inGameMusicPlayer_Buffalo = nil;
//    self.inGameMusicPlayer_Manchester = nil;
//    self.inGameMusicPlayer_Dallas = nil;
//    self.inGameMusicPlayer_Bangkok = nil;
//    self.inGameMusicPlayer_Boston = nil;
//    self.inGameMusicPlayer_MexicoCity = nil;
//    self.inGameMusicPlayer_Philly = nil;
//    self.inGameMusicPlayer_BuenosAires = nil;
//    self.inGameMusicPlayer_Chicago = nil;
//    self.inGameMusicPlayer_Rome = nil;
//    self.inGameMusicPlayer_Honolulu = nil;
//    self.inGameMusicPlayer_SanFrancisco = nil;
//    self.inGameMusicPlayer_SaoPaulo = nil;
//    self.inGameMusicPlayer_Zombies = nil;
//    self.inGameMusicPlayer_NewOrleans = nil;
//    self.inGameMusicPlayer_Toyland = nil;
//    self.inGameMusicPlayer_Melbourne = nil;
//    self.inGameMusicPlayer_Shanghai = nil;
//    self.inGameMusicPlayer_Barcelona = nil;
//    self.inGameMusicPlayer_Denver = nil;
//    self.inGameMusicPlayer_Montreal = nil;
//    self.inGameMusicPlayer_DinoIsland = nil;
//
//    self.currentMusicPlayer = nil;
//}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag {
    //
    //    if( player == self.menuMusicAudioPlayer ) {
    //
    //        NSLog( @"audioPlayer finished" );
    //    }
    //    else if( player == currentAudioBallCall ) {
    //
    //        // Current ball call audio finished playing
    //    }
    //    else if( player == endOfRoundBuzzSoundPlayer ) {
    //
    //        // Round Over "Buzz" has finished. Play "Round Over".
    //        [BBAudioManager playCallerAudioForRoundOver];
    //    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) audioPlayerBeginInterruption:(AVAudioPlayer *)player {
    
//    if( player == self.currentMusicPlayer ) {
//        [BBAudioManager pauseAudioForBackground];
//    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player withFlags:(NSUInteger)flags {
    
//    if( player == self.currentMusicPlayer ) {
//        [BBAudioManager resumeAudioFromBackground];
//    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) audioPlayerEndInterruption:(AVAudioPlayer *)player {
    
//    if( player == self.currentMusicPlayer ) {
//        [BBAudioManager resumeAudioFromBackground];
//    }
}
//---------------------------------------------------------------------------------------------------------------------

+ (void) initializeAudio {
    
    //bgmMainMenuRPG
    
//    if (!self.inGameMusicPlayer_Buffalo) {
//		self.inGameMusicPlayer_Buffalo = [AVAudioPlayer audioPlayerForAsset:@"In Round Buffalo.120bpm.mp3"];
//		self.inGameMusicPlayer_Buffalo.numberOfLoops     = -1;
//		self.inGameMusicPlayer_Buffalo.volume            = defaultMusicVolume;
//		self.inGameMusicPlayer_Buffalo.delegate          = self;
//	}
    
//    NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"Main Theme.100bpm.mix" ofType:@"mp3"];
//    NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
//    self.menuMusicAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
//    
//    self.menuMusicAudioPlayer.numberOfLoops = -1; // A negative value means it will loop forever
//    self.menuMusicAudioPlayer.volume = defaultMusicVolume;
//    [self.menuMusicAudioPlayer setDelegate:self];
    
    
    //static const char g_szBGMMainMenuRPGName[32]	= "BGM_MainMenuRPG";
    // Audio is located in /Resources/game/data/audio
    //P02_BGM_Main_Menu
    //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Main_Menu" ofType:VK_AUDIO_TYPE];
    //NSString *images_misc_file = [[NSBundle mainBundle] pathForResource:@"mainmenu_background" ofType:@"png" inDirectory:@"images/misc"];
    
    
    if([VKAudioManager sharedInstance].isAudioInitialized) {
        //NSLog(@"Audio already initialized!");
        return;
    }
    
    
    //[VKAudioManager sharedInstance].playQueue = dispatch_queue_create("com.pixelvandals.vineking", NULL);
//    [VKAudioManager sharedInstance].playQueue01 = dispatch_queue_create("com.pixelvandals.vineking.sound01", NULL);
//    [VKAudioManager sharedInstance].playQueue02 = dispatch_queue_create("com.pixelvandals.vineking.sound02", NULL);
//    [VKAudioManager sharedInstance].playQueue03 = dispatch_queue_create("com.pixelvandals.vineking.sound03", NULL);
//    [VKAudioManager sharedInstance].playQueue04 = dispatch_queue_create("com.pixelvandals.vineking.sound04", NULL);
//    [VKAudioManager sharedInstance].playQueue05 = dispatch_queue_create("com.pixelvandals.vineking.sound05", NULL);
//    [VKAudioManager sharedInstance].playQueue06 = dispatch_queue_create("com.pixelvandals.vineking.sound06", NULL);
//    [VKAudioManager sharedInstance].playQueue07 = dispatch_queue_create("com.pixelvandals.vineking.sound07", NULL);
//    [VKAudioManager sharedInstance].playQueue08 = dispatch_queue_create("com.pixelvandals.vineking.sound08", NULL);
//    [VKAudioManager sharedInstance].playQueue09 = dispatch_queue_create("com.pixelvandals.vineking.sound09", NULL);
//    [VKAudioManager sharedInstance].playQueue10 = dispatch_queue_create("com.pixelvandals.vineking.sound10", NULL);
    
//    dispatchQueues[0] = dispatch_queue_create("com.pixelvandals.vineking.sound01", NULL);
//    dispatchQueues[1] = dispatch_queue_create("com.pixelvandals.vineking.sound02", NULL);
//    dispatchQueues[2] = dispatch_queue_create("com.pixelvandals.vineking.sound03", NULL);
//    dispatchQueues[3] = dispatch_queue_create("com.pixelvandals.vineking.sound04", NULL);
//    dispatchQueues[4] = dispatch_queue_create("com.pixelvandals.vineking.sound05", NULL);
//    dispatchQueues[5] = dispatch_queue_create("com.pixelvandals.vineking.sound06", NULL);
//    dispatchQueues[6] = dispatch_queue_create("com.pixelvandals.vineking.sound07", NULL);
//    dispatchQueues[7] = dispatch_queue_create("com.pixelvandals.vineking.sound08", NULL);
//    dispatchQueues[8] = dispatch_queue_create("com.pixelvandals.vineking.sound09", NULL);
//    dispatchQueues[9] = dispatch_queue_create("com.pixelvandals.vineking.sound10", NULL);
    
    [VKAudioManager sharedInstance].playQueue = dispatch_queue_create("com.pixelvandals.vineking.sound00", NULL);
    
    char szBuffer[64];
    for( int i = 0; i < MAX_PLAY_QUEUES; ++i ) {
        szBuffer[0] = '\0';
        sprintf(szBuffer, "%s%d", kPlayQueueString, i);
        dispatchQueues[i] = dispatch_queue_create(szBuffer, NULL);
    }
    
    //[VKAudioManager sharedInstance].playQueueArray = [[NSMutableArray alloc] initWithCapacity:MAX_PLAY_QUEUES];
    
    //sprintf( szString, "%s - Must be added to a t2dAnimatedSprite.", __FUNCTION__ );
    
    //char szBuffer[64];
    
    //[VKAudioManager sharedInstance]->dispatchQueueArray = malloc(sizeof(dispatch_queue_t) * 10);
    
//    for( int i =0; i < MAX_PLAY_QUEUES; ++i ) {
//        szBuffer[0] = '\0';
//        sprintf(szBuffer, "%s%d", kPlayQueueString, i);
//        dispatch_queue_t dispatchQueue = dispatch_queue_create(szBuffer, NULL);
//        dispatchQueueArray[i] = dispatchQueue;
//        //[[VKAudioManager sharedInstance].playQueueArray addObject:dispatchQueue];
//        //const char* szBuffer[64] = kPlayQueueString;
//        //strcpy(szBuffer, i);
//        //dispatch_queue_t dispatchQueue = dispatch_queue_create(szBuffer, NULL);
//        //[[VKAudioManager sharedInstance].playQueueArray addObject:dispatchQueue];
//    }

    [VKAudioManager sharedInstance].isSoundMuted = false;
    
    // BGM
    if( [VKAudioManager sharedInstance].bgmMainMenuRPG == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Main_Menu" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Main_Menu" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmMainMenuRPG = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmMainMenuRPG.numberOfLoops = -1;
        [VKAudioManager sharedInstance].bgmMainMenuRPG.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmMapScreen == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Level_Select" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Level_Select" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmMapScreen = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmMapScreen.numberOfLoops = -1;
        [VKAudioManager sharedInstance].bgmMapScreen.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmInGame == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Game_Music" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Game_Music" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmInGame = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmInGame.numberOfLoops = -1;
        [VKAudioManager sharedInstance].bgmInGame.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmBoss == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Final_Boss" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Final_Boss" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmBoss = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmBoss.numberOfLoops = -1;
        [VKAudioManager sharedInstance].bgmBoss.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmYouWin == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Win" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Win" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmYouWin = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmYouWin.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmYouLose == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Lose" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Lose" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmYouLose = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmYouLose.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmBossWin == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Final_Win" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Final_Win" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmBossWin = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmBossWin.volume = DEFAULT_BGM_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].bgmOpeningStory == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Intro_Dramatic" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Intro_Dramatic" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].bgmOpeningStory = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].bgmOpeningStory.numberOfLoops = -1;
        [VKAudioManager sharedInstance].bgmOpeningStory.volume = DEFAULT_BGM_VOLUME;
    }
    
    //AVAudioPlayer* audioPlayer = [VKAudioManager sharedInstance].bgmMainMenuRPG;
    //NSURL* bundleURL = bundleURLFor
    //[VKAudioManager sharedInstance].bgmMainMenuRPG = [[AVAudioPlayer alloc] initWithContentsOfURL:<#(NSURL *)#> error:<#(NSError **)#>]
    
    
//    // Allocate our return value
//    AVAudioPlayer* player = nil;
//    
//    // Fallback to bundle if asset not available
//    if (!asset || !asset.available || !asset.timestampHas) {
//        NSURL *bundleURL = bundleURLForResource(assetName);
//        player = [[AVAudioPlayer alloc] initWithContentsOfURL:bundleURL error:nil];
//    } else {
//        // Asset available
//        NSURL *bundleURL = [NSURL fileURLWithPath:asset.localPath];
//        player = [[AVAudioPlayer alloc] initWithContentsOfURL:bundleURL error:nil];
//    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX UI
    
    if( [VKAudioManager sharedInstance].sfxButtonClickBack == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Click_B" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Click_B" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxButtonClickBack = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxButtonClickBack.volume = DEFAULT_SFX_VOLUME;
    }
    
    if( [VKAudioManager sharedInstance].sfxButtonClickForward == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Click_F" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Click_F" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxButtonClickForward = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxButtonClickForward.volume = DEFAULT_SFX_VOLUME;
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Player
    
    if( [VKAudioManager sharedInstance].sfxBrickBreak == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Rock_Smash" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Rock_Smash" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxBrickBreak = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxBrickBreak.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxBrickBreak prepareToPlay];
        printf( "sfxBrickBreak.numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxBrickBreak.numberOfChannels );
    }
    
    if( [VKAudioManager sharedInstance].sfxCrystalBreak == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_Shatter" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_Shatter" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxCrystalBreak = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxCrystalBreak.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxCrystalBreak prepareToPlay];
        printf( "sfxCrystalBreak. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxCrystalBreak.numberOfChannels );
    }
    
    if( [VKAudioManager sharedInstance].sfxPlayerTakeDamage == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Hit" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Hit" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxPlayerTakeDamage = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxPlayerTakeDamage.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxPlayerTakeDamage prepareToPlay];
        printf( "sfxPlayerTakeDamage. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxPlayerTakeDamage.numberOfChannels );
    }
    
    if( [VKAudioManager sharedInstance].sfxPlayerTap == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Attack_Tap" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Attack_Tap" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxPlayerTap = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxPlayerTap.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxPlayerTap prepareToPlay];
        printf( "sfxPlayerTap. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxPlayerTap.numberOfChannels );
    }
    
    if( [VKAudioManager sharedInstance].sfxPlayerEat == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Eat" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Eat" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxPlayerEat = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxPlayerEat.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxPlayerEat prepareToPlay];
        printf( "sfxPlayerEat. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxPlayerEat.numberOfChannels );
    }
    
    if( [VKAudioManager sharedInstance].sfxPlayerChanting == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Chant" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Chant" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxPlayerChanting = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxPlayerChanting.numberOfLoops = -1; // NOT SURE IF THIS SHOULD BE LOOPING
        [VKAudioManager sharedInstance].sfxPlayerChanting.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxPlayerChanting prepareToPlay];
        printf( "sfxPlayerChanting. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxPlayerChanting.numberOfChannels );
    }
    
    if( [VKAudioManager sharedInstance].sfxHomeBaseDeath == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Death" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Player_Death" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxHomeBaseDeath = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxHomeBaseDeath.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxHomeBaseDeath prepareToPlay];
    }
    
    if( [VKAudioManager sharedInstance].sfxOutOfMana == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_Mana_Out" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_Mana_Out" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxOutOfMana = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxOutOfMana.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxOutOfMana prepareToPlay];
    }
    
    if( [VKAudioManager sharedInstance].sfxVortex == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Vortex" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Vortex" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxVortex = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxVortex.numberOfLoops = -1; // NOT SURE IF THIS SHOULD BE LOOPING
        [VKAudioManager sharedInstance].sfxVortex.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxVortex prepareToPlay];
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Generic Enemy
    if( [VKAudioManager sharedInstance].sfxGenericEnemySpawn == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Enemy_Spawn" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Enemy_Spawn" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxGenericEnemySpawn = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxGenericEnemySpawn.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxGenericEnemySpawn prepareToPlay];
        printf( "sfxGenericEnemySpawn. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxGenericEnemySpawn.numberOfChannels );
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Blob
//    if( [VKAudioManager sharedInstance].sfxBlobAttackPlayer == nil ) {
//        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Enemy_Spawn" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
//        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
//        [VKAudioManager sharedInstance].sfxBlobAttackPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
//        [[VKAudioManager sharedInstance].sfxBlobAttackPlayer prepareToPlay];
//    }
    if( [VKAudioManager sharedInstance].sfxBlobAttackTile == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_PUKE_Blob" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_PUKE_Blob" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxBlobAttackTile = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [[VKAudioManager sharedInstance].sfxBlobAttackTile prepareToPlay];
        printf( "sfxBlobAttackTile. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxBlobAttackTile.numberOfChannels );
    }
    if( [VKAudioManager sharedInstance].sfxBlobDeath == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_A" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_A" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxBlobDeath = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxBlobDeath.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxBlobDeath prepareToPlay];
        printf( "sfxBlobDeath. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxBlobDeath.numberOfChannels );
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Serpent
    if( [VKAudioManager sharedInstance].sfxSerpentAttack == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_ATK_Spit" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_ATK_Spit" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxSerpentAttack = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxSerpentAttack.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxSerpentAttack prepareToPlay];
        printf( "sfxSerpentAttack. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxSerpentAttack.numberOfChannels );
    }
    if( [VKAudioManager sharedInstance].sfxSerpentDeath == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_B" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_B" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxSerpentDeath = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxSerpentDeath.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxSerpentDeath prepareToPlay];
        printf( "sfxSerpentDeath. numberOfChannels: %d\n", [VKAudioManager sharedInstance].sfxSerpentDeath.numberOfChannels );
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Smoke
    if( [VKAudioManager sharedInstance].sfxSmokeSpawn == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_SPAWN_Smoke" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_SPAWN_Smoke" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxSmokeSpawn = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxSmokeSpawn.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxSmokeSpawn prepareToPlay];
    }
//    if( [VKAudioManager sharedInstance].sfxSmokeAttack == nil ) {
//        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_B" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
//        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
//        [VKAudioManager sharedInstance].sfxSmokeAttack = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
//        [[VKAudioManager sharedInstance].sfxSmokeAttack prepareToPlay];
//    }
    if( [VKAudioManager sharedInstance].sfxSmokeDeath == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_D" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Death_D" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxSmokeDeath = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxSmokeDeath.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxSmokeDeath prepareToPlay];
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Boss
    if( [VKAudioManager sharedInstance].sfxBossDamage == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Boss_Hit" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Boss_Hit" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxBossDamage = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxBossDamage.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxBossDamage prepareToPlay];
    }
    if( [VKAudioManager sharedInstance].sfxBossDeath == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Boss_Death" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Boss_Death" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxBossDeath = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxBossDeath.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxBossDeath prepareToPlay];
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Cash
    if( [VKAudioManager sharedInstance].sfxCash == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Cash" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_Cash" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxCash = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxCash.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxCash prepareToPlay];
    }
    
    // SFX Intro / Outro
    if( [VKAudioManager sharedInstance].sfxIntroFall == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_INTRO_FALL" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_INTRO_FALL" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxIntroFall = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxIntroFall.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxIntroFall prepareToPlay];
    }
    if( [VKAudioManager sharedInstance].sfxThud == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_THUD" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_THUD" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxThud = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxThud.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxThud prepareToPlay];
    }
    if( [VKAudioManager sharedInstance].sfxWinGrunt == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_WIN_GRUNT" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_WIN_GRUNT" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxWinGrunt = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxWinGrunt.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxWinGrunt prepareToPlay];
    }
    if( [VKAudioManager sharedInstance].sfxFinalGrow == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_FINAL_GROW" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_SFX_FINAL_GROW" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxFinalGrow = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxFinalGrow.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxFinalGrow prepareToPlay];
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SFX Tutorial
    if( [VKAudioManager sharedInstance].sfxTutorialStinger == nil ) {
        //NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Intro_Stinger" ofType:VK_AUDIO_TYPE inDirectory:VK_AUDIO_DIR];
        NSString* bgmPath = [[NSBundle mainBundle] pathForResource:@"P02_BGM_Intro_Stinger" ofType:VK_AUDIO_TYPE];
        NSURL* bgmURL = [NSURL fileURLWithPath:bgmPath];
        [VKAudioManager sharedInstance].sfxTutorialStinger = [[AVAudioPlayer alloc] initWithContentsOfURL:bgmURL error:nil];
        [VKAudioManager sharedInstance].sfxTutorialStinger.volume = DEFAULT_SFX_VOLUME;
        [[VKAudioManager sharedInstance].sfxTutorialStinger prepareToPlay];
    }
    
    [VKAudioManager sharedInstance].isAudioInitialized = YES;
    
    
} // initializeAudio

//---------------------------------------------------------------------------------------------------------------------

+ (void) playBGMAfterBecomingActive {
    NSLog(@"VKAudioManager.playBGMAfterBecomingActive");
}

+ (void) checkBGMVolume:(AVAudioPlayer*)bgm {
    if(bgm != nil) {
        bgm.volume = DEFAULT_BGM_VOLUME;
    }
}

//---------------------------------------------------------------------------------------------------------------------
// BGM

+ (void) playBGMMainMenuRPG {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmMainMenuRPG;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmMainMenuRPG prepareToPlay];
    [[VKAudioManager sharedInstance].bgmMainMenuRPG play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmMainMenuRPG;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playBGMMapScreen {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmMapScreen;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmMapScreen prepareToPlay];
    [[VKAudioManager sharedInstance].bgmMapScreen play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmMapScreen;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playInGameBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmInGame;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmInGame prepareToPlay];
    [[VKAudioManager sharedInstance].bgmInGame play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmInGame;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playBGMBoss {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmBoss;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmBoss prepareToPlay];
    [[VKAudioManager sharedInstance].bgmBoss play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmBoss;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playYouWinBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        //[VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmYouWin;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmYouWin prepareToPlay];
    [[VKAudioManager sharedInstance].bgmYouWin play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmYouWin;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playYouLoseBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        //[VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmYouLose;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmYouLose prepareToPlay];
    [[VKAudioManager sharedInstance].bgmYouLose play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmYouLose;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playBossWinBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        //[VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmBossWin;
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmBossWin prepareToPlay];
    [[VKAudioManager sharedInstance].bgmBossWin play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmBossWin;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playStoryOpeningBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmOpeningStory prepareToPlay];
    [[VKAudioManager sharedInstance].bgmOpeningStory play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmOpeningStory;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playStoryEndingBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmStoryEnding prepareToPlay];
    [[VKAudioManager sharedInstance].bgmStoryEnding play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmStoryEnding;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playEclipseBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmEclipse prepareToPlay];
    [[VKAudioManager sharedInstance].bgmEclipse play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmEclipse;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) playCreditsBGM {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        return;
    }
    [VKAudioManager stopCurrentBGM];
    [[VKAudioManager sharedInstance].bgmCredits prepareToPlay];
    [[VKAudioManager sharedInstance].bgmCredits play];
    [VKAudioManager sharedInstance].bgmCurrent = [VKAudioManager sharedInstance].bgmCredits;
    [self checkBGMVolume:[VKAudioManager sharedInstance].bgmCurrent];
}

+ (void) stopCurrentBGM {
    if([VKAudioManager sharedInstance].bgmCurrent != nil) {
        [[VKAudioManager sharedInstance].bgmCurrent stop];
        [VKAudioManager sharedInstance].bgmCurrent.currentTime = 0.0;
    }
    //[VKAudioManager sharedInstance].bgmCurrent = nil;
}

//---------------------------------------------------------------------------------------------------------------------
// SFX UI

+ (void) playButtonClickBack {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        return;
    }
    //[[VKAudioManager sharedInstance].sfxButtonClickBack prepareToPlay];
    //[[VKAudioManager sharedInstance].sfxButtonClickBack play];
    [VKAudioManager playSoundAsync:[VKAudioManager sharedInstance].sfxButtonClickBack];
}

+ (void) playButtonClickForward {
    if([VKAudioManager sharedInstance].isSoundMuted == YES) {
        return;
    }
    //[[VKAudioManager sharedInstance].sfxButtonClickForward prepareToPlay];
    //[[VKAudioManager sharedInstance].sfxButtonClickForward play];
    [VKAudioManager playSoundAsync:[VKAudioManager sharedInstance].sfxButtonClickForward];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Player
+ (void) playBrickBreakSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxBrickBreak];
}

+ (void) playCrystalBreakSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxCrystalBreak];
}

+ (void) playHomeBaseTakeDamageSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxPlayerTakeDamage];
}

+ (void) playTapSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxPlayerTap];
}

+ (void) playEatSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxPlayerEat];
}

+ (void) playChantingSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxPlayerChanting];
}

+ (void) stopChantingSound {
    [[VKAudioManager sharedInstance].sfxPlayerChanting stop];
    [VKAudioManager sharedInstance].sfxPlayerChanting.currentTime = 0.0;
    //[[VKAudioManager sharedInstance].sfxPlayerChanting pause];
}

+ (void) playBeamOfLightSound {
    // UNUSED
}

+ (void) stopBeamOfLightSound {
    // UNUSED
}

+ (void) playVineGrowSound {
    // UNUSED
}

+ (void) stopVineGrowSound {
    // UNUSED
}

+ (void) playCheeringSound {
    // UNUSED
}

+ (void) stopCheeringSound {
    // UNUSED
}

+ (void) playHomeBaseDeathSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxHomeBaseDeath];
}

+ (void) playOutOfManaSound {
    if([VKAudioManager sharedInstance].sfxOutOfMana.currentTime > 0.0) {
        return;
    }
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxOutOfMana];
}

+ (void) playVortexSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxVortex];
}

+ (void) stopVortexSound {
    [[VKAudioManager sharedInstance].sfxVortex stop];
    [VKAudioManager sharedInstance].sfxVortex.currentTime = 0.0;
    //[[VKAudioManager sharedInstance].sfxVortex pause];
}



//---------------------------------------------------------------------------------------------------------------------
// SFX Generic Enemy

+ (void) playGenericSpawnSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxGenericEnemySpawn];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Blob

+ (void) playBlobAttackPlayerSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxBlobAttackPlayer];
}

+ (void) playBlobAttackTileSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxBlobAttackTile];
}

+ (void) playBlobDeathSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxBlobDeath];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Serpent

+ (void) playSerpentSpitSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxSerpentAttack];
}

+ (void) playSerpentDeathSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxSerpentDeath];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Smoke

+ (void) playSmokeSpawnSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxSmokeSpawn];
}

+ (void) playSmokeAttackSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxSmokeAttack];
}

+ (void) playSmokeDeathSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxSmokeDeath];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Boss
+ (void) playBossDamageSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxBossDamage];
}

+ (void) playBossDeathSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxBossDeath];
}

// SFX Cash
+ (void) playCashSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxCash];
}

// SFX Intro / Outro
+ (void) playIntroFallSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxIntroFall];
}

+ (void) playThudSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxThud];
}

+ (void) playWinGruntSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxWinGrunt];
}

+ (void) playFinalGrowSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxFinalGrow];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Tutorial

+ (void) playTutorialStingerSound {
    [VKAudioManager playSound:[VKAudioManager sharedInstance].sfxTutorialStinger];
}

//---------------------------------------------------------------------------------------------------------------------
// SFX Global Sound

+ (void) muteAllSound {
    [VKAudioManager sharedInstance].isSoundMuted = YES;
    if([VKAudioManager sharedInstance].bgmCurrent != nil) {
        [VKAudioManager sharedInstance].bgmCurrent.currentTime = 0.0;
        [VKAudioManager sharedInstance].bgmCurrent.volume = 0.0;
        [[VKAudioManager sharedInstance].bgmCurrent stop];
    }
}

+ (void) unmuteAllSound {
    [VKAudioManager sharedInstance].isSoundMuted = NO;
    if([VKAudioManager sharedInstance].bgmCurrent != nil) {
        [VKAudioManager sharedInstance].bgmCurrent.currentTime = 0.0;
        [VKAudioManager sharedInstance].bgmCurrent.volume = DEFAULT_BGM_VOLUME;
        [[VKAudioManager sharedInstance].bgmCurrent play];
    }
}

+ (BOOL) isSoundMuted {
    return [VKAudioManager sharedInstance].isSoundMuted;
}

+ (void) fadeCurrentBGM {
    
}

+ (void) playSound:(AVAudioPlayer*)audioPlayer {
    if([VKAudioManager sharedInstance].isSoundMuted || !audioPlayer) {
        return;
    }
    if(audioPlayer.currentTime > 0.0 && audioPlayer.isPlaying) {
        audioPlayer.currentTime = 0.0;
        return;
    }
    [VKAudioManager playSoundAsync:audioPlayer];
}

+ (void) playSoundAsync:(AVAudioPlayer*)audioPlayer {
    if( audioPlayer == nil ) {
        return;
    }
    dispatch_async([VKAudioManager sharedInstance].playQueue, ^{
        [audioPlayer play];
    });
}

+ (void) playSoundAsync:(AVAudioPlayer *)audioPlayer inQueue:(NSInteger)queueIdx {
    if( audioPlayer == nil || queueIdx >= MAX_PLAY_QUEUES ) {
        return;
    }
    dispatch_async(dispatchQueues[queueIdx], ^{
        [audioPlayer play];
    });
}

//---------------------------------------------------------------------------------------------------------------------

@end // @implementation VKAudioManager

//---------------------------------------------------------------------------------------------------------------------

