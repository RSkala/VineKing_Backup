//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTMANASEEDITEMDROP_H
#define COMPONENTMANASEEDITEMDROP_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
//class CComponentManaSeed;
class t2dAnimatedSprite;
class t2dVector;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentManaSeedItemDrop : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
		
	public:
		DECLARE_CONOBJECT( CComponentManaSeedItemDrop );
		
		CComponentManaSeedItemDrop();
		virtual ~CComponentManaSeedItemDrop();
	
		bool IsInUse() const { return m_bInUse; } // Whether the Owner is visible
		void SetManaSeedNotInUse();
		S32 GetManaValue() const { return m_iManaValue; }
	
		static void SpawnSeedFromGrassAtPosition( const t2dVector& );
		static void SpawnSeedFromEnemyAtPosition( const t2dVector& );
	
		static CComponentManaSeedItemDrop* GetRandomVisibleSeed();
	
		void GetOwnerPosition( t2dVector& );
	
		void SetOwnerUseMouseEvents();
	
		static void InitializeManaSeedDropData();
		static void HideAllSeeds();
	
		static bool AreThereAnySeedsActive();
	
		static void OnEndLevelSequenceStarted();
    
        static void HideAllManaSeeds();
		
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
		static void InitializeManaSeedPool( t2dAnimatedSprite* const );
		static void AddObjectToManaSeedPool( t2dAnimatedSprite* const );
		static void ClearManaSeedPool();

		static CComponentManaSeedItemDrop* GetNextAvailableSeed();
	
		void SpawnFromGrassAtPosition( const t2dVector& );
		void SpawnFromEnemyAtPosition( const t2dVector& );
		void SpawnAtPosition( const t2dVector& );
		
	private:
		t2dAnimatedSprite* m_pManaSeedObject; // The mana seed object that owns the component (for the first one, it will be the object that is cloned).
		bool m_bInUse;
		const char* m_pszGrassDropAnim;
		const char* m_pszEnemyDropAnim;
		S32 m_iGrassDropValue;
		S32 m_iEnemyDropValue;
		S32 m_iManaValue;
		F32 m_fActiveTimer;
	
		bool m_bBlinkedOut;
		F32 m_fBlinkTimer;
		
		static Vector<t2dAnimatedSprite*> sm_ManaSeedItemDropPool;
		static bool sm_bManaSeedItemDropPoolInitialized;
		static bool sm_bManaSeedItemDropPoolCleared;
		static t2dAnimatedSprite* sm_pCurrentlyHeldManaSeedItemDropObject;
	};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTMANASEEDITEMDROP_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

