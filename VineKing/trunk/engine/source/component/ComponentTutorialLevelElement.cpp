//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentTutorialLevelElement.h"

#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentTutorialLevelElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialLevelElement::CComponentTutorialLevelElement()
	: m_pOwner( NULL )
	, m_eType( TUTORIAL_LEVEL_OBJECT_TYPE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialLevelElement::~CComponentTutorialLevelElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevelElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentTutorialLevelElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTutorialLevelElement::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dStaticSprite* pOwnerObject = dynamic_cast<t2dStaticSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s - Must be added to a t2dSceneObject.", __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevelElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevelElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	switch( m_eType )
	{
		case TUTORIAL_LEVEL_OBJECT_TYPE_LINEDRAW_BUTTON:
			CComponentTutorialLevel::OnTutorialButtonPressedLineDraw();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case TUTORIAL_LEVEL_OBJECT_TYPE_SEEDSUCK_BUTTON:
			CComponentTutorialLevel::OnTutorialButtonPressedSeedSuck();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case TUTORIAL_LEVEL_OBJECT_TYPE_BLAVA_BUTTON:
			CComponentTutorialLevel::OnTutorialButtonPressedBlava();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case TUTORIAL_LEVEL_OBJECT_TYPE_VINEATTACK_BUTTON:
			CComponentTutorialLevel::OnTutorialButtonPressedVineAttack();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		case TUTORIAL_LEVEL_OBJECT_TYPE_DAMAGE_BUTTON:
			CComponentTutorialLevel::OnTutorialButtonPressedDamage();
			CSoundManager::GetInstance().PlayButtonClickForward();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevelElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevelElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevelElement::onUpdate()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevelElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	if( strcmp( m_pOwner->getName(), "TutorialLevel_LineDraw_Button" ) == 0 )
	{
		m_eType = TUTORIAL_LEVEL_OBJECT_TYPE_LINEDRAW_BUTTON;
	}
	else if( strcmp( m_pOwner->getName(), "TutorialLevel_SeedSuck_Button" ) == 0 )
	{
		m_eType = TUTORIAL_LEVEL_OBJECT_TYPE_SEEDSUCK_BUTTON;
	}
	else if( strcmp( m_pOwner->getName(), "TutorialLevel_Blava_Button" ) == 0 )
	{
		m_eType = TUTORIAL_LEVEL_OBJECT_TYPE_BLAVA_BUTTON;
	}
	else if( strcmp( m_pOwner->getName(), "TutorialLevel_VineAttack_Button" ) == 0 )
	{
		m_eType = TUTORIAL_LEVEL_OBJECT_TYPE_VINEATTACK_BUTTON;
	}
	else if( strcmp( m_pOwner->getName(), "TutorialLevel_Damage_Button" ) == 0 )
	{
		m_eType = TUTORIAL_LEVEL_OBJECT_TYPE_DAMAGE_BUTTON;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------