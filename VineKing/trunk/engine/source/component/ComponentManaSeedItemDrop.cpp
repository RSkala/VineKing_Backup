//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentManaSeedItemDrop.h"

#include "ComponentBossMountain.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Static Member initialization

Vector<t2dAnimatedSprite*> CComponentManaSeedItemDrop::sm_ManaSeedItemDropPool;
bool CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolInitialized = false;
bool CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolCleared = false;
t2dAnimatedSprite* CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const U32 g_uMaxManaSeeds = 25; //50;

// Distance
static const F32 g_fMinEatDistance = 8.0f;
static const F32 g_fMinEatDistanceSquared = g_fMinEatDistance * g_fMinEatDistance;

// Layers
static const F32 g_fSeedLayerChangeDistance = 64.0f;
static const F32 g_fSeedLayerChangeDistanceSquared = g_fSeedLayerChangeDistance * g_fSeedLayerChangeDistance; // The distance at which the layer of the seeds will be changed to be above the player
static const S32 g_iSeedLayerAbovePlayer = LAYER_PLAYER_BLOB_SERPENT - 1;


// Seed Speed - How fast a seed travels towards the home base when being pulled from "Seed Magnet Mode" (Units/Second)
static const F32 g_fSeedSpeeds[] =
{
	80.0f,
	85.0f,
	90.0f,
	95.0f,
	100.0f,
	105.0f,
	120.0f
};

static F32 g_fSeedPullSpeed = g_fSeedSpeeds[0];
	

// Mana Values
static const S32 g_iGrassDropManaValue = 20;
static const S32 g_iEnemyDropManaValue = 30;

// Active Time / Blinking
static const F32 g_fSeedActiveTime = 7.0f; // Amount of time a seed will stay active on the screen
static const S32 g_fBlinkStartTime = 6.0f;	// Amount of time a seed has to be active before it starts blinking.
static const S32 g_fBlinkTime = 0.333f;		// Cycling time for ON/OFF when a seed is blinking.

// Boss Values
static const F32 g_fBossSeedSuckSpeed = 250.0f;
static t2dVector g_vTempBossPosition;

static const t2dVector g_vSafePosition( 500.0f, -500.0f );

extern bool g_bForceFullyUpgraded;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentManaSeedItemDrop );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentManaSeedItemDrop::CComponentManaSeedItemDrop()
	: m_pManaSeedObject( NULL )
	, m_bInUse( false )
	, m_pszGrassDropAnim( NULL )
	, m_pszEnemyDropAnim( NULL )
	, m_iGrassDropValue( 0 )
	, m_iEnemyDropValue( 0 )
	, m_iManaValue( 0 )
	, m_fActiveTimer( 0.0f )
	, m_bBlinkedOut( false )
	, m_fBlinkTimer( 0.0f )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentManaSeedItemDrop::~CComponentManaSeedItemDrop()
{
	if( CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolCleared == false )
	{
		CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolCleared = true;
		CComponentManaSeedItemDrop::ClearManaSeedPool();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::SetManaSeedNotInUse()
{
	m_bInUse = false;
	m_fActiveTimer = 0.0f;
	m_bBlinkedOut = false;
	m_fBlinkTimer = 0.0f;
	m_pManaSeedObject->setBlendAlpha( 1.0f );
	
	m_pManaSeedObject->setLayer( LAYER_SEEDS );
	
	if( m_pManaSeedObject )
	{
		m_pManaSeedObject->setVisible( false );
		m_pManaSeedObject->setPosition( g_vSafePosition );
		
		m_pManaSeedObject->setUseMouseEvents( false );
	}
	
	//if( CComponentTutorialLevel::IsTutorialLevel() )
	//{
	//	if( CComponentTutorialLevel::HasTutorialBeenShown_LineDraw() == true && CComponentTutorialLevel::HasTutorialBeenShown_Blava() == false )
	//	{
	//		if( CComponentManaSeedItemDrop::AreThereAnySeedsActive() == false )
	//			CComponentTutorialLevel::ShowTutorialBlava();
	//	}
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::SpawnSeedFromGrassAtPosition( const t2dVector& _vSpawnPosition )
{
	CComponentManaSeedItemDrop* pManaSeedComponent = GetNextAvailableSeed();
	if( pManaSeedComponent )
	{
		//printf( "Spawning Seed from grass at position (%f, %f)\n", _vSpawnPosition.mX, _vSpawnPosition.mY );
		pManaSeedComponent->SpawnFromGrassAtPosition( _vSpawnPosition );
		
		//pManaSeedComponent->SetOwnerUseMouseEvents(); // Uncomment this to enable seed touch-to-eat.
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::SpawnSeedFromEnemyAtPosition( const t2dVector& _vSpawnPosition )
{
	CComponentManaSeedItemDrop* pManaSeedComponent = GetNextAvailableSeed();
	if( pManaSeedComponent )
	{
		//printf( "Spawning Seed from enemy at position (%f, %f)\n", _vSpawnPosition.mX, _vSpawnPosition.mY );
		pManaSeedComponent->SpawnFromEnemyAtPosition( _vSpawnPosition );
		
		//pManaSeedComponent->SetOwnerUseMouseEvents(); // Uncomment this to enable seed touch-to-eat.
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ CComponentManaSeedItemDrop* CComponentManaSeedItemDrop::GetRandomVisibleSeed()
{
	Vector<S32> componentIndexes;
	
	CComponentManaSeedItemDrop* pManaSeedComponent = NULL;
	t2dAnimatedSprite* pManaSeed = NULL;
	//S32 iNumComponents = 0;
	
	// For now, just get the first one found that is visible
	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size();
	for( S32 i = 0; i < iNumManaSeeds; ++i )
	{
		pManaSeed = sm_ManaSeedItemDropPool[ i ];
		if( pManaSeed )
		{
			pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>(pManaSeed->getComponent( 0 )); // Use a static cast, as I know this will always be the only component attached
			if( pManaSeedComponent && pManaSeedComponent->IsInUse() )
				componentIndexes.push_back( i );
			
			pManaSeedComponent = NULL;
		}
	}
	
	S32 iNumIndexes = componentIndexes.size();
	if( iNumIndexes > 0 )
	{
		S32 iIndex = CComponentGlobals::GetInstance().GetRandomRange( 0,  iNumIndexes - 1 );
		pManaSeed = sm_ManaSeedItemDropPool[ iIndex ];
		if( pManaSeed )
		{
			pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>(pManaSeed->getComponent( 0 ));
			return pManaSeedComponent;
		}
	}
	
	//printf( "No visible seeds available\n" );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::GetOwnerPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = m_pManaSeedObject ? m_pManaSeedObject->getPosition() : t2dVector::getZero();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::SetOwnerUseMouseEvents()
{
	if( m_pManaSeedObject )
		m_pManaSeedObject->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::InitializeManaSeedDropData()
{
	//printf( "Setting up Mana Seed Item Drop Data...\n" );
	
	//F32 fPercentHealthXPAdded = CComponentGlobals::GetInstance().GetPercentHealthXPAdded(); // Get the amount of XP that the player added to Health in the LevelUp Screen
	//printf( "- Percent Health XP Added: %f\n", fPercentHealthXPAdded );
	
	//F32 fPercentSpeedXPAdded = CComponentGlobals::GetInstance().GetPercentSpeedXPAdded();
	//printf( "- Percent Speed XP Added: %f\n", fPercentSpeedXPAdded );
	
	//g_fCurrentManaSeedPullSpeed = g_fBaseSeedPullSpeed;
	//printf( "- Current Speed: %f\n", g_fCurrentManaSeedPullSpeed );
	//F32 fSpeedAmountToAdd = g_fMaxSeedPullSpeedGrowth * fPercentSpeedXPAdded;
	//g_fCurrentManaSeedPullSpeed += fSpeedAmountToAdd;
	//printf( "- New Speed: %f\n", g_fCurrentManaSeedPullSpeed );
	
	if( g_bForceFullyUpgraded == false )
	{
		// Cheat Disabled
		g_fSeedPullSpeed = g_fSeedSpeeds[ CComponentGlobals::GetInstance().GetXPLevelGreen() ];
	}
	else
	{
		// Cheat Enabled
		g_fSeedPullSpeed = g_fSeedSpeeds[ 6 ];
	}
	//printf( "g_fSeedPullSpeed: %f\n", g_fSeedPullSpeed );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::HideAllSeeds()
{
	t2dAnimatedSprite* pManaSeed = NULL;
	CComponentManaSeedItemDrop* pManaSeedComponent = NULL;
	S32 iNumComponents = 0;
	
	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size(); // RKS TODO: Optimize this -- create a list of the components
	for( S32 i = 0; i < iNumManaSeeds; ++i )
	{
		pManaSeed = sm_ManaSeedItemDropPool[ i ];
		if( pManaSeed )
		{
			iNumComponents = pManaSeed->getComponentCount();
			for( S32 j = 0; j < iNumComponents; ++j )
			{
				pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>(pManaSeed->getComponent(j));
				if( pManaSeedComponent )
					pManaSeedComponent->SetManaSeedNotInUse();
				
				pManaSeedComponent = NULL;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentManaSeedItemDrop::AreThereAnySeedsActive()
{
	t2dAnimatedSprite* pManaSeed = NULL;
	CComponentManaSeedItemDrop* pManaSeedComponent = NULL;
	
	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size();
	for( S32 i = 0; i < iNumManaSeeds; ++i )
	{
		pManaSeed = sm_ManaSeedItemDropPool[ i ];
		if( pManaSeed )
		{
			pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>( pManaSeed->getComponent(0) );
			if( pManaSeedComponent && pManaSeedComponent->IsInUse() )
				return true;
		}
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::OnEndLevelSequenceStarted()
{
	// If the end level sequence started, then force all active seeds to start to blink out
	
	t2dAnimatedSprite* pManaSeed = NULL;
	CComponentManaSeedItemDrop* pManaSeedComponent = NULL;
	
	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size();
	for( S32 i = 0; i < iNumManaSeeds; ++i )
	{
		pManaSeed = sm_ManaSeedItemDropPool[ i ];
		if( pManaSeed )
		{
			pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>( pManaSeed->getComponent(0) );
			if( pManaSeedComponent && pManaSeedComponent->IsInUse() )
			{
				if( pManaSeedComponent->m_fActiveTimer <= g_fBlinkStartTime )
					pManaSeedComponent->m_fActiveTimer = g_fBlinkStartTime;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::HideAllManaSeeds()
{
    t2dAnimatedSprite* pManaSeed = NULL;
    CComponentManaSeedItemDrop* pManaSeedComponent = NULL;
	
	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size();
	for( S32 i = 0; i < iNumManaSeeds; ++i )
	{
		pManaSeed = sm_ManaSeedItemDropPool[ i ];
		if( pManaSeed )
		{
			pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>( pManaSeed->getComponent(0) );
			if( pManaSeedComponent )
                pManaSeedComponent->SetManaSeedNotInUse();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pManaSeedObject, CComponentManaSeedItemDrop ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "GrassDropAnim",		TypeString,	Offset( m_pszGrassDropAnim, CComponentManaSeedItemDrop ) );
	addField( "GrassDropManaValue", TypeS32,	Offset( m_iGrassDropValue,	CComponentManaSeedItemDrop ) );
	
	addField( "EnemyDropAnim",		TypeString,	Offset( m_pszEnemyDropAnim, CComponentManaSeedItemDrop ) );
	addField( "EnemyDropManaValue", TypeS32,	Offset( m_iEnemyDropValue,	CComponentManaSeedItemDrop ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from SimComponent
bool CComponentManaSeedItemDrop::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentManaSeedItemDrop::onComponentAdd - Must be added to a t2dAnimatedSprite.\n" );
		return false;
	}
	
	m_pManaSeedObject = pOwnerObject;
	
	// NOTE: Do not set up any other member pointers here, because Torque may not have created the objects yet
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::onUpdate()
{
	DynamicConsoleMethodComponent::onUpdate();
	
	if( m_pManaSeedObject == NULL )
		return;
	
	if( CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolInitialized == false )
	{
		CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolInitialized = true; // This is called here for now because when InitializeManaSeedPool is called, it clone() is called on the object which calls this function again.
		CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolCleared = false;
		CComponentManaSeedItemDrop::InitializeManaSeedPool( m_pManaSeedObject );
		
		m_pManaSeedObject->disableUpdateCallback();
		m_pManaSeedObject->setVisible( false );
		m_pManaSeedObject->setUseMouseEvents( false );
	}
	
	// The the current level has an active boss, then the boss will suck in any active seeds
	if( CComponentBossMountain::LevelHasActiveBoss() )
	{
		// Make sure this seed is actually in use
		if( IsInUse() )
		{
			if( m_bBlinkedOut )
			{
				m_pManaSeedObject->setBlendAlpha( 1.0f );
				m_bBlinkedOut = false;
			}
			
			CComponentBossMountain::GetCurrentAttackingBossMouthPosition( g_vTempBossPosition );
			
			t2dVector vManaSeedPosition = m_pManaSeedObject->getPosition();
			t2dVector vSeedToBoss = g_vTempBossPosition - vManaSeedPosition;
			
			// Scale the length of the SeedToBoss vector to get the new position for this seed
			t2dVector vSeedToBossDirection = vSeedToBoss;
			vSeedToBossDirection.normalise();
			
			F32 fDistance = g_fBossSeedSuckSpeed * ITickable::smTickSec;
			vSeedToBossDirection *= fDistance; // Scale the direction vector to the distance the seed will travel
			
			// Add the distance vector to this seed's position to get its new position (pulled towards the boss)
			t2dVector vNewPosition = vManaSeedPosition + vSeedToBossDirection;
			m_pManaSeedObject->setPosition( vNewPosition );

			
			F32 fLengthSquared = vSeedToBoss.lenSquared();
			if( fLengthSquared <= g_fMinEatDistanceSquared )
			{
				SetManaSeedNotInUse();
				CSoundManager::GetInstance().PlayEatSound();
			}
		}
		
		return;
	}
	
	// If the home base is in "Seed Magnet" mode, then move the seed towards the home base.
	if( CComponentPlayerHomeBase::GetInstance().IsHomeBaseInSeedMagnetMode() )
	{
		// Make sure this seed is actually in use
		if( IsInUse() )
		{
			if( m_bBlinkedOut )
			{
				m_pManaSeedObject->setBlendAlpha( 1.0f );
				m_bBlinkedOut = false;
			}
			
			t2dVector vHomeBasePosition;
			CComponentPlayerHomeBase::GetInstance().GetHomeBasePosition( vHomeBasePosition );
			vHomeBasePosition.mY += 32.0f;
			
			t2dVector vManaSeedPosition = m_pManaSeedObject->getPosition();
			t2dVector vSeedToHomeBase = vHomeBasePosition - vManaSeedPosition;
			
			// Scale the length of the SeedToHomeBase vector to get the new position for this seed
			t2dVector vSeedToHomeBaseDirection = vSeedToHomeBase;
			vSeedToHomeBaseDirection.normalise();
			
			F32 fDistance = g_fSeedPullSpeed * ITickable::smTickSec;
			vSeedToHomeBaseDirection *= fDistance; // Scale the direction vector to the distance the seed will travel
			
			// Add the distance vector to this seed's position to get its new position (pulled towards the home base)
			t2dVector vNewPosition = vManaSeedPosition + vSeedToHomeBaseDirection;
			m_pManaSeedObject->setPosition( vNewPosition );			
			
			F32 fLengthSquared = vSeedToHomeBase.lenSquared();
			if( fLengthSquared <= g_fMinEatDistanceSquared )
			{
				SetManaSeedNotInUse();
				
				F32 fManaValue = (F32)m_iManaValue;
				CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToEatSeedFromMagnetMode( fManaValue );
			}
			else if( fLengthSquared <= g_fSeedLayerChangeDistanceSquared )
			{
				m_pManaSeedObject->setLayer( g_iSeedLayerAbovePlayer );
			}
		}
		
		return;
	}
	
	// If this seed is active on screen for a certain amount of time, then it should become inactive. Note that this step is done after
	// the check for home base "Magnet Mode" -- this is intentional, so the seed doesn't disappear while being pulled in.
	if( IsInUse() )
	{
        // In the Tutorial, seeds will never disappear.
        if( CComponentTutorialLevel::IsTutorialLevel() == false )
        {
            m_fActiveTimer += ITickable::smTickSec;
            
            if( m_fActiveTimer >= g_fBlinkStartTime )
            {
                m_fBlinkTimer += ITickable::smTickSec;
                if( m_fBlinkTimer >= g_fBlinkTime )
                {
                    m_fBlinkTimer = 0.0f;
                    
                    if( m_bBlinkedOut )
                    {
                        // The seed is currently invisible. Make it visible again.
                        m_pManaSeedObject->setBlendAlpha( 1.0f );
                        m_bBlinkedOut = false;
                    }
                    else
                    {
                        // The seed is currently visible.  Make it invisible.
                        m_pManaSeedObject->setBlendAlpha( 0.5f );
                        m_bBlinkedOut = true;
                    }
                }
            }
            
            if( m_fActiveTimer >= g_fSeedActiveTime )
            {
                SetManaSeedNotInUse();
                m_fActiveTimer = 0.0f;
                m_fBlinkTimer = 0.0f;
            }
        }
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pManaSeedObject == NULL )
		return;
	
	////////////////////////////////////////////////////////////////////////////////////
	// NEW TOUCHING STUFF
	if( CComponentGlobals::GetInstance().IsTouchingDown() )
		return;
	
	CComponentGlobals::GetInstance().SetTouchedObject( m_pManaSeedObject );
	////////////////////////////////////////////////////////////////////////////////////
	
	//CComponentPlayerHomeBase::GetInstance().IsHomeBaseInSeedMagnetMode()
	//CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToAttackSeed( m_pManaSeedObject, this ); 

	
//	if( CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject != NULL )
//		return;
//	
//	m_bInUse = true;
//	
//	m_pManaSeedObject->setPosition( _vWorldMousePoint );
//	
//	CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject = m_pManaSeedObject;
//	
//	// Make sure the player drags this object even when not touching it (from, for example, moving his finger very fast)
//	m_pManaSeedObject->setMouseLocked( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pManaSeedObject == NULL )
		return;
	
	if( CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject == m_pManaSeedObject )
	{
		m_pManaSeedObject->setMouseLocked( false );
	}
	
	CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	if( m_pManaSeedObject == NULL )
		return;
	
	if( CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject != m_pManaSeedObject || CComponentManaSeedItemDrop::sm_pCurrentlyHeldManaSeedItemDropObject == NULL )
		return;
	
	m_pManaSeedObject->setPosition( _vWorldMousePoint );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

/*static*/ void CComponentManaSeedItemDrop::InitializeManaSeedPool( t2dAnimatedSprite* const _pManaSeedObject )
{
	if( _pManaSeedObject == NULL )
		return;
	
	_pManaSeedObject->setLayer( LAYER_SEEDS );
	
	AddObjectToManaSeedPool( _pManaSeedObject );
	
	for( U32 i = 1; i < g_uMaxManaSeeds; ++i )
	{
		t2dAnimatedSprite* pNewManaSeed = static_cast<t2dAnimatedSprite*>(_pManaSeedObject->clone());
		if( pNewManaSeed == NULL )
		{
			printf( "%s - Error cloning mana seed object.\n", __FUNCTION__ );
			continue;
		}
		
		S32 iNumComponents = pNewManaSeed->getComponentCount();
		if( iNumComponents <= 0 )
		{
			printf( "%s - Mana seed has zero components.\n", __FUNCTION__ );
			continue;
		}
		
		AddObjectToManaSeedPool( pNewManaSeed );
	}
	
	//S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size();
	//printf( "Created %i Mana Seed Item Drops\n", iNumManaSeeds );
	
	sm_bManaSeedItemDropPoolInitialized = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::AddObjectToManaSeedPool( t2dAnimatedSprite* const _pManaSeed )
{
	if( _pManaSeed == NULL )
		return;
	
	sm_ManaSeedItemDropPool.push_back( _pManaSeed );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaSeedItemDrop::ClearManaSeedPool()
{
	//t2dAnimatedSprite* pManaSeed = NULL;

	// RKS TODO: Deleting the objects might not be necessary, as they may get cleared out automatically by Torque when the game exits. 
	//  I'm guessing that's the correct way, as the pools are only allocated when the level loads
//	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size();
//	for( S32 i = 0; i < iNumManaSeeds; ++i )
//	{
//		pManaSeed = sm_ManaSeedItemDropPool[ i ];
//		if( pManaSeed )
//			pManaSeed->deleteObject();
//	}
	
	sm_ManaSeedItemDropPool.clear();
	
	CComponentManaSeedItemDrop::sm_bManaSeedItemDropPoolInitialized = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ CComponentManaSeedItemDrop* CComponentManaSeedItemDrop::GetNextAvailableSeed()
{
	t2dAnimatedSprite* pManaSeed = NULL;
	CComponentManaSeedItemDrop* pManaSeedComponent = NULL;
	S32 iNumComponents = 0;
	S32 iNumManaSeeds = sm_ManaSeedItemDropPool.size(); // RKS TODO: Optimize this -- create a list of the components
	for( S32 i = 0; i < iNumManaSeeds; ++i )
	{
		pManaSeed = sm_ManaSeedItemDropPool[ i ];
		if( pManaSeed )
		{
			iNumComponents = pManaSeed->getComponentCount();
			for( S32 j = 0; j < iNumComponents; ++j )
			{
				pManaSeedComponent = static_cast<CComponentManaSeedItemDrop*>(pManaSeed->getComponent(j));
				if( pManaSeedComponent && pManaSeedComponent->IsInUse() == false )
					return pManaSeedComponent;
				
				pManaSeedComponent = NULL;
			}
		}
	}
	
	//printf( "No available Mana Seeds -- increase the size of the Mana Seed Item Drop pool.\n" );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::SpawnFromGrassAtPosition( const t2dVector& _vSpawnPosition )
{
	if( m_pManaSeedObject == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
    
    if( CComponentTutorialLevel::IsTutorialLevel() == true )
    {
        // If this is the tutorial level, always spawn large seeds.
        SpawnFromEnemyAtPosition( _vSpawnPosition );
        return;
    }
	
	m_pManaSeedObject->playAnimation( m_pszGrassDropAnim, false );
	m_iManaValue = g_iGrassDropManaValue;
	
	SpawnAtPosition( _vSpawnPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::SpawnFromEnemyAtPosition( const t2dVector& _vSpawnPosition )
{
	if( m_pManaSeedObject == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	m_pManaSeedObject->playAnimation( m_pszEnemyDropAnim, false );
	//m_pManaSeedObject->playAnimation( m_pszGrassDropAnim, false );
	m_iManaValue = g_iEnemyDropManaValue;
	
	SpawnAtPosition( _vSpawnPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::SpawnAtPosition( const t2dVector& _vSpawnPosition )
{	
	if( m_pManaSeedObject == NULL )
		return;
	
	m_bInUse = true;
	
	m_pManaSeedObject->setPosition( _vSpawnPosition );
	m_pManaSeedObject->setVisible( true );
	
	if( CComponentTutorialLevel::IsTutorialLevel() )
	{
		if( CComponentTutorialLevel::HasTutorialBeenShown_SeedSuck() == false )
		{
			CComponentTutorialLevel::StartTutorialSeedSuckDelayTimer();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaSeedItemDrop::OnPostInit()
{
	SetManaSeedNotInUse();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
