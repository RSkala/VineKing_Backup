//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTHEARTSCREEN_H
#define COMPONENTHEARTSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentHeartScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentHeartScreen );
		
		CComponentHeartScreen();
		virtual ~CComponentHeartScreen();
		
		static void OpenHeartScreen();
	
		static void OnHeartScreenButtonPressed_Back();
		static void OnHeartScreenButtonPressed_AppStore();
		static void OnHeartScreenButtonPressed_Facebook();
		static void OnHeartScreenButtonPressed_MoreVineKing();
		static void OnHeartScreenButtonPressed_Twitter();
		
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
	
		static CComponentHeartScreen* sm_pInstance;
		
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTHEARTSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

