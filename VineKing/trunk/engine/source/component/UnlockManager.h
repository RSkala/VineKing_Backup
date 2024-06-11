//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef UNLOCKMANAGER_H
#define UNLOCKMANAGER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "platform/platformAssert.h"
#include "platform/types.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CUnlockManager
{
	public:
		CUnlockManager();
		~CUnlockManager();
		
		static inline CUnlockManager& GetInstance();
		
		static void Initialize();
	
		const char* GetMapButtonImageName();
		S32 GetLockedImageCellFrame();
		S32 GetUnlockedImageCellFrame();
		S32 GetCompletedImageCellFrame();
		
	private:
		static void CreateInstance();
		static void DeleteInstance(); // TODO: Call this on shutdown
		
	private:
		static CUnlockManager* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CUnlockManager& CUnlockManager::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CUnlockManager::sm_pInstance == NULL" );
	return *sm_pInstance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // UNLOCKMANAGER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------