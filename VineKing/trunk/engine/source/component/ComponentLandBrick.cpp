//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLandBrick.h"

#include "ComponentBossMountain.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentManaBar.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPauseScreen.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const S32 g_iMaxLandBrickHealth = 100;

// Unfortunately, there is no way around the hard-coding of these values
static const char g_szLandBrickImageMapName[32] = "lava_tile_cell_01ImageMap"; // "lava_tile_cell_01ImageMap";		// Image Map for the Land Bricks.
//static const char g_szInvisibleSquareImageMapName[32] = "InvisibleSquareImageMap";	// Image Map that is just a blank box

// Hard-coded values for the grass frames
static const S32 g_iLandBrickBreakFrames[] =	{ 17, 18, 19, 20, 21 };			// ImageMap frames for a breaking brick
static const S32 g_iLandBrickGrowFrames[] =		{ 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28 };	// ImageMap frames for the growing grass
static const S32 g_iLandBrickRespawnFrames[] =	{ 48, 49, 50, 51 };				// ImageMap frames for the respawning of the brick

static S32 g_iNumBrickBreakFrames	= sizeof(g_iLandBrickBreakFrames) / sizeof(S32);
static S32 g_iNumBrickGrowFrames	= sizeof(g_iLandBrickGrowFrames) / sizeof(S32);
static S32 g_iNumBrickRespawnFrames = sizeof(g_iLandBrickRespawnFrames) / sizeof(S32);

// Tile switch speeds
static const F32 g_fBrickBreakTileChangeTimeMS = 100.0f;
static const F32 g_fGrassGrowTileChangeTimeMS = 66.67f;
static const F32 g_fBrickRespawnTileChangeTimeMS = 66.67f;

// Danger Brick values
static const F32 g_fDangerBrickDamageAmount = 2.5; //5.0f;


// Level Win brick frames (when the player stomps the ground)
static const S32 g_iLevelWinGrassFrames[] = { 29, 30, 31 };
static S32 g_iNumWinSequenceGrassFrames = sizeof(g_iLevelWinGrassFrames) / sizeof(S32);
static const F32 g_fWinSequenceGrassTileChangeTimeMS = 100.0f;

// Level Lose brick frames (when the player dies)
static const S32 g_iLevelLoseBrickFrames[] = { 48, 49, 50, 51 };
static S32 g_iNumLoseSequenceBrickFrames = sizeof(g_iLevelLoseBrickFrames) / sizeof(S32);
static const F32 g_fLoseSequenceTileChangeTimeMS = 66.67f;

// Danger Brick collision time
static const F32 g_fDangerBrickCollisionEnableTimeMS = 250.0f; // This is to solve the problem of the player getting damaged too quickly from one danger brick

//Mana drain system
extern F32 g_fBrickDestroyManaDrain;
extern F32 g_fBrickFirstDestroyManaDrain;
extern F32 g_fTotalManaDrained;
extern bool g_bIsFirstBrickDestroyed;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<CComponentLandBrick*> CComponentLandBrick::m_LandBrickList;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLandBrick );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLandBrick::CComponentLandBrick()
	: m_pOwnerObject( NULL )
	, m_eLandBrickState( LAND_BRICK_STATE_NONE )
	, m_iCurrentBrickFrameIndex( -1 )
	, m_iCurrentGrassFrameIndex( -1 )
	, m_iCurrentBrickRespawnFrameIndex( -1 )
	, m_fTileChangeTimer( 0.0f )
	//, m_pszGrassFrameNumber( NULL )
	, m_iGrassFrameNumber( 0 )
	, m_iOriginalBrickFrameNumber( 0 )
	, m_bIsDangerBrick( false )
	, m_bIsUnbreakable( false )
	, m_bIsDoubleBrick( false )
	, m_iCurrentLandBrickHealth( g_iMaxLandBrickHealth )
	, m_bIsBeingFiredUpon( false )
	//, m_bIsInCollisionEnableList( false )
	, m_fDangerBrickCollisionTimer( 0.0f )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLandBrick::~CComponentLandBrick()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::EnableCollision()
{
	if( m_pOwnerObject )
	{
		m_pOwnerObject->setCollisionActive( false, true );
		m_pOwnerObject->setCollisionCallback( true );
		//m_pOwnerObject->setUsesPhysics( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::DisableCollision()
{
	if( m_pOwnerObject )
	{
		m_pOwnerObject->setCollisionActive( false, false );
		m_pOwnerObject->setCollisionCallback( false );
		//m_pOwnerObject->setUsesPhysics( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLandBrick::IsAttackable() const
{
	if( m_bIsUnbreakable || m_bIsDangerBrick )
		return false;
	
	if( m_bIsBeingFiredUpon )
		return false;
	
	if( m_eLandBrickState == LAND_BRICK_STATE_NONE || 
	    m_eLandBrickState == LAND_BRICK_STATE_PURE_STONE ||
	    m_eLandBrickState == LAND_BRICK_STATE_RESPAWNING )
	{
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::DealDamage( const S32& _iDamageAmount )
{
	if( m_bIsUnbreakable )
	{
		printf( "Attempting to deal damage to an unbreakable brick\n" );
		return;
	}
		
	// Do nothing if a Land Brick is being damaged after the level has completed (most likely from flying projectiles)
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;

	if( m_eLandBrickState == LAND_BRICK_STATE_NONE ||
	    m_eLandBrickState == LAND_BRICK_STATE_PURE_STONE || 
	    m_eLandBrickState == LAND_BRICK_STATE_RESPAWNING )
	{
		return;
	}
	
	m_iCurrentLandBrickHealth -= _iDamageAmount;
	if( m_iCurrentLandBrickHealth <= 0 )
	{
		m_iCurrentLandBrickHealth = g_iMaxLandBrickHealth;
		
		if( m_pOwnerObject )
		{
			m_iCurrentBrickRespawnFrameIndex = 0;
			m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLandBrickRespawnFrames[m_iCurrentBrickRespawnFrameIndex] );
			m_iCurrentBrickRespawnFrameIndex++;
			
			//printf( "%s - Respawning\n", m_pOwnerObject ? m_pOwnerObject->getName() : "" );
			if( m_eLandBrickState == LAND_BRICK_STATE_FULLY_GROWN )
				CComponentPathGridHandler::GetInstance().DecrementNumGrassFullyGrown();
			
			m_eLandBrickState = LAND_BRICK_STATE_RESPAWNING;
			
			// Set the image map back into 'Pure Stone'. The grass frame number corresponds directly to the stone frame number, so reuse that. 
			//m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, m_iGrassFrameNumber );
			//m_pOwnerObject->setCollisionActive( false, true );
			//m_eLandBrickState = LAND_BRICK_STATE_PURE_STONE;
			
			//CComponentGlobals::GetInstance().DecrementNumFullyGrownLandBricks();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::GetOwnerPosition( t2dVector& _vOutPosition)
{
	if( m_pOwnerObject == NULL )
	{
		_vOutPosition = t2dVector::getZero();
		return;
	}
	
	_vOutPosition = m_pOwnerObject->getPosition();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLandBrick::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentLandBrick ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	//addField( "GrassFrameNumber",	TypeString,	Offset( m_pszGrassFrameNumber,	CComponentLandBrick ) );
	addField( "DangerBrick",		TypeBool,	Offset( m_bIsDangerBrick,		CComponentLandBrick ) );
	addField( "Unbreakable",		TypeBool,	Offset( m_bIsUnbreakable,		CComponentLandBrick ) );
	
	//addField( "DoubleBrick",		TypeBool,	Offset( m_bIsDoubleBrick,		CComponentLandBrick ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLandBrick::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dStaticSprite* pOwnerObject = dynamic_cast<t2dStaticSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "%s - Must be added to a t2dStaticSprite.\n", __FUNCTION__ );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::onUpdate()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( CComponentPauseScreen::IsQuittingToMapScreen() )
		return;
	
	switch( m_eLandBrickState )
	{
		case LAND_BRICK_STATE_BREAKING:
			UpdateBrickBreak();
			break;
			
		case LAND_BRICK_STATE_GROWING:
			UpdateGrassGrow();
			break;
			
		case LAND_BRICK_STATE_RESPAWNING:
			UpdateBrickRespawning();
			break;
			
		case LAND_BRICK_STATE_WIN_SEQUENCE:
			UpdateBrickWinSequence();
			break;
			
		case LAND_BRICK_STATE_LOSE_SEQUENCE:
			UpdateBrickLoseSequence();
			
		default:
			break;
	}
	
	// Enable collision on a danger brick after a certain amount of time after damaging the player
	if( m_bIsDangerBrick && m_fDangerBrickCollisionTimer > 0.0f )
	{
		m_fDangerBrickCollisionTimer -= (F32)ITickable::smTickMs;
		if( m_fDangerBrickCollisionTimer <= 0.0f )
		{
			m_fDangerBrickCollisionTimer = 0.0f;
			m_pOwnerObject->setCollisionActive( false, true );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( CComponentEndLevelScreen::HasLevelEnded() )
	{
		if( CComponentEndLevelScreen::IsLevelWon() )
		{
			HandleCollisionWinSequence();
			
			if( CComponentEndLevelScreen::GetInstance().HasFinalGrowSoundBeenPlayed() == false )
			{
				CSoundManager::GetInstance().StopCurrentBGM();
				CSoundManager::GetInstance().PlayFinalGrowSound();
				CComponentEndLevelScreen::GetInstance().SetFinalGrowSoundPlayed();
			}
		}
		else
		{
			HandleCollisionLoseSequence();
		}
	}
	else
	{
		HandleCollisionGameplay();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::OnPostInit()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setLayer( LAYER_LAND_BRICKS );
	
	CComponentLandBrick::m_LandBrickList.push_back( this );
	
	m_pOwnerObject->setUseMouseEvents( false );
	m_pOwnerObject->setCollisionActive( false, true );
	m_pOwnerObject->setCollisionCallback( true );
	m_pOwnerObject->setUsesPhysics( true );
	
	if( m_bIsDangerBrick == false )
	{
		if( m_bIsUnbreakable )
		{
			// Disable collision for unbreakable non-danger bricks
			m_pOwnerObject->setCollisionActive( false, false );
			m_pOwnerObject->setCollisionCallback( false );
		}
		else 
		{
			m_eLandBrickState = LAND_BRICK_STATE_PURE_STONE;
			ConvertGrassFrameNumberStringToEnum();
			CComponentGlobals::GetInstance().IncrementTotalNumBreakableLandBricks();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLandBrick::AddLandBrickDataToPathGrid()
{
	S32 iNumLandBricks = m_LandBrickList.size();
	
	CComponentLandBrick* pLandBrick = NULL;
	for( S32 i = 0; i < iNumLandBricks; ++i )
	{
		pLandBrick = m_LandBrickList[i];
		if( pLandBrick )
		{
			CComponentPathGridHandler::GetInstance().AddLandBrickData( pLandBrick );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLandBrick::ClearAllLandBrickData()
{
	m_LandBrickList.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLandBrick::UnbreakAllLandBricks() 
{
    printf( "CComponentLandBrick::UnbreakAllLandBricks\n" );
    S32 iNumLandBricks = m_LandBrickList.size();
	
	CComponentLandBrick* pLandBrick = NULL;
	for( S32 i = 0; i < iNumLandBricks; ++i )
	{
		pLandBrick = m_LandBrickList[i];
		if( pLandBrick &&
            pLandBrick->IsDangerBrick() == false && 
            pLandBrick->IsUnbreakable() == false &&
            pLandBrick->m_pOwnerObject )
		{
            pLandBrick->m_fTileChangeTimer = 0.0f;
            pLandBrick->m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, pLandBrick->m_iOriginalBrickFrameNumber );
            pLandBrick->m_pOwnerObject->setCollisionActive( false, true );
            pLandBrick->m_eLandBrickState = LAND_BRICK_STATE_PURE_STONE;
            pLandBrick->m_iCurrentBrickRespawnFrameIndex = -1;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentLandBrick::ConvertGrassFrameNumberStringToEnum()
{
	// It is easier to hardcode these values, as the brick->grass frames are a one-to-one mapping
	
	if( m_pOwnerObject == NULL )
		return;
	
	m_iOriginalBrickFrameNumber = m_pOwnerObject->getFrame();
	m_iGrassFrameNumber = m_iOriginalBrickFrameNumber + 55;
	if( m_iGrassFrameNumber > 63 )
		m_iGrassFrameNumber = 63;
	
//	if( m_pszGrassFrameNumber == NULL )
//		return;
//	
//	// RKS TODO: Use an 'atoi' function
//	
//	// %grassFrameNumbers = "0" TAB "1" TAB "2" TAB "3" TAB "4" TAB "5" TAB "6" TAB "7" TAB "8";
//	if( strcmp( m_pszGrassFrameNumber, "0" ) == 0 ) 
//		m_iGrassFrameNumber = 0;
//	else if( strcmp( m_pszGrassFrameNumber, "1" ) == 0 ) 
//		m_iGrassFrameNumber = 1;
//	else if( strcmp( m_pszGrassFrameNumber, "2" ) == 0 ) 
//		m_iGrassFrameNumber = 2;
//	else if( strcmp( m_pszGrassFrameNumber, "3" ) == 0 ) 
//		m_iGrassFrameNumber = 3;
//	else if( strcmp( m_pszGrassFrameNumber, "4" ) == 0 ) 
//		m_iGrassFrameNumber = 4;
//	else if( strcmp( m_pszGrassFrameNumber, "5" ) == 0 ) 
//		m_iGrassFrameNumber = 5;
//	else if( strcmp( m_pszGrassFrameNumber, "6" ) == 0 ) 
//		m_iGrassFrameNumber = 6;
//	else if( strcmp( m_pszGrassFrameNumber, "7" ) == 0 ) 
//		m_iGrassFrameNumber = 7;
//	else if( strcmp( m_pszGrassFrameNumber, "8" ) == 0 ) 
//		m_iGrassFrameNumber = 8;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::UpdateBrickBreak()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fTileChangeTimer += ITickable::smTickMs;
	
	if( m_fTileChangeTimer < g_fBrickBreakTileChangeTimeMS )
		return;
	
	m_fTileChangeTimer = 0.0f;

	if( m_iCurrentBrickFrameIndex != -1 )
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLandBrickBreakFrames[m_iCurrentBrickFrameIndex] );
		m_iCurrentBrickFrameIndex++;
	}
	else 
	{
		m_iCurrentGrassFrameIndex = 0;
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLandBrickGrowFrames[m_iCurrentGrassFrameIndex] );
		m_iCurrentBrickFrameIndex++;
		m_eLandBrickState = LAND_BRICK_STATE_GROWING;
		
	}
	
	if( m_iCurrentBrickFrameIndex >= g_iNumBrickBreakFrames )
		m_iCurrentBrickFrameIndex = -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::UpdateGrassGrow()
{
	if( m_pOwnerObject == NULL )
		return;
		
	m_fTileChangeTimer += ITickable::smTickMs;
	
	if( m_fTileChangeTimer < g_fGrassGrowTileChangeTimeMS )
		return;
	
	m_fTileChangeTimer = 0.0f;
	
	if( m_iCurrentGrassFrameIndex != -1 )
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLandBrickGrowFrames[m_iCurrentGrassFrameIndex] );
		m_iCurrentGrassFrameIndex++;
	}
	else 
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, m_iGrassFrameNumber );
		m_eLandBrickState = LAND_BRICK_STATE_FULLY_GROWN;
		OnGrassFullyGrown();
	}
	
	if( m_iCurrentGrassFrameIndex >= g_iNumBrickGrowFrames )
		m_iCurrentGrassFrameIndex = -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::OnGrassFullyGrown()
{
	//printf( "%s - Fully grown\n", m_pOwnerObject ? m_pOwnerObject->getName() : "" );
	CComponentPathGridHandler::GetInstance().IncrementNumGrassFullyGrown();
	
	t2dVector vPosition = (m_pOwnerObject ? m_pOwnerObject->getPosition() : t2dVector::getZero());
	CComponentGlobals::GetInstance().OnGrassBrickFullyGrown( vPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::UpdateBrickRespawning()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fTileChangeTimer += ITickable::smTickMs;
	
	if( m_fTileChangeTimer < g_fBrickRespawnTileChangeTimeMS )
		return;
	
	m_fTileChangeTimer = 0.0f;
	
	if( m_iCurrentBrickRespawnFrameIndex != -1 )
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLandBrickRespawnFrames[m_iCurrentBrickRespawnFrameIndex] );
		m_iCurrentBrickRespawnFrameIndex++;
	}
	else 
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, m_iOriginalBrickFrameNumber );
		m_pOwnerObject->setCollisionActive( false, true );
		m_eLandBrickState = LAND_BRICK_STATE_PURE_STONE;
	}

	if( m_iCurrentBrickRespawnFrameIndex >= g_iNumBrickRespawnFrames )
		m_iCurrentBrickRespawnFrameIndex = -1; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::UpdateBrickWinSequence()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fTileChangeTimer += ITickable::smTickMs;
	
	if( m_fTileChangeTimer < g_fWinSequenceGrassTileChangeTimeMS )
		return;
	
	m_fTileChangeTimer = 0.0f;
	
	if( m_iCurrentBrickFrameIndex >= g_iNumWinSequenceGrassFrames )
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, m_iGrassFrameNumber );
		m_eLandBrickState = LAND_BRICK_STATE_FULLY_GROWN;
		return;
	}
	
	m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLevelWinGrassFrames[ m_iCurrentBrickFrameIndex++ ] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::UpdateBrickLoseSequence()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fTileChangeTimer += ITickable::smTickMs;
	
	if( m_fTileChangeTimer < g_fLoseSequenceTileChangeTimeMS )
		return;
	
	m_fTileChangeTimer = 0.0f;
	
	if( m_iCurrentBrickFrameIndex >= g_iNumLoseSequenceBrickFrames )
	{
		m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, m_iOriginalBrickFrameNumber );
		m_eLandBrickState = LAND_BRICK_STATE_PURE_STONE;
		return;
	}
	
	m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLevelLoseBrickFrames[ m_iCurrentBrickFrameIndex++ ] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::HandleCollisionGameplay()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setCollisionActive( false, false );
	
	if( m_bIsDangerBrick )
	{
		// This is a danger block. Deal damage to the home base.
		t2dVector vEffectPosition = m_pOwnerObject->getPosition();
		vEffectPosition.mY -= ( m_pOwnerObject->getSize().mY * 0.5f);// Play the effect at the top of the brick, so it actually appears in the center
		CComponentGlobals::GetInstance().PlayDangerBrickHitEffectAtPosition( vEffectPosition );
		CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_fDangerBrickDamageAmount );
		m_fDangerBrickCollisionTimer = g_fDangerBrickCollisionEnableTimeMS;
		return;
	}
	
	// Play the brick break sound
	CSoundManager::GetInstance().PlayBrickBreakSound();
	
	m_eLandBrickState = LAND_BRICK_STATE_BREAKING;
	m_fTileChangeTimer = 0.0f; //g_fBrickBreakTileChangeTimeMS;
	
	m_iCurrentBrickFrameIndex = 0;
	m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLandBrickBreakFrames[m_iCurrentBrickFrameIndex++] );
	
	// If this is a Boss Level, then fire a Brick Attack projectile at this brick
	if( CComponentGlobals::GetInstance().IsBossLevel() )
	{
		t2dVector vBrickPosition = m_pOwnerObject->getPosition();
		CComponentBossMountain::FireBrickProjectileAtPosition( vBrickPosition );
	}
    
    if( CComponentTutorialLevel::IsTutorialLevel() == true )
    {
        CComponentTutorialLevel::OnBrickBroken();
    }
	
	//DM This is the NOGRASSMANADRAIN code
	//DM To make the "bushido" technique less effective (i.e. less of a complete game changer), the first brick that gets destroyed in a line draw sequence costs more than the subsequent ones
	if(g_bIsFirstBrickDestroyed == false)
	{
		CComponentManaBar::SubtractMana( g_fBrickFirstDestroyManaDrain );
		g_fTotalManaDrained += g_fBrickFirstDestroyManaDrain;
		g_bIsFirstBrickDestroyed = true;
	}
	else
	{
		CComponentManaBar::SubtractMana( g_fBrickDestroyManaDrain );
		g_fTotalManaDrained += g_fBrickDestroyManaDrain;
	}



	m_bIsBeingFiredUpon = false;
	
	//if( m_bIsInCollisionEnableList )
	//{
	//	CComponentGlobals::GetInstance().RemoveLandBrickFromCollisionEnableList( this );
	//	m_bIsInCollisionEnableList = false;
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::HandleCollisionWinSequence()
{
	if( m_pOwnerObject == NULL )
		return;
	
	// Danger Bricks need to have their "grass frame number" forced
	if( m_bIsDangerBrick )
		m_iGrassFrameNumber = 55;
	
	m_pOwnerObject->setCollisionActive( false, false );
	
	m_eLandBrickState = LAND_BRICK_STATE_WIN_SEQUENCE;
	m_fTileChangeTimer = 0.0f;
	
	m_iCurrentBrickFrameIndex = 0;
	
	m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLevelWinGrassFrames[ m_iCurrentBrickFrameIndex++ ] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLandBrick::HandleCollisionLoseSequence()
{
	if( m_pOwnerObject == NULL )
		return;
	
	// Danger Bricks should not be affected
	if( m_bIsDangerBrick )
		return;
	
	m_pOwnerObject->setCollisionActive( false, false );
	
	m_eLandBrickState = LAND_BRICK_STATE_LOSE_SEQUENCE;
	m_fTileChangeTimer = 0.0f;
	
	m_iCurrentBrickFrameIndex = 0;
	
	m_pOwnerObject->setImageMap( g_szLandBrickImageMapName, g_iLevelLoseBrickFrames[ m_iCurrentBrickFrameIndex++ ] );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
