//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  VKAudioManagerWrapper.cpp
//  iTorque2D
//
//  Created by Richard Skala on 2/13/14.
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//#include "GameCenterWrapper.h"
//#include "GameCenter.h"
//
//#include "component/ComponentAchievementBanner.h"
//#include "component/ComponentGlobals.h"

#include "VKAudioManagerWrapper.h"

#include "component/ComponentGlobals.h"
#import "VKAudioManager.h"

// NOTE: I WILL NEED TO IMPLEMENT SOMETHING LIKE THIS:
// void CSoundManager::PlayBGMAfterBecomingActive()

//using namespace VKAudioManagerWrapper;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// General
void VKAudioManagerWrapper::InitializeAudio() {
    [VKAudioManager initializeAudio];
}

void VKAudioManagerWrapper::PlayBGMAfterBecomingActive() {
    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// BGM
void VKAudioManagerWrapper::PlayBGMMainMenuRPG() {
    [VKAudioManager playBGMMainMenuRPG];
}

void VKAudioManagerWrapper::PlayBGMMapScreen() {
    [VKAudioManager playBGMMapScreen];
}

void VKAudioManagerWrapper::PlayInGameBGM() {
    if(CComponentGlobals::GetInstance().IsBossLevel()) {
        [VKAudioManager playBGMBoss];
    }
    else if(CComponentGlobals::GetInstance().IsTutorialLevel()) {
        [VKAudioManager playBGMMapScreen];
    }
    else {
        [VKAudioManager playInGameBGM];
    }
}

void VKAudioManagerWrapper::PlayYouWinBGM() {
    if(CComponentGlobals::GetInstance().IsBossLevel()) {
        [VKAudioManager playBossWinBGM];
    }
    else {
        [VKAudioManager playYouWinBGM];
    }
}

void VKAudioManagerWrapper::PlayYouLoseBGM() {
    [VKAudioManager playYouLoseBGM];
}

void VKAudioManagerWrapper::PlayStoryOpeningBGM() {
    [VKAudioManager playStoryOpeningBGM];
}

void VKAudioManagerWrapper::PlayStoryEndingBGM() {
    [VKAudioManager playStoryEndingBGM];
}

void VKAudioManagerWrapper::PlayEclipseBGM() {
    [VKAudioManager playEclipseBGM];
}

void VKAudioManagerWrapper::PlayCreditsBGM() {
    [VKAudioManager playCreditsBGM];
}

void VKAudioManagerWrapper::StopCurrentBGM(){
    [VKAudioManager stopCurrentBGM];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// SFX UI
void VKAudioManagerWrapper::PlayButtonClickBack() {
    [VKAudioManager playButtonClickBack];
}

void VKAudioManagerWrapper::PlayButtonClickForward() {
    [VKAudioManager playButtonClickForward];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// SFX UI
void VKAudioManagerWrapper::PlayBrickBreakSound() {
    [VKAudioManager playBrickBreakSound];
}

void VKAudioManagerWrapper::PlayCrystalBreakSound() {
    [VKAudioManager playCrystalBreakSound];
}

void VKAudioManagerWrapper::PlayHomeBaseTakeDamageSound() {
    [VKAudioManager playHomeBaseTakeDamageSound];
}

void VKAudioManagerWrapper::PlayTapSound() {
    [VKAudioManager playTapSound];
}

void VKAudioManagerWrapper::PlayEatSound() {
    [VKAudioManager playEatSound];
}

void VKAudioManagerWrapper::PlayChantingSound() {
    [VKAudioManager playChantingSound];
}

void VKAudioManagerWrapper::StopChantingSound() {
    [VKAudioManager stopChantingSound];
}

//void VKAudioManagerWrapper::PlayBeamOfLightSound() {
//    [VKAudioManager playBeamOfLightSound];
//}
//
//void VKAudioManagerWrapper::StopBeamOfLightSound() {
//    [VKAudioManager stopBeamOfLightSound];
//}
//
//void VKAudioManagerWrapper::PlayVineGrowSound() {
//    [VKAudioManager playVineGrowSound];
//}
//
//void VKAudioManagerWrapper::StopVineGrowSound() {
//    [VKAudioManager stopVineGrowSound];
//}

void VKAudioManagerWrapper::PlayHomeBaseDeathSound() {
    [VKAudioManager playHomeBaseDeathSound];
}

void VKAudioManagerWrapper::PlayOutOfManaSound() {
    [VKAudioManager playOutOfManaSound];
}

void VKAudioManagerWrapper::PlayVortexSound() {
    [VKAudioManager playVortexSound];
}

void VKAudioManagerWrapper::StopVortexSound() {
    [VKAudioManager stopVortexSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Generic Enemy
void VKAudioManagerWrapper::PlayGenericSpawnSound() {
    [VKAudioManager playGenericSpawnSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Blob
//void VKAudioManagerWrapper::PlayBlobAttackPlayerSound() {
//    [VKAudioManager playBlobAttackPlayerSound];
//}

void VKAudioManagerWrapper::PlayBlobAttackTileSound() {
    [VKAudioManager playBlobAttackTileSound];
}

void VKAudioManagerWrapper::PlayBlobDeathSound() {
    [VKAudioManager playBlobDeathSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Serpent
void VKAudioManagerWrapper::PlaySerpentSpitSound() {
    [VKAudioManager playSerpentSpitSound];
}

void VKAudioManagerWrapper::PlaySerpentDeathSound() {
    [VKAudioManager playSerpentDeathSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Smoke
void VKAudioManagerWrapper::PlaySmokeSpawnSound() {
    [VKAudioManager playSmokeSpawnSound];
}

//void VKAudioManagerWrapper::PlaySmokeAttackSound() {
//    [VKAudioManager playSmokeAttackSound];
//}

void VKAudioManagerWrapper::PlaySmokeDeathSound() {
    [VKAudioManager playSmokeDeathSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Boss
void VKAudioManagerWrapper::PlayBossDamageSound() {
    [VKAudioManager playBossDamageSound];
}

void VKAudioManagerWrapper::PlayBossDeathSound() {
    [VKAudioManager playBossDeathSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Cash
void VKAudioManagerWrapper::PlayCashSound() {
    [VKAudioManager playCashSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Intro / Outro
void VKAudioManagerWrapper::PlayIntroFallSound() {
    [VKAudioManager playIntroFallSound];
}

void VKAudioManagerWrapper::PlayThudSound() {
    [VKAudioManager playThudSound];
}

void VKAudioManagerWrapper::PlayWinGruntSound() {
    [VKAudioManager playWinGruntSound];
}

void VKAudioManagerWrapper::PlayFinalGrowSound() {
    [VKAudioManager playFinalGrowSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Tutorial
void VKAudioManagerWrapper::PlayTutorialStingerSound() {
    [VKAudioManager playTutorialStingerSound];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global Sound
void VKAudioManagerWrapper::MuteAllSound() {
    [VKAudioManager muteAllSound];
}

void VKAudioManagerWrapper::UnmuteAllSound() {
    [VKAudioManager unmuteAllSound];
}

bool VKAudioManagerWrapper::IsSoundMuted() {
    return (bool)([VKAudioManager isSoundMuted]);
}

















