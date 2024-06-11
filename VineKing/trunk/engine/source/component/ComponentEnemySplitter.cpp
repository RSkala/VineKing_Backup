//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentEnemySplitter.h"

#include "T2D/t2dAnimatedSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentEnemySplitter );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentEnemySplitter::CComponentEnemySplitter()
	: m_pAnimatedSpriteOwner( NULL )
	, m_bSpawned( false )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentEnemySplitter::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pAnimatedSpriteOwner, CComponentEnemySplitter ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentEnemySplitter::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dAnimatedSprite* pOwnerObject = dynamic_cast<t2dAnimatedSprite*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		Con::warnf( "CComponentEnemySplitter::onComponentAdd - Must be added to a t2dAnimatedSprite." );
		return false;
	}
	
	m_pAnimatedSpriteOwner = pOwnerObject;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySplitter::onUpdate()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySplitter::HandleOwnerPositionTargetReached()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySplitter::OnMouseDown( const t2dVector& _vWorldMousePoint )
{
	// This enemy has been touched -- split into multiple
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySplitter::OnMouseUp( const t2dVector& _vWorldMousePoint )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySplitter::OnMouseDragged( const t2dVector& _vWorldMousePoint )
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentEnemySplitter::OnPostInit()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// PRIVATE FUNCTION IMPLEMENTATIONS

void CComponentEnemySplitter::MoveToNextTile()
{
	
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------