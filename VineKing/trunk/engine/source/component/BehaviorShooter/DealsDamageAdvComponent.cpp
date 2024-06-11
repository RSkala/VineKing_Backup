//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "DealsDamageAdvComponent.h"
#include "TakesDamageAdvComponent.h"//for explode()
#include "T2D/t2dSceneObject.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(DealsDamageAdvComponent);
//-----------------------------------------------------------------------------

DealsDamageAdvComponent::DealsDamageAdvComponent()
{
	//
	mOwner = NULL;
	mExplodeEffect = NULL;
	mExplodeEffectName = NULL;

	//SimObject::setInternalName( "DealsDamageAdvComponent" );
}

bool DealsDamageAdvComponent::onAdd()
{
	if (!Parent::onAdd())
		return false;
	
	return true;
}

void DealsDamageAdvComponent::onRemove()
{
	Parent::onRemove();
}

void DealsDamageAdvComponent::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField("Owner", TypeSimObjectPtr, Offset(mOwner, DealsDamageAdvComponent), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );

	addField("strength", TypeS32, Offset(mStrength, DealsDamageAdvComponent));
	addField("deleteOnHit", TypeBool, Offset(mDeleteOnHit, DealsDamageAdvComponent));
	addField("explodeEffect", TypeString, Offset(mExplodeEffectName, DealsDamageAdvComponent));
}

//-----------------------------------------------------------------------------

bool DealsDamageAdvComponent::onComponentAdd(SimComponent *target)
{
	if (!Parent::onComponentAdd(target))
		return false;

	//
	t2dSceneObject *owner = dynamic_cast<t2dSceneObject*>(target);
	if (!owner)
	{
		Con::warnf("DealsDamageAdvComponent::onComponentAdd - Must be added to a t2dSceneObject.");
		return false;
	}

	// Store our owner
	mOwner = owner;

	return true;
}

void DealsDamageAdvComponent::onComponentRemove(SimComponent *target)
{
	Parent::onComponentRemove(target);
}


bool DealsDamageAdvComponent::dealDamage( int amount, t2dSceneObject *victim ) {
	TakesDamageAdvComponent *pComponent = NULL;
	if( !victim ) {
		Con::errorf( "DealsDamageAdvComponent::dealDamage()	No victim" );
		return false;
	}

	for( U32 i = 0; i < victim->getComponentCount(); i++ ) {
		pComponent = (TakesDamageAdvComponent *)victim->getComponent( i );
		//0 means they are equal, go figure
		if( dStrcmp( pComponent->getNamespace()->mName, "TakesDamageAdvComponent" ) == 0 ) {
			break;
		} else {
			pComponent = NULL;//keep looking
		}
	}

	if( pComponent != NULL ) {
		pComponent->takeDamage( amount, mOwner );
		return true;
	}
	return false;
}

//2 is zero function usable args
ConsoleMethod(DealsDamageAdvComponent, dealDamage, bool, 4, 4, "" ) {
	int amount = dAtoi( argv[2] );
	int id = dAtoi( argv[3] );
	SimObject *simObj = Sim::findObject( id );
	t2dSceneObject *victim = (t2dSceneObject*)(simObj);
	return object->dealDamage( amount, victim );
}

void DealsDamageAdvComponent::explode( t2dVector position ) {\

	if( !mExplodeEffectName ) {
		return;
	}
	if( !mExplodeEffect ) {
		mExplodeEffect = (t2dParticleEffect *)Sim::findObject( mExplodeEffectName );
		if( !mExplodeEffect ) {
			return;
		}
	}

	//preserve effect life mode while setting a new one
	t2dParticleEffect::eEffectLifeMode oldMode = mExplodeEffect->getEffectLifeMode();
	//set new mode
	mExplodeEffect->setEffectLifeMode( t2dParticleEffect::KILL , 1.0);
	//copy to new particle effect
	mEffect = (t2dParticleEffect *)mExplodeEffect->clone();
	//restore old mode
	mExplodeEffect->setEffectLifeMode( oldMode, 1.0);
	
	mEffect->setPosition( position );
	//play the effect already!
	mEffect->playEffect( true );//assume resetParticles = true like the ConsoleMethod does
}


ConsoleMethod(DealsDamageAdvComponent, explode, void, 3, 3, "" ) {
	t2dVector position;
	dSscanf(argv[2],"%f %f", &position.mX, &position.mY);

	object->explode( position );
}
