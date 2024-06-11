//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "TakesDamageAdvComponent.h"
#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(TakesDamageAdvComponent);
//-----------------------------------------------------------------------------

TakesDamageAdvComponent::TakesDamageAdvComponent()
{
	//
	mOwner = NULL;
	mRespawnEffect = NULL;
	mExplodeEffect = NULL;   
	mRespawnEffectName = NULL;  
	mExplodeEffectName = NULL;

	mExplosion = NULL;
	mEffect = NULL;

	mExplodeSound = NULL;
	mExplodeSoundName = NULL;


	//SimObject::setInternalName( "TakesDamageAdvComponent" );
}

bool TakesDamageAdvComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void TakesDamageAdvComponent::onRemove()
{
	Parent::onRemove();
}

void TakesDamageAdvComponent::initPersistFields()
{
	Parent::initPersistFields();


	addProtectedField( "Owner", TypeSimObjectPtr, Offset(mOwner, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "health", TypeS32, Offset(mHealth, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "starthealth", TypeS32, Offset(mStartHealth, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "lives", TypeS32, Offset(mLives, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "tintRedForDamage", TypeBool, Offset(mTintRedForDamage, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "respawnTime", TypeF32, Offset(mRespawnTime, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "invincibleTime", TypeF32, Offset(mInvincibleTime, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	//addProtectedField( "invincible", TypeBool, Offset(mInvincible, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addProtectedField( "respawnEffect", TypeSimObjectPtr, Offset(mRespawnEffect, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "explodeEffect", TypeString, Offset(mExplodeEffectName, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addProtectedField( "scoreboard", TypeSimObjectPtr, Offset(mScoreboard, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "value", TypeS32, Offset(mValue, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	addProtectedField( "resetScore", TypeBool, Offset(mResetScore, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addProtectedField( "startframe", TypeS32, Offset(mStartFrame, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addProtectedField( "nextLevel", TypeString, Offset(mNextLevel, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addProtectedField( "explodesound", TypeString, Offset(mExplodeSoundName, TakesDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//-----------------------------------------------------------------------------

bool TakesDamageAdvComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;

	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("TakesDamageAdvComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	return true;
}

void TakesDamageAdvComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

void TakesDamageAdvComponent::onAddToScene() {
	mStartHealth = mHealth;
	mStartFrame = ((t2dStaticSprite *)mOwner)->getFrame();
	spawn();
}



void TakesDamageAdvComponent::setVincible() {
	//mInvincible = false;
	mInvincibleTime = 0;
}

void TakesDamageAdvComponent::spawn() {
	t2dStaticSprite *pStatic = dynamic_cast<t2dStaticSprite *>(mOwner);
	mOwner->setCollisionActiveReceive( true );
	//%this.schedule(%this.invincibleTime * 1000, "setVincible");
	mInvincibleEnd =  Sim::getCurrentTime() + mInvincibleTime;

	mHealth = mStartHealth;
	mOwner->setBlendColour( ColorF( 1, 1, 1, 1 ) );
	mOwner->setVisible( true );
	if( pStatic ) {
		pStatic->setFrame(  mStartFrame );
	}

	mEffect = NULL;
	if( mRespawnEffect != NULL ) {
		//preserve effect life mode while setting a new one
		t2dParticleEffect::eEffectLifeMode oldMode = mRespawnEffect->getEffectLifeMode();
		//set new mode
		mRespawnEffect->setEffectLifeMode( t2dParticleEffect::KILL , 1.0);
		//copy to new particle effect
		mEffect = (t2dParticleEffect *)mRespawnEffect->clone();
		//restore old mode
		mRespawnEffect->setEffectLifeMode( oldMode, 1.0);
		//play the effect already!
		mEffect->setPosition( mOwner->getPosition() );
		mEffect->playEffect( true );//assume resetParticles = true like the ConsoleMethod does
	}
}

void TakesDamageAdvComponent::explode() {
	//-Mat not working
	t2dVector position = mOwner->getPosition();
	mExplosion = NULL;

	if( !mExplodeEffectName) {
		return;
	}
	if( !mExplodeEffect) {
		mExplodeEffect = (t2dParticleEffect *)Sim::findObject( mExplodeEffectName );
		if( !mExplodeEffect ) {
			return;
		}
	}

	if( mExplodeEffect != NULL ) { 
		//copy to new particle effect
		mExplosion = (t2dParticleEffect *)mExplodeEffect->clone();
		if( !mExplosion->getSceneGraph() ) {
		mOwner->getSceneGraph()->addToScene( mExplosion );
		}
		mExplosion->setEnabled( true );
		//get new position		
		mExplosion->setPosition( position );
		//set new mode
		mExplosion->setEffectLifeMode( t2dParticleEffect::KILL , 1.0);
		//play the effect already!
		mExplosion->playEffect( true );//assume resetParticles = true like the ConsoleMethod does
	}


	if( !mExplodeSoundName ) {
		return;
	}
	if( !mExplodeSound ) {
		mExplodeSound = dynamic_cast<AudioProfile*>( Sim::findObject( mExplodeSoundName ) );
		if( !mExplodeSound ) {
			Con::errorf( "TakesDamageAdvComponent::explode -- Cannot find audioProfile %s", mExplodeSoundName );
			return;
		}
	}
	Point3F pos(0.f, 0.f, 0.f);
	MatrixF transform;
	transform.set(EulerF(0,0,0), pos);
	alxPlay( mExplodeSound );
}

void TakesDamageAdvComponent::takeDamage( int amount, t2dSceneObject *attacker ) {
	
	int time = Sim::getCurrentTime();
	if( mInvincibleEnd > time ) {
		return;//we are still invincible
	}

	mHealth -= amount;
	if( mHealth <= 0 ) {
		//update score
		explode();
		kill();
		Con::executef( (SimObject *)mOwner, 1, "onKill" );
	} else {
		if( mTintRedForDamage ) {
			F32 tint = (F32)mHealth / (F32)mStartHealth;
			mOwner->setBlendColour( ColorF( 1, tint, tint, 1 ) );
		}
	}
}

void TakesDamageAdvComponent::kill() {
	mLives--;
	if( mLives <= 0 ) {
		mOwner->safeDelete();
	}

   //mInvincible = true;
	mInvincibleTime = 0;
   mOwner->setVisible( false );
   mOwner->setCollisionActiveReceive( false );
}


ConsoleMethod(TakesDamageAdvComponent, setVincible, void, 2, 2, "" ) {
	object->setVincible();
}

ConsoleMethod(TakesDamageAdvComponent, spawn, void, 2, 2, "" ) {
	object->spawn();
}

ConsoleMethod(TakesDamageAdvComponent, explode, void, 2, 2, "" ) {
	object->explode();
}
