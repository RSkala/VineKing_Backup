//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentPlayerHomeBase.h"

#include "ComponentBossMountain.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentEnemyBlob.h"
#include "ComponentEnemySerpent.h"
#include "ComponentEnemySmoke.h"
#include "ComponentGlobals.h"
#include "ComponentHealthBar.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentManaBar.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPauseScreen.h"
#include "ComponentScoringManager.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dStaticSprite.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Vines
//static const U32 g_uMaxNumTypeCVines = 10;

static bool g_bDebugAnimationState = false; // This is to see if this fixes the issue with the player freezing in place in Distribution builds
static const t2dVector g_vSafeVinePosition( 800.0f, 800.0f ); // This is used to solve the issue of the vines appearing for a split second in their old position before being moved to where they are supposed to be


// Health
static const F32 g_fBaseHealthAmount	= 100.0f;
static const F32 g_fMaxHealthAmount		= 200.0f;
static const F32 g_fMaxAddableHealth	= g_fMaxHealthAmount - g_fBaseHealthAmount; // This is the maximum amount of health that can be added to the base health amount

// Attack
static const F32 g_fBossDamageAmount = 25.0; //50.0f;

// Animation
static const char g_szVineAnimationName[32]				= "attack_vineAnimation"; //"Anim_VineAttack03"; //"Anim_VineAttack02";
static const char g_szHomeBaseIdleAnim[32]				= "VINE_KING_IDLE";
static const char g_szSeedMagnetAnimationName[32]		= "VINE_KING_EAT";
static const char g_szHomeBaseLineDrawAnimName[32]		= "VINE_KING_LINE_DRAW";
static const char g_szHomeBaseVineAttackAnimName[32]	= "VINE_KING_ATTACK";
static const char g_szHomeBaseHitReactionAnimName[32]	= "VINEKING_TAKE_DAMAGE";
static const char g_szHomeBaseOutOfManaAnim[32]			= "vine_king_mana_drain";

static const U32 g_uVineAttackFrame = 1; // The animation frame that should start the vines sprouting when the home base attacks

// Vortex object
static const char g_szVortexObject[32] = "VortexObject";

//static const char g_szVortexImageMap[32] = "vortex_fxImageMap";
static t2dAnimatedSprite* g_pVortexObject = NULL;
//static const t2dVector g_vVortexStartSize( 128.0f, 128.0f );
static const F32 g_fRotationSpeed = 360.0f; // Degrees per second
static F32 g_fCurrentRotation = 0.0f;
static const F32 g_fVortexScale = 2.0f;
//static const S32 g_iVortexLayer = 12;


// Position offsets
static t2dVector g_vHomeBasePositionActual;			// The actual position of the home base
static t2dVector g_vHomeBasePositionBottom;			// The position of the bottom part of the home base
static t2dVector g_vHomeBasePositionVisibleCenter;	// The center of the visible part of the home base
static t2dVector g_vHomeBasePositionBelly;			// The belly of the VineKing

static F32 g_fCenterPositionOffsetY = 28.0f;
static F32 g_fBellyPositionOffsetY = 42.0f;

static t2dVector g_vVortexPosFaceLeft;
static t2dVector g_vVortexPosFaceRight;
static F32 g_fVortexPositionOffsetX = 7.5f;

// Test Colors
static const ColorF g_FingerUpColor( 1.0f, 1.0f, 1.0f );
static const ColorF g_FingerDownColor( 1.0f, 0.0f, 0.0f );

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ CComponentPlayerHomeBase* CComponentPlayerHomeBase::sm_pInstance = NULL;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentPlayerHomeBase );

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPlayerHomeBase::CComponentPlayerHomeBase()
	: m_pHomeBaseObject( NULL )
	, m_pszAnimIdleLoop( NULL )
	, m_pszAnimGrowStart( NULL )
	, m_pszAnimGrowLoop( NULL )
	, m_pszAnimMoveLoop( NULL)
	, m_pszEatAnim( NULL )
	, m_pszWinAnim( NULL )
	, m_pszDeadAnim( NULL )
	, m_pszHitAnim( NULL )
	, m_ePlayerHomeBaseState( HOME_BASE_STATE_NONE )
	, m_pszVineObjectA( NULL )
	, m_pVineObjectA( NULL )
	, m_pszVineObjectB( NULL )
	, m_pVineObjectB( NULL )
	, m_pszVineObjectC( NULL )
	, m_pVineObjectC( NULL )
	, m_fMaxHealth( g_fBaseHealthAmount )
	, m_fCurrentHealth( g_fBaseHealthAmount )

	, m_pCurrentAttackTarget( NULL )
	, m_pCurrentAttackTargetComponent( NULL )
	, m_eCurrentAttackTargetType( TARGET_TYPE_NONE )

	, m_bFacingRight( false )
{
	
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::GetHomeBasePosition( t2dVector& _vOutPosition )
{
	_vOutPosition = g_vHomeBasePositionActual;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::GetHomeBaseBottomPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = g_vHomeBasePositionBottom;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::GetHomeBaseVisibleCenterPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = g_vHomeBasePositionVisibleCenter;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::GetHomeBaseBellyPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = g_vHomeBasePositionBelly;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateNone()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
	
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateIdle()
{
	if( m_pHomeBaseObject == NULL )
		return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateGrowStart()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	AssertFatal( m_ePlayerHomeBaseState == HOME_BASE_STATE_GROW_START, "Home base state incorrect" );
	
	if( m_pHomeBaseObject->mAnimationController.isAnimationFinished() )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_GROW_LOOPING );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStategrowLooping()
{
	if( m_pHomeBaseObject == NULL )
		return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateMoving()
{
	if( m_pHomeBaseObject == NULL )
		return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateAttackingStart()
{
	if( m_pHomeBaseObject == NULL )
		return;

	g_bDebugAnimationState = false;
	
	SwitchHomeBaseState( HOME_BASE_STATE_ATTACKING );
	
	if( m_pCurrentAttackTarget )
		DetermineHomeBaseFlip( m_pCurrentAttackTarget->getPosition() );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateAttacking()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	if( m_pCurrentAttackTarget == NULL || m_pCurrentAttackTargetComponent == NULL || m_pVineObjectA == NULL )
		return;
	
	if( m_pHomeBaseObject->mAnimationController.isAnimationFinished() )
	{
		// Clear the current attack objects	
		
		//----------------------------------------------------------
		// New Attack Stuff 201/03/17
		m_pCurrentAttackTarget = NULL;
		m_pCurrentAttackTargetComponent = NULL;
		m_eCurrentAttackTargetType = TARGET_TYPE_NONE;
		//----------------------------------------------------------

		
		
		// Switch back into idle mode
		SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
		
		if( m_pVineObjectA )
		{
			//printf( "Setting m_pVineObjectA position: (%f, %f)\n", g_vSafeVinePosition.mX, g_vSafeVinePosition.mY );
			m_pVineObjectA->setVisible( false );
			m_pVineObjectA->setPosition( g_vSafeVinePosition ); // Move the position of the vine way outside the visible part of the world (this is an attempt to fix the jumping that happens with the vine) 
		}
		
		if( m_pVineObjectB )
		{
			//printf( "Setting m_pVineObjectB position: (%f, %f)\n", g_vSafeVinePosition.mX, g_vSafeVinePosition.mY );
			m_pVineObjectB->setVisible( false );
			m_pVineObjectB->setPosition( g_vSafeVinePosition ); // Move the position of the vine way outside the visible part of the world (this is an attempt to fix the jumping that happens with the vine)
		}
	}
	else 
	{
		U32 uCurrentFrame = m_pHomeBaseObject->mAnimationController.getCurrentFrame();
		if( uCurrentFrame >= g_uVineAttackFrame && g_bDebugAnimationState == false )
		{
			SetUpVineForAttack();
			
			if( m_eCurrentAttackTargetType == TARGET_TYPE_BLOB )
			{
				AttackBlob();
			}
			else if( m_eCurrentAttackTargetType == TARGET_TYPE_SERPENT )
			{
				AttackSerpent();
			}
			else if( m_eCurrentAttackTargetType == TARGET_TYPE_SMOKE )
			{
				AttackSmoke();
			}
			else if( m_eCurrentAttackTargetType == TARGET_TYPE_BOSS )
			{
				AttackBoss();
			}
			else
			{
				printf( "Unhandled Attack Target Type: %d\n", m_eCurrentAttackTargetType );
			}
			
			//m_pCurrentAttackTarget = NULL;
			//m_pCurrentAttackTargetComponent = NULL;
			//m_eCurrentAttackTargetType = TARGET_TYPE_NONE;
			
			g_bDebugAnimationState = true;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateEating()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	AssertFatal( m_ePlayerHomeBaseState == HOME_BASE_STATE_EATING, "Home base state incorrect" );
	
	if( m_pHomeBaseObject->mAnimationController.isAnimationFinished() )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateSeedMagnet()
{
	if( m_pHomeBaseObject->mAnimationController.isAnimationFinished() )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
	}
	
	//if( g_pVortexObject ) // Vortex spinning disabled
	//{
	//	g_fCurrentRotation += g_fRotationSpeed * ITickable::smTickSec;
	//	if( g_fCurrentRotation >= 360.0f )
	//		g_fCurrentRotation = 0.0f;
	//	
	//	g_pVortexObject->setRotation( g_fCurrentRotation );
	//}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateHitReaction()
{
	if( m_pHomeBaseObject->mAnimationController.isAnimationFinished() )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleHomeBaseStateOutOfMana()
{
	if( m_pHomeBaseObject->mAnimationController.isAnimationFinished() )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::SwitchHomeBaseState( const EPlayerHomeBaseState _eNewHomeBaseState ) 
{ 
	if( CanHomeBaseSwitchStates( _eNewHomeBaseState ) )
	{
		OnSwitchHomeBaseState( _eNewHomeBaseState );
		m_ePlayerHomeBaseState = _eNewHomeBaseState;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::DetermineHomeBaseFlip( const t2dVector& _vDestination )
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	t2dVector vHomeBasePosition = m_pHomeBaseObject->getPosition();
	
	// Do nothing if the destination position is the same as the home base position
	if( vHomeBasePosition.isEqual( _vDestination ) )
		return;
	
	// Check the direction of the player's destination, so we know whether to flip the direction
	// If the direction is to the left, set the flip to False. If the direction is to the right, set the flip to true.
	
	t2dVector vDirection = _vDestination - vHomeBasePosition;
	t2dVector vRight( 1.0f, 0.0f );
	
	F32 fDot = vRight.dot( vDirection );
	if( fDot > 0.0f )
	{
		// Direction is to the right, so set the flip to true
		m_pHomeBaseObject->setFlipX( true );
		
		if( g_pVortexObject )
			g_pVortexObject->setPosition( g_vVortexPosFaceRight );
		
		m_bFacingRight = true;
	}
	else if( fDot < 0.0f )
	{
		// Direction is to the left, so set the flip to false
		m_pHomeBaseObject->setFlipX( false );
		
		if( g_pVortexObject )
			g_pVortexObject->setPosition( g_vVortexPosFaceLeft );
		
		m_bFacingRight = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToAttackBlob( t2dSceneObject* const _pObjectToAttack, DynamicConsoleMethodComponent* const _pObjectToAttackComponent )
{
	SetAttackVariables( _pObjectToAttack, _pObjectToAttackComponent, t2dVector::getZero() );
	
	m_eCurrentAttackTargetType = TARGET_TYPE_BLOB;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToAttackSerpent( t2dSceneObject* const _pObjectToAttack, DynamicConsoleMethodComponent* const _pObjectToAttackComponent )
{
	SetAttackVariables( _pObjectToAttack, _pObjectToAttackComponent, t2dVector::getZero() );
	
	m_eCurrentAttackTargetType = TARGET_TYPE_SERPENT;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToAttackSmoke( t2dSceneObject* const _pObjectToAttack, DynamicConsoleMethodComponent* const _pObjectToAttackComponent )
{
	SetAttackVariables( _pObjectToAttack, _pObjectToAttackComponent, t2dVector::getZero() );
	
	m_eCurrentAttackTargetType = TARGET_TYPE_SMOKE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToAttackBoss( t2dSceneObject* const _pObjectToAttack, DynamicConsoleMethodComponent* const _pObjectToAttackComponent, const t2dVector& _vAttackPosition)
{
	SetAttackVariables( _pObjectToAttack, _pObjectToAttackComponent, _vAttackPosition );
	
	m_eCurrentAttackTargetType = TARGET_TYPE_BOSS;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToEatSeedFromMagnetMode( const F32& _fManaAmount )
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	//printf( "CComponentPlayerHomeBase::NotifyHomeBaseToEatSeedFromMagnetMode - Mana Amount = %f\n", _fManaAmount );
	
	CSoundManager::GetInstance().PlayEatSound();
	
	//CComponentGlobals::GetInstance().AddMana( (F32)_fManaAmount );
	CComponentManaBar::AddMana( (F32)_fManaAmount );
	
	CComponentScoringManager::GetInstance().IncrementNumSeedsEaten();
    
    if( CComponentTutorialLevel::IsTutorialLevel() == true )
    {
        CComponentTutorialLevel::OnPlayerEatSeed();
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseOfLevelWinStart()
{
	SwitchHomeBaseState( HOME_BASE_STATE_WAITING_FOR_END_SEQUENCE );
	
	//CComponentManaSeedItemDrop::HideAllSeeds();
	CComponentManaSeedItemDrop::OnEndLevelSequenceStarted();
	
	if( g_pVortexObject )
		g_pVortexObject->setVisible( false );
	
	CSoundManager::GetInstance().StopVortexSound();
	
	CSoundManager::GetInstance().StopChantingSound();
	
	if( m_pVineObjectA )
		m_pVineObjectA->setVisible( false );
	
	if( m_pVineObjectB )
		m_pVineObjectB->setVisible( false );
	
//	if( m_pVineObjectC )
//		m_pVineObjectC->setVisible( false );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseOfLevelWin()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	SwitchHomeBaseState( HOME_BASE_STATE_WIN );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToEnterSeedMagnetMode()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	SwitchHomeBaseState( HOME_BASE_STATE_SEED_MAGNET );
	
	if( g_pVortexObject )
		g_pVortexObject->setVisible( true );
	
	CSoundManager::GetInstance().PlayVortexSound();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseToExitSeedMagnetMode()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	if( g_pVortexObject )
		g_pVortexObject->setVisible( false );
	
	CSoundManager::GetInstance().StopVortexSound();
	
	// If the player is in "Seed Magnet" mode, exit that mode.
	if( m_ePlayerHomeBaseState == HOME_BASE_STATE_SEED_MAGNET )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_IDLE );
		return;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::NotifyHomeBaseOfOutOfManaLineDrawAttempt()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	if( m_ePlayerHomeBaseState != HOME_BASE_STATE_OUT_OF_MANA )
		SwitchHomeBaseState( HOME_BASE_STATE_OUT_OF_MANA );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::DamageHomeBase( const F32& _fDamageAmount )
{	
	if( m_pHomeBaseObject == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( m_fCurrentHealth <= 0.0f )
		return;
	
	// Only play the hit reaction animation if the home base is not pulling in seeds, otherwise this will make it stop
	if( m_ePlayerHomeBaseState != HOME_BASE_STATE_SEED_MAGNET )
	{
		SwitchHomeBaseState( HOME_BASE_STATE_HITREACTION );
	}
	
	//if( g_bPlayerInvincible == false )
	if( CComponentGlobals::GetInstance().Debug_IsPlayerInvincibleEnabled() == false )
	{
		m_fCurrentHealth -= _fDamageAmount;
		
		//printf( "Home Base health: %f\n", m_fCurrentHealth );
        
        if( CComponentTutorialLevel::IsTutorialLevel() == true )
        {
            CSoundManager::GetInstance().PlayHomeBaseTakeDamageSound();
            
            if( m_fCurrentHealth <= 0.0f )
                m_fCurrentHealth = 0.0;
        }
        else
        {
            if( m_fCurrentHealth <= 0.0f )
            {
                m_fCurrentHealth = 0.0f;
                
                SwitchHomeBaseState( HOME_BASE_STATE_DEAD );
                
                CSoundManager::GetInstance().PlayHomeBaseDeathSound();
                
                //CComponentEndLevelScreen::GetInstance().ShowGameOverScreen();
                CComponentEndLevelScreen::GetInstance().StartLevelLoseSequence();
                
                if( g_pVortexObject )
                    g_pVortexObject->setVisible( false );
                
                CSoundManager::GetInstance().StopVortexSound();
                
                CSoundManager::GetInstance().StopChantingSound();
                
                CComponentManaSeedItemDrop::HideAllSeeds();
                
                if( m_pVineObjectA )
                    m_pVineObjectA->setVisible( false );
                
                if( m_pVineObjectB )
                    m_pVineObjectB->setVisible( false );
                
    //			if( m_pVineObjectC )
    //				m_pVineObjectC->setVisible( false );
            }
            else 
            {
                CSoundManager::GetInstance().PlayHomeBaseTakeDamageSound();
            }
        }

		CComponentHealthBar::NotifyHomeBaseHealthChange();
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HideHomeBase()
{
	if( m_pHomeBaseObject )
	{
		m_pHomeBaseObject->setVisible( false );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::UnhideHomeBase()
{
	if( m_pHomeBaseObject )
	{
		m_pHomeBaseObject->setVisible( true );
		m_pHomeBaseObject->playAnimation( g_szHomeBaseIdleAnim, false );
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ F32 CComponentPlayerHomeBase::GetPercentHealthRemaining()
{
	if( sm_pInstance == NULL || sm_pInstance->m_fMaxHealth < g_fSmallNumber )
		return 0.0f;
	
	return (sm_pInstance->m_fCurrentHealth / sm_pInstance->m_fMaxHealth);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPlayerHomeBase::ForceHomeBaseIntoIdle() 
{
    if( sm_pInstance == NULL )
        return;
    
    sm_pInstance->OnSwitchHomeBaseState( HOME_BASE_STATE_IDLE );
    sm_pInstance->m_ePlayerHomeBaseState = HOME_BASE_STATE_IDLE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPlayerHomeBase::TEST_VineKingFingerUp()
{
	if( sm_pInstance == NULL || sm_pInstance->m_pHomeBaseObject == NULL )
		return;
	
	sm_pInstance->m_pHomeBaseObject->setBlendColour( g_FingerUpColor );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPlayerHomeBase::TEST_VineKingFingerDown()
{
	if( sm_pInstance == NULL || sm_pInstance->m_pHomeBaseObject == NULL )
		return;
	
	sm_pInstance->m_pHomeBaseObject->setBlendColour( g_FingerDownColor );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPlayerHomeBase::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pHomeBaseObject, CComponentPlayerHomeBase ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "IdleLoopAnim",		TypeString, Offset( m_pszAnimIdleLoop,	CComponentPlayerHomeBase ) );
	addField( "GrowingStartAnim",	TypeString, Offset( m_pszAnimGrowStart,	CComponentPlayerHomeBase ) );
	addField( "GrowingLoopAnim",	TypeString, Offset( m_pszAnimGrowLoop,	CComponentPlayerHomeBase ) );
	addField( "MoveLoopAnim",		TypeString, Offset( m_pszAnimMoveLoop,	CComponentPlayerHomeBase ) );
	addField( "EatAnim",			TypeString, Offset( m_pszEatAnim,		CComponentPlayerHomeBase ) );
	addField( "WinAnim",			TypeString, Offset( m_pszWinAnim,		CComponentPlayerHomeBase ) );
	addField( "DeadAnim",			TypeString, Offset( m_pszDeadAnim,		CComponentPlayerHomeBase ) );
	addField( "HitAnim",			TypeString, Offset( m_pszHitAnim,		CComponentPlayerHomeBase ) );
	
	addField( "VineObjectA",		TypeString, Offset( m_pszVineObjectA,	CComponentPlayerHomeBase ) );
	addField( "VineObjectB",		TypeString, Offset( m_pszVineObjectB,	CComponentPlayerHomeBase ) );
	addField( "VineObjectC",		TypeString, Offset( m_pszVineObjectC,	CComponentPlayerHomeBase ) );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPlayerHomeBase::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentPlayerHomeBase::onComponentAdd - Must be added to a t2dAnimatedSprite" );
		return false;
	}
	
	m_pHomeBaseObject = pOwnerObject;
	
	sm_pInstance = this;
	
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::onUpdate()
{
	if( m_pHomeBaseObject == NULL )
		return;
		
	// Check if the user is drawing a line.  If so, put the home base into its line draw animation.
	if( CComponentLineDrawAttack::IsLineDrawAttackActive() )
	{
		if( m_ePlayerHomeBaseState == HOME_BASE_STATE_IDLE )
		{
			if( strcmp( m_pHomeBaseObject->mAnimationController.getCurrentAnimation(), g_szHomeBaseLineDrawAnimName ) != 0 )
			{
				m_pHomeBaseObject->playAnimation( g_szHomeBaseLineDrawAnimName, false );
				CSoundManager::GetInstance().PlayChantingSound();
			}
		}
	}
	else
	{
		if( m_ePlayerHomeBaseState == HOME_BASE_STATE_IDLE )
		{
			if( strcmp( m_pHomeBaseObject->mAnimationController.getCurrentAnimation(), m_pszAnimIdleLoop ) != 0 )
			{
				m_pHomeBaseObject->playAnimation( m_pszAnimIdleLoop, false );
				CSoundManager::GetInstance().StopChantingSound();
			}
		}
	}
	
	// Check individual states
	switch( m_ePlayerHomeBaseState ) 
	{
		case HOME_BASE_STATE_NONE:
			HandleHomeBaseStateNone();
			break;
			
		case HOME_BASE_STATE_IDLE:
			HandleHomeBaseStateIdle();
			break;
			
		case HOME_BASE_STATE_GROW_START:
			HandleHomeBaseStateGrowStart();
			break;
			
		case HOME_BASE_STATE_GROW_LOOPING:
			HandleHomeBaseStategrowLooping();
			break;
			
		case HOME_BASE_STATE_MOVING:
			HandleHomeBaseStateMoving();
			break;
			
		case HOME_BASE_STATE_ATTACKING_START:
			HandleHomeBaseStateAttackingStart();
			break;
			
		case HOME_BASE_STATE_ATTACKING:
			HandleHomeBaseStateAttacking();
			break;
			
		case HOME_BASE_STATE_EATING:
			HandleHomeBaseStateEating();
			break;
			
		case HOME_BASE_STATE_WIN:
			HandleHomeBaseStateWin();
			break;
			
		case HOME_BASE_STATE_SEED_MAGNET:
			HandleHomeBaseStateSeedMagnet();
			break;
			
		case HOME_BASE_STATE_HITREACTION:
			HandleHomeBaseStateHitReaction();
			break;
			
		case HOME_BASE_STATE_OUT_OF_MANA:
			HandleHomeBaseStateOutOfMana();
			break;
			
		default:
			break;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::HandleOwnerPositionTargetReached()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	SwitchHomeBaseState( HOME_BASE_STATE_GROW_START );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::OnMouseDown( const t2dVector& _vWorldMousePoint )
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( m_pHomeBaseObject == NULL )
		return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::OnPostInit()
{
	if( m_pHomeBaseObject == NULL )
		return;
	
	// Set up home base positions
	g_vHomeBasePositionActual = m_pHomeBaseObject->getPosition();
	
	g_vHomeBasePositionBottom = g_vHomeBasePositionActual;
	g_vHomeBasePositionBottom.mY += m_pHomeBaseObject->getSize().mY * 0.5f; // The bottom of the object is half its height added to its Y coordinate
	
	g_vHomeBasePositionVisibleCenter = g_vHomeBasePositionActual;
	g_vHomeBasePositionVisibleCenter.mY += g_fCenterPositionOffsetY;
	
	g_vHomeBasePositionBelly = g_vHomeBasePositionActual;
	g_vHomeBasePositionBelly.mY += g_fBellyPositionOffsetY;
	
	// Set up Vortex positions
	g_vVortexPosFaceLeft = g_vHomeBasePositionVisibleCenter;
	g_vVortexPosFaceLeft.mX -= g_fVortexPositionOffsetX;
	
	g_vVortexPosFaceRight = g_vHomeBasePositionVisibleCenter;
	g_vVortexPosFaceRight.mX += g_fVortexPositionOffsetX;

	// Set up the Vortex object
	g_fCurrentRotation = 0.0f;
	g_pVortexObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szVortexObject ) );
	if( g_pVortexObject )
	{
		g_pVortexObject->setPosition( g_vVortexPosFaceLeft );
		g_pVortexObject->setVisible( false );
		g_pVortexObject->setLayer( LAYER_VORTEX );
		
		t2dVector vVortexSize = g_pVortexObject->getSize();
		vVortexSize.mX *= g_fVortexScale;
		vVortexSize.mY *= g_fVortexScale;
		g_pVortexObject->setSize( vVortexSize );
	}
	
	// We are no longer creating the vortex object as a t2dStaticSprite -- SAVE THIS CODE FOR FUTURE REFERENCE
//	g_pVortexObject = dynamic_cast<t2dStaticSprite*>( ConsoleObject::create( "t2dStaticSprite" ) );
//	if( g_pVortexObject && g_pVortexObject->isProperlyAdded() == false)
//	{
//		if( g_pVortexObject->registerObject() )
//		{
//			if( m_pHomeBaseObject->getSceneGraph() )
//				m_pHomeBaseObject->getSceneGraph()->addToScene( g_pVortexObject );
//
//			g_pVortexObject->setImageMap( g_szVortexImageMap, 0 );
//			
//			t2dVector vPosition = m_pHomeBaseObject->getPosition();
//			vPosition.mY += 32.0f; // Move the position down by this amount as the center of the Home Base is actually slightly above it.
//			g_pVortexObject->setPosition( vPosition );
//			
//			g_pVortexObject->setSize( g_vVortexStartSize );
//			
//			g_pVortexObject->setLayer( g_iVortexLayer );
//			
//			g_pVortexObject->setVisible( false );
//		}
//	}
	
	// Initialize the Vine Objects
	if( m_pVineObjectA == NULL )
	{
		if( m_pszVineObjectA != NULL )
		{
			m_pVineObjectA = static_cast<t2dAnimatedSprite*>( Sim::findObject( m_pszVineObjectA ) );
		}
		
		if( m_pVineObjectA )
		{
			m_pVineObjectA->setVisible( false );
			m_pVineObjectA->setPosition( g_vSafeVinePosition );
			m_pVineObjectA->setLayer( LAYER_PLAYER_VINE_ATTACK );
		}
	}	
	
	// Make sure the proper spatial order is set for the home base's layer, so enemies below will be in front of it.
	m_pHomeBaseObject->setLayer( LAYER_PLAYER_BLOB_SERPENT );
	m_pHomeBaseObject->setLayerOrder( static_cast<S32>( m_pHomeBaseObject->getPosition().mY + 240.0f ) );
	
	// Set up the player's health based on the amount of XP added to health in the Level Up Screen
	//printf( "Setting up the player's health...\n" );
	
	F32 fPercentHealthXPAdded = CComponentGlobals::GetInstance().GetPercentHealthXPAdded(); // Get the amount of XP that the player added to Mana in the LevelUp Screen
	//printf( "- Percent Health Added: %f\n", fPercentHealthXPAdded );

	
	F32 fHealthToAdd = fPercentHealthXPAdded * g_fMaxAddableHealth;
	//printf( "- Health to add: %f\n", fHealthToAdd );
	
	//printf( "- Current Max Health Amount:     %f\n", m_fMaxHealth );
	m_fMaxHealth += fHealthToAdd;
	//printf( "- New Current Max Health Amount: %f\n", m_fMaxHealth );
	
	m_fCurrentHealth = m_fMaxHealth;
	
	
	// Start the home base object invisible, so the home base start animation can play
	m_pHomeBaseObject->setVisible( false );
	
	//static const F32 g_fBaseHealthAmount	= 100.0f;
	//static const F32 g_fMaxHealthAmount	= 200.0f;
	//static const F32 g_fMaxAddedHealth	= g_fMaxHealthAmount - g_fBaseHealthAmount; // This is the maximum amount of health that can be added to the base health amount
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentPlayerHomeBase::SetAttackVariables( t2dSceneObject* const _pObjectToAttack, DynamicConsoleMethodComponent* const _pObjectToAttackComponent, const t2dVector& _vAttackPosition )
{
	if( _pObjectToAttack == NULL || _pObjectToAttackComponent == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	m_pCurrentAttackTarget			= _pObjectToAttack;
	m_pCurrentAttackTargetComponent = _pObjectToAttackComponent;
	m_vCurrentBossAttackTargetPos	= _vAttackPosition;
	
	SwitchHomeBaseState( HOME_BASE_STATE_ATTACKING_START );
	
	CSoundManager::GetInstance().PlayTapSound();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::SetUpVineForAttack()
{
	if( m_pCurrentAttackTarget == NULL || m_pCurrentAttackTargetComponent == NULL || m_pVineObjectA == NULL )
		return;
	
	t2dVector vHomeBaseToEnemy;
	
	// Get the vector from the Home Base to the Enemy being Attacked
	if( m_eCurrentAttackTargetType == TARGET_TYPE_BOSS )
		vHomeBaseToEnemy = (m_vCurrentBossAttackTargetPos - m_pHomeBaseObject->getPosition() );
	else
		vHomeBaseToEnemy = (m_pCurrentAttackTarget->getPosition() - m_pHomeBaseObject->getPosition() );
	
	// Get the length of the vector from the Home Base to the enemy being attacked
	F32 fLength = vHomeBaseToEnemy.len();
	t2dVector vSize = m_pVineObjectA->getSize();
	vSize.mY = fLength;
	m_pVineObjectA->setSize( vSize );
	
	// Get the angle between the Up-vector and the vector that goes from the Home Base to the enemy being attacked
	t2dVector vUp( 0.0f, -1.0f );
	t2dVector vHomeBaseToEnemyNormalized = vHomeBaseToEnemy;
	vHomeBaseToEnemyNormalized.normalise();
	
	F32 fDot = vUp.dot( vHomeBaseToEnemyNormalized );
	F32 fAngleRadians = mAcos( fDot );
	
	F32 fAngleDegrees = mRadToDeg( fAngleRadians );
	
	// If the vector is on the left side of the screen, then reverse the angle (since the angle from the dot product definition can never be greater than 180 degrees (PI radians)
	if( vHomeBaseToEnemy.mX < 0.0f )
		fAngleDegrees *= -1.0f;
	
	// Set the rotation of the Vine Object
	m_pVineObjectA->setRotation( fAngleDegrees );
	
	// Set the position of the Vine Object
	m_pVineObjectA->setPosition( m_pHomeBaseObject->getPosition() ); // Is this line necessary?
	
	// Set the position of the Vine Object on the halfway point between the Home Base and the enemy
	t2dVector vNewVinePosition;
	t2dVector vHalfHomeBaseToEnemy( 0.5f * vHomeBaseToEnemy.mX, 0.5f * vHomeBaseToEnemy.mY );
	vNewVinePosition = ( vHalfHomeBaseToEnemy + m_pHomeBaseObject->getPosition() );
	
	m_pVineObjectA->setPosition( vNewVinePosition );
	
	m_pVineObjectA->setVisible( true );
	m_pVineObjectA->playAnimation( g_szVineAnimationName, false );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::AttackBlob()
{
	if( m_pCurrentAttackTargetComponent == NULL )
		return;
	
	CComponentEnemyBlob* pEnemy = static_cast<CComponentEnemyBlob*>( m_pCurrentAttackTargetComponent );
	if( pEnemy )
		pEnemy->Kill();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::AttackSerpent()
{
	if( m_pCurrentAttackTargetComponent == NULL )
		return;
	
	CComponentEnemySerpent* pEnemy = static_cast<CComponentEnemySerpent*>( m_pCurrentAttackTargetComponent );
	if( pEnemy )
		pEnemy->Kill();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::AttackSmoke()
{
	if( m_pCurrentAttackTargetComponent == NULL )
		return;
	
	CComponentEnemySmoke* pEnemy = static_cast<CComponentEnemySmoke*>( m_pCurrentAttackTargetComponent );
	if( pEnemy )
		pEnemy->Kill();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::AttackBoss()
{
	if( m_pCurrentAttackTargetComponent == NULL )
		return;
	
	CComponentBossMountain* pEnemy = static_cast<CComponentBossMountain*>( m_pCurrentAttackTargetComponent );
	if( pEnemy )
		pEnemy->DamageBoss( g_fBossDamageAmount );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPlayerHomeBase::OnSwitchHomeBaseState( const EPlayerHomeBaseState _eNewHomeBaseState )
{
	switch( _eNewHomeBaseState )
	{
		case HOME_BASE_STATE_IDLE:
			m_pHomeBaseObject->playAnimation( m_pszAnimIdleLoop, false );
			break;
			
		case HOME_BASE_STATE_GROW_LOOPING:
			m_pHomeBaseObject->playAnimation( m_pszAnimGrowLoop, false );
			break;
			
		case HOME_BASE_STATE_ATTACKING:
			m_pHomeBaseObject->playAnimation( g_szHomeBaseVineAttackAnimName, false );
			break;
			
		case HOME_BASE_STATE_EATING:
			m_pHomeBaseObject->playAnimation( m_pszEatAnim, false );
			break;
			
		case HOME_BASE_STATE_WIN:
			m_pHomeBaseObject->playAnimation( m_pszWinAnim, false );
			break;
			
		case HOME_BASE_STATE_HITREACTION:
			m_pHomeBaseObject->playAnimation( g_szHomeBaseHitReactionAnimName, false );
			break;
			
		case HOME_BASE_STATE_DEAD:
			m_pHomeBaseObject->playAnimation( m_pszDeadAnim, false );
			break;
			
		case HOME_BASE_STATE_GROW_START:
			m_pHomeBaseObject->playAnimation( m_pszAnimGrowStart, false );
			break;
			
		case HOME_BASE_STATE_SEED_MAGNET:
			m_pHomeBaseObject->playAnimation( g_szSeedMagnetAnimationName, false );
			break;
			
		case HOME_BASE_STATE_MOVING:
			m_pHomeBaseObject->playAnimation( m_pszAnimMoveLoop, false );
			break;
			
		case HOME_BASE_STATE_WAITING_FOR_END_SEQUENCE:
			//CComponentManaSeedItemDrop::HideAllSeeds();
			break;
			
		case HOME_BASE_STATE_OUT_OF_MANA:
			m_pHomeBaseObject->playAnimation( g_szHomeBaseOutOfManaAnim, false );
			break;
			
		default:
			break;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPlayerHomeBase::CanHomeBaseSwitchStates( const EPlayerHomeBaseState _eNewHomeBaseState ) const
{
	if( _eNewHomeBaseState == HOME_BASE_STATE_WIN || _eNewHomeBaseState == HOME_BASE_STATE_DEAD  || _eNewHomeBaseState == HOME_BASE_STATE_WAITING_FOR_END_SEQUENCE )
		return true;
	
	if( m_ePlayerHomeBaseState == HOME_BASE_STATE_WIN || m_ePlayerHomeBaseState == HOME_BASE_STATE_DEAD )
	{
		return false;
	}
	else if( m_ePlayerHomeBaseState == HOME_BASE_STATE_ATTACKING_START )
	{
		// If the home base is in "attacking start" mode, the ONLY mode that it should be switching to is "attacking"
		if( _eNewHomeBaseState != HOME_BASE_STATE_ATTACKING )
		{
			return false;
		}
	}
	else if( m_ePlayerHomeBaseState == HOME_BASE_STATE_ATTACKING )
	{
		// If the home base has an "object to attack", then it hasn't finished its attack sequence
		if( m_pCurrentAttackTarget != NULL )
		{
			return false;
		}
	}
	
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------










