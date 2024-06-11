//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentGlobals.h"

#include "ComponentAchievementBanner.h"
#include "ComponentBossMountain.h"
#include "ComponentDoubleBrick.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentEnemyProjectileBrickAttack.h"
#include "ComponentEnemySmokePositionList.h"
#include "ComponentEnemySpawner.h"
#include "ComponentEnemySpawnPosition.h"
#include "ComponentHealthBar.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentLandBrick.h"
#include "ComponentLevelBeginSequence.h"
#include "ComponentLevelUpScreen.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentMainMenuScreen.h"
#include "ComponentManaBar.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentMapSelectMenu.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPauseScreen.h"
#include "ComponentScoringManager.h"
#include "ComponentSpawnController.h"
#include "ComponentStaticPath.h"
#include "ComponentStoryOpeningScreen.h"
#include "ComponentTutorialLevel.h"
#include "ComponentTutorialOpeningScreen.h"
#include "ComponentWinLevel.h"

#include "SoundManager.h"

#include "core/fileObject.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "gui/controls/guiTextCtrl.h"
#include "gui/controls/guiMLTextCtrl.h"
#include "math/mRandom.h"

#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dStaticSprite.h"
//#include "T2D/t2dSceneGraph.h"
#include "T2D/t2dSceneWindow.h"
#include "T2D/t2dTextObject.h"

#include "platformiPhone/GameCenterWrapper.h"
#include "platformiPhone/iPhoneInterfaceWrapper.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static MRandomLCG g_GlobalRandom;

// Brick Attack Projectiles
static const char g_szBrickAttackObjectName[32] = "SerpentBrickAttackProjectile";
static const U32 g_uMaxNumBrickAttackProjectiles = 50;

t2dStaticSprite* g_pLoadingScreenText = NULL;

static t2dSceneWindow* g_pSceneWindow = NULL;

// Save Data related globals
FileObject g_FileObject;
S32 g_SaveData[NUM_SAVE_DATA_VALUES]; // RKS TODO: Move this into the class
const char g_szSaveDataFileName[256] = "game/data/savegame/save.dat";

static const S32 g_iMaxXPLevel = 6; // Maximum XP Level that Green, Blue, and Purple can be

// Forced loading time extension
static const F32 g_fLoadingTimeExtensionDefaultSeconds	= 2.0f;
static const F32 g_fLoadingTimeExtensionLevelsSeconds	= 2.0f;
static const F32 g_fLoadingTimeExtensionLogoScreen		= 0.0f;
static const F32 g_fLoadingTimeExtensionMainMenu		= 1.0f;
static F32 g_fLoadingExtensionTimeSeconds = g_fLoadingTimeExtensionDefaultSeconds;

// Fade
static const char g_szBlackBox[32] = "BlackBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

// Timer
static const char g_szGameTimer_Text[32] = "GameTimer_Text";
static t2dTextObject* g_pGameTimer_Text = NULL;

// Hit Effects
static const char g_szHitEffectObject[32] = "HitFX_Object";
static t2dAnimatedSprite* g_pHitEffectObject = NULL;
static const S32 g_iNumHitEffectObjects = 4;
static const char g_szHitEffectAnimation[32] = "hit_fxAnimation";
static const t2dVector g_vSafeHitEffectPosition( 2000.0f, -2000.0f );

// Smoke Hit Effects
static const char g_szSmokeHitEffectObject[32] = "Smoke_HitFX_Object";
static const S32 g_iNumSmokeHitEffectObjects = 4;
static t2dAnimatedSprite* g_pSmokeHitEffectObject = NULL;
static const char g_szSmokeHitEffectAnimation[32] = "smoke_hit_fxAnimation";
static const t2dVector g_vSafeSmokeHitEffectPosition( 750.0f, 750.0f );

// Danger Brick Hit Effects
static const char g_szDangerBrickHitEffectObject[32] = "DangerBrick_HitEffect_Object";
static const S32 g_iNumDangerBrickHitEffectObjects = 2;
static t2dAnimatedSprite* g_pDangerBrickHitEffectObject = NULL;
static const char g_szDangerBrickHitEffectAnimation[32] = "DANGER_BRICK_DAMAGE";
static const t2dVector g_vSafeDangerBrickHitEffectPosition( 800.0f, 800.0f );

t2dAnimatedSprite* g_pTempHitEffect = NULL;

//		Seed Speed		Power	Vine Speed
//Base		80			500			285
//1			85			550			290
//2			90			600			295
//3			95			650			300
//4			100			750			305
//5			105			850			310
//6			120			1000		320

// Dead time seed spawning variables
static S32 g_iNumberOfDeadTimeSeeds = 3;
static const F32 g_fDeadTimeSeedSpawnDelaySeconds = 1.0f; // Time between dead time seed spawns

static F32 g_fDeadTimeSeedSpawnTimer = 0.0f;
static S32 g_iNumberOfDeadTimePositions = 0;
static bool g_bSpawningDeadTimeSeeds = false;
Vector<t2dVector> g_aSeedPositions;

// IOS Device Type
DeviceTypeWrapper::EIOSDeviceType g_eIOSDeviceType = DeviceTypeWrapper::DEVICE_TYPE_NONE;

static const F32 g_fExtraLoadTimeSeconds_iPodTouch2ndGen	= 5.0;
static const F32 g_fExtraLoadTimeSeconds_iPhone3G			= 5.0;

// Debug
static bool g_bShowFPS = false;
static S32 g_iStartingXP = 0; // Change this value to force the total amount of XP earned (Note: A saved game will take precedence over this value).

bool g_bDebugLoadingLevel = false;
bool g_bAllLevelsUnlocked = false;

bool g_bUseGameTimerDisplay = false;

bool g_bForceFullyUpgraded = false; // true = all upgrades are forced to be max value

// Loading Tip Text
static GuiTextCtrl* g_pLoadingTipTextLine01 = NULL;
static GuiMLTextCtrl* g_pLoadingTipTextLine02 = NULL;

static const char g_szLoadingTipTextLine01[32] = "LoadingTipText01";
static const char g_szLoadingTipTextLine02[32] = "LoadingTipText02";

// Loading Tip Text Strings
static const char g_szTextStringTip[8] = "TIP:";

static const S32 g_iMaxTipTextStringLength = 128;
static const char g_szTipTextStrings[][g_iMaxTipTextStringLength] =
{
	"Drawing a new line will cancel the previous one!",
	"When your line reaches its maximum length, it will turn red!",
	"Breaking Lava Rock uses up your green meter!",
	"Gather Seeds to fill your green meter!",
	"When the green meter is empty, you will not be able to draw lines!",
	"When you break a Lava Rock, a seed will sometimes appear.",
	"Monsters drop powerful seeds.",
	"Remember that you can stop gathering seeds when the green meter is full.",
	"Keep an eye on the red health bar. Monsters and Lava Pits hurt the VineKing!",
	"When the red health bar is empty, the game is over!",
	"If left alone too long, Seeds will disappear.",
	"Dragons build Lava Rocks faster than other monsters!",
	"Lava Crystals never drop seeds!",
	"Take advantage of slow moving monsters by drawing lines.",
	"Draw! Tap! Gather! The VineKing is a great multitasker! Are you?",
	"You can always revisit levels to earn more coins and try for a better star rating!",
	"Spend your hard earned coins on upgrades at the Shop!",	
	"The faster you finish a level, the higher your star rating!",
};

static const S32 g_iNumTipTextStrings = sizeof( g_szTipTextStrings ) / (sizeof( char ) * g_iMaxTipTextStringLength);

static const bool g_bDebugAlwaysShowTipText = false;

// Achievement Goals
// Killed Blobs
static const S32 g_iAchievementGoalKilledBlob01 = 100;
static const S32 g_iAchievementGoalKilledBlob02 = 300;

// Killed Serpents
static const S32 g_iAchievementGoalKilledSerpent01 = 100;
static const S32 g_iAchievementGoalKilledSerpent02 = 200;

// Killed Smoke monsters
static const S32 g_iAchievementGoalKilledSmoke01 = 150;
static const S32 g_iAchievementGoalKilledSmoke02 = 300;

// Num collected seeds
static const S32 g_iAchievementGoalSeedsCollected01 = 500;
static const S32 g_iAchievementGoalSeedsCollected02 = 1000;
static const S32 g_iAchievementGoalSeedsCollected03 = 2000;

// Num Crystals broken
static const S32 g_iAchievementGoalCrystalsBroken01 = 100;
static const S32 g_iAchievementGoalCrystalsBroken02 = 200;
static const S32 g_iAchievementGoalCrystalsBroken03 = 400;

// Num Grass Grown
static const S32 g_iAchievementGoalGrassGrown01 = 100;
static const S32 g_iAchievementGoalGrassGrown02 = 500;
static const S32 g_iAchievementGoalGrassGrown03 = 1000;
static const S32 g_iAchievementGoalGrassGrown04 = 5000;

// Num levels beaten with full health
static const S32 g_iAchievementGoalFullHealth01 = 5;
static const S32 g_iAchievementGoalFullHealth02 = 10;
static const S32 g_iAchievementGoalFullHealth03 = 20;

// Num levels beaten with full mana
static const S32 g_iAchievementGoalFullMana01 = 1;
static const S32 g_iAchievementGoalFullMana02 = 3;
static const S32 g_iAchievementGoalFullMana03 = 5;



//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// CComponentGlobals public functions
/*static*/ CComponentGlobals& CComponentGlobals::GetInstance()
{
	static CComponentGlobals instance;
	return instance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

t2dStaticSprite* pTestStaticSprite = NULL;

void CComponentGlobals::OnPostInit()
{
	//printf( "CComponentGlobals::OnPostInit\n" );
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( g_bUseGameTimerDisplay )
	{
		g_pGameTimer_Text = static_cast<t2dTextObject*>( Sim::findObject( g_szGameTimer_Text ) );
		if( g_pGameTimer_Text )
		{
			g_pGameTimer_Text->setVisible( true );
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if( g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G &&
		g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN &&
	    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN )
	{
		ActivityIndicatorWrapper::CreateIndicator();
	}
	
	InitializeHitEffects();

	g_bDebugLoadingLevel = false;
	
	// If enabled, show the FPS counter
	if( g_bShowFPS )
		Con::executef( 2, "metrics", "fps" );
	
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( "sceneWindow2D" ) );
	
	if( CComponentPauseScreen::DoesLevelHavePauseScreen() )
	{
		CComponentPauseScreen::GetInstance().InitializePauseScreenElements();
	}
	
	
	t2dSceneObject* pGridNumberObject = static_cast<t2dSceneObject*>( Sim::findObject( "GridNumbers" ) );
	if( pGridNumberObject )
		pGridNumberObject->setVisible( false );
	
	m_bPlayerTouchingEnemy = false;
	
	// Brick attack initialization
	m_pBrickAttackProjectileObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBrickAttackObjectName ) );
	
	m_BrickAttackProjectileList.clear();
	m_BrickAttackProjectileComponentList.clear();
	
	if( m_pBrickAttackProjectileObject )
	{
		m_pBrickAttackProjectileObject->setUsesPhysics( true );
		m_pBrickAttackProjectileObject->setLayer( LAYER_ENEMY_BULLETS );
		
		const char* pszOriginalName = m_pBrickAttackProjectileObject->getName();
		char szName[32];
		
		m_BrickAttackProjectileList.push_back( m_pBrickAttackProjectileObject );
		
		t2dAnimatedSprite* pNewBrickProjectile = NULL;
		for( U32 i = 1; i < g_uMaxNumBrickAttackProjectiles; ++i )
		{
			pNewBrickProjectile = static_cast<t2dAnimatedSprite*>( m_pBrickAttackProjectileObject->clone() );
			if( pNewBrickProjectile )
			{
				sprintf( szName, "%s_%d", pszOriginalName, i );
				
				m_BrickAttackProjectileList.push_back( pNewBrickProjectile );
				
				CComponentEnemyProjectileBrickAttack* pComponent = static_cast<CComponentEnemyProjectileBrickAttack*>( pNewBrickProjectile->getComponent( 0 ) );
				if( pComponent )
					m_BrickAttackProjectileComponentList.push_back( pComponent );
			}
		}
	}
	
	m_LandBrickCollisionEnableList.clear();
	
	m_pLastMouseDownObject = NULL;
	m_pLastMouseEnterObject = NULL;
	m_pTouchedObject = NULL;
	m_bTouchingDown = false;
	
	// Set the HUD position
	t2dSceneObject* pInGameHUD = static_cast<t2dSceneObject*>( Sim::findObject( "InGameHUD" ) );
	if( pInGameHUD )
	{
		pInGameHUD->setLayer( LAYER_HUD_BACKGROUND );
		pInGameHUD->setPosition( t2dVector::getZero() );
	}
	
	
	t2dSceneObject* pWaterObject = static_cast<t2dSceneObject*>( Sim::findObject( "Water_Object" ) );
	if( pWaterObject )
	{
		pWaterObject->setLayer( LAYER_WATER_BACKGROUND );
		pWaterObject->setPosition( t2dVector::getZero() );
	}
	
	CSoundManager::Initialize();
	
	CComponentLevelBeginSequence::InitializeBeginSequenceObjects();
	
//	if( CComponentMainMenuScreen::IsInMainMenuScreen() )
//		CComponentMainMenuScreen::GetInstance().StartMainMenuScreen();
//	
//	//if( CComponentMapSelectMenu::IsInMapSelectMenu() )
//	//	CSoundManager::GetInstance().PlayBGMMainMenuRPG();	
//	
//	if( CComponentMapSelectMenu::IsInMapSelectMenu() )
//		CSoundManager::GetInstance().PlayBGMMapScreen();
//	
//	if( CComponentLevelBeginSequence::DoesLevelHaveBeginSequence() )
//		CComponentLevelBeginSequence::GetInstance().StartLevelBeginSequence();
//	
//	if( CComponentStoryOpeningScreen::DoesLevelHaveStoryOpening() )
//		CComponentStoryOpeningScreen::StartStoryOpeningScreen();
//	
//	if( CComponentTutorialOpeningScreen::IsLevelTutorialOpeningScreen() )
//		CComponentTutorialOpeningScreen::PlayOpeningTutorialMusic();
	
	CComponentHealthBar::InitializeHealthBar();
	CComponentManaSeedItemDrop::InitializeManaSeedDropData();
	
	m_bTouchingDown = false;
	m_pFirstTouchDownObject = NULL;
	
	m_pszCurrentLevelName = Con::getVariable( "$CurrentLevelFileName" );
	//printf( "Current Level Name: %s\n", m_pszCurrentLevelName );
	
	m_uLevelTimerMS = 0;
	m_bTimerStopped = true;
	
	InitializeInGameTutorials();
	
	g_fLoadingExtensionTimeSeconds = g_fLoadingTimeExtensionDefaultSeconds;
	

	m_bIsGameplayLevel = true;
	
	g_fDeadTimeSeedSpawnTimer = 0.0f;
	g_iNumberOfDeadTimePositions = 0;
	g_bSpawningDeadTimeSeeds = false;
	g_aSeedPositions.clear();
	
	bool bSetTutorialInactive = true;
	if( dStricmp( m_pszCurrentLevelName, "Screen_PixelVandalsLogo.t2d" ) == 0 )
	{
		g_fLoadingExtensionTimeSeconds = g_fLoadingTimeExtensionLogoScreen;
		m_bIsGameplayLevel = false;
	}
	else if( dStricmp( m_pszCurrentLevelName, "Screen_MainMenu.t2d" ) == 0 )
	{
//		// In the Main Menu, try to log in to Game Center only once. After that, they need to press the Game Center button (which opens Achievements)
//		//if( m_bGameCenterLoginAttempted == false )
//		{
//			printf( "Authenticating Game Center...\n" );
//			GameCenter_Authenticate();
//			
//			m_bGameCenterLoginAttempted = true;
//		}
		
		g_fLoadingExtensionTimeSeconds = g_fLoadingTimeExtensionMainMenu;
		m_bIsGameplayLevel = false;
	}
	else if( dStricmp( m_pszCurrentLevelName, "Screen_MapSelect_PowerUp.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
		
		m_bIsGameplayLevel = false;
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_03_01.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_SERPENT ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_03_02.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_CRYSTAL ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_04_01.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_LAVAPIT ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_04_02.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_SMOKE ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Boss_01.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_BOSS ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_05_03.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_BLOB ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_06_02.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SERPENT ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_07_01.t2d" ) == 0 )
	{
		if( g_SaveData[ SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SMOKE ] == 0 )
		{
			CComponentInGameTutorialObject::SetInGameTutorialActive();
			bSetTutorialInactive = false;
		}
	}
	
	else if( dStricmp( m_pszCurrentLevelName, "Screen_StoryEnding.t2d" ) == 0 || 
			 dStricmp( m_pszCurrentLevelName, "Screen_StoryOpening.t2d" ) == 0 || 
			 dStricmp( m_pszCurrentLevelName, "Screen_TutorialOpening.t2d" ) == 0 )
	{
		m_bIsGameplayLevel = false;
	}
	
	//if( m_bIsGameplayLevel )
	{
		if( g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPHONE_ORIGINAL ||
		    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G ||
		    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN )
		{
			g_fLoadingExtensionTimeSeconds += g_fExtraLoadTimeSeconds_iPhone3G;
		}
		else if( g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN )
		{
			g_fLoadingExtensionTimeSeconds += g_fExtraLoadTimeSeconds_iPodTouch2ndGen;
		}
	}
		
	
	if( bSetTutorialInactive )
	{
		CComponentInGameTutorialObject::SetInGameTutorialInactive();
	}

	// Set up Boss level stuff
	if( dStricmp( m_pszCurrentLevelName, "Boss_01.t2d" ) == 0 || 
	    dStricmp( m_pszCurrentLevelName, "Boss_02.t2d" ) == 0 ||
	    dStricmp( m_pszCurrentLevelName, "Boss_03.t2d" ) == 0 )
	{
		m_bIsBossLevel = true;
		
		CComponentBossMountain::SetUpBossData();
		
		// Add extra loading for boss levels
		if( IsOldIOSDeviceType() )
		{
			if( dStricmp( m_pszCurrentLevelName, "Boss_03.t2d" ) == 0 )
				g_fLoadingExtensionTimeSeconds += 3.0f;
			else
				g_fLoadingExtensionTimeSeconds += 6.0f;
		}
		else
		{
			g_fLoadingExtensionTimeSeconds += 1.0f;
		}	
	}
    
    // Check if Tutorial Level
    if( dStricmp( m_pszCurrentLevelName, "Level_01_01.t2d" ) == 0 )
    {
        m_bIsTutorialLevel = true;
    }
	
	CComponentScoringManager::InitializeScoreScreenPositions();
	
	m_bLoadingExtensionTimerStarted = true;
	m_bLoadingExtensionTimerFinished = false;
	m_fLoadingExtensionTimer = 0.0f;
	
	// Set the random seed
	U32 uSeed = Platform::getRealMilliseconds();
	MRandomLCG::setGlobalRandSeed( uSeed );
	
	// Hide the loading screen assets
	if( m_pLoadingScreen == NULL )
		m_pLoadingScreen = static_cast<GuiBitmapCtrl*>( Sim::findObject( "LoadingScreen01" ) );
	
	if( m_pLoadingScreenBG == NULL )
		m_pLoadingScreenBG = static_cast<GuiBitmapCtrl*>( Sim::findObject( "LoadingScreenBG" ) );
	
	// Loading Tip Text
	InitializeLoadingTipText();
	
	// Initialize fade variables
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szBlackBox ) );
	if( g_pFadeBox )
	{
		g_pFadeBox->setLayer( LAYER_FADE_BOX );
		g_pFadeBox->setPosition( t2dVector::getZero() );
		m_eFadeState = FADE_STATE_NONE;
		m_fFadeTimer = g_fFadeTimeSeconds;
		m_fFadeAmount = 0.0f;
	}
	
	m_bLevelInitializationComplete = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

U32 CComponentGlobals::GetRandomRange( const S32& _iMin, const S32& _iMax )
{
	return g_GlobalRandom.randI( _iMin, _iMax );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::SetLineDrawAttackComponent( CComponentLineDrawAttack* const _pComponent )
{
	m_pLineDrawAttackComponent = _pComponent;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::OnLineAttackObjectPathFinished()
{
	if( m_pLineDrawAttackComponent == NULL )
		return;
	
	m_pLineDrawAttackComponent->OnLineAttackObjectPathFinished();
	
	CComponentLandBrick* pLandBrickComponent = NULL;
	for( S32 i = 0; i < m_LandBrickCollisionEnableList.size(); ++i )
	{
		pLandBrickComponent = m_LandBrickCollisionEnableList[i];
		if( pLandBrickComponent )
			pLandBrickComponent->EnableCollision();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::FireBrickAttackProjectileAtPosition( const t2dVector& _vStartPosition, const t2dVector& _vEndPosition )
{
	S32 iNumProjectileComponents = m_BrickAttackProjectileComponentList.size();
	
	CComponentEnemyProjectileBrickAttack* pComponent = NULL;
	
	//printf( "FIRING BRICK PROJECTILE AT POSITION: (%f, %f)\n", _vEndPosition.mX, _vEndPosition.mY );
	
	// Get the next available projectile
	for( S32 i = 0; i < iNumProjectileComponents; ++i ) // RKS TODO: Optimize this
	{
		pComponent = m_BrickAttackProjectileComponentList[i];
		if( pComponent && pComponent->IsProjectileInUse() == false )
		{
			pComponent->MakeVisibleAtPosition( _vStartPosition );
			pComponent->FireAtPositionFromSerpent( _vEndPosition );
			return;
		}
	}
	
	//printf( "%s - All brick attack projectiles in use\n", __FUNCTION__ );
	return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::FireBossBrickAttackProjectileAtPosition( const t2dVector& _vStartPosition, const t2dVector& _vEndPosition )
{
	S32 iNumProjectileComponents = m_BrickAttackProjectileComponentList.size();
	
	CComponentEnemyProjectileBrickAttack* pComponent = NULL;
	
	// Get the next available projectile
	for( S32 i = 0; i < iNumProjectileComponents; ++i ) // RKS TODO: Optimize this
	{
		pComponent = m_BrickAttackProjectileComponentList[i];
		if( pComponent && pComponent->IsProjectileInUse() == false )
		{
			pComponent->MakeVisibleAtPosition( _vStartPosition );
			pComponent->FireAtPositionFromBoss( _vEndPosition );
			return;
		}
	}
	
	//printf( "%s - All brick attack projectiles in use\n", __FUNCTION__ );
	return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::OnGrassBrickFullyGrown( const t2dVector& _vGrassBrickPosition )
{
	S32 iNumFullyGrownLandBricks = CComponentPathGridHandler::GetInstance().GetNumFullyGrownLandBricks();
	
	if( iNumFullyGrownLandBricks >= m_iTotalNumBreakableLandBricks )
	{
		// Start the win game sequence
		CComponentEndLevelScreen::GetInstance().StartLevelWinSequence();
		
		StopTimer();
	}
	else 
	{
        bool bSpawnSeed = true;
        if( CComponentTutorialLevel::IsTutorialLevel() )
        {
            if( CComponentTutorialLevel::IsSeedSpawningAllowed() == false )
                bSpawnSeed = false;
        }
        
        if( bSpawnSeed == true )
        {
            // When a tile is fully grown, there is a chance that a seed will spawn there
            U32 uRandom = g_GlobalRandom.randI( 0, 1 );
            
            if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsSeedSpawningAllowed() )
                uRandom = 1;
            
            if( uRandom == 1 )
            {
                // Spawn a seed at the location of the grass brick
                CComponentManaSeedItemDrop::SpawnSeedFromGrassAtPosition( _vGrassBrickPosition );
            } 
        }
	}

	CComponentScoringManager::GetInstance().IncrementNumGrassTilesGrown();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::ScheduleLoadLevel( const char* _pszLevelName )
{		   
    printf( "_pszLevelName: %s\n", _pszLevelName );
    
	if( _pszLevelName == NULL )
	{
		printf( "%s - Invalid level name\n", __FUNCTION__ );
		return;
	}
	
	g_bDebugLoadingLevel = true;
	
	m_bLevelInitializationComplete = false;
	
	if( m_pLoadingScreenBG )
		m_pLoadingScreenBG->setVisible( true );
	
	if( m_pLoadingScreen )
		m_pLoadingScreen->setVisible( true );
	
	if( g_pLoadingScreenText )
		g_pLoadingScreenText->setVisible( true );
	
	if( g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G &&
	    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN &&
	    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN )
	{
		ActivityIndicatorWrapper::ShowIndicator();
	}
	
	ShowLoadingTipText( _pszLevelName );
	
	SaveGameSettings();
	
	m_iNumBlobsKilledThisLevel = 0;
	m_iNumSerpentsKilledThisLevel = 0;
	m_iNumSmokesKilledThisLevel = 0;
	m_iNumSeedsCollectedThisLevel = 0;
	m_iNumGrassGrownThisLevel = 0;
	
	CSoundManager::GetInstance().StopCurrentBGM();
	
	//strcpy( m_szLoadLevelName, _pszLevelName );
	printf( "Loading %s\n", _pszLevelName );
	//Con::executef( 3, "ScheduleLoadLevel", _pszLevelName, "128" );
	Con::executef( 3, "ScheduleLoadLevel", _pszLevelName, "960" ); // 30 frames * 32 milliseconds per frame
	
	CComponentLandBrick::ClearAllLandBrickData();
	CComponentDoubleBrick::ClearDoubleBrickList();
	
	//CComponentInGameTutorialObject::SetInGameTutorialInactive();
	
	CComponentBossMountain::ClearBossData();
	
	// Clear Spawning Data every time a level loads
	CComponentStaticPath::ClearStaticPathList();
	CComponentEnemySpawnPosition::ClearSpawnPositionList();
	CComponentEnemySmokePositionList::ClearSmokePositionLists();
	
	m_bIsBossLevel = false;
	m_bIsGameplayLevel = false;
    m_bIsTutorialLevel = false;
	
	g_pGameTimer_Text = NULL;
	StopTimer();
	
	CComponentAchievementBanner::OnLoadLevelScheduled();
	CComponentInGameTutorialObject::ResetInGameTutorialData();
	
	
	// Reset some level specific data
	m_bLevelComplete = false;
	m_bPlayerTouchingEnemy = false;
	m_pBrickAttackProjectileObject = NULL;
	m_iTotalNumBreakableLandBricks = 0;
	m_LandBrickCollisionEnableList.clear();
	m_bLevelUpScreenIsValid = false;
	m_pLineDrawAttackComponent = NULL;
	
	m_bLoadingFinished = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::ScheduleLoadLevelNoSave( const char* _pszLevelName )
{
    printf( "_pszLevelName: %s\n", _pszLevelName );
    
	if( _pszLevelName == NULL )
	{
		printf( "%s - Invalid level name\n", __FUNCTION__ );
		return;
	}
	
	g_bDebugLoadingLevel = true;
	
	m_bLevelInitializationComplete = false;
	
	if( m_pLoadingScreenBG )
		m_pLoadingScreenBG->setVisible( true );
	
	if( m_pLoadingScreen )
		m_pLoadingScreen->setVisible( true );
	
	ShowLoadingTipText( _pszLevelName );
	
	if( g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G &&
	    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN &&
	    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN )
	{
		ActivityIndicatorWrapper::ShowIndicator();
	}
	
	m_iNumBlobsKilledThisLevel = 0;
	m_iNumSerpentsKilledThisLevel = 0;
	m_iNumSmokesKilledThisLevel = 0;
	m_iNumSeedsCollectedThisLevel = 0;
	m_iNumGrassGrownThisLevel = 0;
	
	//strcpy( m_szLoadLevelName, _pszLevelName );
	printf( "Loading %s\n", _pszLevelName );
	Con::executef( 3, "ScheduleLoadLevel", _pszLevelName, "960" );
	
	CComponentLandBrick::ClearAllLandBrickData();
	CComponentDoubleBrick::ClearDoubleBrickList();
	
	CComponentInGameTutorialObject::SetInGameTutorialInactive();
	
	CComponentBossMountain::ClearBossData();
	
	g_pGameTimer_Text = NULL;
	StopTimer();
	
	m_bIsBossLevel = false;
	m_bIsGameplayLevel = false;
    m_bIsTutorialLevel = false;
	
	CComponentAchievementBanner::OnLoadLevelScheduled();
	
	m_bLoadingFinished = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::AddLandBrickToCollisionEnableList( CComponentLandBrick* _pLandBrickComponent )
{
	if( _pLandBrickComponent == NULL )
		return;
	
	m_LandBrickCollisionEnableList.push_back( _pLandBrickComponent );
	_pLandBrickComponent->SetIsInCollisionEnableList();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::RemoveLandBrickFromCollisionEnableList( CComponentLandBrick* _pLandBrickComponent )
{
	if( _pLandBrickComponent == NULL )
		return;
	
	for( S32 i = 0; i < m_LandBrickCollisionEnableList.size(); ++i )
	{
		if( _pLandBrickComponent == m_LandBrickCollisionEnableList[ i ] )
		   m_LandBrickCollisionEnableList.erase_fast( i );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::SubtractFromTotalXP( const S32& _iXP )
{ 
	if( m_iTotalXP > 0 ) 
		m_iTotalXP -= _iXP;
	
	if( m_iTotalXP < 0 ) 
		m_iTotalXP = 0;
	
	AddToTotalXPSpent( _iXP );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementXPLevelGreen()
{
	if( g_SaveData[ SAVE_DATA_INDEX_XP_LEVEL_GREEN ] >= g_iMaxXPLevel )
	{
		//printf( "Warning: Attempting to increment Green level beyond limit.\n" );
		return;
	}
	
	g_SaveData[ SAVE_DATA_INDEX_XP_LEVEL_GREEN ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementXPLevelBlue()
{
	if( g_SaveData[ SAVE_DATA_INDEX_XP_LEVEL_BLUE ] >= g_iMaxXPLevel )
	{
		//printf( "Warning: Attempting to increment Blue level beyond limit.\n" );
		return;
	}
	
	g_SaveData[ SAVE_DATA_INDEX_XP_LEVEL_BLUE ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementXPLevelPurple()
{
	if( g_SaveData[ SAVE_DATA_INDEX_XP_LEVEL_PURPLE ] >= g_iMaxXPLevel )
	{
		//printf( "Warning: Attempting to increment Purple level beyond limit.\n" );
		return;
	}
	
	g_SaveData[ SAVE_DATA_INDEX_XP_LEVEL_PURPLE ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetXPLevelGreen() const
{
	return ( (g_SaveData[SAVE_DATA_INDEX_XP_LEVEL_GREEN] > g_iMaxXPLevel) ? g_iMaxXPLevel : g_SaveData[SAVE_DATA_INDEX_XP_LEVEL_GREEN] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetXPLevelBlue() const
{
	return ( (g_SaveData[SAVE_DATA_INDEX_XP_LEVEL_BLUE] > g_iMaxXPLevel) ? g_iMaxXPLevel : g_SaveData[SAVE_DATA_INDEX_XP_LEVEL_BLUE] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetXPLevelPurple() const
{
	return ( (g_SaveData[SAVE_DATA_INDEX_XP_LEVEL_PURPLE] > g_iMaxXPLevel) ? g_iMaxXPLevel : g_SaveData[SAVE_DATA_INDEX_XP_LEVEL_PURPLE] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetXPLevelMax() const
{
	return g_iMaxXPLevel;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::AddToTotalXPSpent( const S32& _iXPSpent )
{
	g_SaveData[ SAVE_DATA_INDEX_XP_SPENT_TOTAL ] += _iXPSpent;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetTotalXPEarned()
{
	return (g_SaveData[SAVE_DATA_INDEX_XP_SPENT_TOTAL] + m_iTotalXP);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::DoesSaveDataExist() const
{
	//return g_FileObject.readMemory( g_szSaveDataFileName );
	return g_SaveData[ SAVE_DATA_INDEX_DATA_SAVED ] != 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::LoadGameSettings()
{
	//printf( "Loading Game Save data...\n" );
	
	//const char* pszCWD = Platform::getCurrentDirectory();
	//printf( "LoadGameSettings - current working directory: %s\n", pszCWD );
	
	// This call to 'openForWrite' is a workaround for the iTGB 1.4 issue where files are no longer being read (http://www.torquepowered.com/community/forums/viewthread/117719)
	if( g_FileObject.openForWrite( g_szSaveDataFileName, true ) )
		g_FileObject.close();
	
	if( g_FileObject.readMemory( g_szSaveDataFileName ) )
	{
		//printf( " - Reading %s\n - ", g_szSaveDataFileName );
		
		S32 iNumLinesRead = 0;
		const char* szLineRead;
		
		while( g_FileObject.isEOF() == false )
		{
			szLineRead = reinterpret_cast<const char*>( g_FileObject.readLine() );
			//printf( "%s; ", szLineRead );
			
			g_SaveData[iNumLinesRead] = dAtoi( szLineRead ); // Convert the string to an integer
			
			++iNumLinesRead;
		}
		
		//printf( "\n" );
		g_FileObject.close();
		
		m_iTotalXP			= g_SaveData[SAVE_DATA_INDEX_TOTAL_XP]; /// DO NOT CHECK THIS LINE IN COMMENTED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		m_iTotalHealthXP	= g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_HEALTH];
		m_iTotalManaXP		= g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_MANA];
		m_iTotalSpeedXP		= g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_SEED_SPEED];
		//g_SaveData[SAVE_DATA_INDEX_LAST_LEVEL_COMPLETE];
		//g_SaveData[SAVE_DATA_INDEX_LAST_LEVEL_COMPLETE];
		//g_SaveData[SAVE_DATA_INDEX_MAP_CONDITIONS];
		//g_SaveData[SAVE_DATA_INDEX_RPG_CONDITIONS];
		//g_SaveData[SAVE_DATA_INDEX_RPG_NUM_CURRENT_VINELINGS];
		
		return true;
	}
	
	g_FileObject.close();
	printf( " - Unable to read %s or file doesn't exist\n", g_szSaveDataFileName );
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::SaveGameSettings()
{
	//const char* pszCWD = Platform::getCurrentDirectory();
	//printf( "SaveGameSettings - current working directory: %s\n", pszCWD );
	
	g_SaveData[ SAVE_DATA_INDEX_DATA_SAVED ]				= 1;
	g_SaveData[SAVE_DATA_INDEX_TOTAL_XP]					= m_iTotalXP;
	g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_HEALTH]		= m_iTotalHealthXP;
	g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_MANA]		= m_iTotalManaXP;
	g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_SEED_SPEED] = m_iTotalSpeedXP;
	
	// Add the current level totals to the save data
	g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_BLOB] += m_iNumBlobsKilledThisLevel;
	g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SERPENT] += m_iNumSerpentsKilledThisLevel;
	g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SMOKE] += m_iNumSmokesKilledThisLevel;
	g_SaveData[SAVE_DATA_INDEX_SEEDS_COLLECTED] += m_iNumSeedsCollectedThisLevel;
	g_SaveData[SAVE_DATA_INDEX_GRASS_GROWN] += m_iNumGrassGrownThisLevel;
	
	if( g_FileObject.openForWrite( g_szSaveDataFileName, false ) )
	{
		//printf( " - Writing %s\n - ", g_szSaveDataFileName );
		
		char szLineData[32];
		
		for( S32 i = 0; i < NUM_SAVE_DATA_VALUES; ++i )
		{
			sprintf( szLineData, "%d", g_SaveData[i] );
			g_FileObject.writeLine( reinterpret_cast<U8*>( szLineData ) );
			//printf( "%s; ", szLineData );
		}
		
		//printf( "\n" );
		g_FileObject.close();
		return true;
	}
	
	g_FileObject.close();
	printf( " - Unable to open %s\n", g_szSaveDataFileName );
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::ClearSaveGameSettings()
{
	//printf( "Clearing Game Save data...\n" );
	
	g_SaveData[ SAVE_DATA_INDEX_DATA_SAVED ] = 0;
	
	m_iTotalXP			= 0; // g_SaveData[SAVE_DATA_INDEX_TOTAL_XP];
	m_iTotalHealthXP	= 0; // g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_HEALTH];
	m_iTotalManaXP		= 0; // g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_MANA];
	m_iTotalSpeedXP		= 0; // g_SaveData[SAVE_DATA_INDEX_APPLIED_XP_UNITS_SEED_SPEED];
	
	if( g_FileObject.openForWrite( g_szSaveDataFileName, false ) )
	{
		//printf( " - Clearing %s\n - ", g_szSaveDataFileName );
		
		char szLineData[32];
		
		for( S32 i = 0; i < NUM_SAVE_DATA_VALUES; ++i )
		{
			if( i == SAVE_DATA_INDEX_XP_SPENT_TOTAL ) // Never clear this value
				continue;
			
			g_SaveData[i] = 0;
			sprintf( szLineData, "%d", g_SaveData[i] );
			g_FileObject.writeLine( reinterpret_cast<U8*>( szLineData ) );
			//printf( "%s; ", szLineData );
		}
		
		//printf( "\n" );
		g_FileObject.close();
		return;
	}
	
	g_FileObject.close();
	
	g_bAllLevelsUnlocked = false;
	
	printf( " - Unable to open %s\n", g_szSaveDataFileName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::IsMapUnlocked( S32 _iLevelSaveIndex ) const // This function is probably not needed
{
	if( _iLevelSaveIndex >= NUM_SAVE_DATA_VALUES )
		return false;
	
	return static_cast<bool>( g_SaveData[_iLevelSaveIndex] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::IsLevelUnlocked( S32 _iLevelSaveIndex ) const
{
	if( _iLevelSaveIndex >= NUM_SAVE_DATA_VALUES )
		return false;
	
	return static_cast<bool>( g_SaveData[_iLevelSaveIndex] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::IsLevelComplete( S32 _iLevelSaveIndex ) const
{
	if( _iLevelSaveIndex >= NUM_SAVE_DATA_VALUES )
		return false;
	
	return static_cast<bool>( g_SaveData[_iLevelSaveIndex] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::MarkActiveLevelAsComplete( S32 _iStarRanking ) // Star Ranking should only be 1, 2, or 3
{
	if( m_iActiveLevelSaveIndex >= NUM_SAVE_DATA_VALUES || m_iActiveLevelSaveIndex < 0 )
		return;
	
	if( _iStarRanking <= 0 || _iStarRanking > 3 )
	{
		printf( "MarkActiveLevelAsComplete - Invalid Star Ranking value: %d\n", _iStarRanking );
		g_SaveData[ m_iActiveLevelSaveIndex ] = 1;
		return;
	}
	
	// If this star ranking is lower than a previous star ranking for the level, do not save it.
	if( g_SaveData[ m_iActiveLevelSaveIndex ] > _iStarRanking )
		return;
	
	g_SaveData[ m_iActiveLevelSaveIndex ] = _iStarRanking;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::MarkTutorialShown( S32 _iTutorialSaveIndex )
{
	if( _iTutorialSaveIndex < SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP || _iTutorialSaveIndex > SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER03 )
	{
		if( _iTutorialSaveIndex != SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP_DLC_INTRO )
			return;
	}
	
	g_SaveData[ _iTutorialSaveIndex ] = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::HasTutorialBeenShown( S32 _iTutorialSaveIndex )
{
	if( _iTutorialSaveIndex < SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP || _iTutorialSaveIndex > SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER03 )
	{
		if( _iTutorialSaveIndex != SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP_DLC_INTRO )
			return false;
	}
	
	return static_cast<bool>( g_SaveData[ _iTutorialSaveIndex ] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::InitializeInGameTutorials()
{	
	if( dStricmp( m_pszCurrentLevelName, "Screen_MapSelect_PowerUp.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_MAP_RPG );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_03_01.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_SERPENT );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_03_02.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_CRYSTAL );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_04_01.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_LAVAPIT );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_04_02.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_SMOKE );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Boss_01.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_BOSS );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_05_03.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_SUPER_BLOB );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_06_02.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_SUPER_SERPENT );
	}
	else if( dStricmp( m_pszCurrentLevelName, "Level_07_01.t2d" ) == 0 )
	{
		CComponentInGameTutorialObject::SetTutorialType( CComponentInGameTutorialObject::TUTORIAL_SUPER_SMOKE );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetLevelStarRanking( const S32 _iLevelSaveIndex ) const
{
	if( _iLevelSaveIndex >= NUM_SAVE_DATA_VALUES )
		return 0;
	
	return g_SaveData[ _iLevelSaveIndex ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::HandleGlobalMouseInput( const EMouseEvent _eMouseEvent, const t2dVector& _vWorldMousePosition )
{
	// This function is so I can handle input AFTER the objects have handled it
	// RKS TODO: Handle the line draw attack and whatever else may need to use global mouse input
	
	if( m_pLineDrawAttackComponent )
	{
		if( _eMouseEvent == MOUSE_EVENT_DOWN )
		{
			m_pLineDrawAttackComponent->OnMouseDown( _vWorldMousePosition );
		}
		else if( _eMouseEvent == MOUSE_EVENT_UP )
		{
			m_pLineDrawAttackComponent->OnMouseUp( _vWorldMousePosition );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::processTick()
{
	if( m_bLoadingExtensionTimerStarted )
	{
		if( m_bLoadingExtensionTimerFinished == false )
		{
			m_fLoadingExtensionTimer += ITickable::smTickSec;
			if( m_fLoadingExtensionTimer >= g_fLoadingExtensionTimeSeconds )
			{
				if( g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G &&
				    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN &&
				    g_eIOSDeviceType != DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN )
				{
					ActivityIndicatorWrapper::HideIndicator();
				}
				
				if( m_pLoadingScreen )
					m_pLoadingScreen->setVisible( false );
				
				if( m_pLoadingScreenBG )
					m_pLoadingScreenBG->setVisible( false );
				
				if( g_bDebugAlwaysShowTipText == false )
					HideLoadingTipText();
				
				m_bLoadingExtensionTimerFinished = true;
				m_bLoadingExtensionTimerStarted = false;
				
				
				// This shit was moved from OnPostInit() because this is the stuff that is dependent on the post init finishing
				if( CComponentMainMenuScreen::IsInMainMenuScreen() )
				{
					CComponentMainMenuScreen::GetInstance().StartMainMenuScreen();
					
					// In the Main Menu, try to log in to Game Center only once. After that, they need to press the Game Center button (which opens Achievements)
					if( m_bGameCenterLoginAttempted == false )
					{
						//printf( "Authenticating Game Center...\n" );
						GameCenterWrapper::Authenticate();
						
						m_bGameCenterLoginAttempted = true;
					}
				}
				
				//if( CComponentMapSelectMenu::IsInMapSelectMenu() )
				//	CSoundManager::GetInstance().PlayBGMMainMenuRPG();	
				
				if( CComponentMapSelectMenu::IsInMapSelectMenu() )
					CSoundManager::GetInstance().PlayBGMMapScreen();
				
				//if( CComponentLevelBeginSequence::DoesLevelHaveBeginSequence() )
				//	CComponentLevelBeginSequence::GetInstance().StartLevelBeginSequence();
				
				if( CComponentStoryOpeningScreen::DoesLevelHaveStoryOpening() )
					CComponentStoryOpeningScreen::StartStoryOpeningScreen();
				
				if( CComponentTutorialOpeningScreen::IsLevelTutorialOpeningScreen() )
					CComponentTutorialOpeningScreen::PlayOpeningTutorialMusic();
				
				// If this is a standard gameplay level, then the level has not finished "Loading" until the fade finishes
				if( strstr( m_pszCurrentLevelName, "Level_" ) != NULL || strstr( m_pszCurrentLevelName, "Boss_" ) != NULL )
				{
					m_bLoadingFinished = false;
					m_eFadeState = FADE_STATE_FADING_IN;
				}
				else
				{
					m_eFadeState = FADE_STATE_NONE;
					m_bLoadingFinished = true;
				}
			}
		}
		
		return;
	}
	
	if( m_bLoadingExtensionTimerFinished == false )
		return;
	
	if( m_bTimerStopped == false )
	{
		m_uLevelTimerMS += ITickable::smTickMs;

		if( g_bUseGameTimerDisplay )
		{
			if( g_pGameTimer_Text )
			{
				F32 fLevelTimerSeconds = (F32)m_uLevelTimerMS * 0.001;
				char szBuffer[32];
				sprintf( szBuffer, "%0.2f", fLevelTimerSeconds );
				g_pGameTimer_Text->setText( szBuffer );
			}
		}
	}
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
	
	if( m_bLevelInitializationComplete )
	{
		UpdateHitEffects();
		
		if( m_bIsGameplayLevel )
			UpdateDeadTimeSeedSpawn();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::UpdateDeadTimeSeedSpawn()
{
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
		return;
    
    if( CComponentTutorialLevel::IsTutorialLevel() )
    {
        if( CComponentTutorialLevel::IsSeedSpawningAllowed() == false )
            return;
    }
	
	 // DM: To address dead time (where player doesn't have anything to do), we provide him/her with
	 //		free seeds dispensed only if there are no seeds left on the board and the player is out of mana (i.e. can't draw)
	
	if( g_bSpawningDeadTimeSeeds )
	{
		g_fDeadTimeSeedSpawnTimer += ITickable::smTickSec;
		
		// Every X seconds, spawn a seed (up to 3) in a grass tile
		if( g_fDeadTimeSeedSpawnTimer >= g_fDeadTimeSeedSpawnDelaySeconds )
		{
			g_fDeadTimeSeedSpawnTimer = 0.0f;
			
			--g_iNumberOfDeadTimePositions;
			t2dVector vCurrentPosition = g_aSeedPositions[ g_iNumberOfDeadTimePositions ];
			CComponentManaSeedItemDrop::SpawnSeedFromGrassAtPosition( vCurrentPosition );
		}
		
		// Reset all the variables once all dead time seeds have been spawned.
		if( g_iNumberOfDeadTimePositions <= 0 )
		{
			g_bSpawningDeadTimeSeeds = false;
			g_iNumberOfDeadTimePositions = 0;
			g_fDeadTimeSeedSpawnTimer = 0.0f;
			g_aSeedPositions.clear();
		}
	}
	else
	{
		if( CComponentManaBar::IsThereAnyAvailableMana() == false && CComponentManaSeedItemDrop::AreThereAnySeedsActive() == false )
		{
			g_aSeedPositions.clear();
			
			// Get the number of available grass tile for spawning seeds
			g_iNumberOfDeadTimePositions = CComponentPathGridHandler::GetInstance().GetArrayRandomAttackableLandBrickPosition( g_aSeedPositions, g_iNumberOfDeadTimeSeeds );
			if( g_iNumberOfDeadTimePositions > 0 )
			{
				g_bSpawningDeadTimeSeeds = true;
				g_fDeadTimeSeedSpawnTimer = 0.0f;
			}
		}
	}

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::UpdateFade()
{
	if( g_pFadeBox == NULL )
		return;
	
	switch( m_eFadeState )
	{
		case FADE_STATE_FADING_IN: // Fading In (Turning to transparent)
			
			m_fFadeTimer -= ITickable::smTickSec;
			if( m_fFadeTimer <= 0.0f )
				m_fFadeTimer = 0.0f;
			
			m_fFadeAmount = m_fFadeTimer / g_fFadeTimeSeconds;
			g_pFadeBox->setBlendAlpha( m_fFadeAmount );
			
			
			if( m_fFadeAmount <= 0.0f )
				OnFadeInFinished();
			
			break;
			
		case FADE_STATE_FADING_OUT: // Fading Out (Turning to black)
			
			m_fFadeTimer += ITickable::smTickSec;
			if( m_fFadeTimer >= g_fFadeTimeSeconds )
				m_fFadeTimer = g_fFadeTimeSeconds;
			
			m_fFadeAmount = m_fFadeTimer / g_fFadeTimeSeconds;
			g_pFadeBox->setBlendAlpha( m_fFadeAmount );
			
			if( m_fFadeAmount >= 1.0f )
				OnFadeOutFinished();
			
			break;
			
		default:
			break;	
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::OnFadeOutFinished()
{
	m_eFadeState = FADE_STATE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::OnFadeInFinished()
{
	if( CComponentLevelBeginSequence::DoesLevelHaveBeginSequence() )
		CComponentLevelBeginSequence::GetInstance().StartLevelBeginSequence();
	
	m_bLoadingFinished = true;
	m_eFadeState = FADE_STATE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::StartCameraShake( F32 _fMagnitude, F32 _fTime )
{
	if( g_pSceneWindow == NULL )
		return;
	
	g_pSceneWindow->startCameraShake( _fMagnitude, _fTime );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::RecordCurrentLevelTime()
{
	printf( "Current level index %d has time: %u milliseconds\n", m_iActiveLevelSaveIndex, m_uLevelTimerMS );
	
	switch( m_iActiveLevelSaveIndex )
	{
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_01_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_02_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_03_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_03_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_04_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_04_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_05_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_05_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_05_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_06_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_06_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_06_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_07_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_07_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_07_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_07_04 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_08_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_08_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_08_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_08_04 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_09_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_09_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_09_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_09_04 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_10_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_10_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_10_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_10_04 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01:	g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02:	g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03:	g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_01 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_02 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_03 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_04 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_05:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_05 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_06:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_06 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_07:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_07 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_08:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_08 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_09:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_09 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_10:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_10 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_11:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_11 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_12:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_12 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_13:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_13 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_14:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_14 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_15:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_15 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_16:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_16 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_17:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_17 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_18:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_18 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_19:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_19 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_20:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_20 ] = m_uLevelTimerMS; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_21:		g_SaveData[ SAVE_DATA_INDEX_LEVEL_TIME_11_21 ] = m_uLevelTimerMS; break;
			
		default: 
			printf( "RecordCurrentLevelTime: Unhandled level index: %d\n", m_iActiveLevelSaveIndex );
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::UploadScoresToGameCenter()
{
	printf( "CComponentGlobals::UploadScoresToGameCenter\n" );
	
	//void GameCenterWrapper::ReportScore( int _iLeaderboardID, int _iScore )
	
	////////////
	// VALUES //
	////////////
	
	// Grass (Life) Grown
	//GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TOP_LIFE, g_SaveData[ SAVE_DATA_INDEX_GRASS_GROWN ] );
	GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TOP_LIFE, GetTotalXPEarned() );
	
	// Monsters killed
	S32 iMonstersKilled = g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_BLOB] + g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SERPENT] + g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SMOKE];
	GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TOP_MONSTER, iMonstersKilled );
	
	// Seeds Gathered
	GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TOP_SEED, g_SaveData[SAVE_DATA_INDEX_SEEDS_COLLECTED] );
	
	// Coins Collected (Coins == XP)
	GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TOP_COINS, g_SaveData[SAVE_DATA_INDEX_TOTAL_XP] );
	
	
	///////////
	// TIMES //
	///////////
	
	//S32 iLevelTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_02_01];
	//printf( "Time for World 2 (Milliseconds): %d\n", iLevelTimeMS );	
	//F32 fLevelTimeSeconds = (F32)iLevelTimeMS * 0.001f;
	//printf( "Time for World 2 (Seconds):      %f\n", fLevelTimeSeconds );
	//S32 iReportTimeValue = ( (S32)(fLevelTimeSeconds * 100.0f) );
	//printf( "Report Value: %d\n", iReportTimeValue );
	
	S32 iWorldTimeMS = 0;			// This is the actual (total) time in milliseconds for an entire World
	F32 fWorldTimeSeconds = 0.0f;	// This is the total time in seconds
	S32 iReportTimeValue = 0;		// This is the value that is reported to Game Center. The value must be an integer in this format:  For XXX.YY seconds, send XXXYY. That is XXX.YY * 100
	//GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W2, iReportTimeValue );
	
	// Check Level times - Make sure all levels in a World are complete. Make sure that all levels have a time greater than 0 (so UnlockAllLevels doesn't upload 0 value scores).
	switch( m_iActiveLevelSaveIndex )
	{
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_02_01] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_02_01];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W2, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_03_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_03_02] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_03_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_03_02];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W3, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_04_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_04_02] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_04_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_04_02];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W4, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_05_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_05_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_05_03] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_05_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_05_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_05_03];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W5, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_06_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_06_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_06_03] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_06_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_06_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_06_03];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W6, iReportTimeValue );
				}
			}
			break;
			
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_03] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_04] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_03] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_07_04];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W7, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_03] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_04] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_03] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_08_04];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W8, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_03] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_04] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_03] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_09_04];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W9, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_03] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_04] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_03] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_10_04];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W10, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_B1, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_B2, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_B3, iReportTimeValue );
				}
			}
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03:
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04:
			if( IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02 ) && 
			    IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03 ) && IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04 ) )
			{
				if( g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_01] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_02] > 0 && 
				    g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_03] > 0 && g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_04] > 0 )
				{
					iWorldTimeMS = g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_01] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_02] +
								   g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_03] + g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_11_04];
					fWorldTimeSeconds = (F32)iWorldTimeMS * 0.001f;
					iReportTimeValue = ( (S32)(fWorldTimeSeconds * 100.0f) );
					GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W11_1, iReportTimeValue );
				}
			}
			break;
			
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_05:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_06:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_07:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_08:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_09:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_10:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_11:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_12:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_13:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_14:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_15:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_16:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_17:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_18:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_19:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_20:
		//case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_21:
			
		default: 
			printf( "UploadScoresToGameCenter: Unhandled level index: %d\n", m_iActiveLevelSaveIndex );
			break;
	}
	
	//printf( "Report Time for World 2: %d\n", g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_02_01] );
	//GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W2, g_SaveData[SAVE_DATA_INDEX_LEVEL_TIME_02_01] );
	//GameCenterWrapper::ReportScore( GameCenterWrapper::VK_LB_TIME_W2, iReportTimeValue );
	
	//	VK_LB_TOP_LIFE = 0,
	//	VK_LB_TOP_MONSTER,
	//	VK_LB_TOP_SEED,
	//	VK_LB_TOP_COINS,
	//	VK_LB_TIME_W1,
	//	VK_LB_TIME_W2,
	//	VK_LB_TIME_W3,
	//	VK_LB_TIME_W4,
	//	VK_LB_TIME_W5,
	//	VK_LB_TIME_W6,
	//	VK_LB_TIME_W7,
	//	VK_LB_TIME_W8,
	//	VK_LB_TIME_W9,
	//	VK_LB_TIME_W10,
	//	VK_LB_TIME_W11,
	//	VK_LB_TIME_B1,
	//	VK_LB_TIME_B2,
	//	VK_LB_TIME_B3,				
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::UploadCurrentLevelTimeToGameCenter()
{
	printf( "CComponentGlobals::UploadCurrentLevelTimeToGameCenter\n" );
	
	printf( "TODO:  THIS SHOULD BE THE TOTAL FOR ALL LEVELS IN A WORLD!!!!!\n" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementAchievementTotal( const S32 _iSaveIndex )
{
	if( _iSaveIndex >= NUM_SAVE_DATA_VALUES )
		return;
	
	switch( _iSaveIndex )
	{
		case SAVE_DATA_INDEX_NUM_KILLED_BLOB:
			++m_iNumBlobsKilledThisLevel;
			break;
			
		case SAVE_DATA_INDEX_NUM_KILLED_SERPENT:
			++m_iNumSerpentsKilledThisLevel;
			break;
			
		case SAVE_DATA_INDEX_NUM_KILLED_SMOKE:
			++m_iNumSmokesKilledThisLevel;
			break;
			
		case SAVE_DATA_INDEX_SEEDS_COLLECTED:
			++m_iNumSeedsCollectedThisLevel;
			break;
			
		case SAVE_DATA_INDEX_GRASS_GROWN:
			++m_iNumGrassGrownThisLevel;
			break;
			
		case SAVE_DATA_INDEX_CRYSTALS_BROKEN:
			g_SaveData[SAVE_DATA_INDEX_CRYSTALS_BROKEN]++;
			break;
		
		default:
			break;
			
	}
	
	//g_SaveData[ _iSaveIndex ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetAchievementTotal( const S32 _iSaveIndex )
{
	if( _iSaveIndex >= NUM_SAVE_DATA_VALUES )
		return 0;
	
	switch( _iSaveIndex )
	{
		case SAVE_DATA_INDEX_NUM_KILLED_BLOB:
			return (g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_BLOB] + m_iNumBlobsKilledThisLevel);
			
		case SAVE_DATA_INDEX_NUM_KILLED_SERPENT:
			return (g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SERPENT] + m_iNumSerpentsKilledThisLevel);
			
		case SAVE_DATA_INDEX_NUM_KILLED_SMOKE:
			return (g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SMOKE] + m_iNumSmokesKilledThisLevel);
			
		case SAVE_DATA_INDEX_SEEDS_COLLECTED:
			return (g_SaveData[SAVE_DATA_INDEX_SEEDS_COLLECTED] + m_iNumSeedsCollectedThisLevel);
			
		case SAVE_DATA_INDEX_GRASS_GROWN:
			return (g_SaveData[SAVE_DATA_INDEX_GRASS_GROWN] + m_iNumGrassGrownThisLevel);
			
		case SAVE_DATA_INDEX_CRYSTALS_BROKEN:
			return g_SaveData[SAVE_DATA_INDEX_CRYSTALS_BROKEN];
			
		default:
			break;
			
	}
	
	return 0;
	//return g_SaveData[ _iSaveIndex ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumLevelsWithTwoOrMoreStars() const
{
	S32 iCount = 0;
	for( S32 i = SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01; i <= SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03; ++i )
	{
		if( g_SaveData[i] >= 2 )
			++iCount;
	}
	
	return iCount;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumLevelsWithThreeStars() const
{
	S32 iCount = 0;
	for( S32 i = SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01; i <= SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03; ++i )
	{
		if( g_SaveData[i] == 3 )
			++iCount;
	}
	
	return iCount;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumLevelsBeatenWithFullHealth() const
{
	return g_SaveData[ SAVE_DATA_INDEX_NUM_LEVELS_BEATEN_WITH_MAX_HEALTH ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumLevelsBeatenWithFullPower() const
{
	return g_SaveData[ SAVE_DATA_INDEX_NUM_LEVELS_BEATEN_WITH_MAX_POWER ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementNumLevelsBeatenWithFullHealth() const
{
	g_SaveData[ SAVE_DATA_INDEX_NUM_LEVELS_BEATEN_WITH_MAX_HEALTH ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementNumLevelsBeatenWithFullPower() const
{
	g_SaveData[ SAVE_DATA_INDEX_NUM_LEVELS_BEATEN_WITH_MAX_POWER ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumTimesMuteButtonPressed() const
{
	return g_SaveData[ SAVE_DATA_INDEX_NUM_TIMES_MUTE_BUTTON_PRESSED ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementNumTimesMuteButtonPressed()
{
	g_SaveData[ SAVE_DATA_INDEX_NUM_TIMES_MUTE_BUTTON_PRESSED ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumUpgradesPurchased() const
{
	return g_SaveData[ SAVE_DATA_INDEX_NUM_UPGRADES_PURCHASED ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::IncrementNumUpgradesPurchased()
{
	g_SaveData[ SAVE_DATA_INDEX_NUM_UPGRADES_PURCHASED ]++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::InitializeHitEffects()
{
	// Set up the Hit Effect objects. This object will not exist in non-game levels.
	m_HitEffectList.clear();
	m_iCurrentHitEffectIndex = 0;
	g_pHitEffectObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szHitEffectObject ) );
	if( g_pHitEffectObject )
	{
		g_pHitEffectObject->setLayer( LAYER_HIT_EFFECTS );
		g_pHitEffectObject->setVisible( false );
		
		m_HitEffectList.push_back( g_pHitEffectObject );
		
		char szName[32];
		t2dAnimatedSprite* pNewHitEffect = NULL;
		for( S32 i = 1; i < g_iNumHitEffectObjects; ++i )
		{
			pNewHitEffect = static_cast<t2dAnimatedSprite*>( g_pHitEffectObject->clone() );
			if( pNewHitEffect )
			{
				sprintf( szName, "%s_%d", g_szHitEffectObject, i );
				pNewHitEffect->assignName( szName );
				m_HitEffectList.push_back( pNewHitEffect );
			}
		}
	}
	
	// Set up the Smoke Hit Effect objects. This object will not exist in non-game levels and levels without Smokes.
	m_SmokeHitEffectList.clear();
	m_iCurrentSmokeHitEffectIndex = 0;
	g_pSmokeHitEffectObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szSmokeHitEffectObject ) );
	if( g_pSmokeHitEffectObject )
	{
		g_pSmokeHitEffectObject->setVisible( false );
		g_pSmokeHitEffectObject->setLayer( LAYER_HIT_EFFECTS );
		
		m_SmokeHitEffectList.push_back( g_pSmokeHitEffectObject );
		
		char szName[32];
		t2dAnimatedSprite* pNewSmokeHitEffect = NULL;
		for( S32 i = 1; i < g_iNumSmokeHitEffectObjects; ++i )
		{
			pNewSmokeHitEffect = static_cast<t2dAnimatedSprite*>( g_pSmokeHitEffectObject->clone() );
			if( pNewSmokeHitEffect )
			{
				sprintf( szName, "%s_%d", g_szSmokeHitEffectObject, i );
				pNewSmokeHitEffect->assignName( szName );
				m_SmokeHitEffectList.push_back( pNewSmokeHitEffect );
			}
		}
	}
	
	// Set up the Danger Brick Hit Effect objects. This object will not exist in non-game levels.
	m_DangerBrickHitEffectList.clear();
	m_iCurrentDangerBrickHitEffectIndex = 0;
	g_pDangerBrickHitEffectObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szDangerBrickHitEffectObject ) );
	if( g_pDangerBrickHitEffectObject )
	{
		g_pDangerBrickHitEffectObject->setVisible( false );
		g_pDangerBrickHitEffectObject->setLayer( LAYER_DANGER_BRICK_HIT_EFFECT );
		
		m_DangerBrickHitEffectList.push_back( g_pDangerBrickHitEffectObject );
		
		char szName[32];
		t2dAnimatedSprite* pNewDangerBrickHitEffect = NULL;
		for( S32 i = 1; i < g_iNumDangerBrickHitEffectObjects; ++i )
		{
			pNewDangerBrickHitEffect = static_cast<t2dAnimatedSprite*>( g_pDangerBrickHitEffectObject->clone() );
			if( pNewDangerBrickHitEffect )
			{
				sprintf( szName, "%s_%d", g_szDangerBrickHitEffectObject, i );
				pNewDangerBrickHitEffect->assignName( szName );
				m_DangerBrickHitEffectList.push_back( pNewDangerBrickHitEffect );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::UpdateHitEffects()
{
	if( m_HitEffectList.size() > 0 )
	{
		g_pTempHitEffect = NULL;
		for( S32 i = 0; i < m_HitEffectList.size(); ++i )
		{
			g_pTempHitEffect = m_HitEffectList[i];
			if( g_pTempHitEffect && g_pTempHitEffect->getVisible() && g_pTempHitEffect->mAnimationController.isAnimationFinished() )
			{
				g_pTempHitEffect->setVisible( false );
				g_pTempHitEffect->setPosition( g_vSafeHitEffectPosition );
			}
		}
	}
	
	if( m_SmokeHitEffectList.size() > 0 )
	{
		g_pTempHitEffect = NULL;
		for( S32 i = 0; i < m_SmokeHitEffectList.size(); ++i )
		{
			g_pTempHitEffect = m_SmokeHitEffectList[i];
			if( g_pTempHitEffect && g_pTempHitEffect->getVisible() && g_pTempHitEffect->mAnimationController.isAnimationFinished() )
			{
				g_pTempHitEffect->setVisible( false );
				g_pTempHitEffect->setPosition( g_vSafeSmokeHitEffectPosition );
			}
		}
	}
	
	if( m_DangerBrickHitEffectList.size() > 0 )
	{
		g_pTempHitEffect = NULL;
		for( S32 i = 0; i < m_DangerBrickHitEffectList.size(); ++i )
		{
			g_pTempHitEffect = m_DangerBrickHitEffectList[i];
			if( g_pTempHitEffect && g_pTempHitEffect->getVisible() && g_pTempHitEffect->mAnimationController.isAnimationFinished() )
			{
				g_pTempHitEffect->setVisible( false );
				g_pTempHitEffect->setPosition( g_vSafeDangerBrickHitEffectPosition );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::PlayHitEffectAtPosition( const t2dVector& _vPosition )
{
	if( m_HitEffectList.size() == 0 )
		return;
	
	t2dAnimatedSprite* pHitEffect = m_HitEffectList[ m_iCurrentHitEffectIndex ];
	if( pHitEffect )
	{
		pHitEffect->setVisible( true );
		pHitEffect->setPosition( _vPosition );
		pHitEffect->playAnimation( g_szHitEffectAnimation, false );
		
		m_iCurrentHitEffectIndex++;
		if( m_iCurrentHitEffectIndex >= m_HitEffectList.size() )
			m_iCurrentHitEffectIndex = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::PlaySmokeHitEffectAtPosition( const t2dVector& _vPosition )
{
	if( m_SmokeHitEffectList.size() == 0 )
		return;
	
	t2dAnimatedSprite* pSmokeHitEffect = m_HitEffectList[ m_iCurrentSmokeHitEffectIndex ];
	if( pSmokeHitEffect )
	{
		pSmokeHitEffect->setVisible( true );
		pSmokeHitEffect->setPosition( _vPosition );
		pSmokeHitEffect->playAnimation( g_szSmokeHitEffectAnimation, false );
		
		m_iCurrentSmokeHitEffectIndex++;
		if( m_iCurrentSmokeHitEffectIndex >= m_SmokeHitEffectList.size() )
			m_iCurrentSmokeHitEffectIndex = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::PlayDangerBrickHitEffectAtPosition( const t2dVector& _vPosition )
{
	if( m_DangerBrickHitEffectList.size() == 0 )
		return;
	
	t2dAnimatedSprite* pDangerBrickHitEffect = m_DangerBrickHitEffectList[ m_iCurrentDangerBrickHitEffectIndex ];
	if( pDangerBrickHitEffect )
	{
		pDangerBrickHitEffect->setVisible( true );
		pDangerBrickHitEffect->setPosition( _vPosition );
		pDangerBrickHitEffect->playAnimation( g_szDangerBrickHitEffectAnimation, false );
		
		m_iCurrentDangerBrickHitEffectIndex++;
		if( m_iCurrentDangerBrickHitEffectIndex >= m_DangerBrickHitEffectList.size() )
			m_iCurrentDangerBrickHitEffectIndex = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::SetIOSDeviceType()
{
	g_eIOSDeviceType = DeviceTypeWrapper::GetIOSDeviceType();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::IsOldIOSDeviceType()
{
	if( g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPHONE_ORIGINAL ||
	    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G ||
	    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN ||
	    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_2ND_GEN )
	{
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentGlobals::CanIOSDeviceSupportGameCenter()
{
	// According to Apple, Game Center is only available on the following devices and only on iOS 4.1 and up:
	// http://www.apple.com/game-center/
	// - iPod touch 4th generation
	// - iPod touch 3rd generation
	// - iPod touch 2nd generation
	// - iPhone 4
	// - iPhone 3GS
	// - iPad
	
	if( g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPHONE_ORIGINAL ||
	    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPHONE_3G ||
	    g_eIOSDeviceType == DeviceTypeWrapper::DEVICE_TYPE_IPOD_TOUCH_1ST_GEN )
	{
        //printf( "CanIOSDeviceSupportGameCenter? NO!\n" );
		return false;
	}
	
    //printf( "CanIOSDeviceSupportGameCenter? YES!\n" );
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::InitializeLoadingTipText()
{
	g_pLoadingTipTextLine01 = static_cast<GuiTextCtrl*>( Sim::findObject( g_szLoadingTipTextLine01 ) );
	g_pLoadingTipTextLine02 = static_cast<GuiMLTextCtrl*>( Sim::findObject( g_szLoadingTipTextLine02 ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::ShowLoadingTipText( const char* _pszLevelName )
{
	// Don't show Tip Text if loading into some levels
	if( strcmp( _pszLevelName, "game/data/levels/Screen_MainMenu.t2d"         ) == 0 || 
	    strcmp( _pszLevelName, "game/data/levels/Screen_PixelVandalsLogo.t2d" ) == 0 || 
		strcmp( _pszLevelName, "game/data/levels/Screen_StoryOpening.t2d"     ) == 0 || 
	    strcmp( _pszLevelName, "game/data/levels/Screen_StoryEnding.t2d"      ) == 0 || 
	    strcmp( _pszLevelName, "game/data/levels/Screen_TutorialOpening.t2d"  ) == 0 || 
	    strcmp( _pszLevelName, "game/data/levels/Level_01_01.t2d"             ) == 0 )
	{
		return;
	}
	
	if( g_pLoadingTipTextLine01 )
	{
		g_pLoadingTipTextLine01->setText( g_szTextStringTip );
		g_pLoadingTipTextLine01->setVisible( true );
	}
	
	if( g_pLoadingTipTextLine02 )
	{
		// Get a random Tip
		//S32 iRandom = GetRandomRange(0 , g_iNumTipTextStrings - 1 );
		//g_pLoadingTipTextLine02->setText( g_szTipTextStrings[ iRandom ], strlen( g_szTipTextStrings[ iRandom ] ) );
		
		// Display the Tips in order
		g_pLoadingTipTextLine02->setText( g_szTipTextStrings[ g_SaveData[SAVE_DATA_INDEX_CURRENT_TIP_TEXT_INDEX] ],
										  strlen( g_szTipTextStrings[ g_SaveData[SAVE_DATA_INDEX_CURRENT_TIP_TEXT_INDEX] ] ) );
										 
		g_SaveData[SAVE_DATA_INDEX_CURRENT_TIP_TEXT_INDEX]++;
		if( g_SaveData[SAVE_DATA_INDEX_CURRENT_TIP_TEXT_INDEX] >= g_iNumTipTextStrings )
			g_SaveData[SAVE_DATA_INDEX_CURRENT_TIP_TEXT_INDEX] = 0;
										 
		g_pLoadingTipTextLine02->setVisible( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::HideLoadingTipText()
{
	if( g_pLoadingTipTextLine01 )
		g_pLoadingTipTextLine01->setVisible( false );
	
	if( g_pLoadingTipTextLine02 )
		g_pLoadingTipTextLine02->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ const S32 CComponentGlobals::GetAchievementGoalKilledBlob01()		{ return g_iAchievementGoalKilledBlob01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalKilledBlob02()		{ return g_iAchievementGoalKilledBlob02; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalKilledSerpent01()		{ return g_iAchievementGoalKilledSerpent01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalKilledSerpent02()		{ return g_iAchievementGoalKilledSerpent02; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalKilledSmoke01()		{ return g_iAchievementGoalKilledSmoke01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalKilledSmoke02()		{ return g_iAchievementGoalKilledSmoke02; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalSeedsCollected01()	{ return g_iAchievementGoalSeedsCollected01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalSeedsCollected02()	{ return g_iAchievementGoalSeedsCollected02; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalSeedsCollected03()	{ return g_iAchievementGoalSeedsCollected03; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalCrystalsBroken01()	{ return g_iAchievementGoalCrystalsBroken01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalCrystalsBroken02()	{ return g_iAchievementGoalCrystalsBroken02; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalCrystalsBroken03()	{ return g_iAchievementGoalCrystalsBroken03; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalGrassGrown01()		{ return g_iAchievementGoalGrassGrown01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalGrassGrown02()		{ return g_iAchievementGoalGrassGrown02; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalGrassGrown03()		{ return g_iAchievementGoalGrassGrown03; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalGrassGrown04()		{ return g_iAchievementGoalGrassGrown04; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalFullHealth01()		{ return g_iAchievementGoalFullHealth01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalFullHealth02()		{ return g_iAchievementGoalFullHealth02; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalFullHealth03()		{ return g_iAchievementGoalFullHealth03; }

/*static*/ const S32 CComponentGlobals::GetAchievementGoalFullMana01()			{ return g_iAchievementGoalFullMana01; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalFullMana02()			{ return g_iAchievementGoalFullMana02; }
/*static*/ const S32 CComponentGlobals::GetAchievementGoalFullMana03()			{ return g_iAchievementGoalFullMana03; }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentGlobals::OnDeviceResignActive()
{
	printf( "CComponentGlobals::OnDeviceResignActive - \n" );
	if( GetInstance().IsGameplayLevel() == false )
	{
		printf( " -> Not a gameplay level.\n" );
		return;
	}
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
	{
		printf( " -> Gameplay level has ended.\n" );
		return;
	}
    
    if( CComponentTutorialLevel::IsTutorialLevel() )
    {
        printf( " -> Tutorial Level is active.\n" );
        return;
    }
	
	if( CComponentInGameTutorialObject::IsTutorialActive() )
	{
		printf( " -> In-Game Tutorial is active.\n" );
		return;
	}
	
	if( CComponentTutorialLevel::IsTutorialActive() )
	{
		printf( " -> Tutorial Level message is active.\n" );
		return;
	}
    
    
	
	if( CComponentPauseScreen::DoesLevelHavePauseScreen() )
	{
		printf( " -> Forcing pause button press.\n" );
		CComponentPauseScreen::GetInstance().OnPauseButtonPressed();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumGrassGrown()
{
	return g_SaveData[SAVE_DATA_INDEX_GRASS_GROWN];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumMonstersDefeated()
{
	S32 iTotal = g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_BLOB] + g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SERPENT] + g_SaveData[SAVE_DATA_INDEX_NUM_KILLED_SMOKE];
	return iTotal;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetNumSeedsCollected()
{
	return g_SaveData[SAVE_DATA_INDEX_SEEDS_COLLECTED];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentGlobals::GetLevelCompleteTimeMilliseconds( S32 _iLevelTimeSaveIndex )
{
	if( _iLevelTimeSaveIndex >= NUM_SAVE_DATA_VALUES )
		return 0;
	
	if( _iLevelTimeSaveIndex < SAVE_DATA_INDEX_LEVEL_TIME_01_01 || _iLevelTimeSaveIndex > SAVE_DATA_INDEX_LEVEL_TIME_11_21 )
	{
		printf( "GetLevelCompleteTimeMilliseconds: Invalid Level Time index: %d\n", _iLevelTimeSaveIndex );
		return 0;
	}
	
	return g_SaveData[ _iLevelTimeSaveIndex ];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentGlobals::Debug_UnlockAllLevels()
{
	for( S32 i = SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01; i <= SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03; ++i )
	{
		g_SaveData[i] = 1;
	}
	
	// DO NOT CHECK THIS IN UNCOMMENTED -- This is for testing the new DLC levels
	//g_SaveData[ SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ] = 0;
	
	// Unlock all the DLC maps
	for( S32 i = SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01; i < SAVE_DATA_INDEX_LEVEL_COMPLETE_11_21; ++i )
	{
		g_SaveData[i] = 1;
	}
	
	g_bAllLevelsUnlocked = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentGlobals::CComponentGlobals()
	: m_bLevelComplete( false )
	, m_pBrickAttackProjectileObject( NULL )
	, m_iTotalNumBreakableLandBricks( 0 )
	, m_iTotalXP( g_iStartingXP )
	, m_iTotalHealthXP( 0 )
	, m_iTotalSpeedXP( 0 )
	, m_iTotalManaXP( 0 )
	, m_iMaxLevelUpUnits( g_iMaxLevelUpUnits )
	, m_bLevelUpScreenIsValid( false )

	, m_pLoadingScreen( NULL )
	, m_pLoadingScreenBG( NULL )
	, m_pPVILoadingScreenImage( NULL )

	, m_bTouchingDown( false )
	, m_pFirstTouchDownObject( NULL )
	, m_iActiveLevelSaveIndex( -1 )

	, m_bLoadingExtensionTimerStarted( false )
	, m_bLoadingExtensionTimerFinished( false )
	, m_fLoadingExtensionTimer( 0.0f )

	, m_pszCurrentLevelName( NULL )
	, m_bIsBossLevel( false )
	, m_bIsGameplayLevel( false )
    , m_bIsTutorialLevel( false )

	, m_uLevelTimerMS( 0 )
	, m_bTimerStopped( true )

	, m_bDebugSpawningDisabled( false )
	, m_bDebugUnlimitedMana( false )	// Set to true for debugging
	, m_bDebugPlayerInvincible( false ) // Set to true for debugging

	, m_bLevelInitializationComplete( false )

	, m_bLoadingFinished( false )

	, m_bGameCenterLoginAttempted( false )

	, m_iNumBlobsKilledThisLevel( 0 )
	, m_iNumSerpentsKilledThisLevel( 0 )
	, m_iNumSmokesKilledThisLevel( 0 )
	, m_iNumSeedsCollectedThisLevel( 0 )
	, m_iNumGrassGrownThisLevel( 0 )

	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	// Initialize the Save Data structure
	for( S32 i = 0; i < NUM_SAVE_DATA_VALUES; ++i )
		g_SaveData[i] = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------