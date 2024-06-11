//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSHOOTERENEMY_H
#define COMPONENTSHOOTERENEMY_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dAnimatedSprite;
class t2dVector;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentShooterEnemy : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentShooterEnemy );
		
		CComponentShooterEnemy();
		virtual ~CComponentShooterEnemy();
		
		void DealDamage( const F32& );
		bool IsAvailable() const { return m_bIsAvailable; }
		void SpawnAtPosition( const t2dVector& );
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		// Inherited from SimComponent, but added by me
		virtual void HandleOwnerCollision( t2dPhysics::cCollisionStatus* );
		virtual void HandleOwnerPositionTargetReached();
		virtual void OnPostInit();
		
	private:
		void Die();
		
	private:
		t2dAnimatedSprite* m_pOwnerObject;
		const char* m_pszDefaultAnim;
		const char* m_pszDeathAnim;
		F32 m_fMaxHealth;
		F32 m_fCurrentHealth;
		bool m_bIsDying;
		bool m_bIsAvailable;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSHOOTERENEMY_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------