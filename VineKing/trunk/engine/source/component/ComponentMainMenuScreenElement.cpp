//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMainMenuScreenElement.h"

#include "ComponentGlobals.h"
#include "ComponentMainMenuScreen.h"

#include "GameCenterWrapper.h"
#include "SoundManager.h"

#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static ColorF g_ColorRed( 1.0f, 0.0f, 0.0f );
static ColorF g_ColorGreen( 0.0f, 1.0f, 0.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMainMenuScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMainMenuScreenElement::CComponentMainMenuScreenElement()
	: m_pOwner( NULL )
	, m_pszElementType( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMainMenuScreenElement::~CComponentMainMenuScreenElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMainMenuScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMainMenuScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "ElementType", TypeString, Offset( m_pszElementType, CComponentMainMenuScreenElement ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMainMenuScreenElement::onComponentAdd( SimComponent* _pTarget )
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

void CComponentMainMenuScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{	
	if( m_pOwner == NULL )
		return;
	
	if( CComponentMainMenuScreen::GetInstance().IsFading() )
		return;
	
	if( m_eElementType == ELEMENT_TYPE_NONE )
	{
		return;
	}
		
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentMainMenuScreen::GetInstance().IsFading() )
		return;
	
	//printf( "CComponentMainMenuScreenElement::OnMouseUp: %s\n", m_pOwner->getName() );
	
	// Debug Buttons
	if( m_eElementType == ELEMENT_TYPE_NONE )
	{
		CSoundManager::GetInstance().PlayButtonClickForward();
		
		if( strcmp( m_pOwner->getName(), "DEBUG_UnlockAllLevels_Button" ) == 0 )
		{
			CComponentGlobals::GetInstance().Debug_UnlockAllLevels();
			m_pOwner->setBlendColour( g_ColorGreen );
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_UnlimitedMana_Button" ) == 0 )
		{
			// Toggle Unlimited Mana
			if( CComponentGlobals::GetInstance().Debug_IsUnlimitedManaEnabled() )
			{
				m_pOwner->setBlendColour( g_ColorRed );
				CComponentGlobals::GetInstance().Debug_DisableUnlimitedMana();
			}
			else
			{
				m_pOwner->setBlendColour( g_ColorGreen );
				CComponentGlobals::GetInstance().Debug_EnableUnlimitedMana();
			}
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_PlayerInvincible_Button" ) == 0 )
		{
			// Toggle Player Invincibility
			if( CComponentGlobals::GetInstance().Debug_IsPlayerInvincibleEnabled() )
			{
				m_pOwner->setBlendColour( g_ColorRed );
				CComponentGlobals::GetInstance().Debug_DisablePlayerInvincible();
			}
			else
			{
				m_pOwner->setBlendColour( g_ColorGreen );
				CComponentGlobals::GetInstance().Debug_EnablePlayerInvincible();
			}
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_SpawningDisabled_Button" ) == 0 )
		{
			// Toggle Spawning Disabled
			if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
			{
				m_pOwner->setBlendColour( g_ColorRed );
				CComponentGlobals::GetInstance().Debug_EnableSpawning();
			}
			else
			{
				m_pOwner->setBlendColour( g_ColorGreen );
				CComponentGlobals::GetInstance().Debug_DisableSpawning();
			}
		}
		
		return;
	}
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	switch( m_eElementType )
	{
		// Buttons
		case ELEMENT_TYPE_NEWGAME_TEXT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnNewGameButtonPressed();
			break;
			
		case ELEMENT_TYPE_CONTINUE_TEXT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnContinueButtonPressed();
			break;
			
		case ELEMENT_TYPE_TUTORIAL_TEXT:
            //printf( "\n\n\nCOCK!!!!!!!!!!!!!!!!!!!!!!!\n\n\n" );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnTutorialButtonPressed();
			break;
			
		case ELEMENT_TYPE_OPTIONS_TEXT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().CloseMainMenuScreen();
			CComponentMainMenuScreen::GetInstance().OpenOptionsScreen();
			break;
			
		case ELEMENT_TYPE_CLEARDATA_TEXT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().CloseOptionsScreen();
			CComponentMainMenuScreen::GetInstance().OpenClearDataScreen();
			break;
			
		case ELEMENT_TYPE_CREDITS_TEXT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().CloseOptionsScreen();
			CComponentMainMenuScreen::GetInstance().OpenCreditsScreen();
			break;
			
		case ELEMENT_TYPE_CLEARDATA_YES_TEXT:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnClearDataButtonPressed();
			break;
			
		case ELEMENT_TYPE_CLEARDATA_NO_TEXT:
			CSoundManager::GetInstance().PlayButtonClickBack();
			CComponentMainMenuScreen::GetInstance().CloseClearDataScreen();
			CComponentMainMenuScreen::GetInstance().OpenOptionsScreen();
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnMuteButtonPressed();
			break;
			
		case ELEMENT_TYPE_BACK_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickBack();
			CComponentMainMenuScreen::GetInstance().OnBackButtonPressed();
			break;
			
		case ELEMENT_TYPE_GAME_CENTER_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnGameCenterButtonPressed();
			break;
			
		case ELEMENT_TYPE_CREDITS_NEXT_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnCreditsNextButtonPressed();
			break;
			
		case ELEMENT_TYPE_GAMECENTER_RESET_ACHIEVEMENTS_BUTTON:
			m_pOwner->setBlendColour( g_ColorRed );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnGameCenterResetAchievementsButtonPressed();
			break;
			
		case ELEMENT_TYPE_MY_SCORES_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnMyScoresButtonPressed();
			break;
			
		case ELEMENT_TYPE_HEART_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnHeartButtonPressed();
			break;
			
		case ELEMENT_TYPE_FACEBOOK_LOGIN_BUTTON:
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentMainMenuScreen::GetInstance().OnFacebookLoginButtonPressed();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentMainMenuScreen::GetInstance().IsFading() )
		return;
	
	if( m_eElementType == ELEMENT_TYPE_NONE )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentMainMenuScreen::GetInstance().IsFading() )
		return;
	
	if( m_eElementType == ELEMENT_TYPE_NONE )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	if( m_eElementType == ELEMENT_TYPE_GAMECENTER_RESET_ACHIEVEMENTS_BUTTON )
		m_pOwner->setBlendColour( g_ColorRed );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMainMenuScreenElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	ConvertElementTypeStringToEnum();

	// DEBUG
	if( m_eElementType == ELEMENT_TYPE_NONE )
	{
		if( strcmp( m_pOwner->getName(), "GameCenter_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_GAME_CENTER_BUTTON;
		}
		else if( strcmp( m_pOwner->getName(), "Credits_Next_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_CREDITS_NEXT_BUTTON;
		}
		else if( strcmp( m_pOwner->getName(), "MyScores_Text" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_MY_SCORES_BUTTON;
		}
		else if( strcmp( m_pOwner->getName(), "Heart_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_HEART_BUTTON;
		}
		else if( strcmp( m_pOwner->getName(), "FaceBook_Options_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_FACEBOOK_LOGIN_BUTTON;
		}
		else if( strcmp( m_pOwner->getName(), "GameCenter_ResetAchievements_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_GAMECENTER_RESET_ACHIEVEMENTS_BUTTON;
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_UnlockAllLevels_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_NONE;
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setBlendColour( g_ColorRed );
			return;
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_UnlimitedMana_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_NONE;
			m_pOwner->setUseMouseEvents( true );
			
			if( CComponentGlobals::GetInstance().Debug_IsUnlimitedManaEnabled() )
				m_pOwner->setBlendColour( g_ColorGreen );
			else
				m_pOwner->setBlendColour( g_ColorRed );
			
			return;
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_PlayerInvincible_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_NONE;
			m_pOwner->setUseMouseEvents( true );
			
			if( CComponentGlobals::GetInstance().Debug_IsPlayerInvincibleEnabled() )
				m_pOwner->setBlendColour( g_ColorGreen );
			else
				m_pOwner->setBlendColour( g_ColorRed );
				
			return;
		}
		else if( strcmp( m_pOwner->getName(), "DEBUG_SpawningDisabled_Button" ) == 0 )
		{
			m_eElementType = ELEMENT_TYPE_NONE;
			m_pOwner->setUseMouseEvents( true );
			
			if( CComponentGlobals::GetInstance().Debug_IsSpawningDisabled() )
				m_pOwner->setBlendColour( g_ColorGreen );
			else
				m_pOwner->setBlendColour( g_ColorRed );
			
			return;
		}
	}
	
	switch( m_eElementType )
	{
		// Screens
		case ELEMENT_TYPE_MAINMENUSCREEN_BG:
			m_pOwner->setPosition( t2dVector::getZero() );
			m_pOwner->setVisible( true );
			// This object should always be visible
			//CComponentMainMenuScreen::GetInstance().SetMainMenuBGObject( m_pOwner ); 
			break;
			
		case ELEMENT_TYPE_OPTIONSSCREEN_BG:
			m_pOwner->setPosition( t2dVector::getZero() );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddOptionsScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CREDITSSCREEN_BG:
			m_pOwner->setPosition( t2dVector::getZero() );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddCreditsScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CLEARDATASCREEN_BG:
			m_pOwner->setPosition( t2dVector::getZero() );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddClearDataScreenElement( m_pOwner );
			break;
			
		// Buttons
		case ELEMENT_TYPE_NEWGAME_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			//CComponentMainMenuScreen::GetInstance().AddMainMenuScreenElement( m_pOwner );
			CComponentMainMenuScreen::GetInstance().SetNewGameButton( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CONTINUE_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			//CComponentMainMenuScreen::GetInstance().AddMainMenuScreenElement( m_pOwner );
			CComponentMainMenuScreen::GetInstance().SetContinueButton( m_pOwner );
			break;
			
		case ELEMENT_TYPE_TUTORIAL_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddMainMenuScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_OPTIONS_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddMainMenuScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CLEARDATA_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddOptionsScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CREDITS_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddOptionsScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CLEARDATA_YES_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddClearDataScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CLEARDATA_NO_TEXT:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().AddClearDataScreenElement( m_pOwner );
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().SetMuteButton( m_pOwner );
			break;
			
		case ELEMENT_TYPE_BACK_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			CComponentMainMenuScreen::GetInstance().SetBackButton( m_pOwner );
			break;
			
		case ELEMENT_TYPE_GAME_CENTER_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			
			if( GameCenterWrapper::IsGameCenterAvailableOnDevice() )
				m_pOwner->setVisible( true );
			else 
				m_pOwner->setVisible( false );
			
			CComponentMainMenuScreen::GetInstance().SetGameCenterButton( m_pOwner );
			break;
			
		case ELEMENT_TYPE_CREDITS_NEXT_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			break;
			
		case ELEMENT_TYPE_GAMECENTER_RESET_ACHIEVEMENTS_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( true );
			break;
			
		case ELEMENT_TYPE_MY_SCORES_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( true );
			break;
			
		case ELEMENT_TYPE_HEART_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( true );
			break;
			
		case ELEMENT_TYPE_FACEBOOK_LOGIN_BUTTON:
			m_pOwner->setUseMouseEvents( true );
			m_pOwner->setVisible( false );
			break;
			
		default:
			break;
	}
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE

void CComponentMainMenuScreenElement::ConvertElementTypeStringToEnum()
{
	if( m_pszElementType == NULL )
		return;
	
	// %mainMenuScreenElementTypes = 
	//			"NONE"					TAB 
	//			"MAINMENUSCREEN_BG"		TAB
	//			"OPTIONSSCREEN_BG"		TAB
	//			"CREDITSSCREEN_BG"		TAB
	//			"CLEARDATASCREEN_BG"	TAB
	//			"NEWGAME_TEXT"			TAB
	//			"CONTINUE_TEXT"			TAB
	//			"TUTORIAL_TEXT"			TAB
	//			"OPTIONS_TEXT"			TAB
	//			"CLEARDATA_TEXT"		TAB
	//			"CREDITS_TEXT"			TAB
	//			"CLEARDATA_YES_TEXT"	TAB
	//			"CLEARDATA_NO_TEXT"		TAB
	//			"MUTE_BUTTON"			TAB
	//			"BACK_BUTTON";
	
	if( dStricmp( m_pszElementType, "MAINMENUSCREEN_BG" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MAINMENUSCREEN_BG;
	}
	else if( dStricmp( m_pszElementType, "OPTIONSSCREEN_BG" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_OPTIONSSCREEN_BG;
	}
	else if( dStricmp( m_pszElementType, "CREDITSSCREEN_BG" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CREDITSSCREEN_BG;
	}
	else if( dStricmp( m_pszElementType, "CLEARDATASCREEN_BG" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CLEARDATASCREEN_BG;
	}
	else if( dStricmp( m_pszElementType, "NEWGAME_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_NEWGAME_TEXT;
	}
	else if( dStricmp( m_pszElementType, "CONTINUE_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONTINUE_TEXT;
	}
	else if( dStricmp( m_pszElementType, "TUTORIAL_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_TUTORIAL_TEXT;
	}
	else if( dStricmp( m_pszElementType, "OPTIONS_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_OPTIONS_TEXT;
	}
	else if( dStricmp( m_pszElementType, "CLEARDATA_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CLEARDATA_TEXT;
	}
	else if( dStricmp( m_pszElementType, "CREDITS_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CREDITS_TEXT;
	}
	else if( dStricmp( m_pszElementType, "CLEARDATA_YES_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CLEARDATA_YES_TEXT;
	}
	else if( dStricmp( m_pszElementType, "CLEARDATA_NO_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CLEARDATA_NO_TEXT;
	}
	else if( dStricmp( m_pszElementType, "MUTE_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MUTE_BUTTON;
	}
	else if( dStricmp( m_pszElementType, "BACK_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BACK_BUTTON;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
