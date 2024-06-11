//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySpawner.h"

#include "ComponentAStarActor.h"
#include "ComponentGlobals.h"
#include "ComponentStaticPath.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySpawner );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawner::CComponentEnemySpawner()
	: m_pOwner( NULL )
	, m_bSpawningEnabled( true )
	, m_pszEnemyType( NULL )
	, m_eEnemyType( ENEMY_TYPE_NONE )
	, m_pszObjectToSpawnName( NULL )
	, m_pObjectToSpawn( NULL )
	, m_uCurrentNumSpawns( 0 )
	, m_bSpawnedEnemyAlive( false )
	, m_pszStaticPath01Name( NULL )
	, m_pszStaticPath02Name( NULL )
	, m_pszStaticPath03Name( NULL )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawner::SpawnEnemy() // The old way using allocations
{
	// Create a new copy of the object that is spawned from the spawn point
	t2dSceneObject* pNewSpawn = m_pObjectToSpawn->clone(); // RKS FUCK:  This needs to be a pool!

	if( pNewSpawn )
	{
		CComponentAStarActor* pAStarActorComponent = static_cast<CComponentAStarActor*>( pNewSpawn->getComponent( 0 ) );
		if( pAStarActorComponent )
		{
			pNewSpawn->setPosition( m_pOwner ? m_pOwner->getPosition() : t2dVector::getZero() );
			
			pAStarActorComponent->SetOwnerSpawnedByComponent( this );
			
			if( m_StaticPathList.size() > 0 )
			{
				S32 iRandom = CComponentGlobals::GetInstance().GetRandomRange( 0, m_StaticPathList.size() - 1 );
			
				//printf( "%s spawning enemy with path #%d\n", m_pOwner ? m_pOwner->getName() : "?", iRandom );
				pAStarActorComponent->SpawnWithPath( m_StaticPathList[iRandom] );
			}
			else 
			{
				//printf( "%s spawning enemy without path\n", m_pOwner ? m_pOwner->getName() : "?" );
				pAStarActorComponent->SpawnWithoutPath();
			}

			m_bSpawnedEnemyAlive = true;
		}
	}
	else
	{
		printf( "%s - Error cloning '%s'\n", __FUNCTION__, m_pObjectToSpawn->getName() );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawner::SpawnEnemy( CComponentAStarActor* const _pEnemyComponent ) // This is the new way using pools
{
	if( _pEnemyComponent == NULL )
		return;
	
	if( _pEnemyComponent )
	{
		_pEnemyComponent->SetOwnerPosition( m_pOwner ? m_pOwner->getPosition() : t2dVector::getZero() );
		
		_pEnemyComponent->SetOwnerSpawnedByComponent( this );
		
		if( m_StaticPathList.size() > 0 )
		{
			S32 iRandom = CComponentGlobals::GetInstance().GetRandomRange( 0, m_StaticPathList.size() - 1 );
			
			//printf( "%s spawning enemy with path #%d\n", m_pOwner ? m_pOwner->getName() : "?", iRandom );
			_pEnemyComponent->SpawnWithPath( m_StaticPathList[iRandom] );
		}
		else 
		{
			//printf( "%s spawning enemy without path\n", m_pOwner ? m_pOwner->getName() : "?" );
			_pEnemyComponent->SpawnWithoutPath();
		}
		
		m_bSpawnedEnemyAlive = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawner::SpawnedEnemyDeathNotify()
{
	//if( m_eEnemyType == ENEMY_TYPE_A )
	//	CComponentGlobals::GetInstance().DecrementSpawnedEnemyACount();
	//else if( m_eEnemyType == ENEMY_TYPE_B )
	//	CComponentGlobals::GetInstance().DecrementSpawnedEnemyBCount();
	//else if( m_eEnemyType == ENEMY_TYPE_C )
	//	CComponentGlobals::GetInstance().DecrementSpawnedEnemyCCount();
	//else
	//	printf( "%s - Unhandled enemy type - %d\n", __FUNCTION__, m_eEnemyType );
	
	m_bSpawnedEnemyAlive = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySpawner::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySpawner ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	// These must match the 'addBehaviorField' calls in the script
	addField( "SpawningEnabled",	TypeBool,	Offset( m_bSpawningEnabled,			CComponentEnemySpawner ) );
	addField( "SpawnedObject",		TypeString, Offset( m_pszObjectToSpawnName,		CComponentEnemySpawner ) );
	
	//%enemyTypes = "A" TAB "B" TAB "C" TAB "D";
	addField( "EnemyType", TypeString, Offset( m_pszEnemyType, CComponentEnemySpawner ) );
	
	addField( "StaticPath01", TypeString, Offset( m_pszStaticPath01Name, CComponentEnemySpawner ) );
	addField( "StaticPath02", TypeString, Offset( m_pszStaticPath02Name, CComponentEnemySpawner ) );
	addField( "StaticPath03", TypeString, Offset( m_pszStaticPath03Name, CComponentEnemySpawner ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySpawner::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySpawner::onComponentAdd - Must be added to a t2dSceneObject" );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	if( m_bSpawningEnabled == false )
		return true;
	
	// Convert the script string type to the C++ enum type
	ConvertEnemyTypeStringToEnum();
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawner::onUpdate()
{
	DynamicConsoleMethodComponent::onUpdate();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawner::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_pObjectToSpawn == NULL )
	{
		// Find the spawning object
		if( m_pszObjectToSpawnName != NULL )
			m_pObjectToSpawn = static_cast<t2dAnimatedSprite*>( Sim::findObject( m_pszObjectToSpawnName ) );
	}
	
	m_StaticPathList.clear();
	
	FindAndAddStaticPath( m_pszStaticPath01Name );
	FindAndAddStaticPath( m_pszStaticPath02Name );
	FindAndAddStaticPath( m_pszStaticPath03Name );
	
	//printf( "%s - Added %d Static Paths\n", m_pOwner ? m_pOwner->getName() : "?", m_StaticPathList.size() );
	
	// Add this enemy spawner
	//switch( m_eEnemyType )
	//{
	//	case ENEMY_TYPE_A:
	//		CComponentGlobals::GetInstance().AddEnemyASpawner( this );
	//		break;
	//		
	//	case ENEMY_TYPE_B:
	//		CComponentGlobals::GetInstance().AddEnemyBSpawner( this );
	//		break;
	//		
	//	case ENEMY_TYPE_C:
	//		CComponentGlobals::GetInstance().AddEnemyCSpawner( this );
	//		break;
	//		
	//	default:
	//		printf( "Unknown enemy spawner type: %d\n", m_eEnemyType );
	//		break;
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentEnemySpawner::ConvertEnemyTypeStringToEnum()
{
	if( m_pszEnemyType == NULL )
		return;
	
	// %enemyTypes = "A" TAB "B" TAB "C" TAB "D";
	if( strcmp( m_pszEnemyType, "A" ) == 0 )
	{
		m_eEnemyType = ENEMY_TYPE_A;
	}
	else if( strcmp( m_pszEnemyType, "B" ) == 0 )
	{
		m_eEnemyType = ENEMY_TYPE_B;
	}
	else if( strcmp( m_pszEnemyType, "C" ) == 0 )
	{
		m_eEnemyType = ENEMY_TYPE_C;
	}
	else if( strcmp( m_pszEnemyType, "D" ) == 0 )
	{
		m_eEnemyType = ENEMY_TYPE_D;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawner::FindAndAddStaticPath( const char* _pszStaticPathOwnerName )
{
	if( _pszStaticPathOwnerName == NULL || _pszStaticPathOwnerName[0] == '\0' )
		return;
	
	t2dSceneObject* pStaticPathOwner = static_cast<t2dSceneObject*>( Sim::findObject( _pszStaticPathOwnerName ) );
	if( pStaticPathOwner )
	{
		CComponentStaticPath* pStaticPathComponent = static_cast<CComponentStaticPath*>( pStaticPathOwner->getComponent( 0 ) );
		if( pStaticPathComponent )
			m_StaticPathList.push_back( pStaticPathComponent );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
