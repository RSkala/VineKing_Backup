//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "ShootsAdvComponent.h"
#include "T2D/t2dSceneObject.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(ShootsAdvComponent);
//-----------------------------------------------------------------------------

ShootsAdvComponent::ShootsAdvComponent()
{
	//
	mOwner = NULL;
	mProjectile = NULL;
	mProjectileName = NULL;
	mShootSound = NULL;
	mShootSoundName = NULL;
	mTimeTilNextFireEnable = 0;
	mFireRate = 0;

	//SimObject::setInternalName( "ShootsAdvComponent" );
}

bool ShootsAdvComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void ShootsAdvComponent::onRemove()
{
	Parent::onRemove();
}

void ShootsAdvComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, ShootsAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("projectile", TypeString, Offset( mProjectileName, ShootsAdvComponent ) );
	//addField("fireKey",
	addField("fireRate", TypeF32, Offset( mFireRate, ShootsAdvComponent ) );
	addField("projectileSpeed", TypeF32, Offset( mProjectileSpeed, ShootsAdvComponent ) );
	addField("fireAngleOffset", TypeF32, Offset( mFireAngleOffset, ShootsAdvComponent ) );
	addField("rotationOffset", TypeF32, Offset( mRotationOffset, ShootsAdvComponent ) );
	addField("shootsound", TypeString, Offset( mShootSoundName, ShootsAdvComponent ) );
}

//-----------------------------------------------------------------------------

bool ShootsAdvComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;
  
	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("ShootsAdvComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	return true;
}

void ShootsAdvComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

bool ShootsAdvComponent::fire() {

	if( !mProjectileName ) {
		Con::errorf( "ShootsAdvComponent::fire -- No projectile to fire" );
		return false;
	}
	if( !mProjectile ) {
		//lookup the object now, in case is was defined after we init'ed
		mProjectile = (t2dSceneObject *)Sim::findObject( mProjectileName );
		if( !mProjectile ) {
			Con::errorf( "ShootsAdvComponent::fire -- Projectile name given, but cannot find object (%s)", mProjectileName );
			return false;
		}
	}

	int time = Sim::getCurrentTime();
	if( mTimeTilNextFireEnable > time ) {
		return false;//we can only fire so often
	}

	mTimeTilNextFireEnable = time + (mFireRate * 1000 );


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
			Con::errorf( "ShootsAdvComponent::fire -- Cannot find audioProfile %s", mShootSoundName );
			return true;//because we still fired the shot
		}
	}
	Point3F pos(0.f, 0.f, 0.f);
	MatrixF transform;
	transform.set(EulerF(0,0,0), pos);
	alxPlay( mShootSound );

	return true;
}

ConsoleMethod(ShootsAdvComponent, fire, bool, 2, 3, "no arguments" ) {
	//if we are bound to an action we will get one argument
	return object->fire();
}

