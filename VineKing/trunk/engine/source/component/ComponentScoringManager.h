//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSCORINGMANAGER_H
#define COMPONENTSCORINGMANAGER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;
class t2dStaticSprite;
class t2dTextObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentScoringManager : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentScoringManager );
		
		CComponentScoringManager();
		virtual ~CComponentScoringManager();
	
		static inline CComponentScoringManager& GetInstance();
	
		void IncrementNumKilledEnemyA()			{ ++m_iNumKilledEnemyA;			OnIncrementNumKilledEnemyA();		}
		void IncrementNumKilledEnemyB()			{ ++m_iNumKilledEnemyB;			OnIncrementNumKilledEnemyB();		}
		void IncrementNumKilledSmokeMonster()	{ ++m_iNumKilledSmokeMonster;	OnIncrementNumKilledSmokeMonster(); }
		void IncrementNumSeedsEaten()			{ ++m_iNumSeedsEaten;			OnIncrementNumSeedsEaten();			}
		void IncrementNumGrassTilesGrown()		{ ++m_iNumGrassTilesGrown;		OnIncrementNumGrassTilesGrown();	}
		void IncrementNumCrystalsDestroyed()	{ ++m_iNumCrystalsDestroyed;	OnIncrementNumCrystalsDestroyed();	}
	
		void OpenScoreScreen();
	
		S32 GetTotalLevelScore() const { return m_iTotalScore; }
	
		static void InitializeScoreScreenPositions();
		
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
		void InitializePositionsForScoreScreen01();
		void InitializePositionsForScoreScreen02();
		void InitializePositionsForScoreScreen03();
		void InitializePositionsForScoreScreen04();
		void InitializePositionsForScoreScreenBoss();
	
		void SetTextObjectFromName( t2dTextObject*&, const char* );
		void InitializeTextObjectData( t2dTextObject*& );
		
		void OnIncrementNumKilledEnemyA();
		void OnIncrementNumKilledEnemyB();
		void OnIncrementNumKilledSmokeMonster();
		void OnIncrementNumSeedsEaten();
		void OnIncrementNumGrassTilesGrown();
		void OnIncrementNumCrystalsDestroyed();
	
		void CalculateScores();
		void DisplayScoreTexts();
		void DisplayScoreText( t2dTextObject* const, S32& );
		void DisplayScoreTextWithZeroes( t2dTextObject* const, S32& );
	
		void CalculateAndDisplayGameTime();
		void CalculateAndDisplayStarRanking();
		void GetStarRankingValues( F32&, F32& );
	
		void UpdateFade();
		void OnFadeOutFinished();
		void OnFadeInFinished();
	
	private:
		enum EFadeState
		{
			FADE_STATE_NONE = 0,
			FADE_STATE_FADING_IN,
			FADE_STATE_FADING_OUT,
		};
		
	private:
		t2dSceneObject* m_pOwner; // The owner of this will be the "OK" button
	
		const char* m_pszScoreScreenBGName;
		t2dSceneObject* m_pScoreScreenBG;
	
		///////////////////////////////////////////////////////
		// Count Text
		const char* m_pszTextNumKilledA;
		t2dTextObject* m_pTextNumKilledA;
	
		const char* m_pszTextNumKilledB;
		t2dTextObject* m_pTextNumKilledB;
	
		const char* m_pszTextNumKilledSmoke;
		t2dTextObject* m_pTextNumKilledSmoke;
	
		const char* m_pszTextNumSeedsEaten;
		t2dTextObject* m_pTextNumSeedsEaten;
	
		const char* m_pszTextGrassTilesGrown;
		t2dTextObject* m_pTextGrassTilesGrown;
	
		const char* m_pszTextCrystalsDestroyed;
		t2dTextObject* m_pTextCrystalsDestroyed;
	
		const char* m_pszTextHealthRemaining;
		t2dTextObject* m_pTextHealthRemaining;
	
		const char* m_pszTextManaRemaining;
		t2dTextObject* m_pTextManaRemaining;
	
		///////////////////////////////////////////////////////
		// Score Text
		const char* m_pszTextScoreNumKilledA;
		t2dTextObject* m_pTextScoreNumKilledA;
		
		const char* m_pszTextScoreNumKilledB;
		t2dTextObject* m_pTextScoreNumKilledB;
		
		const char* m_pszTextScoreNumKilledSmoke;
		t2dTextObject* m_pTextScoreNumKilledSmoke;
		
		const char* m_pszTextScoreNumSeedsEaten;
		t2dTextObject* m_pTextScoreNumSeedsEaten;
		
		const char* m_pszTextScoreGrassTilesGrown;
		t2dTextObject* m_pTextScoreGrassTilesGrown;
		
		const char* m_pszTextScoreCrystalsDestroyed;
		t2dTextObject* m_pTextScoreCrystalsDestroyed;
		
		const char* m_pszTextScoreHealthRemaining;
		t2dTextObject* m_pTextScoreHealthRemaining;
		
		const char* m_pszTextScoreManaRemaining;
		t2dTextObject* m_pTextScoreManaRemaining;
	
		const char* m_pszTextScoreTotal;
		t2dTextObject* m_pTextScoreTotal;
	
		t2dTextObject* m_pGameTimerText;
		t2dStaticSprite* m_pStarMarker;
	
		static CComponentScoringManager* sm_pInstance;
	
		S32 m_iNumKilledEnemyA;
		S32 m_iNumKilledEnemyB;
		S32 m_iNumKilledSmokeMonster;
		S32 m_iNumSeedsEaten;
		S32 m_iNumGrassTilesGrown;
		S32 m_iNumCrystalsDestroyed;
	
		S32 m_iScoreEnemyA;
		S32 m_iScoreEnemyB;
		S32 m_iScoreSmokeMonster;
		S32 m_iScoreSeedsEaten;
		S32 m_iScoreGrassTilesGrown;
		S32 m_iScoreCrystalsDestroyed;
	
		S32 m_iHealthScore;
		S32 m_iManaScore;
	
		S32 m_iTotalScore;
	
		S32 m_iStarRanking;
	
		// Fade Stuff
		EFadeState m_eFadeState;
		F32 m_fFadeTimer;
		F32 m_fFadeAmount;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentScoringManager& CComponentScoringManager::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentScoringManager::sm_pInstance == NULL" );
	return *sm_pInstance;
}

#endif // COMPONENTSCORINGMANAGER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

