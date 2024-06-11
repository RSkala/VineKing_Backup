//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTBREAKABLETILELAYER_H
#define COMPONENTBREAKABLETILELAYER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dTileLayer;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

const S32 g_iMaxTileBreakArrayX = 10;
const S32 g_iMaxTileBreakArrayY = 15;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentBreakableTileLayer : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	struct STileInfo
	{
		F32 fTileTimer;
		bool bIsBreaking;
		bool bIsGrowing;
		bool bIsFullyGrown;
		U32 uCurrentTileIndex;
		S32 iCurrentHealth;
		bool bIsDangerBlock;
	};
	
	public:
		DECLARE_CONOBJECT( CComponentBreakableTileLayer );
		
		CComponentBreakableTileLayer();
		virtual ~CComponentBreakableTileLayer();
	
		void GetTileWorldPosition( const U32&, const U32&, t2dVector& );
	
		void BreakTile( const S32&, const S32& );
	
		U32 GetNumFullyGrownTiles() const;
	
		bool IsTileAttackable( const S32&, const S32& ) const;
	
		S32 GetTileHealth( const U32&, const U32& );
		void DealDamageToTile( const U32&, const U32&, const S32& );
		void DealDamageToTileAtPosition( const S32&, const t2dVector& );
	
		void EnableDangerBlockCollisions();
		void GetNextAttackableTilePosition( t2dVector& );
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		
		// Inherited from SimComponent, but added by me
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseDragged( const t2dVector& _vWorldMousePoint );
	
		virtual void HandleOwnerCollision( t2dPhysics::cCollisionStatus* );
		virtual void OnPostInit();
	
	private:
		void InitializeTileInfo();
		void UpdateBreak( const S32&, const S32& );
		void UpdateGrowing( const S32&, const S32& );
		void OnTileFullyGrown( const S32&, const S32& );
		void DestroyTile( const S32&, const S32& );
		
	private:
		t2dTileLayer* m_pTileLayerOwner;
	
		//bool m_bIsBreaking;
		U32 m_uCurrentTileIndex;
	
		U32 m_uTotalNumBreakableTiles;
	
		STileInfo m_aTileInfoList[ g_iMaxTileBreakArrayX ][ g_iMaxTileBreakArrayY ];
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTBREAKABLETILELAYER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------