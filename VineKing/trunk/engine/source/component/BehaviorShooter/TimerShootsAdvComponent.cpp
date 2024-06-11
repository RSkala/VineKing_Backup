//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "TimerShootsAdvComponent.h"
#include "T2D/t2dSceneObject.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(TimerShootsAdvComponent);
//-----------------------------------------------------------------------------

TimerShootsAdvComponent::TimerShootsAdvComponent()
{
	//
	mOwner = NULL;
	mProjectile = NULL;
	mProjectileName = NULL;
	mShootSound = NULL;
	mShootSoundName = NULL;

	//SimObject::setInternalName( "TimerShootsAdvComponent" );
}

bool TimerShootsAdvComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void TimerShootsAdvComponent::onRemove()
{
	Parent::onRemove();
}

void TimerShootsAdvComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, TimerShootsAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("projectile", TypeString, Offset( mProjectileName, TimerShootsAdvComponent ) );
	//addField("fireKey",
	addField("fireRate", TypeF32, Offset( mFireRate, TimerShootsAdvComponent ) );
	addField("projectileSpeed", TypeF32, Offset( mProjectileSpeed, TimerShootsAdvComponent ) );
	addField("fireAngleOffset", TypeF32, Offset( mFireAngleOffset, TimerShootsAdvComponent ) );
	addField("rotationOffset", TypeF32, Offset( mRotationOffset, TimerShootsAdvComponent ) );
	addField("shootsound", TypeString, Offset( mShootSoundName, TimerShootsAdvComponent ) );
	addField("fireStartDelay", TypeF32, Offset( mFireStartDelay, TimerShootsAdvComponent ) );
	addField("fireRateVariance", TypeF32, Offset( mFireRateVariance, TimerShootsAdvComponent ) );
}

//-----------------------------------------------------------------------------

bool TimerShootsAdvComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;
  
	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("TimerShootsAdvComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	return true;
}

void TimerShootsAdvComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

void TimerShootsAdvComponent::onAddToScene() {
	mTimeTilNextFireEnable = Sim::getCurrentTime() + (mFireStartDelay * 1000);
}

void TimerShootsAdvComponent::onUpdate() { 

	int time = Sim::getCurrentTime();
	if( mTimeTilNextFireEnable > time ) {
		return;//we can only fire so often
	}

	F32 min = (mFireRate - mFireRateVariance) * 1000;
	F32 max = (mFireRate + mFireRateVariance) * 1000;
	F32 delay = gRandGen.randF( min, max );
	mTimeTilNextFireEnable = time + delay;
	fire();
}

bool TimerShootsAdvComponent::fire() {

	if( !mProjectileName ) {
		Con::errorf( "TimerShootsAdvComponent::fire -- No projectile to fire" );
		return false;
	}
	if( !mProjectile ) {
		//lookup the object now, in case is was defined after we init'ed
		mProjectile = (t2dSceneObject *)Sim::findObject( mProjectileName );
		if( !mProjectile ) {
			Con::errorf( "TimerShootsAdvComponent::fire -- Projectile name given, but cannot find object (%s)", mProjectileName );
			return false;
		}
	}

	t2dSceneObject *projectile = mProjectile->clone();

	t2dVector newPOS =  mOwner->getPosition();
	projectile->setPosition( newPOS );
	projectile->setEnabled( true );

	F32 newRot = mOwner->getRotation() + mRotationOffset;
	projectile->setRotation( newRot );

    // Renormalise Angle.
	F32 velAngle = mOwner->getRotation() + mFireAngleOffset;
    F32 angle = mDegToRad( mFmod(velAngle, 360.0f) );
    // Fetch Speed.
    F32 speed = mProjectileSpeed;

    // Calculate Angle.
    F32 sin, cos;
    mSinCos( angle, sin, cos );

	projectile->setLinearVelocity( t2dVector( sin*speed, -cos*speed ) );

	if( !mShootSoundName ) {
		return true;//because we still fired the shot
	}
	if( !mShootSound ) {
		mShootSound = dynamic_cast<AudioProfile*>( Sim::findObject( mShootSoundName ) );
		if( !mShootSound ) {
			Con::errorf( "TimerShootsAdvComponent::fire -- Cannot find audioProfile %s", mShootSoundName );
			return true;//because we still fired the shot
		}
	}
	Point3F pos(0.f, 0.f, 0.f);
	MatrixF transform;
	transform.set(EulerF(0,0,0), pos);
	alxPlay( mShootSound );

	return true;
}

ConsoleMethod(TimerShootsAdvComponent, fire, bool, 2, 3, "no arguments" ) {
	//if we are bound to an action we will get one argument
	return object->fire();
}

