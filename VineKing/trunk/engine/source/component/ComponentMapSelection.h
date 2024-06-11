//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMAPSELECTION_H
#define COMPONENTMAPSELECTION_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLevelSelectButton;
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentMapSelection : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentMapSelection );
		
		CComponentMapSelection();
		virtual ~CComponentMapSelection();
	
		void SetMapSelectionActive();
		void SetMapSelectionInactive();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void AddLevelSelectButtonFromObjectName( const char* );
		
	private:
		t2dSceneObject* m_pOwner;
	
		const char* m_pszLevelSelectButton01;
		const char* m_pszLevelSelectButton02;
		const char* m_pszLevelSelectButton03;
		const char* m_pszLevelSelectButton04;
	
		Vector<CComponentLevelSelectButton*> m_LevelSelectButtonList;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMAPSELECTION_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

