//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------



#ifdef PUAP_NAMESPACE_CHANGE

#include "platform/platform.h"
#include "console/console.h"

#include "console/ast.h"
#include "core/tAlgorithm.h"
#include "core/resManager.h"

#include "core/findMatch.h"
#include "console/consoleInternal.h"
#include "core/fileStream.h"
#include "console/compiler.h"

#include "console/simBase.h"
#include "console/telnetDebugger.h"
#include "sim/netStringTable.h"
#include "component/dynamicConsoleMethodComponent.h"
#include "console/stringStack.h"
#include "util/messaging/message.h"
#include "core/frameAllocator.h"

using namespace Compiler; 

#ifdef PUAP_OPTIMIZE //-Mat 
enum EvalConstants {
	MaxStackSize = 256,
	MethodOnComponent = -2
};
#else
enum EvalConstants {
	MaxStackSize = 1024,
	MethodOnComponent = -2
};
#endif

namespace Con
{
	// Current script file name and root, these are registered as
	// console variables.
	extern StringTableEntry gCurrentFile;
	extern StringTableEntry gCurrentRoot;
}

#ifdef PUAP_OPTIMIZE //-Mat 
F32 floatStack[MaxStackSize];
S32 intStack[MaxStackSize];
#else
F64 floatStack[MaxStackSize];
S64 intStack[MaxStackSize];
#endif

StringStack STR;

U32 FLT = 0;
U32 UINT = 0;

static const char *getNamespaceList(Namespace *ns)
{
	//this should return the structure of the Namespace, from the root package 
	//through all packages and namespaces to this Namespace

	U32 size = dStrlen(ns->mName);
	Package *currentPackage = ns->getOwner();
	//calculate size
	Vector<Package*> packageList;
	while( currentPackage ) {
		size += dStrlen(currentPackage->mName) + 4;
		packageList.push_back( currentPackage );
		currentPackage = currentPackage->mParent;//move up the tree
	}

	char *ret = Con::getReturnBuffer(size);
	ret[0] = 0;

	//build string with "->" between names
	for( S32 i = packageList.size(); i > 0; i-- ) {
		dStrcat(ret, packageList[i-1]->mName);
		//if(packageList[i-1]->mParent)
		dStrcat(ret, " -> ");
	}
	dStrcat( ret, ns->mName );
	return ret;
}

//------------------------------------------------------------

F64 consoleStringToNumber(const char *str, StringTableEntry file, U32 line)
{
	F64 val = dAtof(str);
	if(val != 0)
		return val;
	else if(!dStricmp(str, "true"))
		return 1;
	else if(!dStricmp(str, "false"))
		return 0;
	else if(file)
	{
		Con::warnf(ConsoleLogEntry::General, "%s (%d): string always evaluates to 0.", file, line);
		return 0;
	}
	return 0;
}

//------------------------------------------------------------

namespace Con
{

	char *getReturnBuffer(U32 bufferSize)

	{
		return STR.getReturnBuffer(bufferSize);
	}

	char *getReturnBuffer( const char *stringToCopy )
	{
		char *ret = STR.getReturnBuffer( dStrlen( stringToCopy ) + 1 );
		dStrcpy( ret, stringToCopy );
		ret[dStrlen( stringToCopy )] = '\0';
		return ret;
	}

	char *getArgBuffer(U32 bufferSize)
	{
		return STR.getArgBuffer(bufferSize);
	}

	char *getFloatArg(F64 arg)
	{

		char *ret = STR.getArgBuffer(32);
		dSprintf(ret, 32, "%g", arg);
		return ret;
	}

	char *getIntArg(S32 arg)
	{

		char *ret = STR.getArgBuffer(32);
		dSprintf(ret, 32, "%d", arg);
		return ret;
	}
}

//------------------------------------------------------------

inline void ExprEvalState::setCurVarName(StringTableEntry name,StringTableEntry parentName)
{
	name = StringTable->insert( name );
	if(parentName) parentName = StringTable->insert( parentName );
	if(name[0] == '$')
		currentVariable = globalVars.lookup(name);
	else if(DictionaryStack.size())
		currentVariable = DictionaryStack.last()->lookup(name,parentName);
	if(!currentVariable && gWarnUndefinedScriptVariables)
		Con::warnf(ConsoleLogEntry::Script, "Variable referenced before assignment: %s", name);
}

inline void ExprEvalState::setCurVarNameCreate(StringTableEntry name, StringTableEntry parentName)
{
	name = StringTable->insert( name );
	if(parentName) 
		parentName = StringTable->insert( parentName );
	if(name[0] == '$')
		currentVariable = globalVars.add(name);
	else if(DictionaryStack.size())
		currentVariable = DictionaryStack.last()->add(name, parentName);
	else
	{
		currentVariable = NULL;
		Con::warnf(ConsoleLogEntry::Script, "Accessing local variable in global scope... failed: %s", name);
	}
}

//------------------------------------------------------------

inline S32 ExprEvalState::getIntVariable()
{
	return currentVariable ? currentVariable->getIntValue() : 0;
}

inline F64 ExprEvalState::getFloatVariable()
{
	return currentVariable ? currentVariable->getFloatValue() : 0;
}

inline const char *ExprEvalState::getStringVariable()
{
	return currentVariable ? currentVariable->getStringValue() : "";
}

//------------------------------------------------------------

inline void ExprEvalState::setIntVariable(S32 val)
{
	AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
	currentVariable->setIntValue(val);
}

inline void ExprEvalState::setFloatVariable(F64 val)
{
	AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
	currentVariable->setFloatValue(val);
}

inline void ExprEvalState::setStringVariable(const char *val)
{
	AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
	currentVariable->setStringValue(val);
}

//------------------------------------------------------------

void CodeBlock::getFunctionArgs(char buffer[1024], U32 ip)
{
	U32 fnArgc = code[ip + 5];
	buffer[0] = 0;
	for(U32 i = 0; i < fnArgc; i++)
	{
		StringTableEntry var = U32toSTE(code[ip + i + 6]);
		//-Mat check this for our pointer

		// Add a comma so it looks nice!
		if(i != 0)
			dStrcat(buffer, ", ");

		dStrcat(buffer, "var ");

		// Try to capture junked parameters
		if(var[0])
			dStrcat(buffer, var+1);
		else
			dStrcat(buffer, "JUNK");
	}
}

// Returns, in 'val', the specified component of a string.
static void getUnit(const char *string, U32 index, const char *set, char val[], S32 len)
{
	U32 sz;
	while(index--)
	{
		if(!*string)
			return;
		sz = dStrcspn(string, set);
		if (string[sz] == 0)
			return;
		string += (sz + 1);
	}
	sz = dStrcspn(string, set);
	if (sz == 0)
		return;

	if( ( sz + 1 ) > len )
		return;

	dStrncpy(val, string, sz);
	val[sz] = '\0';
}

// Copies a string, replacing the (space separated) specified component. The
// return value is stored in 'val'.
static void setUnit(const char *string, U32 index, const char *replace, const char *set, char val[], S32 len)
{
	U32 sz;
	const char *start = string;
	if( ( dStrlen(string) + dStrlen(replace) + 1 ) > len )
		return;

	U32 padCount = 0;

	while(index--)
	{
		sz = dStrcspn(string, set);
		if(string[sz] == 0)
		{
			string += sz;
			padCount = index + 1;
			break;
		}
		else
			string += (sz + 1);
	}
	// copy first chunk
	sz = string-start;
	dStrncpy(val, start, sz);
	for(U32 i = 0; i < padCount; i++)
		val[sz++] = set[0];

	// replace this unit
	val[sz] = '\0';
	dStrcat(val, replace);

	// copy remaining chunks
	sz = dStrcspn(string, set);         // skip chunk we're replacing
	if(!sz && !string[sz])
		return;

	string += sz;
	dStrcat(val, string);
	return;
}

// Gets a component of an object's field value or a variable and returns it
// in val.
static void getFieldComponent( SimObject* object, StringTableEntry field, const char* array, StringTableEntry subField, char val[], S32 count )
{
	const char* prevVal = NULL;
	// Grab value from object.
	if( object && field )
		prevVal = object->getDataField( field, array );
	// Otherwise, grab from the string DictionaryStack. The value coming in will always
	// be a string because that is how multicomponent variables are handled.
	else
		prevVal = STR.getStringValue();

	// Make sure we got a value.
	if( prevVal && *prevVal )
	{
		// 'x', 'y', and 'z' grab the 1st, 2nd, or 3rd component of the
		// variable or field.
		if( subField == StringTable->insert( "x" ) )
			getUnit( prevVal, 0, " ", val, count );
		else if( subField == StringTable->insert( "y" ) )
			getUnit( prevVal, 1, " ", val, count );
		else if( subField == StringTable->insert( "z" ) )
			getUnit( prevVal, 2, " ", val, count );
	}
}

// Sets a component of an object's field value based on the sub field. 'x' will
// set the first field, 'y' the second, and 'z' the third.
static void setFieldComponent( SimObject* object, StringTableEntry field, const char* array, StringTableEntry subField )
{
	char val[1024] = "";
	const char* prevVal;
	// Set the value on an object field.
	if( object && field )
		prevVal = object->getDataField( field, array );

	// Set the value on a variable.
	else if( gEvalState.currentVariable )
		prevVal = gEvalState.getStringVariable();

	// Insert the value into the specified component of the string.
	bool set = false;
	if( subField == StringTable->insert( "x" ) )
	{
		setUnit( prevVal, 0, STR.getStringValue(), " ", val, 1024 );
		set = true;
	}
	else if( subField == StringTable->insert( "y" ) )
	{
		setUnit( prevVal, 1, STR.getStringValue(), " ", val, 1024 );
		set = true;
	}
	else if( subField == StringTable->insert( "z" ) )
	{
		setUnit( prevVal, 2, STR.getStringValue(), " ", val, 1024 );
		set = true;
	}

	if( set )
	{
		// Update the field or variable.
		if( object && field )
			object->setDataField( field, array, val );
		else if( gEvalState.currentVariable )
			gEvalState.setStringVariable( val );
	}
}

#ifdef PUAP_SCRIPT_CHANGE
const char *CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, const char **argv, bool noCalls, StringTableEntry packageName, S32 setFrame, Namespace::Entry *parentEntry )
{
#ifdef TORQUE_DEBUG
	U32 stackStart = STR.mStartStackSize;
#endif

	static char traceBuffer[1024];
	U32 i;

	incRefCount();
	F64 *curFloatTable;
	char *curStringTable;
	STR.clearFunctionOffset();
	StringTableEntry thisFunctionName = NULL;
	bool popFrame = false;

	if(argv)
	{
		// assume this points into a function decl:
		//-Mat the +3 is because the first 3 IPs are strings
		U32 fnArgc = code[ip + 5 + 3];
		thisFunctionName = U32toSTE(code[ip]);
		//-Mat advance past our pointer
		ip++;

		argc = getMin(argc-1, fnArgc); // argv[0] is func name
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Entering ");
			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s(", thisNamespace->mName, thisFunctionName);
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s(", thisFunctionName);
			}
			for(i = 0; i < argc; i++)
			{
				dStrcat(traceBuffer, argv[i+1]);
				if(i != argc - 1)
					dStrcat(traceBuffer, ", ");
			}
			dStrcat(traceBuffer, ")");
			Con::printf("%s", traceBuffer);
		}

		//-Mat this is the frame of variables for each function CALL
		gEvalState.pushFrame(thisFunctionName, thisNamespace, setFrame);
		popFrame = true;
		for(i = 0; i < argc; i++)
		{
			//-Mat use i * 2 to account for the pointer
			StringTableEntry var = U32toSTE(code[ip + i*2 + 8]);
			gEvalState.setCurVarNameCreate(var,thisFunctionName);//-Mat pass in function name
			gEvalState.setStringVariable( argv[ (i + 1) ]  );
		}
		//count 2 pointer for each argument (whether we get it passed to us or not)
		//fnArgc is the number of args in the function declaration
		//argc is the number actually passed to us
		ip = ip + (fnArgc * 2) + 6;
		//-Mat + 2 past our 2 strings (3, but we already incremented for thisFunctionName)
		ip += 2;
		curFloatTable = functionFloats;
		curStringTable = functionStrings;
	}
	else
	{
		curFloatTable = globalFloats;
		curStringTable = globalStrings;

		// Do we want this code to execute using a new DictionaryStack frame?
		if (setFrame < 0)
		{
			gEvalState.pushFrame(NULL, NULL, setFrame);
			popFrame = true;
		}
		else if (!gEvalState.DictionaryStack.empty())
		{
			// We want to copy a reference to an existing DictionaryStack frame
			// on to the top of the DictionaryStack.  Any change that occurs to 
			// the locals during this new frame will also occur in the 
			// original frame.
			//S32 stackIndex = gEvalState.DictionaryStack.size() - setFrame - 1;
			//gEvalState.pushFrameRef( stackIndex );
			popFrame = false;
		}
	}

	// Grab the state of the telenet debugger here once
	// so that the push and pop frames are always balanced.
	const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
	if ( telDebuggerOn && setFrame < 0 )
		TelDebugger->pushStackFrame();

	StringTableEntry var, objParent;
	U32 failJump;
	StringTableEntry fnName;
	StringTableEntry fnNamespace, fnPackage;
	SimObject *currentNewObject = 0;
	StringTableEntry prevField = NULL;
	StringTableEntry curField = NULL;
	SimObject *prevObject = NULL;
	SimObject *curObject = NULL;
	SimObject *saveObject=NULL;
	Namespace::Entry *nsEntry;
	Namespace *ns;
	Package *pg = NULL;

	Dictionary::Entry *pVarEntry = NULL;

	const char* curFNDocBlock = NULL;
	const char* curNSDocBlock = NULL;
	const S32 nsDocLength = 128;
	char nsDocBlockClass[nsDocLength];

	U32 callArgc;
	const char **callArgv;

	static char curFieldArray[256];
	static char prevFieldArray[256];

	CodeBlock *saveCodeBlock = smCurrentCodeBlock;
	smCurrentCodeBlock = this;
	if(this->name)
	{
		Con::gCurrentFile = this->name;
		Con::gCurrentRoot = mRoot;
	}
	const char * val;

	// The frame temp is used by the variable accessor ops (OP_SAVEFIELD_* and
	// OP_LOADFIELD_*) to store temporary values for the fields.
	static S32 VAL_BUFFER_SIZE = 1024;
	FrameTemp<char> valBuffer( VAL_BUFFER_SIZE );

	for(;;)
	{
		U32 instruction = code[ip++];
breakContinue:
		switch(instruction)
		{
		case OP_FUNC_DECL:
			{
				U32 startIP = ip;
				if(!noCalls)
				{
					fnName       = U32toSTE(code[ip]);
					ip++;//-Mat to account for our new pointer
					fnNamespace  = U32toSTE(code[ip+1]);
					ip++;//-Mat to account for our new pointer
					fnPackage    = U32toSTE(code[ip+2]);
					ip++;//-Mat to account for our new pointer
					bool hasBody = bool(code[ip+3]);

					//Package::unlinkPackages();
					ns = Namespace::findAndCreate(fnNamespace, fnPackage);
					ns->addFunction(fnName, this, hasBody ? startIP : 0, curFNDocBlock ? dStrdup( curFNDocBlock ) : NULL );// if no body, set the IP to 0
					if( curNSDocBlock )
					{
						if( fnNamespace == StringTable->lookup( nsDocBlockClass ) )
						{
							char *usageStr = dStrdup( curNSDocBlock );
							usageStr[dStrlen(usageStr)] = '\0';
							ns->mUsage = usageStr;
							ns->mCleanUpUsage = true;
							curNSDocBlock = NULL;
						}
					}
					//Package::relinkPackages();

					// If we had a docblock, it's definitely not valid anymore, so clear it out.
					curFNDocBlock = NULL;

					//Con::printf("Adding function %s::%s (%d)", fnNamespace, fnName, ip);
				}
				//-Mat + 4 because we moved 3 ip after we got the name(fnNamespace, fnPackage, hasbody)
				// + 3 for the pointer in after each string (fnName, fnNamespace, fnPackage)
				ip = code[startIP + 4 + 3];
			}
			break;

		case OP_CREATE_OBJECT:
			{
				// Read some useful info.
				objParent        = U32toSTE(code[ip    ]);

				//-Mat increment past our variable
				ip++;

				bool isDataBlock =          code[ip + 1];
				bool isInternal  =          code[ip + 2];
				bool isMessage   =          code[ip + 3];
				failJump         =          code[ip + 4];

				// If we don't allow calls, we certainly don't allow creating objects!
				// Moved this to after failJump is set. Engine was crashing when
				// noCalls = true and an object was being created at the beginning of
				// a file. ADL.
				if(noCalls)
				{
					ip = failJump;
					break;
				}

				// Get the constructor information off the stack.
				STR.getArgcArgv(NULL, &callArgc, &callArgv, true);

				// Con::printf("Creating object...");

				// objectName = argv[1]...
				currentNewObject = NULL;

				// Are we creating a datablock? If so, deal with case where we override
				// an old one.
				if(isDataBlock)
				{
					// Con::printf("  - is a datablock");

					// Find the old one if any.
					SimObject *db = Sim::getDataBlockGroup()->findObject(callArgv[2]);

					// Make sure we're not changing types on ourselves...
					if(db && dStricmp(db->getClassName(), callArgv[1]))
					{
						Con::errorf(ConsoleLogEntry::General, "Cannot re-declare data block %s with a different class.", callArgv[2]);
						ip = failJump;
						break;
					}

					// If there was one, set the currentNewObject and move on.
					if(db)
						currentNewObject = db;
				}

				if(!currentNewObject)
				{
					// Well, looks like we have to create a new object.
					ConsoleObject *object = ConsoleObject::create(callArgv[1]);

					// Deal with failure!
					if(!object)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-conobject class %s.", getFileLine(ip-1), callArgv[1]);
						ip = failJump;
						break;
					}

					// Do special datablock init if appropros
					if(isDataBlock)
					{
						SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(object);
						if(dataBlock)
						{
							dataBlock->assignId();
						}
						else
						{
							// They tried to make a non-datablock with a datablock keyword!
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-datablock class %s.", getFileLine(ip-1), callArgv[1]);

							// Clean up...
							delete object;
							ip = failJump;
							break;
						}
					}

					// Finally, set currentNewObject to point to the new one.
					currentNewObject = dynamic_cast<SimObject *>(object);

					// Deal with the case of a non-SimObject.
					if(!currentNewObject)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-SimObject class %s.", getFileLine(ip-1), callArgv[1]);
						delete object;
						ip = failJump;
						break;
					}

					// Does it have a parent object? (ie, the copy constructor : syntax, not inheriance)
					// [tom, 9/8/2006] it is inheritance if it's a message ... muwahahah!
					//-Mat if objParent is NULL, evaluating *objParent will crash
					if(!isMessage && objParent != NULL && *objParent)
					{
						// Find it!
						SimObject *parent;
						if(Sim::findObject(objParent, parent))
						{
							// Con::printf(" - Parent object found: %s", parent->getClassName());

							// and suck the juices from it!
							currentNewObject->assignFieldsFrom(parent);
						}
						else
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to find parent object %s for %s.", getFileLine(ip-1), objParent, callArgv[1]);

						// Mm! Juices!
					}

					// If a name was passed, assign it.
					if(callArgv[2][0])
					{
						if(! isMessage)
						{
							if(! isInternal)
								currentNewObject->assignName(callArgv[2]);
							else
								currentNewObject->setInternalName(callArgv[2]);
						}
						else
						{
							Message *msg = dynamic_cast<Message *>(currentNewObject);
							if(msg)
							{
								msg->setClassNamespace(callArgv[2]);
								msg->setSuperClassNamespace(objParent);
							}
							else
							{
								Con::errorf(ConsoleLogEntry::General, "%s: Attempting to use newmsg on non-message type %s", getFileLine(ip-1), callArgv[1]);
								delete currentNewObject;
								currentNewObject = NULL;
								ip = failJump;
								break;
							}
						}
					}

					// Do the constructor parameters.
					if(!currentNewObject->processArguments(callArgc-3, callArgv+3))
					{
						delete currentNewObject;
						currentNewObject = NULL;
						ip = failJump;
						break;
					}

					// If it's not a datablock, allow people to modify bits of it.
					if(!isDataBlock)
					{
						currentNewObject->setModStaticFields(true);
						currentNewObject->setModDynamicFields(true);
					}
				}

				// Advance the IP past the create info...
				ip += 5;
				break;
			}

		case OP_ADD_OBJECT:
			{
				// See OP_SETCURVAR for why we do this.
				curFNDocBlock = NULL;
				curNSDocBlock = NULL;

				// Do we place this object at the root?
				bool placeAtRoot = code[ip++];

				// Con::printf("Adding object %s", currentNewObject->getName());

				// Make sure it wasn't already added, then add it.
				if(currentNewObject->isProperlyAdded() == false)
				{
					bool ret = false;

					Message *msg = dynamic_cast<Message *>(currentNewObject);
					if(msg)
					{
						SimObjectId id = Message::getNextMessageID();
						if(id != 0xffffffff)
							ret = currentNewObject->registerObject(id);
						else
							Con::errorf("%s: No more object IDs available for messages", getFileLine(ip-2));
					}
					else
						ret = currentNewObject->registerObject();

					if(! ret)
					{
						// This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
						Con::warnf(ConsoleLogEntry::General, "%s: Register object failed for object %s of class %s.", getFileLine(ip-2), currentNewObject->getName(), currentNewObject->getClassName());
						delete currentNewObject;
						ip = failJump;
						break;
					}
				}

				// Are we dealing with a datablock?
				SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(currentNewObject);
				static char errorBuffer[256];

				// If so, preload it.
				if(dataBlock && !dataBlock->preload(true, errorBuffer))
				{
					Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", getFileLine(ip-2),
						currentNewObject->getName(), errorBuffer);
					dataBlock->deleteObject();
					ip = failJump;
					break;
				}

				// What group will we be added to, if any?
				U32 groupAddId = intStack[UINT];
				SimGroup *grp = NULL;
				SimSet   *set = NULL;
				SimComponent *comp = NULL;
				bool isMessage = dynamic_cast<Message *>(currentNewObject) != NULL;

				if(!placeAtRoot || !currentNewObject->getGroup())
				{
					if(! isMessage)
					{
						if(! placeAtRoot)
						{
							// Otherwise just add to the requested group or set.
							if(!Sim::findObject(groupAddId, grp))
								if(!Sim::findObject(groupAddId, comp))
									Sim::findObject(groupAddId, set);
						}

						if(placeAtRoot || comp != NULL)
						{
							// Deal with the instantGroup if we're being put at the root or we're adding to a component.
							const char *addGroupName = Con::getVariable("instantGroup");
							if(!Sim::findObject(addGroupName, grp))
								Sim::findObject(RootGroupId, grp);
						}

						if(comp)
						{
							SimComponent *newComp = dynamic_cast<SimComponent *>(currentNewObject);
							if(newComp)
							{
								if(! comp->addComponent(newComp))
									Con::errorf("%s: Unable to add component %s, template not loaded?", getFileLine(ip-2), currentNewObject->getName() ? currentNewObject->getName() : currentNewObject->getIdString());
							}
						}
					}

					// If we didn't get a group, then make sure we have a pointer to
					// the rootgroup.
					if(!grp)
						Sim::findObject(RootGroupId, grp);

					// add to the parent group
					grp->addObject(currentNewObject);

					// add to any set we might be in
					if(set)
						set->addObject(currentNewObject);
				}

				// store the new object's ID on the stack (overwriting the group/set
				// id, if one was given, otherwise getting pushed)
				if(placeAtRoot) 
					intStack[UINT] = currentNewObject->getId();
				else
					intStack[++UINT] = currentNewObject->getId();

				break;
			}

		case OP_END_OBJECT:
			{
				// If we're not to be placed at the root, make sure we clean up
				// our group reference.
				bool placeAtRoot = code[ip++];
				if(!placeAtRoot)
					UINT--;
				break;
			}

		case OP_JMPIFFNOT:
			if(floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT:
			if(intStack[UINT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFF:
			if(!floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF:
			if(!intStack[UINT--])
			{
				ip ++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT_NP:
			if(intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF_NP:
			if(!intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMP:
			ip = code[ip];
			break;
		case OP_RETURN:
			goto execFinished;
		case OP_CMPEQ:
			intStack[UINT+1] = bool(floatStack[FLT] == floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGR:
			intStack[UINT+1] = bool(floatStack[FLT] > floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGE:
			intStack[UINT+1] = bool(floatStack[FLT] >= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLT:
			intStack[UINT+1] = bool(floatStack[FLT] < floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLE:
			intStack[UINT+1] = bool(floatStack[FLT] <= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPNE:
			intStack[UINT+1] = bool(floatStack[FLT] != floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_XOR:
			intStack[UINT-1] = intStack[UINT] ^ intStack[UINT-1];
			UINT--;
			break;

		case OP_MOD:
			if(  intStack[UINT-1] != 0 )
				intStack[UINT-1] = intStack[UINT] % intStack[UINT-1];
			else
				intStack[UINT-1] = 0;
			UINT--;
			break;

		case OP_BITAND:
			intStack[UINT-1] = intStack[UINT] & intStack[UINT-1];
			UINT--;
			break;

		case OP_BITOR:
			intStack[UINT-1] = intStack[UINT] | intStack[UINT-1];
			UINT--;
			break;

		case OP_NOT:
			intStack[UINT] = !intStack[UINT];
			break;

		case OP_NOTF:
			intStack[UINT+1] = !floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_ONESCOMPLEMENT:
			intStack[UINT] = ~intStack[UINT];
			break;

		case OP_SHR:
			intStack[UINT-1] = intStack[UINT] >> intStack[UINT-1];
			UINT--;
			break;

		case OP_SHL:
			intStack[UINT-1] = intStack[UINT] << intStack[UINT-1];
			UINT--;
			break;

		case OP_AND:
			intStack[UINT-1] = intStack[UINT] && intStack[UINT-1];
			UINT--;
			break;

		case OP_OR:
			intStack[UINT-1] = intStack[UINT] || intStack[UINT-1];
			UINT--;
			break;

		case OP_ADD:
			floatStack[FLT-1] = floatStack[FLT] + floatStack[FLT-1];
			FLT--;
			break;

		case OP_SUB:
			floatStack[FLT-1] = floatStack[FLT] - floatStack[FLT-1];
			FLT--;
			break;

		case OP_MUL:
			floatStack[FLT-1] = floatStack[FLT] * floatStack[FLT-1];
			FLT--;
			break;
		case OP_DIV:
			floatStack[FLT-1] = floatStack[FLT] / floatStack[FLT-1];
			FLT--;
			break;
		case OP_NEG:
			floatStack[FLT] = -floatStack[FLT];
			break;

		case OP_SETCURVAR:
			var = U32toSTE(code[ip]);

			//-Mat to account for our empty pointer before the string
			ip++;

			// If a variable is set, then these must be NULL. It is necessary
			// to set this here so that the vector parser can appropriately
			// identify whether it's dealing with a vector.
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			//if we are recursing do things the slow lookup way, otherwise use our quick pointer 
			if( setFrame > 1 ) {
				gEvalState.setCurVarNameCreate(var, thisFunctionName);
			} else {
				pVarEntry = ((Dictionary::Entry*)code[ip]);

				if( !pVarEntry ) {
					gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function
					//-Mat if we can't find it, it is probably a global that is being checked(but not created) right here
					//in which case we can just make a new, blank one, because it should be == "" anyway
					if( gEvalState.currentVariable == NULL ) {
						gEvalState.setCurVarNameCreate(var, thisFunctionName);
					}
						code[ip] = (U32)gEvalState.currentVariable;
				} else {
					gEvalState.currentVariable = pVarEntry;
				}
			}

			// In order to let docblocks work properly with variables, we have
			// clear the current docblock when we do an assign. This way it 
			// won't inappropriately carry forward to following function decls.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			ip++;
			break;

		case OP_SETCURVAR_CREATE:
			var = U32toSTE(code[ip]);

			//-Mat to account for our empty pointer before the string
			ip++;
			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			//if we are recursing do things the slow lookup way, otherwise use our quick pointer 
			if( setFrame > 1 ) {
				gEvalState.setCurVarNameCreate(var, thisFunctionName);
			} else {
				pVarEntry = ((Dictionary::Entry*)code[ip]);
				if( !pVarEntry ) {
					gEvalState.setCurVarNameCreate(var, thisFunctionName);//-Mat pass in the name of this function
						code[ip] =  (U32)gEvalState.currentVariable;
					}
				else{
					gEvalState.currentVariable = pVarEntry;
				}
			}

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			ip++;
			break;

		case OP_SETCURVAR_ARRAY:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_ARRAY_CREATE:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarNameCreate(var,thisFunctionName);//-Mat pass in function name

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_LOADVAR_UINT:
			intStack[UINT+1] = gEvalState.getIntVariable();
			UINT++;
			break;

		case OP_LOADVAR_FLT:
			floatStack[FLT+1] = gEvalState.getFloatVariable();
			FLT++;
			break;

		case OP_LOADVAR_STR:
			val = gEvalState.getStringVariable();
			STR.setStringValue(val);
			break;

		case OP_SAVEVAR_UINT:
			gEvalState.setIntVariable(intStack[UINT]);
			break;

		case OP_SAVEVAR_FLT:
			gEvalState.setFloatVariable(floatStack[FLT]);
			break;

		case OP_SAVEVAR_STR:
			gEvalState.setStringVariable(STR.getStringValue());
			break;

		case OP_SETCUROBJECT:
			// Save the previous object for parsing vector fields.
			prevObject = curObject;
			val = STR.getStringValue();

			// Sim::findObject will sometimes find valid objects from
			// multi-component strings. This makes sure that doesn't
			// happen.
			for( const char* check = val; *check; check++ )
			{
				if( *check == ' ' )
				{
					val = "";
					break;
				}
			}
			curObject = Sim::findObject(val);
			break;

		case OP_SETCUROBJECT_INTERNAL:
			++ip; // To skip the recurse flag if the object wasnt found
			if(curObject)
			{
				SimGroup *group = dynamic_cast<SimGroup *>(curObject);
				if(group)
				{
					StringTableEntry intName = StringTable->insert(STR.getStringValue());
					bool recurse = code[ip-1];
					SimObject *obj = group->findObjectByInternalName(intName, recurse);
					intStack[UINT+1] = obj ? obj->getId() : 0;
					UINT++;
				}
				else
				{
					Con::errorf(ConsoleLogEntry::Script, "%s: Attempt to use -> on non-group %s of class %s.", getFileLine(ip-2), curObject->getName(), curObject->getClassName());
					intStack[UINT] = 0;
				}
			}
			break;

		case OP_SETCUROBJECT_NEW:
			curObject = currentNewObject;
			break;

		case OP_SETCURFIELD:
			// Save the previous field for parsing vector fields.
			prevField = curField;
			dStrcpy( prevFieldArray, curFieldArray );
			curField = U32toSTE(code[ip]);

			//-Mat increment past our pointer
			ip++;

			curFieldArray[0] = 0;
			ip++;
			break;

		case OP_SETCURFIELD_ARRAY:
			dStrcpy(curFieldArray, STR.getStringValue());
			break;

		case OP_LOADFIELD_UINT:
			if(curObject)
				intStack[UINT+1] = U32(dAtoi(curObject->getDataField(curField, curFieldArray)));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				intStack[UINT+1] = dAtoi( valBuffer );
			}
			UINT++;
			break;

		case OP_LOADFIELD_FLT:
			if(curObject)
				floatStack[FLT+1] = dAtof(curObject->getDataField(curField, curFieldArray));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				floatStack[FLT+1] = dAtof( valBuffer );
			}
			FLT++;
			break;

		case OP_LOADFIELD_STR:
			if(curObject)
			{
				val = curObject->getDataField(curField, curFieldArray);
				STR.setStringValue( val );
			}
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				STR.setStringValue( valBuffer );
			}

			break;

		case OP_SAVEFIELD_UINT:
			STR.setIntValue(intStack[UINT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_FLT:
			STR.setFloatValue(floatStack[FLT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_STR:
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_STR_TO_UINT:
			intStack[UINT+1] = STR.getIntValue();
			UINT++;
			break;

		case OP_STR_TO_FLT:
			floatStack[FLT+1] = STR.getFloatValue();
			FLT++;
			break;

		case OP_STR_TO_NONE:
			// This exists simply to deal with certain typecast situations.
			break;

		case OP_FLT_TO_UINT:
			intStack[UINT+1] = (S64)floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_FLT_TO_STR:
			STR.setFloatValue(floatStack[FLT]);
			FLT--;
			break;

		case OP_FLT_TO_NONE:
			FLT--;
			break;

		case OP_UINT_TO_FLT:
			floatStack[FLT+1] = intStack[UINT];
			UINT--;
			FLT++;
			break;

		case OP_UINT_TO_STR:
			STR.setIntValue(intStack[UINT]);
			UINT--;
			break;

		case OP_UINT_TO_NONE:
			UINT--;
			break;

		case OP_LOADIMMED_UINT:
			intStack[UINT+1] = code[ip++];
			UINT++;
			break;

		case OP_LOADIMMED_FLT:
			floatStack[FLT+1] = curFloatTable[code[ip]];
			ip++;
			FLT++;
			break;
		case OP_TAG_TO_STR:
			code[ip-1] = OP_LOADIMMED_STR;
			// it's possible the string has already been converted
			if(U8(curStringTable[code[ip]]) != StringTagPrefixByte)
			{
				U32 id = GameAddTaggedString(curStringTable + code[ip]);
				dSprintf(curStringTable + code[ip] + 1, 7, "%d", id);
				*(curStringTable + code[ip]) = StringTagPrefixByte;
			}
		case OP_LOADIMMED_STR:
			STR.setStringValue(curStringTable + code[ip++]);
			break;

		case OP_DOCBLOCK_STR:
			{
				// If the first word of the doc is '\class' or '@class', then this
				// is a namespace doc block, otherwise it is a function doc block.
				const char* docblock = curStringTable + code[ip++];

				const char* sansClass = dStrstr( docblock, "@class" );
				if( !sansClass )
					sansClass = dStrstr( docblock, "\\class" );

				if( sansClass )
				{
					// Don't save the class declaration. Scan past the 'class'
					// keyword and up to the first whitespace.
					sansClass += 7;
					S32 index = 0;
					while( ( *sansClass != ' ' ) && ( *sansClass != '\n' ) && *sansClass && ( index < ( nsDocLength - 1 ) ) )
					{
						nsDocBlockClass[index++] = *sansClass;
						sansClass++;
					}
					nsDocBlockClass[index] = '\0';

					curNSDocBlock = sansClass + 1;
				}
				else
					curFNDocBlock = docblock;
			}

			break;

		case OP_LOADIMMED_IDENT:
			STR.setStringValue(U32toSTE(code[ip++]));

			//-Mat increment past our pointer
			ip++;

			break;

		case OP_CALLFUNC_RESOLVE:
			{
				U32 startIP = ip;
				// This deals with a function that is potentially living in a namespace.
				fnName      = U32toSTE(code[ip]);
				ip++;//-Mat to account for our new pointer
				fnNamespace = U32toSTE(code[ip+1]);
				ip++;//-Mat to account for our new pointer

				// Try to look it up.
				ns = Namespace::findAndCreate( fnNamespace, Package::getCurrentPackage()->mName );
				nsEntry = ns->lookup(fnName);
				if(!nsEntry)
				{
					ip+= 3;
					Con::warnf(ConsoleLogEntry::General,
						"%s: Unable to find function %s%s%s",
						getFileLine(ip-4), fnNamespace ? fnNamespace : "",
						fnNamespace ? "::" : "", fnName);
					STR.popFrame();
					break;
				}
				// Now, rewrite our code a bit (ie, avoid future lookups) and fall
				// through to OP_CALLFUNC
				code[ip+1] = *((U32 *) &nsEntry);
				code[ip-1] = OP_CALLFUNC;

				//because we change the ip, let's put it back to the start for OP_CALLFUNC
				ip = startIP;
			}

		case OP_CALLFUNC:
			{
				// This routingId is set when we query the object as to whether
				// it handles this method.  It is set to an enum from the table
				// above indicating whether it handles it on a component it owns
				// or just on the object.
				S32 routingId = 0;

				fnName = U32toSTE(code[ip]);

				ip++;//-Mat to account for our new pointer

				//if this is called from inside a function, append the ip and codeptr
				//-Mat this is used for breakpoints
				if (!gEvalState.DictionaryStack.empty())
				{
					gEvalState.DictionaryStack.last()->code = this;
					//-Mat may need to fix this -1
					gEvalState.DictionaryStack.last()->ip = ip - 1;
				}
				//-Mat advance an extra one for our pointer ( instead of + 2 and += 3)
				U32 callType = code[ip+3];
				ip += 4;

				STR.getArgcArgv(fnName, &callArgc, &callArgv);

				if(callType == FuncCallExprNode::FunctionCall) 
				{
					nsEntry = *((Namespace::Entry **) &code[ip-2]);
					ns = NULL;
				}
				else if(callType == FuncCallExprNode::MethodCall)
				{
					saveObject = gEvalState.thisObject;
					gEvalState.thisObject = Sim::findObject(callArgv[1]);
					if(!gEvalState.thisObject)
					{
						Con::warnf(ConsoleLogEntry::General,"%s: Unable to find object: '%s' attempting to call function '%s'", getFileLine(ip-4), callArgv[1], fnName);
						gEvalState.thisObject = 0;

						STR.popFrame(); // [neo, 5/7/2007 - #2974]

						break;
					}

					bool handlesMethod = gEvalState.thisObject->handlesConsoleMethod(fnName,&routingId);
					if( handlesMethod && routingId == MethodOnComponent )
					{
						DynamicConsoleMethodComponent *pComponent = dynamic_cast<DynamicConsoleMethodComponent*>( gEvalState.thisObject );
						if( pComponent )
							pComponent->callMethodArgList( callArgc, callArgv, false );
					}

					ns = gEvalState.thisObject->getNamespace();
					if(ns) {
						//nsEntry = ns->lookup(fnName);
						//-Mat was lookup()
						nsEntry = ns->lookupMethod(fnName, gEvalState.thisObject );
						//ns->printAllEntries();
					} else {
						nsEntry = NULL;
					}
				}
				else // it's a ParentCall
				{
					if(thisNamespace) {
						//this we can lookupparent right, but later we
						ns = thisNamespace->mParent;
						nsEntry = ns->lookup(fnName);						
					}
					else
					{
						ns = NULL;
						nsEntry = NULL;
					}
				}

				S32 nsType = -1;
				S32 nsMinArgs = 0;
				S32 nsMaxArgs = 0;
				Namespace::Entry::CallbackUnion * nsCb = NULL;
				Namespace::Entry::CallbackUnion cbu;
				const char * nsUsage = NULL;
				if (nsEntry)
				{
					nsType = nsEntry->mType;
					nsMinArgs = nsEntry->mMinArgs;
					nsMaxArgs = nsEntry->mMaxArgs;
					nsCb = &nsEntry->cb;
					nsUsage = nsEntry->mUsage;
					routingId = 0;
				}
				if(!nsEntry || noCalls)
				{
					if(!noCalls && !( routingId == MethodOnComponent ) )
					{
						Con::warnf(ConsoleLogEntry::General,"%s: Unknown command %s.", getFileLine(ip-4), fnName);
						if(callType == FuncCallExprNode::MethodCall)
						{
							Con::warnf(ConsoleLogEntry::General, "  Object %s(%d) %s",
								gEvalState.thisObject->getName() ? gEvalState.thisObject->getName() : "",
								gEvalState.thisObject->getId(), getNamespaceList(ns) );
						}
					}
					STR.popFrame();
					STR.setStringValue("");
					break;
				}
				if(nsEntry->mType == Namespace::Entry::ScriptFunctionType)
				{
					const char *ret = "";
					if(nsEntry->mFunctionOffset) {
						nsEntry->recursionCount++;
						ret = nsEntry->mCode->exec(nsEntry->mFunctionOffset, fnName, nsEntry->mNamespace, callArgc, callArgv, false, nsEntry->mNamespace->getOwner()->mName, nsEntry->recursionCount);
						nsEntry->recursionCount--;
					}
					STR.popFrame();
					STR.setStringValue(ret);
				}
				else
				{
					const char* nsName = ns? ns->mName: "";
#ifndef TORQUE_DEBUG
					// [tom, 12/13/2006] This stops tools functions from working in the console,
					// which is useful behavior when debugging so I'm ifdefing this out for debug builds.
					if(nsEntry->mToolOnly && ! Con::isCurrentScriptToolScript())
					{
						Con::errorf(ConsoleLogEntry::Script, "%s: %s::%s - attempting to call tools only function from outside of tools.", getFileLine(ip-4), nsName, fnName);
					}
					else
#endif
						if((nsEntry->mMinArgs && S32(callArgc) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(callArgc) > nsEntry->mMaxArgs))
						{
							Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", getFileLine(ip-4), nsName, fnName);
							Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", getFileLine(ip-4), nsEntry->mUsage);
							STR.popFrame();
						}
						else
						{
							switch(nsEntry->mType)
							{
							case Namespace::Entry::StringCallbackType:
								{
									const char *ret = nsEntry->cb.mStringCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(ret != STR.getStringValue())
										STR.setStringValue(ret);
									else
										STR.setLen(dStrlen(ret));
									break;
								}
							case Namespace::Entry::IntCallbackType:
								{
									S32 result = nsEntry->cb.mIntCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							case Namespace::Entry::FloatCallbackType:
								{
									F64 result = nsEntry->cb.mFloatCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = (S64)result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setFloatValue(result);
									break;
								}
							case Namespace::Entry::VoidCallbackType:
								nsEntry->cb.mVoidCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
								if(code[ip] != OP_STR_TO_NONE)
									Con::warnf(ConsoleLogEntry::General, "%s: Call to %s in %s uses result of void function call.", getFileLine(ip-4), fnName, functionName);

								STR.popFrame();
								STR.setStringValue("");
								break;
							case Namespace::Entry::BoolCallbackType:
								{
									bool result = nsEntry->cb.mBoolCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							}
						}
				}

				if(callType == FuncCallExprNode::MethodCall)
					gEvalState.thisObject = saveObject;
				break;
			}
		case OP_ADVANCE_STR:
			STR.advance();
			break;
		case OP_ADVANCE_STR_APPENDCHAR:
			STR.advanceChar(code[ip++]);
			break;

		case OP_ADVANCE_STR_COMMA:
			STR.advanceChar('_');
			break;

		case OP_ADVANCE_STR_NUL:
			STR.advanceChar(0);
			break;

		case OP_REWIND_STR:
			STR.rewind();
			break;

		case OP_TERMINATE_REWIND_STR:
			STR.rewindTerminate();
			break;

		case OP_COMPARE_STR:
			intStack[++UINT] = STR.compare();
			break;
		case OP_PUSH:
			STR.push();
			break;

		case OP_PUSH_FRAME:
			STR.pushFrame();
			break;
		case OP_BREAK:
			{
				//append the ip and codeptr before managing the breakpoint!
				AssertFatal( !gEvalState.DictionaryStack.empty(), "Empty eval DictionaryStack on break!");
				gEvalState.DictionaryStack.last()->code = this;
				gEvalState.DictionaryStack.last()->ip = ip - 1;
				U32 breakLine;
				findBreakLine(ip-1, breakLine, instruction);
				if(!breakLine)
					goto breakContinue;
				TelDebugger->executionStopped(this, breakLine);
				goto breakContinue;
			}
		case OP_INVALID:

		default:
			// error!
			goto execFinished;
		}
	}
execFinished:

	if ( telDebuggerOn && setFrame < 0 )
		TelDebugger->popStackFrame();

	if ( popFrame )
		gEvalState.popFrame();

	if(argv)
	{
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Leaving ");

			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s() - return %s", thisNamespace->mName, thisFunctionName, STR.getStringValue());
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s() - return %s", thisFunctionName, STR.getStringValue());
			}
			Con::printf("%s", traceBuffer);
		}
	}
	else
	{
		//-Mat this may leak
		delete[] const_cast<char*>(globalStrings);
		delete[] globalFloats;
		globalStrings = NULL;
		globalFloats = NULL;
	}
	smCurrentCodeBlock = saveCodeBlock;
	if(saveCodeBlock && saveCodeBlock->name)
	{
		Con::gCurrentFile = saveCodeBlock->name;
		Con::gCurrentRoot = saveCodeBlock->mRoot;
	}

	decRefCount();

#ifdef TORQUE_DEBUG
	AssertFatal(!(STR.mStartStackSize > stackStart), "String stack not popped enough in script exec");
	AssertFatal(!(STR.mStartStackSize < stackStart), "String stack popped too much in script exec");
#endif
	return STR.getStringValue();
}

#else

const char *CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, const char **argv, bool noCalls, StringTableEntry packageName, S32 setFrame, Namespace::Entry *parentEntry )
{
#ifdef TORQUE_DEBUG
	U32 stackStart = STR.mStartStackSize;
#endif

	static char traceBuffer[1024];
	U32 i;

	incRefCount();
	F64 *curFloatTable;
	char *curStringTable;
	STR.clearFunctionOffset();
	StringTableEntry thisFunctionName = NULL;
	bool popFrame = false;
	if(argv)
	{
		// assume this points into a function decl:
		U32 fnArgc = code[ip + 5];
		thisFunctionName = U32toSTE(code[ip]);
		argc = getMin(argc-1, fnArgc); // argv[0] is func name
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Entering ");
			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s(", thisNamespace->mName, thisFunctionName);
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s(", thisFunctionName);
			}
			for(i = 0; i < argc; i++)
			{
				dStrcat(traceBuffer, argv[i+1]);
				if(i != argc - 1)
					dStrcat(traceBuffer, ", ");
			}
			dStrcat(traceBuffer, ")");
			Con::printf("%s", traceBuffer);
		}

		//-Mat this is the frame of variables for each function CALL
		gEvalState.pushFrame(thisFunctionName, thisNamespace, setFrame);
		popFrame = true;
		for(i = 0; i < argc; i++)
		{
			StringTableEntry var = U32toSTE(code[ip + i + 6]);
			gEvalState.setCurVarNameCreate(var,thisFunctionName);//-Mat pass in function name
			gEvalState.setStringVariable(argv[i+1]);
		}
		ip = ip + fnArgc + 6;
		curFloatTable = functionFloats;
		curStringTable = functionStrings;
	}
	else
	{
		curFloatTable = globalFloats;
		curStringTable = globalStrings;

		// Do we want this code to execute using a new DictionaryStack frame?
		if (setFrame < 0)
		{
			gEvalState.pushFrame(NULL, NULL, setFrame);
			popFrame = true;
		}
		else if (!gEvalState.DictionaryStack.empty())
		{
			// We want to copy a reference to an existing DictionaryStack frame
			// on to the top of the DictionaryStack.  Any change that occurs to 
			// the locals during this new frame will also occur in the 
			// original frame.
			//S32 stackIndex = gEvalState.DictionaryStack.size() - setFrame - 1;
			//gEvalState.pushFrameRef( stackIndex );
			popFrame = false;
		}
	}

	// Grab the state of the telenet debugger here once
	// so that the push and pop frames are always balanced.
	const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
	if ( telDebuggerOn && setFrame < 0 )
		TelDebugger->pushStackFrame();

	StringTableEntry var, objParent;
	U32 failJump;
	StringTableEntry fnName;
	StringTableEntry fnNamespace, fnPackage;
	SimObject *currentNewObject = 0;
	StringTableEntry prevField = NULL;
	StringTableEntry curField = NULL;
	SimObject *prevObject = NULL;
	SimObject *curObject = NULL;
	SimObject *saveObject=NULL;
	Namespace::Entry *nsEntry;
	Namespace *ns;
	Package *pg = NULL;
	const char* curFNDocBlock = NULL;
	const char* curNSDocBlock = NULL;
	const S32 nsDocLength = 128;
	char nsDocBlockClass[nsDocLength];

	U32 callArgc;
	const char **callArgv;

	static char curFieldArray[256];
	static char prevFieldArray[256];

	CodeBlock *saveCodeBlock = smCurrentCodeBlock;
	smCurrentCodeBlock = this;
	if(this->name)
	{
		Con::gCurrentFile = this->name;
		Con::gCurrentRoot = mRoot;
	}
	const char * val;

	// The frame temp is used by the variable accessor ops (OP_SAVEFIELD_* and
	// OP_LOADFIELD_*) to store temporary values for the fields.
	static S32 VAL_BUFFER_SIZE = 1024;
	FrameTemp<char> valBuffer( VAL_BUFFER_SIZE );

	for(;;)
	{
		U32 instruction = code[ip++];
breakContinue:
		switch(instruction)
		{
		case OP_FUNC_DECL:
			if(!noCalls)
			{
				fnName       = U32toSTE(code[ip]);
				fnNamespace  = U32toSTE(code[ip+1]);
				fnPackage    = U32toSTE(code[ip+2]);
				bool hasBody = bool(code[ip+3]);

				//Package::unlinkPackages();
				ns = Namespace::findAndCreate(fnNamespace, fnPackage);
				ns->addFunction(fnName, this, hasBody ? ip : 0, curFNDocBlock ? dStrdup( curFNDocBlock ) : NULL );// if no body, set the IP to 0
				if( curNSDocBlock )
				{
					if( fnNamespace == StringTable->lookup( nsDocBlockClass ) )
					{
						char *usageStr = dStrdup( curNSDocBlock );
						usageStr[dStrlen(usageStr)] = '\0';
						ns->mUsage = usageStr;
						ns->mCleanUpUsage = true;
						curNSDocBlock = NULL;
					}
				}
				//Package::relinkPackages();

				// If we had a docblock, it's definitely not valid anymore, so clear it out.
				curFNDocBlock = NULL;

				//Con::printf("Adding function %s::%s (%d)", fnNamespace, fnName, ip);
			}
			ip = code[ip + 4];
			break;

		case OP_CREATE_OBJECT:
			{
				// Read some useful info.
				objParent        = U32toSTE(code[ip    ]);
				bool isDataBlock =          code[ip + 1];
				bool isInternal  =          code[ip + 2];
				bool isMessage   =          code[ip + 3];
				failJump         =          code[ip + 4];

				// If we don't allow calls, we certainly don't allow creating objects!
				// Moved this to after failJump is set. Engine was crashing when
				// noCalls = true and an object was being created at the beginning of
				// a file. ADL.
				if(noCalls)
				{
					ip = failJump;
					break;
				}

				// Get the constructor information off the stack.
				STR.getArgcArgv(NULL, &callArgc, &callArgv, true);

				// Con::printf("Creating object...");

				// objectName = argv[1]...
				currentNewObject = NULL;

				// Are we creating a datablock? If so, deal with case where we override
				// an old one.
				if(isDataBlock)
				{
					// Con::printf("  - is a datablock");

					// Find the old one if any.
					SimObject *db = Sim::getDataBlockGroup()->findObject(callArgv[2]);

					// Make sure we're not changing types on ourselves...
					if(db && dStricmp(db->getClassName(), callArgv[1]))
					{
						Con::errorf(ConsoleLogEntry::General, "Cannot re-declare data block %s with a different class.", callArgv[2]);
						ip = failJump;
						break;
					}

					// If there was one, set the currentNewObject and move on.
					if(db)
						currentNewObject = db;
				}

				if(!currentNewObject)
				{
					// Well, looks like we have to create a new object.
					ConsoleObject *object = ConsoleObject::create(callArgv[1]);

					// Deal with failure!
					if(!object)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-conobject class %s.", getFileLine(ip-1), callArgv[1]);
						ip = failJump;
						break;
					}

					// Do special datablock init if appropros
					if(isDataBlock)
					{
						SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(object);
						if(dataBlock)
						{
							dataBlock->assignId();
						}
						else
						{
							// They tried to make a non-datablock with a datablock keyword!
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-datablock class %s.", getFileLine(ip-1), callArgv[1]);

							// Clean up...
							delete object;
							ip = failJump;
							break;
						}
					}

					// Finally, set currentNewObject to point to the new one.
					currentNewObject = dynamic_cast<SimObject *>(object);

					// Deal with the case of a non-SimObject.
					if(!currentNewObject)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-SimObject class %s.", getFileLine(ip-1), callArgv[1]);
						delete object;
						ip = failJump;
						break;
					}

					// Does it have a parent object? (ie, the copy constructor : syntax, not inheriance)
					// [tom, 9/8/2006] it is inheritance if it's a message ... muwahahah!
					//-Mat if objParent is NULL, evaluating *objParent will crash
					if(!isMessage && objParent != NULL && *objParent)
					{
						// Find it!
						SimObject *parent;
						if(Sim::findObject(objParent, parent))
						{
							// Con::printf(" - Parent object found: %s", parent->getClassName());

							// and suck the juices from it!
							currentNewObject->assignFieldsFrom(parent);
						}
						else
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to find parent object %s for %s.", getFileLine(ip-1), objParent, callArgv[1]);

						// Mm! Juices!
					}

					// If a name was passed, assign it.
					if(callArgv[2][0])
					{
						if(! isMessage)
						{
							if(! isInternal)
								currentNewObject->assignName(callArgv[2]);
							else
								currentNewObject->setInternalName(callArgv[2]);
						}
						else
						{
							Message *msg = dynamic_cast<Message *>(currentNewObject);
							if(msg)
							{
								msg->setClassNamespace(callArgv[2]);
								msg->setSuperClassNamespace(objParent);
							}
							else
							{
								Con::errorf(ConsoleLogEntry::General, "%s: Attempting to use newmsg on non-message type %s", getFileLine(ip-1), callArgv[1]);
								delete currentNewObject;
								currentNewObject = NULL;
								ip = failJump;
								break;
							}
						}
					}

					// Do the constructor parameters.
					if(!currentNewObject->processArguments(callArgc-3, callArgv+3))
					{
						delete currentNewObject;
						currentNewObject = NULL;
						ip = failJump;
						break;
					}

					// If it's not a datablock, allow people to modify bits of it.
					if(!isDataBlock)
					{
						currentNewObject->setModStaticFields(true);
						currentNewObject->setModDynamicFields(true);
					}
				}

				// Advance the IP past the create info...
				ip += 5;
				break;
			}

		case OP_ADD_OBJECT:
			{
				// See OP_SETCURVAR for why we do this.
				curFNDocBlock = NULL;
				curNSDocBlock = NULL;

				// Do we place this object at the root?
				bool placeAtRoot = code[ip++];

				// Con::printf("Adding object %s", currentNewObject->getName());

				// Make sure it wasn't already added, then add it.
				if(currentNewObject->isProperlyAdded() == false)
				{
					bool ret = false;

					Message *msg = dynamic_cast<Message *>(currentNewObject);
					if(msg)
					{
						SimObjectId id = Message::getNextMessageID();
						if(id != 0xffffffff)
							ret = currentNewObject->registerObject(id);
						else
							Con::errorf("%s: No more object IDs available for messages", getFileLine(ip-2));
					}
					else
						ret = currentNewObject->registerObject();

					if(! ret)
					{
						// This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
						Con::warnf(ConsoleLogEntry::General, "%s: Register object failed for object %s of class %s.", getFileLine(ip-2), currentNewObject->getName(), currentNewObject->getClassName());
						delete currentNewObject;
						ip = failJump;
						break;
					}
				}

				// Are we dealing with a datablock?
				SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(currentNewObject);
				static char errorBuffer[256];

				// If so, preload it.
				if(dataBlock && !dataBlock->preload(true, errorBuffer))
				{
					Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", getFileLine(ip-2),
						currentNewObject->getName(), errorBuffer);
					dataBlock->deleteObject();
					ip = failJump;
					break;
				}

				// What group will we be added to, if any?
				U32 groupAddId = intStack[UINT];
				SimGroup *grp = NULL;
				SimSet   *set = NULL;
				SimComponent *comp = NULL;
				bool isMessage = dynamic_cast<Message *>(currentNewObject) != NULL;

				if(!placeAtRoot || !currentNewObject->getGroup())
				{
					if(! isMessage)
					{
						if(! placeAtRoot)
						{
							// Otherwise just add to the requested group or set.
							if(!Sim::findObject(groupAddId, grp))
								if(!Sim::findObject(groupAddId, comp))
									Sim::findObject(groupAddId, set);
						}

						if(placeAtRoot || comp != NULL)
						{
							// Deal with the instantGroup if we're being put at the root or we're adding to a component.
							const char *addGroupName = Con::getVariable("instantGroup");
							if(!Sim::findObject(addGroupName, grp))
								Sim::findObject(RootGroupId, grp);
						}

						if(comp)
						{
							SimComponent *newComp = dynamic_cast<SimComponent *>(currentNewObject);
							if(newComp)
							{
								if(! comp->addComponent(newComp))
									Con::errorf("%s: Unable to add component %s, template not loaded?", getFileLine(ip-2), currentNewObject->getName() ? currentNewObject->getName() : currentNewObject->getIdString());
							}
						}
					}

					// If we didn't get a group, then make sure we have a pointer to
					// the rootgroup.
					if(!grp)
						Sim::findObject(RootGroupId, grp);

					// add to the parent group
					grp->addObject(currentNewObject);

					// add to any set we might be in
					if(set)
						set->addObject(currentNewObject);
				}

				// store the new object's ID on the stack (overwriting the group/set
				// id, if one was given, otherwise getting pushed)
				if(placeAtRoot) 
					intStack[UINT] = currentNewObject->getId();
				else
					intStack[++UINT] = currentNewObject->getId();

				break;
			}

		case OP_END_OBJECT:
			{
				// If we're not to be placed at the root, make sure we clean up
				// our group reference.
				bool placeAtRoot = code[ip++];
				if(!placeAtRoot)
					UINT--;
				break;
			}

		case OP_JMPIFFNOT:
			if(floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT:
			if(intStack[UINT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFF:
			if(!floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF:
			if(!intStack[UINT--])
			{
				ip ++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT_NP:
			if(intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF_NP:
			if(!intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMP:
			ip = code[ip];
			break;
		case OP_RETURN:
			goto execFinished;
		case OP_CMPEQ:
			intStack[UINT+1] = bool(floatStack[FLT] == floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGR:
			intStack[UINT+1] = bool(floatStack[FLT] > floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGE:
			intStack[UINT+1] = bool(floatStack[FLT] >= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLT:
			intStack[UINT+1] = bool(floatStack[FLT] < floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLE:
			intStack[UINT+1] = bool(floatStack[FLT] <= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPNE:
			intStack[UINT+1] = bool(floatStack[FLT] != floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_XOR:
			intStack[UINT-1] = intStack[UINT] ^ intStack[UINT-1];
			UINT--;
			break;

		case OP_MOD:
			if(  intStack[UINT-1] != 0 )
				intStack[UINT-1] = intStack[UINT] % intStack[UINT-1];
			else
				intStack[UINT-1] = 0;
			UINT--;
			break;

		case OP_BITAND:
			intStack[UINT-1] = intStack[UINT] & intStack[UINT-1];
			UINT--;
			break;

		case OP_BITOR:
			intStack[UINT-1] = intStack[UINT] | intStack[UINT-1];
			UINT--;
			break;

		case OP_NOT:
			intStack[UINT] = !intStack[UINT];
			break;

		case OP_NOTF:
			intStack[UINT+1] = !floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_ONESCOMPLEMENT:
			intStack[UINT] = ~intStack[UINT];
			break;

		case OP_SHR:
			intStack[UINT-1] = intStack[UINT] >> intStack[UINT-1];
			UINT--;
			break;

		case OP_SHL:
			intStack[UINT-1] = intStack[UINT] << intStack[UINT-1];
			UINT--;
			break;

		case OP_AND:
			intStack[UINT-1] = intStack[UINT] && intStack[UINT-1];
			UINT--;
			break;

		case OP_OR:
			intStack[UINT-1] = intStack[UINT] || intStack[UINT-1];
			UINT--;
			break;

		case OP_ADD:
			floatStack[FLT-1] = floatStack[FLT] + floatStack[FLT-1];
			FLT--;
			break;

		case OP_SUB:
			floatStack[FLT-1] = floatStack[FLT] - floatStack[FLT-1];
			FLT--;
			break;

		case OP_MUL:
			floatStack[FLT-1] = floatStack[FLT] * floatStack[FLT-1];
			FLT--;
			break;
		case OP_DIV:
			floatStack[FLT-1] = floatStack[FLT] / floatStack[FLT-1];
			FLT--;
			break;
		case OP_NEG:
			floatStack[FLT] = -floatStack[FLT];
			break;

		case OP_SETCURVAR:
			var = U32toSTE(code[ip]);
			ip++;

			// If a variable is set, then these must be NULL. It is necessary
			// to set this here so that the vector parser can appropriately
			// identify whether it's dealing with a vector.
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function

			// In order to let docblocks work properly with variables, we have
			// clear the current docblock when we do an assign. This way it 
			// won't inappropriately carry forward to following function decls.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_CREATE:
			var = U32toSTE(code[ip]);
			ip++;

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarNameCreate(var,thisFunctionName);//-Mat pass in function name		

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_ARRAY:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_ARRAY_CREATE:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarNameCreate(var,thisFunctionName);//-Mat pass in function name

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_LOADVAR_UINT:
			intStack[UINT+1] = gEvalState.getIntVariable();
			UINT++;
			break;

		case OP_LOADVAR_FLT:
			floatStack[FLT+1] = gEvalState.getFloatVariable();
			FLT++;
			break;

		case OP_LOADVAR_STR:
			val = gEvalState.getStringVariable();
			STR.setStringValue(val);
			break;

		case OP_SAVEVAR_UINT:
			gEvalState.setIntVariable(intStack[UINT]);
			break;

		case OP_SAVEVAR_FLT:
			gEvalState.setFloatVariable(floatStack[FLT]);
			break;

		case OP_SAVEVAR_STR:
			gEvalState.setStringVariable(STR.getStringValue());
			break;

		case OP_SETCUROBJECT:
			// Save the previous object for parsing vector fields.
			prevObject = curObject;
			val = STR.getStringValue();

			// Sim::findObject will sometimes find valid objects from
			// multi-component strings. This makes sure that doesn't
			// happen.
			for( const char* check = val; *check; check++ )
			{
				if( *check == ' ' )
				{
					val = "";
					break;
				}
			}
			curObject = Sim::findObject(val);
			break;

		case OP_SETCUROBJECT_INTERNAL:
			++ip; // To skip the recurse flag if the object wasnt found
			if(curObject)
			{
				SimGroup *group = dynamic_cast<SimGroup *>(curObject);
				if(group)
				{
					StringTableEntry intName = StringTable->insert(STR.getStringValue());
					bool recurse = code[ip-1];
					SimObject *obj = group->findObjectByInternalName(intName, recurse);
					intStack[UINT+1] = obj ? obj->getId() : 0;
					UINT++;
				}
				else
				{
					Con::errorf(ConsoleLogEntry::Script, "%s: Attempt to use -> on non-group %s of class %s.", getFileLine(ip-2), curObject->getName(), curObject->getClassName());
					intStack[UINT] = 0;
				}
			}
			break;

		case OP_SETCUROBJECT_NEW:
			curObject = currentNewObject;
			break;

		case OP_SETCURFIELD:
			// Save the previous field for parsing vector fields.
			prevField = curField;
			dStrcpy( prevFieldArray, curFieldArray );
			curField = U32toSTE(code[ip]);
			curFieldArray[0] = 0;
			ip++;
			break;

		case OP_SETCURFIELD_ARRAY:
			dStrcpy(curFieldArray, STR.getStringValue());
			break;

		case OP_LOADFIELD_UINT:
			if(curObject)
				intStack[UINT+1] = U32(dAtoi(curObject->getDataField(curField, curFieldArray)));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				intStack[UINT+1] = dAtoi( valBuffer );
			}
			UINT++;
			break;

		case OP_LOADFIELD_FLT:
			if(curObject)
				floatStack[FLT+1] = dAtof(curObject->getDataField(curField, curFieldArray));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				floatStack[FLT+1] = dAtof( valBuffer );
			}
			FLT++;
			break;

		case OP_LOADFIELD_STR:
			if(curObject)
			{
				val = curObject->getDataField(curField, curFieldArray);
				STR.setStringValue( val );
			}
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				STR.setStringValue( valBuffer );
			}

			break;

		case OP_SAVEFIELD_UINT:
			STR.setIntValue(intStack[UINT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_FLT:
			STR.setFloatValue(floatStack[FLT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_STR:
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_STR_TO_UINT:
			intStack[UINT+1] = STR.getIntValue();
			UINT++;
			break;

		case OP_STR_TO_FLT:
			floatStack[FLT+1] = STR.getFloatValue();
			FLT++;
			break;

		case OP_STR_TO_NONE:
			// This exists simply to deal with certain typecast situations.
			break;

		case OP_FLT_TO_UINT:
			intStack[UINT+1] = (S64)floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_FLT_TO_STR:
			STR.setFloatValue(floatStack[FLT]);
			FLT--;
			break;

		case OP_FLT_TO_NONE:
			FLT--;
			break;

		case OP_UINT_TO_FLT:
			floatStack[FLT+1] = intStack[UINT];
			UINT--;
			FLT++;
			break;

		case OP_UINT_TO_STR:
			STR.setIntValue(intStack[UINT]);
			UINT--;
			break;

		case OP_UINT_TO_NONE:
			UINT--;
			break;

		case OP_LOADIMMED_UINT:
			intStack[UINT+1] = code[ip++];
			UINT++;
			break;

		case OP_LOADIMMED_FLT:
			floatStack[FLT+1] = curFloatTable[code[ip]];
			ip++;
			FLT++;
			break;
		case OP_TAG_TO_STR:
			code[ip-1] = OP_LOADIMMED_STR;
			// it's possible the string has already been converted
			if(U8(curStringTable[code[ip]]) != StringTagPrefixByte)
			{
				U32 id = GameAddTaggedString(curStringTable + code[ip]);
				dSprintf(curStringTable + code[ip] + 1, 7, "%d", id);
				*(curStringTable + code[ip]) = StringTagPrefixByte;
			}
		case OP_LOADIMMED_STR:
			STR.setStringValue(curStringTable + code[ip++]);
			break;

		case OP_DOCBLOCK_STR:
			{
				// If the first word of the doc is '\class' or '@class', then this
				// is a namespace doc block, otherwise it is a function doc block.
				const char* docblock = curStringTable + code[ip++];

				const char* sansClass = dStrstr( docblock, "@class" );
				if( !sansClass )
					sansClass = dStrstr( docblock, "\\class" );

				if( sansClass )
				{
					// Don't save the class declaration. Scan past the 'class'
					// keyword and up to the first whitespace.
					sansClass += 7;
					S32 index = 0;
					while( ( *sansClass != ' ' ) && ( *sansClass != '\n' ) && *sansClass && ( index < ( nsDocLength - 1 ) ) )
					{
						nsDocBlockClass[index++] = *sansClass;
						sansClass++;
					}
					nsDocBlockClass[index] = '\0';

					curNSDocBlock = sansClass + 1;
				}
				else
					curFNDocBlock = docblock;
			}

			break;

		case OP_LOADIMMED_IDENT:
			STR.setStringValue(U32toSTE(code[ip++]));
			break;

		case OP_CALLFUNC_RESOLVE:
			{
				fnName      = U32toSTE(code[ip]);
				fnNamespace = U32toSTE(code[ip+1]);
				// Try to look it up.
				ns = Namespace::findAndCreate( fnNamespace, Package::getCurrentPackage()->mName );
				nsEntry = ns->lookup(fnName);
				if(!nsEntry)
				{
					ip+= 3;
					Con::warnf(ConsoleLogEntry::General,
						"%s: Unable to find function %s%s%s",
						getFileLine(ip-4), fnNamespace ? fnNamespace : "",
						fnNamespace ? "::" : "", fnName);
					STR.popFrame();
					break;
				}
				// Now, rewrite our code a bit (ie, avoid future lookups) and fall
				// through to OP_CALLFUNC
				code[ip+1] = *((U32 *) &nsEntry);
				code[ip-1] = OP_CALLFUNC;
			}

		case OP_CALLFUNC:
			{
				// This routingId is set when we query the object as to whether
				// it handles this method.  It is set to an enum from the table
				// above indicating whether it handles it on a component it owns
				// or just on the object.
				S32 routingId = 0;

				fnName = U32toSTE(code[ip]);
				//if this is called from inside a function, append the ip and codeptr
				if (!gEvalState.DictionaryStack.empty())
				{
					gEvalState.DictionaryStack.last()->code = this;
					//-Mat may need to fix this -1
					gEvalState.DictionaryStack.last()->ip = ip - 1;
				}
				U32 callType = code[ip+2];
				ip += 3;

				STR.getArgcArgv(fnName, &callArgc, &callArgv);

				if(callType == FuncCallExprNode::FunctionCall) 
				{
					nsEntry = *((Namespace::Entry **) &code[ip-2]);
					ns = NULL;
				}
				else if(callType == FuncCallExprNode::MethodCall)
				{
					saveObject = gEvalState.thisObject;
					gEvalState.thisObject = Sim::findObject(callArgv[1]);
					if(!gEvalState.thisObject)
					{
						Con::warnf(ConsoleLogEntry::General,"%s: Unable to find object: '%s' attempting to call function '%s'", getFileLine(ip-4), callArgv[1], fnName);
						gEvalState.thisObject = 0;

						STR.popFrame(); // [neo, 5/7/2007 - #2974]

						break;
					}

					bool handlesMethod = gEvalState.thisObject->handlesConsoleMethod(fnName,&routingId);
					if( handlesMethod && routingId == MethodOnComponent )
					{
						DynamicConsoleMethodComponent *pComponent = dynamic_cast<DynamicConsoleMethodComponent*>( gEvalState.thisObject );
						if( pComponent )
							pComponent->callMethodArgList( callArgc, callArgv, false );
					}

					ns = gEvalState.thisObject->getNamespace();
					if(ns) {
						//nsEntry = ns->lookup(fnName);
						//-Mat was lookup()
						nsEntry = ns->lookupMethod(fnName, gEvalState.thisObject );
						//ns->printAllEntries();
					} else {
						nsEntry = NULL;
					}
				}
				else // it's a ParentCall
				{
					if(thisNamespace)
					{
						ns = thisNamespace->mParent;
						if(ns)
							nsEntry = ns->lookup(fnName);
						else
							nsEntry = NULL;
					}
					else
					{
						ns = NULL;
						nsEntry = NULL;
					}
				}

				S32 nsType = -1;
				S32 nsMinArgs = 0;
				S32 nsMaxArgs = 0;
				Namespace::Entry::CallbackUnion * nsCb = NULL;
				Namespace::Entry::CallbackUnion cbu;
				const char * nsUsage = NULL;
				if (nsEntry)
				{
					nsType = nsEntry->mType;
					nsMinArgs = nsEntry->mMinArgs;
					nsMaxArgs = nsEntry->mMaxArgs;
					nsCb = &nsEntry->cb;
					nsUsage = nsEntry->mUsage;
					routingId = 0;
				}
				if(!nsEntry || noCalls)
				{
					if(!noCalls && !( routingId == MethodOnComponent ) )
					{
						Con::warnf(ConsoleLogEntry::General,"%s: Unknown command %s.", getFileLine(ip-4), fnName);
						if(callType == FuncCallExprNode::MethodCall)
						{
							Con::warnf(ConsoleLogEntry::General, "  Object %s(%d) %s",
								gEvalState.thisObject->getName() ? gEvalState.thisObject->getName() : "",
								gEvalState.thisObject->getId(), getNamespaceList(ns) );
						}
					}
					STR.popFrame();
					STR.setStringValue("");
					break;
				}
				if(nsEntry->mType == Namespace::Entry::ScriptFunctionType)
				{
					const char *ret = "";
					if(nsEntry->mFunctionOffset) {
						//has extra stuff that messes things up
						//ret = nsEntry->execute( callArgc, callArgv );
						nsEntry->recursionCount++;
						ret = nsEntry->mCode->exec(nsEntry->mFunctionOffset, fnName, nsEntry->mNamespace, callArgc, callArgv, false, nsEntry->mNamespace->getOwner()->mName, nsEntry->recursionCount);
						nsEntry->recursionCount--;
					}
					STR.popFrame();
					STR.setStringValue(ret);
				}
				else
				{
					const char* nsName = ns ? ns->mName: "";
#ifndef TORQUE_DEBUG
					// [tom, 12/13/2006] This stops tools functions from working in the console,
					// which is useful behavior when debugging so I'm ifdefing this out for debug builds.
					if(nsEntry->mToolOnly && ! Con::isCurrentScriptToolScript())
					{
						Con::errorf(ConsoleLogEntry::Script, "%s: %s::%s - attempting to call tools only function from outside of tools.", getFileLine(ip-4), nsName, fnName);
					}
					else
#endif
						if((nsEntry->mMinArgs && S32(callArgc) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(callArgc) > nsEntry->mMaxArgs))
						{
							Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", getFileLine(ip-4), nsName, fnName);
							Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", getFileLine(ip-4), nsEntry->mUsage);
							STR.popFrame();
						}
						else
						{
							switch(nsEntry->mType)
							{
							case Namespace::Entry::StringCallbackType:
								{
									const char *ret = nsEntry->cb.mStringCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(ret != STR.getStringValue())
										STR.setStringValue(ret);
									else
										STR.setLen(dStrlen(ret));
									break;
								}
							case Namespace::Entry::IntCallbackType:
								{
									S32 result = nsEntry->cb.mIntCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							case Namespace::Entry::FloatCallbackType:
								{
									F64 result = nsEntry->cb.mFloatCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = (S64)result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setFloatValue(result);
									break;
								}
							case Namespace::Entry::VoidCallbackType:
								nsEntry->cb.mVoidCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
								if(code[ip] != OP_STR_TO_NONE)
									Con::warnf(ConsoleLogEntry::General, "%s: Call to %s in %s uses result of void function call.", getFileLine(ip-4), fnName, functionName);

								STR.popFrame();
								STR.setStringValue("");
								break;
							case Namespace::Entry::BoolCallbackType:
								{
									bool result = nsEntry->cb.mBoolCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							}
						}
				}

				if(callType == FuncCallExprNode::MethodCall)
					gEvalState.thisObject = saveObject;
				break;
			}
		case OP_ADVANCE_STR:
			STR.advance();
			break;
		case OP_ADVANCE_STR_APPENDCHAR:
			STR.advanceChar(code[ip++]);
			break;

		case OP_ADVANCE_STR_COMMA:
			STR.advanceChar('_');
			break;

		case OP_ADVANCE_STR_NUL:
			STR.advanceChar(0);
			break;

		case OP_REWIND_STR:
			STR.rewind();
			break;

		case OP_TERMINATE_REWIND_STR:
			STR.rewindTerminate();
			break;

		case OP_COMPARE_STR:
			intStack[++UINT] = STR.compare();
			break;
		case OP_PUSH:
			STR.push();
			break;

		case OP_PUSH_FRAME:
			STR.pushFrame();
			break;
		case OP_BREAK:
			{
				//append the ip and codeptr before managing the breakpoint!
				AssertFatal( !gEvalState.DictionaryStack.empty(), "Empty eval DictionaryStack on break!");
				gEvalState.DictionaryStack.last()->code = this;
				gEvalState.DictionaryStack.last()->ip = ip - 1;
				U32 breakLine;
				findBreakLine(ip-1, breakLine, instruction);
				if(!breakLine)
					goto breakContinue;
				TelDebugger->executionStopped(this, breakLine);
				goto breakContinue;
			}
		case OP_INVALID:

		default:
			// error!
			goto execFinished;
		}
	}
execFinished:

	if ( telDebuggerOn && setFrame < 0 )
		TelDebugger->popStackFrame();

	if ( popFrame )
		gEvalState.popFrame();

	if(argv)
	{
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Leaving ");

			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s() - return %s", thisNamespace->mName, thisFunctionName, STR.getStringValue());
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s() - return %s", thisFunctionName, STR.getStringValue());
			}
			Con::printf("%s", traceBuffer);
		}
	}
	else
	{
		//-Mat this may leak
		delete[] const_cast<char*>(globalStrings);
		delete[] globalFloats;
		globalStrings = NULL;
		globalFloats = NULL;
	}
	smCurrentCodeBlock = saveCodeBlock;
	if(saveCodeBlock && saveCodeBlock->name)
	{
		Con::gCurrentFile = saveCodeBlock->name;
		Con::gCurrentRoot = saveCodeBlock->mRoot;
	}

	decRefCount();

#ifdef TORQUE_DEBUG
	AssertFatal(!(STR.mStartStackSize > stackStart), "String stack not popped enough in script exec");
	AssertFatal(!(STR.mStartStackSize < stackStart), "String stack popped too much in script exec");
#endif
	return STR.getStringValue();
}

#endif //PUAP_SCRIPT_CHANGE

//------------------------------------------------------------

#else //normal TGB

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

#include "console/simBase.h"
#include "console/telnetDebugger.h"
#include "sim/netStringTable.h"
#include "component/dynamicConsoleMethodComponent.h"
#include "console/stringStack.h"
#include "util/messaging/message.h"
#include "core/frameAllocator.h"

using namespace Compiler;

enum EvalConstants {
	MaxStackSize = 1024,
	MethodOnComponent = -2
};

namespace Con
{
	// Current script file name and root, these are registered as
	// console variables.
	extern StringTableEntry gCurrentFile;
	extern StringTableEntry gCurrentRoot;
}

F64 floatStack[MaxStackSize];
S64 intStack[MaxStackSize];

StringStack STR;

U32 FLT = 0;
U32 UINT = 0;

static const char *getNamespaceList(Namespace *ns)
{
	U32 size = 1;
	Namespace * walk;
	for(walk = ns; walk; walk = walk->mParent)
		size += dStrlen(walk->mName) + 4;
	char *ret = Con::getReturnBuffer(size);
	ret[0] = 0;
	for(walk = ns; walk; walk = walk->mParent)
	{
		dStrcat(ret, walk->mName);
		if(walk->mParent)
			dStrcat(ret, " -> ");
	}
	return ret;
}

//------------------------------------------------------------

F64 consoleStringToNumber(const char *str, StringTableEntry file, U32 line)
{
	F64 val = dAtof(str);
	if(val != 0)
		return val;
	else if(!dStricmp(str, "true"))
		return 1;
	else if(!dStricmp(str, "false"))
		return 0;
	else if(file)
	{
		Con::warnf(ConsoleLogEntry::General, "%s (%d): string always evaluates to 0.", file, line);
		return 0;
	}
	return 0;
}

//------------------------------------------------------------

namespace Con
{

	char *getReturnBuffer(U32 bufferSize)

	{
		return STR.getReturnBuffer(bufferSize);
	}

	char *getReturnBuffer( const char *stringToCopy )
	{
		char *ret = STR.getReturnBuffer( dStrlen( stringToCopy ) + 1 );
		dStrcpy( ret, stringToCopy );
		ret[dStrlen( stringToCopy )] = '\0';
		return ret;
	}

	char *getArgBuffer(U32 bufferSize)
	{
		return STR.getArgBuffer(bufferSize);
	}

	char *getFloatArg(F64 arg)
	{

		char *ret = STR.getArgBuffer(32);
		dSprintf(ret, 32, "%g", arg);
		return ret;
	}

	char *getIntArg(S32 arg)
	{

		char *ret = STR.getArgBuffer(32);
		dSprintf(ret, 32, "%d", arg);
		return ret;
	}
}

//------------------------------------------------------------
#ifdef PUAP_SCRIPT_CHANGE
inline void ExprEvalState::setCurVarName(StringTableEntry name,StringTableEntry parentName)
{
	if(name[0] == '$')
		currentVariable = globalVars.lookup( name, StringTable->insert( "GlobalVar" ) );
	else if(DictionaryStack.size())
		currentVariable = DictionaryStack.last()->lookup(name,parentName);
	if(!currentVariable && gWarnUndefinedScriptVariables)
		Con::warnf(ConsoleLogEntry::Script, "Variable referenced before assignment: %s", name);
}

inline void ExprEvalState::setCurParamName(StringTableEntry name,StringTableEntry parentName)
{
	if(name[0] == '$') {
		//this shouldn't happen on parameters
		currentVariable = globalVars.lookup( name, StringTable->insert( "GlobalVar" ) );
	} else if(DictionaryStack.size()) {
		//Parameters must only be recursive, not looked up from inherited Dictionary
		currentVariable = DictionaryStack.last()->lookupLocal(name,parentName);
		if( currentVariable == NULL ) {
			currentVariable = DictionaryStack.last()->add(name,parentName,false);//don't do a lookup(recursively)
		}
	}
	if(!currentVariable && gWarnUndefinedScriptVariables)
		Con::warnf(ConsoleLogEntry::Script, "Variable referenced before assignment: %s", name);
}

inline void ExprEvalState::setCurVarNameCreate(StringTableEntry name,StringTableEntry parentName)
{
	name = StringTable->insert(name);
	if( name == StringTable->insert( "" ) ) {
		Con::warnf( "ExprEvalState::setCurVarNameCreate() -- creating a variable with no name");
	}
	if(name[0] == '$')
		currentVariable = globalVars.add( name );
	else if(DictionaryStack.size())
		currentVariable = DictionaryStack.last()->add(name,parentName);
	else
	{
		currentVariable = NULL;
		Con::warnf(ConsoleLogEntry::Script, "Accessing local variable in global scope... failed: %s", name);
	}
}
#else

inline void ExprEvalState::setCurVarName(StringTableEntry name,StringTableEntry parentName)
{
	if(name[0] == '$')
		currentVariable = globalVars.lookup(name);
	else if(DictionaryStack.size())
		currentVariable = DictionaryStack.last()->lookup(name);
	if(!currentVariable && gWarnUndefinedScriptVariables)
		Con::warnf(ConsoleLogEntry::Script, "Variable referenced before assignment: %s", name);
}

inline void ExprEvalState::setCurVarNameCreate(StringTableEntry name,StringTableEntry parentName)
{
	name = StringTable->insert( name );
	if( name == StringTable->insert( "" ) ) {
		Con::warnf( "ExprEvalState::setCurVarNameCreate() -- creating a variable with no name");
	}
	if(name[0] == '$')
		currentVariable = globalVars.add(name);
	else if(DictionaryStack.size())
		currentVariable = DictionaryStack.last()->add(name);
	else
	{
		currentVariable = NULL;
		Con::warnf(ConsoleLogEntry::Script, "Accessing local variable in global scope... failed: %s", name);
	}
}

#endif //PUAP_SCRIPT_CHANGE


//------------------------------------------------------------

inline S32 ExprEvalState::getIntVariable()
{
	return currentVariable ? currentVariable->getIntValue() : 0;
}

inline F64 ExprEvalState::getFloatVariable()
{
	return currentVariable ? currentVariable->getFloatValue() : 0;
}

inline const char *ExprEvalState::getStringVariable()
{
	return currentVariable ? currentVariable->getStringValue() : "";
}

//------------------------------------------------------------

inline void ExprEvalState::setIntVariable(S32 val)
{
	AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
	currentVariable->setIntValue(val);
}

inline void ExprEvalState::setFloatVariable(F64 val)
{
	AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
	currentVariable->setFloatValue(val);
}

inline void ExprEvalState::setStringVariable(const char *val)
{
	AssertFatal(currentVariable != NULL, "Invalid evaluator state - trying to set null variable!");
	currentVariable->setStringValue(val);
}

//------------------------------------------------------------

void CodeBlock::getFunctionArgs(char buffer[1024], U32 ip)
{
	U32 fnArgc = code[ip + 5];
	buffer[0] = 0;
	for(U32 i = 0; i < fnArgc; i++)
	{
		StringTableEntry var = U32toSTE(code[ip + i + 6]);

		// Add a comma so it looks nice!
		if(i != 0)
			dStrcat(buffer, ", ");

		dStrcat(buffer, "var ");

		// Try to capture junked parameters
		if(var[0])
			dStrcat(buffer, var+1);
		else
			dStrcat(buffer, "JUNK");
	}
}

// Returns, in 'val', the specified component of a string.
static void getUnit(const char *string, U32 index, const char *set, char val[], S32 len)
{
	U32 sz;
	while(index--)
	{
		if(!*string)
			return;
		sz = dStrcspn(string, set);
		if (string[sz] == 0)
			return;
		string += (sz + 1);
	}
	sz = dStrcspn(string, set);
	if (sz == 0)
		return;

	if( ( sz + 1 ) > len )
		return;

	dStrncpy(val, string, sz);
	val[sz] = '\0';
}

// Copies a string, replacing the (space separated) specified component. The
// return value is stored in 'val'.
static void setUnit(const char *string, U32 index, const char *replace, const char *set, char val[], S32 len)
{
	U32 sz;
	const char *start = string;
	if( ( dStrlen(string) + dStrlen(replace) + 1 ) > len )
		return;

	U32 padCount = 0;

	while(index--)
	{
		sz = dStrcspn(string, set);
		if(string[sz] == 0)
		{
			string += sz;
			padCount = index + 1;
			break;
		}
		else
			string += (sz + 1);
	}
	// copy first chunk
	sz = string-start;
	dStrncpy(val, start, sz);
	for(U32 i = 0; i < padCount; i++)
		val[sz++] = set[0];

	// replace this unit
	val[sz] = '\0';
	dStrcat(val, replace);

	// copy remaining chunks
	sz = dStrcspn(string, set);         // skip chunk we're replacing
	if(!sz && !string[sz])
		return;

	string += sz;
	dStrcat(val, string);
	return;
}

// Gets a component of an object's field value or a variable and returns it
// in val.
static void getFieldComponent( SimObject* object, StringTableEntry field, const char* array, StringTableEntry subField, char val[], S32 count )
{
	const char* prevVal = NULL;
	// Grab value from object.
	if( object && field )
		prevVal = object->getDataField( field, array );
	// Otherwise, grab from the string stack. The value coming in will always
	// be a string because that is how multicomponent variables are handled.
	else
		prevVal = STR.getStringValue();

	// Make sure we got a value.
	if( prevVal && *prevVal )
	{
		// 'x', 'y', and 'z' grab the 1st, 2nd, or 3rd component of the
		// variable or field.
		if( subField == StringTable->insert( "x" ) )
			getUnit( prevVal, 0, " ", val, count );
		else if( subField == StringTable->insert( "y" ) )
			getUnit( prevVal, 1, " ", val, count );
		else if( subField == StringTable->insert( "z" ) )
			getUnit( prevVal, 2, " ", val, count );
	}
}

// Sets a component of an object's field value based on the sub field. 'x' will
// set the first field, 'y' the second, and 'z' the third.
static void setFieldComponent( SimObject* object, StringTableEntry field, const char* array, StringTableEntry subField )
{
	char val[1024] = "";
	const char* prevVal;
	// Set the value on an object field.
	if( object && field )
		prevVal = object->getDataField( field, array );

	// Set the value on a variable.
	else if( gEvalState.currentVariable )
		prevVal = gEvalState.getStringVariable();

	// Insert the value into the specified component of the string.
	bool set = false;
	if( subField == StringTable->insert( "x" ) )
	{
		setUnit( prevVal, 0, STR.getStringValue(), " ", val, 1024 );
		set = true;
	}
	else if( subField == StringTable->insert( "y" ) )
	{
		setUnit( prevVal, 1, STR.getStringValue(), " ", val, 1024 );
		set = true;
	}
	else if( subField == StringTable->insert( "z" ) )
	{
		setUnit( prevVal, 2, STR.getStringValue(), " ", val, 1024 );
		set = true;
	}

	if( set )
	{
		// Update the field or variable.
		if( object && field )
			object->setDataField( field, array, val );
		else if( gEvalState.currentVariable )
			gEvalState.setStringVariable( val );
	}
}

void CodeBlock::resetUsedEntries( Namespace *ns, StringTableEntry user ) { 
	Dictionary::Entry *current = NULL;
	while( !mUsedVars.empty() ) {
		current = mUsedVars[mUsedVars.size()-1];
		mUsedVars.pop_back();
		//-Mat make sure we are the first one in the callstack using this variable
		//local variables only
		if( current->name[0] == '%' && current->firstUser == user && current->mOwner->scopeNamespace == ns ) {
			current->setStringValue( "" );
			current->firstUser = NULL;
		}
	}
}
void CodeBlock::addUsedVariable( Dictionary::Entry *var, StringTableEntry user ) {
	if( var ) {
		mUsedVars.push_back( var );
		if( !var->firstUser ) {
			var->firstUser = user;
		}
	}
}

#ifdef PUAP_SCRIPT_CHANGE
//-Mat if this was called from an eval(), we need to have the functionName, and namespace of the
//Codeblock that called us
const char *CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, const char **argv, bool noCalls, StringTableEntry packageName, S32 setFrame)
{
#ifdef TORQUE_DEBUG
	U32 stackStart = STR.mStartStackSize;
#endif

	//for level loading among other things
	if( thisNamespace == NULL && gEvalState.DictionaryStack.size() > 0 ) {
		//"inhereit" the calling function's Namespace
		thisNamespace = getCurrentNamespace();
	}

	static char traceBuffer[1024];
	U32 i;

	incRefCount();
	F64 *curFloatTable;
	char *curStringTable;
	STR.clearFunctionOffset();
	StringTableEntry thisFunctionName = NULL;
	bool popFrame = false;

	if(argv)
	{
		// assume this points into a function decl:
		//-Mat the +3 is because the first 3 IPs are strings
		U32 fnArgc = code[ip + 5 + 3];
		thisFunctionName = U32toSTE(code[ip]);
		//-Mat advance past our pointer
		ip++;

		argc = getMin(argc-1, fnArgc); // argv[0] is func name
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Entering ");
			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s(", thisNamespace->mName, thisFunctionName);
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s(", thisFunctionName);
			}
			for(i = 0; i < argc; i++)
			{
				dStrcat(traceBuffer, argv[i+1]);
				if(i != argc - 1)
					dStrcat(traceBuffer, ", ");
			}
			dStrcat(traceBuffer, ")");
			Con::printf("%s", traceBuffer);
		}

		//-Mat this is the frame of variables for each function CALL
		gEvalState.pushFrame(thisFunctionName, thisNamespace, setFrame);
		popFrame = true;
		for(i = 0; i < argc; i++)
		{
			//-Mat use i * 2 to account for the pointer
			StringTableEntry var = U32toSTE(code[ip + i*2 + 8]);
			//-Mat if the arg is never uses, var will be "", skip it. Harmless, but 
			//you never know, this seems like it may cause errors down the road, so heads up
			if( dStrlen( var ) < 1 ) {
				continue;
			}
			gEvalState.setCurParamName(var,thisFunctionName);//-Mat pass in function name
			gEvalState.setStringVariable( argv[ (i + 1) ]  );
		}
		//now get the names of all the arguments that weren't passed, and make sure those variables are ""
		for(i = argc; i < fnArgc; i++)
		{
			//-Mat use i * 2 to account for the pointer
			StringTableEntry var = U32toSTE(code[ip + i*2 + 8]);
			//-Mat if the arg is never uses, var will be "", skip it. Harmless, but 
			//you never know, this seems like it may cause errors down the road, so heads up
			if( dStrlen( var ) < 1 ) {
				continue;
			}
			gEvalState.setCurParamName(var,thisFunctionName);//-Mat pass in function name
			gEvalState.setStringVariable( ""  );
		}

		//count 2 pointer for each argument (whether we get it passed to us or not)
		//fnArgc is the number of args in the function declaration
		//argc is the number actually passed to us
		ip = ip + (fnArgc * 2) + 6;
		//-Mat + 2 past our 2 strings (3, but we already incremented for thisFunctionName)
		ip += 2;
		curFloatTable = functionFloats;
		curStringTable = functionStrings;
	}
	else
	{
		curFloatTable = globalFloats;
		curStringTable = globalStrings;

		// Do we want this code to execute using a new DictionaryStack frame?
		if (setFrame < 0)
		{
			gEvalState.pushFrame(NULL, NULL, setFrame);
			popFrame = true;
		}
		else if (!gEvalState.DictionaryStack.empty())
		{
			// We want to copy a reference to an existing DictionaryStack frame
			// on to the top of the DictionaryStack.  Any change that occurs to 
			// the locals during this new frame will also occur in the 
			// original frame.
			//S32 stackIndex = gEvalState.DictionaryStack.size() - setFrame - 1;
			//gEvalState.pushFrameRef( stackIndex );
			popFrame = false;
#ifdef PUAP_SCRIPT_CHANGE
			//-Mat to prevent having no namespace when we are exec()ed from inside another function
			if( !thisFunctionName && !functionName ) {
				thisFunctionName = getCurrentFunctionName();
			}
#endif// PUAP_SCRIPT_CHANGE
		}
	}


	//-Mat clear out used variables if we've called this function before
	//-Mat this works, if only we could make it per function
	resetUsedEntries( thisNamespace, functionName );



	// Grab the state of the telenet debugger here once
	// so that the push and pop frames are always balanced.
	const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
	if ( telDebuggerOn /*&& setFrame < 0*/ )//-Mat use debugger regardless of recursion frame
		TelDebugger->pushStackFrame();

	StringTableEntry var, objParent;
	U32 failJump;
	StringTableEntry fnName;
	StringTableEntry fnNamespace, fnPackage;
	SimObject *currentNewObject = 0;
	StringTableEntry prevField = NULL;
	StringTableEntry curField = NULL;
	SimObject *prevObject = NULL;
	SimObject *curObject = NULL;
	SimObject *saveObject=NULL;
	Namespace::Entry *nsEntry;
	Namespace *ns;

	Dictionary::Entry *pVarEntry = NULL;

	const char* curFNDocBlock = NULL;
	const char* curNSDocBlock = NULL;
	const S32 nsDocLength = 128;
	char nsDocBlockClass[nsDocLength];

	U32 callArgc;
	const char **callArgv;

	static char curFieldArray[256];
	static char prevFieldArray[256];

	CodeBlock *saveCodeBlock = smCurrentCodeBlock;
	smCurrentCodeBlock = this;
	if(this->name)
	{
		Con::gCurrentFile = this->name;
		Con::gCurrentRoot = mRoot;
	}
	const char * val;

	// The frame temp is used by the variable accessor ops (OP_SAVEFIELD_* and
	// OP_LOADFIELD_*) to store temporary values for the fields.
	static S32 VAL_BUFFER_SIZE = 1024;
	FrameTemp<char> valBuffer( VAL_BUFFER_SIZE );

	for(;;)
	{
		U32 instruction = code[ip++];
breakContinue:
		switch(instruction)
		{
		case OP_FUNC_DECL:
			{
				U32 startIP = ip;
				if(!noCalls)
				{
					fnName       = U32toSTE(code[ip]);
					ip++;//-Mat to account for our new pointer
					fnNamespace  = U32toSTE(code[ip+1]);
					ip++;//-Mat to account for our new pointer
					fnPackage    = U32toSTE(code[ip+2]);
					ip++;//-Mat to account for our new pointer
					bool hasBody = bool(code[ip+3]);

					Namespace::unlinkPackages();
					ns = Namespace::find(fnNamespace, fnPackage);
					ns->addFunction(fnName, this, hasBody ? startIP : 0, curFNDocBlock ? dStrdup( curFNDocBlock ) : NULL );// if no body, set the IP to 0
					if( curNSDocBlock )
					{
						if( fnNamespace == StringTable->lookup( nsDocBlockClass ) )
						{
							char *usageStr = dStrdup( curNSDocBlock );
							usageStr[dStrlen(usageStr)] = '\0';
							ns->mUsage = usageStr;
							ns->mCleanUpUsage = true;
							curNSDocBlock = NULL;
						}
					}
					Namespace::relinkPackages();

					// If we had a docblock, it's definitely not valid anymore, so clear it out.
					curFNDocBlock = NULL;

					//Con::printf("Adding function %s::%s (%d)", fnNamespace, fnName, ip);
				}
				//-Mat + 4 because we moved 3 ip after we got the name(fnNamespace, fnPackage, hasbody)
				// + 3 for the pointer in after each string (fnName, fnNamespace, fnPackage)
				ip = code[startIP + 4 + 3];
			}
			break;

		case OP_CREATE_OBJECT:
			{
				// Read some useful info.
				objParent        = U32toSTE(code[ip    ]);

				//-Mat increment past our variable
				ip++;

				bool isDataBlock =          code[ip + 1];
				bool isInternal  =          code[ip + 2];
				bool isMessage   =          code[ip + 3];
				failJump         =          code[ip + 4];

				// If we don't allow calls, we certainly don't allow creating objects!
				// Moved this to after failJump is set. Engine was crashing when
				// noCalls = true and an object was being created at the beginning of
				// a file. ADL.
				if(noCalls)
				{
					ip = failJump;
					break;
				}

				// Get the constructor information off the stack.
				STR.getArgcArgv(NULL, &callArgc, &callArgv, true);

				// Con::printf("Creating object...");

				// objectName = argv[1]...
				currentNewObject = NULL;

				// Are we creating a datablock? If so, deal with case where we override
				// an old one.
				if(isDataBlock)
				{
					// Con::printf("  - is a datablock");

					// Find the old one if any.
					SimObject *db = Sim::getDataBlockGroup()->findObject(callArgv[2]);

					// Make sure we're not changing types on ourselves...
					if(db && dStricmp(db->getClassName(), callArgv[1]))
					{
						Con::errorf(ConsoleLogEntry::General, "Cannot re-declare data block %s with a different class.", callArgv[2]);
						ip = failJump;
						break;
					}

					// If there was one, set the currentNewObject and move on.
					if(db)
						currentNewObject = db;
				}

				if(!currentNewObject)
				{
					// Well, looks like we have to create a new object.
					ConsoleObject *object = ConsoleObject::create(callArgv[1]);

					// Deal with failure!
					if(!object)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-conobject class %s.", getFileLine(ip-1), callArgv[1]);
						ip = failJump;
						break;
					}

					// Do special datablock init if appropros
					if(isDataBlock)
					{
						SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(object);
						if(dataBlock)
						{
							dataBlock->assignId();
						}
						else
						{
							// They tried to make a non-datablock with a datablock keyword!
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-datablock class %s.", getFileLine(ip-1), callArgv[1]);

							// Clean up...
							delete object;
							ip = failJump;
							break;
						}
					}

					// Finally, set currentNewObject to point to the new one.
					currentNewObject = dynamic_cast<SimObject *>(object);

					// Deal with the case of a non-SimObject.
					if(!currentNewObject)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-SimObject class %s.", getFileLine(ip-1), callArgv[1]);
						delete object;
						ip = failJump;
						break;
					}

					// Does it have a parent object? (ie, the copy constructor : syntax, not inheriance)
					// [tom, 9/8/2006] it is inheritance if it's a message ... muwahahah!
					//-Mat if objParent is NULL, evaluating *objParent will crash
					if(!isMessage && objParent != NULL && *objParent)
					{
						// Find it!
						SimObject *parent;
						if(Sim::findObject(objParent, parent))
						{
							// Con::printf(" - Parent object found: %s", parent->getClassName());

							// and suck the juices from it!
							currentNewObject->assignFieldsFrom(parent);
						}
						else
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to find parent object %s for %s.", getFileLine(ip-1), objParent, callArgv[1]);

						// Mm! Juices!
					}

					// If a name was passed, assign it.
					if(callArgv[2][0])
					{
						if(! isMessage)
						{
							if(! isInternal)
								currentNewObject->assignName(callArgv[2]);
							else
								currentNewObject->setInternalName(callArgv[2]);
						}
						else
						{
							Message *msg = dynamic_cast<Message *>(currentNewObject);
							if(msg)
							{
								msg->setClassNamespace(callArgv[2]);
								msg->setSuperClassNamespace(objParent);
							}
							else
							{
								Con::errorf(ConsoleLogEntry::General, "%s: Attempting to use newmsg on non-message type %s", getFileLine(ip-1), callArgv[1]);
								delete currentNewObject;
								currentNewObject = NULL;
								ip = failJump;
								break;
							}
						}
					}

					// Do the constructor parameters.
					if(!currentNewObject->processArguments(callArgc-3, callArgv+3))
					{
						delete currentNewObject;
						currentNewObject = NULL;
						ip = failJump;
						break;
					}

					// If it's not a datablock, allow people to modify bits of it.
					if(!isDataBlock)
					{
						currentNewObject->setModStaticFields(true);
						currentNewObject->setModDynamicFields(true);
					}
				}

				// Advance the IP past the create info...
				ip += 5;
				break;
			}

		case OP_ADD_OBJECT:
			{
				// See OP_SETCURVAR for why we do this.
				curFNDocBlock = NULL;
				curNSDocBlock = NULL;

				// Do we place this object at the root?
				bool placeAtRoot = code[ip++];

				// Con::printf("Adding object %s", currentNewObject->getName());

				// Make sure it wasn't already added, then add it.
				if(currentNewObject->isProperlyAdded() == false)
				{
					bool ret = false;

					Message *msg = dynamic_cast<Message *>(currentNewObject);
					if(msg)
					{
						SimObjectId id = Message::getNextMessageID();
						if(id != 0xffffffff)
							ret = currentNewObject->registerObject(id);
						else
							Con::errorf("%s: No more object IDs available for messages", getFileLine(ip-2));
					}
					else
						ret = currentNewObject->registerObject();

					if(! ret)
					{
						// This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
						Con::warnf(ConsoleLogEntry::General, "%s: Register object failed for object %s of class %s.", getFileLine(ip-2), currentNewObject->getName(), currentNewObject->getClassName());
						delete currentNewObject;
						ip = failJump;
						break;
					}
				}

				// Are we dealing with a datablock?
				SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(currentNewObject);
				static char errorBuffer[256];

				// If so, preload it.
				if(dataBlock && !dataBlock->preload(true, errorBuffer))
				{
					Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", getFileLine(ip-2),
						currentNewObject->getName(), errorBuffer);
					dataBlock->deleteObject();
					ip = failJump;
					break;
				}

				// What group will we be added to, if any?
				U32 groupAddId = intStack[UINT];
				SimGroup *grp = NULL;
				SimSet   *set = NULL;
				SimComponent *comp = NULL;
				bool isMessage = dynamic_cast<Message *>(currentNewObject) != NULL;

				if(!placeAtRoot || !currentNewObject->getGroup())
				{
					if(! isMessage)
					{
						if(! placeAtRoot)
						{
							// Otherwise just add to the requested group or set.
							if(!Sim::findObject(groupAddId, grp))
								if(!Sim::findObject(groupAddId, comp))
									Sim::findObject(groupAddId, set);
						}

						if(placeAtRoot || comp != NULL)
						{
							// Deal with the instantGroup if we're being put at the root or we're adding to a component.
							const char *addGroupName = Con::getVariable("instantGroup");
							if(!Sim::findObject(addGroupName, grp))
								Sim::findObject(RootGroupId, grp);
						}

						if(comp)
						{
							SimComponent *newComp = dynamic_cast<SimComponent *>(currentNewObject);
							if(newComp)
							{
								if(! comp->addComponent(newComp))
									Con::errorf("%s: Unable to add component %s, template not loaded?", getFileLine(ip-2), currentNewObject->getName() ? currentNewObject->getName() : currentNewObject->getIdString());
							}
						}
					}

					// If we didn't get a group, then make sure we have a pointer to
					// the rootgroup.
					if(!grp)
						Sim::findObject(RootGroupId, grp);

					// add to the parent group
					grp->addObject(currentNewObject);

					// add to any set we might be in
					if(set)
						set->addObject(currentNewObject);
				}

				// store the new object's ID on the stack (overwriting the group/set
				// id, if one was given, otherwise getting pushed)
				if(placeAtRoot) 
					intStack[UINT] = currentNewObject->getId();
				else
					intStack[++UINT] = currentNewObject->getId();

				break;
			}

		case OP_END_OBJECT:
			{
				// If we're not to be placed at the root, make sure we clean up
				// our group reference.
				bool placeAtRoot = code[ip++];
				if(!placeAtRoot)
					UINT--;
				break;
			}

		case OP_JMPIFFNOT:
			if(floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT:
			if(intStack[UINT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFF:
			if(!floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF:
			if(!intStack[UINT--])
			{
				ip ++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT_NP:
			if(intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF_NP:
			if(!intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMP:
			ip = code[ip];
			break;
		case OP_RETURN:
			goto execFinished;
		case OP_CMPEQ:
			intStack[UINT+1] = bool(floatStack[FLT] == floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGR:
			intStack[UINT+1] = bool(floatStack[FLT] > floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGE:
			intStack[UINT+1] = bool(floatStack[FLT] >= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLT:
			intStack[UINT+1] = bool(floatStack[FLT] < floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLE:
			intStack[UINT+1] = bool(floatStack[FLT] <= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPNE:
			intStack[UINT+1] = bool(floatStack[FLT] != floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_XOR:
			intStack[UINT-1] = intStack[UINT] ^ intStack[UINT-1];
			UINT--;
			break;

		case OP_MOD:
			if(  intStack[UINT-1] != 0 )
				intStack[UINT-1] = intStack[UINT] % intStack[UINT-1];
			else
				intStack[UINT-1] = 0;
			UINT--;
			break;

		case OP_BITAND:
			intStack[UINT-1] = intStack[UINT] & intStack[UINT-1];
			UINT--;
			break;

		case OP_BITOR:
			intStack[UINT-1] = intStack[UINT] | intStack[UINT-1];
			UINT--;
			break;

		case OP_NOT:
			intStack[UINT] = !intStack[UINT];
			break;

		case OP_NOTF:
			intStack[UINT+1] = !floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_ONESCOMPLEMENT:
			intStack[UINT] = ~intStack[UINT];
			break;

		case OP_SHR:
			intStack[UINT-1] = intStack[UINT] >> intStack[UINT-1];
			UINT--;
			break;

		case OP_SHL:
			intStack[UINT-1] = intStack[UINT] << intStack[UINT-1];
			UINT--;
			break;

		case OP_AND:
			intStack[UINT-1] = intStack[UINT] && intStack[UINT-1];
			UINT--;
			break;

		case OP_OR:
			intStack[UINT-1] = intStack[UINT] || intStack[UINT-1];
			UINT--;
			break;

		case OP_ADD:
			floatStack[FLT-1] = floatStack[FLT] + floatStack[FLT-1];
			FLT--;
			break;

		case OP_SUB:
			floatStack[FLT-1] = floatStack[FLT] - floatStack[FLT-1];
			FLT--;
			break;

		case OP_MUL:
			floatStack[FLT-1] = floatStack[FLT] * floatStack[FLT-1];
			FLT--;
			break;
		case OP_DIV:
			floatStack[FLT-1] = floatStack[FLT] / floatStack[FLT-1];
			FLT--;
			break;
		case OP_NEG:
			floatStack[FLT] = -floatStack[FLT];
			break;

		case OP_SETCURVAR:
			var = U32toSTE(code[ip]);
			//-Mat to account for our empty pointer before the string
			ip++;

			// If a variable is set, then these must be NULL. It is necessary
			// to set this here so that the vector parser can appropriately
			// identify whether it's dealing with a vector.
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			//if we are recursing do things the slow lookup way, otherwise use our quick pointer 
			if( setFrame > 1 ) {
				gEvalState.setCurVarNameCreate(var, thisFunctionName);
			} else {
				pVarEntry = ((Dictionary::Entry*)code[ip]);

				//-Mat make sure the pVarEntry is not an old one from a previous, unrelated call of this function
				if( !pVarEntry || pVarEntry->mOwner->scopeNamespace != thisNamespace ) {
					gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function
					//-Mat if we can't find it, it is probably a global that is being checked(but not created) right here
					//in which case we can just make a new, blank one, because it should be == "" anyway
					if( gEvalState.currentVariable == NULL || gEvalState.currentVariable->mOwner->scopeNamespace != thisNamespace ) {

						gEvalState.setCurVarNameCreate(var, thisFunctionName);
					}
					code[ip] =  (U32)gEvalState.currentVariable;
				} else {
					gEvalState.currentVariable = pVarEntry;
				}
			}

			// In order to let docblocks work properly with variables, we have
			// clear the current docblock when we do an assign. This way it 
			// won't inappropriately carry forward to following function decls.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			ip++;
			break;

		case OP_SETCURVAR_CREATE:
			var = U32toSTE(code[ip]);
			//-Mat to account for our empty pointer before the string
			ip++;
			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			//if we are recursing do things the slow lookup way, otherwise use our quick pointer 
			if( setFrame > 1 ) {
				gEvalState.setCurVarNameCreate(var, thisFunctionName);
			} else {
				pVarEntry = ((Dictionary::Entry*)code[ip]);
				//-Mat make sure the pVarEntry is not an old one from a previous, unrelated call of this function
				if( !pVarEntry || pVarEntry->mOwner->scopeNamespace != thisNamespace ) {
					gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function
					if( gEvalState.currentVariable == NULL || 
						gEvalState.currentVariable->mOwner->scopeNamespace != thisNamespace ) {

							gEvalState.setCurVarNameCreate(var, thisFunctionName);//-Mat pass in the name of this function
					}
					code[ip] =  (U32)gEvalState.currentVariable;
				}
				else {
					gEvalState.currentVariable = pVarEntry;
				}
			}

			//-Mat this variable is now used in this function
			addUsedVariable( gEvalState.currentVariable, thisFunctionName );

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			ip++;
			break;

		case OP_SETCURVAR_ARRAY:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarName(var, thisFunctionName);//-Mat pass in the name of this function

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_ARRAY_CREATE:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarNameCreate(var,thisFunctionName);//-Mat pass in function name

			//-Mat this variable is now used in this function
			addUsedVariable( gEvalState.currentVariable,thisFunctionName );

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_LOADVAR_UINT:
			intStack[UINT+1] = gEvalState.getIntVariable();
			UINT++;
			break;

		case OP_LOADVAR_FLT:
			floatStack[FLT+1] = gEvalState.getFloatVariable();
			FLT++;
			break;

		case OP_LOADVAR_STR:
			val = gEvalState.getStringVariable();
			STR.setStringValue(val);
			break;

		case OP_SAVEVAR_UINT:
			gEvalState.setIntVariable(intStack[UINT]);
			break;

		case OP_SAVEVAR_FLT:
			gEvalState.setFloatVariable(floatStack[FLT]);
			break;

		case OP_SAVEVAR_STR:
			gEvalState.setStringVariable(STR.getStringValue());
			break;

		case OP_SETCUROBJECT:
			// Save the previous object for parsing vector fields.
			prevObject = curObject;
			val = STR.getStringValue();

			// Sim::findObject will sometimes find valid objects from
			// multi-component strings. This makes sure that doesn't
			// happen.
			for( const char* check = val; *check; check++ )
			{
				if( *check == ' ' )
				{
					val = "";
					break;
				}
			}
			curObject = Sim::findObject(val);
			break;

		case OP_SETCUROBJECT_INTERNAL:
			++ip; // To skip the recurse flag if the object wasnt found
			if(curObject)
			{
				SimGroup *group = dynamic_cast<SimGroup *>(curObject);
				if(group)
				{
					StringTableEntry intName = StringTable->insert(STR.getStringValue());
					bool recurse = code[ip-1];
					SimObject *obj = group->findObjectByInternalName(intName, recurse);
					intStack[UINT+1] = obj ? obj->getId() : 0;
					UINT++;
				}
				else
				{
					Con::errorf(ConsoleLogEntry::Script, "%s: Attempt to use -> on non-group %s of class %s.", getFileLine(ip-2), curObject->getName(), curObject->getClassName());
					intStack[UINT] = 0;
				}
			}
			break;

		case OP_SETCUROBJECT_NEW:
			curObject = currentNewObject;
			break;

		case OP_SETCURFIELD:
			// Save the previous field for parsing vector fields.
			prevField = curField;
			dStrcpy( prevFieldArray, curFieldArray );
			curField = U32toSTE(code[ip]);
			//-Mat increment past our pointer
			ip++;

			curFieldArray[0] = 0;
			ip++;
			break;

		case OP_SETCURFIELD_ARRAY:
			dStrcpy(curFieldArray, STR.getStringValue());
			break;

		case OP_LOADFIELD_UINT:
			if(curObject)
				intStack[UINT+1] = U32(dAtoi(curObject->getDataField(curField, curFieldArray)));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				intStack[UINT+1] = dAtoi( valBuffer );
			}
			UINT++;
			break;

		case OP_LOADFIELD_FLT:
			if(curObject)
				floatStack[FLT+1] = dAtof(curObject->getDataField(curField, curFieldArray));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				floatStack[FLT+1] = dAtof( valBuffer );
			}
			FLT++;
			break;

		case OP_LOADFIELD_STR:
			if(curObject)
			{
				val = curObject->getDataField(curField, curFieldArray);
				STR.setStringValue( val );
			}
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				STR.setStringValue( valBuffer );
			}

			break;

		case OP_SAVEFIELD_UINT:
			STR.setIntValue(intStack[UINT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_FLT:
			STR.setFloatValue(floatStack[FLT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_STR:
			if(curObject) {
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			}
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_STR_TO_UINT:
			intStack[UINT+1] = STR.getIntValue();
			UINT++;
			break;

		case OP_STR_TO_FLT:
			floatStack[FLT+1] = STR.getFloatValue();
			FLT++;
			break;

		case OP_STR_TO_NONE:
			// This exists simply to deal with certain typecast situations.
			break;

		case OP_FLT_TO_UINT:
			intStack[UINT+1] = (S64)floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_FLT_TO_STR:
			STR.setFloatValue(floatStack[FLT]);
			FLT--;
			break;

		case OP_FLT_TO_NONE:
			FLT--;
			break;

		case OP_UINT_TO_FLT:
			floatStack[FLT+1] = intStack[UINT];
			UINT--;
			FLT++;
			break;

		case OP_UINT_TO_STR:
			STR.setIntValue(intStack[UINT]);
			UINT--;
			break;

		case OP_UINT_TO_NONE:
			UINT--;
			break;

		case OP_LOADIMMED_UINT:
			intStack[UINT+1] = code[ip++];
			UINT++;
			break;

		case OP_LOADIMMED_FLT:
			floatStack[FLT+1] = curFloatTable[code[ip]];
			ip++;
			FLT++;
			break;
		case OP_TAG_TO_STR:
			code[ip-1] = OP_LOADIMMED_STR;
			// it's possible the string has already been converted
			if(U8(curStringTable[code[ip]]) != StringTagPrefixByte)
			{
				U32 id = GameAddTaggedString(curStringTable + code[ip]);
				dSprintf(curStringTable + code[ip] + 1, 7, "%d", id);
				*(curStringTable + code[ip]) = StringTagPrefixByte;
			}
		case OP_LOADIMMED_STR:
			STR.setStringValue(curStringTable + code[ip++]);
			break;

		case OP_DOCBLOCK_STR:
			{
				// If the first word of the doc is '\class' or '@class', then this
				// is a namespace doc block, otherwise it is a function doc block.
				const char* docblock = curStringTable + code[ip++];

				const char* sansClass = dStrstr( docblock, "@class" );
				if( !sansClass )
					sansClass = dStrstr( docblock, "\\class" );

				if( sansClass )
				{
					// Don't save the class declaration. Scan past the 'class'
					// keyword and up to the first whitespace.
					sansClass += 7;
					S32 index = 0;
					while( ( *sansClass != ' ' ) && ( *sansClass != '\n' ) && *sansClass && ( index < ( nsDocLength - 1 ) ) )
					{
						nsDocBlockClass[index++] = *sansClass;
						sansClass++;
					}
					nsDocBlockClass[index] = '\0';

					curNSDocBlock = sansClass + 1;
				}
				else
					curFNDocBlock = docblock;
			}

			break;

		case OP_LOADIMMED_IDENT:
			STR.setStringValue(U32toSTE(code[ip++]));

			//-Mat increment past our pointer
			ip++;

			break;

		case OP_CALLFUNC_RESOLVE:
			{
				U32 startIP = ip;
				// This deals with a function that is potentially living in a namespace.
				fnName      = U32toSTE(code[ip]);
				ip++;//-Mat to account for our new pointer
				fnNamespace = U32toSTE(code[ip+1]);
				ip++;//-Mat to account for our new pointer

				// Try to look it up.
				ns = Namespace::find(fnNamespace);
				nsEntry = ns->lookup(fnName);
				if(!nsEntry)
				{
					ip+= 3;
					Con::warnf(ConsoleLogEntry::General,
						"%s: Unable to find function %s%s%s",
						getFileLine(ip-4), fnNamespace ? fnNamespace : "",
						fnNamespace ? "::" : "", fnName);
					STR.popFrame();
					break;
				}
				// Now, rewrite our code a bit (ie, avoid future lookups) and fall
				// through to OP_CALLFUNC
				code[ip+1] = *((U32 *) &nsEntry);
				code[ip-1] = OP_CALLFUNC;

				//because we change the ip, let's put it back to the start for OP_CALLFUNC
				ip = startIP;
			}

		case OP_CALLFUNC:
			{
				// This routingId is set when we query the object as to whether
				// it handles this method.  It is set to an enum from the table
				// above indicating whether it handles it on a component it owns
				// or just on the object.
				S32 routingId = 0;

				fnName = U32toSTE(code[ip]);

				ip++;//-Mat to account for our new pointer

				//if this is called from inside a function, append the ip and codeptr
				//-Mat this is used for breakpoints
				if (!gEvalState.DictionaryStack.empty())
				{
					gEvalState.DictionaryStack.last()->code = this;
					//-Mat may need to fix this -1
					gEvalState.DictionaryStack.last()->ip = ip - 1;
				}
				//-Mat advance an extra one for our pointer ( instead of + 2 and += 3)
				U32 callType = code[ip+3];
				ip += 4;

				STR.getArgcArgv(fnName, &callArgc, &callArgv);

				if(callType == FuncCallExprNode::FunctionCall) 
				{
					nsEntry = *((Namespace::Entry **) &code[ip-2]);
					ns = NULL;
				}
				else if(callType == FuncCallExprNode::MethodCall)
				{
					saveObject = gEvalState.thisObject;
					gEvalState.thisObject = Sim::findObject(callArgv[1]);
					if(!gEvalState.thisObject)
					{
						gEvalState.thisObject = 0;
						Con::warnf(ConsoleLogEntry::General,"%s: Unable to find object: '%s' attempting to call function '%s'", getFileLine(ip-4), callArgv[1], fnName);

						STR.popFrame(); // [neo, 5/7/2007 - #2974]

						break;
					}

					bool handlesMethod = gEvalState.thisObject->handlesConsoleMethod(fnName,&routingId);
					if( handlesMethod && routingId == MethodOnComponent )
					{
						DynamicConsoleMethodComponent *pComponent = dynamic_cast<DynamicConsoleMethodComponent*>( gEvalState.thisObject );
						if( pComponent )
							pComponent->callMethodArgList( callArgc, callArgv, false );
					}

					ns = gEvalState.thisObject->getNamespace();
					if(ns)
						nsEntry = ns->lookup(fnName);
					else
						nsEntry = NULL;
				}
				else // it's a ParentCall
				{
					if(thisNamespace) {
						ns = thisNamespace->mParent;
						if(ns)
							nsEntry = ns->lookup(fnName);
						else
							nsEntry = NULL;
					} else  {
						ns = NULL;
						nsEntry = NULL;
					}
				}

				S32 nsType = -1;
				S32 nsMinArgs = 0;
				S32 nsMaxArgs = 0;
				Namespace::Entry::CallbackUnion * nsCb = NULL;
				// UNUSED: JOSEPH THOMAS -> Namespace::Entry::CallbackUnion cbu;
				const char * nsUsage = NULL;
				if (nsEntry)
				{
					nsType = nsEntry->mType;
					nsMinArgs = nsEntry->mMinArgs;
					nsMaxArgs = nsEntry->mMaxArgs;
					nsCb = &nsEntry->cb;
					nsUsage = nsEntry->mUsage;
					routingId = 0;
				}
				if(!nsEntry || noCalls)
				{
					if(!noCalls && !( routingId == MethodOnComponent ) )
					{
						Con::warnf(ConsoleLogEntry::General,"%s: Unknown command %s.", getFileLine(ip-4), fnName);
						if(callType == FuncCallExprNode::MethodCall)
						{
							Con::warnf(ConsoleLogEntry::General, "  Object %s(%d) %s",
								gEvalState.thisObject->getName() ? gEvalState.thisObject->getName() : "",
								gEvalState.thisObject->getId(), getNamespaceList(ns) );
						}
					}
					STR.popFrame();
					STR.setStringValue("");
					break;
				}
				if(nsEntry->mType == Namespace::Entry::ScriptFunctionType)
				{
					const char *ret = "";
					if(nsEntry->mFunctionOffset) {
						gEvalState.NamespaceEntryStack.push_back( nsEntry );
						nsEntry->recursionCount++;
						ret = nsEntry->mCode->exec(nsEntry->mFunctionOffset, fnName, nsEntry->mNamespace, callArgc, callArgv, false, nsEntry->mPackage, nsEntry->recursionCount);
						nsEntry->recursionCount--;
						gEvalState.NamespaceEntryStack.pop_back();
					}
					STR.popFrame();
					STR.setStringValue(ret);
				}
				else
				{
					const char* nsName = ns? ns->mName: "";
#ifndef TORQUE_DEBUG
					// [tom, 12/13/2006] This stops tools functions from working in the console,
					// which is useful behavior when debugging so I'm ifdefing this out for debug builds.
					if(nsEntry->mToolOnly && ! Con::isCurrentScriptToolScript())
					{
						Con::errorf(ConsoleLogEntry::Script, "%s: %s::%s - attempting to call tools only function from outside of tools.", getFileLine(ip-4), nsName, fnName);
					}
					else
#endif
						if((nsEntry->mMinArgs && S32(callArgc) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(callArgc) > nsEntry->mMaxArgs))
						{
							Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", getFileLine(ip-4), nsName, fnName);
							Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", getFileLine(ip-4), nsEntry->mUsage);
							STR.popFrame();
						}
						else
						{
							switch(nsEntry->mType)
							{
							case Namespace::Entry::StringCallbackType:
								{
									const char *ret = nsEntry->cb.mStringCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(ret != STR.getStringValue())
										STR.setStringValue(ret);
									else
										STR.setLen(dStrlen(ret));
									break;
								}
							case Namespace::Entry::IntCallbackType:
								{
									S32 result = nsEntry->cb.mIntCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							case Namespace::Entry::FloatCallbackType:
								{
									F64 result = nsEntry->cb.mFloatCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = (S64)result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setFloatValue(result);
									break;
								}
							case Namespace::Entry::VoidCallbackType:
								nsEntry->cb.mVoidCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
								if(code[ip] != OP_STR_TO_NONE)
									Con::warnf(ConsoleLogEntry::General, "%s: Call to %s in %s uses result of void function call.", getFileLine(ip-4), fnName, functionName);

								STR.popFrame();
								STR.setStringValue("");
								break;
							case Namespace::Entry::BoolCallbackType:
								{
									bool result = nsEntry->cb.mBoolCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							}
						}
				}

				if(callType == FuncCallExprNode::MethodCall)
					gEvalState.thisObject = saveObject;
				break;
			}
		case OP_ADVANCE_STR:
			STR.advance();
			break;
		case OP_ADVANCE_STR_APPENDCHAR:
			STR.advanceChar(code[ip++]);
			break;

		case OP_ADVANCE_STR_COMMA:
			STR.advanceChar('_');
			break;

		case OP_ADVANCE_STR_NUL:
			STR.advanceChar(0);
			break;

		case OP_REWIND_STR:
			STR.rewind();
			break;

		case OP_TERMINATE_REWIND_STR:
			STR.rewindTerminate();
			break;

		case OP_COMPARE_STR:
			intStack[++UINT] = STR.compare();
			break;
		case OP_PUSH:
			STR.push();
			break;

		case OP_PUSH_FRAME:
			STR.pushFrame();
			break;
		case OP_BREAK:
			{
				//append the ip and codeptr before managing the breakpoint!
				AssertFatal( !gEvalState.DictionaryStack.empty(), "Empty eval DictionaryStack on break!");
				gEvalState.DictionaryStack.last()->code = this;
				gEvalState.DictionaryStack.last()->ip = ip - 1;
				U32 breakLine;
				findBreakLine(ip-1, breakLine, instruction);
				if(!breakLine)
					goto breakContinue;
				TelDebugger->executionStopped(this, breakLine);
				goto breakContinue;
			}
		case OP_INVALID:

		default:
			// error!
			goto execFinished;
		}
	}
execFinished:

	if ( telDebuggerOn /*&& setFrame < 0*/ )//-Mat use debugger regardless of recursion frame
		TelDebugger->popStackFrame();

	if ( popFrame )
		gEvalState.popFrame();

	if(argv)
	{
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Leaving ");

			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s() - return %s", thisNamespace->mName, thisFunctionName, STR.getStringValue());
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s() - return %s", thisFunctionName, STR.getStringValue());
			}
			Con::printf("%s", traceBuffer);
		}
	}
	else
	{
		delete[] const_cast<char*>(globalStrings);
		delete[] globalFloats;
		globalStrings = NULL;
		globalFloats = NULL;
	}
	smCurrentCodeBlock = saveCodeBlock;
	if(saveCodeBlock && saveCodeBlock->name)
	{
		Con::gCurrentFile = saveCodeBlock->name;
		Con::gCurrentRoot = saveCodeBlock->mRoot;
	}

	decRefCount();

	/* cleanup */


#ifdef TORQUE_DEBUG
	AssertFatal(!(STR.mStartStackSize > stackStart), "String stack not popped enough in script exec");
	AssertFatal(!(STR.mStartStackSize < stackStart), "String stack popped too much in script exec");
#endif
	return STR.getStringValue();
}

#else 

const char *CodeBlock::exec(U32 ip, const char *functionName, Namespace *thisNamespace, U32 argc, const char **argv, bool noCalls, StringTableEntry packageName, S32 setFrame)
{
#ifdef TORQUE_DEBUG
	U32 stackStart = STR.mStartStackSize;
#endif


	static char traceBuffer[1024];
	U32 i;

	incRefCount();
	F64 *curFloatTable;
	char *curStringTable;
	STR.clearFunctionOffset();
	StringTableEntry thisFunctionName = NULL;
	bool popFrame = false;
	if(argv)
	{
		// assume this points into a function decl:
		U32 fnArgc = code[ip + 5];
		thisFunctionName = U32toSTE(code[ip]);
		argc = getMin(argc-1, fnArgc); // argv[0] is func name
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Entering ");
			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s(", thisNamespace->mName, thisFunctionName);
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s(", thisFunctionName);
			}
			for(i = 0; i < argc; i++)
			{
				dStrcat(traceBuffer, argv[i+1]);
				if(i != argc - 1)
					dStrcat(traceBuffer, ", ");
			}
			dStrcat(traceBuffer, ")");
			Con::printf("%s", traceBuffer);
		}
		gEvalState.pushFrame(thisFunctionName, thisNamespace);
		popFrame = true;
		for(i = 0; i < argc; i++)
		{
			StringTableEntry var = U32toSTE(code[ip + i + 6]);
			//-Mat DANGEROUS! if the arg is never used, var will be "", skip it?
			//-Mat not sure why this is happenening, seems to be compiled in this way but
			//it doesn't seem to be breaking anything, a bison optimization maybe?
			if( dStrlen( var ) < 1 ) {
				continue;
			}

			gEvalState.setCurVarNameCreate(var);
			gEvalState.setStringVariable(argv[i+1]);
		}
		ip = ip + fnArgc + 6;
		curFloatTable = functionFloats;
		curStringTable = functionStrings;
	}
	else
	{
		curFloatTable = globalFloats;
		curStringTable = globalStrings;

		// Do we want this code to execute using a new stack frame?
		if (setFrame < 0)
		{
			gEvalState.pushFrame(NULL, NULL);
			popFrame = true;
		}
		else if (!gEvalState.DictionaryStack.empty())
		{
			// We want to copy a reference to an existing stack frame
			// on to the top of the stack.  Any change that occurs to 
			// the locals during this new frame will also occur in the 
			// original frame.
			S32 stackIndex = gEvalState.DictionaryStack.size() - setFrame - 1;
			gEvalState.pushFrameRef( stackIndex );
			popFrame = true;
		}
	}

	// Grab the state of the telenet debugger here once
	// so that the push and pop frames are always balanced.
	const bool telDebuggerOn = TelDebugger && TelDebugger->isConnected();
	if ( telDebuggerOn && setFrame < 0 )
		TelDebugger->pushStackFrame();

	StringTableEntry var, objParent;
	U32 failJump;
	StringTableEntry fnName;
	StringTableEntry fnNamespace, fnPackage;
	SimObject *currentNewObject = 0;
	StringTableEntry prevField = NULL;
	StringTableEntry curField = NULL;
	SimObject *prevObject = NULL;
	SimObject *curObject = NULL;
	SimObject *saveObject=NULL;
	Namespace::Entry *nsEntry;
	Namespace *ns;
	const char* curFNDocBlock = NULL;
	const char* curNSDocBlock = NULL;
	const S32 nsDocLength = 128;
	char nsDocBlockClass[nsDocLength];

	U32 callArgc;
	const char **callArgv;

	static char curFieldArray[256];
	static char prevFieldArray[256];

	CodeBlock *saveCodeBlock = smCurrentCodeBlock;
	smCurrentCodeBlock = this;
	if(this->name)
	{
		Con::gCurrentFile = this->name;
		Con::gCurrentRoot = mRoot;
	}
	const char * val;

	// The frame temp is used by the variable accessor ops (OP_SAVEFIELD_* and
	// OP_LOADFIELD_*) to store temporary values for the fields.
	static S32 VAL_BUFFER_SIZE = 1024;
	FrameTemp<char> valBuffer( VAL_BUFFER_SIZE );

	for(;;)
	{
		U32 instruction = code[ip++];
breakContinue:
		switch(instruction)
		{
		case OP_FUNC_DECL:
			if(!noCalls)
			{
				fnName       = U32toSTE(code[ip]);
				fnNamespace  = U32toSTE(code[ip+1]);
				fnPackage    = U32toSTE(code[ip+2]);
				bool hasBody = bool(code[ip+3]);

				Namespace::unlinkPackages();
				ns = Namespace::find(fnNamespace, fnPackage);
				ns->addFunction(fnName, this, hasBody ? ip : 0, curFNDocBlock ? dStrdup( curFNDocBlock ) : NULL );// if no body, set the IP to 0
				if( curNSDocBlock )
				{
					if( fnNamespace == StringTable->lookup( nsDocBlockClass ) )
					{
						char *usageStr = dStrdup( curNSDocBlock );
						usageStr[dStrlen(usageStr)] = '\0';
						ns->mUsage = usageStr;
						ns->mCleanUpUsage = true;
						curNSDocBlock = NULL;
					}
				}
				Namespace::relinkPackages();

				// If we had a docblock, it's definitely not valid anymore, so clear it out.
				curFNDocBlock = NULL;

				//Con::printf("Adding function %s::%s (%d)", fnNamespace, fnName, ip);
			}
			ip = code[ip + 4];
			break;

		case OP_CREATE_OBJECT:
			{
				// Read some useful info.
				objParent        = U32toSTE(code[ip    ]);
				bool isDataBlock =          code[ip + 1];
				bool isInternal  =          code[ip + 2];
				bool isMessage   =          code[ip + 3];
				failJump         =          code[ip + 4];

				// If we don't allow calls, we certainly don't allow creating objects!
				// Moved this to after failJump is set. Engine was crashing when
				// noCalls = true and an object was being created at the beginning of
				// a file. ADL.
				if(noCalls)
				{
					ip = failJump;
					break;
				}

				// Get the constructor information off the stack.
				STR.getArgcArgv(NULL, &callArgc, &callArgv, true);

				// Con::printf("Creating object...");

				// objectName = argv[1]...
				currentNewObject = NULL;

				// Are we creating a datablock? If so, deal with case where we override
				// an old one.
				if(isDataBlock)
				{
					// Con::printf("  - is a datablock");

					// Find the old one if any.
					SimObject *db = Sim::getDataBlockGroup()->findObject(callArgv[2]);

					// Make sure we're not changing types on ourselves...
					if(db && dStricmp(db->getClassName(), callArgv[1]))
					{
						Con::errorf(ConsoleLogEntry::General, "Cannot re-declare data block %s with a different class.", callArgv[2]);
						ip = failJump;
						break;
					}

					// If there was one, set the currentNewObject and move on.
					if(db)
						currentNewObject = db;
				}

				if(!currentNewObject)
				{
					// Well, looks like we have to create a new object.
					ConsoleObject *object = ConsoleObject::create(callArgv[1]);

					// Deal with failure!
					if(!object)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-conobject class %s.", getFileLine(ip-1), callArgv[1]);
						ip = failJump;
						break;
					}

					// Do special datablock init if appropros
					if(isDataBlock)
					{
						SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(object);
						if(dataBlock)
						{
							dataBlock->assignId();
						}
						else
						{
							// They tried to make a non-datablock with a datablock keyword!
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-datablock class %s.", getFileLine(ip-1), callArgv[1]);

							// Clean up...
							delete object;
							ip = failJump;
							break;
						}
					}

					// Finally, set currentNewObject to point to the new one.
					currentNewObject = dynamic_cast<SimObject *>(object);

					// Deal with the case of a non-SimObject.
					if(!currentNewObject)
					{
						Con::errorf(ConsoleLogEntry::General, "%s: Unable to instantiate non-SimObject class %s.", getFileLine(ip-1), callArgv[1]);
						delete object;
						ip = failJump;
						break;
					}

					// Does it have a parent object? (ie, the copy constructor : syntax, not inheriance)
					// [tom, 9/8/2006] it is inheritance if it's a message ... muwahahah!
					if(!isMessage && *objParent)
					{
						// Find it!
						SimObject *parent;
						if(Sim::findObject(objParent, parent))
						{
							// Con::printf(" - Parent object found: %s", parent->getClassName());

							// and suck the juices from it!
							currentNewObject->assignFieldsFrom(parent);
						}
						else
							Con::errorf(ConsoleLogEntry::General, "%s: Unable to find parent object %s for %s.", getFileLine(ip-1), objParent, callArgv[1]);

						// Mm! Juices!
					}

					// If a name was passed, assign it.
					if(callArgv[2][0])
					{
						if(! isMessage)
						{
							if(! isInternal)
								currentNewObject->assignName(callArgv[2]);
							else
								currentNewObject->setInternalName(callArgv[2]);
						}
						else
						{
							Message *msg = dynamic_cast<Message *>(currentNewObject);
							if(msg)
							{
								msg->setClassNamespace(callArgv[2]);
								msg->setSuperClassNamespace(objParent);
							}
							else
							{
								Con::errorf(ConsoleLogEntry::General, "%s: Attempting to use newmsg on non-message type %s", getFileLine(ip-1), callArgv[1]);
								delete currentNewObject;
								currentNewObject = NULL;
								ip = failJump;
								break;
							}
						}
					}

					// Do the constructor parameters.
					if(!currentNewObject->processArguments(callArgc-3, callArgv+3))
					{
						delete currentNewObject;
						currentNewObject = NULL;
						ip = failJump;
						break;
					}

					// If it's not a datablock, allow people to modify bits of it.
					if(!isDataBlock)
					{
						currentNewObject->setModStaticFields(true);
						currentNewObject->setModDynamicFields(true);
					}
				}

				// Advance the IP past the create info...
				ip += 5;
				break;
			}

		case OP_ADD_OBJECT:
			{
				// See OP_SETCURVAR for why we do this.
				curFNDocBlock = NULL;
				curNSDocBlock = NULL;

				// Do we place this object at the root?
				bool placeAtRoot = code[ip++];

				// Con::printf("Adding object %s", currentNewObject->getName());

				// Make sure it wasn't already added, then add it.
				if(currentNewObject->isProperlyAdded() == false)
				{
					bool ret = false;

					Message *msg = dynamic_cast<Message *>(currentNewObject);
					if(msg)
					{
						SimObjectId id = Message::getNextMessageID();
						if(id != 0xffffffff)
							ret = currentNewObject->registerObject(id);
						else
							Con::errorf("%s: No more object IDs available for messages", getFileLine(ip-2));
					}
					else
						ret = currentNewObject->registerObject();

					if(! ret)
					{
						// This error is usually caused by failing to call Parent::initPersistFields in the class' initPersistFields().
						Con::warnf(ConsoleLogEntry::General, "%s: Register object failed for object %s of class %s.", getFileLine(ip-2), currentNewObject->getName(), currentNewObject->getClassName());
						delete currentNewObject;
						ip = failJump;
						break;
					}
				}

				// Are we dealing with a datablock?
				SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(currentNewObject);
				static char errorBuffer[256];

				// If so, preload it.
				if(dataBlock && !dataBlock->preload(true, errorBuffer))
				{
					Con::errorf(ConsoleLogEntry::General, "%s: preload failed for %s: %s.", getFileLine(ip-2),
						currentNewObject->getName(), errorBuffer);
					dataBlock->deleteObject();
					ip = failJump;
					break;
				}

				// What group will we be added to, if any?
				U32 groupAddId = intStack[UINT];
				SimGroup *grp = NULL;
				SimSet   *set = NULL;
				SimComponent *comp = NULL;
				bool isMessage = dynamic_cast<Message *>(currentNewObject) != NULL;

				if(!placeAtRoot || !currentNewObject->getGroup())
				{
					if(! isMessage)
					{
						if(! placeAtRoot)
						{
							// Otherwise just add to the requested group or set.
							if(!Sim::findObject(groupAddId, grp))
								if(!Sim::findObject(groupAddId, comp))
									Sim::findObject(groupAddId, set);
						}

						if(placeAtRoot || comp != NULL)
						{
							// Deal with the instantGroup if we're being put at the root or we're adding to a component.
							const char *addGroupName = Con::getVariable("instantGroup");
							if(!Sim::findObject(addGroupName, grp))
								Sim::findObject(RootGroupId, grp);
						}

						if(comp)
						{
							SimComponent *newComp = dynamic_cast<SimComponent *>(currentNewObject);
							if(newComp)
							{
								if(! comp->addComponent(newComp))
									Con::errorf("%s: Unable to add component %s, template not loaded?", getFileLine(ip-2), currentNewObject->getName() ? currentNewObject->getName() : currentNewObject->getIdString());
							}
						}
					}

					// If we didn't get a group, then make sure we have a pointer to
					// the rootgroup.
					if(!grp)
						Sim::findObject(RootGroupId, grp);

					// add to the parent group
					grp->addObject(currentNewObject);

					// add to any set we might be in
					if(set)
						set->addObject(currentNewObject);
				}

				// store the new object's ID on the stack (overwriting the group/set
				// id, if one was given, otherwise getting pushed)
				if(placeAtRoot) 
					intStack[UINT] = currentNewObject->getId();
				else
					intStack[++UINT] = currentNewObject->getId();

				break;
			}

		case OP_END_OBJECT:
			{
				// If we're not to be placed at the root, make sure we clean up
				// our group reference.
				bool placeAtRoot = code[ip++];
				if(!placeAtRoot)
					UINT--;
				break;
			}

		case OP_JMPIFFNOT:
			if(floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT:
			if(intStack[UINT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFF:
			if(!floatStack[FLT--])
			{
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF:
			if(!intStack[UINT--])
			{
				ip ++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIFNOT_NP:
			if(intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMPIF_NP:
			if(!intStack[UINT])
			{
				UINT--;
				ip++;
				break;
			}
			ip = code[ip];
			break;
		case OP_JMP:
			ip = code[ip];
			break;
		case OP_RETURN:
			goto execFinished;
		case OP_CMPEQ:
			intStack[UINT+1] = bool(floatStack[FLT] == floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGR:
			intStack[UINT+1] = bool(floatStack[FLT] > floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPGE:
			intStack[UINT+1] = bool(floatStack[FLT] >= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLT:
			intStack[UINT+1] = bool(floatStack[FLT] < floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPLE:
			intStack[UINT+1] = bool(floatStack[FLT] <= floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_CMPNE:
			intStack[UINT+1] = bool(floatStack[FLT] != floatStack[FLT-1]);
			UINT++;
			FLT -= 2;
			break;

		case OP_XOR:
			intStack[UINT-1] = intStack[UINT] ^ intStack[UINT-1];
			UINT--;
			break;

		case OP_MOD:
			if(  intStack[UINT-1] != 0 )
				intStack[UINT-1] = intStack[UINT] % intStack[UINT-1];
			else
				intStack[UINT-1] = 0;
			UINT--;
			break;

		case OP_BITAND:
			intStack[UINT-1] = intStack[UINT] & intStack[UINT-1];
			UINT--;
			break;

		case OP_BITOR:
			intStack[UINT-1] = intStack[UINT] | intStack[UINT-1];
			UINT--;
			break;

		case OP_NOT:
			intStack[UINT] = !intStack[UINT];
			break;

		case OP_NOTF:
			intStack[UINT+1] = !floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_ONESCOMPLEMENT:
			intStack[UINT] = ~intStack[UINT];
			break;

		case OP_SHR:
			intStack[UINT-1] = intStack[UINT] >> intStack[UINT-1];
			UINT--;
			break;

		case OP_SHL:
			intStack[UINT-1] = intStack[UINT] << intStack[UINT-1];
			UINT--;
			break;

		case OP_AND:
			intStack[UINT-1] = intStack[UINT] && intStack[UINT-1];
			UINT--;
			break;

		case OP_OR:
			intStack[UINT-1] = intStack[UINT] || intStack[UINT-1];
			UINT--;
			break;

		case OP_ADD:
			floatStack[FLT-1] = floatStack[FLT] + floatStack[FLT-1];
			FLT--;
			break;

		case OP_SUB:
			floatStack[FLT-1] = floatStack[FLT] - floatStack[FLT-1];
			FLT--;
			break;

		case OP_MUL:
			floatStack[FLT-1] = floatStack[FLT] * floatStack[FLT-1];
			FLT--;
			break;
		case OP_DIV:
			floatStack[FLT-1] = floatStack[FLT] / floatStack[FLT-1];
			FLT--;
			break;
		case OP_NEG:
			floatStack[FLT] = -floatStack[FLT];
			break;

		case OP_SETCURVAR:
			var = U32toSTE(code[ip]);
			ip++;

			// If a variable is set, then these must be NULL. It is necessary
			// to set this here so that the vector parser can appropriately
			// identify whether it's dealing with a vector.
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarName(var);

			// In order to let docblocks work properly with variables, we have
			// clear the current docblock when we do an assign. This way it 
			// won't inappropriately carry forward to following function decls.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_CREATE:
			var = U32toSTE(code[ip]);
			ip++;

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarNameCreate(var);

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_ARRAY:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarName(var);

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_SETCURVAR_ARRAY_CREATE:
			var = STR.getSTValue();

			// See OP_SETCURVAR
			prevField = NULL;
			prevObject = NULL;
			curObject = NULL;

			gEvalState.setCurVarNameCreate(var);

			// See OP_SETCURVAR for why we do this.
			curFNDocBlock = NULL;
			curNSDocBlock = NULL;
			break;

		case OP_LOADVAR_UINT:
			intStack[UINT+1] = gEvalState.getIntVariable();
			UINT++;
			break;

		case OP_LOADVAR_FLT:
			floatStack[FLT+1] = gEvalState.getFloatVariable();
			FLT++;
			break;

		case OP_LOADVAR_STR:
			val = gEvalState.getStringVariable();
			STR.setStringValue(val);
			break;

		case OP_SAVEVAR_UINT:
			gEvalState.setIntVariable(intStack[UINT]);
			break;

		case OP_SAVEVAR_FLT:
			gEvalState.setFloatVariable(floatStack[FLT]);
			break;

		case OP_SAVEVAR_STR:
			gEvalState.setStringVariable(STR.getStringValue());
			break;

		case OP_SETCUROBJECT:
			// Save the previous object for parsing vector fields.
			prevObject = curObject;
			val = STR.getStringValue();

			// Sim::findObject will sometimes find valid objects from
			// multi-component strings. This makes sure that doesn't
			// happen.
			for( const char* check = val; *check; check++ )
			{
				if( *check == ' ' )
				{
					val = "";
					break;
				}
			}
			curObject = Sim::findObject(val);
			break;

		case OP_SETCUROBJECT_INTERNAL:
			++ip; // To skip the recurse flag if the object wasnt found
			if(curObject)
			{
				SimGroup *group = dynamic_cast<SimGroup *>(curObject);
				if(group)
				{
					StringTableEntry intName = StringTable->insert(STR.getStringValue());
					bool recurse = code[ip-1];
					SimObject *obj = group->findObjectByInternalName(intName, recurse);
					intStack[UINT+1] = obj ? obj->getId() : 0;
					UINT++;
				}
				else
				{
					Con::errorf(ConsoleLogEntry::Script, "%s: Attempt to use -> on non-group %s of class %s.", getFileLine(ip-2), curObject->getName(), curObject->getClassName());
					intStack[UINT] = 0;
				}
			}
			break;

		case OP_SETCUROBJECT_NEW:
			curObject = currentNewObject;
			break;

		case OP_SETCURFIELD:
			// Save the previous field for parsing vector fields.
			prevField = curField;
			dStrcpy( prevFieldArray, curFieldArray );
			curField = U32toSTE(code[ip]);
			curFieldArray[0] = 0;
			ip++;
			break;

		case OP_SETCURFIELD_ARRAY:
			dStrcpy(curFieldArray, STR.getStringValue());
			break;

		case OP_LOADFIELD_UINT:
			if(curObject)
				intStack[UINT+1] = U32(dAtoi(curObject->getDataField(curField, curFieldArray)));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				intStack[UINT+1] = dAtoi( valBuffer );
			}
			UINT++;
			break;

		case OP_LOADFIELD_FLT:
			if(curObject)
				floatStack[FLT+1] = dAtof(curObject->getDataField(curField, curFieldArray));
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				floatStack[FLT+1] = dAtof( valBuffer );
			}
			FLT++;
			break;

		case OP_LOADFIELD_STR:
			if(curObject)
			{
				val = curObject->getDataField(curField, curFieldArray);
				STR.setStringValue( val );
			}
			else
			{
				// The field is not being retrieved from an object. Maybe it's
				// a special accessor?
				getFieldComponent( prevObject, prevField, prevFieldArray, curField, valBuffer, VAL_BUFFER_SIZE );
				STR.setStringValue( valBuffer );
			}

			break;

		case OP_SAVEFIELD_UINT:
			STR.setIntValue(intStack[UINT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_FLT:
			STR.setFloatValue(floatStack[FLT]);
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_SAVEFIELD_STR:
			if(curObject)
				curObject->setDataField(curField, curFieldArray, STR.getStringValue());
			else
			{
				// The field is not being set on an object. Maybe it's
				// a special accessor?
				setFieldComponent( prevObject, prevField, prevFieldArray, curField );
				prevObject = NULL;
			}
			break;

		case OP_STR_TO_UINT:
			intStack[UINT+1] = STR.getIntValue();
			UINT++;
			break;

		case OP_STR_TO_FLT:
			floatStack[FLT+1] = STR.getFloatValue();
			FLT++;
			break;

		case OP_STR_TO_NONE:
			// This exists simply to deal with certain typecast situations.
			break;

		case OP_FLT_TO_UINT:
			intStack[UINT+1] = (S64)floatStack[FLT];
			FLT--;
			UINT++;
			break;

		case OP_FLT_TO_STR:
			STR.setFloatValue(floatStack[FLT]);
			FLT--;
			break;

		case OP_FLT_TO_NONE:
			FLT--;
			break;

		case OP_UINT_TO_FLT:
			floatStack[FLT+1] = intStack[UINT];
			UINT--;
			FLT++;
			break;

		case OP_UINT_TO_STR:
			STR.setIntValue(intStack[UINT]);
			UINT--;
			break;

		case OP_UINT_TO_NONE:
			UINT--;
			break;

		case OP_LOADIMMED_UINT:
			intStack[UINT+1] = code[ip++];
			UINT++;
			break;

		case OP_LOADIMMED_FLT:
			floatStack[FLT+1] = curFloatTable[code[ip]];
			ip++;
			FLT++;
			break;
		case OP_TAG_TO_STR:
			code[ip-1] = OP_LOADIMMED_STR;
			// it's possible the string has already been converted
			if(U8(curStringTable[code[ip]]) != StringTagPrefixByte)
			{
				U32 id = GameAddTaggedString(curStringTable + code[ip]);
				dSprintf(curStringTable + code[ip] + 1, 7, "%d", id);
				*(curStringTable + code[ip]) = StringTagPrefixByte;
			}
		case OP_LOADIMMED_STR:
			STR.setStringValue(curStringTable + code[ip++]);
			break;

		case OP_DOCBLOCK_STR:
			{
				// If the first word of the doc is '\class' or '@class', then this
				// is a namespace doc block, otherwise it is a function doc block.
				const char* docblock = curStringTable + code[ip++];

				const char* sansClass = dStrstr( docblock, "@class" );
				if( !sansClass )
					sansClass = dStrstr( docblock, "\\class" );

				if( sansClass )
				{
					// Don't save the class declaration. Scan past the 'class'
					// keyword and up to the first whitespace.
					sansClass += 7;
					S32 index = 0;
					while( ( *sansClass != ' ' ) && ( *sansClass != '\n' ) && *sansClass && ( index < ( nsDocLength - 1 ) ) )
					{
						nsDocBlockClass[index++] = *sansClass;
						sansClass++;
					}
					nsDocBlockClass[index] = '\0';

					curNSDocBlock = sansClass + 1;
				}
				else
					curFNDocBlock = docblock;
			}

			break;

		case OP_LOADIMMED_IDENT:
			STR.setStringValue(U32toSTE(code[ip++]));
			break;

		case OP_CALLFUNC_RESOLVE:
			// This deals with a function that is potentially living in a namespace.
			fnNamespace = U32toSTE(code[ip+1]);
			fnName      = U32toSTE(code[ip]);

			// Try to look it up.
			ns = Namespace::find(fnNamespace);
			nsEntry = ns->lookup(fnName);
			if(!nsEntry)
			{
				ip+= 3;
				Con::warnf(ConsoleLogEntry::General,
					"%s: Unable to find function %s%s%s",
					getFileLine(ip-4), fnNamespace ? fnNamespace : "",
					fnNamespace ? "::" : "", fnName);
				STR.popFrame();
				break;
			}
			// Now, rewrite our code a bit (ie, avoid future lookups) and fall
			// through to OP_CALLFUNC
			code[ip+1] = *((U32 *) &nsEntry);
			code[ip-1] = OP_CALLFUNC;

		case OP_CALLFUNC:
			{
				// This routingId is set when we query the object as to whether
				// it handles this method.  It is set to an enum from the table
				// above indicating whether it handles it on a component it owns
				// or just on the object.
				S32 routingId = 0;

				fnName = U32toSTE(code[ip]);

				//if this is called from inside a function, append the ip and codeptr
				if (!gEvalState.DictionaryStack.empty())
				{
					gEvalState.DictionaryStack.last()->code = this;
					gEvalState.DictionaryStack.last()->ip = ip - 1;
				}

				U32 callType = code[ip+2];

				ip += 3;
				STR.getArgcArgv(fnName, &callArgc, &callArgv);

				if(callType == FuncCallExprNode::FunctionCall) 
				{
					nsEntry = *((Namespace::Entry **) &code[ip-2]);
					ns = NULL;
				}
				else if(callType == FuncCallExprNode::MethodCall)
				{
					saveObject = gEvalState.thisObject;
					gEvalState.thisObject = Sim::findObject(callArgv[1]);
					if(!gEvalState.thisObject)
					{
						gEvalState.thisObject = 0;
						Con::warnf(ConsoleLogEntry::General,"%s: Unable to find object: '%s' attempting to call function '%s'", getFileLine(ip-4), callArgv[1], fnName);

						STR.popFrame(); // [neo, 5/7/2007 - #2974]

						break;
					}

					bool handlesMethod = gEvalState.thisObject->handlesConsoleMethod(fnName,&routingId);
					if( handlesMethod && routingId == MethodOnComponent )
					{
						DynamicConsoleMethodComponent *pComponent = dynamic_cast<DynamicConsoleMethodComponent*>( gEvalState.thisObject );
						if( pComponent )
							pComponent->callMethodArgList( callArgc, callArgv, false );
					}

					ns = gEvalState.thisObject->getNamespace();
					if(ns)
						nsEntry = ns->lookup(fnName);
					else
						nsEntry = NULL;
				}
				else // it's a ParentCall
				{
					if(thisNamespace) {
						ns = thisNamespace->mParent;
						if(ns)
							nsEntry = ns->lookup(fnName);
						else
							nsEntry = NULL;
					} else  {
						ns = NULL;
						nsEntry = NULL;
					}
				}

				S32 nsType = -1;
				S32 nsMinArgs = 0;
				S32 nsMaxArgs = 0;
				Namespace::Entry::CallbackUnion * nsCb = NULL;
				Namespace::Entry::CallbackUnion cbu;
				const char * nsUsage = NULL;
				if (nsEntry)
				{
					nsType = nsEntry->mType;
					nsMinArgs = nsEntry->mMinArgs;
					nsMaxArgs = nsEntry->mMaxArgs;
					nsCb = &nsEntry->cb;
					nsUsage = nsEntry->mUsage;
					routingId = 0;
				}
				if(!nsEntry || noCalls)
				{
					if(!noCalls && !( routingId == MethodOnComponent ) )
					{
						Con::warnf(ConsoleLogEntry::General,"%s: Unknown command %s.", getFileLine(ip-4), fnName);
						if(callType == FuncCallExprNode::MethodCall)
						{
							Con::warnf(ConsoleLogEntry::General, "  Object %s(%d) %s",
								gEvalState.thisObject->getName() ? gEvalState.thisObject->getName() : "",
								gEvalState.thisObject->getId(), getNamespaceList(ns) );
						}
					}
					STR.popFrame();
					STR.setStringValue("");
					break;
				}
				if(nsEntry->mType == Namespace::Entry::ScriptFunctionType)
				{
					const char *ret = "";
					if(nsEntry->mFunctionOffset)
						ret = nsEntry->mCode->exec(nsEntry->mFunctionOffset, fnName, nsEntry->mNamespace, callArgc, callArgv, false, nsEntry->mPackage);

					STR.popFrame();
					STR.setStringValue(ret);
				}
				else
				{
					const char* nsName = ns? ns->mName: "";
#ifndef TORQUE_DEBUG
					// [tom, 12/13/2006] This stops tools functions from working in the console,
					// which is useful behavior when debugging so I'm ifdefing this out for debug builds.
					if(nsEntry->mToolOnly && ! Con::isCurrentScriptToolScript())
					{
						Con::errorf(ConsoleLogEntry::Script, "%s: %s::%s - attempting to call tools only function from outside of tools.", getFileLine(ip-4), nsName, fnName);
					}
					else
#endif
						if((nsEntry->mMinArgs && S32(callArgc) < nsEntry->mMinArgs) || (nsEntry->mMaxArgs && S32(callArgc) > nsEntry->mMaxArgs))
						{
							Con::warnf(ConsoleLogEntry::Script, "%s: %s::%s - wrong number of arguments.", getFileLine(ip-4), nsName, fnName);
							Con::warnf(ConsoleLogEntry::Script, "%s: usage: %s", getFileLine(ip-4), nsEntry->mUsage);
							STR.popFrame();
						}
						else
						{
							switch(nsEntry->mType)
							{
							case Namespace::Entry::StringCallbackType:
								{
									const char *ret = nsEntry->cb.mStringCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(ret != STR.getStringValue())
										STR.setStringValue(ret);
									else
										STR.setLen(dStrlen(ret));
									break;
								}
							case Namespace::Entry::IntCallbackType:
								{
									S32 result = nsEntry->cb.mIntCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							case Namespace::Entry::FloatCallbackType:
								{
									F64 result = nsEntry->cb.mFloatCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = (S64)result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setFloatValue(result);
									break;
								}
							case Namespace::Entry::VoidCallbackType:
								nsEntry->cb.mVoidCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
								if(code[ip] != OP_STR_TO_NONE)
									Con::warnf(ConsoleLogEntry::General, "%s: Call to %s in %s uses result of void function call.", getFileLine(ip-4), fnName, functionName);

								STR.popFrame();
								STR.setStringValue("");
								break;
							case Namespace::Entry::BoolCallbackType:
								{
									bool result = nsEntry->cb.mBoolCallbackFunc(gEvalState.thisObject, callArgc, callArgv);
									STR.popFrame();
									if(code[ip] == OP_STR_TO_UINT)
									{
										ip++;
										intStack[++UINT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_FLT)
									{
										ip++;
										floatStack[++FLT] = result;
										break;
									}
									else if(code[ip] == OP_STR_TO_NONE)
										ip++;
									else
										STR.setIntValue(result);
									break;
								}
							}
						}
				}

				if(callType == FuncCallExprNode::MethodCall)
					gEvalState.thisObject = saveObject;
				break;
			}
		case OP_ADVANCE_STR:
			STR.advance();
			break;
		case OP_ADVANCE_STR_APPENDCHAR:
			STR.advanceChar(code[ip++]);
			break;

		case OP_ADVANCE_STR_COMMA:
			STR.advanceChar('_');
			break;

		case OP_ADVANCE_STR_NUL:
			STR.advanceChar(0);
			break;

		case OP_REWIND_STR:
			STR.rewind();
			break;

		case OP_TERMINATE_REWIND_STR:
			STR.rewindTerminate();
			break;

		case OP_COMPARE_STR:
			intStack[++UINT] = STR.compare();
			break;
		case OP_PUSH:
			STR.push();
			break;

		case OP_PUSH_FRAME:
			STR.pushFrame();
			break;
		case OP_BREAK:
			{
				//append the ip and codeptr before managing the breakpoint!
				AssertFatal( !gEvalState.DictionaryStack.empty(), "Empty eval stack on break!");
				gEvalState.DictionaryStack.last()->code = this;
				gEvalState.DictionaryStack.last()->ip = ip - 1;

				U32 breakLine;
				findBreakLine(ip-1, breakLine, instruction);
				if(!breakLine)
					goto breakContinue;
				TelDebugger->executionStopped(this, breakLine);
				goto breakContinue;
			}
		case OP_INVALID:

		default:
			// error!
			goto execFinished;
		}
	}
execFinished:

	if ( telDebuggerOn && setFrame < 0 )
		TelDebugger->popStackFrame();

	if ( popFrame )
		gEvalState.popFrame();

	if(argv)
	{
		if(gEvalState.traceOn)
		{
			traceBuffer[0] = 0;
			dStrcat(traceBuffer, "Leaving ");

			if(packageName)
			{
				dStrcat(traceBuffer, "[");
				dStrcat(traceBuffer, packageName);
				dStrcat(traceBuffer, "]");
			}
			if(thisNamespace && thisNamespace->mName)
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s::%s() - return %s", thisNamespace->mName, thisFunctionName, STR.getStringValue());
			}
			else
			{
				dSprintf(traceBuffer + dStrlen(traceBuffer), sizeof(traceBuffer) - dStrlen(traceBuffer),
					"%s() - return %s", thisFunctionName, STR.getStringValue());
			}
			Con::printf("%s", traceBuffer);
		}
	}
	else
	{
		//-Mat this may leak
		delete[] const_cast<char*>(globalStrings);
		delete[] globalFloats;
		globalStrings = NULL;
		globalFloats = NULL;
	}
	smCurrentCodeBlock = saveCodeBlock;
	if(saveCodeBlock && saveCodeBlock->name)
	{
		Con::gCurrentFile = saveCodeBlock->name;
		Con::gCurrentRoot = saveCodeBlock->mRoot;
	}

	decRefCount();

#ifdef TORQUE_DEBUG
	AssertFatal(!(STR.mStartStackSize > stackStart), "String stack not popped enough in script exec");
	AssertFatal(!(STR.mStartStackSize < stackStart), "String stack popped too much in script exec");
#endif
	return STR.getStringValue();
}

#endif //PUAP_SCRIPT_CHANGE
//------------------------------------------------------------

#endif //PUAP_NAMESPACE_CHANGE
