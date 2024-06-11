//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#import <GameKit/GameKit.h>  
#include "console/console.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@interface GameCenter : UIViewController <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate>
{  
    BOOL m_bLocalPlayerIsAuthenticated;  
    BOOL m_bGameCenterAvailableOnDevice;
	BOOL m_bIsAuthenticating;
	
    UIViewController*		m_pGameCenterViewController;
	NSMutableDictionary*	m_pAchievementDescriptionDictionary; // List of Achievements (Will hold GKAchievementDescription objects)
	NSMutableDictionary*	m_pLocalPlayerAchievementDictionary; // List of Earned Achievements (Will hold GKAchievements)
	
	GKLeaderboard* m_pLeaderboard_TopLife;		//	VK_LB_TOP_LIFE = 0,
	GKLeaderboard* m_pLeaderboard_Monster;		//	VK_LB_TOP_MONSTER,
	GKLeaderboard* m_pLeaderboard_TopSeed;		//	VK_LB_TOP_SEED,
	GKLeaderboard* m_pLeaderboard_TopCoins;		//	VK_LB_TOP_COINS,
	GKLeaderboard* m_pLeaderboard_Time_W2;		//	VK_LB_TIME_W2,
	GKLeaderboard* m_pLeaderboard_Time_W3;		//	VK_LB_TIME_W3,
	GKLeaderboard* m_pLeaderboard_Time_W4;		//	VK_LB_TIME_W4,
	GKLeaderboard* m_pLeaderboard_Time_W5;		//	VK_LB_TIME_W5,
	GKLeaderboard* m_pLeaderboard_Time_W6;		//	VK_LB_TIME_W6,
	GKLeaderboard* m_pLeaderboard_Time_W7;		//	VK_LB_TIME_W7,
	GKLeaderboard* m_pLeaderboard_Time_W8;		//	VK_LB_TIME_W8,
	GKLeaderboard* m_pLeaderboard_Time_W9;		//	VK_LB_TIME_W9,
	GKLeaderboard* m_pLeaderboard_Time_W10;		//	VK_LB_TIME_W10,
	GKLeaderboard* m_pLeaderboard_Time_B1;		//	VK_LB_TIME_B1,
	GKLeaderboard* m_pLeaderboard_Time_B2;		//	VK_LB_TIME_B2,
	GKLeaderboard* m_pLeaderboard_Time_B3;		//	VK_LB_TIME_B3,
	GKLeaderboard* m_pLeaderboard_Time_11_1;	//	VK_LB_TIME_W11_1,
	
	NSInteger m_iLeaderboardRank_TopLife;		//	VK_LB_TOP_LIFE = 0,
	NSInteger m_iLeaderboardRank_Monster;		//	VK_LB_TOP_MONSTER,
	NSInteger m_iLeaderboardRank_TopSeed;		//	VK_LB_TOP_SEED,
	NSInteger m_iLeaderboardRank_TopCoins;		//	VK_LB_TOP_COINS,
	NSInteger m_iLeaderboardRank_Time_W2;		//	VK_LB_TIME_W2,
	NSInteger m_iLeaderboardRank_Time_W3;		//	VK_LB_TIME_W3,
	NSInteger m_iLeaderboardRank_Time_W4;		//	VK_LB_TIME_W4,
	NSInteger m_iLeaderboardRank_Time_W5;		//	VK_LB_TIME_W5,
	NSInteger m_iLeaderboardRank_Time_W6;		//	VK_LB_TIME_W6,
	NSInteger m_iLeaderboardRank_Time_W7;		//	VK_LB_TIME_W7,
	NSInteger m_iLeaderboardRank_Time_W8;		//	VK_LB_TIME_W8,
	NSInteger m_iLeaderboardRank_Time_W9;		//	VK_LB_TIME_W9,
	NSInteger m_iLeaderboardRank_Time_W10;		//	VK_LB_TIME_W10,
	NSInteger m_iLeaderboardRank_Time_B1;		//	VK_LB_TIME_B1,
	NSInteger m_iLeaderboardRank_Time_B2;		//	VK_LB_TIME_B2,
	NSInteger m_iLeaderboardRank_Time_B3;		//	VK_LB_TIME_B3,
	NSInteger m_iLeaderboardRank_Time_11_1;		//	VK_LB_TIME_W11_1,
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@property( nonatomic, retain ) UIViewController*	m_pGameCenterViewController;
@property( nonatomic, retain ) NSMutableDictionary* m_pAchievementDescriptionDictionary;
@property( nonatomic, retain ) NSMutableDictionary* m_pLocalPlayerAchievementDictionary;

@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_TopLife;	//	VK_LB_TOP_LIFE = 0,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Monster;	//	VK_LB_TOP_MONSTER,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_TopSeed;	//	VK_LB_TOP_SEED,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_TopCoins;	//	VK_LB_TOP_COINS,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W2;	//	VK_LB_TIME_W2,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W3;	//	VK_LB_TIME_W3,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W4;	//	VK_LB_TIME_W4,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W5;	//	VK_LB_TIME_W5,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W6;	//	VK_LB_TIME_W6,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W7;	//	VK_LB_TIME_W7,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W8;	//	VK_LB_TIME_W8,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W9;	//	VK_LB_TIME_W9,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_W10;	//	VK_LB_TIME_W10,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_B1;	//	VK_LB_TIME_B1,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_B2;	//	VK_LB_TIME_B2,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_B3;	//	VK_LB_TIME_B3,
@property( nonatomic, retain ) GKLeaderboard* m_pLeaderboard_Time_11_1;	//	VK_LB_TIME_W11_1,

@property( getter=GetLeaderboardRank_TopLife  ) NSInteger m_iLeaderboardRank_TopLife;
@property( getter=GetLeaderboardRank_Monster  ) NSInteger m_iLeaderboardRank_Monster;
@property( getter=GetLeaderboardRank_TopSeed  ) NSInteger m_iLeaderboardRank_TopSeed;
@property( getter=GetLeaderboardRank_TopCoins ) NSInteger m_iLeaderboardRank_TopCoins;
@property( getter=GetLeaderboardRank_Time_W2  ) NSInteger m_iLeaderboardRank_Time_W2;
@property( getter=GetLeaderboardRank_Time_W3  ) NSInteger m_iLeaderboardRank_Time_W3;
@property( getter=GetLeaderboardRank_Time_W4  ) NSInteger m_iLeaderboardRank_Time_W4;
@property( getter=GetLeaderboardRank_Time_W5  ) NSInteger m_iLeaderboardRank_Time_W5;
@property( getter=GetLeaderboardRank_Time_W6  ) NSInteger m_iLeaderboardRank_Time_W6;
@property( getter=GetLeaderboardRank_Time_W7  ) NSInteger m_iLeaderboardRank_Time_W7;
@property( getter=GetLeaderboardRank_Time_W8  ) NSInteger m_iLeaderboardRank_Time_W8;
@property( getter=GetLeaderboardRank_Time_W9  ) NSInteger m_iLeaderboardRank_Time_W9;
@property( getter=GetLeaderboardRank_Time_W10 ) NSInteger m_iLeaderboardRank_Time_W10;
@property( getter=GetLeaderboardRank_Time_B1  ) NSInteger m_iLeaderboardRank_Time_B1;
@property( getter=GetLeaderboardRank_Time_B2  ) NSInteger m_iLeaderboardRank_Time_B2;
@property( getter=GetLeaderboardRank_Time_B3  ) NSInteger m_iLeaderboardRank_Time_B3;
@property( getter=GetLeaderboardRank_Time_11_1) NSInteger m_iLeaderboardRank_Time_11_1;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from NSObject
- (id)init;

// Methods for LOCAL PLAYER AUTHENTICATION
- (void) authenticateLocalPlayer;
- (void) registerForAuthenticationNotification;
- (void) authenticationChanged;
- (BOOL) isAuthenticated;

// Methods for LEADERBOARDS
- (void) reportScore:		(int64_t) _iScore 
		 forLeaderboardID:	(NSString*) _pLeaderboardIDString;

- (void) reportScore: (GKScore *)scoreReporter;

- (void) saveScoreToDevice: (GKScore *)score;
- (void) retrieveScoresFromDevice;
//- (void) showLeaderboard;
- (void) leaderboardViewControllerDidFinish: (GKLeaderboardViewController*) viewController;

- (void) DisplayLeaderboard:	(NSString*)_pLeaderboardIDString;

- (void) DisplayLeaderboards;

- (void) OnScoreSuccessfullyReportedForLeaderboard	: (NSString*) _pLeaderboardIDString
		 WithScore									: (GKScore*) _pScore;

- (NSInteger) GetLeaderboardIDFromString : (NSString*) _pLeaderboardIDString;

// Methods for ACHIEVEMENTS
- (void) reportAchievementIdentifier: (NSString*)_pAchievementIDString 
         percentComplete:			  (float)_fPercent
		 displayBanner:				  (BOOL) _bDisplayBanner;

//- (void) reportAchievementIdentifier: (GKAchievement *)achievement;
- (void) saveAchievementToDevice: (GKAchievement *)achievement;
- (void) retrieveAchievementsFromDevice;
- (void) achievementViewControllerDidFinish: (GKAchievementViewController *)viewController; // Required by the class

- (void) DisplayAchievements;
- (void) LoadAchievements;
- (void) ResetAchievements;

- (char*) GetAchievementTitle: (NSString*) keyString;

- (bool) HasAchievementBeenEarned: (NSString*) _pAchievementIDString;

- (void) ShutDownGameCenter;

@end // @interface GameCenter

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//static GameCenter* g_pGameCenter; // The Game Center object instance  -- TODO: PUT THIS IN THE .mm FILE THEN extern IT

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL _IsGameCenterAvailableOnDevice();

//----------------------------------------------------------------------------------------------------------------------------------------------------------------