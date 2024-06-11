//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMYSCORESCREENELEMENT_H
#define COMPONENTMYSCORESCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMyScoreScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		
		enum EElementType
		{
			ELEMENT_TYPE_NONE = -1,
			
			ELEMENT_TYPE_BUTTON_LEFT,
			ELEMENT_TYPE_BUTTON_RIGHT,
			ELEMENT_TYPE_BUTTON_BACK,
			ELEMENT_TYPE_BUTTON_LEADERBOARD,
			ELEMENT_TYPE_BUTTON_ACHIEVEMENTS,
			ELEMENT_TYPE_BUTTON_FACEBOOK,
            ELEMENT_TYPE_BUTTON_TWITTER,
		};
		
	public:
		
		DECLARE_CONOBJECT( CComponentMyScoreScreenElement );
		
		CComponentMyScoreScreenElement();
		virtual ~CComponentMyScoreScreenElement();
		
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

#endif // COMPONENTMYSCORESCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

