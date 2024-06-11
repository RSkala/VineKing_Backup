//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTASTARACTOR_H
#define COMPONENTASTARACTOR_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dAnimatedSprite;
class t2dTileLayer;
class CComponentEnemySpawner;
class CComponentStaticPath;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentAStarActor : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EAStarActorState
		{
			ASTAR_ACTOR_STATE_NONE = 0,
			ASTAR_ACTOR_STATE_SPAWN_WARNING,
			ASTAR_ACTOR_STATE_SPAWNING,
			ASTAR_ACTOR_STATE_MOVING,
			ASTAR_ACTOR_STATE_ATTACKING,
			ASTAR_ACTOR_STATE_DEATH,
			ASTAR_ACTOR_STATE_INACTIVE,
		};
		
		enum EAttackVulnerabilityType
		{
			VULNERABILITY_TYPE_NONE = 0,
			VULNERABILITY_TYPE_TAP,
			VULNERABILITY_TYPE_SWIPE,
			VULNERABILITY_TYPE_TAPTHENSWIPE,
			VULNERABILITY_TYPE_TAPTHENSHAKE,		
		};
	
		enum EAttackBehaviorType
		{
			ATTACK_BEHAVIOR_TYPE_NONE = 0,
			ATTACK_BEHAVIOR_TYPE_MOVE_TO_NEXT_TILE,		 // Enemy A and B: Move to adjacent tile and attack the tiles one by one (up or down then right or left).
			ATTACK_BEHAVIOR_TYPE_MOVE_DIRECTLY_TO_PLAYER, // Enemy C: Move directly to player without stopping. Destroy each tile on the way.
			ATTACK_BEHAVIOR_TYPE_FLY_TO_PLAYER,			 // Enemy D: Fly directly to enemy, occupying a space 3-6 tiles away. Then what???????
			ATTACK_BEHAVIOR_TYPE_STATIONARY,
		};
	
		enum EMovementDirection
		{
			MOVEMENT_DIRECTION_NONE = 0,
			MOVEMENT_DIRECTION_RIGHT,
			MOVEMENT_DIRECTION_LEFT,
			MOVEMENT_DIRECTION_UP,
			MOVEMENT_DIRECTION_DOWN,
		};
	
	public:
		DECLARE_CONOBJECT( CComponentAStarActor );
		
		CComponentAStarActor();
		
		void SetOwnerSpawnedByComponent( CComponentEnemySpawner* _pComponent ) { m_pSpawnedByComponent = _pComponent; }
		void HandleActorStateNone();
		void HandleActorStateSpawnWarning();
		void HandleActorStateSpawning();
		void HandleActorStateMoving();
		void HandleMovingWithPathObject();
		void HandleMovingWithoutPathObject();
		void HandleActorStateAttacking();
		void HandleActorStateDeath();
		void SwitchAStarActorState( const EAStarActorState _eAStarActorState );// { m_ePreviousActorState = m_eAStarActorState; m_eAStarActorState = _eAStarActorState; }
		void OnOwnerCurrentPathComplete();
	
		void KillActor();
	
		EAttackVulnerabilityType GetAttackVulnerabilityType() const { return m_eAttackVulnerabilityType; }
	
		bool IsActorInDeathState() { return m_eAStarActorState == ASTAR_ACTOR_STATE_DEATH; }
	
		void SpawnWithPath( CComponentStaticPath* const );
		void SpawnWithoutPath();
	
		void NotifySpawnerOfDeath();
	
		bool IsSpawned() const { return m_eAStarActorState != ASTAR_ACTOR_STATE_INACTIVE; }
	
		void SetOwnerPosition( const t2dVector & );
		void GetOwnerPosition( t2dVector& );
	
		static void ClearLastSpawnedWaveEnemy() { sm_pLastSpawnedWaveEnemy = NULL; }
		static void SetLastSpawnedWaveEnemy( CComponentAStarActor* const _pObject ) { sm_pLastSpawnedWaveEnemy = _pObject; }
		
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
		void ConvertStringAttackVulnerabilityToEnum();
		void ConvertStringAttackBehaviorToEnum();
		void DetermineOwnerFlip();
		void DeterminePath();
	
		bool CheckEnemyDirectionLeftOrRight( const Point2I&, const Point2I&, Point2I& );	// Return value true if direction was set
		bool CheckEnemyDirectionAboveOrBelow( const Point2I&, const Point2I&, Point2I& );	// Return value true if direction was set
	
		bool PickTileAtCurrentBottomPosition( Point2I& );
	
		void MoveToNextPathNode();
		void UpdateSpitting();
		
	private:
		t2dAnimatedSprite* m_pAnimatedSpriteOwner; // The object that this component is assigned to (the owner of this component)
		const char* m_pszTileLayerName; // Name of the t2dTileLayer object in the level that will be used as an AStar grid
		t2dTileLayer* m_pTileLayer; // The Tile Layer that the Path Grid uses
		
		const char* m_pszSpawnAnimName;		// Name of the animation played when spawning
		const char* m_pszMoveAnimName;		// Name of the animation played when moving
		const char* m_pszAttackAnimName;	// Name of the animation played when attacking / eating
		const char* m_pszDeathAnimName;		// Name of the animation played when dying
		const char* m_pszStunAnimName;		// Name of the animation played when stunned
		
		SimObjectId m_uCurrentPathID; // The current path ID the actor is traveling -- This is actually the Torque Object ID. What I need is the Path Node Index
		U32 m_uCurrentPathNodeIndex; // The index of the current path node the actor is traveling to.
		
		CComponentEnemySpawner* m_pSpawnedByComponent; // This is the component that spawned this AStar actor. We need this so we are able to decrement the number of spawns from the component.
		
		EAStarActorState m_eAStarActorState;
	
		const char* m_pszAttackVulnerabilityType;
		EAttackVulnerabilityType m_eAttackVulnerabilityType;
	
		const char* m_pszAttackBehaviorType;
		EAttackBehaviorType m_eAttackBehaviorType;
	
		EAStarActorState m_ePreviousActorState;
	
		EMovementDirection m_eLastMovementDirection;
	
		Point2I m_currentTilePoint; // The logical tile position this enemy is currently at (This is currently only set when the enemy STOPS moving.)
	
		S32 m_iTileAttackPower;		// Amount of damage to deal to a tile when this enemy attacks it.
		F32 m_fPlayerAttackPower;	// Amount of damage to deal to the player when this enemy attacks it.
	
		bool m_bAttackedDuringThisAnim; // Whether the enemy attacked during this current animation cycle (in case the animation frame is the same)
	
		F32 m_fMovementSpeed;
	
		bool m_bIsAttackingPlayer;
	
		U32 m_uNumSpawnWarnings; // Number of times "warning" animation plays before actually spawning
	
		F32 m_fSpitTimer;
	
		S32 m_iNumPathNodes;
	
		CComponentStaticPath* m_pStaticPath;
		S32 m_iCurrentStaticPathIndex;
	
		bool m_bIsDying; // This might not be necessary, as the states can probably be used
		
		// Unique variables for spitters (Serpents)
		bool m_bHasSpitAttackPosition;
		t2dVector m_vSpitAttackPosition;
	
		static CComponentAStarActor* sm_pLastSpawnedWaveEnemy; // This is used so the spawn sound is not played multiple times during the same frame.
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTASTARACTOR_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------