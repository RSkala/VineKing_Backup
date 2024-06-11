//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTTUTORIALLEVEL_H
#define COMPONENTTUTORIALLEVEL_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dPath;
class t2dSceneObject;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentTutorialLevel : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
    
    public:
        enum EFadeState
        {
            FADE_STATE_NONE = 0,
            FADE_STATE_FADING_IN,
            FADE_STATE_FADING_OUT,
        };
	
	public:
		DECLARE_CONOBJECT( CComponentTutorialLevel );
		
		CComponentTutorialLevel();
		virtual ~CComponentTutorialLevel();
	
		static bool IsTutorialLevel() { return sm_pInstance != NULL; }
		static bool IsPauseAllowed();
		static bool IsLineDrawAllowed();
		static bool IsSpawningAllowed();
		static bool IsManaSeedButtonAllowed();
    
        
    
    
    
	
		static bool HasTutorialBeenShown_LineDraw()		{ return sm_pInstance ? sm_pInstance->m_bTutorialShown_LineDraw : false; }
		static bool HasTutorialBeenShown_SeedSuck()		{ return sm_pInstance ? sm_pInstance->m_bTutorialShown_SeedSuck : false; }
		static bool HasTutorialBeenShown_Blava()		{ return sm_pInstance ? sm_pInstance->m_bTutorialShown_Blava : false; }
		static bool HasTutorialBeenShown_VineAttack()	{ return sm_pInstance ? sm_pInstance->m_bTutorialShown_VineAttack : false; }
		static bool HasTutorialBeenShown_Damage()		{ return sm_pInstance ? sm_pInstance->m_bTutorialShown_Damage : false; }
	
		static void StartTutorial();
    
        //2012/06/25 - NEW TUTORIAL STUFF
        void StartTutorialState( int );
    
        static void OnTheFingerPathEndReached();
        static void OnPlayerDrewLine();
        static void OnBrickBroken();
        static void FadeToNextTutorialState();
    
        static bool IsBrickBreakingAllowed();
        static bool IsManaDrainingAllowed();
        static bool IsSeedSpawningAllowed();
        static void OnPlayerOutOfMana();
        static void OnPlayerEatSeed();
        static void OnPlayerPressedSeedButton();
        static void OnMonsterSpawned();
        static void OnMonsterTapped();
        static bool CanLevelEnd();
        static void OnLevelEnded();
        //////////////////////////////////////////////////////////////////
    
		static S32 GetSufficientNumBricks() { return 4; }
		static void OnSufficientNumBricksBroken();
    
	
		static void StartTutorialSeedSuckDelayTimer() { if( sm_pInstance ) sm_pInstance->m_bSeedTutorialTimerStarted = true; }
		static void EnableTutorialSeedSuck() { if( sm_pInstance ) sm_pInstance->m_bTutorialSeedSuckEnabled = true; }
		static void EnableTutorialBlava() { if( sm_pInstance ) sm_pInstance->m_bTutorialBlavaEnabled = true; }
	
		static void ShowTutorialLineDraw();
		static void ShowTutorialSeedSuck();
		static void ShowTutorialBlava();
		static void ShowTutorialVineAttack();
		static void ShowTutorialDamage();
	
		static void OnTutorialButtonPressedLineDraw();
		static void OnTutorialButtonPressedSeedSuck();
		static void OnTutorialButtonPressedBlava();
		static void OnTutorialButtonPressedVineAttack();
		static void OnTutorialButtonPressedDamage();
	
		static bool IsTutorialActive();
    
        void AddAllPathNodes();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void SetDefaultTutorialImageProperties( t2dStaticSprite* const );
		void SetDefaultTutorialButtonProperties( t2dStaticSprite* const );
		void InitializeCurrentDisplayObject( t2dStaticSprite* const, t2dStaticSprite* const );
		void UpdateCurrentDisplayObjectEntry();
		void OnTutorialImageAtMaxSize();
    
        
        void UpdateFade();
        void OnFadeOutFinished();
        void OnFadeInFinished();
			
	private:
    
        enum ETutorialState
        {
            TUTORIAL_STATE_NONE = 0,
            
            TUTORIAL_STATE_GAMEPLAY,
            
            // Part 1
            TUTORIAL_STATE_THE_FINGER_DRAWING,
            TUTORIAL_STATE_BREAKING_ROCKS,
            
            // Part 2
            TUTORIAL_STATE_GREEN_METER,
            TUTORIAL_STATE_GATHERING_SEEDS,
            TUTORIAL_STATE_EATING_SEEDS,
            
            
            // Part 3
            TUTORIAL_STATE_SPAWNING_MONSTERS,
            TUTORIAL_STATE_FINISHING_LEVEL,
        };
    
    
        enum ETutorialBillboardState
        {
            BILLBOARD_STATE_NONE = 0,
            BILLBOARD_STATE_SCALING,
            BILLBOARD_STATE_FULL_SIZE,
            BILLBOARD_STATE_FINISHED,
        };
    
    
//		enum ETutorialState
//		{
//			TUTORIAL_STATE_NONE = 0,
//			
//			TUTORIAL_STATE_GAMEPLAY,
//			
//			TUTORIAL_STATE_DISPLAY_LINEDRAW_SCALING,
//			TUTORIAL_STATE_DISPLAY_LINEDRAW_FULL_SIZE,
//			
//			TUTORIAL_STATE_DISPLAY_SEEDSUCK_SCALING,
//			TUTORIAL_STATE_DISPLAY_SEEDSUCK_FULL_SIZE,
//			
//			TUTORIAL_STATE_DISPLAY_BLAVA_SCALING,
//			TUTORIAL_STATE_DISPLAY_BLAVA_FULL_SIZE,
//			
//			TUTORIAL_STATE_DISPLAY_VINEATTACK_SCALING,
//			TUTORIAL_STATE_DISPLAY_VINEATTACK_FULL_SIZE,
//			
//			TUTORIAL_STATE_DISPLAY_DAMAGE_SCALING,
//			TUTORIAL_STATE_DISPLAY_DAMAGE_FULL_SIZE,
//			
//			TUTORIAL_STATE_FINISHED,
//		};
		
	private:
		t2dSceneObject* m_pOwner;
	
		ETutorialState m_eTutorialState;
	
		t2dStaticSprite* m_pCurrentDisplayObject;
    
    
        t2dStaticSprite* m_pTextImageTop;
        t2dStaticSprite* m_pTextImageBottom;
    
    
        bool m_bTutorialTextDisplayed;
    
    
        t2dStaticSprite* m_pTheFinger;
        t2dPath* m_pFingerPath;

        ETutorialBillboardState m_eBillboardState;
    
    
        bool m_bUpdateFingerRepeating;
        bool m_bFingerPausedToLift;
        bool m_bFingerLifted;
        bool m_bFingerVanished;
        bool m_bNodesReadded;
    
        F32 m_fFingerPausedTimer; // Time to pause to stop and then lift the finger
        F32 m_fFingerLiftedTimer; // Time to keep the finger raised after lifting
        F32 m_fFingerVanishedTimer; // Time before the finger will restart the line draw
        
    
	
		bool m_bTutorialShown_LineDraw;
		bool m_bTutorialShown_SeedSuck;
		bool m_bTutorialShown_Blava;
		bool m_bTutorialShown_VineAttack;
		bool m_bTutorialShown_Damage;
	
		bool m_bSpawningEnabled;
	
		bool m_bTutorialSeedSuckEnabled;	
		bool m_bTutorialBlavaEnabled;
	
		bool m_bTutorialSeedSuckInProgress;
		bool m_bTutorialBlavaInProgress; // This is needed because time will increment slightly between panels
	
		bool m_bSeedTutorialTimerStarted;
		bool m_bSeedTutorialTimerActive;
		F32 m_fSeedTutorialDisplayDelayTimer;
	
		bool m_bTimeScaleStopped;
    
    
    
    
    
        // Fade Stuff
        EFadeState m_eFadeState;
        F32 m_fFadeTimer;
        F32 m_fFadeAmount;
        
	
		static CComponentTutorialLevel* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTTUTORIALLEVEL_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

