//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemyBlob.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPauseScreen.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"
#include "ComponentStaticPath.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dTileMap.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 600.0f, 600.0f );

static bool g_bPathDebugOutputEnabled = false;	

static const char g_szSpawnAnim[16]			= "BLOB_SPAWN";
static const char g_szMoveAnim[16]			= "BLOB_MOVE";
static const char g_szAttackAnim[16]		= "BLOB_ATTACK";
static const char g_szDeathAnim[16]			= "BLOB_DEATH";
static const char g_szTileAttackAnim[16]	= "BLOB_BARF";

static const char g_szSpawnAnimSuper[32]		= "SUPER_BLOB_SPAWN";
static const char g_szMoveAnimSuper[32]			= "SUPER_BLOB_MOVE";
static const char g_szAttackAnimSuper[32]		= "SUPER_BLOB_ATTACK";
static const char g_szDeathAnimSuper[32]		= "SUPER_BLOB_DEATH";
static const char g_szTileAttackAnimSuper[32]	= "SUPER_BLOB_BARF";




static const U32 g_uEnemyABiteFrame = 4; // The frame at which the enemy should attack the current tile

//static const char g_szPathGrid[32] = "PathGrid";
//static t2dTileLayer* g_pPathGrid = NULL;


// Normal Blob / Super Blob values
static const ColorF g_ColorNormal( 1.0f, 1.0f, 1.0f );
static const ColorF g_ColorSuper( 1.0f, 0.0f, 0.0f );

static const F32 g_fBlobAttackPowerNormal	= 3.0f;
static const F32 g_fBlobMovementSpeedNormal = 40.0f;

static const F32 g_fBlobAttackPowerSuper	= 6.0f;
static const F32 g_fBlobMovementSpeedSuper	= 48.0f;


static const bool g_bSuicideBlobs = true;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static void _SetLayerOrder( t2dSceneObject* _pObject );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemyBlob );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyBlob::CComponentEnemyBlob()
	: m_pOwner( NULL )
	, m_eBlobState( BLOB_STATE_INACTIVE )
	, m_bAttackedDuringThisAnim( false )
	, m_bIsAttackingPlayer( false )
	, m_pStaticPath( NULL )
	, m_iCurrentStaticPathNodeIndex( -1 )
	, m_bIsSuperEnemy( false )
	, m_bWaitingForStomp( false )
	, m_bKilledBySuicide( false )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleActorStateNone()
{
	if( m_pOwner == NULL )
		return;
	
	PickTileAtCurrentBottomPosition( m_currentTilePoint );
	
	m_pOwner->setUseMouseEvents( true );
	
	SwitchBlobState( BLOB_STATE_SPAWNING );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szSpawnAnimSuper, false );
	else
		m_pOwner->playAnimation( g_szSpawnAnim, false );
	
	CSoundManager::GetInstance().PlayGenericSpawnSound();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleActorStateSpawning()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.isAnimationFinished() )
	{
		m_pOwner->setUsesPhysics( true ); // This is needed for movement
		SwitchBlobState( BLOB_STATE_MOVING );
		
		if( m_bIsSuperEnemy )
			m_pOwner->playAnimation( g_szMoveAnimSuper, false );
		else
			m_pOwner->playAnimation( g_szMoveAnim, false );
		
		m_pOwner->setUseMouseEvents( true ); // Allows the object to receive mouse events when touched
		
		// The first node will always be the spawn point. The Blob will initially move to the 2nd node in the list.
		m_iCurrentStaticPathNodeIndex = 0;
		
		MoveToNextPathNode();
		
		if( CComponentTutorialLevel::IsTutorialLevel() )
		{
            CComponentTutorialLevel::OnMonsterSpawned();
			//if( CComponentTutorialLevel::HasTutorialBeenShown_Blava() == false )
			//{
			//	CComponentTutorialLevel::EnableTutorialBlava();
			//}
		}
//		else
//		{
//			if( CComponentInGameTutorialObject::DoesLevelHaveSuperBlobTutorial() )
//			{
//				if( m_bIsSuperEnemy )
//				{
//					if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_BLOB ) == false )
//					{
//						CComponentInGameTutorialObject::ActivateTutorial();
//						CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_BLOB );
//					}
//				}
//			}
//		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleActorStateMoving()
{
	HandleMovingWithoutPathObject();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleMovingWithoutPathObject()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleActorStateAttacking()
{
	AssertFatal( m_eBlobState == BLOB_STATE_ATTACKING, "HandleActorStateAttacking(): Not in correct state." );
	
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.getCurrentFrame() == g_uEnemyABiteFrame && m_bAttackedDuringThisAnim == false )
	{
		if( m_bIsAttackingPlayer )
		{
			// Deal Damage to the player
			if( m_bIsSuperEnemy )
				CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_fBlobAttackPowerSuper );
			else
				CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_fBlobAttackPowerNormal );
			
			m_bAttackedDuringThisAnim = true;
		}
		else
		{
			if( CComponentPathGridHandler::GetInstance().IsLandBrickAttackable( m_currentTilePoint.x, m_currentTilePoint.y ) )
			{
				// Attack the tile
				CComponentPathGridHandler::GetInstance().DealDamageToLandBrick( m_currentTilePoint.x, m_currentTilePoint.y, 500 );
				
				m_bAttackedDuringThisAnim = true;
			}
		}
	}
	else if( m_pOwner->mAnimationController.isAnimationFinished() )
	{
		if( m_bIsAttackingPlayer )
		{
			if( m_bIsSuperEnemy )
				m_pOwner->playAnimation( g_szAttackAnimSuper, false );
			else
				m_pOwner->playAnimation( g_szAttackAnim, false );
			
			CSoundManager::GetInstance().PlayBlobAttackPlayerSound();
		}
		else
		{
			if(m_bIsSuperEnemy == false || CComponentPathGridHandler::GetInstance().IsLandBrickAttackable( m_currentTilePoint.x, m_currentTilePoint.y ) == false )
			{
				// Change state to moving
				SwitchBlobState( BLOB_STATE_MOVING );
				
				if( m_bIsSuperEnemy )
					m_pOwner->playAnimation( g_szMoveAnimSuper, false );
				else		
					m_pOwner->playAnimation( g_szMoveAnim, false );
				
				MoveToNextPathNode();
			}
			else
			{
				// The tile is not yet destroyed, so play the attack animation again
				if( m_bIsSuperEnemy )
					m_pOwner->playAnimation( g_szTileAttackAnimSuper, false );
				else
					m_pOwner->playAnimation( g_szTileAttackAnim, false );
				
				CSoundManager::GetInstance().PlayBlobAttackTileSound();
			}
		}
		m_bAttackedDuringThisAnim = false;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleActorStateDeath()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pOwner->mAnimationController.isAnimationFinished() )
	{
		// This is the new method without deleting the enemy
		t2dVector vEnemyPosition = m_pOwner->getPosition();
		m_eBlobState = BLOB_STATE_INACTIVE;
		m_pOwner->setVisible( false );
		m_pOwner->setPosition( g_vSafePosition );
		
		//m_pOwner->setBlendColour( g_ColorNormal );
		m_bIsSuperEnemy = false;
		
		if( m_bKilledBySuicide == false )
		{
			// Spawn a mana seed at the position where this enemy died
			CComponentManaSeedItemDrop::SpawnSeedFromEnemyAtPosition( vEnemyPosition );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::SwitchBlobState( const EBlobState _eBlobState )
{ 
	m_eBlobState = _eBlobState; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::OnOwnerCurrentPathComplete()
{
	if( g_bPathDebugOutputEnabled )
		printf( "OnOwnerCurrentPathComplete - Owner: %s\n", m_pOwner->getName() );
	
	if( m_pOwner == NULL )
		return;
	
	// Make the owner stop moving
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::Kill()
{
	if( m_pOwner == NULL )
		return;
	
	// Turn off mouse events for the owner
	m_pOwner->setUseMouseEvents( false );
	
	// Start Death for owner
	SwitchBlobState( BLOB_STATE_DEATH );
	
	// Stop the movement of the owner
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szDeathAnimSuper, false );
	else
		m_pOwner->playAnimation( g_szDeathAnim, false );
	
	CSoundManager::GetInstance().PlayBlobDeathSound();
	
	CComponentScoringManager::GetInstance().IncrementNumKilledEnemyA();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::KillFromStomp()
{
	if( m_pOwner == NULL )
		return;
	
	// Turn off mouse events for the owner
	m_pOwner->setUseMouseEvents( false );
	
	// Start Death for owner
	SwitchBlobState( BLOB_STATE_DEATH );
	
	// Stop the movement of the owner
	m_pOwner->setPositionTargetOff();
	m_pOwner->setLinearVelocity( t2dVector::getZero() );
	
	if( m_bIsSuperEnemy )
		m_pOwner->playAnimation( g_szDeathAnimSuper, false );
	else
		m_pOwner->playAnimation( g_szDeathAnim, false );
	
	//CSoundManager::GetInstance().PlayBlobDeathSound();
	
	CComponentScoringManager::GetInstance().IncrementNumKilledEnemyA();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::SpawnWithPath( CComponentStaticPath* const _pStaticPath )
{
	if( m_pOwner == NULL )
		return;
	
	m_bKilledBySuicide = false;
	
	m_iCurrentStaticPathNodeIndex = 0;
	m_pStaticPath = _pStaticPath;
	
	m_pOwner->setVisible( true );
	
	m_eBlobState = BLOB_STATE_NONE;
	
	if( m_pStaticPath )
	{
		// If this enemy has a path, then set its initial position to be the first node.
		t2dVector vStartNodePosition;
		m_pStaticPath->GetPositionAtNodeIndex( m_iCurrentStaticPathNodeIndex, vStartNodePosition );
		
		// Offset the position upwards by half the enemy's height, so the enemy appears visually to be standing on the center of the tile.
		F32 fPositionYBottomOffset = (m_pOwner->getSize().mY * 0.5f);
		vStartNodePosition.mY -= fPositionYBottomOffset;
		
		m_pOwner->setPosition( vStartNodePosition );
	}
	
	m_bIsAttackingPlayer = false;
	m_bAttackedDuringThisAnim = false;
	
	_SetLayerOrder( m_pOwner );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::SpawnSuperWithPath( CComponentStaticPath* const _pStaticPath )
{
	//if( m_pOwner )
	//	m_pOwner->setBlendColour( g_ColorSuper );
	
	m_bIsSuperEnemy = true;
	
	SpawnWithPath( _pStaticPath );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::SetOwnerPosition( const t2dVector & _vNewPosition )
{
	if( m_pOwner )
		m_pOwner->setPosition( _vNewPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::GetOwnerPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = ( m_pOwner ? m_pOwner->getPosition() : t2dVector::getZero() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyBlob::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is necessary for the script to be able to access %behavior.Owner, which in turn adds the component like this:  %behavior.Owner.Component = new CComponentEnemyBlob();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemyBlob ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemyBlob::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemyBlob::onComponentAdd - Must be added to a t2dAnimatedSprite." );
		return false;
	}
	
	// Store the owner
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::onUpdate()
{
	if( m_pOwner == NULL || m_eBlobState == BLOB_STATE_INACTIVE )
		return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
	{
		//if( CComponentEndLevelScreen::IsLevelWon() )
		//{
		//	if( m_eBlobState != BLOB_STATE_DEATH )
		//		Kill(); // RKS TODO: Use a specific 'level has ended' kill function
		//}
		//else
		if( m_bWaitingForStomp == false )
		{
			// The level has ended, but the player has lost. Make sure these enemies stop moving.
			m_eBlobState = BLOB_STATE_INACTIVE;
			m_pOwner->setPositionTargetOff();
			m_pOwner->setLinearVelocity( t2dVector::getZero() );
			
			// Should we play the idle animation here?
			if( m_bIsSuperEnemy )
				m_pOwner->playAnimation( g_szMoveAnimSuper, false );
			else
				m_pOwner->playAnimation( g_szMoveAnim, false );
			
			m_bWaitingForStomp = true;
		}
	}
	
	_SetLayerOrder( m_pOwner );
	
	// Get the Tile Layer
	//if( g_pPathGrid == NULL )
	//{
	//	// Find the Tile Layer object
	//	g_pPathGrid = static_cast<t2dTileLayer*>( Sim::findObject( g_szPathGrid ) );
	//}
	
	// Handle the appropriate action, depending on the AStar actor's current state
	switch( m_eBlobState )
	{
		case BLOB_STATE_NONE:
			HandleActorStateNone();
			break;
			
		case BLOB_STATE_SPAWNING:
			HandleActorStateSpawning();
			break;
			
		case BLOB_STATE_MOVING:
			HandleActorStateMoving();
			break;
			
		case BLOB_STATE_ATTACKING:
			HandleActorStateAttacking();
			break;
			
		case BLOB_STATE_DEATH:
			HandleActorStateDeath();
			break;
			
		default:
			break;
	}
	
	DetermineOwnerFlip();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::HandleOwnerPositionTargetReached()
{
	if( m_pOwner == NULL )
		return;
	
	if( g_bPathDebugOutputEnabled )
		printf( "Target reached, Owner Position (%f, %f)\n", m_pOwner->getPosition().mX, m_pOwner->getPosition().mY );
	
	if( m_pStaticPath == NULL )
		return;
	
	if( PickTileAtCurrentBottomPosition( m_currentTilePoint ) == false )
		return;
	
	if( m_iCurrentStaticPathNodeIndex == m_pStaticPath->GetEndNodeIndex() )
	{
		// This enemy has reached its last path node. Start attacking the home base (the end nodes are always next to the home base).
		m_bIsAttackingPlayer = true;
		
		if( g_bSuicideBlobs )
		{
			CComponentGlobals::GetInstance().PlayHitEffectAtPosition( m_pOwner->getPosition() );
			
			m_bKilledBySuicide = true;
			
			Kill();
			
			// Deal Damage to the player
			if( m_bIsSuperEnemy )
				CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_fBlobAttackPowerSuper );
			else
				CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_fBlobAttackPowerNormal );
		}
		else
		{
			m_pOwner->setPositionTargetOff();
			m_pOwner->setLinearVelocity( t2dVector::getZero() );
			
			SwitchBlobState( BLOB_STATE_ATTACKING );
			
			if( m_bIsSuperEnemy )
				m_pOwner->playAnimation( g_szAttackAnimSuper, false );
			else
				m_pOwner->playAnimation( g_szAttackAnim, false );
			   
			m_bAttackedDuringThisAnim = false;
		}
		
		return;
	}
	
	// Only attack this destination if the tile is in an attackable state (not solid rock).
	if( m_bIsSuperEnemy == true && CComponentPathGridHandler::GetInstance().IsLandBrickAttackable( m_currentTilePoint.x, m_currentTilePoint.y ) )
	{
		SwitchBlobState( BLOB_STATE_ATTACKING );
		
		if( m_bIsSuperEnemy )
			m_pOwner->playAnimation( g_szTileAttackAnimSuper, false );
		else
			m_pOwner->playAnimation( g_szTileAttackAnim, false );
		
		m_bAttackedDuringThisAnim = false;
		
		CSoundManager::GetInstance().PlayBlobAttackTileSound();
	}
	else 
	{
		MoveToNextPathNode();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsTutorialActive() )
		return;
	
	if( m_pOwner == NULL )
		return;
	
	if( m_eBlobState == BLOB_STATE_DEATH )
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
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackBlob( m_pOwner, this );
	
	CComponentGlobals::GetInstance().PlayHitEffectAtPosition( m_pOwner->getPosition() );
    
    if( CComponentTutorialLevel::IsTutorialLevel() == true )
    {
        CComponentTutorialLevel::OnMonsterTapped();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	CComponentGlobals::GetInstance().SetPlayerNotTouchingEnemy();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( m_eBlobState == BLOB_STATE_DEATH )
		return;
	
	if( CComponentPlayerHomeBase::GetInstance().DoesPlayerHomeBaseHaveAttackTarget() )
	{
		// Do nothing if the player home base is already attacking a target
		return;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::OnPostInit()
{
	if( m_pOwner )
		m_pOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::DetermineOwnerFlip()
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
// CComponentEnemyBlob::PickTileAtCurrentBottomPosition
//
// - This function picks the tile that the bottom of this enemy is contacting with
// - Return value:
//	 - true:  A tile was picked and _outTilePosition will take the value of the picked tile.
//   - false: A tile was not picked.

bool CComponentEnemyBlob::PickTileAtCurrentBottomPosition( Point2I& _outTilePosition )
{
	t2dVector vOwnerPositionBottom = m_pOwner->getPosition();
	t2dVector vOwnerSize = m_pOwner->getSize();
	F32 fPositionYBottomOffset = (vOwnerSize.mY * 0.5f);
	vOwnerPositionBottom.mY += fPositionYBottomOffset;
	
	//if( g_pPathGrid->pickTile( vOwnerPositionBottom, _outTilePosition ) )
	if( CComponentPathGridHandler::PickTileAtPosition( vOwnerPositionBottom, _outTilePosition ) )
		return true;
	
	printf( "%s - Unable to pick tile at enemy position (%f, %f)\n", __FUNCTION__, vOwnerPositionBottom.mX, vOwnerPositionBottom.mY );
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyBlob::MoveToNextPathNode()
{
	if( m_pOwner == NULL || m_pStaticPath == NULL )
		return;
	
	if( m_iCurrentStaticPathNodeIndex == m_pStaticPath->GetEndNodeIndex() )
	{
		//printf( "%s - Reached end node at (%f, %f)\n", m_pOwner->getName(), m_pOwner->getPosition().mX, m_pOwner->getPosition().mY );
		m_iCurrentStaticPathNodeIndex = -1;
		return;
	}
	
	m_iCurrentStaticPathNodeIndex++;
	t2dVector vDestination;
	m_pStaticPath->GetPositionAtNodeIndex( m_iCurrentStaticPathNodeIndex, vDestination );
	
	// Since the 'position' of objects in T2D is the centroid of the object, and visually we want the bottom of the enemy to be seen as its current position,
	// so offset the enemy's destination upwards by half of its total height. (in T2D, +Y points down and -Y points up)
	F32 fYOffset = m_pOwner->getSize().mY * 0.5f;
	vDestination.mY -= fYOffset;
	
	if( m_bIsSuperEnemy )
		m_pOwner->moveTo( vDestination, g_fBlobMovementSpeedSuper, true, true, false, 0.5f );
	else
		m_pOwner->moveTo( vDestination, g_fBlobMovementSpeedNormal, true, true, false, 0.5f );
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
