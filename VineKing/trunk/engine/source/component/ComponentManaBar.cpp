//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentManaBar.h"

#include "ComponentGlobals.h"
#include "ComponentPathGridHandler.h"
#include "ComponentTutorialLevel.h"

#include "core/iTickable.h"
#include "math/mMathFn.h"
#include "t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const Point2I g_TopFrameGridPoint( 0, 1 ); // Use this point on the grid to grab the world position to place the top part of the mana bar frame

static const F32 g_fManaAmounts[] =
{
	500.0f,	// Level 0 (Base, no upgrades)
	550.0f,	// Level 1
	600.0f,	// Level 2
	650.0f,	// Level 3
	750.0f,	// Level 4
	850.0f,	// Level 5
	1000.0f	// Level 6
};

// Mana bar size
static const F32 g_fMinVerticalManaBarSize		= 100.0f; // The smallest size the mana bar will be
static const F32 g_fMaxVerticalManaBarSize		= 200.0f; // The largest size the mana bar can be
static const F32 g_fMaxVerticalManaBarGrowth	= g_fMaxVerticalManaBarSize - g_fMinVerticalManaBarSize; // The maximum amount that the mana bar can grow

static const ColorF g_ManaBarColor( 16.0f/255.0f, 1.0f, 10.0f/255.0f );

// Mana Bar Flash
static const ColorF g_ManaBarFlashColor( 0.5f, 1.0f, 0.5f );
static const F32 g_fManaBarFlashTimeSeconds = 0.125f;
static F32 g_fManaBarFlashTimer = 0.0f;

// Bar that represents the mana that will be drained from a line draw
static const char g_szDrainingManaBar[32] = "DrainingManaBar";
static t2dStaticSprite* g_pDrainingManaBar = NULL;

//static const ColorF g_DrainingManaBarColor( 0.0f/255.0f, 1.0f, 150.0f/255.0f );
static const ColorF g_DrainingManaBarColor( 255.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f );

extern bool g_bUseMaxLineDrawLength;
extern bool g_bForceFullyUpgraded;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentManaBar* CComponentManaBar::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentManaBar );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentManaBar::CComponentManaBar()
	: m_pManaBarObject( NULL )
	, m_pszFrameTopName( NULL )
	, m_pFrameTop( NULL )
	, m_pszFrameCenterName( NULL )
	, m_pFrameCenter( NULL )
	, m_pszFrameBottomName( NULL )
	, m_pFrameBottom( NULL )
	, m_fCurrentMaxVerticalSize( 0.0f )
	, m_fCurrentManaAmount( g_fManaAmounts[0] )
	, m_fCurrentMaxManaAmount( g_fManaAmounts[0] )
{
	CComponentManaBar::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentManaBar::~CComponentManaBar()
{ 
	CComponentManaBar::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaBar::AddMana( const F32& _fManaAmount )
{	
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_fCurrentManaAmount >= sm_pInstance->m_fCurrentMaxManaAmount )
		return;
	
	sm_pInstance->m_fCurrentManaAmount += _fManaAmount;
	if( sm_pInstance->m_fCurrentManaAmount > sm_pInstance->m_fCurrentMaxManaAmount )
		sm_pInstance->m_fCurrentManaAmount = sm_pInstance->m_fCurrentMaxManaAmount;
	
	F32 fNewVerticalSizePercent = sm_pInstance->m_fCurrentManaAmount / sm_pInstance->m_fCurrentMaxManaAmount;
	F32 fNewVerticalSize = sm_pInstance->m_fCurrentMaxVerticalSize * fNewVerticalSizePercent;
	
	sm_pInstance->ChangeVerticalSize( fNewVerticalSize );
	
	g_fManaBarFlashTimer = g_fManaBarFlashTimeSeconds;
	
	t2dStaticSprite* pManaBarSprite = static_cast<t2dStaticSprite*>( sm_pInstance->m_pManaBarObject );
	if( pManaBarSprite )
		pManaBarSprite->setBlendColour( g_ManaBarFlashColor );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaBar::SubtractMana( const F32& _fManaAmount )
{
	if( sm_pInstance == NULL )
		return;
    
    // Do not subtract mana if this is the tutorial level (and the user has not passed a certain point)
    if( CComponentTutorialLevel::IsTutorialLevel() == true )
    {
        if( CComponentTutorialLevel::IsManaDrainingAllowed() == false )
            return;
    }
	
	if( CComponentGlobals::GetInstance().Debug_IsUnlimitedManaEnabled() )
		return;
	
	if( sm_pInstance->m_fCurrentManaAmount <= 0.0f )
	{
		if( g_bUseMaxLineDrawLength  )
			CComponentGlobals::GetInstance().OnLineAttackObjectPathFinished();
    
		return;
	}
	
	sm_pInstance->m_fCurrentManaAmount -= _fManaAmount;
	if( sm_pInstance->m_fCurrentManaAmount <= 0.0f )
    {
		sm_pInstance->m_fCurrentManaAmount = 0.0f;
        
        if( CComponentTutorialLevel::IsTutorialLevel() )
        {
            CComponentTutorialLevel::OnPlayerOutOfMana();
        }
    }
	
	F32 fNewVerticalSizePercent = sm_pInstance->m_fCurrentManaAmount / sm_pInstance->m_fCurrentMaxManaAmount;
	F32 fNewVerticalSize = sm_pInstance->m_fCurrentMaxVerticalSize * fNewVerticalSizePercent;
	
	sm_pInstance->ChangeVerticalSize( fNewVerticalSize );
	
	if( g_bUseMaxLineDrawLength && sm_pInstance->m_fCurrentManaAmount <= 0.0f )
	{
		//CComponentLineDrawAttack::GetInstance().OnLineAttackObjectPathFinished();
		CComponentGlobals::GetInstance().OnLineAttackObjectPathFinished();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ F32 CComponentManaBar::GetPercentManaRemaining()
{
	if( sm_pInstance == NULL )
		return 0.0f;
	
	return sm_pInstance->m_fCurrentManaAmount / sm_pInstance->m_fCurrentMaxManaAmount;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaBar::OnLineDrawAttackReset()
{
	if( sm_pInstance == NULL || sm_pInstance->m_pManaBarObject == NULL || g_pDrainingManaBar == NULL )
		return;
	
	//g_pDrainingManaBar->setSize( sm_pInstance->m_pManaBarObject->getSize() );
	
	// Set the vertical height to zero
	g_pDrainingManaBar->setSize( t2dVector(g_pDrainingManaBar->getSize().mX, 0.0f) );
	
	// Set the position to be the top of the mana bar
	t2dVector vPosition = sm_pInstance->m_pManaBarObject->getPosition();
	vPosition.mY -= sm_pInstance->m_pManaBarObject->getSize().mY * 0.5f;
	g_pDrainingManaBar->setPosition( vPosition );
	
	// Hide the draining mana bar
	g_pDrainingManaBar->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaBar::OnLineDrawLengthIncrease( F32& _fManaUsed )
{
	if( sm_pInstance == NULL || sm_pInstance->m_pManaBarObject == NULL || g_pDrainingManaBar == NULL )
		return;
	
	if( g_pDrainingManaBar->getVisible() == false )
		g_pDrainingManaBar->setVisible( true );
	
	F32 fPercentManaUsed = _fManaUsed / sm_pInstance->m_fCurrentManaAmount;
	if( fPercentManaUsed >= 1.0f )
		fPercentManaUsed = 1.0f;
	
	F32 fHeight = sm_pInstance->m_pManaBarObject->getSize().mY * fPercentManaUsed;
	F32 fHeightDifference = mFabs( fHeight - g_pDrainingManaBar->getSize().mY );
	g_pDrainingManaBar->setSize( t2dVector(g_pDrainingManaBar->getSize().mX, fHeight) );
	
	// Since the size is increasing, move the position DOWN by half of its new height
	t2dVector vNewPosition = g_pDrainingManaBar->getPosition();
	vNewPosition.mY += ( fHeightDifference * 0.5f );
	g_pDrainingManaBar->setPosition( vNewPosition );
	
	//printf( "Draining Mana Bar Height: %f\n", g_pDrainingManaBar->getSize().mY );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaBar::ForceFullMana()
{
    AddMana( GetCurrentMaxManaAmount() );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentManaBar::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pManaBarObject, CComponentManaBar ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "FrameTop",		TypeString,	Offset( m_pszFrameTopName,		CComponentManaBar ) );
	addField( "FrameCenter",	TypeString,	Offset( m_pszFrameCenterName,	CComponentManaBar ) );
	addField( "FrameBottom",	TypeString,	Offset( m_pszFrameBottomName,	CComponentManaBar ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Inherited from SimComponent
bool CComponentManaBar::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentManaBar::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pManaBarObject = pOwnerObject;
	
	//CComponentGlobals::GetInstance().SetManaBarComponent( this );
	CComponentManaBar::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaBar::onUpdate()
{
	if( m_pManaBarObject == NULL )
		return;
	
	if( g_fManaBarFlashTimer >= 0.0f )
	{
		g_fManaBarFlashTimer -= ITickable::smTickSec;
		if( g_fManaBarFlashTimer <= 0.0f )
		{
			g_fManaBarFlashTimer = 0.0f;
			
			t2dStaticSprite* pManaBarSprite = static_cast<t2dStaticSprite*>( m_pManaBarObject );
			if( pManaBarSprite )
				pManaBarSprite->setBlendColour( g_ManaBarColor );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentManaBar::OnPostInit()
{
	if( m_pManaBarObject == NULL )
		return;
	
	m_pManaBarObject->setLayer( LAYER_HUD_BARS );
	
	t2dStaticSprite* pManaBarSprite = static_cast<t2dStaticSprite*>( m_pManaBarObject );
	if( pManaBarSprite )
		pManaBarSprite->setBlendColour( g_ManaBarColor );

	F32 fPercentManaXPAdded = (F32)(CComponentGlobals::GetInstance().GetXPLevelBlue()) / (F32)(CComponentGlobals::GetInstance().GetXPLevelMax());
				
	if( g_bForceFullyUpgraded == false )
	{
		// Cheat Disabled
		m_fCurrentManaAmount = m_fCurrentMaxManaAmount = g_fManaAmounts[ CComponentGlobals::GetInstance().GetXPLevelBlue() ];
	}
	else 
	{
		// Cheat Enabled
		m_fCurrentManaAmount = m_fCurrentMaxManaAmount = g_fManaAmounts[ 6 ];
		fPercentManaXPAdded = ( 6.0f / (F32)(CComponentGlobals::GetInstance().GetXPLevelMax()) );
	}
		

	
	//m_fCurrentManaAmount = m_fCurrentMaxManaAmount;
	
	m_pFrameTop = static_cast<t2dSceneObject*>( Sim::findObject( m_pszFrameTopName ) );
	if( m_pFrameTop )
		m_pFrameTop->setLayer( LAYER_HUD_BAR_FRAMES );
	
	m_pFrameCenter = static_cast<t2dSceneObject*>( Sim::findObject( m_pszFrameCenterName ) );
	if( m_pFrameCenter )
		m_pFrameCenter->setLayer( LAYER_HUD_BAR_FRAMES );
	
	m_pFrameBottom = static_cast<t2dSceneObject*>( Sim::findObject( m_pszFrameBottomName ) );
	if( m_pFrameBottom )
		m_pFrameBottom->setLayer( LAYER_HUD_BAR_FRAMES );
	
	if( m_pFrameTop == NULL || m_pFrameCenter == NULL || m_pFrameBottom == NULL )
		return;
	
	// Position the Top Frame Part
	t2dVector vTopFramePosition;
	CComponentPathGridHandler::GetInstance().GetTileWorldPosition( g_TopFrameGridPoint.x, g_TopFrameGridPoint.y, vTopFramePosition );
	m_pFrameTop->setPosition( vTopFramePosition );
	

	// Set the Mana Bar's minimum vertical size
	m_pManaBarObject->setSize( t2dVector( m_pManaBarObject->getSize().mX, g_fMinVerticalManaBarSize ) );
	
	// Set the Mana Bar's new vertical size
	F32 fVerticalSizeToAdd = fPercentManaXPAdded * g_fMaxVerticalManaBarGrowth;
	m_fCurrentMaxVerticalSize = g_fMinVerticalManaBarSize + fVerticalSizeToAdd;

	m_pManaBarObject->setSize( t2dVector( m_pManaBarObject->getSize().mX, m_fCurrentMaxVerticalSize ) );
	
	// Set the Center Frame's vertical size to be the same as the Mana Bar
	m_pFrameCenter->setSize( t2dVector( m_pFrameCenter->getSize().mX, m_pManaBarObject->getSize().mY ) );
	
	// Set the Center Frame position
	t2dVector vCenterFramePosition = vTopFramePosition;
	vCenterFramePosition.mY += (m_pFrameTop->getSize().mY * 0.5f + m_pManaBarObject->getSize().mY * 0.5f);
	m_pFrameCenter->setPosition( vCenterFramePosition );
	
	// Set the Mana Bar in the same approximate position as the Center Frame, but needs to be offset to the left by half it's width, so it doesn't show through the frame's alpha.
	t2dVector vManaBarPosition = vCenterFramePosition;
	vManaBarPosition.mX -= (m_pManaBarObject->getSize().mX * 0.5f);
	m_pManaBarObject->setPosition( vManaBarPosition );
	

	// Set the Bottom Frame position
	t2dVector vBottomFramePosition = vCenterFramePosition;
	vBottomFramePosition.mY += (m_pFrameCenter->getSize().mY * 0.5f + m_pFrameBottom->getSize().mY * 0.5f);
	m_pFrameBottom->setPosition( vBottomFramePosition );
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	g_pDrainingManaBar = static_cast<t2dStaticSprite*>( Sim::findObject( g_szDrainingManaBar ) );
	if( g_pDrainingManaBar )
	{
		g_pDrainingManaBar->setPosition( m_pManaBarObject->getPosition() );
		g_pDrainingManaBar->setSize( m_pManaBarObject->getSize() );
		g_pDrainingManaBar->setBlendColour( g_DrainingManaBarColor );
		g_pDrainingManaBar->setLayer( LAYER_HUD_BARS );
		g_pDrainingManaBar->setVisible( false );
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	g_fManaBarFlashTimer = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentManaBar::ChangeVerticalSize( const F32& _fNewVerticalSize )
{
	if( m_pManaBarObject == NULL )
		return;
	
	F32 fCurrentManaBarSize = m_pManaBarObject->getSize().mY;
	
	// Exit if the new size is greater than the maximum or equal to the current size of the mana bar
	if( _fNewVerticalSize > m_fCurrentMaxVerticalSize || _fNewVerticalSize == fCurrentManaBarSize )
		return;
	
	bool bResizeSmaller = ( _fNewVerticalSize < fCurrentManaBarSize );
	F32 fDiff = mFabs( _fNewVerticalSize - fCurrentManaBarSize );
	
	m_pManaBarObject->setSize( t2dVector( m_pManaBarObject->getSize().mX, _fNewVerticalSize ) );
	
	//Setting the size will shrink the object towards its center, so reposition the object accordingly
	if( bResizeSmaller )
	{
		// Move the Mana Bar down
		t2dVector vNewPosition( m_pManaBarObject->getPosition() );
		vNewPosition.mY += ( fDiff * 0.5f );
		m_pManaBarObject->setPosition( vNewPosition );
	}
	else
	{
		// Move the Mana Bar up
		t2dVector vNewPosition( m_pManaBarObject->getPosition() );
		vNewPosition.mY -= ( fDiff * 0.5f );
		m_pManaBarObject->setPosition( vNewPosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------






