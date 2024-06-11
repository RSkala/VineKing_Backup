//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentScoreScreenFacebookButton.h"

#include "ComponentGlobals.h"
#include "ComponentScoringManager.h"

#include "platformiPhone/iPhoneInterfaceWrapper.h"
#include "SoundManager.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vFacebookButtonPosition( -119.0f, 127.0f );

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//static const char g_szDefaultMessage[32] = "VineKing is AWESOME!";

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void _GetCurrentWorldAndLevelNumbers( char* );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentScoreScreenFacebookButton );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentScoreScreenFacebookButton::CComponentScoreScreenFacebookButton()
	: m_pOwner( NULL )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentScoreScreenFacebookButton::~CComponentScoreScreenFacebookButton()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentScoreScreenFacebookButton::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentScoreScreenFacebookButton ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentScoreScreenFacebookButton::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s %s - Must be added to a t2dSceneObject.", __FILE__, __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoreScreenFacebookButton::OnMouseDown( const t2dVector& _vWordMousePosition )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoreScreenFacebookButton::OnMouseUp( const t2dVector& _vWordMousePosition )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_UnpressedColor );
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	char szFacebookMessage[128];
	S32 iTotalLevelScore = CComponentScoringManager::GetInstance().GetTotalLevelScore();
	char szCurrentLevel[32] = "";
	
	if( CComponentGlobals::GetInstance().GetActiveLevelSaveIndex() == SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 )
	{
		sprintf( szFacebookMessage, "I just earned %d coins in the Tutorial in VineKing! Learning pays!", iTotalLevelScore );
	}
	else
	{
		_GetCurrentWorldAndLevelNumbers( szCurrentLevel );
		sprintf( szFacebookMessage, "I just earned %d coins on level %s in VineKing! It's good to be the king!", iTotalLevelScore, szCurrentLevel );
	}
	
	FacebookWrapper::OpenFeedDialog( szFacebookMessage );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoreScreenFacebookButton::OnMouseEnter( const t2dVector& _vWordMousePosition )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoreScreenFacebookButton::OnMouseLeave( const t2dVector& _vWordMousePosition )
{
	if( m_pOwner == NULL )
		return;
	
	t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pOwner );
	if( pSprite )
		pSprite->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentScoreScreenFacebookButton::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( true );
	m_pOwner->setVisible( false );
	m_pOwner->setPosition( g_vFacebookButtonPosition );
	m_pOwner->setLayer( LAYER_SCORE_SCREEN_ELEMENTS );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void _GetCurrentWorldAndLevelNumbers( char* _pszOutCurrentLevel )
{
	S32 iCurrentLevelSaveIndex = CComponentGlobals::GetInstance().GetActiveLevelSaveIndex();
	
	switch( iCurrentLevelSaveIndex )
	{
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01: sprintf( _pszOutCurrentLevel, "0.1" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01: sprintf( _pszOutCurrentLevel, "1.1" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01: sprintf( _pszOutCurrentLevel, "2.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02: sprintf( _pszOutCurrentLevel, "2.2" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01: sprintf( _pszOutCurrentLevel, "3.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02: sprintf( _pszOutCurrentLevel, "3.2" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01: sprintf( _pszOutCurrentLevel, "4.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02: sprintf( _pszOutCurrentLevel, "4.2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03: sprintf( _pszOutCurrentLevel, "4.3" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01: sprintf( _pszOutCurrentLevel, "5.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02: sprintf( _pszOutCurrentLevel, "5.2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03: sprintf( _pszOutCurrentLevel, "5.3" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01: sprintf( _pszOutCurrentLevel, "6.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02: sprintf( _pszOutCurrentLevel, "6.2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03: sprintf( _pszOutCurrentLevel, "6.3" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04: sprintf( _pszOutCurrentLevel, "6.4" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01: sprintf( _pszOutCurrentLevel, "7.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02: sprintf( _pszOutCurrentLevel, "7.2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03: sprintf( _pszOutCurrentLevel, "7.3" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04: sprintf( _pszOutCurrentLevel, "7.4" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01: sprintf( _pszOutCurrentLevel, "8.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02: sprintf( _pszOutCurrentLevel, "8.2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03: sprintf( _pszOutCurrentLevel, "8.3" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04: sprintf( _pszOutCurrentLevel, "8.4" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01: sprintf( _pszOutCurrentLevel, "9.1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02: sprintf( _pszOutCurrentLevel, "9.2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03: sprintf( _pszOutCurrentLevel, "9.3" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04: sprintf( _pszOutCurrentLevel, "9.4" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01: sprintf( _pszOutCurrentLevel, "Boss 1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02: sprintf( _pszOutCurrentLevel, "Boss 2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03: sprintf( _pszOutCurrentLevel, "Boss 3" ); break;
		
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01: sprintf( _pszOutCurrentLevel, "MoM 1" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02: sprintf( _pszOutCurrentLevel, "MoM 2" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03: sprintf( _pszOutCurrentLevel, "MoM 3" ); break;
		case SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04: sprintf( _pszOutCurrentLevel, "MoM 4" ); break;
			
		default:
			printf( "_GetCurrentWorldAndLevelNumbers - Unhandled level save index: %d\n", iCurrentLevelSaveIndex );
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------