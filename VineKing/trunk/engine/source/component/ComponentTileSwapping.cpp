//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentTileSwapping.h"

#include "ComponentGlobals.h"
#include "ComponentManaSeedItemDrop.h"

#include "math/mRandom.h"
#include "T2D/t2dTileMap.h"

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const F32 g_fTileChangeTime = 2000.0; //4000.0f; // in milliseconds

static MRandomLCG g_TileRandom;
static const U32 g_uTileRandomSeed = 1376312589;

static MRandomLCG g_SpawnRandom;

static const S32 g_iMaxTileHealth = 100;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentTileSwapping );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PUBLIC MEMBER FUNCTIONS

CComponentTileSwapping::CComponentTileSwapping()
	: m_pTileLayerOwner( NULL )
	, m_uTotalNumGrowableSquares( 0 )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTileSwapping::~CComponentTileSwapping()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------	

void CComponentTileSwapping::InitializeTileInfo()
{
	t2dTileLayer::tTileObject* pTileObject = NULL;
	
	for( S32 i = 0; i < g_iMaxArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxArrayY; ++j )
		{
			m_aTileInfoList[i][j].fTileTimer = 0.0f;
			m_aTileInfoList[i][j].bIsGrowing = false;
			m_aTileInfoList[i][j].bIsFullyGrown = false;
			m_aTileInfoList[i][j].iCurrentGrowLevel = 0;
			m_aTileInfoList[i][j].iCurrentHealth = g_iMaxTileHealth;
			
			// Set the tile custom data to "0". This means that the tile is completely passable.
			if( m_pTileLayerOwner )
			{
				m_pTileLayerOwner->setTileCustomData( i, j, "0" );
				
				// Check the tile collision on each tile. If collision is on, then
				if( m_pTileLayerOwner->getTileObject( i, j, &pTileObject ) && pTileObject )
				{
					if( pTileObject->mReceiveCollisions == true ) // This is probably unnecessary, as only valid tiles will have collision
						m_uTotalNumGrowableSquares++;
					
					pTileObject = NULL;
				}
			}
		}
	}
	
	AssertFatal( m_uTotalNumGrowableSquares > 0, "CComponentTileSwapping::InitializeTileInfo - No valid tile squares." );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::UpdateTileInfo()
{
	for( S32 i = 0; i < g_iMaxArrayX; ++i )
	{
		for( S32 j = 0; j < g_iMaxArrayY; ++j )
		{
			if( m_aTileInfoList[i][j].bIsGrowing )
			{
				IncrementTileTimer( i, j );
				CheckTileSwap( i, j );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::IncrementTileTimer( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::IncrementTileTimer - Array out of bounds" );
	
	m_aTileInfoList[_iTileX][_iTileY].fTileTimer += ITickable::smTickMs;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::CheckTileSwap( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::CheckTileSwap - Array out of bounds" );
	
	if( m_pTileLayerOwner == NULL )
		return;
	
	if( m_aTileInfoList[ _iTileX ][ _iTileY ].fTileTimer < g_fTileChangeTime )
		return;
	
	switch( m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentGrowLevel )
	{
		case 0:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "grass01ImageMap", 0 );
			break;
			
		case 1:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "grass02ImageMap", 0 );
			break;
			
		case 2:
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "grass03ImageMap", 0 );
			break;
			
		case 3:
			//m_aTileInfoList[ _iTileX ][ _iTileY ].bIsFullyGrown = true;
			SetTileFullyGrown( _iTileX, _iTileY );
			m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "grass_4bitImageMap", 0 );
			OnTileFullyGrown( _iTileX, _iTileY );
			break;
			
		default:
			break;
	}
	
	m_aTileInfoList[ _iTileX ][ _iTileY ].fTileTimer = 0.0f;
	m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentGrowLevel++;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::SetTileFullyGrown( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::SetTileFullyGrown - Array out of bounds" );
	
	m_aTileInfoList[ _iTileX ][ _iTileY ].bIsFullyGrown = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::SetTileNotFullyGrown( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::SetTileNotFullyGrown - Array out of bounds" );
	
	m_aTileInfoList[ _iTileX ][ _iTileY ].bIsFullyGrown = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTileSwapping::IsTileFullyGrown( const S32& _iTileX, const S32& _iTileY ) const
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::IsTileFullyGrown - Array out of bounds" );
	
	return m_aTileInfoList[ _iTileX ][ _iTileY ].bIsFullyGrown;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

U32 CComponentTileSwapping::GetNumFullyGrownTiles() const
{
	U32 uNumFullyGrownTiles = 0;
	
	for( U32 i = 0; i < g_iMaxArrayX; ++i )
	{
		for( U32 j = 0; j < g_iMaxArrayY; ++j )
		{
			if( IsTileFullyGrown( i, j ) )
			   uNumFullyGrownTiles++;
		}
	}
	
	return uNumFullyGrownTiles;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::GetRandomSpawnPosition( t2dVector& _vOutSpawnPosition )
{
	_vOutSpawnPosition.mX = 0.0f;
	_vOutSpawnPosition.mY = 0.0f;
	
	U32 uNumFullyGrownTiles = GetNumFullyGrownTiles();
	if( uNumFullyGrownTiles == 0 )
	{
		//printf( "Error: CComponentTileSwapping::GetRandomSpawnPosition called without any fully grown tiles.\n" );
		
		return;
	}
	
	U32 uSeed = Platform::getRealMilliseconds();
	MRandomLCG::setGlobalRandSeed( uSeed );
	
	// Get a random number between 0 and the number of fully grown tiles.  Use this value to get a random index into the list of tiles to pick a random position
	//MRandomGenerator::randI(i,n)
	//return F32(gRandGen.randI(0,getMax( dAtoi(argv[1]), 0 )));
	U32 uRandom = g_TileRandom.randI( 0, uNumFullyGrownTiles - 1 );

	U32 uIndexCounter = 0; // This counts the indices of fully grown tiles. When this number is equal to the random number, then use that tile's position.
	for( U32 i = 0; i < g_iMaxArrayX; ++i )
	{
		for( U32 j = 0; j < g_iMaxArrayY; ++j )
		{
			if( IsTileFullyGrown( i, j ) )
			{
				if( uIndexCounter == uRandom )
				{
					t2dVector vTileWorldPosition;
					GetTileWorldPosition(i, j, vTileWorldPosition );
					_vOutSpawnPosition = vTileWorldPosition;
					return;
				}
				
				uIndexCounter++;
			}
		}
	}
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTileSwapping::IsTileGrowing( const S32& _iTileX, const S32& _iTileY ) const
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::IsTileGrowing - Array out of bounds" );
	
	return m_aTileInfoList[ _iTileX ][ _iTileY ].bIsGrowing;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::SetTileIsGrowing( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::SetTileIsGrowing - Array out of bounds" );
	
	m_aTileInfoList[ _iTileX ][ _iTileY ].bIsGrowing = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::SetTileIsNotGrowing( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::SetTileIsNotGrowing - Array out of bounds" );
	
	m_aTileInfoList[ _iTileX ][ _iTileY ].bIsGrowing = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::StartTileGrowing( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::StartTileGrowing - Array out of bounds" );
	
	if( m_pTileLayerOwner == NULL )
		return;
	
	if( m_aTileInfoList[ _iTileX ][ _iTileY ].bIsGrowing )
		return;
	
	// Set the tile to the 'Growing' state
	SetTileIsGrowing( _iTileX, _iTileY );
	
	// For the first tile swap, force the tile change time to be completed
	m_aTileInfoList[ _iTileX ][ _iTileY ].fTileTimer = g_fTileChangeTime;
	
	// Turn collision off the tile
	m_pTileLayerOwner->setTileCollisionActive( _iTileX, _iTileY, false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::DestroyTile( const S32& _iTileX, const S32& _iTileY )
{
	ResetTile( _iTileX, _iTileY );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::DestroyTileAtPosition( const t2dVector& _vDestroyPosition )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	Point2I tilePoint;
	
	if( m_pTileLayerOwner->pickTile( _vDestroyPosition, tilePoint ) )
		DestroyTile( tilePoint.x, tilePoint.y );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::GetTileWorldPosition( const U32 _uLogicalPositionX, const U32 _uLogicalPositionY, t2dVector& _vOutWorldPosition )
{
	AssertFatal( _uLogicalPositionX < g_iMaxArrayX && _uLogicalPositionY < g_iMaxArrayY, "CComponentTileSwapping::GetTileWorldPosition - Array out of bounds" );
	
	if( m_pTileLayerOwner == NULL )
	{
		_vOutWorldPosition.mX = 0.0f;
		_vOutWorldPosition.mY = 0.0f;
		return;
	}
	
	U32 uTileSizeX = static_cast<U32>( m_pTileLayerOwner->getTileSizeX() );
	U32 uTileSizeY = static_cast<U32>( m_pTileLayerOwner->getTileSizeY() );
	
	U32 uTileCountX = static_cast<U32>( m_pTileLayerOwner->getTileCountX() );
	U32 uTileCountY = static_cast<U32>( m_pTileLayerOwner->getTileCountY() );
	
	t2dVector vTileLayerPosition = m_pTileLayerOwner->getPosition();
	
	// Get the Top Left position
	t2dVector vTileLayerTopLeftPosition;
	vTileLayerTopLeftPosition.mX = -1.0f * ( ( (uTileCountX * uTileSizeX) / 2.0f ) + (uTileSizeX / 2.0f) );
	vTileLayerTopLeftPosition.mY = -1.0f * ( ( (uTileCountY * uTileSizeY) / 2.0f ) + (uTileSizeY / 2.0f) );
	
	// Determine the tile's world position
	t2dVector vTileWorldPosition;
	vTileWorldPosition.mX = vTileLayerPosition.mX + (vTileLayerTopLeftPosition.mX + (uTileSizeX * (_uLogicalPositionX + 1) ) );
	vTileWorldPosition.mY = vTileLayerPosition.mY + (vTileLayerTopLeftPosition.mY + (uTileSizeY * (_uLogicalPositionY + 1) ) );
	
	// Finally, set the tile world position
	_vOutWorldPosition = vTileWorldPosition;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentTileSwapping::GetTileHealth( const U32& _iTileX, const U32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::GetTileHealth - Array out of bounds" );
	
	return m_aTileInfoList[ _iTileX ] [_iTileY ].iCurrentHealth;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::DealDamageToTile( const U32& _iTileX, const U32& _iTileY, const U32& _iDamageAmount )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::DealDamageToTile - Array out of bounds" );
	
	m_aTileInfoList[ _iTileX ] [_iTileY ].iCurrentHealth -= _iDamageAmount;
	
	if( m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentHealth < 0 )
		m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentHealth = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentTileSwapping::GetTileGrowLevel( const U32& _iTileX, const U32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::GetTileGrowLevel - Array out of bounds" );
	
	return m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentGrowLevel;
}
	
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTileSwapping::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// Add the fields that will be accessed from script
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pTileLayerOwner, CComponentTileSwapping ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTileSwapping::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	t2dTileLayer* pOwnerTileLayer = dynamic_cast<t2dTileLayer*>( _pTarget );
	if( pOwnerTileLayer == NULL )
	{
		//printf( "%s - Must be added to a t2dTileLayer\n", __FUNCTION__ );
		return false;
	}
	
	//CComponentGlobals::GetInstance().SetTileSwappingComponent( this );
	
	m_pTileLayerOwner = pOwnerTileLayer;
	
	m_pTileLayerOwner->enableUpdateCallback();
	
	InitializeTileInfo();	
	
	
	
	// Set the random seed
	
	//g_TileRandom.setSeed( g_uTileRandomSeed );
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::onUpdate()
{
	UpdateTileInfo();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS
	
void CComponentTileSwapping::ResetTile( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::ResetTile - Array out of bounds" );
	
	if( m_pTileLayerOwner == NULL )
		return;
	
	m_aTileInfoList[ _iTileX ][ _iTileY ].fTileTimer = 0.0f;
	m_aTileInfoList[ _iTileX ][ _iTileY ].bIsGrowing = false;
	m_aTileInfoList[ _iTileX ][ _iTileY ].bIsFullyGrown = false;
	m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentGrowLevel = 0;
	m_aTileInfoList[ _iTileX ][ _iTileY ].iCurrentHealth = g_iMaxTileHealth;
	
	// Change the tile to the invisible image map
	m_pTileLayerOwner->setStaticTile( _iTileX, _iTileY, "InvisibleSquareImageMap", 0 );
	
	// Turn collision back on the tile so it can be grown again
	m_pTileLayerOwner->setTileCollisionActive( _iTileX, _iTileY, true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTileSwapping::OnTileFullyGrown( const S32& _iTileX, const S32& _iTileY )
{
	AssertFatal( _iTileX < g_iMaxArrayX && _iTileY < g_iMaxArrayY, "CComponentTileSwapping::OnTileFullyGrown - Array out of bounds" );
	
	// When a tile is fully grown, there is a 50% chance that a seed will grow on that spot.
	
	U32 uRandom = g_TileRandom.randI( 0, 1 );
	if( uRandom == 1 )
	{
		// Spawn a seed at the location of this tile
		t2dVector vTilePosition;
		GetTileWorldPosition( _iTileX, _iTileY, vTilePosition );
		CComponentManaSeedItemDrop::SpawnSeedFromGrassAtPosition( vTilePosition );
	}
	
	// Check if all the tiles have been grown. If so, then start the level complete / win condition screen.
	if( GetNumFullyGrownTiles() >= m_uTotalNumGrowableSquares )
	{
		CComponentGlobals::GetInstance().OnAllTilesFullyGrown();
	}
}



