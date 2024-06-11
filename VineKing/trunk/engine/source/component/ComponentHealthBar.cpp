//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentHealthBar.h"

#include "ComponentGlobals.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPlayerHomeBase.h"

#include "core/iTickable.h"
#include "math/mMathFn.h"
//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const Point2I g_LeftFrameGridPoint( 1, 0 ); // Use this point on the grid to grab the world position to place the left part of the health bar frame

static const F32 g_fMinHorizontalHealthBarSize		= 100.0f; // The smallest size the health bar will be
static const F32 g_fMaxHorizontalHealthBarSize		= 200.0f; // The largest size the health bar can be
static const F32 g_fMaxHorizontalHealthBarGrowth	= g_fMaxHorizontalHealthBarSize - g_fMinHorizontalHealthBarSize; // The maximum amount that the health bar can grow

static const ColorF g_HealthBarColor( 1.0f, 0.0f, 0.0f );

// Health Bar Flash
static const ColorF g_HealthBarFlashColor( 1.0f, 1.0f, 1.0f );
static const F32 g_fHealthBarFlashTimeSeconds = 0.125f;
static F32 g_fHealthBarFlashTimer = 0.0f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHealthBar* CComponentHealthBar::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentHealthBar );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentHealthBar::CComponentHealthBar()
	: m_pHealthBarObject( NULL )
	, m_pszFrameLeftName( NULL )
	, m_pFrameLeft( NULL )
	, m_pszFrameCenterName( NULL )
	, m_pFrameCenter( NULL )
	, m_pszFrameRightName( NULL )
	, m_pFrameRight( NULL )
	, m_fCurrentMaxHorizontalSize( 0.0f )
{
	CComponentHealthBar::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHealthBar::NotifyHomeBaseHealthChange()
{
	if( sm_pInstance == NULL )
		return;
	
	F32 fPercent = CComponentPlayerHomeBase::GetPercentHealthRemaining();
	F32 fNewHorizontalSize = sm_pInstance->m_fCurrentMaxHorizontalSize * fPercent;
	sm_pInstance->ChangeHorizontalSize( fNewHorizontalSize );
	
	g_fHealthBarFlashTimer = g_fHealthBarFlashTimeSeconds;
	
	t2dStaticSprite* pHealthBarSprite = static_cast<t2dStaticSprite*>( sm_pInstance->m_pHealthBarObject );
	if( pHealthBarSprite )
		pHealthBarSprite->setBlendColour( g_HealthBarFlashColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHealthBar::InitializeHealthBar()
{
	if( sm_pInstance == NULL )
		return;
	
	sm_pInstance->SetUpHealthBar();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentHealthBar::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pHealthBarObject, CComponentHealthBar ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "FrameLeft",		TypeString,	Offset( m_pszFrameLeftName,		CComponentHealthBar ) );
	addField( "FrameCenter",	TypeString,	Offset( m_pszFrameCenterName,	CComponentHealthBar ) );
	addField( "FrameRight",		TypeString,	Offset( m_pszFrameRightName,	CComponentHealthBar ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentHealthBar::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentHealthBar::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pHealthBarObject = pOwnerObject;
	
	CComponentHealthBar::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHealthBar::onUpdate()
{
	if( m_pHealthBarObject == NULL )
		return;
	
	if( g_fHealthBarFlashTimer >= 0.0f )
	{
		g_fHealthBarFlashTimer -= ITickable::smTickSec;
		if( g_fHealthBarFlashTimer <= 0.0f )
		{
			g_fHealthBarFlashTimer = 0.0f;
			
			t2dStaticSprite* pHealthBarSprite = static_cast<t2dStaticSprite*>( m_pHealthBarObject );
			if( pHealthBarSprite )
				pHealthBarSprite->setBlendColour( g_HealthBarColor );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHealthBar::OnPostInit()
{
	if( m_pHealthBarObject == NULL )
		return;
	
	m_pHealthBarObject->setLayer( LAYER_HUD_BARS );
	
	m_pFrameLeft	= static_cast<t2dSceneObject*>( Sim::findObject( m_pszFrameLeftName ) );
	if( m_pFrameLeft )
		m_pFrameLeft->setLayer( LAYER_HUD_BAR_FRAMES );
	
	m_pFrameCenter	= static_cast<t2dSceneObject*>( Sim::findObject( m_pszFrameCenterName ) );
	if( m_pFrameCenter )
		m_pFrameCenter->setLayer( LAYER_HUD_BAR_FRAMES );
	
	m_pFrameRight	= static_cast<t2dSceneObject*>( Sim::findObject( m_pszFrameRightName ) );
	if( m_pFrameRight )
		m_pFrameRight->setLayer( LAYER_HUD_BAR_FRAMES );
	
	g_fHealthBarFlashTimer = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentHealthBar::SetUpHealthBar()
{
	if( m_pHealthBarObject == NULL )
		return;
	
	F32 fPercentHealthXPAdded = CComponentGlobals::GetInstance().GetPercentHealthXPAdded(); // Get the amount of XP that the player added to Health in the LevelUp Screen
	
	if( m_pFrameLeft == NULL || m_pFrameCenter == NULL || m_pFrameRight == NULL )
		return;	
	
	// Position the Left Frame Part
	t2dVector vLeftFramePosition;
	CComponentPathGridHandler::GetInstance().GetTileWorldPosition( g_LeftFrameGridPoint.x, g_LeftFrameGridPoint.y, vLeftFramePosition );
	m_pFrameLeft->setPosition( vLeftFramePosition );
	
	// Set the Health Bar's minimum horizontal size
	m_pHealthBarObject->setSize( t2dVector( g_fMinHorizontalHealthBarSize, m_pHealthBarObject->getSize().mY ) );
	
	// Set the Health Bar's new horizontal size
	F32 fHorizontalSizeToAdd = g_fMaxHorizontalHealthBarGrowth * fPercentHealthXPAdded;
	m_fCurrentMaxHorizontalSize = g_fMinHorizontalHealthBarSize + fHorizontalSizeToAdd;
	
	m_pHealthBarObject->setSize( t2dVector( m_fCurrentMaxHorizontalSize, m_pHealthBarObject->getSize().mY ) );
	
	// Set the Center Frame's horizontal size to be the same as the Health Bar
	m_pFrameCenter->setSize( t2dVector( m_pHealthBarObject->getSize().mX, m_pFrameCenter->getSize().mY ) );
	
	// Set the Center Frame position
	t2dVector vCenterFramePosition = vLeftFramePosition;
	vCenterFramePosition.mX += (m_pFrameLeft->getSize().mX * 0.5f + m_pHealthBarObject->getSize().mX * 0.5f);
	m_pFrameCenter->setPosition( vCenterFramePosition );
	
	// Set the Health Bar in the same approximate position as the Center Frame, but needs to be offset up by half it's height, so it doesn't show through the frame's alpha.
	t2dVector vHealthBarPosition = vCenterFramePosition;
	vHealthBarPosition.mY -= (m_pHealthBarObject->getSize().mY * 0.5f);
	m_pHealthBarObject->setPosition( vHealthBarPosition );
	
	// Set the Bottom Frame position
	t2dVector vRightFramePosition = vCenterFramePosition;
	vRightFramePosition.mX += (m_pFrameCenter->getSize().mX * 0.5f + m_pFrameRight->getSize().mX * 0.5f);
	m_pFrameRight->setPosition( vRightFramePosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentHealthBar::ChangeHorizontalSize( const F32& _fNewHorizontalSize )
{
	if( m_pHealthBarObject == NULL )
		return;
	
	F32 fCurrentHealthBarHorizontalSize = m_pHealthBarObject->getSize().mX;
	
	// Exit if the new size is greater than the maximum or equal to the current size of the health bar
	if( _fNewHorizontalSize > m_fCurrentMaxHorizontalSize || _fNewHorizontalSize == fCurrentHealthBarHorizontalSize )
		return;
	
	bool bResizeSmaller = (_fNewHorizontalSize < fCurrentHealthBarHorizontalSize );
	F32 fDiff = mFabs( _fNewHorizontalSize - fCurrentHealthBarHorizontalSize );
	
	// Setting the size will shrink or grow the object around its center, so reposition the object accordingly
	if( bResizeSmaller )
	{
		// Move the health bar LEFT
		t2dVector vNewPosition( m_pHealthBarObject->getPosition() );
		vNewPosition.mX -= (fDiff * 0.5f);
		m_pHealthBarObject->setPosition( vNewPosition );
	}
	else
	{
		// Move the health bar RIGHT
		t2dVector vNewPosition( m_pHealthBarObject->getPosition() );
		vNewPosition.mX += (fDiff * 0.5f );
		m_pHealthBarObject->setPosition( vNewPosition );
	}

	// Set the new vertical size
	m_pHealthBarObject->setSize( t2dVector( _fNewHorizontalSize, m_pHealthBarObject->getSize().mY ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------




