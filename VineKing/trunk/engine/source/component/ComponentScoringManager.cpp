//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentScoringManager.h"

#include "ComponentAchievementBanner.h"
#include "ComponentGlobals.h"
#include "ComponentManaBar.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentSeedMagnetButton.h"
#include "ComponentWinLevel.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "platformiPhone/GameCenterWrapper.h"
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const S32 g_iPointValueEnemyA			= 10;	// Killing a Lava Blob
static const S32 g_iPointValueEnemyB			= 20;	// Killing a Spitter
static const S32 g_iPointValueSmokeMonster		= 15;	// Killing a Smoke Monster

static const S32 g_iPointValueSeedEating		= 1;	// For eating a seed
static const S32 g_iPointValueGrassGrown		= 2;	// For fully grown grass
static const S32 g_iPointValueDestroyCrystal	= 50;	// For destroying a double brick

static const t2dVector g_vScoreScreenPosition( 0.0f, 0.0f );	//g_vScoreScreenPosition( 0.0f, -20.0f );
static const t2dVector g_vOKButtonPosition( 140.0f, 219.0f );	// Position of the OK button (which is also the owner of this component)

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static const char g_szMapLevelName[128]			= "game/data/levels/Screen_MapSelect_PowerUp.t2d";
static const char g_szStoryEndLevelName[128]	= "game/data/levels/Screen_StoryEnding.t2d";

static const char g_szGameTimer_Text[32] = "GameTimer_Text";
static const t2dVector g_vGameTimerTextPositionStart( 77.0f, 223.0f );
static const t2dVector g_vGameTimerTextPosition( -65.0f, 193.0f ); // -50.0f, 193.0f

static const char g_szScore_StarMarker[32] = "Score_StarMarker";
//static const char g_szScore_StarMarkerImageMap[32] = "_3_starsImageMap"; 
static const char g_szScore_StarMarkerImageMap[32] = "score_screen_starsImageMap";
static const S32 g_iFrameNumStarsOne	= 0;
static const S32 g_iFrameNumStarsTwo	= 1;
static const S32 g_iFrameNumStarsThree	= 2;
static const S32 g_iFrameNumStarsZero	= 3;

//static const t2dVector g_vStarMarkerPosition( 79.0f, 177.0f );
static const t2dVector g_vStarMarkerPosition( 73.0f, 194.0f );



static const ColorF g_ScoreScreenTextColor( 1.0f, 1.0f, 1.0f );

static const char g_szText_Score_Mana_Bank[32] = "Text_Score_Mana_Bank";
static t2dTextObject* g_pText_Score_Mana_Bank = NULL;
static const t2dVector g_vText_Score_Mana_BankPos( 65.0f, -205.0f );
static const ColorF g_Text_Score_Mana_Bank_Color( 0.0f, 0.0f, 0.0f );

// Full Screen Fade
static const char g_szBlackBox[32] = "BlackBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

// Star Ranking
static const F32 g_fStarRanking03_SecondsMax = 60.0f;
static const F32 g_fStarRanking02_SecondsMax = 90.0f;

// Star Ranking Times (Seconds): 
// - Index 0: Achieve this or less time for a 3 Star Ranking
// - Index 1: Achieve this or less time for a 2 Star Ranking
// - A time greater than Index 1 will yield a 1 Star Ranking
static const F32 g_fStarRankingSeconds_01_01[2] = { 600.0f, 660.0f }; //{ 91.0f, 111.0f };

static const F32 g_fStarRankingSeconds_02_01[2] = { 71.0f, 91.0f };

static const F32 g_fStarRankingSeconds_03_01[2] = { 91.0f, 111.0f };
static const F32 g_fStarRankingSeconds_03_02[2] = { 71.0f, 91.0f };

static const F32 g_fStarRankingSeconds_04_01[2] = { 71.0f, 91.0f };
static const F32 g_fStarRankingSeconds_04_02[2] = { 81.0f, 101.0f };

static const F32 g_fStarRankingSeconds_05_01[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_05_02[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_05_03[2] = { 81.0f, 101.0f };

static const F32 g_fStarRankingSeconds_06_01[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_06_02[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_06_03[2] = { 81.0f, 101.0f };

static const F32 g_fStarRankingSeconds_07_01[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_07_02[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_07_03[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_07_04[2] = { 81.0f, 101.0f };

static const F32 g_fStarRankingSeconds_08_01[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_08_02[2] = { 76.0f, 96.0f };
static const F32 g_fStarRankingSeconds_08_03[2] = { 86.0f, 106.0f };
static const F32 g_fStarRankingSeconds_08_04[2] = { 86.0f, 106.0f };

static const F32 g_fStarRankingSeconds_09_01[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_09_02[2] = { 91.0f, 111.0f };
static const F32 g_fStarRankingSeconds_09_03[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_09_04[2] = { 91.0f, 131.0f };

static const F32 g_fStarRankingSeconds_10_01[2] = { 101.0f, 141.0f };
static const F32 g_fStarRankingSeconds_10_02[2] = { 101.0f, 141.0f };
static const F32 g_fStarRankingSeconds_10_03[2] = { 101.0f, 141.0f };
static const F32 g_fStarRankingSeconds_10_04[2] = { 101.0f, 141.0f };

static const F32 g_fStarRankingSeconds_Boss_01[2] = { 91.0f, 111.0f };
static const F32 g_fStarRankingSeconds_Boss_02[2] = { 81.0f, 101.0f };
static const F32 g_fStarRankingSeconds_Boss_03[2] = { 151.0f, 201.0f };

static const F32 g_fStarRankingSeconds_11_01[2]	= { 101.0f, 141.0f };
static const F32 g_fStarRankingSeconds_11_02[2]	= { 101.0f, 141.0f };
static const F32 g_fStarRankingSeconds_11_03[2]	= { 101.0f, 141.0f };
static const F32 g_fStarRankingSeconds_11_04[2]	= { 101.0f, 141.0f };


// Facebook Button
static t2dStaticSprite* g_pFacebookButton = NULL;
static const char g_szFacebookButton[32] = "ScoreScreen_FacebookButton";

static const t2dVector g_vFacebookButtonPosition( -119.0f, 127.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentScoringManager* CComponentScoringManager::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentScoringManager );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentScoringManager::CComponentScoringManager()
	: m_pOwner( NULL )
	, m_pszScoreScreenBGName( NULL )
	, m_pScoreScreenBG( NULL )
	, m_pszTextNumKilledA( NULL )
	, m_pTextNumKilledA( NULL )
	, m_pszTextNumKilledB( NULL )
	, m_pTextNumKilledB( NULL )
	, m_pszTextNumKilledSmoke( NULL )
	, m_pTextNumKilledSmoke( NULL )
	, m_pszTextNumSeedsEaten( NULL )
	, m_pTextNumSeedsEaten( NULL )
	, m_pszTextGrassTilesGrown( NULL )
	, m_pTextGrassTilesGrown( NULL )
	, m_pszTextCrystalsDestroyed( NULL )
	, m_pTextCrystalsDestroyed( NULL )
	, m_pszTextHealthRemaining( NULL )
	, m_pTextHealthRemaining( NULL )
	, m_pszTextManaRemaining( NULL )
	, m_pTextManaRemaining( NULL )
	, m_pszTextScoreNumKilledA( NULL )
	, m_pTextScoreNumKilledA( NULL )
	, m_pszTextScoreNumKilledB( NULL )
	, m_pTextScoreNumKilledB( NULL )
	, m_pszTextScoreNumKilledSmoke( NULL )
	, m_pTextScoreNumKilledSmoke( NULL )
	, m_pszTextScoreNumSeedsEaten( NULL )
	, m_pTextScoreNumSeedsEaten( NULL )
	, m_pszTextScoreGrassTilesGrown( NULL )
	, m_pTextScoreGrassTilesGrown( NULL )
	, m_pszTextScoreCrystalsDestroyed( NULL )
	, m_pTextScoreCrystalsDestroyed( NULL )
	, m_pszTextScoreHealthRemaining( NULL )
	, m_pTextScoreHealthRemaining( NULL )
	, m_pszTextScoreManaRemaining( NULL )
	, m_pTextScoreManaRemaining( NULL )
	//, m_pszTextScoreTotalLabel( NULL )
	//, m_pTextScoreTotalLabel( NULL )
	, m_pszTextScoreTotal( NULL )
	, m_pTextScoreTotal( NULL )
	//, m_pszXPLabel( NULL )
	//, m_pTextXPLabel( NULL )
	//, m_pszXPTotal( NULL )
	//, m_pTextXPTotal( NULL )
	//, m_pszScoreScreenBlackBox( NULL )
	//, m_pScoreScreenBlackBox( NULL )
	//, m_pszScoreScreenItemImage( NULL )
	//, m_pScoreScreenItemImage( NULL )

	, m_pGameTimerText( NULL )
	, m_pStarMarker( NULL )

	, m_iNumKilledEnemyA( 0 )
	, m_iNumKilledEnemyB( 0 )
	, m_iNumKilledSmokeMonster( 0 )
	, m_iNumSeedsEaten( 0 )
	, m_iNumGrassTilesGrown( 0 )
	, m_iNumCrystalsDestroyed( 0 )
	, m_iScoreEnemyA( 0 )
	, m_iScoreEnemyB( 0 )
	, m_iScoreSmokeMonster( 0 )
	, m_iScoreSeedsEaten( 0 )
	, m_iScoreGrassTilesGrown( 0 )
	, m_iScoreCrystalsDestroyed( 0 )
	, m_iHealthScore( 0 )
	, m_iManaScore( 0 )
	, m_iTotalScore( 0 )
	, m_iStarRanking( 0 )

	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentScoringManager::~CComponentScoringManager()
{
	CComponentScoringManager::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OpenScoreScreen()
{
	CComponentSeedMagnetButton::HideSeedMagnetButton();
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds;
	
	if( m_pScoreScreenBG )
	{
		m_pScoreScreenBG->setVisible( true );
		m_pScoreScreenBG->setPosition( g_vScoreScreenPosition );
	}
	
	if( m_pTextNumKilledA ) m_pTextNumKilledA->setVisible( true );
	if( m_pTextNumKilledB ) m_pTextNumKilledB->setVisible( true );
	if( m_pTextNumKilledSmoke ) m_pTextNumKilledSmoke->setVisible( true );
	if( m_pTextNumSeedsEaten ) m_pTextNumSeedsEaten->setVisible( true );
	if( m_pTextGrassTilesGrown ) m_pTextGrassTilesGrown->setVisible( true );
	if( m_pTextCrystalsDestroyed ) m_pTextCrystalsDestroyed->setVisible( true );
	
	//if( m_pTextHealthRemaining ) m_pTextHealthRemaining->setVisible( true );
	//if( m_pTextManaRemaining ) m_pTextManaRemaining->setVisible( true );
	
	if( g_pText_Score_Mana_Bank )
	{
		g_pText_Score_Mana_Bank->setPosition( g_vText_Score_Mana_BankPos );
		g_pText_Score_Mana_Bank->setVisible( true );
	}
	
	CalculateScores();
	DisplayScoreTexts();
	
	CalculateAndDisplayGameTime();
	CalculateAndDisplayStarRanking();
	
	if( m_pOwner )
	{
		m_pOwner->setUseMouseEvents( false );
		m_pOwner->setVisible( true );
	}
	
	CComponentGlobals::GetInstance().MarkActiveLevelAsComplete( m_iStarRanking );
	
	CComponentGlobals::GetInstance().RecordCurrentLevelTime();
	
	if( CComponentPlayerHomeBase::GetInstance().GetPercentHomeBaseHealthRemaining() >= 1.0f )
	{
		CComponentGlobals::GetInstance().IncrementNumLevelsBeatenWithFullHealth();
	}
	
	if( CComponentManaBar::GetPercentManaRemaining() >= 1.0f )
	{
		CComponentGlobals::GetInstance().IncrementNumLevelsBeatenWithFullPower();
	}
	
    if( CComponentGlobals::GetInstance().IsOldIOSDeviceType() == false )
    {
        if( g_pFacebookButton )
            g_pFacebookButton->setVisible( true );
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void  CComponentScoringManager::InitializeScoreScreenPositions()
{
	if( sm_pInstance == NULL )
		return;
	
	const char* pszCurrentLevelName = CComponentGlobals::GetInstance().GetCurrentLevelName();
	if( pszCurrentLevelName == NULL || pszCurrentLevelName[0] == '\0' )
		return;
	
	char* pszTemp;
	
	if( ( pszTemp = strstr( pszCurrentLevelName, "Boss_" ) ) != NULL )
	{
		sm_pInstance->InitializePositionsForScoreScreenBoss();
	}
	else if( ( pszTemp = strstr( pszCurrentLevelName, "Level_" ) ) != NULL )
	{
		if( ( pszTemp = strstr( pszCurrentLevelName, "Level_01" ) ) != NULL )
		{
			sm_pInstance->InitializePositionsForScoreScreen01();
		}
		else if( ( pszTemp = strstr( pszCurrentLevelName, "Level_02" ) ) != NULL )
		{
			sm_pInstance->InitializePositionsForScoreScreen01();
		}
		else if( ( pszTemp = strstr( pszCurrentLevelName, "Level_03" ) ) != NULL )
		{ 
			if( strcmp( pszCurrentLevelName, "Level_03_01.t2d" ) == 0 )
			{
				sm_pInstance->InitializePositionsForScoreScreen02();
			}
			else if( strcmp( pszCurrentLevelName, "Level_03_02.t2d" ) == 0 )
			{
				sm_pInstance->InitializePositionsForScoreScreen03();
			}
		}
		else if( ( pszTemp = strstr( pszCurrentLevelName, "Level_04" ) ) != NULL )
		{
			if( strcmp( pszCurrentLevelName, "Level_04_01.t2d" ) == 0 )
			{
				sm_pInstance->InitializePositionsForScoreScreen03();
			}
			else if( strcmp( pszCurrentLevelName, "Level_04_02.t2d" ) == 0 )
			{
				sm_pInstance->InitializePositionsForScoreScreen04();
			}
		}
		else 
		{
			sm_pInstance->InitializePositionsForScoreScreen04();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentScoringManager::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentScoringManager ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "ScoreScreenBG",			TypeString, Offset( m_pszScoreScreenBGName,		CComponentScoringManager ) );
	
	addField( "NumKilledA",				TypeString, Offset( m_pszTextNumKilledA,		CComponentScoringManager ) );
	addField( "NumKilledB",				TypeString, Offset( m_pszTextNumKilledB,		CComponentScoringManager ) );
	addField( "NumKilledSmoke",			TypeString, Offset( m_pszTextNumKilledSmoke,	CComponentScoringManager ) );
	addField( "NumSeedsEaten",			TypeString, Offset( m_pszTextNumSeedsEaten,		CComponentScoringManager ) );
	addField( "NumGrassGrown",			TypeString, Offset( m_pszTextGrassTilesGrown,	CComponentScoringManager ) );
	addField( "NumDestroyedCrystals",	TypeString, Offset( m_pszTextCrystalsDestroyed, CComponentScoringManager ) );
	
	addField( "HealthRemaining",		TypeString, Offset( m_pszTextHealthRemaining,	CComponentScoringManager ) );
	addField( "ManaRemaining",			TypeString, Offset( m_pszTextManaRemaining,		CComponentScoringManager ) );
	
	addField( "ScoreEnemyA",			TypeString, Offset( m_pszTextScoreNumKilledA,		CComponentScoringManager ) );
	addField( "ScoreEnemyB",			TypeString, Offset( m_pszTextScoreNumKilledB,		CComponentScoringManager ) );
	addField( "ScoreSmokeMonster",		TypeString, Offset( m_pszTextScoreNumKilledSmoke,	CComponentScoringManager ) );
	addField( "ScoreSeedsEaten",		TypeString, Offset( m_pszTextScoreNumSeedsEaten,	CComponentScoringManager ) );
	addField( "ScoreGrassTilesGrown",	TypeString, Offset( m_pszTextScoreGrassTilesGrown,	CComponentScoringManager ) );
	addField( "ScoreCrystalsDestroyed",	TypeString, Offset( m_pszTextScoreCrystalsDestroyed,CComponentScoringManager ) );
	addField( "ScoreHealthRemaining",	TypeString, Offset( m_pszTextScoreHealthRemaining,	CComponentScoringManager ) );
	addField( "ScoreManaRemaining",		TypeString, Offset( m_pszTextScoreManaRemaining,	CComponentScoringManager ) );
	
	//addField( "ScoreTotalLabel",		TypeString, Offset( m_pszTextScoreTotalLabel,	CComponentScoringManager ) );
	addField( "ScoreTotal",				TypeString, Offset( m_pszTextScoreTotal,		CComponentScoringManager ) );
	
	//addField( "XPLabel",				TypeString, Offset( m_pszXPLabel,	CComponentScoringManager ) );
	//addField( "XPTotal",				TypeString, Offset( m_pszXPTotal,	CComponentScoringManager ) );
	
	//addField( "ScoreScreenBlackBox",	TypeString, Offset( m_pszScoreScreenBlackBox,	CComponentScoringManager ) );
	//addField( "ScoreScreenItemImage",	TypeString, Offset( m_pszScoreScreenItemImage,	CComponentScoringManager ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentScoringManager::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentScoringManager::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( false );
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( false );
	
	m_eFadeState = FADE_STATE_FADING_OUT;
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	if( g_pFacebookButton )
		g_pFacebookButton->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnPostInit()
{
	if( m_pszScoreScreenBGName != NULL && m_pszScoreScreenBGName[0] != '\0' )
	{
		m_pScoreScreenBG = static_cast<t2dSceneObject*>(Sim::findObject( m_pszScoreScreenBGName ));
		if( m_pScoreScreenBG )
		{
			m_pScoreScreenBG->setPosition( g_vScoreScreenPosition );
			m_pScoreScreenBG->setLayer( LAYER_SCORE_SCREEN );
			m_pScoreScreenBG->setVisible( false );
		}
	}
	
	SetTextObjectFromName( m_pTextNumKilledA,				m_pszTextNumKilledA );
	SetTextObjectFromName( m_pTextNumKilledB,				m_pszTextNumKilledB );
	SetTextObjectFromName( m_pTextNumKilledSmoke,			m_pszTextNumKilledSmoke );
	SetTextObjectFromName( m_pTextNumSeedsEaten,			m_pszTextNumSeedsEaten );
	SetTextObjectFromName( m_pTextGrassTilesGrown,			m_pszTextGrassTilesGrown );
	SetTextObjectFromName( m_pTextCrystalsDestroyed,		m_pszTextCrystalsDestroyed );
	//SetTextObjectFromName( m_pTextHealthRemaining,			m_pszTextHealthRemaining );
	//SetTextObjectFromName( m_pTextManaRemaining,			m_pszTextManaRemaining );
	SetTextObjectFromName( m_pTextScoreNumKilledA,			m_pszTextScoreNumKilledA );
	SetTextObjectFromName( m_pTextScoreNumKilledB,			m_pszTextScoreNumKilledB );
	SetTextObjectFromName( m_pTextScoreNumKilledSmoke,		m_pszTextScoreNumKilledSmoke );
	SetTextObjectFromName( m_pTextScoreNumSeedsEaten,		m_pszTextScoreNumSeedsEaten );
	SetTextObjectFromName( m_pTextScoreGrassTilesGrown,		m_pszTextScoreGrassTilesGrown );
	SetTextObjectFromName( m_pTextScoreCrystalsDestroyed,	m_pszTextScoreCrystalsDestroyed );
	//SetTextObjectFromName( m_pTextScoreHealthRemaining,		m_pszTextScoreHealthRemaining );
	//SetTextObjectFromName( m_pTextScoreManaRemaining,		m_pszTextScoreManaRemaining );
	SetTextObjectFromName( m_pTextScoreTotal,				m_pszTextScoreTotal );	
	
	InitializeTextObjectData( m_pTextNumKilledA );
	InitializeTextObjectData( m_pTextNumKilledB );
	InitializeTextObjectData( m_pTextNumKilledSmoke );
	InitializeTextObjectData( m_pTextNumSeedsEaten );
	InitializeTextObjectData( m_pTextGrassTilesGrown );
	InitializeTextObjectData( m_pTextCrystalsDestroyed );
	//InitializeTextObjectData( m_pTextHealthRemaining );
	//InitializeTextObjectData( m_pTextManaRemaining );
	InitializeTextObjectData( m_pTextScoreNumKilledA );
	InitializeTextObjectData( m_pTextScoreNumKilledB );
	InitializeTextObjectData( m_pTextScoreNumKilledSmoke );
	InitializeTextObjectData( m_pTextScoreNumSeedsEaten );
	InitializeTextObjectData( m_pTextScoreGrassTilesGrown );
	InitializeTextObjectData( m_pTextScoreCrystalsDestroyed );
	//InitializeTextObjectData( m_pTextScoreHealthRemaining );
	//InitializeTextObjectData( m_pTextScoreManaRemaining );
	//InitializeTextObjectData( m_pTextScoreManaRemaining );
	InitializeTextObjectData( m_pTextScoreTotal );
	
	SetTextObjectFromName( m_pGameTimerText, g_szGameTimer_Text );
	InitializeTextObjectData( m_pGameTimerText );
	if( m_pGameTimerText )
	{
		m_pGameTimerText->setPosition( g_vGameTimerTextPositionStart );
		m_pGameTimerText->setBlendColour( g_ScoreScreenTextColor );
		m_pGameTimerText->setLayer( LAYER_SCORE_SCREEN_ELEMENTS );
	}
	
	SetTextObjectFromName( g_pText_Score_Mana_Bank, g_szText_Score_Mana_Bank );
	InitializeTextObjectData( g_pText_Score_Mana_Bank );
	if( g_pText_Score_Mana_Bank )
	{
		g_pText_Score_Mana_Bank->setVisible( false );
		g_pText_Score_Mana_Bank->setBlendColour( g_Text_Score_Mana_Bank_Color );
		g_pText_Score_Mana_Bank->setLayer( LAYER_SCORE_SCREEN_ELEMENTS );
	}
	
	// Hide the OK Button (owner)
	if( m_pOwner )
	{
		m_pOwner->setLayer( LAYER_SCORE_SCREEN_ELEMENTS );
		m_pOwner->setPosition( g_vOKButtonPosition );
		m_pOwner->setVisible( false );
	}
	
	m_pStarMarker = static_cast<t2dStaticSprite*>( Sim::findObject( g_szScore_StarMarker ) );
	if( m_pStarMarker )
	{
		//m_pStarMarker->setPosition( g_vStarMarkerPosition );
		m_pStarMarker->setVisible( false );
		m_pStarMarker->setLayer( LAYER_SCORE_SCREEN_ELEMENTS );
	}
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szBlackBox ) );
	if( g_pFadeBox )
	{
		//g_pFadeBox->setPosition( t2dVector::getZero() );
		//m_eFadeState = FADE_STATE_FADING_IN;
		//m_fFadeTimer = g_fFadeTimeSeconds;
		m_eFadeState = FADE_STATE_NONE;
		m_fFadeTimer = 0.0f;
	}
	
	g_pFacebookButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFacebookButton ) );
}
	   
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS
	   
void CComponentScoringManager::InitializePositionsForScoreScreen01()
{
	// Seeds
	if( m_pTextNumSeedsEaten )
		m_pTextNumSeedsEaten->setPosition( t2dVector( -97.0f, -135.0f ) );
	
	if( m_pTextScoreNumSeedsEaten )
		m_pTextScoreNumSeedsEaten->setPosition( t2dVector( 75.0f, -135.0f ) );
	
	// Grass
	if( m_pTextGrassTilesGrown )
		m_pTextGrassTilesGrown->setPosition( t2dVector( -97.0, -92.0f ) );
	
	if( m_pTextScoreGrassTilesGrown )
		m_pTextScoreGrassTilesGrown->setPosition( t2dVector( 75.0f, -92.0f ) );
	
	// Enemy A
	if( m_pTextNumKilledA )
		m_pTextNumKilledA->setPosition( t2dVector( -97.0, -49.0f ) );
	
	if( m_pTextScoreNumKilledA )
		m_pTextScoreNumKilledA->setPosition( t2dVector( 75.0f, -49.0f ) );
	
	// Health
	//if( m_pTextHealthRemaining )
	//	m_pTextHealthRemaining->setPosition( t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreHealthRemaining )
	//	m_pTextScoreHealthRemaining->setPosition( t2dVector( 150.0f, 230.0f ) );
	
	// Mana
	//if( m_pTextManaRemaining )
	//	m_pTextManaRemaining->setPosition( t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreManaRemaining )
	//	m_pTextScoreManaRemaining->setPosition( t2dVector( 150.0f, 230.0f ) );
	
	// Total
	if( m_pTextScoreTotal )
		m_pTextScoreTotal->setPosition( t2dVector( 75.0f, 126.0f ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::InitializePositionsForScoreScreen02()
{
	// Seeds
	if( m_pTextNumSeedsEaten )
		m_pTextNumSeedsEaten->setPosition(			t2dVector( -97.0f, -135.0f ) );
	
	if( m_pTextScoreNumSeedsEaten )
		m_pTextScoreNumSeedsEaten->setPosition(		t2dVector( 75.0f, -135.0f ) );
	
	// Grass
	if( m_pTextGrassTilesGrown )
		m_pTextGrassTilesGrown->setPosition(		t2dVector( -97.0f, -92.0f ) );
	
	if( m_pTextScoreGrassTilesGrown )
		m_pTextScoreGrassTilesGrown->setPosition(	t2dVector( 75.0f, -92.0f ) );
	
	// Enemy A
	if( m_pTextNumKilledA )
		m_pTextNumKilledA->setPosition(				t2dVector( -97.0f, -49.0f ) );
	
	if( m_pTextScoreNumKilledA )
		m_pTextScoreNumKilledA->setPosition(		t2dVector( 75.0f, -49.0f ) );
	
	// Enemy B
	if( m_pTextNumKilledB )
		m_pTextNumKilledB->setPosition(				t2dVector( -97.0f, -6.0f ) );
	
	if( m_pTextScoreNumKilledB )
		m_pTextScoreNumKilledB->setPosition(		t2dVector( 75.0f, -6.0f ) );
	
	// Health
	//if( m_pTextHealthRemaining )
	//	m_pTextHealthRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreHealthRemaining )
	//	m_pTextScoreHealthRemaining->setPosition(	t2dVector( 150.0f, 230.0f ) );
	
	// Mana
	//if( m_pTextManaRemaining )
	//	m_pTextManaRemaining->setPosition(			t2dVector( 150.0f, 230.0f) );
	
	//if( m_pTextScoreManaRemaining )
	//	m_pTextScoreManaRemaining->setPosition(		t2dVector( 150.0f, 230.0f) );
	
	// Total
	if( m_pTextScoreTotal )
		m_pTextScoreTotal->setPosition(				t2dVector( 75.0f, 126.0f ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::InitializePositionsForScoreScreen03()
{
	// Seeds
	if( m_pTextNumSeedsEaten )
		m_pTextNumSeedsEaten->setPosition(			t2dVector( -97.0f, -135.0f ) );
	
	if( m_pTextScoreNumSeedsEaten )
		m_pTextScoreNumSeedsEaten->setPosition(		t2dVector( 75.0f, -135.0f ) );
	
	// Grass
	if( m_pTextGrassTilesGrown )
		m_pTextGrassTilesGrown->setPosition(		t2dVector( -97.0f, -92.0f ) );
	
	if( m_pTextScoreGrassTilesGrown )
		m_pTextScoreGrassTilesGrown->setPosition(	t2dVector( 75.0f, -92.0f ) );
	
	// Crystals
	if( m_pTextCrystalsDestroyed )
		m_pTextCrystalsDestroyed->setPosition(		t2dVector( -97.0f, -49.0f ) );
	
	if( m_pTextScoreCrystalsDestroyed )
		m_pTextScoreCrystalsDestroyed->setPosition(	t2dVector( 75.0f, -49.0f ) );
	
	// Enemy A
	if( m_pTextNumKilledA )
		m_pTextNumKilledA->setPosition(				t2dVector( -97.0f, -6.0f ) );
	
	if( m_pTextScoreNumKilledA )
		m_pTextScoreNumKilledA->setPosition(		t2dVector( 75.0f, -6.0f ) );
	
	// Enemy B
	if( m_pTextNumKilledB )
		m_pTextNumKilledB->setPosition(				t2dVector( -97.0f, 37.0f ) );
	
	if( m_pTextScoreNumKilledB )
		m_pTextScoreNumKilledB->setPosition(		t2dVector( 75.0f, 37.0f ) );
	
	// Health
	//if( m_pTextHealthRemaining )
	//	m_pTextHealthRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreHealthRemaining )
	//	m_pTextScoreHealthRemaining->setPosition(	t2dVector( 150.0f, 230.0f ) );
	
	// Mana
	//if( m_pTextManaRemaining )
	//	m_pTextManaRemaining->setPosition(			t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreManaRemaining )
	//	m_pTextScoreManaRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	// Total
	if( m_pTextScoreTotal )
		m_pTextScoreTotal->setPosition(				t2dVector( 75.0f, 126.0f ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::InitializePositionsForScoreScreen04()
{
	// Seeds
	if( m_pTextNumSeedsEaten )
		m_pTextNumSeedsEaten->setPosition(			t2dVector( -97.0f, -135.0f ) );
	
	if( m_pTextScoreNumSeedsEaten )
		m_pTextScoreNumSeedsEaten->setPosition(		t2dVector( 75.0f, -135.0f ) );
	
	// Grass
	if( m_pTextGrassTilesGrown )
		m_pTextGrassTilesGrown->setPosition(		t2dVector( -97.0f, -92.0f ) );
	
	if( m_pTextScoreGrassTilesGrown )
		m_pTextScoreGrassTilesGrown->setPosition(	t2dVector( 75.0f, -92.0f ) );
	
	// Crystals
	if( m_pTextCrystalsDestroyed )
		m_pTextCrystalsDestroyed->setPosition(		t2dVector( -97.0f, -49.0f ) );
	
	if( m_pTextScoreCrystalsDestroyed )
		m_pTextScoreCrystalsDestroyed->setPosition(	t2dVector( 75.0f, -49.0f ) );
	
	// Enemy A
	if( m_pTextNumKilledA )
		m_pTextNumKilledA->setPosition(				t2dVector( -97.0f, -6.0f ) );
	
	if( m_pTextScoreNumKilledA )
		m_pTextScoreNumKilledA->setPosition(		t2dVector( 75.0f, -6.0f ) );
	
	// Enemy B
	if( m_pTextNumKilledB )
		m_pTextNumKilledB->setPosition(				t2dVector( -97.0f, 37.0f ) );
	
	if( m_pTextScoreNumKilledB )
		m_pTextScoreNumKilledB->setPosition(		t2dVector( 75.0f, 37.0f ) );
	
	// Smoke Monster
	if( m_pTextNumKilledSmoke )
		m_pTextNumKilledSmoke->setPosition(			t2dVector( -97.0f, 80.0f ) );
	
	if( m_pTextScoreNumKilledSmoke )
		m_pTextScoreNumKilledSmoke->setPosition(	t2dVector( 75.0f, 80.0f ) );
	
	// Health
	//if( m_pTextHealthRemaining )
	//	m_pTextHealthRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreHealthRemaining )
	//	m_pTextScoreHealthRemaining->setPosition(	t2dVector( 150.0f, 230.0f ) );
	
	// Mana
	//if( m_pTextManaRemaining )
	//	m_pTextManaRemaining->setPosition(			t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreManaRemaining )
	//	m_pTextScoreManaRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	// Total
	if( m_pTextScoreTotal )
		m_pTextScoreTotal->setPosition(				t2dVector( 75.0f, 126.0f ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::InitializePositionsForScoreScreenBoss()
{			
	// Seeds
	if( m_pTextNumSeedsEaten )
		m_pTextNumSeedsEaten->setPosition(			t2dVector( -97.0f, -135.0f ) );
	
	if( m_pTextScoreNumSeedsEaten )
		m_pTextScoreNumSeedsEaten->setPosition(		t2dVector( 75.0f, -135.0f ) );
	
	// Grass
	if( m_pTextGrassTilesGrown )
		m_pTextGrassTilesGrown->setPosition(		t2dVector( -97.0f, -92.0f ) );
	
	if( m_pTextScoreGrassTilesGrown )
		m_pTextScoreGrassTilesGrown->setPosition(	t2dVector( 75.0f, -92.0f ) );
	
	// Crystals
	if( m_pTextCrystalsDestroyed )
		m_pTextCrystalsDestroyed->setPosition(		t2dVector( -97.0f, -49.0f ) );
	
	if( m_pTextScoreCrystalsDestroyed )
		m_pTextScoreCrystalsDestroyed->setPosition(	t2dVector( 75.0f, -49.0f ) );
	
	// Enemy A
	if( m_pTextNumKilledA )
		m_pTextNumKilledA->setPosition(				t2dVector( -97.0f, -6.0f ) );
	
	if( m_pTextScoreNumKilledA )
		m_pTextScoreNumKilledA->setPosition(		t2dVector( 75.0f, -6.0f ) );
	
	// Smoke Monster
	if( m_pTextNumKilledSmoke )
		m_pTextNumKilledSmoke->setPosition(			t2dVector( -97.0f, 37.0f ) );
	
	if( m_pTextScoreNumKilledSmoke )
		m_pTextScoreNumKilledSmoke->setPosition(	t2dVector( 75.0f, 37.0f ) );
	
	// Health
	//if( m_pTextHealthRemaining )
	//	m_pTextHealthRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreHealthRemaining )
	//	m_pTextScoreHealthRemaining->setPosition(	t2dVector( 150.0f, 230.0f ) );
	
	// Mana
	//if( m_pTextManaRemaining )
	//	m_pTextManaRemaining->setPosition(			t2dVector( 150.0f, 230.0f ) );
	
	//if( m_pTextScoreManaRemaining )
	//	m_pTextScoreManaRemaining->setPosition(		t2dVector( 150.0f, 230.0f ) );
	
	// Total
	if( m_pTextScoreTotal )
		m_pTextScoreTotal->setPosition(				t2dVector( 75.0f, 126.0f ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::SetTextObjectFromName( t2dTextObject*& _pOutObject, const char* _pszObjectName )
{
	if( _pszObjectName == NULL || _pszObjectName[0] == '\0' )
		return;
	
	_pOutObject = static_cast<t2dTextObject*>( Sim::findObject( _pszObjectName ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::InitializeTextObjectData( t2dTextObject*& _pTextObject )
{
	if( _pTextObject == NULL )
		return;
	
	char szTemp[8];
	sprintf( szTemp, "0" );
	_pTextObject->setText( szTemp );
	
	_pTextObject->setBlendColour( g_ScoreScreenTextColor );
	
	_pTextObject->setLayer( LAYER_SCORE_SCREEN_ELEMENTS );
	_pTextObject->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnIncrementNumKilledEnemyA()
{
	CComponentGlobals::GetInstance().IncrementAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_BLOB );
	
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_BLOB ) == CComponentGlobals::GetAchievementGoalKilledBlob01() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_BLAVA_HUNTER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_BLOB ) == CComponentGlobals::GetAchievementGoalKilledBlob02() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_BLAVA_BANE );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnIncrementNumKilledEnemyB()
{
	CComponentGlobals::GetInstance().IncrementAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SERPENT );
	
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SERPENT ) == CComponentGlobals::GetAchievementGoalKilledSerpent01() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_DRAGON_SKINNER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SERPENT ) == CComponentGlobals::GetAchievementGoalKilledSerpent02() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_DRAGON_BANE );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnIncrementNumKilledSmokeMonster()
{
	CComponentGlobals::GetInstance().IncrementAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE );
	
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE ) == CComponentGlobals::GetAchievementGoalKilledSmoke01() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SMOGG_HUNTER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE ) == CComponentGlobals::GetAchievementGoalKilledSmoke02() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SMOGG_BANE );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnIncrementNumSeedsEaten()
{
	CComponentGlobals::GetInstance().IncrementAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED );
	
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED ) == CComponentGlobals::GetAchievementGoalSeedsCollected01() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SEED_EATER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED ) == CComponentGlobals::GetAchievementGoalSeedsCollected02() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SEED_HEAD );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED ) == CComponentGlobals::GetAchievementGoalSeedsCollected03() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_MASTER_OF_SEED );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnIncrementNumGrassTilesGrown()
{
	CComponentGlobals::GetInstance().IncrementAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN );
	
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) == CComponentGlobals::GetAchievementGoalGrassGrown01() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_LIFE_GIVER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) == CComponentGlobals::GetAchievementGoalGrassGrown02() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_FIELDS_OF_LIFE );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) == CComponentGlobals::GetAchievementGoalGrassGrown03() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_MASTER_OF_LIFE );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN ) == CComponentGlobals::GetAchievementGoalGrassGrown04() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_THE_TRUE_VINEKING );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnIncrementNumCrystalsDestroyed()
{
	CComponentGlobals::GetInstance().IncrementAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN );
	
	if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN ) == CComponentGlobals::GetAchievementGoalCrystalsBroken01() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_CRYSTAL_HUNTER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN ) == CComponentGlobals::GetAchievementGoalCrystalsBroken02() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_CRYSTAL_SMASHER );
	}
	else if( CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN ) == CComponentGlobals::GetAchievementGoalCrystalsBroken03() )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_CRYSTAL_BANE );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::CalculateScores()
{	
	m_iScoreEnemyA				= m_iNumKilledEnemyA		* g_iPointValueEnemyA;
	m_iScoreEnemyB				= m_iNumKilledEnemyB		* g_iPointValueEnemyB;
	m_iScoreSmokeMonster		= m_iNumKilledSmokeMonster	* g_iPointValueSmokeMonster;
	m_iScoreSeedsEaten			= m_iNumSeedsEaten			* g_iPointValueSeedEating;
	m_iScoreGrassTilesGrown		= m_iNumGrassTilesGrown		* g_iPointValueGrassGrown;
	m_iScoreCrystalsDestroyed	= m_iNumCrystalsDestroyed	* g_iPointValueDestroyCrystal;
	
	//m_iHealthScore = static_cast<S32>( CComponentPlayerHomeBase::GetInstance().GetPercentHomeBaseHealthRemaining() );
	//m_iManaScore = static_cast<S32>( CComponentManaBar::GetPercentManaRemaining() );
	
	m_iTotalScore = m_iScoreEnemyA + m_iScoreEnemyB + m_iScoreSmokeMonster + m_iScoreSeedsEaten + m_iScoreGrassTilesGrown + m_iScoreCrystalsDestroyed;
	
	CComponentGlobals::GetInstance().AddToTotalXP( m_iTotalScore );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::DisplayScoreTexts()
{
	DisplayScoreTextWithZeroes( m_pTextNumKilledA,			m_iNumKilledEnemyA );
	DisplayScoreTextWithZeroes( m_pTextNumKilledB,			m_iNumKilledEnemyB );
	DisplayScoreTextWithZeroes( m_pTextNumKilledSmoke,		m_iNumKilledSmokeMonster );
	DisplayScoreTextWithZeroes( m_pTextNumSeedsEaten,		m_iNumSeedsEaten );
	DisplayScoreTextWithZeroes( m_pTextGrassTilesGrown,		m_iNumGrassTilesGrown );
	DisplayScoreTextWithZeroes( m_pTextCrystalsDestroyed,	m_iNumCrystalsDestroyed );
	
	//DisplayScoreText( m_pTextHealthRemaining, m_iHealthScore );
	//DisplayScoreText( m_pTextManaRemaining, m_iManaScore );
	
	DisplayScoreText( m_pTextScoreNumKilledA,			m_iScoreEnemyA );
	DisplayScoreText( m_pTextScoreNumKilledB,			m_iScoreEnemyB );
	DisplayScoreText( m_pTextScoreNumKilledSmoke,		m_iScoreSmokeMonster );
	DisplayScoreText( m_pTextScoreNumSeedsEaten,		m_iScoreSeedsEaten );
	DisplayScoreText( m_pTextScoreGrassTilesGrown,		m_iScoreGrassTilesGrown );
	DisplayScoreText( m_pTextScoreCrystalsDestroyed,	m_iScoreCrystalsDestroyed );
	//DisplayScoreText( m_pTextScoreHealthRemaining,		m_iScoreEnemyA );
	//DisplayScoreText( m_pTextScoreManaRemaining,		m_iScoreEnemyA );
	//DisplayScoreText( m_pTextScoreHealthRemaining,		m_iHealthScore );
	//DisplayScoreText( m_pTextScoreManaRemaining,		m_iManaScore );
	
	DisplayScoreText( m_pTextScoreTotal,				m_iTotalScore );
	
	S32 iTotalBankXP = CComponentGlobals::GetInstance().GetTotalXP();
	DisplayScoreText( g_pText_Score_Mana_Bank, iTotalBankXP );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::DisplayScoreText( t2dTextObject* const _pTextObject, S32& _iScore )
{
	if( _pTextObject == NULL )
		return;
	
	char szTemp[32];
	sprintf( szTemp, "%d", _iScore );
	_pTextObject->setText( szTemp );
	
	_pTextObject->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::DisplayScoreTextWithZeroes( t2dTextObject* const _pTextObject, S32& _iScore )
{
	// The text on these objects should look like:  00X or 0XX or XXX
	if( _pTextObject == NULL )
		return;
	
	char szScore[32];
	if( _iScore < 10 )
	{
		sprintf( szScore, "00%d", _iScore );
	}
	else if( _iScore < 100 )
	{
		sprintf( szScore, "0%d", _iScore );
	}
	else
	{
		sprintf( szScore, "%d", _iScore );
	}
	
	//sprintf( szScore, "%d", _iScore );
	_pTextObject->setText( szScore );
	
	_pTextObject->setVisible( true );
}
	   
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	   
void CComponentScoringManager::CalculateAndDisplayGameTime()
{
	//printf( "CComponentScoringManager::CalculateAndDisplayGameTime\n" );
		   
	U32 uGameTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelTimeMilliseconds();
	
	F32 fGameTimeMinutes = ( (F32)uGameTimeMilliseconds * 0.001f ) / 60.0f;
	S32 iDisplayMinutes = (S32)fGameTimeMinutes;
	//if( iDisplayMinutes > 9 )
	//	iDisplayMinutes = 9;
	
	//printf( "fGameTimeMinutes: %f\n", fGameTimeMinutes );
	//printf( "iDisplayMinutes:  %d\n", iDisplayMinutes );
	
	F32 fGameTimeSeconds = (fGameTimeMinutes - (F32)iDisplayMinutes) * 60.0f;
	//printf( "fGameTimeSeconds (normal): %f\n",		fGameTimeSeconds );
	//printf( "fGameTimeSeconds (format): %0.2f\n",	fGameTimeSeconds );
	
	char szDisplayTime[32];
	if( fGameTimeSeconds < 10 )
		sprintf( szDisplayTime, "%d:0%0.2f", iDisplayMinutes, fGameTimeSeconds );
	else
		sprintf( szDisplayTime, "%d:%0.2f", iDisplayMinutes, fGameTimeSeconds );
	
	if( m_pGameTimerText )
	{
		m_pGameTimerText->setText( szDisplayTime );
		m_pGameTimerText->setVisible( true );
		m_pGameTimerText->setPosition( g_vGameTimerTextPosition );
	}
	
		   
	
	////////////////////////////////////////////////////////////////////////////////
	/// OLD WAY:
//	F32 fGameTimeSeconds = CComponentGlobals::GetInstance().GetLevelTimeSeconds();
//	
//	F32 fMinutes = fGameTimeSeconds / 60.0f;
//	//F32 fSeconds = fGameTimeSeconds % 60.0f;
//	
//	S32 iMinutes = static_cast<S32>( fMinutes );
//	
//	//printf( "Minutes (F32): %f\n", fMinutes );
//	//printf( "Minutes (int):	  %d\n", iMinutes );
//	
//	F32 fSecondsToSubtract = (F32)iMinutes * 60.0f;
//	F32 fSeconds = fGameTimeSeconds - fSecondsToSubtract;
//	
//	char szDisplayTime[16];
//	if( (S32)fSeconds < 10 )
//		sprintf( szDisplayTime, "%d:0%d", iMinutes, (S32)fSeconds );
//	else
//		sprintf( szDisplayTime, "%d:%d", iMinutes, (S32)fSeconds );
//	
//	if( m_pGameTimerText )
//	{
//		m_pGameTimerText->setText( szDisplayTime );
//		m_pGameTimerText->setVisible( true );
//		m_pGameTimerText->setPosition( g_vGameTimerTextPosition );
//	}
	
	//printf( "Seconds: %f\n", fSeconds );
	////////////////////////////////////////////////////////////////////////////////
	
	
//	// TEST: New time display -- MM:SS:HH  => Minutes:Seconds:Hundredths
//	{
//		U32 uTimeMilliseconds = CComponentGlobals::GetInstance().GetLevelTimeMilliseconds();
//		printf( "Time Milliseconds: %u\n", uTimeMilliseconds );
//		
//		
//		F32 fSeconds = (F32)uTimeMilliseconds * 0.001f;
//		F32 fMinutes = fSeconds / 60.0f;
//		
//		printf( " - Minutes: %f\n", fMinutes );
//		printf( " - Seconds: %f\n", fSeconds );
//	}
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::CalculateAndDisplayStarRanking()
{
	// Calculate Star Ranking based on time
	F32 fGameTimeSeconds = CComponentGlobals::GetInstance().GetLevelTimeSeconds();
	
	F32 fThreeStarRankingSeconds = g_fStarRanking03_SecondsMax;
	F32 fTwoStarRankingSeconds = g_fStarRanking02_SecondsMax;
	
	GetStarRankingValues( fThreeStarRankingSeconds, fTwoStarRankingSeconds );
	
	
	if( fGameTimeSeconds < fThreeStarRankingSeconds )
	{
		m_iStarRanking = 3;
		
		if( m_pStarMarker )
		{
			m_pStarMarker->setImageMap( g_szScore_StarMarkerImageMap, g_iFrameNumStarsThree );
			m_pStarMarker->setPosition( g_vStarMarkerPosition );
			m_pStarMarker->setVisible( true );
		}
	}
	else if( fGameTimeSeconds < fTwoStarRankingSeconds )
	{
		m_iStarRanking = 2;
		
		if( m_pStarMarker )
		{
			m_pStarMarker->setImageMap( g_szScore_StarMarkerImageMap, g_iFrameNumStarsTwo );
			m_pStarMarker->setPosition( g_vStarMarkerPosition );
			m_pStarMarker->setVisible( true );
		}
	}
	else
	{
		m_iStarRanking = 1;
		
		if( m_pStarMarker )
		{
			m_pStarMarker->setImageMap( g_szScore_StarMarkerImageMap, g_iFrameNumStarsOne );
			m_pStarMarker->setPosition( g_vStarMarkerPosition );
			m_pStarMarker->setVisible( true );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::GetStarRankingValues( F32& _fOutThreeStarValue, F32& _fOutTwoStarValue )
{
	const char* pszCurrentLevelName = CComponentGlobals::GetInstance().GetCurrentLevelName();
	if( pszCurrentLevelName == NULL )
		return;
	
	S32 iActiveSaveIndex = CComponentGlobals::GetInstance().GetActiveLevelSaveIndex();
	
	switch( iActiveSaveIndex )
	{
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_01_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_01_01[1];
			break;

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_02_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_02_01[1];
			break;

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_03_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_03_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_03_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_03_02[1];
			break;

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_04_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_04_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_04_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_04_02[1];
			break;
			

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_05_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_05_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_05_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_05_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_05_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_05_03[1];
			break;
			

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_06_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_06_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_06_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_06_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_06_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_06_03[1];
			break;
			

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_07_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_07_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_07_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_07_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_07_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_07_03[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04:
			_fOutThreeStarValue = g_fStarRankingSeconds_07_04[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_07_04[1];
			break;
			

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_08_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_08_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_08_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_08_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_08_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_08_03[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04:
			_fOutThreeStarValue = g_fStarRankingSeconds_08_04[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_08_04[1];
			break;
			

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_09_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_09_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_09_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_09_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_09_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_09_03[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04:
			_fOutThreeStarValue = g_fStarRankingSeconds_09_04[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_09_04[1];
			break;

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_10_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_10_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_10_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_10_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_10_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_10_03[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04:
			_fOutThreeStarValue = g_fStarRankingSeconds_10_04[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_10_04[1];
			break;
			

		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_Boss_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_Boss_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_Boss_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_Boss_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_Boss_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_Boss_03[1];
			break;
			
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01:
			_fOutThreeStarValue = g_fStarRankingSeconds_11_01[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_11_01[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02:
			_fOutThreeStarValue = g_fStarRankingSeconds_11_02[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_11_02[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03:
			_fOutThreeStarValue = g_fStarRankingSeconds_11_03[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_11_03[1];
			break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04:
			_fOutThreeStarValue = g_fStarRankingSeconds_11_04[0];
			_fOutTwoStarValue	= g_fStarRankingSeconds_11_04[1];
			break;
			
		default:
			printf( "GetStarRankingValues - Unhandled level save index: %d\n", iActiveSaveIndex );
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::UpdateFade()
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

void CComponentScoringManager::OnFadeOutFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	CComponentGlobals::GetInstance().SaveGameSettings();
	
	// Report Game Center Leaderboards
	CComponentGlobals::GetInstance().UploadScoresToGameCenter();
	
	
	//===========================================
	// Report Game Center Achievement percentages
	
	//printf( "-------------------------------------------------------\n" );
	
	// Blobs
	S32 iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_BLOB );
	F32 fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalKilledBlob01());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_BLAVA_HUNTER, fPercent );
	//printf( "Percentage for VK_BLAVA_HUNTER: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalKilledBlob02());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_BLAVA_BANE, fPercent );
	//printf( "Percentage for VK_BLAVA_BANE: %f\n", fPercent );
	
	
	// Serpents
	iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SERPENT );
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalKilledSerpent01());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_DRAGON_SKINNER, fPercent );
	//printf( "Percentage for VK_DRAGON_SKINNER: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalKilledSerpent02());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_DRAGON_BANE, fPercent );
	//printf( "Percentage for VK_DRAGON_BANE: %f\n", fPercent );
	
	
	// Smoke monsters
	iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE );
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalKilledSmoke01());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_SMOGG_HUNTER, fPercent );
	//printf( "Percentage for VK_SMOGG_HUNTER: %f\n", fPercent );
	
	//iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_NUM_KILLED_SMOKE );
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalKilledSmoke02());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_SMOGG_BANE, fPercent );
	//printf( "Percentage for VK_SMOGG_BANE: %f\n", fPercent );
	
	
	// Seeds
	iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_SEEDS_COLLECTED );
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalSeedsCollected01());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_SEED_EATER, fPercent );
	//printf( "Percentage for VK_SEED_EATER: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalSeedsCollected02());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_SEED_HEAD, fPercent );
	//printf( "Percentage for VK_SEED_HEAD: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalSeedsCollected03());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_MASTER_OF_SEED, fPercent );
	//printf( "Percentage for VK_MASTER_OF_SEED: %f\n", fPercent );
	
	
	// Grass
	iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_GRASS_GROWN );
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalGrassGrown01());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_LIFE_GIVER, fPercent );
	//printf( "Percentage for VK_LIFE_GIVER: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalGrassGrown02());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_FIELDS_OF_LIFE, fPercent );
	//printf( "Percentage for VK_FIELDS_OF_LIFE: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalGrassGrown03());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_MASTER_OF_LIFE, fPercent );
	//printf( "Percentage for VK_MASTER_OF_LIFE: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalGrassGrown04());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_THE_TRUE_VINEKING, fPercent );
	//printf( "Percentage for VK_THE_TRUE_VINEKING: %f\n", fPercent );
	
	
	// Crystals
	iAchievementTotal = CComponentGlobals::GetInstance().GetAchievementTotal( SAVE_DATA_INDEX_CRYSTALS_BROKEN );
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalCrystalsBroken01());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_CRYSTAL_HUNTER, fPercent );
	//printf( "Percentage for VK_CRYSTAL_HUNTER: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalCrystalsBroken02());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_CRYSTAL_SMASHER, fPercent );
	//printf( "Percentage for VK_CRYSTAL_SMASHER: %f\n", fPercent );
	
	fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalCrystalsBroken03());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_CRYSTAL_BANE, fPercent );
	//printf( "Percentage for VK_CRYSTAL_BANE: %f\n", fPercent );
	
	
	// Achievements for Full Health
	if( CComponentPlayerHomeBase::GetInstance().GetPercentHomeBaseHealthRemaining() >= 1.0f )
	{
		iAchievementTotal = CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullHealth();
		fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalFullHealth01());
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_HEALTHY_VINEKING, fPercent );
		//printf( "Percentage for VK_HEALTHY_VINEKING: %f\n", fPercent );
		
		fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalFullHealth02());
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_STRONG_VINEKING, fPercent );
		//printf( "Percentage for VK_STRONG_VINEKING: %f\n", fPercent );
		
		fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalFullHealth03());
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_VIGOROUS_VINEKING, fPercent );
		//printf( "Percentage for VK_VIGOROUS_VINEKING: %f\n", fPercent );
	}
	
	
	// Achievements for Full Mana
	if( CComponentManaBar::GetPercentManaRemaining() >= 1.0f )
	{
		iAchievementTotal = CComponentGlobals::GetInstance().GetNumLevelsBeatenWithFullPower();
		fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalFullMana01());
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_INITIATE_OF_POWER, fPercent );
		//printf( "Percentage for VK_INITIATE_OF_POWER: %f\n", fPercent );
		
		fPercent = (F32)iAchievementTotal / (F32)(CComponentGlobals::GetAchievementGoalFullMana02());
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_POWER_ADEPT, fPercent );
		//printf( "Percentage for VK_POWER_ADEPT: %f\n", fPercent );
		
		fPercent = (F32)iAchievementTotal / (F32)CComponentGlobals::GetAchievementGoalFullMana03();
		fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
		CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_AGENT_OF_POWER, fPercent );
		//printf( "Percentage for VK_AGENT_OF_POWER: %f\n", fPercent );
	}
	
	// Star achievements
	// - Num levels beaten with two or more stars
	fPercent = (F32)(CComponentGlobals::GetInstance().GetNumLevelsWithTwoOrMoreStars()) / (F32)(CComponentGlobals::GetInstance().GetNumLevels());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_TWOSTAR_VINEKING, fPercent );
	//printf( "Percentage for VK_TWOSTAR_VINEKING: %f\n", fPercent );
	
	// - Num levels beaten with three stars
	fPercent = (F32)(CComponentGlobals::GetInstance().GetNumLevelsWithThreeStars()) / (F32)(CComponentGlobals::GetInstance().GetNumLevels());
	fPercent = (100.0f * mClampF( fPercent, 0.0f, 1.0f ));
	CComponentAchievementBanner::ReportAchievementPercentage( GameCenterWrapper::VK_THREESTAR_VINEKING, fPercent );
	//printf( "Percentage for VK_THREESTAR_VINEKING: %f\n", fPercent );
	
	
	//==================================================================
	
	if( CComponentGlobals::GetInstance().GetActiveLevelSaveIndex() == SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 )
	{
		// Once Boss 3 is completed, force the all the Map Tutorials to be 'shown', so they don't possibly interfere with the DLC map tutorial message
		CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP );
		CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER01 );
		CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER02 );
		CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER03 );

		// Load the Story Ending
		CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szStoryEndLevelName );
	}
	else
	{
		CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szMapLevelName );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoringManager::OnFadeInFinished()
{
	m_eFadeState = FADE_STATE_NONE;
	
	//printf( "CComponentScoringManager::OnFadeInFinished\n" );
	
	// ENABLE THE BUTTON
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
