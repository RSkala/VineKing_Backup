//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"

#include "console/ast.h"
#include "core/tAlgorithm.h"
#include "core/resManager.h"

#include "core/findMatch.h"
#include "console/consoleInternal.h"
#include "core/fileStream.h"
#include "console/compiler.h"

#include "tHashTable.h"



//-Mat other files need this, so let's just put it here so we don't forget not to remove it
S32 HashPointer(StringTableEntry ptr)
   {
   return (S32)(((dsize_t)ptr) >> 2);
}


//---------------------------------------------------------------
//
// Dictionary functions
//
//---------------------------------------------------------------
struct StringValue
{
   S32 size;
   char *val;

   operator char *() { return val; }
   StringValue &operator=(const char *string);

   StringValue() { size = 0; val = NULL; }
   ~StringValue() { dFree(val); }
};


StringValue & StringValue::operator=(const char *string)
{
   if(!val)
   {
      val = dStrdup(string);
      size = dStrlen(val);
   }
   else
   {
      S32 len = dStrlen(string);
      if(len < size)
         dStrcpy(val, string);
      else
      {
         size = len;
         dFree(val);
         val = dStrdup(string);
      }
   }
   return *this;
}

void ExprEvalState::pushFrame(StringTableEntry frameName, Namespace *ns, S32 recursionCount)
{
	/*
		-Mat to accomodate recursion, we do the following:
		first time through, check for a NULL dictionary in the Namespace, and create one. After that all Namespaces
		should have a good dictionary. Without recursion, this works fine. If the recursionCount is not
		1, then we are recursing, so make a new dictionary and push that. On popFrame, check for that recursionCount,
		and if it's greater than 1, delete the dictionary that we push, so we don't have any leaks from our new
		recursion Dictionarys
	*/
#ifdef PUAP_SCRIPT_CHANGE
#ifdef PUAP_NAMESPACE_CHANGE
	if(ns == NULL){
		ns = Package::getMainNamespace();
   }
#else
	if(ns == NULL) {
		ns = Namespace::global();
	}
#endif //PUAP_NAMESPACE_CHANGE
	if(ns->mDictionary==NULL){
		ns->mDictionary = new Dictionary(this);
		ns->mDictionary->scopeName = frameName;
		ns->mDictionary->scopeNamespace = ns;
}
	//this is passed from the nsEntry that called us
	if( recursionCount > 1 ) {
	   Dictionary *newFrame = new Dictionary(this);
	   newFrame->scopeName = frameName;
	   newFrame->scopeNamespace = ns;
	   newFrame->recursiveDictionary = true;// flag so we know to delete it on popFrame
	   DictionaryStack.push_back(newFrame);
	} else {
		//no recursion, everything is normal
		ns->mDictionary->scopeName = frameName;//always set this for the debugger
		DictionaryStack.push_back(ns->mDictionary);
	}
#else
   Dictionary *newFrame = new Dictionary(this);
   newFrame->scopeName = frameName;
   newFrame->scopeNamespace = ns;
   DictionaryStack.push_back(newFrame);
#endif //PUAP_SCRIPT_CHANGE
}

void ExprEvalState::popFrame()
{
#ifdef PUAP_SCRIPT_CHANGE
   Dictionary *last = DictionaryStack.last();
   DictionaryStack.pop_back();
   //delete recursive dictionaries only (non-recursive one get re-used, 
   //but we don't know if we'll ever recurse to this level again)
   if( last->recursiveDictionary == true ) {
	   delete last;
   }
#else
   Dictionary *last = DictionaryStack.last();
   DictionaryStack.pop_back();
   delete last;
#endif //PUAP_SCRIPT_CHANGE
}

void ExprEvalState::pushFrameRef(S32 stackIndex)
{
   AssertFatal( stackIndex >= 0 && stackIndex < DictionaryStack.size(), "You must be asking for a valid frame!" );
   Dictionary *newFrame = new Dictionary(this, DictionaryStack[stackIndex]);
   DictionaryStack.push_back(newFrame);
}

ExprEvalState::ExprEvalState()
{
   VECTOR_SET_ASSOCIATION(DictionaryStack);
   globalVars.setState(this);
   thisObject = NULL;
   traceOn = false;
}

ExprEvalState::~ExprEvalState()
{
   while(DictionaryStack.size())
      popFrame();
}

ConsoleFunction(backtrace, void, 1, 1, "() Use the backtrace function to print the current callstack to the console. This is used to trace functions called from withing functions and can help discover what functions were called (and not yet exited) before the current point in your scripts.\n"
																"@return No return value")
{
   argc; argv;
   U32 totalSize = 1;

   for(U32 i = 0; i < gEvalState.DictionaryStack.size(); i++)
   {
      totalSize += dStrlen(gEvalState.DictionaryStack[i]->scopeName) + 3;
      if(gEvalState.DictionaryStack[i]->scopeNamespace && gEvalState.DictionaryStack[i]->scopeNamespace->mName)
         totalSize += dStrlen(gEvalState.DictionaryStack[i]->scopeNamespace->mName) + 2;
   }

   char *buf = Con::getReturnBuffer(totalSize);
   buf[0] = 0;
   for(U32 i = 0; i < gEvalState.DictionaryStack.size(); i++)
   {
      dStrcat(buf, "->");
      if(gEvalState.DictionaryStack[i]->scopeNamespace && gEvalState.DictionaryStack[i]->scopeNamespace->mName)
      {
         dStrcat(buf, gEvalState.DictionaryStack[i]->scopeNamespace->mName);
         dStrcat(buf, "::");
      }
      dStrcat(buf, gEvalState.DictionaryStack[i]->scopeName);
   }
   Con::printf("BackTrace: %s", buf);

}
