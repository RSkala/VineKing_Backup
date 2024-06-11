//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentShooterEnemy.h"

#include "ComponentGlobals.h"

#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const F32 g_fEnemyMaxHealth = 100.0f;

static const t2dVector g_vSafePosition( 500.0f, 500.0f );


// Bullet sound globals
static const char g_szEnemyDeathSoundName[32] = "TapSound";
AudioProfile* g_pDeathSound = NULL;
AUDIOHANDLE g_hDeathSound = NULL_AUDIOHANDLE; // NULL_AUDIOHANDLE defined in platform/platformAudio.h

static U32 g_uNumEnemiesKilled = 0;



//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentShooterEnemy );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentShooterEnemy::CComponentShooterEnemy()
	: m_pOwnerObject( NULL )
	, m_pszDefaultAnim( NULL )
	, m_pszDeathAnim( NULL )
	, m_fMaxHealth( g_fEnemyMaxHealth )
	, m_fCurrentHealth( g_fEnemyMaxHealth )
	, m_bIsDying( false )
	, m_bIsAvailable( true )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentShooterEnemy::~CComponentShooterEnemy()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemy::DealDamage( const F32& _fDamageAmount )
{
	if( m_fCurrentHealth <= 0.0f )
		return;
	
	m_fCurrentHealth -= _fDamageAmount;
	if( m_fCurrentHealth <= 0.0f )
	{
		m_fCurrentHealth = 0.0f;
		Die();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemy::SpawnAtPosition( const t2dVector& _vSpawnPosition )
{
	if( m_pOwnerObject == NULL || m_bIsAvailable == false )
		return;
	
	m_bIsDying = false;
	m_bIsAvailable = false;
	
	m_pOwnerObject->setCollisionActive( false, true );
	m_pOwnerObject->setCollisionPhysics( false, true );
	
	m_pOwnerObject->setPosition( _vSpawnPosition );
	m_pOwnerObject->setVisible( true );
	m_pOwnerObject->enableUpdateCallback();
	
	m_pOwnerObject->playAnimation( m_pszDefaultAnim, false );
	
	// Get a random destination at the bottom of the screen
	t2dVector vDestination( -140.0f, 280.0f );
	F32 fRandomX = static_cast<F32>( CComponentGlobals::GetInstance().GetRandomRange( 0, 300 ) );
	vDestination.mX += fRandomX;	
	
	// Get a random speed
	F32 fSpeed = static_cast<F32>( CComponentGlobals::GetInstance().GetRandomRange( 60, 150 ) );
	
	m_pOwnerObject->moveTo( vDestination, fSpeed, true, true, false, 10.0f );
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentShooterEnemy::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentShooterEnemy ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "DefaultAnim",	TypeString, Offset( m_pszDefaultAnim,		CComponentShooterEnemy ) );
	addField( "DeathAnim",		TypeString, Offset( m_pszDeathAnim,		CComponentShooterEnemy ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentShooterEnemy::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentShooterEnemy::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	m_pOwnerObject->setCollisionActive( false, false );
	m_pOwnerObject->setCollisionPhysics( false, false );
	m_pOwnerObject->setCollisionCallback( false );
	m_pOwnerObject->disableUpdateCallback();
	
	
	// NOTE: Do not set up any other member pointers here, because Torque may not have created the objects yet
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemy::onUpdate()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( m_bIsDying )
	{
		if( m_pOwnerObject->mAnimationController.isAnimationFinished() )
		{
			m_pOwnerObject->setVisible( false );
			m_pOwnerObject->disableUpdateCallback();
			
			m_pOwnerObject->setPosition( g_vSafePosition );
			m_bIsAvailable = true;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemy::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( m_pOwnerObject == NULL || _pCollisionStatus == NULL )
		return;
	
	//t2dTileLayer* pTileLayer = dynamic_cast<t2dTileLayer*>( _pCollisionStatus->mDstObject );
	//if( pTileLayer == NULL )
	//	return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemy::HandleOwnerPositionTargetReached()
{
	// Kill this enemy when it reaches its destination
	Die();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemy::OnPostInit()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( g_pDeathSound == NULL )
		g_pDeathSound = static_cast<AudioProfile*>( Sim::findObject( g_szEnemyDeathSoundName ) );
	
	m_bIsDying = false;
	m_bIsAvailable = true;
	
	m_pOwnerObject->setCollisionPhysics( false, false );
	m_pOwnerObject->setCollisionCallback( false );
	m_pOwnerObject->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE FUNCTION IMPLEMENTATIONS

void CComponentShooterEnemy::Die()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( alxIsPlaying( g_hDeathSound ) )
	{
		alxStop( g_hDeathSound );
		g_hDeathSound = NULL_AUDIOHANDLE;
	}
	alxPlay( g_pDeathSound );
	
	m_pOwnerObject->setCollisionActive( false, false );
	m_pOwnerObject->setCollisionPhysics( false, false );
	m_pOwnerObject->setCollisionCallback( false );
	
	m_fCurrentHealth = g_fEnemyMaxHealth;
	
	m_pOwnerObject->playAnimation( m_pszDeathAnim, false );
	
	m_bIsDying = true;
	
	// If it has a destination, stop it
	m_pOwnerObject->setPositionTargetOff();
	m_pOwnerObject->getParentPhysics().setGrossLinearVelocity( t2dVector::getZero() );
	
	g_uNumEnemiesKilled++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

