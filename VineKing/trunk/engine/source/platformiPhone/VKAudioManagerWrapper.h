//
//  VKAudioManagerWrapper.h
//  iTorque2D
//
//  Created by Richard Skala on 2/13/14.
//
//

//#ifndef __iTorque2D__VKAudioManagerWrapper__
//#define __iTorque2D__VKAudioManagerWrapper__

//#include <iostream>

//#endif /* defined(__iTorque2D__VKAudioManagerWrapper__) */
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef VKAUDIOMANAGERWRAPPER_H
#define VKAUDIOMANAGERWRAPPER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace VKAudioManagerWrapper
{
    ///////////////////////////////////
    // GENERAL
    void InitializeAudio();
    void PlayBGMAfterBecomingActive();
    
    ///////////////////////////////////
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
    
    //////////////////////////////////
    // SFX UI
    //+ (void) playButtonClickBack;
    //+ (void) playButtonClickForward;
    void PlayButtonClickBack();
    void PlayButtonClickForward();
    
    //////////////////////////////////
    // SFX Player
    void PlayBrickBreakSound();
    void PlayCrystalBreakSound();
    void PlayHomeBaseTakeDamageSound();
    void PlayTapSound();
    void PlayEatSound();
    void PlayChantingSound();
    void StopChantingSound();
    //void PlayBeamOfLightSound();
    //void StopBeamOfLightSound();
    //void PlayVineGrowSound();
    //void StopVineGrowSound();
    //void PlayCheeringSound();
    //void StopCheeringSound();
    void PlayHomeBaseDeathSound();
    void PlayOutOfManaSound();
    void PlayVortexSound();
    void StopVortexSound();
    
    //////////////////////////////////
    // Generic Enemy
    void PlayGenericSpawnSound();
    
    //////////////////////////////////
    // Blob
    //void PlayBlobAttackPlayerSound(); // NOT USED
    void PlayBlobAttackTileSound();
    void PlayBlobDeathSound();
    
    //////////////////////////////////
    // Serpent
    void PlaySerpentSpitSound();
    void PlaySerpentDeathSound();
    
    //////////////////////////////////
    // Smoke
    void PlaySmokeSpawnSound();
    //void PlaySmokeAttackSound();
    void PlaySmokeDeathSound();
    
    //////////////////////////////////
    // Boss
    void PlayBossDamageSound();
    void PlayBossDeathSound();
    
    //////////////////////////////////
    // Cash
    void PlayCashSound();
    
    //////////////////////////////////
    // Intro / Outro
    void PlayIntroFallSound();
    void PlayThudSound();
    void PlayWinGruntSound();
    void PlayFinalGrowSound();
    
    //////////////////////////////////
    // Tutorial
    void PlayTutorialStingerSound();
    
    //////////////////////////////////
    // Global Sound
    void MuteAllSound();
    void UnmuteAllSound();
    bool IsSoundMuted();
    
    //////////////////////////////////
    //void FadeCurrentBGM( const F32& );
    //+ (void) fadeCurrentBGM;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // VKAUDIOMANAGERWRAPPER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------