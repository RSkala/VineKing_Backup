//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYMANAEATERSPAWNER_H
#define COMPONENTENEMYMANAEATERSPAWNER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyManaEater;
class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyManaEaterSpawner : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemyManaEaterSpawner );
		
		// Constructor
		CComponentEnemyManaEaterSpawner();
		virtual ~CComponentEnemyManaEaterSpawner();
	
		static void Initialize();
		static void InitializeEnemyPool();
	
		static bool ShouldEnemyUseSpawnAnim() { return sm_pInstance ? sm_pInstance->m_bUseSpawnAnim : false; }
	
		static F32 GetMovementSpeedNormal() { return sm_pInstance ? sm_pInstance->m_fMovementSpeedNormal : 64.0f; }
		static F32 GetMovementSpeedSuper() { return sm_pInstance ? sm_pInstance->m_fMovementSpeedSuper : 64.0f; }
	
		static F32 GetAttackPowerNormal() { return sm_pInstance ? sm_pInstance->m_fAttackPowerNormal : 50.0f; }
		static F32 GetAttackPowerSuper() { return sm_pInstance ? sm_pInstance->m_fAttackPowerSuper : 50.0f; }
	
		static void PlaySmokeHitEffectAtPosition( const t2dVector& );
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		// These were added to SimComponent by me and are inherited from that class
		virtual void OnPostInit();
	
	private:
		void UpdateSpawning();
		void CheckTimeSlotIncrement();
		CComponentEnemyManaEater* GetNextAvailableEnemy() const;
	
	private:
		class CTimeSlotInfo
		{
			public:
				CTimeSlotInfo();
				
			public:
				bool m_bUseTimeSlot;				// Whether this time slot is used
				F32 m_fSlotStartTimeSeconds;		// The time at which this Time Slot becomes active
				F32 m_fStaggeredSpawnTimeSeconds;	// Amount of time between individual spawns
				S32 m_iMaxNumSpawns;				// Total number of spawns that happen during this time slot
				S32 m_iPercentChanceSuper;			// Percent chance this will be a Super enemy
		};
		
	private:
		t2dSceneObject* m_pOwner;
	
		static Vector<t2dAnimatedSprite*> m_EnemyPool;
		static Vector<CComponentEnemyManaEater*> m_EnemyComponentPool;
	
	
		//F32 m_fSpawnTimer;
		F32 m_fTimeSlotTimer;
		S32 m_iNumEnemiesSpawnedThisTimeSlot;
		F32 m_fStaggeredSpawnTimer;
		S32 m_iCurrentTimeSlotIndex;
		S32 m_iNumUsedTimeSlots;
		bool m_bAtLastTimeSlot;
		F32 m_fNextTimeSlotStartTime;
		F32 m_fFinalTimeSlotTimeDifference; // Amount of time between the time slot at (n) and the time slot at (n-1).
	
	
		S32 m_iCurrentSpawnPositionIndex;
		
		bool m_bUseSpawnAnim;
	
		F32 m_fMovementSpeedNormal;
		F32 m_fMovementSpeedSuper;
	
		F32 m_fAttackPowerNormal;
		F32 m_fAttackPowerSuper;
	
		static const U32 m_uMaxTimeSlots = 12;
		CTimeSlotInfo m_TimeSlots[ m_uMaxTimeSlots ];
	
		Vector<F32> m_vSpawnPosition01;
		Vector<F32> m_vSpawnPosition02;
		Vector<F32> m_vSpawnPosition03;
		Vector<F32> m_vSpawnPosition04;
		Vector<F32> m_vSpawnPosition05;
		Vector<F32> m_vSpawnPosition06;
		Vector<F32> m_vSpawnPosition07;
		Vector<F32> m_vSpawnPosition08;
	
		Vector<F32> m_vEndPosition01;
		Vector<F32> m_vEndPosition02;
		Vector<F32> m_vEndPosition03;
		Vector<F32> m_vEndPosition04;
		Vector<F32> m_vEndPosition05;
		Vector<F32> m_vEndPosition06;
		Vector<F32> m_vEndPosition07;
		Vector<F32> m_vEndPosition08;
	
		Vector<t2dVector> m_SpawnPositionList;
		Vector<t2dVector> m_EndPositionList;
	
		Vector<t2dAnimatedSprite*> m_SmokeHitEffectList;
		S32 m_iCurrentSmokeHitEffectIndex;
	
		static CComponentEnemyManaEaterSpawner* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYMANAEATERSPAWNER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------