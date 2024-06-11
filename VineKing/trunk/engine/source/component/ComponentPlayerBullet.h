//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTPLAYERBULLET_H
#define COMPONENTPLAYERBULLET_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dSceneObject;
class t2dVector;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentPlayerBullet : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
public:
	DECLARE_CONOBJECT( CComponentPlayerBullet );
	
	CComponentPlayerBullet();
	virtual ~CComponentPlayerBullet();
	
	void Spawn( const t2dVector& );
	
	// Inherited from ConsoleObject
	static void initPersistFields();
	
	// Inherited from SimComponent
	virtual bool onComponentAdd( SimComponent* );
	virtual void onUpdate();
	
	// Inherited from SimComponent, but added by me
	virtual void HandleOwnerCollision( t2dPhysics::cCollisionStatus* );
	virtual void OnPostInit();
	
private:
	void Die();
	void DamageObject(){}
	
private:
	t2dSceneObject* m_pOwnerObject; // The bullet object template
	bool m_bIsAlive;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTPLAYERBULLET_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------