//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSEEDMAGNETBUTTON_H
#define COMPONENTSEEDMAGNETBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentSeedMagnetButton : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
		
	public:
		DECLARE_CONOBJECT( CComponentSeedMagnetButton );
		
		// Constructor
		CComponentSeedMagnetButton();
		virtual ~CComponentSeedMagnetButton();
	
		static void HideSeedMagnetButton();
        static void ShowSeedMagnetButton();
    
        static bool IsSeedMagnetButtonVisible();
        static void GetSeedMagnetButtonPosition( t2dVector &);
	
		static bool IsPressed() { return sm_pInstance ? sm_pInstance->m_bIsPressed : false; }
    
        static void DisableInput();
        static void EnableInput();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnMouseDown( const t2dVector& );
		virtual void OnMouseUp( const t2dVector& );
		virtual void OnMouseEnter( const t2dVector& );
		virtual void OnMouseLeave( const t2dVector& );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwner;
		bool m_bIsActivated;
	
		bool m_bIsPressed;
	
		static CComponentSeedMagnetButton* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSEEDMAGNETBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------