//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLevelUpScreenElement.h"

#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentLevelUpScreen.h"

#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static const ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static const t2dVector g_vRedButtonPosition( -97.0f, 45.0f );
static const t2dVector g_vGreenButtonPosition( 57.0f, 143.0f );
static const t2dVector g_vBlueButtonPosition( 114.0f, 23.0f );

// BG
static const char g_szRPG_Screen_BG[32] = "RPG_Screen_BG";
static const char g_szRPG_Background[32] = "RPG_Background";

// Buy button
static const char g_szRPG_Buy_Button[32] = "RPG_Buy_Button";

// Buy button image map stuff
//static const char g_szRPGBuyButtonImageMap[32] = "rpg_buy_buttonImageMap";
//static const S32 g_iPurpleBuyButtonFrame	= 0;
//static const S32 g_iBlueBuyButtonFrame		= 1;
//static const S32 g_iGrayBuyButtonFrame		= 2;
//static const S32 g_iGreenBuyButtonFrame		= 3;

// Purchase selection buttons
static const char g_szRPG_Green_Button[32]	= "RPG_Green_Button";
static const char g_szRPG_Blue_Button[32]	= "RPG_Blue_Button";
static const char g_szRPG_Purple_Button[32] = "RPG_Purple_Button";

// Total XP Text
 static const char g_szTotalXPText[32] = "TOTAL_XP_Text";

// Back Button
static const char g_szRPG_BackButton[32] = "RPG_BackButton";

// Help Button
static const char g_szRPG_HelpButton[32] = "RPG_Help_Button";

static const char g_szPurchaseSelectionImageMap[32] = "rpg_power_buttonImageMap";
static const char g_iPurchaseSelectionFrameGreen	= 0;
static const char g_iPurchaseSelectionFrameBlue		= 1;
static const char g_iPurchaseSelectionFramePurple	= 2;
static const char g_iPurchaseSelectionFrameBlank	= 3;

// Green markers
//static const char g_szRPG_Green_Button_Level_1[32] = "RPG_Green_Button_Level_1";
//static const char g_szRPG_Green_Button_Level_2[32] = "RPG_Green_Button_Level_2";
//static const char g_szRPG_Green_Button_Level_3[32] = "RPG_Green_Button_Level_3";
//static const char g_szRPG_Green_Button_Level_4[32] = "RPG_Green_Button_Level_4";
//static const char g_szRPG_Green_Button_Level_5[32] = "RPG_Green_Button_Level_5";
//static const char g_szRPG_Green_Button_Level_6[32] = "RPG_Green_Button_Level_6";
//
//// Blue markers
//static const char g_szRPG_Blue_Button_Level_1[32] = "RPG_Blue_Button_Level_1";
//static const char g_szRPG_Blue_Button_Level_2[32] = "RPG_Blue_Button_Level_2";
//static const char g_szRPG_Blue_Button_Level_3[32] = "RPG_Blue_Button_Level_3";
//static const char g_szRPG_Blue_Button_Level_4[32] = "RPG_Blue_Button_Level_4";
//static const char g_szRPG_Blue_Button_Level_5[32] = "RPG_Blue_Button_Level_5";
//static const char g_szRPG_Blue_Button_Level_6[32] = "RPG_Blue_Button_Level_6";
//
//// Purple markers
//static const char g_szRPG_Purple_Button_Level_1[32] = "RPG_Purple_Button_Level_1";
//static const char g_szRPG_Purple_Button_Level_2[32] = "RPG_Purple_Button_Level_2";
//static const char g_szRPG_Purple_Button_Level_3[32] = "RPG_Purple_Button_Level_3";
//static const char g_szRPG_Purple_Button_Level_4[32] = "RPG_Purple_Button_Level_4";
//static const char g_szRPG_Purple_Button_Level_5[32] = "RPG_Purple_Button_Level_5";
//static const char g_szRPG_Purple_Button_Level_6[32] = "RPG_Purple_Button_Level_6";

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLevelUpScreenElement::sm_bPressed = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLevelUpScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelUpScreenElement::CComponentLevelUpScreenElement()
	: m_pOwner( NULL )
	, m_pszElementType( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
	, m_bPressed( false )
{
	CComponentLevelUpScreenElement::sm_bPressed = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelUpScreenElement::~CComponentLevelUpScreenElement()
{
	CComponentLevelUpScreenElement::sm_bPressed = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::EnableElement()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setVisible( true );
	
//	if( m_eElementType == ELEMENT_TYPE_TOTALXP_TEXT )
//	{
//		t2dTextObject* pTextObject = dynamic_cast<t2dTextObject*>( m_pOwner );
//		if( pTextObject )
//		{
//			S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
//			char szText[32];
//			sprintf( szText, "%d", iTotalXP );
//			pTextObject->setText( szText );
//		}
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::DisableElement()
{
	if( m_pOwner == NULL )
		return;
	
	//m_pOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLevelUpScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentLevelUpScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	//addField( "ElementType", TypeString, Offset( m_pszElementType, CComponentLevelUpScreenElement ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLevelUpScreenElement::onComponentAdd( SimComponent* _pTarget )
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

void CComponentLevelUpScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( m_bPressed )
		return;
	
	m_bPressed = true;
	
	t2dStaticSprite* pSprite;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_GREEN_BUTTON:
			pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameGreen );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_PressedColor );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentLevelUpScreen::GetInstance().OnGreenButtonPressed();
			break;
			
		case ELEMENT_TYPE_BLUE_BUTTON:
			pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlue );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_PressedColor );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentLevelUpScreen::GetInstance().OnBlueButtonPressed();
			break;
			
		case ELEMENT_TYPE_PURPLE_BUTTON:
			pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFramePurple );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_PressedColor );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentLevelUpScreen::GetInstance().OnPurpleButtonPressed();
			break;
			
		default:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
	}

	
	//m_pOwner->setBlendColour( g_PressedColor );
	
//	if( m_eElementType == ELEMENT_TYPE_BACK_BUTTON )
//	{
//		m_pOwner->setBlendColour( g_PressedColor );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_HEALTH_BUTTON )
//	{
//		if( CComponentGlobals::GetInstance().GetTotalXP() > 0 && 
//		   CComponentGlobals::GetInstance().GetTotalHealthXP() < CComponentGlobals::GetInstance().GetMaxLevelUpUnits() )
//		{
//			m_pOwner->setBlendColour( g_PressedColor );
//		
//			CComponentLevelUpScreen::GetInstance().OnHealthButtonPressed();
//		}
//	}
//	else if( m_eElementType == ELEMENT_TYPE_SPEED_BUTTON )
//	{
//		if( CComponentGlobals::GetInstance().GetTotalXP() > 0 && 
//		   CComponentGlobals::GetInstance().GetTotalSpeedXP() < CComponentGlobals::GetInstance().GetMaxLevelUpUnits() )
//		{
//			m_pOwner->setBlendColour( g_PressedColor );
//		
//			CComponentLevelUpScreen::GetInstance().OnSpeedButtonPressed();
//		}
//	}
//	else if( m_eElementType == ELEMENT_TYPE_MANA_BUTTON )
//	{
//		if( CComponentGlobals::GetInstance().GetTotalXP() > 0 && 
//		   CComponentGlobals::GetInstance().GetTotalManaXP() < CComponentGlobals::GetInstance().GetMaxLevelUpUnits() )
//		{
//			m_pOwner->setBlendColour( g_PressedColor );
//		
//			CComponentLevelUpScreen::GetInstance().OnManaButtonPressed();
//		}
//	}
//	
//	//printf( "OnMouseDown: %s\n", m_pOwner ? m_pOwner->getName() : "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	//printf( "OnMouseUp: %s\n", m_pOwner ? m_pOwner->getName() : "" );
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	m_bPressed = false;
	
	//m_pOwner->setBlendColour( g_UnpressedColor );
	
	//t2dStaticSprite* pSprite;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_BACK_BUTTON:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CSoundManager::GetInstance().PlayButtonClickBack();
			CComponentLevelUpScreen::GetInstance().OnBackButtonPressed();
			break;
			
		case ELEMENT_TYPE_HELP_BUTTON:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentLevelUpScreen::GetInstance().OnHelpButtonPressed();
			break;
			
		case ELEMENT_TYPE_GREEN_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameGreen );
			//pSprite->setBlendColour( g_UnpressedColor );
			//CSoundManager::GetInstance().PlayButtonClickForward();
			//CComponentLevelUpScreen::GetInstance().OnGreenButtonPressed();
			break;
		
		case ELEMENT_TYPE_BLUE_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlue );
			//pSprite->setBlendColour( g_UnpressedColor );
			//CSoundManager::GetInstance().PlayButtonClickForward();
			//CComponentLevelUpScreen::GetInstance().OnBlueButtonPressed();
			break;
			
		case ELEMENT_TYPE_PURPLE_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFramePurple );
			//pSprite->setBlendColour( g_UnpressedColor );
			//CSoundManager::GetInstance().PlayButtonClickForward();
			//CComponentLevelUpScreen::GetInstance().OnPurpleButtonPressed();
			break;
			
		case ELEMENT_TYPE_BUY_BUTTON:
			m_pOwner->setBlendColour( g_UnpressedColor );
			CSoundManager::GetInstance().PlayButtonClickForward();
			CComponentLevelUpScreen::GetInstance().OnBuyButtonPressed();
			break;
			
		default:
			printf( "Unhandled Level Up Screen Element: %s\n", m_pOwner->getName() );
			break;
	}


	
//	if( m_eElementType == ELEMENT_TYPE_BACK_BUTTON )
//	{
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		OnBackButtonPressed();
//		
//		CSoundManager::GetInstance().PlayButtonClickBack();
//	}
//	else if( m_eElementType == ELEMENT_TYPE_HEALTH_BUTTON )
//	{
//		// Stop moving the object upwards
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		
//		CComponentLevelUpScreen::GetInstance().OnHealthButtonReleased();
//	}
//	else if( m_eElementType == ELEMENT_TYPE_SPEED_BUTTON )
//	{
//		// Stop moving the object upwards
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		
//		CComponentLevelUpScreen::GetInstance().OnSpeedButtonReleased();
//	}
//	else if( m_eElementType == ELEMENT_TYPE_MANA_BUTTON )
//	{
//		// Stop moving the object upwards
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		
//		CComponentLevelUpScreen::GetInstance().OnManaButtonReleased();
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	//printf( "OnMouseEnter: %s\n", m_pOwner ? m_pOwner->getName() : "" );
	if( m_pOwner == NULL )
		return;
	
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	//t2dStaticSprite* pSprite;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_GREEN_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameGreen );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_PressedColor );
			break;

		case ELEMENT_TYPE_BLUE_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlue );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_PressedColor );
			break;

		case ELEMENT_TYPE_PURPLE_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFramePurple );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_PressedColor );
			break;
			
		default:
			m_pOwner->setBlendColour( g_PressedColor );
			break;
	}
	 
	
	//m_pOwner->setBlendColour( g_PressedColor );
	
	//if( m_eElementType == ELEMENT_TYPE_BACK_BUTTON )
	//{
	//	m_pOwner->setBlendColour( g_PressedColor );
	//}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	//printf( "OnMouseLeave: %s\n", m_pOwner ? m_pOwner->getName() : "" );
	if( m_pOwner == NULL )
		return;
	
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	m_bPressed = false;
	
	//t2dStaticSprite* pSprite;
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_GREEN_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_UnpressedColor );
			break;
			
		case ELEMENT_TYPE_BLUE_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_UnpressedColor );
			break;
			
		case ELEMENT_TYPE_PURPLE_BUTTON:
			//pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
			//pSprite->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
			//pSprite->setBlendColour( g_UnpressedColor );
			break;
			
		default:
			m_pOwner->setBlendColour( g_UnpressedColor );
			break;
	}
	
	//m_pOwner->setBlendColour( g_UnpressedColor );
	
//	if( m_eElementType == ELEMENT_TYPE_BACK_BUTTON )
//	{
//		m_pOwner->setBlendColour( g_UnpressedColor );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_HEALTH_BUTTON )
//	{
//		// Stop moving the object upwards
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		
//		CComponentLevelUpScreen::GetInstance().OnHealthButtonReleased();
//	}
//	else if( m_eElementType == ELEMENT_TYPE_SPEED_BUTTON )
//	{
//		// Stop moving the object upwards
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		
//		CComponentLevelUpScreen::GetInstance().OnSpeedButtonReleased();
//	}
//	else if( m_eElementType == ELEMENT_TYPE_MANA_BUTTON )
//	{
//		// Stop moving the object upwards
//		m_pOwner->setBlendColour( g_UnpressedColor );
//		
//		CComponentLevelUpScreen::GetInstance().OnManaButtonReleased();
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
		
	const char* pszName = m_pOwner->getName();
	
	if( dStricmp( pszName, g_szRPG_Buy_Button ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUY_BUTTON;
		m_pOwner->setUseMouseEvents( true );
	}
	else if( dStricmp( pszName, g_szRPG_Green_Button ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_GREEN_BUTTON;
		m_pOwner->setUseMouseEvents( true );
		
		//m_pOwner->setVisible( false );
	}
	else if( dStricmp( pszName, g_szRPG_Blue_Button ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BLUE_BUTTON;
		m_pOwner->setUseMouseEvents( true );
		
		//m_pOwner->setVisible( false );
	}
	else if( dStricmp( pszName, g_szRPG_Purple_Button ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_PURPLE_BUTTON;
		m_pOwner->setUseMouseEvents( true );
		
		//m_pOwner->setVisible( false );
	}
	else if( dStricmp( pszName, g_szTotalXPText ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_TOTAL_XP_TEXT;
		
		//t2dTextObject* pTextObject = dynamic_cast<t2dTextObject*>( m_pOwner );
		//if( pTextObject )
		//{
			//pTextObject->setAutoSize( true );
			//pTextObject->addFontSize( 32 );
			//pTextObject->setFont( "Lambada LET" );
			//pTextObject->setLineHeight( 32.0f );
			//pTextObject->setTextAlign( t2dTextObject::CENTER );
			
			////pTextObject->setFont( "Courier" );
			//pTextObject->setFont( "Lambada LET" );
			//pTextObject->addFontSize( 32 );
			//pTextObject->setLineHeight( 32.0f );
			//pTextObject->setTextAlign( t2dTextObject::CENTER );
			////CComponentLevelUpScreen::GetInstance().SetTotalXPTextObject( pTextObject );
		//}
	}
	else if( dStricmp( pszName, g_szRPG_BackButton ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BACK_BUTTON;
		m_pOwner->setUseMouseEvents( true );
	}
	else if( dStricmp( pszName, g_szRPG_HelpButton ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_HELP_BUTTON;
		m_pOwner->setUseMouseEvents( true );
	}
	
	//ConvertElementTypeStringToEnum();
	
//	if( m_pOwner == NULL )
//	{
//		printf( "CComponentLevelUpScreenElement::OnPostInit - Owner is NULL!\n" );
//		return;
//	}
	
//	//printf( "CComponentLevelUpScreenElement::OnPostInit - %s\n", m_pOwner->getName() );
//	
//	if( m_eElementType == ELEMENT_TYPE_BACK_BUTTON )
//	{
//		m_pOwner->setUseMouseEvents( true );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_BACKGROUND )
//	{
//		m_pOwner->setPosition( t2dVector::getZero() );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_HEALTH )
//	{
//		CComponentLevelUpScreen::GetInstance().SetHealthBarObject( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_MANA )
//	{
//		CComponentLevelUpScreen::GetInstance().SetManaBarObject( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_SPEED )
//	{
//		CComponentLevelUpScreen::GetInstance().SetSpeedBarObject( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_TOTALXP_BG )
//	{
//		
//	}
//	else if( m_eElementType == ELEMENT_TYPE_TOTALXP_TEXT )
//	{
//		t2dTextObject* pTextObject = dynamic_cast<t2dTextObject*>( m_pOwner );
//		if( pTextObject )
//		{
//			pTextObject->setFont( "Courier" );
//			pTextObject->addFontSize( 32 );
//			pTextObject->setLineHeight( 32.0f );
//			pTextObject->setTextAlign( t2dTextObject::RIGHT );
//			
//			CComponentLevelUpScreen::GetInstance().SetTotalXPTextObject( pTextObject );
//		}
//	}
//	else if( m_eElementType == ELEMENT_TYPE_HEALTH_BUTTON )
//	{
//		m_pOwner->setUseMouseEvents( true );
//		m_pOwner->setPosition( g_vRedButtonPosition );
//		CComponentLevelUpScreen::GetInstance().SetRedButtonObject( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_MANA_BUTTON )
//	{
//		m_pOwner->setUseMouseEvents( true );
//		m_pOwner->setPosition( g_vBlueButtonPosition );
//		CComponentLevelUpScreen::GetInstance().SetBlueButtonObject( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_SPEED_BUTTON )
//	{
//		m_pOwner->setUseMouseEvents( true );
//		m_pOwner->setPosition( g_vGreenButtonPosition );
//		CComponentLevelUpScreen::GetInstance().SetGreenButtonObject( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_HEALTH_BAR_POSITION_MARKER )
//	{
//		//CComponentLevelUpScreen::GetInstance().SetHealthBarInitialPosition( m_pOwner->getPosition() );
//		CComponentLevelUpScreen::GetInstance().SetHealthBarPositionMarker( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_SPEED_BAR_POSITION_MARKER )
//	{
//		//CComponentLevelUpScreen::GetInstance().SetSpeedBarInitialPosition( m_pOwner->getPosition() );
//		CComponentLevelUpScreen::GetInstance().SetSpeedBarPositionMarker( m_pOwner );
//	}
//	else if( m_eElementType == ELEMENT_TYPE_MANA_BAR_POSITION_MARKER )
//	{
//		//CComponentLevelUpScreen::GetInstance().SetManaBarInitialPosition( m_pOwner->getPosition() );
//		CComponentLevelUpScreen::GetInstance().SetManaBarPositionMarker( m_pOwner );
//	}
	
	CComponentLevelUpScreen::GetInstance().AddElement( this );
	
	// Make sure it starts out disabled
	DisableElement();
	
	CComponentLevelUpScreenElement::sm_bPressed = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::ConvertElementTypeStringToEnum()
{
	if( m_pszElementType == NULL )
		return;
	
	//%levelUpScreenElementTypes = "BACK_BUTTON" TAB "BACKGROUND" TAB "HEALTH_BAR" TAB "MANA_BAR" TAB "SPEED_BAR" TAB "TOTAL_XP_BG" TAB "TOTAL_XP_TEXT" TAB "HEALTH_BUTTON" TAB
	//"MANA_BUTTON" TAB "SPEED_BUTTON" TAB "HEALTH_BAR_POSITION_MARKER" TAB "SPEED_BAR_POSITION_MARKER" TAB "MANA_BAR_POSITION_MARKER";
	
//	if( dStricmp( m_pszElementType, "BACK_BUTTON" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_BACK_BUTTON;
//	}
//	else if( dStricmp( m_pszElementType, "BACKGROUND" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_BACKGROUND;
//	}
//	else if( dStricmp( m_pszElementType, "HEALTH_BAR" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_HEALTH;
//	}
//	else if( dStricmp( m_pszElementType, "MANA_BAR" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_MANA;
//	}
//	else if( dStricmp( m_pszElementType, "SPEED_BAR" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_SPEED;
//	}
//	else if( dStricmp( m_pszElementType, "TOTAL_XP_BG" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_TOTALXP_BG;
//	}
//	else if( dStricmp( m_pszElementType, "TOTAL_XP_TEXT" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_TOTALXP_TEXT;
//	}
//	else if( dStricmp( m_pszElementType, "HEALTH_BUTTON" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_HEALTH_BUTTON;
//	}
//	else if( dStricmp( m_pszElementType, "MANA_BUTTON" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_MANA_BUTTON;
//	}
//	else if( dStricmp( m_pszElementType, "SPEED_BUTTON" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_SPEED_BUTTON;
//	}
//	else if( dStricmp( m_pszElementType, "HEALTH_BAR_POSITION_MARKER" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_HEALTH_BAR_POSITION_MARKER;
//	}
//	else if( dStricmp( m_pszElementType, "SPEED_BAR_POSITION_MARKER" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_SPEED_BAR_POSITION_MARKER;
//	}
//	else if( dStricmp( m_pszElementType, "MANA_BAR_POSITION_MARKER" ) == 0 )
//	{
//		m_eElementType = ELEMENT_TYPE_MANA_BAR_POSITION_MARKER;
//	}
//	
//	if( m_eElementType == ELEMENT_TYPE_NONE )
//	{
//		printf( "%s: %s - Level Up Screen Element has no type.\n", __FUNCTION__, m_pOwner ? m_pOwner->getName() : "" );
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnBackButtonPressed()
{
	//CComponentLevelUpScreen::GetInstance().CloseLevelUpScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnHealthButtonPressed()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnSpeedButtonPressed()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreenElement::OnManaButtonPressed()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------