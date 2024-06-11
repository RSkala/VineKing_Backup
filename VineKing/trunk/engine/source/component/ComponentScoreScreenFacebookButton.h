//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSCORESCREENFACEBOOKBUTTON_H
#define COMPONENTSCORESCREENFACEBOOKBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentScoreScreenFacebookButton : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentScoreScreenFacebookButton );
		
		// Constructor
		CComponentScoreScreenFacebookButton();
		virtual ~CComponentScoreScreenFacebookButton();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnMouseDown( const t2dVector& );
		virtual void OnMouseUp( const t2dVector& );
		virtual void OnMouseEnter( const t2dVector& );
		virtual void OnMouseLeave( const t2dVector& );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwner;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSCORESCREENFACEBOOKBUTTON_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------