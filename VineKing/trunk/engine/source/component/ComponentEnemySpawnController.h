//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSPAWNCONTROLLER_H
#define COMPONENTENEMYSPAWNCONTROLLER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyBlob;
class CComponentEnemySerpent;
class CComponentEnemySmoke;
class CComponentEnemySpawnTimeSlot;
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySpawnController : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemySpawnController );
		
		CComponentEnemySpawnController();
		virtual ~CComponentEnemySpawnController();
	
		void ResetTimeSlotEntryArrays();
	
		static void KillAllActiveEnemiesFromStomp();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void InitializeEnemyPools();
		void UpdateSpawning();
		void SpawnEnemy( const S32&, const S32& );
		void CheckTimeSlotIncrement();
	
		CComponentEnemyBlob*	GetNextAvailableEnemyBlob()	const;
		CComponentEnemySerpent* GetNextAvailableEnemySerpent() const;
		CComponentEnemySmoke*	GetNextAvailableEnemySmoke() const;
	
	public:
		static const S32 m_iMaxTimeSlots = 16;
	
	private:
		enum ELoopBehaviorType
		{
			LOOP_BEHAVIOR_TYPE_NONE = 0,
			LOOP_BEHAVIOR_TYPE_START_SLOT,
			LOOP_BEHAVIOR_TYPE_LAST_SLOT,
		};
	
	private:
		t2dSceneObject* m_pOwner;
	
		bool m_bUseTimeSlotArray[ m_iMaxTimeSlots ];
		Vector<CComponentEnemySpawnTimeSlot*> m_TimeSlots;
	
		const char* m_pszLoopBehavior;
		ELoopBehaviorType m_eLoopBehaviorType;
	
		F32 m_fTimeSlotTimer;
	
		// These arrays are for keeping track of entry values in the CURRENT Time Slot. These values must be reset when the Time Slots change
		static const S32 m_iMaxTimeSlotEntries = 12; // This must match the member in CComponentEnemySpawnTimeSlot
		F32 m_fEntryStaggerTimerArray[ m_iMaxTimeSlotEntries ];
		S32 m_iEntrySpawnCountArray[ m_iMaxTimeSlotEntries ];
		S32 m_iEntryCurrentSpawnPointIndexArray[ m_iMaxTimeSlotEntries ]; // Used when iterating across spawn points. NOTE: Care must be taken not to use an invalid index, since this is shared.

	
		F32 m_fNextTimeSlotStartTime;
		F32 m_fFinalTimeSlotTimeDifference;
	
		S32 m_iCurrentTimeSlotIndex;
		//S32 m_iCurrentSpawnIndexBlob;
		//S32 m_iCurrentSpawnIndexSerpent;
		//S32 m_iCurrentSpawnIndexSmoke;
	
		S32 m_iNumUsedTimeSlots;
		bool m_bAtLastTimeSlot;
	
		Vector<CComponentEnemyBlob*>	m_EnemyBlobPool;
		Vector<CComponentEnemySerpent*> m_EnemySerpentPool;
		Vector<CComponentEnemySmoke*>	m_EnemySmokePool;
	
		static CComponentEnemySpawnController* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSPAWNCONTROLLER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------