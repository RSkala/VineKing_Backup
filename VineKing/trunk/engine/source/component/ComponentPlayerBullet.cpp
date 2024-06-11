//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentPlayerBullet.h"

#include "ComponentShooterEnemy.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dAnimatedSprite.h"

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafeDespawnPosition( 500.0f, 500.0f );
static const t2dVector g_vUpVector( 0.0f, -1.0f );
static const F32 g_fBulletSpeed = 300.0f; // Units per second

static const F32 g_fScreenLimitPositionTop		= -250.0f;

static const F32 g_fDefaultBulletDamage = 100.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentPlayerBullet );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPlayerBullet::CComponentPlayerBullet()
	: m_pOwnerObject( NULL )
	, m_bIsAlive( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPlayerBullet::~CComponentPlayerBullet()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerBullet::Spawn( const t2dVector& _vSpawnPosition )
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setPosition( _vSpawnPosition );
	m_pOwnerObject->setHidden( false );
	m_bIsAlive = true;
	
	m_pOwnerObject->setCollisionActive( true, false );
	m_pOwnerObject->setCollisionPhysics( true, false );
	m_pOwnerObject->setCollisionCallback( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Inherited from ConsoleObject
/*static*/ void CComponentPlayerBullet::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentPlayerBullet ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPlayerBullet::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentPlayerBullet::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	m_pOwnerObject->setUseMouseEvents( false );
	m_pOwnerObject->disableUpdateCallback();
	m_pOwnerObject->setHidden( true );
	m_pOwnerObject->setPosition( g_vSafeDespawnPosition );
	
	
	// NOTE: Do not set up any other member pointers here, because Torque may not have created the objects yet
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerBullet::onUpdate()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( m_bIsAlive )
	{
		if( m_pOwnerObject->getPosition().mY < g_fScreenLimitPositionTop )
		{
			Die();
			return;
		}
		
		F32 fDistanceToTravel = g_fBulletSpeed * ITickable::smTickSec;
		t2dVector vVelocity = g_vUpVector * fDistanceToTravel;
		t2dVector vNewBulletPosition = vVelocity + m_pOwnerObject->getPosition();
		m_pOwnerObject->setPosition( vNewBulletPosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerBullet::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( m_pOwnerObject == NULL || _pCollisionStatus == NULL )
		return;
	
	Die();
	
	t2dAnimatedSprite* pCollidedObject = static_cast<t2dAnimatedSprite*>( _pCollisionStatus->mDstObject );
	if( pCollidedObject == NULL || pCollidedObject->getComponentCount() == 0 )
		return;
	
	// For now, the collided object will have a ComponentShooterEnemy and nothing else
	CComponentShooterEnemy* pEnemyComponent = static_cast<CComponentShooterEnemy*>( pCollidedObject->getComponent( 0 ) );
	if( pEnemyComponent )
	{
		pEnemyComponent->DealDamage( g_fDefaultBulletDamage );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerBullet::OnPostInit()
{
	if( m_pOwnerObject == NULL )
		return;
	
	//m_pOwnerObject->setMouseLocked( true ); // React to any mouse down event on the screen
	m_pOwnerObject->setUseMouseEvents( false );
	m_pOwnerObject->disableUpdateCallback();
	m_pOwnerObject->setHidden( true );
	m_pOwnerObject->setPosition( g_vSafeDespawnPosition );
	m_bIsAlive = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentPlayerBullet::Die()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->disableUpdateCallback();
	m_pOwnerObject->setHidden( true );
	m_pOwnerObject->setPosition( g_vSafeDespawnPosition );
	m_bIsAlive = false;
	m_pOwnerObject->setCollisionActive( false, false );
	m_pOwnerObject->setCollisionPhysics( false, false );
	m_pOwnerObject->setCollisionCallback( false );
}






