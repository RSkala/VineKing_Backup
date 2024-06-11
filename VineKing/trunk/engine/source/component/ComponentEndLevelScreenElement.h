//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENDLEVELSCREENELEMENT_H
#define COMPONENTENDLEVELSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEndLevelScreenElement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		
		//%endLevelScreenElementTypes = "YOU_WIN" TAB "GAMEOVER" TAB "OK_BUTTON";
		enum EElementType
		{
			ELEMENT_TYPE_NONE = -1,
			ELEMENT_TYPE_YOUWIN = 0,
			ELEMENT_TYPE_GAMEOVER,
			ELEMENT_TYPE_OK_BUTTON,
		};
		
	public:
		
		DECLARE_CONOBJECT( CComponentEndLevelScreenElement );
		
		CComponentEndLevelScreenElement();
		virtual ~CComponentEndLevelScreenElement();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint ); // NO LONGER USED
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

#endif // COMPONENTENDLEVELSCREENELEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

