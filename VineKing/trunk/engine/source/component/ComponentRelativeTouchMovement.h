//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTRELATIVETOUCHMOVEMENT_H
#define COMPONENTRELATIVETOUCHMOVEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dAnimatedSprite;
class t2dVector;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentRelativeTouchMovement : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentRelativeTouchMovement );
		
		CComponentRelativeTouchMovement();
		virtual ~CComponentRelativeTouchMovement();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		// Inherited from SimComponent, but added by me
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
		
	private:
		void FireBullets();
		
	private:
		t2dAnimatedSprite* m_pOwnerObject; // The Health Bar object will be the owner of this component
		t2dVector m_vPreviousWorldMousePoint; // Not sure if this is even needed
		
		const char* m_pszBulletObjectName; // Name of the object that will be used to create bullets
		t2dSceneObject* m_pBulletObject;
		
		Vector<t2dSceneObject*> m_BulletPool;
		
		bool m_bIsFiringBullets;
		F32 m_fBulletFireTimer;
		U32 m_uLastUsedBulletIndex;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTRELATIVETOUCHMOVEMENT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------