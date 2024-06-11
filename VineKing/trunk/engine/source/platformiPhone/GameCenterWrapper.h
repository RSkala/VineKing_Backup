//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef GAMECENTERWRAPPER_H
#define GAMECENTERWRAPPER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace GameCenterWrapper
{  
    void Authenticate();
    void ReportScore( int, int );
    void ReportAchievement( int );
	void ReportAchievementNoBanner( int );
	void ReportAchievementPercentage( int, float );
    bool IsAuthenticated();
	
	void DisplayLeaderboard( int );
    void DisplayLeaderboards();
	//void ReportScoreToLeaderboard( int, int ); // Not used
	
    void DisplayAchievements();
    void Close();
	
	void ResetAchievements();
	char* GetAchievementTitle( int );
	
	bool IsGameCenterAvailable();
	bool HasAchievementBeenEarned( int );
	
	void CheckAchievementIntegrity();
	
	void NotifyAchievementBannerToDisplay( const char* );
	
	bool IsGameCenterAvailableOnDevice();
	
	int GetRankForLeaderboard( int );
	
	enum EGameCenterAchievements
	{
		INVALID_ACHIEVEMENT_ID = -1,
		
		VK_BOOK_WORM = 0,
		VK_SHH,
		VK_MASTER_OF_SILENCE,
		VK_MAGMA_MASHER,
		VK_MAGMA_BANE,
		VK_MAGMA_MASTER,
		VK_TWOSTAR_VINEKING,
		VK_THREESTAR_VINEKING,
		VK_HEALTHY_VINEKING,
		VK_STRONG_VINEKING,
		VK_VIGOROUS_VINEKING,
		//VK_IRON_VINEKING,
		VK_INITIATE_OF_POWER,
		VK_POWER_ADEPT,
		VK_AGENT_OF_POWER,
		//VK_MASTER_OF_POWER,
		VK_SEED_SPEED_MASTER,
		VK_POWER_MASTER,
		VK_VINE_SPEED_MASTER,
		VK_BLAVA_HUNTER,
		VK_BLAVA_BANE,
		VK_DRAGON_SKINNER,
		VK_DRAGON_BANE,
		VK_SMOGG_HUNTER,
		VK_SMOGG_BANE,
		VK_SEED_EATER,
		VK_SEED_HEAD,
		VK_MASTER_OF_SEED,
		VK_CRYSTAL_HUNTER,
		VK_CRYSTAL_SMASHER,
		VK_CRYSTAL_BANE,
		VK_LIFE_GIVER,
		VK_FIELDS_OF_LIFE,
		VK_MASTER_OF_LIFE,
		VK_THE_TRUE_VINEKING,
		VK_LEVEL_COMPLETE_1_1,
		VK_LEVEL_COMPLETE_2_1,
		VK_LEVEL_COMPLETE_3_1,
		VK_LEVEL_COMPLETE_3_2,
		VK_LEVEL_COMPLETE_4_1,
		VK_LEVEL_COMPLETE_4_2,
		VK_LEVEL_COMPLETE_5_1,
		VK_LEVEL_COMPLETE_5_2,
		VK_LEVEL_COMPLETE_5_3,
		VK_LEVEL_COMPLETE_6_1,
		VK_LEVEL_COMPLETE_6_2,
		VK_LEVEL_COMPLETE_6_3,
		VK_LEVEL_COMPLETE_7_1,
		VK_LEVEL_COMPLETE_7_2,
		VK_LEVEL_COMPLETE_7_3,
		VK_LEVEL_COMPLETE_7_4,
		VK_LEVEL_COMPLETE_8_1,
		VK_LEVEL_COMPLETE_8_2,
		VK_LEVEL_COMPLETE_8_3,
		VK_LEVEL_COMPLETE_8_4,
		VK_LEVEL_COMPLETE_9_1,
		VK_LEVEL_COMPLETE_9_2,
		VK_LEVEL_COMPLETE_9_3,
		VK_LEVEL_COMPLETE_9_4,
		VK_LEVEL_COMPLETE_10_1,
		VK_LEVEL_COMPLETE_10_2,
		VK_LEVEL_COMPLETE_10_3,
		VK_LEVEL_COMPLETE_10_4,
		VK_FAIRY_TALE_ENDING,
		
		VK_LEVEL_COMPLETE_MOM_01,
		VK_LEVEL_COMPLETE_MOM_02,
		VK_LEVEL_COMPLETE_MOM_03,
		VK_LEVEL_COMPLETE_MOM_04,
		
		VK_RATE_US,
		VK_FACEBOOK,
	};
	
	enum EGameCenterLeaderboards
	{
		INVALID_LEADERBOARD_ID = -1,
		
		VK_LB_TOP_LIFE = 0,
		VK_LB_TOP_MONSTER,
		VK_LB_TOP_SEED,
		VK_LB_TOP_COINS,
		//VK_LB_TIME_W1,
		VK_LB_TIME_W2,
		VK_LB_TIME_W3,
		VK_LB_TIME_W4,
		VK_LB_TIME_W5,
		VK_LB_TIME_W6,
		VK_LB_TIME_W7,
		VK_LB_TIME_W8,
		VK_LB_TIME_W9,
		VK_LB_TIME_W10,
		VK_LB_TIME_B1,
		VK_LB_TIME_B2,
		VK_LB_TIME_B3,
		VK_LB_TIME_W11_1,
	};
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // GAMECENTERWRAPPER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------