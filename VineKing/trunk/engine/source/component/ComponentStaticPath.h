//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTSTATICPATH_H
#define COMPONENTSTATICPATH_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Forward Declarations

class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentStaticPath : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
	public:
		DECLARE_CONOBJECT( CComponentStaticPath );
	
		CComponentStaticPath();
		virtual ~CComponentStaticPath();
	
		S32 GetNumNodePositions() const { return m_NodePositionList.size(); }
		void GetPositionAtNodeIndex( const S32&, t2dVector& );
		S32 GetEndNodeIndex() const { return m_NodePositionList.size() - 1; }
	
		static S32 GetNumStaticPaths() { return m_StaticPathList.size(); }
		static CComponentStaticPath* GetStaticPathAtIndex( const S32& _iIndex ) { return m_StaticPathList[ _iIndex ]; }
	
		static void ClearStaticPathList() { m_StaticPathList.clear(); }
		
		// Inherited from ConsoleObject
		static void initPersistFields();
		
		// Inherited from SimComponent
		virtual bool onComponentAdd( SimComponent* );
		//virtual void onUpdate();
		virtual void OnPostInit();
	
	private:
		void SetUpNodePositions();
		void AssignPositionFromNodeText( const char* );
		
	private:
		t2dSceneObject* m_pOwner;
	
		Vector<t2dVector> m_NodePositionList;
	
		// Strings to find node objects
		const char* m_pszNode01;
		const char* m_pszNode02;
		const char* m_pszNode03;
		const char* m_pszNode04;
		const char* m_pszNode05;
		const char* m_pszNode06;
		const char* m_pszNode07;
		const char* m_pszNode08;
		const char* m_pszNode09;
		const char* m_pszNode10;
		const char* m_pszNode11;
		const char* m_pszNode12;
		const char* m_pszNode13;
		const char* m_pszNode14;
		const char* m_pszNode15;
		const char* m_pszNode16;
		const char* m_pszNode17;
		const char* m_pszNode18;
		const char* m_pszNode19;
		const char* m_pszNode20;
		const char* m_pszNode21;
		const char* m_pszNode22;
		const char* m_pszNode23;
		const char* m_pszNode24;
		const char* m_pszNode25;
		const char* m_pszNode26;
		const char* m_pszNode27;
		const char* m_pszNode28;
		const char* m_pszNode29;
		const char* m_pszNode30;
	
		static Vector<CComponentStaticPath*> m_StaticPathList;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTSTATICPATH_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------