//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentStaticPath.h"

#include "T2D/t2dSceneObject.h"
#include "platform/platformAssert.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static t2dVector g_vTempVector( 0.0f, 0.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<CComponentStaticPath*> CComponentStaticPath::m_StaticPathList;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentStaticPath );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStaticPath::CComponentStaticPath()
	: m_pOwner( NULL )
	, m_pszNode01( NULL )
	, m_pszNode02( NULL )
	, m_pszNode03( NULL )
	, m_pszNode04( NULL )
	, m_pszNode05( NULL )
	, m_pszNode06( NULL )
	, m_pszNode07( NULL )
	, m_pszNode08( NULL )
	, m_pszNode09( NULL )
	, m_pszNode10( NULL )
	, m_pszNode11( NULL )
	, m_pszNode12( NULL )
	, m_pszNode13( NULL )
	, m_pszNode14( NULL )
	, m_pszNode15( NULL )
	, m_pszNode16( NULL )
	, m_pszNode17( NULL )
	, m_pszNode18( NULL )
	, m_pszNode19( NULL )
	, m_pszNode20( NULL )
	, m_pszNode21( NULL )
	, m_pszNode22( NULL )
	, m_pszNode23( NULL )
	, m_pszNode24( NULL )
	, m_pszNode25( NULL )
	, m_pszNode26( NULL )
	, m_pszNode27( NULL )
	, m_pszNode28( NULL )
	, m_pszNode29( NULL )
	, m_pszNode30( NULL )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStaticPath::~CComponentStaticPath()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStaticPath::GetPositionAtNodeIndex( const S32& _iNodeIndex, t2dVector& _vOutPosition )
{
	AssertFatal( _iNodeIndex >= 0, "CComponentStaticPath::GetPositionAtNodeIndex - Node index is < 0" );
	AssertFatal( _iNodeIndex < m_NodePositionList.size(), "CComponentStaticPath::GetPositionAtNodeIndex - Index out of bounds" );
	
	_vOutPosition = m_NodePositionList[_iNodeIndex];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStaticPath::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentStaticPath ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "Node01",	TypeString,	Offset( m_pszNode01, CComponentStaticPath ) );
	addField( "Node02",	TypeString,	Offset( m_pszNode02, CComponentStaticPath ) );
	addField( "Node03",	TypeString,	Offset( m_pszNode03, CComponentStaticPath ) );
	addField( "Node04",	TypeString,	Offset( m_pszNode04, CComponentStaticPath ) );
	addField( "Node05",	TypeString,	Offset( m_pszNode05, CComponentStaticPath ) );
	addField( "Node06",	TypeString,	Offset( m_pszNode06, CComponentStaticPath ) );
	addField( "Node07",	TypeString,	Offset( m_pszNode07, CComponentStaticPath ) );
	addField( "Node08",	TypeString,	Offset( m_pszNode08, CComponentStaticPath ) );
	addField( "Node09",	TypeString,	Offset( m_pszNode09, CComponentStaticPath ) );
	addField( "Node10",	TypeString,	Offset( m_pszNode10, CComponentStaticPath ) );
	addField( "Node11",	TypeString,	Offset( m_pszNode11, CComponentStaticPath ) );
	addField( "Node12",	TypeString,	Offset( m_pszNode12, CComponentStaticPath ) );
	addField( "Node13",	TypeString,	Offset( m_pszNode13, CComponentStaticPath ) );
	addField( "Node14",	TypeString,	Offset( m_pszNode14, CComponentStaticPath ) );
	addField( "Node15",	TypeString,	Offset( m_pszNode15, CComponentStaticPath ) );
	addField( "Node16",	TypeString,	Offset( m_pszNode16, CComponentStaticPath ) );
	addField( "Node17",	TypeString,	Offset( m_pszNode17, CComponentStaticPath ) );
	addField( "Node18",	TypeString,	Offset( m_pszNode18, CComponentStaticPath ) );
	addField( "Node19",	TypeString,	Offset( m_pszNode19, CComponentStaticPath ) );
	addField( "Node20",	TypeString,	Offset( m_pszNode20, CComponentStaticPath ) );
	addField( "Node21",	TypeString,	Offset( m_pszNode21, CComponentStaticPath ) );
	addField( "Node22",	TypeString,	Offset( m_pszNode22, CComponentStaticPath ) );
	addField( "Node23",	TypeString,	Offset( m_pszNode23, CComponentStaticPath ) );
	addField( "Node24",	TypeString,	Offset( m_pszNode24, CComponentStaticPath ) );
	addField( "Node25",	TypeString,	Offset( m_pszNode25, CComponentStaticPath ) );
	addField( "Node26",	TypeString,	Offset( m_pszNode26, CComponentStaticPath ) );
	addField( "Node27",	TypeString,	Offset( m_pszNode27, CComponentStaticPath ) );
	addField( "Node28",	TypeString,	Offset( m_pszNode28, CComponentStaticPath ) );
	addField( "Node29",	TypeString,	Offset( m_pszNode29, CComponentStaticPath ) );
	addField( "Node30",	TypeString,	Offset( m_pszNode30, CComponentStaticPath ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentStaticPath::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentStaticPath::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStaticPath::OnPostInit()
{
	//assignName( "CComponentStaticPath" );
	if( m_pOwner == NULL )
		return;	
	
	m_NodePositionList.clear();
	
	SetUpNodePositions();
	
	m_StaticPathList.push_back( this );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHOD IMPLEMENTATION

void CComponentStaticPath::SetUpNodePositions()
{
	t2dSceneObject* pObject = NULL;
	
	// New Way
	AssignPositionFromNodeText( m_pszNode01 );
	AssignPositionFromNodeText( m_pszNode02 );
	AssignPositionFromNodeText( m_pszNode03 );
	AssignPositionFromNodeText( m_pszNode04 );
	AssignPositionFromNodeText( m_pszNode05 );
	AssignPositionFromNodeText( m_pszNode06 );
	AssignPositionFromNodeText( m_pszNode07 );
	AssignPositionFromNodeText( m_pszNode08 );
	AssignPositionFromNodeText( m_pszNode09 );
	AssignPositionFromNodeText( m_pszNode10 );
	AssignPositionFromNodeText( m_pszNode11 );
	AssignPositionFromNodeText( m_pszNode12 );
	AssignPositionFromNodeText( m_pszNode13 );
	AssignPositionFromNodeText( m_pszNode14 );
	AssignPositionFromNodeText( m_pszNode15 );
	
	// This is the old way of doing it
	if( 0 )
		return;
	
	if( m_pszNode01 && m_pszNode01[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode01 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode02 && m_pszNode02[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode02 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode03 && m_pszNode03[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode03 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode04 && m_pszNode04[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode04 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode05 && m_pszNode05[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode05 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode06 && m_pszNode06[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode06 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode07 && m_pszNode07[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode07 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode08 && m_pszNode08[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode08 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode09 && m_pszNode09[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode09 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode10 && m_pszNode10[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode10 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode11 && m_pszNode11[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode11 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode12 && m_pszNode12[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode12 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode13 && m_pszNode13[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode13 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode14 && m_pszNode14[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode14 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode15 && m_pszNode15[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode15 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode16 && m_pszNode16[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode16 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode17 && m_pszNode17[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode17 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode18 && m_pszNode18[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode18 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode19 && m_pszNode19[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode19 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	if( m_pszNode20 && m_pszNode20[0] != '\0' )
	{
		pObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszNode20 ) );
		if( pObject )
			m_NodePositionList.push_back( pObject->getPosition() );
	}
	
	//printf( "%s - Added %d position nodes\n", m_pOwner ? m_pOwner->getName() : "?", m_NodePositionList.size() );
	//for( S32 i = 0; i < m_NodePositionList.size(); ++i )
	//{
	//	printf( "- Node %d: (%f, %f)\n", i, m_NodePositionList[i].mX, m_NodePositionList[i].mY );
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStaticPath::AssignPositionFromNodeText( const char* _pszNodeText )
{
	if( _pszNodeText == NULL || _pszNodeText[0] == '\0' )
		return;

	if( strcmp( _pszNodeText, "-" ) == 0 )
		return;
	
	g_vTempVector = t2dVector::getZero();
	
	//char* pszTemp;
	
	// Nodes have the format:  PREFIX_SUFFIX
	//  - Nodes with the same PREFIX will have the same Y-value
	//	- Nodes with the same SUFFIX will have the same X-value
	
	// Check PREFIX for Y-value
	if( strstr( _pszNodeText, "01_" ) != NULL )
	{
		g_vTempVector.mY = -192.0f;
	}
	else if( strstr( _pszNodeText, "02_" ) != NULL )
	{
		g_vTempVector.mY = -160.0f;
	}
	else if( strstr( _pszNodeText, "03_" ) != NULL )
	{
		g_vTempVector.mY = -128.0f;
	}
	else if( strstr( _pszNodeText, "04_" ) != NULL )
	{
		g_vTempVector.mY = -96.0f;
	}
	else if( strstr( _pszNodeText, "05_" ) != NULL )
	{
		g_vTempVector.mY = -64.0f;
	}
	else if( strstr( _pszNodeText, "06_" ) != NULL )
	{
		g_vTempVector.mY = -32.0f;
	}
	else if( strstr( _pszNodeText, "07_" ) != NULL )
	{
		g_vTempVector.mY = 0.0f;
	}
	else if( strstr( _pszNodeText, "08_" ) != NULL )
	{
		g_vTempVector.mY = 32.0f;
	}
	else if( strstr( _pszNodeText, "09_" ) != NULL )
	{
		g_vTempVector.mY = 64.0f;
	}
	else if( strstr( _pszNodeText, "10_" ) != NULL )
	{
		g_vTempVector.mY = 96.0f;
	}
	else if( strstr( _pszNodeText, "11_" ) != NULL )
	{
		g_vTempVector.mY = 128.0f;
	}
	else if( strstr( _pszNodeText, "12_" ) != NULL )
	{
		g_vTempVector.mY = 160.0f;
	}
	else if( strstr( _pszNodeText, "13_" ) != NULL )
	{
		g_vTempVector.mY = 192.0f;
	}
	else
	{
		printf( "Cannot find get SUFFIX '%s' from Node for Static Path: %s\n", _pszNodeText, m_pOwner ? m_pOwner->getName() : "" );
		return;
	}
	
	
	// Check SUFFIX for X-value
	if( strstr( _pszNodeText, "_01" ) != NULL )
	{
		g_vTempVector.mX = -112.0f;
	}
	else if( strstr( _pszNodeText, "_02" ) != NULL )
	{
		g_vTempVector.mX = -80.0f;
	}
	else if( strstr( _pszNodeText, "_03" ) != NULL )
	{
		g_vTempVector.mX = -48.0f;
	}
	else if( strstr( _pszNodeText, "_04" ) != NULL )
	{
		g_vTempVector.mX = -16.0f;
	}
	else if( strstr( _pszNodeText, "_05" ) != NULL )
	{
		g_vTempVector.mX = 16.0f;
	}
	else if( strstr( _pszNodeText, "_06" ) != NULL )
	{
		g_vTempVector.mX = 48.0f;
	}
	else if( strstr( _pszNodeText, "_07" ) != NULL )
	{
		g_vTempVector.mX = 80.0f;
	}
	else if( strstr( _pszNodeText, "_08" ) != NULL )
	{
		g_vTempVector.mX = 112.0f;
	}
	else
	{
		printf( "Cannot find get PREFIX '%s' from Node for Static Path: %s\n", _pszNodeText, m_pOwner ? m_pOwner->getName() : "" );
		return;
	}
	
	m_NodePositionList.push_back( g_vTempVector );
}