//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMainMenuScreen.h"

#include "ComponentAchievementBanner.h"
#include "ComponentGlobals.h"
#include "ComponentHeartScreen.h"
#include "ComponentMyScoreScreen.h"

#include "iPhoneInterfaceWrapper.h"
#include "GameCenterWrapper.h"
#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szTutorialLevelName[128]		= "game/data/levels/Screen_TutorialOpening.t2d";
static const char g_szStoryOpeningLevelName[128]	= "game/data/levels/Screen_StoryOpening.t2d";
static const char g_szMapSelectLevelName[128]		= "game/data/levels/Screen_MapSelect_PowerUp.t2d";

static const char g_szVineKing_TitleBG[32] = "VineKing_TitleBG";
static t2dStaticSprite* g_pTitleBG = NULL;

// Version Number display
static const char g_szVersionNumber[16] = "VersionNumber";
static t2dTextObject* g_pVersionNumber = NULL;

// Eyes
static const char g_szVineKingEye_Small[32] = "VineKingEye_Small";
static t2dStaticSprite* g_pVineKingEye_Small = NULL;
static const t2dVector g_vPositionVineKingEye_Small( -115.084, 69.888 );

static const char g_szVineKingEye_Large[32] = "VineKingEye_Large";
static t2dStaticSprite* g_pVineKingEye_Large = NULL;
static const t2dVector g_vPositionVineKingEye_Large( -99.015, 69.755 );

// Credits
static const char g_szCredits1ImageMap[32] = "Credits1ImageMap";
//static const char g_szCredits2ImageMap[32] = "Credits2ImageMap";
static const char g_szCredits_BG[16] = "Credits_BG";
static t2dStaticSprite* g_pCreditsPage = NULL;
static S32 g_iCurrentCreditsPage = 0;

static t2dSceneObject* g_pCreditsBackButton = NULL;
static const char g_szCredits_Next_Button[32] = "Credits_Next_Button";

static t2dSceneObject* g_pMyScoresButton = NULL;
static const char g_szMyScoresButton[32] = "MyScores_Text";

// Heart Button
static t2dSceneObject* g_pHeartButton = NULL;
static const char g_szHeartButton[32] = "Heart_Button";

static F32 g_fCurrentHeartScale		= 1.0f;
static const F32 g_fMinHeartScale	= 1.0f;
static const F32 g_fMaxHeartScale	= 1.5f;
static const F32 g_fHeartScaleRate	= 1.0f;
static bool g_bHeartScalingUp = true;

static t2dVector g_vOriginalHeartButtonSize( 32.0f, 32.0f );
t2dVector g_vTempHeartSize;

// Facebook button
static t2dStaticSprite* g_pFacebookLoginButton = NULL;
static const char g_szFaceBook_Options_Button[32] = "FaceBook_Options_Button";

static const char g_szFacebookButtonImageMap[32] = "FB_LOGINOUT_64x64ImageMap";
static const S32 g_iFacebookButtonImageFrame_Login = 0;
static const S32 g_iFacebookButtonImageFrame_Logout = 1;

// Fade
static const char g_szBlackBox[32] = "BlackBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

extern bool g_bAllLevelsUnlocked;

// Scene Window - For setting the camera position
static const char g_szSceneWindow[32] = "sceneWindow2D";
static t2dSceneWindow* g_pSceneWindow = NULL;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMainMenuScreen* CComponentMainMenuScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMainMenuScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMainMenuScreen::CComponentMainMenuScreen()
	: m_pOwner( NULL )
	, m_pNewGameButton( NULL )
	, m_pContinueButton( NULL )
	, m_pBackButton( NULL )
	, m_eScreenMode( SCREEN_MODE_NONE )
	, m_bLoadingIntoTutorial( false )
	, m_bFadingToMyScoreScreen( false )
	, m_bFadingToHeartScreen( false )
	, m_bSaveDataExists( false )
	, m_bFading( false )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentMainMenuScreen::sm_pInstance = NULL;
	m_szSelectedLevelName[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMainMenuScreen::~CComponentMainMenuScreen()
{
	CComponentMainMenuScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::StartMainMenuScreen()
{
	// This function should only be called once, in order to open it up for the first time after a completed PostInit
	
	CComponentGlobals::GetInstance().LoadGameSettings();
	
	m_bSaveDataExists = CComponentGlobals::GetInstance().DoesSaveDataExist();
	
	//HideOptionsMenuObjects();
	//ShowMainMenuObjects();
	OpenMainMenuScreen();
	
	CSoundManager::GetInstance().PlayBGMMainMenuRPG();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnNewGameButtonPressed() 
{	
	if( g_bAllLevelsUnlocked )
	{
		strcpy( m_szSelectedLevelName, g_szMapSelectLevelName );
	}
	else
	{
		strcpy( m_szSelectedLevelName, g_szStoryOpeningLevelName );
	}
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_bFading = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnContinueButtonPressed()
{
	strcpy( m_szSelectedLevelName, g_szMapSelectLevelName );
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_bFading = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnOptionsButtonPressed()
{
	//printf( "CComponentMainMenuScreen::OnOptionsButtonPressed\n" );
	
	//HideMainMenuObjects();
	//ShowOptionsMenuObjects();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnTutorialButtonPressed()
{
	m_bLoadingIntoTutorial = true;
	
    //printf( "m_szSelectedLevelName: %s\n", m_szSelectedLevelName );
	strcpy( m_szSelectedLevelName, g_szTutorialLevelName );
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_bFading = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnCreditsNextButtonPressed()
{
	if( g_pCreditsPage == NULL )
		return;
	
	CloseCreditsScreen();
	OpenOptionsScreen();
	
	//if( g_iCurrentCreditsPage == 0 )
	//{
	//	g_pCreditsPage->setImageMap( g_szCredits2ImageMap, 0 );
	//	++g_iCurrentCreditsPage;
	//}
	//else if( g_iCurrentCreditsPage == 1 )
	//{
	//	CloseCreditsScreen();
	//	OpenOptionsScreen();
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnGameCenterButtonPressed()
{
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_bFading = true;
	
	m_bFadingToMyScoreScreen = true;
	m_bFadingToHeartScreen = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnGameCenterResetAchievementsButtonPressed()
{
	GameCenterWrapper::ResetAchievements();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnMyScoresButtonPressed()
{
	//printf( "CComponentMainMenuScreen::OnMyScoresButtonPressed\n" );
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_bFading = true;
	
	m_bFadingToMyScoreScreen = true;
	m_bFadingToHeartScreen = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnHeartButtonPressed()
{
	//printf( "CComponentMainMenuScreen::OnHeartButtonPressed\n" );
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_bFading = true;
	
	m_bFadingToHeartScreen = true;
	m_bFadingToMyScoreScreen = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnFacebookLoginButtonPressed()
{
	//printf( "CComponentMainMenuScreen::OnFacebookLoginButtonPressed\n" );
	
	// The Facebook Login button is necessary in case the user does not have the Facebook app installed
	
	if( g_pFacebookLoginButton )
	{
		if( FacebookWrapper::IsLoggedIn() )
			FacebookWrapper::LogOut();
		else
			FacebookWrapper::LogIn();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMainMenuScreen::OnFacebookUserLoggedIn()
{
	if( g_pFacebookLoginButton )
		g_pFacebookLoginButton->setImageMap( g_szFacebookButtonImageMap, g_iFacebookButtonImageFrame_Logout );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMainMenuScreen::OnFacebookUserLoggedOut()
{
	if( g_pFacebookLoginButton )
		g_pFacebookLoginButton->setImageMap( g_szFacebookButtonImageMap, g_iFacebookButtonImageFrame_Login );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnMuteButtonPressed()
{
	if( CSoundManager::GetInstance().IsSoundMuted() )
	{
		CSoundManager::GetInstance().UnmuteAllSound();
		
		if( m_pMuteButton )
		{
			t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pMuteButton );
			if( pSprite )
				pSprite->setImageMap( "buttons_newImageMap", 1 );
		}
	}
	else
	{
		CSoundManager::GetInstance().MuteAllSound();
		
		if( m_pMuteButton )
		{
			t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pMuteButton );
			if( pSprite )
				pSprite->setImageMap( "buttons_newImageMap", 3 );
		}
	}
	
	CComponentGlobals::GetInstance().IncrementNumTimesMuteButtonPressed();
	
	if( CComponentGlobals::GetInstance().GetNumTimesMuteButtonPressed() >= 1 )
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SHH );
	
	if( CComponentGlobals::GetInstance().GetNumTimesMuteButtonPressed() >= 5 )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_MASTER_OF_SILENCE );
	}
	else
	{
		F32 fPercent = (CComponentGlobals::GetInstance().GetNumTimesMuteButtonPressed() / 5.0f );
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_MASTER_OF_SILENCE, fPercent );
		//printf( "Percentage for VK_MASTER_OF_SILENCE: %f\n", fPercent );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnClearDataButtonPressed()
{
	CComponentGlobals::GetInstance().ClearSaveGameSettings();
	
	CloseClearDataScreen();
	OpenOptionsScreen();
	
	CSoundManager::GetInstance().PlayHomeBaseDeathSound();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnBackButtonPressed()
{
	m_bSaveDataExists = CComponentGlobals::GetInstance().DoesSaveDataExist(); // This call needs to be here as the player may have Cleared the Save Data
	
	switch( m_eScreenMode )
	{
		//case SCREEN_MODE_MAINMENU:
		//	break;
			
		case SCREEN_MODE_OPTIONS:
			CloseOptionsScreen();
			OpenMainMenuScreen();
			break;
			
		case SCREEN_MODE_CREDITS:
			CloseCreditsScreen();
			OpenOptionsScreen();
			break;
			
		case SCREEN_MODE_CLEARDATA:
			CloseClearDataScreen();
			OpenOptionsScreen();
			break;
			
		default:
			break;
	}
	
	//HideOptionsMenuObjects();
	//ShowMainMenuObjects();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OpenMainMenuScreenFromMyScoresScreen()
{
	if( m_pOwner && g_pSceneWindow )
	{
		g_pSceneWindow->mount( m_pOwner, t2dVector::getZero(), 0.0f, true );
	}
	
	if( g_pFadeBox )
	{
		g_pFadeBox->setVisible( true );
		g_pFadeBox->setBlendAlpha( 1.0f );
		g_pFadeBox->setPosition( m_pOwner->getPosition() );
		m_eFadeState = FADE_STATE_FADING_IN;
		m_fFadeTimer = g_fFadeTimeSeconds;
		m_bFading = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OpenMainMenuScreenFromHeartScreen()
{
	if( m_pOwner && g_pSceneWindow )
	{
		g_pSceneWindow->mount( m_pOwner, t2dVector::getZero(), 0.0f, true );
	}
	
	if( g_pFadeBox )
	{
		g_pFadeBox->setVisible( true );
		g_pFadeBox->setBlendAlpha( 1.0f );
		g_pFadeBox->setPosition( m_pOwner->getPosition() );
		m_eFadeState = FADE_STATE_FADING_IN;
		m_fFadeTimer = g_fFadeTimeSeconds;
		m_bFading = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OpenMainMenuScreen()
{
	for( S32 i = 0; i < m_MainMenuElements.size(); ++i )
	{
		if( m_MainMenuElements[i] )
			m_MainMenuElements[i]->setVisible( true );
	}
	
	if( m_bSaveDataExists )
	{
		if( m_pNewGameButton )
			m_pNewGameButton->setVisible( false );
		
		if( m_pContinueButton )
			m_pContinueButton->setVisible( true );
	}
	else
	{
		if( m_pNewGameButton )
			m_pNewGameButton->setVisible( true );
		
		if( m_pContinueButton )
			m_pContinueButton->setVisible( false );
	}
	
	if( m_pBackButton )
		m_pBackButton->setVisible( false );
	
	if( m_pGameCenterButton )
	{
		if( GameCenterWrapper::IsGameCenterAvailableOnDevice() )
			m_pGameCenterButton->setVisible( true );
		else
			m_pGameCenterButton->setVisible( false );
	}
	
	if( g_pMyScoresButton )
		g_pMyScoresButton->setVisible( true );
	
	if( g_pHeartButton )
		g_pHeartButton->setVisible( true );
	
	m_eScreenMode = SCREEN_MODE_MAINMENU;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::CloseMainMenuScreen()
{
	for( S32 i = 0; i < m_MainMenuElements.size(); ++i )
	{
		if( m_MainMenuElements[i] )
			m_MainMenuElements[i]->setVisible( false );
	}
	
	if( m_pNewGameButton )
		m_pNewGameButton->setVisible( false );
	
	if( m_pContinueButton )
		m_pContinueButton->setVisible( false );
	
	if( m_pBackButton )
		m_pBackButton->setVisible( true );
	
	if( g_pMyScoresButton )
		g_pMyScoresButton->setVisible( false );
	
	if( g_pHeartButton )
		g_pHeartButton->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OpenOptionsScreen()
{
	for( S32 i = 0; i < m_OptionsScreenElements.size(); ++i )
	{
		if( m_OptionsScreenElements[i] )
			m_OptionsScreenElements[i]->setVisible( true );
	}
	
	if( m_pMuteButton )
		m_pMuteButton->setVisible( true );
	
	if( m_pGameCenterButton )
		m_pGameCenterButton->setVisible( false );
	
	if( m_pBackButton )
		m_pBackButton->setVisible( true );
	
	if( g_pFacebookLoginButton )
	{
		if( FacebookWrapper::IsLoggedIn() )
		{			
			// Show "LogOut" image
			g_pFacebookLoginButton->setImageMap( g_szFacebookButtonImageMap, g_iFacebookButtonImageFrame_Logout );
		}
		else
		{
			// Show "LogIn" image
			g_pFacebookLoginButton->setImageMap( g_szFacebookButtonImageMap, g_iFacebookButtonImageFrame_Login );
		}

		g_pFacebookLoginButton->setVisible( true );
	}
	
	m_eScreenMode = SCREEN_MODE_OPTIONS;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::CloseOptionsScreen()
{
	for( S32 i = 0; i < m_OptionsScreenElements.size(); ++i )
	{
		if( m_OptionsScreenElements[i] )
			m_OptionsScreenElements[i]->setVisible( false );
	}
	
	if( m_pMuteButton )
		m_pMuteButton->setVisible( false );
	
	if( g_pFacebookLoginButton )
		g_pFacebookLoginButton->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OpenCreditsScreen()
{
	for( S32 i = 0; i < m_CreditsScreenElements.size(); ++i )
	{
		if( m_CreditsScreenElements[i] )
			m_CreditsScreenElements[i]->setVisible( true );
	}
	
	m_eScreenMode = SCREEN_MODE_CREDITS;
	
	g_iCurrentCreditsPage = 0;
	
	if( m_pBackButton )
		m_pBackButton->setVisible( false );
	
	if( g_pCreditsPage )
		g_pCreditsPage->setImageMap( g_szCredits1ImageMap, 0 );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::CloseCreditsScreen()
{
	for( S32 i = 0; i < m_CreditsScreenElements.size(); ++i )
	{
		if( m_CreditsScreenElements[i] )
			m_CreditsScreenElements[i]->setVisible( false );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OpenClearDataScreen()
{
	for( S32 i = 0; i < m_ClearDataScreenElements.size(); ++i )
	{
		if( m_ClearDataScreenElements[i] )
			m_ClearDataScreenElements[i]->setVisible( true );
	}
	
	m_eScreenMode = SCREEN_MODE_CLEARDATA;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::CloseClearDataScreen()
{
	for( S32 i = 0; i < m_ClearDataScreenElements.size(); ++i )
	{
		if( m_ClearDataScreenElements[i] )
			m_ClearDataScreenElements[i]->setVisible( false );
	}
	
	OpenOptionsScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMainMenuScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMainMenuScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMainMenuScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentMainMenuScreen::sm_pInstance = this;
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::onUpdate()
{
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
	
	// Make the heart pulsate
//	if( g_pHeartButton && g_pHeartButton->getVisible() )
//	{
//		if( g_bHeartScalingUp )
//		{
//			g_fCurrentHeartScale += (g_fHeartScaleRate * ITickable::smTickSec);
//			if( g_fCurrentHeartScale >= g_fMaxHeartScale )
//			{
//				g_bHeartScalingUp = false;
//				g_fCurrentHeartScale = g_fMaxHeartScale;
//			}
//			
//			g_vTempHeartSize.mX = g_vOriginalHeartButtonSize.mX * g_fCurrentHeartScale;
//			g_vTempHeartSize.mY = g_vOriginalHeartButtonSize.mY * g_fCurrentHeartScale;
//			g_pHeartButton->setSize( g_vTempHeartSize );
//		}
//		else
//		{
//			g_fCurrentHeartScale -= (g_fHeartScaleRate * ITickable::smTickSec);
//			if( g_fCurrentHeartScale <= g_fMinHeartScale )
//			{
//				g_bHeartScalingUp = true;
//				g_fCurrentHeartScale = g_fMinHeartScale;
//			}
//			
//			g_vTempHeartSize.mX = g_vOriginalHeartButtonSize.mX * g_fCurrentHeartScale;
//			g_vTempHeartSize.mY = g_vOriginalHeartButtonSize.mY * g_fCurrentHeartScale;
//			g_pHeartButton->setSize( g_vTempHeartSize );
//		}
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnPostInit()
{
	if( m_pOwner )
		m_pOwner->setPosition( t2dVector::getZero() );
	
	m_MainMenuElements.clear();
	m_OptionsScreenElements.clear();
	m_CreditsScreenElements.clear();
	m_ClearDataScreenElements.clear();
	
	g_pTitleBG = static_cast<t2dStaticSprite*>( Sim::findObject( g_szVineKing_TitleBG ) );
	if( g_pTitleBG )
		g_pTitleBG->setPosition( t2dVector::getZero() );
	
	g_pVineKingEye_Small = static_cast<t2dStaticSprite*>( Sim::findObject( g_szVineKingEye_Small ) );
	if( g_pVineKingEye_Small )
		g_pVineKingEye_Small->setPosition( g_vPositionVineKingEye_Small );
	
	g_pVineKingEye_Large = static_cast<t2dStaticSprite*>( Sim::findObject( g_szVineKingEye_Large ) );
	if( g_pVineKingEye_Large )
		g_pVineKingEye_Large->setPosition( g_vPositionVineKingEye_Large );
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szBlackBox ) );
	if( g_pFadeBox )
	{
		g_pFadeBox->setPosition( t2dVector::getZero() );
		m_eFadeState = FADE_STATE_FADING_IN;
		m_fFadeTimer = g_fFadeTimeSeconds;
		m_bFading = true;
	}
	
	//static const char g_szVersionNumber[16] = "VersionNumber";
	//static t2dSceneObject* g_pVersionNumber = NULL;
	g_pVersionNumber = static_cast<t2dTextObject*>( Sim::findObject( g_szVersionNumber ) );
	if( g_pVersionNumber )
	{
		char szBuffer[32];
		sprintf( szBuffer, "v%d.%d", g_iVersionNumber_Major, g_iVersionNumber_Minor );
		g_pVersionNumber->setText( szBuffer );
	}
	
	g_iCurrentCreditsPage = 0;
	
	g_pCreditsPage = static_cast<t2dStaticSprite*>( Sim::findObject( g_szCredits_BG ) );
	g_pCreditsBackButton = static_cast<t2dSceneObject*>( Sim::findObject( g_szCredits_Next_Button ) );
	
	g_pMyScoresButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScoresButton ) );
	
	g_pHeartButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szHeartButton ) );
	if( g_pHeartButton )
	{
		g_vOriginalHeartButtonSize.mX = g_pHeartButton->getSize().mX;
		g_vOriginalHeartButtonSize.mY = g_pHeartButton->getSize().mY;
	}
	
	g_pFacebookLoginButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFaceBook_Options_Button ) );
	
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( g_szSceneWindow ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHODS

void CComponentMainMenuScreen::UpdateFade()
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

void CComponentMainMenuScreen::OnFadeOutFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	if( m_bFadingToMyScoreScreen )
	{
		m_bFadingToMyScoreScreen = false;
		CComponentMyScoreScreen::OpenMyScoreScreen();
		return;
	}
	
	if( m_bFadingToHeartScreen )
	{
		m_bFadingToHeartScreen = false;
		CComponentHeartScreen::OpenHeartScreen();
		return;
	}
	
	if( m_bLoadingIntoTutorial )
		CComponentGlobals::GetInstance().ScheduleLoadLevelNoSave( m_szSelectedLevelName );
	else
		CComponentGlobals::GetInstance().ScheduleLoadLevel( m_szSelectedLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreen::OnFadeInFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	m_bFading = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
