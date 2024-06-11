//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTTUTORIALOPENINGSCREEN_H
#define COMPONENTTUTORIALOPENINGSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentTutorialOpeningScreen : public DynamicConsoleMethodComponent
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
		DECLARE_CONOBJECT( CComponentTutorialOpeningScreen );
		
		CComponentTutorialOpeningScreen();
		virtual ~CComponentTutorialOpeningScreen();
		
		static bool IsLevelTutorialOpeningScreen() { return sm_pInstance != NULL; }
		static void PlayOpeningTutorialMusic();
		static void SetLoadedFromMainMenu() { sm_bLoadedFromMainMenu = true; sm_bLoadedFromStoryScreen = false; }
		static void SetLoadedFromStoryScreen() { sm_bLoadedFromStoryScreen = true; sm_bLoadedFromMainMenu = false; }
		
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
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
		
	private:
		t2dStaticSprite* m_pOwner;
		
		S32 m_iCurrentTutorialImage;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
		
		static CComponentTutorialOpeningScreen* sm_pInstance;
		static bool sm_bLoadedFromMainMenu;
		static bool sm_bLoadedFromStoryScreen;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTTUTORIALOPENINGSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

