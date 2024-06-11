//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSTORYINGENDINGSCREEN_H
#define COMPONENTSTORYINGENDINGSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentStoryEndingScreen : public DynamicConsoleMethodComponent
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
		DECLARE_CONOBJECT( CComponentStoryEndingScreen );
		
		CComponentStoryEndingScreen();
		virtual ~CComponentStoryEndingScreen();
	
		static bool IsLevelStoryEndingScreen() { return sm_pInstance != NULL; }
		static void StartStoryEndingScreen();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseEnter( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseLeave( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		void UpdateCurrentTextLineAlphaFade();
	
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
	
	private:
		enum EStoryState
		{
			STORY_STATE_NONE = 0,
			STORY_STATE_FADING_IN,
			
			STORY_STATE_TEXT_LINE_01_FADING_IN,
			STORY_STATE_TEXT_LINE_01_FADING_IN_FINISHED,
			
			STORY_STATE_TEXT_LINE_01_FADING_OUT,
			STORY_STATE_TEXT_LINE_01_FADING_OUT_FINISHED,
			
			STORY_STATE_TEXT_LINE_02_FADING_IN,
			STORY_STATE_TEXT_LINE_02_FADING_IN_FINISHED,
			
			STORY_STATE_TEXT_LINE_02_FADING_OUT,
			STORY_STATE_TEXT_LINE_02_FADING_OUT_FINISHED,
			
			STORY_STATE_TEXT_LINE_03_FADING_IN,
			STORY_STATE_TEXT_LINE_03_FADING_IN_FINISHED,
			
			STORY_STATE_TEXT_LINE_03_FADING_OUT,
			STORY_STATE_TEXT_LINE_03_FADING_OUT_FINISHED,
			
			STORY_STATE_TEXT_LINE_04_FADING_IN,
			STORY_STATE_TEXT_LINE_04_FADING_IN_FINISHED,
			
			STORY_STATE_TEXT_LINE_04_FADING_OUT,
			STORY_STATE_TEXT_LINE_04_FADING_OUT_FINISHED,
			
			STORY_STATE_MOVING_UPWARDS_START,
			STORY_STATE_MOVING_UPWARDS,
			
			STORY_STATE_ECLIPSE,
			STORY_STATE_ECLIPSE_FINISHED,
			
			STORY_STATE_FINISHED,
			
			STORY_STATE_CREDITS_1,
			STORY_STATE_CREDITS_2,
			STORY_STATE_CREDITS_3,
			
			STORY_STATE_CREDITS_FINISHED,
		};
		
	private:
		t2dStaticSprite* m_pOwner;
	
		bool m_bAchievementDisplayed;
	
		EStoryState m_eStoryState;
	
		bool m_bEndingMusicStarted;
		bool m_bEclipseMusicStarted;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
	
		static CComponentStoryEndingScreen* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSTORYINGENDINGSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

