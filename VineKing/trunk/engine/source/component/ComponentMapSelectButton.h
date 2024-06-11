//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class CComponentMapSelection;
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMapSelectButton : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentMapSelectButton );
		
		CComponentMapSelectButton();
		virtual ~CComponentMapSelectButton();
	
		void DisableInput();
		void EnableInput();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseEnter( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseLeave( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwner;
		
		const char* m_pszMapObjectName;
		CComponentMapSelection* m_pMapSelection;
	
		// This is the actual graphic image displayed that the user thinks is the button. It is separate from the button in order to allow the button to be larger without any scaling.
		// The name of the visual object will be "<ownername>_Image". This is so the editor values don't need to be set up.
		t2dSceneObject* m_pVisualObject;
	
		bool m_bLocked;
		bool m_bCompleted;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

