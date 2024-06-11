//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "FaceObjectComponent.h"
#include "T2D/t2dSceneObject.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(FaceObjectComponent);
//-----------------------------------------------------------------------------

FaceObjectComponent::FaceObjectComponent()
{
	//
	mOwner = NULL;
	mObject = NULL;
	mRotationOffset = 0;

	//SimObject::setInternalName( "FaceObjectComponent" );
}

bool FaceObjectComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void FaceObjectComponent::onRemove()
{
	Parent::onRemove();
}

void FaceObjectComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, FaceObjectComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("object", TypeString, Offset( mObjectName, FaceObjectComponent ) );
	addField("turnspeed", TypeF32, Offset( mTurnSpeed, FaceObjectComponent ) );
	addField("rotationOffset", TypeF32, Offset( mRotationOffset, FaceObjectComponent ) );
}

//-----------------------------------------------------------------------------

bool FaceObjectComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;
  
	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("FaceObjectComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	if( !mObjectName ) {
		Con::errorf( "FaceObjectComponent has no object to follow" );
	}

	return true;
}

void FaceObjectComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

void FaceObjectComponent::onAddToScene() {
#ifdef PUAP_OPTIMIZE
	mOwner->setUsesPhysics( true );
#endif
}

void FaceObjectComponent::onUpdate() {
	if( !mObjectName ) {
		return;
	}
	if( !mObject ) {
		mObject = (t2dSceneObject *) Sim::findObject( mObjectName );
		if( !mObject ) {
			Con::errorf( "FaceObjectComponent invalid object (%s)", mObjectName );
			return;
		}
	}

	

   t2dVector dist =  mObject->getPosition() - mOwner->getPosition();
   F32 targetRot = mRadToDeg(mAtan(dist.mY, dist.mX)) + 90 + mRotationOffset;
   
	if( mTurnSpeed == 0 ) {
		mOwner->setRotation( targetRot );
	}
	else {
		mOwner->rotateTo(targetRot, mTurnSpeed, true, false, true, 0.1f );
	}

}
