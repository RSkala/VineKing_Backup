//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemyManaEaterSpawner.h"

#include "ComponentGlobals.h"
#include "ComponentEndLevelScreen.h"
#include "ComponentEnemyManaEater.h"
#include "ComponentLevelBeginSequence.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const U32 g_uMaxNumManaEaterEnemies = 10;
static const F32 g_fSpawnTimeMS = 4000.0f;

//static const bool g_bSpawningEnabled = true;

static const char g_szManaEaterTemplateObject[32] = "Enemy_ManaEater";

static const char g_szSmokeHitEffectObject[32] = "Smoke_HitFX_Object";
static const char g_szSmokeHitAnimName[32] = "smoke_hit_fxAnimation";
static const S32 g_iNumSmokeHitEffectObjects = 4;
static t2dAnimatedSprite* g_pTempSmokeHitEffect = NULL;

static const t2dVector g_vSafeSmokeHitEffectPosition( 750.0f, 750.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<t2dAnimatedSprite*> CComponentEnemyManaEaterSpawner::m_EnemyPool;
Vector<CComponentEnemyManaEater*> CComponentEnemyManaEaterSpawner::m_EnemyComponentPool;

CComponentEnemyManaEaterSpawner* CComponentEnemyManaEaterSpawner::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemyManaEaterSpawner );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyManaEaterSpawner::CComponentEnemyManaEaterSpawner()
	: m_pOwner( NULL )
	, m_fTimeSlotTimer( 0.0f )
	, m_iNumEnemiesSpawnedThisTimeSlot( 0 )
	, m_fStaggeredSpawnTimer( 0.0f )
	, m_iCurrentTimeSlotIndex( 0 )
	, m_iNumUsedTimeSlots( 0 )
	, m_bAtLastTimeSlot( false )
	, m_fNextTimeSlotStartTime( 0.0f )
	, m_fFinalTimeSlotTimeDifference( 0.0f )
	, m_iCurrentSmokeHitEffectIndex( 0 )
{
	CComponentEnemyManaEaterSpawner::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyManaEaterSpawner::~CComponentEnemyManaEaterSpawner()
{
	m_EnemyPool.clear();
	m_EnemyComponentPool.clear();
	
	CComponentEnemyManaEaterSpawner::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyManaEaterSpawner::Initialize()
{
	InitializeEnemyPool();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyManaEaterSpawner::InitializeEnemyPool()
{
	m_EnemyPool.clear();
	m_EnemyComponentPool.clear();
	
	t2dAnimatedSprite* pObjectToClone = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szManaEaterTemplateObject ) );
	if( pObjectToClone == NULL )
	{
		printf( "%s - template object not found\n", __FUNCTION__ );
		return;
	}
	
	if( pObjectToClone->getComponentCount() <= 0 )
	{
		printf( "%s - No components on the Mana Eater Object template\n", __FUNCTION__ ); // RKS FUCK: This is a hack to fix a crash on restart! FIX THIS!
		return;
	}
	
	m_EnemyPool.push_back( pObjectToClone );
	
	CComponentEnemyManaEater* pEnemyComponent = static_cast<CComponentEnemyManaEater*>( pObjectToClone->getComponent( 0 ) );
	if( pEnemyComponent )
		m_EnemyComponentPool.push_back( pEnemyComponent );
	
	const char* pszOriginalName = pObjectToClone->getName();
	char szName[32];
	
	t2dAnimatedSprite* pClonedObject = NULL;
	for( U32 i = 1; i < g_uMaxNumManaEaterEnemies; ++i )
	{
		pClonedObject = static_cast<t2dAnimatedSprite*>( pObjectToClone->clone() );
		if( pClonedObject )
		{
			m_EnemyPool.push_back( pClonedObject );
			
			pEnemyComponent = static_cast<CComponentEnemyManaEater*>( pClonedObject->getComponent( 0 ) );
			if( pEnemyComponent )
				m_EnemyComponentPool.push_back( pEnemyComponent );
			
			sprintf( szName, "%s_%d", pszOriginalName, i );
			pClonedObject->assignName( szName );
		}
	}
	
	//printf( "Added %d Mana Eater enemies\n", m_EnemyPool.size() );
	//printf( "Added %d Mana Eater components\n", m_EnemyComponentPool.size() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyManaEaterSpawner::PlaySmokeHitEffectAtPosition( const t2dVector& _vPosition )
{
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_SmokeHitEffectList[sm_pInstance->m_iCurrentSmokeHitEffectIndex] )
	{
		sm_pInstance->m_SmokeHitEffectList[sm_pInstance->m_iCurrentSmokeHitEffectIndex]->setVisible( true );
		sm_pInstance->m_SmokeHitEffectList[sm_pInstance->m_iCurrentSmokeHitEffectIndex]->setPosition( _vPosition );
		sm_pInstance->m_SmokeHitEffectList[sm_pInstance->m_iCurrentSmokeHitEffectIndex]->playAnimation( g_szSmokeHitAnimName, false );
		
		sm_pInstance->m_iCurrentSmokeHitEffectIndex++;
		if( sm_pInstance->m_iCurrentSmokeHitEffectIndex >= sm_pInstance->m_SmokeHitEffectList.size() )
			sm_pInstance->m_iCurrentSmokeHitEffectIndex = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemyManaEaterSpawner::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemyManaEaterSpawner ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	// Use spawn anim (for Boss levels)
	addField( "UseSpawnAnim",		TypeBool,	Offset( m_bUseSpawnAnim, CComponentEnemyManaEaterSpawner ) );

	// Movement Speed
	addField( "MoveSpeedNorm",	TypeF32,	Offset( m_fMovementSpeedNormal,		 CComponentEnemyManaEaterSpawner ) );
	addField( "MoveSpeedSuper",	TypeF32,	Offset( m_fMovementSpeedSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Attack Power
	addField( "AttackPowNorm",	TypeF32,	Offset( m_fAttackPowerNormal,		 CComponentEnemyManaEaterSpawner ) );
	addField( "AttackPowSuper",	TypeF32,	Offset( m_fAttackPowerSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 1
	addField( "UseTimeSlot_1",		TypeBool,	Offset( m_TimeSlots[0].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_1",	TypeF32,	Offset( m_TimeSlots[0].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_1",		TypeF32,	Offset( m_TimeSlots[0].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_1",		TypeS32,	Offset( m_TimeSlots[0].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_1",		TypeS32,	Offset( m_TimeSlots[0].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 2
	addField( "UseTimeSlot_2",		TypeBool,	Offset( m_TimeSlots[1].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_2",	TypeF32,	Offset( m_TimeSlots[1].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_2",		TypeF32,	Offset( m_TimeSlots[1].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_2",		TypeS32,	Offset( m_TimeSlots[1].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_2",		TypeS32,	Offset( m_TimeSlots[1].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 3
	addField( "UseTimeSlot_3",		TypeBool,	Offset( m_TimeSlots[2].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_3",	TypeF32,	Offset( m_TimeSlots[2].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_3",		TypeF32,	Offset( m_TimeSlots[2].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_3",		TypeS32,	Offset( m_TimeSlots[2].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_3",		TypeS32,	Offset( m_TimeSlots[2].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 4
	addField( "UseTimeSlot_4",		TypeBool,	Offset( m_TimeSlots[3].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_4",	TypeF32,	Offset( m_TimeSlots[3].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_4",		TypeF32,	Offset( m_TimeSlots[3].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_4",		TypeS32,	Offset( m_TimeSlots[3].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_4",		TypeS32,	Offset( m_TimeSlots[3].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 5
	addField( "UseTimeSlot_5",		TypeBool,	Offset( m_TimeSlots[4].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_5",	TypeF32,	Offset( m_TimeSlots[4].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_5",		TypeF32,	Offset( m_TimeSlots[4].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_5",		TypeS32,	Offset( m_TimeSlots[4].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_5",		TypeS32,	Offset( m_TimeSlots[4].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 6
	addField( "UseTimeSlot_6",		TypeBool,	Offset( m_TimeSlots[5].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_6",	TypeF32,	Offset( m_TimeSlots[5].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_6",		TypeF32,	Offset( m_TimeSlots[5].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_6",		TypeS32,	Offset( m_TimeSlots[5].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_6",		TypeS32,	Offset( m_TimeSlots[5].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 7
	addField( "UseTimeSlot_7",		TypeBool,	Offset( m_TimeSlots[6].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_7",	TypeF32,	Offset( m_TimeSlots[6].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_7",		TypeF32,	Offset( m_TimeSlots[6].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_7",		TypeS32,	Offset( m_TimeSlots[6].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_7",		TypeS32,	Offset( m_TimeSlots[6].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 8
	addField( "UseTimeSlot_8",		TypeBool,	Offset( m_TimeSlots[7].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_8",	TypeF32,	Offset( m_TimeSlots[7].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_8",		TypeF32,	Offset( m_TimeSlots[7].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_8",		TypeS32,	Offset( m_TimeSlots[7].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_8",		TypeS32,	Offset( m_TimeSlots[7].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 9
	addField( "UseTimeSlot_9",		TypeBool,	Offset( m_TimeSlots[8].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_9",	TypeF32,	Offset( m_TimeSlots[8].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_9",		TypeF32,	Offset( m_TimeSlots[8].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_9",		TypeS32,	Offset( m_TimeSlots[8].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_9",		TypeS32,	Offset( m_TimeSlots[8].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 10
	addField( "UseTimeSlot_10",		TypeBool,	Offset( m_TimeSlots[9].m_bUseTimeSlot,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_10",	TypeF32,	Offset( m_TimeSlots[9].m_fSlotStartTimeSeconds,		 CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_10",		TypeF32,	Offset( m_TimeSlots[9].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_10",		TypeS32,	Offset( m_TimeSlots[9].m_iMaxNumSpawns,				 CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_10",		TypeS32,	Offset( m_TimeSlots[9].m_iPercentChanceSuper,		 CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 11
	addField( "UseTimeSlot_11",		TypeBool,	Offset( m_TimeSlots[10].m_bUseTimeSlot,				  CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_11",	TypeF32,	Offset( m_TimeSlots[10].m_fSlotStartTimeSeconds,	  CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_11",		TypeF32,	Offset( m_TimeSlots[10].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_11",		TypeS32,	Offset( m_TimeSlots[10].m_iMaxNumSpawns,			  CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_11",		TypeS32,	Offset( m_TimeSlots[10].m_iPercentChanceSuper,		  CComponentEnemyManaEaterSpawner ) );
	
	// Time Slot 12
	addField( "UseTimeSlot_12",		TypeBool,	Offset( m_TimeSlots[11].m_bUseTimeSlot,				  CComponentEnemyManaEaterSpawner ) );
	addField( "SlotStartTime_12",	TypeF32,	Offset( m_TimeSlots[11].m_fSlotStartTimeSeconds,	  CComponentEnemyManaEaterSpawner ) );
	addField( "StaggerTime_12",		TypeF32,	Offset( m_TimeSlots[11].m_fStaggeredSpawnTimeSeconds, CComponentEnemyManaEaterSpawner ) );
	addField( "NumSpawns_12",		TypeS32,	Offset( m_TimeSlots[11].m_iMaxNumSpawns,			  CComponentEnemyManaEaterSpawner ) );
	addField( "SuperChance_12",		TypeS32,	Offset( m_TimeSlots[11].m_iPercentChanceSuper,		  CComponentEnemyManaEaterSpawner ) );
	
	addField( "SpawnPosition01", TypeF32Vector, Offset( m_vSpawnPosition01, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition02", TypeF32Vector, Offset( m_vSpawnPosition02, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition03", TypeF32Vector, Offset( m_vSpawnPosition03, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition04", TypeF32Vector, Offset( m_vSpawnPosition04, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition05", TypeF32Vector, Offset( m_vSpawnPosition05, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition06", TypeF32Vector, Offset( m_vSpawnPosition06, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition07", TypeF32Vector, Offset( m_vSpawnPosition07, CComponentEnemyManaEaterSpawner ) );
	addField( "SpawnPosition08", TypeF32Vector, Offset( m_vSpawnPosition08, CComponentEnemyManaEaterSpawner ) );
	
	addField( "EndPosition01", TypeF32Vector, Offset( m_vEndPosition01, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition02", TypeF32Vector, Offset( m_vEndPosition02, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition03", TypeF32Vector, Offset( m_vEndPosition03, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition04", TypeF32Vector, Offset( m_vEndPosition04, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition05", TypeF32Vector, Offset( m_vEndPosition05, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition06", TypeF32Vector, Offset( m_vEndPosition06, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition07", TypeF32Vector, Offset( m_vEndPosition07, CComponentEnemyManaEaterSpawner ) );
	addField( "EndPosition08", TypeF32Vector, Offset( m_vEndPosition08, CComponentEnemyManaEaterSpawner ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemyManaEaterSpawner::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemyManaEaterSpawner::onComponentAdd - Must be added to a t2dAnimatedSprite." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	CComponentEnemyManaEaterSpawner::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEaterSpawner::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
		return;
	
	if( CComponentGlobals::GetInstance().IsLevelInitializationComplete() == false )
		return;
	
	if( CComponentEndLevelScreen::GetInstance().HasLevelEnded() )
		return;
	
	if( CComponentLevelBeginSequence::GetInstance().HasLevelBeginSequenceFinished() == false )
		return;
	
	UpdateSpawning();
	
	// Update the Smoke Hit Effects
	for( S32 i = 0; i < m_SmokeHitEffectList.size(); ++i )
	{
		g_pTempSmokeHitEffect = m_SmokeHitEffectList[i];
		if( g_pTempSmokeHitEffect && g_pTempSmokeHitEffect->getVisible() && g_pTempSmokeHitEffect->mAnimationController.isAnimationFinished() )
		{
			g_pTempSmokeHitEffect->setVisible( false );
			g_pTempSmokeHitEffect->setPosition( g_vSafeSmokeHitEffectPosition );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEaterSpawner::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
		
	// Create the Smoke Hit Effects
	t2dAnimatedSprite* pSmokeHitEffect = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szSmokeHitEffectObject ) );
	if( pSmokeHitEffect )
	{
		m_SmokeHitEffectList.push_back( pSmokeHitEffect );
		
		char szName[32];
		t2dAnimatedSprite* pNewSmokeHitEffect = NULL;
		
		for( S32 i = 1; i < g_iNumSmokeHitEffectObjects; ++i )
		{
			pNewSmokeHitEffect = static_cast<t2dAnimatedSprite*>( pSmokeHitEffect->clone() );
			if( pNewSmokeHitEffect )
			{
				sprintf( szName, "%s_%d", g_szSmokeHitEffectObject, i );
				pNewSmokeHitEffect->assignName( szName );
				m_SmokeHitEffectList.push_back( pNewSmokeHitEffect );
			}
		}
	}
	g_pTempSmokeHitEffect = NULL;
	m_iCurrentSmokeHitEffectIndex = 0;
	
	m_SpawnPositionList.clear();
	m_EndPositionList.clear();
	
	t2dVector vPosition( 0.0f, 0.0f );
	
	// Start Positions
	S32 iSize = m_vSpawnPosition01.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition01: (%f, %f): \n", m_vSpawnPosition01[0], m_vSpawnPosition01[1] );
		vPosition.mX = m_vSpawnPosition01[0];
		vPosition.mY = m_vSpawnPosition01[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition02.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition02: (%f, %f): \n", m_vSpawnPosition02[0], m_vSpawnPosition02[1] );
		vPosition.mX = m_vSpawnPosition02[0];
		vPosition.mY = m_vSpawnPosition02[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition03.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition03: (%f, %f): \n", m_vSpawnPosition03[0], m_vSpawnPosition03[1] );
		vPosition.mX = m_vSpawnPosition03[0];
		vPosition.mY = m_vSpawnPosition03[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition04.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition04: (%f, %f): \n", m_vSpawnPosition04[0], m_vSpawnPosition04[1] );
		vPosition.mX = m_vSpawnPosition04[0];
		vPosition.mY = m_vSpawnPosition04[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition05.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition05: (%f, %f): \n", m_vSpawnPosition05[0], m_vSpawnPosition05[1] );
		vPosition.mX = m_vSpawnPosition05[0];
		vPosition.mY = m_vSpawnPosition05[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition06.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition06: (%f, %f): \n", m_vSpawnPosition06[0], m_vSpawnPosition06[1] );
		vPosition.mX = m_vSpawnPosition06[0];
		vPosition.mY = m_vSpawnPosition06[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition07.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition07: (%f, %f): \n", m_vSpawnPosition07[0], m_vSpawnPosition07[1] );
		vPosition.mX = m_vSpawnPosition07[0];
		vPosition.mY = m_vSpawnPosition07[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	iSize = m_vSpawnPosition08.size();
	if( iSize  == 2 )
	{
		//printf( "m_vSpawnPosition08: (%f, %f): \n", m_vSpawnPosition08[0], m_vSpawnPosition08[1] );
		vPosition.mX = m_vSpawnPosition08[0];
		vPosition.mY = m_vSpawnPosition08[1];
		m_SpawnPositionList.push_back( vPosition );
	}
	
	// End Positions
	iSize = m_vEndPosition01.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition01[0];
		vPosition.mY = m_vEndPosition01[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition02.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition02[0];
		vPosition.mY = m_vEndPosition02[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition03.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition03[0];
		vPosition.mY = m_vEndPosition03[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition04.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition04[0];
		vPosition.mY = m_vEndPosition04[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition05.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition05[0];
		vPosition.mY = m_vEndPosition05[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition06.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition06[0];
		vPosition.mY = m_vEndPosition06[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition07.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition07[0];
		vPosition.mY = m_vEndPosition07[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	iSize = m_vEndPosition08.size();
	if( iSize == 2 )
	{
		vPosition.mX = m_vEndPosition08[0];
		vPosition.mY = m_vEndPosition08[1];
		m_EndPositionList.push_back( vPosition );
	}
	
	
	if( m_SpawnPositionList.size() != m_EndPositionList.size() )
		printf( "WARNING: Number of Smoke Monster Spawn Positions(%d) does not match the number of End Positions(%d).\n", m_SpawnPositionList.size(), m_EndPositionList.size() );
	
	m_fTimeSlotTimer = 0.0f;
	for( S32 i = 0; i < m_uMaxTimeSlots; ++i )
	{
		if( m_TimeSlots[i].m_bUseTimeSlot )
			m_iNumUsedTimeSlots++;
	}
	
	m_iCurrentSpawnPositionIndex = 0;
	m_iCurrentTimeSlotIndex = -1;
	m_fStaggeredSpawnTimer = 0.0f;
	
	m_fNextTimeSlotStartTime = m_TimeSlots[0].m_fSlotStartTimeSeconds;
	
	if( m_iNumUsedTimeSlots >= 2 )
	{
		//m_fNextTimeSlotStartTime = m_TimeSlots[1].m_fSlotStartTimeSeconds;
		m_fFinalTimeSlotTimeDifference = m_TimeSlots[m_iNumUsedTimeSlots - 1].m_fSlotStartTimeSeconds - m_TimeSlots[m_iNumUsedTimeSlots - 2].m_fSlotStartTimeSeconds;
		//printf( "m_fNextTimeSlotStartTime: %f\n", m_fNextTimeSlotStartTime );
		//printf( "m_fFinalTimeSlotTimeDifference: %f\n", m_fFinalTimeSlotTimeDifference );
	}
	else
	{
		m_fFinalTimeSlotTimeDifference = m_TimeSlots[0].m_fSlotStartTimeSeconds;
	}
	
	
	//printf( "m_iCurrentTimeSlotIndex: %d\n", m_iCurrentTimeSlotIndex );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private member functions

void CComponentEnemyManaEaterSpawner::UpdateSpawning()
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
	
	// Handle the staggered spawning for the current time slot
	if( m_iNumEnemiesSpawnedThisTimeSlot < m_TimeSlots[m_iCurrentTimeSlotIndex].m_iMaxNumSpawns )
	{
		m_fStaggeredSpawnTimer -= ITickable::smTickSec;
		if( m_fStaggeredSpawnTimer <= 0.0f )
		{
			CComponentEnemyManaEater* pEnemyComponent = GetNextAvailableEnemy();
			if( pEnemyComponent )
			{
				// Spawn enemy - Check the chance that a Super enemy might spawn
				S32 iRandom = CComponentGlobals::GetInstance().GetRandomRange( 0, 100 );
				if( iRandom <= m_TimeSlots[m_iCurrentTimeSlotIndex].m_iPercentChanceSuper )
				{
					//printf( "Spawning SUPER SMOKE at Pos(%f, %f), Rand = %d\n", m_SpawnPositionList[m_iCurrentSpawnPositionIndex].mX, m_SpawnPositionList[m_iCurrentSpawnPositionIndex].mY, iRandom );
					pEnemyComponent->SpawnSuperAtPositionWithEndpoint( m_SpawnPositionList[m_iCurrentSpawnPositionIndex], m_EndPositionList[m_iCurrentSpawnPositionIndex] );
				}
				else
				{
					//printf( "Spawning NORMAL SMOKE at Pos(%f, %f), Rand = %d\n", m_SpawnPositionList[m_iCurrentSpawnPositionIndex].mX, m_SpawnPositionList[m_iCurrentSpawnPositionIndex].mY, iRandom );
					pEnemyComponent->SpawnAtPositionWithEndpoint( m_SpawnPositionList[m_iCurrentSpawnPositionIndex], m_EndPositionList[m_iCurrentSpawnPositionIndex] );
				}
				
				m_iNumEnemiesSpawnedThisTimeSlot++;
				
				//printf( "m_iNumEnemiesSpawnedThisTimeSlot: %d\n", m_iNumEnemiesSpawnedThisTimeSlot );
				
				m_iCurrentSpawnPositionIndex++;
				if( m_iCurrentSpawnPositionIndex >= m_SpawnPositionList.size() ) 
					m_iCurrentSpawnPositionIndex = 0;
				
				m_fStaggeredSpawnTimer = m_TimeSlots[m_iCurrentTimeSlotIndex].m_fStaggeredSpawnTimeSeconds;
				//printf( "Stagger Time: %f\n", m_fStaggeredSpawnTimer );
			}
		}
	}
	
	CheckTimeSlotIncrement();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemyManaEaterSpawner::CheckTimeSlotIncrement()
{
	// If not in last time slot, count UP.  If in last time slot, count DOWN.
	if( m_bAtLastTimeSlot == false )
	{
		m_fTimeSlotTimer += ITickable::smTickSec;
		if( m_fTimeSlotTimer < m_fNextTimeSlotStartTime )
			return;
	}
	else
	{
		m_fTimeSlotTimer -= ITickable::smTickSec;
		if( m_fTimeSlotTimer > 0.0f )
			return;
		
		m_fTimeSlotTimer = m_fFinalTimeSlotTimeDifference;
		m_fStaggeredSpawnTimer = 0.0f;
		m_iNumEnemiesSpawnedThisTimeSlot = 0;
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
					m_iNumEnemiesSpawnedThisTimeSlot = 0;
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
			m_iNumEnemiesSpawnedThisTimeSlot = 0;
		}
	}
	
	AssertFatal( m_iCurrentTimeSlotIndex < m_uMaxTimeSlots, "CComponentEnemyManaEaterSpawner::CheckTimeSlotIncrement - Time Slot index out of bounds\n" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemyManaEater* CComponentEnemyManaEaterSpawner::GetNextAvailableEnemy() const
{
	CComponentEnemyManaEater* pEnemyComponent = NULL;
	S32 iNumComponents = m_EnemyComponentPool.size();
	for( S32 i = 0; i < iNumComponents; ++i )
	{
		pEnemyComponent = m_EnemyComponentPool[i];
		if( pEnemyComponent && pEnemyComponent->IsSpawned() == false )
			return pEnemyComponent;
	}
	
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// CComponentEnemyManaEaterSpawner::CTimeSlotInfo

CComponentEnemyManaEaterSpawner::CTimeSlotInfo::CTimeSlotInfo()
	: m_bUseTimeSlot( false )
	, m_fSlotStartTimeSeconds( 0.0f )
	, m_fStaggeredSpawnTimeSeconds( 0.0f )
	, m_iMaxNumSpawns( 0 )
	, m_iPercentChanceSuper( 0 )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
