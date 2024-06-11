//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSERPENT_H
#define COMPONENTENEMYSERPENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySpawnPosition;
class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySerpent : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:

		enum ESerpentState
		{
			SERPENT_STATE_NONE = 0,
			SERPENT_STATE_SPAWNING,
			SERPENT_STATE_SPITTING,
			SERPENT_STATE_DEATH,
			SERPENT_STATE_INACTIVE,
		};
		
	public:
		DECLARE_CONOBJECT( CComponentEnemySerpent );
		
		CComponentEnemySerpent();
		
		void HandleSerpentStateNone();
		void HandleSerpentStateSpawning();
		void HandleSerpentStateDeath();
	
		void SwitchSerpentState( const ESerpentState );
		
		void Kill();
		void KillFromStomp();
		
		bool IsActorInDeathState() { return m_eSerpentState == SERPENT_STATE_DEATH; }
		
		void SpawnAtPosition( CComponentEnemySpawnPosition* const );
		void SpawnSuperAtPosition( CComponentEnemySpawnPosition* const );
	
		bool IsSpawned() const { return m_eSerpentState != SERPENT_STATE_INACTIVE; }
		
		bool IsWaitingForStomp() const { return m_bWaitingForStomp; }
	
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		void DetermineOwnerFlip();
		
		void UpdateSpitting();
		
	private:
		t2dAnimatedSprite* m_pOwner; // The object that this component is assigned to (the owner of this component)
		
		ESerpentState m_eSerpentState;
		
		bool m_bAttackedDuringThisAnim; // Whether the enemy attacked during this current animation cycle (in case the animation frame is the same)
		
		F32 m_fSpitTimer;
		
		bool m_bHasSpitAttackPosition;
		t2dVector m_vSpitAttackPosition;
	
		bool m_bIsSuperEnemy;
		F32 m_fSpitDelayTime;
	
		CComponentEnemySpawnPosition* m_pSpawnPosition;
	
		bool m_bWaitingForStomp;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSERPENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------