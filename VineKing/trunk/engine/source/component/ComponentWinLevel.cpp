//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentWinLevel.h"

#include "ComponentGlobals.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"

//#include "platform/gameInterface.h" // for Game object
#include "T2D/t2dSceneObject.h"
//#include "T2D/t2dSceneWindow.h" // for gCurrentSceneWindow object

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

//extern GameInterface* Game;
//extern t2dSceneWindow* gCurrentSceneWindow;

//extern t2dSceneWindow* g_pCurrentSceneWindow;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vOKButtonPressedSize( 24.0f, 24.0f );
static t2dVector g_vOKButtonUnpressedSize( 32.0f, 32.0f );

static const F32 g_fOKButtonPressedTimeMs = 80.0f; // Time that the OK button stays pressed (in milliseconds)

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ CComponentWinLevel* CComponentWinLevel::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentWinLevel );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentWinLevel::CComponentWinLevel()
	: m_pOwnerOKButtonObject( NULL )
	, m_pszYouWinObject( NULL )
	, m_pYouWinObject( NULL )
	, m_pszGameOverObject( NULL )
	, m_pGameOverObject( NULL )
	, m_fOKButtonPressedTimer( 0.0f )
	, m_bOKButtonPressed( false )
	, m_bOKButtonUnpressed( false )
	, m_bReturningToTileScreen( false )
	, m_bLevelHasEnded( false )
	, m_bLevelLost( false )
{
	CComponentWinLevel::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentWinLevel::~CComponentWinLevel()
{
	CComponentWinLevel::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::StartWinLevelSequence()
{
	//printf( "CComponentWinLevel::StartWinLevelSequence\n" );
	
	// This check is here in case this function is called more than once (which shouldn't actually happen)
	if( CComponentGlobals::GetInstance().IsLevelComplete() )
		return;
	
	m_bLevelHasEnded = true;
	m_bLevelLost = false;
	
	Con::executef( (SimObject*)m_pOwnerOKButtonObject, 1, "PlayLevelWinMusic" );
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseOfLevelWin();
	
	if( m_pOwnerOKButtonObject )
	{
		m_pOwnerOKButtonObject->setVisible( true );
		m_pOwnerOKButtonObject->setUseMouseEvents( true );
	}
	
	if( m_pYouWinObject )
	{
		m_pYouWinObject->setVisible( true );
	}
	
	CComponentGlobals::GetInstance().SetLevelComplete();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::StartGameOverSequence()
{
	//printf( "CComponentWinLevel::StartGameOverSequence\n" );
	
	//m_bLevelHasEnded = true;
	m_bLevelLost = true;
	
	if( m_pOwnerOKButtonObject )
	{
		m_pOwnerOKButtonObject->setVisible( true );
		m_pOwnerOKButtonObject->setUseMouseEvents( true );
	}
	
	Con::executef( (SimObject*)m_pOwnerOKButtonObject, 1, "PlayLevelLoseMusic" );
	
	if( m_pGameOverObject )
	{
		m_pGameOverObject->setVisible( true );
	}
	
	// For now, just use the same "level complete" variable
	CComponentGlobals::GetInstance().SetLevelComplete();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::ReturnToTitleScreen()
{
	//printf( "Calling CComponentWinLevel::ReturnToTitleScreen\n" );
	
	char szLevelName[128] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";
	CComponentGlobals::GetInstance().ScheduleLoadLevel( szLevelName );
	
	//Con::executef( (SimObject*)m_pOwnerOKButtonObject, 1, "OnWinLevel" ); // RKS NOTE: This is how you call a script function in code
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentWinLevel::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerOKButtonObject, CComponentWinLevel ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "YouWinObject",	TypeString, Offset( m_pszYouWinObject,	CComponentWinLevel ) );
	addField( "GameOverObject",	TypeString, Offset( m_pszGameOverObject,	CComponentWinLevel ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from SimComponent
bool CComponentWinLevel::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		//printf( "CComponentWinLevel::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwnerOKButtonObject = pOwnerObject;
	
	CComponentWinLevel::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::onUpdate()
{
	DynamicConsoleMethodComponent::onUpdate();
	
	if( m_pOwnerOKButtonObject == NULL )
		return;
	
	//if( CComponentGlobals::GetInstance().IsLevelComplete() && m_bReturningToTileScreen == false )
	if( m_bOKButtonPressed && m_bReturningToTileScreen == false )
	{
		if( m_bOKButtonUnpressed == false )
		{
			m_fOKButtonPressedTimer += ITickable::smTickMs;
			if( m_fOKButtonPressedTimer >= g_fOKButtonPressedTimeMs )
			{
				if( m_pOwnerOKButtonObject )
				{
					m_pOwnerOKButtonObject->setSize( g_vOKButtonUnpressedSize );
					m_bOKButtonUnpressed = true;
				}
			}
		}
		else
		{
			if( m_bLevelLost )
			{
				ReturnToTitleScreen();
			}
			else 
			{
				//CComponentScoringManager::GetInstance().OnLevelComplete();
				
				if( m_pOwnerOKButtonObject ) m_pOwnerOKButtonObject->setVisible( false );
				if( m_pYouWinObject ) m_pYouWinObject->setVisible( false );
				if( m_pGameOverObject) m_pGameOverObject->setVisible( false );
				
				m_bReturningToTileScreen = true;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwnerOKButtonObject )
	{
		m_pOwnerOKButtonObject->setUseMouseEvents( false ); // Disable mouse events so another press is not registered
		
		m_bOKButtonPressed = true;
		
		g_vOKButtonUnpressedSize.set( m_pOwnerOKButtonObject->getSize().mX, m_pOwnerOKButtonObject->getSize().mY );
		t2dVector vOKButtonPressedSize( m_pOwnerOKButtonObject->getSize().mX * 0.80f, m_pOwnerOKButtonObject->getSize().mY * 0.80f );
		m_pOwnerOKButtonObject->setSize( vOKButtonPressedSize );
	}
	
	//CComponentGlobals::GetInstance().SetLevelComplete();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentWinLevel::OnPostInit()
{
	m_bOKButtonUnpressed = false;
	m_bReturningToTileScreen = false;
	
	if( m_pYouWinObject == NULL )
	{
		// Find the object that displays "You Win"
		if( m_pszYouWinObject != NULL )
		{
			m_pYouWinObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszYouWinObject ) );
			if( m_pYouWinObject )
				m_pYouWinObject->setVisible( false );
		}
	}
	
	if( m_pGameOverObject == NULL )
	{
		// Find the object that displays "Game Over"
		if( m_pszGameOverObject != NULL )
		{
			m_pGameOverObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszGameOverObject ) );
			if( m_pGameOverObject )
				m_pGameOverObject->setVisible( false );
		}
	}
	
	if( m_pOwnerOKButtonObject )
		m_pOwnerOKButtonObject->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------




