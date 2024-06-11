//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLineDrawAttackObject.h"

#include "T2D/t2dTileMap.h"
#include "ComponentGlobals.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLineDrawAttackObject* CComponentLineDrawAttackObject::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLineDrawAttackObject );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLineDrawAttackObject::CComponentLineDrawAttackObject()
	: m_pOwner( NULL )
{
	CComponentLineDrawAttackObject::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLineDrawAttackObject::~CComponentLineDrawAttackObject()
{
	CComponentLineDrawAttackObject::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentLineDrawAttackObject::IsLineDrawAttackObjectActive()
{
	if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
		return false;
	
	return sm_pInstance->m_pOwner->getVisible();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLineDrawAttackObject::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentLineDrawAttackObject ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLineDrawAttackObject::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentLineDrawAttackObject::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	CComponentLineDrawAttackObject::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttackObject::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setCollisionActive( false, false );
	m_pOwner->setCollisionCallback( false );
	m_pOwner->setNeverSolvePhysics( true ); // Setting this to true => this object will not react or move from a collision.
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
