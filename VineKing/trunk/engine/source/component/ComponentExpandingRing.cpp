//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentExpandingRing.h"

#include "ComponentGlobals.h"
#include "ComponentTileSwapping.h"

#include "platform/profiler.h"
#include "T2D/t2dAnimatedSprite.h"
#include "T2D/t2dTileMap.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//#define USE_PLAYER_BASE_MOVEMENT_FOR_EXPANDING_RING

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const F32 g_fExpandingBoxUnitsScaledPerSecond = 16.0f;
static const F32 g_fStartingExpandingRingSize = 1.0f;	//32.0f;
static F32 g_fCurrentExpandingRingSize = 1.0f;			// 32.0f;
static const F32 g_fMaxExpandingRingSize = 140.0f; //140.0f;
static const F32 g_fFingerHoldTime = 0.3f; // Amount of time before the ring starts growing (in seconds)

// Values used for exponential growth over time
// Use: y = x^2, where x = (t + 1), which results in:
//  y = (t + 1)^2, where t is some value that increases over time
static const F32 g_fCoefficientStartValue		= 5.65f;			// So the start value will be approximately 32 (5.65f * 5.65f ~ 32)
static const F32 g_fCoefficientGrowthRate		= 3.0f;			// Coefficient growth rate per second
static F32 g_fCurrentCoefficientValue	= g_fCoefficientStartValue;
static const F32 g_fMaxRingSize = g_fMaxExpandingRingSize;		// Max size of the ring

// Audio variables
// USE THE AudioProfile OBJECTS FROM THE SCRIPTS!
const char g_szTitleBGM[32]				= "BackgroundMusic01";
const char g_szExpandingSoundName[16] = "RingSound";

AUDIOHANDLE g_hTitleBGM			= NULL_AUDIOHANDLE;			// NULL_AUDIOHANDLE defined in platform/platformAudio.h
AUDIOHANDLE g_hExpandingSound	= NULL_AUDIOHANDLE;

//AudioProfile* m_pTitleBGM = NULL;
AudioProfile* g_pExpandingSound = NULL;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentExpandingRing );

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentExpandingRing::CComponentExpandingRing()
	: m_pOwner( NULL )
	, m_bPlayerTouchingScreen( false )
	, m_fExpansionTimer( 0.0f )
	, m_bIsExpanding( false )
	, m_fExpansionStartTimer( 0.0f )
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentExpandingRing::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentExpandingRing ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentExpandingRing::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	t2dSceneObject* pOwner = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwner == NULL )
	{
		Con::warnf( "CComponentExpandingRing::onComponentAdd - Must be added to a t2dSceneObject." );
		return false;
	}
	
	// Store the owner
	m_pOwner = pOwner;
	
	this->setProcessTicks( true );
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentExpandingRing::onUpdate()
{
	if( m_pOwner == NULL )
		return;
	
	// Initialize the Sounds
	if( g_pExpandingSound == NULL )
		g_pExpandingSound = dynamic_cast<AudioProfile*>( Sim::findObject( g_szExpandingSoundName ) ); // RKS: DO THIS ON LOAD, NOT HERE!!!!
	
	// Do not expand the ring if the home base is in Seed Magnet Mode
	if( CComponentGlobals::GetInstance().IsHomeBaseInSeedMagnetMode() )
		return;
	
#ifdef USE_PLAYER_BASE_MOVEMENT_FOR_EXPANDING_RING
	
	// Do nothing if the Home Base has not yet been placed
	if( CComponentGlobals::GetInstance().HasHomeBaseBeenPlaced() == false )
		return;
			
	if( CComponentGlobals::GetInstance().IsRingExpansionAllowed() )
	{
		if( m_bIsExpanding == false )
		{
			m_bIsExpanding = true;
				
			// Increase the size of the Expanding Ring
			t2dVector vNewSize( g_fStartingExpandingRingSize, g_fStartingExpandingRingSize );
			g_fCurrentExpandingRingSize = g_fStartingExpandingRingSize;
			m_pOwner->setSize( vNewSize );
			
			// Set the Expanding Ring's position to the Home Base's position
			t2dAnimatedSprite* pHomeBaseObject =  CComponentGlobals::GetInstance().GetPlayerHomeBaseObject();
			if( pHomeBaseObject )
				m_pOwner->setPosition( pHomeBaseObject->getPosition() );
			
			// Turn collision on the Expanding Ring
			m_pOwner->setCollisionActive( true, false );
			
			// Play the Expanding Sound
			if( g_hExpandingSound == NULL_AUDIOHANDLE )
			{
				if( g_pExpandingSound )
					g_hExpandingSound = alxPlay( g_pExpandingSound );
			}
			
			return;
		}
			
		// Increase the size
		m_fExpansionTimer += ITickable::smTickSec;
		
		g_fCurrentExpandingRingSize = g_fExpandingBoxUnitsScaledPerSecond * m_fExpansionTimer;
		
		if( g_fCurrentExpandingRingSize <= g_fMaxExpandingRingSize )
		{
			t2dVector vNewSize( g_fCurrentExpandingRingSize, g_fCurrentExpandingRingSize );
			m_pOwner->setSize( vNewSize );
		}
	}
	else
	{
		if( m_bIsExpanding )
		{
			m_bIsExpanding = false;
			m_fExpansionTimer = 2.0f;
			
			// Shrink the Expanding Ring to be invisible
			t2dVector vZeroSize( 0.0f, 0.0f );
			m_pOwner->setSize( vZeroSize );
			
			// Turn collision off the Expanding Ring object
			m_pOwner->setCollisionActive( false, false );
			
			// Stop the Expandin/Grow sound
			if( g_hExpandingSound != NULL_AUDIOHANDLE )
			{
				alxStop( g_hExpandingSound );
				g_hExpandingSound = NULL_AUDIOHANDLE;
			}
		}
	}
	
#else // USE_PLAYER_BASE_MOVEMENT_FOR_EXPANDING_RING
	
	// Check if the ring can start expanding
	if( CComponentGlobals::GetInstance().HasHomeBaseBeenPlaced() && CComponentGlobals::GetInstance().IsRingExpansionAllowed() )
	{
		// RKS TODO:  Review this
		if( m_fExpansionStartTimer >= g_fFingerHoldTime )
		{
			if( m_bIsExpanding == false )
			{
				m_bIsExpanding = true;
				
				t2dVector vNewSize( g_fStartingExpandingRingSize, g_fStartingExpandingRingSize );
				g_fCurrentExpandingRingSize = g_fStartingExpandingRingSize;
				
				m_pOwner->setSize( vNewSize );
				m_pOwner->setPosition( m_vLastMouseDownPosition ); // void t2dSceneObject::setPosition( const t2dVector& position )
				
				m_pOwner->setCollisionActive( true, false );
				
				// Play the Expanding Sound
				if( g_hExpandingSound == NULL_AUDIOHANDLE )
				{
					if( g_pExpandingSound )
						g_hExpandingSound = alxPlay( g_pExpandingSound );
				}
			}
		}
	}
	
	// Expand the ring
	if( CComponentGlobals::GetInstance().HasHomeBaseBeenPlaced() && CComponentGlobals::GetInstance().IsRingExpansionAllowed() )
	{
		if( m_bPlayerTouchingScreen )
			m_fExpansionStartTimer += ITickable::smTickSec;
		
		if( m_bIsExpanding )
		{
			m_fExpansionTimer += ITickable::smTickSec;
			
			//g_fCurrentExpandingRingSize = g_fExpandingBoxUnitsScaledPerSecond * m_fExpansionTimer;
			
			// RKS: Use the new 'exponential' expanding ring size scale
				
//			static const F32 g_fCoefficientStartValue		= 5.65f;			// So the start value will be approximately 32 (5.65f * 5.65f ~ 32)
//			static const F32 g_fCoefficientGrowthRate		= 2.0f;			// Coefficient growth rate per second
//			static const F32 g_fCurrentCoefficientValue	= g_fCoefficientStartValue;
//			static const F32 g_fMaxRingSize = g_fMaxExpandingRingSize;		// Max size of the ring
			
			g_fCurrentCoefficientValue = g_fCoefficientGrowthRate * m_fExpansionTimer; // RKS TODO: This should actually add the start value first, but let's try it out for now
			g_fCurrentExpandingRingSize = (g_fCurrentCoefficientValue * g_fCurrentCoefficientValue );
			
			if( g_fCurrentExpandingRingSize > g_fMaxExpandingRingSize )
				g_fCurrentExpandingRingSize = g_fMaxExpandingRingSize;
				
			if( g_fCurrentExpandingRingSize <= g_fMaxExpandingRingSize )
			{
				t2dVector vNewSize( g_fCurrentExpandingRingSize, g_fCurrentExpandingRingSize );
				m_pOwner->setSize( vNewSize );
			}
		}
	}
	
#endif // USE_PLAYER_BASE_MOVEMENT_FOR_EXPANDING_RING
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CComponentExpandingRing::onAddToScene()
{
	if( m_pOwner )
	{
		m_pOwner->enableUpdateCallback();
		m_pOwner->setUseMouseEvents( true ); // Allows the object to receive mouse events
		m_pOwner->setMouseLocked( true ); // Allows the object to receive all mouse events, regardless of whether it is under the mouse.  (REDUNDANT TO ONE ABOVE?)
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentExpandingRing::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_bPlayerTouchingScreen = true;
	m_vLastMouseDownPosition = _vWorldMousePoint;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentExpandingRing::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwner == NULL )
		return;
	
	m_bPlayerTouchingScreen = false;
	m_fExpansionStartTimer = 0.0f;
	
#ifndef USE_PLAYER_BASE_MOVEMENT_FOR_EXPANDING_RING
	if( CComponentGlobals::GetInstance().HasHomeBaseBeenPlaced() && CComponentGlobals::GetInstance().IsRingExpansionAllowed() )
	{
		m_bPlayerTouchingScreen = false;
		//m_fExpansionTimer = 2.0f; // fuck
		m_fExpansionTimer = 0.0f;
		
		// RKS TODO: Stop the grow sound
		t2dVector vZeroSize( 0.0f, 0.0f );
		m_pOwner->setSize( vZeroSize );
		
		m_pOwner->setCollisionActive( false, false );
		
		if( g_hExpandingSound != NULL_AUDIOHANDLE )
		{
			alxStop( g_hExpandingSound );
			g_hExpandingSound = NULL_AUDIOHANDLE;
		}
		
		m_bIsExpanding = false;
	}
#endif // USE_PLAYER_BASE_MOVEMENT_FOR_EXPANDING_RING
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentExpandingRing::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentExpandingRing::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( m_pOwner == NULL || _pCollisionStatus == NULL )
		return;
	
	// Note: In this case, m_pOwner will be a t2dSceneObject (or a t2dShapeVector I think)
	//  The destination collision (in the status) should be a t2dTileLayer.  I need to figure out how to get
	//  the tile and tile position that was collided with.
	
	/*
	 bool                mHandled;
	 bool                mValidCollision;
	 bool                mOverlapped;
	 bool                mSrcSolve;
	 bool                mDstSolve;
	 t2dSceneObject*     mSrcObject;
	 t2dSceneObject*     mDstObject;
	 t2dPhysics*         mpSrcPhysics;
	 t2dPhysics*         mpDstPhysics;
	 t2dVector           mCollisionNormal;
	 F32                 mCollisionTimeNorm;
	 F32                 mCollisionTimeReal;
	 F32                 mFullTimeStep;
	 U32                 mContactCount;
	 t2dVector           mSrcContacts[MAX_COLLISION_CONTACTS];
	 t2dVector           mDstContacts[MAX_COLLISION_CONTACTS];
	 */
	
	//char pszSrcObjectName[64];
	//char pszDstObjectName[64];
	
	//strcpy( pszSrcObjectName, _pCollisionStatus->mSrcObject->getName() );
	//strcpy( pszDstObjectName, _pCollisionStatus->mDstObject->getName() );
	
	//printf( "srcObject: %s\n", pszSrcObjectName );
	//printf( "dstObject: %s\n", pszDstObjectName );
	
	//printf( "src Ref: %s\n", _pCollisionStatus->mpSrcPhysics->getRefMetaString() );
	//printf( "dst Ref: %s\n", _pCollisionStatus->mpDstPhysics->getRefMetaString() );
	
	//dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    //dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);
	
	if( CComponentGlobals::GetInstance().IsThereAnyAvailableMana() == false )
		return;
	
	t2dTileLayer* pTileLayer = dynamic_cast<t2dTileLayer*>( _pCollisionStatus->mDstObject );
	if( pTileLayer == NULL )
		return;
	
	if( _pCollisionStatus->mpDstPhysics == NULL )
		return;
	
	const char* pszDstMetaString = _pCollisionStatus->mpDstPhysics->getRefMetaString();
	if( pszDstMetaString == NULL )
		return;
	
	U32 uLogicalX, uLogicalY;
	//int iNumItemsRead = sscanf( _pCollisionStatus->mpDstPhysics->getRefMetaString(), "%u %u", uLogicalX, uLogicalY );
	int iNumItemsRead = sscanf( pszDstMetaString, "%u %u", &uLogicalX, &uLogicalY );
	if( iNumItemsRead == 2 )
	{
		// The expanding ring has collided with a tile -- start the growing for that tile
		CComponentTileSwapping* pTileSwappingComponent = CComponentGlobals::GetInstance().GetTileSwappingComponent();
		if( pTileSwappingComponent )
			pTileSwappingComponent->StartTileGrowing( uLogicalX, uLogicalY );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

