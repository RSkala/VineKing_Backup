//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLOGOSCREEN_H
#define COMPONENTLOGOSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLogoScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentLogoScreen );
		
		CComponentLogoScreen();
		virtual ~CComponentLogoScreen();
		
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
		enum EFadeState
		{
			FADE_STATE_NONE = 0,
			FADE_STATE_FADING_IN,
			FADE_STATE_FADING_OUT,
		};
		
	private:
		t2dSceneObject* m_pOwner;
		F32 m_fTimer;
	
		bool m_bTimerEnabled;
		bool m_bStopUpdating;	
    
        bool m_bShowing2ndLogo;
		
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLOGOSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

