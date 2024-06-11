//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "GameCenterWrapper.h"
#include "GameCenter.h"

#include "component/ComponentAchievementBanner.h"
#include "component/ComponentGlobals.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//extern g_pGameCenter;
extern GameCenter* g_pGameCenter;

extern S32 g_iNumPrices;

//extern BOOL _IsGameCenterAvailableOnDevice();

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

NSString* _ConvertAchievementIDToString( int _iAchievementID )
{
	switch( _iAchievementID )
	{
		case GameCenterWrapper::VK_BOOK_WORM				: return @"VK_BOOK_WORM";
		case GameCenterWrapper::VK_SHH						: return @"VK_SHH";
		case GameCenterWrapper::VK_MASTER_OF_SILENCE		: return @"VK_MASTER_OF_SILENCE";
		case GameCenterWrapper::VK_MAGMA_MASHER				: return @"VK_MAGMA_MASHER";
		case GameCenterWrapper::VK_MAGMA_BANE				: return @"VK_MAGMA_BANE";
		case GameCenterWrapper::VK_MAGMA_MASTER				: return @"VK_MAGMA_MASTER";
		case GameCenterWrapper::VK_TWOSTAR_VINEKING			: return @"VK_TWOSTAR_VINEKING";
		case GameCenterWrapper::VK_THREESTAR_VINEKING		: return @"VK_THREESTAR_VINEKING";
		case GameCenterWrapper::VK_HEALTHY_VINEKING			: return @"VK_HEALTHY_VINEKING";
		case GameCenterWrapper::VK_STRONG_VINEKING			: return @"VK_STRONG_VINEKING";
		case GameCenterWrapper::VK_VIGOROUS_VINEKING		: return @"VK_VIGOROUS_VINEKING";
		//case GameCenterWrapper::VK_IRON_VINEKING			: return @"VK_IRON_VINEKING";
		case GameCenterWrapper::VK_INITIATE_OF_POWER		: return @"VK_INITIATE_OF_POWER";
		case GameCenterWrapper::VK_POWER_ADEPT				: return @"VK_POWER_ADEPT";
		case GameCenterWrapper::VK_AGENT_OF_POWER			: return @"VK_AGENT_OF_POWER";
		//case GameCenterWrapper::VK_MASTER_OF_POWER			: return @"VK_MASTER_OF_POWER";
		case GameCenterWrapper::VK_SEED_SPEED_MASTER		: return @"VK_SEED_SPEED_MASTER";
		case GameCenterWrapper::VK_POWER_MASTER				: return @"VK_POWER_MASTER";
		case GameCenterWrapper::VK_VINE_SPEED_MASTER		: return @"VK_VINE_SPEED_MASTER";
		case GameCenterWrapper::VK_BLAVA_HUNTER				: return @"VK_BLAVA_HUNTER";
		case GameCenterWrapper::VK_BLAVA_BANE				: return @"VK_BLAVA_BANE";
		case GameCenterWrapper::VK_DRAGON_SKINNER			: return @"VK_DRAGON_SKINNER";
		case GameCenterWrapper::VK_DRAGON_BANE				: return @"VK_DRAGON_BANE";
		case GameCenterWrapper::VK_SMOGG_HUNTER				: return @"VK_SMOGG_HUNTER";
		case GameCenterWrapper::VK_SMOGG_BANE				: return @"VK_SMOGG_BANE";
		case GameCenterWrapper::VK_SEED_EATER				: return @"VK_SEED_EATER";
		case GameCenterWrapper::VK_SEED_HEAD				: return @"VK_SEED_HEAD";
		case GameCenterWrapper::VK_MASTER_OF_SEED			: return @"VK_MASTER_OF_SEED";
		case GameCenterWrapper::VK_CRYSTAL_HUNTER			: return @"VK_CRYSTAL_HUNTER";
		case GameCenterWrapper::VK_CRYSTAL_SMASHER			: return @"VK_CRYSTAL_SMASHER";
		case GameCenterWrapper::VK_CRYSTAL_BANE				: return @"VK_CRYSTAL_BANE";
		case GameCenterWrapper::VK_LIFE_GIVER				: return @"VK_LIFE_GIVER";
		case GameCenterWrapper::VK_FIELDS_OF_LIFE			: return @"VK_FIELDS_OF_LIFE";
		case GameCenterWrapper::VK_MASTER_OF_LIFE			: return @"VK_MASTER_OF_LIFE";
		case GameCenterWrapper::VK_THE_TRUE_VINEKING		: return @"VK_THE_TRUE_VINEKING";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_1_1		: return @"VK_LEVEL_COMPLETE_1_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_2_1		: return @"VK_LEVEL_COMPLETE_2_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_3_1		: return @"VK_LEVEL_COMPLETE_3_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_3_2		: return @"VK_LEVEL_COMPLETE_3_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_4_1		: return @"VK_LEVEL_COMPLETE_4_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_4_2		: return @"VK_LEVEL_COMPLETE_4_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_5_1		: return @"VK_LEVEL_COMPLETE_5_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_5_2		: return @"VK_LEVEL_COMPLETE_5_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_5_3		: return @"VK_LEVEL_COMPLETE_5_3";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_6_1		: return @"VK_LEVEL_COMPLETE_6_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_6_2		: return @"VK_LEVEL_COMPLETE_6_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_6_3		: return @"VK_LEVEL_COMPLETE_6_3";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_7_1		: return @"VK_LEVEL_COMPLETE_7_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_7_2		: return @"VK_LEVEL_COMPLETE_7_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_7_3		: return @"VK_LEVEL_COMPLETE_7_3";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_7_4		: return @"VK_LEVEL_COMPLETE_7_4";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_8_1		: return @"VK_LEVEL_COMPLETE_8_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_8_2		: return @"VK_LEVEL_COMPLETE_8_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_8_3		: return @"VK_LEVEL_COMPLETE_8_3";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_8_4		: return @"VK_LEVEL_COMPLETE_8_4";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_9_1		: return @"VK_LEVEL_COMPLETE_9_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_9_2		: return @"VK_LEVEL_COMPLETE_9_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_9_3		: return @"VK_LEVEL_COMPLETE_9_3";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_9_4		: return @"VK_LEVEL_COMPLETE_9_4";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_10_1		: return @"VK_LEVEL_COMPLETE_10_1";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_10_2		: return @"VK_LEVEL_COMPLETE_10_2";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_10_3		: return @"VK_LEVEL_COMPLETE_10_3";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_10_4		: return @"VK_LEVEL_COMPLETE_10_4";
		case GameCenterWrapper::VK_FAIRY_TALE_ENDING		: return @"VK_FAIRY_TALE_ENDING";
			
		case GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_01	: return @"VK_LEVEL_COMPLETE_MOM_01";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_02	: return @"VK_LEVEL_COMPLETE_MOM_02";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_03	: return @"VK_LEVEL_COMPLETE_MOM_03";
		case GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_04	: return @"VK_LEVEL_COMPLETE_MOM_04";
		
		case GameCenterWrapper::VK_RATE_US					: return @"VK_RATE_US";
		case GameCenterWrapper::VK_FACEBOOK					: return @"VK_FACEBOOK";
			
		default:
			NSLog( @"_ConvertAchievementIDToString - Unhandled Achievement ID: %d", _iAchievementID );
			break;	
	}
	
	return nil;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

NSString* _ConvertLeaderboardIDToString( int _iLeaderboardID )
{
	switch( _iLeaderboardID )
	{
		case GameCenterWrapper::VK_LB_TOP_LIFE		: return @"VK_LB_TOP_LIFE";
		case GameCenterWrapper::VK_LB_TOP_MONSTER	: return @"VK_LB_TOP_MONSTER";
		case GameCenterWrapper::VK_LB_TOP_SEED		: return @"VK_LB_TOP_SEED";
		case GameCenterWrapper::VK_LB_TOP_COINS		: return @"VK_LB_TOP_COINS";
		//case GameCenterWrapper::VK_LB_TIME_W1		: return @"VK_LB_TIME_W1";
		case GameCenterWrapper::VK_LB_TIME_W2		: return @"VK_LB_TIME_W2";
		case GameCenterWrapper::VK_LB_TIME_W3		: return @"VK_LB_TIME_W3";
		case GameCenterWrapper::VK_LB_TIME_W4		: return @"VK_LB_TIME_W4";
		case GameCenterWrapper::VK_LB_TIME_W5		: return @"VK_LB_TIME_W5";
		case GameCenterWrapper::VK_LB_TIME_W6		: return @"VK_LB_TIME_W6";
		case GameCenterWrapper::VK_LB_TIME_W7		: return @"VK_LB_TIME_W7";
		case GameCenterWrapper::VK_LB_TIME_W8		: return @"VK_LB_TIME_W8";
		case GameCenterWrapper::VK_LB_TIME_W9		: return @"VK_LB_TIME_W9";
		case GameCenterWrapper::VK_LB_TIME_W10		: return @"VK_LB_TIME_W10";
		case GameCenterWrapper::VK_LB_TIME_B1		: return @"VK_LB_TIME_B1";
		case GameCenterWrapper::VK_LB_TIME_B2		: return @"VK_LB_TIME_B2";
		case GameCenterWrapper::VK_LB_TIME_B3		: return @"VK_LB_TIME_B3";
		case GameCenterWrapper::VK_LB_TIME_W11_1	: return @"VK_LB_TIME_W11_1";
			
		default:
			NSLog( @"_ConvertLeaderboardIDToString - Unhandled Leaderboard ID: %d", _iLeaderboardID );
			break;
	}
	
	return nil;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::Authenticate()
{  
	//printf( "GameCenterWrapper::Authenticate()\n" );
	
    if( _IsGameCenterAvailableOnDevice() )
	{  
		//printf( "Game Center is AVAILABLE on the device -- allocating Local Player\n" );
		g_pGameCenter = [[GameCenter alloc] init];  
		[g_pGameCenter authenticateLocalPlayer];  
    }
	//else
	//{
	//	printf( "Game Center NOT available on the device\n" );
	//}
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::ReportScore( int _iLeaderboardID, int _iScore ) // If the category names are updated, make sure they are also updated in iTunes Connect.
{  
	printf( "GameCenterWrapper::ReportScore\n" );
	
	NSString* pLeaderboardIDString = [[NSString alloc] autorelease];
	
	pLeaderboardIDString = _ConvertLeaderboardIDToString( _iLeaderboardID );
	
	[g_pGameCenter reportScore:	_iScore
				   forLeaderboardID: pLeaderboardIDString];
	
//    NSString *categoryID = [[NSString alloc] autorelease];  
//    switch( _iCategory )
//	{  
//        case 0:  
//            categoryID = @"YOUR_CATEGORY_ID_0";  
//            break;  
//			
//        case 1:  
//            categoryID = @"YOUR_CATEGORY_ID_1";  
//            break;  
//    }  
//	
//	[g_pGameCenter reportScore:_iScore forCategory:categoryID];
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::ReportAchievement( int _iAchievement ) // This is the function to call to unlock Achievements
{  
	//printf( "GameCenterWrapper::ReportAchievement\n" );
	
    NSString* pAchievementIDString = [[NSString alloc] autorelease];
	
	pAchievementIDString = _ConvertAchievementIDToString( _iAchievement );
	//NSLog( @"ReportAchievement - Converted string: %@", pAchievementIDString );
	
    [g_pGameCenter reportAchievementIdentifier: pAchievementIDString 
				   percentComplete:				100.0f // This value is the percent complete for the Achievement. Here, it is forced to be 100%.
				   displayBanner:				true];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::ReportAchievementNoBanner( int _iAchievement ) // This is the function to call to unlock Achievements
{  
	//printf( "GameCenterWrapper::ReportAchievement\n" );
	
    NSString* pAchievementIDString = [[NSString alloc] autorelease];
	
	pAchievementIDString = _ConvertAchievementIDToString( _iAchievement );
	//NSLog( @"ReportAchievementNoBanner - Converted string: %@", pAchievementIDString );
	
    [g_pGameCenter reportAchievementIdentifier: pAchievementIDString 
				   percentComplete:				100.0f // This value is the percent complete for the Achievement. Here, it is forced to be 100%.
				   displayBanner:				false];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::ReportAchievementPercentage( int _iAchievement, float _fPercentage )
{
	//printf( "GameCenterWrapper::ReportAchievementPercentage\n" );
	
    NSString* pAchievementIDString = [[NSString alloc] autorelease];
	
	pAchievementIDString = _ConvertAchievementIDToString( _iAchievement );
	//NSLog( @"ReportAchievementPercentage - Converted string: %@", pAchievementIDString );
	
    [g_pGameCenter reportAchievementIdentifier: pAchievementIDString 
				   percentComplete:				_fPercentage // This value is the percent complete for the Achievement.
				   displayBanner:				true];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool GameCenterWrapper::IsAuthenticated()
{  
	//printf( "GameCenterWrapper::IsAuthenticated\n" );
	
    return [g_pGameCenter isAuthenticated];  
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::DisplayLeaderboard( int _iLeaderboardID )
{
	printf( "GameCenterWrapper::DisplayLeaderboard, ID: %d\n", _iLeaderboardID );
	
	NSString* pLeaderboardIDString = [[NSString alloc] autorelease];
	
	pLeaderboardIDString = _ConvertLeaderboardIDToString( _iLeaderboardID );
	
	[g_pGameCenter DisplayLeaderboard: pLeaderboardIDString];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::DisplayLeaderboards()
{  
	printf( "GameCenterWrapper::DisplayLeaderboards\n" );
	
    [g_pGameCenter DisplayLeaderboards];  
}  

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//void GameCenterWrapper::ReportScoreToLeaderboard( int _iScore, int _iLeaderboardID )
//{
//	printf( "GameCenterWrapper::ReportScoreToLeaderboard\n" );
//	
//	NSString* pLeaderboardIDString = [[NSString alloc] autorelease];
//	
//	pLeaderboardIDString = _ConvertLeaderboardIDToString( _iLeaderboardID );
//	
//	//int64_t
//}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::DisplayAchievements()
{  
	printf( "GameCenterWrapper::DisplayAchievements\n" );
	
	[g_pGameCenter DisplayAchievements];  
} 

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::ResetAchievements()
{
	//printf( "GameCenterWrapper::ResetAchievements\n" );
	[g_pGameCenter ResetAchievements];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

char* GameCenterWrapper::GetAchievementTitle( int _iAchievement )
{
	//printf( "GameCenterWrapper::GetAchievementTitle\n" );
	
    NSString* pAchievementID = [[NSString alloc] autorelease];

	pAchievementID = _ConvertAchievementIDToString( _iAchievement );
	//NSLog( @"GetAchievementTitle - Converted string: %@", pAchievementID );
	
	return [g_pGameCenter GetAchievementTitle: pAchievementID];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool GameCenterWrapper::IsGameCenterAvailable()
{
	return [g_pGameCenter isAuthenticated];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool GameCenterWrapper::HasAchievementBeenEarned( int _iAchievement )
{
	NSString* pAchievementID = [[NSString alloc] autorelease];
	
	pAchievementID = _ConvertAchievementIDToString( _iAchievement );
	//NSLog( @"HasAchievementBeenEarned - Converted string: %@", pAchievementID );
	
	return [g_pGameCenter HasAchievementBeenEarned: pAchievementID];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::CheckAchievementIntegrity()
{
	// Check save game vs earned achievements. Report unearned achievements to Game Center, but do not show the achievement banner.
	//printf( "Checking achievement integrity\n" );
	
	// VK_SHH
	if( CComponentGlobals::GetInstance().GetNumTimesMuteButtonPressed() >= 1 )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_SHH ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_SHH\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_SHH );
		}
	}
	
	// VK_MASTER_OF_SILENCE
	if( CComponentGlobals::GetInstance().GetNumTimesMuteButtonPressed() >= 5 )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_MASTER_OF_SILENCE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_MASTER_OF_SILENCE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_MASTER_OF_SILENCE );
		}
	}
	
	//	VK_TWOSTAR_VINEKING
	if( CComponentGlobals::GetInstance().GetNumLevelsWithTwoOrMoreStars() >= CComponentGlobals::GetInstance().GetNumLevels() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_TWOSTAR_VINEKING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_TWOSTAR_VINEKING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_TWOSTAR_VINEKING );
		}
	}
	
	//	VK_THREESTAR_VINEKING
	if( CComponentGlobals::GetInstance().GetNumLevelsWithThreeStars() >= CComponentGlobals::GetInstance().GetNumLevels() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_THREESTAR_VINEKING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_THREESTAR_VINEKING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_THREESTAR_VINEKING );
		}
	}
	
	// VK_HEALTHY_VINEKING
	if( CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullHealth() >= CComponentGlobals::GetAchievementGoalFullHealth01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_HEALTHY_VINEKING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_HEALTHY_VINEKING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_HEALTHY_VINEKING );
		}
	}
	
	// VK_STRONG_VINEKING
	if( CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullHealth() >= CComponentGlobals::GetAchievementGoalFullHealth02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_STRONG_VINEKING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_STRONG_VINEKING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_STRONG_VINEKING );
		}
	}
	
	// VK_VIGOROUS_VINEKING
	if( CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullHealth() >= CComponentGlobals::GetAchievementGoalFullHealth03() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_VIGOROUS_VINEKING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_VIGOROUS_VINEKING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_VIGOROUS_VINEKING );
		}
	}
	
	// VK_INITIATE_OF_POWER
	if( CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullPower() >= CComponentGlobals::GetAchievementGoalFullMana01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_INITIATE_OF_POWER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_INITIATE_OF_POWER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_INITIATE_OF_POWER );
		}
	}
	
	// VK_POWER_ADEPT
	if( CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullPower() >= CComponentGlobals::GetAchievementGoalFullMana02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_POWER_ADEPT ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_POWER_ADEPT\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_POWER_ADEPT );
		}
	}
	
	// VK_AGENT_OF_POWER
	if( CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullPower() >= CComponentGlobals::GetAchievementGoalFullMana03() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_AGENT_OF_POWER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_AGENT_OF_POWER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_AGENT_OF_POWER );
		}
	}
	
	// VK_SEED_SPEED_MASTER
	if( CComponentGlobals::GetInstance().GetXPLevelGreen() >= g_iNumPrices )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_SEED_SPEED_MASTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_SEED_SPEED_MASTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_SEED_SPEED_MASTER );
		}
	}
	
	// VK_POWER_MASTER
	if( CComponentGlobals::GetInstance().GetXPLevelBlue() >= g_iNumPrices )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_POWER_MASTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_POWER_MASTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_POWER_MASTER );
		}
	}
	
	// VK_VINE_SPEED_MASTER
	if( CComponentGlobals::GetInstance().GetXPLevelPurple() >= g_iNumPrices )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_VINE_SPEED_MASTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_VINE_SPEED_MASTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_VINE_SPEED_MASTER );
		}
	}
	
	// VK_BLAVA_HUNTER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_BLOB ) >= CComponentGlobals::GetAchievementGoalKilledBlob01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_BLAVA_HUNTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_BLAVA_HUNTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_BLAVA_HUNTER );
		}
	}
	
	// VK_BLAVA_BANE
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_BLOB ) >= CComponentGlobals::GetAchievementGoalKilledBlob02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_BLAVA_BANE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_BLAVA_BANE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_BLAVA_BANE );
		}
	}
	
	// VK_DRAGON_SKINNER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SERPENT ) >= CComponentGlobals::GetAchievementGoalKilledSerpent01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_DRAGON_SKINNER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_DRAGON_SKINNER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_DRAGON_SKINNER );
		}
	}
	
	// VK_DRAGON_BANE
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SERPENT ) >= CComponentGlobals::GetAchievementGoalKilledSerpent02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_DRAGON_BANE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_DRAGON_BANE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_DRAGON_BANE );
		}
	}
	
	
	// VK_SMOGG_HUNTER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE ) >= CComponentGlobals::GetAchievementGoalKilledSmoke01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_SMOGG_HUNTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_SMOGG_HUNTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_SMOGG_HUNTER );
		}
	}
	
	// VK_SMOGG_BANE
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE ) >= CComponentGlobals::GetAchievementGoalKilledSmoke02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_SMOGG_BANE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_SMOGG_BANE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_SMOGG_BANE );
		}
	}
	
	// VK_SEED_EATER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED ) >= CComponentGlobals::GetAchievementGoalSeedsCollected01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_SEED_EATER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_SEED_EATER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_SEED_EATER );
		}
	}
	
	// VK_SEED_HEAD
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED ) >= CComponentGlobals::GetAchievementGoalSeedsCollected02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_SEED_HEAD ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_SEED_HEAD\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_SEED_HEAD );
		}
	}
	
	// VK_MASTER_OF_SEED
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED ) >= CComponentGlobals::GetAchievementGoalSeedsCollected03() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_MASTER_OF_SEED ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_MASTER_OF_SEED\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_MASTER_OF_SEED );
		}
	}
	
	// VK_CRYSTAL_HUNTER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN ) >= CComponentGlobals::GetAchievementGoalCrystalsBroken01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_CRYSTAL_HUNTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_CRYSTAL_HUNTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_CRYSTAL_HUNTER );
		}
	}
	
	// VK_CRYSTAL_SMASHER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN ) >= CComponentGlobals::GetAchievementGoalCrystalsBroken02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_CRYSTAL_SMASHER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_CRYSTAL_SMASHER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_CRYSTAL_SMASHER );
		}
	}
	
	// VK_CRYSTAL_BANE
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN ) >= CComponentGlobals::GetAchievementGoalCrystalsBroken03() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_CRYSTAL_BANE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_CRYSTAL_BANE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_CRYSTAL_BANE );
		}
	}
	
	// VK_LIFE_GIVER
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) >= CComponentGlobals::GetAchievementGoalGrassGrown01() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LIFE_GIVER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LIFE_GIVER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LIFE_GIVER );
		}
	}
	
	// VK_FIELDS_OF_LIFE
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) >= CComponentGlobals::GetAchievementGoalGrassGrown02() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_FIELDS_OF_LIFE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_FIELDS_OF_LIFE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_FIELDS_OF_LIFE );
		}
	}
	
	// VK_MASTER_OF_LIFE
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) >= CComponentGlobals::GetAchievementGoalGrassGrown03() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_MASTER_OF_LIFE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_MASTER_OF_LIFE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_MASTER_OF_LIFE );
		}
	}
	
	// VK_THE_TRUE_VINEKING
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) >= CComponentGlobals::GetAchievementGoalGrassGrown04() )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_THE_TRUE_VINEKING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_THE_TRUE_VINEKING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_THE_TRUE_VINEKING );
		}
	}
	
	// Level Complete Achievements
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_1_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_1_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_1_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_2_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_2_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_2_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_3_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_3_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_3_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_3_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_3_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_3_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_4_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_4_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_4_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_4_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_4_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_4_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_5_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_5_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_5_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_5_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_5_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_5_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_5_3 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_5_3\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_5_3 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_6_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_6_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_6_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_6_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_6_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_6_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_6_3 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_6_3\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_6_3 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_7_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_7_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_7_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_7_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_7_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_7_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_7_3 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_7_3\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_7_3 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_7_4 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_7_4\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_7_4 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_8_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_8_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_8_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_8_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_8_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_8_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_8_3 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_8_3\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_8_3 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_8_4 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_8_4\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_8_4 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_9_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_9_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_9_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_9_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_9_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_9_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_9_3 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_9_3\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_9_3 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_9_4 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_9_4\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_9_4 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_10_1 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_10_1\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_10_1 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_10_2 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_10_2\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_10_2 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_10_3 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_10_3\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_10_3 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_10_4 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_10_4\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_10_4 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_MAGMA_MASHER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_MAGMA_MASHER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_MAGMA_MASHER );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_MAGMA_BANE ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_MAGMA_BANE\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_MAGMA_BANE );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_MAGMA_MASTER ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_MAGMA_MASTER\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_MAGMA_MASTER );
		}
		
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_FAIRY_TALE_ENDING ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_FAIRY_TALE_ENDING\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_FAIRY_TALE_ENDING );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_01 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_MOM_01\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_01 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_02 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_MOM_02\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_02 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_03 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_MOM_03\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_03 );
		}
	}
	
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04 ) )
	{
		if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_04 ) == false )
		{
			//printf( " - Reporting unearned achievement: VK_LEVEL_COMPLETE_MOM_04\n" );
			GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_04 );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::NotifyAchievementBannerToDisplay( const char* _pszAchievementTitle )
{
	if( _pszAchievementTitle == nil )
		return;
	
	CComponentAchievementBanner::DisplayAchievement( _pszAchievementTitle );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool GameCenterWrapper::IsGameCenterAvailableOnDevice()
{
	return _IsGameCenterAvailableOnDevice();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

int GameCenterWrapper::GetRankForLeaderboard( int _iLeaderboardID )
{
	if( g_pGameCenter == nil )
		return -1;
	
	if( [g_pGameCenter isAuthenticated] == NO )
		return -1;
	
	if( _IsGameCenterAvailableOnDevice() == NO )
		return -1;
	
	switch( _iLeaderboardID )
	{
		case VK_LB_TOP_LIFE:	return [g_pGameCenter GetLeaderboardRank_TopLife];
		case VK_LB_TOP_MONSTER: return [g_pGameCenter GetLeaderboardRank_Monster];
		case VK_LB_TOP_SEED:	return [g_pGameCenter GetLeaderboardRank_TopSeed];
		case VK_LB_TOP_COINS:	return [g_pGameCenter GetLeaderboardRank_TopCoins];
		case VK_LB_TIME_W2:		return [g_pGameCenter GetLeaderboardRank_Time_W2];
		case VK_LB_TIME_W3:		return [g_pGameCenter GetLeaderboardRank_Time_W3];
		case VK_LB_TIME_W4:		return [g_pGameCenter GetLeaderboardRank_Time_W4];
		case VK_LB_TIME_W5:		return [g_pGameCenter GetLeaderboardRank_Time_W5];
		case VK_LB_TIME_W6:		return [g_pGameCenter GetLeaderboardRank_Time_W6];
		case VK_LB_TIME_W7:		return [g_pGameCenter GetLeaderboardRank_Time_W7];
		case VK_LB_TIME_W8:		return [g_pGameCenter GetLeaderboardRank_Time_W8];
		case VK_LB_TIME_W9:		return [g_pGameCenter GetLeaderboardRank_Time_W9];
		case VK_LB_TIME_W10:	return [g_pGameCenter GetLeaderboardRank_Time_W10];
		case VK_LB_TIME_B1:		return [g_pGameCenter GetLeaderboardRank_Time_B1];
		case VK_LB_TIME_B2:		return [g_pGameCenter GetLeaderboardRank_Time_B2];
		case VK_LB_TIME_B3:		return [g_pGameCenter GetLeaderboardRank_Time_B3];
		case VK_LB_TIME_W11_1:	return [g_pGameCenter GetLeaderboardRank_Time_11_1];
			
		default: NSLog( @"GetRankForLeaderboard - Unhandled Leaderboard ID: %d", _iLeaderboardID ); break;
	}
	
	return -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameCenterWrapper::Close()
{  
	//printf( "GameCenterWrapper::Close\n" );
	
    if( _IsGameCenterAvailableOnDevice() )
	{
		//printf( "Game Center is available on device -- shutting down Game Center\n" );
		
		[g_pGameCenter ShutDownGameCenter];
		[g_pGameCenter release];
    }
	//else
	//{
	//	printf( "Game Center NOT available on device\n" );
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
