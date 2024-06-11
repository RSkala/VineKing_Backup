//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSTARTGAMEBUTTON_H
#define COMPONENTSTARTGAMEBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentStartGameButton : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentStartGameButton );
		
		CComponentStartGameButton();
		virtual ~CComponentStartGameButton();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		// Inherited from SimComponent, but added by me
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwnerButton;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSTARTGAMEBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

