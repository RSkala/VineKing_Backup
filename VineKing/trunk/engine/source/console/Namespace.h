#ifndef _NAMESPACE_H
#define _NAMESPACE_H



#ifdef PUAP_NAMESPACE_CHANGE


#include "core/stringTable.h"
#include "core/tVector.h"
#include "console/consoleTypes.h"

#include "tHashTable.h"
#include "Dictionary.h"

class ExprEvalState;
struct FunctionDecl;
class CodeBlock;
class AbstractClassRep;
class Package;

class Namespace
{
private:
	//this is the Package that we belong to (are held in)
    Package *mOwner;
public:
	friend class Package;
	//for our inheritance on Namespaces (i.e. ConsoleObject->ScriptObject->SimObject)
	Namespace *mParent;
	Namespace *mChild;
    StringTableEntry mName;
	Dictionary *mDictionary;

    AbstractClassRep *mClassRep;
    U32 mRefCountToParent;
    const char* mUsage;
    // Script defined usage strings need to be cleaned up. This
    // field indicates whether or not the usage was set from script.
    bool mCleanUpUsage;

    struct Entry
    {
        enum {
            GroupMarker                  = -3,
            OverloadMarker               = -2,
            InvalidFunctionType          = -1,
            ScriptFunctionType,
            StringCallbackType,
            IntCallbackType,
            FloatCallbackType,
            VoidCallbackType,
            BoolCallbackType
        };

        Namespace *mNamespace;
        StringTableEntry mFunctionName;
        S32 mType;
        S32 mMinArgs;
        S32 mMaxArgs;
        const char *mUsage;
        bool mToolOnly;
		S32 recursionCount;//-Mat used to keep track of how many times this function has been called and not yet returned

        CodeBlock *mCode;
        U32 mFunctionOffset;
        union CallbackUnion {
            StringCallback mStringCallbackFunc;
            IntCallback mIntCallbackFunc;
            VoidCallback mVoidCallbackFunc;
            FloatCallback mFloatCallbackFunc;
            BoolCallback mBoolCallbackFunc;
            const char* mGroupName;
        } cb;
        Entry();
        void clear();

        const char *execute(S32 argc, const char **argv);

    };

	//-Mat we are now only allowing a Namespace to be declared with it's parent
    Namespace( Package *parent, StringTableEntry name = NULL );
    ~Namespace();
	
	tHashTable<StringTableEntry, Entry*> *mChildEntryList;

	Package *getOwner() { return mOwner; }
	Namespace *getParent() { return mParent; }

    void addFunction(StringTableEntry name, CodeBlock *cb, U32 functionOffset, const char* usage = NULL);
    void addCommand(StringTableEntry name,StringCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,IntCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,FloatCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,VoidCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,BoolCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);

    void addOverload(const char *name, const char* altUsage);

    void markGroup(const char* name, const char* usage);
    char * lastUsage;

    void getEntryList(Vector<Entry *> *);

    Entry *lookup(StringTableEntry name);
	//for finding "inherited" functions
	Entry *lookupMethod(StringTableEntry name, SimObject *obj);
    Entry *createLocalEntry(StringTableEntry name);

	void printAllEntries();

    bool classLinkTo(Namespace *parent);
    bool unlinkClass(Namespace *parent);

    const char *tabComplete(const char *prevText, S32 baseLen, bool fForward);

	static Namespace *globalFunctionNamespace();

    static Namespace *find(StringTableEntry name, StringTableEntry package=NULL);
	static Namespace *findAndCreate(StringTableEntry name, StringTableEntry package=NULL);

	//-Mat implement these
    static void dumpClasses( bool dumpScript = true, bool dumpEngine = true );
    static void dumpFunctions( bool dumpScript = true, bool dumpEngine = true );
    static void printNamespaceEntries(Namespace * g, bool dumpScript = true, bool dumpEngine = true);
};


#else //normal TGB

#ifndef _STRINGTABLE_H_
#include "core/stringTable.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#include "Dictionary.h"

class ExprEvalState;
struct FunctionDecl;
class CodeBlock;
class AbstractClassRep;

class Namespace
{
    enum {
        MaxActivePackages = 512,
    };

    static U32 mNumActivePackages;
    static U32 mOldNumActivePackages;
    static StringTableEntry mActivePackages[MaxActivePackages];
public:
    StringTableEntry mName;
    StringTableEntry mPackage;

    Namespace *mParent;
    Namespace *mNext;
	Dictionary *mDictionary;//-Mat to keep from constantly newing and deleting Dictionaries
    AbstractClassRep *mClassRep;
    U32 mRefCountToParent;
    const char* mUsage;
    // Script defined usage strings need to be cleaned up. This
    // field indicates whether or not the usage was set from script.
    bool mCleanUpUsage;

    struct Entry
    {
        enum {
            GroupMarker                  = -3,
            OverloadMarker               = -2,
            InvalidFunctionType          = -1,
            ScriptFunctionType,
            StringCallbackType,
            IntCallbackType,
            FloatCallbackType,
            VoidCallbackType,
            BoolCallbackType
        };

        Namespace *mNamespace;
        Entry *mNext;
        StringTableEntry mFunctionName;
        S32 mType;
        S32 mMinArgs;
        S32 mMaxArgs;
        const char *mUsage;
        StringTableEntry mPackage;
        bool mToolOnly;
		U32 recursionCount;//-Mat used to keep track of how many times this function has been called and not yet returned

        CodeBlock *mCode;
        U32 mFunctionOffset;
        union CallbackUnion {
            StringCallback mStringCallbackFunc;
            IntCallback mIntCallbackFunc;
            VoidCallback mVoidCallbackFunc;
            FloatCallback mFloatCallbackFunc;
            BoolCallback mBoolCallbackFunc;
            const char* mGroupName;
        } cb;
        Entry();
        void clear();

		const char *execute(S32 argc, const char **argv, ExprEvalState *state);

    };
    Entry *mEntryList;

    Entry **mHashTable;
    U32 mHashSize;
    U32 mHashSequence;  ///< @note The hash sequence is used by the autodoc console facility
                        ///        as a means of testing reference state.

    Namespace();
    ~Namespace();
    void addFunction(StringTableEntry name, CodeBlock *cb, U32 functionOffset, const char* usage = NULL);
    void addCommand(StringTableEntry name,StringCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,IntCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,FloatCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,VoidCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);
    void addCommand(StringTableEntry name,BoolCallback, const char *usage, S32 minArgs, S32 maxArgs, bool toolOnly = false);

    void addOverload(const char *name, const char* altUsage);

    void markGroup(const char* name, const char* usage);
    char * lastUsage;

    void getEntryList(Vector<Entry *> *);

    Entry *lookup(StringTableEntry name);
    Entry *lookupRecursive(StringTableEntry name);
    Entry *createLocalEntry(StringTableEntry name);
    void buildHashTable();
    void clearEntries();
    bool classLinkTo(Namespace *parent);
    bool unlinkClass(Namespace *parent);

    const char *tabComplete(const char *prevText, S32 baseLen, bool fForward);

    static U32 mCacheSequence;
    static DataChunker mCacheAllocator;
    static DataChunker mAllocator;
    static void trashCache();
    static Namespace *mNamespaceList;
    static Namespace *mGlobalNamespace;

    static void init();
    static void shutdown();
    static Namespace *global();

    static Namespace *find(StringTableEntry name, StringTableEntry package=NULL);

    static void activatePackage(StringTableEntry name);
    static void deactivatePackage(StringTableEntry name);
    static void dumpClasses( bool dumpScript = true, bool dumpEngine = true );
    static void dumpFunctions( bool dumpScript = true, bool dumpEngine = true );
    static void printNamespaceEntries(Namespace * g, bool dumpScript = true, bool dumpEngine = true);
    static void unlinkPackages();
    static void relinkPackages();
    static bool isPackage(StringTableEntry name);
};

#endif //PUAP_NAMESPACE_CHANGE




#endif// _NAMESPACE_H
