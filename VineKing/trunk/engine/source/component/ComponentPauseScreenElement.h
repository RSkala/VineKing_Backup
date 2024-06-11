//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTPAUSESCREENELEMENT_H
#define COMPONENTPAUSESCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;
class t2dTextObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentPauseScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
	
		//%pauseScreenElementTypes = "PAUSE_MENU_IMAGE" TAB "CONFIRM_QUIT_MENU_IMAGE" TAB "PAUSE_BUTTON" TAB "RESUME_BUTTON" TAB "MUTE_BUTTON" TAB "QUIT_BUTTON" 
		//								TAB "BUTTON_PRESSED_IMAGE" TAB "MUTE_ONOFF_IMAGE" TAB "CONFIRM_QUIT_BUTTON_YES" TAB "CONFIRM_QUIT_BUTTON_NO"
		//								TAB "RESUME_PRESSED_IMAGE" TAB "QUIT_PRESSED_IMAGE" TAB "CONFIRM_YES_PRESSED_IMAGE" TAB "CONFIRM_NO_PRESSED_IMAGE"
		//								TAB "RESUME_TEXT" TAB "MUTE_TEXT" TAB "QUIT_TEXT" TAB "QUIT_CONFIRM_TEXT_1" TAB "QUIT_CONFIRM_TEXT_2"
		//								TAB "CONFIRM_YES_TEXT" TAB "QUIT_CONFIRM_NO_TEXT";
	
		enum EElementType
		{
			ELEMENT_TYPE_NONE = -1,
			ELEMENT_TYPE_PAUSE_MENU_IMAGE = 0,
			ELEMENT_TYPE_CONFIRM_QUIT_MENU_IMAGE,
			ELEMENT_TYPE_PAUSE_BUTTON,
			ELEMENT_TYPE_RESUME_BUTTON,
			ELEMENT_TYPE_MUTE_BUTTON,
			ELEMENT_TYPE_QUIT_BUTTON,
			ELEMENT_TYPE_BUTTON_PRESSED_IMAGE,
			ELEMENT_TYPE_MUTE_ONOFF_IMAGE,
			ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES,
			ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO,
			ELEMENT_TYPE_RESUME_PRESSED_IMAGE,
			ELEMENT_TYPE_QUIT_PRESSED_IMAGE,
			ELEMENT_TYPE_CONFIRM_YES_PRESSED_IMAGE,
			ELEMENT_TYPE_CONFIRM_NO_PRESSED_IMAGE,
			
			// NEW
			ELEMENT_TYPE_RESUME_TEXT,
			ELEMENT_TYPE_MUTE_TEXT,
			ELEMENT_TYPE_QUIT_TEXT,
			ELEMENT_TYPE_QUIT_CONFIRM_TEXT_1,
			ELEMENT_TYPE_QUIT_CONFIRM_TEXT_2,
			ELEMENT_TYPE_QUIT_CONFIRM_YES_TEXT,
			ELEMENT_TYPE_QUIT_CONFIRM_NO_TEXT,
			
			// NEW - 2011/01/02
			//ELEMENT_TYPE_PAUSE_SCREEN_BG, // This will be just the owner
			ELEMENT_TYPE_RESUME_BUTTON_NEW,
			ELEMENT_TYPE_MAP_BUTTON_NEW,
			ELEMENT_TYPE_MUTE_BUTTON_NEW,
			
			ELEMENT_TYPE_CONFIRM_QUIT_BG_NEW,
			ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW,
			ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW,
		};
	
	public:
	
		DECLARE_CONOBJECT( CComponentPauseScreenElement );
		
		CComponentPauseScreenElement();
		virtual ~CComponentPauseScreenElement();
		
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
		void ConvertElementTypeStringToEnum();
		
	private:
		t2dSceneObject* m_pOwner;
		const char* m_pszElementType;
		EElementType m_eElementType;
	
		bool m_bAllowPressedEvent;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTPAUSESCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

