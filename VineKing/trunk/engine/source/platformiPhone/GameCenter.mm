//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#import "platformiPhone/GameCenter.h"

#import "GameCenterWrapper.h"

#include "component/ComponentGlobals.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

GameCenter* g_pGameCenter = nil;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL _IsGameCenterAvailableOnDevice()
{  
	// According to Apple, Game Center is only available on the following devices and only on iOS 4.1 and up:
	// - iPod touch 4th generation
	// - iPod touch 3rd generation
	// - iPod touch 2nd generation
	// - iPhone 4
	// - iPhone 3GS
	// - iPad
	if( CComponentGlobals::GetInstance().CanIOSDeviceSupportGameCenter() == false )
		return NO;
	
    // Check for presence of GKLocalPlayer API.   
    Class gcClass = (NSClassFromString( @"GKLocalPlayer" ));
	
    // The device must be running running iOS 4.1 or later.
    //NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    
    //NSLog( @"CURRENT SYSTEM VERSION: %@", currSysVer );
    
    float fSystemVersion = [currSysVer floatValue];
    //printf( "fSystemVersion: %f\n", fSystemVersion );
    //if( fSystemVersion >= 4.1 )
    //    printf( "iOS Version SUPPORTS Game Center\n" );
    //else
   //     printf( "iOS Version DOES NOT support Game Center\n" );
    
    
    BOOL bOSVersionSupported = (fSystemVersion >= 4.1);
    
    //BOOL bOSVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
	
    return ( gcClass && bOSVersionSupported );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void _ReportGameCenterError( const char* _pszFunctionName, NSError* _pError )
{
	if( _pszFunctionName == nil || _pError == nil )
		return;
	
	//NSLog( @"GKErrorDomain: %@", GKErrorDomain );
	
	NSLog( @"Game Center error in function %s", _pszFunctionName );
	
	NSLog( @"Error domain: %@", [_pError domain] );
	NSLog( @"Error code:   %d", [_pError code] );
	
	NSInteger errorCode = [_pError code]; // 'code' is dependent on the error 'domain'
	switch( errorCode )
	{
		case GKErrorUnknown:					NSLog( @" - Error: GKErrorUnknown" );					break;
		case GKErrorCancelled:					NSLog( @" - Error: GKErrorCancelled" );					break;
		case GKErrorCommunicationsFailure:		NSLog( @" - Error: GKErrorCommunicationsFailure" );		break;
		case GKErrorUserDenied:					NSLog( @" - Error: GKErrorUserDenied" );				break;
		case GKErrorInvalidCredentials:			NSLog( @" - Error: GKErrorInvalidCredentials" );		break;
		case GKErrorNotAuthenticated:			NSLog( @" - Error: GKErrorNotAuthenticated" );			break;
		case GKErrorAuthenticationInProgress:	NSLog( @" - Error: GKErrorAuthenticationInProgress" );	break;
		case GKErrorInvalidPlayer:				NSLog( @" - Error: GKErrorInvalidPlayer" );				break;
		case GKErrorScoreNotSet:				NSLog( @" - Error: GKErrorScoreNotSet" );				break;
		case GKErrorParentalControlsBlocked:	NSLog( @" - Error: GKErrorParentalControlsBlocked" );	break;
		case GKErrorMatchRequestInvalid:		NSLog( @" - Error: GKErrorMatchRequestInvalid" );		break;
		case GKErrorUnderage:					NSLog( @" - Error: GKErrorUnderage" );					break;
		case GKErrorGameUnrecognized:			NSLog( @" - Error: GKErrorGameUnrecognized" );			break;
		case GKErrorNotSupported:				NSLog( @" - Error: GKErrorNotSupported" );				break;
		case GKErrorInvalidParameter:			NSLog( @" - Error: GKErrorInvalidParameter" );			break;
			
		default: NSLog( @" - Unhandled error code: %d", errorCode ); break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// GameCenter class implementation
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@implementation GameCenter  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@synthesize m_pGameCenterViewController;
@synthesize m_pAchievementDescriptionDictionary;
@synthesize m_pLocalPlayerAchievementDictionary;

@synthesize m_pLeaderboard_TopLife;		//	VK_LB_TOP_LIFE = 0,
@synthesize m_pLeaderboard_Monster;		//	VK_LB_TOP_MONSTER,
@synthesize m_pLeaderboard_TopSeed;		//	VK_LB_TOP_SEED,
@synthesize m_pLeaderboard_TopCoins;	//	VK_LB_TOP_COINS,
@synthesize m_pLeaderboard_Time_W2;		//	VK_LB_TIME_W2,
@synthesize m_pLeaderboard_Time_W3;		//	VK_LB_TIME_W3,
@synthesize m_pLeaderboard_Time_W4;		//	VK_LB_TIME_W4,
@synthesize m_pLeaderboard_Time_W5;		//	VK_LB_TIME_W5,
@synthesize m_pLeaderboard_Time_W6;		//	VK_LB_TIME_W6,
@synthesize m_pLeaderboard_Time_W7;		//	VK_LB_TIME_W7,
@synthesize m_pLeaderboard_Time_W8;		//	VK_LB_TIME_W8,
@synthesize m_pLeaderboard_Time_W9;		//	VK_LB_TIME_W9,
@synthesize m_pLeaderboard_Time_W10;	//	VK_LB_TIME_W10,
@synthesize m_pLeaderboard_Time_B1;		//	VK_LB_TIME_B1,
@synthesize m_pLeaderboard_Time_B2;		//	VK_LB_TIME_B2,
@synthesize m_pLeaderboard_Time_B3;		//	VK_LB_TIME_B3,
@synthesize m_pLeaderboard_Time_11_1;	//	VK_LB_TIME_W11_1,

@synthesize m_iLeaderboardRank_TopLife;
@synthesize m_iLeaderboardRank_Monster;
@synthesize m_iLeaderboardRank_TopSeed;
@synthesize m_iLeaderboardRank_TopCoins;
@synthesize m_iLeaderboardRank_Time_W2;
@synthesize m_iLeaderboardRank_Time_W3;
@synthesize m_iLeaderboardRank_Time_W4;
@synthesize m_iLeaderboardRank_Time_W5;
@synthesize m_iLeaderboardRank_Time_W6;
@synthesize m_iLeaderboardRank_Time_W7;
@synthesize m_iLeaderboardRank_Time_W8;
@synthesize m_iLeaderboardRank_Time_W9;
@synthesize m_iLeaderboardRank_Time_W10;
@synthesize m_iLeaderboardRank_Time_B1;
@synthesize m_iLeaderboardRank_Time_B2;
@synthesize m_iLeaderboardRank_Time_B3;
@synthesize m_iLeaderboardRank_Time_11_1;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------  
// Static functions/variables  
//--------------------------------------------------------  

static NSString *getGameCenterSavePath()  
{  
	//NSLog( @"getGameCenterSavePath()\n" );
	
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);  
    return [NSString stringWithFormat:@"%@/GameCenterSave.txt",[paths objectAtIndex:0]];  
}  


//static NSString* scoresArchiveKey		= @"Scores";

static NSString* achievementsArchiveKey = @"Achievements";

//--------------------------------------------------------  
// Initialize 
//--------------------------------------------------------  
- (id)init
{
	m_bLocalPlayerIsAuthenticated = NO;  
    m_bGameCenterAvailableOnDevice = NO;
	m_bIsAuthenticating = NO;
	
	m_iLeaderboardRank_TopLife =
	m_iLeaderboardRank_Monster =
	m_iLeaderboardRank_TopSeed =
	m_iLeaderboardRank_TopCoins =
	m_iLeaderboardRank_Time_W2 =
	m_iLeaderboardRank_Time_W3 =
	m_iLeaderboardRank_Time_W4 =
	m_iLeaderboardRank_Time_W5 =
	m_iLeaderboardRank_Time_W6 =
	m_iLeaderboardRank_Time_W7 =
	m_iLeaderboardRank_Time_W8 =
	m_iLeaderboardRank_Time_W9 =
	m_iLeaderboardRank_Time_W10 =
	m_iLeaderboardRank_Time_B1 =
	m_iLeaderboardRank_Time_B2 =
	m_iLeaderboardRank_Time_B3 =
	m_iLeaderboardRank_Time_11_1 = -1;
	
	return [super init];
}

//--------------------------------------------------------  
// Authentication  
//--------------------------------------------------------  

- (void)authenticateLocalPlayer 
{  
	//NSLog( @"authenticateLocalPlayer" );
	
	m_bLocalPlayerIsAuthenticated = NO; // Assume the player isn't authenticated
	m_bGameCenterAvailableOnDevice = _IsGameCenterAvailableOnDevice();
	
	if( m_bGameCenterAvailableOnDevice == NO )
        return;
	
	// Allocate the GameCenter member variables
    m_pGameCenterViewController	= [[UIViewController alloc] init];
	m_pAchievementDescriptionDictionary	= [[NSMutableDictionary alloc] init];
	m_pLocalPlayerAchievementDictionary = [[NSMutableDictionary alloc] init];
	
	m_bIsAuthenticating = YES;
	
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError* pError)
	{        
        if( pError == nil )
		{
			// Handle successful authentication
			m_bLocalPlayerIsAuthenticated = YES;
			m_bIsAuthenticating = NO;
			
            [self registerForAuthenticationNotification];  
			
			// Report any unreported Achievements
			[self LoadAchievements];
			
			// VK_LB_TOP_LIFE
			m_pLeaderboard_TopLife = [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_TopLife != nil )
			{
				[m_pLeaderboard_TopLife setCategory: @"VK_LB_TOP_LIFE"];
				m_pLeaderboard_TopLife.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_TopLife.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_TopLife loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				{
					if( pError == nil && pScores != nil )
					{
						NSLog( @"VK_LB_TOP_LIFE Rank: %d", m_pLeaderboard_TopLife.localPlayerScore.rank );
						m_iLeaderboardRank_TopLife = m_pLeaderboard_TopLife.localPlayerScore.rank;
					}
					else
						_ReportGameCenterError( __FUNCTION__, pError );  
				}];
			}
			
			// VK_LB_TOP_MONSTER
			m_pLeaderboard_Monster		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Monster != nil )
			{
				[m_pLeaderboard_Monster setCategory: @"VK_LB_TOP_MONSTER"];
				m_pLeaderboard_Monster.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Monster.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Monster loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				{
					if( pError == nil && pScores != nil )
					{
						NSLog( @"VK_LB_TOP_MONSTER Rank: %d", m_pLeaderboard_Monster.localPlayerScore.rank );
						m_iLeaderboardRank_Monster = m_pLeaderboard_Monster.localPlayerScore.rank;
					}
					else
						_ReportGameCenterError( __FUNCTION__, pError );
				}];
			}
			
			// VK_LB_TOP_SEED
			m_pLeaderboard_TopSeed		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_TopSeed != nil )
			{
				[m_pLeaderboard_TopSeed setCategory: @"VK_LB_TOP_SEED"];
				m_pLeaderboard_TopSeed.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_TopSeed.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_TopSeed loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TOP_SEED Rank: %d", m_pLeaderboard_TopSeed.localPlayerScore.rank );
						 m_iLeaderboardRank_TopSeed = m_pLeaderboard_TopSeed.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TOP_COINS
			m_pLeaderboard_TopCoins		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_TopCoins != nil )
			{
				[m_pLeaderboard_TopCoins setCategory: @"VK_LB_TOP_COINS"];
				m_pLeaderboard_TopCoins.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_TopCoins.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_TopCoins loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TOP_COINS Rank: %d", m_pLeaderboard_TopCoins.localPlayerScore.rank );
						 m_iLeaderboardRank_TopCoins = m_pLeaderboard_TopCoins.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W2
			m_pLeaderboard_Time_W2		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W2 != nil )
			{
				[m_pLeaderboard_Time_W2 setCategory: @"VK_LB_TIME_W2"];
				m_pLeaderboard_Time_W2.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W2.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W2 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W2 Rank: %d", m_pLeaderboard_Time_W2.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W2 = m_pLeaderboard_Time_W2.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W3
			m_pLeaderboard_Time_W3		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W3 != nil )
			{
				[m_pLeaderboard_Time_W3 setCategory: @"VK_LB_TIME_W3"];
				m_pLeaderboard_Time_W3.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W3.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W3 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W3 Rank: %d", m_pLeaderboard_Time_W3.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W3 = m_pLeaderboard_Time_W3.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W4
			m_pLeaderboard_Time_W4		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W4 != nil )
			{
				[m_pLeaderboard_Time_W4 setCategory: @"VK_LB_TIME_W4"];
				m_pLeaderboard_Time_W4.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W4.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W4 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W4 Rank: %d", m_pLeaderboard_Time_W4.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W4 = m_pLeaderboard_Time_W4.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W5
			m_pLeaderboard_Time_W5		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W5 != nil )
			{
				[m_pLeaderboard_Time_W5 setCategory: @"VK_LB_TIME_W5"];
				m_pLeaderboard_Time_W5.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W5.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W5 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W5 Rank: %d", m_pLeaderboard_Time_W5.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W5 = m_pLeaderboard_Time_W5.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W6
			m_pLeaderboard_Time_W6		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W6 != nil )
			{
				[m_pLeaderboard_Time_W6 setCategory: @"VK_LB_TIME_W6"];
				m_pLeaderboard_Time_W6.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W6.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W6 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W6 Rank: %d", m_pLeaderboard_Time_W6.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W6 = m_pLeaderboard_Time_W6.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W7
			m_pLeaderboard_Time_W7		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W7 != nil )
			{
				[m_pLeaderboard_Time_W7 setCategory: @"VK_LB_TIME_W7"];
				m_pLeaderboard_Time_W7.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W7.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W7 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W7 Rank: %d", m_pLeaderboard_Time_W7.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W7 = m_pLeaderboard_Time_W7.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W8
			m_pLeaderboard_Time_W8		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W8 != nil )
			{
				[m_pLeaderboard_Time_W8 setCategory: @"VK_LB_TIME_W8"];
				m_pLeaderboard_Time_W8.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W8.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W8 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W8 Rank: %d", m_pLeaderboard_Time_W8.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W8 = m_pLeaderboard_Time_W8.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W9
			m_pLeaderboard_Time_W9		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W9 != nil )
			{
				[m_pLeaderboard_Time_W9 setCategory: @"VK_LB_TIME_W9"];
				m_pLeaderboard_Time_W9.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W9.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W9 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W9 Rank: %d", m_pLeaderboard_Time_W9.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W9 = m_pLeaderboard_Time_W9.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W10
			m_pLeaderboard_Time_W10		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_W10 != nil )
			{
				[m_pLeaderboard_Time_W10 setCategory: @"VK_LB_TIME_W10"];
				m_pLeaderboard_Time_W10.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_W10.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_W10 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W10 Rank: %d", m_pLeaderboard_Time_W10.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_W10 = m_pLeaderboard_Time_W10.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_B1
			m_pLeaderboard_Time_B1		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_B1 != nil )
			{
				[m_pLeaderboard_Time_B1 setCategory: @"VK_LB_TIME_B1"];
				m_pLeaderboard_Time_B1.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_B1.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_B1 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_B1 Rank: %d", m_pLeaderboard_Time_B1.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_B1 = m_pLeaderboard_Time_B1.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_B2
			m_pLeaderboard_Time_B2		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_B2 != nil )
			{
				[m_pLeaderboard_Time_B2 setCategory: @"VK_LB_TIME_B2"];
				m_pLeaderboard_Time_B2.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_B2.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_B2 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_B2 Rank: %d", m_pLeaderboard_Time_B2.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_B2 = m_pLeaderboard_Time_B2.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_B3
			m_pLeaderboard_Time_B3		= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_B3 != nil )
			{
				[m_pLeaderboard_Time_B3 setCategory: @"VK_LB_TIME_B3"];
				m_pLeaderboard_Time_B3.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_B3.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_B3 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_B3 Rank: %d", m_pLeaderboard_Time_B3.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_B3 = m_pLeaderboard_Time_B3.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
			
			// VK_LB_TIME_W11_1
			m_pLeaderboard_Time_11_1	= [[GKLeaderboard alloc] init];
			if( m_pLeaderboard_Time_11_1 != nil )
			{
				[m_pLeaderboard_Time_11_1 setCategory: @"VK_LB_TIME_W11_1"];
				m_pLeaderboard_Time_11_1.playerScope = GKLeaderboardPlayerScopeGlobal;	// All players or just friends?
				m_pLeaderboard_Time_11_1.timeScope = GKLeaderboardTimeScopeAllTime;		// Today, This Week, or All Time?
				
				[m_pLeaderboard_Time_11_1 loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
				 {
					 if( pError == nil && pScores != nil )
					 {
						 NSLog( @"VK_LB_TIME_W11_1 Rank: %d", m_pLeaderboard_Time_11_1.localPlayerScore.rank );
						 m_iLeaderboardRank_Time_11_1 = m_pLeaderboard_Time_11_1.localPlayerScore.rank;
					 }
					 else
						 _ReportGameCenterError( __FUNCTION__, pError );
				 }];
			}
        }
		else
		{  
			//NSLog( @"authenticateLocalPlayer - FAILURE" );
			m_bLocalPlayerIsAuthenticated = NO;
			m_bIsAuthenticating = NO;
			
			// Handle unsuccessful authentication  
			_ReportGameCenterError( __FUNCTION__, pError );
			
			if( m_pGameCenterViewController )
				[m_pGameCenterViewController release];
			m_pGameCenterViewController = nil;
			
			if( m_pAchievementDescriptionDictionary )
				[m_pAchievementDescriptionDictionary release];
			m_pAchievementDescriptionDictionary = nil;
			
			if( m_pLocalPlayerAchievementDictionary )
				[m_pLocalPlayerAchievementDictionary release];
			m_pLocalPlayerAchievementDictionary = nil;
        }  
    }];
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)registerForAuthenticationNotification  
{  
	//NSLog( @"registerForAuthenticationNotification" );
	
    //NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    //[nc addObserver: self selector:@selector(authenticationChanged) name:GKPlayerAuthenticationDidChangeNotificationName object:nil];
	NSNotificationCenter* pNotificationCenter = [NSNotificationCenter defaultCenter];
	
	[pNotificationCenter addObserver:	self											// This will be the GameCenter instance object
						 selector:		@selector(authenticationChanged)				// The function 'authenticationChanged'
					     name:			GKPlayerAuthenticationDidChangeNotificationName 
						 object:		nil];											
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)authenticationChanged  
{  
	//NSLog( @"authenticationChanged" );
	
	m_bLocalPlayerIsAuthenticated = NO; // Assume the player isn't authenticated
	m_bGameCenterAvailableOnDevice = _IsGameCenterAvailableOnDevice();
	
	if( m_bGameCenterAvailableOnDevice == NO )
	{  
		//NSLog( @"authenticationChanged - Game Center NOT available" );
        return;  
    }  
	
    if( [GKLocalPlayer localPlayer].isAuthenticated )
	{         
        // Insert code here to handle a successful authentication.  
		m_bLocalPlayerIsAuthenticated = YES;
		
		//NSLog( @"authenticationChanged - Local Player is Authenticated" );
    }
	//else
	//{    
	//	//NSLog( @"authenticationChanged - Local Player is NOT Authenticated" );
    //}  
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (BOOL)isAuthenticated
{  
	//NSLog( @"isAuthenticated" );
	
    return m_bGameCenterAvailableOnDevice && m_bLocalPlayerIsAuthenticated;  
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// LEADERBOARD METHODS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) reportScore:		(int64_t) _iScore 
		 forLeaderboardID:	(NSString*) _pLeaderBoardIDString  
{  
	NSLog( @"(void)reportScore:(int64_t)score forCategory:(NSString*)category" );
	
    //if(!gameCenterAvailable)  
	//	return;  
	if( m_bGameCenterAvailableOnDevice == NO )
	{
		//NSLog( @"reportScore - Game Center NOT available" );
		return;
	}
	
    GKScore* pLeaderboardScore = [[[GKScore alloc] initWithCategory:_pLeaderBoardIDString] autorelease];  
    if( pLeaderboardScore )
	{  
        pLeaderboardScore.value = _iScore;      
		
        [pLeaderboardScore reportScoreWithCompletionHandler:^(NSError* pError) 
		{   
            if( pError == nil )
			{
				// RKS NOTE: This doesn't work, as the only way to get the user's rank is through GKLeaderboard
				[self OnScoreSuccessfullyReportedForLeaderboard: _pLeaderBoardIDString 
													  WithScore: pLeaderboardScore ];
				
				
			}
			else
			{
				_ReportGameCenterError( __FUNCTION__, pError );
			}
		}];   
    }  
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)reportScore:(GKScore *)scoreReporter  
{  
//	//NSLog( @"reportScore:(GKScore *)scoreReporter" );
//	  
//	if( m_bGameCenterAvailableOnDevice == NO )
//		return;
//	
//    if(scoreReporter)
//	{  
//        [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) 
//		{   
//            if (error != nil)
//			{  
//                // handle the reporting error  
//                [self saveScoreToDevice:scoreReporter];  
//            }  
//        }];   
//    }  
}  


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)saveScoreToDevice:(GKScore *)score  
{  
//	//NSLog( @"saveScoreToDevice" );
//	
//    NSString *savePath = getGameCenterSavePath();  
//	
//    // If scores already exist, append the new score.  
//    NSMutableArray *scores = [[[NSMutableArray alloc] init] autorelease];  
//    NSMutableDictionary *dict;  
//    if([[NSFileManager defaultManager] fileExistsAtPath:savePath])
//	{  
//        dict = [[[NSMutableDictionary alloc] initWithContentsOfFile:savePath] autorelease];  
//		
//        NSData *data = [dict objectForKey:scoresArchiveKey];  
//        if(data) 
//		{  
//            NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];  
//            scores = [unarchiver decodeObjectForKey:scoresArchiveKey];  
//            [unarchiver finishDecoding];  
//            [unarchiver release];  
//            [dict removeObjectForKey:scoresArchiveKey]; // remove it so we can add it back again later  
//        }  
//    }
//	else
//	{  
//        dict = [[[NSMutableDictionary alloc] init] autorelease];  
//    }  
//	
//    [scores addObject:score];  
//	
//    // The score has been added, now save the file again  
//    NSMutableData *data = [NSMutableData data];   
//    NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];  
//    [archiver encodeObject:scores forKey:scoresArchiveKey];  
//    [archiver finishEncoding];  
//    [dict setObject:data forKey:scoresArchiveKey];  
//    [dict writeToFile:savePath atomically:YES];  
//    [archiver release];  
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)retrieveScoresFromDevice  
{  
//	//NSLog( @"retrieveScoresFromDevice" );
//	
//    NSString *savePath = getGameCenterSavePath();  
//	
//    // If there are no files saved, return  
//    if(![[NSFileManager defaultManager] fileExistsAtPath:savePath])
//	{  
//        return;  
//    }  
//	
//    // First get the data  
//    NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithContentsOfFile:savePath];  
//    NSData *data = [dict objectForKey:scoresArchiveKey];  
//	
//    // A file exists, but it isn't for the scores key so return  
//    if(!data)
//	{  
//        return;  
//    }  
//	
//    NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];  
//    NSArray *scores = [unarchiver decodeObjectForKey:scoresArchiveKey];  
//    [unarchiver finishDecoding];  
//    [unarchiver release];  
//	
//    // remove the scores key and save the dictionary back again  
//    [dict removeObjectForKey:scoresArchiveKey];  
//    [dict writeToFile:savePath atomically:YES];  
//	
//	
//    // Since the scores key was removed, we can go ahead and report the scores again  
//    for(GKScore *score in scores)
//	{  
//        [self reportScore:score];  
//    }  
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//- (void)showLeaderboard  
//{  
//	//NSLog( @"showLeaderboard" );
//	 
//	if( m_bLocalPlayerIsAuthenticated == NO )
//		return;
//	
//    GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];      
//    if (leaderboardController != nil)
//	{  
//        leaderboardController.leaderboardDelegate = self;  
//		
//        UIWindow* window = [UIApplication sharedApplication].keyWindow;  
//        [window addSubview: m_pGameCenterViewController.view];  
//        [m_pGameCenterViewController presentModalViewController: leaderboardController animated: YES];  
//    }     
//}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController // This is called whent the Leaderboard View is closed by the user
{  
	NSLog( @"leaderboardViewControllerDidFinish" );
	
	[m_pGameCenterViewController dismissModalViewControllerAnimated:YES];
	
	//[viewController.view removeFromSuperview]; 
	[viewController.view.superview removeFromSuperview];
	[viewController release];
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) DisplayLeaderboard:	(NSString*)_pLeaderboardIDString
{
	NSLog( @"DisplayLeaderboard" );
	
	if( m_bLocalPlayerIsAuthenticated == NO )
	{
		// The Local Player has not been authenticated. Attempt authentication of Local Player. Also, make sure that Game Center is not in the middle of authenticating a local player.
		if( m_bIsAuthenticating == NO )
			[self authenticateLocalPlayer];
		
		return;
	}
	
	// Display the specfied Leaderboard
	GKLeaderboardViewController* pLeaderboardController = [[GKLeaderboardViewController alloc] init];
	if( pLeaderboardController != nil )
	{
		pLeaderboardController.leaderboardDelegate = self;
		
		pLeaderboardController.category = _pLeaderboardIDString;
		
		UIWindow* pWindow = [UIApplication sharedApplication].keyWindow;
		[pWindow addSubview: m_pGameCenterViewController.view];
		[m_pGameCenterViewController presentModalViewController: pLeaderboardController animated: YES];
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) DisplayLeaderboards
{
	NSLog( @"DisplayLeaderboards" );
	
	if( m_bLocalPlayerIsAuthenticated == NO )
	{
		// The Local Player has not been authenticated. Attempt authentication of Local Player. Also, make sure that Game Center is not in the middle of authenticating a local player.
		if( m_bIsAuthenticating == NO )
			[self authenticateLocalPlayer];
		
		return;
	}
	
	GKLeaderboardViewController* pLeaderboardController = [[GKLeaderboardViewController alloc] init];
	if( pLeaderboardController != nil )
	{
		pLeaderboardController.leaderboardDelegate = self;
		
		UIWindow* pWindow = [UIApplication sharedApplication].keyWindow;
		[pWindow addSubview: m_pGameCenterViewController.view];
		[m_pGameCenterViewController presentModalViewController: pLeaderboardController animated: YES];
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) OnScoreSuccessfullyReportedForLeaderboard: (NSString*) _pLeaderboardIDString
										 WithScore: (GKScore*) _pScore // This is not valid
{
	NSLog( @"OnScoreSuccessfullyReported" );
	
	if( _pLeaderboardIDString == nil || _pScore == nil )
		return;
	
	NSLog( @" - _pLeaderboardIDString: %@", _pLeaderboardIDString );
	//NSLog( @" - _pScore.rank (DOES NOT WORK):  %d", _pScore.rank );
	
	NSInteger iLeaderboardID = [self GetLeaderboardIDFromString: _pLeaderboardIDString];
	
	GKLeaderboard* pLeaderboard = nil;
	NSInteger* piLeaderboardRank = nil;
	
	switch( iLeaderboardID )
	{
		case GameCenterWrapper::VK_LB_TOP_LIFE:		pLeaderboard = m_pLeaderboard_TopLife;   piLeaderboardRank = &m_iLeaderboardRank_TopLife;	break;
		case GameCenterWrapper::VK_LB_TOP_MONSTER:	pLeaderboard = m_pLeaderboard_Monster;   piLeaderboardRank = &m_iLeaderboardRank_Monster;	break;
		case GameCenterWrapper::VK_LB_TOP_SEED:		pLeaderboard = m_pLeaderboard_TopSeed;   piLeaderboardRank = &m_iLeaderboardRank_TopSeed;	break;
		case GameCenterWrapper::VK_LB_TOP_COINS:	pLeaderboard = m_pLeaderboard_TopCoins;  piLeaderboardRank = &m_iLeaderboardRank_TopCoins;	break;
		case GameCenterWrapper::VK_LB_TIME_W2:		pLeaderboard = m_pLeaderboard_Time_W2;   piLeaderboardRank = &m_iLeaderboardRank_Time_W2;	break;
		case GameCenterWrapper::VK_LB_TIME_W3:		pLeaderboard = m_pLeaderboard_Time_W3;   piLeaderboardRank = &m_iLeaderboardRank_Time_W3;	break;
		case GameCenterWrapper::VK_LB_TIME_W4:		pLeaderboard = m_pLeaderboard_Time_W4;   piLeaderboardRank = &m_iLeaderboardRank_Time_W4;	break;
		case GameCenterWrapper::VK_LB_TIME_W5:		pLeaderboard = m_pLeaderboard_Time_W5;   piLeaderboardRank = &m_iLeaderboardRank_Time_W5;	break;
		case GameCenterWrapper::VK_LB_TIME_W6:		pLeaderboard = m_pLeaderboard_Time_W6;   piLeaderboardRank = &m_iLeaderboardRank_Time_W6;	break;
		case GameCenterWrapper::VK_LB_TIME_W7:		pLeaderboard = m_pLeaderboard_Time_W7;   piLeaderboardRank = &m_iLeaderboardRank_Time_W7;	break;
		case GameCenterWrapper::VK_LB_TIME_W8:		pLeaderboard = m_pLeaderboard_Time_W8;   piLeaderboardRank = &m_iLeaderboardRank_Time_W8;	break;
		case GameCenterWrapper::VK_LB_TIME_W9:		pLeaderboard = m_pLeaderboard_Time_W9;   piLeaderboardRank = &m_iLeaderboardRank_Time_W9;	break;
		case GameCenterWrapper::VK_LB_TIME_W10:		pLeaderboard = m_pLeaderboard_Time_W10;  piLeaderboardRank = &m_iLeaderboardRank_Time_W10;	break;
		case GameCenterWrapper::VK_LB_TIME_B1:		pLeaderboard = m_pLeaderboard_Time_B1;   piLeaderboardRank = &m_iLeaderboardRank_Time_B1;	break;
		case GameCenterWrapper::VK_LB_TIME_B2:		pLeaderboard = m_pLeaderboard_Time_B2;   piLeaderboardRank = &m_iLeaderboardRank_Time_B2;	break;
		case GameCenterWrapper::VK_LB_TIME_B3:		pLeaderboard = m_pLeaderboard_Time_B3;   piLeaderboardRank = &m_iLeaderboardRank_Time_B3;	break;
		case GameCenterWrapper::VK_LB_TIME_W11_1:	pLeaderboard = m_pLeaderboard_Time_11_1; piLeaderboardRank = &m_iLeaderboardRank_Time_11_1;	break;
		
		default:
			break;
	}
	
	if( pLeaderboard == nil || piLeaderboardRank == nil )
		return;
	
	[pLeaderboard loadScoresWithCompletionHandler:^(NSArray* pScores, NSError* pError)
	 {
		 if( pError == nil && pScores != nil )
		 {
			 //NSLog( @"VK_LB_TOP_LIFE Rank: %d", m_pLeaderboard_TopLife.localPlayerScore.rank );
			 //m_iLeaderboardRank_TopLife = m_pLeaderboard_TopLife.localPlayerScore.rank;
			 NSLog( @" - Loaded Leaderboard: %@", pLeaderboard.title );
			 NSLog( @" - Leaderboard Rank:   %d", pLeaderboard.localPlayerScore.rank );
			 *piLeaderboardRank = pLeaderboard.localPlayerScore.rank;
		 }
		 else
		 {
			 _ReportGameCenterError( __FUNCTION__, pError );
		 }
	 }];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (NSInteger) GetLeaderboardIDFromString : (NSString*) _pLeaderboardIDString
{
	NSLog( @"GetLeaderboardIDFromString" );
	
	if( _pLeaderboardIDString == nil )
		return -1;
	
	if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TOP_LIFE"] == YES )
		return GameCenterWrapper::VK_LB_TOP_LIFE;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TOP_MONSTER"] == YES )
		return GameCenterWrapper::VK_LB_TOP_MONSTER;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TOP_SEED"] == YES )
		return GameCenterWrapper::VK_LB_TOP_SEED;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TOP_COINS"] == YES )
		return GameCenterWrapper::VK_LB_TOP_COINS;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W2"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W2;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W3"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W3;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W4"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W4;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W5"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W5;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W6"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W6;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W7"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W7;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W8"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W8;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W9"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W9;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W10"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W10;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_B1"] == YES )
		return GameCenterWrapper::VK_LB_TIME_B1;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_B2"] == YES )
		return GameCenterWrapper::VK_LB_TIME_B2;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_B3"] == YES )
		return GameCenterWrapper::VK_LB_TIME_B3;
	else if( [_pLeaderboardIDString isEqualToString: @"VK_LB_TIME_W11_1"] == YES )
		return GameCenterWrapper::VK_LB_TIME_W11_1;
	
	
	NSLog( @"GetLeaderboardIDFromString - Unhandled Leaderboard ID String: %@", _pLeaderboardIDString );
	return -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// ACHIEVEMENT METHODS 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) reportAchievementIdentifier: (NSString*) _pAchievementIDString
		 percentComplete:			  (float) _fPercent
		 displayBanner:				  (BOOL) _bDisplayBanner;
{  
	//NSLog( @"reportAchievementIdentifier: %@", _pAchievementIDString );
	 
	if( m_bGameCenterAvailableOnDevice == NO )
		return;
	
    GKAchievement* pAchievement = [[[GKAchievement alloc] initWithIdentifier: _pAchievementIDString] autorelease];    
    if( pAchievement )
	{         
		pAchievement.percentComplete = _fPercent;
		
        [pAchievement reportAchievementWithCompletionHandler:^(NSError* pError)
		{  
			if( pError == nil )
			{
				//NSLog( @"Successfully Reported Achievement: %@ with %f%%", _pAchievementIDString, _fPercent );
				
				// No errors reporting this Achievement - save the Achievement to the Local Player's Earned Achievement Dictionary (only if 100%)
				if( m_pLocalPlayerAchievementDictionary && _fPercent >= 100.0f )
				{
					//NSLog( @"Adding Achievement %@ to Local Player's Earned Achievements", _pAchievementIDString );
					
					[m_pLocalPlayerAchievementDictionary setObject: pAchievement forKey:_pAchievementIDString];
					
					if( _bDisplayBanner )
					{
						// Display the Achievement Banner with the earned Achievement's Title
						GKAchievementDescription* pAchievementDescription = [m_pAchievementDescriptionDictionary objectForKey: _pAchievementIDString];
						if( pAchievementDescription )
						{
							NSString* pAchievementTitleString = [[NSString alloc] autorelease];
							pAchievementTitleString = pAchievementDescription.title;
							if( pAchievementTitleString )
							{
								// Convert the NSString to a c-string
								const char* pszAchievementTitle = [pAchievementTitleString cStringUsingEncoding: [NSString defaultCStringEncoding]];
								
								// Display the Achievement
								GameCenterWrapper::NotifyAchievementBannerToDisplay( pszAchievementTitle );
							}
						}
					}
				}
			}
			else
			{
				_ReportGameCenterError( __FUNCTION__, pError );
			}
        }];
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//- (void)reportAchievementIdentifier: (GKAchievement *)achievement  // RKS NOTE:  I don't think this is used.
//{     
//	//NSLog( @"reportAchievementIdentifier:(GKAchievement *)achievement" );
//	  
//	if( m_bGameCenterAvailableOnDevice == NO )
//		return;
//	
//    if( achievement )
//	{         
//        [achievement reportAchievementWithCompletionHandler:^(NSError *error)
//		{
//            //if( error != nil )
//			//{  
//            //    [self saveAchievementToDevice:achievement]; // This is to save the Achievements in case reporting doesn't work.
//            //}          
//        }];  
//    }  
//}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)saveAchievementToDevice:(GKAchievement *)achievement  
{  
	//NSLog( @"saveAchievementToDevice" );
	
    NSString *savePath = getGameCenterSavePath();  
	
    // If achievements already exist, append the new achievement.  
    NSMutableArray *achievements = [[[NSMutableArray alloc] init] autorelease];  
    NSMutableDictionary *dict;  
    if([[NSFileManager defaultManager] fileExistsAtPath:savePath])
	{  
        dict = [[[NSMutableDictionary alloc] initWithContentsOfFile:savePath] autorelease];  
		
        NSData *data = [dict objectForKey:achievementsArchiveKey];  
        if(data) 
		{  
            NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];  
            achievements = [unarchiver decodeObjectForKey:achievementsArchiveKey];  
            [unarchiver finishDecoding];  
            [unarchiver release];  
            [dict removeObjectForKey:achievementsArchiveKey]; // remove it so we can add it back again later  
        }  
    }
	else
	{  
        dict = [[[NSMutableDictionary alloc] init] autorelease];  
    }  
	
	
    [achievements addObject:achievement];  
	
    // The achievement has been added, now save the file again  
    NSMutableData *data = [NSMutableData data];   
    NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];  
    [archiver encodeObject:achievements forKey:achievementsArchiveKey];  
    [archiver finishEncoding];  
    [dict setObject:data forKey:achievementsArchiveKey];  
    [dict writeToFile:savePath atomically:YES];  
    [archiver release];   
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)retrieveAchievementsFromDevice
{  
//	//NSLog( @"retrieveAchievementsFromDevice" );
//	
//    NSString *savePath = getGameCenterSavePath();
//	
//    // If there are no files saved, return  
//    if(![[NSFileManager defaultManager] fileExistsAtPath:savePath])
//	{  
//        return;  
//    }  
//	
//    // First get the data
//    NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithContentsOfFile:savePath];  
//    NSData *data = [dict objectForKey:achievementsArchiveKey];  
//	
//    // A file exists, but it isn't for the achievements key so return  
//    if(!data)
//	{  
//        return;  
//    }  
//	
//    NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];  
//    NSArray *achievements = [unarchiver decodeObjectForKey:achievementsArchiveKey];  
//    [unarchiver finishDecoding];  
//    [unarchiver release];  
//	
//    // remove the achievements key and save the dictionary back again  
//    [dict removeObjectForKey:achievementsArchiveKey];  
//    [dict writeToFile:savePath atomically:YES];  
//	
//    // Since the key file was removed, we can go ahead and try to report the achievements again  
//    //for(GKAchievement *achievement in achievements)
//	//{  
//    //    [self reportAchievementIdentifier:achievement /*percentComplete: 100.0f*/ ];  
//    //}  
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController // This is called when the Achievement View is closed by the user
{  
	//NSLog( @"achievementViewControllerDidFinish" );
	
    [m_pGameCenterViewController dismissModalViewControllerAnimated:YES];
	
    //[viewController.view removeFromSuperview];
	[viewController.view.superview removeFromSuperview];
    [viewController release];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) DisplayAchievements
{
	//NSLog( @"DisplayAchievements" );
	
	if( m_bLocalPlayerIsAuthenticated == NO )
	{
		// The Local Player has not been authenticated. Attempt authentication of Local Player. Also, make sure that Game Center is not in the middle of authenticating a local player.
		if( m_bIsAuthenticating == NO )
			[self authenticateLocalPlayer];
		
		return;
	}
	
    GKAchievementViewController *achievements = [[GKAchievementViewController alloc] init];
    if (achievements != nil)
	{
        achievements.achievementDelegate = self;
		
        UIWindow* window = [UIApplication sharedApplication].keyWindow;
        [window addSubview: m_pGameCenterViewController.view];
        [m_pGameCenterViewController presentModalViewController: achievements animated: YES];
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

- (void) LoadAchievements
{
	//NSLog( @"LoadAchievements" );
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Load the Achievement Descriptions for ALL Achievements
	[GKAchievementDescription loadAchievementDescriptionsWithCompletionHandler: ^(NSArray* pAchievementDescriptionArray,
																			      NSError* pError )
	{
		// Process the array of Achievement Descriptions
		if( pAchievementDescriptionArray != nil )
		{
			//NSLog( @"---------------------------------------------------" );
			//NSLog( @" - Processing the array of Achievement Descriptions" );
			
			for( GKAchievementDescription* pAchievementDescription in pAchievementDescriptionArray )
			{
				// 'self' is probably not needed here
				[self.m_pAchievementDescriptionDictionary setObject	: pAchievementDescription					// Setting the object
														  forKey	: pAchievementDescription.identifier];		// Setting the key for the object
			}
		}
		
		// Handle Errors
		if( pError != nil )
			_ReportGameCenterError( __FUNCTION__, pError );
	}];
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Load the Local Player's EARNED Achievements
	[GKAchievement loadAchievementsWithCompletionHandler: ^( NSArray* pAchievementArray, // The '^' signifies a 'code block' to be executed, which is what the following curly braces are
														     NSError* pError ) 
	{
		// Process the array of Achievements
		if( pAchievementArray != nil )
		{
			//NSLog( @"---------------------------------------" );
			//NSLog( @" - Processing the array of Achievements (from Local Player)" );
			
			for( GKAchievement* pAchievement in pAchievementArray ) // The 'in' keyword is for "Fast Enumeration" with NSArray objects.
			{
				// 'self' is probably not needed here
				[self.m_pLocalPlayerAchievementDictionary setObject	: pAchievement
														  forKey	: pAchievement.identifier];
			}
			
			//NSLog( @" - Finished Processing Achievements array" );
			
			GameCenterWrapper::CheckAchievementIntegrity();
		}
		
		// Handle Errors
		if( pError != nil )
			_ReportGameCenterError( __FUNCTION__, pError );
	 }];
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------  

- (void) ResetAchievements
{
	//NSLog( @"ResetAchievements" );
	
	if( m_bLocalPlayerIsAuthenticated == NO )
		return;
	
	if( m_pLocalPlayerAchievementDictionary == nil )
		return;
	
	//NSLog( @" - Current list of locally saved achievements (which are about to be erased)" );
	
	for( NSString* keyString in m_pLocalPlayerAchievementDictionary )
	{
		//NSLog( @"%@", keyString );
		GKAchievement* pAchievement = [m_pLocalPlayerAchievementDictionary objectForKey: keyString];
		if( pAchievement )
		{
			NSLog( @" - identifier:            %@", pAchievement.identifier );
			NSLog( @" - percentComplete:       %f", pAchievement.percentComplete );
			NSLog( @" - completed:             %d", pAchievement.completed );
			//NSLog( @" - hidden:                %d", pAchievement.hidden );	
			//NSLog( @" - lastReportedDate:      %@", pAchievement.lastReportedDate );			
		}	
	}
	
	
	// Clear all locally saved Achievement objects
	[m_pLocalPlayerAchievementDictionary removeAllObjects];
	
	// Clear all progress saved on Game Center
	[GKAchievement resetAchievementsWithCompletionHandler: ^(NSError* pError)
	{
		if( pError != nil )
			_ReportGameCenterError( __FUNCTION__, pError );
	}];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------  

- (const char*) GetAchievementTitle: (NSString*) keyString
{
	if( m_pAchievementDescriptionDictionary == nil )
		return nil;
	
	GKAchievementDescription* pAchievementDescription = [m_pAchievementDescriptionDictionary objectForKey: keyString];
	if( pAchievementDescription == nil )
		return nil;
	
	NSString* achievementTitleString =  [[NSString alloc] autorelease];
	achievementTitleString = pAchievementDescription.title;
	
	if( achievementTitleString )
	{
		const char* szAchievementTitle = [achievementTitleString cStringUsingEncoding: [NSString defaultCStringEncoding]];
		
		//NSLog( @"%s", szAchievementTitle );
		
		return szAchievementTitle;
	}
	
	return nil;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------  

- (bool) HasAchievementBeenEarned: (NSString*) _pAchievementIDString
{
	if( m_pLocalPlayerAchievementDictionary == nil )
		return false;
	
	GKAchievement* pAchievement = [m_pLocalPlayerAchievementDictionary objectForKey: _pAchievementIDString];
	
	if( pAchievement == nil )
		return false;
	
	return (pAchievement.percentComplete >= 100.0f );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------  

- (void) ShutDownGameCenter
{  	
	if( m_pGameCenterViewController )
		[m_pGameCenterViewController release];
	
	if( m_pAchievementDescriptionDictionary )
		[m_pAchievementDescriptionDictionary release];
	
	if( m_pLocalPlayerAchievementDictionary )
		[m_pLocalPlayerAchievementDictionary release];
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

@end // @implementation GameCenter 

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

