//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySpawnController.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentEnemyBlob.h"
#include "ComponentEnemySerpent.h"
#include "ComponentEnemySmoke.h"
#include "ComponentEnemySpawnPosition.h"
#include "ComponentEnemySmokePositionList.h"
#include "ComponentEnemySpawnTimeSlot.h"
#include "ComponentGlobals.h"
#include "ComponentLevelBeginSequence.h"
#include "ComponentStaticPath.h"
#include "ComponentTutorialLevel.h"

#include "core/iTickable.h"
#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szEnemyBlob[32]		= "Enemy_Blob";
static const char g_szEnemySerpent[32]	= "Enemy_Serpent";
static const char g_szEnemySmoke[32]	= "Enemy_ManaEater";

static const S32 g_iMaxNumEnemyBlobs	= 20;
static const S32 g_iMaxNumEnemySerpents = 5;
static const S32 g_iMaxNumEnemySmoke	= 15;

static const char g_szTimeSlotArray[ CComponentEnemySpawnController::m_iMaxTimeSlots ][16] =
{
	"TimeSlot01",
	"TimeSlot02",
	"TimeSlot03",
	"TimeSlot04",
	"TimeSlot05",
	"TimeSlot06",
	"TimeSlot07",
	"TimeSlot08",
	"TimeSlot09",
	"TimeSlot10",
	"TimeSlot11",
	"TimeSlot12",
	"TimeSlot13",
	"TimeSlot14",
	"TimeSlot15",
	"TimeSlot16"
};


// Temp variable for the SpawnEnemy() function, so there is no need to keep creating these objects
static CComponentStaticPath*			g_pStaticPath			= NULL;
static CComponentEnemyBlob*				g_pEnemyBlob			= NULL;
static CComponentEnemySerpent*			g_pEnemySerpent			= NULL;
static CComponentEnemySmoke*			g_pEnemySmoke			= NULL;
static CComponentEnemySpawnPosition*	g_pEnemySpawnPosition	= NULL;

static t2dVector g_vSpawnPosition, g_vStartPos, g_vEndPos;
static S32 g_iSmokePosIndex = 0;

//static const F32 g_fSmokeScaleAmount = 1.2f;

static const F32 g_fNewEnemySizeX = 44.0f;
static const F32 g_fNewEnemySizeY = 88.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnController* CComponentEnemySpawnController::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySpawnController );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnController::CComponentEnemySpawnController()
	: m_pOwner( NULL )
	, m_pszLoopBehavior( NULL )
	, m_eLoopBehaviorType( LOOP_BEHAVIOR_TYPE_NONE )
	, m_fTimeSlotTimer( 0.0f )
	, m_fNextTimeSlotStartTime( 0.0f )
	, m_fFinalTimeSlotTimeDifference( 0.0f )
	, m_iCurrentTimeSlotIndex( 0 )
	//, m_iCurrentSpawnIndexBlob( 0 )
	//, m_iCurrentSpawnIndexSerpent( 0 )
	//, m_iCurrentSpawnIndexSmoke( 0 )
	, m_iNumUsedTimeSlots( 0 )
	, m_bAtLastTimeSlot( false )
{
	for( S32 i = 0; i < m_iMaxTimeSlots; ++i )
	{
		m_bUseTimeSlotArray[i] = false;
	}
	
	//ResetTimeSlotEntryArrays();
	
	CComponentEnemySpawnController::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnController::~CComponentEnemySpawnController()
{
	CComponentEnemySpawnController::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnController::ResetTimeSlotEntryArrays()
{
	//printf( "CComponentEnemySpawnController::ResetTimeSlotEntryArrays\n" );
	
	for( S32 i = 0; i < m_iMaxTimeSlotEntries; ++i )
	{
		m_fEntryStaggerTimerArray[i] = 0.0f;
		m_iEntrySpawnCountArray[i] = 0;
		m_iEntryCurrentSpawnPointIndexArray[i] = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySpawnController::KillAllActiveEnemiesFromStomp()
{
	if( sm_pInstance == NULL )
		return;
	
	//S32 iNumBlobsKilled, iNumSerpentsKilled, iNumSmokesKilled;
	//iNumBlobsKilled = iNumSerpentsKilled = iNumSmokesKilled = 0;
	
	// Kill all active Blobs
	S32 iNumEnemies = sm_pInstance->m_EnemyBlobPool.size();
	CComponentEnemyBlob* pBlob = NULL;
	for( S32 i = 0; i < iNumEnemies; ++i )
	{
		pBlob = sm_pInstance->m_EnemyBlobPool[i];
		if( pBlob && pBlob->IsWaitingForStomp() )
		{
			pBlob->KillFromStomp();
			//++iNumBlobsKilled;
		}
	}
	
	// Kill all active Serpents
	iNumEnemies = sm_pInstance->m_EnemySerpentPool.size();
	CComponentEnemySerpent* pSerpent = NULL;
	for( S32 i = 0; i < iNumEnemies; ++i )
	{
		pSerpent = sm_pInstance->m_EnemySerpentPool[i];
		if( pSerpent && pSerpent->IsWaitingForStomp() )
		{
			pSerpent->KillFromStomp();
			//++iNumSerpentsKilled;
		}
	}
	
	// Kill all active Smokes
	iNumEnemies = sm_pInstance->m_EnemySmokePool.size();
	CComponentEnemySmoke* pSmoke = NULL;
	for( S32 i = 0; i < iNumEnemies; ++i )
	{
		pSmoke = sm_pInstance->m_EnemySmokePool[i];
		if( pSmoke && pSmoke->IsWaitingForStomp() )
		{
			pSmoke->KillFromStomp();
			//++iNumSmokesKilled;
		}
	}
	
	//printf( "Blobs killed from stomp:    %d\n", iNumBlobsKilled );
	//printf( "Serpents killed from stomp: %d\n", iNumSerpentsKilled );
	//printf( "Smokes killed from stomp:   %d\n", iNumSmokesKilled );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySpawnController::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySpawnController ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	

	addField( "LoopBehavior", TypeCaseString, Offset( m_pszLoopBehavior, CComponentEnemySpawnController ) );
	
	addField( "UseTimeSlot01", TypeBool, Offset( m_bUseTimeSlotArray[0],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot02", TypeBool, Offset( m_bUseTimeSlotArray[1],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot03", TypeBool, Offset( m_bUseTimeSlotArray[2],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot04", TypeBool, Offset( m_bUseTimeSlotArray[3],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot05", TypeBool, Offset( m_bUseTimeSlotArray[4],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot06", TypeBool, Offset( m_bUseTimeSlotArray[5],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot07", TypeBool, Offset( m_bUseTimeSlotArray[6],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot08", TypeBool, Offset( m_bUseTimeSlotArray[7],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot09", TypeBool, Offset( m_bUseTimeSlotArray[8],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot10", TypeBool, Offset( m_bUseTimeSlotArray[9],  CComponentEnemySpawnController ) );
	addField( "UseTimeSlot11", TypeBool, Offset( m_bUseTimeSlotArray[10], CComponentEnemySpawnController ) );
	addField( "UseTimeSlot12", TypeBool, Offset( m_bUseTimeSlotArray[11], CComponentEnemySpawnController ) );
	addField( "UseTimeSlot13", TypeBool, Offset( m_bUseTimeSlotArray[12], CComponentEnemySpawnController ) );
	addField( "UseTimeSlot14", TypeBool, Offset( m_bUseTimeSlotArray[13], CComponentEnemySpawnController ) );
	addField( "UseTimeSlot15", TypeBool, Offset( m_bUseTimeSlotArray[14], CComponentEnemySpawnController ) );
	addField( "UseTimeSlot16", TypeBool, Offset( m_bUseTimeSlotArray[15], CComponentEnemySpawnController ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySpawnController::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySpawnController::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	CComponentEnemySpawnController::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnController::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLevelInitializationComplete() == false )
		return;
	
	if( CComponentLevelBeginSequence::GetInstance().HasLevelBeginSequenceFinished() == false )
		return;
	
	if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
		return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
		return;
	
	if( CComponentGlobals::GetInstance().IsTimerActive() == false )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() )
	{
		//if( CComponentTutorialLevel::HasTutorialBeenShown_LineDraw() == false )
		if( CComponentTutorialLevel::IsSpawningAllowed() == false )
			return;
	}
	
	UpdateSpawning();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnController::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
		return;
	
	// Get the usable Time Slots
	for( S32 i = 0; i < m_iMaxTimeSlots; ++i )
	{
		if( m_bUseTimeSlotArray[i] )
		{
			t2dSceneObject* pTimeSlotOwner = static_cast<t2dSceneObject*>( Sim::findObject( g_szTimeSlotArray[i] ) );
			if( pTimeSlotOwner )
			{
				CComponentEnemySpawnTimeSlot* pTimeSlot = static_cast<CComponentEnemySpawnTimeSlot*>( pTimeSlotOwner->getComponent( 0 ) );
				if( pTimeSlot )
					m_TimeSlots.push_back( pTimeSlot );
			}
		}
	}
	
	m_iNumUsedTimeSlots = m_TimeSlots.size();
	//printf( "Using %d Spawning Time Slots\n", m_iNumUsedTimeSlots );
	AssertFatal( m_iNumUsedTimeSlots >= 2 , "Error: At least two Time Slots must be used" );

	InitializeEnemyPools();
	
	m_iCurrentTimeSlotIndex = -1;
	//m_iCurrentSpawnIndexBlob = m_iCurrentSpawnIndexSerpent = m_iCurrentSpawnIndexSmoke = 0;
	
	ResetTimeSlotEntryArrays();
	
	m_fTimeSlotTimer = 0.0f;
	//m_fNextTimeSlotStartTime = m_TimeSlots[0]->GetStartTime(); // RKS TODO: Check if it is safe to assume that the values are set
	m_fNextTimeSlotStartTime = m_TimeSlots[0]->m_fSlotStartTime; // RKS TODO: Check if it is safe to assume that the values are set
	
	//m_fFinalTimeSlotTimeDifference = ( m_TimeSlots[m_iNumUsedTimeSlots - 1]->GetStartTime() - m_TimeSlots[m_iNumUsedTimeSlots - 2]->GetStartTime() );
	m_fFinalTimeSlotTimeDifference = ( m_TimeSlots[m_iNumUsedTimeSlots - 1]->m_fSlotStartTime - m_TimeSlots[m_iNumUsedTimeSlots - 2]->m_fSlotStartTime );
	
	if( m_pszLoopBehavior && m_pszLoopBehavior[0] != '\0' )
	{
		//%loopBehavior = "-" TAB "LOOP TO START" TAB "LOOP LAST TIME SLOT";
		if( strcmp( m_pszLoopBehavior, "LOOP TO START" ) == 0 )
		{
			m_eLoopBehaviorType = LOOP_BEHAVIOR_TYPE_START_SLOT;
		}
		else if( strcmp( m_pszLoopBehavior, "LOOP LAST TIME SLOT" ) == 0 )
		{
			m_eLoopBehaviorType = LOOP_BEHAVIOR_TYPE_LAST_SLOT;
		}
		else
		{
			AssertFatal( 0, "ERROR: Loop Behavior Type not set.\n" );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private member functions

void CComponentEnemySpawnController::InitializeEnemyPools()
{
	// Set up the enemy pools
	t2dSceneObject* pClonedObject = NULL;
	char szName[32];
	
	// Set up enemy Blob pool
	t2dSceneObject* pObjectToClone = static_cast<t2dSceneObject*>( Sim::findObject( g_szEnemyBlob ) );
	if( pObjectToClone )
	{
        // Force the size to be 44 x 88 (which is 1.375 times 32x64)
        t2dVector vNewSize = pObjectToClone->getSize();
		vNewSize.mX = g_fNewEnemySizeX;
		vNewSize.mY = g_fNewEnemySizeY;
		pObjectToClone->setSize( vNewSize );
        
        
		pObjectToClone->setLayer( LAYER_PLAYER_BLOB_SERPENT );
		
		CComponentEnemyBlob* pBlobComponent = static_cast<CComponentEnemyBlob*>( pObjectToClone->getComponent( 0 ) );
		if( pBlobComponent )
			m_EnemyBlobPool.push_back( pBlobComponent );
		
		for( S32 i = 1; i < g_iMaxNumEnemyBlobs; ++i )
		{			
			pClonedObject = static_cast<t2dSceneObject*>( pObjectToClone->clone() );
			if( pClonedObject )
			{
				sprintf( szName, "%s_%d", pObjectToClone->getName(), i );
				pClonedObject->assignName( szName );
				
				pBlobComponent = static_cast<CComponentEnemyBlob*>( pClonedObject->getComponent( 0 ) );
				if( pBlobComponent )
					m_EnemyBlobPool.push_back( pBlobComponent );
			}
		}
	}
	//printf( "Num Blob Enemies: %d\n", m_EnemyBlobPool.size() );
	
	// Set up enemy Serpent pool
	pObjectToClone = static_cast<t2dSceneObject*>( Sim::findObject( g_szEnemySerpent ) );
	if( pObjectToClone )
	{
       // Force the size to be 44 x 88 (which is 1.375 times 32x64)
        t2dVector vNewSize = pObjectToClone->getSize();
		vNewSize.mX = g_fNewEnemySizeX;
		vNewSize.mY = g_fNewEnemySizeY;
		pObjectToClone->setSize( vNewSize );
        
		pObjectToClone->setLayer( LAYER_PLAYER_BLOB_SERPENT );
		
		CComponentEnemySerpent* pSerpentComponent = static_cast<CComponentEnemySerpent*>( pObjectToClone->getComponent( 0 ) );
		if( pSerpentComponent )
			m_EnemySerpentPool.push_back( pSerpentComponent );
		
		for( S32 i = 1; i < g_iMaxNumEnemySerpents; ++i )
		{
			pClonedObject = static_cast<t2dSceneObject*>( pObjectToClone->clone() );
			if( pClonedObject )
			{
				sprintf( szName, "%s_%d", pObjectToClone->getName(), i );
				pClonedObject->assignName( szName );
				
				pSerpentComponent = static_cast<CComponentEnemySerpent*>( pClonedObject->getComponent( 0 ) );
				if( pSerpentComponent )
					m_EnemySerpentPool.push_back( pSerpentComponent );
			}
		}		
	}
	//printf( "Num Serpent Enemies: %d\n", m_EnemySerpentPool.size() );
	
	// Set up enemy Smoke pool
	pObjectToClone = static_cast<t2dSceneObject*>( Sim::findObject( g_szEnemySmoke ) );
	if( pObjectToClone )
	{
        // Force the size to be 44 x 88 (which is 1.375 times 32x64)
		t2dVector vNewSize = pObjectToClone->getSize();
		//vNewSize.mX *= g_fSmokeScaleAmount;
		//vNewSize.mY *= g_fSmokeScaleAmount;
        vNewSize.mX = g_fNewEnemySizeX;
		vNewSize.mY = g_fNewEnemySizeY;
		pObjectToClone->setSize( vNewSize );
		
		pObjectToClone->setLayer( LAYER_SMOKE );
		
		CComponentEnemySmoke* pSmokeComponent = static_cast<CComponentEnemySmoke*>( pObjectToClone->getComponent( 0 ) );
		if( pSmokeComponent )
			m_EnemySmokePool.push_back( pSmokeComponent );
		
		for( S32 i = 1; i < g_iMaxNumEnemySmoke; ++i )
		{
			pClonedObject = static_cast<t2dSceneObject*>( pObjectToClone->clone() );
			if( pClonedObject )
			{
				sprintf( szName, "%s_%d", pObjectToClone->getName(), i );
				pClonedObject->assignName( szName );
				
				pSmokeComponent = static_cast<CComponentEnemySmoke*>( pClonedObject->getComponent( 0 ) );
				if( pSmokeComponent )
					m_EnemySmokePool.push_back( pSmokeComponent );
			}
		}
	}
	//printf( "Num Smoke Enemies: %d\n", m_EnemySmokePool.size() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnController::UpdateSpawning()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
		return;
	
	// Make sure that no spawning occurs until it is supposed to (that is, not until TimeSlot01 (index 0) is supposed to start)
	if( m_iCurrentTimeSlotIndex == -1 )
	{
		m_fTimeSlotTimer += ITickable::smTickSec;
		if( m_fTimeSlotTimer < m_fNextTimeSlotStartTime )
			return;
		
		m_iCurrentTimeSlotIndex = 0;
		m_fNextTimeSlotStartTime = m_TimeSlots[1]->m_fSlotStartTime;
	}
	
	if( m_iCurrentTimeSlotIndex == -1 ) // This shouldn't ever happen, but catch it just in case
	{
		printf( "WARNING: CComponentEnemySpawnController::UpdateSpawning - m_iCurrentTimeSlotIndex == -1\n" );
		return;
	}
	
	// Iterate through the list of stagger timers for each entry in the current Time Slot, spawn the appropriate type and number of enemies according to each entry.
	for( S32 i = 0; i < m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_iNumUsedEntries; ++i )
	{
		if( m_iEntrySpawnCountArray[i] < m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[i].m_iNumEnemiesToSpawn )
		{
			m_fEntryStaggerTimerArray[i] -= ITickable::smTickSec;
			if( m_fEntryStaggerTimerArray[i] <= 0.0f )
			{
				SpawnEnemy( i, static_cast<S32>( m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[i].m_eEnemyType ) );
				m_fEntryStaggerTimerArray[i] = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[i].m_fStaggerTime;
				m_iEntrySpawnCountArray[i]++; // Increment this value whether an enemy was successfully spawned or not.
				//printf( "m_iEntrySpawnCountArray[i]: %d\n", m_iEntrySpawnCountArray[i] );
			}
		}
	}
	
	CheckTimeSlotIncrement();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnController::SpawnEnemy( const S32& _iEntryIndex, const S32& _iEnemyType )
{
	bool bValidSmokePosition = false;
	
	// Note: This function assumes that the current Time Slot index and Entry index are correct
	switch( _iEnemyType )
	{
		//--------------------------------------------------------
		case CComponentEnemySpawnTimeSlot::ENEMY_TYPE_BLOB:
			
			if( m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pStaticPath == NULL )
			{
				// Iterate through the list of Spawn Points
				g_pStaticPath = CComponentStaticPath::GetStaticPathAtIndex( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] );
				
				m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex]++;
				if( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] >= CComponentStaticPath::GetNumStaticPaths() )
					m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] = 0;
			}
			else
			{
				// Use a specific Spawn Point
				g_pStaticPath = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pStaticPath;
			}
			
			g_pEnemyBlob = GetNextAvailableEnemyBlob();
			if( g_pEnemyBlob )
			{
				g_pEnemyBlob->SpawnWithPath( g_pStaticPath );
			}
			
			break;
			
		//--------------------------------------------------------
		case CComponentEnemySpawnTimeSlot::ENEMY_TYPE_SUPER_BLOB:
			
			if( m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pStaticPath == NULL )
			{
				// Iterate through the list of Spawn Points
				g_pStaticPath = CComponentStaticPath::GetStaticPathAtIndex( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] );
				
				m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex]++;
				if( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] >= CComponentStaticPath::GetNumStaticPaths() )
					m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] = 0;
			}
			else
			{
				// Use a specific Spawn Point
				g_pStaticPath = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pStaticPath;
			}
			
			g_pEnemyBlob = GetNextAvailableEnemyBlob();
			if( g_pEnemyBlob )
			{
				g_pEnemyBlob->SpawnSuperWithPath( g_pStaticPath );
			}
			
			break;
			
		//--------------------------------------------------------
		case CComponentEnemySpawnTimeSlot::ENEMY_TYPE_SERPENT:
			
			if( m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pSpawnPosition == NULL )
			{
				// Find next free position
				g_pEnemySpawnPosition = CComponentEnemySpawnPosition::GetNextAvailableSpawnPosition();
			}
			else
			{
				// Specific spawn position specified
				g_pEnemySpawnPosition = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pSpawnPosition;
				if( g_pEnemySpawnPosition && g_pEnemySpawnPosition->IsSpawnPositionInUse() )
					g_pEnemySpawnPosition = NULL;
			}
			
			if( g_pEnemySpawnPosition )
			{
				g_pEnemySerpent = GetNextAvailableEnemySerpent();
				if( g_pEnemySerpent )
					g_pEnemySerpent->SpawnAtPosition( g_pEnemySpawnPosition );
			}
			
			break;
			
		//--------------------------------------------------------
		case CComponentEnemySpawnTimeSlot::ENEMY_TYPE_SUPER_SERPENT:
				
			if( m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pSpawnPosition == NULL )
			{
				// Find next free position
				g_pEnemySpawnPosition = CComponentEnemySpawnPosition::GetNextAvailableSpawnPosition();
			}
			else
			{
				// Specific spawn position specified
				g_pEnemySpawnPosition = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_pSpawnPosition;
				if( g_pEnemySpawnPosition && g_pEnemySpawnPosition->IsSpawnPositionInUse() )
					g_pEnemySpawnPosition = NULL;
			}
			
			if( g_pEnemySpawnPosition )
			{
				g_pEnemySerpent = GetNextAvailableEnemySerpent();
				if( g_pEnemySerpent )
					g_pEnemySerpent->SpawnSuperAtPosition( g_pEnemySpawnPosition );
			}
			
			break;
			
		//--------------------------------------------------------
		case CComponentEnemySpawnTimeSlot::ENEMY_TYPE_SMOKE:
			
			g_iSmokePosIndex = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_iSmokePosIndex;
			if( g_iSmokePosIndex == -1 )
			{
				// Iterate through list
				bValidSmokePosition = CComponentEnemySmokePositionList::GetPositionsAtIndex( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex], g_vStartPos, g_vEndPos );
				
				m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex]++;
				if( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] >= CComponentEnemySmokePositionList::GetNumSmokePositions() )
				   m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] = 0;
			}
			else
			{
				// Specific spawn position specified
				bValidSmokePosition = CComponentEnemySmokePositionList::GetPositionsAtIndex( g_iSmokePosIndex, g_vStartPos, g_vEndPos );
			}
			
			//printf( "Spawning Smoke at Time: %f, at (%f, %f)\n", m_fTimeSlotTimer, g_vStartPos.mX, g_vStartPos.mY );
			if( bValidSmokePosition )
			{
				g_pEnemySmoke = GetNextAvailableEnemySmoke();
				if( g_pEnemySmoke )
				{
					g_pEnemySmoke->SpawnAtPositionWithEndpoint( g_vStartPos, g_vEndPos );
				}
			}
				   
			break;
			
		//--------------------------------------------------------
		case CComponentEnemySpawnTimeSlot::ENEMY_TYPE_SUPER_SMOKE:
			
			g_iSmokePosIndex = m_TimeSlots[ m_iCurrentTimeSlotIndex ]->m_TimeSlotEntries[ _iEntryIndex ].m_iSmokePosIndex;
			if( g_iSmokePosIndex == -1 )
			{
				// Iterate through list
				bValidSmokePosition = CComponentEnemySmokePositionList::GetPositionsAtIndex( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex], g_vStartPos, g_vEndPos );
				
				m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex]++;
				if( m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] >= CComponentEnemySmokePositionList::GetNumSmokePositions() )
					m_iEntryCurrentSpawnPointIndexArray[_iEntryIndex] = 0;
			}
			else
			{
				// Specific spawn position specified
				bValidSmokePosition = CComponentEnemySmokePositionList::GetPositionsAtIndex( g_iSmokePosIndex, g_vStartPos, g_vEndPos );
			}
			
			//printf( "Spawning Super Smoke at Time: %f, at (%f, %f)\n", m_fTimeSlotTimer, g_vStartPos.mX, g_vStartPos.mY );
			if( bValidSmokePosition )
			{
				g_pEnemySmoke = GetNextAvailableEnemySmoke();
				if( g_pEnemySmoke )
				{
					g_pEnemySmoke->SpawnSuperAtPositionWithEndpoint( g_vStartPos, g_vEndPos );
				}
			}
			
			break;
			
		//--------------------------------------------------------
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnController::CheckTimeSlotIncrement()
{
	// Need to check: m_iNumUsedTimeSlots and shit
	
	// If not in the last time slot, count UP. If in the last time slot, count DOWN.
	if( m_bAtLastTimeSlot == false )
	{
		// At last time slot -- Counting UP
		m_fTimeSlotTimer += ITickable::smTickSec;
		if( m_fTimeSlotTimer < m_fNextTimeSlotStartTime )
			return;
	}
	else
	{
		// At last time slot -- Counting DOWN
		m_fTimeSlotTimer -= ITickable::smTickSec;
		if( m_fTimeSlotTimer > 0.0f )
			return;
		
		// After we run the last time slot ONCE, then determine whether to repeat this or Loop to the beginning
		if( m_eLoopBehaviorType == LOOP_BEHAVIOR_TYPE_START_SLOT )
		{
			// Start over from the beginning
			//printf( "Starting Time Slot time over from the beginning\n" );
			
			m_iCurrentTimeSlotIndex = -1;
			m_fTimeSlotTimer = 0.0f;
			m_fNextTimeSlotStartTime = m_TimeSlots[0]->m_fSlotStartTime;
			m_bAtLastTimeSlot = false;
		}
		else if( m_eLoopBehaviorType == LOOP_BEHAVIOR_TYPE_LAST_SLOT )
		{
			m_fTimeSlotTimer = m_fFinalTimeSlotTimeDifference;
			m_bAtLastTimeSlot = true;
		}
		
		//m_fTimeSlotTimer = m_fFinalTimeSlotTimeDifference;
		
		ResetTimeSlotEntryArrays();
		
		//printf( "==============================================\n" );
		//printf( "CHANGING TIME SLOT TO (FINAL): %d, TIME: %f\n", m_iCurrentTimeSlotIndex, m_fTimeSlotTimer );
		//printf( "TIME SLOT LOOP TYPE: %d\n", m_eLoopBehaviorType );
		//printf( " - m_fFinalTimeSlotTimeDifference: %f\n", m_fFinalTimeSlotTimeDifference );
	}
	
	
	// Increase the Time Slot Index
	if( m_bAtLastTimeSlot == false )
	{
		bool bAtLastTimeSlot = false;
		if( m_iCurrentTimeSlotIndex != m_iMaxTimeSlots - 1 ) // Make sure that the current Time Slot is not the ACTUAL last Time Slot
		{
			S32 iNextTimeSlot = m_iCurrentTimeSlotIndex + 1;

			if( m_bUseTimeSlotArray[iNextTimeSlot] ) // Can the upcoming Time Slot be used?
			{
				// Move to the next Time Slot
				m_iCurrentTimeSlotIndex++;
				
				S32 iNewNextTimeSlot = m_iCurrentTimeSlotIndex + 1;

				// Check if the NEW next Time Slot is usable and check if the NEW next Time Slot is the actual last (this determines if we have just moved to the last Time Slot)
				if( (m_bUseTimeSlotArray[iNewNextTimeSlot]) && (iNewNextTimeSlot < m_iMaxTimeSlots - 1) )
				{
					// We did not just move to the last Time Slot. Set the Start Time for the next Time Slot.
					m_fNextTimeSlotStartTime = m_TimeSlots[iNewNextTimeSlot]->m_fSlotStartTime;
					
					// When changing the Time Slot, reset the staggered spawn timers and number of enemies spawned
					ResetTimeSlotEntryArrays();
					
					//printf( "==============================================\n" );
					//printf( "CHANGING TIME SLOT TO (1): %d, TIME: %f\n", m_iCurrentTimeSlotIndex, m_fTimeSlotTimer );
					//printf( " - m_fNextTimeSlotStartTime: %f\n", m_fNextTimeSlotStartTime );
				}
				else
				{
					// This is the last USABLE Time Slot
					bAtLastTimeSlot = true;
				}
			}
		}
		else
		{
			// This is the last ACTUAL Time Slot
			bAtLastTimeSlot = true;
		}
		
		// Is the current Time Slot the last one (whether Actual or Last Usable)
		if( bAtLastTimeSlot )
		{
			// Now that we are in the last Time Slot, the count will go backwards.
			m_bAtLastTimeSlot = true;
			m_fTimeSlotTimer = m_fFinalTimeSlotTimeDifference;
			
			// When changing the Time Slot, reset the staggered spawn timers and number of enemies spawned
			ResetTimeSlotEntryArrays();
			
			//printf( "==============================================\n" );
			//printf( "CHANGING TIME SLOT TO (2): %d, TIME: %f\n", m_iCurrentTimeSlotIndex, m_fTimeSlotTimer );
			//printf( "TIME SLOT LOOP TYPE: %d\n", m_eLoopBehaviorType );
			//printf( " - m_fNextTimeSlotStartTime: %f\n", m_fNextTimeSlotStartTime );
		}
	}
	
	//printf( "================================================================\n" );
	//printf( "Changing Time Slot Index: %d, at Time: %f\n", m_iCurrentTimeSlotIndex, CComponentGlobals::GetInstance().GetLevelTimeSeconds() );
	
	AssertFatal( m_iCurrentTimeSlotIndex < m_iMaxTimeSlots, "CComponentEnemySpawnController::CheckTimeSlotIncrement - Time Slot index out of bounds\n" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyBlob* CComponentEnemySpawnController::GetNextAvailableEnemyBlob() const
{
	CComponentEnemyBlob* pEnemyBlob = NULL;
	S32 iNumEnemyBlobs = m_EnemyBlobPool.size();
	for( S32 i = 0; i < iNumEnemyBlobs; ++i )
	{
		pEnemyBlob = m_EnemyBlobPool[i];
		if( pEnemyBlob && pEnemyBlob->IsSpawned() == false )
			return pEnemyBlob;
	}
	
	//printf( "No available Blob enemies.\n" );
	return NULL;
}
				   
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
				   
CComponentEnemySerpent* CComponentEnemySpawnController::GetNextAvailableEnemySerpent() const
{	
	CComponentEnemySerpent* pEnemySerpent = NULL;
	S32 iNumEnemySerpents = m_EnemySerpentPool.size();
	for( S32 i = 0; i < iNumEnemySerpents; ++i )
	{
		pEnemySerpent = m_EnemySerpentPool[i];
		if( pEnemySerpent && pEnemySerpent->IsSpawned() == false )
			return pEnemySerpent;
	}
	
	//printf( "No available Serpent enemies.\n" );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
				   
CComponentEnemySmoke* CComponentEnemySpawnController::GetNextAvailableEnemySmoke() const
{
	CComponentEnemySmoke* pEnemySmoke = NULL;
	S32 iNumComponents = m_EnemySmokePool.size();
	for( S32 i = 0; i < iNumComponents; ++i )
	{
		pEnemySmoke = m_EnemySmokePool[i];
		if( pEnemySmoke && pEnemySmoke->IsSpawned() == false )
			return pEnemySmoke;
	}
	
	//printf( "No available Smoke enemies.\n" );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------