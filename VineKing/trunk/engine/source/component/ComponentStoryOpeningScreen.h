//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSTORYOPENINGSCREEN_H
#define COMPONENTSTORYOPENINGSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentStoryOpeningScreenElement;
class t2dSceneObject;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentStoryOpeningScreen : public DynamicConsoleMethodComponent
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
		DECLARE_CONOBJECT( CComponentStoryOpeningScreen );
		
		CComponentStoryOpeningScreen();
		virtual ~CComponentStoryOpeningScreen();
	
		static void StartStoryOpeningScreen();
		static bool DoesLevelHaveStoryOpening() { return sm_pInstance != NULL; }
		static void PlayOpeningStoryMusic();
	
		static void SetButtonNext( CComponentStoryOpeningScreenElement* const );
		static void SetButtonViewTutorialYES( CComponentStoryOpeningScreenElement* const );
		static void SetButtonViewTutorialNO( CComponentStoryOpeningScreenElement* const );
	
		static void OnButtonNextPressed();
		static void OnButtonViewTutorialYESPressed();
		static void OnButtonViewTutorialNOPressed();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
		
	private:
		t2dSceneObject* m_pOwner;
	
		S32 m_iCurrentStoryImage;
	
		CComponentStoryOpeningScreenElement* m_pElementButtonNext;
		CComponentStoryOpeningScreenElement* m_pElementButtonViewTutorialYES;
		CComponentStoryOpeningScreenElement* m_pElementButtonViewTutorialNO;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
	
		static CComponentStoryOpeningScreen* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSTORYOPENINGSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

