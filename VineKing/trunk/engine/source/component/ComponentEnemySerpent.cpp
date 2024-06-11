//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySerpent.h"

#include "ComponentEnemySpawnPosition.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPauseScreen.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 800.0f, 800.0f );

// Serpent Animation
static const char g_szSerpentSpawnAnim[16]	= "SERPENT_SPAWN";
static const char g_szSerpentIdleAnim[16]	= "SERPENT_IDLE";
static const char g_szSerpentAttackAnim[16]	= "SERPENT_ATTACK";
static const char g_szSerpentDeathAnim[16]	= "SERPENT_DEATH";

static const char g_szSerpentSpawnAnimSuper[32]		= "SUPER_SERPENT_SPAWN";
static const char g_szSerpentIdleAnimSuper[32]		= "SUPER_SERPENT_IDLE";
static const char g_szSerpentAttackAnimSuper[32]	= "SUPER_SERPENT_ATTACK";
static const char g_szSerpentDeathAnimSuper[32]		= "SUPER_SERPENT_DEATH";

static const U32 g_uSerpentAttackAnimFrame = 4;

// Normal Serpent / Super Serpent values
static const ColorF g_ColorNormal( 1.0f, 1.0f, 1.0f );
static const ColorF g_ColorSuper( 1.0f, 0.0f, 0.0f );

static const F32 g_fSpitDelayTimeNormal = 1.0; // 0.5f;		// In Seconds
static const F32 g_fSpitDelayTimeSuper	= 0.5; // 0.25f;	// In Seconds

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static void _SetLayerOrder( t2dSceneObject* _pObject );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySerpent );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySerpent::CComponentEnemySerpent()
	: m_pOwner( NULL )
	, m_eSerpentState( SERPENT_STATE_INACTIVE )
	, m_bAttackedDuringThisAnim( false )
	, m_fSpitTimer( 0.0f )
	, m_bHasSpitAttackPosition( false )
	, m_bIsSuperEnemy( false )
	, m_fSpitDelayTime( g_fSpitDelayTimeNormal )
	, m_pSpawnPosition( NULL )
	, m_bWaitingForStomp( false )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::HandleSerpentStateNone()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( true );
	
	SwitchSerpentState( SERPENT_STATE_SPAWNING );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szSerpentSpawnAnimSuper, false );
	else
		m_pOwner->playAnimation( g_szSerpentSpawnAnim, false );
	
	CSoundManager::GetInstance().PlayGenericSpawnSound();
	
	DetermineOwnerFlip();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::HandleSerpentStateSpawning()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.isAnimationFinished() )
	{
		m_pOwner->setUseMouseEvents( true ); // Allows the object to receive mouse events when touched
		
		if( m_bIsSuperEnemy )
			m_pOwner->playAnimation( g_szSerpentIdleAnimSuper, false );
		else
			m_pOwner->playAnimation( g_szSerpentIdleAnim, false );
		
		m_eSerpentState = SERPENT_STATE_SPITTING;
		m_fSpitTimer = m_fSpitDelayTime;
		
//		if( CComponentInGameTutorialObject::DoesLevelHaveSerpentTutorial() )
//		{
//			if( m_bIsSuperEnemy == false )
//			{
//				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SERPENT ) == false )
//				{
//					CComponentInGameTutorialObject::ActivateTutorial();
//					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SERPENT );
//				}
//			}
//		}
//		else if( CComponentInGameTutorialObject::DoesLevelHaveSuperSerpentTutorial() )
//		{
//			if( m_bIsSuperEnemy )
//			{
//				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SERPENT ) == false )
//				{
//					CComponentInGameTutorialObject::ActivateTutorial();
//					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SERPENT );
//				}
//			}
//		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::HandleSerpentStateDeath()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.isAnimationFinished() )
	{
		// This is the new method without deleting the enemy
		t2dVector vEnemyPosition = m_pOwner->getPosition();
		m_eSerpentState = SERPENT_STATE_INACTIVE;
		m_pOwner->setVisible( false );
		m_pOwner->setPosition( g_vSafePosition );
		
		m_bIsSuperEnemy = false;
		//m_pOwner->setBlendColour( g_ColorNormal );
		m_fSpitDelayTime = g_fSpitDelayTimeNormal;
		
		if( m_pSpawnPosition )
		{
			m_pSpawnPosition->SetSpawnPositionNotInUse();
			m_pSpawnPosition = NULL;
		}
		
		// Spawn a mana seed at the position where this enemy died
		CComponentManaSeedItemDrop::SpawnSeedFromEnemyAtPosition( vEnemyPosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::SwitchSerpentState( const ESerpentState _eSerpentState )
{ 
	m_eSerpentState = _eSerpentState; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::Kill()
{
	if( m_pOwner == NULL )
		return;
	
	// Turn off mouse events for the owner
	m_pOwner->setUseMouseEvents( false );
	
	// Start Death for owner
	SwitchSerpentState( SERPENT_STATE_DEATH );
	
	// Stop the movement of the owner
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szSerpentDeathAnimSuper, false );
	else
		m_pOwner->playAnimation( g_szSerpentDeathAnim, false );
	
	// Play the death sound
	CSoundManager::GetInstance().PlaySerpentDeathSound();
	CComponentScoringManager::GetInstance().IncrementNumKilledEnemyB();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::KillFromStomp()
{
	if( m_pOwner == NULL )
		return;
	
	// Turn off mouse events for the owner
	m_pOwner->setUseMouseEvents( false );
	
	// Start Death for owner
	SwitchSerpentState( SERPENT_STATE_DEATH );
	
	// Stop the movement of the owner
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szSerpentDeathAnimSuper, false );
	else
		m_pOwner->playAnimation( g_szSerpentDeathAnim, false );
	
	// Play the death sound
	//CSoundManager::GetInstance().PlaySerpentDeathSound();
	CComponentScoringManager::GetInstance().IncrementNumKilledEnemyB();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::SpawnAtPosition( CComponentEnemySpawnPosition* const _pSpawnPosition )
{
	if( m_pOwner == NULL )
		return;
	
	t2dVector vPosition( 0.0f, 0.0f );
	m_pSpawnPosition = _pSpawnPosition;
	if( m_pSpawnPosition )
	{
		m_pSpawnPosition->GetOwnerPosition( vPosition );
		m_pSpawnPosition->SetSpawnPositionInUse();
	}
	
	m_pOwner->setPosition( vPosition );
	m_pOwner->setVisible( true );
	
	m_eSerpentState = SERPENT_STATE_NONE;
	
	m_bAttackedDuringThisAnim = false;
	m_fSpitTimer = 0.0f;
	m_bHasSpitAttackPosition = false;
	
	_SetLayerOrder( m_pOwner );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::SpawnSuperAtPosition( CComponentEnemySpawnPosition* const _pSpawnPosition )
{
	if( m_pOwner == NULL )
		return;
	
	m_bIsSuperEnemy = true;
	//m_pOwner->setBlendColour( g_ColorSuper );
	m_fSpitDelayTime = g_fSpitDelayTimeSuper;
	
	SpawnAtPosition( _pSpawnPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySerpent::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is necessary for the script to be able to access %behavior.Owner, which in turn adds the component like this:  %behavior.Owner.Component = new CComponentAStarActor();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySerpent ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySerpent::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySerpent::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	// Store the owner
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::onUpdate()
{
	if( m_pOwner == NULL || m_eSerpentState == SERPENT_STATE_INACTIVE )
		return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
	{
		//if( CComponentEndLevelScreen::GetInstance().IsLevelWon() )
		//{
		//	if( m_eSerpentState != SERPENT_STATE_DEATH )
		//		Kill(); // RKS TODO: Use a specific 'level has ended' kill function
		//}
		//else
		if( m_bWaitingForStomp == false )
		{
			// The level has ended, but the player has lost. Make sure these enemies stop attacking.
			m_eSerpentState = SERPENT_STATE_INACTIVE;
			
			if( m_bIsSuperEnemy )
				m_pOwner->playAnimation( g_szSerpentIdleAnimSuper, false );
			else
				m_pOwner->playAnimation( g_szSerpentIdleAnim, false );
			
			m_bWaitingForStomp = true;
		}
	}
	
	//UpdateSpitting();
	
	// Handle the appropriate action, depending on the AStar actor's current state
	switch( m_eSerpentState )
	{
		case SERPENT_STATE_NONE:
			HandleSerpentStateNone();
			break;
			
		case SERPENT_STATE_SPAWNING:
			HandleSerpentStateSpawning();
			break;
			
		case SERPENT_STATE_SPITTING:
			UpdateSpitting();
			break;
			
		case SERPENT_STATE_DEATH:
			HandleSerpentStateDeath();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( m_pOwner == NULL )
		return;
	
	if( m_eSerpentState == SERPENT_STATE_DEATH )
		return;
	
	if( CComponentPlayerHomeBase::GetInstance().DoesPlayerHomeBaseHaveAttackTarget() )
	{
		// Do nothing if the player home base is already attacking a target
		return;
	}
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
		return;
	
	////////////////////////////////////////////////////////////////////////////////////
	// NEW TOUCHING STUFF
	if( CComponentGlobals::GetInstance().IsTouchingDown() )
		return;
	
	CComponentGlobals::GetInstance().SetTouchedObject( m_pOwner );
	////////////////////////////////////////////////////////////////////////////////////
	
	CComponentGlobals::GetInstance().SetPlayerTouchingEnemy();
	//CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttack( m_pOwner, this );
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackSerpent( m_pOwner, this );
	
	CComponentGlobals::GetInstance().PlayHitEffectAtPosition( m_pOwner->getPosition() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	CComponentGlobals::GetInstance().SetPlayerNotTouchingEnemy();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::OnPostInit()
{
	if( m_pOwner )
		m_pOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::DetermineOwnerFlip()
{
	if( m_pOwner == NULL )
		return;
	
	// Determine the facing direction of the owner
	t2dVector vHomeBasePosition;
	CComponentPlayerHomeBase::GetInstance().GetHomeBasePosition( vHomeBasePosition );
	
	t2dVector vOwnerPosition = m_pOwner->getPosition();
	
	// In the rare case that the owner position and the home base position are the same, do nothing
	if( vOwnerPosition.isEqual( vHomeBasePosition ) == false )
	{
		t2dVector vDirection = vHomeBasePosition - vOwnerPosition;
		t2dVector vRight( 1.0f, 0.0f );
		
		F32 fDot = vRight.dot( vDirection );
		if( fDot > 0.0f )
		{
			// Direction is to the right, so set the flip to true
			m_pOwner->setFlipX( true );
		}
		else if( fDot < 0.0f )
		{
			// Direction is to the left, so set the flip to false
			m_pOwner->setFlipX( false );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySerpent::UpdateSpitting()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eSerpentState == SERPENT_STATE_DEATH )
		return;
	
	m_fSpitTimer += ITickable::smTickSec;
	if( m_fSpitTimer >= m_fSpitDelayTime )
	{
		if( m_bHasSpitAttackPosition == false )
		{
			if( CComponentPathGridHandler::GetInstance().GetRandomAttackableLandBrickPosition( m_vSpitAttackPosition ))
			{
				// A spit attack position has been found.  Start the spit attack animation.
				m_bHasSpitAttackPosition = true;
				
				if( m_bIsSuperEnemy )
					m_pOwner->playAnimation( g_szSerpentAttackAnimSuper, false );
				else
					m_pOwner->playAnimation( g_szSerpentAttackAnim, false );
				
				CSoundManager::GetInstance().PlaySerpentSpitSound();
			}
			else 
			{
				// There are no attack positions (There are most likely no grass tiles). Reset the attack timer.
				m_fSpitTimer = 0.0f;
				m_bHasSpitAttackPosition = false;
				m_bAttackedDuringThisAnim = false;
			}
		}
		else 
		{
			// A spit attack position was found and the attack animation was started. Check for the attack frame.
			if( m_pOwner->mAnimationController.getCurrentFrame() == g_uSerpentAttackAnimFrame && m_bAttackedDuringThisAnim == false )
			{
				CComponentGlobals::GetInstance().FireBrickAttackProjectileAtPosition( m_pOwner->getPosition(), m_vSpitAttackPosition );
				CComponentPathGridHandler::GetInstance().SetLandBrickAtPositionAsFiredUpon( m_vSpitAttackPosition );
				m_bAttackedDuringThisAnim = true;
			}
			else 
			{
				// The spitter has attacked. Now wait for the animation to finish.
				if( m_pOwner->mAnimationController.isAnimationFinished() )
				{
					// The attack animation has finished. Return to the idle animation.
					if( m_bIsSuperEnemy )
						m_pOwner->playAnimation( g_szSerpentIdleAnimSuper, false );
					else
						m_pOwner->playAnimation( g_szSerpentIdleAnim, false );
					
					m_fSpitTimer = 0.0f;
					m_bHasSpitAttackPosition = false;
					m_bAttackedDuringThisAnim = false;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------


void _SetLayerOrder( t2dSceneObject* _pObject )
{
	if( _pObject == NULL )
		return;
	
	// Layer Orders go from 0 - 480
	
	S32 iNewLayerOrder = static_cast<S32>( _pObject->getPosition().mY + 240.0f );
	if( iNewLayerOrder != _pObject->getLayerOrder() )
		_pObject->setLayerOrder( iNewLayerOrder );
}
