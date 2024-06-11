//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYPROJECTILEBRICKATTACK_H
#define COMPONENTENEMYPROJECTILEBRICKATTACK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyProjectileBrickAttack : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemyProjectileBrickAttack );
		
		CComponentEnemyProjectileBrickAttack();
		virtual ~CComponentEnemyProjectileBrickAttack();
	
		void MakeVisibleAtPosition( const t2dVector& );
		void FireAtPositionFromSerpent( const t2dVector& );
		void FireAtPositionFromBoss( const t2dVector& );
	
		bool IsProjectileInUse() const { return m_bIsInUse; }
		
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
		t2dAnimatedSprite* m_pOwnerObject; // The object that owns this component.
		bool m_bIsInUse;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYPROJECTILEBRICKATTACK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------