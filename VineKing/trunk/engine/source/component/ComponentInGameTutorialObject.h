//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTINGAMETUTORIALOBJECT_H
#define COMPONENTINGAMETUTORIALOBJECT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dStaticSprite;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentInGameTutorialObject : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum ETutorialType
		{
			TUTORIAL_NONE = 0,
			TUTORIAL_MAP_RPG,			// Displayed in Map Screen / Power Up Screen
			TUTORIAL_SERPENT,			// Displayed in Level_03_01	
			TUTORIAL_CRYSTAL,			// Displayed in Level_03_02
			TUTORIAL_LAVAPIT,			// Displayed in Level_04_01
			TUTORIAL_SMOKE,				// Displayed in Level_04_02
			TUTORIAL_BOSS,				// Displayed in Boss_01
			TUTORIAL_SUPER_BLOB,		// Displayed in Level_05_03
			TUTORIAL_SUPER_SERPENT,		// Displayed in Level_06_02
			TUTORIAL_SUPER_SMOKE,		// Displayed in Level_07_01
		};
	
	public:
		DECLARE_CONOBJECT( CComponentInGameTutorialObject );
		
		CComponentInGameTutorialObject();
		virtual ~CComponentInGameTutorialObject();
		
		static void Initialize();
	
		static void ResetInGameTutorialData();
		static bool DoesCurrentLevelHaveTutorial() { return sm_bLevelHasTutorial; }
		static void SetTutorialType( ETutorialType _eTutorialType ) { sm_eTutorialType = _eTutorialType; }
		static void ActivateTutorial();
		static void ActivateMapTutorial();
		static void ActivateRPGTutorial();
		static void ActivateSmokeTutorial();
		static void ActivateBossTutorial();
		static void ActivateShopReminderTutorial();
		static void ActivateDLCMapAnnouncementTutorial();
		static void DeactivateTutorial();
	
		static bool DoesLevelHaveMapTutorial()			{ return sm_eTutorialType == TUTORIAL_MAP_RPG;		}
		//static bool DoesLevelHaveRPGTutorial()
		static bool DoesLevelHaveSerpentTutorial()		{ return sm_eTutorialType == TUTORIAL_SERPENT;		}
		static bool DoesLevelHaveCrystalTutorial()		{ return sm_eTutorialType == TUTORIAL_CRYSTAL;		}
		static bool DoesLevelHaveLavaPitTutorial()		{ return sm_eTutorialType == TUTORIAL_LAVAPIT;		}
		static bool DoesLevelHaveSmokeTutorial()		{ return sm_eTutorialType == TUTORIAL_SMOKE;		}
		static bool DoesLevelHaveBossTutorial()			{ return sm_eTutorialType == TUTORIAL_BOSS;			}
		static bool DoesLevelHaveSuperBlobTutorial()	{ return sm_eTutorialType == TUTORIAL_SUPER_BLOB;	}
		static bool DoesLevelHaveSuperSerpentTutorial() { return sm_eTutorialType == TUTORIAL_SUPER_SERPENT;}
		static bool DoesLevelHaveSuperSmokeTutorial()	{ return sm_eTutorialType == TUTORIAL_SUPER_SMOKE;	}
	
		static bool IsTutorialActive() { return sm_eTutorialState != TUTORIAL_STATE_NONE; }
	
	
		//// OLD SHIT BELOW
		static bool IsInGameTutorialActive() { return sm_bTutorialActive; }
		static void SetInGameTutorialActive();
		static void SetInGameTutorialInactive();
	
	
		static void SetInPowerUpScreen() { sm_bIsInPowerUpScreen = true; }
		static void SetNotInPowerUpScreen() { sm_bIsInPowerUpScreen = false; }
		static bool IsInPowerUpScreen() { return sm_bIsInPowerUpScreen; }

		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnMouseDown( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseUp( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseEnter( const t2dVector& _vWorldMousePoint );
		virtual void OnMouseLeave( const t2dVector& _vWorldMousePoint );
		virtual void OnPostInit();
	
	private:
		enum ETutorialObjectType
		{
			OBJECT_TYPE_NONE = 0,
			OBJECT_TYPE_IMAGE,
			OBJECT_TYPE_BUTTON,
		};
	
		enum ETutorialState
		{
			TUTORIAL_STATE_NONE = 0,
			
			TUTORIAL_STATE_DISPLAY_SCALING,
			TUTORIAL_STATE_DISPLAY_FULL_SIZE,
		};
		
	private:
		t2dStaticSprite* m_pOwner;
	
		ETutorialObjectType m_eObjectType;
	
		static ETutorialState sm_eTutorialState;
		static ETutorialType sm_eTutorialType;
	
		static bool sm_bTutorialActive;
	
		static bool sm_bOpenedRPGTutorial; // This will only be used for the Map/RPG screens
		
		static t2dStaticSprite* sm_pImage;
		static t2dStaticSprite* sm_pButton;
	
		static bool sm_bLevelHasTutorial;
		static bool sm_bIsInPowerUpScreen;
	
		static bool sm_bTimeScaleStopped;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTINGAMETUTORIALOBJECT_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------