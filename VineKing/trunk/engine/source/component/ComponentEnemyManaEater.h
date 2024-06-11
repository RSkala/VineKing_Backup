//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYMANAEATER_H
#define COMPONENTENEMYMANAEATER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentManaSeedItemDrop;
class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemyManaEater : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemyManaEater );
		
		CComponentEnemyManaEater();
		virtual ~CComponentEnemyManaEater();
	
		void SpawnAtPositionWithEndpoint( const t2dVector&, const t2dVector& );
		void SpawnSuperAtPositionWithEndpoint( const t2dVector&, const t2dVector& );
		void Kill();
	
		bool IsSpawned() const { return m_eState != STATE_NONE; }
		
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
		enum EState
		{
			STATE_NONE = 0,
			STATE_SPAWNING,
			STATE_MOVING,
			STATE_DYING,
		};
		
		
	private:
		t2dAnimatedSprite* m_pAnimatedSpriteOwner; // The object that this component is assigned to (the owner of this component)
	
		t2dVector m_vStartPosition;
		t2dVector m_vEndPosition;
	
		bool m_bIsSuperEnemy;
	
		EState m_eState;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYMANAEATER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------