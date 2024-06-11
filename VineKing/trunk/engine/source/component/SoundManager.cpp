//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "SoundManager.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentSeedMagnetButton.h"
#include "ComponentTutorialLevel.h"

#include "audio/audioDataBlock.h"
#include <stdio.h>

//#include "platform/platformAudio.h"
//#include "audio/audio.h" // This file includes audioDataBlock.h and platformAudio.h

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Audio - These names are the names from the AudioProfile objects from the scripts

// Audio Descriptions
static const char g_szAudioDesc_Looping[32]		= "AudioLooping";
static const char g_szAudioDesc_NonLooping[32]  = "AudioNonLooping";

// Music
static const char g_szBGMMainMenuRPGName[32]	= "BGM_MainMenuRPG";
static const char g_szBGMMapSelectScreen[32]	= "BGM_MapSelectScreen";
static const char g_szBGMInGame[32]				= "BGMGameMusic";
static const char g_szBGMYouWin[32]				= "BGMWin";
static const char g_szBGMYouLose[32]			= "BGMLose";
static const char g_szBGMOpeningStory[32]		= "BGM_StoryOpening";

// UI
static const char g_szButtonClickBackSound[32]		= "ButtonClickBack";
static const char g_szButtonClickForwardSound[32]	= "ButtonClickForward";

// Death Audio
//static const char g_szEnemyADeathSoundName[16]	= "EnemyDeath_A";
//static const char g_szEnemyBDeathSoundName[16]	= "EnemyDeath_B";
//static const char g_szEnemyCDeathSoundName[16]	= "EnemyDeath_C";
//static const char g_szEnemyDDeathSoundName[16]	= "EnemyDeath_D";
//static const char g_szManaEaterDeathSound[32]	= "EnemyDeath_D";

// Player related audio
static const char g_szBrickBreakSound[32]			= "LavaTileShatter";
static const char g_szCrystalBreakSound[32]			= "CrystalShatter"; 
static const char g_szHomeBaseTakeDamageSound[32]	= "PlayerTakeDamage";
static const char g_szPlayerTapSound[16]			= "AttackTap";
static const char g_szPlayerEatSound[16]			= "PlayerEat";
static const char g_szPlayerChantSound[32]			= "PlayerChanting";

//static const char g_szBeamOfLightSound[32]			= "BeamOfLight";
//static const char g_szVineGrowingSound[32]			= "VineGrowing";
//static const char g_szCheeringSound[32]				= "CheeringSmall";

static const char g_szHomeBaseDeathSound[32]		= "PlayerDeath";
static const char g_szOutOfManaSound[32]			= "OutOfMana";
static const char g_szVortexSound[32]				= "VortexSound";

// Non-specific enemy audio
static const char g_szGenericSpawnSound[32]		= "EnemySpawn";

// Blob audio
//static const char g_szBlobAttackPlayerSound[32] = "BlobAttackPlayer";
static const char g_szBlobAttackTileSound[32]	= "BlobPuke";
static const char g_szBlobDeathSound[32]		= "BlobDeath";

// Serpent audio
static const char g_szSerpentSpitSound[32]		= "SerpentAttack";
static const char g_szSerpentDeathSound[32]		= "SerpentDeath";

// Smoke audio
static const char g_szSmokeSpawnSound[32]		= "SmokeSpawn";
//static const char g_szSmokeAttackSound[32]		= "SmokeAttack";
static const char g_szSmokeDeathSound[32]		= "SmokeMonsterDeath";

// Boss audio
static const char g_szBossDamageSound[32]		= "BossDamage";
static const char g_szBossDeathSound[32]		= "BossDeath";

// Cash sound
static const char g_szCashSound[32]				= "CashSound";

// Intro / Outro Sounds
static const char g_szIntroFallSound[32]		= "IntroFallSound";
static const char g_szThudSound[32]				= "ThudSound";
static const char g_szWinGruntSound[32]			= "WinGruntSound";
static const char g_szFinalGrowSound[32]		= "FinalGrowSound";

// Ending
static const char g_szBGM_StoryEnding[32]		= "BGM_StoryEnding";
static const char g_szBGM_Eclipse[32]			= "BGM_Eclipse";
static const char g_szBGM_Credits[32]			= "BGM_Credits";

// Tutorial
static const char g_szTutorialStingerSound[32]	= "TutorialStinger";


// Audio Types / Channels (Indexes into mAudioTypeVolume array)
static const S32 g_iAudioChannelBGM_Looping		= 1; // $pref::Audio::channelVolume1
static const S32 g_iAudioChannelBGM_NonLooping	= 2; // $pref::Audio::channelVolume2
static const S32 g_iAudioChannelSFX				= 3; // $pref::Audio::channelVolume3

static const F32 g_fMaxAudioVolume_BGM_Looping		= 0.8f;  // $pref::Audio::channelVolume1
static const F32 g_fMaxAudioVolume_BGM_NonLooping	= 0.8f;  // $pref::Audio::channelVolume2
static const F32 g_fMaxAudioVolume_SFX				= 0.8f;  // $pref::Audio::channelVolume3

static const F32 g_fDefaultMasterVolume = 0.8f; // Needs to match $pref::Audio::masterVolume in defaultPrefs.cs (Probably should do Con::getFloatVariable, but that is unsafe)

static const bool g_bPrintVolumeDebugInfo = false;

// Fade
static const F32 g_fEndingBGMFadeTimeSeconds = 1.0f;	// Matches the text fade time
static const F32 g_fEclipseBGMFadeTimeSeconds = 2.0f;	// Matches the eclipse fade time

// Variables to make sure that not too many sounds are played at once
static U32 g_uTimeSinceLastSpawnSoundPlayed = 0;
static U32 g_uTimeSinceLastSmokeSpawnSoundPlayed = 0;
static U32 g_uTimeSinceLastBrickBreakSoundPlayed = 0;
static U32 g_uTimeSinceLastCrystalBreakSoundPlayed = 0;
static U32 g_uTimeSinceLastSeedEatSoundPlayed = 0;
static U32 g_uTimeSinceLastSerpentSpitSoundPlayed = 0;

static const U32 g_uDuplicateSoundMinTime = 200;			// Amount of time that must have passed before stopping a spawn sound (Milliseconds)
static const U32 g_uDuplicateBrickBreakSoundMinTime = 50;	// Amount of time that must have passed before stopping a brick/crystal break sound (Milliseconds)
static const U32 g_uDuplicateSeedEatSoundMinTime = 50;		// Amount of time that must have passed before stopping a seed eat sound (Milliseconds)
static const U32 g_uDuplicateSpitSoundMinTime = 50;			// Amount of time that must have passed before stopping a serpent spit sound


static const bool g_bAllowSoundStopping = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CSoundManager CSoundManager::sm_Instance;

extern F32 mAudioTypeVolume[Audio::NumAudioTypes];

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CSoundManager::CSoundManager()
	: m_bAllSoundMuted( false )
	, m_bFadeCurrentBGM( false )
	, m_fCurrentBGMVolume( 1.0f )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CSoundManager::~CSoundManager()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CSoundManager::Initialize()
{
	sm_Instance.InitializeSounds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBGMMainMenuRPG()
{
	StopCurrentBGM();
	
	m_hCurrentBGM = m_hBGMMainMenuRPG = alxPlay( m_pBGMMainMenuRPG );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBGMMapScreen()
{
	StopCurrentBGM();
	
	m_hCurrentBGM = m_hBGMMapScreen = alxPlay( m_pBGMMapScreen );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayInGameBGM()
{
	StopCurrentBGM();
	
	m_hCurrentBGM = m_hBGMInGame = alxPlay( m_pBGMInGame );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayYouWinBGM()
{
	StopCurrentBGM();
	
	if( alxIsPlaying( m_hWinGruntSound ) )
		alxStop( m_hWinGruntSound );
	
	if( alxIsPlaying( m_hFinalGrowSound ) )
		alxStop( m_hFinalGrowSound );
	
	m_hCurrentBGM = m_hBGMYouWin = alxPlay( m_pBGMYouWin );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayYouLoseBGM()
{
	StopCurrentBGM();
	
	m_hCurrentBGM = m_hBGMYouLose = alxPlay( m_pBGMYouLose );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayStoryOpeningBGM()
{
	StopCurrentBGM();
	
	m_hCurrentBGM = m_hBGMOpeningStory = alxPlay( m_pBGMOpeningStory );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayStoryEndingBGM()
{
	StopCurrentBGM();
	//printf( "PlayStoryEndingBGM\n" );
	
	//m_hCurrentBGM = m_hBGM_StoryEnding = alxPlay( m_pBGM_StoryEnding );
	m_hCurrentBGM = alxPlay( m_pBGM_StoryEnding );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayEclipseBGM()
{
	//StopCurrentBGM();
	//printf( "PlayEclipseBGM\n" );
	
	FadeCurrentBGM( g_fEndingBGMFadeTimeSeconds );
	
	//m_hCurrentBGM = m_hBGM_Eclipse = alxPlay( m_pBGM_Eclipse );
	m_pBGMToPlayAfterFade = m_pBGM_Eclipse;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayCreditsBGM()
{
	//StopCurrentBGM();
	//printf( "PlayCreditsBGM\n" );
	
	FadeCurrentBGM( g_fEclipseBGMFadeTimeSeconds );
	
	//m_hCurrentBGM = m_hBGM_Credits = alxPlay( m_pBGM_Credits );
	m_pBGMToPlayAfterFade = m_pBGM_Credits;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::StopCurrentBGM()
{
	//printf( "StopCurrentBGM\n" );
	
	if( alxIsPlaying( m_hCurrentBGM ) )
		alxStop( m_hCurrentBGM );
	
	m_hCurrentBGM = m_hBGMMainMenuRPG = NULL_AUDIOHANDLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayButtonClickBack()
{
	if( alxIsPlaying( m_hButtonClickBackSound ) )
		alxStop( m_hButtonClickBackSound );
	
	m_hButtonClickBackSound = alxPlay( m_pButtonClickBackSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayButtonClickForward()
{
	if( alxIsPlaying( m_hButtonClickForwardSound ) )
		alxStop( m_hButtonClickForwardSound );
	
	m_hButtonClickForwardSound = alxPlay( m_pButtonClickForwardSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBrickBreakSound()
{
	if( ( Platform::getRealMilliseconds() - g_uTimeSinceLastBrickBreakSoundPlayed ) <= g_uDuplicateBrickBreakSoundMinTime )
		return;
	
	if( alxIsPlaying( m_hBrickBreakSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hBrickBreakSound );
		m_hBrickBreakSound = NULL_AUDIOHANDLE;
	}
	
	m_hBrickBreakSound = alxPlay( m_pBrickBreakSound );
	
	g_uTimeSinceLastBrickBreakSoundPlayed = Platform::getRealMilliseconds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayCrystalBreakSound()
{
	if( ( Platform::getRealMilliseconds() - g_uTimeSinceLastCrystalBreakSoundPlayed ) <= g_uDuplicateBrickBreakSoundMinTime )
		return;
	
	if( alxIsPlaying( m_hCrystalBreakSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hCrystalBreakSound );
		m_hCrystalBreakSound = NULL_AUDIOHANDLE;
	}
	
	m_hCrystalBreakSound = alxPlay( m_pCrystalBreakSound );
	
	g_uTimeSinceLastCrystalBreakSoundPlayed = Platform::getRealMilliseconds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayHomeBaseTakeDamageSound()
{
	if( alxIsPlaying( m_hPlayerTakeDamageSound ) )
		alxStop( m_hPlayerTakeDamageSound );
	
	m_hPlayerTakeDamageSound = alxPlay( m_pPlayerTakeDamageSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayTapSound()
{
	if( alxIsPlaying( m_hPlayerTapSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hPlayerTapSound );
	}
	
	m_hPlayerTapSound = alxPlay( m_pPlayerTapSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayEatSound()
{
	if( ( Platform::getRealMilliseconds() - g_uTimeSinceLastSeedEatSoundPlayed ) <= g_uDuplicateSeedEatSoundMinTime )
		return;
	
	if( alxIsPlaying( m_hPlayerEatSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hPlayerEatSound );
		m_hPlayerEatSound = NULL_AUDIOHANDLE;
	}
	
	m_hPlayerEatSound = alxPlay( m_pPlayerEatSound );
	
	g_uTimeSinceLastSeedEatSoundPlayed = Platform::getRealMilliseconds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayChantingSound()
{	
	if( m_bAllSoundMuted )
		return;
	
	if( alxIsPlaying( m_hPlayerChantingSound ) )
		alxStop( m_hPlayerChantingSound );
	
	m_hPlayerChantingSound = alxPlay( m_pPlayerChantingSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::StopChantingSound()
{
	if( alxIsPlaying( m_hPlayerChantingSound ) )
		alxStop( m_hPlayerChantingSound );
	
	m_hPlayerChantingSound = NULL_AUDIOHANDLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBeamOfLightSound()
{
	//if( alxIsPlaying( m_hBeamOfLightSound ) )
	//	alxStop( m_hBeamOfLightSound );
	
	//m_hBeamOfLightSound = alxPlay( m_pBeamOfLightSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::StopBeamOfLightSound()
{
	if( m_bAllSoundMuted )
		return;
	
	if( alxIsPlaying( m_hBeamOfLightSound ) )
		alxStop( m_hBeamOfLightSound );
	
	m_hBeamOfLightSound = NULL_AUDIOHANDLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayVineGrowSound()
{
	//if( alxIsPlaying( m_hVineGrowSound ) )
	//	alxStop( m_hVineGrowSound );
	
	//m_hVineGrowSound = alxPlay( m_pVineGrowSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::StopVineGrowSound()
{
	//if( m_bAllSoundMuted )
	//	return;
	
	//if( alxIsPlaying( m_hVineGrowSound ) )
	//	alxStop( m_hVineGrowSound );
	
	//m_hVineGrowSound = NULL_AUDIOHANDLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayCheeringSound()
{
	//if( m_bAllSoundMuted )
	//	return;
	
	//if( alxIsPlaying( m_hCheeringSound ) )
	//	alxStop( m_hCheeringSound );
	
	//m_hCheeringSound = alxPlay( m_pCheeringSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::StopCheeringSound()
{
	//if( alxIsPlaying( m_hCheeringSound ) )
	//	alxStop( m_hCheeringSound );
	
	//m_hCheeringSound = NULL_AUDIOHANDLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayHomeBaseDeathSound()
{
	if( alxIsPlaying( m_hHomeBaseDeathSound ) )
		alxStop( m_hHomeBaseDeathSound );
	
	m_hHomeBaseDeathSound = alxPlay( m_pHomeBaseDeathSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayOutOfManaSound()
{
	if( alxIsPlaying( m_hOutOfManaSound ) )
		return;
	
	m_hOutOfManaSound = alxPlay( m_pOutOfManaSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayVortexSound()
{
	if( alxIsPlaying( m_hVortexSound ) )
		alxStop( m_hVortexSound );
	
	m_hVortexSound = alxPlay( m_pVortexSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::StopVortexSound()
{
	if( m_bAllSoundMuted )
		return;
	
	if( alxIsPlaying( m_hVortexSound ) )
		alxStop( m_hVortexSound );
	
	m_hVortexSound = NULL_AUDIOHANDLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBGMAfterBecomingActive()
{
	if( g_bPrintVolumeDebugInfo )
		printf( "PlayBGMAfterBecomingActive()\n" );
	
	// Reset Master Volume
	//alxListenerf( AL_GAIN_LINEAR, $pref::Audio::masterVolume );
	F32 fMasterVolume = Con::getFloatVariable( "$pref::Audio::masterVolume", g_fDefaultMasterVolume );
	if( g_bPrintVolumeDebugInfo )
		printf( "fMasterVolume: %f\n", fMasterVolume );
	alxListenerf( AL_GAIN_LINEAR, fMasterVolume );
	
	const char* pszCurrentLevelName = CComponentGlobals::GetInstance().GetCurrentLevelName();
	if( pszCurrentLevelName == NULL || pszCurrentLevelName[0] == '\0' )
		return;
	
	if( m_bAllSoundMuted == false )
	{
		if( g_bPrintVolumeDebugInfo )
		{
			printf( "mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]: %f\n", mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] );
			printf( "mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]: %f\n", mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ] );
			printf( "mAudioTypeVolume[ g_iAudioChannelSFX ]: %f\n", mAudioTypeVolume[ g_iAudioChannelSFX ] );
		}
		
		mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]		= Con::getFloatVariable( "$pref::Audio::channelVolume1", g_fMaxAudioVolume_BGM_Looping );
		alxUpdateTypeGain( g_iAudioChannelBGM_Looping );
			
		mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]	= Con::getFloatVariable( "$pref::Audio::channelVolume2", g_fMaxAudioVolume_BGM_NonLooping );
		alxUpdateTypeGain( g_iAudioChannelBGM_NonLooping );
		
		mAudioTypeVolume[ g_iAudioChannelSFX ]				= Con::getFloatVariable( "$pref::Audio::channelVolume3", g_fMaxAudioVolume_SFX );
		alxUpdateTypeGain( g_iAudioChannelSFX );
		
		if( g_bPrintVolumeDebugInfo )
		{
			printf( "mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]: %f\n", mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] );
			printf( "mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]: %f\n", mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ] );
			printf( "mAudioTypeVolume[ g_iAudioChannelSFX ]: %f\n", mAudioTypeVolume[ g_iAudioChannelSFX ] );
		}
	}
	
	m_hCurrentBGM = m_hBGMOpeningStory = m_hBGMYouLose = m_hBGMYouWin = m_hBGMInGame = m_hBGMMapScreen = m_hBGMMainMenuRPG = NULL_AUDIOHANDLE;
	
	if( strcmp( pszCurrentLevelName, "Screen_MainMenu.t2d" ) == 0 )
	{
		PlayBGMMainMenuRPG();
	}
	else if( strcmp( pszCurrentLevelName, "Screen_MapSelect_PowerUp.t2d" ) == 0 )
	{
		PlayBGMMapScreen();
	}
	else if( strcmp( pszCurrentLevelName, "Screen_StoryOpening.t2d" ) == 0 )
	{
		PlayStoryOpeningBGM();
	}
	else if( strcmp( pszCurrentLevelName, "Screen_TutorialOpening.t2d" ) == 0 )
	{
		CSoundManager::GetInstance().PlayBGMMapScreen();
	}
	else if( strstr( pszCurrentLevelName, "Level_" ) != NULL || strstr( pszCurrentLevelName, "Boss_" ) != NULL )
	{
		if( CComponentEndLevelScreen::HasLevelEnded() == false )
		{
			if( CComponentGlobals::GetInstance().IsTimerActive() )
				PlayInGameBGM();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayGenericSpawnSound()
{
	if( ( Platform::getRealMilliseconds() - g_uTimeSinceLastSpawnSoundPlayed ) <= g_uDuplicateSoundMinTime )
		return;
	
	if( alxIsPlaying( m_hGenericEnemySpawnSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hGenericEnemySpawnSound );
		m_hGenericEnemySpawnSound = NULL_AUDIOHANDLE;
	}
	
	m_hGenericEnemySpawnSound = alxPlay( m_pGenericEnemySpawnSound );

	g_uTimeSinceLastSpawnSoundPlayed = Platform::getRealMilliseconds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBlobAttackPlayerSound()
{
	//if( alxIsPlaying( m_hBlobAttackPlayerSound ) )
	//	alxStop( m_hBlobAttackPlayerSound );
	
	//m_hBlobAttackPlayerSound = alxPlay( m_pBlobAttackPlayerSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBlobAttackTileSound()
{
	if( alxIsPlaying( m_hBlobAttackTileSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hBlobAttackTileSound );
	}
	
	m_hBlobAttackTileSound = alxPlay( m_pBlobAttackTileSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBlobDeathSound()
{
	if( alxIsPlaying( m_hBlobDeathSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hBlobDeathSound );
	}
	
	m_hBlobDeathSound = alxPlay( m_pBlobDeathSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlaySerpentSpitSound()
{
	if( ( Platform::getRealMilliseconds() - g_uTimeSinceLastSerpentSpitSoundPlayed ) <= g_uDuplicateSpitSoundMinTime )
		return;
	
	if( alxIsPlaying( m_hSerpentAttackSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hSerpentAttackSound );
		m_hSerpentAttackSound = NULL_AUDIOHANDLE;
	}
	
	m_hSerpentAttackSound = alxPlay( m_pSerpentAttackSound );
	
	g_uTimeSinceLastSerpentSpitSoundPlayed = Platform::getRealMilliseconds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlaySerpentDeathSound()
{
	if( alxIsPlaying( m_hSerpentDeathSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hSerpentDeathSound );
	}
	
	m_hSerpentDeathSound = alxPlay( m_pSerpentDeathSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlaySmokeSpawnSound()
{
	if( ( Platform::getRealMilliseconds() - g_uTimeSinceLastSmokeSpawnSoundPlayed ) <= g_uDuplicateSoundMinTime )
		return;
	
	if( alxIsPlaying( m_hSmokeSpawnSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hSmokeSpawnSound );
		m_hSmokeSpawnSound = NULL_AUDIOHANDLE;
	}
	
	m_hSmokeSpawnSound = alxPlay( m_pSmokeSpawnSound );
	
	g_uTimeSinceLastSmokeSpawnSoundPlayed = Platform::getRealMilliseconds();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlaySmokeAttackSound()
{
	//if( alxIsPlaying( m_hSmokeAttackSound ) )
	//	alxStop( m_hSmokeAttackSound );
	
	//m_hSmokeAttackSound = alxPlay( m_pSmokeAttackSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlaySmokeDeathSound()
{
	if( alxIsPlaying( m_hSmokeDeathSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hSmokeDeathSound );
	}
	
	m_hSmokeDeathSound = alxPlay( m_pSmokeDeathSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBossDamageSound()
{
	if( alxIsPlaying( m_hBossDamageSound ) )
	{
		if( g_bAllowSoundStopping )
			alxStop( m_hBossDamageSound );
	}
	
	m_hBossDamageSound = alxPlay( m_pBossDamageSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayBossDeathSound()
{
	if( alxIsPlaying( m_hBossDamageSound ) )
		alxStop( m_hBossDamageSound );
	
	if( alxIsPlaying( m_hBossDeathSound ) )
		alxStop( m_hBossDeathSound );
	
	m_hBossDeathSound = alxPlay( m_pBossDeathSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayCashSound()
{
	if( alxIsPlaying( m_hCashSound ) )
	   alxStop( m_hCashSound );
	   
	m_hCashSound = alxPlay( m_pCashSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayIntroFallSound()
{
	if( alxIsPlaying( m_hIntroFallSound ) )
		alxStop( m_hIntroFallSound );
	
	m_hIntroFallSound = alxPlay( m_pIntroFallSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayThudSound()
{
	if( alxIsPlaying( m_hThudSound ) )
		alxStop( m_hThudSound );
	
	m_hThudSound = alxPlay( m_pThudSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayWinGruntSound()
{
	if( alxIsPlaying( m_hWinGruntSound ) )
		alxStop( m_hWinGruntSound );
	
	m_hWinGruntSound = alxPlay( m_pWinGruntSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayFinalGrowSound()
{
	if( alxIsPlaying( m_hFinalGrowSound ) )
		alxStop( m_hFinalGrowSound );
	
	m_hFinalGrowSound = alxPlay( m_pFinalGrowSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::PlayTutorialStingerSound()
{
	m_hTutorialStingerSound = alxPlay( m_pTutorialStingerSound );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::MuteAllSound()
{
	if( g_bPrintVolumeDebugInfo )
		printf( "MuteAllSound()\n" );
		
	if( g_bPrintVolumeDebugInfo )
	{
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]: %f\n",		mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]: %f\n",	mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelSFX ]: %f\n",				mAudioTypeVolume[ g_iAudioChannelSFX ] );
	}
	
	mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]		= 0.0f;
	alxUpdateTypeGain( g_iAudioChannelBGM_Looping );
	
	mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]	= 0.0f;
	alxUpdateTypeGain( g_iAudioChannelBGM_NonLooping );
	
	mAudioTypeVolume[ g_iAudioChannelSFX ]				= 0.0f;
	alxUpdateTypeGain( g_iAudioChannelSFX );
	
	if( g_bPrintVolumeDebugInfo )
	{
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]: %f\n",		mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]: %f\n",	mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelSFX ]: %f\n",				mAudioTypeVolume[ g_iAudioChannelSFX ] );
	}
	
	m_bAllSoundMuted = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::UnmuteAllSound()
{
	if( g_bPrintVolumeDebugInfo )
		printf( "UnmuteAllSound()\n" );
	
	if( g_bPrintVolumeDebugInfo )
	{
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]: %f\n",		mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]: %f\n",	mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelSFX ]: %f\n",				mAudioTypeVolume[ g_iAudioChannelSFX ] );
	}
	
	mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]		= Con::getFloatVariable( "$pref::Audio::channelVolume1", g_fMaxAudioVolume_BGM_Looping );
	alxUpdateTypeGain( g_iAudioChannelBGM_Looping );
	
	mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]	= Con::getFloatVariable( "$pref::Audio::channelVolume2", g_fMaxAudioVolume_BGM_NonLooping );
	alxUpdateTypeGain( g_iAudioChannelBGM_NonLooping );
	
	mAudioTypeVolume[ g_iAudioChannelSFX ]				= Con::getFloatVariable( "$pref::Audio::channelVolume3", g_fMaxAudioVolume_SFX );
	alxUpdateTypeGain( g_iAudioChannelSFX );
	
	if( g_bPrintVolumeDebugInfo )
	{
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_Looping ]: %f\n",		mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ]: %f\n",	mAudioTypeVolume[ g_iAudioChannelBGM_NonLooping ] );
		printf( "mAudioTypeVolume[ g_iAudioChannelSFX ]: %f\n",				mAudioTypeVolume[ g_iAudioChannelSFX ] );
	}
	
	m_bAllSoundMuted = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSoundManager::FadeCurrentBGM( const F32& _fFadeTimeSeconds )
{ 
	if( m_bAllSoundMuted )
		return;
	
	m_bFadeCurrentBGM = true;
	
	m_fCurrentBGMVolume = Con::getFloatVariable( "$pref::Audio::channelVolume1", g_fMaxAudioVolume_BGM_Looping );
	
	m_fBGMFadeTimeSeconds = m_fBGMFadeTimer = _fFadeTimeSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*virtual*/ void CSoundManager::processTick()
{
	// Update the faked looped sounds
	if( CComponentGlobals::GetInstance().IsGameplayLevel() )
	{
		// Play the chanting sound if the Line Draw is active
		if( CComponentLineDrawAttack::IsLineDrawAttackActive() &&
		    m_hPlayerChantingSound != NULL_AUDIOHANDLE &&
			alxIsPlaying( m_hPlayerChantingSound ) == false )
		{
			PlayChantingSound();
		}
		
		// Play the Vortex sound if the Seed Button is pressed
		if( CComponentSeedMagnetButton::IsPressed() && 
		    CComponentInGameTutorialObject::IsTutorialActive() == false &&
		    CComponentTutorialLevel::IsTutorialActive() == false &&
		    m_hVortexSound != NULL_AUDIOHANDLE &&
		    alxIsPlaying( m_hVortexSound ) == false )
		{
			PlayVortexSound();
		}
	}
	
	// Update BGM fade - Works only for looping BGM's
	if( m_bFadeCurrentBGM )
	{
		if( m_fCurrentBGMVolume <= 0.0f )
		{
			if( m_pBGMToPlayAfterFade )
			{
				StopCurrentBGM();
				
				if( m_bAllSoundMuted == false )
				{
					mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] = Con::getFloatVariable( "$pref::Audio::channelVolume1", g_fMaxAudioVolume_BGM_Looping );
					alxUpdateTypeGain( g_iAudioChannelBGM_Looping );
				}
				
				m_hCurrentBGM = alxPlay( m_pBGMToPlayAfterFade );
				//printf( "Playing new BGM\n" );
			
				m_pBGMToPlayAfterFade = NULL;
			}
			
			m_bFadeCurrentBGM = false;
			return;
		}
		
		m_fBGMFadeTimer -= ITickable::smTickSec;
		if( m_fBGMFadeTimer <= 0.0f )
		{
			m_fBGMFadeTimer = 0.0f;
		}
		
		m_fCurrentBGMVolume = m_fBGMFadeTimer / m_fBGMFadeTimeSeconds;
		//if( m_fCurrentBGMVolume > 0.8f )
		//{
		//	m_fCurrentBGMVolume = 0.8f;
		//}
		
		//printf( "m_fCurrentBGMVolume: %f\n", m_fCurrentBGMVolume ); 
		
		mAudioTypeVolume[ g_iAudioChannelBGM_Looping ] = m_fCurrentBGMVolume;
		alxUpdateTypeGain( g_iAudioChannelBGM_Looping );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CSoundManager::InitializeSounds()
{	
	g_uTimeSinceLastSpawnSoundPlayed = 0;
	g_uTimeSinceLastSmokeSpawnSoundPlayed = 0;
	g_uTimeSinceLastBrickBreakSoundPlayed = 0;
	g_uTimeSinceLastCrystalBreakSoundPlayed = 0;
	g_uTimeSinceLastSeedEatSoundPlayed = 0;
	g_uTimeSinceLastSerpentSpitSoundPlayed = 0;
	
	// Audio Descriptions
	m_pAudioDesc_AudioLooping = NULL;
	m_pAudioDesc_AudioNonLooping = NULL;
	
	//BGM
	m_pBGMMainMenuRPG = NULL;
	m_hBGMMainMenuRPG = NULL_AUDIOHANDLE;
	
	m_pBGMInGame = NULL;
	m_hBGMInGame = NULL_AUDIOHANDLE;
	
	m_pBGMYouWin = NULL;
	m_hBGMYouWin = NULL_AUDIOHANDLE;
	
	m_pBGMYouLose = NULL;
	m_hBGMYouLose = NULL_AUDIOHANDLE;
	
	m_pBGMMapScreen = NULL;
	m_hBGMMapScreen = NULL_AUDIOHANDLE;
	
	m_pBGMOpeningStory = NULL;
	m_hBGMOpeningStory = NULL_AUDIOHANDLE;
	
	m_hCurrentBGM = NULL_AUDIOHANDLE;
	
	// UI
	m_pButtonClickBackSound = NULL;
	m_hButtonClickBackSound = NULL_AUDIOHANDLE;
	
	m_pButtonClickForwardSound = NULL;
	m_hButtonClickForwardSound = NULL_AUDIOHANDLE;
	
	
	// Player SFX
	m_pBrickBreakSound = NULL;
	m_hBrickBreakSound = NULL_AUDIOHANDLE;
	
	m_pCrystalBreakSound = NULL;
	m_hCrystalBreakSound = NULL_AUDIOHANDLE;

	m_pPlayerTakeDamageSound = NULL;
	m_hPlayerTakeDamageSound = NULL_AUDIOHANDLE;

	m_pPlayerTapSound = NULL;
	m_hPlayerTapSound = NULL_AUDIOHANDLE;

	m_pPlayerEatSound = NULL;
	m_hPlayerEatSound = NULL_AUDIOHANDLE;
	
	m_pPlayerChantingSound = NULL;
	m_hPlayerChantingSound = NULL_AUDIOHANDLE;
	
	
	//m_pBeamOfLightSound = NULL;
	//m_hBeamOfLightSound = NULL_AUDIOHANDLE;
	
	//m_pVineGrowSound = NULL;
	//m_hVineGrowSound = NULL_AUDIOHANDLE;
	
	//m_pCheeringSound = NULL;
	//m_hCheeringSound = NULL_AUDIOHANDLE;
	
	m_pHomeBaseDeathSound = NULL;
	m_hHomeBaseDeathSound = NULL_AUDIOHANDLE;
	
	m_pOutOfManaSound = NULL;
	m_hOutOfManaSound = NULL_AUDIOHANDLE;
	
	m_pVortexSound = NULL;
	m_hVortexSound = NULL_AUDIOHANDLE;
	
	// Generic SFX
	m_pGenericEnemySpawnSound = NULL;
	m_hGenericEnemySpawnSound = NULL_AUDIOHANDLE;

	// Enemy SFX
	//m_pBlobAttackPlayerSound = NULL;
	//m_hBlobAttackPlayerSound = NULL_AUDIOHANDLE;

	m_pBlobAttackTileSound = NULL;
	m_hBlobAttackTileSound = NULL_AUDIOHANDLE;

	m_pBlobDeathSound = NULL;
	m_hBlobDeathSound = NULL_AUDIOHANDLE;

	m_pSerpentAttackSound = NULL;
	m_hSerpentAttackSound = NULL_AUDIOHANDLE;

	m_pSerpentDeathSound = NULL;
	m_hSerpentDeathSound = NULL_AUDIOHANDLE;
	
	m_pSmokeSpawnSound = NULL;
	m_hSmokeSpawnSound = NULL_AUDIOHANDLE;

	//m_pSmokeAttackSound = NULL;
	//m_hSmokeAttackSound = NULL_AUDIOHANDLE;

	m_pSmokeDeathSound = NULL;
	m_hSmokeDeathSound = NULL_AUDIOHANDLE;
	
	// Boss
	m_pBossDamageSound = NULL;
	m_hBossDamageSound = NULL_AUDIOHANDLE;
	
	m_pBossDeathSound = NULL;
	m_hBossDeathSound = NULL_AUDIOHANDLE;
	
	// Intro / Outro Sounds
	m_pIntroFallSound = NULL;
	m_hIntroFallSound = NULL_AUDIOHANDLE;

	m_pThudSound = NULL;
	m_hThudSound = NULL_AUDIOHANDLE;

	m_pWinGruntSound = NULL;
	m_hWinGruntSound = NULL_AUDIOHANDLE;

	m_pFinalGrowSound = NULL;
	m_hFinalGrowSound = NULL_AUDIOHANDLE;
	
	// Ending
	m_pBGM_StoryEnding = NULL;
	m_hBGM_StoryEnding = NULL_AUDIOHANDLE;
	
	m_pBGM_Eclipse = NULL;
	m_hBGM_Eclipse = NULL_AUDIOHANDLE;
	
	m_pBGM_Credits = NULL;
	m_hBGM_Credits = NULL_AUDIOHANDLE;
	
	// Tutorial
	m_pTutorialStingerSound = NULL;
	m_hTutorialStingerSound = NULL_AUDIOHANDLE;
	
	// Audio Descriptions
	m_pAudioDesc_AudioLooping		= static_cast<AudioDescription*>( Sim::findObject( g_szAudioDesc_Looping ) );
	m_pAudioDesc_AudioNonLooping	= static_cast<AudioDescription*>( Sim::findObject( g_szAudioDesc_NonLooping ) );
	
	// BGM
	m_pBGMMainMenuRPG	= static_cast<AudioProfile*>( Sim::findObject( g_szBGMMainMenuRPGName ) );
	m_pBGMMapScreen		= static_cast<AudioProfile*>( Sim::findObject( g_szBGMMapSelectScreen ) );
	m_pBGMInGame		= static_cast<AudioProfile*>( Sim::findObject( g_szBGMInGame ) );
	m_pBGMYouWin		= static_cast<AudioProfile*>( Sim::findObject( g_szBGMYouWin ) );
	m_pBGMYouLose		= static_cast<AudioProfile*>( Sim::findObject( g_szBGMYouLose ) );
	m_pBGMOpeningStory	= static_cast<AudioProfile*>( Sim::findObject( g_szBGMOpeningStory ) );
	
	// UI
	m_pButtonClickBackSound		= static_cast<AudioProfile*>( Sim::findObject( g_szButtonClickBackSound ) );
	m_pButtonClickForwardSound	= static_cast<AudioProfile*>( Sim::findObject( g_szButtonClickForwardSound ) );
	
	// Player audio
	m_pBrickBreakSound			= static_cast<AudioProfile*>( Sim::findObject( g_szBrickBreakSound ) );
	m_pCrystalBreakSound			= static_cast<AudioProfile*>( Sim::findObject( g_szCrystalBreakSound ) );
	m_pPlayerTakeDamageSound	= static_cast<AudioProfile*>( Sim::findObject( g_szHomeBaseTakeDamageSound ) );
	m_pPlayerTapSound			= static_cast<AudioProfile*>( Sim::findObject( g_szPlayerTapSound ) );
	m_pPlayerEatSound			= static_cast<AudioProfile*>( Sim::findObject( g_szPlayerEatSound ) );
	m_pPlayerChantingSound		= static_cast<AudioProfile*>( Sim::findObject( g_szPlayerChantSound ) );
	
	//m_pBeamOfLightSound = static_cast<AudioProfile*>( Sim::findObject( g_szBeamOfLightSound ) );
	//m_pVineGrowSound	= static_cast<AudioProfile*>( Sim::findObject( g_szVineGrowingSound ) );
	//m_pCheeringSound	= static_cast<AudioProfile*>( Sim::findObject( g_szCheeringSound ) );
	
	m_pHomeBaseDeathSound	= static_cast<AudioProfile*>( Sim::findObject( g_szHomeBaseDeathSound ) );
	
	m_pOutOfManaSound		= static_cast<AudioProfile*>( Sim::findObject( g_szOutOfManaSound ) );
	
	m_pVortexSound		= static_cast<AudioProfile*>( Sim::findObject( g_szVortexSound ) );
	
	// Generic Enemy audio
	m_pGenericEnemySpawnSound	= static_cast<AudioProfile*>( Sim::findObject( g_szGenericSpawnSound ) );
	
	// Blob audio
	//m_pBlobAttackPlayerSound	= static_cast<AudioProfile*>( Sim::findObject( g_szBlobAttackPlayerSound ) );
	m_pBlobAttackTileSound		= static_cast<AudioProfile*>( Sim::findObject( g_szBlobAttackTileSound ) );
	m_pBlobDeathSound			= static_cast<AudioProfile*>( Sim::findObject( g_szBlobDeathSound ) );
	
	// Serpent audio
	m_pSerpentAttackSound		= static_cast<AudioProfile*>( Sim::findObject( g_szSerpentSpitSound ) );
	m_pSerpentDeathSound		= static_cast<AudioProfile*>( Sim::findObject( g_szSerpentDeathSound ) );
	
	// Smoke audio
	m_pSmokeSpawnSound		= static_cast<AudioProfile*>( Sim::findObject( g_szSmokeSpawnSound ) );
	//m_pSmokeAttackSound		= static_cast<AudioProfile*>( Sim::findObject( g_szSmokeAttackSound ) );
	m_pSmokeDeathSound		= static_cast<AudioProfile*>( Sim::findObject( g_szSmokeDeathSound ) );
	
	// Boss audio
	m_pBossDamageSound		= static_cast<AudioProfile*>( Sim::findObject( g_szBossDamageSound ) );
	m_pBossDeathSound		= static_cast<AudioProfile*>( Sim::findObject( g_szBossDeathSound ) );
	
	// Cash Sound
	m_pCashSound			= static_cast<AudioProfile*>( Sim::findObject( g_szCashSound ) );
	
	// Intro / Outro Sounds
	m_pIntroFallSound	= static_cast<AudioProfile*>( Sim::findObject( g_szIntroFallSound ) );
	m_pThudSound		= static_cast<AudioProfile*>( Sim::findObject( g_szThudSound ) );
	m_pWinGruntSound	= static_cast<AudioProfile*>( Sim::findObject( g_szWinGruntSound ) );
	m_pFinalGrowSound	= static_cast<AudioProfile*>( Sim::findObject( g_szFinalGrowSound ) );
	
	
	// Ending
	m_pBGM_StoryEnding	= static_cast<AudioProfile*>( Sim::findObject( g_szBGM_StoryEnding ) );
	m_pBGM_Eclipse		= static_cast<AudioProfile*>( Sim::findObject( g_szBGM_Eclipse ) );
	m_pBGM_Credits		= static_cast<AudioProfile*>( Sim::findObject( g_szBGM_Credits ) );
	
	// Tutorial
	m_pTutorialStingerSound = static_cast<AudioProfile*>( Sim::findObject( g_szTutorialStingerSound ) );
	
	
	// Ending fades
	m_bFadeCurrentBGM = false;
	m_fCurrentBGMVolume = 0.0f;
	m_fBGMFadeTimer = 0.0f;
	m_fBGMFadeTimeSeconds = 0.0f;
	m_pBGMToPlayAfterFade = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
