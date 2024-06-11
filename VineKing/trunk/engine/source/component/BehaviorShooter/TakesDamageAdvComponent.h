//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "component/dynamicConsoleMethodComponent.h"
#include "T2D/t2dSceneObject.h"
#include "T2D/t2dParticleEffect.h"

//-----------------------------------------------------------------------------

class TakesDamageAdvComponent : public DynamicConsoleMethodComponent
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
   S32 mHealth;
   S32 mStartHealth;
   S32 mLives;
   bool mTintRedForDamage;
   F32 mRespawnTime;
   F32 mInvincibleTime;
   //bool mInvincible;
   t2dParticleEffect *mRespawnEffect;
   const char *mRespawnEffectName;
   t2dParticleEffect *mExplodeEffect;   
   const char *mExplodeEffectName;
   t2dParticleEffect *mExplosion;//our local copy
   t2dParticleEffect *mEffect;//our local copy
   SimObject *mScoreboard;
   S32 mValue;
   bool mResetScore;
   StringTableEntry mNextLevel;
   S32 mStartFrame;

   S32 mInvincibleEnd;

   AudioProfile *mExplodeSound;
   const char *mExplodeSoundName;

public:

	DECLARE_CONOBJECT(TakesDamageAdvComponent);

	TakesDamageAdvComponent();

	bool onAdd();
	void onRemove();

	static void initPersistFields();

	//
	void setVincible();
	void spawn();
	void explode();
	void takeDamage( int, t2dSceneObject *);
	void kill();

	virtual bool onComponentAdd(SimComponent *target);
	virtual void onComponentRemove(SimComponent *target);

	virtual void onAddToScene();

	//
};