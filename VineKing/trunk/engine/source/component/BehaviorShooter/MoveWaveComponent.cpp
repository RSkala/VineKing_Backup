//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "MoveWaveComponent.h"
#include "T2D/t2dSceneObject.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(MoveWaveComponent);
//-----------------------------------------------------------------------------


#define LEFT -1
#define RIGHT 1
#define WAVE_ADJUST_VALUE (0.05f)

static int gStartOffset = 0;

MoveWaveComponent::MoveWaveComponent()
{
	//
	mOwner = NULL;
	mDirectionName = NULL;

	mSinStep = 0;
	mAdjustVal = WAVE_ADJUST_VALUE;
	mWaveOffset = 0;

	//SimObject::setInternalName( "MoveWaveComponent" );
}

bool MoveWaveComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;

	return true;
}

void MoveWaveComponent::onRemove()
{
	Parent::onRemove();
}

void MoveWaveComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, MoveWaveComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("direction", TypeString, Offset(mDirectionName, MoveWaveComponent) );
	addField("speed", TypeF32, Offset(mSpeed, MoveWaveComponent) );
	addField("useWaveOffset", TypeBool, Offset(mUseWaveOffset, MoveWaveComponent) );
	addField("waveOffset", TypeS32, Offset(mWaveOffset, MoveWaveComponent) );

}

//-----------------------------------------------------------------------------

bool MoveWaveComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;

	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("MoveWaveComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	//hack city
	gStartOffset += 3;
	if( gStartOffset > 15 ) {
		gStartOffset = 0;//we only want a certain amount of variance here
	}

	mSinStep = mAdjustVal * gStartOffset;
	//now check that we are within reasonable bounds
	while( mSinStep > .99 ) {
		mSinStep -= .99;
	}

	return true;
}

void MoveWaveComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}

void MoveWaveComponent::onAddToScene() {
	//adjust the starting point by a little for each one
#ifdef PUAP_OPTIMIZE
	mOwner->setUsesPhysics( true );
#endif
}


void MoveWaveComponent::onUpdate() {
	updateMove();
}

void MoveWaveComponent::updateMove() {
	if( mDirection != LEFT && mDirection != RIGHT ) {
		if( dStrcmp( mDirectionName, "right" ) == 0 ) {
			mDirection = RIGHT;
		} else {
			mDirection = LEFT;
		}
	}

	t2dVector newPos = mOwner->getPosition();
	float deltaX = mSpeed * mDirection;

	if( mSinStep > .99 || mSinStep < -.99 ) {
		mAdjustVal = -mAdjustVal;
	}

	mSinStep += mAdjustVal;


	float deltaY = mSin( mSinStep ) * mSpeed;

	newPos.mX += deltaX;
	newPos.mY += deltaY;
	mOwner->moveTo( newPos, mSpeed, false, false, false, 0.1f );
}

ConsoleMethod(MoveWaveComponent, updateMove, void, 2, 2, "" ) {
	object->updateMove();
}
