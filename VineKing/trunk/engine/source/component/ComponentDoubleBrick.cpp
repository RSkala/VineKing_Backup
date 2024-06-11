//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentDoubleBrick.h"

#include "ComponentGlobals.h"
#include "ComponentLandBrick.h"
#include "ComponentManaBar.h"
#include "ComponentPathGridHandler.h"
#include "ComponentScoringManager.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szDoubleBrickImageMapName[32] = "lava_tile_cell_01ImageMap"; // Image Map for the Land Bricks (and Double Bricks)
static const S32 g_iDoubleBrickBreakFrames[] = { 33, 33, 34, 34, 35, 35 }; // Frames of the image map that displays the breaking Double Brick tiles
static const S32 g_iNumDoubleBrickBreakFrames = sizeof(g_iDoubleBrickBreakFrames) / sizeof(S32);

static const F32 g_fBrickBreakTileChangeTimeMS = 50.0f; //100.0f;

static const S32 g_iDoubleBrickLayerNumber = 14;

//Mana drain system
extern F32 g_fTotalManaDrained;
extern F32 g_fBrickDestroyManaDrain;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

Vector<CComponentDoubleBrick*> CComponentDoubleBrick::m_DoubleBrickList;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentDoubleBrick );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentDoubleBrick::CComponentDoubleBrick()
	: m_pOwnerObject( NULL )
	, m_pAssociatedLandBrick( NULL )
	, m_bIsBreaking( false )
	, m_fBreakTimer( 0.0f )
	, m_iCurrentBreakFrameIndex( -1 )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentDoubleBrick::~CComponentDoubleBrick()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentDoubleBrick::SetLandBricksForAllDoubleBricks()
{
	S32 iNumDoubleBricks = m_DoubleBrickList.size();

	CComponentDoubleBrick* pDoubleBrick = NULL;
	CComponentLandBrick* pLandBrick = NULL;
	for( S32 i = 0; i < iNumDoubleBricks; ++i )
	{
		pDoubleBrick = m_DoubleBrickList[i];
		if( pDoubleBrick )
		{
			// Get the Land Brick that is beneath this Double Brick
			t2dVector vPosition;
			pDoubleBrick->GetOwnerPositionWithOffset( vPosition );
			pLandBrick = CComponentPathGridHandler::GetInstance().GetLandBrickAtPosition( vPosition );
			
			if( pLandBrick )
			{
				// Set the associated land brick
				pDoubleBrick->SetAssociatedLandBrick( pLandBrick );
				
				// Disable the Land Brick that is beneath this Double Brick
				pLandBrick->DisableCollision();
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentDoubleBrick::DisableCollisionOnAllDoubleBricks()
{
	CComponentDoubleBrick* pDoubleBrick = NULL;
	
	S32 iNumDoubleBricks = m_DoubleBrickList.size();
	for( S32 i = 0; i < iNumDoubleBricks; ++i )
	{
		pDoubleBrick = m_DoubleBrickList[i];
		if( pDoubleBrick )
			pDoubleBrick->DisableCollision();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentDoubleBrick::initPersistFields()
{
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwnerObject, CComponentDoubleBrick ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentDoubleBrick::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dAnimatedSprite
	t2dStaticSprite* pOwnerObject = dynamic_cast<t2dStaticSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		printf( "%s - Must be added to a t2dStaticSprite.\n", __FUNCTION__ );
		return false;
	}
	
	m_pOwnerObject = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentDoubleBrick::onUpdate()
{
	if( m_pOwnerObject == NULL )
		return;
	
	if( m_bIsBreaking )
		UpdateBreaking();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentDoubleBrick::HandleOwnerCollision( t2dPhysics::cCollisionStatus* _pCollisionStatus )
{
	if( m_pOwnerObject == NULL ||  _pCollisionStatus == NULL )
		return;
	
	m_pOwnerObject->setCollisionActive( false, false );
	m_pOwnerObject->setCollisionCallback( false );
	
	// Enable the Land Brick that is beneath this Double Brick. NOTE: This needs to be done AFTER the line attack object has finished its path.
	//CComponentGlobals::GetInstance().AddLandBrickToCollisionEnableList( m_pAssociatedLandBrick );
	
	// Play the break sound
	CSoundManager::GetInstance().PlayCrystalBreakSound();
	
	// Start the breaking animation
	m_iCurrentBreakFrameIndex = 0;
	m_pOwnerObject->setImageMap( g_szDoubleBrickImageMapName, g_iDoubleBrickBreakFrames[m_iCurrentBreakFrameIndex++] );
	m_bIsBreaking = true;
	
	//DM This is the NOGRASSMANADRAIN code
	CComponentManaBar::SubtractMana( g_fBrickDestroyManaDrain );
	g_fTotalManaDrained += g_fBrickDestroyManaDrain;
	
	CComponentScoringManager::GetInstance().IncrementNumCrystalsDestroyed();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentDoubleBrick::OnPostInit()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setLayer( LAYER_CRYSTALS );
	
	S32 iNewLayerOrder = static_cast<S32>( m_pOwnerObject->getPosition().mY + 240.0f );
	if( iNewLayerOrder != m_pOwnerObject->getLayerOrder() )
		m_pOwnerObject->setLayerOrder( iNewLayerOrder );
	
	m_pOwnerObject->setUseMouseEvents( false );
	m_pOwnerObject->setCollisionActive( false, true );
	m_pOwnerObject->setCollisionCallback( true );
	
	m_DoubleBrickList.push_back( this );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentDoubleBrick::GetOwnerPositionWithOffset( t2dVector& _vOutPosition )
{
	if( m_pOwnerObject )
	{
		_vOutPosition = m_pOwnerObject->getPosition();
		
		// Since a double brick is aligned off-center, select from a position 1/4 down from its center
		_vOutPosition.mY += (m_pOwnerObject->getSize().mY * 0.25f);
	}
	else 
	{
		_vOutPosition = t2dVector::getZero();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentDoubleBrick::UpdateBreaking()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_fBreakTimer += ITickable::smTickMs;
	if( m_fBreakTimer < g_fBrickBreakTileChangeTimeMS )
		return;
	
	m_fBreakTimer = 0.0f;
	
	if( m_iCurrentBreakFrameIndex != -1 )
	{
		m_pOwnerObject->setImageMap( g_szDoubleBrickImageMapName, g_iDoubleBrickBreakFrames[m_iCurrentBreakFrameIndex++] );
		if( m_iCurrentBreakFrameIndex >= g_iNumDoubleBrickBreakFrames )
			m_iCurrentBreakFrameIndex = -1;
	}
	else 
	{
		m_pOwnerObject->setVisible( false );
		m_bIsBreaking = false;
		
		if( m_pAssociatedLandBrick )
			m_pAssociatedLandBrick->EnableCollision();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentDoubleBrick::DisableCollision()
{
	if( m_pOwnerObject == NULL )
		return;
	
	m_pOwnerObject->setCollisionActive( false, false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------