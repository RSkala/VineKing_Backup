//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSTORYOPENINGSCREENELEMENT_H
#define COMPONENTSTORYOPENINGSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentStoryOpeningScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EElementType 
		{
			ELEMENT_TYPE_NONE = 0,
			ELEMENT_TYPE_BUTTON_NEXT,
			ELEMENT_TYPE_BUTTON_TUTORIAL_YES,
			ELEMENT_TYPE_BUTTON_TUTORIAL_NO,
		};
	
	public:
		DECLARE_CONOBJECT( CComponentStoryOpeningScreenElement );
		
		CComponentStoryOpeningScreenElement();
		virtual ~CComponentStoryOpeningScreenElement();
	
		void Enable();
		void Disable();
	
		void EnableMouseEvents();
		void DisableMouseEvents();
		
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
		t2dStaticSprite* m_pVisualObject;
	
		EElementType m_eElementType;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSTORYOPENINGSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

