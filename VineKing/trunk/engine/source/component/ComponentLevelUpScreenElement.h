//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLEVELUPSCREENELEMENT_H
#define COMPONENTLEVELUPSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLevelUpScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
	
		enum EElementType 
		{
			ELEMENT_TYPE_NONE = -1,
			ELEMENT_TYPE_BACK_BUTTON,
			ELEMENT_TYPE_HELP_BUTTON,
			ELEMENT_TYPE_BUY_BUTTON,
			ELEMENT_TYPE_GREEN_BUTTON,
			ELEMENT_TYPE_BLUE_BUTTON,
			ELEMENT_TYPE_PURPLE_BUTTON,
			ELEMENT_TYPE_TOTAL_XP_TEXT,
		};
		//%levelUpScreenElementTypes = "Back Button" TAB "Background" TAB "Health" TAB "Mana" TAB "Speed" TAB "Total XP";
		//enum EElementType
		//{
		//	ELEMENT_TYPE_NONE = -1,
		//	ELEMENT_TYPE_BACK_BUTTON = 0,
		//	ELEMENT_TYPE_BACKGROUND,
		//	ELEMENT_TYPE_HEALTH,
		//	ELEMENT_TYPE_MANA,
		//	ELEMENT_TYPE_SPEED,
		//	ELEMENT_TYPE_TOTALXP_BG,
		//	ELEMENT_TYPE_TOTALXP_TEXT,
		//	ELEMENT_TYPE_HEALTH_BUTTON,
		//	ELEMENT_TYPE_MANA_BUTTON,
		//	ELEMENT_TYPE_SPEED_BUTTON,
		//	ELEMENT_TYPE_HEALTH_BAR_POSITION_MARKER,
		//	ELEMENT_TYPE_SPEED_BAR_POSITION_MARKER,
		//	ELEMENT_TYPE_MANA_BAR_POSITION_MARKER,
		//};
	
	public:
		DECLARE_CONOBJECT( CComponentLevelUpScreenElement );
		
		CComponentLevelUpScreenElement();
		virtual ~CComponentLevelUpScreenElement();
	
		void EnableElement();
		void DisableElement();
		
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
		void OnBackButtonPressed();
		void OnHealthButtonPressed();
		void OnSpeedButtonPressed();
		void OnManaButtonPressed();
		
	private:
		t2dSceneObject* m_pOwner;
	
		const char* m_pszElementType;
		EElementType m_eElementType;
	
		bool m_bPressed;
		static bool sm_bPressed;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLEVELUPSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

