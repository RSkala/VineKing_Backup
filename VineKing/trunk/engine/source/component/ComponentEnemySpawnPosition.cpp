//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySpawnPosition.h"

#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 CComponentEnemySpawnPosition::sm_iCurrentSpawnIndex = 0;
Vector<CComponentEnemySpawnPosition*> CComponentEnemySpawnPosition::sm_SerpentSpawnPositionList;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static CComponentEnemySpawnPosition* g_pTempSpawnPosition = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySpawnPosition );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnPosition::CComponentEnemySpawnPosition()
	: m_pOwner( NULL )
	, m_bInUse( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnPosition::~CComponentEnemySpawnPosition()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnPosition::GetOwnerPosition( t2dVector& _vOutPosition )
{
	_vOutPosition = m_pOwner ? m_pOwner->getPosition() : t2dVector::getZero();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ CComponentEnemySpawnPosition* const CComponentEnemySpawnPosition::GetNextAvailableSpawnPosition()
{
	for( S32 i = 0; i < sm_SerpentSpawnPositionList.size(); ++i )
	{
		g_pTempSpawnPosition = sm_SerpentSpawnPositionList[ sm_iCurrentSpawnIndex ];
		IncrementCurrentSpawnIndex();
		if( g_pTempSpawnPosition && g_pTempSpawnPosition->IsSpawnPositionInUse() == false )
			return g_pTempSpawnPosition;
	}
	
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySpawnPosition::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySpawnPosition ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySpawnPosition::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySpawnPosition::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnPosition::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	CComponentEnemySpawnPosition::sm_SerpentSpawnPositionList.push_back( this );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
