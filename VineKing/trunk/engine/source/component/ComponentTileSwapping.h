//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTTILESWAPPING_H
#define COMPONENTTILESWAPPING_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations
class t2dTileLayer;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef Vector<F32> tFloatVector;
static const S32 g_iMaxArrayX = 10;
static const S32 g_iMaxArrayY = 15;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentTileSwapping : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	struct STileInfo
	{
		F32 fTileTimer;
		bool bIsGrowing;
		bool bIsFullyGrown;
		S32 iCurrentGrowLevel;
		S32 iCurrentHealth;
	};
	
	public:
		DECLARE_CONOBJECT( CComponentTileSwapping );
		
	
		CComponentTileSwapping();
		virtual ~CComponentTileSwapping();
	
		void InitializeTileInfo();
		void UpdateTileInfo();
	
		void IncrementTileTimer( const S32&, const S32& );
		void CheckTileSwap( const S32&, const S32& );

		void SetTileFullyGrown( const S32&, const S32& );
		void SetTileNotFullyGrown( const S32&, const S32& );
		bool IsTileFullyGrown( const S32&, const S32& ) const;
		U32 GetNumFullyGrownTiles() const;
		void GetRandomSpawnPosition( t2dVector& );
		
		void SetTileIsGrowing( const S32&, const S32& );
		void SetTileIsNotGrowing( const S32&, const S32& );
		bool IsTileGrowing( const S32&, const S32& ) const;
	
		void StartTileGrowing( const S32&, const S32& );
	
		void DestroyTile( const S32&, const S32& );
		void DestroyTileAtPosition( const t2dVector& );
	
		U32 GetNumGrowableSquares() const { return m_uTotalNumGrowableSquares; }
	
		void GetTileWorldPosition( const U32, const U32, t2dVector& );
	
		S32 GetTileHealth( const U32&, const U32& );
		void DealDamageToTile( const U32&, const U32&, const U32& );
	
		S32 GetTileGrowLevel( const U32&, const U32& );
	
		// Inherited from ConsoleObject
		static void initPersistFields();
	
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
	
	private:
		void ResetTile( const S32&, const S32& );
		void OnTileFullyGrown( const S32&, const S32& );
	
	private:
		t2dTileLayer* m_pTileLayerOwner; // The Tile Layer that has this component
	
		STileInfo m_aTileInfoList[ g_iMaxArrayX ][ g_iMaxArrayY ]; // Technically: [Columns][Rows]
	
		// RKS TODO:  Need a growable or static array to handle the tile swapping information
		// Note: tVector.h is included in componentInterface.h. componentInterface.h is included in behaviorComponent.h, so there should be no need to include that file
		//Vector<F32> m_afTileChangeTimers;
		Vector<Vector<F32> > m_afTileChangeTimers2;
		Vector<tFloatVector> m_afFloatVectorList;
	
		// RKS TODO:  Remove the tile swap data from t2dTileLayer!!!!!
	
		U32 m_uTotalNumGrowableSquares;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTTILESWAPPING_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------