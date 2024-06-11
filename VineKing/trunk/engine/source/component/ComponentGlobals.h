//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTGLOBALS_H
#define COMPONENTGLOBALS_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "core/ITickable.h"
#include "platform/types.h"
#include "T2D/t2dVector.h"

#include "MouseEventEnums.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// VERSION NUMBERS - Updated 2011/05/16 (1.1)
//                   Updated 2012/07/02 (1.2)
//                   Updated 2012/09/22 (1.3)
//                   Updated 2014/03/08 (1.4)
const S32 g_iVersionNumber_Major = 1;
const S32 g_iVersionNumber_Minor = 4;

const F32 g_fSmallNumber = 0.001f;
const S32 g_iMaxLevelUpUnits = 35000; //1000;

const S32 g_iMaxXP = 2147483646; // MAX_INT: 2,147,483,647 //99999; // The maximum amount of XP the user can accumulate

// Layers used during the actual gameplay. This is not used for non-gameplay levels.
enum EGameplayLayers
{
	LAYER_FADE_BOX				 = 0,	// 0 - Fade In/Fade Out object should always be on top
	
	// Interface Objects
	LAYER_SCORE_SCREEN_ELEMENTS		,	// 1  - Score text, star ranking, time text
	LAYER_SCORE_SCREEN				,	// 2  - Score Screen 
	LAYER_YOU_WIN_GAME_OVER_DISPLAY	,	// 3  - "You Win" or "Game Over"
	
	LAYER_TUTORIAL_BUTTON			,	// 4  - The 'close tutorial' button
	LAYER_TUTORIAL_IMAGE			,	// 5  - The tutorial image that is displayed
	
	LAYER_PAUSE_SCREEN_ELEMENTS		,	// 6  - Buttons of Pause Screen
	LAYER_PAUSE_SCREENS				,	// 7  - The Pause Screen and Confirm Quit Screen
	LAYER_HUD_BUTTONS				,	// 8  - Mana Seed Button and Pause Button
	LAYER_HUD_BAR_FRAMES			,	// 9  - Frames around the Health and Mana bars
	LAYER_HUD_BARS					,	// 10 - The actual Health and Mana bars
	LAYER_HUD_BACKGROUND			,	// 11 - The HUD frame
	
	// Gameplay
	LAYER_VORTEX					,	// 12 - Player Vortex
	LAYER_LINE_DRAW					,	// 13 - The user's line draw
	LAYER_HIT_EFFECTS				,	// 14 - Enemy kill hit effects
	LAYER_PLAYER_VINE_ATTACK		,	// 15 - Vine Attack from player to enemies
	LAYER_BRICK_ATTACK_VINE			,	// 16 - The vine that travels to break bricks
	LAYER_BRICK_ATTACK_VINE_TRAIL	,	// 17 - The objects that follow the brick attack vine
	LAYER_INTRO_OUTRO_OBJECTS		,	// 18 - The player's intro/outro animations
	
	// Enemies and Player
	LAYER_ENEMY_BULLETS				,	// 19 - Bullets fired from Serpents and Boss Mountains
	LAYER_SMOKE						,	// 20 - Smoke enemies
	LAYER_DANGER_BRICK_HIT_EFFECT	,	// 21 - Effect that plays when the brick attack vine hits a danger brick
	LAYER_PLAYER_BLOB_SERPENT		,	// 22 - The Player, Blobs, and Serpents must set their layer order according to their positions (setLayerOrder())
	LAYER_INTRO_OUTRO_SMOKE			,	// 23 - The smoke that appears when the intro/outro objects stomp
	
	LAYER_SEEDS						,	// 24 - Seeds
	
	LAYER_CRYSTALS					,	// 25 - Crystals (Double Bricks)
	
	LAYER_BOSS_MOUNTAIN_PARTS		,	// 26 - Boss Mountain parts
	LAYER_BOSS_MOUNTAIN				,	// 27 - Boss Mountain
	
	LAYER_LAND_BRICKS				,	// 28 - Land brick objects (including danger bricks)
	LAYER_WATER_BACKGROUND			,	// 29 - Water background

	LAYER_PATH_GRID					,	// 30 - Path Grid should be behind everything
	
	NUM_LAYERS
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

enum ESaveDataIndex
{
	SAVE_DATA_INDEX_DATA_SAVED = 0,				// Whether the data was saved.  New Game = 0; Continue = 1. "Clear Data" will change this value to 0.
	
	SAVE_DATA_INDEX_TOTAL_XP,					// Current amount of XP the player has accumulated
	SAVE_DATA_INDEX_APPLIED_XP_UNITS_HEALTH,	// Amount of XP points applied to Health
	SAVE_DATA_INDEX_APPLIED_XP_UNITS_MANA,		// Amount of XP points applied to Mana
	SAVE_DATA_INDEX_APPLIED_XP_UNITS_SEED_SPEED,// Amount of XP points applied to Seed Speed
	
	SAVE_DATA_INDEX_XP_LEVEL_GREEN,			// Number of Green units purchased
	SAVE_DATA_INDEX_XP_LEVEL_BLUE,			// Number of Blue units purchased
	SAVE_DATA_INDEX_XP_LEVEL_PURPLE,		// Number of Purple units purchased
	
	// This saves which levels are complete. This is a horrible way to do it, but it is quick.
	// 0 = Incomplete;  1 = 1 Star earned;  2 = 2 Stars earned;  3 = 3 Stars earned;
	SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03,
	
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP,				// Displayed in Map Screen
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_POWER_UP,		// Displayed in Power Up Screen
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SERPENT,			// Displayed in Level_03_01
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_CRYSTAL,			// Displayed in Level_03_02
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_LAVAPIT,			// Displayed in Level_04_01
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SMOKE,			// Displayed in Level_04_02
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_BOSS,			// Displayed in Boss_01
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_BLOB,		// Displayed in Level_05_03
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SERPENT,	// Displayed in Level_06_02
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SMOKE,		// Displayed in Level_07_01
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER01,	// Displayed in Map Screen
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER02,	// Displayed in Map Screen
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER03,	// Displayed in Map Screen
	
	SAVE_DATA_INDEX_NUM_KILLED_BLOB,		// Achievement at 50 and 100
	SAVE_DATA_INDEX_NUM_KILLED_SERPENT,		// Achievement at 50 and 100
	SAVE_DATA_INDEX_NUM_KILLED_SMOKE,		// Achievement at 50
	
	SAVE_DATA_INDEX_SEEDS_COLLECTED,		// Achievement at 500, 1000, and 2000
	SAVE_DATA_INDEX_CRYSTALS_BROKEN,		// Achievement at 50, 100, and 300
	SAVE_DATA_INDEX_GRASS_GROWN,			// Achievement at 100, 500, 1000, and 5000
	
	SAVE_DATA_INDEX_NUM_LEVELS_BEATEN_WITH_MAX_HEALTH,	// Achievement at 5, 20, and 30
	SAVE_DATA_INDEX_NUM_LEVELS_BEATEN_WITH_MAX_POWER,	// Achievement at 1, 3, 5, and 10
	
	SAVE_DATA_INDEX_NUM_TIMES_MUTE_BUTTON_PRESSED,
	
	SAVE_DATA_INDEX_NUM_UPGRADES_PURCHASED,
	
	SAVE_DATA_INDEX_CURRENT_TIP_TEXT_INDEX,
	
	
	
	// NEW SAVE DATA VALUES ADDED 06/17/2011
	SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP_DLC_INTRO,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_05,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_06,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_07,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_08,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_09,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_10,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_11,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_12,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_13,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_14,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_15,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_16,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_17,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_18,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_19,
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_20,
	
	SAVE_DATA_INDEX_LEVEL_COMPLETE_11_21,
	
	// NEW SAVE DATA VALUES ADDED 7/1/2011 - TIMES SHOULD BE IN MILLISECONDS
	// RKS TODO: SAVE AS MILLISECONDS, THEN UPLOAD PROPER FORMAT -- MAKE SURE TIMES ARE ONLY UPLOADED WHEN LEVEL IS FINISHED!!!
	SAVE_DATA_INDEX_LEVEL_TIME_01_01, // Tutorial Level: Not Used
	
	SAVE_DATA_INDEX_LEVEL_TIME_02_01,
	
	SAVE_DATA_INDEX_LEVEL_TIME_03_01,
	SAVE_DATA_INDEX_LEVEL_TIME_03_02,
	
	SAVE_DATA_INDEX_LEVEL_TIME_04_01,
	SAVE_DATA_INDEX_LEVEL_TIME_04_02,
	
	SAVE_DATA_INDEX_LEVEL_TIME_05_01,
	SAVE_DATA_INDEX_LEVEL_TIME_05_02,
	SAVE_DATA_INDEX_LEVEL_TIME_05_03,
	
	SAVE_DATA_INDEX_LEVEL_TIME_06_01,
	SAVE_DATA_INDEX_LEVEL_TIME_06_02,
	SAVE_DATA_INDEX_LEVEL_TIME_06_03,
	
	SAVE_DATA_INDEX_LEVEL_TIME_07_01,
	SAVE_DATA_INDEX_LEVEL_TIME_07_02,
	SAVE_DATA_INDEX_LEVEL_TIME_07_03,
	SAVE_DATA_INDEX_LEVEL_TIME_07_04,
	
	SAVE_DATA_INDEX_LEVEL_TIME_08_01,
	SAVE_DATA_INDEX_LEVEL_TIME_08_02,
	SAVE_DATA_INDEX_LEVEL_TIME_08_03,
	SAVE_DATA_INDEX_LEVEL_TIME_08_04,
	
	SAVE_DATA_INDEX_LEVEL_TIME_09_01,
	SAVE_DATA_INDEX_LEVEL_TIME_09_02,
	SAVE_DATA_INDEX_LEVEL_TIME_09_03,
	SAVE_DATA_INDEX_LEVEL_TIME_09_04,
	
	SAVE_DATA_INDEX_LEVEL_TIME_10_01,
	SAVE_DATA_INDEX_LEVEL_TIME_10_02,
	SAVE_DATA_INDEX_LEVEL_TIME_10_03,
	SAVE_DATA_INDEX_LEVEL_TIME_10_04,
	
	SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01,
	SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02,
	SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03,
	
	
	SAVE_DATA_INDEX_LEVEL_TIME_11_01,
	SAVE_DATA_INDEX_LEVEL_TIME_11_02,
	SAVE_DATA_INDEX_LEVEL_TIME_11_03,
	SAVE_DATA_INDEX_LEVEL_TIME_11_04,
	SAVE_DATA_INDEX_LEVEL_TIME_11_05,
	SAVE_DATA_INDEX_LEVEL_TIME_11_06,
	SAVE_DATA_INDEX_LEVEL_TIME_11_07,
	SAVE_DATA_INDEX_LEVEL_TIME_11_08,
	SAVE_DATA_INDEX_LEVEL_TIME_11_09,
	SAVE_DATA_INDEX_LEVEL_TIME_11_10,
	SAVE_DATA_INDEX_LEVEL_TIME_11_11,
	SAVE_DATA_INDEX_LEVEL_TIME_11_12,
	SAVE_DATA_INDEX_LEVEL_TIME_11_13,
	SAVE_DATA_INDEX_LEVEL_TIME_11_14,
	SAVE_DATA_INDEX_LEVEL_TIME_11_15,
	SAVE_DATA_INDEX_LEVEL_TIME_11_16,
	SAVE_DATA_INDEX_LEVEL_TIME_11_17,
	SAVE_DATA_INDEX_LEVEL_TIME_11_18,
	SAVE_DATA_INDEX_LEVEL_TIME_11_19,
	SAVE_DATA_INDEX_LEVEL_TIME_11_20,
	SAVE_DATA_INDEX_LEVEL_TIME_11_21,
	
	SAVE_DATA_INDEX_XP_SPENT_TOTAL,

	NUM_SAVE_DATA_VALUES,
};

#define NUM_LEVELS ( (SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 + 1) - SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 )

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyProjectileBrickAttack;
class CComponentEnemySpawner;
class CComponentManaBar;
class CComponentLandBrick;
class CComponentLineDrawAttack;
class CComponentWinLevel;

class GuiBitmapCtrl;

class t2dAnimatedSprite;
class t2dSceneObject;
//class t2dSceneGraph;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentGlobals : public virtual ITickable
{
	public:
		static CComponentGlobals& GetInstance();
	
		void OnPostInit();
	
		// For random numbers
		U32 GetRandomRange( const S32&, const S32& );
	
		// Home Base related functions
		void OnAllTilesFullyGrown();
	
		// Win Level related functions
		void SetLevelComplete() { m_bLevelComplete = true; }
		bool IsLevelComplete() const { return m_bLevelComplete; }
		//bool HasLevelEnded() const;
	
		// Line Draw Attack functions
		void SetLineDrawAttackComponent( CComponentLineDrawAttack* const );
		void OnLineAttackObjectPathFinished();
	
		bool IsPlayerTouchingEnemy() const { return m_bPlayerTouchingEnemy; }
		void SetPlayerTouchingEnemy() { m_bPlayerTouchingEnemy = true; }
		void SetPlayerNotTouchingEnemy() { m_bPlayerTouchingEnemy = false; }
	
		// Brick attack projectile functions
		void FireBrickAttackProjectileAtPosition( const t2dVector&, const t2dVector& );
		void FireBossBrickAttackProjectileAtPosition( const t2dVector&, const t2dVector & );
	
		// Path Grid related functions
		void IncrementTotalNumBreakableLandBricks() { ++m_iTotalNumBreakableLandBricks; } // SAVE!
		void OnGrassBrickFullyGrown( const t2dVector& _vGrassBrickPosition ); // SAVE
	
		// Loading levels
		void ScheduleLoadLevel( const char* );
		void ScheduleLoadLevelNoSave( const char* );
		
		// Land Brick / Double Brick functions
		void AddLandBrickToCollisionEnableList( CComponentLandBrick* );
		void RemoveLandBrickFromCollisionEnableList( CComponentLandBrick* );
	
		// XP
		S32 GetTotalXP() const { return m_iTotalXP; }
		void AddToTotalXP( const S32& _iXP ) { m_iTotalXP += _iXP; if( m_iTotalXP > g_iMaxXP ) m_iTotalXP = g_iMaxXP; }
		void SubtractFromTotalXP( const S32& _iXP );
	
		void AddToTotalHealthXP( const S32& _iXP )	{ m_iTotalHealthXP	+= _iXP; if( m_iTotalHealthXP	> g_iMaxLevelUpUnits ) m_iTotalHealthXP = g_iMaxLevelUpUnits; }
		void AddToTotalSpeedXP( const S32& _iXP )	{ m_iTotalSpeedXP	+= _iXP; if( m_iTotalSpeedXP	> g_iMaxLevelUpUnits ) m_iTotalSpeedXP	= g_iMaxLevelUpUnits; }
		void AddToTotalManaXP( const S32& _iXP )	{ m_iTotalManaXP	+= _iXP; if( m_iTotalManaXP		> g_iMaxLevelUpUnits ) m_iTotalManaXP	= g_iMaxLevelUpUnits; }
	
		S32 GetTotalHealthXP() const { return m_iTotalHealthXP; }
		S32 GetTotalSpeedXP() const { return m_iTotalSpeedXP; }
		S32 GetTotalManaXP() const { return m_iTotalManaXP; }
	
		S32 GetMaxLevelUpUnits() const { return m_iMaxLevelUpUnits; }
	
		F32 GetPercentHealthXPAdded() const {	return getMin( 1.0f, (F32)m_iTotalHealthXP / (F32)m_iMaxLevelUpUnits ); }
		F32 GetPercentSpeedXPAdded() const {	return getMin( 1.0f, (F32)m_iTotalSpeedXP  / (F32)m_iMaxLevelUpUnits ); }
		F32 GetPercentManaXPAdded() const {		return getMin( 1.0f, (F32)m_iTotalManaXP   / (F32)m_iMaxLevelUpUnits ); }
	
		void IncrementXPLevelGreen();
		void IncrementXPLevelBlue();
		void IncrementXPLevelPurple();
	
		S32 GetXPLevelGreen() const;
		S32 GetXPLevelBlue() const;
		S32 GetXPLevelPurple() const;
		S32 GetXPLevelMax() const;
	
		void AddToTotalXPSpent( const S32& );
		S32 GetTotalXPEarned(); // This returns the total amount of XP (coins) the user has earned over the entire game. This is the total of all current + all spent.
	
		// Level Up Screen related things
		void SetLevelUpScreenIsValid() { m_bLevelUpScreenIsValid = true; } // This function is to ensure that there is a Level Up Screen // RKS NOTE: IS THIS USED ANY MORE?????
	
		// Save / Load Data
		bool DoesSaveDataExist() const;
		bool LoadGameSettings();
		bool SaveGameSettings();
		void ClearSaveGameSettings();
	
		bool IsMapUnlocked( S32 ) const;	// Not needed
		bool IsLevelUnlocked( S32 ) const;	// Not needed
		bool IsLevelComplete( S32 ) const;
		S32 GetActiveLevelSaveIndex() const { return m_iActiveLevelSaveIndex; }
		void SetActiveLevelSaveIndex( const S32 _iIndex ) { m_iActiveLevelSaveIndex = _iIndex; }
		void MarkActiveLevelAsComplete( S32 );
		void MarkTutorialShown( S32 );
		bool HasTutorialBeenShown( S32 );
	
		void InitializeInGameTutorials();
	
		S32 GetLevelStarRanking( const S32 ) const;
	
		// Mouse Events - For non-mouselocked objects handling mouse events
		void SetLastMouseDownObject( t2dSceneObject* const _pObject ) { m_pLastMouseDownObject = _pObject; }
		void ClearLastMouseDownObject() { m_pLastMouseDownObject = NULL; }
		t2dSceneObject* GetLastMouseDownObject() const { return m_pLastMouseDownObject; }
		
		void SetLastMouseEnterObject( t2dSceneObject* const _pObject ) { m_pLastMouseEnterObject = _pObject; }
		void ClearLastMouseEnterObject() { m_pLastMouseEnterObject = NULL; }
		t2dSceneObject* GetLastMouseEnterObject() const { return m_pLastMouseEnterObject; }
	
		void SetTouchedObject( t2dSceneObject* const _pObject ) { m_pTouchedObject = _pObject; }
		void ClearTouchedObject() { m_pTouchedObject = NULL; }
		bool IsTouchingObject() const { return m_pTouchedObject != NULL; }
	
		void HandleGlobalMouseInput( const EMouseEvent, const t2dVector& );
	
		bool IsTouchingDown() const { return m_bTouchingDown; }		// Is the player touching anywhere on the screen?
		void SetIsTouchingDown() { m_bTouchingDown = true; }		// This is set regardless of whether the user is touching an object or not (set when anywhere on the screen is touched)
		void SetNotTouchingDown() { m_bTouchingDown = false; }		// This is set regardless of whether the user is touching an object or not (set when anywhere on the screen is touched)
		void SetFirstTouchDownObject( t2dSceneObject* _pObject ) { m_pFirstTouchDownObject = _pObject; }
		t2dSceneObject* const GetFirstTouchDownObject() const { return m_pFirstTouchDownObject; }
		void ClearFirstTouchDownObject() { m_pFirstTouchDownObject = NULL; }
	
		bool IsLevelInitializationComplete() const { return m_bLevelInitializationComplete; }
	
		// Inherited from ITickable
		virtual void interpolateTick( F32 delta ) { }
		virtual void processTick();
		virtual void advanceTime( F32 timeDelta ) { };
	
		// Handling dead time seed spawn
		void UpdateDeadTimeSeedSpawn();
		
		// Fade Screen functions
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
	
		void StartCameraShake( F32, F32 );
	
		bool IsBossLevel() const { return m_bIsBossLevel; }
		bool IsGameplayLevel() const { return m_bIsGameplayLevel; }
        bool IsTutorialLevel() const { return m_bIsTutorialLevel; }
	
		const char* GetCurrentLevelName() const { return m_pszCurrentLevelName; }
	
		void StopTimer() { m_bTimerStopped = true; }
		void StartTimer() { m_bTimerStopped = false; }
		bool IsTimerActive() const { return m_bTimerStopped == false; }
		F32 GetLevelTimeSeconds() const { return (F32)(m_uLevelTimerMS) * 0.001f; }
		U32 GetLevelTimeMilliseconds() const { return m_uLevelTimerMS; }
	
		void RecordCurrentLevelTime();
		void UploadScoresToGameCenter();
		void UploadCurrentLevelTimeToGameCenter();
	
		bool IsLoadingFinished() const { return m_bLoadingFinished; }
	
		// Game Center / Achievement Functions
		void IncrementAchievementTotal( const S32 );
		S32 GetAchievementTotal( const S32 );
	
	
		S32 GetNumLevelsWithTwoOrMoreStars() const;
		S32 GetNumLevelsWithThreeStars() const;
		
		S32 GetNumLevelsBeatenWithFullHealth() const;
		S32 GetNumLevelsBeatenWithFullPower() const;
	
		void IncrementNumLevelsBeatenWithFullHealth() const;
		void IncrementNumLevelsBeatenWithFullPower() const;
	
		S32 GetNumLevels() const { return NUM_LEVELS; }
	
		S32 GetNumTimesMuteButtonPressed() const;
		void IncrementNumTimesMuteButtonPressed();
	
		S32 GetNumUpgradesPurchased() const;
		void IncrementNumUpgradesPurchased();
	
		void InitializeHitEffects();
		void UpdateHitEffects();
		void PlayHitEffectAtPosition( const t2dVector& );
		void PlaySmokeHitEffectAtPosition( const t2dVector& );
		void PlayDangerBrickHitEffectAtPosition( const t2dVector& );
	
		void SetIOSDeviceType();
		bool IsOldIOSDeviceType();
		bool CanIOSDeviceSupportGameCenter();
	
		void InitializeLoadingTipText();
		void ShowLoadingTipText( const char* );
		void HideLoadingTipText();
	
	
		// Achievement Goal Functions
		static const S32 GetAchievementGoalKilledBlob01();
		static const S32 GetAchievementGoalKilledBlob02();		
	
		static const S32 GetAchievementGoalKilledSerpent01();
		static const S32 GetAchievementGoalKilledSerpent02();	
	
		static const S32 GetAchievementGoalKilledSmoke01();	
		static const S32 GetAchievementGoalKilledSmoke02();		
	
		static const S32 GetAchievementGoalSeedsCollected01();
		static const S32 GetAchievementGoalSeedsCollected02();	
		static const S32 GetAchievementGoalSeedsCollected03();	
	
		static const S32 GetAchievementGoalCrystalsBroken01();	
		static const S32 GetAchievementGoalCrystalsBroken02();	
		static const S32 GetAchievementGoalCrystalsBroken03();	
		
		static const S32 GetAchievementGoalGrassGrown01();	
		static const S32 GetAchievementGoalGrassGrown02();		
		static const S32 GetAchievementGoalGrassGrown03();		
		static const S32 GetAchievementGoalGrassGrown04();		
	
		static const S32 GetAchievementGoalFullHealth01();		
		static const S32 GetAchievementGoalFullHealth02();		
		static const S32 GetAchievementGoalFullHealth03();		
	
		static const S32 GetAchievementGoalFullMana01();		
		static const S32 GetAchievementGoalFullMana02();		
		static const S32 GetAchievementGoalFullMana03();
	
		static void OnDeviceResignActive();
	
		// Totals for Leaderboard scores
		S32 GetNumGrassGrown();
		S32 GetNumMonstersDefeated();
		S32 GetNumSeedsCollected();
		
		S32 GetLevelCompleteTimeMilliseconds( S32 );
	
		// Debug functions
		void Debug_UnlockAllLevels();
	
		inline bool Debug_IsSpawningDisabled() const { return m_bDebugSpawningDisabled; }
		void Debug_DisableSpawning() { m_bDebugSpawningDisabled = true; }
		void Debug_EnableSpawning() { m_bDebugSpawningDisabled = false; }
	
		inline bool Debug_IsUnlimitedManaEnabled() const { return m_bDebugUnlimitedMana; }
		void Debug_EnableUnlimitedMana() { m_bDebugUnlimitedMana = true; }
		void Debug_DisableUnlimitedMana() { m_bDebugUnlimitedMana = false; }
		
		inline bool Debug_IsPlayerInvincibleEnabled() const { return m_bDebugPlayerInvincible; }
		void Debug_EnablePlayerInvincible() { m_bDebugPlayerInvincible = true; }
		void Debug_DisablePlayerInvincible() { m_bDebugPlayerInvincible = false; }
	
	private:
		CComponentGlobals(); // Hide the Default Constructor
		CComponentGlobals( CComponentGlobals const &) : m_iMaxLevelUpUnits( 0 ){} // Hide the Copy Constructor
		~CComponentGlobals(){}
	
	private:
		enum EFadeState
		{
			FADE_STATE_NONE = 0,
			FADE_STATE_FADING_IN,
			FADE_STATE_FADING_OUT,
		};
		
	private:
		bool m_bLevelComplete;
	
		// Line Draw Attack objects
		CComponentLineDrawAttack* m_pLineDrawAttackComponent;

		bool m_bPlayerTouchingEnemy;
	
		// Brick Attack stuff
		t2dAnimatedSprite* m_pBrickAttackProjectileObject;			// The list of brick attack objects will be cloned from this
		Vector<t2dAnimatedSprite*> m_BrickAttackProjectileList;		// The list of brick attack objects
		Vector<CComponentEnemyProjectileBrickAttack*> m_BrickAttackProjectileComponentList;
	
		// Land Brick data
		S32 m_iTotalNumBreakableLandBricks;
	
		char m_szLoadLevelName[1024];
	
		Vector<CComponentLandBrick*> m_LandBrickCollisionEnableList; // Is this needed any more?
	
		Vector<t2dAnimatedSprite*> m_HitEffectList;
		S32 m_iCurrentHitEffectIndex;
	
		Vector<t2dAnimatedSprite*> m_SmokeHitEffectList;
		S32 m_iCurrentSmokeHitEffectIndex;
	
		Vector<t2dAnimatedSprite*> m_DangerBrickHitEffectList;
		S32 m_iCurrentDangerBrickHitEffectIndex;
	
		// XP
		S32 m_iTotalXP;
		S32 m_iTotalHealthXP;	// Amount of XP added to Health
		S32 m_iTotalSpeedXP;	// Amount of XP added to Speed
		S32 m_iTotalManaXP;		// Amount of XP added to Mana
	
		S32 m_iTotalXPSpent; // Amount of XP Spent. When spending XP, this value is incremented
		
		const S32 m_iMaxLevelUpUnits;
	
		// Level Up Screen
		bool m_bLevelUpScreenIsValid;
	
		GuiBitmapCtrl* m_pLoadingScreen;
		GuiBitmapCtrl* m_pLoadingScreenBG;
		GuiBitmapCtrl* m_pPVILoadingScreenImage;
	
		t2dSceneObject* m_pLastMouseDownObject;		// For checking the last object that was touched from 'onMouseDown'
		t2dSceneObject* m_pLastMouseEnterObject;	// For checking the last object that was touched from 'onMouseEnter'
	
		t2dSceneObject* m_pTouchedObject; // This is set ONLY from touching down on an object after not touching the screen (first touched)
	
		bool m_bTouchingDown; // Whether the user is currently touching down
		t2dSceneObject* m_pFirstTouchDownObject; // The first object that is touched when the player touched the screen
		
		S32 m_iActiveLevelSaveIndex;
	
		bool m_bLoadingExtensionTimerStarted;
		bool m_bLoadingExtensionTimerFinished;
		F32 m_fLoadingExtensionTimer;
	
		const char* m_pszCurrentLevelName;
		bool m_bIsBossLevel;
		bool m_bIsGameplayLevel;
        bool m_bIsTutorialLevel;
	
		U32 m_uLevelTimerMS;
		bool m_bTimerStopped;
	
		bool m_bDebugSpawningDisabled;
		bool m_bDebugUnlimitedMana;
		bool m_bDebugPlayerInvincible;
		
		bool m_bLevelInitializationComplete;
	
		bool m_bLoadingFinished;
	
		bool m_bGameCenterLoginAttempted;
	
		// These variables are here to fix the issue where the counts become incorrect (compared to Game Center) because of the player losing a level
		S32 m_iNumBlobsKilledThisLevel;
		S32 m_iNumSerpentsKilledThisLevel;
		S32 m_iNumSmokesKilledThisLevel;
		S32 m_iNumSeedsCollectedThisLevel;
		S32 m_iNumGrassGrownThisLevel;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTGLOBALS_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
