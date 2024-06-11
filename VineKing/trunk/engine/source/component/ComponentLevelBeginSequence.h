//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTLEVELBEGINSEQUENCE_H
#define COMPONENTLEVELBEGINSEQUENCE_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentLevelBeginSequence : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentLevelBeginSequence );
		
		CComponentLevelBeginSequence();
		virtual ~CComponentLevelBeginSequence();
		
		static inline CComponentLevelBeginSequence& GetInstance();
		static bool DoesLevelHaveBeginSequence() { return sm_pInstance != NULL; }
	
		void StartLevelBeginSequence() {  m_eLevelBeginState = LEVEL_BEGIN_STATE_READY; }
		bool HasLevelBeginSequenceFinished() const { return m_bLevelBeginSequenceFinished; }
	
		static void InitializeBeginSequenceObjects();
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		enum ELevelBeginState
		{
			LEVEL_BEGIN_STATE_NONE = 0,
			LEVEL_BEGIN_STATE_READY,
			LEVEL_BEGIN_STATE_SET,
			LEVEL_BEGIN_STATE_GO,
			LEVEL_BEGIN_STATE_FINISHED,
		};
		
	private:
		t2dSceneObject* m_pOwner;
		static CComponentLevelBeginSequence* sm_pInstance;
	
		F32 m_fTimer;
		ELevelBeginState m_eLevelBeginState;
		bool m_bLevelBeginSequenceFinished;
	
		bool m_bSmokeActive;
		F32 m_fSmokeTimer;
	
		F32 m_fPostLandingTimer;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static inline*/ CComponentLevelBeginSequence& CComponentLevelBeginSequence::GetInstance()
{
	AssertFatal( sm_pInstance != NULL, "CComponentLevelBeginSequence::sm_pInstance == NULL" );
	return *sm_pInstance;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTLEVELBEGINSEQUENCE_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

