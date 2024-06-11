//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSHOOTERENEMYSPAWNER_H
#define COMPONENTSHOOTERENEMYSPAWNER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dAnimatedSprite;
class t2dVector;
class CComponentShooterEnemy;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentShooterEnemySpawner : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentShooterEnemySpawner );
		
		CComponentShooterEnemySpawner();
		virtual ~CComponentShooterEnemySpawner();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		// Inherited from SimComponent, but added by me
		virtual void OnPostInit();
		
	private:
		void SpawnEnemy();
		CComponentShooterEnemy* GetNextAvailableEnemy();
		
	private:
		t2dSceneObject* m_pOwnerObject;
		Vector<t2dAnimatedSprite*> m_EnemySpawnPool;
		F32 m_fEnemySpawnTimer;
	
		const char* m_pszSpawnedEnemyName;
		t2dAnimatedSprite* m_pEnemyToSpawn;
	
		F32 m_fInitialSpawnDelayTimer;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSHOOTERENEMYSPAWNER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------