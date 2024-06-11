//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMYSCORESCREEN_H
#define COMPONENTMYSCORESCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMyScoreScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentMyScoreScreen );
		
		CComponentMyScoreScreen();
		virtual ~CComponentMyScoreScreen();
	
		static void OpenMyScoreScreen();
	
		static void OnMyScoreScreenButtonPressed_Left();
		static void OnMyScoreScreenButtonPressed_Right();
		static void OnMyScoreScreenButtonPressed_Back();
		static void OnMyScoreScreenButtonPressed_Leaderboard();
		static void OnMyScoreScreenButtonPressed_Achievements();
		static void OnMyScoreScreenButtonPressed_Facebook();
        static void OnMyScoreScreenButtonPressed_Twitter();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
		
	private:
		void DecrementPage();
		void IncrementPage();
		void HandlePageChange();
	
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
		
	private:
		enum EScreenPage
		{	
			SCREEN_PAGE_LIFE_GROWN = 0,
			SCREEN_PAGE_MONSTERS_DEFEATED,
			SCREEN_PAGE_SEEDS_GATHERED,
			SCREEN_PAGE_COINS_COLLECTED,
			SCREEN_PAGE_TIMES_WORLD_2,
			SCREEN_PAGE_TIMES_WORLD_3,
			SCREEN_PAGE_TIMES_WORLD_4,
			SCREEN_PAGE_TIMES_BOSS_1,
			SCREEN_PAGE_TIMES_WORLD_5,
			SCREEN_PAGE_TIMES_WORLD_6,
			SCREEN_PAGE_TIMES_WORLD_7,
			SCREEN_PAGE_TIMES_BOSS_2,
			SCREEN_PAGE_TIMES_WORLD_8,
			SCREEN_PAGE_TIMES_WORLD_9,
			SCREEN_PAGE_TIMES_WORLD_10,
			SCREEN_PAGE_TIMES_BOSS_3,
			SCREEN_PAGE_TIMES_WORLD_11_1, // Maw of Magma (DLC)
			
			NUM_SCREEN_PAGES,
		};
	
		enum EFadeState
		{
			FADE_STATE_NONE = 0,
			FADE_STATE_FADING_IN,
			FADE_STATE_FADING_OUT,
		};
		
	private:
		t2dSceneObject* m_pOwner;
		static CComponentMyScoreScreen* sm_pInstance;
	
		S32 m_iCurrentScreenPage;
		
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMYSCORESCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

