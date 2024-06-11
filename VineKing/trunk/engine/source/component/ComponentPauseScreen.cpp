//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentPauseScreen.h"

#include "ComponentAchievementBanner.h"
#include "ComponentGlobals.h"
#include "ComponentLineDrawAttack.h"

#include "SoundManager.h"

#include "platformiPhone/GameCenterWrapper.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTextObject.h"

static const char g_szButtonsImageMap[32] = "buttons_newImageMap";

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPauseScreen* CComponentPauseScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentPauseScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPauseScreen::CComponentPauseScreen()
	: m_pOwner( NULL )
	, m_pSceneGraph( NULL )
	, m_pPauseButton( NULL )
	, m_pPauseMenuImage( NULL )
	, m_pConfirmQuitMenuImage( NULL )
	, m_pResumeButton( NULL )
	, m_pResumeButtonPressedImage( NULL )
	, m_pMuteButton( NULL )
	, m_pMuteOnOffImage( NULL )
	, m_pQuitButton( NULL )
	, m_pQuitButtonPressedImage( NULL )
	, m_pConfirmQuitButtonYes( NULL )
	, m_pConfirmQuitButtonYesPressedImage( NULL )
	, m_pConfirmQuitButtonNo( NULL )
	, m_pConfrimQuitButtonNoPressedImage( NULL )
	, m_pResumeText( NULL )
	, m_pMuteText( NULL )
	, m_pQuitText( NULL )
	, m_pQuitConfirmText1( NULL )
	, m_pQuitConfirmText2( NULL )
	, m_pQuitConfirmYesText( NULL )
	, m_pQuitConfirmNoText( NULL )

	, m_pResume( NULL )
	, m_pMap( NULL )
	, m_pMute( NULL )

	, m_pQuitBG( NULL )
	, m_pQuitYes( NULL )
	, m_pQuitNo( NULL )

	, m_bIsPaused( false )
	, m_bSoundMuted( false )

	, m_bQuittingToMapScreen( false )
{
	CComponentPauseScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPauseScreen::~CComponentPauseScreen()
{
	CComponentPauseScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::InitializePauseScreenElements()
{
	if( m_pOwner )
	{
		t2dVector vDefaultPos( 0.0f, 0.0f );
		m_pOwner->setPosition( vDefaultPos );
	}
	
	HidePauseMenuElements();
	HideConfirmQuitMenuElements();
	
	///////////////////////////////////////////////////
	// New Shit 2011/01/02
	HidePauseScreen();
	HideQuitScreen();
	///////////////////////////////////////////////////
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnPauseButtonPressed()
{	
	//printf( "Pausing the game...\n" );
	
	m_bIsPaused = true;
	
	if( m_pSceneGraph )
	{
		m_pSceneGraph->setScenePause( true );
		Con::setVariable( "$timeScale", "0" );
	}
	
	UnhidePauseMenuElements();
	HideConfirmQuitMenuElements();
	
	///////////////////////////////////////////////////
	// New Shit 2011/01/02
	UnhidePauseScreen();
	HideQuitScreen();
	///////////////////////////////////////////////////
	
	if( m_pResumeButtonPressedImage )
		m_pResumeButtonPressedImage->setVisible( false );
	
	if( m_pQuitButtonPressedImage )
		m_pQuitButtonPressedImage->setVisible( false );
	
	if( m_pConfirmQuitButtonYesPressedImage )
		m_pConfirmQuitButtonYesPressedImage->setVisible( false );

	if( m_pConfrimQuitButtonNoPressedImage )
		m_pConfrimQuitButtonNoPressedImage->setVisible( false );
	
	if( m_pPauseButton )
		m_pPauseButton->setUseMouseEvents( false );
	
	//CComponentLineDrawAttack::OnInvalidLineDrawAreaTouched();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnResumeButtonPressed()
{
	if( m_bIsPaused )
	{
		//printf( "Unpausing the game...\n" );
		
		m_bIsPaused = false;
		
		if( m_pSceneGraph )
		{
			m_pSceneGraph->setScenePause( false );
			Con::setVariable( "$timeScale", "1" );
		}
		
		HidePauseMenuElements();
		HideConfirmQuitMenuElements();
		
		///////////////////////////////////////////////////
		// New Shit 2011/01/02
		HidePauseScreen();
		HideQuitScreen();
		///////////////////////////////////////////////////
		
		if( m_pPauseButton )
			m_pPauseButton->setUseMouseEvents( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnMuteButtonPressed()
{
	if( CSoundManager::GetInstance().IsSoundMuted() )
	{
		if( m_pMuteOnOffImage )
			m_pMuteOnOffImage->setVisible( false );
		
		CSoundManager::GetInstance().UnmuteAllSound();
	}
	else 
	{
		if( m_pMuteOnOffImage )
			m_pMuteOnOffImage->setVisible( true );
		
		CSoundManager::GetInstance().MuteAllSound();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnQuitButtonPressed()
{
	HidePauseMenuElements();
	UnhideConfirmQuitMenuElements();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnConfirmQuitYesButtonPressed()
{
	
	if( m_pPauseButton )
		m_pPauseButton->setUseMouseEvents( false );
	
	if( m_pResumeButton )
		m_pResumeButton->setUseMouseEvents( false );
	
	if( m_pMuteButton )
		m_pMuteButton->setUseMouseEvents( false );
	
	if( m_pQuitButton )
		m_pQuitButton->setUseMouseEvents( false );
	
	if( m_pConfirmQuitButtonYes )
		m_pConfirmQuitButtonYes->setUseMouseEvents( false );
	
	if( m_pConfirmQuitButtonNo )
		m_pConfirmQuitButtonNo->setUseMouseEvents( false );
	
	m_bQuittingToMapScreen = true;
	
	printf( "Exiting level...\n" );
	m_pSceneGraph->setScenePause( false );
	Con::setVariable( "$timeScale", "1" );
	
	char szLevelName[128] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";
	CComponentGlobals::GetInstance().ScheduleLoadLevel( szLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnConfirmQuitNoButtonPressed()
{
	HideConfirmQuitMenuElements();
	UnhidePauseMenuElements();
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HideResumeButtonPressedImage()
{
	if( m_pResumeButtonPressedImage )
		m_pResumeButtonPressedImage->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhideResumeButtonPressedImage()
{
	if( m_pResumeButtonPressedImage )
		m_pResumeButtonPressedImage->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HideQuitButtonPressedImage()
{
	if( m_pQuitButtonPressedImage )
		m_pQuitButtonPressedImage->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhideQuitButtonPressedImage()
{
	if( m_pQuitButtonPressedImage )
		m_pQuitButtonPressedImage->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HideConfirmQuitYesPressedImage()
{
	if( m_pConfirmQuitButtonYesPressedImage )
		m_pConfirmQuitButtonYesPressedImage->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhideConfirmQuitYesPressedImage()
{
	if( m_pConfirmQuitButtonYesPressedImage )
		m_pConfirmQuitButtonYesPressedImage->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HideConfirmQuitNoPressedImage()
{
	if( m_pConfrimQuitButtonNoPressedImage )
		m_pConfrimQuitButtonNoPressedImage->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhideConfirmQuitNoPressedImage()
{
	if( m_pConfrimQuitButtonNoPressedImage )
		m_pConfrimQuitButtonNoPressedImage->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnResumePressed()
{
	if( m_bIsPaused )
	{
		//printf( "Unpausing the game...\n" );
		
		m_bIsPaused = false;
		
		if( m_pSceneGraph )
		{
			m_pSceneGraph->setScenePause( false );
			Con::setVariable( "$timeScale", "1" );
		}
		
		HidePauseScreen();
		HideQuitScreen();
		
		if( m_pPauseButton )
			m_pPauseButton->setUseMouseEvents( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnMapPressed()
{
	HidePauseScreen();
	UnhideQuitScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnMutePressed()
{
	if( m_pMute == NULL )
		return;
	
	if( CSoundManager::GetInstance().IsSoundMuted() )
	{
		CSoundManager::GetInstance().UnmuteAllSound();
		t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pMute );
		if( pSprite )
			pSprite->setImageMap( g_szButtonsImageMap, 1 );
	}
	else
	{
		CSoundManager::GetInstance().MuteAllSound();
		t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pMute );
		if( pSprite )
			pSprite->setImageMap( g_szButtonsImageMap, 3 );
	}
	
	//CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SHH );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnQuitYesPressed()
{
	HideQuitScreen();
	HidePauseScreen();
	
	//printf( "Exiting level...\n" );
	m_bQuittingToMapScreen = true;
	
	if( m_pSceneGraph )
		m_pSceneGraph->setScenePause( false );
	
	Con::setVariable( "$timeScale", "1" );
	
	char szLevelName[128] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";
	CComponentGlobals::GetInstance().ScheduleLoadLevel( szLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnQuitNoPressed()
{
	HideQuitScreen();
	UnhidePauseScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HidePauseScreen()
{
	if( m_pOwner )
		m_pOwner->setVisible( false );
	
	if( m_pResume )
		m_pResume->setVisible( false );
	
	if( m_pMap )
		m_pMap->setVisible( false );
	
	if( m_pMute )
		m_pMute->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhidePauseScreen()
{
	if( m_pOwner )
		m_pOwner->setVisible( true );
	
	if( m_pResume )
		m_pResume->setVisible( true );
	
	if( m_pMap )
		m_pMap->setVisible( true );
	
	if( m_pMute )
	{
		m_pMute->setVisible( true );
	
		if( CSoundManager::GetInstance().IsSoundMuted() )
		{
			t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pMute );
			if( pSprite )
				pSprite->setImageMap( g_szButtonsImageMap, 3 );
		}
		else
		{
			t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pMute );
			if( pSprite )
				pSprite->setImageMap( g_szButtonsImageMap, 1 );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HideQuitScreen()
{
	if( m_pQuitBG )
		m_pQuitBG->setVisible( false );
	
	if( m_pQuitYes )
		m_pQuitYes->setVisible( false );
	
	if( m_pQuitNo )
		m_pQuitNo->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhideQuitScreen()
{
	if( m_pQuitBG )
		m_pQuitBG->setVisible( true );
	
	if( m_pQuitYes )
		m_pQuitYes->setVisible( true );
	
	if( m_pQuitNo )
		m_pQuitNo->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPauseScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentPauseScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPauseScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentPauseScreen::sm_pInstance = this;
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setPosition( t2dVector::getZero() );
	
	m_pOwner->setVisible( false );
	
	m_pOwner->setLayer( LAYER_PAUSE_SCREENS );
	
	HidePauseMenuElements();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HidePauseMenuElements()
{
	if( m_pPauseMenuImage )
		m_pPauseMenuImage->setVisible( false );
	
	if( m_pResumeButton )
		m_pResumeButton->setVisible( false );
	
	if( m_pMuteButton )
		m_pMuteButton->setVisible( false );
	
	if( m_pQuitButton )
		m_pQuitButton->setVisible( false );
	
	//if( m_pButtonPressedImage )
	//	m_pButtonPressedImage->setVisible( false );
	
	if( m_pMuteOnOffImage )
		m_pMuteOnOffImage->setVisible( false );
	
	if( m_pResumeText )
		m_pResumeText->setVisible( false );
	
	if( m_pMuteText )
		m_pMuteText->setVisible( false );
	
	if( m_pQuitText )
		m_pQuitText->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhidePauseMenuElements()
{
	if( m_pPauseMenuImage )
		m_pPauseMenuImage->setVisible( true );
	
	if( m_pResumeButton )
		m_pResumeButton->setVisible( true );
	
	if( m_pMuteButton )
		m_pMuteButton->setVisible( true );
	
	if( m_pQuitButton )
		m_pQuitButton->setVisible( true );
	
	//if( m_pButtonPressedImage )
	//	m_pButtonPressedImage->setVisible( true );
	
	if( m_pMuteOnOffImage )
	{
		if( CSoundManager::GetInstance().IsSoundMuted() )
			m_pMuteOnOffImage->setVisible( true );
		else 
			m_pMuteOnOffImage->setVisible( false );
		
	}
	
	if( m_pResumeText )
	{
		m_pResumeText->setVisible( true );
	}
	
	if( m_pMuteText )
	{
		m_pMuteText->setVisible( true );
	}
	
	if( m_pQuitText )
	{
		m_pQuitText->setVisible( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::HideConfirmQuitMenuElements()
{
	if( m_pConfirmQuitMenuImage )
		m_pConfirmQuitMenuImage->setVisible( false );

	if( m_pConfirmQuitButtonYes )
	{
		m_pConfirmQuitButtonYes->setVisible( false );
		m_pConfirmQuitButtonYes->setUseMouseEvents( false );
	}

	if( m_pConfirmQuitButtonNo )
	{
		m_pConfirmQuitButtonNo->setVisible( false );
		m_pConfirmQuitButtonNo->setUseMouseEvents( false );
	}
	
	if( m_pQuitConfirmText1 )
		m_pQuitConfirmText1->setVisible( false );
	
	if( m_pQuitConfirmText2 )
		m_pQuitConfirmText2->setVisible( false );
	
	if( m_pQuitConfirmYesText )
		m_pQuitConfirmYesText->setVisible( false );
	
	if( m_pQuitConfirmNoText )
		m_pQuitConfirmNoText->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreen::UnhideConfirmQuitMenuElements()
{
	if( m_pConfirmQuitMenuImage )
		m_pConfirmQuitMenuImage->setVisible( true );
	
	if( m_pConfirmQuitButtonYes )
	{
		m_pConfirmQuitButtonYes->setVisible( true );
		m_pConfirmQuitButtonYes->setUseMouseEvents( true );
	}
	
	if( m_pConfirmQuitButtonNo )
	{
		m_pConfirmQuitButtonNo->setVisible( true );
		m_pConfirmQuitButtonNo->setUseMouseEvents( true );
	}
	
	if( m_pQuitConfirmText1 )
		m_pQuitConfirmText1->setVisible( true );
	
	if( m_pQuitConfirmText2 )
		m_pQuitConfirmText2->setVisible( true );
	
	if( m_pQuitConfirmYesText )
		m_pQuitConfirmYesText->setVisible( true );
	
	if( m_pQuitConfirmNoText )
		m_pQuitConfirmNoText->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
