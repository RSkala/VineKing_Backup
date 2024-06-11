//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTDOUBLEBRICK_H
#define COMPONENTDOUBLEBRICK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class CComponentLandBrick;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentDoubleBrick : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentDoubleBrick );
		
		CComponentDoubleBrick();
		virtual ~CComponentDoubleBrick();
	
		static void SetLandBricksForAllDoubleBricks();
		static void ClearDoubleBrickList() { m_DoubleBrickList.clear(); }
	
		static void DisableCollisionOnAllDoubleBricks();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void HandleOwnerCollision( t2dPhysics::cCollisionStatus* );
		virtual void OnPostInit();
	
	private:
		void SetAssociatedLandBrick( CComponentLandBrick* _pLandBrick ) { m_pAssociatedLandBrick = _pLandBrick; }
		void GetOwnerPositionWithOffset( t2dVector& );
		void UpdateBreaking();
		void DisableCollision();
		
	private:
		t2dStaticSprite* m_pOwnerObject; // The object that owns this component.
	
		CComponentLandBrick* m_pAssociatedLandBrick; // The component that belongs to the Land Brick that is beneath this Double Brick
		bool m_bIsBreaking;
		F32 m_fBreakTimer;
		S32 m_iCurrentBreakFrameIndex;
	
		static Vector<CComponentDoubleBrick*> m_DoubleBrickList;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTDOUBLEBRICK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------