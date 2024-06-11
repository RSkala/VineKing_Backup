//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentPauseScreenElement.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentPauseScreen.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentPauseScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPauseScreenElement::CComponentPauseScreenElement()
	: m_pOwner( NULL )
	//, m_pTextOwner( NULL )
	, m_pszElementType( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
	, m_bAllowPressedEvent( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentPauseScreenElement::~CComponentPauseScreenElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentPauseScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentPauseScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "ElementType", TypeString, Offset( m_pszElementType, CComponentPauseScreenElement ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentPauseScreenElement::onComponentAdd( SimComponent* _pTarget )
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

void CComponentPauseScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( CComponentInGameTutorialObject::IsTutorialActive() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsPauseAllowed() == false )
		return;
	
	//printf( "m_pOwner->getName():  %s;  Position(%f, %f)\n", m_pOwner->getName(), m_pOwner->getPosition().mX, m_pOwner->getPosition().mY );
	
	t2dVector vPosition;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_PAUSE_BUTTON:
			
			// Only register the Pause Button as pressed if it is the First Touched object (the first object the player touched after not touching the screen)
			if( CComponentGlobals::GetInstance().IsTouchingDown() == false && CComponentGlobals::GetInstance().GetFirstTouchDownObject() == m_pOwner )
			{
				//CComponentLineDrawAttack::OnInvalidLineDrawAreaTouched();
				
				m_pOwner->setBlendColour( g_PressedColor );
				//CComponentGlobals::GetInstance().SetTouchedObject( m_pOwner );
				
				m_bAllowPressedEvent = true;
			}
			break;
			
		case ELEMENT_TYPE_RESUME_BUTTON:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideResumeButtonPressedImage();
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON:
			break;
			
		case ELEMENT_TYPE_QUIT_BUTTON:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideQuitButtonPressedImage();
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideConfirmQuitYesPressedImage();
			break;	
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideConfirmQuitNoPressedImage();
			break;
			
		//////////////////////////////////////////////////////////////////////////
		case ELEMENT_TYPE_RESUME_BUTTON_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_MAP_BUTTON_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
		//////////////////////////////////////////////////////////////////////////
			
		default:
			break;
	}
}
	
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{	
	if( m_pOwner == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( CComponentInGameTutorialObject::IsTutorialActive() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsPauseAllowed() == false )
		return;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_PAUSE_BUTTON:
			
			// Only register the Pause Button as pressed if it is the First Touched object (the first object the player touched after not touching the screen)
			if( m_bAllowPressedEvent == true )
			{
				m_pOwner->setBlendColour( g_UnpressedColor );
				CComponentPauseScreen::GetInstance().OnPauseButtonPressed();
				
				CSoundManager::GetInstance().PlayButtonClickForward();
				
				m_bAllowPressedEvent = false;
			}
			break;
			
		case ELEMENT_TYPE_RESUME_BUTTON:
			CComponentPauseScreen::GetInstance().HideResumeButtonPressedImage();
			CComponentPauseScreen::GetInstance().OnResumeButtonPressed();
			
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON:
			//CComponentPauseScreen::GetInstance().HideButtonPressedImage();
			CComponentPauseScreen::GetInstance().OnMuteButtonPressed();
			
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case ELEMENT_TYPE_QUIT_BUTTON:
			CComponentPauseScreen::GetInstance().HideQuitButtonPressedImage();
			CComponentPauseScreen::GetInstance().OnQuitButtonPressed();
			
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES:
			CComponentPauseScreen::GetInstance().HideConfirmQuitYesPressedImage();
			CComponentPauseScreen::GetInstance().OnConfirmQuitYesButtonPressed();
			
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;	
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO:
			CComponentPauseScreen::GetInstance().HideConfirmQuitNoPressedImage();
			CComponentPauseScreen::GetInstance().OnConfirmQuitNoButtonPressed();
			
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		//////////////////////////////////////////////////////////////////////////
		case ELEMENT_TYPE_RESUME_BUTTON_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CComponentPauseScreen::GetInstance().OnResumePressed();
			CSoundManager::GetInstance().PlayButtonClickBack();
			break;
			
		case ELEMENT_TYPE_MAP_BUTTON_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CComponentPauseScreen::GetInstance().OnMapPressed();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CComponentPauseScreen::GetInstance().OnMutePressed();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CComponentPauseScreen::GetInstance().OnQuitYesPressed();
			CSoundManager::GetInstance().PlayButtonClickBack();
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CComponentPauseScreen::GetInstance().OnQuitNoPressed();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
		//////////////////////////////////////////////////////////////////////////
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{	
	if( m_pOwner == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( CComponentInGameTutorialObject::IsTutorialActive() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsPauseAllowed() == false )
		return;
	
	//printf( "m_pOwner->getName():  %s;  Position(%f, %f)\n", m_pOwner->getName(), m_pOwner->getPosition().mX, m_pOwner->getPosition().mY );
	
	t2dVector vPosition;
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_PAUSE_BUTTON:
			// Do nothing with the Pause Button for the OnMouseEnter event
			//m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_RESUME_BUTTON:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideResumeButtonPressedImage();
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON:
			break;
			
		case ELEMENT_TYPE_QUIT_BUTTON:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideQuitButtonPressedImage();
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideConfirmQuitYesPressedImage();
			break;	
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO:
			vPosition = m_pOwner->getPosition();
			CComponentPauseScreen::GetInstance().UnhideConfirmQuitNoPressedImage();
			break;
			
		//////////////////////////////////////////////////////////////////////////
		case ELEMENT_TYPE_RESUME_BUTTON_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_MAP_BUTTON_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
		//////////////////////////////////////////////////////////////////////////
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{	
	if( m_pOwner == NULL )
		return;
	
	if( CComponentInGameTutorialObject::IsTutorialActive() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsPauseAllowed() == false )
		return;
	
	m_bAllowPressedEvent = false;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_PAUSE_BUTTON:
			m_pOwner->setBlendColour( g_UnpressedColor );
			
			//if( CComponentGlobals::GetInstance().GetFirstTouchDownObject() == m_pOwner )
			//	CComponentGlobals::GetInstance().ClearFirstTouchDownObject();
			
			break;
			
		case ELEMENT_TYPE_RESUME_BUTTON:
			CComponentPauseScreen::GetInstance().HideResumeButtonPressedImage();
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON:
			break;
			
		case ELEMENT_TYPE_QUIT_BUTTON:
			CComponentPauseScreen::GetInstance().HideQuitButtonPressedImage();
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES:
			CComponentPauseScreen::GetInstance().HideConfirmQuitYesPressedImage();
			break;	
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO:
			CComponentPauseScreen::GetInstance().HideConfirmQuitNoPressedImage();
			break;
		
		//////////////////////////////////////////////////////////////////////////
		case ELEMENT_TYPE_RESUME_BUTTON_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			break;
			
		case ELEMENT_TYPE_MAP_BUTTON_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			break;
			
		case ELEMENT_TYPE_MUTE_BUTTON_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			break;
			
		case ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW:
			m_pOwner->setBlendColour( g_UnpressedColor );
			break;
		//////////////////////////////////////////////////////////////////////////
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentPauseScreenElement::OnPostInit()
{
	ConvertElementTypeStringToEnum();
	
	if( m_pOwner == NULL )
		return;
	
	if( m_eElementType == ELEMENT_TYPE_PAUSE_BUTTON )
	{
		m_pOwner->setVisible( true );
		m_pOwner->setUseMouseEvents( true );
		
		m_pOwner->setLayer( LAYER_HUD_BUTTONS );
		
		CComponentPauseScreen::GetInstance().SetPauseButtonObject( m_pOwner );
		
		t2dSceneGraph* pSceneGraph = m_pOwner->getSceneGraph();
		if( pSceneGraph )
			CComponentPauseScreen::GetInstance().SetSceneGraph( pSceneGraph );
	}
	else if( m_eElementType == ELEMENT_TYPE_PAUSE_MENU_IMAGE )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setPosition( t2dVector::getZero() );
		CComponentPauseScreen::GetInstance().SetPauseMenuImageObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_QUIT_MENU_IMAGE )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setPosition( t2dVector::getZero() );
		CComponentPauseScreen::GetInstance().SetConfirmQuitMenuImageObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_RESUME_BUTTON )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setUseMouseEvents( true );
		CComponentPauseScreen::GetInstance().SetResumeButton( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_MUTE_BUTTON )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setUseMouseEvents( true );
		CComponentPauseScreen::GetInstance().SetMuteButton( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_BUTTON )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setUseMouseEvents( true );
		CComponentPauseScreen::GetInstance().SetQuitButton( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_BUTTON_PRESSED_IMAGE )
	{
		//m_pOwner->setVisible( false );
		//CComponentPauseScreen::GetInstance().SetButtonPressedImageObject( m_pOwner );
		printf( "THIS SHOULD NOT BE USED!\n" );
	}
	else if( m_eElementType == ELEMENT_TYPE_MUTE_ONOFF_IMAGE )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetMuteOnOffImageObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setUseMouseEvents( true );
		CComponentPauseScreen::GetInstance().SetConfirmQuitYesButton( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO )
	{
		m_pOwner->setVisible( false );
		m_pOwner->setUseMouseEvents( true );
		CComponentPauseScreen::GetInstance().SetConfirmQuitNoButton( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_RESUME_PRESSED_IMAGE )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetResumeButtonPressedImageObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_PRESSED_IMAGE )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetQuitButtonPressedImageObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_YES_PRESSED_IMAGE )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetConfirmQuitYesButtonPressedImageObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_NO_PRESSED_IMAGE )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetConfirmQuitNoButtonPressedImageObject( m_pOwner );
	}
	
	// NEW
	else if( m_eElementType == ELEMENT_TYPE_RESUME_TEXT )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetResumeTextObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_MUTE_TEXT )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetMuteTextObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_TEXT )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetQuitTextObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_CONFIRM_TEXT_1 )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetQuitConfirmText1Object( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_CONFIRM_TEXT_2 )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetQuitConfirmText2Object( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_CONFIRM_YES_TEXT )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetQuitConfirmYesTextObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_QUIT_CONFIRM_NO_TEXT )
	{
		m_pOwner->setVisible( false );
		CComponentPauseScreen::GetInstance().SetQuitConfirmNoTextObject( m_pOwner );
	}
	
	
	// NEW SHIT 2011/01/02
	else if( m_eElementType == ELEMENT_TYPE_RESUME_BUTTON_NEW )
	{
		m_pOwner->setUseMouseEvents( true );
		m_pOwner->setVisible( false );
		m_pOwner->setLayer( LAYER_PAUSE_SCREEN_ELEMENTS );
		CComponentPauseScreen::GetInstance().SetResumeObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_MAP_BUTTON_NEW )
	{
		m_pOwner->setUseMouseEvents( true );
		m_pOwner->setVisible( false );
		m_pOwner->setLayer( LAYER_PAUSE_SCREEN_ELEMENTS );
		CComponentPauseScreen::GetInstance().SetMapObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_MUTE_BUTTON_NEW )
	{
		m_pOwner->setUseMouseEvents( true );
		m_pOwner->setVisible( false );
		m_pOwner->setLayer( LAYER_PAUSE_SCREEN_ELEMENTS );
		CComponentPauseScreen::GetInstance().SetMuteObject( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_QUIT_BG_NEW )
	{
		m_pOwner->setPosition( t2dVector::getZero() );
		m_pOwner->setVisible( false );
		m_pOwner->setLayer( LAYER_PAUSE_SCREENS );
		CComponentPauseScreen::GetInstance().SetQuitBG( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW )
	{
		m_pOwner->setUseMouseEvents( true );
		m_pOwner->setVisible( false );
		m_pOwner->setLayer( LAYER_PAUSE_SCREEN_ELEMENTS );
		CComponentPauseScreen::GetInstance().SetQuitYes( m_pOwner );
	}
	else if( m_eElementType == ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW )
	{
		m_pOwner->setUseMouseEvents( true );
		m_pOwner->setVisible( false );
		m_pOwner->setLayer( LAYER_PAUSE_SCREEN_ELEMENTS );
		CComponentPauseScreen::GetInstance().SetQuitNo( m_pOwner );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentPauseScreenElement::ConvertElementTypeStringToEnum()
{
	if( m_pszElementType == NULL )
		return;
	
	//%pauseScreenElementTypes = "PAUSE_MENU_IMAGE" TAB "CONFIRM_QUIT_MENU_IMAGE" TAB "PAUSE_BUTTON" TAB "RESUME_BUTTON" TAB "MUTE_BUTTON" TAB "QUIT_BUTTON" 
	//								TAB "BUTTON_PRESSED_IMAGE" TAB "MUTE_ONOFF_IMAGE" TAB "CONFIRM_QUIT_BUTTON_YES" TAB "CONFIRM_QUIT_BUTTON_NO"
	//								TAB "RESUME_PRESSED_IMAGE" TAB "QUIT_PRESSED_IMAGE" TAB "CONFIRM_YES_PRESSED_IMAGE" TAB "CONFIRM_NO_PRESSED_IMAGE"
	//								TAB "RESUME_TEXT" TAB "MUTE_TEXT" TAB "QUIT_TEXT" TAB "QUIT_CONFIRM_TEXT_1" TAB "QUIT_CONFIRM_TEXT_2"
	//								TAB "CONFIRM_YES_TEXT" TAB "QUIT_CONFIRM_NO_TEXT";
	
	if( strcmp( m_pszElementType, "PAUSE_MENU_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_PAUSE_MENU_IMAGE;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_QUIT_MENU_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_QUIT_MENU_IMAGE;
	}
	else if( strcmp( m_pszElementType, "PAUSE_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_PAUSE_BUTTON;
	}
	else if( strcmp( m_pszElementType, "RESUME_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_RESUME_BUTTON;
	}
	else if( strcmp( m_pszElementType, "MUTE_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MUTE_BUTTON;
	}
	else if( strcmp( m_pszElementType, "QUIT_BUTTON" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_BUTTON;
	}
	else if( strcmp( m_pszElementType, "BUTTON_PRESSED_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_PRESSED_IMAGE;
	}
	else if( strcmp( m_pszElementType, "MUTE_ONOFF_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MUTE_ONOFF_IMAGE;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_QUIT_BUTTON_YES" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_QUIT_BUTTON_NO" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO;
	}
	else if( strcmp( m_pszElementType, "RESUME_PRESSED_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_RESUME_PRESSED_IMAGE;
	}
	else if( strcmp( m_pszElementType, "QUIT_PRESSED_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_PRESSED_IMAGE;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_YES_PRESSED_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_YES_PRESSED_IMAGE;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_NO_PRESSED_IMAGE" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_NO_PRESSED_IMAGE;
	}
	
	// NEW
	else if( strcmp( m_pszElementType, "RESUME_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_RESUME_TEXT;
	}
	else if( strcmp( m_pszElementType, "MUTE_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MUTE_TEXT;
	}
	else if( strcmp( m_pszElementType, "QUIT_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_TEXT;
	}
	else if( strcmp( m_pszElementType, "QUIT_CONFIRM_TEXT_1" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_CONFIRM_TEXT_1;
	}
	else if( strcmp( m_pszElementType, "QUIT_CONFIRM_TEXT_2" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_CONFIRM_TEXT_2;
	}
	else if( strcmp( m_pszElementType, "QUIT_CONFIRM_YES_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_CONFIRM_YES_TEXT;
	}
	else if( strcmp( m_pszElementType, "QUIT_CONFIRM_NO_TEXT" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_QUIT_CONFIRM_NO_TEXT;
	}
	
	// NEW SHIT 2010/01/02
	else if( strcmp( m_pszElementType, "RESUME_BUTTON_NEW" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_RESUME_BUTTON_NEW;
	}
	else if( strcmp( m_pszElementType, "MAP_BUTTON_NEW" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MAP_BUTTON_NEW;
	}
	else if( strcmp( m_pszElementType, "MUTE_BUTTON_NEW" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_MUTE_BUTTON_NEW;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_QUIT_BG_NEW" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_QUIT_BG_NEW;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_QUIT_BUTTON_YES_NEW" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_YES_NEW;
	}
	else if( strcmp( m_pszElementType, "CONFIRM_QUIT_BUTTON_NO_NEW" ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_CONFIRM_QUIT_BUTTON_NO_NEW;
	}
	//	"RESUME_BUTTON_NEW"				TAB
	//	"MAP_BUTTON_NEW"				TAB
	//	"MUTE_BUTTON_NEW"				TAB
	//	"CONFIRM_QUIT_BG_NEW"			TAB
	//	"CONFIRM_QUIT_BUTTON_YES_NEW"	TAB
	//	"CONFIRM_QUIT_BUTTON_NO_NEW";
}
