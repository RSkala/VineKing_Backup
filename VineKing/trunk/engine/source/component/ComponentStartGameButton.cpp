//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentStartGameButton.h"

#include "ComponentGlobals.h"
#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szLevelName[64] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentStartGameButton );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStartGameButton::CComponentStartGameButton()
	: m_pOwnerButton( NULL )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStartGameButton::~CComponentStartGameButton()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStartGameButton::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerButton, CComponentStartGameButton ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from SimComponent
bool CComponentStartGameButton::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s - Must be added to a t2dSceneObject.", __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwnerButton = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStartGameButton::onUpdate()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStartGameButton::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStartGameButton::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwnerButton )
	{
		m_pOwnerButton->setUseMouseEvents( false );
		CComponentGlobals::GetInstance().LoadGameSettings(); // I think this component is no longer used
		
		CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szLevelName );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStartGameButton::OnPostInit()
{
	if( m_pOwnerButton )
	{
		m_pOwnerButton->setUseMouseEvents( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------