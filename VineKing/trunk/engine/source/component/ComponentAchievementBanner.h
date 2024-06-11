//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTACHIEVEMENTBANNER_H
#define COMPONENTACHIEVEMENTBANNER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentAchievementBanner : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentAchievementBanner );
		
		CComponentAchievementBanner();
		virtual ~CComponentAchievementBanner();
	
		static void ReportAchievement( int );
		static void DisplayAchievement( int );
		static void DisplayAchievement( const char* );
		static void ReportAchievementPercentage( int, float );
	
		static void OnLoadLevelScheduled();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void HandleOwnerPositionTargetReached();
		virtual void OnPostInit();
	
	private:
		void UpdateStateDisplaying();
		void UpdateStateLoadingQueued();
	
	private:
		enum EBannerState
		{
			BANNER_STATE_NONE = -1,
			BANNER_STATE_HIDDEN = 0,	// Not displayed
			BANNER_STATE_MOVING_DOWN,	// Moving down to its max position for display
			BANNER_STATE_DISPLAYING,	// Displayed and sitting at its max position
			BANNER_STATE_MOVING_UP,		// Moving up to its start position for hiding
			BANNER_STATE_DISABLE,		// Only set when loading a level. This is to prevent any manipulation of the objects.
			BANNER_STATE_LOADING_QUEUED,// This is set if there have been queued Achievements to display after a load.
		};
		
	private:
		t2dSceneObject* m_pOwner;
		
		EBannerState m_eBannerState;
		F32 m_fDisplayTimer;
	
		static Vector<int> sm_AchievementBannerQueue;
		static Vector<const char*> sm_AchievementBannerTitleQueue;
	
		static CComponentAchievementBanner* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTACHIEVEMENTBANNER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------