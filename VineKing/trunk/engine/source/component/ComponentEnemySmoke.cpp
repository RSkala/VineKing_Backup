//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySmoke.h"

#include "ComponentEndLevelScreen.h"
//#include "ComponentEnemyManaEaterSpawner.h"
#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"
#include "ComponentSeedMagnetButton.h"

#include "SoundManager.h"

#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 700.0f, 700.0f );

//static const char g_szMoveAnimation[32]	= "MANA_THIEF_MOVE";
static const char g_szMoveAnimation[32]		= "MANA_THIEF_EAT";	// Use the old eat animation for the Smoke move
static const char g_szDeathAnimation[32]	= "MANA_THIEF_DEATH";
static const char g_szDrainAnimation[32]	= "MANA_THIEF_DRAIN";
static const char g_szEatAnimation[32]		= "MANA_THIEF_EAT";
static const char g_szSpawnAnimation[32]	= "MANA_THIEF_SPAWN";

//static const char g_szMoveAnimationSuper[32]	= "SUPER_MANA_THIEF_MOVE";
static const char g_szMoveAnimationSuper[32]	= "SUPER_MANA_THIEF_EAT"; // Use the old eat animation for the Super Smoke move
static const char g_szDeathAnimationSuper[32]	= "SUPER_MANA_THIEF_DEATH";
static const char g_szDrainAnimationSuper[32]	= "SUPER_MANA_THIEF_DRAIN";
static const char g_szEatAnimationSuper[32]		= "SUPER_MANA_THIEF_EAT";
static const char g_szSpawnAnimationSuper[32]	= "SUPER_MANA_THIEF_SPAWN";


static const ColorF g_ColorNormal( 1.0f, 1.0f, 1.0f );
static const ColorF g_ColorSuper( 1.0f, 0.0f, 0.0f );

static const S32 g_iSmokeDamageNormal	= 10;
static const F32 g_fSmokeSpeedNormal	= 32.0f;

static const S32 g_iSmokeDamageSuper	= 10;
static const F32 g_fSmokeSpeedSuper		= 64.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySmoke );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySmoke::CComponentEnemySmoke()
	: m_pOwner( NULL )
	, m_bIsSuperEnemy( false )
	, m_eSmokeState( SMOKE_STATE_NONE )
	, m_bWaitingForStomp( false )
	, m_bKilledBySuicide( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySmoke::~CComponentEnemySmoke()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::SpawnAtPositionWithEndpoint( const t2dVector& _vStartPosition, const t2dVector& _vEndPosition)
{
	if( m_pOwner == NULL )
		return;
	
	m_bKilledBySuicide = false;
	
	//printf( "Spawning Smoke %s at (%f, %f)\n", m_pOwner->getName(), _vStartPosition.mX, _vStartPosition.mY );
	
	m_pOwner->setPosition( _vStartPosition );
	m_pOwner->setVisible( true );
	m_pOwner->setUsesPhysics( true ); // This is now needed for movement
	
	m_vEndPosition = _vEndPosition;
	
	// If this is a Boss level, then use the spawn animations (as they will be spawned on-screen)
	if( CComponentGlobals::GetInstance().IsBossLevel() )
	{
		m_eSmokeState = SMOKE_STATE_SPAWNING;
		m_pOwner->setUseMouseEvents( false );
		
		if( m_bIsSuperEnemy )
			m_pOwner->playAnimation( g_szSpawnAnimationSuper, false );
		else
			m_pOwner->playAnimation( g_szSpawnAnimation, false );
	}
	else
	{
		m_eSmokeState = SMOKE_STATE_MOVING;
		m_pOwner->setUseMouseEvents( true );
		
		if( m_bIsSuperEnemy )
			m_pOwner->playAnimation( g_szMoveAnimationSuper, false );
		else
			m_pOwner->playAnimation( g_szMoveAnimation, false );
		
		if( m_bIsSuperEnemy )
			m_pOwner->moveTo( m_vEndPosition, g_fSmokeSpeedSuper, true, true, false, 0.2f );
		else
			m_pOwner->moveTo( m_vEndPosition, g_fSmokeSpeedNormal, true, true, false, 0.2f );
	}
	
	CSoundManager::GetInstance().PlaySmokeSpawnSound();
	
//	// Check for Smoke tutorial
//	if( CComponentInGameTutorialObject::DoesLevelHaveSmokeTutorial() )
//	{
//		if( m_bIsSuperEnemy == false )
//		{
//			if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SMOKE ) == false )
//			{
//				CComponentInGameTutorialObject::ActivateSmokeTutorial();
//				CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SMOKE );
//			}
//		}
//	}
//	else if( CComponentInGameTutorialObject::DoesLevelHaveSuperSmokeTutorial() )
//	{
//		if( m_bIsSuperEnemy )
//		{
//			if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SMOKE ) == false )
//			{
//				CComponentInGameTutorialObject::ActivateSmokeTutorial();
//				CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SMOKE );
//			}
//		}
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::SpawnSuperAtPositionWithEndpoint( const t2dVector& _vStartPosition, const t2dVector& _vEndPosition )
{
	m_bIsSuperEnemy = true;
	
	//if( m_pOwner )
	//	m_pOwner->setBlendColour( g_ColorSuper );
	
	SpawnAtPositionWithEndpoint( _vStartPosition, _vEndPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::Kill()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eSmokeState == SMOKE_STATE_NONE || m_eSmokeState == SMOKE_STATE_DYING )
		return;
	
	CComponentGlobals::GetInstance().PlaySmokeHitEffectAtPosition( m_pOwner->getPosition() );
	
	m_bIsSuperEnemy = false;
	
	//printf( "Killing Mana Eater %s\n", m_pOwner->getName() );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szDeathAnimationSuper, false );
	else
		m_pOwner->playAnimation( g_szDeathAnimation, false );
	   
	
	m_eSmokeState = SMOKE_STATE_DYING;
	
	m_pOwner->setUseMouseEvents( false );
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	CSoundManager::GetInstance().PlaySmokeDeathSound();
	
	CComponentScoringManager::GetInstance().IncrementNumKilledSmokeMonster();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::KillFromStomp()
{
	if( m_pOwner == NULL )
		return;
	
	//if( m_eSmokeState == SMOKE_STATE_NONE || m_eSmokeState == SMOKE_STATE_DYING )
	//	return;
	
	//CComponentGlobals::GetInstance().PlaySmokeHitEffectAtPosition( m_pOwner->getPosition() );
	
	m_bIsSuperEnemy = false;
	
	//printf( "Killing Mana Eater %s\n", m_pOwner->getName() );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szDeathAnimationSuper, false );
	else
		m_pOwner->playAnimation( g_szDeathAnimation, false );
	
	
	m_eSmokeState = SMOKE_STATE_DYING;
	
	m_pOwner->setUseMouseEvents( false );
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	//CSoundManager::GetInstance().PlaySmokeDeathSound();
	
	CComponentScoringManager::GetInstance().IncrementNumKilledSmokeMonster();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySmoke::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySmoke ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySmoke::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "ComponentEnemySmoke::onComponentAdd - Must be added to a t2dAnimatedSprite." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
	{
		//if( CComponentEndLevelScreen::IsLevelWon() )
		//{
		//	if( m_eSmokeState != SMOKE_STATE_DYING )
		//		Kill();
		//}
		//else
		if( m_eSmokeState != SMOKE_STATE_NONE && m_eSmokeState != SMOKE_STATE_DYING )
		{
			if( m_bWaitingForStomp == false )
			{
				// The level has ended, but the player has lost. Make sure these enemies stop moving.
				m_eSmokeState = SMOKE_STATE_NONE;
				m_pOwner->setPositionTargetOff();
				m_pOwner->setLinearVelocity( t2dVector::getZero() );
				
				m_bWaitingForStomp = true;
			}
		}
	}
	
	switch( m_eSmokeState )
	{
		case SMOKE_STATE_SPAWNING:
			UpdateStateSpawning();
			break;
			
		case SMOKE_STATE_MOVING:
			UpdateStateMoving();
			break;
			
		case SMOKE_STATE_DYING:
			UpdateStateDying();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::HandleOwnerPositionTargetReached()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_bIsSuperEnemy )
		CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_iSmokeDamageSuper );
	else
		CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_iSmokeDamageNormal );
	
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	m_pOwner->setVisible( false );
	
	m_bKilledBySuicide = true;
	
	Kill();
	
	m_pOwner->setPosition( g_vSafePosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPlayerHomeBase::GetInstance().DoesPlayerHomeBaseHaveAttackTarget() )
	{
		// Do nothing if the player home base is already attacking a target
		return;
	}
	
	if( CComponentGlobals::GetInstance().IsTouchingDown() )
		return;
	
	if( CComponentSeedMagnetButton::IsPressed() )
		return;
	
	CComponentGlobals::GetInstance().SetTouchedObject( m_pOwner );
	
	CComponentGlobals::GetInstance().SetPlayerTouchingEnemy();
	
	//CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackSmokeMonster( m_pOwner, this ); // TODO: ALL REFERENCES TO OLD SMOKE MUST BE CHANGED
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackSmoke( m_pOwner, this );
	
	//Kill();
	//CComponentGlobals::GetInstance().PlayHitEffectAtPosition( m_pOwner->getPosition() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setVisible( false );
	m_pOwner->setPosition( g_vSafePosition );
	m_pOwner->setUsesPhysics( true ); // This is needed for movement
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private Member Functions

void CComponentEnemySmoke::UpdateStateSpawning()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.isAnimationFinished() )
	{
		m_eSmokeState = SMOKE_STATE_MOVING;
		m_pOwner->setUseMouseEvents( true );
		m_pOwner->setUsesPhysics( true );
		
		if( m_bIsSuperEnemy )
			m_pOwner->playAnimation( g_szMoveAnimationSuper, false );
		else
			m_pOwner->playAnimation( g_szMoveAnimation, false );
		
		if( m_bIsSuperEnemy )
			m_pOwner->moveTo( m_vEndPosition, g_fSmokeSpeedSuper, true, true, false, 0.2f );
		else
			m_pOwner->moveTo( m_vEndPosition, g_fSmokeSpeedNormal, true, true, false, 0.2f );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::UpdateStateMoving()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmoke::UpdateStateDying()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.isAnimationFinished() == false )
		return;
	
	// Hide this enemy
	m_pOwner->setVisible( false );
	
	if( m_bKilledBySuicide == false )
	{
		// Spawn a mana seed at the position where this enemy died
		CComponentManaSeedItemDrop::SpawnSeedFromEnemyAtPosition( m_pOwner->getPosition() );
	}
	
	m_pOwner->setPosition( g_vSafePosition );
	
	//m_pOwner->setBlendColour( g_ColorNormal );
	m_bIsSuperEnemy = false;
	
	m_eSmokeState = SMOKE_STATE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------