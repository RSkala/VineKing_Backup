//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentStoryOpeningScreen.h"

#include "ComponentGlobals.h"
#include "ComponentStoryOpeningScreenElement.h"
#include "ComponentTutorialOpeningScreen.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szFirstLevelName[64]	= "game/data/levels/Level_01_01.t2d";
//static const char g_szFirstLevelName[64]	= "game/data/levels/Level_00_00.t2d";
static const char g_szTutorialLevelName[64] = "game/data/levels/Screen_TutorialOpening.t2d";

// Fade stuff
static const char g_szFadeBox[32] = "FadeBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.25f;


static const char g_szStoryScreen01[32] = "StoryScreen01";
static const char g_szStoryScreen02[32] = "StoryScreen02";
static const char g_szStoryScreen03[32] = "StoryScreen03";
static const char g_szStoryScreen04[32] = "StoryScreen04";
static const char g_szStoryScreen05[32] = "StoryScreen05";
static const char g_szStoryScreen06[32] = "StoryScreen06";
static const char g_szStoryScreen07[32] = "StoryScreen07";

static t2dStaticSprite* g_pStoryScreen01 = NULL;
static t2dStaticSprite* g_pStoryScreen02 = NULL;
static t2dStaticSprite* g_pStoryScreen03 = NULL;
static t2dStaticSprite* g_pStoryScreen04 = NULL;
static t2dStaticSprite* g_pStoryScreen05 = NULL;
static t2dStaticSprite* g_pStoryScreen06 = NULL;
static t2dStaticSprite* g_pStoryScreen07 = NULL;

static const t2dVector g_vSafePosition( 1000.0f, 1000.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryOpeningScreen* CComponentStoryOpeningScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentStoryOpeningScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryOpeningScreen::CComponentStoryOpeningScreen()
	: m_pOwner( NULL )
	, m_iCurrentStoryImage( 0 )
	, m_pElementButtonNext( NULL )
	, m_pElementButtonViewTutorialYES( NULL )
	, m_pElementButtonViewTutorialNO( NULL )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentStoryOpeningScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryOpeningScreen::~CComponentStoryOpeningScreen()
{
	CComponentStoryOpeningScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::StartStoryOpeningScreen()
{
	if( sm_pInstance == NULL )
		return;
	
	PlayOpeningStoryMusic();
	
	if( sm_pInstance->m_pElementButtonNext )
		sm_pInstance->m_pElementButtonNext->Enable();
	
	if( sm_pInstance->m_pElementButtonViewTutorialYES )
		sm_pInstance->m_pElementButtonViewTutorialYES->Disable();
	
	if( sm_pInstance->m_pElementButtonViewTutorialNO )
		sm_pInstance->m_pElementButtonViewTutorialNO->Disable();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::PlayOpeningStoryMusic()
{
	CSoundManager::GetInstance().PlayStoryOpeningBGM();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::SetButtonNext( CComponentStoryOpeningScreenElement* const _pObject )
{
	if( sm_pInstance == NULL )
		return;
	
	sm_pInstance->m_pElementButtonNext = _pObject;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::SetButtonViewTutorialYES( CComponentStoryOpeningScreenElement* const _pObject )
{
	if( sm_pInstance == NULL )
		return;
	
	sm_pInstance->m_pElementButtonViewTutorialYES = _pObject;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::SetButtonViewTutorialNO( CComponentStoryOpeningScreenElement* const _pObject )
{
	if( sm_pInstance == NULL )
		return;
	
	sm_pInstance->m_pElementButtonViewTutorialNO = _pObject;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::OnButtonNextPressed()
{
	if( sm_pInstance == NULL )
		return;
	
	sm_pInstance->m_eFadeState = FADE_STATE_FADING_OUT;
	sm_pInstance->m_fFadeTimer = 0.0f;
	sm_pInstance->m_fFadeAmount = 0.0f;
	
	if( sm_pInstance->m_pElementButtonNext )
		sm_pInstance->m_pElementButtonNext->DisableMouseEvents();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::OnButtonViewTutorialYESPressed()
{
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_pElementButtonNext )
		sm_pInstance->m_pElementButtonNext->Disable();
	
	if( sm_pInstance->m_pElementButtonViewTutorialYES )
		sm_pInstance->m_pElementButtonViewTutorialYES->Disable();
	
	if( sm_pInstance->m_pElementButtonViewTutorialNO )
		sm_pInstance->m_pElementButtonViewTutorialNO->Disable();
	
	CComponentTutorialOpeningScreen::SetLoadedFromStoryScreen();
	
	CComponentGlobals::GetInstance().SetActiveLevelSaveIndex( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 );
	//CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szTutorialLevelName );
	//CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( g_szTutorialLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::OnButtonViewTutorialNOPressed()
{
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_pElementButtonNext )
		sm_pInstance->m_pElementButtonNext->Disable();
	
	if( sm_pInstance->m_pElementButtonViewTutorialYES )
		sm_pInstance->m_pElementButtonViewTutorialYES->Disable();
	
	if( sm_pInstance->m_pElementButtonViewTutorialNO )
		sm_pInstance->m_pElementButtonViewTutorialNO->Disable();
	
	CComponentGlobals::GetInstance().SetActiveLevelSaveIndex( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 );
	//CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szFirstLevelName );
	CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( g_szFirstLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentStoryOpeningScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentStoryOpeningScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentStoryOpeningScreen::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreen::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_eFadeState != FADE_STATE_NONE )
	{
		UpdateFade();
		
		//if( g_pFadeBox )
		//	printf( "Alpha: %f\n", g_pFadeBox->getBlendAlpha() );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreen::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_iCurrentStoryImage = 1;

	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
		g_pFadeBox->setPosition( t2dVector::getZero() );

	g_pStoryScreen01 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen01 ) );
	if( g_pStoryScreen01 )
		g_pStoryScreen01->setPosition( t2dVector::getZero() );
	
	g_pStoryScreen02 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen02 ) );
	g_pStoryScreen03 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen03 ) );
	g_pStoryScreen04 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen04 ) );
	g_pStoryScreen05 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen05 ) );
	g_pStoryScreen06 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen06 ) );
	g_pStoryScreen07 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryScreen07 ) );
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentStoryOpeningScreen::UpdateFade()
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

void CComponentStoryOpeningScreen::OnFadeOutFinished()
{
	//printf( "CComponentStoryOpeningScreen::OnFadeOutFinished()\n" );
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds; // Set to g_fFadeTimeSeconds because the Fade Box alpha will go from 1.0 -> 0.0
	
	if( g_pFadeBox) 
		g_pFadeBox->setBlendAlpha( 1.0f );
	
	++m_iCurrentStoryImage;
	
	switch( m_iCurrentStoryImage )
	{
		case 1:
			// Code should never get here
			printf( "CComponentStoryOpeningScreen::OnFadeOutFinished - Invalid switch case: 1\n" );
			break;
			
		case 2:
			if( g_pStoryScreen01 )
				g_pStoryScreen01->setPosition( g_vSafePosition );
			
			if( g_pStoryScreen02 )
				g_pStoryScreen02->setPosition( t2dVector::getZero() );
			break;
			
		case 3:
			if( g_pStoryScreen02 )
				g_pStoryScreen02->setPosition( g_vSafePosition );
			
			if( g_pStoryScreen03 )
				g_pStoryScreen03->setPosition( t2dVector::getZero() );
			break;
			
		case 4:
			if( g_pStoryScreen03 )
				g_pStoryScreen03->setPosition( g_vSafePosition );
			
			if( g_pStoryScreen04 )
				g_pStoryScreen04->setPosition( t2dVector::getZero() );
			break;
			
		case 5:
			if( g_pStoryScreen04 )
				g_pStoryScreen04->setPosition( g_vSafePosition );
			
			if( g_pStoryScreen05 )
				g_pStoryScreen05->setPosition( t2dVector::getZero() );
			break;
			
		case 6:
			if( g_pStoryScreen05 )
				g_pStoryScreen05->setPosition( g_vSafePosition );
			
			if( g_pStoryScreen06 )
				g_pStoryScreen06->setPosition( t2dVector::getZero() );
			break;
			
		case 7:
			if( g_pStoryScreen06 )
				g_pStoryScreen06->setPosition( g_vSafePosition );
			
			if( g_pStoryScreen07 )
				g_pStoryScreen07->setPosition( t2dVector::getZero() );
			break;
			
		case 8:
			CComponentGlobals::GetInstance().SetActiveLevelSaveIndex( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 );
			CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( g_szFirstLevelName );
			break;
			
		default:
			printf( "CComponentStoryOpeningScreen::OnFadeOutFinished - Unhandled switch case: %d\n", m_iCurrentStoryImage );
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreen::OnFadeInFinished()
{
	//printf( "CComponentStoryOpeningScreen::OnFadeInFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;  // Set to 0.0f because the Fade Box alpha will go from 0.0f -> 1.0 on Fade Out
	
	if( g_pFadeBox )
		g_pFadeBox->setBlendAlpha( 0.0f );
	
	if( m_pElementButtonNext )
		m_pElementButtonNext->EnableMouseEvents();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
