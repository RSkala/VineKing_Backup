//-------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTEXPANDINGRING_H
#define COMPONENTEXPANDINGRING_H

//-------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

#ifndef _ITICKABLE_H_
#include "core/iTickable.h"
#endif

//-------------------------------------------------------------------------------------------------

class CComponentExpandingRing : public DynamicConsoleMethodComponent, public virtual ITickable
{
	// I put this in front because that's how Torque does it and I'm having compile errors:
	//	error: 'typedef class SimComponent DynamicConsoleMethodComponent::Parent' is private
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentExpandingRing );
		
		// Constructor
		CComponentExpandingRing();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// These are inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		
		// These are inherited from SimComponent
		virtual void onUpdate();
		virtual void onAddToScene();
		
		// RKS: I added these to SimComponent and are inherited from that class
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint );
		virtual void HandleOwnerCollision( t2dPhysics::cCollisionStatus* );
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// This method is called every frame and lets the control interpolate between ticks so you can smooth things as long as isProcessingTicks returns truewhen it is called on the object
		virtual void interpolateTick( F32 delta ) {}; // = 0;
		virtual void processTick() {}; // = 0; // This method is called once every 32ms if isProcessingTicks returns true when called on the object
		virtual void advanceTime( F32 timeDelta ) {};// = 0; 	// This method is called once every frame regardless of the return value ofisProcessingTicks and informs the object of the passage of time
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	protected:
		t2dSceneObject* m_pOwner;		// This is actually a t2dShapeVector, but I don't know if I will be using that later on.
		bool m_bPlayerTouchingScreen;
		F32 m_fExpansionTimer;
		bool m_bIsExpanding;
		F32 m_fExpansionStartTimer; // Keeps time before ring is allowed to start growing
		t2dVector m_vLastMouseDownPosition;
};

//-------------------------------------------------------------------------------------------------

#endif // COMPONENTEXPANDINGRING_H

//-------------------------------------------------------------------------------------------------
