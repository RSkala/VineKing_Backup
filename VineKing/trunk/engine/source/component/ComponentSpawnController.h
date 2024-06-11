//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSPAWNCONTROLLER_H
#define COMPONENTSPAWNCONTROLLER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentAStarActor;
class CComponentEnemySpawner;
class CComponentSpawnController;
class t2dSceneObject;
class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CTimeSlotInfo
{
	friend class CComponentSpawnController;
	
	public:
		CTimeSlotInfo();
		
	private:
		bool m_bUseTimeSlot;				// Whether to use this Time Slot
		F32 m_fSlotStartTimeSeconds;		// The time at which this Time Slot is used
		F32 m_fStaggeredSpawnTimeSeconds;	// Amount of time between individual spawns
		S32	m_iMaxNumSpawnsEnemyA;				// Max number of enemy A at one time
		S32 m_iMaxNumSpawnsEnemyB;				// Max number of enemy B at one time
		S32 m_iMaxNumSpawnsEnemyC;				// Max number of enemy C at one time
		S32 m_iMaxTotalSpawns;				// Total number of spawns at a time
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentSpawnController : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentSpawnController );
		
		CComponentSpawnController();
		virtual ~CComponentSpawnController();
	
		static inline CComponentSpawnController& GetInstance();
	
		void AddEnemyASpawner( CComponentEnemySpawner* const );
		void AddEnemyBSpawner( CComponentEnemySpawner* const );
		void AddEnemyCSpawner( CComponentEnemySpawner* const );
	
		void DecrementSpawnedEnemyACount() { if( m_iCurrentEnemyASpawns > 0 ) --m_iCurrentEnemyASpawns; /*if( m_iNumSpawnsThisTimeSlot_A > 0 ) --m_iNumSpawnsThisTimeSlot_A;*/ }
		void DecrementSpawnedEnemyBCount() { if( m_iCurrentEnemyBSpawns > 0 ) --m_iCurrentEnemyBSpawns; /*if( m_iNumSpawnsThisTimeSlot_B > 0 ) --m_iNumSpawnsThisTimeSlot_B;*/ }
		void DecrementSpawnedEnemyCCount() { if( m_iCurrentEnemyCSpawns > 0 ) --m_iCurrentEnemyCSpawns; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		static void InitializeEnemyPools();
		t2dAnimatedSprite* GetEnemyBlobTemplateObject() const { return m_pEnemyBlobTemplateObject; }
		t2dAnimatedSprite* GetEnemySerpentTemplateObject() const { return m_pEnemySerpentTemplateObject; }
		void UpdateSpawning();
		void HandleBlobSpawning();
		void HandleSerpentSpawning();
		void CheckTimeSlotIncrement();
	
		CComponentAStarActor* GetNextAvailableBlobEnemy();
		CComponentAStarActor* GetNextAvailableSerpentEnemy();
		CComponentEnemySpawner* GetNextAvailableSerpentSpawner();
	
		////////////////////////////////////////////////////////////////////////////
		// NEW SHIT 2010/01/03
		void UpdateBlobSpawning();
		void UpdateSerpentSpawning();
		////////////////////////////////////////////////////////////////////////////
		
	private:
		t2dSceneObject* m_pOwner;
		
		static CComponentSpawnController* sm_pInstance;
	
		static const U32 m_uMaxTimeSlots = 12;
	
		CTimeSlotInfo m_TimeSlots[ m_uMaxTimeSlots ];
	
		F32 m_fTimeSlotTimer;
		F32 m_fStaggeredSpawnTimer;
		S32 m_iCurrentTimeSlotIndex;
		S32 m_iNumUsedTimeSlots;
		bool m_bAtLastTimeSlot;
	
		Vector<CComponentEnemySpawner*> m_EnemySpawnerAList;
		Vector<CComponentEnemySpawner*> m_EnemySpawnerBList;
		Vector<CComponentEnemySpawner*> m_EnemySpawnerCList;
	
		S32 m_iCurrentEnemySpawnerAIndex;
		S32 m_iCurrentEnemySpawnerBIndex;
		S32 m_iCurrentEnemySpawnerCIndex;
	
		S32 m_iCurrentEnemyASpawns;
		S32 m_iCurrentEnemyBSpawns;
		S32 m_iCurrentEnemyCSpawns;
	
		S32 m_iNumSpawnsThisTimeSlot_A;
		S32 m_iNumSpawnsThisTimeSlot_B;
		S32 m_iNumSpawnsThisTimeSlot_Count;
	
		// Enemy Pool members
		const char* m_pszEnemyBlobTemplate;
		t2dAnimatedSprite* m_pEnemyBlobTemplateObject;
	
		const char* m_pszEnemySerpentTemplate;
		t2dAnimatedSprite* m_pEnemySerpentTemplateObject;
	
		static Vector<t2dAnimatedSprite*> m_EnemyBlobPool;
		static Vector<CComponentAStarActor*> m_EnemyBlobComponentPool;
	
		static Vector<t2dAnimatedSprite*> m_EnemySerpentPool;
		static Vector<CComponentAStarActor*> m_EnemySerpentComponentPool;
	
	
		////////////////////////////////////////////////////////////////////////////
		// NEW SHIT 2010/01/03
		F32 m_fBlobSpawnTimer;
		F32 m_fStaggeredBlobSpawnTimer;
	
		F32 m_fSerpentSpawnTimer;
		F32 m_fStaggeredSerpentSpawnTimer;
	
		F32 m_fNextTimeSlotStartTime;
		F32 m_fFinalTimeSlotTimeDifference; // Amount of time between the time slot at (n) and the time slot at (n-1).
		////////////////////////////////////////////////////////////////////////////
	
		////////////////////////////////////////////////////////////////////////////
		// NEW SHIT 2011/03/10
		bool m_bSpawnBlob; // This is to intermittently spawn Blob->Serpent->Blob->Serpent->... etc.
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentSpawnController& CComponentSpawnController::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentSpawnController::sm_pInstance == NULL" );
	return *sm_pInstance;
}

#endif // COMPONENTSPAWNCONTROLLER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------