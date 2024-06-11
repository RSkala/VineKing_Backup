//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentSpawnController.h"

#include "ComponentAStarActor.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentEnemySpawner.h"
#include "ComponentGlobals.h"
#include "ComponentLevelBeginSequence.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const S32 g_iMaxEnemyBlobs = 10;
static const S32 g_iMaxEnemySerpents = 3;

static const S32 g_fStaggerSpawnTimeSeconds = 1.0f; // Number of time between spawns

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentSpawnController* CComponentSpawnController::sm_pInstance = NULL;

Vector<t2dAnimatedSprite*> CComponentSpawnController::m_EnemyBlobPool;
Vector<CComponentAStarActor*> CComponentSpawnController::m_EnemyBlobComponentPool;

Vector<t2dAnimatedSprite*> CComponentSpawnController::m_EnemySerpentPool;
Vector<CComponentAStarActor*> CComponentSpawnController::m_EnemySerpentComponentPool;

//static CComponentAStarActor* g_pLastSpawnedEnemy = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// CTimeSlotInfo

CTimeSlotInfo::CTimeSlotInfo()
	: m_bUseTimeSlot( false )
	, m_fSlotStartTimeSeconds( 0.0f) 
	, m_fStaggeredSpawnTimeSeconds( 0.0f )
	, m_iMaxNumSpawnsEnemyA( 0 )
	, m_iMaxNumSpawnsEnemyB( 0 )
	, m_iMaxNumSpawnsEnemyC( 0 )
	, m_iMaxTotalSpawns( 0 )
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// CComponentSpawnController

IMPLEMENT_CO_NETOBJECT_V1( CComponentSpawnController );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentSpawnController::CComponentSpawnController()
	: m_pOwner( NULL )
	, m_fTimeSlotTimer( 0.0f )
	, m_fStaggeredSpawnTimer( 0.0f )
	, m_iCurrentTimeSlotIndex( 0 )
	, m_iNumUsedTimeSlots( 0 )
	, m_bAtLastTimeSlot( false )
	, m_iCurrentEnemySpawnerAIndex( 0 )
	, m_iCurrentEnemySpawnerBIndex( 0 )
	, m_iCurrentEnemySpawnerCIndex( 0 )
	, m_iCurrentEnemyASpawns( 0 )
	, m_iCurrentEnemyBSpawns( 0 )
	, m_iCurrentEnemyCSpawns( 0 )
	, m_iNumSpawnsThisTimeSlot_A( 0 )
	, m_iNumSpawnsThisTimeSlot_B( 0 )
	, m_pszEnemyBlobTemplate( NULL )
	, m_pEnemyBlobTemplateObject( NULL )
	, m_pszEnemySerpentTemplate( NULL )
	, m_pEnemySerpentTemplateObject( NULL )
	, m_fBlobSpawnTimer( NULL )
	, m_fStaggeredBlobSpawnTimer( NULL )
	, m_fSerpentSpawnTimer( NULL )
	, m_fStaggeredSerpentSpawnTimer( NULL )
	, m_fNextTimeSlotStartTime( 0.0f )
	, m_fFinalTimeSlotTimeDifference( 0.0f )
	, m_bSpawnBlob( true )
{
	CComponentSpawnController::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentSpawnController::~CComponentSpawnController()
{
	CComponentSpawnController::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::AddEnemyASpawner( CComponentEnemySpawner* const _pComponent )
{
	if( _pComponent == NULL )
		return;
	
	m_EnemySpawnerAList.push_back( _pComponent );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::AddEnemyBSpawner( CComponentEnemySpawner* const _pComponent )
{
	if( _pComponent == NULL )
		return;
	
	m_EnemySpawnerBList.push_back( _pComponent );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::AddEnemyCSpawner( CComponentEnemySpawner* const _pComponent )
{
	if( _pComponent == NULL )
		return;
	
	m_EnemySpawnerCList.push_back( _pComponent );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSpawnController::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentSpawnController ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "BlobTemplate",		TypeString, Offset( m_pszEnemyBlobTemplate,		CComponentSpawnController ) );
	addField( "SerpentTemplate",	TypeString, Offset( m_pszEnemySerpentTemplate,	CComponentSpawnController ) );
	
	// Time Slot 1
	addField( "UseTimeSlot_1",		TypeBool,	Offset( m_TimeSlots[0].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_1",	TypeF32,	Offset( m_TimeSlots[0].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_1",		TypeF32,	Offset( m_TimeSlots[0].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_1",		TypeS32,	Offset( m_TimeSlots[0].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_1",		TypeS32,	Offset( m_TimeSlots[0].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_1",		TypeS32,	Offset( m_TimeSlots[0].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_1",	TypeS32,	Offset( m_TimeSlots[0].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 2
	addField( "UseTimeSlot_2",		TypeBool,	Offset( m_TimeSlots[1].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_2",	TypeF32,	Offset( m_TimeSlots[1].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_2",		TypeF32,	Offset( m_TimeSlots[1].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_2",		TypeS32,	Offset( m_TimeSlots[1].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_2",		TypeS32,	Offset( m_TimeSlots[1].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_2",		TypeS32,	Offset( m_TimeSlots[1].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_2",	TypeS32,	Offset( m_TimeSlots[1].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 3
	addField( "UseTimeSlot_3",		TypeBool,	Offset( m_TimeSlots[2].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_3",	TypeF32,	Offset( m_TimeSlots[2].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_3",		TypeF32,	Offset( m_TimeSlots[2].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_3",		TypeS32,	Offset( m_TimeSlots[2].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_3",		TypeS32,	Offset( m_TimeSlots[2].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_3",		TypeS32,	Offset( m_TimeSlots[2].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_3",	TypeS32,	Offset( m_TimeSlots[2].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 4
	addField( "UseTimeSlot_4",		TypeBool,	Offset( m_TimeSlots[3].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_4",	TypeF32,	Offset( m_TimeSlots[3].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_4",		TypeF32,	Offset( m_TimeSlots[3].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_4",		TypeS32,	Offset( m_TimeSlots[3].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_4",		TypeS32,	Offset( m_TimeSlots[3].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_4",		TypeS32,	Offset( m_TimeSlots[3].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_4",	TypeS32,	Offset( m_TimeSlots[3].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 5
	addField( "UseTimeSlot_5",		TypeBool,	Offset( m_TimeSlots[4].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_5",	TypeF32,	Offset( m_TimeSlots[4].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_5",		TypeF32,	Offset( m_TimeSlots[4].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_5",		TypeS32,	Offset( m_TimeSlots[4].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_5",		TypeS32,	Offset( m_TimeSlots[4].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_5",		TypeS32,	Offset( m_TimeSlots[4].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_5",	TypeS32,	Offset( m_TimeSlots[4].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 6
	addField( "UseTimeSlot_6",		TypeBool,	Offset( m_TimeSlots[5].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_6",	TypeF32,	Offset( m_TimeSlots[5].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_6",		TypeF32,	Offset( m_TimeSlots[5].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_6",		TypeS32,	Offset( m_TimeSlots[5].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_6",		TypeS32,	Offset( m_TimeSlots[5].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_6",		TypeS32,	Offset( m_TimeSlots[5].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_6",	TypeS32,	Offset( m_TimeSlots[5].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 7
	addField( "UseTimeSlot_7",		TypeBool,	Offset( m_TimeSlots[6].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_7",	TypeF32,	Offset( m_TimeSlots[6].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_7",		TypeF32,	Offset( m_TimeSlots[6].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_7",		TypeS32,	Offset( m_TimeSlots[6].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_7",		TypeS32,	Offset( m_TimeSlots[6].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_7",		TypeS32,	Offset( m_TimeSlots[6].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_7",	TypeS32,	Offset( m_TimeSlots[6].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 8
	addField( "UseTimeSlot_8",		TypeBool,	Offset( m_TimeSlots[7].m_bUseTimeSlot,				CComponentSpawnController ) );
	addField( "SlotStartTime_8",	TypeF32,	Offset( m_TimeSlots[7].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_8",		TypeF32,	Offset( m_TimeSlots[7].m_fStaggeredSpawnTimeSeconds, CComponentSpawnController ) );
	addField( "NumSpawnsA_8",		TypeS32,	Offset( m_TimeSlots[7].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_8",		TypeS32,	Offset( m_TimeSlots[7].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_8",		TypeS32,	Offset( m_TimeSlots[7].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_8",	TypeS32,	Offset( m_TimeSlots[7].m_iMaxTotalSpawns,			CComponentSpawnController ) );
	
	// Time Slot 9
	addField( "UseTimeSlot_9",		TypeBool,	Offset( m_TimeSlots[8].m_bUseTimeSlot,					CComponentSpawnController ) );
	addField( "SlotStartTime_9",	TypeF32,	Offset( m_TimeSlots[8].m_fSlotStartTimeSeconds,			CComponentSpawnController ) );
	addField( "StaggerTime_9",		TypeF32,	Offset( m_TimeSlots[8].m_fStaggeredSpawnTimeSeconds,	CComponentSpawnController ) );
	addField( "NumSpawnsA_9",		TypeS32,	Offset( m_TimeSlots[8].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_9",		TypeS32,	Offset( m_TimeSlots[8].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_9",		TypeS32,	Offset( m_TimeSlots[8].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_9",	TypeS32,	Offset( m_TimeSlots[8].m_iMaxTotalSpawns,				CComponentSpawnController ) );
	
	// Time Slot 10
	addField( "UseTimeSlot_10",		TypeBool,	Offset( m_TimeSlots[9].m_bUseTimeSlot,					CComponentSpawnController ) );
	addField( "SlotStartTime_10",	TypeF32,	Offset( m_TimeSlots[9].m_fSlotStartTimeSeconds,			CComponentSpawnController ) );
	addField( "StaggerTime_10",		TypeF32,	Offset( m_TimeSlots[9].m_fStaggeredSpawnTimeSeconds,	CComponentSpawnController ) );
	addField( "NumSpawnsA_10",		TypeS32,	Offset( m_TimeSlots[9].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_10",		TypeS32,	Offset( m_TimeSlots[9].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_10",	TypeS32,	Offset( m_TimeSlots[9].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_10",	TypeS32,	Offset( m_TimeSlots[9].m_iMaxTotalSpawns,				CComponentSpawnController ) );
	
	// Time Slot 11
	addField( "UseTimeSlot_11",		TypeBool,	Offset( m_TimeSlots[10].m_bUseTimeSlot,					CComponentSpawnController ) );
	addField( "SlotStartTime_11",	TypeF32,	Offset( m_TimeSlots[10].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_11",		TypeF32,	Offset( m_TimeSlots[10].m_fStaggeredSpawnTimeSeconds,	CComponentSpawnController ) );
	addField( "NumSpawnsA_11",		TypeS32,	Offset( m_TimeSlots[10].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_11",		TypeS32,	Offset( m_TimeSlots[10].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_11",	TypeS32,	Offset( m_TimeSlots[10].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_11",	TypeS32,	Offset( m_TimeSlots[10].m_iMaxTotalSpawns,				CComponentSpawnController ) );
	
	// Time Slot 12
	addField( "UseTimeSlot_12",		TypeBool,	Offset( m_TimeSlots[11].m_bUseTimeSlot,					CComponentSpawnController ) );
	addField( "SlotStartTime_12",	TypeF32,	Offset( m_TimeSlots[11].m_fSlotStartTimeSeconds,		CComponentSpawnController ) );
	addField( "StaggerTime_12",		TypeF32,	Offset( m_TimeSlots[11].m_fStaggeredSpawnTimeSeconds,	CComponentSpawnController ) );
	addField( "NumSpawnsA_12",		TypeS32,	Offset( m_TimeSlots[11].m_iMaxNumSpawnsEnemyA,			CComponentSpawnController ) );
	addField( "NumSpawnsB_12",		TypeS32,	Offset( m_TimeSlots[11].m_iMaxNumSpawnsEnemyB,			CComponentSpawnController ) );
	//addField( "NumSpawnsC_12",	TypeS32,	Offset( m_TimeSlots[11].m_iMaxNumSpawnsEnemyC,			CComponentSpawnController ) );
	//addField( "TotalSpawns_12",	TypeS32,	Offset( m_TimeSlots[11].m_iMaxTotalSpawns,				CComponentSpawnController ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentSpawnController::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentSpawnController::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	//CComponentGlobals::GetInstance().SetSpawnControllerComponent( this );
	
	CComponentSpawnController::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLevelInitializationComplete() == false )
		return;
	
	if( CComponentLevelBeginSequence::GetInstance().HasLevelBeginSequenceFinished() == false )
		return;
	
	//if( g_bSpawningEnabled == false )
	if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
		return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
		return;
	
	UpdateSpawning();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::OnPostInit()
{	
	m_fTimeSlotTimer = 0.0f;
	
	for( U32 i = 0; i < m_uMaxTimeSlots; ++i )
	{
		if( m_TimeSlots[i].m_bUseTimeSlot )
			m_iNumUsedTimeSlots++;
	}
	
	
	
	m_iCurrentEnemySpawnerAIndex = m_iCurrentEnemySpawnerBIndex = m_iCurrentEnemySpawnerCIndex = 0;
	
	m_pEnemyBlobTemplateObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( m_pszEnemyBlobTemplate ) );
	if( m_pEnemyBlobTemplateObject == NULL )
		printf( "Enemy Blob template object not found.\n" );
	
	m_pEnemySerpentTemplateObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( m_pszEnemySerpentTemplate ) );
	if( m_pEnemySerpentTemplateObject == NULL )
		printf( "Enemy Serpent template object not found.\n" );
	
	CComponentSpawnController::InitializeEnemyPools();
	
	m_iCurrentEnemySpawnerAIndex = 0;
	m_iCurrentEnemySpawnerBIndex = 0;
	m_iCurrentTimeSlotIndex = -1;
	m_fStaggeredSpawnTimer = 0.0f;
	m_fNextTimeSlotStartTime = m_TimeSlots[0].m_fSlotStartTimeSeconds;
	
	if( m_iNumUsedTimeSlots >= 2 )
		m_fFinalTimeSlotTimeDifference = m_TimeSlots[m_iNumUsedTimeSlots - 1].m_fSlotStartTimeSeconds - m_TimeSlots[m_iNumUsedTimeSlots - 2].m_fSlotStartTimeSeconds;
	else
		m_fFinalTimeSlotTimeDifference = m_TimeSlots[0].m_fSlotStartTimeSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

/*static*/ void CComponentSpawnController::InitializeEnemyPools()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set up Enemy Blob pools
	CComponentSpawnController::m_EnemyBlobPool.clear();
	CComponentSpawnController::m_EnemyBlobComponentPool.clear();
	
	t2dAnimatedSprite* pObjectToClone = GetInstance().GetEnemyBlobTemplateObject();
	if( pObjectToClone == NULL )
		return;
	
	pObjectToClone->setVisible( false );
	
	m_EnemyBlobPool.push_back( pObjectToClone );
	
	CComponentAStarActor* pEnemyBlobComponent = static_cast<CComponentAStarActor*>( pObjectToClone->getComponent( 0 ) );
	if( pEnemyBlobComponent )
		m_EnemyBlobComponentPool.push_back( pEnemyBlobComponent );
	
	const char* pszOriginalName = pObjectToClone->getName();
	char szName[32];
	
	t2dAnimatedSprite* pClonedObject = NULL;
	for( S32 i = 1; i < g_iMaxEnemyBlobs; ++i )
	{
		pClonedObject = static_cast<t2dAnimatedSprite*>( pObjectToClone->clone() );
		if( pClonedObject )
		{
			m_EnemyBlobPool.push_back( pClonedObject );
			pEnemyBlobComponent = static_cast<CComponentAStarActor*>( pClonedObject->getComponent( 0 ) );
			if( pEnemyBlobComponent )
				m_EnemyBlobComponentPool.push_back( pEnemyBlobComponent );
			
			sprintf( szName, "%s_%d", pszOriginalName, i );
			pClonedObject->assignName( szName );
			pClonedObject->setVisible( false );
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set up Enemy Serpent pools
	CComponentSpawnController::m_EnemySerpentPool.clear();
	CComponentSpawnController::m_EnemySerpentComponentPool.clear();
	
	pObjectToClone = GetInstance().GetEnemySerpentTemplateObject();
	
	m_EnemySerpentPool.push_back( pObjectToClone );
	if( pObjectToClone == NULL )
		return;
	
	pObjectToClone->setVisible( false );
	
	CComponentAStarActor* pEnemySerpentComponent = static_cast<CComponentAStarActor*>( pObjectToClone->getComponent( 0 ) );
	if( pEnemySerpentComponent )
		m_EnemySerpentComponentPool.push_back( pEnemySerpentComponent );
	
	pszOriginalName = pObjectToClone->getName();
	//char szName[32];
	
	pClonedObject = NULL;
	for( S32 i = 1; i < g_iMaxEnemySerpents; ++i )
	{
		pClonedObject = static_cast<t2dAnimatedSprite*>( pObjectToClone->clone() );
		if( pClonedObject )
		{
			m_EnemySerpentPool.push_back( pClonedObject );
			pEnemySerpentComponent = static_cast<CComponentAStarActor*>( pClonedObject->getComponent( 0 ) );
			if( pEnemySerpentComponent )
				m_EnemySerpentComponentPool.push_back( pEnemySerpentComponent );
			
			sprintf( szName, "%s_%d", pszOriginalName, i );
			pClonedObject->assignName( szName );
			pClonedObject->setVisible( false );
		}
	}
		
	//printf( "Added %d Blob enemies\n",			m_EnemyBlobPool.size() );
	//printf( "Added %d Blob components\n",		m_EnemyBlobComponentPool.size() );
	
	//printf( "Added %d Serpent enemies\n",		m_EnemySerpentPool.size() );
	//printf( "Added %d Serpent components\n",	m_EnemySerpentComponentPool.size() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::UpdateSpawning()
{
	if( m_pOwner == NULL )
		return;
	
	// Make sure that no spawning occurs until it is supposed to (that is, not until Time Slot 0 is supposed to start)
	if( m_iCurrentTimeSlotIndex == -1 )
	{
		m_fTimeSlotTimer += ITickable::smTickSec;
		if( m_fTimeSlotTimer < m_fNextTimeSlotStartTime )
			return;
		
		m_iCurrentTimeSlotIndex = 0;
		m_fNextTimeSlotStartTime = m_TimeSlots[1].m_fSlotStartTimeSeconds;
	}
	
	if( m_iCurrentTimeSlotIndex == -1 ) // This shouldn't ever happen, but catch it just in case
	{
		printf( "WARNING: CComponentEnemyManaEaterSpawner::UpdateSpawning - m_iCurrentTimeSlotIndex == -1\n" );
		return;
	}
	
	// Handle the staggered spawning for the current time slot -- Spawn either a Blob or a Serpent
	if( m_bSpawnBlob )
	{
		if( m_iNumSpawnsThisTimeSlot_A < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawnsEnemyA )
		{
			m_fStaggeredSpawnTimer -= ITickable::smTickSec;
			if( m_fStaggeredSpawnTimer <= 0.0f )
			{
				HandleBlobSpawning();
				m_fStaggeredSpawnTimer = m_TimeSlots[m_iCurrentTimeSlotIndex].m_fStaggeredSpawnTimeSeconds;
			}
		}
	}
	else
	{
		if( m_iNumSpawnsThisTimeSlot_B < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawnsEnemyB )
		{
			m_fStaggeredSpawnTimer -= ITickable::smTickSec;
			if( m_fStaggeredSpawnTimer <= 0.0f )
			{
				HandleSerpentSpawning();
				m_fStaggeredSpawnTimer = m_TimeSlots[m_iCurrentTimeSlotIndex].m_fStaggeredSpawnTimeSeconds;
			}
		}
	}
	
	CheckTimeSlotIncrement();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::HandleBlobSpawning()
{
	CComponentEnemySpawner* pSpawner = m_EnemySpawnerAList[m_iCurrentEnemySpawnerAIndex];
	if( pSpawner )
	{
		CComponentAStarActor* pEnemyComponentToSpawn = GetNextAvailableBlobEnemy();
		if( pEnemyComponentToSpawn )
		{
			pSpawner->SpawnEnemy( pEnemyComponentToSpawn );
			
			++m_iCurrentEnemySpawnerAIndex;
			if( m_iCurrentEnemySpawnerAIndex >= m_EnemySpawnerAList.size() )
				m_iCurrentEnemySpawnerAIndex = 0;
			
			CComponentAStarActor::SetLastSpawnedWaveEnemy( pEnemyComponentToSpawn );
		}
	}

	m_iNumSpawnsThisTimeSlot_A++;
	
	if( m_iNumSpawnsThisTimeSlot_B < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawnsEnemyB )
	{
		// Make sure the other enemy type can be spawned before switching
		m_bSpawnBlob = false;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::HandleSerpentSpawning()
{
	CComponentEnemySpawner* pSpawner = GetNextAvailableSerpentSpawner();
	if( pSpawner )
	{
		CComponentAStarActor* pEnemyComponentToSpawn = GetNextAvailableSerpentEnemy();
		if( pEnemyComponentToSpawn )
		{
			pSpawner->SpawnEnemy( pEnemyComponentToSpawn );
			
			++m_iCurrentEnemySpawnerBIndex;
			if( m_iCurrentEnemySpawnerBIndex >= m_EnemySpawnerBList.size() )
				m_iCurrentEnemySpawnerBIndex = 0;
			
			CComponentAStarActor::SetLastSpawnedWaveEnemy( pEnemyComponentToSpawn );
		}
	}

	// If pSpawner == NULL then:
	// All the current Serpent spawners have enemies. We cannot use any of them because the Serpents are stationary, and to use them would cause Serpents to spawn on top of each other.
	
	m_iNumSpawnsThisTimeSlot_B++;
	
	if( m_iNumSpawnsThisTimeSlot_A < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawnsEnemyA )
	{
		// Make sure the other enemy type can be spawned before switching
		m_bSpawnBlob = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::CheckTimeSlotIncrement()
{
	// If not in last time slot, count UP.  If in last time slot, count DOWN.
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
		
		m_fTimeSlotTimer = m_fFinalTimeSlotTimeDifference;
		m_iNumSpawnsThisTimeSlot_A = 0;
		m_iNumSpawnsThisTimeSlot_B = 0;
	}
	
	// Increase the Time Slot Index
	if( m_bAtLastTimeSlot == false )
	{
		bool bAtLastTimeSlot = false;
		if( m_iCurrentTimeSlotIndex != m_uMaxTimeSlots - 1 ) // Make sure that the current Time Slot is not the ACTUAL last Time Slot
		{
			S32 iNextTimeSlot = m_iCurrentTimeSlotIndex + 1;
			if( m_TimeSlots[iNextTimeSlot].m_bUseTimeSlot ) // Can the upcoming Time Slot be used?
			{
				// Move to the next Time SLot
				m_iCurrentTimeSlotIndex++;
				
				iNextTimeSlot = m_iCurrentTimeSlotIndex + 1;
				if( (iNextTimeSlot >= m_uMaxTimeSlots - 1) || (m_TimeSlots[iNextTimeSlot].m_bUseTimeSlot) )
				{
					m_fNextTimeSlotStartTime = m_TimeSlots[iNextTimeSlot].m_fSlotStartTimeSeconds;
					
					// When changing the Time Slot, reset the staggered spawn timer and the number of enemies spawned for the Time Slot
					m_fStaggeredSpawnTimer = 0.0f;
					m_iNumSpawnsThisTimeSlot_A = 0;
					m_iNumSpawnsThisTimeSlot_B = 0;
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
		
		// Is the current Time Slot the last one?
		if( bAtLastTimeSlot )
		{
			// Now that we are in the last Time Slot, the count will go backwards.
			m_bAtLastTimeSlot = true;
			m_fTimeSlotTimer = m_fFinalTimeSlotTimeDifference;
			
			// When changing the time slot, reset the staggered spawn timer and the number of enemies spawned
			m_fStaggeredSpawnTimer = 0.0f;
			m_iNumSpawnsThisTimeSlot_A = 0;
			m_iNumSpawnsThisTimeSlot_B = 0;
		}
	}
	
	AssertFatal( m_iCurrentTimeSlotIndex < m_uMaxTimeSlots, "CComponentSpawnController::UpdateSpawning - Time Slot index out of bounds\n" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentAStarActor* CComponentSpawnController::GetNextAvailableBlobEnemy()
{
	CComponentAStarActor* pEnemyComponent = NULL;
	S32 iNumEnemyBlobs = m_EnemyBlobComponentPool.size();
	for( S32 i = 0; i < iNumEnemyBlobs; ++i )
	{
		pEnemyComponent = m_EnemyBlobComponentPool[i];
		if( pEnemyComponent && pEnemyComponent->IsSpawned() == false )
			return pEnemyComponent;
	}
	
	//printf( "GetNextAvailableBlobEnemy() - No available Blob enemies!\n" );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentAStarActor* CComponentSpawnController::GetNextAvailableSerpentEnemy()
{
	CComponentAStarActor* pEnemyComponent = NULL;
	S32 iNumEnemyBlobs = m_EnemySerpentComponentPool.size();
	for( S32 i = 0; i < iNumEnemyBlobs; ++i )
	{
		pEnemyComponent = m_EnemySerpentComponentPool[i];
		if( pEnemyComponent && pEnemyComponent->IsSpawned() == false )
			return pEnemyComponent;
	}
	
	//printf( "GetNextAvailableSerpentEnemy() - No available Serpent enemies!\n" );
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawner* CComponentSpawnController::GetNextAvailableSerpentSpawner() // RKS TODO: Create two lists -- one with spawned enemies and unspawned enemies
{
	CComponentEnemySpawner* pSpawner = NULL;
	S32 iNumSpawners = m_EnemySpawnerBList.size();
	for( S32 i = 0; i < iNumSpawners; ++i )
	{
		pSpawner = m_EnemySpawnerBList[i];
		if( pSpawner && pSpawner->IsSpawnedEnemyAlive() == false )
		{
			return pSpawner;
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::UpdateBlobSpawning()
{
	m_fBlobSpawnTimer += ITickable::smTickSec;
	
	m_fStaggeredBlobSpawnTimer -= ITickable::smTickSec;
	if( m_fStaggeredBlobSpawnTimer > 0.0f )
		return;
	
	m_fStaggeredBlobSpawnTimer = 0.0f;
	
	if( m_iCurrentEnemyASpawns < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawnsEnemyA )
	{
		HandleBlobSpawning();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSpawnController::UpdateSerpentSpawning()
{
	m_fStaggeredSerpentSpawnTimer -= ITickable::smTickSec;
	if( m_fStaggeredSerpentSpawnTimer > 0.0f )
		return;
	
	m_fStaggeredSerpentSpawnTimer = 0.0f;
	
	if( m_iCurrentEnemyBSpawns < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawnsEnemyB )
	{
		HandleSerpentSpawning();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
