//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEndLevelScreen.h"

#include "ComponentAchievementBanner.h"
#include "ComponentDoubleBrick.h"
#include "ComponentEnemySpawnController.h"
#include "ComponentGlobals.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentScoringManager.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

#include "core/iTickable.h"

#include "platformiPhone/GameCenterWrapper.h"

#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dStaticSprite.h"



//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szPlayerOutroObject[32] = "PlayerOutroObject";
static t2dAnimatedSprite* g_pPlayerOutroObject = NULL;

static const char g_szPlayerOutroAnim[64] = "vine_king_finishing_moveAnimation";

//static const char g_szPlayerOutroSmoke[32] = "PlayerIntroSmoke";
//static t2dAnimatedSprite* g_pPlayerOutroSmoke = NULL;

static const F32 g_fLevelEndCameraShakeMagnitude	= 10.0f;
static const F32 g_fLevelEndCameraShakeTime			= 1.0f; // In seconds

static const t2dVector g_vSmokeSafePos( 500.0f, 500.0f );
static t2dVector g_vSmokePos;

static const U32 g_uOutroLandAnimFrame = 25;


// Outro collision
static const char g_szPlayerOutroCollision[32] = "PlayerOutroCollision";
static t2dSceneObject* g_pPlayerOutroCollision = NULL;


static const F32 g_fOutroCollisionStartSize = 32.0f;
static const F32 g_fOutroCollisionMaxSize	= 750.0f;
static const F32 g_fOutroCollisionGrowSpeed = 400.0f; // Units per second

static F32 g_fOutroCurrentCollisionSize = g_fOutroCollisionStartSize;

static const t2dVector g_vPlayerOutroCollisionStartSize( g_fOutroCollisionStartSize, g_fOutroCollisionStartSize );

static const F32 g_fPostLandingCompletionTime = 3.0f; // Amount of time that elapses before the "You Win" screen appears.



// End Level Message variables
static const char g_szEndLevelMessage[32] = "EndLevelMessage";
static const char g_szEndLevelMessageImageMap[32] = "win_loseImageMap";
static const S32 g_iEndLevelMessageFrameWin = 0;
static const S32 g_iEndLevelMessageFrameLose = 1;

//static const t2dVector g_vEndMessageSizeStart( t2dVector::getZero() );
//static const t2dVector g_vEndMessageSizeMax( 360.0f, 90.0f ); // Note: SizeX = SizeY * 4.0
//static const t2dVector g_vEndMessageSizeMax( 256.0f, 64.0f ); // Note: SizeX = SizeY * 4.0


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scaling and Rotation for End Message (You Win / Game Over)
static F32 g_fRotationCurrent = 0.0f;

static const F32 g_fScaleMin = 0.0f;
static const F32 g_fScaleMax = 1.0f;

static F32 g_fScaleCurrent = g_fScaleMin;

static const F32 g_fEndMessageScaleRate = 1.5f;		// Units of scaling per second
static const F32 g_iEndMessageNumRotations = 2;		// The number of rotations

static const F32 g_fTotalScaleTime = (g_fScaleMax - g_fScaleMin) / g_fEndMessageScaleRate; // TOTAL_SCALE_TIME = SCALE_DISTANCE / SCALE_RATE
static const F32 g_fTotalDegreesToRotate = 360.0f * (F32)g_iEndMessageNumRotations;
static const F32 g_fEndMessageRotationRate = g_fTotalDegreesToRotate / g_fTotalScaleTime; // ROTATION_RATE = ROTATION_DISTANCE / TIME ("Distance" means # of degrees to rotate)

static t2dVector g_vTempEndMessageSize;
static t2dVector g_vOriginalEndMessageSize; // The original (max) size of the end message object

static const F32 g_fFadeOutDelaySeconds = 4.0f; // The amount of time between You Win/Game Over is fully displayed and the fade out start

// Fade
static const char g_szBlackBox[32] = "BlackBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 0.5f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEndLevelScreen* CComponentEndLevelScreen::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEndLevelScreen );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEndLevelScreen::CComponentEndLevelScreen()
	: m_pOwner( NULL )
	, m_pYouWinTextObject( NULL )
	, m_pGameOverTextObject( NULL )
	, m_pOKButton( NULL )
	, m_bLevelHasEnded( false )
	, m_bLevelWon( false )
	, m_bSmokePlayed( false )
	, m_fPostLandTimer( 0.0f )
	, m_bEndScreenShown( false )
	, m_bDisplayEndLevelMessage( false )
	, m_pEndLevelMessage( NULL )
	//, m_fCurrentEndLevelMessageRotation( 0.0f )
	//, m_vCurrentEndLevelMessageSize( g_vEndMessageSizeStart )
	//, m_fCurrentEndLevelMessageSizeX( g_vEndMessageSizeStart.mX )
	//, m_fCurrentEndLevelMessageSizeY( g_vEndMessageSizeStart.mY )
	, m_bRotationStopped( false )
	, m_bFinalGrowSoundPlayed( false )
	, m_bStartFadeOut( false )
	, m_fFadeDelayTimer( 0.0f )
	, m_eFadeState( FADE_STATE_NONE )
	, m_fFadeTimer( 0.0f )
	, m_fFadeAmount( 0.0f )
{
	CComponentEndLevelScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEndLevelScreen::~CComponentEndLevelScreen()
{
	CComponentEndLevelScreen::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::ShowYouWinScreen()
{
	m_bLevelHasEnded = true;
	
	CSoundManager::GetInstance().PlayYouWinBGM();
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseOfLevelWin();
	
	CComponentGlobals::GetInstance().SetLevelComplete();
	
	//if( m_pOwner )
	//	m_pOwner->setVisible( true );
	
	//if( m_pYouWinTextObject )
	//	m_pYouWinTextObject->setVisible( false );
	
	//if( m_pGameOverTextObject )
	//	m_pGameOverTextObject->setVisible( false );
	
	//if( m_pOKButton )
	//	m_pOKButton->setVisible( true );
	
	CComponentEndLevelScreen::StartFadeOut();
	
	m_bLevelWon = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::ShowGameOverScreen()
{
	m_bLevelHasEnded = true;
	
	CSoundManager::GetInstance().PlayYouLoseBGM();
	
	//if( m_pOwner )
	//	m_pOwner->setVisible( true );
	
	//if( m_pYouWinTextObject )
	//	m_pYouWinTextObject->setVisible( false );
	
	//if( m_pGameOverTextObject )
	//	m_pGameOverTextObject->setVisible( false );
	
	//if( m_pOKButton )
	//	m_pOKButton->setVisible( true );
	
	CComponentEndLevelScreen::StartFadeOut();
	
	m_bLevelWon = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::HideAllElements()
{	
	if( m_pOwner )
		m_pOwner->setVisible( false );
	
	if( m_pYouWinTextObject) 
		m_pYouWinTextObject->setVisible( false );
	
	if( m_pGameOverTextObject )
		m_pGameOverTextObject->setVisible( false );
	
	if( m_pOKButton )
		m_pOKButton->setVisible( false );
	
	if( m_pYouWinTextObject )
		m_pYouWinTextObject->setVisible( false );
	
	if( m_pEndLevelMessage )
		m_pEndLevelMessage->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::OnOKButtonPressed()
{
	if( m_bLevelWon )
	{
		HideAllElements();
		CComponentScoringManager::GetInstance().OpenScoreScreen();
		
		DisplayLevelCompleteAchievement();
	}
	else 
	{
		HideAllElements();
		char szLevelName[128] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";
		CComponentGlobals::GetInstance().ScheduleLoadLevel( szLevelName );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::StartLevelWinSequence()
{
	if( g_pPlayerOutroObject == NULL )
	{
		ShowYouWinScreen();
		return;
	}
	
	m_bLevelHasEnded = true;
	m_bLevelWon = true;
	
	CSoundManager::GetInstance().PlayWinGruntSound();
	
	// Calculate the position of the player outro object - Line up the bottom positions
	t2dVector vPosition;
	CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( vPosition );
	vPosition.mY -= (g_pPlayerOutroObject->getSize().mY * 0.5f);
	g_pPlayerOutroObject->setPosition( vPosition );
	
	if( CComponentPlayerHomeBase::GetInstance().IsHomeBaseFacingRight() )
		g_pPlayerOutroObject->setFlipX( true );
	
	g_pPlayerOutroObject->setLayer( LAYER_INTRO_OUTRO_OBJECTS );
	g_pPlayerOutroObject->setVisible( true );
	g_pPlayerOutroObject->playAnimation( g_szPlayerOutroAnim, false );
	CComponentPlayerHomeBase::GetInstance().HideHomeBase();
	
	CComponentPathGridHandler::GetInstance().EnableAllLandBrickCollisions();
	
	if( g_pPlayerOutroCollision )
	{
		g_pPlayerOutroCollision->setVisible( true );
		t2dVector vPosition;
		CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( vPosition );
		g_pPlayerOutroCollision->setPosition( vPosition );
		//g_pPlayerOutroCollision->setCollisionActive( true, false );
		g_pPlayerOutroCollision->setUsesPhysics( true );
		g_pPlayerOutroCollision->setSize( g_vPlayerOutroCollisionStartSize );
	}
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseOfLevelWinStart();
	//CComponentDoubleBrick::DisableCollisionOnAllDoubleBricks();
	CComponentLineDrawAttack::KillAllLineDrawEffects();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::StartLevelLoseSequence()
{
	m_bLevelHasEnded = true;
	m_bLevelWon = false;
	
	CComponentPathGridHandler::GetInstance().EnableAllLandBrickCollisions();
	
	if( g_pPlayerOutroCollision )
	{
		g_pPlayerOutroCollision->setVisible( true );
		t2dVector vPosition;
		CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( vPosition );
		g_pPlayerOutroCollision->setPosition( vPosition );
		g_pPlayerOutroCollision->setCollisionActive( true, false );
		g_pPlayerOutroCollision->setUsesPhysics( true );
		g_pPlayerOutroCollision->setSize( g_vPlayerOutroCollisionStartSize );
	}
	
	CComponentDoubleBrick::DisableCollisionOnAllDoubleBricks();
	CComponentLineDrawAttack::KillAllLineDrawEffects();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::StartEndLevelTextSequence()
{
	if( m_pEndLevelMessage == NULL )
		return;
	
	t2dStaticSprite* pMessage = static_cast<t2dStaticSprite*>( m_pEndLevelMessage );
	if( pMessage )
	{
		if( m_bLevelWon )
			pMessage->setImageMap( g_szEndLevelMessageImageMap, g_iEndLevelMessageFrameWin );
		else
			pMessage->setImageMap( g_szEndLevelMessageImageMap, g_iEndLevelMessageFrameLose );
	}
	
	g_vOriginalEndMessageSize = m_pEndLevelMessage->getSize();
	g_fScaleCurrent = g_fScaleMin;
	g_fRotationCurrent = 0.0f;
	t2dVector vNewSize = g_vOriginalEndMessageSize;
	vNewSize.mX *= g_fScaleCurrent;
	vNewSize.mY *= g_fScaleCurrent;
	m_pEndLevelMessage->setSize( vNewSize );
	
	m_pEndLevelMessage->setPosition( t2dVector::getZero() );
	m_pEndLevelMessage->setVisible( true );
	
	m_bDisplayEndLevelMessage = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::UpdateEndLevelText()
{
	if( m_pEndLevelMessage == NULL )
		return;
	
	if( m_bRotationStopped )
		return;
	
	// Handle Rotation
	g_fRotationCurrent += g_fEndMessageRotationRate * ITickable::smTickSec;
	if( g_fRotationCurrent >= 360.0f )
		g_fRotationCurrent = 0.0f;
	
	m_pEndLevelMessage->setRotation( g_fRotationCurrent );
	
	// Handle Scaling
	g_fScaleCurrent += g_fEndMessageScaleRate * ITickable::smTickSec;
	if( g_fScaleCurrent >= g_fScaleMax )
	{
		g_fScaleCurrent = g_fScaleMax;
		
		// AT MAX SCALE -- DO SOMETHING HERE!
		//printf( "AT MAX SCALE - DO SOMETHING!\n" );
		m_bRotationStopped = true;
		
		m_pEndLevelMessage->setRotation( 0.0f );
		m_bRotationStopped = true;
		
		if( m_bLevelWon )
			OnLevelWon();
		else
			OnLevelLost();
	}
	
	g_vTempEndMessageSize = g_vOriginalEndMessageSize;
	g_vTempEndMessageSize.mX *= g_fScaleCurrent;
	g_vTempEndMessageSize.mY *= g_fScaleCurrent;
	m_pEndLevelMessage->setSize( g_vTempEndMessageSize );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::DisplayLevelCompleteAchievement()
{
	S32 iActiveSaveLevelIndex = CComponentGlobals::GetInstance().GetActiveLevelSaveIndex();
	
	S32 iAchievement = GameCenterWrapper::INVALID_ACHIEVEMENT_ID;
	
	switch( iActiveSaveLevelIndex )
	{
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_1_1; break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_2_1; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_3_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_3_2; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_4_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_4_2; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_5_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_5_2; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_5_3; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_6_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_6_2; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_6_3; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_7_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_7_2; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_7_3; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_7_4; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_8_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_8_2; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_8_3; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_8_4; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_9_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_9_2; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_9_3; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_9_4; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_10_1; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_10_2; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_10_3; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_10_4; break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01: iAchievement = GameCenterWrapper::VK_MAGMA_MASHER; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02: iAchievement = GameCenterWrapper::VK_MAGMA_BANE;	break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03: iAchievement = GameCenterWrapper::VK_MAGMA_MASTER; break;
			
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_01; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_02; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_03; break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04: iAchievement = GameCenterWrapper::VK_LEVEL_COMPLETE_MOM_04; break;
			
		default:
			printf( "CComponentEndLevelScreen::DisplayLevelCompleteAchievement - Unhandled Save Level Index: %d\n", iActiveSaveLevelIndex );
			break;
	}
	
	if( iAchievement != GameCenterWrapper::INVALID_ACHIEVEMENT_ID )
		CComponentAchievementBanner::ReportAchievement( iAchievement );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::OnLevelWon()
{
	m_bLevelHasEnded = true;
	
	CSoundManager::GetInstance().PlayYouWinBGM();
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseOfLevelWin();
	
	CComponentGlobals::GetInstance().SetLevelComplete();
	
	CComponentEndLevelScreen::StartFadeOut();
	
	m_bLevelWon = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::OnLevelLost()
{
	m_bLevelHasEnded = true;
	
	CSoundManager::GetInstance().PlayYouLoseBGM();
	
	CComponentEndLevelScreen::StartFadeOut();
	
	m_bLevelWon = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::StartFadeOut()
{
	if( sm_pInstance == NULL )
		return;
	
	m_bStartFadeOut = true;
	m_eFadeState = FADE_STATE_FADING_OUT;
	m_fFadeTimer = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEndLevelScreen::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentEndLevelScreen ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEndLevelScreen::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentEndLevelScreen::sm_pInstance = this;
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::onUpdate()
{
	if( m_bLevelHasEnded == false )
		return;
	
	if( g_pPlayerOutroObject == NULL )
		return;
	
	if( m_bDisplayEndLevelMessage )
	{
		UpdateEndLevelText();
	}
	
	if( m_bEndScreenShown )
	{
		if( m_eFadeState != FADE_STATE_NONE )
		{
			m_fFadeDelayTimer += ITickable::smTickSec;
			if( m_fFadeDelayTimer >= g_fFadeOutDelaySeconds )
			{
				UpdateFade();
			}
		}
		
		return;
	}
	
	if( m_bLevelWon )
	{
		if( m_bSmokePlayed == false )
		{
			if( g_pPlayerOutroObject->mAnimationController.getCurrentFrame() == g_uOutroLandAnimFrame )
			{
				CSoundManager::GetInstance().PlayThudSound();
				
				CComponentGlobals::GetInstance().StartCameraShake( g_fLevelEndCameraShakeMagnitude, g_fLevelEndCameraShakeTime );
				if( g_pPlayerOutroCollision )
				{
					g_pPlayerOutroCollision->setSize( g_vPlayerOutroCollisionStartSize );
					g_pPlayerOutroCollision->setCollisionActive( true, false );
				}
				
				//if( g_pPlayerOutroSmoke )
				//{
				//	//g_pPlayerOutroSmoke->setPosition( g_vSmokePos );
				//	g_pPlayerOutroSmoke->setVisible( true );
				//}
				
				m_bSmokePlayed = true;
				
				// The stomp has occurred. Kill all active enemies.
				CComponentEnemySpawnController::KillAllActiveEnemiesFromStomp();
			}
		}
		else
		{
			if( g_pPlayerOutroObject->getVisible() && g_pPlayerOutroObject->mAnimationController.isAnimationFinished() )
			{
				g_pPlayerOutroObject->setVisible( false );
				CComponentPlayerHomeBase::GetInstance().UnhideHomeBase();
			}
			
			if( g_pPlayerOutroCollision )
			{
				F32 fGrowAmount = g_fOutroCollisionGrowSpeed * ITickable::smTickSec;
				g_fOutroCurrentCollisionSize += fGrowAmount;
				if( g_fOutroCurrentCollisionSize >= g_fOutroCollisionMaxSize )
					g_fOutroCurrentCollisionSize = g_fOutroCollisionMaxSize;
				
				t2dVector vSize( g_fOutroCurrentCollisionSize, g_fOutroCurrentCollisionSize );
				g_pPlayerOutroCollision->setSize( vSize );
			}
			
			m_fPostLandTimer += ITickable::smTickSec;
			if( m_fPostLandTimer >= g_fPostLandingCompletionTime )
			{
                /////////////////////////////////////////////////////////////////////////////
                if( CComponentTutorialLevel::IsTutorialLevel() == true && CComponentTutorialLevel::CanLevelEnd() == false ) 
                {
                    // This is the tutorial level. Reset the outro collision stuff/ Check the tutorial
                    m_bLevelWon = false;
                    m_bLevelHasEnded = false;
                    m_fPostLandTimer = 0.0;
                    m_bSmokePlayed = false;
                    if( g_pPlayerOutroCollision )
                    {
                        g_fOutroCurrentCollisionSize = g_fOutroCollisionStartSize;
                        t2dVector vSize( g_fOutroCurrentCollisionSize, g_fOutroCurrentCollisionSize );
                        g_pPlayerOutroCollision->setSize( vSize );
                        g_pPlayerOutroCollision->setUsesPhysics( false );
                        g_pPlayerOutroCollision->setCollisionActive( false, false );
                    }
                    
                    CComponentTutorialLevel::FadeToNextTutorialState();
                }
                /////////////////////////////////////////////////////////////////////////////
                else 
                {
                    StartEndLevelTextSequence();
                    m_bEndScreenShown = true;
                    
                    CComponentTutorialLevel::OnLevelEnded();
                }
			}
		}
	}
	else
	{
		if( g_pPlayerOutroCollision )
		{
			F32 fGrowAmount = g_fOutroCollisionGrowSpeed * ITickable::smTickSec;
			g_fOutroCurrentCollisionSize += fGrowAmount;
			if( g_fOutroCurrentCollisionSize >= g_fOutroCollisionMaxSize )
				g_fOutroCurrentCollisionSize = g_fOutroCollisionMaxSize;
			
			t2dVector vSize( g_fOutroCurrentCollisionSize, g_fOutroCurrentCollisionSize );
			g_pPlayerOutroCollision->setSize( vSize );
		}
		
		m_fPostLandTimer += ITickable::smTickSec;
		if( m_fPostLandTimer >= g_fPostLandingCompletionTime )
		{
            /////////////////////////////////////////////////////////////////////////////
            if( CComponentTutorialLevel::IsTutorialLevel() == true && CComponentTutorialLevel::CanLevelEnd() == false ) 
            {
                // This is the tutorial level. Reset the outro collision stuff/ Check the tutorial
                m_bLevelWon = false;
                m_bLevelHasEnded = false;
                m_bSmokePlayed = false;
                m_fPostLandTimer = 0.0;
                if( g_pPlayerOutroCollision )
                {
                    g_fOutroCurrentCollisionSize = g_fOutroCollisionStartSize;
                    t2dVector vSize( g_fOutroCurrentCollisionSize, g_fOutroCurrentCollisionSize );
                    g_pPlayerOutroCollision->setSize( vSize );
                    g_pPlayerOutroCollision->setUsesPhysics( false );
                    g_pPlayerOutroCollision->setCollisionActive( false, false );
                }
                CComponentTutorialLevel::FadeToNextTutorialState();
            }
            /////////////////////////////////////////////////////////////////////////////
            else
            {
                StartEndLevelTextSequence();
                m_bEndScreenShown = true;
                
                CComponentTutorialLevel::OnLevelEnded();
            }
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	// Position the owner at the center
	m_pOwner->setPosition( t2dVector::getZero() );
	
	m_pOwner->setVisible( false );
	
	if( m_pYouWinTextObject )
		m_pYouWinTextObject->setVisible( false );
	
	if( m_pGameOverTextObject )
		m_pGameOverTextObject->setVisible( false );
	
	if( m_pOKButton )
		m_pOKButton->setVisible( false );
	
	g_pPlayerOutroObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szPlayerOutroObject ) );
	if( g_pPlayerOutroObject )
	{
//		// Calculate the position of the player outro object
//		t2dVector vPosition;
//		CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( vPosition );
//
//		vPosition.mY -= (g_pPlayerOutroObject->getSize().mY * 0.5f);
//		g_pPlayerOutroObject->setPosition( vPosition );
	
		g_pPlayerOutroObject->setVisible( false );
	}
	
	g_pPlayerOutroCollision = static_cast<t2dSceneObject*>( Sim::findObject( g_szPlayerOutroCollision ) );
	if( g_pPlayerOutroCollision )
	{
		t2dVector vPosition;
		CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( vPosition );
		g_pPlayerOutroCollision->setPosition( vPosition );
		g_pPlayerOutroCollision->setVisible( true );
		
		g_pPlayerOutroCollision->setCollisionCallback( false );
		g_pPlayerOutroCollision->setNeverSolvePhysics( true ); // TRUE = Object will not react or move from a collision.
	}
	
	//g_pPlayerOutroSmoke = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szPlayerOutroSmoke ) );
	//if( g_pPlayerOutroSmoke )
	//{
	//	//g_pPlayerOutroSmoke->setVisible( false );
	//	//g_pPlayerOutroSmoke->setLayer( LAYER_INTRO_OUTRO_SMOKE );
	//	CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( g_vSmokePos );
	//}
	
	g_fOutroCurrentCollisionSize = g_fOutroCollisionStartSize;
	
	
	// Test for End Level Message
	m_pEndLevelMessage = static_cast<t2dSceneObject*>( Sim::findObject( g_szEndLevelMessage ) );
	if( m_pEndLevelMessage )
	{
		m_pEndLevelMessage->setVisible( false );
		m_pEndLevelMessage->setLayer( LAYER_YOU_WIN_GAME_OVER_DISPLAY );
	}
	//m_fCurrentEndLevelMessageRotation = 0.0f;
	m_bRotationStopped = false;
	
	g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szBlackBox ) );
	if( g_pFadeBox )
	{
		//g_pFadeBox->setPosition( t2dVector::getZero() );
		m_eFadeState = FADE_STATE_NONE;
		m_fFadeTimer = 0.0f;
	}
	
	g_fScaleCurrent = g_fScaleMin;
	g_fRotationCurrent = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private member functions

void CComponentEndLevelScreen::UpdateFade()
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

void CComponentEndLevelScreen::OnFadeOutFinished()
{
	if( m_bLevelWon )
	{
		HideAllElements();
		CComponentScoringManager::GetInstance().OpenScoreScreen();
		
		DisplayLevelCompleteAchievement();
	}
	else 
	{
		HideAllElements();
		char szLevelName[128] = "game/data/levels/Screen_MapSelect_PowerUp.t2d";
		CComponentGlobals::GetInstance().ScheduleLoadLevel( szLevelName );
	}
	
	m_eFadeState = FADE_STATE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEndLevelScreen::OnFadeInFinished()
{
	m_eFadeState = FADE_STATE_NONE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------