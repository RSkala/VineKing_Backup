//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMapSelectButton.h"

#include "ComponentGlobals.h"
#include "ComponentMapSelection.h"
#include "ComponentMapSelectMenu.h"
#include "ComponentInGameTutorialObject.h"

#include "SoundManager.h"
//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

static const char g_szMapIconsImageMap[32] = "map_stuff_01ImageMap";
static const S32 g_iLockedImageMapFrame		= 0;
static const S32 g_iUnlockedImageMapFrame	= 1;
static const S32 g_iCompletedImageMapFrame	= 2;

// Level Rating
static const char g_szLevelGradeImageMap[32] = "Map_Level_Grade_IconsImageMap";
static const S32 g_iLevelGradeBrozeIndex	= 0;
static const S32 g_iLevelGradeSilverIndex	= 1;
static const S32 g_iLevelGradeGoldIndex		= 2;

static const F32 g_fPercentageGold		= 100.0f;
static const F32 g_fPercentageSilver	= 66.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMapSelectButton );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelectButton::CComponentMapSelectButton()
	: m_pOwner( NULL )
	, m_pszMapObjectName( NULL )
	, m_pMapSelection( NULL )
	, m_pVisualObject( NULL )
	, m_bLocked( true )
	, m_bCompleted( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelectButton::~CComponentMapSelectButton()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::DisableInput()
{
	if( m_pOwner )
	{
		m_pOwner->setUseMouseEvents( false );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::EnableInput()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_bCompleted || m_bLocked == false )
		m_pOwner->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMapSelectButton::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMapSelectButton ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "Map", TypeString, Offset( m_pszMapObjectName, CComponentMapSelectButton ) ); 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMapSelectButton::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentMapSelectButton::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( false );
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_UnpressedColor );
	
	if( m_pMapSelection )
		m_pMapSelection->SetMapSelectionActive();
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	CComponentMapSelectMenu::GetInstance().OnMapSelectButtonPressed();
	
	// This is to force the position of the Back Button for the new map levels
	if( m_pOwner && dStrcmp( m_pOwner->getName(), "Map_Selection_11" ) == 0 )
	{
		CComponentMapSelectMenu::GetInstance().OnLevel11MapSelectButtonPressed();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		return;
	
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelectButton::OnPostInit()
{
	if( m_pOwner == NULL )
		return;

	CComponentMapSelectMenu::GetInstance().AddMapSelectButton( this );
	
	//m_pOwner->setCollisionActive( false, true );
	//m_pOwner->setUseMouseEvents( true );
	
	if( m_pszMapObjectName && m_pszMapObjectName[0] != '\0' )
	{
		t2dSceneObject* pMapObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszMapObjectName ) );
		if( pMapObject && pMapObject->getComponentCount() > 0 )
			m_pMapSelection = static_cast<CComponentMapSelection*>( pMapObject->getComponent( 0 ) );
	}
	
	char szVisualObjectName[64];
	sprintf( szVisualObjectName, "%s_Image", m_pOwner->getName() );
	m_pVisualObject = static_cast<t2dSceneObject*>( Sim::findObject( szVisualObjectName ) );
	
	if( m_pVisualObject ==  NULL )
		return;
	
	t2dStaticSprite* pVisualObject = static_cast<t2dStaticSprite*>( m_pVisualObject );
	if( pVisualObject == NULL )
		return;
	
	F32 fPercentStarsEarned = 0.0f;
	
	char* szName = const_cast<char*>( m_pOwner->getName() );
	if( szName )
	{
		if( strcmp( szName, "Map_Selection_01" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 );
				fPercentStarsEarned = (F32)iStarTotal / 3.0f;
			}
			else
			{
				CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 0 );
			}
			
			m_bLocked = false;
		}
		else if( strcmp( szName, "Map_Selection_02" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01 );
				fPercentStarsEarned = (F32)iStarTotal / 3.0f;
			}
			else
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_01_01 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 1 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_03" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02 );
				fPercentStarsEarned = (F32)iStarTotal / 6.0f;
			}
			else
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_02_01 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 2 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_04" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 );
				fPercentStarsEarned = (F32)iStarTotal / 6.0f;
			}
			else 
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_01 ) &&
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_03_02 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 3 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_05" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03 );
				fPercentStarsEarned = (F32)iStarTotal / 9.0f;
			}
			else 
			{
				//if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) &&
				//    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 ) )
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 5 );
				}
			}	
		}
		else if( strcmp( szName, "Map_Selection_06" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03 );
				fPercentStarsEarned = (F32)iStarTotal / 9.0f;
			}
			else 
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_01 ) &&
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_02 ) && 
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_05_03 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 6 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_07" ) == 0 )
		{
			
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 );
				fPercentStarsEarned = (F32)iStarTotal / 12.0f;
			}
			else
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_01 ) &&
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_02 ) && 
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_06_03 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 7 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_08" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04 );
				fPercentStarsEarned = (F32)iStarTotal / 12.0f;
			}
			else
			{
				//if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) &&
				//	CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) && 
				//	CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) && 
				//	CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 ) )
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 9 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_09" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04 );
				fPercentStarsEarned = (F32)iStarTotal / 12.0f;
			}
			else 
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_01 ) &&
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_02 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_03 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_08_04 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 10 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_10" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01 ) &&
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03 ) && 
			    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03 ) +
								 CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04 );
				fPercentStarsEarned = (F32)iStarTotal / 12.0f;
			}
			else
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_01 ) &&
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_02 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_03 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_09_04 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 11 );
				}
			}
		}
		else if( strcmp( szName, "Map_Boss_Selection_01" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_01 );
				fPercentStarsEarned = (F32)iStarTotal / 3.0f;
			}
			else 
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_01 ) &&
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_04_02 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 4 );
				}
			}

		}
		else if( strcmp( szName, "Map_Boss_Selection_02" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_02 );
				fPercentStarsEarned = (F32)iStarTotal / 3.0f;
			}
			else 
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_01 ) &&
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_02 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_03 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_07_04 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 8 );
				}
			}
		}
		else if( strcmp( szName, "Map_Boss_Selection_03" ) == 0 )
		{
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
			{
				m_bCompleted = true;
				
				S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 );
				fPercentStarsEarned = (F32)iStarTotal / 3.0f;
				
				// All levels are complete. Make sure the Map Help Arrow is hidden
				CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( NULL, 12 );
			}
			else 
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_01 ) &&
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_02 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_03 ) && 
				    CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_10_04 ) )
				{
					m_bLocked = false;
					CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 12 );
				}
			}
		}
		else if( strcmp( szName, "Map_Selection_11" ) == 0 )
		{
			//printf( "HANDLING: MAP 11\n" );
			
			if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) == false )
			{
				// Hide the Level 11 Button if the final Boss has not yet been beaten
				if( m_pOwner )
					m_pOwner->setUseMouseEvents( false );
				
				if( m_pVisualObject )
					m_pVisualObject->setVisible( false );
				
				return;
			}
			else
			{
				if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01 ) &&
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_02 ) && 
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_03 ) && 
					CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_04 ) )
				{
					m_bCompleted = true;
					
					S32 iStarTotal = CComponentGlobals::GetInstance().GetLevelStarRanking( SAVE_DATA_INDEX_LEVEL_COMPLETE_11_01 );
					fPercentStarsEarned = (F32)iStarTotal / 9.0f; // RKS NOTE: Reminder - the divisor is the number of levels times 3, so this needs to change when more levels are added.
				}
				else 
				{
					if( CComponentGlobals::GetInstance().IsLevelComplete( SAVE_DATA_INDEX_LEVEL_COMPLETE_BOSS_03 ) )
					{
						m_bLocked = false;
						CComponentMapSelectMenu::GetInstance().SetMapHelpArrowPositionObject( m_pOwner, 13 );
					}
				}	
			}
		}
	}
	
	// Set the proper image map and mouse event flag
	if( m_bCompleted )
	{
		m_pOwner->setUseMouseEvents( true );
		//pVisualObject->setImageMap( g_szMapIconsImageMap, g_iCompletedImageMapFrame );
		
		fPercentStarsEarned *= 100.0f;
		if( fPercentStarsEarned >= g_fPercentageGold )
		{
			// Gold Earned
			pVisualObject->setImageMap( g_szLevelGradeImageMap, g_iLevelGradeGoldIndex );
		}
		else if( fPercentStarsEarned >= g_fPercentageSilver )
		{
			// Silver Earned
			pVisualObject->setImageMap( g_szLevelGradeImageMap, g_iLevelGradeSilverIndex );
		}
		else
		{
			// Bronze Earned
			pVisualObject->setImageMap( g_szLevelGradeImageMap, g_iLevelGradeBrozeIndex );
		}
		
		//printf( "%s - COMPLETED\n", szName );
	}
	else 
	{
		if( m_bLocked )
		{
			m_pOwner->setUseMouseEvents( false );
			pVisualObject->setImageMap( g_szMapIconsImageMap, g_iLockedImageMapFrame );
		}
		else 
		{
			m_pOwner->setUseMouseEvents( true );
			pVisualObject->setImageMap( g_szMapIconsImageMap, g_iUnlockedImageMapFrame );
			//printf( "%s - UNLOCKED\n", szName );
		}

	}	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
