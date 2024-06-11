//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTWINLEVEL_H
#define COMPONENTWINLEVEL_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentWinLevel : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
		
	public:
		DECLARE_CONOBJECT( CComponentWinLevel );
	
		CComponentWinLevel();
		virtual ~CComponentWinLevel();
	
		static inline CComponentWinLevel& GetInstance();
	
		void StartWinLevelSequence();
	
		void StartGameOverSequence();
	
		void ReturnToTitleScreen();
	
		bool HasLevelEnded() const { return m_bLevelHasEnded; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwnerOKButtonObject; // This will be the OK button, so we can receive the mouse events
	
		const char* m_pszYouWinObject;
		t2dSceneObject* m_pYouWinObject;
	
		const char* m_pszGameOverObject;
		t2dSceneObject* m_pGameOverObject;
	
		F32 m_fOKButtonPressedTimer;
		bool m_bOKButtonPressed;		// Set to true after the player pushes in the OK button
		bool m_bOKButtonUnpressed;		// Set to true after the OK button automatically pops back out
		bool m_bReturningToTileScreen;
	
		bool m_bLevelHasEnded;
		bool m_bLevelLost;
	
		static CComponentWinLevel* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentWinLevel& CComponentWinLevel::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentWinLevel::sm_pInstance == NULL" );
	return *sm_pInstance;
}


#endif // COMPONENTWINLEVEL_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

