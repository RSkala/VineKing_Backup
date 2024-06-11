//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMyScoreScreenElement.h"

#include "ComponentGlobals.h"
#include "ComponentMyScoreScreen.h"
#include "GameCenterWrapper.h"

#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMyScoreScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMyScoreScreenElement::CComponentMyScoreScreenElement()
	: m_pOwner( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMyScoreScreenElement::~CComponentMyScoreScreenElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMyScoreScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMyScoreScreenElement::onComponentAdd( SimComponent* _pTarget )
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

void CComponentMyScoreScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_UnpressedColor );
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_BUTTON_LEFT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Left();
			break;
			
		case ELEMENT_TYPE_BUTTON_RIGHT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Right();
			break;
		
		case ELEMENT_TYPE_BUTTON_BACK:
			CSoundManager::GetInstance().PlayButtonClickBack();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Back();
			break;
			
		case ELEMENT_TYPE_BUTTON_LEADERBOARD:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Leaderboard();
			break;
			
		case ELEMENT_TYPE_BUTTON_ACHIEVEMENTS:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Achievements();
			break;
			
		case ELEMENT_TYPE_BUTTON_FACEBOOK:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Facebook();
			break;
            
        case ELEMENT_TYPE_BUTTON_TWITTER:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Twitter();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreenElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	if( strcmp( m_pOwner->getName(), "MyScores_ButtonLeft" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_LEFT;
	}
	else if( strcmp( m_pOwner->getName(), "MyScores_ButtonRight" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_RIGHT;
	}
	else if( strcmp( m_pOwner->getName(), "MyScores_BackButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_BACK;
	}
	else if( strcmp( m_pOwner->getName(), "MyScores_LeaderboardButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_LEADERBOARD;
        
        if( GameCenterWrapper::IsGameCenterAvailableOnDevice() == false )
            m_pOwner->setVisible( false );
	}
	else if( strcmp( m_pOwner->getName(), "MyScores_AchievementsButton" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_ACHIEVEMENTS;
        
        if( GameCenterWrapper::IsGameCenterAvailableOnDevice() == false )
            m_pOwner->setVisible( false );
	}
	else if( strcmp( m_pOwner->getName(), "MyScores_Facebook_Button" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_FACEBOOK;
        
        if( CComponentGlobals::GetInstance().IsOldIOSDeviceType() )
            m_pOwner->setVisible( false );
	}
    else if( strcmp( m_pOwner->getName(), "MyScores_Twitter_Button" ) == 0 )
    {
        m_eElementType = ELEMENT_TYPE_BUTTON_TWITTER;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------