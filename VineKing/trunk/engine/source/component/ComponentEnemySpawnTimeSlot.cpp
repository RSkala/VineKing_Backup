//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySpawnTimeSlot.h"

#include "ComponentEnemySpawnPosition.h"
#include "ComponentStaticPath.h"

#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySpawnTimeSlot );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnTimeSlot::CComponentEnemySpawnTimeSlot()
	: m_pOwner( NULL )
	, m_fSlotStartTime( 0.0f )
	, m_iNumUsedEntries( 0 )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySpawnTimeSlot::~CComponentEnemySpawnTimeSlot()
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySpawnTimeSlot::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySpawnTimeSlot ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "StartTime", TypeF32,	Offset( m_fSlotStartTime, CComponentEnemySpawnTimeSlot ) );
	
	// Entry 01
	addField( "UseEntry_01",		TypeBool,			Offset( m_TimeSlotEntries[0].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_01",		TypeCaseString,		Offset( m_TimeSlotEntries[0].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_01",		TypeCaseString,		Offset( m_TimeSlotEntries[0].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_01",		TypeS32,			Offset( m_TimeSlotEntries[0].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_01",		TypeF32,			Offset( m_TimeSlotEntries[0].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_01",			TypeCaseString,		Offset( m_TimeSlotEntries[0].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_01",	TypeCaseString,		Offset( m_TimeSlotEntries[0].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 02
	addField( "UseEntry_02",		TypeBool,			Offset( m_TimeSlotEntries[1].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_02",		TypeCaseString,		Offset( m_TimeSlotEntries[1].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_02",		TypeCaseString,		Offset( m_TimeSlotEntries[1].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_02",		TypeS32,			Offset( m_TimeSlotEntries[1].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_02",		TypeF32,			Offset( m_TimeSlotEntries[1].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_02",			TypeCaseString,		Offset( m_TimeSlotEntries[1].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_02",	TypeCaseString,		Offset( m_TimeSlotEntries[1].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 03
	addField( "UseEntry_03",		TypeBool,			Offset( m_TimeSlotEntries[2].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_03",		TypeCaseString,		Offset( m_TimeSlotEntries[2].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_03",		TypeCaseString,		Offset( m_TimeSlotEntries[2].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_03",		TypeS32,			Offset( m_TimeSlotEntries[2].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_03",		TypeF32,			Offset( m_TimeSlotEntries[2].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_03",			TypeCaseString,		Offset( m_TimeSlotEntries[2].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_03",	TypeCaseString,		Offset( m_TimeSlotEntries[2].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 04
	addField( "UseEntry_04",		TypeBool,			Offset( m_TimeSlotEntries[3].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_04",		TypeCaseString,		Offset( m_TimeSlotEntries[3].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_04",		TypeCaseString,		Offset( m_TimeSlotEntries[3].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_04",		TypeS32,			Offset( m_TimeSlotEntries[3].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_04",		TypeF32,			Offset( m_TimeSlotEntries[3].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_04",			TypeCaseString,		Offset( m_TimeSlotEntries[3].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_04",	TypeCaseString,		Offset( m_TimeSlotEntries[3].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 05
	addField( "UseEntry_05",		TypeBool,			Offset( m_TimeSlotEntries[4].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_05",		TypeCaseString,		Offset( m_TimeSlotEntries[4].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_05",		TypeCaseString,		Offset( m_TimeSlotEntries[4].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_05",		TypeS32,			Offset( m_TimeSlotEntries[4].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_05",		TypeF32,			Offset( m_TimeSlotEntries[4].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_05",			TypeCaseString,		Offset( m_TimeSlotEntries[4].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_05",	TypeCaseString,		Offset( m_TimeSlotEntries[4].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 06
	addField( "UseEntry_06",		TypeBool,			Offset( m_TimeSlotEntries[5].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_06",		TypeCaseString,		Offset( m_TimeSlotEntries[5].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_06",		TypeCaseString,		Offset( m_TimeSlotEntries[5].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_06",		TypeS32,			Offset( m_TimeSlotEntries[5].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_06",		TypeF32,			Offset( m_TimeSlotEntries[5].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_06",			TypeCaseString,		Offset( m_TimeSlotEntries[5].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_06",	TypeCaseString,		Offset( m_TimeSlotEntries[5].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 07
	addField( "UseEntry_07",		TypeBool,			Offset( m_TimeSlotEntries[6].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_07",		TypeCaseString,		Offset( m_TimeSlotEntries[6].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_07",		TypeCaseString,		Offset( m_TimeSlotEntries[6].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_07",		TypeS32,			Offset( m_TimeSlotEntries[6].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_07",		TypeF32,			Offset( m_TimeSlotEntries[6].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_07",			TypeCaseString,		Offset( m_TimeSlotEntries[6].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_07",	TypeCaseString,		Offset( m_TimeSlotEntries[6].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 08
	addField( "UseEntry_08",		TypeBool,			Offset( m_TimeSlotEntries[7].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_08",		TypeCaseString,		Offset( m_TimeSlotEntries[7].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_08",		TypeCaseString,		Offset( m_TimeSlotEntries[7].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_08",		TypeS32,			Offset( m_TimeSlotEntries[7].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_08",		TypeF32,			Offset( m_TimeSlotEntries[7].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_08",			TypeCaseString,		Offset( m_TimeSlotEntries[7].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_08",	TypeCaseString,		Offset( m_TimeSlotEntries[7].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 09
	addField( "UseEntry_09",		TypeBool,			Offset( m_TimeSlotEntries[8].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_09",		TypeCaseString,		Offset( m_TimeSlotEntries[8].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_09",		TypeCaseString,		Offset( m_TimeSlotEntries[8].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_09",		TypeS32,			Offset( m_TimeSlotEntries[8].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_09",		TypeF32,			Offset( m_TimeSlotEntries[8].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_09",			TypeCaseString,		Offset( m_TimeSlotEntries[8].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_09",	TypeCaseString,		Offset( m_TimeSlotEntries[8].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 10
	addField( "UseEntry_10",		TypeBool,			Offset( m_TimeSlotEntries[9].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_10",		TypeCaseString,		Offset( m_TimeSlotEntries[9].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_10",		TypeCaseString,		Offset( m_TimeSlotEntries[9].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_10",		TypeS32,			Offset( m_TimeSlotEntries[9].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_10",		TypeF32,			Offset( m_TimeSlotEntries[9].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_10",			TypeCaseString,		Offset( m_TimeSlotEntries[9].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_10",	TypeCaseString,		Offset( m_TimeSlotEntries[9].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );

	// Entry 11
	addField( "UseEntry_11",		TypeBool,			Offset( m_TimeSlotEntries[10].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_11",		TypeCaseString,		Offset( m_TimeSlotEntries[10].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_11",		TypeCaseString,		Offset( m_TimeSlotEntries[10].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_11",		TypeS32,			Offset( m_TimeSlotEntries[10].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_11",		TypeF32,			Offset( m_TimeSlotEntries[10].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_11",			TypeCaseString,		Offset( m_TimeSlotEntries[10].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_11",	TypeCaseString,		Offset( m_TimeSlotEntries[10].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
	
	// Entry 12
	addField( "UseEntry_12",		TypeBool,			Offset( m_TimeSlotEntries[11].m_bUseEntry,			CComponentEnemySpawnTimeSlot ) );
	addField( "SpawnPoint_12",		TypeCaseString,		Offset( m_TimeSlotEntries[11].m_pszSpawnPoint,		CComponentEnemySpawnTimeSlot ) );
	addField( "EnemyType_12",		TypeCaseString,		Offset( m_TimeSlotEntries[11].m_pszEnemyType,		CComponentEnemySpawnTimeSlot ) );
	addField( "NumEnemies_12",		TypeS32,			Offset( m_TimeSlotEntries[11].m_iNumEnemiesToSpawn,	CComponentEnemySpawnTimeSlot ) );
	addField( "StaggerTime_12",		TypeF32,			Offset( m_TimeSlotEntries[11].m_fStaggerTime,		CComponentEnemySpawnTimeSlot ) );
	addField( "Path_12",			TypeCaseString,		Offset( m_TimeSlotEntries[11].m_pszStaticPath,		CComponentEnemySpawnTimeSlot ) );
	addField( "SmokePosIndex_12",	TypeCaseString,		Offset( m_TimeSlotEntries[11].m_pszSmokePosIndex,	CComponentEnemySpawnTimeSlot ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySpawnTimeSlot::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySpawnTimeSlot::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnTimeSlot::onUpdate()
{
	if( m_pOwner == NULL )
		return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnTimeSlot::OnPostInit()
{
	for( S32 i = 0; i < m_iMaxTimeSlotEntries; ++i )
	{
		SetUpTimeSlotEntry( m_TimeSlotEntries[i] );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySpawnTimeSlot::SetUpTimeSlotEntry( CTimeSlotEntry& _TimeSlotEntry )
{
	if( _TimeSlotEntry.m_bUseEntry == false )
		return;
	
	m_iNumUsedEntries++;
	
	// Get Spawn Point position (if set up)
	if( _TimeSlotEntry.m_pszSpawnPoint != NULL && _TimeSlotEntry.m_pszSpawnPoint[0] != '\0' )
	{
		t2dSceneObject* pSpawnPoint = static_cast<t2dSceneObject*>( Sim::findObject( _TimeSlotEntry.m_pszSpawnPoint ) );
		if( pSpawnPoint )
		{
			CComponentEnemySpawnPosition* pSpawnPosition = static_cast<CComponentEnemySpawnPosition*>( pSpawnPoint->getComponent( 0 ) );
			if( pSpawnPosition )
				_TimeSlotEntry.m_pSpawnPosition = pSpawnPosition;
		}
	}
	
	// Get Enemy Type
	if( _TimeSlotEntry.m_pszEnemyType[0] != '\0' )
	{
		if( dStricmp( _TimeSlotEntry.m_pszEnemyType, "BLOB" ) == 0 )
		{
			_TimeSlotEntry.m_eEnemyType = ENEMY_TYPE_BLOB;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszEnemyType, "SUPER BLOB" ) == 0 )
		{
			_TimeSlotEntry.m_eEnemyType = ENEMY_TYPE_SUPER_BLOB;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszEnemyType, "SERPENT" ) == 0 )
		{
			_TimeSlotEntry.m_eEnemyType = ENEMY_TYPE_SERPENT;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszEnemyType, "SUPER SERPENT" ) == 0 )
		{
			_TimeSlotEntry.m_eEnemyType = ENEMY_TYPE_SUPER_SERPENT;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszEnemyType, "SMOKE" ) == 0 )
		{
			_TimeSlotEntry.m_eEnemyType = ENEMY_TYPE_SMOKE;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszEnemyType, "SUPER SMOKE" ) == 0 )
		{
			_TimeSlotEntry.m_eEnemyType = ENEMY_TYPE_SUPER_SMOKE;
		}
		else
		{
			printf( "Enemy Type not set up for object: %s\n", m_pOwner ? m_pOwner->getName() : "(no name)" );
		}
	}
	
	// Get Static Path (if set up)
	if( _TimeSlotEntry.m_pszStaticPath != NULL && _TimeSlotEntry.m_pszStaticPath[0] != '\0' )
	{
		t2dSceneObject* pStaticPathObject = static_cast<t2dSceneObject*>( Sim::findObject( _TimeSlotEntry.m_pszStaticPath ) );
		if( pStaticPathObject )
		{
			//CComponentStaticPath* pStaticPath = static_cast<CComponentStaticPath*>( _TimeSlotEntry.m_pStaticPathOwner->getComponent( 0 ) );
			CComponentStaticPath* pStaticPath = static_cast<CComponentStaticPath*>( pStaticPathObject->getComponent( 0 ) );
			if( pStaticPath )
				_TimeSlotEntry.m_pStaticPath = pStaticPath;
		}
	}
	
	// Set up the Smoke Start and End Positions
	if( _TimeSlotEntry.m_pszSmokePosIndex != NULL && _TimeSlotEntry.m_pszSmokePosIndex[0] != '\0' )
	{
		if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "-" ) == 0 )
		{
			// Not set. This Time Slot does not use Smoke Positions.
		}		
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "ITERATE" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = -1;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "01" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 0;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "02" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 1;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "03" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 2;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "04" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 3;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "05" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 4;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "06" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 5;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "07" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 6;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "08" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 7;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "09" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 8;
		}
		else if( dStricmp( _TimeSlotEntry.m_pszSmokePosIndex, "10" ) == 0 )
		{
			_TimeSlotEntry.m_iSmokePosIndex = 9;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------