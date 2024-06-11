//-------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTPATHGRIDHANDLER_H
#define COMPONENTPATHGRIDHANDLER_H

//-------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//-------------------------------------------------------------------------------------------------

class CComponentLandBrick;
class t2dStaticSprite;
class t2dTileLayer;
//class pathGrid2d;

const S32 g_iMaxTileX = 10;
const S32 g_iMaxTileY = 15;

//-------------------------------------------------------------------------------------------------

class CComponentPathGridHandler : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	struct STileInfo
	{
		t2dStaticSprite* pBrickObject;
		CComponentLandBrick* pLandBrickComponent;
	};
	
	public:
		DECLARE_CONOBJECT( CComponentPathGridHandler );
	
		CComponentPathGridHandler();
		virtual ~CComponentPathGridHandler();
	
		static inline CComponentPathGridHandler& GetInstance();
	
		void AddLandBrickData( t2dStaticSprite* const, CComponentLandBrick* const );
		void AddLandBrickData( CComponentLandBrick* const );
	
		void GetTileWorldPosition( const S32&, const S32&, t2dVector& ) const;
		bool IsLandBrickAttackable( const S32&, const S32& ) const;
		void EnableAllDangerBrickCollisions();
		void DealDamageToLandBrick( const S32&, const S32& , const S32& );
		void DealDamageToLandBrickAtPosition( const S32&, const t2dVector& );
		void GetNextAttackableLandBrickPosition( t2dVector& );
		S32 GetArrayRandomAttackableLandBrickPosition( Vector<t2dVector>&, const S32& ) const;
		bool GetRandomAttackableLandBrickPosition( t2dVector& );
	
		inline S32 GetNumFullyGrownLandBricks() const { return m_iNumFullyGrownLandBricks; }
	
		CComponentLandBrick* GetLandBrickAtPosition( const t2dVector& );
		
		void SetLandBrickAtPositionAsFiredUpon( const t2dVector& );
	
		void EnableAllLandBrickCollisions();
	
		S32 GetNumGrassFullyGrown() const { return m_iNumFullyGrownLandBricks; }
		void DecrementNumGrassFullyGrown() { --m_iNumFullyGrownLandBricks; if( m_iNumFullyGrownLandBricks < 0 ) AssertFatal( 0, "m_iNumFullyGrownLandBricks < 0" ); }
		void IncrementNumGrassFullyGrown() { ++m_iNumFullyGrownLandBricks; }
	
		static bool PickTileAtPosition( const t2dVector&, Point2I& );
    
        static void ResetNumFullyGrownLandBricks();
	
		// Inherited from ConsoleObject
		static void initPersistFields();
	
		// These are inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void InitializePathInfo();
	
	private:
		t2dTileLayer* m_pTileLayerOwner;
		STileInfo m_aTileInfo[g_iMaxTileX][g_iMaxTileY];
	
		S32 m_iNumFullyGrownLandBricks;
	
		static CComponentPathGridHandler* sm_pInstance;
};

//-------------------------------------------------------------------------------------------------

/*static inline*/ CComponentPathGridHandler& CComponentPathGridHandler::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentPathGridHandler::sm_pInstance == NULL\n" );
	return *sm_pInstance;
}

//-------------------------------------------------------------------------------------------------

#endif // COMPONENTPATHGRIDHANDLER_H

//-------------------------------------------------------------------------------------------------