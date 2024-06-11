//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLevelBeginSequence.h"

#include "SoundManager.h"

#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentTutorialLevel.h"

#include "core/iTickable.h"
#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const F32 g_fNextItemTimeMS = 2000.0f; // Time between "Ready", "Set", and "Go!"


static const char g_szIntroObjectName[32] = "PlayerIntroObject";
static t2dAnimatedSprite* g_pPlayerIntroObject = NULL;
static const char g_szIntroObjectAnim[32] = "vine_king_fall_introAnimation";
static bool g_bIntroAnimPlayed = false;

static const char g_szPlayerIntroSmoke[32] = "PlayerIntroSmoke";
static t2dAnimatedSprite* g_pPlayerIntroSmoke = NULL;

static const F32 g_fLevelBeginCameraShakeMagnitude	= 10.0f;
static const F32 g_fLevelBeginCameraShakeTime		= 0.5f; // In seconds


static const t2dVector g_vSmokeSafePos( 500.0f, 500.0f );
static const F32 g_fActiveSmokeTime = 2.0f; // How long the smoke stays active after the player lands

static const F32 g_fPostLandingTime = 1.5f; // How long until the game actually starts after the player lands

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelBeginSequence* CComponentLevelBeginSequence::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLevelBeginSequence );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelBeginSequence::CComponentLevelBeginSequence()
	: m_pOwner( NULL )
	, m_fTimer( 0.0f )
	, m_eLevelBeginState( LEVEL_BEGIN_STATE_NONE )
	, m_bLevelBeginSequenceFinished( false )
	, m_bSmokeActive( false )
	, m_fSmokeTimer( 0.0f )
	, m_fPostLandingTimer( 0.0f )
{
	CComponentLevelBeginSequence::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelBeginSequence::~CComponentLevelBeginSequence()
{
	CComponentLevelBeginSequence::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLevelBeginSequence::InitializeBeginSequenceObjects()
{
	if( sm_pInstance == NULL )
		return;
	
	// Calculate the position of the player intro object
	t2dVector vPosition;
	CComponentPlayerHomeBase::GetInstance().GetHomeBaseBottomPosition( vPosition );
	
	if( g_pPlayerIntroSmoke )
	{
		g_pPlayerIntroSmoke->setPosition( vPosition );
		g_pPlayerIntroSmoke->setVisible( false );
		g_pPlayerIntroSmoke->setLayer( LAYER_INTRO_OUTRO_SMOKE );
	}
	
	if( g_pPlayerIntroObject )
	{
		vPosition.mY -= (g_pPlayerIntroObject->getSize().mY * 0.5f);
		g_pPlayerIntroObject->setPosition( vPosition );
		
		g_pPlayerIntroObject->setLayer( LAYER_INTRO_OUTRO_OBJECTS );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLevelBeginSequence::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentLevelBeginSequence ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLevelBeginSequence::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentLevelBeginSequence::sm_pInstance = this;
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelBeginSequence::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	// Handle the Intro animation
	if( m_eLevelBeginState == LEVEL_BEGIN_STATE_READY )
	{
		if( m_bLevelBeginSequenceFinished == false && g_pPlayerIntroObject )
		{
			if( g_bIntroAnimPlayed == false )
			{
				g_pPlayerIntroObject->setVisible( true );
				g_pPlayerIntroObject->playAnimation( g_szIntroObjectAnim, false );
				g_bIntroAnimPlayed = true;
				
				CSoundManager::GetInstance().PlayIntroFallSound();
			}
			else 
			{
				if( g_pPlayerIntroObject->mAnimationController.isAnimationFinished() == true )
				{
					//CSoundManager::GetInstance().PlayInGameBGM();
					m_eLevelBeginState = LEVEL_BEGIN_STATE_GO;
					g_pPlayerIntroObject->setVisible( false );
					CComponentPlayerHomeBase::GetInstance().UnhideHomeBase();
					
					CSoundManager::GetInstance().PlayThudSound();
					
					if( g_pPlayerIntroSmoke )
					{
						g_pPlayerIntroSmoke->setVisible( true );
						m_bSmokeActive = true;
					}
					
					CComponentGlobals::GetInstance().StartCameraShake( g_fLevelBeginCameraShakeMagnitude, g_fLevelBeginCameraShakeTime );
				}
			}
		}
	}
	
	if( m_bSmokeActive )
	{
		m_fSmokeTimer += ITickable::smTickSec;
		if( m_fSmokeTimer >= g_fActiveSmokeTime )
		{
			m_bSmokeActive = false;
			if( g_pPlayerIntroSmoke )
			{
				g_pPlayerIntroSmoke->setVisible( false );
				g_pPlayerIntroSmoke->setPosition( g_vSmokeSafePos );
			}
		}
	}
	
	// Handle the Intro smoke
	if( m_eLevelBeginState == LEVEL_BEGIN_STATE_GO )
	{
		m_fPostLandingTimer += ITickable::smTickSec;
		if( m_fPostLandingTimer >= g_fPostLandingTime )
		{
			m_eLevelBeginState = LEVEL_BEGIN_STATE_FINISHED;
			m_bLevelBeginSequenceFinished = true;
			
			//CSoundManager::GetInstance().PlayInGameBGM();
			//CComponentGlobals::GetInstance().StartTimer();
			
			// Check Tutorial Level
			if( CComponentTutorialLevel::IsTutorialLevel() )
			{
				CComponentTutorialLevel::StartTutorial();
			}
			
			bool bPlayBGMAndStartTimer = true;
	
			// Display the In-Game tutorials, if a level has one and has not already been shown
			
			// Serpent
			if( CComponentInGameTutorialObject::DoesLevelHaveSerpentTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SERPENT ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SERPENT );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			// Crystal
			else if( CComponentInGameTutorialObject::DoesLevelHaveCrystalTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_CRYSTAL ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_CRYSTAL );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			// Lava Pit
			else if( CComponentInGameTutorialObject::DoesLevelHaveLavaPitTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_LAVAPIT ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_LAVAPIT );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			// Smoke
			else if( CComponentInGameTutorialObject::DoesLevelHaveSmokeTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SMOKE ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SMOKE );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			// Boss
			else if( CComponentInGameTutorialObject::DoesLevelHaveBossTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_BOSS ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateBossTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_BOSS );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			// Super Blob
			else if( CComponentInGameTutorialObject::DoesLevelHaveSuperBlobTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_BLOB ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_BLOB );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			// Super Serpent
			else if( CComponentInGameTutorialObject::DoesLevelHaveSuperSerpentTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SERPENT ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SERPENT );
					
					bPlayBGMAndStartTimer = false;
				}				
			}
			
			// Super Smoke
			else if( CComponentInGameTutorialObject::DoesLevelHaveSuperSmokeTutorial() )
			{
				if( CComponentGlobals::GetInstance().HasTutorialBeenShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SMOKE ) == false )
				{
					CSoundManager::GetInstance().PlayTutorialStingerSound();
					
					CComponentInGameTutorialObject::ActivateSmokeTutorial();
					CComponentGlobals::GetInstance().MarkTutorialShown( SAVE_DATA_INDEX_TUTORIAL_SHOWN_SUPER_SMOKE );
					
					bPlayBGMAndStartTimer = false;
				}
			}
			
			if( bPlayBGMAndStartTimer )
			{
				// This level does not have an In-Game tutorial to be activated, so start the music and timer as normal.
				CSoundManager::GetInstance().PlayInGameBGM();
				CComponentGlobals::GetInstance().StartTimer();
			}
			
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelBeginSequence::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_eLevelBeginState = LEVEL_BEGIN_STATE_NONE;
		
	// Height of Player Intro Object:  512.000
	g_pPlayerIntroObject = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szIntroObjectName ) );
	if( g_pPlayerIntroObject )
	{
		g_pPlayerIntroObject->setVisible( false );
		m_bLevelBeginSequenceFinished = false;
		g_bIntroAnimPlayed = false;
	}
	
	g_pPlayerIntroSmoke = static_cast<t2dAnimatedSprite*>( Sim::findObject( g_szPlayerIntroSmoke ) );
	if( g_pPlayerIntroSmoke )
	{
		g_pPlayerIntroSmoke->setVisible( false );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

