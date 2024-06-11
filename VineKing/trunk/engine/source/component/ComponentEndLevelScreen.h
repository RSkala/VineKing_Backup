//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENDLEVELSCREEN_H
#define COMPONENTENDLEVELSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEndLevelScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEndLevelScreen );
		
		CComponentEndLevelScreen();
		virtual ~CComponentEndLevelScreen();
		
		static inline CComponentEndLevelScreen& GetInstance();
	
		void ShowYouWinScreen();
		void ShowGameOverScreen();
		void HideAllElements();
	
		void OnOKButtonPressed();
	
		void SetYouWinTextObject( t2dSceneObject* const _pObject ) { m_pYouWinTextObject = _pObject; }
		void SetGameOverTextObject( t2dSceneObject* const _pObject ) { m_pGameOverTextObject = _pObject; }
		void SetOKButtonObject( t2dSceneObject* const _pObject ) { m_pOKButton = _pObject; }
	
		static bool HasLevelEnded() { return sm_pInstance ? sm_pInstance->m_bLevelHasEnded : false; }
		static bool IsLevelWon() { return sm_pInstance ? sm_pInstance->m_bLevelWon : false; }
		
		void SetFinalGrowSoundPlayed() { m_bFinalGrowSoundPlayed = true; }
        void SetFinalGrowSoundNotPlayed() { m_bFinalGrowSoundPlayed = false; }
		bool HasFinalGrowSoundBeenPlayed() const { return m_bFinalGrowSoundPlayed; } 
	
		void StartLevelWinSequence();
		void StartLevelLoseSequence();
	
		void StartEndLevelTextSequence();
		void UpdateEndLevelText();
	
		void DisplayLevelCompleteAchievement();
	
		void OnLevelWon();
		void OnLevelLost();
		void StartFadeOut();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		// Fade Screen functions
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
		static CComponentEndLevelScreen* sm_pInstance;
	
		t2dSceneObject* m_pYouWinTextObject;
		t2dSceneObject* m_pGameOverTextObject;
		t2dSceneObject* m_pOKButton;
	
		bool m_bLevelHasEnded;
		bool m_bLevelWon;
	
		bool m_bSmokePlayed;
	
		F32 m_fPostLandTimer;
	
		bool m_bEndScreenShown;
	
		// End Level Message variables
		bool m_bDisplayEndLevelMessage;
		t2dSceneObject* m_pEndLevelMessage;
		//F32 m_fCurrentEndLevelMessageRotation;
		t2dVector m_vCurrentEndLevelMessageSize;
		F32 m_fCurrentEndLevelMessageSizeX;
		F32 m_fCurrentEndLevelMessageSizeY;
		bool m_bRotationStopped;
	
		bool m_bFinalGrowSoundPlayed;
	
		// Fade Stuff
		bool m_bStartFadeOut;
		F32 m_fFadeDelayTimer;
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentEndLevelScreen& CComponentEndLevelScreen::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentEndLevelScreen::sm_pInstance == NULL" );
	return *sm_pInstance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENDLEVELSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

