//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentRelativeTouchMovement.h"

#include "T2D/t2dAnimatedSprite.h"
#include "ComponentPlayerBullet.h"

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const F32 g_fScreenLimitPositionTop		= -240.0f;
static const F32 g_fScreenLimitPositionBottom	= 240.0f;
static const F32 g_fScreenLimitPositionLeft		= -160.0f;
static const F32 g_fScreenLimitPositionRight		= 160.0f;

// Bullet constant globals
static const U32 g_uMaxNumBullets = 70;
static const F32 g_fTimeBetweenShots = 100; // In milliseconds;

// Bullet sound globals
static const char g_szShootSoundName[32] = "ButtonClickSound";
AudioProfile* g_pShootSound = NULL;
AUDIOHANDLE g_hShootSound = NULL_AUDIOHANDLE; // NULL_AUDIOHANDLE defined in platform/platformAudio.h

static const t2dVector g_vWorldRight( 1.0f, 0.0f );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentRelativeTouchMovement );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentRelativeTouchMovement::CComponentRelativeTouchMovement()
	: m_pOwnerObject( NULL )
	, m_pszBulletObjectName( NULL )
	, m_pBulletObject( NULL )
	, m_bIsFiringBullets( false )
	, m_fBulletFireTimer( 0.0f )
	, m_uLastUsedBulletIndex( 0 )
{
	m_vPreviousWorldMousePoint = t2dVector::getZero();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentRelativeTouchMovement::~CComponentRelativeTouchMovement()
{
	// Note: It is not necessary to delete the bullet objects, as the engine does it for us.  RKS TODO: Make sure that is the case!
	m_BulletPool.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Inherited from ConsoleObject
/*static*/ void CComponentRelativeTouchMovement::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	// This is needed so the script can access %behavior.Owner, which creates the C++ component as such: %behavior.Owner.Component = new <ComponentClassName>();
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentRelativeTouchMovement ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
	
	addField( "Bullet",	TypeString, Offset( m_pszBulletObjectName,	CComponentRelativeTouchMovement ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentRelativeTouchMovement::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2SceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "CComponentRelativeTouchMovement::onComponentAdd - Must be added to a t2dSceneObject.\n" );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	
	// NOTE: Do not set up any other member pointers here, because Torque may not have created the objects yet
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentRelativeTouchMovement::onUpdate()
{
	if( m_bIsFiringBullets )
		FireBullets();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentRelativeTouchMovement::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	m_vPreviousWorldMousePoint = _vWorldMousePoint;
	m_bIsFiringBullets = true;
	m_fBulletFireTimer = g_fTimeBetweenShots;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentRelativeTouchMovement::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	m_vPreviousWorldMousePoint = _vWorldMousePoint;
	m_bIsFiringBullets = false;
	m_fBulletFireTimer = g_fTimeBetweenShots;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentRelativeTouchMovement::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	if( m_pOwnerObject == NULL )
		return;
	
	// Get the vector from the previous mouse point to the current mouse point
	t2dVector vPrevPtToCurrPt = _vWorldMousePoint - m_vPreviousWorldMousePoint;
	m_vPreviousWorldMousePoint = _vWorldMousePoint;
	
	//F32 fLength = vPrevPtToCurrPt.len();
	
	//printf("Length of vPrevPtToCurrPt: %f\n", fLength );
	
	t2dVector vCurrentOwnerPosition = m_pOwnerObject->getPosition();
	t2dVector vNewOwnerPosition = vPrevPtToCurrPt + vCurrentOwnerPosition;
	
	//printf("vCurrentOwnerPosition: (%f, %f)\n", vCurrentOwnerPosition.mX, vCurrentOwnerPosition.mY );
	//printf("vNewOwnerPosition:     (%f, %f)\n", vNewOwnerPosition.mX, vNewOwnerPosition.mY );
	
	// If the ship's new position is outside the screen, set it to be the edge of the screen
	if( vNewOwnerPosition.mY > g_fScreenLimitPositionBottom )
	{
		vNewOwnerPosition.mY = g_fScreenLimitPositionBottom;
	}
	else if( vNewOwnerPosition.mY < g_fScreenLimitPositionTop )
	{
		vNewOwnerPosition.mY = g_fScreenLimitPositionTop;
	}
	
	// If the ship's new position is outside the screen, set it to be the edge of the screen
	if( vNewOwnerPosition.mX > g_fScreenLimitPositionRight )
	{
		vNewOwnerPosition.mX = g_fScreenLimitPositionRight;
	}
	else if( vNewOwnerPosition.mX < g_fScreenLimitPositionLeft )
	{
		vNewOwnerPosition.mX = g_fScreenLimitPositionLeft;
	}
	
	// Check which direction the ship is traveling and set the flip accordingly
	F32 fDot = vPrevPtToCurrPt.dot( g_vWorldRight );
	if( fDot > 0.0f )
	{
		// Moving right, so set the flip
		m_pOwnerObject->setFlipX( true );
	}
	else if( fDot < 0.0f )
	{
		// Moving left, so unset the flip
		m_pOwnerObject->setFlipX( false );
	}
	
	// Set the new position
	m_pOwnerObject->setPosition( vNewOwnerPosition );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentRelativeTouchMovement::OnPostInit()
{
	if( m_pOwnerObject )
	{
		m_pOwnerObject->setMouseLocked( true ); // React to any mouse down event on the screen
	}
	
	// Initialize the bullet object and create the bullet pool from it
	if( m_pBulletObject == NULL )
	{ 
		m_pBulletObject = static_cast<t2dSceneObject*>( Sim::findObject( m_pszBulletObjectName ) );
		
		if( m_pBulletObject )
		{
			t2dSceneObject* pBullet = NULL;
			for( U32 i = 0; i < g_uMaxNumBullets; ++i )
			{
				pBullet = m_pBulletObject->clone();
				if( pBullet )
					m_BulletPool.push_back( pBullet );
			}
			
			S32 iNumBullets = m_BulletPool.size();
			//printf( "Number of bullets created: %i\n", iNumBullets );
		}
	}
	
	if( g_pShootSound == NULL )
		g_pShootSound = static_cast<AudioProfile*>( Sim::findObject( g_szShootSoundName ) );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE MEMBER FUNCTIONS

void CComponentRelativeTouchMovement::FireBullets()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fBulletFireTimer += ITickable::smTickMs;
	if( m_fBulletFireTimer < g_fTimeBetweenShots )
		return;
	
	m_fBulletFireTimer = 0.0f;
	
	m_uLastUsedBulletIndex++;
	
	U32 uNumBulletsInPool = m_BulletPool.size();
	if( m_uLastUsedBulletIndex > uNumBulletsInPool - 1 )
		m_uLastUsedBulletIndex = 0;
	
	t2dSceneObject* pBulletToFire = m_BulletPool[ m_uLastUsedBulletIndex ];
	if( pBulletToFire )
	{
		// RKS FUCK: TEMPORARY!!!!
		if( pBulletToFire->getComponentCount() > 0 )
		{
			// Play the shoot sound
			if( alxIsPlaying( g_hShootSound ) )
			{
				alxStop( g_hShootSound );
				g_hShootSound = NULL_AUDIOHANDLE;
			}
			alxPlay( g_pShootSound );
			
			CComponentPlayerBullet* pBulletComponent = static_cast<CComponentPlayerBullet*>( pBulletToFire->getComponent( 0 ) );
			t2dVector vFirePosition = m_pOwnerObject->getPosition();
			vFirePosition.mY += 16.0f;
			pBulletComponent->Spawn( vFirePosition );
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------





