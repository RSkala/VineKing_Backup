//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef PLAYERHOMEBASECOMPONENT_H
#define PLAYERHOMEBASECOMPONENT_H

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dAnimatedSprite;
class CComponentBossMountain;
class CComponentEnemyBlob;
class CComponentEnemySerpent;
class CComponentEnemySmoke;
class CComponentManaSeedItemDrop;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentPlayerHomeBase : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EPlayerHomeBaseState
		{
			HOME_BASE_STATE_NONE = 0,
			HOME_BASE_STATE_IDLE,
			HOME_BASE_STATE_GROW_START,
			HOME_BASE_STATE_GROW_LOOPING,
			HOME_BASE_STATE_MOVING,
			HOME_BASE_STATE_ATTACKING_START,
			HOME_BASE_STATE_ATTACKING,
			HOME_BASE_STATE_EATING,
			HOME_BASE_STATE_WIN,
			HOME_BASE_STATE_DEAD,
			HOME_BASE_STATE_SEED_MAGNET,
			HOME_BASE_STATE_HITREACTION,
			HOME_BASE_STATE_OUT_OF_MANA,
			
			HOME_BASE_STATE_WAITING_FOR_END_SEQUENCE,
		};
	
		enum EAttackTargetType
		{
			TARGET_TYPE_NONE = 0,
			TARGET_TYPE_BLOB,
			TARGET_TYPE_SERPENT,
			TARGET_TYPE_SMOKE,
			TARGET_TYPE_BOSS,
			TARGET_TYPE_SEED,
		};
		
	public:
		DECLARE_CONOBJECT( CComponentPlayerHomeBase );
		
		CComponentPlayerHomeBase();
	
		static inline CComponentPlayerHomeBase& GetInstance();
	
		void GetHomeBasePosition( t2dVector& );
		void GetHomeBaseBottomPosition( t2dVector& );
		void GetHomeBaseVisibleCenterPosition( t2dVector& );
		void GetHomeBaseBellyPosition( t2dVector& );
	
		void HandleHomeBaseStateNone();
		void HandleHomeBaseStateIdle();
		void HandleHomeBaseStateGrowStart();
		void HandleHomeBaseStategrowLooping();
		void HandleHomeBaseStateMoving();
		void HandleHomeBaseStateAttackingStart();
		void HandleHomeBaseStateAttacking();
		void HandleHomeBaseStateEating();
		void HandleHomeBaseStateWin() { }
		void HandleHomeBaseStateSeedMagnet();
		void HandleHomeBaseStateHitReaction();
		void HandleHomeBaseStateOutOfMana();
	
		void SwitchHomeBaseState( const EPlayerHomeBaseState _eHomeBaseState );
	
		void DetermineHomeBaseFlip( const t2dVector& );
	
	
		void NotifyHomeBaseToAttackBlob		( t2dSceneObject* const, DynamicConsoleMethodComponent* const );
		void NotifyHomeBaseToAttackSerpent	( t2dSceneObject* const, DynamicConsoleMethodComponent* const );
		void NotifyHomeBaseToAttackSmoke	( t2dSceneObject* const, DynamicConsoleMethodComponent* const );
		void NotifyHomeBaseToAttackBoss		( t2dSceneObject* const, DynamicConsoleMethodComponent* const, const t2dVector& );
		//void NotifyHomeBaseToAttackSeed	( t2dSceneObject* const, DynamicConsoleMethodComponent* const );
	
		void NotifyHomeBaseToEatSeedFromMagnetMode( const F32& );
		void NotifyHomeBaseOfLevelWinStart();
		void NotifyHomeBaseOfLevelWin();
		void NotifyHomeBaseToEnterSeedMagnetMode();
		void NotifyHomeBaseToExitSeedMagnetMode();
		
		void NotifyHomeBaseOfOutOfManaLineDrawAttempt();
	
		bool DoesPlayerHomeBaseHaveAttackTarget() const { return (m_pCurrentAttackTarget != NULL); } 
		bool IsHomeBaseInSeedMagnetMode() const { return m_ePlayerHomeBaseState == HOME_BASE_STATE_SEED_MAGNET; }
	
		void DamageHomeBase( const F32& );
		F32 GetMaxHomeBaseHealth() const { return m_fMaxHealth; }
		F32 GetCurrentHomeBaseHealth() const { return m_fCurrentHealth; }
	
		F32 GetPercentHomeBaseHealthRemaining() const { return m_fCurrentHealth / m_fMaxHealth; }
	
		void HideHomeBase();
		void UnhideHomeBase();
	
		//static F32 GetBaseToMaxHealthPercentage();
		static F32 GetPercentHealthRemaining();
	
		bool IsHomeBaseFacingRight() const { return m_bFacingRight; }
    
        static void ForceHomeBaseIntoIdle();
	
		static void TEST_VineKingFingerUp();
		static void TEST_VineKingFingerDown();
		
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void HandleOwnerPositionTargetReached();
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
	
	private:
		void SetAttackVariables( t2dSceneObject* const, DynamicConsoleMethodComponent* const, const t2dVector& ); 
		void SetUpVineForAttack();
		void AttackBlob();
		void AttackSerpent();
		void AttackSmoke();
		void AttackBoss();

		void OnSwitchHomeBaseState( const EPlayerHomeBaseState _eHomeBaseState );
		bool CanHomeBaseSwitchStates( const EPlayerHomeBaseState _eHomeBaseState ) const;
		
	private:
		t2dAnimatedSprite* m_pHomeBaseObject;	// The owner of this component will be the home base object
		
		const char* m_pszAnimIdleLoop;
		const char* m_pszAnimGrowStart;
		const char* m_pszAnimGrowLoop;
		const char* m_pszAnimMoveLoop;
		const char* m_pszEatAnim;
		const char* m_pszWinAnim;
		const char* m_pszDeadAnim;
		const char* m_pszHitAnim;
	
		EPlayerHomeBaseState m_ePlayerHomeBaseState;
	
		const char* m_pszVineObjectA;		// The Name of the Vine Object used for attacking
		t2dAnimatedSprite* m_pVineObjectA;	// The Vine Object used for attacking
	
		const char* m_pszVineObjectB;		// The Name of the Vine Object used for attacking
		t2dAnimatedSprite* m_pVineObjectB;	// The Vine Object used for attacking
	
		const char* m_pszVineObjectC;		// The Name of the Vine Object used for attacking
		t2dAnimatedSprite* m_pVineObjectC;	// The Vine Object used for attacking
		Vector<t2dAnimatedSprite*> m_VineObjectCList;
	
		F32 m_fMaxHealth;
		F32 m_fCurrentHealth;
	
		//------------------------------------------------------------
		// New Attack Stuff - 2011/03/17
		t2dSceneObject* m_pCurrentAttackTarget;
		DynamicConsoleMethodComponent* m_pCurrentAttackTargetComponent;
		EAttackTargetType m_eCurrentAttackTargetType;
		t2dVector m_vCurrentBossAttackTargetPos;
		//------------------------------------------------------------
	
		bool m_bFacingRight;
	
		static CComponentPlayerHomeBase* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentPlayerHomeBase& CComponentPlayerHomeBase::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentPlayerHomeBase::sm_pInstance == NULL" );
	return *sm_pInstance;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // PLAYERHOMEBASECOMPONENT_H

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
