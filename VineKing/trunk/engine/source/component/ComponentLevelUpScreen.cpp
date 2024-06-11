//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLevelUpScreen.h"

#include "ComponentAchievementBanner.h"
#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentLevelUpScreenElement.h"
#include "ComponentMapSelectMenu.h"

#include "SoundManager.h"

#include "core/ITickable.h"
#include "platformiPhone/GameCenterWrapper.h"
#include "math/mMathFn.h"
#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dSceneWindow.h"
#include "T2D/t2dStaticSprite.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 2000.0f, 2000.0f );
static char g_szStringBuffer[32];

static const F32 g_fXPDrainSpeedBase = 100.0f;
static F32 g_fXPDrainSpeed	= 200.0f; //g_fXPDrainSpeedBase;
static F32 g_fDrainButtonHeldTimer = 0.0f; // The timer to see how long the user has been holding down the XP drain buttons. This is used to increase the drain speed.

static const char g_szRedButtonEffectName[32]	= "PowerUpScreen_RedButtonEffect";
static const char g_szGreenButtonEffectName[32] = "PowerUpScreen_GreenButtonEffect";
static const char g_szBlueButtonEffectName[32]	= "PowerUpScreen_BlueButtonEffect";

static const t2dVector g_vRedEffectPosition( -97, -49 );
static const t2dVector g_vGreenEffectPosition( 57, 49 );
static const t2dVector g_vBlueEffectPosition( 114, -71 );

static const char g_szPillarOfLightRed[32]		= "PillarOfLight_Red";
static const char g_szPillarOfLightGreen[32]	= "PillarOfLight_Green";
static const char g_szPillarOfLightBlue[32]		= "PillarOfLight_Blue";

//static const char g_szRPGVineKing[32]			= "RPG_VineKing";
//static const char g_szRPGVineKingChantAnim[32]	= "RPG_VK_CHANT";
//static const char g_szRPGVineKingIdleAnim[32]	= "RPG_VK_IDLE";

//static const char g_szVineling01[32] = "Vineling01";
//static const char g_szVineling02[32] = "Vineling02";
//static const char g_szVineling03[32] = "Vineling03";

// Tutorial stuff
static const char g_szTutorialTop[32]		= "Tutorial_TOP";
static const char g_szTutorialBottom[32]	= "Tutorial_BOTTOM";
static const char g_szTutorialNext[32]		= "Tutorial_NEXT";

static t2dStaticSprite* g_pTutorialTop		= NULL;
static t2dStaticSprite* g_pTutorialBottom	= NULL;
static t2dStaticSprite* g_pTutorialNext		= NULL;

static const char g_szPowerUpScreenTutorialImageMapTop[64]		= "intro_throne_room_picImageMap";
static const char g_szPowerUpScreenTutorialImageMapBottom01[64]	= "intro_throne_room_text_001ImageMap";
//static const char g_szPowerUpScreenTutorialImageMapBottom02[64]	= "intro_throne_room_text_002ImageMap";
//static const char g_szPowerUpScreenTutorialImageMapBottom03[64]	= "intro_throne_room_text_003ImageMap";
//static const char g_szPowerUpScreenTutorialImageMapBottom04[64]	= "intro_throne_room_text_004ImageMap";

//	SAVE_DATA_INDEX_TUTORIAL_SHOWN_POWER_UP,		// Displayed in Power Up Screen



// BG
static const char g_szRPG_Screen_BG[32] = "RPG_Screen_BG";
static const char g_szRPG_Background[32] = "RPG_Background";

// Buy button
static const char g_szRPG_Buy_Button[32] = "RPG_Buy_Button";

// Buy button image map stuff
static const char g_szRPGBuyButtonImageMap[32] = "rpg_buy_buttonImageMap";
static const S32 g_iPurpleBuyButtonFrame	= 0;
static const S32 g_iBlueBuyButtonFrame		= 1;
static const S32 g_iGrayBuyButtonFrame		= 2;
static const S32 g_iGreenBuyButtonFrame		= 3;

// Purchase selection buttons
static const char g_szRPG_Green_Button[32]	= "RPG_Green_Button";
static const char g_szRPG_Blue_Button[32]	= "RPG_Blue_Button";
static const char g_szRPG_Purple_Button[32] = "RPG_Purple_Button";

static const char g_szPurchaseSelectionImageMap[32] = "rpg_power_buttonImageMap";
static const char g_iPurchaseSelectionFrameGreen	= 0;
static const char g_iPurchaseSelectionFrameBlue		= 1;
static const char g_iPurchaseSelectionFramePurple	= 2;
static const char g_iPurchaseSelectionFrameBlank	= 3;

// Green markers
static const char g_szRPG_Green_Button_Level_1[32] = "RPG_Green_Button_Level_1";
static const char g_szRPG_Green_Button_Level_2[32] = "RPG_Green_Button_Level_2";
static const char g_szRPG_Green_Button_Level_3[32] = "RPG_Green_Button_Level_3";
static const char g_szRPG_Green_Button_Level_4[32] = "RPG_Green_Button_Level_4";
static const char g_szRPG_Green_Button_Level_5[32] = "RPG_Green_Button_Level_5";
static const char g_szRPG_Green_Button_Level_6[32] = "RPG_Green_Button_Level_6";

// Blue markers
static const char g_szRPG_Blue_Button_Level_1[32] = "RPG_Blue_Button_Level_1";
static const char g_szRPG_Blue_Button_Level_2[32] = "RPG_Blue_Button_Level_2";
static const char g_szRPG_Blue_Button_Level_3[32] = "RPG_Blue_Button_Level_3";
static const char g_szRPG_Blue_Button_Level_4[32] = "RPG_Blue_Button_Level_4";
static const char g_szRPG_Blue_Button_Level_5[32] = "RPG_Blue_Button_Level_5";
static const char g_szRPG_Blue_Button_Level_6[32] = "RPG_Blue_Button_Level_6";

// Purple markers
static const char g_szRPG_Purple_Button_Level_1[32] = "RPG_Purple_Button_Level_1";
static const char g_szRPG_Purple_Button_Level_2[32] = "RPG_Purple_Button_Level_2";
static const char g_szRPG_Purple_Button_Level_3[32] = "RPG_Purple_Button_Level_3";
static const char g_szRPG_Purple_Button_Level_4[32] = "RPG_Purple_Button_Level_4";
static const char g_szRPG_Purple_Button_Level_5[32] = "RPG_Purple_Button_Level_5";
static const char g_szRPG_Purple_Button_Level_6[32] = "RPG_Purple_Button_Level_6";

// Total XP Text
static const char g_szTotalXPText[32] = "TOTAL_XP_Text";

// Prices
static const char g_szRPG_Price_Green[32]	= "RPG_Price_Green";  
static const char g_szRPG_Price_Blue[32]	= "RPG_Price_Blue";
static const char g_szRPG_Price_Purple[32]	= "RPG_Price_Purple";


// Back Button (Map Button)
static const char g_szRPG_BackButton[32] = "RPG_BackButton";

// Help Button
static const char g_szRPG_Help_Button[32] = "RPG_Help_Button";

// Purchase effect
static const char g_szRPG_BuyAnimObject[32] = "RPG_BuyAnim";	// This is the actual object
static const char g_szRPG_BuyAnimName[32] = "rpg_fxAnimation";	// This is the name of the animation

// Fade Stuff
static const char g_szFadeBox[32] = "FadeBox_RPG";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.25f;

static const char g_szSceneWindow[32] = "sceneWindow2D"; // Scene Window - For setting the camera position
static t2dSceneWindow* g_pSceneWindow = NULL;


static const char g_szPriceImageMap[32] = "rpg_price_listImageMap";
//-------------------------------------------
//		Price		|  Image Index
//-------------------------------------------
//		2000		|	0
//		4000		|	1
//		6000		|	2
//		8000		|	3
//		10000		|	4
//		15000		|	5
//		MAX			|	6
//-------------------------------------------

S32 g_iPrices[] = 
{
	2000,	// 0
	4000,	// 1
	6000,	// 2
	8000,	// 3
	10000,	// 4
	15000,	// 5
	//MAX
};

S32 g_iNumPrices = sizeof(g_iPrices) / sizeof(S32);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelUpScreen* CComponentLevelUpScreen::sm_pInstance = NULL;
t2dSceneObject* CComponentLevelUpScreen::sm_pLastPressedElement = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLevelUpScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelUpScreen::CComponentLevelUpScreen()
	: m_pOwner( NULL )
	, m_pHealthBarObject( NULL )
	, m_pSpeedBarObject( NULL )
	, m_pManaBarObject( NULL )
	, m_pHealthBarPositionMarker( NULL )
	, m_pSpeedBarPositionMarker( NULL )
	, m_pManaBarPositionMarker( NULL )
	, m_vHealthBarInitialPosition( t2dVector::getZero() )
	, m_vSpeedBarInitialPosition( t2dVector::getZero() )
	, m_vManaBarInitialPosition( t2dVector::getZero() )
	, m_vHealthBarMaxPosition( t2dVector::getZero() )
	, m_vSpeedBarMaxPosition( t2dVector::getZero() )
	, m_vManaBarMaxPosition( t2dVector::getZero() )
	, m_fHealthBarYSizeDifference( 0.0f )
	, m_fSpeedBarYSizeDifference( 0.0f )
	, m_fManaBarYSizeDifference( 0.0f )
	, m_bHealthButtonPressed( false )
	, m_bSpeedButtonPressed( false )
	, m_bManaButtonPressed( false )
	, m_pTotalXPText( NULL )
	, m_pRedButton( NULL )
	, m_pGreenButton( NULL )
	, m_pBlueButton( NULL )
	, m_pRedButtonEffect( NULL )
	, m_pGreenButtonEffect( NULL )
	, m_pBlueButtonEffect( NULL )
	, m_pPillarOfLightRed( NULL )
	, m_pPillarOfLightGreen( NULL )
	, m_pPillarOfLightBlue( NULL )
	, m_pRPGVineKing( NULL )
	//, m_pVineling01( NULL )
	//, m_pVineling02( NULL )
	//, m_pVineling03( NULL )

	, m_pRPGScreenBG( NULL )
	, m_pRPGBackground( NULL )

	, m_pRPGBuyButton( NULL )

	, m_pRPGGreenButton( NULL )
	, m_pRPGBlueButton( NULL )
	, m_pRPGPurpleButton( NULL )

	, m_pGreenMarker01( NULL )
	, m_pGreenMarker02( NULL )
	, m_pGreenMarker03( NULL )
	, m_pGreenMarker04( NULL )
	, m_pGreenMarker05( NULL )
	, m_pGreenMarker06( NULL )

	, m_pBlueMarker01( NULL )
	, m_pBlueMarker02( NULL )
	, m_pBlueMarker03( NULL )
	, m_pBlueMarker04( NULL )
	, m_pBlueMarker05( NULL )
	, m_pBlueMarker06( NULL )

	, m_pPurpleMarker01( NULL )
	, m_pPurpleMarker02( NULL )
	, m_pPurpleMarker03( NULL )
	, m_pPurpleMarker04( NULL )
	, m_pPurpleMarker05( NULL )
	, m_pPurpleMarker06( NULL )

	, m_pRPGMapButton( NULL )
	, m_pRPGHelpButton( NULL )

	, m_pPriceGreen( NULL )
	, m_pPriceBlue( NULL )
	, m_pPricePurple( NULL )

	, m_pPurchaseEffect( NULL )

	, m_eBuyButtonState( BUY_BUTTON_STATE_NONE )

	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentLevelUpScreen::sm_pInstance = NULL;
	CComponentLevelUpScreen::sm_pLastPressedElement = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelUpScreen::~CComponentLevelUpScreen()
{
	CComponentLevelUpScreen::sm_pInstance = NULL;
	CComponentLevelUpScreen::sm_pLastPressedElement = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OpenLevelUpScreen()
{
	UpdateTotalXPTextDisplay();
	UpdatePriceDisplay();
	UpdateMarkerDisplay();
	
	if( m_pRPGScreenBG )
	{
		m_pRPGScreenBG->setPosition( t2dVector::getZero() );
		
		if( m_pRPGBackground )
			m_pRPGBackground->setPosition( m_pRPGScreenBG->getPosition() );
	};
	
	if( g_pFadeBox )
	{
		g_pFadeBox->setVisible( true );
		g_pFadeBox->setBlendAlpha( 1.0f );
		
		g_pFadeBox->setPosition( m_pRPGScreenBG ? m_pRPGScreenBG->getPosition() : t2dVector::getZero() );	
	}
	
	if( g_pSceneWindow && m_pRPGScreenBG )
		g_pSceneWindow->mount( m_pRPGScreenBG, t2dVector::getZero(), 0.0f, true );
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds;
	
	if( m_pRPGBuyButton )
	{
		m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
		m_pRPGBuyButton->setUseMouseEvents( false );
	}
	
	if( m_pRPGGreenButton )
	{
		if( CComponentGlobals::GetInstance().GetXPLevelGreen() >= g_iNumPrices )
			m_pRPGGreenButton->setUseMouseEvents( false );
		else
			m_pRPGGreenButton->setUseMouseEvents( true );
		
		m_pRPGGreenButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	}
	
	if( m_pRPGBlueButton )
	{
		if( CComponentGlobals::GetInstance().GetXPLevelBlue() >= g_iNumPrices ) 
			m_pRPGBlueButton->setUseMouseEvents( false );
		else
			m_pRPGBlueButton->setUseMouseEvents( true );
		
		m_pRPGBlueButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	}
	
	if( m_pRPGPurpleButton )
	{
		if( CComponentGlobals::GetInstance().GetXPLevelPurple() >= g_iNumPrices )
			m_pRPGPurpleButton->setUseMouseEvents( false );
		else
			m_pRPGPurpleButton->setUseMouseEvents( true );
		
		m_pRPGPurpleButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	}
	
	if( m_pRPGMapButton )
		m_pRPGMapButton->setUseMouseEvents( true );
	
	if( m_pRPGHelpButton )
		m_pRPGHelpButton->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::CloseLevelUpScreen()
{
	// Start Fade Out
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_fFadeTimer = 0.0f;
	
	if( m_pRPGGreenButton )
		m_pRPGGreenButton->setUseMouseEvents( false );
	
	if( m_pRPGBlueButton )
		m_pRPGBlueButton->setUseMouseEvents( false );
	
	if( m_pRPGPurpleButton )
		m_pRPGPurpleButton->setUseMouseEvents( false );
	
	if( m_pRPGMapButton )
		m_pRPGMapButton->setUseMouseEvents( false );
	
	if( m_pRPGHelpButton )
		m_pRPGHelpButton->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::AddElement( CComponentLevelUpScreenElement* const _pObject )
{
	if( _pObject == NULL )
		return;
	
	m_ElementList.push_back( _pObject );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::InitializeElements()
{
	if( m_pHealthBarPositionMarker )
	{
		m_vHealthBarInitialPosition = m_pHealthBarPositionMarker->getPosition();
		//printf( "m_vHealthBarInitialPosition = (%f, %f)\n", m_vHealthBarInitialPosition.mX, m_vHealthBarInitialPosition.mY );
	}
	
	if( m_pSpeedBarPositionMarker )
	{
		m_vSpeedBarInitialPosition = m_pSpeedBarPositionMarker->getPosition();\
		//printf( "m_vSpeedBarInitialPosition = (%f, %f)\n", m_vSpeedBarInitialPosition.mX, m_vSpeedBarInitialPosition.mY );
	}
	
	if( m_pManaBarPositionMarker )
	{
		m_vManaBarInitialPosition = m_pManaBarPositionMarker->getPosition();
		//printf( "m_vManaBarInitialPosition = (%f, %f)\n", m_vManaBarInitialPosition.mX, m_vManaBarInitialPosition.mY );
	}
	
	// Set the object positions
	if( m_pHealthBarObject )
	{
		t2dAnimatedSprite* pAnimatedSprite = static_cast<t2dAnimatedSprite*>( m_pHealthBarObject );
		pAnimatedSprite->mAnimationController.PauseAnimation();
		
		// Since objects' positions are their centers, move the initial position down by half its height
		F32 fVerticalSize = m_pHealthBarObject->getSize().mY;
		m_vHealthBarInitialPosition.mY += (fVerticalSize * 0.5f);
		m_pHealthBarObject->setPosition( m_vHealthBarInitialPosition );
		
		// Set the Health Bar's max position (The position it will be when the Health XP is completely filled)
		m_vHealthBarMaxPosition = m_vHealthBarInitialPosition;
		F32 fHeight = m_pHealthBarObject->getSize().mY;
		m_vHealthBarMaxPosition.mY -= fHeight;
		
		//printf( "m_vHealthBarInitialPosition: (%f, %f)\n", m_vHealthBarInitialPosition.mX, m_vHealthBarInitialPosition.mY );
		//printf( "m_vHealthBarMaxPosition:     (%f, %f)\n", m_vHealthBarMaxPosition.mX, m_vHealthBarMaxPosition.mY );
		
		m_fHealthBarYSizeDifference = mFabs( m_vHealthBarMaxPosition.mY - m_vHealthBarInitialPosition.mY );
		//printf( " - Health Y difference: %f\n", m_fHealthBarYSizeDifference );
		
		
		// Move the Health Bar up according to how much XP has been added to the Health XP
		F32 fPercentFilled = ( ((F32)CComponentGlobals::GetInstance().GetTotalHealthXP()) / ((F32)CComponentGlobals::GetInstance().GetMaxLevelUpUnits() ));
		
		F32 fMovementAmount = fPercentFilled * m_fHealthBarYSizeDifference;
		
		t2dVector vNewPosition = m_vHealthBarInitialPosition;
		vNewPosition.mY -= fMovementAmount;
		
		m_pHealthBarObject->setPosition( vNewPosition );
	}
	
	if( m_pSpeedBarObject )
	{
		t2dAnimatedSprite* pAnimatedSprite = static_cast<t2dAnimatedSprite*>( m_pSpeedBarObject );
		pAnimatedSprite->mAnimationController.PauseAnimation();
		
		// Since objects' positions are their centers, move the initial position down by half its height
		F32 fVerticalSize = m_pSpeedBarObject->getSize().mY;
		m_vSpeedBarInitialPosition.mY += (fVerticalSize * 0.5f);
		m_pSpeedBarObject->setPosition( m_vSpeedBarInitialPosition );
		
		// Set the Speed Bar's max position (The position it will be when the Speed XP is completely filled)
		m_vSpeedBarMaxPosition = m_vSpeedBarInitialPosition;
		F32 fHeight = m_pSpeedBarObject->getSize().mY;
		m_vSpeedBarMaxPosition.mY -= fHeight;
		
		//printf( "m_vHealthBarInitialPosition: (%f, %f)\n", m_vSpeedBarInitialPosition.mX, m_vSpeedBarInitialPosition.mY );
		//printf( "m_vSpeedBarMaxPosition:      (%f, %f)\n", m_vSpeedBarMaxPosition.mX, m_vSpeedBarMaxPosition.mY );
		
		m_fSpeedBarYSizeDifference = mFabs( m_vSpeedBarMaxPosition.mY - m_vSpeedBarInitialPosition.mY );
		//printf( " - Speed Y difference: %f\n", m_fSpeedBarYSizeDifference );
		
		
		// Move the Speed Bar up according to how much XP has been added to the Speed XP
		F32 fPercentFilled = ( ((F32)CComponentGlobals::GetInstance().GetTotalSpeedXP()) / ((F32)CComponentGlobals::GetInstance().GetMaxLevelUpUnits()) );
		//printf( "Percent Speed Bar Filled: %f\n", fPercentFilled );
		
		F32 fMovementAmount = fPercentFilled * m_fSpeedBarYSizeDifference;
		//printf( "Speed Movement amount: %f\n", fMovementAmount );
		
		t2dVector vNewPosition = m_vSpeedBarInitialPosition;
		vNewPosition.mY -= fMovementAmount;
		
		m_pSpeedBarObject->setPosition( vNewPosition );
	}
	
	if( m_pManaBarObject )
	{
		t2dAnimatedSprite* pAnimatedSprite = static_cast<t2dAnimatedSprite*>( m_pManaBarObject );
		pAnimatedSprite->mAnimationController.PauseAnimation();
		
		// Since objects' positions are their centers, move the initial position down by half its height
		F32 fVerticalSize = m_pManaBarObject->getSize().mY;
		m_vManaBarInitialPosition.mY += (fVerticalSize * 0.5f);
		m_pManaBarObject->setPosition( m_vManaBarInitialPosition );
		
		// Set the Mana Bar's max position (The position it will be when the Mana XP is completely filled)
		m_vManaBarMaxPosition = m_vManaBarInitialPosition;
		F32 fHeight = m_pManaBarObject->getSize().mY;
		m_vManaBarMaxPosition.mY -= fHeight;
		
		//printf( "m_vManaBarInitialPosition: (%f, %f)\n", m_vManaBarInitialPosition.mX, m_vManaBarInitialPosition.mY );
		//printf( "m_vManaBarMaxPosition:     (%f, %f)\n", m_vManaBarMaxPosition.mX, m_vManaBarMaxPosition.mY );
		
		m_fManaBarYSizeDifference = mFabs( m_vManaBarMaxPosition.mY - m_vManaBarInitialPosition.mY );
		//printf( "- Mana Y difference: %f\n", m_fManaBarYSizeDifference );
		
		
		// Move the Mana Bar up according to how much XP has been added to the Mana XP
		F32 fPercentFilled = ( ((F32)CComponentGlobals::GetInstance().GetTotalManaXP()) / ((F32)CComponentGlobals::GetInstance().GetMaxLevelUpUnits()) );
		//printf( "Percent Mana Bar Filled: %f\n", fPercentFilled );
		
		F32 fMovementAmount = fPercentFilled * m_fManaBarYSizeDifference;
		//printf( "Mana Movement amount: %f\n", fMovementAmount );
		
		t2dVector vNewPosition = m_vManaBarInitialPosition;
		vNewPosition.mY -= fMovementAmount;
		
		m_pManaBarObject->setPosition( vNewPosition );
	}
	
	// Position the Pillar of Light objects - Position them halfway between the top of the screen and the position markers
	t2dVector vPillarOfLightPosition;
	t2dVector vNewPillarSize;
	if( m_pPillarOfLightRed && m_pRedButton )
	{
		vPillarOfLightPosition = m_pRedButton->getPosition();
		vPillarOfLightPosition.mY = (-240.0f + vPillarOfLightPosition.mY) * 0.5f;
		m_pPillarOfLightRed->setPosition( vPillarOfLightPosition );
		
		t2dVector vNewPillarSize( m_pPillarOfLightRed->getSize().mX, 240.0f + mFabs(m_pRedButton->getPosition().mY) );
		m_pPillarOfLightRed->setSize( vNewPillarSize );
		
		m_pPillarOfLightRed->setVisible( false );
	}

	if( m_pPillarOfLightGreen && m_pGreenButton )
	{
		vPillarOfLightPosition = m_pGreenButton->getPosition();
		vPillarOfLightPosition.mY = (-240.0f + vPillarOfLightPosition.mY) * 0.5f;
		m_pPillarOfLightGreen->setPosition( vPillarOfLightPosition );
		
		t2dVector vNewPillarSize( m_pPillarOfLightGreen->getSize().mX, 240.0f + mFabs(m_pGreenButton->getPosition().mY) );
		m_pPillarOfLightGreen->setSize( vNewPillarSize );
		
		m_pPillarOfLightGreen->setVisible( false );
	}
	
	if( m_pPillarOfLightBlue && m_pBlueButton )
	{
		vPillarOfLightPosition = m_pBlueButton->getPosition();
		vPillarOfLightPosition.mY = (-240.0f + vPillarOfLightPosition.mY) * 0.5f;
		m_pPillarOfLightBlue->setPosition( vPillarOfLightPosition );
		
		t2dVector vNewPillarSize( m_pPillarOfLightBlue->getSize().mX, 240.0f + mFabs(m_pBlueButton->getPosition().mY) );
		m_pPillarOfLightBlue->setSize( vNewPillarSize );

		m_pPillarOfLightBlue->setVisible( false );
	}
	
	//if( m_pRPGVineKing )
	//{
	//	m_pRPGVineKing->setVisible( true );
	//	m_pRPGVineKing->playAnimation( g_szRPGVineKingIdleAnim, false );
	//}
	
	//if( m_pVineling01 )
	//	m_pVineling01->setVisible( true );
	
	//if( m_pVineling02 )
	//	m_pVineling02->setVisible( true );
	
	//if( m_pVineling03 )
	//	m_pVineling03->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnHealthButtonPressed()
{
	//printf( "OnHealthButtonPressed()\n" );
	
	m_bHealthButtonPressed = true;
	
	if( m_pHealthBarObject )
	{	
		t2dAnimatedSprite* pHealthBar = static_cast<t2dAnimatedSprite*>( m_pHealthBarObject );
		pHealthBar->mAnimationController.UnpauseAnimation();
	}
	
	if( m_pRedButtonEffect )
		m_pRedButtonEffect->setVisible( true );
	
	if( m_pPillarOfLightRed )
		m_pPillarOfLightRed->setVisible( true );
	
	PlayVineKingChant();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnHealthButtonReleased()
{
	//printf( "OnHealthButtonReleased()\n" );
	
	m_bHealthButtonPressed = false;
	
	if( m_pHealthBarObject )
	{	
		t2dAnimatedSprite* pHealthBar = static_cast<t2dAnimatedSprite*>( m_pHealthBarObject );
		pHealthBar->mAnimationController.PauseAnimation();
	}
	
	if( m_pRedButtonEffect )
		m_pRedButtonEffect->setVisible( false );
	
	if( m_pPillarOfLightRed )
		m_pPillarOfLightRed->setVisible( false );
	
	StopVineKingChant();
	
	g_fDrainButtonHeldTimer = 0.0f;
	g_fXPDrainSpeed = g_fXPDrainSpeedBase;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnSpeedButtonPressed()
{
	//printf( "OnSpeedButtonPressed()\n" );
	
	m_bSpeedButtonPressed = true;
	
	if( m_pSpeedBarObject )
	{	
		t2dAnimatedSprite* pSpeedBar = static_cast<t2dAnimatedSprite*>( m_pSpeedBarObject );
		pSpeedBar->mAnimationController.UnpauseAnimation();
	}
	
	if( m_pGreenButtonEffect )
		m_pGreenButtonEffect->setVisible( true );
	
	if( m_pPillarOfLightGreen )
		m_pPillarOfLightGreen->setVisible( true );
	
	PlayVineKingChant();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnSpeedButtonReleased()
{
	//printf( "OnSpeedButtonReleased()\n" );
	
	m_bSpeedButtonPressed = false;
	
	if( m_pSpeedBarObject )
	{	
		t2dAnimatedSprite* pSpeedBar = static_cast<t2dAnimatedSprite*>( m_pSpeedBarObject );
		pSpeedBar->mAnimationController.PauseAnimation();
	}
	
	if( m_pGreenButtonEffect )
		m_pGreenButtonEffect->setVisible( false );
	
	if( m_pPillarOfLightGreen )
		m_pPillarOfLightGreen->setVisible( false );
	
	StopVineKingChant();
	
	g_fDrainButtonHeldTimer = 0.0f;
	g_fXPDrainSpeed = g_fXPDrainSpeedBase;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnManaButtonPressed()
{
	//printf( "OnManaButtonPressed()\n" );
	
	m_bManaButtonPressed = true;
	
	if( m_pManaBarObject )
	{	
		t2dAnimatedSprite* pManaBar = static_cast<t2dAnimatedSprite*>( m_pManaBarObject );
		pManaBar->mAnimationController.UnpauseAnimation();
	}
	
	if( m_pBlueButtonEffect )
		m_pBlueButtonEffect->setVisible( true );
	
	if( m_pPillarOfLightBlue )
		m_pPillarOfLightBlue->setVisible( true );
	
	PlayVineKingChant();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnManaButtonReleased()
{
	//printf( "OnManaButtonReleased()\n" );
	
	m_bManaButtonPressed = false;
	
	if( m_pManaBarObject )
	{	
		t2dAnimatedSprite* pManaBar = static_cast<t2dAnimatedSprite*>( m_pManaBarObject );
		pManaBar->mAnimationController.PauseAnimation();
	}
	
	if( m_pBlueButtonEffect )
		m_pBlueButtonEffect->setVisible( false );
	
	if( m_pPillarOfLightBlue )
		m_pPillarOfLightBlue->setVisible( false );
	
	StopVineKingChant();
	
	g_fDrainButtonHeldTimer = 0.0f;
	g_fXPDrainSpeed = g_fXPDrainSpeedBase;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::PlayVineKingChant()
{
	//if( m_pRPGVineKing )
	//	m_pRPGVineKing->playAnimation( g_szRPGVineKingChantAnim, false );
	
	CSoundManager::GetInstance().PlayChantingSound();
	
	CSoundManager::GetInstance().PlayBeamOfLightSound();
	CSoundManager::GetInstance().PlayVineGrowSound();
	CSoundManager::GetInstance().PlayCheeringSound();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::StopVineKingChant()
{
	//if( m_pRPGVineKing )
	//	m_pRPGVineKing->playAnimation( g_szRPGVineKingIdleAnim, false );
	
	CSoundManager::GetInstance().StopChantingSound();
	
	CSoundManager::GetInstance().StopBeamOfLightSound();
	CSoundManager::GetInstance().StopVineGrowSound();
	CSoundManager::GetInstance().StopCheeringSound();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnBackButtonPressed()
{
	CloseLevelUpScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnHelpButtonPressed()
{
	DisplayTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnGreenButtonPressed()
{	
	if( m_pRPGBlueButton )
		m_pRPGBlueButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	
	if( m_pRPGPurpleButton )
		m_pRPGPurpleButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	
	
	if( m_pRPGBuyButton )
	{
		S32 iAmount = g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelGreen() ];
		S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
		if( iTotalXP < iAmount )
		{
			m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
			m_pRPGBuyButton->setUseMouseEvents( false );
		}
		else
		{
			m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGreenBuyButtonFrame );
			m_pRPGBuyButton->setUseMouseEvents( true );
		}
	}
	
	m_eBuyButtonState = BUY_BUTTON_STATE_GREEN;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnBlueButtonPressed()
{
	if( m_pRPGGreenButton )
		m_pRPGGreenButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	
	if( m_pRPGPurpleButton )
		m_pRPGPurpleButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	
	if( m_pRPGBuyButton )
	{
		
		S32 iAmount = g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelBlue() ];
		S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
		if( iTotalXP < iAmount )
		{
			m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
			m_pRPGBuyButton->setUseMouseEvents( false );
		}
		else
		{
			m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iBlueBuyButtonFrame );
			m_pRPGBuyButton->setUseMouseEvents( true );
		}
	}
	
	m_eBuyButtonState = BUY_BUTTON_STATE_BLUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnPurpleButtonPressed()
{
	if( m_pRPGGreenButton )
		m_pRPGGreenButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	
	if( m_pRPGBlueButton )
		m_pRPGBlueButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
	
	if( m_pRPGBuyButton )
	{
		S32 iAmount = g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelPurple() ];
		S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
		if( iTotalXP < iAmount )
		{
			m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
			m_pRPGBuyButton->setUseMouseEvents( false );
		}
		else
		{
			m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iPurpleBuyButtonFrame );
			m_pRPGBuyButton->setUseMouseEvents( true );
		}
	}
	
	m_eBuyButtonState = BUY_BUTTON_STATE_PURPLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnBuyButtonPressed()
{
	S32 iTotalXP = 0;
	S32 iAmount = 0;
	
	switch( m_eBuyButtonState )
	{
		case BUY_BUTTON_STATE_GREEN:
			if( CComponentGlobals::GetInstance().GetXPLevelGreen() >= g_iNumPrices )
				return;
			
			iAmount = g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelGreen() ];
			
			iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
			if( iTotalXP < iAmount )
				return;
			
			CComponentGlobals::GetInstance().SubtractFromTotalXP( iAmount );
			
			CSoundManager::GetInstance().PlayCashSound();
			
			CComponentGlobals::GetInstance().IncrementXPLevelGreen();
			CComponentGlobals::GetInstance().IncrementNumUpgradesPurchased();
			
			PlayPurchaseEffect();
			
			if( CComponentGlobals::GetInstance().GetXPLevelGreen() >= g_iNumPrices )
			{
				// This attribute has been upgraded to max
				m_eBuyButtonState = BUY_BUTTON_STATE_NONE;
				if( m_pRPGGreenButton )
				{
					m_pRPGGreenButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
					m_pRPGGreenButton->setUseMouseEvents( false );
				}
				
				if( m_pRPGBuyButton )
				{
					m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
					m_pRPGBuyButton->setUseMouseEvents( false );
				}
				
				// Report the Achievement
				CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_SEED_SPEED_MASTER );
			}
			
			// Disable the button if the new XP amount is less than the new purchase amount
			if( m_pRPGBuyButton )
			{
				if( CComponentGlobals::GetInstance().GetTotalXP() < g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelGreen() ] )
				{
					m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
					m_pRPGBuyButton->setUseMouseEvents( false );
				}
			}
			
			break;
			
		case BUY_BUTTON_STATE_BLUE:
			if( CComponentGlobals::GetInstance().GetXPLevelBlue() >= g_iNumPrices )
				return;
			
			iAmount = g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelBlue() ];
			
			iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
			if( iTotalXP < iAmount )
				return;
			
			CComponentGlobals::GetInstance().SubtractFromTotalXP( iAmount );
			
			CSoundManager::GetInstance().PlayCashSound();
			
			CComponentGlobals::GetInstance().IncrementXPLevelBlue();
			CComponentGlobals::GetInstance().IncrementNumUpgradesPurchased();
			
			PlayPurchaseEffect();
			
			if( CComponentGlobals::GetInstance().GetXPLevelBlue() >= g_iNumPrices )
			{
				// This attribute has been upgraded to max
				m_eBuyButtonState = BUY_BUTTON_STATE_NONE;
				if( m_pRPGBlueButton )
				{
					m_pRPGBlueButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
					m_pRPGBlueButton->setUseMouseEvents( false );
				}
				
				if( m_pRPGBuyButton )
				{
					m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
					m_pRPGBuyButton->setUseMouseEvents( false );
				}
				
				// Report the Achievement
				CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_POWER_MASTER );
			}
			
			// Disable the button if the new XP amount is less than the new purchase amount
			if( m_pRPGBuyButton )
			{
				if( CComponentGlobals::GetInstance().GetTotalXP() < g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelBlue() ] )
				{
					m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
					m_pRPGBuyButton->setUseMouseEvents( false );
				}
			}
			
			break;
			
		case BUY_BUTTON_STATE_PURPLE:
			if( CComponentGlobals::GetInstance().GetXPLevelPurple() >= g_iNumPrices )
				return;
			
			iAmount = g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelPurple() ];
			
			iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
			if( iTotalXP < iAmount )
				return;
			
			CComponentGlobals::GetInstance().SubtractFromTotalXP( iAmount );
			
			CSoundManager::GetInstance().PlayCashSound();
			
			CComponentGlobals::GetInstance().IncrementXPLevelPurple();
			CComponentGlobals::GetInstance().IncrementNumUpgradesPurchased();
			
			PlayPurchaseEffect();
			
			if( CComponentGlobals::GetInstance().GetXPLevelPurple() >= g_iNumPrices )
			{
				// This attribute has been upgraded to max
				m_eBuyButtonState = BUY_BUTTON_STATE_NONE;
				if( m_pRPGPurpleButton )
				{
					m_pRPGPurpleButton->setImageMap( g_szPurchaseSelectionImageMap, g_iPurchaseSelectionFrameBlank );
					m_pRPGPurpleButton->setUseMouseEvents( false );
				}
				
				if( m_pRPGBuyButton )
				{
					m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
					m_pRPGBuyButton->setUseMouseEvents( false );
				}
				
				// Report the Achievement
				CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_VINE_SPEED_MASTER );
			}
			
			// Disable the button if the new XP amount is less than the new purchase amount
			if( m_pRPGBuyButton )
			{
				if( CComponentGlobals::GetInstance().GetTotalXP() < g_iPrices[ CComponentGlobals::GetInstance().GetXPLevelPurple() ] )
				{
					m_pRPGBuyButton->setImageMap( g_szRPGBuyButtonImageMap, g_iGrayBuyButtonFrame );
					m_pRPGBuyButton->setUseMouseEvents( false );
				}
			}
			
			break;
			
		default:
			break;
	}
	
	//m_eBuyButtonState = BUY_BUTTON_STATE_NONE;
	
	UpdateTotalXPTextDisplay();
	UpdatePriceDisplay();
	UpdateMarkerDisplay();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::UpdateTotalXPTextDisplay()
{
	// Change the total XP displayed
	if( m_pTotalXPText )
	{
		S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
		if( iTotalXP <= 0 )
		{
			iTotalXP = 0;
			m_bHealthButtonPressed = false;
		}
		
		sprintf( g_szStringBuffer, "%d", iTotalXP );
		m_pTotalXPText->setText( g_szStringBuffer );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::UpdatePriceDisplay()
{
	// Green
	if( m_pPriceGreen )
	{
		if( CComponentGlobals::GetInstance().GetXPLevelGreen() <= g_iNumPrices )
			m_pPriceGreen->setImageMap( g_szPriceImageMap, CComponentGlobals::GetInstance().GetXPLevelGreen() );
	}

	
	// Blue
	if( m_pPriceBlue )
	{
		if( CComponentGlobals::GetInstance().GetXPLevelBlue() <= g_iNumPrices )
			m_pPriceBlue->setImageMap( g_szPriceImageMap, CComponentGlobals::GetInstance().GetXPLevelBlue() );
	}
	
	// Purple
	if( m_pPricePurple )
	{
		if( CComponentGlobals::GetInstance().GetXPLevelPurple() <= g_iNumPrices )
			m_pPricePurple->setImageMap( g_szPriceImageMap, CComponentGlobals::GetInstance().GetXPLevelPurple() );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::UpdateMarkerDisplay()
{
	// Green
	switch( CComponentGlobals::GetInstance().GetXPLevelGreen() )
	{
			// case statements have intentional fall-through
		case 6:
			if( m_pGreenMarker06 )
				m_pGreenMarker06->setVisible( true );
			
		case 5:
			if( m_pGreenMarker05 )
				m_pGreenMarker05->setVisible( true );
			
		case 4:
			if( m_pGreenMarker04 )
				m_pGreenMarker04->setVisible( true );
			
		case 3:
			if( m_pGreenMarker03 )
				m_pGreenMarker03->setVisible( true );
			
		case 2:
			if( m_pGreenMarker02 )
				m_pGreenMarker02->setVisible( true );
			
		case 1:
			if( m_pGreenMarker01 )
				m_pGreenMarker01->setVisible( true );
			
			break;
			
		default:
			break;
	}
	
	// Blue
	switch( CComponentGlobals::GetInstance().GetXPLevelBlue() )
	{
		// case statements have intentional fall-through
		case 6:
			if( m_pBlueMarker06 )
				m_pBlueMarker06->setVisible( true );
			
		case 5:
			if( m_pBlueMarker05 )
				m_pBlueMarker05->setVisible( true );
			
		case 4:
			if( m_pBlueMarker04 )
				m_pBlueMarker04->setVisible( true );
			
		case 3:
			if( m_pBlueMarker03 )
				m_pBlueMarker03->setVisible( true );
			
		case 2:
			if( m_pBlueMarker02 )
				m_pBlueMarker02->setVisible( true );
			
		case 1:
			if( m_pBlueMarker01 )
				m_pBlueMarker01->setVisible( true );
			
			break;
			
		default:
			break;
	}
	
	// Purple
	switch( CComponentGlobals::GetInstance().GetXPLevelPurple() )
	{
		// case statements have intentional fall-through
		case 6:
			if( m_pPurpleMarker06 )
				m_pPurpleMarker06->setVisible( true );
			
		case 5:
			if( m_pPurpleMarker05 )
				m_pPurpleMarker05->setVisible( true );
			
		case 4:
			if( m_pPurpleMarker04 )
				m_pPurpleMarker04->setVisible( true );
			
		case 3:
			if( m_pPurpleMarker03 )
				m_pPurpleMarker03->setVisible( true );
			
		case 2:
			if( m_pPurpleMarker02 )
				m_pPurpleMarker02->setVisible( true );
			
		case 1:
			if( m_pPurpleMarker01 )
				m_pPurpleMarker01->setVisible( true );
			
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::PlayPurchaseEffect()
{
	bool bPlayEffect = true;
	t2dVector vPosition( t2dVector::getZero() );
	
	switch( m_eBuyButtonState )
	{
		case BUY_BUTTON_STATE_GREEN:
			
			if( CComponentGlobals::GetInstance().GetXPLevelGreen() == 1 )
				vPosition = m_pGreenMarker01 ? m_pGreenMarker01->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelGreen() == 2 )
				vPosition = m_pGreenMarker02 ? m_pGreenMarker02->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelGreen() == 3 )
				vPosition = m_pGreenMarker03 ? m_pGreenMarker03->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelGreen() == 4 )
				vPosition = m_pGreenMarker04 ? m_pGreenMarker04->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelGreen() == 5 )
				vPosition = m_pGreenMarker05 ? m_pGreenMarker05->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelGreen() == 6 )
				vPosition = m_pGreenMarker06 ? m_pGreenMarker06->getPosition() : t2dVector::getZero();
			
			break;
			
		case BUY_BUTTON_STATE_BLUE:
			
			if( CComponentGlobals::GetInstance().GetXPLevelBlue() == 1 )
				vPosition = m_pBlueMarker01 ? m_pBlueMarker01->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelBlue() == 2 )
				vPosition = m_pBlueMarker02 ? m_pBlueMarker02->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelBlue() == 3 )
				vPosition = m_pBlueMarker03 ? m_pBlueMarker03->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelBlue() == 4 )
				vPosition = m_pBlueMarker04 ? m_pBlueMarker04->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelBlue() == 5 )
				vPosition = m_pBlueMarker05 ? m_pBlueMarker05->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelBlue() == 6 )
				vPosition = m_pBlueMarker06 ? m_pBlueMarker06->getPosition() : t2dVector::getZero();
			
			break;
			
		case BUY_BUTTON_STATE_PURPLE:
			
			if( CComponentGlobals::GetInstance().GetXPLevelPurple() == 1 )
				vPosition = m_pPurpleMarker01 ? m_pPurpleMarker01->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelPurple() == 2 )
				vPosition = m_pPurpleMarker02 ? m_pPurpleMarker02->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelPurple() == 3 )
				vPosition = m_pPurpleMarker03 ? m_pPurpleMarker03->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelPurple() == 4 )
				vPosition = m_pPurpleMarker04 ? m_pPurpleMarker04->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelPurple() == 5 )
				vPosition = m_pPurpleMarker05 ? m_pPurpleMarker05->getPosition() : t2dVector::getZero();
			else if( CComponentGlobals::GetInstance().GetXPLevelPurple() == 6 )
				vPosition = m_pPurpleMarker06 ? m_pPurpleMarker06->getPosition() : t2dVector::getZero();
			
			break;
			
		default:
			bPlayEffect = false;
			break;
	}
	
	if( bPlayEffect )
	{
		if( m_pPurchaseEffect )
		{
			m_pPurchaseEffect->setPosition( vPosition );
			m_pPurchaseEffect->setVisible( true );
			m_pPurchaseEffect->playAnimation( g_szRPG_BuyAnimName, false );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::EnableRPGScreenButtonsFromTutorial()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLevelUpScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentLevelUpScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLevelUpScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentLevelUpScreen::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::onUpdate()
{
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
	
	
	if( m_bHealthButtonPressed == false && m_bSpeedButtonPressed == false && m_bManaButtonPressed == false )
		return;
	
//	g_fDrainButtonHeldTimer += ITickable::smTickSec;
//	if( g_fDrainButtonHeldTimer <= 1.0f )
//		g_fXPDrainSpeed = 100.0f;
//	else if( g_fDrainButtonHeldTimer < 2.0f )
//		g_fXPDrainSpeed = 200.0f;
//	else if( g_fDrainButtonHeldTimer < 3.0f )
//		g_fXPDrainSpeed = 400.0f;
//	else if( g_fDrainButtonHeldTimer < 4.0f )
//		g_fXPDrainSpeed = 800.0f;
//	else if( g_fDrainButtonHeldTimer >= 4.0f )
//		g_fXPDrainSpeed = 1000.0f;
	
	//S32 iDrainAmount = (S32)(g_fXPDrainSpeed * ITickable::smTickSec);
	//printf( "Held Timer:   %f\n", g_fDrainButtonHeldTimer );
	//printf( "Drain Speed:  %f\n", g_fXPDrainSpeed );
	//printf( "Drain Amount: %d\n", iDrainAmount );
	
	//-----------------------------------
	if( m_bHealthButtonPressed )
	{
		// Drain XP from total and add it to Health XP
		CComponentGlobals::GetInstance().SubtractFromTotalXP( (S32)(g_fXPDrainSpeed * ITickable::smTickSec) );
		CComponentGlobals::GetInstance().AddToTotalHealthXP( (S32)(g_fXPDrainSpeed * ITickable::smTickSec) );
		
		// Change the total XP displayed
		if( m_pTotalXPText )
		{
			S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
			if( iTotalXP <= 0 )
			{
				iTotalXP = 0;
				m_bHealthButtonPressed = false;
			}
			
			sprintf( g_szStringBuffer, "%d", iTotalXP );
			m_pTotalXPText->setText( g_szStringBuffer );
		}
		
		// Move the Health bar upwards
		if( m_pHealthBarObject )
		{
			F32 fPercentFilled = ( ((F32)CComponentGlobals::GetInstance().GetTotalHealthXP()) / ((F32)CComponentGlobals::GetInstance().GetMaxLevelUpUnits()) );
			//printf( "Percent Health Bar Filled: %f\n", fPercentFilled );
			
			F32 fMovementAmount = fPercentFilled * m_fHealthBarYSizeDifference;
			//printf( "Health Movement amount: %f\n", fMovementAmount );
			
			t2dVector vNewPosition = m_vHealthBarInitialPosition;
			vNewPosition.mY -= fMovementAmount;
			
			m_pHealthBarObject->setPosition( vNewPosition );
		}
		
		if( CComponentGlobals::GetInstance().GetTotalHealthXP() >= CComponentGlobals::GetInstance().GetMaxLevelUpUnits() )
		{
			m_bHealthButtonPressed = false;
		}
	}
	
	//-----------------------------------
	if( m_bSpeedButtonPressed )
	{
		
		// Drain XP from total and add it to Seed Speed XP
		CComponentGlobals::GetInstance().SubtractFromTotalXP( (S32)(g_fXPDrainSpeed * ITickable::smTickSec) );
		CComponentGlobals::GetInstance().AddToTotalSpeedXP( (S32)(g_fXPDrainSpeed * ITickable::smTickSec) );
		
		// Change the total XP displayed
		if( m_pTotalXPText )
		{
			S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
			if( iTotalXP <= 0 )
			{
				iTotalXP = 0;
				m_bSpeedButtonPressed = false;
			}
			
			sprintf( g_szStringBuffer, "%d", iTotalXP );
			m_pTotalXPText->setText( g_szStringBuffer );
		}
		
		// Move the Speed bar upwards
		if( m_pSpeedBarObject )
		{
			F32 fPercentFilled = ( ((F32)CComponentGlobals::GetInstance().GetTotalSpeedXP()) / ((F32)CComponentGlobals::GetInstance().GetMaxLevelUpUnits()) );
			//printf( "Percent Speed Bar Filled: %f\n", fPercentFilled );
			
			F32 fMovementAmount = fPercentFilled * m_fSpeedBarYSizeDifference;
			//printf( "Speed Movement amount: %f\n", fMovementAmount );
			
			t2dVector vNewPosition = m_vSpeedBarInitialPosition;
			vNewPosition.mY -= fMovementAmount;
			
			m_pSpeedBarObject->setPosition( vNewPosition );
		}
		
		if( CComponentGlobals::GetInstance().GetTotalSpeedXP() >= CComponentGlobals::GetInstance().GetMaxLevelUpUnits() )
		{
			m_bSpeedButtonPressed = false;
		}
	}
	
	//-----------------------------------
	if( m_bManaButtonPressed )
	{
		// Drain XP from total and add it to Mana XP
		CComponentGlobals::GetInstance().SubtractFromTotalXP( (S32)(g_fXPDrainSpeed * ITickable::smTickSec) );
		CComponentGlobals::GetInstance().AddToTotalManaXP( (S32)(g_fXPDrainSpeed * ITickable::smTickSec) );
		
		// Change the total XP displayed
		if( m_pTotalXPText )
		{
			S32 iTotalXP = CComponentGlobals::GetInstance().GetTotalXP();
			if( iTotalXP <= 0 )
			{
				iTotalXP = 0;
				//m_bManaButtonPressed = false;
				OnManaButtonReleased();
			}
			
			sprintf( g_szStringBuffer, "%d", iTotalXP );
			m_pTotalXPText->setText( g_szStringBuffer );
		}
		
		// Move the Mana bar upwards
		if( m_pManaBarObject )
		{
			F32 fPercentFilled = ( ((F32)CComponentGlobals::GetInstance().GetTotalManaXP()) / ((F32)CComponentGlobals::GetInstance().GetMaxLevelUpUnits()) );
			//printf( "Percent Mana Bar Filled: %f\n", fPercentFilled );
			
			F32 fMovementAmount = fPercentFilled * m_fManaBarYSizeDifference;
			//printf( "Mana Movement amount: %f\n", fMovementAmount );
			
			t2dVector vNewPosition = m_vManaBarInitialPosition;
			vNewPosition.mY -= fMovementAmount;
			
			m_pManaBarObject->setPosition( vNewPosition );
		}
		
		if( CComponentGlobals::GetInstance().GetTotalManaXP() >= CComponentGlobals::GetInstance().GetMaxLevelUpUnits() )
		{
			m_bManaButtonPressed = false;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnPostInit()
{
	g_fDrainButtonHeldTimer = 0.0f;
	g_fXPDrainSpeed = g_fXPDrainSpeedBase;
	
	CComponentGlobals::GetInstance().SetLevelUpScreenIsValid();
	
	
	
	// Start the screen invisible
	//if( m_pOwner )
	//	m_pOwner->setVisible( false );
	
//	m_pRedButtonEffect = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szRedButtonEffectName ) );
//	if( m_pRedButtonEffect )
//	{
//		m_pRedButtonEffect->setPosition( g_vRedEffectPosition );
//		m_pRedButtonEffect->setVisible( false );
//	}
//	
//	m_pGreenButtonEffect = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szGreenButtonEffectName ) );
//	if( m_pGreenButtonEffect )
//	{
//		m_pGreenButtonEffect->setPosition( g_vGreenEffectPosition );
//		m_pGreenButtonEffect->setVisible( false );
//	}
//	
//	m_pBlueButtonEffect = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szBlueButtonEffectName ) );
//	if( m_pBlueButtonEffect )
//	{
//		m_pBlueButtonEffect->setPosition( g_vBlueEffectPosition );
//		m_pBlueButtonEffect->setVisible( false );
//	}
//	
//
//	m_pPillarOfLightRed = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szPillarOfLightRed ) );
//	if( m_pPillarOfLightRed )
//		m_pPillarOfLightRed->setVisible( false );
//	
//	m_pPillarOfLightGreen = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szPillarOfLightGreen ) );
//	if( m_pPillarOfLightGreen )
//		m_pPillarOfLightGreen->setVisible( false );
//	
//	m_pPillarOfLightBlue = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szPillarOfLightBlue ) );
//	if( m_pPillarOfLightBlue )
//		m_pPillarOfLightBlue->setVisible( false );
//	
//	m_pRPGVineKing = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szRPGVineKing ) );
//	if( m_pRPGVineKing )
//		m_pRPGVineKing->setVisible( false );
//	
//	
//	m_pVineling01 = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szVineling01 ) );
//	if( m_pVineling01 )
//		m_pVineling01->setVisible( false );
//	
//	m_pVineling02 = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szVineling02 ) );
//	if( m_pVineling02 )
//		m_pVineling02->setVisible( false );
//	
//	m_pVineling03 = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szVineling03 ) );
//	if( m_pVineling03 )
//		m_pVineling03->setVisible( false );
	
	g_pTutorialTop		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialTop ) );
	g_pTutorialBottom	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialBottom ) );
	g_pTutorialNext		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szTutorialNext ) );
	
	
	
	// New RPG stuff
	m_pRPGScreenBG		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Screen_BG ) );
	m_pRPGBackground	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Background ) );
	if( m_pRPGScreenBG && m_pRPGBackground )
		m_pRPGBackground->setPosition( m_pRPGScreenBG->getPosition() );

	m_pRPGBuyButton		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Buy_Button ) );
	
	m_pRPGGreenButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button ) );
	
	m_pRPGBlueButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button ) );
	
	m_pRPGPurpleButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button ) );

	m_pGreenMarker01	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button_Level_1 ) );
	SetDefaultMarkerAttributes( m_pGreenMarker01 );
	
	m_pGreenMarker02	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button_Level_2 ) );
	SetDefaultMarkerAttributes( m_pGreenMarker02 );
	
	m_pGreenMarker03	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button_Level_3 ) );
	SetDefaultMarkerAttributes( m_pGreenMarker03 );
	
	m_pGreenMarker04	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button_Level_4 ) );
	SetDefaultMarkerAttributes( m_pGreenMarker04 );
	
	m_pGreenMarker05	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button_Level_5 ) );
	SetDefaultMarkerAttributes( m_pGreenMarker05 );
	
	m_pGreenMarker06	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Green_Button_Level_6 ) );
	SetDefaultMarkerAttributes( m_pGreenMarker06 );
	

	m_pBlueMarker01		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button_Level_1 ) );
	SetDefaultMarkerAttributes( m_pBlueMarker01 );
	
	m_pBlueMarker02		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button_Level_2 ) );
	SetDefaultMarkerAttributes( m_pBlueMarker02 );
	
	m_pBlueMarker03		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button_Level_3 ) );
	SetDefaultMarkerAttributes( m_pBlueMarker03 );
	
	m_pBlueMarker04		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button_Level_4 ) );
	SetDefaultMarkerAttributes( m_pBlueMarker04 );
	
	m_pBlueMarker05		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button_Level_5 ) );
	SetDefaultMarkerAttributes( m_pBlueMarker05 );
	
	m_pBlueMarker06		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Blue_Button_Level_6 ) );
	SetDefaultMarkerAttributes( m_pBlueMarker06 );

	
	m_pPurpleMarker01	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button_Level_1 ) );
	SetDefaultMarkerAttributes( m_pPurpleMarker01 );
	
	m_pPurpleMarker02	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button_Level_2 ) );
	SetDefaultMarkerAttributes( m_pPurpleMarker02 );
	
	m_pPurpleMarker03	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button_Level_3 ) );
	SetDefaultMarkerAttributes( m_pPurpleMarker03 );
	
	m_pPurpleMarker04	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button_Level_4 ) );
	SetDefaultMarkerAttributes( m_pPurpleMarker04 );
	
	m_pPurpleMarker05	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button_Level_5 ) );
	SetDefaultMarkerAttributes( m_pPurpleMarker05 );
	
	m_pPurpleMarker06	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Purple_Button_Level_6 ) );
	SetDefaultMarkerAttributes( m_pPurpleMarker06 );
	
	m_pRPGMapButton		= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_BackButton ) );
	
	m_pRPGHelpButton	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Help_Button ) );
	
	m_pTotalXPText		= static_cast<t2dTextObject*>( Sim::findObject( g_szTotalXPText ) );
	
	m_pPriceGreen	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Price_Green ) );
	m_pPriceBlue	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Price_Blue ) );
	m_pPricePurple	= static_cast<t2dStaticSprite*>( Sim::findObject( g_szRPG_Price_Purple ) );
	
	//static const char g_szRPG_BuyAnimObject[32] = "RPG_BuyAnim";	// This is the actual object
	//static const char g_szRPG_BuyAnimName[32] = "rpg_fxAnimation";	// This is the name of the animation
	m_pPurchaseEffect = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szRPG_BuyAnimObject ) );
	if( m_pPurchaseEffect )
		m_pPurchaseEffect->setVisible( false );
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
	{
		g_pFadeBox->setPosition( m_pRPGScreenBG ? m_pRPGScreenBG->getPosition() : t2dVector::getZero() );
		g_pFadeBox->setVisible( true );
		g_pFadeBox->setBlendAlpha( 1.0f );
		
		// Start by fading in
		m_eFadeState = FADE_STATE_NONE;
		m_fFadeTimer = g_fFadeTimeSeconds;
	}
	
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( g_szSceneWindow ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::UpdateFade()
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

void CComponentLevelUpScreen::OnFadeOutFinished() // Called when completely black
{
	//printf( "CComponentLevelUpScreen::OnFadeOutFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;
	
	if( g_pFadeBox) 
		g_pFadeBox->setBlendAlpha( 1.0f );
	
	if( m_pRPGScreenBG )
	{
		m_pRPGScreenBG->setPosition( g_vSafePosition );
		
		if( m_pRPGBackground )
			m_pRPGBackground->setPosition( m_pRPGScreenBG->getPosition() );
		
		if( g_pFadeBox )
		{
			g_pFadeBox->setBlendAlpha( 1.0f );
			g_pFadeBox->setPosition( m_pRPGScreenBG->getPosition() );
		}
	}
	
	CComponentMapSelectMenu::GetInstance().OpenMapSelectScreen(); // This function should change the camera and then start a fade in (handled on that end)
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::OnFadeInFinished() // Called when completely transparent
{
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;
	
	if( g_pFadeBox )
		g_pFadeBox->setBlendAlpha( 0.0f );
	
	if( CComponentInGameTutorialObject::DoesLevelHaveMapTutorial() )
	{
		if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_POWER_UP ) == false )
		{
			CComponentInGameTutorialObject::ActivateRPGTutorial();
			CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_POWER_UP );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::DisplayTutorial()
{
	if( CComponentInGameTutorialObject::DoesLevelHaveMapTutorial() )
		CComponentInGameTutorialObject::ActivateRPGTutorial();
	
	CComponentInGameTutorialObject::SetInGameTutorialActive();
	
	CComponentInGameTutorialObject::SetInPowerUpScreen();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelUpScreen::SetDefaultMarkerAttributes( t2dSceneObject* const _pMarker )
{
	if( _pMarker == NULL )
		return;
	
	_pMarker->setMountInheritAttributes( false );
	_pMarker->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------