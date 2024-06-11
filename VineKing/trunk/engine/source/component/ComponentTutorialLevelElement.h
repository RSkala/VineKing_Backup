//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTTUTORIALLEVELELEMENT_H
#define COMPONENTTUTORIALLEVELELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentTutorialLevelElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentTutorialLevelElement );
		
		CComponentTutorialLevelElement();
		virtual ~CComponentTutorialLevelElement();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseEnter( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseLeave( const t2dVector& _vWorldMousePoint );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		enum ETutorialLevelObjectType
		{
			TUTORIAL_LEVEL_OBJECT_TYPE_NONE = 0,
			TUTORIAL_LEVEL_OBJECT_TYPE_LINEDRAW_BUTTON,
			TUTORIAL_LEVEL_OBJECT_TYPE_SEEDSUCK_BUTTON,
			TUTORIAL_LEVEL_OBJECT_TYPE_BLAVA_BUTTON,
			TUTORIAL_LEVEL_OBJECT_TYPE_VINEATTACK_BUTTON,
			TUTORIAL_LEVEL_OBJECT_TYPE_DAMAGE_BUTTON,
		};
	
	private:
		t2dStaticSprite* m_pOwner;
	
		ETutorialLevelObjectType m_eType;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTTUTORIALLEVELELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

