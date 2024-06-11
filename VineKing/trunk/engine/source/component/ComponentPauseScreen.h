//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTPAUSESCREEN_H
#define COMPONENTPAUSESCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneGraph;
class t2dSceneObject;
class t2dTextObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentPauseScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentPauseScreen );
		
		CComponentPauseScreen();
		virtual ~CComponentPauseScreen();
	
		static inline CComponentPauseScreen& GetInstance();
	
		void InitializePauseScreenElements();
	
		void SetPauseButtonObject( t2dSceneObject* const _pObject ) { m_pPauseButton = _pObject; }
		//void SetPausedTextObject( t2dTextObject* const _pObject ) { m_pPausedText = _pObject; }
		//void SetResumeTextObject( t2dTextObject* const _pObject ) { m_pResumeText = _pObject; }
		//void SetExitTextObject( t2dTextObject* const _pObject ) { m_pExitText = _pObject; }
		
		void SetPauseMenuImageObject( t2dSceneObject* const _pObject ) { m_pPauseMenuImage = _pObject; }
	
		void SetConfirmQuitMenuImageObject( t2dSceneObject* const _pObject ) { m_pConfirmQuitMenuImage = _pObject; }
	
		void SetResumeButton( t2dSceneObject* const _pObject ) { m_pResumeButton = _pObject; }
		void SetResumeButtonPressedImageObject( t2dSceneObject* const _pObject ) { m_pResumeButtonPressedImage = _pObject; }
	
		void SetMuteButton( t2dSceneObject* const _pObject ) { m_pMuteButton = _pObject; }
		void SetMuteOnOffImageObject( t2dSceneObject* const _pObject ) { m_pMuteOnOffImage = _pObject; }
	
		void SetQuitButton( t2dSceneObject* const _pObject ) { m_pQuitButton = _pObject; }
		void SetQuitButtonPressedImageObject( t2dSceneObject* const _pObject ) { m_pQuitButtonPressedImage = _pObject; }
	
		//void SetButtonPressedImageObject( t2dSceneObject* const _pObject ) { m_pButtonPressedImage = _pObject; }
		
		void SetConfirmQuitYesButton( t2dSceneObject* const _pObject ) { m_pConfirmQuitButtonYes = _pObject; }
		void SetConfirmQuitYesButtonPressedImageObject( t2dSceneObject* const _pObject ) { m_pConfirmQuitButtonYesPressedImage = _pObject; }
	
		void SetConfirmQuitNoButton( t2dSceneObject* const _pObject ) { m_pConfirmQuitButtonNo = _pObject; }
		void SetConfirmQuitNoButtonPressedImageObject( t2dSceneObject* const _pObject ) { m_pConfrimQuitButtonNoPressedImage = _pObject; }
	
		// NEW
		void SetResumeTextObject( t2dSceneObject* const _pObject ) { m_pResumeText = _pObject; }
		void SetMuteTextObject( t2dSceneObject* const _pObject ) { m_pMuteText = _pObject; }
		void SetQuitTextObject( t2dSceneObject* const _pObject ) { m_pQuitText = _pObject; }
	
		void SetQuitConfirmText1Object( t2dSceneObject* const _pObject ) { m_pQuitConfirmText1 = _pObject; }
		void SetQuitConfirmText2Object( t2dSceneObject* const _pObject ) { m_pQuitConfirmText2 = _pObject; }
		void SetQuitConfirmYesTextObject( t2dSceneObject* const _pObject ) { m_pQuitConfirmYesText = _pObject; }
		void SetQuitConfirmNoTextObject( t2dSceneObject* const _pObject ) { m_pQuitConfirmNoText = _pObject; }
	
	
		void SetSceneGraph( t2dSceneGraph* const _pObject ) { m_pSceneGraph = _pObject; }
		
		void OnPauseButtonPressed();
		void OnResumeButtonPressed();
		void OnMuteButtonPressed();
		void OnQuitButtonPressed();
		void OnConfirmQuitYesButtonPressed();
		void OnConfirmQuitNoButtonPressed();
	
		bool IsGamePaused() const { return m_bIsPaused; }
	
		//void HideButtonPressedImage();
		//void UnhideButtonPressedImageAtPosition( const t2dVector& _vPosition );
		void HideResumeButtonPressedImage();
		void UnhideResumeButtonPressedImage();
	
		void HideQuitButtonPressedImage();
		void UnhideQuitButtonPressedImage();
	
		void HideConfirmQuitYesPressedImage();
		void UnhideConfirmQuitYesPressedImage();
	
		void HideConfirmQuitNoPressedImage();
		void UnhideConfirmQuitNoPressedImage();
	
		static bool DoesLevelHavePauseScreen() { return sm_pInstance != NULL; }
	
		// New Shit 2010/01/02
		void SetResumeObject( t2dSceneObject* const _pObject ) { m_pResume	= _pObject; }
		void SetMapObject	( t2dSceneObject* const _pObject ) { m_pMap		= _pObject; }
		void SetMuteObject	( t2dSceneObject* const _pObject ) { m_pMute	= _pObject; }
		void SetQuitBG		( t2dSceneObject* const _pObject ) { m_pQuitBG	= _pObject; }
		void SetQuitYes		( t2dSceneObject* const _pObject ) { m_pQuitYes = _pObject; }
		void SetQuitNo		( t2dSceneObject* const _pObject ) { m_pQuitNo	= _pObject; }
	
		void OnResumePressed();
		void OnMapPressed();
		void OnMutePressed();
		void OnQuitYesPressed();
		void OnQuitNoPressed();
	
		void HidePauseScreen();
		void UnhidePauseScreen();
	
		void HideQuitScreen();
		void UnhideQuitScreen();
	
		static bool IsQuittingToMapScreen() { return sm_pInstance ? sm_pInstance->m_bQuittingToMapScreen: false ; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnPostInit();
	
	private:
		void HidePauseMenuElements();
		void UnhidePauseMenuElements();
	
		void HideConfirmQuitMenuElements();
		void UnhideConfirmQuitMenuElements();
		
	private:
		t2dSceneObject* m_pOwner;
		static CComponentPauseScreen* sm_pInstance;
		
		t2dSceneGraph* m_pSceneGraph;
	
		t2dSceneObject* m_pPauseButton; // Always displayed
	
		//------------------------------------------------
	
		t2dSceneObject* m_pPauseMenuImage;			// Don't know what this is
		t2dSceneObject* m_pConfirmQuitMenuImage;	// Don't know what this is
	
		//------------------------------------------------
	
		t2dSceneObject* m_pResumeButton;
		t2dSceneObject* m_pResumeButtonPressedImage;
	
		t2dSceneObject* m_pMuteButton;
		t2dSceneObject* m_pMuteOnOffImage;
	
		t2dSceneObject* m_pQuitButton;
		t2dSceneObject* m_pQuitButtonPressedImage;
	
		//------------------------------------------------
	
		t2dSceneObject* m_pConfirmQuitButtonYes;
		t2dSceneObject* m_pConfirmQuitButtonYesPressedImage;
	
		t2dSceneObject* m_pConfirmQuitButtonNo;
		t2dSceneObject* m_pConfrimQuitButtonNoPressedImage;
	
		//------------------------------------------------
	
		// Pause Menu Text object
		t2dSceneObject* m_pResumeText;
		t2dSceneObject* m_pMuteText;
		t2dSceneObject* m_pQuitText;
	
		// Confirm Quit Menu text objects
		t2dSceneObject* m_pQuitConfirmText1;
		t2dSceneObject* m_pQuitConfirmText2;
		t2dSceneObject* m_pQuitConfirmYesText;
		t2dSceneObject* m_pQuitConfirmNoText;
	
		// New Shit 2011/01/02
		t2dSceneObject* m_pResume;
		t2dSceneObject* m_pMap;
		t2dSceneObject* m_pMute;
	
		t2dSceneObject* m_pQuitBG;
		t2dSceneObject* m_pQuitYes;
		t2dSceneObject* m_pQuitNo;
	
		bool m_bIsPaused;
		bool m_bSoundMuted;
	
		bool m_bQuittingToMapScreen;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentPauseScreen& CComponentPauseScreen::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentPauseScreen::sm_pInstance == NULL" );
	return *sm_pInstance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTPAUSESCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

