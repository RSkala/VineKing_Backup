//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSPLITTER_H
#define COMPONENTENEMYSPLITTER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dAnimatedSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySplitter : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemySplitter );
		
		// Constructor
		CComponentEnemySplitter();
		virtual ~CComponentEnemySplitter() {}
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		virtual void HandleOwnerPositionTargetReached();
		
		// These were added to SimComponent by me and are inherited from that class
		virtual void OnMouseDown( const t2dVector& );
		virtual void OnMouseUp( const t2dVector& );
		virtual void OnMouseDragged( const t2dVector& );
		virtual void OnPostInit();
	
	private:
		void MoveToNextTile();
		
	private:
		t2dAnimatedSprite* m_pAnimatedSpriteOwner; // The object that this component is assigned to (the owner of this component)
		bool m_bSpawned;
	
		const char* m_pszInitialFormAnimName;
		const char* m_pszSplitFormAnimName;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSPLITTER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------