//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMANABAR_H
#define COMPONENTMANABAR_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentManaBar : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
		
	public:
		DECLARE_CONOBJECT( CComponentManaBar );
		
		CComponentManaBar();
		virtual ~CComponentManaBar();
	
		static F32 GetCurrentMaxManaAmount() { return sm_pInstance ? sm_pInstance->m_fCurrentMaxManaAmount : 0.0f; }
		static F32 GetCurrentManaAmount() { return sm_pInstance ? sm_pInstance->m_fCurrentManaAmount : 0.0f; }
		static bool IsThereAnyAvailableMana() { return sm_pInstance ? sm_pInstance->m_fCurrentManaAmount > 0.0f : false; }
	
		static void AddMana( const F32& );
		static void SubtractMana( const F32& );
	
		static F32 GetPercentManaRemaining();
	
		static void OnLineDrawAttackReset();
		static void OnLineDrawLengthIncrease( F32& );
    
        static void ForceFullMana();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void ChangeVerticalSize( const F32& );
	
	private:
		t2dSceneObject* m_pManaBarObject; // The Mana Bar object will be the owner of this component
	
		const char* m_pszFrameTopName;
		t2dSceneObject* m_pFrameTop;
		
		const char* m_pszFrameCenterName;
		t2dSceneObject* m_pFrameCenter;
		
		const char* m_pszFrameBottomName;
		t2dSceneObject* m_pFrameBottom;
	
		F32 m_fCurrentMaxVerticalSize;
	
		F32 m_fCurrentManaAmount;		// The amount of Mana the player has available
		F32 m_fCurrentMaxManaAmount;	// The maximum amount of Mana the player can have (This is dependent on how much XP the player has put into Mana.)
	
		static CComponentManaBar* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMANABAR_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------