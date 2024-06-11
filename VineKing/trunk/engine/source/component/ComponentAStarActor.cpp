//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentAStarActor.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentEnemySpawner.h"
#include "ComponentGlobals.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPauseScreen.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"
#include "ComponentStaticPath.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dTileMap.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PATH_GRID_ID_NONE (0)
#define PATH_NODE_INDEX_INVALID (~0)

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 600.0f, 600.0f );

static const F32 g_fAStarActorMovementSpeed = 20.0f; //10.0f;		// Movement speed of the AStar actor
static const F32 g_fAStarActorTargetPositionMargin = 0.1f;	// The amount of slack in order to consider a target position "reached"
static bool g_bPathDebugOutputEnabled = false;

// Variables to use when calling t2dSceneObject::moveTo()
static const bool g_bActorPathAutoStop = true;			// Whether to stop when the path is reached
static const bool g_bActorPathUseCallback = true;		// Call OnPositionTargetReached() and script "onPositionTarget" when the destination is reached
static const bool g_bActorPathSnapToPosition = false;	// Snap to the position when within an adequate range
static const F32 g_fActorPathTargetMargin = 0.5f;		// The amount of slack in order to consider a target position "reached"	

static const U32 g_uEnemyANumBitesToDestroyTile = 4;
static const U32 g_uEnemyABiteFrame = 4; //10; // The frame at which the enemy should attack the current tile

// Spawn Warning stuff (RKS TODO: Move this into the editor)
static const U32 g_uMaxNumSpawnWarnings = 1;
static const char g_szSpawnWarningAnim[24] = "BLOB_pre_SPAWN"; // "Anim_SpawnSteam01";

// Globals related to spitter enemies
static const F32 g_fSpitDelayTimeMS = 1000.0f; //2000.0f; //5000.0f; // Time between spitting

// Blob Animation
static const char g_szTileAttackAnimName[16] = "BLOB_BARF";

// Serpent Animation
static const char g_szSerpentIdleAnimName[16] = "SERPENT_IDLE";
static const char g_szSerpentAttackAnimName[16] = "SERPENT_ATTACK";
static const U32 g_uSerpentAttackAnimFrame = 4;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentAStarActor* CComponentAStarActor::sm_pLastSpawnedWaveEnemy = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static void _SetLayerOrder( t2dSceneObject* _pObject );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentAStarActor );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentAStarActor::CComponentAStarActor()
	: m_pAnimatedSpriteOwner( NULL )
	, m_pszTileLayerName( NULL )
	, m_pTileLayer( NULL )
	, m_pszSpawnAnimName( NULL )
	, m_pszMoveAnimName( NULL )
	, m_pszAttackAnimName( NULL )
	, m_pszDeathAnimName( NULL )
	, m_pszStunAnimName( NULL )
	, m_uCurrentPathID( 0 )
	, m_uCurrentPathNodeIndex( PATH_NODE_INDEX_INVALID )
	, m_pSpawnedByComponent( NULL )
	, m_eAStarActorState( ASTAR_ACTOR_STATE_INACTIVE )
	, m_pszAttackVulnerabilityType( NULL )
	, m_eAttackVulnerabilityType( VULNERABILITY_TYPE_NONE )
	, m_pszAttackBehaviorType( NULL )
	, m_eAttackBehaviorType( ATTACK_BEHAVIOR_TYPE_NONE )
	, m_ePreviousActorState( ASTAR_ACTOR_STATE_INACTIVE )
	, m_eLastMovementDirection( MOVEMENT_DIRECTION_NONE )
	, m_iTileAttackPower( 0 )
	, m_fPlayerAttackPower( 0.0f )
	, m_bAttackedDuringThisAnim( false )
	, m_fMovementSpeed( 0.0f )
	, m_bIsAttackingPlayer( false )
	, m_uNumSpawnWarnings( 0 )
	, m_fSpitTimer( 0.0f )
	, m_iNumPathNodes( 0 )
	, m_pStaticPath( NULL )
	, m_iCurrentStaticPathIndex( -1 )
	, m_bIsDying( false )
	, m_bHasSpitAttackPosition( false )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleActorStateNone()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	PickTileAtCurrentBottomPosition( m_currentTilePoint );
	
	m_pAnimatedSpriteOwner->setUseMouseEvents( true );
	
	SwitchAStarActorState( ASTAR_ACTOR_STATE_SPAWNING );
	//m_pAnimatedSpriteOwner->mAnimationController.resetTime();
	m_pAnimatedSpriteOwner->playAnimation( m_pszSpawnAnimName, false );
	
	CSoundManager::GetInstance().PlayGenericSpawnSound();
	
	//SwitchAStarActorState( ASTAR_ACTOR_STATE_SPAWN_WARNING );
	//m_pAnimatedSpriteOwner->mAnimationController.resetTime();
	//m_pAnimatedSpriteOwner->playAnimation( g_szSpawnWarningAnim, false );
	
	m_uNumSpawnWarnings++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleActorStateSpawnWarning()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() )
	{
		if( m_uNumSpawnWarnings >= g_uMaxNumSpawnWarnings )
		{
			if( CComponentAStarActor::sm_pLastSpawnedWaveEnemy == this )
				CSoundManager::GetInstance().PlayGenericSpawnSound();
			
			// Go to the Spawn state
			SwitchAStarActorState( ASTAR_ACTOR_STATE_SPAWNING );
			m_pAnimatedSpriteOwner->mAnimationController.resetTime();
			m_pAnimatedSpriteOwner->playAnimation( m_pszSpawnAnimName, false );
		}
		else
		{
			// Repeat the "Spawn Warning" anim
			m_pAnimatedSpriteOwner->mAnimationController.resetTime();
			m_pAnimatedSpriteOwner->playAnimation( g_szSpawnWarningAnim, false );
			m_uNumSpawnWarnings++;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleActorStateSpawning()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() )
	{
		m_pAnimatedSpriteOwner->setUsesPhysics( true ); // RKS FUCK TODO: See if this fixes why enemies do not move
		SwitchAStarActorState( ASTAR_ACTOR_STATE_MOVING );
		m_pAnimatedSpriteOwner->playAnimation( m_pszMoveAnimName, false );
		m_pAnimatedSpriteOwner->setUseMouseEvents( true ); // Allows the object to receive mouse events when touched
		MoveToNextPathNode();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleActorStateMoving()
{
	HandleMovingWithoutPathObject();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleMovingWithoutPathObject()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleActorStateAttacking()
{
	AssertFatal( m_eAStarActorState == ASTAR_ACTOR_STATE_ATTACKING, "HandleActorStateAttacking(): Not in correct state." );
	
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_pAnimatedSpriteOwner->mAnimationController.getCurrentFrame() == g_uEnemyABiteFrame && m_bAttackedDuringThisAnim == false )
	{
		if( m_bIsAttackingPlayer )
		{
			// Deal Damage to the player
			//printf( "Dealing %f damage to Home Base\n", m_fPlayerAttackPower );
			CComponentPlayerHomeBase::GetInstance().DamageHomeBase( m_fPlayerAttackPower );
			m_bAttackedDuringThisAnim = true;
		}
		else
		{
			if( CComponentPathGridHandler::GetInstance().IsLandBrickAttackable( m_currentTilePoint.x, m_currentTilePoint.y ) )
			{
				// Attack the tile
				CComponentPathGridHandler::GetInstance().DealDamageToLandBrick( m_currentTilePoint.x, m_currentTilePoint.y, m_iTileAttackPower );
				
				m_bAttackedDuringThisAnim = true;
			}
		}
	}
	else if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() )
	{
		if( m_bIsAttackingPlayer )
		{
			m_pAnimatedSpriteOwner->playAnimation( m_pszAttackAnimName, false );
			
			CSoundManager::GetInstance().PlayBlobAttackPlayerSound();
		}
		else if( CComponentPathGridHandler::GetInstance().IsLandBrickAttackable( m_currentTilePoint.x, m_currentTilePoint.y ) == false )
		{
			// Change state to moving
			SwitchAStarActorState( ASTAR_ACTOR_STATE_MOVING );
			m_pAnimatedSpriteOwner->playAnimation( m_pszMoveAnimName, false );
			
			MoveToNextPathNode();
		}
		else
		{
			// The tile is not yet destroyed, so play the attack animation again
			m_pAnimatedSpriteOwner->playAnimation( g_szTileAttackAnimName, false );
			
			CSoundManager::GetInstance().PlayBlobAttackTileSound();
		}
		
		m_bAttackedDuringThisAnim = false;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleActorStateDeath()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() )
	{
		// This is the new method without deleting the enemy
		t2dVector vEnemyPosition = m_pAnimatedSpriteOwner->getPosition();
		m_eAStarActorState = ASTAR_ACTOR_STATE_INACTIVE;
		m_pAnimatedSpriteOwner->setVisible( false );
		m_pAnimatedSpriteOwner->setPosition( g_vSafePosition );
		NotifySpawnerOfDeath();
		
		
//		t2dVector vEnemyPosition = m_pAnimatedSpriteOwner->getPosition();
//		
//		// Decrement the spawn count of the component that spawned this AStar Actor
//		if( m_pSpawnedByComponent )
//		{
//			//m_pSpawnedByComponent->DecrementSpawnCount();
//			NotifySpawnerOfDeath();
//			m_pAnimatedSpriteOwner->safeDelete();
//		}
//		
//		m_pAnimatedSpriteOwner = NULL;
		
		// Spawn a mana seed at the position where this enemy died
		CComponentManaSeedItemDrop::SpawnSeedFromEnemyAtPosition( vEnemyPosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::SwitchAStarActorState( const EAStarActorState _eAStarActorState )
{ 
	m_ePreviousActorState = m_eAStarActorState;
	m_eAStarActorState = _eAStarActorState; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::OnOwnerCurrentPathComplete()
{
	if( g_bPathDebugOutputEnabled )
		printf( "OnOwnerCurrentPathComplete - Owner: %s\n", m_pAnimatedSpriteOwner->getName() );
	
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	// Set the current path node index to be invalid
	m_uCurrentPathNodeIndex = PATH_NODE_INDEX_INVALID;
	
	// Make the owner stop moving
	m_pAnimatedSpriteOwner->setLinearVelocity( t2dVector::getZero() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::KillActor()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	m_bIsDying = true;
	
	// Turn off mouse events for the owner
	m_pAnimatedSpriteOwner->setUseMouseEvents( false );
	
	// Start Death for owner
	SwitchAStarActorState( ASTAR_ACTOR_STATE_DEATH );
	
	// Stop the movement of the owner
	m_pAnimatedSpriteOwner->setPositionTargetOff();
	m_pAnimatedSpriteOwner->setLinearVelocity( t2dVector::getZero() );
	
	if( m_pszDeathAnimName )
		m_pAnimatedSpriteOwner->playAnimation( m_pszDeathAnimName, false );
	
	// Play the death sound
	if( m_pSpawnedByComponent )
	{
		if( m_pSpawnedByComponent->IsEnemyTypeA() ) // Blob
		{
			CSoundManager::GetInstance().PlayBlobDeathSound();
			
			CComponentScoringManager::GetInstance().IncrementNumKilledEnemyA();
		}
		else if( m_pSpawnedByComponent->IsEnemyTypeB() ) // Serpent
		{
			CSoundManager::GetInstance().PlaySerpentDeathSound();
			
			CComponentScoringManager::GetInstance().IncrementNumKilledEnemyB();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::SpawnWithPath( CComponentStaticPath* const _pStaticPath )
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	m_iCurrentStaticPathIndex = 0;
	m_pStaticPath = _pStaticPath;
	
	m_pAnimatedSpriteOwner->setVisible( true );
	
	m_eAStarActorState = ASTAR_ACTOR_STATE_NONE;
	
	if( m_pStaticPath )
	{
		// If this enemy has a path, then set its initial position to be the first node.
		t2dVector vStartNodePosition;
		m_pStaticPath->GetPositionAtNodeIndex( m_iCurrentStaticPathIndex, vStartNodePosition );
		
		// Offset the position upwards by half the enemy's height, so the enemy appears visually to be standing on the center of the tile.
		F32 fPositionYBottomOffset = (m_pAnimatedSpriteOwner->getSize().mY * 0.5f);
		vStartNodePosition.mY -= fPositionYBottomOffset;
		
		m_pAnimatedSpriteOwner->setPosition( vStartNodePosition );
	}
	
	m_bIsDying = false;
	m_bIsAttackingPlayer = false;
	m_bAttackedDuringThisAnim = false;
	m_fSpitTimer = 0.0f;
	m_bHasSpitAttackPosition = false;
	m_uNumSpawnWarnings = 0;
	
	_SetLayerOrder( m_pAnimatedSpriteOwner );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::SpawnWithoutPath()
{
	SpawnWithPath( NULL );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::NotifySpawnerOfDeath()
{
	if( m_pSpawnedByComponent )
		m_pSpawnedByComponent->SpawnedEnemyDeathNotify();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::SetOwnerPosition( const t2dVector & _vNewPosition )
{
	if( m_pAnimatedSpriteOwner )
		m_pAnimatedSpriteOwner->setPosition( _vNewPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::GetOwnerPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = ( m_pAnimatedSpriteOwner ? m_pAnimatedSpriteOwner->getPosition() : t2dVector::getZero() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAStarActor::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is necessary for the script to be able to access %behavior.Owner, which in turn adds the component like this:  %behavior.Owner.Component = new CComponentAStarActor();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pAnimatedSpriteOwner, CComponentAStarActor ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "tileLayer", TypeString, Offset( m_pszTileLayerName, CComponentAStarActor ) );
	
	addField( "SpawnAnim",	TypeString, Offset( m_pszSpawnAnimName,		CComponentAStarActor ) );
	addField( "MoveAnim",	TypeString, Offset( m_pszMoveAnimName,		CComponentAStarActor ) );
	addField( "AttackAnim", TypeString, Offset( m_pszAttackAnimName,	CComponentAStarActor ) );
	addField( "DeathAnim",	TypeString, Offset( m_pszDeathAnimName,		CComponentAStarActor ) );
	addField( "StunAnim",	TypeString, Offset( m_pszStunAnimName,		CComponentAStarActor ) );
	
	//%attackVulnerabilityTypes = "Tap" TAB "Swipe" TAB "Tap Then Swipe" TAB "Tap Then Shake";
	addField( "AttackVulnerability", TypeString, Offset( m_pszAttackVulnerabilityType, CComponentAStarActor ) );
	
	//%attackBehaviorTypes		= "Move To Next Tile" TAB "Move To Player" TAB "Fly To Player";
	addField( "AttackBehavior", TypeString, Offset( m_pszAttackBehaviorType, CComponentAStarActor ) );
	
	addField( "TileAttackPower",	TypeS32, Offset( m_iTileAttackPower,		CComponentAStarActor ) );
	addField( "PlayerAttackPower",	TypeF32, Offset( m_fPlayerAttackPower,	CComponentAStarActor ) );
	addField( "MovementSpeed",		TypeF32, Offset( m_fMovementSpeed,			CComponentAStarActor ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentAStarActor::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentAStarActor::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	// Store the owner
	m_pAnimatedSpriteOwner = pOwnerObject;
	
	// Convert the script string types to the C++ enum types
	ConvertStringAttackVulnerabilityToEnum();
	ConvertStringAttackBehaviorToEnum();
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::onUpdate()
{
	if( m_pAnimatedSpriteOwner == NULL || m_pSpawnedByComponent == NULL || m_eAStarActorState == ASTAR_ACTOR_STATE_INACTIVE )
		return;
	
	//if( m_bIsDying )
	//	return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
	{
		if( CComponentEndLevelScreen::GetInstance().IsLevelWon() )
		{
			if( m_bIsDying == false )
				KillActor(); // RKS TODO: Use a specific 'level has ended' kill function
		}
		else
		{
			// The level has ended, but the player has lost. Make sure these enemies stop moving.
			m_eAStarActorState = ASTAR_ACTOR_STATE_INACTIVE;
			m_pAnimatedSpriteOwner->setPositionTargetOff();
			m_pAnimatedSpriteOwner->setLinearVelocity( t2dVector::getZero() );
			
			// Should we play the idle animation here?
			if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_MOVE_TO_NEXT_TILE )
			{
				// Blob
				m_pAnimatedSpriteOwner->playAnimation( m_pszMoveAnimName, false );
			}
			else
			{
				// Serpent
				m_pAnimatedSpriteOwner->playAnimation( g_szSerpentIdleAnimName, false );
			}
		}
	}
	
	// Update the layer order (so it will appear in the correct spatial order)
	if( m_eAStarActorState != ASTAR_ACTOR_STATE_NONE && m_eAttackBehaviorType != ATTACK_BEHAVIOR_TYPE_STATIONARY )
		_SetLayerOrder( m_pAnimatedSpriteOwner );

	// Get the Tile Layer
	if( m_pTileLayer == NULL )
	{
		// Find the Tile Layer object
		if( m_pszTileLayerName != NULL )
			m_pTileLayer = static_cast<t2dTileLayer*>( Sim::findObject( m_pszTileLayerName ) );
	}
	
	// Update Spitting
	if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_STATIONARY )
		UpdateSpitting();
	
	// Handle the appropriate action, depending on the AStar actor's current state
	switch( m_eAStarActorState )
	{
		case ASTAR_ACTOR_STATE_NONE:
			HandleActorStateNone();
			break;
			
		case ASTAR_ACTOR_STATE_SPAWN_WARNING:
			HandleActorStateSpawnWarning();
			break;
			
		case ASTAR_ACTOR_STATE_SPAWNING:
			HandleActorStateSpawning();
			break;
			
		case ASTAR_ACTOR_STATE_MOVING:
			HandleActorStateMoving();
			break;
			
		case ASTAR_ACTOR_STATE_ATTACKING:
			HandleActorStateAttacking();
			break;
			
		case ASTAR_ACTOR_STATE_DEATH:
			HandleActorStateDeath();
			break;
			
		default:
			break;
	}
	
	DetermineOwnerFlip();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::HandleOwnerPositionTargetReached()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( g_bPathDebugOutputEnabled )
		printf( "Target reached, Owner Position (%f, %f)\n", m_pAnimatedSpriteOwner->getPosition().mX, m_pAnimatedSpriteOwner->getPosition().mY );
	
	if( m_pStaticPath == NULL )
		return;
	
	if( PickTileAtCurrentBottomPosition( m_currentTilePoint ) == false )
		return;
	
	if( m_iCurrentStaticPathIndex == m_pStaticPath->GetEndNodeIndex() )
	{
		// This enemy has reached its last path node. Start attacking the home base (the end nodes are always next to the home base).
		m_bIsAttackingPlayer = true;
		
		m_pAnimatedSpriteOwner->setPositionTargetOff();
		m_pAnimatedSpriteOwner->setLinearVelocity( t2dVector::getZero() );
		
		SwitchAStarActorState( ASTAR_ACTOR_STATE_ATTACKING );
		m_pAnimatedSpriteOwner->playAnimation( m_pszAttackAnimName, false );
		m_bAttackedDuringThisAnim = false;
		
		return;
	}
	
	if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_MOVE_TO_NEXT_TILE )
	{
		// Only attack this destination if the tile is in an attackable state (not solid rock).
		if( CComponentPathGridHandler::GetInstance().IsLandBrickAttackable( m_currentTilePoint.x, m_currentTilePoint.y ) )
		{
			SwitchAStarActorState( ASTAR_ACTOR_STATE_ATTACKING );
			//m_pAnimatedSpriteOwner->playAnimation( m_pszAttackAnimName, false );
			m_pAnimatedSpriteOwner->playAnimation( g_szTileAttackAnimName, false );
			m_bAttackedDuringThisAnim = false;
			
			CSoundManager::GetInstance().PlayBlobAttackTileSound();
		}
		else 
		{
			MoveToNextPathNode();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_eAStarActorState == ASTAR_ACTOR_STATE_DEATH )
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
	
	CComponentGlobals::GetInstance().SetTouchedObject( m_pAnimatedSpriteOwner );
	////////////////////////////////////////////////////////////////////////////////////
	
	CComponentGlobals::GetInstance().SetPlayerTouchingEnemy();
	//CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttack( m_pAnimatedSpriteOwner, this );
	
	CComponentGlobals::GetInstance().PlayHitEffectAtPosition( m_pAnimatedSpriteOwner->getPosition() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	CComponentGlobals::GetInstance().SetPlayerNotTouchingEnemy();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( m_eAStarActorState == ASTAR_ACTOR_STATE_DEATH )
		return;
	
	if( CComponentPlayerHomeBase::GetInstance().DoesPlayerHomeBaseHaveAttackTarget() )
	{
		// Do nothing if the player home base is already attacking a target
		return;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::OnPostInit()
{
	if( m_pAnimatedSpriteOwner )
		m_pAnimatedSpriteOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::ConvertStringAttackVulnerabilityToEnum()
{
	if( m_pszAttackVulnerabilityType == NULL )
		return;
	
	//%attackVulnerabilityTypes = "Tap" TAB "Swipe" TAB "Tap Then Swipe" TAB "Tape Then Shake";
	if( strcmp( m_pszAttackVulnerabilityType, "Tap" ) == 0 )
	{
		m_eAttackVulnerabilityType = VULNERABILITY_TYPE_TAP;
	}
	else if( strcmp( m_pszAttackVulnerabilityType, "Swipe" ) == 0 )
	{
		m_eAttackVulnerabilityType = VULNERABILITY_TYPE_SWIPE;
	}
	else if( strcmp( m_pszAttackVulnerabilityType, "Tap Then Swipe" ) == 0 )
	{
		m_eAttackVulnerabilityType = VULNERABILITY_TYPE_TAPTHENSWIPE;
	}
	else if( strcmp( m_pszAttackVulnerabilityType, "Tap Then Shake" ) == 0 )
	{
		m_eAttackVulnerabilityType = VULNERABILITY_TYPE_TAPTHENSHAKE;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::ConvertStringAttackBehaviorToEnum()
{
	if( m_pszAttackBehaviorType == NULL )
		return;
	
	//%attackBehaviorTypes		= "Move To Next Tile" TAB "Move To Player" TAB "Fly To Player";
	
	if( strcmp( m_pszAttackBehaviorType, "Move To Next Tile" ) == 0 )
	{
		m_eAttackBehaviorType = ATTACK_BEHAVIOR_TYPE_MOVE_TO_NEXT_TILE;
	}
	else if( strcmp( m_pszAttackBehaviorType, "Move To Player" ) == 0 )
	{
		m_eAttackBehaviorType = ATTACK_BEHAVIOR_TYPE_MOVE_DIRECTLY_TO_PLAYER;
	}
	else if( strcmp( m_pszAttackBehaviorType, "Fly To Player" ) == 0 )
	{
		m_eAttackBehaviorType = ATTACK_BEHAVIOR_TYPE_FLY_TO_PLAYER;
	}
	else if( strcmp( m_pszAttackBehaviorType, "Stationary" ) == 0 )
	{
		m_eAttackBehaviorType = ATTACK_BEHAVIOR_TYPE_STATIONARY;
	}
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::DetermineOwnerFlip()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	// Determine the facing direction of the owner
	t2dVector vHomeBasePosition;
	CComponentPlayerHomeBase::GetInstance().GetHomeBasePosition( vHomeBasePosition );
	
	t2dVector vOwnerPosition = m_pAnimatedSpriteOwner->getPosition();
	
	// In the rare case that the owner position and the home base position are the same, do nothing
	if( vOwnerPosition.isEqual( vHomeBasePosition ) == false )
	{
		t2dVector vDirection = vHomeBasePosition - vOwnerPosition;
		t2dVector vRight( 1.0f, 0.0f );
		
		F32 fDot = vRight.dot( vDirection );
		if( fDot > 0.0f )
		{
			// Direction is to the right, so set the flip to true
			m_pAnimatedSpriteOwner->setFlipX( true );
		}
		else if( fDot < 0.0f )
		{
			// Direction is to the left, so set the flip to false
			m_pAnimatedSpriteOwner->setFlipX( false );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::DeterminePath()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	t2dVector vOwnerPosition = m_pAnimatedSpriteOwner->getPosition();
	t2dVector vOwnerBottomPosition = vOwnerPosition;
	vOwnerBottomPosition.mY += 32.0f;
	
	if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_STATIONARY )
	{
		m_uCurrentPathID = 666;
		
		t2dVector vAttackPosition;
		//CComponentGlobals::GetInstance().GetNextAttackableLandBrickPosition( vAttackPosition );
		//CComponentGlobals::GetInstance().GetRandomAttackableLandBrickPosition( vAttackPosition );
		//CComponentGlobals::GetInstance().FireBrickAttackProjectileAtPosition( m_pAnimatedSpriteOwner->getPosition(), vAttackPosition );
	}
	else if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_MOVE_TO_NEXT_TILE )
	{
		t2dVector vHomeBasePosition( 0.0f, 0.0f );
		CComponentPlayerHomeBase::GetInstance().GetHomeBasePosition( vHomeBasePosition );
		
		// Get the tile positions of both the enemy and the player
		Point2I playerTilePoint;
		Point2I enemyTilePoint;
		if( m_pTileLayer->pickTile( vHomeBasePosition, playerTilePoint ) &&		// Pick the Player's tile location
			m_pTileLayer->pickTile( m_pAnimatedSpriteOwner->getPosition(), enemyTilePoint ) )			// Pick this enemy's tile location
		{
			if( playerTilePoint == enemyTilePoint )
			{
				// For now, just return when the enemy reaches the player, otherwise GetTileWorldPosition() will Assert
				return;
			}
			
			if( m_eLastMovementDirection == MOVEMENT_DIRECTION_NONE )
			{
				// If the enemy's last direction was MOVEMENT_DIRECTION_NONE, then it has not yet received a path direction
				
				// Use an arbitrary value to determine its first direction
				U32 uDirection = CComponentGlobals::GetInstance().GetRandomRange( 0, 1 );
				if( uDirection == 0 )
					m_eLastMovementDirection = MOVEMENT_DIRECTION_RIGHT;
				else
					m_eLastMovementDirection = MOVEMENT_DIRECTION_UP;
			}
			
			bool bTilePointDestinationSet = false;
			Point2I tilePointDestination;
			
			if( m_eLastMovementDirection == MOVEMENT_DIRECTION_RIGHT || m_eLastMovementDirection == MOVEMENT_DIRECTION_LEFT )
			{
				bTilePointDestinationSet = CheckEnemyDirectionAboveOrBelow( playerTilePoint, enemyTilePoint, tilePointDestination );
				
				if( bTilePointDestinationSet == false )
					bTilePointDestinationSet = CheckEnemyDirectionLeftOrRight( playerTilePoint, enemyTilePoint, tilePointDestination );
			}
			else if( m_eLastMovementDirection == MOVEMENT_DIRECTION_UP || m_eLastMovementDirection == MOVEMENT_DIRECTION_DOWN )
			{
				bTilePointDestinationSet = CheckEnemyDirectionLeftOrRight( playerTilePoint, enemyTilePoint, tilePointDestination );

				if( bTilePointDestinationSet == false )
					bTilePointDestinationSet = CheckEnemyDirectionAboveOrBelow( playerTilePoint, enemyTilePoint, tilePointDestination );
			}
				
			if( bTilePointDestinationSet )
			{
				t2dVector vTilePointDestination;
				CComponentPathGridHandler::GetInstance().GetTileWorldPosition( tilePointDestination.x, tilePointDestination.y, vTilePointDestination );
				m_pAnimatedSpriteOwner->moveTo(	vTilePointDestination, m_fMovementSpeed, g_bActorPathAutoStop, g_bActorPathUseCallback, g_bActorPathSnapToPosition, g_fActorPathTargetMargin );
				m_uCurrentPathID = 666;
			}
		}
	}
	else if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_MOVE_DIRECTLY_TO_PLAYER )
	{
		// Just go directly to the home base
		t2dVector vHomeBasePosition( 0.0f, 0.0f );
		CComponentPlayerHomeBase::GetInstance().GetHomeBasePosition( vHomeBasePosition );
		vHomeBasePosition.mY += 32.0f;
		
		m_pAnimatedSpriteOwner->moveTo( vHomeBasePosition, m_fMovementSpeed, g_bActorPathAutoStop, g_bActorPathUseCallback, g_bActorPathSnapToPosition, g_fActorPathTargetMargin );
		m_uCurrentPathID = 666;
	}
	else if( m_eAttackBehaviorType == ATTACK_BEHAVIOR_TYPE_FLY_TO_PLAYER )
	{
		
	}

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentAStarActor::CheckEnemyDirectionLeftOrRight( const Point2I& _playerTilePosition, const Point2I& _enemyTilePosition, Point2I& _outTilePosition ) // Return value true if direction was set
{
	// Note: Player is directly left or right of the enemy if playerTilePoint.y == enemyTilePoint.y
	
	if( _playerTilePosition.x > _enemyTilePosition.x )
	{
		// Player is to the right of the enemy. The destination will be one tile to the right.
		_outTilePosition.x = _enemyTilePosition.x + 1;
		_outTilePosition.y = _enemyTilePosition.y;
		
		m_eLastMovementDirection = MOVEMENT_DIRECTION_RIGHT;
		return true;
	}
	else if( _playerTilePosition.x < _enemyTilePosition.x )
	{
		// Player is to the left of the enemy. The destination will be one tile to the left.
		_outTilePosition.x = _enemyTilePosition.x - 1;
		_outTilePosition.y = _enemyTilePosition.y;
		
		m_eLastMovementDirection = MOVEMENT_DIRECTION_LEFT;
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentAStarActor::CheckEnemyDirectionAboveOrBelow( const Point2I& _playerTilePosition, const Point2I& _enemyTilePosition, Point2I& _outTilePosition )	// Return value true if direction was set
{
	// Note: Player is directly above or below the enemy if playerTilePoint.x == enemyTilePoint.x
	
	if( _playerTilePosition.y > _enemyTilePosition.y )
	{
		// Player is above the enemy. The destination will be one tile up.
		_outTilePosition.x = _enemyTilePosition.x;
		_outTilePosition.y = _enemyTilePosition.y + 1;
		
		m_eLastMovementDirection = MOVEMENT_DIRECTION_UP;
		return true;
	}
	else if( _playerTilePosition.y < _enemyTilePosition.y )
	{
		// Player is below the enemy. The destination will be one tile down.
		_outTilePosition.x = _enemyTilePosition.x;
		_outTilePosition.y = _enemyTilePosition.y - 1;
		
		m_eLastMovementDirection = MOVEMENT_DIRECTION_DOWN;
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// CComponentAStarActor::PickTileAtCurrentBottomPosition
//
// - This function picks the tile that the bottom of this enemy is contacting with
// - Return value:
//	 - true:  A tile was picked and _outTilePosition will take the value of the picked tile.
//   - false: A tile was not picked.

bool CComponentAStarActor::PickTileAtCurrentBottomPosition( Point2I& _outTilePosition )
{
	if( m_pTileLayer == NULL ) // RKS TODO FUCK: This needs to call the PathGrindHandler -- This is not a good way to do this
		return false;
	
	t2dVector vOwnerPositionBottom = m_pAnimatedSpriteOwner->getPosition();
	t2dVector vOwnerSize = m_pAnimatedSpriteOwner->getSize();
	F32 fPositionYBottomOffset = (vOwnerSize.mY * 0.5f);
	vOwnerPositionBottom.mY += fPositionYBottomOffset;
	
	if( m_pTileLayer->pickTile( vOwnerPositionBottom, _outTilePosition ) )
		return true;
	
	printf( "%s - Unable to pick tile at enemy position (%f, %f)\n", __FUNCTION__, vOwnerPositionBottom.mX, vOwnerPositionBottom.mY );
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::MoveToNextPathNode()
{
	if( m_pAnimatedSpriteOwner == NULL || m_pStaticPath == NULL )
		return;
	
	if( m_iCurrentStaticPathIndex == m_pStaticPath->GetEndNodeIndex() )
	{
		printf( "%s - Reached end node at (%f, %f)\n", m_pAnimatedSpriteOwner->getName(), m_pAnimatedSpriteOwner->getPosition().mX, m_pAnimatedSpriteOwner->getPosition().mY );
		m_iCurrentStaticPathIndex = -1;
		return;
	}
	
	m_iCurrentStaticPathIndex++;
	t2dVector vDestination;
	m_pStaticPath->GetPositionAtNodeIndex( m_iCurrentStaticPathIndex, vDestination );
	
	// Since the 'position' of objects in T2D is the centroid of the object, and visually we want the bottom of the enemy to be seen as its current position,
	// so offset the enemy's destination upwards by half of its total height. (in T2D, +Y points down and -Y points up)
	F32 fYOffset = m_pAnimatedSpriteOwner->getSize().mY * 0.5f;
	vDestination.mY -= fYOffset;
	
	m_pAnimatedSpriteOwner->moveTo( vDestination, m_fMovementSpeed, g_bActorPathAutoStop, g_bActorPathUseCallback, g_bActorPathSnapToPosition, g_fActorPathTargetMargin );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAStarActor::UpdateSpitting()
{
	if( m_pAnimatedSpriteOwner == NULL )
		return;
	
	if( m_eAStarActorState != ASTAR_ACTOR_STATE_MOVING ) // This is the wrong state, but it works for now
		return;
	
	if( m_bIsDying )
		return;
	
	m_fSpitTimer += ITickable::smTickMs;
	if( m_fSpitTimer >= g_fSpitDelayTimeMS )
	{
		if( m_bHasSpitAttackPosition == false )
		{
			if( CComponentPathGridHandler::GetInstance().GetRandomAttackableLandBrickPosition( m_vSpitAttackPosition ) )
			{
				// A spit attack position has been found.  Start the spit attack animation.
				m_bHasSpitAttackPosition = true;
				m_pAnimatedSpriteOwner->playAnimation( g_szSerpentAttackAnimName, false );
				
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
			if( m_pAnimatedSpriteOwner->mAnimationController.getCurrentFrame() == g_uSerpentAttackAnimFrame && m_bAttackedDuringThisAnim == false )
			{
				CComponentGlobals::GetInstance().FireBrickAttackProjectileAtPosition( m_pAnimatedSpriteOwner->getPosition(), m_vSpitAttackPosition );
				CComponentPathGridHandler::GetInstance().SetLandBrickAtPositionAsFiredUpon( m_vSpitAttackPosition );
				m_bAttackedDuringThisAnim = true;
			}
			else 
			{
				// The spitter has attacked. Now wait for the animation to finish.
				if( m_pAnimatedSpriteOwner->mAnimationController.isAnimationFinished() )
				{
					// The attack animation has finished. Return to the idle animation.
					m_pAnimatedSpriteOwner->playAnimation( g_szSerpentIdleAnimName, false );
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
