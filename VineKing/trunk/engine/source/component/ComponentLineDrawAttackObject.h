//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLINEDRAWATTACKOBJECT_H
#define COMPONENTLINEDRAWATTACKOBJECT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLineDrawAttackObject : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentLineDrawAttackObject );
		
		CComponentLineDrawAttackObject();
		virtual ~CComponentLineDrawAttackObject();
	
		static bool IsLineDrawAttackObjectActive();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwner;
	
		static CComponentLineDrawAttackObject* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLINEDRAWATTACKOBJECT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------