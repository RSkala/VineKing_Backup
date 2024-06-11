//-------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSPAWNER_H
#define COMPONENTENEMYSPAWNER_H

//-------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward Declarations

class CComponentAStarActor;
class CComponentStaticPath;
class t2dAnimatedSprite;
class t2dTileLayer;

//-------------------------------------------------------------------------------------------------

class CComponentEnemySpawner : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	enum EEnemyType
	{
		ENEMY_TYPE_NONE = 0,
		ENEMY_TYPE_A,
		ENEMY_TYPE_B,
		ENEMY_TYPE_C,
		ENEMY_TYPE_D,
	};
	
	public:
		DECLARE_CONOBJECT( CComponentEnemySpawner );
		
		CComponentEnemySpawner();
	
		void SpawnEnemy();	// This is the old way using allocations
		void SpawnEnemy( CComponentAStarActor* const );	// This is the new way
	
		void DecrementSpawnCount() { AssertFatal( m_uCurrentNumSpawns != 0, "DecrementSpawnCount() - m_uCurrentNumSpawns == 0"); m_uCurrentNumSpawns--; }
	
		bool IsEnemyTypeA() const { return m_eEnemyType == ENEMY_TYPE_A; }
		bool IsEnemyTypeB() const { return m_eEnemyType == ENEMY_TYPE_B; }
		bool IsEnemyTypeC() const { return m_eEnemyType == ENEMY_TYPE_C; }
		bool IsEnemyTypeD() const { return m_eEnemyType == ENEMY_TYPE_D; }
	
		void SpawnedEnemyDeathNotify();
	
		bool IsSpawnedEnemyAlive() const { return m_bSpawnedEnemyAlive; } // This function is only meant to be used with spawners that are supposed to only have ONE enemy active at a time (like the Serpents)
	
		// Inherited from ConsoleObject
		static void initPersistFields();
	
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void ConvertEnemyTypeStringToEnum();
		void FindAndAddStaticPath( const char* );
	
	private:
		t2dSceneObject* m_pOwner;				// Owner of this component
		bool m_bSpawningEnabled;				// Whether or not to spawn enemies from this spawner
		const char* m_pszEnemyType;				// The type of enemy spawned from this spawner
		EEnemyType m_eEnemyType;
		char* m_pszObjectToSpawnName;			// Name of object that will be spawned
		t2dAnimatedSprite* m_pObjectToSpawn;	// Pointer to object that will be spawned (cloned)
		U32 m_uCurrentNumSpawns;				// Current number of spawns
	
		bool m_bSpawnedEnemyAlive;
	
		const char* m_pszStaticPath01Name;
		const char* m_pszStaticPath02Name;
		const char* m_pszStaticPath03Name;
	
		Vector<CComponentStaticPath*> m_StaticPathList; // The list of possible Static Paths that an enemy spawned can use
};

//-------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSPAWNER_H

//-------------------------------------------------------------------------------------------------
