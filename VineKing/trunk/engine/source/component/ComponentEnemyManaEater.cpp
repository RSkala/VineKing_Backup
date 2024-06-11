//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemyManaEater.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentEnemyManaEaterSpawner.h"
#include "ComponentGlobals.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"

#include "SoundManager.h"

#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 700.0f, 700.0f );

static const char g_szMoveAnimation[32]		= "MANA_THIEF_MOVE";
static const char g_szDeathAnimation[32]	= "MANA_THIEF_DEATH";
static const char g_szDrainAnimation[32]	= "MANA_THIEF_DRAIN";
static const char g_szEatAnimation[32]		= "MANA_THIEF_EAT";
static const char g_szSpawnAnimation[32]	= "MANA_THIEF_SPAWN";

static const ColorF g_ColorNormal( 1.0f, 1.0f, 1.0f );
static const ColorF g_ColorSuper( 1.0f, 0.0f, 0.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemyManaEater );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyManaEater::CComponentEnemyManaEater()
	: m_pAnimatedSpriteOwner( NULL )
	, m_bIsSuperEnemy( false )
	, m_eState( STATE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyManaEater::~CComponentEnemyManaEater()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::SpawnAtPositionWithEndpoint( const t2dVector& _vSpawnPosition, const t2dVector& _vEndPosition )
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	m_pAnimatedSpriteOwner->setPosition( _vSpawnPosition );
	m_pAnimatedSpriteOwner->setVisible( true );
	m_pAnimatedSpriteOwner->setUsesPhysics( true ); // This is now needed for movement

	m_vEndPosition = _vEndPosition;
	
	if( CComponentEnemyManaEaterSpawner::ShouldEnemyUseSpawnAnim() )
	{
		m_eState = STATE_SPAWNING;
		m_pAnimatedSpriteOwner->setUseMouseEvents( false );
		m_pAnimatedSpriteOwner->playAnimation( g_szSpawnAnimation, false );
	}
	else
	{
		m_eState = STATE_MOVING;
		m_pAnimatedSpriteOwner->setUseMouseEvents( true );
		m_pAnimatedSpriteOwner->playAnimation( g_szMoveAnimation, false );
		
		if( m_bIsSuperEnemy )
			m_pAnimatedSpriteOwner->moveTo( m_vEndPosition, CComponentEnemyManaEaterSpawner::GetMovementSpeedSuper(), true, true, false, 0.2f );
		else
			m_pAnimatedSpriteOwner->moveTo( m_vEndPosition, CComponentEnemyManaEaterSpawner::GetMovementSpeedNormal(), true, true, false, 0.2f );
	}
	
	CSoundManager::GetInstance().PlaySmokeSpawnSound();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::SpawnSuperAtPositionWithEndpoint( const t2dVector& _vSpawnPosition, const t2dVector& _vEndPosition )
{
	m_bIsSuperEnemy = true;
	
	m_pAnimatedSpriteOwner->setBlendColour( g_ColorSuper );
	
	SpawnAtPositionWithEndpoint( _vSpawnPosition, _vEndPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::Kill()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_eState == STATE_DYING )
		return;
	
	CComponentEnemyManaEaterSpawner::PlaySmokeHitEffectAtPosition( m_pAnimatedSpriteOwner->getPosition() );
	
	m_bIsSuperEnemy = false;
	//m_pAnimatedSpriteOwner->setBlendColour( g_ColorNormal );
	
	//printf( "Killing Mana Eater %s\n", m_pAnimatedSpriteOwner->getName() );
	
	m_pAnimatedSpriteOwner->playAnimation( g_szDeathAnimation, false );

	m_eState = STATE_DYING;
	
	m_pAnimatedSpriteOwner->setUseMouseEvents( false );
	m_pAnimatedSpriteOwner->setPositionTargetOff();
	m_pAnimatedSpriteOwner->setLinearVelocity( t2dVector::getZero() );
	
	CSoundManager::GetInstance().PlaySmokeDeathSound();
	
	CComponentScoringManager::GetInstance().IncrementNumKilledSmokeMonster();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyManaEater::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pAnimatedSpriteOwner, CComponentEnemyManaEater ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemyManaEater::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemyManaEater::onComponentAdd - Must be added to a t2dAnimatedSprite." );
		return false;
	}
	
	m_pAnimatedSpriteOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::onUpdate()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	switch( m_eState )
	{
		case STATE_SPAWNING:
			UpdateStateSpawning();
			break;
			
		case STATE_MOVING:
			UpdateStateMoving();
			break;
			
		case STATE_DYING:
			UpdateStateDying();
			break;
			
		default:
			break;
	}
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
	{
		Kill(); // RKS TODO FUCK: Use a specific 'level has ended' kill function
		//return;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::HandleOwnerPositionTargetReached()
{	 
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_bIsSuperEnemy )
		CComponentPlayerHomeBase::GetInstance().DamageHomeBase( CComponentEnemyManaEaterSpawner::GetAttackPowerSuper() );
	else
		CComponentPlayerHomeBase::GetInstance().DamageHomeBase( CComponentEnemyManaEaterSpawner::GetAttackPowerNormal() );
	
	m_pAnimatedSpriteOwner->setPositionTargetOff();
	m_pAnimatedSpriteOwner->setLinearVelocity( t2dVector::getZero() );
	
	m_pAnimatedSpriteOwner->setVisible( false );
	
	Kill();
	
	m_pAnimatedSpriteOwner->setPosition( g_vSafePosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::OnMouseDown( const t2dVector& _vWorldMousePoint )
{	
	if( CComponentPlayerHomeBase::GetInstance().DoesPlayerHomeBaseHaveAttackTarget() )
	{
		// Do nothing if the player home base is already attacking a target
		return;
	}
	
	if( CComponentGlobals::GetInstance().IsTouchingDown() )
		return;
	
	CComponentGlobals::GetInstance().SetTouchedObject( m_pAnimatedSpriteOwner );
	
	CComponentGlobals::GetInstance().SetPlayerTouchingEnemy();
	
	//CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackSmokeMonster( m_pAnimatedSpriteOwner, this );
	
	//Kill();
	//CComponentGlobals::GetInstance().PlayHitEffectAtPosition( m_pAnimatedSpriteOwner->getPosition() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::OnPostInit()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	m_pAnimatedSpriteOwner->setVisible( false );
	m_pAnimatedSpriteOwner->setPosition( g_vSafePosition );
	m_pAnimatedSpriteOwner->setUsesPhysics( true ); // This is now needed for movement
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHODS

void CComponentEnemyManaEater::UpdateStateSpawning()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() )
	{
		m_eState = STATE_MOVING;
		m_pAnimatedSpriteOwner->setUseMouseEvents( true );
		m_pAnimatedSpriteOwner->setUsesPhysics( true );
		m_pAnimatedSpriteOwner->playAnimation( g_szMoveAnimation, false );
		
		if( m_bIsSuperEnemy )
			m_pAnimatedSpriteOwner->moveTo( m_vEndPosition, CComponentEnemyManaEaterSpawner::GetMovementSpeedSuper(), true, true, false, 0.2f );
		else
			m_pAnimatedSpriteOwner->moveTo( m_vEndPosition, CComponentEnemyManaEaterSpawner::GetMovementSpeedNormal(), true, true, false, 0.2f );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::UpdateStateMoving()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEater::UpdateStateDying()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() == false )
		return;
	
	// Hide this enemy
	m_pAnimatedSpriteOwner->setVisible( false );
	
	// Spawn a mana seed at the position where this enemy died
	//t2dVector vPosition = m_pAnimatedSpriteOwner->getPosition();
	CComponentManaSeedItemDrop::SpawnSeedFromEnemyAtPosition( m_pAnimatedSpriteOwner->getPosition() );
	
	m_pAnimatedSpriteOwner->setPosition( g_vSafePosition );
	
	m_pAnimatedSpriteOwner->setBlendColour( g_ColorNormal );
	m_bIsSuperEnemy = false;
	
	m_eState = STATE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
