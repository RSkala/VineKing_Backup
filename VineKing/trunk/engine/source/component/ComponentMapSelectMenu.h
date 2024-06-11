//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMAPSELECTMENU_H
#define COMPONENTMAPSELECTMENU_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class CComponentMapSelectButton;
class CComponentMapSelection;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMapSelectMenu : public DynamicConsoleMethodComponent
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
		enum EFadeToState
		{
			FADE_TO_NONE = 0,
			FADE_TO_RPG_SCREEN,
			FADE_TO_MAIN_MENU,
			FADE_TO_SELECTED_LEVEL
		};
	
	public:
		DECLARE_CONOBJECT( CComponentMapSelectMenu );
		
		CComponentMapSelectMenu();
		virtual ~CComponentMapSelectMenu();
	
		static inline CComponentMapSelectMenu& GetInstance();
	
		void AddMapSelectButton( CComponentMapSelectButton* const );
		void OnMapSelectButtonPressed();
		void OnLevel11MapSelectButtonPressed();
	
		void AddMapSelection( CComponentMapSelection* const );
	
		void OnMapSelectionBackButtonPressed();
		void OnMapSelectionRPGButtonPressed();
		void OnReturnToMainMenuButtonPressed();
		void OnLevelSelectButtonPressed( const char* );
	
		void OnHelpButtonPressed();
	
		//void OnLevelUpScreenClosed();
		
		void OpenMapSelectScreen();
	
		static bool IsInMapSelectMenu() { return sm_pInstance != NULL; }
	
		void EnableMapScreenButtonsFromTutorial();
	
		void SetMapHelpArrowPositionObject( const t2dSceneObject*, const S32& );
		
		void HideHelpArrowFromMapSelectionPressed();
		void ShowHelpArrowFromMapSelectionBackPressed();
	
		void AddMapHelpArrowToLevelSelectButton( const t2dSceneObject* );
		void RemoveMapHelpArrowFromLevelSelectButton( const t2dSceneObject* );
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void EnableMapScreenButtons();
		void DisableAllMapScreenButtons();
	
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
		
	private:
		t2dSceneObject* m_pOwner;
	
		const char* m_pszGrayBoxObjectName;
		t2dSceneObject* m_pGrayBox; // Box that is displayed over the main island map (but beneath the currently selected island map).
	
		const char* m_pszBackButtonObjectName;
		t2dSceneObject* m_pBackButton; // RKS TODO: Get the component from this
	
		const char* m_pszRPGButtonObjectName;
		t2dSceneObject* m_pRPGButton; // Opens the RPG screen
	
		t2dSceneObject* m_pHelpButton;
	
		t2dSceneObject* m_pReturnToMainButton;
	
		Vector<CComponentMapSelectButton*> m_MapSelectButtonList;
		
		Vector<CComponentMapSelection*> m_MapSelectionList;
	
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
	
		EFadeToState m_eFadeToState;
	
		char m_szSelectedLevelName[128];
	
		static CComponentMapSelectMenu* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentMapSelectMenu& CComponentMapSelectMenu::GetInstance()
{
	AssertFatal( CComponentMapSelectMenu::sm_pInstance != NULL, "CComponentMapSelectMenu::sm_pInstance == NULL" );
	return *CComponentMapSelectMenu::sm_pInstance; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMAPSELECTMENU_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

