//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLINEDRAWATTACK_H
#define COMPONENTLINEDRAWATTACK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dPath;
class t2dSceneObject;
class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLineDrawAttack : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentLineDrawAttack );
		
		CComponentLineDrawAttack();
		virtual ~CComponentLineDrawAttack() { CComponentLineDrawAttack::sm_pInstance = NULL; }
	
		static void OnInvalidLineDrawAreaTouched();
		static bool IsLineDrawAttackActive();
	
		void OnLineAttackObjectPathFinished();
	
		static void KillAllLineDrawEffects();
	
		void SetLineDrawColorDefault();
		void SetLineDrawColorMax();
    
        static void ForceLineDrawAttackPathFinished();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		void InitializeLineObjects();
		void HideAllLineDrawingObjects();
		void ClearLineDrawFromInvalidInput();
		void PrepareAllObjectsForLineDraw();
		void StartLineDrawAttack();
		
	private:
		t2dSceneObject* m_pOwner; // The object that owns this component. It is used to capture mouse events (it is a mouse-locked object).
		
		const char* m_pszLineObjectName; // No longer used
		t2dSceneObject* m_pLineObject;
	
		const char* m_pszConnectionObjectName; // No longer used
		t2dSceneObject* m_pConnectionObject;
	
		const char* m_pszPathObjectName; // No longer used
		t2dPath* m_pPathObject;
	
		const char* m_pszTestPathTraversalObjectName; // No longer used
		t2dStaticSprite* m_pPathTraversalObject;
	
		Vector<t2dSceneObject*> m_ConnectionObjectList; // The circles that connect the lines 
		Vector<t2dSceneObject*> m_LineObjectList;		// The lines that are drawn
	
		S32 m_iCurrentConnectionObjectIndex;
		S32 m_iCurrentLineObjectIndex;
	
		bool m_bFingerDown;
	
		F32 m_fPathLength;
	
		bool m_bAttackObjectActive;
		F32 m_fFollowObjectTimer;
		S32 m_iCurrentFollowObjectIndex;
		Vector<t2dSceneObject*> m_FollowObjects;
	
		bool m_bLineDrawActive;  // This will be true only when the user is actually drawing the lines
		
		t2dVector m_vFirstTouchDownPoint;
		t2dVector m_vPreviousTouchPoint;
	
		static CComponentLineDrawAttack* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLINEDRAWATTACK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
