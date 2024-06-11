//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentStoryOpeningScreenElement.h"

#include "ComponentStoryOpeningScreen.h"

#include "SoundManager.h"

#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szNextButton[32]		= "StoryButton_Next";
static const char g_szNextButtonImage[32]	= "StoryButton_Next_Image";

static const char g_szViewTutorialButtonYES[32] = "ViewTutorialButton_YES";
static const char g_szViewTutorialButtonNO[32] = "ViewTutorialButton_NO";

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentStoryOpeningScreenElement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryOpeningScreenElement::CComponentStoryOpeningScreenElement()
	: m_pOwner( NULL )
	, m_pVisualObject( NULL )
	, m_eElementType( ELEMENT_TYPE_NONE )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentStoryOpeningScreenElement::~CComponentStoryOpeningScreenElement()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::Enable()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( true );
	m_pOwner->setVisible( true );
	
	if( m_pVisualObject )
		m_pVisualObject->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::Disable()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( false );
	m_pOwner->setVisible( false );
	
	if( m_pVisualObject )
		m_pVisualObject->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::EnableMouseEvents()
{
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::DisableMouseEvents()
{
	if( m_pOwner )
		m_pOwner->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentStoryOpeningScreenElement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentStoryOpeningScreenElement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentStoryOpeningScreenElement::onComponentAdd( SimComponent* _pTarget )
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
	
	m_pOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_UnpressedColor );
	
	CSoundManager::GetInstance().PlayButtonClickForward();
	
	switch( m_eElementType )
	{
		case ELEMENT_TYPE_BUTTON_NEXT:
			CComponentStoryOpeningScreen::OnButtonNextPressed();
			break;
			
		case ELEMENT_TYPE_BUTTON_TUTORIAL_YES:
			CComponentStoryOpeningScreen::OnButtonViewTutorialYESPressed();
			break;
			
		case ELEMENT_TYPE_BUTTON_TUTORIAL_NO:
			CComponentStoryOpeningScreen::OnButtonViewTutorialNOPressed();
			break;
			
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_PressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pVisualObject )
		m_pVisualObject->setBlendColour( g_UnpressedColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentStoryOpeningScreenElement::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	if( dStrcmp( m_pOwner->getName(), g_szNextButton ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_NEXT;
		
		m_pVisualObject = static_cast<t2dStaticSprite*>( Sim::findObject( g_szNextButtonImage ) );
		
		m_pOwner->setUseMouseEvents( true );
		
		CComponentStoryOpeningScreen::SetButtonNext( this );
	}
	else if( dStrcmp( m_pOwner->getName(), g_szViewTutorialButtonYES ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_TUTORIAL_YES;
		
		m_pOwner->setUseMouseEvents( false );
		
		CComponentStoryOpeningScreen::SetButtonViewTutorialYES( this );
	}
	else if( dStrcmp( m_pOwner->getName(), g_szViewTutorialButtonNO ) == 0 )
	{
		m_eElementType = ELEMENT_TYPE_BUTTON_TUTORIAL_NO;
		
		m_pOwner->setUseMouseEvents( false );
		
		CComponentStoryOpeningScreen::SetButtonViewTutorialNO( this );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------