//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLEVELUPSCREEN_H
#define COMPONENTLEVELUPSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLevelUpScreenElement;
class t2dAnimatedSprite;
class t2dSceneObject;
class t2dStaticSprite;
class t2dTextObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLevelUpScreen : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		enum EFadeState
		{
			FADE_STATE_NONE = 0,
			FADE_STATE_FADING_IN,
			FADE_STATE_FADING_OUT,
		};
	
		enum EBuyButtonState
		{
			BUY_BUTTON_STATE_NONE = 0,
			BUY_BUTTON_STATE_GREEN,
			BUY_BUTTON_STATE_BLUE,
			BUY_BUTTON_STATE_PURPLE,
		};
	
	public:
		DECLARE_CONOBJECT( CComponentLevelUpScreen );
		
		CComponentLevelUpScreen();
		virtual ~CComponentLevelUpScreen();
		
		static inline CComponentLevelUpScreen& GetInstance();
	
		void OpenLevelUpScreen();
		void CloseLevelUpScreen();
	
		void AddElement( CComponentLevelUpScreenElement* const );
		void SetLastPressedElement( t2dSceneObject* const _pObject ) { CComponentLevelUpScreen::sm_pLastPressedElement = _pObject; }
		t2dSceneObject* GetLastPressedElement() const { return CComponentLevelUpScreen::sm_pLastPressedElement; }
	
		void SetHealthBarObject( t2dSceneObject* const _pObject ) { m_pHealthBarObject = _pObject; }
		t2dSceneObject* GetHealthBarObject() const { return m_pHealthBarObject; }
		void SetHealthBarInitialPosition( const t2dVector& _vPosition ) { m_vHealthBarInitialPosition = _vPosition; }
		void SetHealthBarPositionMarker( t2dSceneObject* const _pObject ) { m_pHealthBarPositionMarker = _pObject; }
		
		void SetSpeedBarObject( t2dSceneObject* const _pObject ) { m_pSpeedBarObject = _pObject; }
		t2dSceneObject* GetSpeedBarObject() const { return m_pSpeedBarObject; }
		void SetSpeedBarInitialPosition( const t2dVector& _vPosition ) { m_vSpeedBarInitialPosition = _vPosition; }
		void SetSpeedBarPositionMarker( t2dSceneObject* const _pObject ) { m_pSpeedBarPositionMarker = _pObject; }
	
		void SetManaBarObject( t2dSceneObject* const _pObject ) { m_pManaBarObject = _pObject; }
		t2dSceneObject* GetManaBarObject() const { return m_pManaBarObject; }
		void SetManaBarInitialPosition( const t2dVector& _vPosition ) { m_vManaBarInitialPosition = _vPosition; }
		void SetManaBarPositionMarker( t2dSceneObject* const _pObject ) { m_pManaBarPositionMarker = _pObject; }
	
		void InitializeElements();
	
		void OnHealthButtonPressed();
		void OnHealthButtonReleased();
	
		void OnSpeedButtonPressed();
		void OnSpeedButtonReleased();
	
		void OnManaButtonPressed();
		void OnManaButtonReleased();
	
		void SetRedButtonObject( t2dSceneObject* const _pObject ) { m_pRedButton = _pObject; }
		void SetGreenButtonObject( t2dSceneObject* const _pObject ) { m_pGreenButton = _pObject; }
		void SetBlueButtonObject( t2dSceneObject* const _pObject ) { m_pBlueButton = _pObject; }
	
		void SetTotalXPTextObject( t2dTextObject* const _pObject ) { m_pTotalXPText = _pObject; }
	
		void PlayVineKingChant();
		void StopVineKingChant();
	
		void OnBackButtonPressed();
		void OnHelpButtonPressed();
	
		void OnGreenButtonPressed();
		void OnBlueButtonPressed();
		void OnPurpleButtonPressed();
	
		void OnBuyButtonPressed();
	
		void UpdateTotalXPTextDisplay();
		void UpdatePriceDisplay();
		void UpdateMarkerDisplay();
		void PlayPurchaseEffect();
	
		void EnableRPGScreenButtonsFromTutorial();
	
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
	
		void DisplayTutorial();
	
		void SetDefaultMarkerAttributes( t2dSceneObject* const );
		
	private:
		t2dSceneObject* m_pOwner;
		static CComponentLevelUpScreen* sm_pInstance;
	
		Vector<CComponentLevelUpScreenElement*> m_ElementList;
	
		static t2dSceneObject* sm_pLastPressedElement;
	
		t2dSceneObject* m_pHealthBarObject;
		t2dSceneObject* m_pSpeedBarObject;
		t2dSceneObject* m_pManaBarObject;
	
		t2dSceneObject* m_pHealthBarPositionMarker;
		t2dSceneObject* m_pSpeedBarPositionMarker;
		t2dSceneObject* m_pManaBarPositionMarker;
	
		t2dVector m_vHealthBarInitialPosition;
		t2dVector m_vSpeedBarInitialPosition;
		t2dVector m_vManaBarInitialPosition;
	
		t2dVector m_vHealthBarMaxPosition;
		t2dVector m_vSpeedBarMaxPosition;
		t2dVector m_vManaBarMaxPosition;
	
		F32 m_fHealthBarYSizeDifference;
		F32 m_fSpeedBarYSizeDifference;
		F32 m_fManaBarYSizeDifference;
	
		bool m_bHealthButtonPressed;
		bool m_bSpeedButtonPressed;
		bool m_bManaButtonPressed;
		
		t2dTextObject* m_pTotalXPText;
	
		t2dSceneObject* m_pRedButton;
		t2dSceneObject* m_pGreenButton;
		t2dSceneObject* m_pBlueButton;
	
		t2dAnimatedSprite* m_pRedButtonEffect;
		t2dAnimatedSprite* m_pGreenButtonEffect;
		t2dAnimatedSprite* m_pBlueButtonEffect;
	
		t2dAnimatedSprite* m_pPillarOfLightRed;
		t2dAnimatedSprite* m_pPillarOfLightGreen;
		t2dAnimatedSprite* m_pPillarOfLightBlue;
	
		t2dAnimatedSprite* m_pRPGVineKing;
	
		//t2dAnimatedSprite* m_pVineling01;
		//t2dAnimatedSprite* m_pVineling02;
		//t2dAnimatedSprite* m_pVineling03;
	
	
		// New RPG Menu stuff
	
		t2dStaticSprite* m_pRPGScreenBG;	// Background image
		t2dStaticSprite* m_pRPGBackground;	// Black background behind image
	
		t2dStaticSprite* m_pRPGBuyButton;	// "Buy" button
		
		t2dStaticSprite* m_pRPGGreenButton;		// Green purchase button
		t2dStaticSprite* m_pRPGBlueButton;		// Blue purchase button
		t2dStaticSprite* m_pRPGPurpleButton;	// Purple purchase button
	
		t2dStaticSprite* m_pGreenMarker01;
		t2dStaticSprite* m_pGreenMarker02;
		t2dStaticSprite* m_pGreenMarker03;
		t2dStaticSprite* m_pGreenMarker04;
		t2dStaticSprite* m_pGreenMarker05;
		t2dStaticSprite* m_pGreenMarker06;
	
		t2dStaticSprite* m_pBlueMarker01;
		t2dStaticSprite* m_pBlueMarker02;
		t2dStaticSprite* m_pBlueMarker03;
		t2dStaticSprite* m_pBlueMarker04;
		t2dStaticSprite* m_pBlueMarker05;
		t2dStaticSprite* m_pBlueMarker06;
		
		t2dStaticSprite* m_pPurpleMarker01;
		t2dStaticSprite* m_pPurpleMarker02;
		t2dStaticSprite* m_pPurpleMarker03;
		t2dStaticSprite* m_pPurpleMarker04;
		t2dStaticSprite* m_pPurpleMarker05;
		t2dStaticSprite* m_pPurpleMarker06;
	
		t2dStaticSprite* m_pRPGMapButton;	// Goes back to the Map Screen
		t2dStaticSprite* m_pRPGHelpButton;	// Displays the tutorial
	
		t2dStaticSprite* m_pPriceGreen;
		t2dStaticSprite* m_pPriceBlue;
		t2dStaticSprite* m_pPricePurple;
	
		t2dAnimatedSprite* m_pPurchaseEffect;
			
		EBuyButtonState m_eBuyButtonState;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentLevelUpScreen& CComponentLevelUpScreen::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentLevelUpScreen::sm_pInstance == NULL" );
	return *sm_pInstance;
}

#endif // COMPONENTLEVELUPSCREEN_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

