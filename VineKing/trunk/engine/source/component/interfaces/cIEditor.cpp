#include "component/interfaces/cIEditor.h"

IMPLEMENT_CONOBJECT(_cIEditorSimObj);

//////////////////////////////////////////////////////////////////////////
_cIEditorFactory *_cIEditorFactory::mFirst = NULL;

cIEditor *_cIEditorFactory::createEditorInterface( SimObject *obj )
{
   // Easy out
   if( obj == NULL )
      return NULL;

   AbstractClassRep *editorClass = obj->getClassRep();

   // Traverse the linked list of editor interfaces that have been
   // declared, and find the most appropriate interface to instantiate.
   while( editorClass != NULL )
   {
      // Try to match the SimObject's class rep to a declared editor class. If
      // the specific class does not exist, use the class rep to walk up the
      // hierarchy to find the most appropriate editor.
      _cIEditorFactory *cur = mFirst;

      while( cur != NULL )
      {
         // If this evaluates to 'true' than we don't need to traverse up the 
         // inheritance list any further.
         if( dStrcmp( cur->_getClassName(), editorClass->getClassName() ) == 0 )
         {
            // Make sure that there is an instance of the SimObject around
            if( cur->mEditorSimObject == NULL )
            {
               cur->mEditorSimObject = new _cIEditorSimObj();
               AssertFatal( cur->mEditorSimObject->registerObject( avar( "_%sSimObjectRep", cur->_getClassName() ) ),
                  "Failed to register SimObjectRep for editor interface" );
            }

            return cur->_createInstance();
         }

         // Go to the next entry in the list of declared editor interfaces
         cur = cur->mNext;
      }

      // Ok we failed to find an editor interface for this specific class rep, so
      // go up one level in the class hierarchy, and try again
      editorClass = editorClass->getParentClass();

      AssertFatal( editorClass != NULL, "This should never happen. Talk to Dave Moore." ); // Like I want to fuckin deal with it...go bug Dave...
   }

   return NULL;
}

ConsoleFunction( createEditorInterface, S32, 2, 2, "(SimObjectId obj) - Creates the most appropriate editor interface for obj, and returns it." )
{
   SimObject *obj = Sim::findObject( argv[1] );

   if( obj == NULL )
      return -1;

   cIEditor *ret = _cIEditorFactory::createEditorInterface( obj );

   return 0; // ret's ID number
}

//////////////////////////////////////////////////////////////////////////

// Test interfaces

DECLARE_EDITOR_INTERFACE(SimObject);
IMPLEMENT_EDITOR_INTERFACE(SimObject)
{

};