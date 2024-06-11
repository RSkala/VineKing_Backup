//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentMapSelection.h"


#include "ComponentLevelSelectButton.h"
#include "ComponentMapSelectMenu.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( -500.0f, 500.0f );

// These are for the new "Eternal Abyss" Level 11 Map
static const char g_szMapSelection11[16] = "Map_Level_11";

static const char g_szMapSelection11_MASK[24] = "Map_Level_11_MASK"; // This is to place over the Map Selection to hide the question marks from the user

static const char g_szMap11LevelSelectButton01[32] = "Button_Level_11_01";
static const char g_szMap11LevelSelectButton02[32] = "Button_Level_11_02";
static const char g_szMap11LevelSelectButton03[32] = "Button_Level_11_03";
static const char g_szMap11LevelSelectButton04[32] = "Button_Level_11_04";


//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentMapSelection );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelection::CComponentMapSelection()
	: m_pOwner( NULL )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentMapSelection::~CComponentMapSelection()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelection::SetMapSelectionActive()
{
	if( m_pOwner == NULL )
		return;

	m_pOwner->setPosition( t2dVector::getZero() );
	m_pOwner->setVisible( true );
	
	CComponentMapSelectMenu::GetInstance().HideHelpArrowFromMapSelectionPressed();
	
	for( S32 i = 0; i < m_LevelSelectButtonList.size(); ++i )
	{
		if( m_LevelSelectButtonList[i] )
			m_LevelSelectButtonList[i]->EnableButton();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelection::SetMapSelectionInactive()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setPosition( g_vSafePosition );
	m_pOwner->setVisible( false );
	
	for( S32 i = 0; i < m_LevelSelectButtonList.size(); ++i )
	{
		if( m_LevelSelectButtonList[i] )
			m_LevelSelectButtonList[i]->DisableButton();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentMapSelection::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentMapSelection ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "LevelButton01", TypeString, Offset( m_pszLevelSelectButton01, CComponentMapSelection ) );
	addField( "LevelButton02", TypeString, Offset( m_pszLevelSelectButton02, CComponentMapSelection ) );
	addField( "LevelButton03", TypeString, Offset( m_pszLevelSelectButton03, CComponentMapSelection ) );
	addField( "LevelButton04", TypeString, Offset( m_pszLevelSelectButton04, CComponentMapSelection ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentMapSelection::onComponentAdd( SimComponent* _pTarget )
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

void CComponentMapSelection::onUpdate()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentMapSelection::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	CComponentMapSelectMenu::GetInstance().AddMapSelection( this );
	
	// This is the DLC Map Selection (Mini Map)
	if( m_pOwner->getName() && dStricmp( m_pOwner->getName(), g_szMapSelection11 ) == 0 )
	{
		AddLevelSelectButtonFromObjectName( g_szMap11LevelSelectButton01 );
		AddLevelSelectButtonFromObjectName( g_szMap11LevelSelectButton02 );
		AddLevelSelectButtonFromObjectName( g_szMap11LevelSelectButton03 );
		AddLevelSelectButtonFromObjectName( g_szMap11LevelSelectButton04 );
        
//        t2dStaticSprite* pMapLevel11Mask = static_cast<t2dStaticSprite*>( Sim::findObject( g_szMapSelection11_MASK ) );
//        if( pMapLevel11Mask )
//        {
//            pMapLevel11Mask->setPosition( m_pOwner->getPosition() );
//        }
	}
	else
	{
		AddLevelSelectButtonFromObjectName( m_pszLevelSelectButton01 );
		AddLevelSelectButtonFromObjectName( m_pszLevelSelectButton02 );
		AddLevelSelectButtonFromObjectName( m_pszLevelSelectButton03 );
		AddLevelSelectButtonFromObjectName( m_pszLevelSelectButton04 );
	}
	
	
	// Disabled all the Level Select buttons associated with this Mini Map Selection
	CComponentLevelSelectButton* pButtonComponent = NULL;
	
	S32 iNumLevelSelectButtons = m_LevelSelectButtonList.size();
	for( S32 i = 0; i < iNumLevelSelectButtons; ++i )
	{
		pButtonComponent = m_LevelSelectButtonList[i];
		if( pButtonComponent )
			pButtonComponent->DisableButton();
	}
	
	// Disable this MiniMap
	m_pOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE METHODS

void CComponentMapSelection::AddLevelSelectButtonFromObjectName( const char* _pszObjectName )
{
	if( _pszObjectName == NULL || _pszObjectName[0] == '\0' )
		return;
	
	t2dSceneObject* pObject = static_cast<t2dSceneObject*>( Sim::findObject( _pszObjectName ) );
	if( pObject && pObject->getComponentCount() > 0 )
	{
		CComponentLevelSelectButton* pButtonComponent = static_cast<CComponentLevelSelectButton*>( pObject->getComponent( 0 ) );
		if( pButtonComponent )
			m_LevelSelectButtonList.push_back( pButtonComponent );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

