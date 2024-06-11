//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYBLOB_H
#define COMPONENTENEMYBLOB_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dAnimatedSprite;
class CComponentStaticPath;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyBlob : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	private:
		enum EBlobState
		{
			BLOB_STATE_NONE = 0,
			BLOB_STATE_SPAWN_WARNING,
			BLOB_STATE_SPAWNING,
			BLOB_STATE_MOVING,
			BLOB_STATE_ATTACKING,
			BLOB_STATE_DEATH,
			BLOB_STATE_INACTIVE,
		};
	
	public:
		DECLARE_CONOBJECT( CComponentEnemyBlob );
		
		CComponentEnemyBlob();
		
		void HandleActorStateNone();
		void HandleActorStateSpawning();
		void HandleActorStateMoving();
		void HandleMovingWithPathObject();
		void HandleMovingWithoutPathObject();
		void HandleActorStateAttacking();
		void HandleActorStateDeath();
		void SwitchBlobState( const EBlobState );
		void OnOwnerCurrentPathComplete();
		
		void Kill();
		void KillFromStomp();
		
		void SpawnWithPath( CComponentStaticPath* const );
		void SpawnSuperWithPath( CComponentStaticPath* const );
		
		bool IsSpawned() const { return m_eBlobState != BLOB_STATE_INACTIVE; }
		
		void SetOwnerPosition( const t2dVector & );
		void GetOwnerPosition( t2dVector& );
	
		bool IsWaitingForStomp() const { return m_bWaitingForStomp; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		virtual void HandleOwnerPositionTargetReached();
		
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		void DetermineOwnerFlip();
		bool PickTileAtCurrentBottomPosition( Point2I& );
		void MoveToNextPathNode();
		
	private:
		t2dAnimatedSprite* m_pOwner; // The object that this component is assigned to (the owner of this component)
		
		EBlobState m_eBlobState;
		
		Point2I m_currentTilePoint; // The logical tile position this enemy is currently at (This is currently only set when the enemy STOPS moving.)
		
		bool m_bAttackedDuringThisAnim; // Whether the enemy attacked during this current animation cycle (in case the animation frame is the same)
		
		bool m_bIsAttackingPlayer;
		
		CComponentStaticPath* m_pStaticPath;
		S32 m_iCurrentStaticPathNodeIndex;
	
		bool m_bIsSuperEnemy;
	
		bool m_bWaitingForStomp;
		
		bool m_bKilledBySuicide;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYBLOB_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------