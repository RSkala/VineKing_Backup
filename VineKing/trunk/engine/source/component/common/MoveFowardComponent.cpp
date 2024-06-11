//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "MoveForwardComponent.h"
#include "T2D/t2dSceneObject.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(MoveForwardComponent);
//-----------------------------------------------------------------------------

MoveForwardComponent::MoveForwardComponent()
{
	//
	mOwner = NULL;
	mTimeTilNextUpdate = 0;

	//SimObject::setInternalName( "MoveForwardComponent" );
}

bool MoveForwardComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void MoveForwardComponent::onRemove()
{
	Parent::onRemove();
}

void MoveForwardComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, MoveForwardComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("speed", TypeF32, Offset( mSpeed, MoveForwardComponent ) );
	addField("angleOffset", TypeF32, Offset( mAngleOffset, MoveForwardComponent ) );
	addField("updateRate", TypeF32, Offset( mUpdateRate, MoveForwardComponent ) );
}

//-----------------------------------------------------------------------------

bool MoveForwardComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;
  
	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("MoveForwardComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	return true;
}

void MoveForwardComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

void MoveForwardComponent::onAddToScene() {
#ifdef PUAP_OPTIMIZE
	mOwner->setUsesPhysics( true );
#endif
	mTimeTilNextUpdate = 0;
}

void MoveForwardComponent::onUpdate() {

	if( mTimeTilNextUpdate > Sim::getCurrentTime() ) {
		return;
	}

	F32 newRot = mOwner->getRotation() + mAngleOffset;
    F32 angle = mDegToRad(mFmod(newRot, 360.0f));
    // Fetch Speed.
    F32 speed = mSpeed;

    // Calculate Angle.
    F32 sin, cos;
    mSinCos( angle, sin, cos );

    // Set Gross Linear Velocity.
    mOwner->setLinearVelocity( t2dVector( sin*speed, -cos*speed ) );

	mTimeTilNextUpdate = Sim::getCurrentTime() + (mUpdateRate * 1000);
}
