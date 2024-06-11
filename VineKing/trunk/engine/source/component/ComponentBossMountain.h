//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTBOSSMOUNTAIN_H
#define COMPONENTBOSSMOUNTAIN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dAnimatedSprite;
class t2dSceneObject;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentBossMountain : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EBossState 
		{
			BOSS_STATE_NONE = 0,
			BOSS_STATE_IDLE01,
			BOSS_STATE_IDLE02,
			BOSS_STATE_IDLE03,
			BOSS_STATE_FIRING,
			BOSS_STATE_HIT_REACTION,
			BOSS_STATE_DYING,
			BOSS_STATE_DEAD,
		};
	
	public:
		DECLARE_CONOBJECT( CComponentBossMountain );
		
		CComponentBossMountain();
		virtual ~CComponentBossMountain();
	
		static bool LevelHasActiveBoss() { return CComponentBossMountain::sm_pCurrentAttackingBoss != NULL; }
		static void FireBrickProjectileAtPosition( const t2dVector& );
	
		void DamageBoss( const F32& );
		bool IsDead() const { return m_bDead; }
	
		void GetLeftEyePosition( t2dVector& );
		void GetRightEyePosition( t2dVector& );
	
		static void SetUpBossData();
		static void ClearBossData();
	
		static void GetCurrentAttackingBossMouthPosition( t2dVector& );
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
	
	private:
		void SetUpLinkedSmokeIndex( S32&, const char*& );
		
	private:
		t2dAnimatedSprite* m_pOwner;
	
		EBossState m_eBossState;
						
		F32 m_fCurrentHealth;
		bool m_bDead;
	
		t2dAnimatedSprite* m_pEyeLeft;
		t2dAnimatedSprite* m_pEyeRight;
		t2dAnimatedSprite* m_pJaw;
	
		const char* m_pszLinkedSmokeIndex_01;
		const char* m_pszLinkedSmokeIndex_02;
		const char* m_pszLinkedSmokeIndex_03;
	
		S32 m_iLinkedSmokeIndex_01;
		S32 m_iLinkedSmokeIndex_02;
		S32 m_iLinkedSmokeIndex_03;
	
		static Vector<CComponentBossMountain*> sm_BossMountainList;
		static S32 sm_iCurrentAttackingBossIndex;
		static CComponentBossMountain* sm_pCurrentAttackingBoss; // The current boss for attacking
	
		static CComponentBossMountain* sm_pBossLeft;
		static CComponentBossMountain* sm_pBossRight;
		static CComponentBossMountain* sm_pBossCenter;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTBOSSMOUNTAIN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

