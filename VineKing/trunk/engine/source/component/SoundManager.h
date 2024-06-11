//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "core/ITickable.h"
#include "platform/platformAudio.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class AudioProfile;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CSoundManager : public virtual ITickable
{
	public:
		CSoundManager();
		~CSoundManager();
	
		static inline CSoundManager& GetInstance();
	
		static void Initialize();
	
		// BGM
		void PlayBGMMainMenuRPG();
		void PlayBGMMapScreen();
		void PlayInGameBGM();
		void PlayYouWinBGM();
		void PlayYouLoseBGM();
	
		void PlayStoryOpeningBGM();
	
		void PlayStoryEndingBGM();
		void PlayEclipseBGM();
		void PlayCreditsBGM();
		
		void StopCurrentBGM();
	
		// UI
		void PlayButtonClickBack();
		void PlayButtonClickForward();
	
		// Player
		void PlayBrickBreakSound();
		void PlayCrystalBreakSound();
		void PlayHomeBaseTakeDamageSound();
		void PlayTapSound();
		void PlayEatSound();
	
		void PlayChantingSound();
		void StopChantingSound();
	
		void PlayBeamOfLightSound();
		void StopBeamOfLightSound();
	
		void PlayVineGrowSound();
		void StopVineGrowSound();
	
		void PlayCheeringSound();
		void StopCheeringSound();
	
		void PlayHomeBaseDeathSound();
	
		void PlayOutOfManaSound();
	
		void PlayVortexSound();
		void StopVortexSound();
	
		void PlayBGMAfterBecomingActive();
	
	
		// Generic Enemy
		void PlayGenericSpawnSound();
	
		// Blob
		void PlayBlobAttackPlayerSound();
		void PlayBlobAttackTileSound();
		void PlayBlobDeathSound();
	
		// Serpent
		void PlaySerpentSpitSound();
		void PlaySerpentDeathSound();
	
		// Smoke
		void PlaySmokeSpawnSound();
		void PlaySmokeAttackSound();
		void PlaySmokeDeathSound();
	
		// Boss
		void PlayBossDamageSound();
		void PlayBossDeathSound();
	
		// Cash
		void PlayCashSound();
	
		// Intro / Outro
		void PlayIntroFallSound();
		void PlayThudSound();
		void PlayWinGruntSound();
		void PlayFinalGrowSound();
	
		// Tutorial
		void PlayTutorialStingerSound();
	
		// Global Sound
		void MuteAllSound();
		void UnmuteAllSound();
	
		bool IsSoundMuted() const { return m_bAllSoundMuted; }
	
		void FadeCurrentBGM( const F32& );
	
		// Inherited from ITickable
		virtual void interpolateTick( F32 delta ) { }
		virtual void processTick();
		virtual void advanceTime( F32 timeDelta ) { };
	
	private:
		void InitializeSounds();
		
	private:
		static CSoundManager sm_Instance;
	
		// Audio Descriptions
		AudioDescription* m_pAudioDesc_AudioLooping;
		AudioDescription* m_pAudioDesc_AudioNonLooping;
	
		bool m_bAllSoundMuted;
	
	
		// Music
		AudioProfile* m_pBGMMainMenuRPG;
		AUDIOHANDLE m_hBGMMainMenuRPG;
	
		AudioProfile* m_pBGMMapScreen;
		AUDIOHANDLE m_hBGMMapScreen;
	
		AudioProfile* m_pBGMInGame;
		AUDIOHANDLE m_hBGMInGame;
	
		AudioProfile* m_pBGMYouWin;
		AUDIOHANDLE m_hBGMYouWin;
	
		AudioProfile* m_pBGMYouLose;
		AUDIOHANDLE m_hBGMYouLose;
	
		AudioProfile* m_pBGMOpeningStory;
		AUDIOHANDLE m_hBGMOpeningStory;
		
		AUDIOHANDLE m_hCurrentBGM;
	
		// UI
		AudioProfile* m_pButtonClickBackSound;
		AUDIOHANDLE m_hButtonClickBackSound;
	
		AudioProfile* m_pButtonClickForwardSound;
		AUDIOHANDLE m_hButtonClickForwardSound;
	
	
		// Player
		AudioProfile* m_pBrickBreakSound;
		AUDIOHANDLE m_hBrickBreakSound;	
	
		AudioProfile* m_pCrystalBreakSound;
		AUDIOHANDLE m_hCrystalBreakSound;
	
		AudioProfile* m_pPlayerTakeDamageSound;
		AUDIOHANDLE m_hPlayerTakeDamageSound;
	
		AudioProfile* m_pPlayerTapSound;
		AUDIOHANDLE m_hPlayerTapSound;
	
		AudioProfile* m_pPlayerEatSound;
		AUDIOHANDLE m_hPlayerEatSound;
	
		AudioProfile* m_pPlayerChantingSound;
		AUDIOHANDLE m_hPlayerChantingSound;
	
	
		AudioProfile* m_pBeamOfLightSound;
		AUDIOHANDLE m_hBeamOfLightSound;
	
		AudioProfile* m_pVineGrowSound;
		AUDIOHANDLE m_hVineGrowSound;
	
		AudioProfile* m_pCheeringSound;
		AUDIOHANDLE m_hCheeringSound;
	
		AudioProfile* m_pHomeBaseDeathSound;
		AUDIOHANDLE m_hHomeBaseDeathSound;
	
		AudioProfile* m_pOutOfManaSound;
		AUDIOHANDLE m_hOutOfManaSound;
	
		AudioProfile* m_pVortexSound;
		AUDIOHANDLE m_hVortexSound;
	
		// Generic Enemy
		AudioProfile* m_pGenericEnemySpawnSound;
		AUDIOHANDLE m_hGenericEnemySpawnSound;
	
		// Blob
		AudioProfile* m_pBlobAttackPlayerSound;
		AUDIOHANDLE m_hBlobAttackPlayerSound;
		
		AudioProfile* m_pBlobAttackTileSound;
		AUDIOHANDLE m_hBlobAttackTileSound;
	
		AudioProfile* m_pBlobDeathSound;
		AUDIOHANDLE m_hBlobDeathSound;
	
		// Serpent
		AudioProfile* m_pSerpentAttackSound;
		AUDIOHANDLE m_hSerpentAttackSound;
	
		AudioProfile* m_pSerpentDeathSound;
		AUDIOHANDLE m_hSerpentDeathSound;
		
		// Smoke
		AudioProfile* m_pSmokeSpawnSound;
		AUDIOHANDLE m_hSmokeSpawnSound;
	
		AudioProfile* m_pSmokeAttackSound;
		AUDIOHANDLE m_hSmokeAttackSound;
	
		AudioProfile* m_pSmokeDeathSound;
		AUDIOHANDLE m_hSmokeDeathSound;
	
		// Boss
		AudioProfile* m_pBossDamageSound;
		AUDIOHANDLE m_hBossDamageSound;
	
		AudioProfile* m_pBossDeathSound;
		AUDIOHANDLE m_hBossDeathSound;
	
		// Cash
		AudioProfile* m_pCashSound;
		AUDIOHANDLE m_hCashSound;
	
		// Intro / Outro Sounds
		AudioProfile* m_pIntroFallSound;
		AUDIOHANDLE m_hIntroFallSound;
	
		AudioProfile* m_pThudSound;
		AUDIOHANDLE m_hThudSound;
	
		AudioProfile* m_pWinGruntSound;
		AUDIOHANDLE m_hWinGruntSound;
	
		AudioProfile* m_pFinalGrowSound;
		AUDIOHANDLE m_hFinalGrowSound;
	
		// Ending
		AudioProfile* m_pBGM_StoryEnding;
		AUDIOHANDLE m_hBGM_StoryEnding;
		
		AudioProfile* m_pBGM_Eclipse;
		AUDIOHANDLE m_hBGM_Eclipse;
		
		AudioProfile* m_pBGM_Credits;
		AUDIOHANDLE m_hBGM_Credits;
	
		// Tutorial
		AudioProfile* m_pTutorialStingerSound;
		AUDIOHANDLE m_hTutorialStingerSound;
	
	
		// Story Ending fade
		bool m_bFadeCurrentBGM;
		F32 m_fCurrentBGMVolume;
		F32 m_fBGMFadeTimer;
		F32 m_fBGMFadeTimeSeconds;
		AudioProfile* m_pBGMToPlayAfterFade;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CSoundManager& CSoundManager::GetInstance()
{
	return sm_Instance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // SOUNDMANAGER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------