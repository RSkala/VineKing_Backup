//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentBreakableTileLayer.h"

#include "ComponentGlobals.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"

#include "core/iTickable.h"
#include "T2D/t2dTileMap.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Tile Break Sound data
const char g_szTileBreakSound[16] = "TileBreakSound";
AudioProfile* g_pTileBreakSound = NULL;
AUDIOHANDLE g_hTileBreakSound = NULL_AUDIOHANDLE;

// Tile texture data
const char g_szTileImageMap[32] = "Lava_brickImageMap";

static const F32 g_fTileBreakSwitchTimeMS = 50.0f; // Amount of time before switching a tile (break animation)
static const F32 g_fTileGrowSwitchTimeMS = 750.0f; // Amount of time before switching a tile (grow animation)

static const S32 g_iMaxTileHealth = 100;

static const F32 g_fDangerBlockDamageAmount = 20.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentBreakableTileLayer );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentBreakableTileLayer::CComponentBreakableTileLayer()
	: m_pTileLayerOwner( NULL )
	, m_uCurrentTileIndex( 0 )
	, m_uTotalNumBreakableTiles( 0 )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentBreakableTileLayer::~CComponentBreakableTileLayer()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::BreakTile( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxTileBreakArrayX && _iTileY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::BreakTile - Array out of bounds" );
	
	if( m_pTileLayerOwner == NULL )
		return;
	
	m_pTileLayerOwner->setTileCollisionActive( _iTileX, _iTileY, false );
	
	if( m_aTileInfoList[_iTileX][_iTileY].bIsDangerBlock )
	{
		// This is a danger block. Deal damage to the home base.
		CComponentPlayerHomeBase::GetInstance().DamageHomeBase( g_fDangerBlockDamageAmount );
		return;
	}

	// Play the tile break sound
	if( alxIsPlaying( g_hTileBreakSound ) )
		alxStop( g_hTileBreakSound );
	
	g_hTileBreakSound = alxPlay( g_pTileBreakSound );
	
	m_aTileInfoList[_iTileX][_iTileY].bIsBreaking = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

U32 CComponentBreakableTileLayer::GetNumFullyGrownTiles() const
{
	U32 uNumFullyGrownTiles = 0;
	
	for( S32 i = 0; i < g_iMaxTileBreakArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileBreakArrayY; ++j )
		{
			if( m_aTileInfoList[i][j].bIsFullyGrown )
				uNumFullyGrownTiles++;
		}
	}
	
	return uNumFullyGrownTiles;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentBreakableTileLayer::IsTileAttackable( const S32& _iTileX, const S32& _iTileY ) const
{
	AssertFatal( _iTileX < g_iMaxTileBreakArrayX && _iTileY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::IsTileAttackable - Array out of bounds" );
	
	if( m_aTileInfoList[_iTileX][_iTileY].bIsDangerBlock )
		return false;
	
	return	m_aTileInfoList[_iTileX][_iTileY].bIsBreaking	||
			m_aTileInfoList[_iTileX][_iTileY].bIsGrowing	||
			m_aTileInfoList[_iTileX][_iTileY].bIsFullyGrown;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentBreakableTileLayer::GetTileHealth( const U32& _iTileX, const U32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxTileBreakArrayX && _iTileY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::GetTileHealth - Array out of bounds" );
	
	return m_aTileInfoList[_iTileX][_iTileY].iCurrentHealth;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::DealDamageToTile( const U32& _iTileX, const U32& _iTileY, const S32& _iDamageAmount )
{
	AssertFatal( _iTileX < g_iMaxTileBreakArrayX && _iTileY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::DealDamageToTile - Array out of bounds" );
	
	m_aTileInfoList[_iTileX][_iTileY].iCurrentHealth -= _iDamageAmount;
	
	if( m_aTileInfoList[_iTileX][_iTileY].iCurrentHealth <= 0 )
	{
		m_aTileInfoList[_iTileX][_iTileY].iCurrentHealth = 0;
		
		// Destroy this tile
		DestroyTile( _iTileX, _iTileY );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::DealDamageToTileAtPosition( const S32& _iDamageAmount, const t2dVector& _vPosition )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	Point2I tilePoint;
	if( m_pTileLayerOwner->pickTile( _vPosition, tilePoint ) )
	{
		m_aTileInfoList[tilePoint.x][tilePoint.y].iCurrentHealth -= _iDamageAmount;
		
		if( m_aTileInfoList[tilePoint.x][tilePoint.y].iCurrentHealth <= 0 )
		{
			m_aTileInfoList[tilePoint.x][tilePoint.y].iCurrentHealth = 0;
			
			// Destroy this tile
			DestroyTile( tilePoint.x, tilePoint.y );
		}
	}
	else 
	{
		printf( "%s - Tile not found at world position (%f, %f)\n", __FUNCTION__, _vPosition.mX, _vPosition.mY );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// This function is used to fix the problem where the Line Attack Object would collide against a danger block multiple times,
// causing the home base to become damaged excessively for each line draw attack, sometimes killing the home base prematurely.

void CComponentBreakableTileLayer::EnableDangerBlockCollisions()
{
	for( S32 i = 0; i < g_iMaxTileBreakArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileBreakArrayY; ++j )
		{
			if( m_aTileInfoList[i][j].bIsDangerBlock )
			{
				if( m_pTileLayerOwner )
					m_pTileLayerOwner->setTileCollisionActive( i, j, true );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::GetNextAttackableTilePosition( t2dVector& _vOutTileWorldPosition )
{
	for( S32 i = 0; i < g_iMaxTileBreakArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileBreakArrayY; ++j )
		{
			if( IsTileAttackable( i, j ) )
			{
				GetTileWorldPosition( i, j, _vOutTileWorldPosition );
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from ConsoleObject
/*static*/ void CComponentBreakableTileLayer::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pTileLayerOwner, CComponentBreakableTileLayer ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentBreakableTileLayer::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dTileLayer* pOwnerObject = dynamic_cast<t2dTileLayer*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentBreakableTileLayer::onComponentAdd - Must be added to a t2dTileLayer.\n" );
		return false;
	}
	
	m_pTileLayerOwner = pOwnerObject;
	
	//CComponentGlobals::GetInstance().SetBreakableTileLayerComponent( this );
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::onUpdate()
{
	for( S32 i = 0; i < g_iMaxTileBreakArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileBreakArrayY; ++j )
		{
			if( m_aTileInfoList[i][j].bIsBreaking )
			{
				UpdateBreak( i, j );
			}
			else if( m_aTileInfoList[i][j].bIsGrowing )
			{
				UpdateGrowing( i, j );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
static t2dVector vLastPosition( 0.0f, 0.0f );

void CComponentBreakableTileLayer::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	vLastPosition = _vWorldMousePoint;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	Point2I tilePoint;
	t2dVector vPrevToCurrPoint = _vWorldMousePoint - vLastPosition;
	F32 fDistance = vPrevToCurrPoint.len();
	printf( "Distance: %f\n", fDistance );	
	
	if( m_pTileLayerOwner->pickTile( _vWorldMousePoint, tilePoint ) )
	{
		printf( "Tile Picked: (%d, %d)\n", tilePoint.x, tilePoint.y );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( m_pTileLayerOwner == NULL || _pCollisionStatus == NULL )
		return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::OnPostInit()
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	m_pTileLayerOwner->disableUpdateCallback(); // Note: This only works for the script 'onUpdate' call, so this is pointless
	
	g_pTileBreakSound = static_cast<AudioProfile*>( Sim::findObject( g_szTileBreakSound ) );
	
	InitializeTileInfo();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::InitializeTileInfo()
{
	t2dTileLayer::tTileObject* pTileObject = NULL;
	const char* pszTileCustomData = NULL;
	
	for( S32 i = 0; i < g_iMaxTileBreakArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileBreakArrayY; ++j )
		{
			m_aTileInfoList[i][j].fTileTimer = 0.0f;
			m_aTileInfoList[i][j].bIsBreaking = false;
			m_aTileInfoList[i][j].bIsGrowing = false;
			m_aTileInfoList[i][j].bIsFullyGrown = false;
			m_aTileInfoList[i][j].uCurrentTileIndex = 0;
			m_aTileInfoList[i][j].iCurrentHealth = g_iMaxTileHealth;
			m_aTileInfoList[i][j].bIsDangerBlock = false;
			
			// Get the total number of growable / breakable tiles
			if( m_pTileLayerOwner )
			{
				if( m_pTileLayerOwner->getTileObject( i, j, &pTileObject ) && pTileObject )
				{
					if( pTileObject->mReceiveCollisions == true )
						m_uTotalNumBreakableTiles++;
					
					pTileObject = NULL;
				}
				
				pszTileCustomData = m_pTileLayerOwner->getTileCustomData( i, j );
				if( pszTileCustomData )
				{
					if( strcmp( pszTileCustomData, "danger" ) == 0 )
					{
						// This tile is a Danger block
						m_aTileInfoList[i][j].bIsDangerBlock = true;
						
						m_uTotalNumBreakableTiles--;
					}
					pszTileCustomData = NULL;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::UpdateBreak( const S32& _iTileX, const S32& _iTileY )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	// Update the tile break timer
	m_aTileInfoList[_iTileX][_iTileY].fTileTimer += ITickable::smTickMs;
	if( m_aTileInfoList[_iTileX][_iTileY].fTileTimer < g_fTileBreakSwitchTimeMS )
		return;	
	   
   m_aTileInfoList[_iTileX][_iTileY].fTileTimer = 0.0f;
	
	switch( m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex )
	{
		case 0:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, g_szTileImageMap, 1 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 1;
			break;
			
		case 1:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, g_szTileImageMap, 2 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 2;
			break;
			
		case 2:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, g_szTileImageMap, 3 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 3;
			break;
			
		case 3:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "InvisibleSquareImageMap", 0 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 0;
			m_aTileInfoList[_iTileX][_iTileY].bIsBreaking = false;
			m_aTileInfoList[_iTileX][_iTileY].bIsGrowing = true;
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::UpdateGrowing( const S32& _iTileX, const S32& _iTileY )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	// Update the tile break timer
	m_aTileInfoList[_iTileX][_iTileY].fTileTimer += ITickable::smTickMs;
	if( m_aTileInfoList[_iTileX][_iTileY].fTileTimer < g_fTileGrowSwitchTimeMS )
		return;	
	
	m_aTileInfoList[_iTileX][_iTileY].fTileTimer = 0.0f;
	
	switch( m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex )
	{
		case 0:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "P02_Grass_Texture_pageImageMap", 0 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 1;
			break;
			
		case 1:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "P02_Grass_Texture_pageImageMap", 1 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 2;
			break;
			
		case 2:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "P02_Grass_Texture_pageImageMap", 2 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 3;
			break;
			
		case 3:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "P02_Grass_Texture_pageImageMap", 3 );
			m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 0;
			m_aTileInfoList[_iTileX][_iTileY].bIsBreaking = false;
			m_aTileInfoList[_iTileX][_iTileY].bIsGrowing = false;
			m_aTileInfoList[_iTileX][_iTileY].bIsFullyGrown = true;
			
			OnTileFullyGrown( _iTileX, _iTileY );
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::GetTileWorldPosition( const U32& _uLogicalPositionX, const U32& _uLogicalPositionY, t2dVector& _vOutWorldPosition )
{
	AssertFatal( _uLogicalPositionX < g_iMaxTileBreakArrayX && _uLogicalPositionY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::GetTileWorldPosition - Array out of bounds" );
	
	if( m_pTileLayerOwner == NULL )
	{
		_vOutWorldPosition.mX = 0.0f;
		_vOutWorldPosition.mY = 0.0f;
		return;
	}
	
	//U32 uTileSizeX = static_cast<U32>( m_pTileLayerOwner->getTileSizeX() );
	//U32 uTileSizeY = static_cast<U32>( m_pTileLayerOwner->getTileSizeY() );
	//U32 uTileCountX = static_cast<U32>( m_pTileLayerOwner->getTileCountX() );
	//U32 uTileCountY = static_cast<U32>( m_pTileLayerOwner->getTileCountY() );
	
	F32 fTileSizeX = m_pTileLayerOwner->getTileSizeX();
	F32 fTileSizeY = m_pTileLayerOwner->getTileSizeY();
	F32 fTileCountX = static_cast<F32>( m_pTileLayerOwner->getTileCountX() );
	F32 fTileCountY = static_cast<F32>( m_pTileLayerOwner->getTileCountY() );
	
	t2dVector vTileLayerPosition = m_pTileLayerOwner->getPosition();
	
	// Get the Top Left position
	t2dVector vTileLayerTopLeftPosition;
	vTileLayerTopLeftPosition.mX = -1.0f * ( ( (fTileCountX * fTileSizeX) / 2.0f ) + (fTileSizeX / 2.0f) );
	vTileLayerTopLeftPosition.mY = -1.0f * ( ( (fTileCountY * fTileSizeY) / 2.0f ) + (fTileSizeY / 2.0f) );
	
	// Determine the tile's world position
	t2dVector vTileWorldPosition;
	vTileWorldPosition.mX = vTileLayerPosition.mX + (vTileLayerTopLeftPosition.mX + (fTileSizeX * ((F32)_uLogicalPositionX + 1.0f) ) );
	vTileWorldPosition.mY = vTileLayerPosition.mY + (vTileLayerTopLeftPosition.mY + (fTileSizeY * ((F32)_uLogicalPositionY + 1.0f) ) );
	
	// Finally, set the tile world position
	_vOutWorldPosition = vTileWorldPosition;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::OnTileFullyGrown( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxTileBreakArrayX && _iTileY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::OnTileFullyGrown - Array out of bounds" );
	
	
	// When a tile is fully grown, there is some % chance that a seed will grow on that spot.
	U32 uRandom = CComponentGlobals::GetInstance().GetRandomRange( 0, 3 );
	if( uRandom == 0 )
	{
		// Spawn a seed at the location of this tile
		t2dVector vTilePosition;
		GetTileWorldPosition( _iTileX, _iTileY, vTilePosition );
		CComponentManaSeedItemDrop::SpawnSeedFromGrassAtPosition( vTilePosition );
	}
	
	// Check if all the tiles have been grown. If so, then start the level complete / win condition screen.
	if( GetNumFullyGrownTiles() >= m_uTotalNumBreakableTiles )
	{
		CComponentGlobals::GetInstance().OnAllTilesFullyGrown();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentBreakableTileLayer::DestroyTile( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxTileBreakArrayX && _iTileY < g_iMaxTileBreakArrayY, "CComponentBreakableTileLayer::DestroyTile - Array out of bounds" );
	
	m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, g_szTileImageMap, 0 );
	m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 0;
	
	m_aTileInfoList[_iTileX][_iTileY].fTileTimer = 0.0f;
	m_aTileInfoList[_iTileX][_iTileY].bIsBreaking = false;
	m_aTileInfoList[_iTileX][_iTileY].bIsGrowing = false;
	m_aTileInfoList[_iTileX][_iTileY].bIsFullyGrown = false;
	m_aTileInfoList[_iTileX][_iTileY].uCurrentTileIndex = 0;
	m_aTileInfoList[_iTileX][_iTileY].iCurrentHealth = g_iMaxTileHealth;
	
	m_pTileLayerOwner->setTileCollisionActive( _iTileX, _iTileY, true );
}


