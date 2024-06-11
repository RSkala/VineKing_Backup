//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "component/dynamicConsoleMethodComponent.h"
#include "T2D/t2dSceneObject.h"
#include "T2D/t2dParticleEffect.h"

//-----------------------------------------------------------------------------

class DealsDamageAdvComponent : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;

public:
	struct GroundCheck
	{
		F32 mMaxNormal;
		F32 mThreshold;
		F32 mBuffer;
	};

protected:

	//
	t2dSceneObject *mOwner;

	//
	S32 mStrength;
	bool mDeleteOnHit;
	t2dParticleEffect *mExplodeEffect;
	const char *mExplodeEffectName;
	t2dParticleEffect *mEffect;

public:

	DECLARE_CONOBJECT(DealsDamageAdvComponent);

	DealsDamageAdvComponent();

	bool onAdd();
	void onRemove();

	static void initPersistFields();

	//
	virtual bool onComponentAdd(SimComponent *target);
	virtual void onComponentRemove(SimComponent *target);

	//
	bool dealDamage( int amount, t2dSceneObject *victim );
	void explode( t2dVector position );
};