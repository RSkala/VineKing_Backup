//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMapSelectMenu.h"

#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentLevelUpScreen.h"
#include "ComponentMapSelectButton.h"
#include "ComponentMapSelection.h"

#include "SoundManager.h"

#include "T2D/t2dSceneWindow.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( -2000.0f, -2000.0f );
static const t2dVector g_vBackButtonPosition( -76.0, 106.0f );
static const t2dVector g_vLevel11BackButtonPosition( -135.0, 212.0 );
static const char g_szReturnToMainButtonName[32] = "ReturnToMainMenuButton";

static const char g_szMainMenuLevelName[64] = "game/data/levels/Screen_MainMenu.t2d";

// Fade stuff
static const char g_szFadeBox[32] = "FadeBox_Map";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeRPGSeconds = 0.25f;	// Time to fade to the RPG screen
static const F32 g_fFadeTimeLoadSeconds = 0.5f; // Time to fade in/out from/to a hard load
static F32 g_fFadeTimeSeconds = g_fFadeTimeLoadSeconds;

// Scene Window - For setting the camera position
static const char g_szSceneWindow[32] = "sceneWindow2D";
static t2dSceneWindow* g_pSceneWindow = NULL;

// Button enable timer
static const F32 g_fButtonEnableTimeSeconds = 0.25f;
static F32 g_fButtonEnableTimer;
static bool g_bEnablingButtonsFromTutorial = false;

static F32 g_fReturnToMainButtonEnableTimer;
static bool g_bEnablingButtonsFromMiniMap = false;

// Help Button
static const char g_szMap_Help_Button[32] = "Map_Help_Button";

// Help Arrow and VineKing
static const char g_szMapArrow[16] = "MapArrow";
static t2dStaticSprite* g_pMapArrow = NULL;
static const t2dVector g_vMapArrowOffset( 0.0f, -1.0f );

static const char g_szMap_VineKing[16] = "Map_VineKing";
static t2dSceneObject* g_pMap_VineKing = NULL;
static const t2dVector g_vMapVineKingOffset( 0.0f, -1.0f );

static t2dSceneObject* g_pMapArrowPositionObject = NULL;
static t2dSceneObject* g_pPreviousMapArrowPositionObject = NULL;
static S32 g_iHighestUnlockedMap = -1;

// Shop Screen Reminder
static const S32 g_iShopScreenReminderXP_01 = 3000;
static const S32 g_iShopScreenReminderXP_02 = 6000;
static const S32 g_iShopScreenReminderXP_03 = 10000;

static const S32 g_iShopScreenReminderMaxUpgrades_01 = 0;
static const S32 g_iShopScreenReminderMaxUpgrades_02 = 1;
static const S32 g_iShopScreenReminderMaxUpgrades_03 = 2;

static const char g_szNewWorldMap[32] = "world_map_DLCImageMap";

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelectMenu* CComponentMapSelectMenu::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMapSelectMenu );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelectMenu::CComponentMapSelectMenu()
	: m_pOwner( NULL )
	, m_pszGrayBoxObjectName( NULL )
	, m_pGrayBox( NULL )
	, m_pszBackButtonObjectName( NULL )
	, m_pBackButton( NULL )
	, m_pszRPGButtonObjectName( NULL )
	, m_pRPGButton( NULL )
	, m_pHelpButton( NULL )
	, m_pReturnToMainButton( NULL )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
	, m_eFadeToState( FADE_TO_NONE )
{
	CComponentMapSelectMenu::sm_pInstance = NULL;
	m_szSelectedLevelName[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelectMenu::~CComponentMapSelectMenu()
{
	CComponentMapSelectMenu::sm_pInstance = NULL;
	g_pMapArrowPositionObject = NULL;
	g_pPreviousMapArrowPositionObject = NULL;
	g_pMap_VineKing = NULL;
	g_iHighestUnlockedMap = -1;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// A 'Map Select' button opens a group of islands

void CComponentMapSelectMenu::AddMapSelectButton( CComponentMapSelectButton* const _pMapSelectButton )
{
	if( _pMapSelectButton == NULL )
		return;
	
	m_MapSelectButtonList.push_back( _pMapSelectButton );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnMapSelectButtonPressed()
{
	if( m_pGrayBox )
	{
		m_pGrayBox->setVisible( true );
		m_pGrayBox->setUseMouseEvents( true ); // This is an attempt to fix the issue where when backing out of a Map selection will sometimes press and open Map 3 or 4
	}
	
	// Disable input for all the Map Select buttons
	CComponentMapSelectButton* pMapSelectButton = NULL;
	S32 iNumMapSelectButtons = m_MapSelectButtonList.size();
	for( S32 i = 0; i < iNumMapSelectButtons; ++i )
	{
		pMapSelectButton = m_MapSelectButtonList[i];
		if( pMapSelectButton )
			pMapSelectButton->DisableInput();
	}
	
	if( m_pBackButton )
	{
		m_pBackButton->setPosition( g_vBackButtonPosition );
		m_pBackButton->setVisible( true );
		m_pBackButton->setUseMouseEvents( true );
	}
	
	if( m_pRPGButton )
	{
		m_pRPGButton->setUseMouseEvents( false );
	}
	
	if( m_pReturnToMainButton )
		m_pReturnToMainButton->setUseMouseEvents( false );
	
	if( m_pHelpButton )
		m_pHelpButton->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnLevel11MapSelectButtonPressed()
{
	if( m_pBackButton )
	{
		m_pBackButton->setPosition( g_vLevel11BackButtonPosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::AddMapSelection( CComponentMapSelection* const _pMapSelection )
{
	if( _pMapSelection == NULL )
		return;
	
	m_MapSelectionList.push_back( _pMapSelection );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnMapSelectionBackButtonPressed()
{
	if( m_pBackButton )
	{
		m_pBackButton->setPosition( g_vSafePosition );
		//m_pBackButton->setUseMouseEvents( false );
		m_pBackButton->setVisible( false );
	}
	
	// Disable input for all the Map Selections
	CComponentMapSelection* pMapSelection = NULL;
	S32 iNumMapSelections = m_MapSelectionList.size();
	for( S32 i = 0; i < iNumMapSelections; ++i )
	{
		pMapSelection = m_MapSelectionList[i];
		if( pMapSelection )
			pMapSelection->SetMapSelectionInactive();
	}
	
	//Enable input for all the Map Select buttons
	CComponentMapSelectButton* pMapSelectButton = NULL;
	S32 iNumMapSelectButtons = m_MapSelectButtonList.size();
	for( S32 i = 0; i < iNumMapSelectButtons; ++i )
	{
		pMapSelectButton = m_MapSelectButtonList[i];
		if( pMapSelectButton )
			pMapSelectButton->EnableInput();
	}
	
	if( m_pRPGButton )
	{
		//m_pRPGButton->setVisible( true );
		m_pRPGButton->setUseMouseEvents( true );
	}
	
	if( m_pGrayBox )
	{
		m_pGrayBox->setVisible( false );
	}
	
	//if( m_pReturnToMainButton )
	//	m_pReturnToMainButton->setUseMouseEvents( true );
	g_bEnablingButtonsFromMiniMap = true;
	
	if( m_pHelpButton )
		m_pHelpButton->setUseMouseEvents( true );
	
	ShowHelpArrowFromMapSelectionBackPressed();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnMapSelectionRPGButtonPressed()
{
	DisableAllMapScreenButtons();
	
	// Start Fade Out
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_fFadeTimer = 0.0f;
	m_eFadeToState = FADE_TO_RPG_SCREEN;
	
	g_fFadeTimeSeconds = g_fFadeTimeRPGSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnReturnToMainMenuButtonPressed()
{
	DisableAllMapScreenButtons();
	
	// Start Fade Out
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_fFadeTimer = 0.0f;
	m_eFadeToState = FADE_TO_MAIN_MENU;
	//CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szMainMenuLevelName );
	
	g_fFadeTimeSeconds = g_fFadeTimeLoadSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnLevelSelectButtonPressed( const char* _pszLevelName )
{
	DisableAllMapScreenButtons();
	
	// Start Fade Out
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_fFadeTimer = 0.0f;
	m_eFadeToState = FADE_TO_SELECTED_LEVEL;
	
	//printf( "Selected Level: %s\n", _pszLevelName );
	
	strcpy( m_szSelectedLevelName, _pszLevelName );
	
	g_fFadeTimeSeconds = g_fFadeTimeLoadSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnHelpButtonPressed()
{
	if( CComponentInGameTutorialObject::DoesLevelHaveMapTutorial() )
	{
		DisableAllMapScreenButtons();
		CComponentInGameTutorialObject::ActivateMapTutorial();
	}
}
	
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OpenMapSelectScreen()
{
	if( m_pOwner )
	{
		m_pOwner->setPosition( t2dVector::getZero() );
		
		if( g_pFadeBox )
		{
			g_pFadeBox->setVisible( true );
			g_pFadeBox->setBlendAlpha( 1.0f );
			
			g_pFadeBox->setPosition( m_pOwner->getPosition() );
		}
		
		if( g_pSceneWindow )
			g_pSceneWindow->mount( m_pOwner, t2dVector::getZero(), 0.0f, true );
		
		// Make sure the Back Button is not visible
		if( m_pBackButton )
		{
			m_pBackButton->setPosition( g_vSafePosition );
			m_pBackButton->setVisible( false );
		}
	}
	
	
	// Move Camera, Start Fade In
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds = g_fFadeTimeRPGSeconds;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::EnableMapScreenButtonsFromTutorial()
{
	//EnableMapScreenButtons(); // NOTE: Enabling the buttons here causes a problem where the player automatically opens the RPG screen
	g_fButtonEnableTimer = 0.0f;
	g_bEnablingButtonsFromTutorial = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::SetMapHelpArrowPositionObject( const t2dSceneObject* _pObject, const S32& _iLevel )
{
	if( g_pMapArrow == NULL || g_pMap_VineKing == NULL )
		return;
	
	if( g_iHighestUnlockedMap >= _iLevel )
		return;
	
	g_iHighestUnlockedMap = _iLevel;
	
	g_pMapArrowPositionObject = const_cast<t2dSceneObject*>( _pObject );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::HideHelpArrowFromMapSelectionPressed()
{
	if( g_pMapArrow )
		g_pMapArrow->setVisible( false );
	
	if( g_pMap_VineKing )
		g_pMap_VineKing->setVisible( false );
	
	// Disable the "Return to Main Menu" button. This stops the problem where the user exits to the main menu after pressing a mini map back button.
	if( m_pReturnToMainButton )
		m_pReturnToMainButton->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::ShowHelpArrowFromMapSelectionBackPressed()
{
	if( g_pMapArrow && g_pMap_VineKing && g_pMapArrowPositionObject )
	{
		// Arrow
		if( g_pMapArrow->getIsMounted() )
			g_pMapArrow->setVisible( true );
		
		// VineKing
		if( g_pMap_VineKing->getIsMounted() )
			g_pMap_VineKing->setVisible( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::AddMapHelpArrowToLevelSelectButton( const t2dSceneObject* _pObject )
{
	if( _pObject == NULL )
		return;
	
	if( g_pMapArrow && g_pMap_VineKing )
	{
		g_pPreviousMapArrowPositionObject = g_pMapArrowPositionObject;
		g_pMapArrowPositionObject = const_cast<t2dSceneObject*>( _pObject );
		
		// Arrow
		g_pMapArrow->dismount();
		g_pMapArrow->mount( g_pMapArrowPositionObject, g_vMapArrowOffset, 0.0f, false, true, false, false );
		
		g_pMapArrow->setVisible( true );
		
		// VineKing
		g_pMap_VineKing->dismount();
		g_pMap_VineKing->mount( g_pMapArrowPositionObject, g_vMapVineKingOffset, 0.0f, false, true, false, false );
		
		g_pMap_VineKing->setVisible( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::RemoveMapHelpArrowFromLevelSelectButton( const t2dSceneObject* _pObject )
{
	if( _pObject != g_pMapArrowPositionObject )
		return;
	
	if( g_pMapArrow && g_pMap_VineKing && g_pPreviousMapArrowPositionObject )
	{
		g_pMapArrowPositionObject = g_pPreviousMapArrowPositionObject;
		g_pPreviousMapArrowPositionObject = NULL;
		
		// Detach the Arrow from the level select button
		g_pMapArrow->dismount();
		
		// Reattach the Arrow to the map select button
		g_pMapArrow->mount( g_pMapArrowPositionObject, g_vMapArrowOffset, 0.0f, false, true, false, false );
		g_pMapArrow->setVisible( true );
		
		
		// Detach the VineKing from the level select button
		g_pMap_VineKing->dismount();
		
		// Reattach the VineKing to the map select button
		g_pMap_VineKing->mount( g_pMapArrowPositionObject, g_vMapVineKingOffset, 0.0f, false, true, false, false );
		g_pMap_VineKing->setVisible( true );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMapSelectMenu::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMapSelectMenu ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "GrayBox",	TypeString, Offset( m_pszGrayBoxObjectName,		CComponentMapSelectMenu ) );
	addField( "BackButton", TypeString, Offset( m_pszBackButtonObjectName,	CComponentMapSelectMenu ) );
	addField( "RPGButton",	TypeString, Offset( m_pszRPGButtonObjectName,	CComponentMapSelectMenu ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMapSelectMenu::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentMapSelectMenu::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	CComponentMapSelectMenu::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
	
	if( g_bEnablingButtonsFromTutorial )
	{
		g_fButtonEnableTimer += ITickable::smTickSec;
		if( g_fButtonEnableTimer >= g_fButtonEnableTimeSeconds )
		{
			EnableMapScreenButtons();
			g_bEnablingButtonsFromTutorial = false;
		}
	}
	
	if( g_bEnablingButtonsFromMiniMap )
	{
		g_fReturnToMainButtonEnableTimer += ITickable::smTickSec;
		if( g_fReturnToMainButtonEnableTimer >= g_fButtonEnableTimeSeconds )
		{
			if( m_pReturnToMainButton )
				m_pReturnToMainButton->setUseMouseEvents( true );
			g_bEnablingButtonsFromMiniMap = false;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnPostInit()
{	
	if( m_pszGrayBoxObjectName && m_pszGrayBoxObjectName[0] != '\0' )
	{
		m_pGrayBox = static_cast<t2dSceneObject*>( Sim::findObject( m_pszGrayBoxObjectName ) );
		if( m_pGrayBox )
		{
			t2dVector vPosition( 0.0f, 0.0f );
			m_pGrayBox->setPosition( vPosition );
			m_pGrayBox->setVisible( false );
			m_pGrayBox->setUseMouseEvents( true );
		}
	}
	
	if( m_pszBackButtonObjectName && m_pszBackButtonObjectName[0] != '\0' )
	{
		m_pBackButton = static_cast<t2dSceneObject*>( Sim::findObject( m_pszBackButtonObjectName ) );
		if( m_pBackButton )
		{
			//m_pBackButton->setPosition( t2dVector::getZero() );
			m_pBackButton->setUseMouseEvents( false );
			m_pBackButton->setVisible( false );
		}
	}
	
	if( m_pszRPGButtonObjectName && m_pszRPGButtonObjectName[0] != '\0' )
	{
		m_pRPGButton = static_cast<t2dSceneObject*>( Sim::findObject( m_pszRPGButtonObjectName ) );
		if( m_pRPGButton )
		{
			m_pRPGButton->setUseMouseEvents( false );
			m_pRPGButton->setVisible( true );
		}
	}
	
	m_pReturnToMainButton = static_cast<t2dSceneObject*>( Sim::findObject( g_szReturnToMainButtonName ) );
	if( m_pReturnToMainButton )
	{
		m_pReturnToMainButton->setUseMouseEvents( false );
		m_pReturnToMainButton->setVisible( true );
	}
	
	CComponentGlobals::GetInstance().SetActiveLevelSaveIndex( -1 );
	
	if( m_pOwner )
		m_pOwner->setPosition( t2dVector::getZero() );
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
	{
		g_pFadeBox->setPosition( m_pOwner ? m_pOwner->getPosition() : t2dVector::getZero() );
		g_pFadeBox->setVisible( true );
		g_pFadeBox->setBlendAlpha( 1.0f );
		
		// Start by fading in
		m_eFadeState = FADE_STATE_FADING_IN;
		m_fFadeTimer = g_fFadeTimeSeconds = g_fFadeTimeLoadSeconds;
	}
	
	m_pHelpButton = static_cast<t2dSceneObject*>( Sim::findObject( g_szMap_Help_Button ) );
	
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( g_szSceneWindow ) );
	
	g_pMapArrow = static_cast<t2dStaticSprite*>( Sim::findObject( g_szMapArrow ) );
	if( g_pMapArrow )
		g_pMapArrow->setVisible( true );
	
	g_pMap_VineKing = static_cast<t2dStaticSprite*>( Sim::findObject( g_szMap_VineKing ) );
	if( g_pMap_VineKing )
		g_pMap_VineKing->setVisible( true );
	
	
	// If the final Boss has been beaten, then change the image
	if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
	{
		if( m_pOwner )
		{
			t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			if( pSprite )
				pSprite->setImageMap( g_szNewWorldMap, 0 );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::EnableMapScreenButtons()
{
	//printf( "EnableMapScreenButtons\n" );
	
	if( m_pRPGButton )
		m_pRPGButton->setUseMouseEvents( true );
	
	if( m_pBackButton )
		m_pBackButton->setUseMouseEvents( true );
	
	if( m_pReturnToMainButton )
		m_pReturnToMainButton->setUseMouseEvents( true );
	
	if( m_pHelpButton )
		m_pHelpButton->setUseMouseEvents( true );
	
	//Enable input for all the Map Select buttons (The buttons themselves will handle whether or not to be enabled).
	CComponentMapSelectButton* pMapSelectButton = NULL;
	S32 iNumMapSelectButtons = m_MapSelectButtonList.size();
	for( S32 i = 0; i < iNumMapSelectButtons; ++i )
	{
		pMapSelectButton = m_MapSelectButtonList[i];
		if( pMapSelectButton )
			pMapSelectButton->EnableInput();
	}
	
	
	if( g_pMapArrow && g_pMap_VineKing && g_pMapArrowPositionObject )
	{
		// Arrow
		g_pMapArrow->dismount();
		
		if( g_pMapArrowPositionObject )
			g_pMapArrow->mount( g_pMapArrowPositionObject, g_vMapArrowOffset, 0.0f, false, true, false, false );
		else
			g_pMapArrow->setPosition( g_vSafePosition );
		
		// VineKing
		g_pMap_VineKing->dismount();
		
		if( g_pMapArrowPositionObject )
			g_pMap_VineKing->mount( g_pMapArrowPositionObject, g_vMapVineKingOffset, 0.0f, false, true, false, false );
		else
			g_pMap_VineKing->setPosition( g_vSafePosition ); 
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::DisableAllMapScreenButtons()
{
	//printf( "DisableAllMapScreenButtons\n" );
	
	if( m_pRPGButton )
		m_pRPGButton->setUseMouseEvents( false );
	
	if( m_pBackButton )
		m_pBackButton->setUseMouseEvents( false );
	
	if( m_pReturnToMainButton )
		m_pReturnToMainButton->setUseMouseEvents( false );
	
	// Disable input for all the Map Select buttons
	CComponentMapSelectButton* pMapSelectButton = NULL;
	S32 iNumMapSelectButtons = m_MapSelectButtonList.size();
	for( S32 i = 0; i < iNumMapSelectButtons; ++i )
	{
		pMapSelectButton = m_MapSelectButtonList[i];
		if( pMapSelectButton )
			pMapSelectButton->DisableInput();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::UpdateFade()
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

void CComponentMapSelectMenu::OnFadeOutFinished() // Called when completely black
{
	//printf( "CComponentMapSelectMenu::OnFadeOutFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;
	
	if( m_pOwner )
	{
		m_pOwner->setPosition( g_vSafePosition );
		
		if( g_pFadeBox )
		{
			g_pFadeBox->setBlendAlpha( 1.0f );
			g_pFadeBox->setPosition( m_pOwner->getPosition() );
		}
		
		if( m_pBackButton )
		{
			m_pBackButton->setPosition( m_pOwner->getPosition() );
			m_pBackButton->setVisible( false );
		}
	}
	
	switch( m_eFadeToState )
	{			
		case FADE_TO_RPG_SCREEN:
			CComponentLevelUpScreen::GetInstance().OpenLevelUpScreen(); // This function should change the camera and then start a fade in (handled on that end)
			break;
			
		case FADE_TO_MAIN_MENU:
			CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szMainMenuLevelName );
			break;
			
		case FADE_TO_SELECTED_LEVEL:
			if( m_szSelectedLevelName[0] != '\0' )
			{
				CComponentGlobals::GetInstance().ScheduleLoadLevel( m_szSelectedLevelName );
			}
			else
			{
				printf( "Selected level not properly set. Loading to Main Menu.\n" );
				CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szMainMenuLevelName );
			}
			
			break;
			
		default:
			break;
	}
	
	m_eFadeToState = FADE_TO_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectMenu::OnFadeInFinished() // Called when completely transparent
{
	//printf( "CComponentMapSelectMenu::OnFadeInFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;
	
	if( g_pFadeBox )
		g_pFadeBox->setBlendAlpha( 0.0f );
	
	g_bEnablingButtonsFromTutorial = false;
	
	if( CComponentInGameTutorialObject::DoesLevelHaveMapTutorial() )
	{
		if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP ) == false )
		{
			CComponentInGameTutorialObject::ActivateMapTutorial();
			CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP );
		}
		else if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER01 ) == false &&
				 CComponentGlobals::GetInstance().GetTotalXP() >= g_iShopScreenReminderXP_01 && 
				 CComponentGlobals::GetInstance().GetNumUpgradesPurchased() <= g_iShopScreenReminderMaxUpgrades_01 )
		{
			// Activate Shop Reminder Tutorial #1
			CComponentInGameTutorialObject::ActivateShopReminderTutorial();
			CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER01 );
		}
		else if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER02 ) == false &&
				 CComponentGlobals::GetInstance().GetTotalXP() >= g_iShopScreenReminderXP_02 && 
				 CComponentGlobals::GetInstance().GetNumUpgradesPurchased() <= g_iShopScreenReminderMaxUpgrades_02 )
		{
			// Activate Shop Reminder Tutorial #2
			CComponentInGameTutorialObject::ActivateShopReminderTutorial();
			CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER02 );
		}
		else if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER03 ) == false &&
				 CComponentGlobals::GetInstance().GetTotalXP() >= g_iShopScreenReminderXP_03 && 
				 CComponentGlobals::GetInstance().GetNumUpgradesPurchased() <= g_iShopScreenReminderMaxUpgrades_03 )
		{
			// Activate Shop Reminder Tutorial #3
			CComponentInGameTutorialObject::ActivateShopReminderTutorial();
			CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SHOP_REMINDER03 );
		}
		else if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP_DLC_INTRO ) == false &&
				 CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
		{
			// Activate DLC Map Announcement
			CComponentInGameTutorialObject::ActivateDLCMapAnnouncementTutorial();
			CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_MAP_DLC_INTRO );
		}
		else
		{
			//g_bEnablingButtonsFromTutorial = false;
			EnableMapScreenButtons();
		}
	}
	else
	{
		// This should never occur
		//g_bEnablingButtonsFromTutorial = false;
		EnableMapScreenButtons();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------