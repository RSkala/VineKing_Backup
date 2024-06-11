//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemyProjectileBrickAttack.h"

#include "ComponentPathGridHandler.h"

#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 500.0f, 500.0f );

static const S32 g_iProjectileDamage = 100;

static const F32 g_fSerpentProjectileSpeed	= 100.0f;
static const F32 g_fBossProjectileSpeed		= 500.0f; //1000.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemyProjectileBrickAttack );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyProjectileBrickAttack::CComponentEnemyProjectileBrickAttack()
	: m_pOwnerObject( NULL )
	, m_bIsInUse( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyProjectileBrickAttack::~CComponentEnemyProjectileBrickAttack()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::MakeVisibleAtPosition( const t2dVector& _vTargetPosition )
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_bIsInUse = true;
	
	m_pOwnerObject->setPosition( _vTargetPosition );
	m_pOwnerObject->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::FireAtPositionFromSerpent( const t2dVector& _vEndPosition )
{
	if( m_pOwnerObject )
		m_pOwnerObject->moveTo( _vEndPosition, g_fSerpentProjectileSpeed, true, true, false, 1.0f );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::FireAtPositionFromBoss( const t2dVector& _vEndPosition )
{
	if( m_pOwnerObject )
		m_pOwnerObject->moveTo( _vEndPosition, g_fBossProjectileSpeed, true, true, false, 1.0f );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyProjectileBrickAttack::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentEnemyProjectileBrickAttack ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemyProjectileBrickAttack::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "%s - Must be added to a t2dAnimatedSprite.\n", __FUNCTION__ );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::onUpdate()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::HandleOwnerPositionTargetReached()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setVisible( false );
	
	m_bIsInUse = false;
	
	t2dVector vPosition = m_pOwnerObject->getPosition();
	CComponentPathGridHandler::GetInstance().DealDamageToLandBrickAtPosition( g_iProjectileDamage, vPosition );
	
	m_pOwnerObject->setPosition( g_vSafePosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyProjectileBrickAttack::OnPostInit()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setPosition( g_vSafePosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
