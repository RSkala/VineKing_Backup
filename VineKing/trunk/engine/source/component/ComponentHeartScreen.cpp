//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentHeartScreen.h"

#include "ComponentMainMenuScreen.h"

#include "platformiPhone/GameCenterWrapper.h"
#include "platformiPhone/iPhoneInterfaceWrapper.h"

#include "T2D/t2dSceneWindow.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Scene Window - For setting the camera position
static const char g_szSceneWindow[32] = "sceneWindow2D";
static t2dSceneWindow* g_pSceneWindow = NULL;

// Elements
static const char g_szHeartScreen_BackButton[32] = "HeartScreen_BackButton";
static t2dStaticSprite* g_pHeartScreen_BackButton = NULL;

static const char g_szHeartScreen_RateButtonn[32] = "HeartScreen_RateButton";
static t2dStaticSprite* g_pHeartScreen_RateButton = NULL;

static const char g_szHeartScreen_FacebookButton[32] = "HeartScreen_FacebookButton";
static t2dStaticSprite* g_pHeartScreen_FacebookButton = NULL;

static const char g_szHeartScreen_WebSiteButton[32] = "HeartScreen_WebSiteButton";
static t2dStaticSprite* g_pHeartScreen_WebSiteButton = NULL;

static const char g_szHeartScreen_TwitterButton[32] = "HeartScreen_TwitterButton";
static t2dStaticSprite* g_pHeartScreen_TwitterButton = NULL;

// Fade Stuff
static const char g_szFadeBox[32] = "HeartScreen_FadeBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHeartScreen* CComponentHeartScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentHeartScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHeartScreen::CComponentHeartScreen()
	: m_pOwner( NULL )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentHeartScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHeartScreen::~CComponentHeartScreen()
{
	CComponentHeartScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::OpenHeartScreen()
{
	//printf( "CComponentHeartScreen::OpenHeartScreen\n" );
	
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_pOwner == NULL )
		return;
	
	if( g_pSceneWindow )
		g_pSceneWindow->mount( sm_pInstance->m_pOwner, t2dVector::getZero(), 0.0f, true );
	
	sm_pInstance->m_eFadeState = FADE_STATE_FADING_IN;
	sm_pInstance->m_fFadeTimer = g_fFadeTimeSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::OnHeartScreenButtonPressed_Back()
{
	//printf( "CComponentHeartScreen::OnHeartScreenButtonPressed_Back\n" );
	
	if( sm_pInstance )
		sm_pInstance->m_eFadeState = FADE_STATE_FADING_OUT;
	
	if( g_pHeartScreen_BackButton )
		g_pHeartScreen_BackButton->setUseMouseEvents( false );
	
	if( g_pHeartScreen_RateButton )
		g_pHeartScreen_RateButton->setUseMouseEvents( false );
	
	if( g_pHeartScreen_FacebookButton )
		g_pHeartScreen_FacebookButton->setUseMouseEvents( false );
	
	if( g_pHeartScreen_WebSiteButton )
		g_pHeartScreen_WebSiteButton->setUseMouseEvents( false );
	
	if( g_pHeartScreen_TwitterButton )
		g_pHeartScreen_TwitterButton->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::OnHeartScreenButtonPressed_AppStore()
{
	//printf( "CComponentHeartScreen::OnHeartScreenButtonPressed_AppStore\n" );
	
	if( GameCenterWrapper::HasAchievementBeenEarned( GameCenterWrapper::VK_RATE_US ) == false )
	{
		GameCenterWrapper::ReportAchievementNoBanner( GameCenterWrapper::VK_RATE_US );
	}
	
	AppStoreLinkWrapper::SendUserToVineKingRatingsPage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::OnHeartScreenButtonPressed_Facebook()
{
	//printf( "CComponentHeartScreen::OnHeartScreenButtonPressed_Facebook\n" );
	AppStoreLinkWrapper::SendUserToVineKingFacebookPage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::OnHeartScreenButtonPressed_MoreVineKing()
{
	//printf( "CComponentHeartScreen::OnHeartScreenButtonPressed_MoreVineKing\n" );
	AppStoreLinkWrapper::SendUserToVineKingWebSite();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::OnHeartScreenButtonPressed_Twitter()
{
	//printf( "CComponentHeartScreen::OnHeartScreenButtonPressed_Twitter\n" );
	AppStoreLinkWrapper::SendUserToVineKingTwitterPage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHeartScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentHeartScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentHeartScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentHeartScreen::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreen::onUpdate()
{
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreen::OnPostInit()
{
	// Scene Window
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( g_szSceneWindow ) );
	
	// Fade Box
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
		g_pFadeBox->setPosition( m_pOwner->getPosition() );
	
	g_pHeartScreen_BackButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szHeartScreen_BackButton ) );
	g_pHeartScreen_RateButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szHeartScreen_RateButtonn ) );
	g_pHeartScreen_FacebookButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szHeartScreen_FacebookButton ) );
	g_pHeartScreen_WebSiteButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szHeartScreen_WebSiteButton ) );
	g_pHeartScreen_TwitterButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szHeartScreen_TwitterButton ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private

void CComponentHeartScreen::UpdateFade()
{
	if( g_pFadeBox == NULL )
		return;
	
	switch( m_eFadeState )
	{
		case FADE_STATE_FADING_IN: // Fading In (Turning to transparent)
			
			m_fFadeTimer -= ITickable::smTickSec;
			if( m_fFadeTimer <= 0.0f )
				m_fFadeTimer = 0.0f;
			
			m_fFadeAmount = m_fFadeTimer / g_fFadeTimeSeconds;
			g_pFadeBox->setBlendAlpha( m_fFadeAmount );
			
			if( m_fFadeAmount <= 0.0f )
				OnFadeInFinished();
			
			break;
			
		case FADE_STATE_FADING_OUT: // Fading Out (Turning to black)
			
			m_fFadeTimer += ITickable::smTickSec;
			if( m_fFadeTimer >= g_fFadeTimeSeconds )
				m_fFadeTimer = g_fFadeTimeSeconds;
			
			m_fFadeAmount = m_fFadeTimer / g_fFadeTimeSeconds;
			g_pFadeBox->setBlendAlpha( m_fFadeAmount );
			
			if( m_fFadeAmount >= 1.0f )
				OnFadeOutFinished();
			
			break;
			
		default:
			break;	
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreen::OnFadeOutFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	CComponentMainMenuScreen::GetInstance().OpenMainMenuScreenFromHeartScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHeartScreen::OnFadeInFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	if( g_pHeartScreen_BackButton )
		g_pHeartScreen_BackButton->setUseMouseEvents( true );
	
	if( g_pHeartScreen_RateButton )
		g_pHeartScreen_RateButton->setUseMouseEvents( true );
	
	if( g_pHeartScreen_FacebookButton )
		g_pHeartScreen_FacebookButton->setUseMouseEvents( true );
	
	if( g_pHeartScreen_WebSiteButton )
		g_pHeartScreen_WebSiteButton->setUseMouseEvents( true );
	
	if( g_pHeartScreen_TwitterButton )
		g_pHeartScreen_TwitterButton->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------