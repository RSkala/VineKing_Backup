//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef COMPONENTTUTORIALPATHHANDLER_H
#define COMPONENTTUTORIALPATHHANDLER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "dynamicConsoleMethodComponent.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class t2dPath;
class t2dSceneObject;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CComponentTutorialPathHandler : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;
	
    public:
        DECLARE_CONOBJECT( CComponentTutorialPathHandler );
        
        CComponentTutorialPathHandler();
        virtual ~CComponentTutorialPathHandler();
        
        // Inherited from ConsoleObject
        static void initPersistFields();
        
        // Inherited from SimComponent
        virtual bool onComponentAdd( SimComponent* );
        virtual void onUpdate();
        virtual void OnPostInit();
    
        static void OnNodeReached( const t2dVector&, const t2dVector& );
        static void OnPathEndReached();

        static void HideAllLineObjects();
        static void RestartFingerPath();
    
        static bool CanPathSafelyAddNodes();
        
    private:
        t2dSceneObject* m_pOwner;
    
        bool m_bFingerAttachedToPath;
        
        S32 m_iCurrentCircleObjectIndex;
        S32 m_iCurrentSquareObjectIndex;
    
        bool m_bObjectsHidden;
    
        bool m_bPathReached;
        bool m_bClearedAfterPathReached;
    
        Vector<t2dSceneObject*> m_CircleObjectList; // The circles that connect the lines 
        Vector<t2dSceneObject*> m_SquareObjectList; // The lines that are drawn
    
        static CComponentTutorialPathHandler* sm_pInstance;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // COMPONENTTUTORIALPATHHANDLER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

