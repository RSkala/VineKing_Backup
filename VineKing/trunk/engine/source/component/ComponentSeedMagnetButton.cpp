//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentSeedMagnetButton.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentPauseScreen.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentTutorialLevel.h"

#include "T2D/t2dSceneObject.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static ColorF g_UnpressedColor( 1.0f, 1.0f, 1.0f, 1.0f );
static ColorF g_PressedColor( 0.5f, 0.5f, 0.5f, 1.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentSeedMagnetButton* CComponentSeedMagnetButton::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentSeedMagnetButton );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentSeedMagnetButton::CComponentSeedMagnetButton()
	: m_pOwner( NULL )
	, m_bIsActivated( false )
	, m_bIsPressed( false )
{
	CComponentSeedMagnetButton::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentSeedMagnetButton::~CComponentSeedMagnetButton()
{
	CComponentSeedMagnetButton::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSeedMagnetButton::HideSeedMagnetButton()
{
	if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
		return;
	
	sm_pInstance->m_pOwner->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSeedMagnetButton::ShowSeedMagnetButton()
{
    if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
		return;
    
    sm_pInstance->m_pOwner->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentSeedMagnetButton::IsSeedMagnetButtonVisible()
{
    if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
		return false;
    
    return sm_pInstance->m_pOwner->getVisible();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSeedMagnetButton::GetSeedMagnetButtonPosition( t2dVector& _outVector )
{
    if( sm_pInstance == NULL || sm_pInstance->m_pOwner == NULL )
    {
        _outVector = t2dVector::getZero();
        return;
    }
    
    _outVector = sm_pInstance->m_pOwner->getPosition();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSeedMagnetButton::DisableInput()
{
   if( sm_pInstance == NULL && sm_pInstance->m_pOwner == NULL )
       return;
    
    sm_pInstance->m_pOwner->setUseMouseEvents( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSeedMagnetButton::EnableInput()
{
    if( sm_pInstance == NULL && sm_pInstance->m_pOwner == NULL )
        return;
    
    sm_pInstance->m_pOwner->setUseMouseEvents( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentSeedMagnetButton::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentSeedMagnetButton ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentSeedMagnetButton::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentSeedMagnetButton::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSeedMagnetButton::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
	{
		if( m_pOwner->getBlendColour() != g_UnpressedColor )
			m_pOwner->setBlendColour( g_UnpressedColor );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSeedMagnetButton::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( CComponentTutorialLevel::IsManaSeedButtonAllowed() == false )
		return;		
    
    
	
	// Only register the Seed Magnet Button as pressed if it is the First Touched object (the first object the player touched after not touching the screen)
	if( CComponentGlobals::GetInstance().IsTouchingDown() == true || CComponentGlobals::GetInstance().GetFirstTouchDownObject() != m_pOwner )
		return;
	
	m_bIsActivated = true;
	m_bIsPressed = true;
	
	m_pOwner->setBlendColour( g_PressedColor );
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToEnterSeedMagnetMode();
	
	//CComponentLineDrawAttack::OnInvalidLineDrawAreaTouched();
    if( CComponentTutorialLevel::IsTutorialLevel() )
    {
        CComponentTutorialLevel::OnPlayerPressedSeedButton();
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSeedMagnetButton::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	m_bIsPressed = false;
	
	// Only register the Seed Magnet Button as pressed if it is the First Touched object (the first object the player touched after not touching the screen)
	//if( CComponentGlobals::GetInstance().GetFirstTouchDownObject() != m_pOwner )
	//	return;
	if( m_bIsActivated == false )
		return;
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToExitSeedMagnetMode();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSeedMagnetButton::OnMouseEnter( const t2dVector& _vWorldMousePoint )
{
	if( 1 ) // RKS TODO: Disable this one?
		return;
	
	if( m_pOwner == NULL )
		return;
	
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	m_pOwner->setBlendColour( g_PressedColor );
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToEnterSeedMagnetMode();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSeedMagnetButton::OnMouseLeave( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_bIsPressed = false;
	
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	//if( CComponentGlobals::GetInstance().GetFirstTouchDownObject() == m_pOwner )
	//	CComponentGlobals::GetInstance().ClearFirstTouchDownObject();
	
	m_pOwner->setBlendColour( g_UnpressedColor );
	
	CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseToExitSeedMagnetMode();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentSeedMagnetButton::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	m_pOwner->setUseMouseEvents( true );
	m_pOwner->setLayer( LAYER_HUD_BUTTONS );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------