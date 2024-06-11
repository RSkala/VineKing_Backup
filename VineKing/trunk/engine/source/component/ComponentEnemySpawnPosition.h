//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSPAWNPOSITION_H
#define COMPONENTENEMYSPAWNPOSITION_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySpawnPosition : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemySpawnPosition );
		
		CComponentEnemySpawnPosition();
		virtual ~CComponentEnemySpawnPosition();
	
		void GetOwnerPosition( t2dVector& );
	
		bool IsSpawnPositionInUse() const { return m_bInUse; }
		void SetSpawnPositionInUse() { m_bInUse = true; }
		void SetSpawnPositionNotInUse() { m_bInUse = false; }
	
		static void ClearSpawnPositionList() { sm_SerpentSpawnPositionList.clear(); sm_iCurrentSpawnIndex = 0; }
	
		static S32 GetNumSerpentSpawnPositions()	{ return sm_SerpentSpawnPositionList.size(); }
	
		static CComponentEnemySpawnPosition* const GetNextAvailableSpawnPosition();
	
		static void IncrementCurrentSpawnIndex() { ++sm_iCurrentSpawnIndex; if( sm_iCurrentSpawnIndex >= sm_SerpentSpawnPositionList.size() ) sm_iCurrentSpawnIndex = 0; }
	
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnPostInit();
		
	private:
		t2dSceneObject* m_pOwner;
	
		bool m_bInUse;
	
		static S32 sm_iCurrentSpawnIndex;
		
		static Vector<CComponentEnemySpawnPosition*> sm_SerpentSpawnPositionList;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSPAWNPOSITION_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------