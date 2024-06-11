//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTENEMYSMOKEPOSITIONLIST_H
#define COMPONENTENEMYSMOKEPOSITIONLIST_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentEnemySmokePositionList : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentEnemySmokePositionList );
		
		CComponentEnemySmokePositionList();
		virtual ~CComponentEnemySmokePositionList();
	
		static S32 GetNumSmokePositions() { return sm_pInstance ? sm_pInstance->sm_StartPositionList.size() : 0; }
		static bool GetPositionsAtIndex( const S32& _iIndex, t2dVector& _vOutStartPos, t2dVector& _vOutEndPos );
	
		static void ClearSmokePositionLists() { sm_StartPositionList.clear(); sm_EndPositionList.clear(); }
	
		static inline void MarkPositionIndexAsInvalid( const S32& );
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void OnPostInit();
	
	private:
		void SetUpStartPosition( Vector<F32>& );
		void SetUpEndPosition( Vector<F32>& );
		
	private:
		t2dSceneObject* m_pOwner;
	
		static const S32 m_iMaxNumPositions = 10;
	
		Vector<F32> m_StartPositionArray[ m_iMaxNumPositions ];
		Vector<F32> m_EndPositionArray[ m_iMaxNumPositions ];
	
		static Vector<bool> sm_bInvalidIndexArray; // true = Position at the same index is marked as invalid
	
		static Vector<t2dVector> sm_StartPositionList;
		static Vector<t2dVector> sm_EndPositionList;
		
		static CComponentEnemySmokePositionList* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTENEMYSMOKEPOSITIONLIST_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ void CComponentEnemySmokePositionList::MarkPositionIndexAsInvalid( const S32& _iIndex )
{
	if( _iIndex < 0 || _iIndex >= sm_StartPositionList.size() || _iIndex >= sm_EndPositionList.size() )
		return;
	
	sm_bInvalidIndexArray[ _iIndex ] = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------