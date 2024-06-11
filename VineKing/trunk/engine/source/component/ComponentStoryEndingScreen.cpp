//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentStoryEndingScreen.h"

#include "ComponentAchievementBanner.h"
#include "ComponentGlobals.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "platformiPhone/GameCenterWrapper.h"
#include "T2D/t2dSceneWindow.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szLoadLevelName[64] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";
//static const char g_szLoadLevelName[64] = "game/data/levels/Screen_StoryEnding.t2d";
//static const char g_szLoadLevelName[64] = "game/data/levels/Screen_MainMenu.t2d";

// Text Lines
//static const char g_szEndingTextLine01[64] = "All Life has been regrown!";
//static const char g_szEndingTextLine02[64] = "The planet belongs to the VineKing!";
//static const char g_szEndingTextLine03[64] = "...";
//static const char g_szEndingTextLine04[64] = "...But wait!!!!";

static const char g_szText_Line01[16] = "Text_Line01";
static const char g_szText_Line02[16] = "Text_Line02";
static const char g_szText_Line03[16] = "Text_Line03";
static const char g_szText_Line04[16] = "Text_Line04";

static t2dStaticSprite* g_pText_Line01 = NULL;
static t2dStaticSprite* g_pText_Line02 = NULL;
static t2dStaticSprite* g_pText_Line03 = NULL;
static t2dStaticSprite* g_pText_Line04 = NULL;
static t2dStaticSprite* g_pCurrentFadingTextLine = NULL;

static F32 g_fCurrentAlphaTextLine01 = 0.0f;
static F32 g_fCurrentAlphaTextLine02 = 0.0f;
static F32 g_fCurrentAlphaTextLine03 = 0.0f;
static F32 g_fCurrentAlphaTextLine04 = 0.0f;

static F32 g_fTextLineAlphaTimer = 0.0f;
static const F32 g_fTextLineAlphaTimeSeconds = 1.0f;

static const t2dVector g_vTextLinePosition( 0.0f, -200.0f );

// BG
static const char g_szStoryEnding_BG[32] = "StoryEnding_BG";
static t2dStaticSprite* g_pStoryEnding_BG = NULL;

static const t2dVector g_vEndingBGStartPos( 0.0f, -222.0f );
//static const t2dVector g_vEndingBGTargetPos( 0.0f, 224.0f );

static const F32 g_fEndingBGMovementSpeed = 64.0f;

// Camera mount
static const char g_szCameraMount[16] = "CameraMount";
static t2dSceneObject* g_pCameraMount = NULL;

static t2dVector g_vCameraTargetPos( 0.0f, -445.0f ); //( 0.0f, -480.0f );

// Scene Window - For setting the camera position
static const char g_szSceneWindow[32] = "sceneWindow2D";
static t2dSceneWindow* g_pSceneWindow = NULL;


// Sun / Eclipse
static const char g_szSunObject[16] = "SunObject";
static t2dSceneObject* g_pSunObject = NULL;

static const char g_szEclipseObject[32] = "EclipseObject";
static t2dStaticSprite* g_pEclipseObject = NULL;

static const char g_szEclipseFadeBox[32] = "EclipseFadeBox";
static t2dStaticSprite* g_pEclipseFadeBox = NULL;

static const F32 g_fEclipseTimeSeconds = 3.0f;
static F32 g_fEclipseTimer = 0.0f;
static F32 g_fCurrentEclipseAlpha = 0.0f;

static t2dVector g_vEclipseStartPos;
static t2dVector g_vEclipseEndPos;

static F32 g_fEclipseMaxDistanceX = 0.0f;
static F32 g_fEclipseDistanceMovedX = 0.0f;

static const F32 g_fEclipseMovementSpeed = 16.0f;

// Credits
static const char g_szCredits_Screen_01[32] = "Credits_Screen_01";
static t2dSceneObject* g_pCredits_Screen_01 = NULL;

static const char g_szCredits_Screen_02[32] = "Credits_Screen_02";
static t2dSceneObject* g_pCredits_Screen_02 = NULL;

static const char g_szCredits_Screen_03[32] = "Credits_Screen_03";
static t2dSceneObject* g_pCredits_Screen_03 = NULL;

// Fade stuff
static const char g_szFadeBox[32] = "FadeBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 2.0f;
//static const F32 g_fFadeOutTimeSeconds = 5.0f;

// Button presses
static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static const F32 g_fEclipseWaitTimeSeconds = 1.0f; // How long to wait before moving the moon
static F32 g_fEclipseWaitTimer = 0.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryEndingScreen* CComponentStoryEndingScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentStoryEndingScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryEndingScreen::CComponentStoryEndingScreen()
	: m_pOwner( NULL )
	, m_bAchievementDisplayed( false )
	, m_eStoryState( STORY_STATE_NONE )
	, m_bEndingMusicStarted( false )
	, m_bEclipseMusicStarted( false )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentStoryEndingScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryEndingScreen::~CComponentStoryEndingScreen()
{
	CComponentStoryEndingScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryEndingScreen::StartStoryEndingScreen()
{
	// TODO:
	
	// Start fade
	
	// Show Achievement
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryEndingScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentStoryEndingScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentStoryEndingScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentStoryEndingScreen::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_bAchievementDisplayed == false )
	{
		CComponentAchievementBanner::ReportAchievement( GameCenterWrapper::VK_FAIRY_TALE_ENDING );
		m_bAchievementDisplayed = true;
	}
	
	if( m_bEndingMusicStarted == false )
	{
		CSoundManager::GetInstance().PlayStoryEndingBGM();
		m_bEndingMusicStarted = true;
	}
	
	if( m_eFadeState != FADE_STATE_NONE )
		UpdateFade();
	
	
	//static F32 g_fTextLineAlphaTimer = 0.0f;
	//static const F32 g_fTextLineAlphaTimeSeconds = 2.0f;
	
	switch( m_eStoryState )
	{
		//////////////////////////////////////////////////////////////////////////////
		// TEXT LINE 01
		case STORY_STATE_TEXT_LINE_01_FADING_IN:
			if( g_pText_Line01 )
			{
				g_fTextLineAlphaTimer += ITickable::smTickSec;
				g_fCurrentAlphaTextLine01 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine01 >= 1.0f )
				{
					g_fCurrentAlphaTextLine01 = 1.0f;
					m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_IN_FINISHED;
					m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line01->setBlendAlpha( g_fCurrentAlphaTextLine01 );
			}
			break;
			
		case STORY_STATE_TEXT_LINE_01_FADING_OUT:
			if( g_pText_Line01 )
			{
				g_fTextLineAlphaTimer -= ITickable::smTickSec;
				g_fCurrentAlphaTextLine01 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine01 <= 0.0f )
				{
					g_fCurrentAlphaTextLine01 = 0.0f;
					//m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_OUT_FINISHED;
					m_eStoryState = STORY_STATE_TEXT_LINE_02_FADING_IN;
					//m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line01->setBlendAlpha( g_fCurrentAlphaTextLine01 );
			}
			break;
			
		//////////////////////////////////////////////////////////////////////////////
		// TEXT LINE 02
		case STORY_STATE_TEXT_LINE_02_FADING_IN:
			if( g_pText_Line02 )
			{
				g_fTextLineAlphaTimer += ITickable::smTickSec;
				g_fCurrentAlphaTextLine02 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine02 >= 1.0f )
				{
					g_fCurrentAlphaTextLine02 = 1.0f;
					m_eStoryState = STORY_STATE_TEXT_LINE_02_FADING_IN_FINISHED;
					m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line02->setBlendAlpha( g_fCurrentAlphaTextLine02 );
			}
			break;
			
		case STORY_STATE_TEXT_LINE_02_FADING_OUT:
			if( g_pText_Line02 )
			{
				g_fTextLineAlphaTimer -= ITickable::smTickSec;
				g_fCurrentAlphaTextLine02 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine02 <= 0.0f )
				{
					g_fCurrentAlphaTextLine02 = 0.0f;
					//m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_OUT_FINISHED;
					m_eStoryState = STORY_STATE_TEXT_LINE_03_FADING_IN;
					//m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line02->setBlendAlpha( g_fCurrentAlphaTextLine02 );
			}
			break;
		
		//////////////////////////////////////////////////////////////////////////////
		// TEXT LINE 03
		case STORY_STATE_TEXT_LINE_03_FADING_IN:
			if( g_pText_Line03 )
			{
				g_fTextLineAlphaTimer += ITickable::smTickSec;
				g_fCurrentAlphaTextLine03 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine03 >= 1.0f )
				{
					g_fCurrentAlphaTextLine03 = 1.0f;
					m_eStoryState = STORY_STATE_TEXT_LINE_03_FADING_IN_FINISHED;
					m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line03->setBlendAlpha( g_fCurrentAlphaTextLine03 );
			}
			break;
			
		case STORY_STATE_TEXT_LINE_03_FADING_OUT:
			if( g_pText_Line03 )
			{
				g_fTextLineAlphaTimer -= ITickable::smTickSec;
				g_fCurrentAlphaTextLine03 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine03 <= 0.0f )
				{
					g_fCurrentAlphaTextLine03 = 0.0f;
					//m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_OUT_FINISHED;
					m_eStoryState = STORY_STATE_TEXT_LINE_04_FADING_IN;
					//m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line03->setBlendAlpha( g_fCurrentAlphaTextLine03 );
			}
			break;
			
		//////////////////////////////////////////////////////////////////////////////
		// TEXT LINE 04
		case STORY_STATE_TEXT_LINE_04_FADING_IN:
			if( g_pText_Line04 )
			{
				g_fTextLineAlphaTimer += ITickable::smTickSec;
				g_fCurrentAlphaTextLine04 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine04 >= 1.0f )
				{
					g_fCurrentAlphaTextLine04 = 1.0f;
					m_eStoryState = STORY_STATE_TEXT_LINE_04_FADING_IN_FINISHED;
					m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line04->setBlendAlpha( g_fCurrentAlphaTextLine04 );
			}
			break;
			
		case STORY_STATE_TEXT_LINE_04_FADING_OUT:
			if( g_pText_Line04 )
			{
				g_fTextLineAlphaTimer -= ITickable::smTickSec;
				g_fCurrentAlphaTextLine04 = g_fTextLineAlphaTimer / g_fTextLineAlphaTimeSeconds;
				if( g_fCurrentAlphaTextLine04 <= 0.0f )
				{
					g_fCurrentAlphaTextLine04 = 0.0f;
					//m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_OUT_FINISHED;
					m_eStoryState = STORY_STATE_MOVING_UPWARDS_START;
					//m_pOwner->setUseMouseEvents( true );
				}
				g_pText_Line04->setBlendAlpha( g_fCurrentAlphaTextLine04 );
			}
			
			if( m_bEclipseMusicStarted == false )
			{
				CSoundManager::GetInstance().PlayEclipseBGM();
				m_bEclipseMusicStarted = true;
			}
			
			break;

		// MOVING UPWARDS
		case STORY_STATE_MOVING_UPWARDS_START:
			if( g_pSceneWindow && g_pStoryEnding_BG )
			{
				g_pSceneWindow->mount( g_pCameraMount, t2dVector::getZero(), 0.0f, true );
				m_eStoryState = STORY_STATE_MOVING_UPWARDS;
				
				//if( m_bEclipseMusicStarted == false )
				//{
				//	CSoundManager::GetInstance().PlayEclipseBGM();
				//	m_bEclipseMusicStarted = true;
				//}
			}
			break;
			
		case STORY_STATE_MOVING_UPWARDS:
			if( g_pCameraMount )
			{
				// Move the camera mount upwards
				F32 fDistance = g_fEndingBGMovementSpeed * ITickable::smTickSec;
				t2dVector vNewPosition = g_pCameraMount->getPosition();
				vNewPosition.mY -= fDistance;
				
				if( vNewPosition.mY <= g_vCameraTargetPos.mY )
				{
					vNewPosition.mY = g_vCameraTargetPos.mY;
					m_eStoryState = STORY_STATE_ECLIPSE;
					
					g_vEclipseStartPos = g_pEclipseObject ? g_pEclipseObject->getPosition() : t2dVector::getZero();
					g_vEclipseEndPos = g_pSunObject ? g_pSunObject->getPosition() : t2dVector::getZero();
					
					g_fEclipseMaxDistanceX = mFabs( g_vEclipseStartPos.mX - g_vEclipseEndPos.mX );
					g_fEclipseDistanceMovedX = 0.0f;
				}
				
				g_pCameraMount->setPosition( vNewPosition );
			}
			break;
			
		// ECLIPSE
		case STORY_STATE_ECLIPSE:

			g_fEclipseWaitTimer += ITickable::smTickSec;
			if( g_fEclipseWaitTimer <= g_fEclipseWaitTimeSeconds )
				return;
			
			if( g_pEclipseObject )
			{
				F32 fDistance = g_fEclipseMovementSpeed * ITickable::smTickSec;
				t2dVector vNewPosition = g_pEclipseObject->getPosition();
				vNewPosition.mX += fDistance;
				g_fEclipseDistanceMovedX += fDistance;
				
				if( vNewPosition.mX >= g_vEclipseEndPos.mX )
				{
					if( g_pFadeBox )
						g_pFadeBox->setPosition( g_pEclipseFadeBox ? g_pEclipseFadeBox->getPosition() : t2dVector::getZero() );
					
					vNewPosition.mX = g_vEclipseEndPos.mX;
					m_eStoryState = STORY_STATE_ECLIPSE_FINISHED;				
					m_eFadeState = FADE_STATE_FADING_OUT;
					
					CSoundManager::GetInstance().PlayCreditsBGM();
				}
				
				if( g_pEclipseFadeBox )
				{
					F32 fPercent = g_fEclipseDistanceMovedX / g_fEclipseMaxDistanceX;
					if( fPercent >= 1.0f )
						fPercent = 1.0f;	
					g_pEclipseFadeBox->setBlendAlpha( fPercent );
				}
				
				g_pEclipseObject->setPosition( vNewPosition );
			}
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	m_pOwner->setUseMouseEvents( false );
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	switch( m_eStoryState )
	{
		////////////////////////////////////////////////////////////////
		case STORY_STATE_TEXT_LINE_01_FADING_IN_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_OUT;
			break;
			
		case STORY_STATE_TEXT_LINE_01_FADING_OUT_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_02_FADING_IN;
			break;
			
		////////////////////////////////////////////////////////////////	
		case STORY_STATE_TEXT_LINE_02_FADING_IN_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_02_FADING_OUT;
			break;
			
		case STORY_STATE_TEXT_LINE_02_FADING_OUT_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_03_FADING_IN;
			break;
			
		////////////////////////////////////////////////////////////////
		case STORY_STATE_TEXT_LINE_03_FADING_IN_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_03_FADING_OUT;
			break;
			
		case STORY_STATE_TEXT_LINE_03_FADING_OUT_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_04_FADING_IN;
			break;
			
		////////////////////////////////////////////////////////////////
		case STORY_STATE_TEXT_LINE_04_FADING_IN_FINISHED:
			m_eStoryState = STORY_STATE_TEXT_LINE_04_FADING_OUT;
			break;
			
		case STORY_STATE_TEXT_LINE_04_FADING_OUT_FINISHED:
			m_eStoryState = STORY_STATE_MOVING_UPWARDS;
			break;
			
		////////////////////////////////////////////////////////////////
			
		case STORY_STATE_FINISHED:
		case STORY_STATE_CREDITS_1:
		case STORY_STATE_CREDITS_2:
		case STORY_STATE_CREDITS_3:
		case STORY_STATE_CREDITS_FINISHED:
			
			m_eFadeState = FADE_STATE_FADING_OUT;
			m_fFadeTimer = 0.0f;
			m_fFadeAmount = 0.0f;
			break;
			
		////////////////////////////////////////////////////////////////
			
		default:
			break;
	}
	
	//CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szLoadLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner )
		m_pOwner->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::OnPostInit()
{
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( false );
	
	// Text Lines
	g_pText_Line01 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szText_Line01 ) );
	if( g_pText_Line01 )
	{
		g_pText_Line01->setBlendAlpha( 0.0f );
		g_pText_Line01->setPosition( g_vTextLinePosition );
	}
	
	g_pText_Line02 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szText_Line02 ) );
	if( g_pText_Line02 )
	{
		g_pText_Line02->setBlendAlpha( 0.0f );
		g_pText_Line02->setPosition( g_vTextLinePosition );
	}
	
	g_pText_Line03 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szText_Line03 ) );
	if( g_pText_Line03 )
	{
		g_pText_Line03->setBlendAlpha( 0.0f );
		g_pText_Line03->setPosition( g_vTextLinePosition );
	}
	
	g_pText_Line04 = static_cast<t2dStaticSprite*>( Sim::findObject( g_szText_Line04 ) );
	if( g_pText_Line04 )
	{
		g_pText_Line04->setBlendAlpha( 0.0f );
		g_pText_Line04->setPosition( g_vTextLinePosition );
	}
	
	// BG
	g_pStoryEnding_BG = static_cast<t2dStaticSprite*>( Sim::findObject( g_szStoryEnding_BG ) );
	if( g_pStoryEnding_BG )
		g_pStoryEnding_BG->setPosition( g_vEndingBGStartPos );
	
	// Sun
	g_pSunObject = static_cast<t2dStaticSprite*>( Sim::findObject( g_szSunObject ) );
	
	// Eclipse
	g_fEclipseTimer = 0.0f;
	g_fCurrentEclipseAlpha = 0.0f;
	
	g_pEclipseObject = static_cast<t2dStaticSprite*>( Sim::findObject( g_szEclipseObject ) );
	
	// Eclipse FadeBox
	g_pEclipseFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szEclipseFadeBox ) );
	if( g_pEclipseFadeBox )
	{
		g_pEclipseFadeBox->setBlendAlpha( 0.0f );
		//g_pEclipseFadeBox->setPosition( g_vCameraTargetPos );
	}
	
	// Camera Mount
	g_pCameraMount = static_cast<t2dSceneObject*>( Sim::findObject( g_szCameraMount ) );
	if( g_pCameraMount )
		g_pCameraMount->setPosition( t2dVector::getZero() );
	
	g_pSceneWindow = static_cast<t2dSceneWindow*>( Sim::findObject( g_szSceneWindow ) );
	
	// Fade Box
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
	if( g_pFadeBox )
	{
		g_pFadeBox->setPosition( t2dVector::getZero() );
		g_pFadeBox->setBlendAlpha( 1.0f );
	}
	
	m_eStoryState = STORY_STATE_FADING_IN;
	
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds;
	m_fFadeAmount = 1.0f;
	
	
	g_pCredits_Screen_01 = static_cast<t2dSceneObject*>( Sim::findObject( g_szCredits_Screen_01 ) );
	if( g_pCredits_Screen_01 )
	{
		g_pCredits_Screen_01->setPosition( t2dVector::getZero() );
		g_pCredits_Screen_01->setVisible( false );
	}
	
	g_pCredits_Screen_02 = static_cast<t2dSceneObject*>( Sim::findObject( g_szCredits_Screen_02 ) );
	if( g_pCredits_Screen_02 )
	{
		g_pCredits_Screen_02->setPosition( t2dVector::getZero() );
		g_pCredits_Screen_02->setVisible( false );
	}
	
	g_pCredits_Screen_03 = static_cast<t2dSceneObject*>( Sim::findObject( g_szCredits_Screen_03 ) );
	if( g_pCredits_Screen_03 )
	{
		g_pCredits_Screen_03->setPosition( t2dVector::getZero() );
		g_pCredits_Screen_03->setVisible( false );
	}
	
	g_fEclipseWaitTimer = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::UpdateCurrentTextLineAlphaFade()
{
	if( g_pCurrentFadingTextLine == NULL )
		return;
	
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private member functions

void CComponentStoryEndingScreen::UpdateFade()
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

void CComponentStoryEndingScreen::OnFadeOutFinished()
{
	//printf( "CComponentStoryOpeningScreen::OnFadeOutFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = g_fFadeTimeSeconds; // Set to g_fFadeTimeSeconds because the Fade Box alpha will go from 1.0 -> 0.0
	
	if( g_pFadeBox ) 
		g_pFadeBox->setBlendAlpha( 1.0f );
	
	
	switch( m_eStoryState )
	{
		//case STORY_STATE_FINISHED:
		case STORY_STATE_ECLIPSE_FINISHED:
			
			//CSoundManager::GetInstance().PlayCreditsBGM();
			if( g_pCredits_Screen_01 )
			{
				if( g_pFadeBox )
					g_pFadeBox->setPosition( t2dVector::getZero() );
					
				g_pCredits_Screen_01->setVisible( true );
				g_pSceneWindow->mount( g_pCredits_Screen_01, t2dVector::getZero(), 0.0f, true );
			}
			
			m_eStoryState = STORY_STATE_CREDITS_1;
			m_eFadeState = FADE_STATE_FADING_IN;
			break;
			
		case STORY_STATE_CREDITS_1:
			
			if( g_pCredits_Screen_01 )
				g_pCredits_Screen_01->setVisible( false );
			
			if( g_pCredits_Screen_02 )
			{
				g_pCredits_Screen_02->setVisible( true );
				//g_pSceneWindow->mount( g_pCredits_Screen_02, t2dVector::getZero(), 0.0f, true );
			}
			
			m_eStoryState = STORY_STATE_CREDITS_2;
			m_eFadeState = FADE_STATE_FADING_IN;
			break;
			
		case STORY_STATE_CREDITS_2:
			//CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szLoadLevelName );	
			if( g_pCredits_Screen_02 )
				g_pCredits_Screen_02->setVisible( false );
			
			if( g_pCredits_Screen_03 )
			{
				g_pCredits_Screen_03->setVisible( true );
				//g_pSceneWindow->mount( g_pCredits_Screen_03, t2dVector::getZero(), 0.0f, true );
			}
			
			m_eStoryState = STORY_STATE_CREDITS_3;
			m_eFadeState = FADE_STATE_FADING_IN;
			break;
			
		case STORY_STATE_CREDITS_3:
			CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szLoadLevelName );
			//m_eStoryState = STORY_STATE_CREDITS_FINISHED;
			//m_eFadeState = FADE_STATE_FADING_IN;
			break;
			
		case STORY_STATE_CREDITS_FINISHED:
			CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szLoadLevelName );
			break;
			
		default:
			break;
	}
	
	//CComponentGlobals::GetInstance().ScheduleLoadLevel( g_szLoadLevelName );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryEndingScreen::OnFadeInFinished()
{
	//printf( "CComponentStoryOpeningScreen::OnFadeInFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;  // Set to 0.0f because the Fade Box alpha will go from 0.0f -> 1.0 on Fade Out
	
	if( g_pFadeBox )
		g_pFadeBox->setBlendAlpha( 0.0f );
	
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( true );
	
	
	switch( m_eStoryState )
	{
		case STORY_STATE_FADING_IN:
			m_eStoryState = STORY_STATE_TEXT_LINE_01_FADING_IN;
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------