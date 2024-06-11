//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMAINMENUSCREEN_H
#define COMPONENTMAINMENUSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMainMenuScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EScreenMode
		{
			SCREEN_MODE_NONE = -1,
			SCREEN_MODE_MAINMENU = 0,
			SCREEN_MODE_OPTIONS,
			SCREEN_MODE_CREDITS,
			SCREEN_MODE_CLEARDATA
		};
		
	public:
		DECLARE_CONOBJECT( CComponentMainMenuScreen );
		
		CComponentMainMenuScreen();
		virtual ~CComponentMainMenuScreen();
		
		static inline CComponentMainMenuScreen& GetInstance();
		
		void StartMainMenuScreen();
	
		// Main Menu Button Handlers
		void OnNewGameButtonPressed();
		void OnContinueButtonPressed();
		void OnOptionsButtonPressed();
		void OnTutorialButtonPressed();
		void OnCreditsNextButtonPressed();
	
		void OnGameCenterButtonPressed();
		void OnGameCenterResetAchievementsButtonPressed();
	
		void OnMyScoresButtonPressed();
		void OnHeartButtonPressed();
	
		void OnFacebookLoginButtonPressed();
	
		static void OnFacebookUserLoggedIn();
		static void OnFacebookUserLoggedOut();
		
		// Options Menu Button Handlers
		void OnMuteButtonPressed();
		void OnClearDataButtonPressed();
		void OnBackButtonPressed();
	
		static bool IsInMainMenuScreen() { return sm_pInstance != NULL; }
	
		void AddMainMenuScreenElement( t2dSceneObject* const _pObject ) { m_MainMenuElements.push_back( _pObject ); }
		void AddOptionsScreenElement( t2dSceneObject* const _pObject ) { m_OptionsScreenElements.push_back( _pObject ); }
		void AddCreditsScreenElement( t2dSceneObject* const _pObject ) { m_CreditsScreenElements.push_back( _pObject ); }
		void AddClearDataScreenElement( t2dSceneObject* const _pObject ) { m_ClearDataScreenElements.push_back( _pObject ); }
		
		void SetNewGameButton( t2dSceneObject* const _pObject ) { m_pNewGameButton = _pObject; }
		void SetContinueButton( t2dSceneObject* const _pObject ) { m_pContinueButton = _pObject; }
		void SetMuteButton( t2dSceneObject* const _pObject ) { m_pMuteButton = _pObject; }
		void SetBackButton( t2dSceneObject* const _pObject ) { m_pBackButton = _pObject; }
		void SetGameCenterButton( t2dSceneObject* const _pObject ) { m_pGameCenterButton = _pObject; }
	
		void OpenMainMenuScreenFromMyScoresScreen();
		void OpenMainMenuScreenFromHeartScreen();
		void OpenMainMenuScreen();
		void CloseMainMenuScreen();
	
		void OpenOptionsScreen();
		void CloseOptionsScreen();
	
		void OpenCreditsScreen();
		void CloseCreditsScreen();
	
		void OpenClearDataScreen();
		void CloseClearDataScreen();
	
		bool IsFading() const { return m_bFading; }
	
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
		static CComponentMainMenuScreen* sm_pInstance;
	
		Vector<t2dSceneObject*> m_MainMenuElements;
		Vector<t2dSceneObject*> m_OptionsScreenElements;
		Vector<t2dSceneObject*> m_CreditsScreenElements;
		Vector<t2dSceneObject*> m_ClearDataScreenElements;
	
		t2dSceneObject* m_pNewGameButton;
		t2dSceneObject* m_pContinueButton;
		t2dSceneObject* m_pMuteButton;
		t2dSceneObject* m_pBackButton;
		t2dSceneObject* m_pGameCenterButton;
	
		EScreenMode m_eScreenMode;
	
		bool m_bLoadingIntoTutorial;
		bool m_bFadingToMyScoreScreen;
		bool m_bFadingToHeartScreen;
	
		bool m_bSaveDataExists;
	
		char m_szSelectedLevelName[128];
		bool m_bFading;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentMainMenuScreen& CComponentMainMenuScreen::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentMainMenuScreen::sm_pInstance == NULL" );
	return *sm_pInstance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMAINMENUSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

