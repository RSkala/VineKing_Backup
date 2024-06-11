//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMyScoreScreen.h"

#include "ComponentGlobals.h"
#include "ComponentMainMenuScreen.h"


#include "platformiPhone/GameCenterWrapper.h"
#include "platformiPhone/iPhoneInterfaceWrapper.h"

#include "T2D/t2dSceneWindow.h"
//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Screen Elements
static t2dTextObject* g_pTitleText = NULL;


static const char g_szMyScores_ButtonLeft[32] = "MyScores_ButtonLeft";
static t2dStaticSprite* g_pMyScores_ButtonLeft = NULL;

static const char g_szMyScores_ButtonRight[32] = "MyScores_ButtonRight";
static t2dStaticSprite* g_pMyScores_ButtonRight = NULL;

static const char g_szMyScores_BackButton[32] = "MyScores_BackButton";
static t2dStaticSprite* g_pMyScores_BackButton = NULL;

static const char g_szMyScores_LeaderboardButton[32] = "MyScores_LeaderboardButton";
static t2dStaticSprite* g_pMyScores_LeaderboardButton = NULL;

static const char g_szMyScores_AchievementsButton[32] = "MyScores_AchievementsButton";
static t2dStaticSprite* g_pMyScores_AchievementsButton = NULL;

static const char g_szMyScores_Facebook_Button[32] = "MyScores_Facebook_Button";
static t2dStaticSprite* g_pMyScores_Facebook_Button = NULL;

static const char g_szMyScores_Twitter_Button[32] = "MyScores_Twitter_Button";
static t2dStaticSprite* g_pMyScores_Twitter_Button = NULL;


// Text Objects - Labels
static const char g_szMyScores_PageTitle[32] = "MyScores_PageTitle";
static t2dTextObject* g_pMyScores_PageTitle = NULL;

static const char g_szMyScores_Label_Level01[32] = "MyScores_Label_Level01";
static t2dTextObject* g_pMyScores_Label_Level01 = NULL;

static const char g_szMyScores_Label_Level02[32] = "MyScores_Label_Level02";
static t2dTextObject* g_pMyScores_Label_Level02 = NULL;

static const char g_szMyScores_Label_Level03[32] = "MyScores_Label_Level03";
static t2dTextObject* g_pMyScores_Label_Level03 = NULL;

static const char g_szMyScores_Label_Level04[32] = "MyScores_Label_Level04";
static t2dTextObject* g_pMyScores_Label_Level04 = NULL;

static const char g_szMyScores_Label_Total[32] = "MyScores_Label_Total";
static t2dTextObject* g_pMyScores_Label_Total = NULL;

static const char g_szMyScores_Label_Underscores[32] = "MyScores_Label_Underscores";
static t2dTextObject* g_pMyScores_Label_Underscores = NULL;


// Text Object - Scores
static const char g_szMyScores_Totals[32] = "MyScores_Totals";
static t2dTextObject* g_pMyScores_Totals = NULL;


// Text Objects - Times
static const char g_szMyScores_Times01[32] = "MyScores_Times01";
static t2dTextObject* g_pMyScores_Times01 = NULL;

static const char g_szMyScores_Times02[32] = "MyScores_Times02";
static t2dTextObject* g_pMyScores_Times02 = NULL;

static const char g_szMyScores_Times03[32] = "MyScores_Times03";
static t2dTextObject* g_pMyScores_Times03 = NULL;

static const char g_szMyScores_Times04[32] = "MyScores_Times04";
static t2dTextObject* g_pMyScores_Times04 = NULL;

static const char g_szMyScores_Times_Total[32] = "MyScores_Times_Total";
static t2dTextObject* g_pMyScores_Times_Total = NULL;

// Text Object - Rank
static const char g_szMyScores_GlobalRank[32] = "MyScores_GlobalRank";
static t2dTextObject* g_pMyScores_GlobalRank = NULL;


// Scene Window - For setting the camera position
static const char g_szSceneWindow[32] = "sceneWindow2D";
static t2dSceneWindow* g_pSceneWindow = NULL;

// Fade Stuff
static const char g_szFadeBox[32] = "MyScores_FadeBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

// Messages
static const char g_szPointBasedMessage[64]			= "I collected %d %s in VineKing!";
static const char g_szTimeBasedMessage[64]			= "I completed World %d in %s in VineKing!";
static const char g_szLeaderboardRankedMessage[32]	= "I am ranked #%d!";

static U32 g_uCurrentlyDisplayedTotal = 0; // This number can apply to any total (tallies or time)


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMyScoreScreen* CComponentMyScoreScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void _GetTimeDisplayString( char*, const U32 );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMyScoreScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMyScoreScreen::CComponentMyScoreScreen()
	: m_pOwner( NULL )
	, m_iCurrentScreenPage( SCREEN_PAGE_LIFE_GROWN )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentMyScoreScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMyScoreScreen::~CComponentMyScoreScreen()
{
	CComponentMyScoreScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OpenMyScoreScreen()
{
	//printf( "CComponentMyScoreScreen::OpenMyScoreScreen\n" );
	
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_pOwner == NULL )
		return;
	
	if( g_pSceneWindow )
		g_pSceneWindow->mount( sm_pInstance->m_pOwner, t2dVector::getZero(), 0.0f, true );
	
	sm_pInstance->m_eFadeState = FADE_STATE_FADING_IN;
	sm_pInstance->m_fFadeTimer = g_fFadeTimeSeconds;
	
	//if( g_pTitleText )
	//{
	//	g_pTitleText->setText( g_szTitleStatistics );
	//}
	
	if( g_pMyScores_ButtonLeft )
		g_pMyScores_ButtonLeft->setUseMouseEvents( true );
		
	if( g_pMyScores_ButtonRight )
		g_pMyScores_ButtonRight->setUseMouseEvents( true );
		
	if( g_pMyScores_BackButton )
		g_pMyScores_BackButton->setUseMouseEvents( true );
		
	if( g_pMyScores_LeaderboardButton )
		g_pMyScores_LeaderboardButton->setUseMouseEvents( true );
		
	if( g_pMyScores_AchievementsButton )
		g_pMyScores_AchievementsButton->setUseMouseEvents( true );
	
	if( g_pMyScores_Facebook_Button )
		g_pMyScores_Facebook_Button->setUseMouseEvents( true );
    
    if( g_pMyScores_Twitter_Button )
        g_pMyScores_Twitter_Button->setUseMouseEvents( false );
	
	sm_pInstance->m_iCurrentScreenPage = SCREEN_PAGE_LIFE_GROWN;
	sm_pInstance->HandlePageChange();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Left()
{
	//printf( "CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Left\n" );
	
	if( sm_pInstance )
		sm_pInstance->DecrementPage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Right()
{
	//printf( "CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Right\n" );
	
	if( sm_pInstance )
		sm_pInstance->IncrementPage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Back()
{
	//printf( "CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Back\n" );
	
	if( sm_pInstance )
		sm_pInstance->m_eFadeState = FADE_STATE_FADING_OUT;
	
	if( g_pMyScores_ButtonLeft )
		g_pMyScores_ButtonLeft->setUseMouseEvents( false );
	
	if( g_pMyScores_ButtonRight	)
		g_pMyScores_ButtonRight->setUseMouseEvents( false );
		
	if( g_pMyScores_BackButton )
		g_pMyScores_BackButton->setUseMouseEvents( false );
	
	if( g_pMyScores_LeaderboardButton )
		g_pMyScores_LeaderboardButton->setUseMouseEvents( false );
	
	if( g_pMyScores_AchievementsButton )
		g_pMyScores_AchievementsButton->setUseMouseEvents( false );
	
	if( g_pMyScores_Facebook_Button )
		g_pMyScores_Facebook_Button->setUseMouseEvents( false );
    
    if( g_pMyScores_Twitter_Button )
		g_pMyScores_Twitter_Button->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Leaderboard()
{
	//printf( "CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Leaderboard\n" );
	
	if( sm_pInstance == NULL )
		return;
	
	GameCenterWrapper::EGameCenterLeaderboards eLeaderboardID = GameCenterWrapper::VK_LB_TOP_LIFE;
	
	switch( sm_pInstance->m_iCurrentScreenPage )
	{
		case SCREEN_PAGE_LIFE_GROWN:			eLeaderboardID = GameCenterWrapper::VK_LB_TOP_LIFE; break;
		case SCREEN_PAGE_MONSTERS_DEFEATED:		eLeaderboardID = GameCenterWrapper::VK_LB_TOP_MONSTER; break;
		case SCREEN_PAGE_SEEDS_GATHERED:		eLeaderboardID = GameCenterWrapper::VK_LB_TOP_SEED; break;
		case SCREEN_PAGE_COINS_COLLECTED:		eLeaderboardID = GameCenterWrapper::VK_LB_TOP_COINS; break;
			
		case SCREEN_PAGE_TIMES_WORLD_2:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W2; break;
		case SCREEN_PAGE_TIMES_WORLD_3:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W3; break;
		case SCREEN_PAGE_TIMES_WORLD_4:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W4; break;
		case SCREEN_PAGE_TIMES_BOSS_1:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_B1; break;
		case SCREEN_PAGE_TIMES_WORLD_5:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W5; break;
		case SCREEN_PAGE_TIMES_WORLD_6:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W6; break;
		case SCREEN_PAGE_TIMES_WORLD_7:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W7; break;
		case SCREEN_PAGE_TIMES_BOSS_2:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_B2; break;
		case SCREEN_PAGE_TIMES_WORLD_8:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W8; break;
		case SCREEN_PAGE_TIMES_WORLD_9:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W9; break;
		case SCREEN_PAGE_TIMES_WORLD_10:		eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W10; break;
		case SCREEN_PAGE_TIMES_BOSS_3:			eLeaderboardID = GameCenterWrapper::VK_LB_TIME_B3; break;
		case SCREEN_PAGE_TIMES_WORLD_11_1:		eLeaderboardID = GameCenterWrapper::VK_LB_TIME_W11_1; break;
			
		default:
			printf( "Unhandled screen page: %d\n", (S32)(sm_pInstance->m_iCurrentScreenPage) );
			eLeaderboardID = GameCenterWrapper::VK_LB_TOP_LIFE;
			break;
	}
	
	GameCenterWrapper::DisplayLeaderboard( (S32)eLeaderboardID );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Achievements()
{
	//printf( "CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Achievements\n" );
	
	GameCenterWrapper::DisplayAchievements();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Facebook()
{
	if( sm_pInstance == NULL /*|| g_pMyScores_PageTitle == NULL || g_pMyScores_Times_Total == NULL*/ )
		return;
	
	char szStringBuffer[256];
	char szTemp[32];
	
	S32 iRank = -1;

	switch( sm_pInstance->m_iCurrentScreenPage )
	{
		case SCREEN_PAGE_LIFE_GROWN:
			sprintf( szStringBuffer, "I have grown a total of %u grass tiles in VineKing! Can you out-grow me?", g_uCurrentlyDisplayedTotal );
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_LIFE );
			break;
			
		case SCREEN_PAGE_MONSTERS_DEFEATED:
			sprintf( szStringBuffer, "I have defeated a total of %u monsters in VineKing! How many have you defeated?", g_uCurrentlyDisplayedTotal );
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_MONSTER );
			break;
			
		case SCREEN_PAGE_SEEDS_GATHERED:
			sprintf( szStringBuffer, "I have collected a total of %u seeds in VineKing! Can you out-collect me?", g_uCurrentlyDisplayedTotal );
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_SEED );
			break;
			
		case SCREEN_PAGE_COINS_COLLECTED:
			sprintf( szStringBuffer, "I have collected a total of %u coins in VineKing! Can you out-collect me?", g_uCurrentlyDisplayedTotal );
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_COINS );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_2:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 1 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W2 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_3:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 2 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W3 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_4:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 3 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W4 );
			break;
			
		case SCREEN_PAGE_TIMES_BOSS_1:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I beat the first Boss in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_B1 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_5:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 4 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W5 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_6:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 5 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W6 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_7:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 6 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W7 );
			break;
			
		case SCREEN_PAGE_TIMES_BOSS_2:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I beat the second Boss in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_B2 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_8:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 7 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W8 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_9:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 8 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W9 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_10:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed World 9 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W10 );
			break;
			
		case SCREEN_PAGE_TIMES_BOSS_3:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I beat the third Boss in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_B3 );
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_11_1:
			_GetTimeDisplayString( szTemp, g_uCurrentlyDisplayedTotal );
			sprintf( szStringBuffer, "I completed Maw of Magma Levels 1-4 in %s in VineKing! Catch me if you can!", szTemp );
			iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W11_1 );
			break;
			
		default:
			sprintf( szStringBuffer, "%s", "VineKing is awesome!" );
			break;
	}
	
	if( iRank != -1 )
	{
		char szTemp[64];
		sprintf( szTemp, " I am now ranked #%d in the world!", iRank );
		strcat( szStringBuffer, szTemp );
	}
	
	FacebookWrapper::OpenFeedDialog( szStringBuffer );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::OnMyScoreScreenButtonPressed_Twitter()
{
    printf( "%s\n", __FUNCTION__ );
    
    if( sm_pInstance == NULL )
		return;
    
    TwitterWrapper::OpenTwitterDialog();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMyScoreScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMyScoreScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMyScoreScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentMyScoreScreen::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreen::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreen::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	// Text Objects
	g_pTitleText = static_cast<t2dTextObject*>( Sim::findObject( "MyScores_PageTitle" ) );
	
	// Sprite Objects
	g_pMyScores_ButtonLeft			= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_ButtonLeft ) );
	g_pMyScores_ButtonRight			= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_ButtonRight ) );
	g_pMyScores_BackButton			= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_BackButton ) );
	//g_pMyScores_LeaderboardButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_LeaderboardButton ) );
	//g_pMyScores_AchievementsButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_AchievementsButton ) );
	//g_pMyScores_Facebook_Button		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_Facebook_Button ) );
    //g_pMyScores_Twitter_Button		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_Twitter_Button ) );
	
	// Scene Window
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( g_szSceneWindow ) );
	
	// Fade Box
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
		g_pFadeBox->setPosition( m_pOwner->getPosition() );
	
	
	// Text Objects - Labels
	g_pMyScores_PageTitle = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_PageTitle ) );
	g_pMyScores_Label_Level01 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Label_Level01 ) );
	g_pMyScores_Label_Level02 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Label_Level02 ) );
	g_pMyScores_Label_Level03 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Label_Level03 ) );
	g_pMyScores_Label_Level04 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Label_Level04 ) );
	g_pMyScores_Totals = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Totals ) );
	
	g_pMyScores_Label_Total = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Label_Total ) );
	
	g_pMyScores_Label_Underscores = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Label_Underscores ) );
	
	// Text Objects - Times
	g_pMyScores_Times01 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Times01 ) );
	g_pMyScores_Times02 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Times02 ) );
	g_pMyScores_Times03 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Times03 ) );
	g_pMyScores_Times04 = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Times04 ) );
	g_pMyScores_Times_Total = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_Times_Total ) );
	
	// Text Object - Rank
	g_pMyScores_GlobalRank = static_cast<t2dTextObject*>( Sim::findObject( g_szMyScores_GlobalRank ) );
    
    
    // Disable certain buttons depending on the device
    g_pMyScores_LeaderboardButton	= NULL;
	g_pMyScores_AchievementsButton	= NULL;
	g_pMyScores_Facebook_Button		= NULL;
    
    if( GameCenterWrapper::IsGameCenterAvailableOnDevice() )
    {
        g_pMyScores_LeaderboardButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_LeaderboardButton ) );
        g_pMyScores_AchievementsButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_AchievementsButton ) );
    }

    if( CComponentGlobals::GetInstance().IsOldIOSDeviceType() == false )
    {
        g_pMyScores_Facebook_Button	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szMyScores_Facebook_Button ) );
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentMyScoreScreen::DecrementPage()
{
	//printf( "CComponentMyScoreScreen::DecrementPage\n" );
	//printf( "->m_iCurrentScreenPage: %d\n", m_iCurrentScreenPage );
	
	m_iCurrentScreenPage = static_cast<EScreenPage>( m_iCurrentScreenPage - 1 );
	
	//printf( "->m_iCurrentScreenPage: %d\n", m_iCurrentScreenPage );
	
	if( m_iCurrentScreenPage < SCREEN_PAGE_LIFE_GROWN )
		m_iCurrentScreenPage = SCREEN_PAGE_TIMES_WORLD_11_1;
	
	HandlePageChange();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreen::IncrementPage()
{
	//printf( "CComponentMyScoreScreen::DecrementPage\n" );
	//printf( "->m_iCurrentScreenPage: %d\n", m_iCurrentScreenPage );
	
	m_iCurrentScreenPage = static_cast<EScreenPage>( m_iCurrentScreenPage + 1 );
	
	//printf( "->m_iCurrentScreenPage: %d\n", m_iCurrentScreenPage );
	
	if( m_iCurrentScreenPage >= NUM_SCREEN_PAGES )
		m_iCurrentScreenPage = SCREEN_PAGE_LIFE_GROWN;
	
	HandlePageChange();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreen::HandlePageChange()
{
	if( g_pMyScores_PageTitle == NULL || g_pMyScores_Label_Level01 == NULL || g_pMyScores_Label_Level02	== NULL ||
	    g_pMyScores_Label_Level03 == NULL || g_pMyScores_Label_Level04 == NULL || g_pMyScores_Totals == NULL ||
		g_pMyScores_Times01	== NULL || g_pMyScores_Times02 == NULL || g_pMyScores_Times03 == NULL ||
	    g_pMyScores_Times04	== NULL || g_pMyScores_Times_Total == NULL || g_pMyScores_Label_Total == NULL ||
	    g_pMyScores_Label_Underscores == NULL )
	{
		return;
	}
	
	switch( m_iCurrentScreenPage )
	{
		case SCREEN_PAGE_LIFE_GROWN:
		case SCREEN_PAGE_MONSTERS_DEFEATED:
		case SCREEN_PAGE_SEEDS_GATHERED:
		case SCREEN_PAGE_COINS_COLLECTED:
			g_pMyScores_Label_Level01->setVisible( false );
			g_pMyScores_Label_Level02->setVisible( false );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			g_pMyScores_Label_Total->setVisible( false );
			
			g_pMyScores_Times01->setVisible( false );
			g_pMyScores_Times02->setVisible( false );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			g_pMyScores_Times_Total->setVisible( false );

			g_pMyScores_Label_Underscores->setVisible( false );
			
			g_pMyScores_Totals->setVisible( true );

			break;
			
		case SCREEN_PAGE_TIMES_WORLD_2:
		case SCREEN_PAGE_TIMES_WORLD_3:
		case SCREEN_PAGE_TIMES_WORLD_4:
		case SCREEN_PAGE_TIMES_BOSS_1:
		case SCREEN_PAGE_TIMES_WORLD_5:
		case SCREEN_PAGE_TIMES_WORLD_6:
		case SCREEN_PAGE_TIMES_WORLD_7:
		case SCREEN_PAGE_TIMES_BOSS_2:
		case SCREEN_PAGE_TIMES_WORLD_8:
		case SCREEN_PAGE_TIMES_WORLD_9:
		case SCREEN_PAGE_TIMES_WORLD_10:
		case SCREEN_PAGE_TIMES_BOSS_3:
		case SCREEN_PAGE_TIMES_WORLD_11_1:
			g_pMyScores_Label_Level01->setVisible( true );
			g_pMyScores_Label_Level02->setVisible( true );
			g_pMyScores_Label_Level03->setVisible( true );
			g_pMyScores_Label_Level04->setVisible( true );
			g_pMyScores_Label_Total->setVisible( true );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( true );
			g_pMyScores_Times_Total->setVisible( true );
			
			g_pMyScores_Label_Underscores->setVisible( true );
			
			g_pMyScores_Totals->setVisible( false );
			break;
			
		default:
			printf( "HandlePageChange: Unhandled Screen Page (1): %d\n", m_iCurrentScreenPage );
			break;
	}
	
	char szTemp[64];

	U32 uTotalTimeMilliseconds = 0;
	bool bDisplayTotalTime = false;
	S32 iRank = -1;
	
	if( g_pMyScores_Facebook_Button )
		g_pMyScores_Facebook_Button->setVisible( true );
    
    if( g_pMyScores_Twitter_Button )
		g_pMyScores_Twitter_Button->setVisible( true );
	
	if( g_pMyScores_GlobalRank )
		g_pMyScores_GlobalRank->setVisible( false );
	
	switch( m_iCurrentScreenPage )
	{
		case SCREEN_PAGE_LIFE_GROWN:
			g_pMyScores_PageTitle->setText( "Life\nGrown" );
			sprintf( szTemp, "%d", CComponentGlobals::GetInstance().GetNumGrassGrown() );
			g_pMyScores_Totals->setText( szTemp );
			
			g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetNumGrassGrown();
			
			if( g_pMyScores_Facebook_Button && g_uCurrentlyDisplayedTotal == 0 )
				g_pMyScores_Facebook_Button->setVisible( false );
            
            if( g_pMyScores_Twitter_Button && g_uCurrentlyDisplayedTotal == 0 )
                g_pMyScores_Twitter_Button->setVisible( false );
			
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_LIFE );
				
			break;
			
		case SCREEN_PAGE_MONSTERS_DEFEATED:
			g_pMyScores_PageTitle->setText( "Monsters\nDefeated" );
			sprintf( szTemp, "%d", CComponentGlobals::GetInstance().GetNumMonstersDefeated() );
			g_pMyScores_Totals->setText( szTemp );
			
			g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetNumMonstersDefeated();
			
			if( g_pMyScores_Facebook_Button && g_uCurrentlyDisplayedTotal == 0 )
				g_pMyScores_Facebook_Button->setVisible( false );
            
            if( g_pMyScores_Twitter_Button && g_uCurrentlyDisplayedTotal == 0 )
                g_pMyScores_Twitter_Button->setVisible( false );
			
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_MONSTER );
			
			break;
			
		case SCREEN_PAGE_SEEDS_GATHERED:
			g_pMyScores_PageTitle->setText( "Seeds\nGathered" );
			sprintf( szTemp, "%d", CComponentGlobals::GetInstance().GetNumSeedsCollected() );
			g_pMyScores_Totals->setText( szTemp );
			
			g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetNumSeedsCollected();
			
			if( g_pMyScores_Facebook_Button && g_uCurrentlyDisplayedTotal == 0 )
				g_pMyScores_Facebook_Button->setVisible( false );
            
            if( g_pMyScores_Twitter_Button && g_uCurrentlyDisplayedTotal == 0 )
				g_pMyScores_Twitter_Button->setVisible( false );
			
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_SEED );
			
			break;
			
		case SCREEN_PAGE_COINS_COLLECTED:
			g_pMyScores_PageTitle->setText( "Coins\nCollected" );
			sprintf( szTemp, "%d", CComponentGlobals::GetInstance().GetTotalXPEarned() );
			g_pMyScores_Totals->setText( szTemp );
			
			g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetTotalXPEarned();
			
			if( g_pMyScores_Facebook_Button && g_uCurrentlyDisplayedTotal == 0 )
				g_pMyScores_Facebook_Button->setVisible( false );
            
            if( g_pMyScores_Twitter_Button && g_uCurrentlyDisplayedTotal == 0 )
				g_pMyScores_Twitter_Button->setVisible( false );
			
			if( g_uCurrentlyDisplayedTotal > 0 )
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TOP_COINS );
			
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_2:
			g_pMyScores_PageTitle->setText( "World 1" );
			g_pMyScores_Label_Level01->setText( "1.1 ....." );
			g_pMyScores_Label_Level02->setVisible( false );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( false );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_02_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_02_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_02_01 );
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W2 );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
                    g_pMyScores_Twitter_Button->setVisible( false );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_3:
			g_pMyScores_PageTitle->setText( "World 2" );
			g_pMyScores_Label_Level01->setText( "2.1 ....." );
			g_pMyScores_Label_Level02->setText( "2.2 ....." );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_01 ) );

				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_02 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_03_02 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W3 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_4:
			g_pMyScores_PageTitle->setText( "World 3" );
			g_pMyScores_Label_Level01->setText( "3.1 ....." );
			g_pMyScores_Label_Level02->setText( "3.2 ....." );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_02 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_04_02 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W4 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_BOSS_1:
			g_pMyScores_PageTitle->setText( "Boss 1" );
			g_pMyScores_Label_Level01->setText( "B1 ....." );
			g_pMyScores_Label_Level02->setVisible( false );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( false );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_01 );
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_B1 );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_5:
			g_pMyScores_PageTitle->setText( "World 4" );
			g_pMyScores_Label_Level01->setText( "4.1 ....." );
			g_pMyScores_Label_Level02->setText( "4.2 ....." );
			g_pMyScores_Label_Level03->setText( "4.3 ....." );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_03 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_05_03 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W5 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_6:
			g_pMyScores_PageTitle->setText( "World 5" );
			g_pMyScores_Label_Level01->setText( "5.1 ....." );
			g_pMyScores_Label_Level02->setText( "5.2 ....." );
			g_pMyScores_Label_Level03->setText( "5.3 ....." );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_03 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_06_03 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W6 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_7:
			g_pMyScores_PageTitle->setText( "World 6" );
			g_pMyScores_Label_Level01->setText( "6.1 ....." );
			g_pMyScores_Label_Level02->setText( "6.2 ....." );
			g_pMyScores_Label_Level03->setText( "6.3 ....." );
			g_pMyScores_Label_Level04->setText( "6.4 ....." );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( true );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_04 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_04 ) );
				
				g_pMyScores_Times04->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times04->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_03 ) <= 0 ||
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_04 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_03 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_07_04 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W7 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_BOSS_2:
			g_pMyScores_PageTitle->setText( "Boss 2" );
			g_pMyScores_Label_Level01->setText( "B2 ....." );
			g_pMyScores_Label_Level02->setVisible( false );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( false );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02 ) );
				
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_02 );
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_B2 );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_8:
			g_pMyScores_PageTitle->setText( "World 7" );
			g_pMyScores_Label_Level01->setText( "7.1 ....." );
			g_pMyScores_Label_Level02->setText( "7.2 ....." );
			g_pMyScores_Label_Level03->setText( "7.3 ....." );
			g_pMyScores_Label_Level04->setText( "7.4 ....." );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( true );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_04 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_04 ) );
				
				g_pMyScores_Times04->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times04->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_03 ) <= 0 ||
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_04 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_03 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_08_04 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W8 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_9:
			g_pMyScores_PageTitle->setText( "World 8" );
			g_pMyScores_Label_Level01->setText( "8.1 ....." );
			g_pMyScores_Label_Level02->setText( "8.2 ....." );
			g_pMyScores_Label_Level03->setText( "8.3 ....." );
			g_pMyScores_Label_Level04->setText( "8.4 ....." );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( true );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_04 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_04 ) );
				
				g_pMyScores_Times04->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times04->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_03 ) <= 0 ||
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_04 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_03 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_09_04 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W9 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_10:
			g_pMyScores_PageTitle->setText( "World 9" );
			g_pMyScores_Label_Level01->setText( "9.1 ....." );
			g_pMyScores_Label_Level02->setText( "9.2 ....." );
			g_pMyScores_Label_Level03->setText( "9.3 ....." );
			g_pMyScores_Label_Level04->setText( "9.4 ....." );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( true );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_04 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_04 ) );
				
				g_pMyScores_Times04->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times04->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_03 ) <= 0 ||
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_04 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_03 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_10_04 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W10 );
			}
			break;
			
		case SCREEN_PAGE_TIMES_BOSS_3:
			g_pMyScores_PageTitle->setText( "Boss 3" );
			g_pMyScores_Label_Level01->setText( "B3 ....." );
			g_pMyScores_Label_Level02->setVisible( false );
			g_pMyScores_Label_Level03->setVisible( false );
			g_pMyScores_Label_Level04->setVisible( false );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( false );	
			g_pMyScores_Times03->setVisible( false );
			g_pMyScores_Times04->setVisible( false );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03 ) );
				
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_BOSS_03 );
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_B3 );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			break;
			
		case SCREEN_PAGE_TIMES_WORLD_11_1:
			g_pMyScores_PageTitle->setText( "Maw of\nMagma 1" );
			g_pMyScores_Label_Level01->setText( "M.1 ....." );
			g_pMyScores_Label_Level02->setText( "M.2 ....." );
			g_pMyScores_Label_Level03->setText( "M.3 ....." );
			g_pMyScores_Label_Level04->setText( "M.4 ....." );
			
			g_pMyScores_Times01->setVisible( true );
			g_pMyScores_Times02->setVisible( true );	
			g_pMyScores_Times03->setVisible( true );
			g_pMyScores_Times04->setVisible( true );
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_01 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_01 ) );
				
				g_pMyScores_Times01->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times01->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_02 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_02 ) );
				
				g_pMyScores_Times02->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times02->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_03 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_03 ) );
				
				g_pMyScores_Times03->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times03->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_04 ) > 0 )
			{
				_GetTimeDisplayString( szTemp, CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_04 ) );
				
				g_pMyScores_Times04->setText( szTemp );
			}
			else
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times04->setText( szTemp );
				bDisplayTotalTime = false;
			}
			
			// Total Time
			if( CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_01 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_02 ) <= 0 || 
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_03 ) <= 0 ||
			    CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_04 ) <= 0 )
			{
				sprintf( szTemp, "-:--:--" );
				g_pMyScores_Times_Total->setText( szTemp );
				
				if( g_pMyScores_Facebook_Button )
					g_pMyScores_Facebook_Button->setVisible( false );
                
                if( g_pMyScores_Twitter_Button )
					g_pMyScores_Twitter_Button->setVisible( false );
			}
			else
			{
				uTotalTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_01 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_02 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_03 ) +
										 CComponentGlobals::GetInstance().GetLevelCompleteTimeMilliseconds( SAVE_DATA_INDEX_LEVEL_TIME_11_04 );
				
				_GetTimeDisplayString( szTemp, uTotalTimeMilliseconds );
				g_pMyScores_Times_Total->setText( szTemp );
				
				g_uCurrentlyDisplayedTotal = uTotalTimeMilliseconds;
				
				iRank = GameCenterWrapper::GetRankForLeaderboard( GameCenterWrapper::VK_LB_TIME_W11_1 );
			}
			break;
			
		default:
			printf( "HandlePageChange: Unhandled Screen Page (2): %d\n", m_iCurrentScreenPage );
			break;
	}
	
	
	if( g_pMyScores_GlobalRank && iRank != -1 )
	{
		sprintf( szTemp, "Global Rank: %d", iRank );
		g_pMyScores_GlobalRank->setText( szTemp );
		g_pMyScores_GlobalRank->setVisible( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreen::UpdateFade()
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

void CComponentMyScoreScreen::OnFadeOutFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	CComponentMainMenuScreen::GetInstance().OpenMainMenuScreenFromMyScoresScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMyScoreScreen::OnFadeInFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	if( g_pMyScores_ButtonLeft )
		g_pMyScores_ButtonLeft->setUseMouseEvents( true );
	
	if( g_pMyScores_ButtonRight	)
		g_pMyScores_ButtonRight->setUseMouseEvents( true );
	
	if( g_pMyScores_BackButton )
		g_pMyScores_BackButton->setUseMouseEvents( true );
	
	if( g_pMyScores_LeaderboardButton )
		g_pMyScores_LeaderboardButton->setUseMouseEvents( true );
	
	if( g_pMyScores_AchievementsButton )
		g_pMyScores_AchievementsButton->setUseMouseEvents( true );
	
	if( g_pMyScores_Facebook_Button )
		g_pMyScores_Facebook_Button->setUseMouseEvents( true );
    
    if( g_pMyScores_Twitter_Button )
        g_pMyScores_Twitter_Button->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void _GetTimeDisplayString( char* _pszOutDisplayString, const U32 _uTimeMilliseconds )
{
	if( _pszOutDisplayString == NULL )
		return;
	
	F32 fGameTimeMinutes = ( (F32)_uTimeMilliseconds * 0.001f ) / 60.0f;
	S32 iDisplayMinutes = (S32)fGameTimeMinutes;
	F32 fGameTimeSeconds = (fGameTimeMinutes - (F32)iDisplayMinutes) * 60.0f;
	S32 iTruncatedMS = (S32)(fGameTimeSeconds * 100.0f); // The formatting of %0.2f in sprintf will round the milliseconds instead of truncating, so truncate here.
	fGameTimeSeconds = (F32)iTruncatedMS / 100.0f;
	
	//sprintf( _pszOutDisplayString, "%d%2.2f", iDisplayMinutes, fGameTimeSeconds );
	
	if( fGameTimeSeconds < 10 )
		sprintf( _pszOutDisplayString, "%d:0%0.2f", iDisplayMinutes, fGameTimeSeconds );
	else
		sprintf( _pszOutDisplayString, "%d:%0.2f", iDisplayMinutes, fGameTimeSeconds );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
