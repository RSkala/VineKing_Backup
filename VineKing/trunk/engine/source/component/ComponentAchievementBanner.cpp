//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentAchievementBanner.h"

#include "ComponentGlobals.h"

#include "core/iTickable.h"
#include "platformiPhone/GameCenterWrapper.h"
#include "T2D/t2dTextObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szAchievement_Text[32] = "Achievement_Text";
static t2dTextObject* g_pAchievementText = NULL;

static const F32 g_fBannerMovementSpeed = 32.0f; // Units per second
static const t2dVector g_vBannerStartPos( 0, -256.0f );
static const t2dVector g_vBannerEndPos( 0, -224.0f );

static const F32 g_fBannerDisplayTimeSeconds = 2.0f;

const char* g_pszCurrentlyDisplayingAchievementTitle = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<int> CComponentAchievementBanner::sm_AchievementBannerQueue;
Vector<const char*> CComponentAchievementBanner::sm_AchievementBannerTitleQueue;
CComponentAchievementBanner* CComponentAchievementBanner::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentAchievementBanner );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentAchievementBanner::CComponentAchievementBanner()
	: m_pOwner( NULL )
	, m_eBannerState( BANNER_STATE_NONE )
	, m_fDisplayTimer( 0.0f )
{
	CComponentAchievementBanner::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentAchievementBanner::~CComponentAchievementBanner()
{
	CComponentAchievementBanner::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAchievementBanner::ReportAchievement( int _iAchievement )
{
	if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
		return;
	
	if( GameCenterWrapper::IsGameCenterAvailable() == false )
		return;
	
	if( GameCenterWrapper::HasAchievementBeenEarned( _iAchievement ) )
		return;
	
	GameCenterWrapper::ReportAchievement( _iAchievement );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAchievementBanner::DisplayAchievement( int _iAchievement )
{
	//if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
	//	return;
	
	//if( GameCenterWrapper::IsGameCenterAvailable() == false )
	//	return;
	
	//if( GameCenterWrapper::HasAchievementBeenEarned( _iAchievement ) )
	//	return;
	
	if( sm_pInstance->m_eBannerState != BANNER_STATE_HIDDEN && sm_pInstance->m_eBannerState != BANNER_STATE_LOADING_QUEUED )
	{
		// Make sure this Achievement is not already in the Queue
		for( S32 i = 0; i < sm_AchievementBannerQueue.size(); ++i )
		{
			if( _iAchievement == sm_AchievementBannerQueue[i] )
			{
				//printf( "Achievement already in queue\n" );
				return;
			}
		}
			   
		//printf( "DisplayAchievement(): Adding Achievement '%d' to queue\n", _iAchievement );
		sm_AchievementBannerQueue.push_back( _iAchievement );
		return;
	}

	if( g_pAchievementText )
	{
		const char* szAchievementTitle = GameCenterWrapper::GetAchievementTitle( _iAchievement );
		if( szAchievementTitle )
		{
			//printf( "Achievement Title: %s\n", szAchievementTitle );
			g_pAchievementText->setText( szAchievementTitle );
		}
	}
	
	sm_pInstance->m_pOwner->setVisible( true );
	sm_pInstance->m_pOwner->moveTo( g_vBannerEndPos, g_fBannerMovementSpeed, true, true, true, 0.1f );
	sm_pInstance->m_eBannerState = BANNER_STATE_MOVING_DOWN;
	
	//GameCenterWrapper::ReportAchievement( _iAchievement );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAchievementBanner::DisplayAchievement( const char* _pszAchievementTitle )
{
	if( _pszAchievementTitle == NULL )
		return;
	
	if( sm_pInstance == NULL )
	{
		// If the instance is NULL, then most likely the asynchronous Game Center methods called during a loading state, so queue the Achievement.
		//printf( "DisplayAchievement(): Instance is NULL. Queueing Achievement '%s'\n", _pszAchievementTitle );
		sm_AchievementBannerTitleQueue.push_back( _pszAchievementTitle );
		return;
	}
	
	if( g_pszCurrentlyDisplayingAchievementTitle != NULL )
	{
		if( strcmp( g_pszCurrentlyDisplayingAchievementTitle, _pszAchievementTitle ) == 0 )
		{
			//printf( "Achievement %s currently being displayed\n", _pszAchievementTitle );
			return;
		}
	}
	
	if( sm_pInstance->m_eBannerState != BANNER_STATE_HIDDEN && sm_pInstance->m_eBannerState != BANNER_STATE_LOADING_QUEUED )
	{
		// Make sure this Achievement is not already in the Queue
		for( S32 i = 0; i < sm_AchievementBannerTitleQueue.size(); ++i )
		{
			//printf( "Achievement in list%s:\n", _pszAchievementTitle );
			
			if( strcmp( _pszAchievementTitle, sm_AchievementBannerTitleQueue[i] ) == 0 )
			{
				//printf( "Achievement '%s' already in queue\n", _pszAchievementTitle );
				return;
			}
		}
		
		//printf( "DisplayAchievement(): Adding Achievement '%s' to queue\n", _pszAchievementTitle );
		sm_AchievementBannerTitleQueue.push_back( _pszAchievementTitle );
		return;
	}
	
	if( g_pAchievementText )
	{
		//printf( "Displaying Earned Achievement: %s\n", _pszAchievementTitle );
		g_pAchievementText->setText( _pszAchievementTitle );
	}
	
	sm_pInstance->m_pOwner->setVisible( true );
	sm_pInstance->m_pOwner->moveTo( g_vBannerEndPos, g_fBannerMovementSpeed, true, true, true, 0.1f );
	sm_pInstance->m_eBannerState = BANNER_STATE_MOVING_DOWN;
	
	g_pszCurrentlyDisplayingAchievementTitle = _pszAchievementTitle;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAchievementBanner::ReportAchievementPercentage( int _iAchievement, float _fPercent )
{
	if( GameCenterWrapper::IsGameCenterAvailable() == false )
		return;
	
	if( _fPercent >= 100.0f )
	{
		//printf( "Reporting completed percentage Achievement: %d\n", _iAchievement );
		ReportAchievement( _iAchievement );
	}
	else
	{
		GameCenterWrapper::ReportAchievementPercentage( _iAchievement, _fPercent );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAchievementBanner::OnLoadLevelScheduled()
{
	if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
		return;
	
	sm_pInstance->m_eBannerState = BANNER_STATE_DISABLE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentAchievementBanner::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentAchievementBanner ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentAchievementBanner::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentAchievementBanner::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	// Store the owner
	m_pOwner = pOwnerObject;
	
	CComponentAchievementBanner::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAchievementBanner::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentGlobals::GetInstance().IsLoadingFinished() == false )
		return;
	
	if( m_eBannerState == BANNER_STATE_DISABLE )
		return;
	
	switch( m_eBannerState )
	{			
		case BANNER_STATE_DISPLAYING:
			UpdateStateDisplaying();
			break;
			
		case BANNER_STATE_LOADING_QUEUED:
			UpdateStateLoadingQueued();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAchievementBanner::HandleOwnerPositionTargetReached()
{
	if( m_pOwner == NULL )
		return;
	
	switch( m_eBannerState )
	{
		case BANNER_STATE_MOVING_DOWN:
			m_pOwner->setPosition( g_vBannerEndPos );
			m_eBannerState = BANNER_STATE_DISPLAYING;
			break;
			
		case BANNER_STATE_MOVING_UP:
			m_pOwner->setPosition( g_vBannerStartPos );
			m_pOwner->setVisible( false );
			
			m_eBannerState = BANNER_STATE_HIDDEN;
			g_pszCurrentlyDisplayingAchievementTitle = NULL;
			
			if( CComponentAchievementBanner::sm_AchievementBannerTitleQueue.size() > 0 )
			{
				const char* pszAchievementTitle = CComponentAchievementBanner::sm_AchievementBannerTitleQueue[0];
				CComponentAchievementBanner::sm_AchievementBannerTitleQueue.pop_front();
				CComponentAchievementBanner::DisplayAchievement( pszAchievementTitle );
			}
			
			//if( CComponentAchievementBanner::sm_AchievementBannerQueue.size() > 0 )
			//{
			//	S32 iAchievement = CComponentAchievementBanner::sm_AchievementBannerQueue[0];
			//	CComponentAchievementBanner::sm_AchievementBannerQueue.pop_front();
			//	CComponentAchievementBanner::DisplayAchievement( iAchievement );
			//	
			//	printf( "Displaying a queued ACHIEVEMENT!\n" );
			//}
			
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAchievementBanner::OnPostInit()
{
	g_pszCurrentlyDisplayingAchievementTitle = NULL;
	
	if( m_pOwner )
	{
		m_pOwner->setPosition( g_vBannerStartPos );
		m_pOwner->setUsesPhysics( true );
		m_pOwner->setVisible( false );
		
		//if( CComponentAchievementBanner::sm_AchievementBannerQueue.size() == 0 )
		if( CComponentAchievementBanner::sm_AchievementBannerTitleQueue.size() == 0 )
			m_eBannerState = BANNER_STATE_HIDDEN;
		else
			m_eBannerState = BANNER_STATE_LOADING_QUEUED;
	}
	
	g_pAchievementText = static_cast<t2dTextObject*>( Sim::findObject( g_szAchievement_Text ) );
	
	m_fDisplayTimer = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAchievementBanner::UpdateStateDisplaying()
{
	if( m_pOwner == NULL )
		return;
	
	m_fDisplayTimer += ITickable::smTickSec;
	if( m_fDisplayTimer >= g_fBannerDisplayTimeSeconds )
	{
		m_pOwner->moveTo( g_vBannerStartPos, g_fBannerMovementSpeed, true, true, true, 0.1f );
		m_eBannerState = BANNER_STATE_MOVING_UP;
		m_fDisplayTimer = 0.0f;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentAchievementBanner::UpdateStateLoadingQueued()
{
	if( m_pOwner == NULL )
		return;
	
	// Handle any queued Achievements
	//if( CComponentAchievementBanner::sm_AchievementBannerQueue.size() > 0 )
	if( CComponentAchievementBanner::sm_AchievementBannerTitleQueue.size() > 0 )
	{
		//S32 iAchievement = CComponentAchievementBanner::sm_AchievementBannerQueue[0];
		//CComponentAchievementBanner::sm_AchievementBannerQueue.pop_front();
		//CComponentAchievementBanner::DisplayAchievement( iAchievement );
		const char* pszAchievementTitle = CComponentAchievementBanner::sm_AchievementBannerTitleQueue[0];
		CComponentAchievementBanner::sm_AchievementBannerTitleQueue.pop_front();
		CComponentAchievementBanner::DisplayAchievement( pszAchievementTitle );
	}
	else
	{
		// This shouldn't ever get here, but just in case, set the appropriate values.
		m_fDisplayTimer = 0.0f;
		m_pOwner->setPosition( g_vBannerStartPos );
		m_pOwner->setVisible( false );
		m_eBannerState = BANNER_STATE_HIDDEN;
		
		g_pszCurrentlyDisplayingAchievementTitle = NULL;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
