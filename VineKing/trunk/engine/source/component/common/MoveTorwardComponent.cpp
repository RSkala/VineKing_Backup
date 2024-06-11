//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "MoveTowardComponent.h"
#include "T2D/t2dSceneObject.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(MoveTowardComponent);
//-----------------------------------------------------------------------------

MoveTowardComponent::MoveTowardComponent()
{
	//
	mOwner = NULL;
	mObject = NULL;

	//SimObject::setInternalName( "MoveTowardComponent" );
}

bool MoveTowardComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void MoveTowardComponent::onRemove()
{
	Parent::onRemove();
}

void MoveTowardComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, MoveTowardComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("object", TypeString, Offset( mObjectName, MoveTowardComponent ) );
	addField("speed", TypeF32, Offset( mSpeed, MoveTowardComponent ) );
}

//-----------------------------------------------------------------------------

bool MoveTowardComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;
  
	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("MoveTowardComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	if( !mObjectName ) {
		Con::errorf( "MoveTowardComponent has no object to follow" );
	}

	return true;
}

void MoveTowardComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

void MoveTowardComponent::onAddToScene() {
#ifdef PUAP_OPTIMIZE
	mOwner->setUsesPhysics( true );
#endif
}

void MoveTowardComponent::onUpdate() {
	if( !mObjectName ) {
		return;
	}
	if( !mObject ) {
		mObject = (t2dSceneObject *) Sim::findObject( mObjectName );
		if( !mObject ) {
			Con::errorf( "MoveTowardComponent invalid object (%s)", mObjectName );
			return;
		}
	}

	mOwner->moveTo( mObject->getPosition(), mSpeed, false, false, false, 0.1f );
}
