//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentTutorialOpeningScreen.h"

#include "ComponentAchievementBanner.h"
#include "ComponentGlobals.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "platformiPhone/GameCenterWrapper.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static const char g_szTutorialObject01[32] = "TutorialObject01";
static const char g_szTutorialObject02[32] = "TutorialObject02";
static const char g_szTutorialObject03[32] = "TutorialObject03";
static const char g_szTutorialObject04[32] = "TutorialObject04";
static const char g_szTutorialObject05[32] = "TutorialObject05";
static const char g_szTutorialObject06[32] = "TutorialObject06";
static const char g_szTutorialObject07[32] = "TutorialObject07";
static const char g_szTutorialObject08[32] = "TutorialObject08";
static const char g_szTutorialObject09[32] = "TutorialObject09";
static const char g_szTutorialObject10[32] = "TutorialObject10";

static t2dStaticSprite* g_pTutorialObject01 = NULL;
static t2dStaticSprite* g_pTutorialObject02 = NULL;
static t2dStaticSprite* g_pTutorialObject03 = NULL;
static t2dStaticSprite* g_pTutorialObject04 = NULL;
//static t2dStaticSprite* g_pTutorialObject05 = NULL;
//static t2dStaticSprite* g_pTutorialObject06 = NULL;
//static t2dStaticSprite* g_pTutorialObject07 = NULL;
//static t2dStaticSprite* g_pTutorialObject08 = NULL;
//static t2dStaticSprite* g_pTutorialObject09 = NULL;
//static t2dStaticSprite* g_pTutorialObject10 = NULL;

static const t2dVector g_vSafePosition( 1000.0f, 1000.0f );

static const char g_szFirstLevelName[64]	= "game/data/levels/Level_01_01.t2d";
//static const char g_szFirstLevelName[64]	= "game/data/levels/Level_00_00.t2d";
static const char g_szMainMenuLevelName[64]	= "game/data/levels/Screen_MainMenu.t2d";

static const t2dVector g_vCornerButtonPosition( 108.0f, 171.0f ); // Position of button (actually the owner)

// Fade stuff
static const char g_szFadeBox[32] = "FadeBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.25f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialOpeningScreen* CComponentTutorialOpeningScreen::sm_pInstance = NULL;
bool CComponentTutorialOpeningScreen::sm_bLoadedFromMainMenu = false;
bool CComponentTutorialOpeningScreen::sm_bLoadedFromStoryScreen = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentTutorialOpeningScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialOpeningScreen::CComponentTutorialOpeningScreen()
	: m_pOwner( NULL )
	, m_iCurrentTutorialImage( 0 )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentTutorialOpeningScreen::sm_pInstance = NULL;
	//CComponentTutorialOpeningScreen::sm_bLoadedFromMainMenu = false;
	//CComponentTutorialOpeningScreen::sm_bLoadedFromStoryScreen = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialOpeningScreen::~CComponentTutorialOpeningScreen()
{
	CComponentTutorialOpeningScreen::sm_pInstance = NULL;
	//CComponentTutorialOpeningScreen::sm_bLoadedFromMainMenu = false;
	//CComponentTutorialOpeningScreen::sm_bLoadedFromStoryScreen = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialOpeningScreen::PlayOpeningTutorialMusic()
{
	CSoundManager::GetInstance().PlayBGMMapScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialOpeningScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentTutorialOpeningScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTutorialOpeningScreen::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dStaticSprite* pOwnerObject = dynamic_cast<t2dStaticSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s - Must be added to a t2dStaticSprite.", __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	CComponentTutorialOpeningScreen::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_fFadeTimer = 0.0f;
	m_fFadeAmount = 0.0f;
	
	m_pOwner->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( true );
	m_pOwner->setPosition( g_vCornerButtonPosition );
	
	m_iCurrentTutorialImage = 1;
	
	g_pTutorialObject01 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject01 ) );
	if( g_pTutorialObject01 )
		g_pTutorialObject01->setPosition( t2dVector::getZero() );
	
	g_pTutorialObject02 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject02 ) ); 
	g_pTutorialObject03 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject03 ) );
	g_pTutorialObject04 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject04 ) );
	//g_pTutorialObject05 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject05 ) );
	//g_pTutorialObject06 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject06 ) );
	//g_pTutorialObject07 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject07 ) );
	//g_pTutorialObject08 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject08 ) );
	//g_pTutorialObject09 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject09 ) );
	//g_pTutorialObject10 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialObject10 ) );
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
		g_pFadeBox->setPosition( t2dVector::getZero() );
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentTutorialOpeningScreen::UpdateFade()
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

void CComponentTutorialOpeningScreen::OnFadeOutFinished()
{
	//printf( "CComponentTutorialOpeningScreen::OnFadeOutFinished()\n" );
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds; // Set to g_fFadeTimeSeconds because the Fade Box alpha will go from 1.0 -> 0.0
	
	if( g_pFadeBox) 
		g_pFadeBox->setBlendAlpha( 1.0f );
	
	++m_iCurrentTutorialImage;
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	switch( m_iCurrentTutorialImage )
	{
		case 1:
			// Code should never get here
			break;
			
		case 2:
			if( g_pTutorialObject01 )
				g_pTutorialObject01->setPosition( g_vSafePosition );
			
			if( g_pTutorialObject02 )
				g_pTutorialObject02->setPosition( t2dVector::getZero() );
			break;
			
		case 3:
			if( g_pTutorialObject02 )
				g_pTutorialObject02->setPosition( g_vSafePosition );
			
			if( g_pTutorialObject03 )
				g_pTutorialObject03->setPosition( t2dVector::getZero() );
			break;
			
		case 4:
			if( g_pTutorialObject03 )
				g_pTutorialObject03->setPosition( g_vSafePosition );
			
			if( g_pTutorialObject04 )
				g_pTutorialObject04->setPosition( t2dVector::getZero() );
			break;
			
//		case 5:
//			if( g_pTutorialObject04 )
//				g_pTutorialObject04->setPosition( g_vSafePosition );
//			
//			if( g_pTutorialObject05 )
//				g_pTutorialObject05->setPosition( t2dVector::getZero() );
//			break;
//			
//		case 6:
//			if( g_pTutorialObject05 )
//				g_pTutorialObject05->setPosition( g_vSafePosition );
//			
//			if( g_pTutorialObject06 )
//				g_pTutorialObject06->setPosition( t2dVector::getZero() );
//			break;
//			
//		case 7:
//			if( g_pTutorialObject06 )
//				g_pTutorialObject06->setPosition( g_vSafePosition );
//			
//			if( g_pTutorialObject07 )
//				g_pTutorialObject07->setPosition( t2dVector::getZero() );
//			break;
//			
//		case 8:
//			if( g_pTutorialObject07 )
//				g_pTutorialObject07->setPosition( g_vSafePosition );
//			
//			if( g_pTutorialObject08 )
//				g_pTutorialObject08->setPosition( t2dVector::getZero() );
//			break;
//			
//		case 9:
//			if( g_pTutorialObject08 )
//				g_pTutorialObject08->setPosition( g_vSafePosition );
//			
//			if( g_pTutorialObject09 )
//				g_pTutorialObject09->setPosition( t2dVector::getZero() );
//			break;
//			
//		case 10:
//			if( g_pTutorialObject09 )
//				g_pTutorialObject09->setPosition( g_vSafePosition );
//			
//			if( g_pTutorialObject10 )
//				g_pTutorialObject10->setPosition( t2dVector::getZero() );
//			break;
			
		//case 11:
		case 5:
			CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( g_szMainMenuLevelName );
			CComponentTutorialOpeningScreen::sm_bLoadedFromMainMenu = false;
			CComponentTutorialOpeningScreen::sm_bLoadedFromStoryScreen = false;
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialOpeningScreen::OnFadeInFinished()
{
	//printf( "CComponentTutorialOpeningScreen::OnFadeInFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;  // Set to 0.0f because the Fade Box alpha will go from 0.0f -> 1.0 on Fade Out
	
	if( g_pFadeBox )
		g_pFadeBox->setBlendAlpha( 0.0f );
	
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( true );
	
	if( m_iCurrentTutorialImage == 1 )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_BOOK_WORM );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
