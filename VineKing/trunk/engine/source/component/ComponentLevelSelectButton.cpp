//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLevelSelectButton.h"

#include "ComponentGlobals.h"
#include "ComponentInGameTutorialObject.h"
#include "ComponentMapSelectMenu.h"

#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );
static const t2dVector g_vBackButtonPosition( -60.0f, 92.0f );

static const char g_szMapIconsImageMap[32] = "map_stuff_01ImageMap";
static const S32 g_iLockedImageMapFrame		= 0;
static const S32 g_iUnlockedImageMapFrame	= 1;
static const S32 g_iCompletedImageMapFrame	= 2;


static const char g_szScore_StarMarkerImageMap[32] = "_3_starsImageMap";
static const S32 g_iFrameNumStarsOne	= 0;
static const S32 g_iFrameNumStarsTwo	= 1;
static const S32 g_iFrameNumStarsThree	= 2;
static const S32 g_iFrameNumStarsZero	= 3;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Static Member Instantiation

bool CComponentLevelSelectButton::sm_bLoadingLevel = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLevelSelectButton );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelSelectButton::CComponentLevelSelectButton()
	: m_pOwnerButton( NULL )
	, m_pszLevelNumber( NULL )
	, m_pszSubLevelNumber( NULL )
	, m_bIsBossLevel( false )
	, m_pszLevelName( NULL )
	, m_bIsBackButton( false )
	, m_bIsRPGButton( false )
	, m_bIsReturnToMainButton( false )
	, m_bIsHelpButton( false )
	, m_pVisualObject( NULL )
	, m_iActiveLevelSaveIndex( -1 )
	, m_bLocked( true )
	, m_bCompleted( false )
	, m_iStarRanking( 0 )
{
	m_szLevelString[0] = '\0';
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLevelSelectButton::~CComponentLevelSelectButton()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::DisableButton()
{
	if( m_pOwnerButton == NULL )
		return;
	
	m_pOwnerButton->setUseMouseEvents( false );
	m_pOwnerButton->setVisible( false );
	
	if( m_pVisualObject )
	{
		m_pVisualObject->setVisible( false );
		CComponentMapSelectMenu::GetInstance().RemoveMapHelpArrowFromLevelSelectButton( m_pOwnerButton );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::EnableButton()
{
	if( m_pOwnerButton == NULL )
		return;
	
	if( m_bCompleted || m_bLocked == false )
		m_pOwnerButton->setUseMouseEvents( true );
	
	m_pOwnerButton->setVisible( true );
	
	if( m_pVisualObject )
	{
		m_pVisualObject->setVisible( true );
			
		// Add the Map Help arrow to this object
		t2dStaticSprite* pSprite = static_cast<t2dStaticSprite*>( m_pVisualObject );
		if( pSprite )
		{			
			if( strcmp( pSprite->getImageMapName(), g_szMapIconsImageMap ) == 0 )
			{
				if( pSprite->getFrame() == g_iUnlockedImageMapFrame )
					CComponentMapSelectMenu::GetInstance().AddMapHelpArrowToLevelSelectButton( m_pOwnerButton );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLevelSelectButton::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerButton, CComponentLevelSelectButton ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	//%levelNumbers		= "01" TAB "02" TAB "03" TAB "04" TAB "05" TAB "06" TAB "07" TAB "08" TAB "09" TAB "10" TAB "11" TAB "Boss";
	addField( "LevelNumber",	TypeString, Offset( m_pszLevelNumber,		CComponentLevelSelectButton ) );
	
	//%subLevelNumbers	= "01" TAB "02" TAB "03" TAB "04";
	addField( "SubLevelNumber", TypeString, Offset( m_pszSubLevelNumber,	CComponentLevelSelectButton ) );
	
	addField( "BossLevel", TypeBool, Offset( m_bIsBossLevel, CComponentLevelSelectButton ) );
	
	addField( "LevelName", TypeString, Offset( m_pszLevelName, CComponentLevelSelectButton ) );
	
	addField( "BackButton", TypeBool, Offset( m_bIsBackButton, CComponentLevelSelectButton ) );
	
	addField( "RPGButton", TypeBool, Offset( m_bIsRPGButton, CComponentLevelSelectButton ) );
	
	addField( "ExitToMainButton", TypeBool, Offset( m_bIsReturnToMainButton, CComponentLevelSelectButton ) );
	
	addField( "MapHelpButton", TypeBool, Offset( m_bIsHelpButton , CComponentLevelSelectButton ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLevelSelectButton::onComponentAdd( SimComponent* _pTarget )
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
	
	m_pOwnerButton = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::OnMouseDown( const t2dVector& _vWorldMousePoint )
{	
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( m_pOwnerButton == NULL )
		return;
	
	if( CComponentLevelSelectButton::sm_bLoadingLevel )
		return;
	
	m_pOwnerButton->setBlendColour( g_PressedColor );
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( CComponentLevelSelectButton::sm_bLoadingLevel )
	{
		//printf( "CComponentLevelSelectButton::OnMouseUp - Pressing %s, but a level is already scheduled to load.\n", m_pOwnerButton->getName() );
		return;
	}
	
	if( m_pOwnerButton == NULL )
		return;
	
	m_pOwnerButton->setBlendColour( g_UnpressedColor );
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_UnpressedColor );
	
	if( m_bIsBackButton )
	{
		CSoundManager::GetInstance().PlayButtonClickBack();
		CComponentMapSelectMenu::GetInstance().OnMapSelectionBackButtonPressed();
		return;
	}
	
	if( m_bIsReturnToMainButton )
	{
		m_pOwnerButton->setBlendColour( g_UnpressedColor );
		CSoundManager::GetInstance().PlayButtonClickBack();
		CComponentMapSelectMenu::GetInstance().OnReturnToMainMenuButtonPressed();
		return;
	}
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	if( m_bIsRPGButton )
	{
		CComponentMapSelectMenu::GetInstance().OnMapSelectionRPGButtonPressed();
		return;
	}
	
	if( m_bIsHelpButton )
	{
		CComponentMapSelectMenu::GetInstance().OnHelpButtonPressed();
		return;
	}
	
	char szFileNameBuffer[1024];
	memset( szFileNameBuffer, 0, sizeof(szFileNameBuffer) );
	
	if( m_pszLevelName && m_pszLevelName[0] != '\0' )
	{
		sprintf( szFileNameBuffer, "%s%s", "game/data/levels/", m_pszLevelName );
		
		//CComponentGlobals::GetInstance().ScheduleLoadLevel( szFileNameBuffer );
		CComponentMapSelectMenu::GetInstance().OnLevelSelectButtonPressed( szFileNameBuffer );
	}
	else if( m_szLevelString[0] != '\0' )
	{
		Con::expandScriptFilename( szFileNameBuffer, sizeof(szFileNameBuffer), m_szLevelString );
		
		//CComponentGlobals::GetInstance().ScheduleLoadLevel( szFileNameBuffer );
		CComponentMapSelectMenu::GetInstance().OnLevelSelectButtonPressed( szFileNameBuffer );
	}
	
	CComponentGlobals::GetInstance().SetActiveLevelSaveIndex( m_iActiveLevelSaveIndex );
	
	CComponentLevelSelectButton::sm_bLoadingLevel = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( CComponentLevelSelectButton::sm_bLoadingLevel )
		return;
	
	if( m_pOwnerButton == NULL )
		return;
	
	m_pOwnerButton->setBlendColour( g_PressedColor );
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( CComponentLevelSelectButton::sm_bLoadingLevel )
		return;
	
	if( m_pOwnerButton == NULL )
		return;
	
	m_pOwnerButton->setBlendColour( g_UnpressedColor );

	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::OnPostInit()
{	
	//if( m_pOwnerButton )
	//	m_pOwnerButton->setUseMouseEvents( true );
	
	//if( m_bIsBackButton )
	//	m_pOwnerButton->setPosition( g_vBackButtonPosition );
	
	if( m_bIsRPGButton )
		return;
	
	if( m_bIsHelpButton )
		return;
	
	char szVisualObjectName[64];
	sprintf( szVisualObjectName, "%s_Image", m_pOwnerButton->getName() );
	m_pVisualObject = static_cast<t2dSceneObject*>( Sim::findObject( szVisualObjectName ) );
	//if( m_pVisualObject == NULL )
	//	printf( "%s does not have a corresponding image!\n", m_pOwnerButton->getName() );
	
	if( m_pszLevelNumber == NULL || m_pszLevelNumber[0] == '\0' )
		return;
	
	if( m_pszSubLevelNumber == NULL || m_pszSubLevelNumber[0] == '\0' )
		return;
	
	// Create the level string in the format:  
	//	"Boss_XX_YY.t2d" or "Level_XX_YY.t2d", where XX = Level# and YY = SubLevel#
	if( m_bIsBossLevel )
		strcpy( m_szLevelString, "game/data/levels/" );
	else
		strcpy( m_szLevelString, "game/data/levels/Level_" );

	strcat( m_szLevelString, m_pszLevelNumber );
	strcat( m_szLevelString, "_" );
	strcat( m_szLevelString, m_pszSubLevelNumber );
	strcat( m_szLevelString, ".t2d" );
	
	SetButtonProperties();
	
	CComponentLevelSelectButton::sm_bLoadingLevel = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLevelSelectButton::SetButtonProperties() // This function determines what image should be displayed for a button (i.e. Unlocked / Star Ranking / Medals)
{
	if( m_pOwnerButton == NULL || m_pVisualObject == NULL )
		return;
	
	t2dStaticSprite* pVisualObject = static_cast<t2dStaticSprite*>( m_pVisualObject );
	if( pVisualObject == NULL )
		return;

	char szLevelString[16];
	sprintf( szLevelString, "%s_%s", m_pszLevelNumber, m_pszSubLevelNumber );
	
	///////////////////////////////////////////////////////////
	// Level 01
	if(	strcmp( szLevelString, "01_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			m_bLocked = false;
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 02
	else if( strcmp( szLevelString, "02_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 ) )
			{
				m_bLocked = false;
				pVisualObject->setImageMap( g_szMapIconsImageMap, g_iUnlockedImageMapFrame );
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 03
	else if( strcmp( szLevelString, "03_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "03_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 04
	else if( strcmp( szLevelString, "04_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "04_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 05
	else if( strcmp( szLevelString, "05_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "05_02" ) == 0 ) 
	{
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "05_03" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) )
			{
				m_bLocked = false;
			}
		}
		
	}
	
	///////////////////////////////////////////////////////////
	// Level 06
	else if( strcmp( szLevelString, "06_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "06_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "06_03" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 07
	else if( strcmp( szLevelString, "07_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "07_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "07_03" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "07_04" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 08
	else if( strcmp( szLevelString, "08_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "08_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "08_03" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "08_04" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 09
	else if( strcmp( szLevelString, "09_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "09_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "09_03" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "09_04" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	///////////////////////////////////////////////////////////
	// Level 10
	else if( strcmp( szLevelString, "10_01" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "10_02" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02; 
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "10_03" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "10_04" ) == 0 ) 
	{ 
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04;
		
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	///////////////////////////////////////////////////////////
	// BOSS LEVELS
	else if( m_bIsBossLevel )
	{
		if( strcmp( m_pszSubLevelNumber, "01" ) == 0 )
		{
			m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01;
			m_bLocked = false;
			if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
			{
				m_bCompleted = true;
			}
		}
		else if( strcmp( m_pszSubLevelNumber, "02" ) == 0 )
		{
			m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02;
			m_bLocked = false;
			if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
			{
				m_bCompleted = true;
			}
		}
		else if( strcmp( m_pszSubLevelNumber, "03" ) == 0 )
		{
			m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03;
			m_bLocked = false;
			if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
			{
				m_bCompleted = true;
			}
		}
	}
	///////////////////////////////////////////////////////////
	// EXTRA DLC LEVELS
	else if( strcmp( szLevelString, "11_01" ) == 0 ) 
	{ 
		//printf( "HANDLING: Level_11_01\n" );
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01;
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "11_02" ) == 0 ) 
	{ 
		//printf( "HANDLING: Level_11_02\n" );
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02;
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "11_03" ) == 0 ) 
	{ 
		//printf( "HANDLING: Level_11_03\n" );
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03;
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02 ) )
			{
				m_bLocked = false;
			}
		}
	}
	else if( strcmp( szLevelString, "11_04" ) == 0 ) 
	{ 
		//printf( "HANDLING: Level_11_04\n" );
		m_iActiveLevelSaveIndex = SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04;
		if( CComponentGlobals::GetInstance().IsLevelComplete( m_iActiveLevelSaveIndex ) )
		{
			m_bCompleted = true;
		}
		else 
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03 ) )
			{
				m_bLocked = false;
			}
		}
	}
	
	
	
	// Report if the Save Index was not found
	if( m_iActiveLevelSaveIndex == -1 )
	{
		printf( "Active Level Save Index not set for %s\n", (m_pOwnerButton ? m_pOwnerButton->getName() : "") );
		return;
	}
	
	m_iStarRanking = CComponentGlobals::GetInstance().GetLevelStarRanking( m_iActiveLevelSaveIndex );
	   
	// Set the appropriate image map and mouse event flag
	if( m_bCompleted )
	{
		m_pOwnerButton->setUseMouseEvents( true );
		pVisualObject->setImageMap( g_szMapIconsImageMap, g_iCompletedImageMapFrame );
	}
	else 
	{
		if( m_bLocked )
		{
			m_pOwnerButton->setUseMouseEvents( false );
			pVisualObject->setImageMap( g_szMapIconsImageMap, g_iLockedImageMapFrame );
		}
		else
		{
			m_pOwnerButton->setUseMouseEvents( true );
			pVisualObject->setImageMap( g_szMapIconsImageMap, g_iUnlockedImageMapFrame );
		}
	}
	
	
	
	if( ( m_bCompleted || m_bLocked == false ) && pVisualObject )
	{
		if( m_iStarRanking == 1 )
			pVisualObject->setImageMap( g_szScore_StarMarkerImageMap, g_iFrameNumStarsOne );
		else if( m_iStarRanking == 2 )
			pVisualObject->setImageMap( g_szScore_StarMarkerImageMap, g_iFrameNumStarsTwo );
		else if( m_iStarRanking == 3 )
			pVisualObject->setImageMap( g_szScore_StarMarkerImageMap, g_iFrameNumStarsThree );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------