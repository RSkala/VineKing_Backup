//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLogoScreen.h"

#include "ComponentGlobals.h"

#include "core/iTickable.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const F32 g_fLogoScreenTimeMS = 3000.0f; //2500.0f; // How long the game stays on the Logo Screen (in Milliseconds)

static const char g_szLevelName[128] = "game/data/levels/Screen_MainMenu.t2d";

// Torque Logo
//static const char g_szTorqueLogo[16] = "TorqueLogo";
//static t2dSceneObject* g_pTorqueLogo = NULL;
static bool g_bDisplayingTorqueLogo = false;

static const char g_szLogoName01[] = "TorqueLogoImageMap";
static const char g_szLogoName02[] = "WWMGImageMap";

// Fade stuff
static const char g_szFadeBox[32] = "FadeBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLogoScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLogoScreen::CComponentLogoScreen()
	: m_pOwner( NULL )
	, m_fTimer( 0.0f )
	, m_bTimerEnabled( false )
	, m_bStopUpdating( false )
    , m_bShowing2ndLogo( false )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLogoScreen::~CComponentLogoScreen()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLogoScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentLogoScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLogoScreen::onComponentAdd( SimComponent* _pTarget )
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

void CComponentLogoScreen::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_bStopUpdating )
		return;
	
	if( m_bTimerEnabled == false )
	{
		UpdateFade();
	}
	else if( m_bTimerEnabled )
	{
		m_fTimer += ITickable::smTickMs;
		if( m_fTimer >= g_fLogoScreenTimeMS )
		{
			m_eFadeState = FADE_STATE_FADING_OUT;
			m_fFadeTimer = 0.0f;
			m_bTimerEnabled = false;
			m_fTimer = 0.0f;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLogoScreen::OnPostInit()
{
	//g_pTorqueLogo = static_cast<t2dSceneObject*>( Sim::findObject( g_szTorqueLogo ) );
	//if( g_pTorqueLogo )
	//	g_pTorqueLogo->setVisible( false );
	
	g_bDisplayingTorqueLogo = true;
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox && m_pOwner )
	{
		g_pFadeBox->setPosition( t2dVector::getZero() );
		
		m_eFadeState = FADE_STATE_FADING_IN;
		m_fFadeTimer = g_fFadeTimeSeconds;
	}
    
    m_bShowing2ndLogo = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHODS

void CComponentLogoScreen::UpdateFade()
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

void CComponentLogoScreen::OnFadeOutFinished()
{
    if( m_bShowing2ndLogo )
    {
		CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( g_szLevelName );
		m_bStopUpdating = true;
		m_eFadeState = FADE_STATE_NONE;
    }
    else 
    {
        if( m_pOwner )
        {
            t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>(m_pOwner);
            if( pSprite )
            {
                pSprite->setImageMap( g_szLogoName01, 0 );
            }
        }
        
		m_eFadeState = FADE_STATE_FADING_IN;
		m_fFadeTimer = g_fFadeTimeSeconds;
        
        m_bShowing2ndLogo = true;
    }
    
//	if( g_bDisplayingTorqueLogo )
//	{
//		CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( g_szLevelName );
//		m_bStopUpdating = true;
//		m_eFadeState = FADE_STATE_NONE;
//	}
//	else
//	{
//		if( m_pOwner )
//			m_pOwner->setVisible( false );
//		
//		//if( g_pTorqueLogo )
//		//	g_pTorqueLogo->setVisible( true );
//		
//		m_eFadeState = FADE_STATE_FADING_IN;
//		m_fFadeTimer = g_fFadeTimeSeconds;
//		
//		g_bDisplayingTorqueLogo = true;
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLogoScreen::OnFadeInFinished()
{
	m_bTimerEnabled = true;
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------