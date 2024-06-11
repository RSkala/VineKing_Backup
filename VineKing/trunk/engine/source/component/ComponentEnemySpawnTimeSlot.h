//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSPAWNTIMESLOT_H
#define COMPONENTENEMYSPAWNTIMESLOT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySpawnPosition;
class CComponentStaticPath;
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySpawnTimeSlot : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EEnemyType
		{
			ENEMY_TYPE_NONE = -1,
			ENEMY_TYPE_BLOB = 0,
			ENEMY_TYPE_SUPER_BLOB,
			ENEMY_TYPE_SERPENT,
			ENEMY_TYPE_SUPER_SERPENT,
			ENEMY_TYPE_SMOKE,
			ENEMY_TYPE_SUPER_SMOKE,
		};
	
	public:
		class CTimeSlotEntry
		{
			public:
				CTimeSlotEntry()
					: m_bUseEntry( false )
					, m_pszSpawnPoint( NULL )
					, m_pSpawnPosition( NULL )
					, m_vSpawnPoint( t2dVector::getZero() )
					, m_pszEnemyType( NULL )
					, m_eEnemyType( ENEMY_TYPE_NONE )
					, m_iNumEnemiesToSpawn( 0 )
					, m_fStaggerTime( 0.0f )
					, m_pszStaticPath( NULL )
					, m_pStaticPath( NULL )
					, m_pszSmokePosIndex( NULL )
					, m_iSmokePosIndex( -1 )
				{ }
			
			public:
				bool m_bUseEntry;								// Whether this entry is used
			
				const char* m_pszSpawnPoint;					// Use this to get the Spawn Position position object
				CComponentEnemySpawnPosition* m_pSpawnPosition; // This is used to determine whether a specific spawn point was set (If NULL, then find the next available)
			
				t2dVector m_vSpawnPoint;						// Spawn Position
			
				const char* m_pszEnemyType;						// Enemy type (used to convert the string to an enum)
				EEnemyType m_eEnemyType;						// Enemy type that will be spawned in this entry
			
				S32 m_iNumEnemiesToSpawn;						// Number of enemies to spawn
				F32 m_fStaggerTime;								// Time between spawns
			
				const char* m_pszStaticPath;					// Owner of Static Path (used only for getting the Component)
				CComponentStaticPath* m_pStaticPath;			// Static Path that the enemies will use (Blobs only)
			
				const char* m_pszSmokePosIndex;					// Enum for the Smoke Position Index
				S32 m_iSmokePosIndex;							// Index into the Smoke Position Array (CComponentEnemySmokePositionList)
			
		}; // class CTimeSlotEntry
	
	public:
		DECLARE_CONOBJECT( CComponentEnemySpawnTimeSlot );
		
		CComponentEnemySpawnTimeSlot();
		virtual ~CComponentEnemySpawnTimeSlot();
	
		//F32 GetStartTime() { return m_fSlotStartTime; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
		
	private:
		void SetUpTimeSlotEntry( CTimeSlotEntry& );
		
	//private: // Make public?
	public:
		t2dSceneObject* m_pOwner;
	
		F32 m_fSlotStartTime;
	
		static const S32 m_iMaxTimeSlotEntries = 12;
		CTimeSlotEntry m_TimeSlotEntries[ m_iMaxTimeSlotEntries ];
	
		S32 m_iNumUsedEntries;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSPAWNTIMESLOT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------