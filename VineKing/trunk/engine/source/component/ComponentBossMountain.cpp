//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentBossMountain.h"

#include "ComponentEnemySmokePositionList.h"
#include "ComponentGlobals.h"
#include "ComponentPlayerHomeBase.h"

#include "SoundManager.h"

#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//static const char g_szEyeLeft[32]	= "BossMountain_EyeLeft";
//static const char g_szEyeRight[32]	= "BossMountain_EyeRight";

// RKS TODO: Move this into the Editor
static const char g_szEyeLeft[32]	= "Boss_LeftEye";
static const char g_szEyeRight[32]	= "Boss_RightEye";

// Left and Right Boss Animations
static const char g_szLeftBoss_IDLE01[32] = "LeftBoss_IDLE01_NEW";
static const char g_szLeftBoss_IDLE02[32] = "LeftBoss_IDLE02_NEW";
static const char g_szLeftBoss_IDLE03[32] = "LeftBoss_IDLE03_NEW";
static const char g_szLeftBoss_HIT01[32] = "LeftBoss_HIT01_NEW";
static const char g_szLeftBoss_HIT02[32] = "LeftBoss_HIT02_NEW";
static const char g_szLeftBoss_HIT03[32] = "LeftBoss_HIT03_NEW";
static const char g_szLeftBoss_DEATH[32] = "LeftBoss_DEATH_NEW";

// Center Boss Animations
static const char g_szCenterBoss_IDLE01[32] = "CENTER_BOSS_IDLE01";
static const char g_szCenterBoss_IDLE02[32] = "CENTER_BOSS_IDLE02";
static const char g_szCenterBoss_IDLE03[32] = "CENTER_BOSS_IDLE03";
static const char g_szCenterBoss_HIT01[32] = "CENTER_BOSS_HIT01";
static const char g_szCenterBoss_HIT02[32] = "CENTER_BOSS_HIT02";
static const char g_szCenterBoss_HIT03[32] = "CENTER_BOSS_HIT03";
static const char g_szCenterBoss_DEATH[32] = "CENTER_BOSS_DEATH";

// Left Boss Objects
static const char g_szBossLeft[32]				= "BossLeft";
static const char g_szBossLeft_LeftEye[32]		= "BossLeft_LeftEye";
static const char g_szBossLeft_RightEye[32]		= "BossLeft_RightEye";
static const char g_szBossLeft_Jaw[32]			= "BossLeft_Jaw";

// Right Boss Objects
static const char g_szBossRight[32]				= "BossRight";
static const char g_szBossRight_LeftEye[32]		= "BossRight_LeftEye";
static const char g_szBossRight_RightEye[32]	= "BossRight_RightEye";
static const char g_szBossRight_Jaw[32]			= "BossRight_Jaw";

// Center Boss Objects
static const char g_szBossCenter[32]			= "BossCenter";
static const char g_szBossCenter_LeftEye[32]	= "BossCenter_LeftEye";
static const char g_szBossCenter_RightEye[32]	= "BossCenter_RightEye";
static const char g_szBossCenter_Jaw[32]		= "BossCenter_Jaw";

// Boss Eye Animations
static const char g_szBossEye_IDLE[32]	= "BossEye_IDLE";
static const char g_szBossEye_HIT[32]	= "BossEye_HIT";
static const char g_szBossEye_SHOOT[32]	= "BossEye_SHOOT";
static const char g_szBossEye_DEATH[32]	= "BossEye_DEATH";

// Boss Health
static const F32 g_fBossHealth = 1000.0f;
static const F32 g_fMediumHealth	= g_fBossHealth * 0.50f;
static const F32 g_fLowHealth		= g_fBossHealth * 0.25f;

//1.) Full Health		- 100% - 51% Health
//2.) Medium Health		- 50%  - 26% Health
//3.) Low Health		- 25%  - 1%  Health
//4.) Dead				- 0% Health

// Boss Mouth Position offsets
static const t2dVector g_vMouthOffsetBoss_Left( 7.0f, 64.0f );
static const t2dVector g_vMouthOffsetBoss_Center( -15.0f, 66.0f );
static const t2dVector g_vMouthOffsetBoss_Right( -7.0f, 64.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<CComponentBossMountain*> CComponentBossMountain::sm_BossMountainList;
S32 CComponentBossMountain::sm_iCurrentAttackingBossIndex = 0;
CComponentBossMountain* CComponentBossMountain::sm_pCurrentAttackingBoss = NULL; // The current boss that will attack bricks

CComponentBossMountain* CComponentBossMountain::sm_pBossLeft	= NULL;
CComponentBossMountain* CComponentBossMountain::sm_pBossRight	= NULL;
CComponentBossMountain* CComponentBossMountain::sm_pBossCenter	= NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentBossMountain );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentBossMountain::CComponentBossMountain()
	: m_pOwner( NULL )
	, m_eBossState( BOSS_STATE_NONE )
	, m_fCurrentHealth( g_fBossHealth )
	, m_bDead( false )
	, m_pEyeLeft( NULL )
	, m_pEyeRight( NULL )
	, m_pszLinkedSmokeIndex_01( NULL )
	, m_pszLinkedSmokeIndex_02( NULL )
	, m_pszLinkedSmokeIndex_03( NULL )
	, m_iLinkedSmokeIndex_01( -1 )
	, m_iLinkedSmokeIndex_02( -1 )
	, m_iLinkedSmokeIndex_03( -1 )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentBossMountain::~CComponentBossMountain()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentBossMountain::FireBrickProjectileAtPosition( const t2dVector& _vEndPosition )
{
	if( sm_pCurrentAttackingBoss == NULL )
		return;
	
	static bool bLeft = true;
	
	t2dVector vFirePosition;
	if( bLeft )
		sm_pCurrentAttackingBoss->GetLeftEyePosition( vFirePosition );
	else
		sm_pCurrentAttackingBoss->GetRightEyePosition( vFirePosition );
	
	CComponentGlobals::GetInstance().FireBossBrickAttackProjectileAtPosition( vFirePosition, _vEndPosition );
	
	if( sm_pCurrentAttackingBoss->m_pEyeLeft )
	{
		sm_pCurrentAttackingBoss->m_pEyeLeft->playAnimation( g_szBossEye_SHOOT, true );
		sm_pCurrentAttackingBoss->m_pEyeLeft->mAnimationController.SetLastAnimationName( g_szBossEye_IDLE );
	}
	
	if( sm_pCurrentAttackingBoss->m_pEyeRight )
	{
		sm_pCurrentAttackingBoss->m_pEyeRight->playAnimation( g_szBossEye_SHOOT, true );
		sm_pCurrentAttackingBoss->m_pEyeRight->mAnimationController.SetLastAnimationName( g_szBossEye_IDLE );
	}
	
	bLeft = !bLeft;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBossMountain::DamageBoss( const F32& _fDamageAmount )
{
	m_fCurrentHealth -= _fDamageAmount;
	
	if( m_pOwner == NULL )
		return;
	
	// Boss Animations
	if( m_fCurrentHealth > 0.0f )
	{
		if( m_eBossState == BOSS_STATE_IDLE01 )
		{
			//printf( "BOSS_STATE_IDLE01\n" );
			
			if( this == CComponentBossMountain::sm_pBossCenter )
			{
				m_pOwner->playAnimation( g_szCenterBoss_HIT01, true );
				m_pOwner->mAnimationController.SetLastAnimationName( g_szCenterBoss_IDLE01 );
			}
			else
			{
				m_pOwner->playAnimation( g_szLeftBoss_HIT01, true );
				m_pOwner->mAnimationController.SetLastAnimationName( g_szLeftBoss_IDLE01 );
			}
		}
		else if( m_eBossState == BOSS_STATE_IDLE02 )
		{
			//printf( "BOSS_STATE_IDLE02\n" );
			
			if( this == CComponentBossMountain::sm_pBossCenter )
			{
				m_pOwner->playAnimation( g_szCenterBoss_HIT02, true );
				m_pOwner->mAnimationController.SetLastAnimationName( g_szCenterBoss_IDLE02 );
			}
			else
			{
				m_pOwner->playAnimation( g_szLeftBoss_HIT02, true );
				m_pOwner->mAnimationController.SetLastAnimationName( g_szLeftBoss_IDLE02 );
			}
		}
		else if( m_eBossState == BOSS_STATE_IDLE03 )
		{
			//printf( "BOSS_STATE_IDLE03\n" );
			
			if( this == CComponentBossMountain::sm_pBossCenter )
			{
				m_pOwner->playAnimation( g_szCenterBoss_HIT03, true );
				m_pOwner->mAnimationController.SetLastAnimationName( g_szCenterBoss_IDLE03 );
			}
			else
			{
				m_pOwner->playAnimation( g_szLeftBoss_HIT03, true );
				m_pOwner->mAnimationController.SetLastAnimationName( g_szLeftBoss_IDLE03 );
			}
		}
	}
	
	// Eye Animations
	if( m_pEyeLeft )
	{
		m_pEyeLeft->playAnimation( g_szBossEye_HIT, true );
		m_pEyeLeft->mAnimationController.SetLastAnimationName( g_szBossEye_IDLE );
	}
	
	if( m_pEyeRight )
	{
		m_pEyeRight->playAnimation( g_szBossEye_HIT, true );
		m_pEyeRight->mAnimationController.SetLastAnimationName( g_szBossEye_IDLE );
	}
	
	
	if( m_fCurrentHealth <= 0.0f )
	{
		//printf( "Kill Boss\n" );
		m_fCurrentHealth = 0.0f;
		CSoundManager::GetInstance().PlayBossDeathSound();
		
		if( m_pOwner )
		{
			if( this == CComponentBossMountain::sm_pBossCenter )
			{
				m_pOwner->setUseMouseEvents( false );
				m_pOwner->playAnimation( g_szCenterBoss_DEATH, false );
			}
			else
			{
				m_pOwner->setUseMouseEvents( false );
				m_pOwner->playAnimation( g_szLeftBoss_DEATH, false );
			}
		}
		
		if( m_pEyeLeft )
			m_pEyeLeft->playAnimation( g_szBossEye_DEATH, false );
		
		if( m_pEyeRight )
			m_pEyeRight->playAnimation( g_szBossEye_DEATH, false );
		
		if( m_pJaw )
			m_pJaw->setVisible( false );
		
		m_bDead = true;
		
		// If this Boss is the one that is currently attacking, then clear set the next boss to be the one that fires (Boss firing priority:  Center, Right, Left)
		if( CComponentBossMountain::sm_pCurrentAttackingBoss == this )
		{
			if( CComponentBossMountain::sm_pBossCenter == this )
			{
				if( CComponentBossMountain::sm_pBossRight && CComponentBossMountain::sm_pBossRight->IsDead() == false )
					sm_pCurrentAttackingBoss = sm_pBossRight;
				else if( CComponentBossMountain::sm_pBossLeft && CComponentBossMountain::sm_pBossLeft->IsDead() == false )
					sm_pCurrentAttackingBoss = sm_pBossLeft;
				else 
					sm_pCurrentAttackingBoss = NULL;

			}
			else if( CComponentBossMountain::sm_pBossRight == this )
			{
				if( CComponentBossMountain::sm_pBossLeft && CComponentBossMountain::sm_pBossLeft->IsDead() == false )
					sm_pCurrentAttackingBoss = sm_pBossLeft;
				else 
					sm_pCurrentAttackingBoss = NULL;
			}
			else if( CComponentBossMountain::sm_pBossLeft == this )
				sm_pCurrentAttackingBoss = NULL;
		}
		
		if( m_iLinkedSmokeIndex_01 != -1 )
			CComponentEnemySmokePositionList::MarkPositionIndexAsInvalid( m_iLinkedSmokeIndex_01 );
		
		if( m_iLinkedSmokeIndex_02 != -1 )
			CComponentEnemySmokePositionList::MarkPositionIndexAsInvalid( m_iLinkedSmokeIndex_02 );
		
		if( m_iLinkedSmokeIndex_03 != -1 )
			CComponentEnemySmokePositionList::MarkPositionIndexAsInvalid( m_iLinkedSmokeIndex_03 );
		
		m_eBossState = BOSS_STATE_DYING;
		
		return;
	}
	else if( m_fCurrentHealth <= g_fLowHealth )
	{
		m_eBossState = BOSS_STATE_IDLE03;
	}
	else if( m_fCurrentHealth <= g_fMediumHealth )
	{
		m_eBossState = BOSS_STATE_IDLE02;
	}
	   
	CSoundManager::GetInstance().PlayBossDamageSound();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBossMountain::GetLeftEyePosition( t2dVector& _vOutPosition )
{
	_vOutPosition = m_pEyeLeft ? m_pEyeLeft->getPosition() : t2dVector::getZero();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBossMountain::GetRightEyePosition( t2dVector& _vOutPosition )
{
	_vOutPosition = m_pEyeRight ? m_pEyeRight->getPosition() : t2dVector::getZero();	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentBossMountain::SetUpBossData()
{
	sm_pCurrentAttackingBoss = NULL;
	
	if( sm_pBossLeft )
	{
		if( sm_pBossLeft->m_pOwner )
		{
			sm_pBossLeft->m_pOwner->setUseMouseEvents( true );
			sm_pBossLeft->m_pOwner->playAnimation( g_szLeftBoss_IDLE01, false );
		}
		
		sm_pBossLeft->m_pEyeLeft = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossLeft_LeftEye ) );
		if( sm_pBossLeft->m_pEyeLeft )
			sm_pBossLeft->m_pEyeLeft->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
			
		sm_pBossLeft->m_pEyeRight = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossLeft_RightEye ) );
		if( sm_pBossLeft->m_pEyeRight )
			sm_pBossLeft->m_pEyeRight->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossLeft->m_pJaw = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossLeft_Jaw ) );
		if( sm_pBossLeft->m_pJaw )
			sm_pBossLeft->m_pJaw->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossLeft->m_eBossState = BOSS_STATE_IDLE01;
		
		sm_pCurrentAttackingBoss = sm_pBossLeft;
	}
	
	if( sm_pBossRight )
	{
		if( sm_pBossRight->m_pOwner )
		{
			sm_pBossRight->m_pOwner->setUseMouseEvents( true );
			sm_pBossRight->m_pOwner->playAnimation( g_szLeftBoss_IDLE01, false );
		}
		
		sm_pBossRight->m_pEyeLeft = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossRight_LeftEye ) );
		if( sm_pBossRight->m_pEyeLeft )
			sm_pBossRight->m_pEyeLeft->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossRight->m_pEyeRight = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossRight_RightEye ) );
		if( sm_pBossRight->m_pEyeRight )
			sm_pBossRight->m_pEyeRight->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossRight->m_pJaw = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossRight_Jaw ) );
		if( sm_pBossRight->m_pJaw )
			sm_pBossRight->m_pJaw->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossRight->m_eBossState = BOSS_STATE_IDLE01;
		
		sm_pCurrentAttackingBoss = sm_pBossRight;
	}
	
	if( sm_pBossCenter )
	{
		if( sm_pBossCenter->m_pOwner )
		{
			sm_pBossCenter->m_pOwner->setUseMouseEvents( true );
			sm_pBossCenter->m_pOwner->playAnimation( g_szCenterBoss_IDLE01, false );
		}
		
		sm_pBossCenter->m_pEyeLeft = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossCenter_LeftEye ) );
		if( sm_pBossCenter->m_pEyeLeft )
			sm_pBossCenter->m_pEyeLeft->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossCenter->m_pEyeRight = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossCenter_RightEye ) );
		if( sm_pBossCenter->m_pEyeRight )
			sm_pBossCenter->m_pEyeRight->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossCenter->m_pJaw = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBossCenter_Jaw ) );
		if( sm_pBossCenter->m_pJaw )
			sm_pBossCenter->m_pJaw->setLayer( LAYER_BOSS_MOUNTAIN_PARTS );
		
		sm_pBossCenter->m_eBossState = BOSS_STATE_IDLE01;
		
		sm_pCurrentAttackingBoss = sm_pBossCenter;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentBossMountain::ClearBossData()
{
	sm_pCurrentAttackingBoss = NULL;
	sm_pBossLeft = NULL;
	sm_pBossRight = NULL;
	sm_pBossCenter = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentBossMountain::GetCurrentAttackingBossMouthPosition( t2dVector& _vOutPosition )
{
	if( sm_pCurrentAttackingBoss == NULL || sm_pCurrentAttackingBoss->m_pOwner == NULL )
	{
		_vOutPosition = t2dVector::getZero();
		return;
	}
	
	_vOutPosition = sm_pCurrentAttackingBoss->m_pOwner->getPosition();
	
	if( sm_pCurrentAttackingBoss == sm_pBossLeft )
		_vOutPosition += g_vMouthOffsetBoss_Left;
	else if( sm_pCurrentAttackingBoss == sm_pBossRight )
		_vOutPosition += g_vMouthOffsetBoss_Right;
	else if( sm_pCurrentAttackingBoss == sm_pBossCenter )
		_vOutPosition += g_vMouthOffsetBoss_Center;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentBossMountain::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentBossMountain ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "LinkedSmokeIndex_01", TypeCaseString, Offset( m_pszLinkedSmokeIndex_01, CComponentBossMountain ) );
	addField( "LinkedSmokeIndex_02", TypeCaseString, Offset( m_pszLinkedSmokeIndex_02, CComponentBossMountain ) );
	addField( "LinkedSmokeIndex_03", TypeCaseString, Offset( m_pszLinkedSmokeIndex_03, CComponentBossMountain ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentBossMountain::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s - Must be added to a t2dAnimatedSprite.", __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBossMountain::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_bDead )
	{
		if( m_pEyeLeft && m_pEyeLeft->getVisible() )
		{
			if( m_pOwner->mAnimationController.isAnimationFinished() )
				m_pEyeLeft->setVisible( false );
		}
		
		if( m_pEyeRight && m_pEyeRight->getVisible() )
		{
			if( m_pOwner->mAnimationController.isAnimationFinished() )
				m_pEyeRight->setVisible( false );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBossMountain::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsTimerActive() == false )
		return;
	
	if( CComponentPlayerHomeBase::GetInstance().DoesPlayerHomeBaseHaveAttackTarget() )
	{
		// Do nothing if the player home base is already attacking a target
		return;
	}
	if( CComponentGlobals::GetInstance().IsTouchingDown() )
		return;

	CComponentGlobals::GetInstance().SetTouchedObject( m_pOwner );

	CComponentGlobals::GetInstance().SetPlayerTouchingEnemy();
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackBoss( m_pOwner, this, _vWorldMousePoint );
	
	CComponentGlobals::GetInstance().PlayHitEffectAtPosition( _vWorldMousePoint );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBossMountain::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setLayer( LAYER_BOSS_MOUNTAIN );
	
	if( strcmp( m_pOwner->getName(), g_szBossLeft ) == 0 )
	{
		CComponentBossMountain::sm_pBossLeft = this;
		m_pOwner->setLayerOrder( 0 );
	}
	else if( strcmp( m_pOwner->getName(), g_szBossRight ) == 0 )
	{
		CComponentBossMountain::sm_pBossRight = this;
		m_pOwner->setLayerOrder( 0 );
	}
	else if( strcmp( m_pOwner->getName(), g_szBossCenter ) == 0 )
	{
		CComponentBossMountain::sm_pBossCenter = this;
		m_pOwner->setLayerOrder( 1 ); // The center mountain should be in front of the left and right ones
	}
	
	CComponentBossMountain::sm_BossMountainList.push_back( this );
	
	SetUpLinkedSmokeIndex( m_iLinkedSmokeIndex_01, m_pszLinkedSmokeIndex_01 );
	SetUpLinkedSmokeIndex( m_iLinkedSmokeIndex_02, m_pszLinkedSmokeIndex_02 );
	SetUpLinkedSmokeIndex( m_iLinkedSmokeIndex_03, m_pszLinkedSmokeIndex_03 );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private member functions

void CComponentBossMountain::SetUpLinkedSmokeIndex( S32& _iOutIndex, const char*& _pszIndex )
{
	if( _pszIndex == NULL || _pszIndex[0] == '\0' )
	{
		_iOutIndex = -1;
		return;
	}
	
	if( dStricmp( _pszIndex, "01" ) == 0 )
	{
		_iOutIndex = 0;
	}
	else if( dStricmp( _pszIndex, "02" ) == 0 )
	{
		_iOutIndex = 1;
	}
	else if( dStricmp( _pszIndex, "03" ) == 0 )
	{
		_iOutIndex = 2;
	}
	else if( dStricmp( _pszIndex, "04" ) == 0 )
	{
		_iOutIndex = 3;
	}
	else if( dStricmp( _pszIndex, "05" ) == 0 )
	{
		_iOutIndex = 4;
	}
	else if( dStricmp( _pszIndex, "06" ) == 0 )
	{
		_iOutIndex = 5;
	}
	else if( dStricmp( _pszIndex, "07" ) == 0 )
	{
		_iOutIndex = 6;
	}
	else if( dStricmp( _pszIndex, "08" ) == 0 )
	{
		_iOutIndex = 7;
	}
	else if( dStricmp( _pszIndex, "09" ) == 0 )
	{
		_iOutIndex = 8;
	}
	else if( dStricmp( _pszIndex, "10" ) == 0 )
	{
		_iOutIndex = 9;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------