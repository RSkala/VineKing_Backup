//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentPathGridHandler.h"

#include "ComponentGlobals.h"
#include "ComponentLandBrick.h"

//#include "T2D/aStar/pathGrid2d.h" // For the AStar path
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTileMap.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<S32> g_IndexListX;
Vector<S32> g_IndexListY;

Vector<t2dVector> g_RandomPositionList;
static t2dVector g_vTempPosition;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPathGridHandler* CComponentPathGridHandler::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentPathGridHandler );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPathGridHandler::CComponentPathGridHandler()
	: m_pTileLayerOwner( NULL )
	, m_iNumFullyGrownLandBricks( 0 )
{
	InitializePathInfo();
	CComponentPathGridHandler::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPathGridHandler::~CComponentPathGridHandler()
{
	CComponentPathGridHandler::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::AddLandBrickData( t2dStaticSprite* const _pBrickObject, CComponentLandBrick* const _pLandBrickComponent )
{
	if( m_pTileLayerOwner == NULL || _pBrickObject == NULL || _pLandBrickComponent == NULL )
		return;
	
	Point2I tilePoint;
	t2dVector vBrickObjectPosition = _pBrickObject->getPosition();
	
	if( m_pTileLayerOwner->pickTile( vBrickObjectPosition, tilePoint ) )
	{
		AssertFatal( tilePoint.x < g_iMaxTileX && tilePoint.y < g_iMaxTileY, "CComponentPathGridHandler::AddBrickObject - Array out of bounds." );
		m_aTileInfo[tilePoint.x][tilePoint.y].pBrickObject = _pBrickObject;
		m_aTileInfo[tilePoint.x][tilePoint.y].pLandBrickComponent = _pLandBrickComponent;
	}
	else 
	{
		printf( "Tile point not found at (%f, %f)\n", vBrickObjectPosition.mX, vBrickObjectPosition.mY );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::AddLandBrickData( CComponentLandBrick* const _pLandBrickComponent )
{
	if( m_pTileLayerOwner == NULL || _pLandBrickComponent == NULL )
		return;
	
	Point2I tilePoint;
	t2dVector vBrickObjectPosition;
	_pLandBrickComponent->GetOwnerPosition( vBrickObjectPosition );
	
	if( m_pTileLayerOwner->pickTile( vBrickObjectPosition, tilePoint ) )
	{
		AssertFatal( tilePoint.x < g_iMaxTileX && tilePoint.y < g_iMaxTileY, "CComponentPathGridHandler::AddBrickObject - Array out of bounds." );
		m_aTileInfo[tilePoint.x][tilePoint.y].pLandBrickComponent = _pLandBrickComponent;
	}
	else 
	{
		printf( "Tile point not found at (%f, %f)\n", vBrickObjectPosition.mX, vBrickObjectPosition.mY );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::GetTileWorldPosition( const S32& _iTileX, const S32& _iTileY, t2dVector& _vOutWorldPosition ) const
{
	AssertFatal( _iTileX < g_iMaxTileX && _iTileY < g_iMaxTileY, "CComponentPathGridHandler::GetTileWorldPosition - Array out of bounds." );
	
	if( m_pTileLayerOwner == NULL )
	{
		_vOutWorldPosition = t2dVector::getZero();
		return;
	}
	
	F32 fTileSizeX = m_pTileLayerOwner->getTileSizeX();
	F32 fTileSizeY = m_pTileLayerOwner->getTileSizeY();
	F32 fTileCountX = static_cast<F32>( m_pTileLayerOwner->getTileCountX() );
	F32 fTileCountY = static_cast<F32>( m_pTileLayerOwner->getTileCountY() );
	
	t2dVector vTileLayerPosition = m_pTileLayerOwner->getPosition();
	
	// Get the world position of the Top Left corner of the tile layer
	t2dVector vTileLayerTopLeftPosition;
	vTileLayerTopLeftPosition.mX = -1.0f * ( ((fTileCountX * fTileSizeX) / 2.0f) + (fTileSizeX / 2.0f) );
	vTileLayerTopLeftPosition.mY = -1.0f * ( ((fTileCountY * fTileSizeY) / 2.0f) + (fTileSizeY / 2.0f) );
	
	// Determine the requested tile's world position
	t2dVector vTileWorldPosition;
	vTileWorldPosition.mX = vTileLayerPosition.mX + (vTileLayerTopLeftPosition.mX + (fTileSizeX * ((F32)_iTileX + 1.0f) ) );
	vTileWorldPosition.mY = vTileLayerPosition.mY + (vTileLayerTopLeftPosition.mY + (fTileSizeY * ((F32)_iTileY + 1.0f) ) );
	
	_vOutWorldPosition = vTileWorldPosition;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPathGridHandler::IsLandBrickAttackable( const S32& _iTileX, const S32& _iTileY ) const
{
	AssertFatal( _iTileX < g_iMaxTileX && _iTileY < g_iMaxTileY, "CComponentPathGridHandler::IsTileAttackable - Array out of bounds" );
	
	if( m_aTileInfo[_iTileX][_iTileY].pLandBrickComponent == NULL )
		return false;
	
	return m_aTileInfo[_iTileX][_iTileY].pLandBrickComponent->IsAttackable();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::EnableAllDangerBrickCollisions()
{
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
		{
			if( m_aTileInfo[i][j].pLandBrickComponent && m_aTileInfo[i][j].pLandBrickComponent->IsDangerBrick() )
				m_aTileInfo[i][j].pLandBrickComponent->EnableCollision();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::DealDamageToLandBrick( const S32& _iTileX, const S32& _iTileY, const S32& _iDamageAmount )
{
	AssertFatal( _iTileX < g_iMaxTileX && _iTileY < g_iMaxTileY, "CComponentPathGridHandler::DealDamageToLandBrick - Array out of bounds" );
	
	if( m_aTileInfo[_iTileX][_iTileY].pLandBrickComponent == NULL )
		return;
	
	m_aTileInfo[_iTileX][_iTileY].pLandBrickComponent->DealDamage( _iDamageAmount );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::DealDamageToLandBrickAtPosition( const S32& _iDamageAmount, const t2dVector& _vWorldPosition )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	Point2I tilePoint;
	if( m_pTileLayerOwner->pickTile( _vWorldPosition, tilePoint ) )
	{
		//printf( "DEALING DAMAGE TO TILE: (%d, %d)\n", tilePoint.x, tilePoint.y );
		if( m_aTileInfo[tilePoint.x][tilePoint.y].pLandBrickComponent )
			m_aTileInfo[tilePoint.x][tilePoint.y].pLandBrickComponent->DealDamage( _iDamageAmount );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::GetNextAttackableLandBrickPosition( t2dVector& _vOutTileWorldPosition )
{
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
		{
			// RKS TODO: This only checks for the first land brick that can be attacked.
			if( IsLandBrickAttackable( i, j ) )
			{
				GetTileWorldPosition(i, j, _vOutTileWorldPosition );
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Gets several random positions from attackable bricks. Returns the number of assigned positions

S32 CComponentPathGridHandler::GetArrayRandomAttackableLandBrickPosition( Vector<t2dVector>& _aOutSpawnPositions, const S32& _iDesiredNumberOfSeeds ) const
{
	_aOutSpawnPositions.clear();
	
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
		{
			if( IsLandBrickAttackable( i, j ) )
			{
				GetTileWorldPosition( i, j, g_vTempPosition );
				g_RandomPositionList.push_back( g_vTempPosition );
			}
		}
	}
	
	S32 iNumberOfPositions = g_RandomPositionList.size();
	if( iNumberOfPositions <= 0 )
		return 0;
	
	// Limit the number of seeds to spawn to the number of available positions
	S32 iActualNumSeeds = _iDesiredNumberOfSeeds;
	if( iActualNumSeeds > iNumberOfPositions )
		iActualNumSeeds = iNumberOfPositions;
	
	S32 iRandomIndex = 0;
	for( S32 i = 0; i < iActualNumSeeds; ++i )
	{
		iRandomIndex = CComponentGlobals::GetInstance().GetRandomRange( 0, iNumberOfPositions - 1 );
		_aOutSpawnPositions.push_back( g_RandomPositionList[ iRandomIndex ] );
		g_RandomPositionList.erase( iRandomIndex );
		--iNumberOfPositions;
	}
	
	// Clear the Random Position List, so the next time the code goes through here, it will be empty
	g_RandomPositionList.clear();
	
	return iActualNumSeeds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Gets a random position from attackable bricks. Returns true, if a position was found, false if not found.

bool CComponentPathGridHandler::GetRandomAttackableLandBrickPosition( t2dVector& _vOutTileWorldPosition )
{
	//S32 iCount = 0;
	
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
		{
			if( IsLandBrickAttackable( i, j ) )
			{
				GetTileWorldPosition(i, j, g_vTempPosition );
				g_RandomPositionList.push_back( g_vTempPosition );
			}
		}
	}
	
	S32 iNumberOfPositions = g_RandomPositionList.size();
	if( iNumberOfPositions <= 0 )
	{
		//printf( "Unable to find any random attackable Land Bricks\n" );
		_vOutTileWorldPosition = t2dVector::getZero();
		return false;
	}
	
	S32 iRandomIndex = CComponentGlobals::GetInstance().GetRandomRange( 0, iNumberOfPositions - 1 );
	
	_vOutTileWorldPosition = g_RandomPositionList[ iRandomIndex ];
	
	// Clear the Random Position List, so the next time the code goes through here, it will be empty
	g_RandomPositionList.clear();
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLandBrick* CComponentPathGridHandler::GetLandBrickAtPosition( const t2dVector& _vPosition )
{
	if( m_pTileLayerOwner == NULL )
		return NULL;
	
	Point2I tilePoint;
	
	if( m_pTileLayerOwner->pickTile( _vPosition, tilePoint ) )
	{
		AssertFatal( tilePoint.x < g_iMaxTileX && tilePoint.y < g_iMaxTileY, "CComponentPathGridHandler::GetLandBrickAtPosition - Array out of bounds." );
		if( m_aTileInfo[tilePoint.x][tilePoint.y].pLandBrickComponent == NULL )
		{
			printf( "%s - No land brick at (%f, %f)\n", __FUNCTION__, _vPosition.mX, _vPosition.mY );
			return NULL;
		}
		
		return m_aTileInfo[tilePoint.x][tilePoint.y].pLandBrickComponent;
	}
	
	printf( "%s - Tile point not found at (%f, %f)\n", __FUNCTION__, _vPosition.mX, _vPosition.mY );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::SetLandBrickAtPositionAsFiredUpon( const t2dVector& _vPosition )
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	CComponentLandBrick* pLandBrick = GetLandBrickAtPosition( _vPosition );
	if( pLandBrick )
		pLandBrick->SetIsBeingFiredUpon();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::EnableAllLandBrickCollisions()
{
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
		{
			if( m_aTileInfo[i][j].pLandBrickComponent && 
			    m_aTileInfo[i][j].pLandBrickComponent->IsDangerBrick() == false && 
			    m_aTileInfo[i][j].pLandBrickComponent->IsUnbreakable() == false )
			{
				m_aTileInfo[i][j].pLandBrickComponent->EnableCollision();
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentPathGridHandler::PickTileAtPosition( const t2dVector& _vPosition, Point2I& _outTilePoint )
{
	if( sm_pInstance == NULL || sm_pInstance->m_pTileLayerOwner == NULL )
		return false;
	
	bool bRet = sm_pInstance->m_pTileLayerOwner->pickTile( _vPosition, _outTilePoint );
	return bRet;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPathGridHandler::ResetNumFullyGrownLandBricks()
{
    if( sm_pInstance )
        sm_pInstance->m_iNumFullyGrownLandBricks = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPathGridHandler::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pTileLayerOwner, CComponentPathGridHandler ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPathGridHandler::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	t2dTileLayer* pOwnerTileMap = dynamic_cast<t2dTileLayer*>( _pTarget );
	if( pOwnerTileMap == NULL )
	{
		Con::warnf( "CComponentPathGridHandler::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pTileLayerOwner = pOwnerTileMap;
	m_pTileLayerOwner->setPosition( t2dVector::getZero() );
	
	CComponentPathGridHandler::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::onUpdate()
{
	if( m_pTileLayerOwner == NULL )
		return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPathGridHandler::OnPostInit()
{
	if( m_pTileLayerOwner == NULL )
		return;
	
	m_pTileLayerOwner->setLayer( LAYER_PATH_GRID );
	
	m_pTileLayerOwner->setPosition( t2dVector::getZero() );
	
	// This call to 'updateWorldClip()' is so that 'mWorldClipBoundary' is set properly when the Land Bricks pick the tiles.  For some reason, Torque is not setting 
	// the position properly when the Tile Layer loads, which is offsetting 'mWorldClipBoundary', which is causing 'pickTile()' to fail, as it's in the wrong position during PostInit.
	m_pTileLayerOwner->updateWorldClip( t2dVector::getZero() );
	
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
			m_pTileLayerOwner->setTileCollisionActive( i, j, false );
	}
	
	// RKS NOTE: This is a test to see if I can get away with NOT rendering this object (for optimization)
	m_pTileLayerOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentPathGridHandler::InitializePathInfo()
{
	for( S32 i = 0; i < g_iMaxTileX; ++i )
	{
		for( S32 j = 0; j < g_iMaxTileY; ++j )
		{
			m_aTileInfo[i][j].pBrickObject = NULL;
			m_aTileInfo[i][j].pLandBrickComponent = NULL;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------











