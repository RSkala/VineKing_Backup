#ifndef _CIEDITOR_H_
#define _CIEDITOR_H_

#include "component/simComponent.h"
#include "component/componentInterface.h"
#include "console/dynamicTypes.h"

class cIEditor;

/// Editor property that will be used extensively
class EditorProperty
{
private:
   cIEditor *mOwningEditor;
   ConsoleBaseType *mConsoleType; ///< The object does not own, and should not free this memory
   void *mMemory;
   StringTableEntry mPropertyName;

public:
   EditorProperty( const char *name, ConsoleBaseType *type, cIEditor *owner ) : 
      mPropertyName( StringTable->insert( name ) ),
      mMemory( NULL ), mConsoleType( type ),
      mOwningEditor( owner ) { };

   EditorProperty( const char *name, const S32 typeID, cIEditor *owner )
   {
      EditorProperty( name, ConsoleBaseType::getType( typeID ), owner );
   };

   void set( const char *data );
   const char *get();
};

//////////////////////////////////////////////////////////////////////////

// This class exists so that the base cIEditor class can have, for default method
// implementations, a call up the class hierarchy, instead of just 'return false'
// functionality, even though this class does that 'return false' bit, it means
// that when a user subclasses cIEditor, if they don't override a function, it
// can have 'parent' functionality, without them putting in a call to Parent::method
class _cIEditorBase : public ComponentInterface
{
   typedef ComponentInterface Parent;

protected:
   virtual bool processInputEvent( /* input event */ ) { return false; };
   virtual bool getEditorInterfaceList( VectorPtr<cIEditor *> &outList ) { return false; };
   virtual bool getProptertyList( Vector<EditorProperty> &outList ) { return false; };
   virtual bool setProperty( EditorProperty &prop ) { return false; };
};

// Editor interface
class cIEditor : public _cIEditorBase
{
   typedef _cIEditorBase Parent;

   // The March 1st episode of the Daily Show is fucking amazing. Get it from
   // iTunes tomorrow.

protected:
   /// All functionality that an overriding editor interface should implement 
   /// is contained in the protected methods of the class. The public methods
   /// are accessors
   
   /// The purpose of this method is to allow Editor Interfaces to process input
   /// events and update themselves or the objects they represent accordingly
   ///
   /// @param inputEvent An input event which the interface may want to respond to
   ///
   /// @return Method should return true if object state was modified, to signal
   /// the caller that they should re-query the list of interfaces and/or properties
   /// as needed.
   virtual bool processInputEvent( /* input event */ ) { return Parent::processInputEvent(); };

   /// This method is used to query the editor interface for a list of editor 
   /// interfaces that it wants exposed.
   ///
   /// @param outList A list which will be populated with editor interfaces
   /// @return This method will return true if the list contains any elements
   virtual bool getEditorInterfaceList( VectorPtr<cIEditor *> &outList ) { return Parent::getEditorInterfaceList( outList ); };

   /// This gets a list of properties that this interface wants exposed and able
   /// to be read and/or modified
   ///
   /// @param outList This vector will be populated with property structures. 
   /// These structures will be passed back to the editor interface so that
   /// the interface can be in-between the actual object and the editing
   /// functionality.
   virtual bool getProptertyList( Vector<EditorProperty> &outList ) { return Parent::getProptertyList( outList ); };

   /// Set a property on the object this interface represents
   ///
   /// @param prop The property structure up for modification
   virtual bool setProperty( EditorProperty &prop ) { return Parent::setProperty( prop ); };


   /// Default constructor
   cIEditor() : Parent() {}; 
public:
   /// The public interface for this class is used by the static SimObject trickery
   /// and by the few code refs to the actual editor interface
   bool _processInputEvent() { return processInputEvent(); };
   bool _getEditorInterfaceList( VectorPtr<cIEditor *> &outList ) { return getEditorInterfaceList( outList ); };
   bool _getPropertyList( Vector<EditorProperty> &outList ) { return getProptertyList( outList ); };
   bool _setProperty( EditorProperty &prop ) { return setProperty( prop ); };
};

//////////////////////////////////////////////////////////////////////////

/// This class is used by the macro for declaring and implementing the editor
/// interfaces. The purpose is to allow scripts to access editor interfaces
/// without making every editor interface a SimObject so that we have a bajillion
/// SimObjects being instantiated and such.
class _cIEditorSimObj : public SimObject
{
   typedef SimObject Parent;

private:
   cIEditor *mActiveInterface; ///< This is the object that the console methods will actually execute on needs set/get

public:
   DECLARE_CONOBJECT(_cIEditorSimObj);

   bool _processInputEvent() {};
   bool _getEditorInterfaceList( /* space-seperated string of editor interface ids  */ ) { return false; };
   bool _getPropertyList( /* space-seperated string of property names */ ) { return false; };
   bool _setProperty( StringTableEntry propName, const char *value ) { return false; };
   const char *_getProperty( StringTableEntry propName ) { return ""; };
};

//////////////////////////////////////////////////////////////////////////

/// A single, static instance of this class will exist for each editor interface
/// type which is declared. It is used to create instances of the most appropriate
/// editor interface based on any SimObject that gets passed to the static method.
/// It will use the namespace information from the SimObject which is passed to
/// it, and traverse the object dependency to the editor interface override which
/// is closest to the actual object class.
class _cIEditorFactory
{

   // Apparently templates are stupid w.r.t. protected fields. Fine...public data
   // members. Blow your leg off at your own peril.
public:
   _cIEditorSimObj *mEditorSimObject;

   static _cIEditorFactory *mFirst;
   _cIEditorFactory *mNext;

protected:
   // The macros will overload this to create instances of the editor interface
   // that they are declaring
   virtual cIEditor *_createInstance() const = 0;
   virtual const char *_getClassName() const = 0;

public:
   // Default constructor
   _cIEditorFactory() : mNext( NULL ), mEditorSimObject( NULL ) {};

   /// Get the first editor interface factory in the list
   static const _cIEditorFactory *getFirst() { return mFirst; };
   const _cIEditorFactory *getNext() const { return mNext; };

   /// Create an instance of the most appropriate editor interface for the SimObject
   /// that is passed in as a parameter. This method will use namespace information
   /// on the SimObject to walk it's heirarchy to find the most
   /// The caller is responsible for freeing the interface returned by this method
   static cIEditor *createEditorInterface( SimObject *obj );

   /// Return the SimObject that represents all instances of the editor interface
   /// that this factory creates.
   _cIEditorSimObj *_getEditorSimObject() const { return mEditorSimObject; };
};

template<class T>
class _cIEditorReg : public _cIEditorFactory
{
public:
   _cIEditorReg() : _cIEditorFactory()
   {
      // Link this entry into the list
      _cIEditorFactory **cur = &mFirst;

      while( *cur != NULL )
         cur = &(*cur)->mNext;

      *cur = this;
   }
};

#define DECLARE_EDITOR_INTERFACE(t) \
   class t##EditorInterface; \
   class t##EditorReg : public _cIEditorReg< t > \
   { \
   protected:\
   virtual cIEditor *_createInstance() const { return NULL; }; \
   virtual const char *_getClassName() const { return #t; }; \
   }; \
   static t##EditorReg _s##t##EditorInterface; \

#define IMPLEMENT_EDITOR_INTERFACE(t) \
   class t##EditorInterface : public cIEditor

#endif