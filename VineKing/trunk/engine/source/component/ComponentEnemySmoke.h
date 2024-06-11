//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSMOKE_H
#define COMPONENTENEMYSMOKE_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySmoke : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
		
	public:
		DECLARE_CONOBJECT( CComponentEnemySmoke );
		
		CComponentEnemySmoke();
		virtual ~CComponentEnemySmoke();
		
		void SpawnAtPositionWithEndpoint( const t2dVector&, const t2dVector& );
		void SpawnSuperAtPositionWithEndpoint( const t2dVector&, const t2dVector& );
		void Kill();
		void KillFromStomp();
		
		bool IsSpawned() const { return m_eSmokeState != SMOKE_STATE_NONE; }
	
		bool IsWaitingForStomp() const { return m_bWaitingForStomp; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void HandleOwnerPositionTargetReached();
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		void UpdateStateSpawning();
		void UpdateStateMoving();
		void UpdateStateDying();
	
	private:
		enum ESmokeState
		{
			SMOKE_STATE_NONE = 0,
			SMOKE_STATE_SPAWNING,
			SMOKE_STATE_MOVING,
			SMOKE_STATE_DYING,
		};
	
	private:
		t2dAnimatedSprite* m_pOwner;
	
		t2dVector m_vStartPosition;
		t2dVector m_vEndPosition;
		
		bool m_bIsSuperEnemy;
		
		ESmokeState m_eSmokeState;
	
		bool m_bWaitingForStomp;
	
		bool m_bKilledBySuicide;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSMOKE_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------