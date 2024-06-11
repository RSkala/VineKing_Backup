//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLANDBRICK_H
#define COMPONENTLANDBRICK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLandBrick : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	enum ELandBrickState
	{
		LAND_BRICK_STATE_NONE = 0,
		LAND_BRICK_STATE_PURE_STONE,	// Pure stone
		LAND_BRICK_STATE_BREAKING,		// In the middle of shattering
		LAND_BRICK_STATE_GROWING,		// Growing grass
		LAND_BRICK_STATE_FULLY_GROWN,	// Grass fully grown
		LAND_BRICK_STATE_RESPAWNING,	// Turning back into pure stone
		
		LAND_BRICK_STATE_WIN_SEQUENCE,	// The tiles being affected by the player's shock wave
		LAND_BRICK_STATE_LOSE_SEQUENCE,
	};
	
	public:
		DECLARE_CONOBJECT( CComponentLandBrick );
		
		CComponentLandBrick();
		virtual ~CComponentLandBrick();
	
		bool IsDangerBrick() const { return m_bIsDangerBrick; }
		bool IsUnbreakable() const { return m_bIsUnbreakable; }
		void EnableCollision();
		void DisableCollision();
		bool IsAttackable() const;
		void DealDamage( const S32& );
		bool IsFullyGrown() const { return m_eLandBrickState == LAND_BRICK_STATE_FULLY_GROWN; }
	
		void GetOwnerPosition( t2dVector& );
	
		bool IsBeingFiredUpon() const { return m_bIsBeingFiredUpon; }
		void SetIsBeingFiredUpon() { m_bIsBeingFiredUpon = true; }
	
		void SetIsInCollisionEnableList() { m_bIsInCollisionEnableList = true; }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void HandleOwnerCollision( t2dPhysics::cCollisionStatus* );
		virtual void OnPostInit();
	
		static void AddLandBrickDataToPathGrid();
		static void ClearAllLandBrickData();
    
        static void UnbreakAllLandBricks();
		
	private:
		void ConvertGrassFrameNumberStringToEnum();
		void UpdateBrickBreak();
		void UpdateGrassGrow();
		void OnGrassFullyGrown();
		void UpdateBrickRespawning();
		void UpdateBrickWinSequence();
		void UpdateBrickLoseSequence();
		
		void HandleCollisionGameplay();
		void HandleCollisionWinSequence();
		void HandleCollisionLoseSequence();
	
	private:
		t2dStaticSprite* m_pOwnerObject; // The object that owns this component.
	
		ELandBrickState m_eLandBrickState;
		S32 m_iCurrentBrickFrameIndex;
		S32 m_iCurrentGrassFrameIndex;
		S32 m_iCurrentBrickRespawnFrameIndex;
		F32 m_fTileChangeTimer;
	
		//const char* m_pszGrassFrameNumber;	// The grass frame number from the editor
		S32 m_iGrassFrameNumber;			// The grass frame number
		S32 m_iOriginalBrickFrameNumber;	// The original brick frame number
		
		bool m_bIsDangerBrick;
		bool m_bIsUnbreakable;
		bool m_bIsDoubleBrick;
	
		S32 m_iCurrentLandBrickHealth;
	
		bool m_bIsBeingFiredUpon; // This variable is for the Serpent spitting, so that a brick is not selected more than once to be fired upon (It still could be a valid target before a bullet hits it.)
	
		bool m_bIsInCollisionEnableList; // This is to fix the issue where a fully grown land brick that was underneath a double brick will still respond to collision.
		F32 m_fDangerBrickCollisionTimer;
	
		static Vector<CComponentLandBrick*> m_LandBrickList;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLANDBRICK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------