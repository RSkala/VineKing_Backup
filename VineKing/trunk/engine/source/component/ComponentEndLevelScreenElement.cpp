//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEndLevelScreenElement.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentScoringManager.h"

#include "SoundManager.h"

#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEndLevelScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEndLevelScreenElement::CComponentEndLevelScreenElement()
	: m_pOwner( NULL )
	, m_pszElementType( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEndLevelScreenElement::~CComponentEndLevelScreenElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEndLevelScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEndLevelScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "ElementType", TypeString, Offset( m_pszElementType, CComponentEndLevelScreenElement ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEndLevelScreenElement::onComponentAdd( SimComponent* _pTarget )
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
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_UnpressedColor );
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	CComponentEndLevelScreen::GetInstance().OnOKButtonPressed();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreenElement::OnMouseDragged( const t2dVector& _vWorldMousePoint ) // NO LONGER USED
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreenElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	ConvertElementTypeStringToEnum();
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_YOUWIN:
			m_pOwner->setVisible( false );
			CComponentEndLevelScreen::GetInstance().SetYouWinTextObject( m_pOwner );
			break;
			
		case ELEMENT_TYPE_GAMEOVER:
			m_pOwner->setVisible( false );
			CComponentEndLevelScreen::GetInstance().SetGameOverTextObject( m_pOwner );
			break;
			
		case ELEMENT_TYPE_OK_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentEndLevelScreen::GetInstance().SetOKButtonObject( m_pOwner );
			break;
			
		default:
			break;
	}
	
	m_pOwner->setLayer( LAYER_YOU_WIN_GAME_OVER_DISPLAY );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHODS

void CComponentEndLevelScreenElement::ConvertElementTypeStringToEnum()
{
	if( m_pszElementType == NULL )
		return;
	
	//%endLevelScreenElementTypes = "YOU_WIN" TAB "GAMEOVER" TAB "OK_BUTTON";
	
	if( dStricmp( m_pszElementType, "YOU_WIN" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_YOUWIN;
	}
	else if( dStricmp( m_pszElementType, "GAMEOVER" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_GAMEOVER;
	}
	else if( dStricmp( m_pszElementType, "OK_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_OK_BUTTON;
	}
}

