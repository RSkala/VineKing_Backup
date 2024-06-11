//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTHEARTCREENELEMENT_H
#define COMPONENTHEARTCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentHeartScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
public:
	
	enum EElementType
	{
		ELEMENT_TYPE_NONE = -1,
		
		ELEMENT_TYPE_BUTTON_BACK,
		ELEMENT_TYPE_BUTTON_RATE,
		ELEMENT_TYPE_BUTTON_FACEBOOK,
		ELEMENT_TYPE_BUTTON_MORE_VINEKING,
		ELEMENT_TYPE_BUTTON_TWITTER,
	};
	
	public:
		
		DECLARE_CONOBJECT( CComponentHeartScreenElement );
		
		CComponentHeartScreenElement();
		virtual ~CComponentHeartScreenElement();
		
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
		EElementType m_eElementType;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTHEARTCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

