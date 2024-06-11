//
//  VKAudioManager.h
//  iTorque2D
//
//  Created by Richard Skala on 2/12/14.
//
//
//---------------------------------------------------------------------------------------------------------------------

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h> // For AVAudioPlayerDelegate
#include <dispatch/dispatch.h> // For dispatch_queue_t
#include <AudioToolbox/AudioToolbox.h>
#include <stdio.h>


//---------------------------------------------------------------------------------------------------------------------

//extern const struct AudioManager {
//	__unsafe_unretained NSString* musicKey;
//	__unsafe_unretained NSString* voiceKey;
//	__unsafe_unretained NSString* effectsKey;
//} AudioManager;

extern const struct AudioKeys {
    
    __unsafe_unretained NSString* key1;
    __unsafe_unretained NSString* key2;
    __unsafe_unretained NSString* key3;
} AudioKeys;

//---------------------------------------------------------------------------------------------------------------------

@interface VKAudioManager : NSObject <UIApplicationDelegate, AVAudioPlayerDelegate> {
    
    dispatch_queue_t* dispatchQueueArray[10];
}

//---------------------------------------------------------------------------------------------------------------------

@property (strong, nonatomic) NSMutableDictionary* bgmDictionary;
@property (nonatomic) BOOL isAudioInitialized;
@property (nonatomic) BOOL isSoundMuted;

// Idea for this came from: http://stackoverflow.com/questions/986983/what-do-you-use-to-play-sound-in-iphone-games
@property (strong, nonatomic) NSMutableArray* playQueueArray;
@property (nonatomic) dispatch_queue_t playQueue;
@property (nonatomic) dispatch_queue_t playQueue01;
@property (nonatomic) dispatch_queue_t playQueue02;
@property (nonatomic) dispatch_queue_t playQueue03;
@property (nonatomic) dispatch_queue_t playQueue04;
@property (nonatomic) dispatch_queue_t playQueue05;
@property (nonatomic) dispatch_queue_t playQueue06;
@property (nonatomic) dispatch_queue_t playQueue07;
@property (nonatomic) dispatch_queue_t playQueue08;
@property (nonatomic) dispatch_queue_t playQueue09;
@property (nonatomic) dispatch_queue_t playQueue10;

// Music
@property (strong, nonatomic) AVAudioPlayer* bgmMainMenuRPG;
@property (strong, nonatomic) AVAudioPlayer* bgmMapScreen;
@property (strong, nonatomic) AVAudioPlayer* bgmInGame;
@property (strong, nonatomic) AVAudioPlayer* bgmBoss;
@property (strong, nonatomic) AVAudioPlayer* bgmYouWin;
@property (strong, nonatomic) AVAudioPlayer* bgmYouLose;
@property (strong, nonatomic) AVAudioPlayer* bgmBossWin;
@property (strong, nonatomic) AVAudioPlayer* bgmOpeningStory;
@property (strong, nonatomic) AVAudioPlayer* bgmCurrent;

// UI
@property (strong, nonatomic) AVAudioPlayer* sfxButtonClickBack;
@property (strong, nonatomic) AVAudioPlayer* sfxButtonClickForward;

// Player
@property (strong, nonatomic) AVAudioPlayer* sfxBrickBreak;
@property (strong, nonatomic) AVAudioPlayer* sfxCrystalBreak;
@property (strong, nonatomic) AVAudioPlayer* sfxPlayerTakeDamage;
@property (strong, nonatomic) AVAudioPlayer* sfxPlayerTap;
@property (strong, nonatomic) AVAudioPlayer* sfxPlayerEat;
@property (strong, nonatomic) AVAudioPlayer* sfxPlayerChanting;
@property (strong, nonatomic) AVAudioPlayer* sfxBeamOfLight;
@property (strong, nonatomic) AVAudioPlayer* sfxVineGrow;
@property (strong, nonatomic) AVAudioPlayer* sfxCheering;
@property (strong, nonatomic) AVAudioPlayer* sfxHomeBaseDeath;
@property (strong, nonatomic) AVAudioPlayer* sfxOutOfMana;
@property (strong, nonatomic) AVAudioPlayer* sfxVortex;

// Generic Enemy
@property (strong, nonatomic) AVAudioPlayer* sfxGenericEnemySpawn;

// Blob
@property (strong, nonatomic) AVAudioPlayer* sfxBlobAttackPlayer;
@property (strong, nonatomic) AVAudioPlayer* sfxBlobAttackTile;
@property (strong, nonatomic) AVAudioPlayer* sfxBlobDeath;

// Serpent
@property (strong, nonatomic) AVAudioPlayer* sfxSerpentAttack;
@property (strong, nonatomic) AVAudioPlayer* sfxSerpentDeath;

// Smoke
@property (strong, nonatomic) AVAudioPlayer* sfxSmokeSpawn;
@property (strong, nonatomic) AVAudioPlayer* sfxSmokeAttack;
@property (strong, nonatomic) AVAudioPlayer* sfxSmokeDeath;

// Boss
@property (strong, nonatomic) AVAudioPlayer* sfxBossDamage;
@property (strong, nonatomic) AVAudioPlayer* sfxBossDeath;

// Cash
@property (strong, nonatomic) AVAudioPlayer* sfxCash;

// Intro / Outro
@property (strong, nonatomic) AVAudioPlayer* sfxIntroFall;
@property (strong, nonatomic) AVAudioPlayer* sfxThud;
@property (strong, nonatomic) AVAudioPlayer* sfxWinGrunt;
@property (strong, nonatomic) AVAudioPlayer* sfxFinalGrow;

// Ending
@property (strong, nonatomic) AVAudioPlayer* bgmStoryEnding;
@property (strong, nonatomic) AVAudioPlayer* bgmEclipse;
@property (strong, nonatomic) AVAudioPlayer* bgmCredits;

// Tutorial
@property (strong, nonatomic) AVAudioPlayer* sfxTutorialStinger;


//---------------------------------------------------------------------------------------------------------------------

+ (VKAudioManager*) sharedInstance; // Singleton

+ (void) initializeAudio;
+ (void) checkBGMVolume:(AVAudioPlayer*)bgm;

// BGM
+ (void) playBGMMainMenuRPG;
+ (void) playBGMMapScreen;
+ (void) playInGameBGM;
+ (void) playBGMBoss;
+ (void) playYouWinBGM;
+ (void) playYouLoseBGM;
+ (void) playBossWinBGM;

+ (void) playStoryOpeningBGM;

+ (void) playStoryEndingBGM;
+ (void) playEclipseBGM;
+ (void) playCreditsBGM;

+ (void) stopCurrentBGM;

// UI
+ (void) playButtonClickBack;
+ (void) playButtonClickForward;

// Player
+ (void) playBrickBreakSound;
+ (void) playCrystalBreakSound;
+ (void) playHomeBaseTakeDamageSound;
+ (void) playTapSound;
+ (void) playEatSound;

+ (void) playChantingSound;
+ (void) stopChantingSound;

+ (void) playBeamOfLightSound;
+ (void) stopBeamOfLightSound;

+ (void) playVineGrowSound;
+ (void) stopVineGrowSound;

+ (void) playCheeringSound;
+ (void) stopCheeringSound;

+ (void) playHomeBaseDeathSound;

+ (void) playOutOfManaSound;

+ (void) playVortexSound;
+ (void) stopVortexSound;

+ (void) playBGMAfterBecomingActive;


// Generic Enemy
+ (void) playGenericSpawnSound;

// Blob
+ (void) playBlobAttackPlayerSound;
+ (void) playBlobAttackTileSound;
+ (void) playBlobDeathSound;

// Serpent
+ (void) playSerpentSpitSound;
+ (void) playSerpentDeathSound;

// Smoke
+ (void) playSmokeSpawnSound;
+ (void) playSmokeAttackSound;
+ (void) playSmokeDeathSound;

// Boss
+ (void) playBossDamageSound;
+ (void) playBossDeathSound;

// Cash
+ (void) playCashSound;

// Intro / Outro
+ (void) playIntroFallSound;
+ (void) playThudSound;
+ (void) playWinGruntSound;
+ (void) playFinalGrowSound;

// Tutorial
+ (void) playTutorialStingerSound;

// Global Sound
+ (void) muteAllSound;
+ (void) unmuteAllSound;

//bool IsSoundMuted() const { return m_bAllSoundMuted; }
+ (BOOL) isSoundMuted;

//void FadeCurrentBGM( const F32& );
+ (void) fadeCurrentBGM;

+ (void) playSoundAsync:(AVAudioPlayer*)audioPlayer;



//=======================================================================================================================

//+ (BBAudioManager*) sharedInstance;// Singleton
//
//- (void) initializeAudio;
//
//- (void) initalizeMenuMusic;
//+ (void) playMenuMusic;
//+ (void) stopMenuMusic;
//
//- (void) initializeInGameMusic;
//+ (void) stopInGameMusic;
//
//+ (void) playBGM_Buffalo;
//+ (void) playBGM_Manchester;
//+ (void) playBGM_Dallas;
//+ (void) playBGM_Bangkok;
//+ (void) playBGM_Boston;
//+ (void) playBGM_MexicoCity;
//+ (void) playBGM_Philly;
//+ (void) playBGM_BuenosAires;
//+ (void) playBGM_Chicago;
//+ (void) playBGM_Rome;
//+ (void) playBGM_Honolulu;
//+ (void) playBGM_Zombies;
//+ (void) playBGM_SaoPaulo;
//+ (void) playBGM_SanFrancisco;
//+ (void) playBGM_NewOrleans;
//+ (void) playBGM_Toyland;
//+ (void) playBGM_Seattle;
//+ (void) playBGM_Shanghai;
//+ (void) playBGM_Melbourne;
//+ (void) playBGM_Barcelona;
//+ (void) playBGM_Denver;
//+ (void) playBGM_Montreal;
//+ (void) playBGM_DinoIsland;
//
//
//
//
//
//
//- (void) initializeCallerAudioForCity:(NSString*)cityName;
//- (void) uninitializeCallerAudio;
//
//+ (void) playCallerAudioForBall:(NSString*)letter number:(NSInteger)number;
//+ (void) playCallerAudioForBingo;
//+ (void) playCallerAudioForBadBingo;
//+ (void) playCallerAudioForReady;
//+ (void) playCallerAudioForGo;
//+ (void) playCallerAudioForRoundOver;
//+ (void) playCallerAudioForLastChance;
//+ (void) playBadBallSound;
//
//+ (void) playCallerAudioForBlackOutBingo; // 2012/05/31 -- Added at the 11th hour -- this does not call into the Caller Audio Array
//
//+ (void) playPowerUpReadySound;
//+ (void) playPowerUpActivatedSound;
//
//+ (void) playTimeRunningOutSound;
//+ (void) playEndOfRoundBuzzSound;
//
//- (void) initializeGameplayAudio;
//+ (void) playChestEarnedSound;
//+ (void) playChestDroppedSound;
//+ (void) playCoinEarnedSound;
//+ (void) playDaubSound;
//+ (void) playAutoDaubSound;
//+ (void) playBallSpeedUpSound;
//+ (void) playButtonClickSound;
//
//+ (void) playFiveBallsAddedSound;
//
//- (void) initializeRoundSummaryAudio;
//+ (void) playDaubSummaryRewardAddedSound;
//
//+ (void) playSummaryItemCityLandSound;
//+ (void) playSummaryItemLandSound;
//- (void) playSummaryItemBingoEnterSound;
//+ (void) playSummaryItemBarEmergeSound;
//+ (void) playSummaryItemDaubBarLandSound;
//
//+ (void) playSummaryBingosSound;
//+ (void) playSummaryDaubFillSound;
//
//+ (void) playBingoActivatedSound;
//
//+ (void) playInstantBingoImpactSound;
//
//+ (void) playSummaryChestOpenSound;
//
//+ (void) playExtraTimeActivatedSound;
//+ (void) playDaubPowerActivatedSound;
//
//+ (void) playItemPurchasedSound;
//+ (void) playSummaryAchievementSound;
//
//+ (void) playScratcherAvailableSound;
//+ (void) playScratcherRevealedSound;
//+ (void) playGoldScratcherAvailableSound;
//+ (void) playGoldScratcherRevealedSound;
//+ (void) playScratchSoundA;
//+ (void) playScratchSoundB;
//+ (void) playScratchSpaceRevealSound;
//+ (void) playBuffaloBonusSound;
//
//+ (void) muteSoundEffects;
//+ (void) unmuteSoundEffects;
//+ (bool) areSoundEffectsMuted;
//
//+ (void) muteCallerVoice;
//+ (void) unmuteCallerVoice;
//+ (bool) isCallerVoiceMuted;
//
//+ (void) muteMusic;
//+ (void) unmuteMusic;
//+ (bool) isMusicMuted;
//
//+ (NSInteger) getAudioIndexForLetterBFromNumber:(NSInteger)number;
//+ (NSInteger) getAudioIndexForLetterIFromNumber:(NSInteger)number;
//+ (NSInteger) getAudioIndexForLetterNFromNumber:(NSInteger)number;
//+ (NSInteger) getAudioIndexForLetterGFromNumber:(NSInteger)number;
//+ (NSInteger) getAudioIndexForLetterOFromNumber:(NSInteger)number;
//
//+ (void) pauseAudioForBackground;
//+ (void) resumeAudioFromBackground;

// Music
//@property (strong, nonatomic) AVAudioPlayer* menuMusicAudioPlayer;
//
//// TODO: Move the music into the database
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Buffalo;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Manchester;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Dallas;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Bangkok;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Boston;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_MexicoCity;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Philly;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_BuenosAires;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Chicago;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Rome;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Honolulu;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Zombies;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_SanFrancisco;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_SaoPaulo;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Toyland;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_NewOrleans;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Seattle;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Shanghai;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Melbourne;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Barcelona;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Denver;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_Montreal;
//@property (strong, nonatomic) AVAudioPlayer* inGameMusicPlayer_DinoIsland;
//
//@property (strong, nonatomic) AVAudioPlayer* currentMusicPlayer;
//
//@property (strong, nonatomic) AVAudioPlayer* summaryItemCityLandSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryItemLandSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryItemBingoEnterSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryItemBarEmergeSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryItemDaubBarLandSound;
//
//// Sound Effects
//@property (strong, nonatomic) AVAudioPlayer* buttonClickSound;
//@property (strong, nonatomic) AVAudioPlayer* badBallSound;
//@property (strong, nonatomic) AVAudioPlayer* daubSound;
//@property (strong, nonatomic) AVAudioPlayer* autoDaubSound;
//@property (strong, nonatomic) AVAudioPlayer* coinEarnedSound;
//@property (strong, nonatomic) AVAudioPlayer* chestEarnedSound;
//@property (strong, nonatomic) AVAudioPlayer* chestDroppedSound;
//@property (strong, nonatomic) AVAudioPlayer* powerUpReadySound;
//@property (strong, nonatomic) AVAudioPlayer* powerUpTriggeredSound;
//@property (strong, nonatomic) AVAudioPlayer* ballSpeedUpSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryBingosSound;
//@property (strong, nonatomic) AVAudioPlayer* fiveBallsAddedSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryDaubFillSound;
//@property (strong, nonatomic) AVAudioPlayer* bingoActivatedSound;
//@property (strong, nonatomic) AVAudioPlayer* instantBingoImpactSound;
//@property (strong, nonatomic) AVAudioPlayer* summaryChestOpenSound;
//@property (strong, nonatomic) AVAudioPlayer* extraTimeActivatedSound;
//@property (strong, nonatomic) AVAudioPlayer* daubPowerActivatedSound;
//@property (strong, nonatomic) AVAudioPlayer* itemPurchasedSound;
//
//@property (strong, nonatomic) AVAudioPlayer* scratcherAvailableSound;
//@property (strong, nonatomic) AVAudioPlayer* scratcherRevealedSound;
//@property (strong, nonatomic) AVAudioPlayer* goldScratcherAppearSound;
//@property (strong, nonatomic) AVAudioPlayer* goldScratcherRevealedSound;
//@property (strong, nonatomic) AVAudioPlayer* scratcherASound;
//@property (strong, nonatomic) AVAudioPlayer* scratcherBSound;
//@property (strong, nonatomic) AVAudioPlayer* scratchSpaceRevealSound;
//@property (strong, nonatomic) AVAudioPlayer* buffaloBonusSound;
//
//@property (strong, nonatomic) AVAudioPlayer* blackoutBingoCallerAudio;
//
//@property (strong, nonatomic) AVAudioPlayer* summaryAchievementSound;
//
//@property (strong, nonatomic) AVAudioPlayer*  daubSummaryRewardAddedPlayer;
//@property (strong, nonatomic) AVAudioPlayer*  endOfRoundBuzzSoundPlayer;
//@property (strong, nonatomic) AVAudioPlayer*  timeRunningOutSoundPlayer;
//
//@property (nonatomic) BOOL audioPausedForBackground;
//@property (nonatomic) CGFloat audioPlayerCurrentTime;



@end
