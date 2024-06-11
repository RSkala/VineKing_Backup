//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTHEALTHBAR_H
#define COMPONENTHEALTHBAR_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentHealthBar : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentHealthBar );
		
		CComponentHealthBar();
		virtual ~CComponentHealthBar() { CComponentHealthBar::sm_pInstance = NULL; }
	
		static void NotifyHomeBaseHealthChange();
	
		static void InitializeHealthBar();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
		
	private:
		void SetUpHealthBar();
		void ChangeHorizontalSize( const F32& );
		
	private:
		t2dSceneObject* m_pHealthBarObject; // The Health Bar object will be the owner of this component
	
		const char* m_pszFrameLeftName;
		t2dSceneObject* m_pFrameLeft;
		
		const char* m_pszFrameCenterName;
		t2dSceneObject* m_pFrameCenter;
		
		const char* m_pszFrameRightName;
		t2dSceneObject* m_pFrameRight;

		F32 m_fCurrentMaxHorizontalSize;
	
		static CComponentHealthBar* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTHEALTHBAR_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------