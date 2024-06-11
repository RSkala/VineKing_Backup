//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLEVELSELECTBUTTON_H
#define COMPONENTLEVELSELECTBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLevelSelectButton : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentLevelSelectButton );
		
		CComponentLevelSelectButton();
		virtual ~CComponentLevelSelectButton();
	
		void DisableButton();
		void EnableButton();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		
		// Inherited from SimComponent
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseEnter( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseLeave( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
	
	private:
		void SetButtonProperties();
		
	private:
		t2dSceneObject* m_pOwnerButton;
	
		const char* m_pszLevelNumber;
		const char* m_pszSubLevelNumber;
		bool m_bIsBossLevel;
		const char* m_pszLevelName; // If this is filled out, this will take precedence over the level numbers
		char m_szLevelString[64];
	
		bool m_bIsBackButton; // If this button is a "Back" button, then it will not load a level. It returns back to the Map Select Menu.
		bool m_bIsRPGButton;  // If this button is an "RPG" button, then it will not load a level. It opens the RPG screen.
		bool m_bIsReturnToMainButton; // If this button is the "Return to Main" button, then it will not load a level.  It returns back to the Main Menu screen (hard load).
		bool m_bIsHelpButton;
	
		t2dSceneObject* m_pVisualObject; // The visual representation of the button
	
		S32 m_iActiveLevelSaveIndex; // Mark the appropriate save data slot, for when this level is completed, so it can be marked as completed properly
	
		bool m_bLocked;
		bool m_bCompleted;
	
		S32 m_iStarRanking;
	
		static bool sm_bLoadingLevel;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLEVELSELECTBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

