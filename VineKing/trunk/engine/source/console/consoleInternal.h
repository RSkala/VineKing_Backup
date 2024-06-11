//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CONSOLEINTERNAL_H_
#define _CONSOLEINTERNAL_H_

#ifndef _STRINGTABLE_H_
#include "core/stringTable.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

/*
	-Mat we may want to do some error checking on this because anyone trying to 
	create packages or namespaces with these names will get the existing ones
*/

//The name of the highest level pacakge that holds all the other packages
#define DEFAULT_PACKAGE_NAME			"DefaultPackage"
//This is the name given to functions in the package, but not a namespace
#define DEFAULT_PACKAGE_NAMESPACE_NAME	"DefaultNamespace"


#include "Package.h"
#include "Namespace.h"
#include "Dictionary.h"

class ExprEvalState;
struct FunctionDecl;
class CodeBlock;
class AbstractClassRep;

bool canTabComplete(const char *prevText, const char *bestMatch,
               const char *newText, S32 baseLen, bool fForward);

extern char *typeValueEmpty;

class ExprEvalState
{
public:
    /// @name Expression Evaluation
    /// @{

    ///
    SimObject *thisObject;
    Dictionary::Entry *currentVariable;
    bool traceOn;

    ExprEvalState();
    ~ExprEvalState();

    /// @}

    /// @name Stack Management
    /// @{

    ///
    Dictionary globalVars;
#ifdef PUAP_NAMESPACE_CHANGE
	Dictionary recursionDictionary;
#endif //PUAP_NAMESPACE_CHANGE
    Vector<Dictionary *> DictionaryStack;
#ifdef PUAP_SCRIPT_CHANGE
	Vector<Namespace::Entry *>NamespaceEntryStack;
#endif// PUAP_SCRIPT_CHANGE
    void setCurVarName(StringTableEntry name,StringTableEntry parentName = NULL);
	void setCurParamName(StringTableEntry name,StringTableEntry parentName = NULL);
    void setCurVarNameCreate(StringTableEntry name,StringTableEntry parentName = NULL);
    S32 getIntVariable();
    F64 getFloatVariable();
    const char *getStringVariable();
    void setIntVariable(S32 val);
    void setFloatVariable(F64 val);
    void setStringVariable(const char *str);

    void pushFrame(StringTableEntry frameName, Namespace *ns, S32 recursionCount = 0);
    void popFrame();

    /// Puts a reference to an existing DictionaryStack frame
    /// on the top of the DictionaryStack.
    void pushFrameRef(S32 stackIndex);

    /// @}
};

#ifdef PUAP_SCRIPT_CHANGE
//-Mat we need these for the telnet debugger and eval()s
StringTableEntry getCurrentNamespaceName();
Namespace *getCurrentNamespace();
StringTableEntry getCurrentFunctionName();
Namespace::Entry *getCurrentFunction();
#endif// PUAP_SCRIPT_CHANGE

#endif
