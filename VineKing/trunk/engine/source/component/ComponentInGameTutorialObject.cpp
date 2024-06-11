//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentInGameTutorialObject.h"

#include "ComponentGlobals.h"
#include "ComponentLevelUpScreen.h"
#include "ComponentMapSelectMenu.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static const char g_szInGameTutorial_Image[32]	= "InGameTutorial_Image";
static const char g_szInGameTutorial_Button[32] = "InGameTutorial_Button";

static const char g_szTutorialImageMap_MapScreen[32] = "intro_mapImageMap";
static const char g_szTutorialImageMap_RPGScreen[32] = "intro_shopImageMap";

static const t2dVector g_vSafePosition( 2000.0f, 2000.0f );

// Scaling
static const F32 g_fScaleMin = 0.0f;
static const F32 g_fScaleMax = 1.0f;

static F32 g_fScaleCurrent = g_fScaleMin;

static const F32 g_fScaleRate = 3.0f;	// Units of scaling per second

//static const F32 g_fScaleTime = (g_fScaleMax - g_fScaleMin) / g_fScaleRate; // TOTAL_SCALE_TIME = SCALE_DISTANCE / SCALE_RATE

static t2dVector g_vOriginalSize_Image;
static t2dVector g_vOriginalSize_Button;
static t2dVector g_vTempSize;

// Gray Box
static const char g_szGrayBox[32] = "GrayBox";
static t2dStaticSprite* g_pGrayBox = NULL;

// Smoke Tutorial
static const F32 g_fSmokeTutorialDelayTime = 2.0f;
static F32 g_fSmokeTutorialTimer = 0.0f;
static bool g_bSmokeTutorialTimerActive = false;

// Boss01 frustration tracking tutorial
static S32 g_iBossTutorialPageNumber = 0;
static const char g_szBoss01HitTutorialImage[32] = "intro_mr_magma_02ImageMap";

// Map / RPG Screen reminder
static const char g_szMapShopScreenReminderImage[32] = "P02_Map_Shop_ReminderImageMap";

// Map DLC Announcement 
static const char g_szDLCMapAnnouncementImage[32] = "P02_MOM_TutorialImageMap";

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentInGameTutorialObject::ETutorialState CComponentInGameTutorialObject::sm_eTutorialState = CComponentInGameTutorialObject::TUTORIAL_STATE_NONE;
CComponentInGameTutorialObject::ETutorialType CComponentInGameTutorialObject::sm_eTutorialType = CComponentInGameTutorialObject::TUTORIAL_NONE;

bool CComponentInGameTutorialObject::sm_bTutorialActive = false;

t2dStaticSprite* CComponentInGameTutorialObject::sm_pImage = NULL;
t2dStaticSprite* CComponentInGameTutorialObject::sm_pButton = NULL;

bool CComponentInGameTutorialObject::sm_bLevelHasTutorial = false;

bool CComponentInGameTutorialObject::sm_bIsInPowerUpScreen = false;

bool CComponentInGameTutorialObject::sm_bOpenedRPGTutorial = false;

bool CComponentInGameTutorialObject::sm_bTimeScaleStopped = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentInGameTutorialObject );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentInGameTutorialObject::CComponentInGameTutorialObject()
	: m_pOwner( NULL )
	, m_eObjectType( OBJECT_TYPE_NONE )
{
	//CComponentInGameTutorialObject::sm_bLevelHasTutorial = false;
	//CComponentInGameTutorialObject::sm_pImage = NULL;  // THIS COULD POTENTIALLY CLEAR BOTH VARIABLES
	//CComponentInGameTutorialObject::sm_pButton = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentInGameTutorialObject::~CComponentInGameTutorialObject()
{
	//CComponentInGameTutorialObject::sm_bTutorialActive = false;
	//CComponentInGameTutorialObject::sm_bLevelHasTutorial = false;
	//CComponentInGameTutorialObject::sm_pImage = NULL;
	//CComponentInGameTutorialObject::sm_pButton = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::Initialize()
{
	//CComponentInGameTutorialObject::sm_bTutorialActive = false;
	//CComponentInGameTutorialObject::sm_bIsInPowerUpScreen = false;
	//CComponentInGameTutorialObject::sm_pImage = NULL;
	//CComponentInGameTutorialObject::sm_pButton = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ResetInGameTutorialData()
{
	// This function should only be called when loading a level
	sm_eTutorialState = TUTORIAL_STATE_NONE;
	sm_eTutorialType = TUTORIAL_NONE;
	sm_bTutorialActive = false;
	sm_bLevelHasTutorial = false;
	sm_bIsInPowerUpScreen = false;
	sm_pImage = NULL;
	sm_pButton = NULL;
	
	//g_fSmokeTutorialDelayTime = 2.0f;
	g_fSmokeTutorialTimer = 0.0f;
	g_bSmokeTutorialTimerActive = false;
	
	sm_bOpenedRPGTutorial = false;
	
	g_pGrayBox = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateTutorial()
{	
	if( sm_pImage == NULL || sm_pButton == NULL )
		return;
	
	sm_bTutorialActive = true;
	
	sm_pImage->setVisible( true );
	sm_pImage->setPosition( t2dVector::getZero() );
	
	sm_pButton->setVisible( true );
	
	//g_vOriginalSize_Image = sm_pImage->getSize();
	//g_vOriginalSize_Button = sm_pButton->getSize();
	
	g_fScaleCurrent = g_fScaleMin;
	
	sm_eTutorialState = TUTORIAL_STATE_DISPLAY_SCALING;
	
	sm_pButton->setUseMouseEvents( false );
	
	if( g_pGrayBox )
	{
		g_pGrayBox->setVisible( true );
		g_pGrayBox->setUseMouseEvents( true );
		g_pGrayBox->setPosition( t2dVector::getZero() );
	}
	
	sm_bTimeScaleStopped = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateMapTutorial()
{
	sm_bOpenedRPGTutorial = false;
	
	if( sm_pImage )
		sm_pImage->setImageMap( g_szTutorialImageMap_MapScreen, 0 );
	
	ActivateTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateRPGTutorial()
{
	sm_bOpenedRPGTutorial = true;
	
	if( sm_pImage )
		sm_pImage->setImageMap( g_szTutorialImageMap_RPGScreen, 0 );
	
	ActivateTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateSmokeTutorial()
{
	//g_fSmokeTutorialTimer = 0.0f;
	//g_bSmokeTutorialTimerActive = true;
	
	ActivateTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateBossTutorial()
{
	if( sm_pImage && g_iBossTutorialPageNumber == 1 )
	{
		sm_pImage->setImageMap( g_szBoss01HitTutorialImage, 0 );
	}
		
	ActivateTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateShopReminderTutorial()
{
	sm_bOpenedRPGTutorial = false;
	
	if( sm_pImage )
		sm_pImage->setImageMap( g_szMapShopScreenReminderImage, 0 );
	
	ActivateTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::ActivateDLCMapAnnouncementTutorial()
{
	sm_bOpenedRPGTutorial = false;
	if( sm_pImage )
		sm_pImage->setImageMap( g_szDLCMapAnnouncementImage, 0 );
	
	ActivateTutorial();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::DeactivateTutorial()
{
	if( sm_pImage == NULL || sm_pButton == NULL )
		return;
	
	Con::setVariable( "$timeScale", "1" );
	
	sm_bTutorialActive = false;
	
	sm_pImage->setPosition( g_vSafePosition );
	
	t2dVector vImageSize( g_vOriginalSize_Image.mX * g_fScaleMin, g_vOriginalSize_Image.mY * g_fScaleMin );
	sm_pImage->setSize( vImageSize );
	sm_pImage->setVisible( false );
	
	t2dVector vButtonSize( g_vOriginalSize_Button.mX * g_fScaleMin, g_vOriginalSize_Button.mY * g_fScaleMin );
	sm_pButton->setSize( vButtonSize );
	sm_pButton->setVisible( false );

	sm_eTutorialState = TUTORIAL_STATE_NONE;
	
	if( g_pGrayBox )
		g_pGrayBox->setVisible( false );
	
	// Start the Music and the Game Timer
	if( sm_eTutorialType == TUTORIAL_SERPENT ||
	   sm_eTutorialType == TUTORIAL_CRYSTAL ||
	   sm_eTutorialType == TUTORIAL_LAVAPIT ||
	   sm_eTutorialType == TUTORIAL_SMOKE ||
	   sm_eTutorialType == TUTORIAL_SUPER_BLOB ||
	   sm_eTutorialType == TUTORIAL_SUPER_SERPENT ||
	   sm_eTutorialType == TUTORIAL_SUPER_SMOKE )
	{
		CSoundManager::GetInstance().PlayInGameBGM();
		CComponentGlobals::GetInstance().StartTimer();
	}
	else if( sm_eTutorialType == TUTORIAL_BOSS && g_iBossTutorialPageNumber == 1 )
	{
		CSoundManager::GetInstance().PlayInGameBGM();
		CComponentGlobals::GetInstance().StartTimer();
	}
	
	// Enable button in the Map or RPG screen
	if( sm_eTutorialType == TUTORIAL_MAP_RPG )
	{
		if( sm_bOpenedRPGTutorial )
		{
			CComponentLevelUpScreen::GetInstance().EnableRPGScreenButtonsFromTutorial();
		}
		else
		{
			CComponentMapSelectMenu::GetInstance().EnableMapScreenButtonsFromTutorial();
		}
	}
	else if( sm_eTutorialType == TUTORIAL_BOSS )
	{
		if( g_iBossTutorialPageNumber == 0 )
		{
			g_iBossTutorialPageNumber = 1;
			ActivateBossTutorial();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::SetInGameTutorialActive()
{ 
	//sm_bTutorialActive = true; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::SetInGameTutorialInactive()
{
	sm_bTutorialActive = false;
	
	CComponentInGameTutorialObject::sm_bIsInPowerUpScreen = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentInGameTutorialObject::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentInGameTutorialObject ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentInGameTutorialObject::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dStaticSprite* pOwnerObject = dynamic_cast<t2dStaticSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s - Must be added to a t2dStaticSprite.", __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentInGameTutorialObject::onUpdate()
{
	if( m_pOwner == NULL || CComponentInGameTutorialObject::sm_pImage == NULL || CComponentInGameTutorialObject::sm_pButton == NULL )
		return;
	
	// Only update the image
	if( m_eObjectType == OBJECT_TYPE_BUTTON )
		return;
	
	//if( g_iBossTutorialPageNumber == 1 )
	//{
		//printf( "---------\n" );
		//printf( "sm_pImage Visible?: %d\n", sm_pImage->getVisible() );
		//printf( "sm_pImage Position: (%f, %f)\n", sm_pImage->getPosition().mX, sm_pImage->getPosition().mY );
		//printf( "sm_pImage Size:     (%f, %f)\n", sm_pImage->getSize().mX, sm_pImage->getSize().mY );
	//}
	
//	if( sm_eTutorialType == TUTORIAL_SMOKE || sm_eTutorialType == TUTORIAL_SUPER_SMOKE )
//	{
//		if( g_bSmokeTutorialTimerActive )
//		{
//			g_fSmokeTutorialTimer += ITickable::smTickSec;
//			if( g_fSmokeTutorialTimer >= g_fSmokeTutorialDelayTime )
//			{
//				ActivateTutorial();
//				g_bSmokeTutorialTimerActive = false;
//			}
//			return;
//		}
//		else if( sm_bTutorialActive == false )
//		{
//			return;
//		}
//	}
//	else
	{
		if( sm_bTutorialActive == false )
			return;
	}
	
	if( sm_eTutorialState == TUTORIAL_STATE_DISPLAY_FULL_SIZE )
	{
		if( sm_bTimeScaleStopped == false )
		{
			Con::setVariable( "$timeScale", "0" );
			sm_bTimeScaleStopped = true;
		}
		return;
	}
	
	// Handle Scaling
	g_fScaleCurrent += g_fScaleRate * ITickable::smTickSec;
	if( g_fScaleCurrent >= g_fScaleMax )
	{
		g_fScaleCurrent = g_fScaleMax;
		CComponentInGameTutorialObject::sm_pButton->setUseMouseEvents( true );
		sm_eTutorialState = TUTORIAL_STATE_DISPLAY_FULL_SIZE;
		
		//printf( "Displaying In Game Tutorial\n" );
		//Con::setVariable( "$timeScale", "0" );
	}
	
	g_vTempSize = g_vOriginalSize_Image;
	g_vTempSize.mX *= g_fScaleCurrent;
	g_vTempSize.mY *= g_fScaleCurrent;
	CComponentInGameTutorialObject::sm_pImage->setSize( g_vTempSize );
	
	//printf( "Image Size: (%f, %f)\n", CComponentInGameTutorialObject::sm_pImage->getSize().mX, CComponentInGameTutorialObject::sm_pImage->getSize().mY );
	//printf( "Image Pos:  (%f, %f)\n", CComponentInGameTutorialObject::sm_pImage->getPosition().mX, CComponentInGameTutorialObject::sm_pImage->getPosition().mY );
	//printf( "Image Visible?  %d\n", CComponentInGameTutorialObject::sm_pImage->getVisible() );
	
	g_vTempSize = g_vOriginalSize_Button;
	g_vTempSize.mX *= g_fScaleCurrent;
	g_vTempSize.mY *= g_fScaleCurrent;
	CComponentInGameTutorialObject::sm_pButton->setSize( g_vTempSize );
	
//	if( sm_eTutorialState == TUTORIAL_STATE_DISPLAY_FULL_SIZE )
//	{
//		Con::setVariable( "$timeScale", "0" );
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentInGameTutorialObject::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eObjectType != OBJECT_TYPE_BUTTON )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentInGameTutorialObject::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eObjectType != OBJECT_TYPE_BUTTON )
		return;

	m_pOwner->setBlendColour( g_UnpressedColor );
	m_pOwner->setUseMouseEvents( false );
	
	CComponentInGameTutorialObject::DeactivateTutorial();
	
	CSoundManager::GetInstance().PlayButtonClickForward();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentInGameTutorialObject::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eObjectType != OBJECT_TYPE_BUTTON )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentInGameTutorialObject::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( m_eObjectType != OBJECT_TYPE_BUTTON )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentInGameTutorialObject::OnPostInit()
{
	if( m_pOwner == NULL )
		return;

	if( strcmp( m_pOwner->getName(), g_szInGameTutorial_Image ) == 0 )
	{
		m_eObjectType = OBJECT_TYPE_IMAGE;
		CComponentInGameTutorialObject::sm_pImage = m_pOwner;
		CComponentInGameTutorialObject::sm_pImage->setLayer( LAYER_TUTORIAL_IMAGE );
		//CComponentInGameTutorialObject::sm_pImage->setPosition( g_vSafePosition );
		
		g_vOriginalSize_Image = CComponentInGameTutorialObject::sm_pImage->getSize();
		
		// Shrink the image to its smallest possible size, so that it does not appear on screen for a split second
		t2dVector vImageSize( g_vOriginalSize_Image.mX * g_fScaleMin, g_vOriginalSize_Image.mY * g_fScaleMin );
		CComponentInGameTutorialObject::sm_pImage->setSize( vImageSize );

	}
	else if( strcmp( m_pOwner->getName(), g_szInGameTutorial_Button ) == 0 )
	{
		m_eObjectType = OBJECT_TYPE_BUTTON;
		CComponentInGameTutorialObject::sm_pButton = m_pOwner;
		CComponentInGameTutorialObject::sm_pButton->setLayer( LAYER_TUTORIAL_BUTTON );
		
		g_vOriginalSize_Button = CComponentInGameTutorialObject::sm_pButton->getSize();
		
		// Shrink the button to its smallest possible size, so that it does not appear on screen for a split second
		t2dVector vButtonSize( g_vOriginalSize_Button.mX * g_fScaleMin, g_vOriginalSize_Button.mY * g_fScaleMin );
		sm_pButton->setSize( vButtonSize );
	}
	else
	{
		printf( "Unknown tutorial object: %s\n", m_pOwner->getName() );
	}
	
	if( CComponentInGameTutorialObject::sm_pImage != NULL &&
	    CComponentInGameTutorialObject::sm_pButton != NULL )
	{
		CComponentInGameTutorialObject::sm_bLevelHasTutorial = true;
	}
	
	if( g_pGrayBox == NULL )
	{
		g_pGrayBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szGrayBox ) );
		if( g_pGrayBox )
		{
			g_pGrayBox->setVisible( false );
			g_pGrayBox->setUseMouseEvents( true );
			g_pGrayBox->setLayer( LAYER_TUTORIAL_IMAGE + 1 );
		}
	}
	
	g_iBossTutorialPageNumber = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

