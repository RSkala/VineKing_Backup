//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentLineDrawAttack.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentLineDrawAttackObject.h"
#include "ComponentManaBar.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPauseScreen.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

#include "core/ITickable.h"
#include "T2D/t2dPath.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const t2dVector g_vSafePosition( 400.0f, 400.0f );
static const F32 g_fDragLengthLimit = 16.0f; // The amount of finger drag distance before placing a Line Object

static t2dVector g_vLastPosition;
static const t2dVector g_vWorldUp( 0.0f, -1.0f );

// Vine Speed
static const F32 g_fVineSpeeds[] =
{
	285.0f,		// Base
	292.0f,		// Level 1
	299.0f,		// Level 2
	306.0f,		// Level 3
	313.0f,		// Level 4
	320.0f,		// Level 5
	330.0f		// Level 6
};

static F32 g_fTraversalObjectSpeed = g_fVineSpeeds[0];

static const S32 g_iNumFollowObjects = 6;
static const F32 g_fFollowObjectStaggerTimeMS = 32.0f; // Time between follow object spawn

static const t2dVector g_vLineSizeOverride( 4.0f, 4.0f );

static const S32 g_iMaxNumConnectionsObjects	= 80;
static const S32 g_iMaxNumLineObjects			= g_iMaxNumConnectionsObjects;


static const char g_szConnectionObjectName[32]	= "LineDrawAttack_Circle";
static const char g_szLineObjectName[32]		= "LineDrawAttack_Square";
static const char g_szPathTraveralObject[32]	= "LineDrawAttack_VineTip";
static const char g_szPathObject[32]			= "LineDrawAttack_PathObject";
static const char g_szFollowObject[32]			= "LineDrawAttack_FollowObject";

// New mana drain variables
bool g_bUseNewLineDrawManaDrainMethod	= true;
bool g_bUseMaxLineDrawLength			= true;
bool g_bIsFirstBrickDestroyed			= false;

static F32 g_fMaxManaToDrain			= 0.0f;
static F32 g_fManaDrainRate				= 0.0f;
static const F32 g_fMaxLineDrawLength	= 600.0f; //550.0f;

F32 g_fTotalManaDrained					= 0.0f;
F32 g_fBrickDestroyManaDrain			= 20.0f;
F32 g_fBrickFirstDestroyManaDrain		= 20.0f; //25.0f;

// New Line Draw variables
static const F32 g_fMinDistFromHomeBase = 60.0f;
static const F32 g_fMinDistFromHomeBaseSquared = g_fMinDistFromHomeBase * g_fMinDistFromHomeBase;

static const ColorF g_DefaultLineDrawColor( 0.0f, 1.0f, 0.0f );
static const ColorF g_MaxLineDrawColor( 1.0f, 0.0f, 0.0f );

extern bool g_bForceFullyUpgraded;

static bool g_bStartLineDrawAtVineKingBelly = true;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLineDrawAttack* CComponentLineDrawAttack::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentLineDrawAttack );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentLineDrawAttack::CComponentLineDrawAttack()
	: m_pOwner( NULL )
	, m_pszLineObjectName( NULL )
	, m_pLineObject( NULL )
	, m_pszConnectionObjectName( NULL )
	, m_pConnectionObject( NULL )
	, m_pszPathObjectName( NULL )
	, m_pPathObject( NULL )
	, m_pszTestPathTraversalObjectName( NULL )
	, m_pPathTraversalObject( NULL )
	, m_iCurrentConnectionObjectIndex( 0 )
	, m_iCurrentLineObjectIndex( 0 )
	, m_bFingerDown( false )
	, m_fPathLength( 0.0f )
	, m_bAttackObjectActive( false )
	, m_fFollowObjectTimer( 0.0f )
	, m_iCurrentFollowObjectIndex( 0 )
	, m_bLineDrawActive( false )
{
	CComponentLineDrawAttack::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// OnInvalidLineDrawAreaTouched
//  - Invalid line draw areas are: Pause Button, Seed Magnet Button
/*static*/ void  CComponentLineDrawAttack::OnInvalidLineDrawAreaTouched()
{
	if( sm_pInstance == NULL )
		return;
	
	sm_pInstance->ClearLineDrawFromInvalidInput();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentLineDrawAttack::IsLineDrawAttackActive()
{
	return sm_pInstance ? sm_pInstance->m_bLineDrawActive : false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::OnLineAttackObjectPathFinished()
{
	if( m_pPathTraversalObject == NULL )
		return;
	
	// This crashes Torque, so just hide the traversal object
	//if( m_pPathObject )
	//	m_pPathObject->detachObject( m_pPathTraversalObject );
	
	m_pPathTraversalObject->setCollisionActive( false, false );
	m_pPathTraversalObject->setVisible( false );
	m_pPathTraversalObject->setPosition( g_vSafePosition );
	
	m_iCurrentFollowObjectIndex = 0;
	m_fFollowObjectTimer = 0.0f;
	
	for( S32 i = 0; i < m_FollowObjects.size(); ++i )
	{
		if( m_FollowObjects[i] )
		{
			m_FollowObjects[i]->setVisible( false );
			m_FollowObjects[i]->setPosition( g_vSafePosition );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/void CComponentLineDrawAttack::KillAllLineDrawEffects()
{
	if( sm_pInstance == NULL )
		return;
	
	if( sm_pInstance->m_pPathTraversalObject == NULL )
		return;

	if( sm_pInstance->m_pPathTraversalObject )
	{
		sm_pInstance->m_pPathTraversalObject->setCollisionActive( false, false );
		sm_pInstance->m_pPathTraversalObject->setVisible( false );
		sm_pInstance->m_pPathTraversalObject->setPosition( g_vSafePosition );
	}
	
	sm_pInstance->m_iCurrentFollowObjectIndex = 0;
	sm_pInstance->m_fFollowObjectTimer = 0.0f;
	
	S32 iNumFollowObjects = sm_pInstance->m_FollowObjects.size();
	for( S32 i = 0; i < iNumFollowObjects; ++i )
	{
		if( sm_pInstance->m_FollowObjects[i] )
		{
			sm_pInstance->m_FollowObjects[i]->setVisible( false );
			sm_pInstance->m_FollowObjects[i]->setPosition( g_vSafePosition );
		}
	}
	
	// Hide all the line objects
	t2dSceneObject* pLineObject = NULL;
	for( S32 i = 0; i < g_iMaxNumConnectionsObjects; ++i )
	{
		pLineObject = sm_pInstance->m_ConnectionObjectList[ i ];
		if( pLineObject )
		{
			pLineObject->setVisible( false );
			pLineObject->setPosition( g_vSafePosition );
		}
	}
	
	// Hide all the connection objects
	t2dSceneObject* pConnectionObject = NULL;
	for( S32 i = 0; i < g_iMaxNumLineObjects; ++i )
	{
		pConnectionObject = sm_pInstance->m_LineObjectList[ i ];
		if( pConnectionObject )
		{
			pConnectionObject->setVisible( false );
			pConnectionObject->setPosition( g_vSafePosition );
		}
	}
	
	sm_pInstance->m_iCurrentConnectionObjectIndex = 0;
	sm_pInstance->m_iCurrentLineObjectIndex = 0;
	
	sm_pInstance->SetLineDrawColorDefault();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::SetLineDrawColorDefault()
{
	t2dStaticSprite* pSprite = NULL;
	
	for( S32 i = 0; i < g_iMaxNumConnectionsObjects - 1; ++i )
	{
		pSprite = static_cast<t2dStaticSprite*>( m_ConnectionObjectList[i] );
		if( pSprite )
			pSprite->setBlendColour( g_DefaultLineDrawColor );
	}
	
	for( S32 i = 0; i < g_iMaxNumLineObjects; ++i )
	{
		pSprite = static_cast<t2dStaticSprite*>( m_LineObjectList[i] );
		if( pSprite )
			pSprite->setBlendColour( g_DefaultLineDrawColor );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::SetLineDrawColorMax()
{
	t2dStaticSprite* pSprite = NULL;
	
	for( S32 i = 0; i < m_iCurrentConnectionObjectIndex; ++i )
	{
		pSprite = static_cast<t2dStaticSprite*>( m_ConnectionObjectList[i] );
		if( pSprite )
			pSprite->setBlendColour( g_MaxLineDrawColor );
	}
	
	for( S32 i = 0; i < m_iCurrentLineObjectIndex; ++i )
	{
		pSprite = static_cast<t2dStaticSprite*>( m_LineObjectList[i] );
		if( pSprite )
			pSprite->setBlendColour( g_MaxLineDrawColor );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLineDrawAttack::ForceLineDrawAttackPathFinished()
{
    if( sm_pInstance == NULL )
        return;
    
    sm_pInstance->HideAllLineDrawingObjects();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentLineDrawAttack::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentLineDrawAttack ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "LineObject",			TypeString, Offset( m_pszLineObjectName,					CComponentLineDrawAttack ) );
	addField( "ConnectionObject",	TypeString, Offset( m_pszConnectionObjectName,			CComponentLineDrawAttack ) );
	addField( "PathObject",			TypeString, Offset( m_pszPathObjectName,					CComponentLineDrawAttack ) );
	
	addField( "TestObject",			TypeString, Offset( m_pszTestPathTraversalObjectName,	CComponentLineDrawAttack ) );
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentLineDrawAttack::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentLineDrawAttack::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwner = pOwnerObject;
	
	sm_pInstance = this;
	
	CComponentGlobals::GetInstance().SetLineDrawAttackComponent( this );
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	if( m_bAttackObjectActive == false )
		return;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( g_bUseNewLineDrawManaDrainMethod )
	{
		// If the attack object is visible (and therefore active) check its traveled distance to drain mana
		if( m_pPathTraversalObject && m_pPathTraversalObject->getVisible() )
		{
			if( g_fTotalManaDrained < g_fMaxManaToDrain ) // Total Travel Distance == Max amount of mana to drain
			{
				/*
				F32 fManaToSubtract = fDistanceTraveledThisFrame;
				
				if( (g_fTotalManaDrained + fDistanceTraveledThisFrame) > g_fMaxManaToDrain )
				{
					F32 fDifference = mFabs(g_fTotalManaDrained - g_fMaxManaToDrain);
					fManaToSubtract = fDifference;
				}
				*/
				
				if( g_bUseMaxLineDrawLength == false )
				{
					F32 fManaToDrainThisFrame = g_fManaDrainRate * ITickable::smTickSec;
					
					CComponentManaBar::SubtractMana( fManaToDrainThisFrame );				
					g_fTotalManaDrained += fManaToDrainThisFrame;
				}
			}
		}	
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Add the follow objects to the path
	if( m_pPathObject && 
		m_pPathTraversalObject && 
		m_pPathTraversalObject->getVisible() &&
		m_FollowObjects.size() > 0 )
	{
		if( m_iCurrentFollowObjectIndex >= m_FollowObjects.size() )
			return;
		
		m_fFollowObjectTimer += ITickable::smTickMs; //m_fGlobalSpawnTimer += ITickable::smTickSec;
		if( m_fFollowObjectTimer >= g_fFollowObjectStaggerTimeMS )
		{
			t2dSceneObject* pFollowObject = m_FollowObjects[ m_iCurrentFollowObjectIndex ];
			if( pFollowObject )
			{
				pFollowObject->setVisible( true );
				
				S32 iNumNodes = m_pPathObject->getNodeCount();
				m_pPathObject->attachObject(	pFollowObject,					// Object
												g_fTraversalObjectSpeed,		// Speed
												1,								// Direction (+ or -)
												true,							// OrientToPath?
												0,								// StartNode
												iNumNodes - 1,					// EndNode
												T2D_PATH_WRAP,					// ePathMode
												1,								// Loops
												true );							// sendToStart
				
				++m_iCurrentFollowObjectIndex;
				m_fFollowObjectTimer = 0.0f;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsLineDrawAllowed() == false )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( CComponentPlayerHomeBase::GetInstance().IsHomeBaseInSeedMagnetMode() )
		return;
	
	if( CComponentGlobals::GetInstance().IsTimerActive() == false )
		return;
	
	// If the player's finger is already down, ignore another call to OnMouseDown, as the player is using another finger.
	//if( m_bFingerDown )
	//	return;
	
	if( CComponentGlobals::GetInstance().IsTouchingDown() && CComponentGlobals::GetInstance().GetFirstTouchDownObject() == NULL )
	{
		// DM check if the player is touching home base before allowing for a line to draw
		if( m_bFingerDown == false )
		{
			// Get the visible center position of Home Base
			t2dVector vHomeBasePos;
			CComponentPlayerHomeBase::GetInstance().GetHomeBaseVisibleCenterPosition( vHomeBasePos );
			
			// DM: Get the distance between the Home Base and where the user touched the screen.
			t2dVector vHomeBaseToTouchPoint = _vWorldMousePoint - vHomeBasePos;
			F32 fDistanceSquared = vHomeBaseToTouchPoint.lenSquared();
			
			// If the distance is within range, allow the line to be drawn.
			if( fDistanceSquared <= g_fMinDistFromHomeBaseSquared )
			{
				m_bFingerDown = true;
				//CComponentPlayerHomeBase::TEST_VineKingFingerDown();
			}
		}
		else 
		{
            // 2014/04/08 - This fixes the line draw being allowed outside the normal "game" view on tall devices (like iPhone 5)
            if( _vWorldMousePoint.mY > 240 || _vWorldMousePoint.mY < -240 )
            {
                OnMouseUp( _vWorldMousePoint );
                return;
            }
            
			OnMouseDragged( _vWorldMousePoint );
			return;
		}
	}
	
	
	//=====================================
	// NEW SHIT
	
	//=====================================
	
	if( g_bStartLineDrawAtVineKingBelly )
	{
		CComponentPlayerHomeBase::GetInstance().GetHomeBaseBellyPosition( m_vFirstTouchDownPoint );
		CComponentPlayerHomeBase::GetInstance().GetHomeBaseBellyPosition( m_vPreviousTouchPoint );
	}
	else
	{
		m_vFirstTouchDownPoint = _vWorldMousePoint;
		m_vPreviousTouchPoint = _vWorldMousePoint;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	m_bFingerDown = false;
	//CComponentPlayerHomeBase::TEST_VineKingFingerUp();
	
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsLineDrawAllowed() == false )
		return;
	
	if( CComponentEndLevelScreen::HasLevelEnded() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsLineDrawAllowed() == false )
		return;
	
	CComponentGlobals::GetInstance().SetPlayerNotTouchingEnemy();
	
	if( m_bLineDrawActive )
	{
		g_bIsFirstBrickDestroyed = false;
		StartLineDrawAttack();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	if( CComponentPauseScreen::GetInstance().IsGamePaused() )
		return;
	
	if( CComponentTutorialLevel::IsTutorialLevel() && CComponentTutorialLevel::IsLineDrawAllowed() == false )
		return;
	
	if( CComponentPlayerHomeBase::GetInstance().IsHomeBaseInSeedMagnetMode() )
		return;
	
	if( CComponentGlobals::GetInstance().IsPlayerTouchingEnemy() )
		return;
	
	if( CComponentGlobals::GetInstance().IsTimerActive() == false )
		return;
	
	
	if( m_iCurrentConnectionObjectIndex >= g_iMaxNumConnectionsObjects )
	{
		// All connections objects used. Nothing will be drawn.
		return;
	}
	
	if( m_iCurrentLineObjectIndex >= g_iMaxNumLineObjects )
	{
		// All line objects used. Nothing will be drawn.
		return;
	}

	// Play Out of Mana sound
	if( g_bUseNewLineDrawManaDrainMethod == false )
	{
		if( CComponentManaBar::IsThereAnyAvailableMana() == false )
		{
			CSoundManager::GetInstance().PlayOutOfManaSound();
			return;
		}
	}
	else
	{
		if( g_bUseMaxLineDrawLength )
		{
            if( CComponentTutorialLevel::IsTutorialLevel() == true )
                CComponentTutorialLevel::OnPlayerDrewLine();
            
			if( m_fPathLength >= g_fMaxLineDrawLength )
			{
				CSoundManager::GetInstance().PlayOutOfManaSound();
				
				// Change the color of the drawn line
				SetLineDrawColorMax();
				
				return;
			}
			
			if( CComponentManaBar::GetCurrentManaAmount() <= 0.0f )
			{
				CSoundManager::GetInstance().PlayOutOfManaSound();
				CComponentPlayerHomeBase::GetInstance().NotifyHomeBaseOfOutOfManaLineDrawAttempt();
				return;
			}
		}
		else
		{
			if( m_fPathLength >= CComponentManaBar::GetCurrentManaAmount() ) // Length is limited exactly by the amount of mana
			{
				CSoundManager::GetInstance().PlayOutOfManaSound();
				return;
			}
		}
	}
	
	
	// The user is dragging his finger, so draw the line.
	t2dVector vPrevToNewPos = _vWorldMousePoint - m_vPreviousTouchPoint;
	F32 fDragLength = vPrevToNewPos.len();
	
	if( fDragLength > g_fDragLengthLimit )
	{
		m_fPathLength += fDragLength; // Length of the path is exactly the amount of mana that gets used
		
		if( g_bUseNewLineDrawManaDrainMethod == false )
		{
			CComponentManaBar::SubtractMana( fDragLength );
		}
		else
		{
			if( g_bUseMaxLineDrawLength == false )
				CComponentManaBar::OnLineDrawLengthIncrease( m_fPathLength );
		}
		
		//m_vPreviousTouchPoint = _vWorldMousePoint;
		
		//======================================
		if( m_bLineDrawActive == false )
		{
			PrepareAllObjectsForLineDraw();
			m_bLineDrawActive = true;
		}
		//======================================
		
		// Place the line object
		t2dSceneObject* pLineObject = NULL;
		if( m_iCurrentConnectionObjectIndex == 0 ) // THE FIRST 
		{
			pLineObject = m_ConnectionObjectList[0];
			if( pLineObject )
			{
				pLineObject->setPosition( m_vFirstTouchDownPoint );
				pLineObject->setVisible( true );
				++m_iCurrentConnectionObjectIndex;
			}
		}
		
		// Place the connection object
		pLineObject = m_ConnectionObjectList[ m_iCurrentConnectionObjectIndex ];
		if( pLineObject )
		{
			pLineObject->setPosition( _vWorldMousePoint );
			pLineObject->setVisible( true );
			
			m_iCurrentConnectionObjectIndex++;
		}
		
		
		// Place the line object
		t2dSceneObject* pConnectionObject = m_LineObjectList[ m_iCurrentLineObjectIndex ];
		if( pConnectionObject )
		{
			pConnectionObject->setPosition( _vWorldMousePoint );
			pConnectionObject->setVisible( true );
			
			m_iCurrentLineObjectIndex++;
			
			// Align the connection object in the direction of the last positon towards the current position
			
			// Get the vector from the last mouse position to the current mouse position
			//t2dVector vLastPosToCurrentPos = _vWorldMousePoint - g_vLastPosition;
			t2dVector vLastPosToCurrentPos = _vWorldMousePoint - m_vPreviousTouchPoint;
			
			// Get the distance between the last mouse position and the current mouse position to set the object size
			F32 fDistance = vLastPosToCurrentPos.len();
			t2dVector vSize = pConnectionObject->getSize();
			vSize.mY = fDistance;
			pConnectionObject->setSize( vSize );
			
			pConnectionObject->setPosition( _vWorldMousePoint );
			
			// Get the angle 
			t2dVector vLastPosToCurrentPosNormalized = vLastPosToCurrentPos;
			vLastPosToCurrentPosNormalized.normalise();
			
			F32 fDot = g_vWorldUp.dot( vLastPosToCurrentPosNormalized );
			F32 fAngleRadians = mAcos( fDot );
			
			F32 fAngleDegrees = mRadToDeg( fAngleRadians );
			
			// If the vector is on the left side of the screen, then reverse the angle (since the angle retrieved from the dot product can never be greater than 180 degrees (PI radians)
			if( vLastPosToCurrentPos.mX < 0.0f )
				fAngleDegrees *= -1.0f;
			
			// Set the rotation of the connection object
			pConnectionObject->setRotation( fAngleDegrees );
			
			// Set the position of the connection object on the midpoint between the last position and the current position (an object's position is its center)
			t2dVector vNewObjectPosition;
			t2dVector vHalfLastToCurrent( 0.5f * vLastPosToCurrentPos.mX, 0.5f * vLastPosToCurrentPos.mY );
			//vNewObjectPosition = ( vHalfLastToCurrent + g_vLastPosition );
			vNewObjectPosition = ( vHalfLastToCurrent + m_vPreviousTouchPoint );
			pConnectionObject->setPosition( vNewObjectPosition );
		}
		
		if( m_pPathObject )
		{
			//S32 addNode(t2dVector position, F32 rotation, F32 weight, S32 location = -1);
			m_pPathObject->addNode( _vWorldMousePoint, 0.0f, 0.0f );
			//printf( "Adding Path Node at (%f, %f)\n", _vWorldMousePoint.mX, _vWorldMousePoint.mY );
		}
		
		//g_vLastPosition = _vWorldMousePoint;
		m_vPreviousTouchPoint = _vWorldMousePoint;
		
		//m_bLineDrawActive = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::OnPostInit()
{
	if( m_pOwner )
		m_pOwner->setMouseLocked( true );
	
	m_ConnectionObjectList.clear(); 
	m_LineObjectList.clear();
	m_FollowObjects.clear();
	
	m_vFirstTouchDownPoint		= t2dVector::getZero();
	m_vPreviousTouchPoint		= t2dVector::getZero();
	
	g_bIsFirstBrickDestroyed	= false;
	m_bFingerDown				= false;
	
	InitializeLineObjects();
	
	if( g_bForceFullyUpgraded == false )
	{
		// Cheat Disabled
		g_fTraversalObjectSpeed = g_fVineSpeeds[ CComponentGlobals::GetInstance().GetXPLevelPurple() ];
	}
	else 
	{
		// Cheat Enabled
		g_fTraversalObjectSpeed = g_fVineSpeeds[ 6 ];
	}

	//printf( "g_fTraversalObjectSpeed: %f\n", g_fTraversalObjectSpeed );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentLineDrawAttack::InitializeLineObjects()
{
	// Create the connection objects (the circles that is placed in between each line)
	m_pConnectionObject = static_cast<t2dSceneObject*>( Sim::findObject( g_szConnectionObjectName ) );
	if( m_pConnectionObject )
	{
		m_pConnectionObject->setLayer( LAYER_LINE_DRAW );
		
		const char* pszName = m_pConnectionObject->getName();
		char szNewName[32];
		
		m_pConnectionObject->setSize( g_vLineSizeOverride );
		m_ConnectionObjectList.push_back( m_pConnectionObject );
		
		// Create the connection objects
		t2dSceneObject* pNewConnectionObject = NULL;
		for( S32 i = 1; i < g_iMaxNumConnectionsObjects; ++i )
		{
			pNewConnectionObject = m_pConnectionObject->clone();
			if( pNewConnectionObject )
			{
				sprintf( szNewName, "%s_%d", pszName, i );
				pNewConnectionObject->assignName( szNewName );
				m_ConnectionObjectList.push_back( pNewConnectionObject );
			}
		}
	}
		   
	// Create the line objects (the rectangles that are stretched from point to point)
	m_pLineObject = static_cast<t2dSceneObject*>( Sim::findObject( g_szLineObjectName ) );
	if( m_pLineObject )
	{
		m_pLineObject->setLayer( LAYER_LINE_DRAW );
		
		const char* pszName = m_pLineObject->getName();
		char szNewName[32];
		
		m_pLineObject->setSize( g_vLineSizeOverride );
		m_LineObjectList.push_back( m_pLineObject );
		
		// Create the line objects
		t2dSceneObject* pNewLineObject = NULL;
		for( S32 i = 1; i < g_iMaxNumLineObjects; ++i )
		{
			pNewLineObject = m_pLineObject->clone();
			if( pNewLineObject )
			{
				sprintf( szNewName, "%s_%d", pszName, i );
				pNewLineObject->assignName( szNewName );
				m_LineObjectList.push_back( pNewLineObject );
			}
		}
	}
	
	//printf( "Created %d Line Objects\n", m_LineObjectList.size() );
	
	m_pPathObject = static_cast<t2dPath*>( Sim::findObject( g_szPathObject ) );
	if( m_pPathObject )
		m_pPathObject->clear();
	
	// Get the Path Traversal object - This is the object that actually travels the path, checking collision against land bricks
	m_pPathTraversalObject = static_cast<t2dStaticSprite*>( Sim::findObject( g_szPathTraveralObject ) );
	if( m_pPathTraversalObject )
		m_pPathTraversalObject->setLayer( LAYER_BRICK_ATTACK_VINE );
	
	// Set up Follow Objects - These objects follow the Path Traversal object along its path, creating a trail effect
	m_FollowObjects.clear();
	t2dStaticSprite* pFollowObject = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFollowObject ) );
	if( pFollowObject )
	{
		pFollowObject->setLayer( LAYER_BRICK_ATTACK_VINE_TRAIL );
		
		S32 iCurrentLayerOrder = g_iNumFollowObjects; // NOTE: The higher the number, the closer it is to the user. The lower the number, the further back it is.
		pFollowObject->setVisible( false );
		pFollowObject->setCollisionPhysics( false, false );
		pFollowObject->setPosition( g_vSafePosition );
		pFollowObject->setLayerOrder( iCurrentLayerOrder );
		--iCurrentLayerOrder;
		
		m_FollowObjects.push_back( pFollowObject );
		
		t2dStaticSprite* pFollowObjectClone;
		char szName[64];
		for( S32 i = 0; i < g_iNumFollowObjects - 1; ++i )
		{
			pFollowObjectClone = static_cast<t2dStaticSprite*>( pFollowObject->clone() );
			if( pFollowObjectClone )
			{
				pFollowObjectClone->setVisible( false );
				pFollowObjectClone->setCollisionPhysics( false, false );
				pFollowObjectClone->setPosition( g_vSafePosition );
				pFollowObjectClone->setLayerOrder( iCurrentLayerOrder );
				--iCurrentLayerOrder;
				
				sprintf( szName, "%s_%d", pFollowObjectClone->getName(), i );
				pFollowObjectClone->assignName( szName );
				
				m_FollowObjects.push_back( pFollowObjectClone );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::HideAllLineDrawingObjects()
{
	t2dSceneObject* pLineObject = NULL;
	
	// Hide all the line objects
	for( S32 i = 0; i < g_iMaxNumConnectionsObjects; ++i )
	{
		pLineObject = m_ConnectionObjectList[ i ];
		if( pLineObject )
		{
			pLineObject->setVisible( false );
			pLineObject->setPosition( g_vSafePosition );
		}
	}
	
	t2dSceneObject* pConnectionObject = NULL;
	
	// Hide all the connection objects
	for( S32 i = 0; i < g_iMaxNumLineObjects; ++i )
	{
		pConnectionObject = m_LineObjectList[ i ];
		if( pConnectionObject )
		{
			pConnectionObject->setVisible( false );
			pConnectionObject->setPosition( g_vSafePosition );
		}
	}
	
	// Hide the path traversal objects and the follow objects
	OnLineAttackObjectPathFinished();
	
	m_iCurrentConnectionObjectIndex = 0;
	m_iCurrentLineObjectIndex = 0;
	
	SetLineDrawColorDefault();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::ClearLineDrawFromInvalidInput()
{
	//printf( "ClearLineDrawFromInvalidInput()\n" );
	
	HideAllLineDrawingObjects();
	
	m_fPathLength = 0.0f;
	
	// Clear the traversal path nodes
	if( m_pPathObject )
	{
		m_pPathObject->clear();
		m_pPathObject->setPosition( g_vSafePosition );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::PrepareAllObjectsForLineDraw()
{
	if( m_pPathTraversalObject )
	{
		m_pPathTraversalObject->setCollisionActive( false, false );
		m_pPathTraversalObject->setVisible( false );
		m_pPathTraversalObject->setPosition( g_vSafePosition );
		
		m_iCurrentFollowObjectIndex = 0;
		m_fFollowObjectTimer = 0.0f;
		for( S32 i = 0; i < m_FollowObjects.size(); ++i )
		{
			if( m_FollowObjects[i] )
			{
				m_FollowObjects[i]->setVisible( false );
				m_FollowObjects[i]->setPosition( g_vSafePosition );
			}
		}
	}
	
	//CComponentPathGridHandler::GetInstance().EnableAllDangerBrickCollisions();
	
	m_vPreviousTouchPoint = m_vFirstTouchDownPoint;
	
	m_iCurrentConnectionObjectIndex = 0;
	m_iCurrentLineObjectIndex = 0;
	
	if( m_pPathObject )
	{
		m_pPathObject->clear();
		m_pPathObject->addNode( m_vFirstTouchDownPoint, 0.0f, 0.0f );
		//printf( "Adding Path Node at (%f, %f)\n", m_vFirstTouchDownPoint.mX, m_vFirstTouchDownPoint.mY );
	}
	
	CComponentManaBar::OnLineDrawAttackReset();
	SetLineDrawColorDefault();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentLineDrawAttack::StartLineDrawAttack()
{
	// Do the line attack
	//printf( "Num Connection Objects Used: %d\n", m_iCurrentConnectionObjectIndex + 1 );
	//printf( "Num Line Objects Used:       %d\n", m_iCurrentLineObjectIndex + 1 );
	
	CComponentManaBar::OnLineDrawAttackReset();
	
	HideAllLineDrawingObjects();
	
	m_bLineDrawActive = false;
	
	g_fTotalManaDrained = 0.0f;
	g_fMaxManaToDrain = m_fPathLength;
	
	if( g_fMaxManaToDrain >= CComponentManaBar::GetCurrentManaAmount() )
		g_fMaxManaToDrain = CComponentManaBar::GetCurrentManaAmount();
	
	F32 fPathLength = m_fPathLength;
	m_fPathLength = 0.0f;
	
	if( fPathLength < g_fDragLengthLimit )
	{
		// The line wasn't drawn long enough to be considered an 'attack', so clear the traversal path nodes.
		if( m_pPathObject )
		{
			m_pPathObject->clear();
			m_pPathObject->setPosition( g_vSafePosition );
		}
		return;
	}
	
	if( m_pPathObject && m_pPathTraversalObject )
	{
		m_pPathTraversalObject->setCollisionActive( true, false );
		m_pPathTraversalObject->setVisible( true );
		m_pPathTraversalObject->setUsesPhysics( true );
		//m_pPathTraversalObject->setCollisionCallback( true );
		m_bAttackObjectActive = true;
		
		//t2dPath::attachObject(t2dSceneObject* object, F32 speed, S32 direction, bool orientToPath,S32 startNode, S32 endNode, ePathMode pathMode, S32 loops, bool sendToStart)
		S32 iNumNodes = m_pPathObject->getNodeCount();
		m_pPathObject->attachObject(	m_pPathTraversalObject,			// Object
										g_fTraversalObjectSpeed,		// Speed
										1,								// Direction (+ or -)
										true,							// OrientToPath?
										0,								// StartNode
										iNumNodes - 1,					// EndNode
										T2D_PATH_WRAP,					// ePathMode
										1,								// Loops
										true );							// sendToStart
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
