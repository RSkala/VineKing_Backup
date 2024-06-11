//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentHeartScreenElement.h"

#include "ComponentHeartScreen.h"

#include "SoundManager.h"

#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentHeartScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHeartScreenElement::CComponentHeartScreenElement()
	: m_pOwner( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHeartScreenElement::~CComponentHeartScreenElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentHeartScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentHeartScreenElement::onComponentAdd( SimComponent* _pTarget )
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

void CComponentHeartScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_UnpressedColor );
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_BUTTON_BACK:
			CSoundManager::GetInstance().PlayButtonClickBack();
			CComponentHeartScreen::OnHeartScreenButtonPressed_Back();
			break;
			
		case ELEMENT_TYPE_BUTTON_RATE:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentHeartScreen::OnHeartScreenButtonPressed_AppStore();
			break;
			
		case ELEMENT_TYPE_BUTTON_FACEBOOK:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentHeartScreen::OnHeartScreenButtonPressed_Facebook();
			break;
			
		case ELEMENT_TYPE_BUTTON_MORE_VINEKING:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentHeartScreen::OnHeartScreenButtonPressed_MoreVineKing();
			break;
			
		case ELEMENT_TYPE_BUTTON_TWITTER:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentHeartScreen::OnHeartScreenButtonPressed_Twitter();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreenElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	if( strcmp( m_pOwner->getName(), "HeartScreen_BackButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_BACK;
		m_pOwner->setUseMouseEvents( false );
	}
	else if( strcmp( m_pOwner->getName(), "HeartScreen_RateButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_RATE;
		m_pOwner->setUseMouseEvents( false );
	}
	else if( strcmp( m_pOwner->getName(), "HeartScreen_FacebookButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_FACEBOOK;
		m_pOwner->setUseMouseEvents( false );
	}
	else if( strcmp( m_pOwner->getName(), "HeartScreen_WebSiteButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_MORE_VINEKING;
		m_pOwner->setUseMouseEvents( false );
	}
	else if( strcmp( m_pOwner->getName(), "HeartScreen_TwitterButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_TWITTER;
		m_pOwner->setUseMouseEvents( false );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
