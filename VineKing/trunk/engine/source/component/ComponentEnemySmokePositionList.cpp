//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySmokePositionList.h"

#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<bool> CComponentEnemySmokePositionList::sm_bInvalidIndexArray;
Vector<t2dVector> CComponentEnemySmokePositionList::sm_StartPositionList;
Vector<t2dVector> CComponentEnemySmokePositionList::sm_EndPositionList;
CComponentEnemySmokePositionList* CComponentEnemySmokePositionList::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySmokePositionList );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySmokePositionList::CComponentEnemySmokePositionList()
	: m_pOwner( NULL )
{
	CComponentEnemySmokePositionList::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySmokePositionList::~CComponentEnemySmokePositionList()
{
	CComponentEnemySmokePositionList::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// GetPositionsAtIndex
// - Returns true if the positions are valid
// - Returns false if the positions are invalid and should not be used

/*static*/ bool CComponentEnemySmokePositionList::GetPositionsAtIndex( const S32& _iIndex, t2dVector& _vOutStartPos, t2dVector& _vOutEndPos )
{ 
	if( _iIndex >= sm_StartPositionList.size() || _iIndex >= sm_EndPositionList.size() )
		return false;
	
	if( sm_bInvalidIndexArray[ _iIndex ] )
		return false;
	
	_vOutStartPos = sm_StartPositionList[_iIndex]; 
	_vOutEndPos = sm_EndPositionList[_iIndex];
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySmokePositionList::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEnemySmokePositionList ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "StartPos_01", TypeF32Vector,	Offset( m_StartPositionArray[0], CComponentEnemySmokePositionList ) );
	addField( "StartPos_02", TypeF32Vector,	Offset( m_StartPositionArray[1], CComponentEnemySmokePositionList ) );
	addField( "StartPos_03", TypeF32Vector,	Offset( m_StartPositionArray[2], CComponentEnemySmokePositionList ) );
	addField( "StartPos_04", TypeF32Vector,	Offset( m_StartPositionArray[3], CComponentEnemySmokePositionList ) );
	addField( "StartPos_05", TypeF32Vector,	Offset( m_StartPositionArray[4], CComponentEnemySmokePositionList ) );
	addField( "StartPos_06", TypeF32Vector,	Offset( m_StartPositionArray[5], CComponentEnemySmokePositionList ) );
	addField( "StartPos_07", TypeF32Vector,	Offset( m_StartPositionArray[6], CComponentEnemySmokePositionList ) );
	addField( "StartPos_08", TypeF32Vector,	Offset( m_StartPositionArray[7], CComponentEnemySmokePositionList ) );
	addField( "StartPos_09", TypeF32Vector,	Offset( m_StartPositionArray[8], CComponentEnemySmokePositionList ) );
	addField( "StartPos_10", TypeF32Vector,	Offset( m_StartPositionArray[9], CComponentEnemySmokePositionList ) );
	
	addField( "EndPos_01", TypeF32Vector,	Offset( m_EndPositionArray[0], CComponentEnemySmokePositionList ) );
	addField( "EndPos_02", TypeF32Vector,	Offset( m_EndPositionArray[1], CComponentEnemySmokePositionList ) );
	addField( "EndPos_03", TypeF32Vector,	Offset( m_EndPositionArray[2], CComponentEnemySmokePositionList ) );
	addField( "EndPos_04", TypeF32Vector,	Offset( m_EndPositionArray[3], CComponentEnemySmokePositionList ) );
	addField( "EndPos_05", TypeF32Vector,	Offset( m_EndPositionArray[4], CComponentEnemySmokePositionList ) );
	addField( "EndPos_06", TypeF32Vector,	Offset( m_EndPositionArray[5], CComponentEnemySmokePositionList ) );
	addField( "EndPos_07", TypeF32Vector,	Offset( m_EndPositionArray[6], CComponentEnemySmokePositionList ) );
	addField( "EndPos_08", TypeF32Vector,	Offset( m_EndPositionArray[7], CComponentEnemySmokePositionList ) );
	addField( "EndPos_09", TypeF32Vector,	Offset( m_EndPositionArray[8], CComponentEnemySmokePositionList ) );
	addField( "EndPos_10", TypeF32Vector,	Offset( m_EndPositionArray[9], CComponentEnemySmokePositionList ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySmokePositionList::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySmokePositionList::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	CComponentEnemySmokePositionList::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmokePositionList::OnPostInit()
{
	CComponentEnemySmokePositionList::sm_bInvalidIndexArray.clear();
	
	for( S32 i = 0; i < m_iMaxNumPositions; ++i )
	{
		//printf( "------------------\n" );
		//printf( "INDEX = %d\n", i );
		//printf( "- START POS: " );
		SetUpStartPosition( m_StartPositionArray[i] );
		
		//printf( "- END POS:   " );
		SetUpEndPosition( m_EndPositionArray[i] );
		
		CComponentEnemySmokePositionList::sm_bInvalidIndexArray.push_back( false );
	}
	
	AssertFatal( CComponentEnemySmokePositionList::sm_StartPositionList.size() == CComponentEnemySmokePositionList::sm_EndPositionList.size(), "Error: # of Smoke Start Positions does not match # of End Positions" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmokePositionList::SetUpStartPosition( Vector<F32>& _position )
{
	if( _position.size() != 2 )
	{
		//printf( "Size is not valid: %d\n", _position.size() );
		return;
	}
	
	t2dVector vPosition;
	vPosition.mX = _position[0];
	vPosition.mY = _position[1];
	
	//printf( "Position: (%f, %f)\n", vPosition.mX, vPosition.mY );
	
	// This is a hack to compensate for a bug in TGB where 0.0 MAY OR MAY NOT be a valid position
	if( vPosition.mX == 0.0f || vPosition.mY == 0.0f )
		return;
	
	CComponentEnemySmokePositionList::sm_StartPositionList.push_back( vPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySmokePositionList::SetUpEndPosition( Vector<F32>& _position )
{
	if( _position.size() != 2 )
	{
		//printf( "Size is not valid: %d\n", _position.size() );
		return;
	}
	
	t2dVector vPosition;
	vPosition.mX = _position[0];
	vPosition.mY = _position[1];
	
	//printf( "Position: (%f, %f)\n", vPosition.mX, vPosition.mY );
	
	// This is a hack to compensate for a bug in TGB where 0.0 MAY OR MAY NOT be a valid position
	if( vPosition.mX == 0.0f || vPosition.mY == 0.0f )
		return;
	
	CComponentEnemySmokePositionList::sm_EndPositionList.push_back( vPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------