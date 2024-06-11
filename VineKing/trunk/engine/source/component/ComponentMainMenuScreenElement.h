//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMAINMENUSCREENELEMENT_H
#define COMPONENTMAINMENUSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMainMenuScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		
		enum EElementType
		{
			ELEMENT_TYPE_NONE = -1,
			ELEMENT_TYPE_MAINMENUSCREEN_BG = 0,
			ELEMENT_TYPE_OPTIONSSCREEN_BG,
			ELEMENT_TYPE_CREDITSSCREEN_BG,	
			ELEMENT_TYPE_CLEARDATASCREEN_BG,	
			ELEMENT_TYPE_NEWGAME_TEXT,
			ELEMENT_TYPE_CONTINUE_TEXT,		
			ELEMENT_TYPE_TUTORIAL_TEXT,		
			ELEMENT_TYPE_OPTIONS_TEXT,	
			ELEMENT_TYPE_CLEARDATA_TEXT,		
			ELEMENT_TYPE_CREDITS_TEXT,	
			ELEMENT_TYPE_CLEARDATA_YES_TEXT,
			ELEMENT_TYPE_CLEARDATA_NO_TEXT,
			ELEMENT_TYPE_MUTE_BUTTON,
			ELEMENT_TYPE_BACK_BUTTON,
			
			ELEMENT_TYPE_GAME_CENTER_BUTTON,
			ELEMENT_TYPE_GAMECENTER_RESET_ACHIEVEMENTS_BUTTON,
			
			ELEMENT_TYPE_CREDITS_NEXT_BUTTON,
			
			ELEMENT_TYPE_MY_SCORES_BUTTON,
			ELEMENT_TYPE_HEART_BUTTON,
			
			ELEMENT_TYPE_FACEBOOK_LOGIN_BUTTON,
		};
		
	public:
		
		DECLARE_CONOBJECT( CComponentMainMenuScreenElement );
		
		CComponentMainMenuScreenElement();
		virtual ~CComponentMainMenuScreenElement();
		
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
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMAINMENUSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

