//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentShooterEnemySpawner.h"

#include "ComponentGlobals.h"
#include "ComponentShooterEnemy.h"

#include "T2D/t2dAnimatedSprite.h"

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const U32 g_uMaxSpawnedEnemies = 15;
static const F32 g_fTimeBetweenSpawnsMS = 200; // in Millseconds
static const F32 g_fInitialSpawnDelayTimeMS = 1000; // in Milliseconds

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentShooterEnemySpawner );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentShooterEnemySpawner::CComponentShooterEnemySpawner()
	: m_pOwnerObject( NULL )
	, m_fEnemySpawnTimer( 0.0f )
	, m_pszSpawnedEnemyName( NULL )
	, m_pEnemyToSpawn( NULL )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentShooterEnemySpawner::~CComponentShooterEnemySpawner()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from ConsoleObject
/*static*/ void CComponentShooterEnemySpawner::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentShooterEnemySpawner ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "SpawnedEnemy", TypeString, Offset( m_pszSpawnedEnemyName, CComponentShooterEnemySpawner ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentShooterEnemySpawner::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentShooterEnemySpawner::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	// NOTE: Do not set up any other member pointers here, because Torque may not have created the objects yet
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemySpawner::onUpdate()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fInitialSpawnDelayTimer += ITickable::smTickMs;
	if( m_fInitialSpawnDelayTimer < g_fInitialSpawnDelayTimeMS )
		return;
	
	m_fEnemySpawnTimer += ITickable::smTickMs;
	if( m_fEnemySpawnTimer >= g_fTimeBetweenSpawnsMS )
	{
		m_fEnemySpawnTimer = 0.0f;
		SpawnEnemy();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentShooterEnemySpawner::OnPostInit()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( m_pEnemyToSpawn == NULL )
	{
		m_pEnemyToSpawn = static_cast<t2dAnimatedSprite*>( Sim::findObject( m_pszSpawnedEnemyName ) );
		if( m_pEnemyToSpawn )
		{
			t2dAnimatedSprite* pNewEnemy = NULL;
			for( U32 i = 0; i < g_uMaxSpawnedEnemies; ++i )
			{
				pNewEnemy = static_cast<t2dAnimatedSprite*>( m_pEnemyToSpawn->clone() );
				if( pNewEnemy )
					m_EnemySpawnPool.push_back( pNewEnemy );
			}
		}
	}
	
	S32 iNumSpawns = m_EnemySpawnPool.size();
	printf( "Created %d spawns\n", iNumSpawns );
	
	m_pOwnerObject->enableUpdateCallback();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHOD IMPLMENTATION

void CComponentShooterEnemySpawner::SpawnEnemy()
{
	CComponentShooterEnemy* pEnemyComponent = NULL;
	pEnemyComponent = GetNextAvailableEnemy();
	if( pEnemyComponent )
	{
		// Pick a random position to the right of the spawner
		t2dVector vSpawnPosition = (m_pOwnerObject ? m_pOwnerObject->getPosition() : t2dVector::getZero() );
		F32 fRandomX = static_cast<F32>( CComponentGlobals::GetInstance().GetRandomRange( 0, 300 ) );
		vSpawnPosition.mX += fRandomX;
		
		pEnemyComponent->SpawnAtPosition( vSpawnPosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentShooterEnemy* CComponentShooterEnemySpawner::GetNextAvailableEnemy()
{
	CComponentShooterEnemy* pEnemyComponent = NULL;
	t2dAnimatedSprite* pEnemy = NULL;
	for( U32 i = 0; i < g_uMaxSpawnedEnemies; ++i )
	{
		// RKS TODO: Getting the component should be in OnPostInit(), but since this is just a test, I will leave this for now
		pEnemy = m_EnemySpawnPool[ i ];
		if( pEnemy && pEnemy->getComponentCount() > 0 )
		{
			pEnemyComponent = static_cast<CComponentShooterEnemy*>( pEnemy->getComponent( 0 ) );
			if( pEnemyComponent && pEnemyComponent->IsAvailable() )
			{
				//bool bIsAvailable = pEnemyComponent->IsAvailable();
				//printf( "bIsAvailable = %d\n", bIsAvailable );
				
				//if( bIsAvailable )
					return pEnemyComponent;
			}
		}
	}
	
	// If this point is reached, then there are no available enemies, so return NULL
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
