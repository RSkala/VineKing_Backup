
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

#include "tHashTable.h"

#include "Namespace.h"

class ExprEvalState;


Namespace::Entry::Entry()
{
   mCode = NULL;
   mType = InvalidFunctionType;
   recursionCount = 0;
   mMinArgs = 0;
   mMaxArgs = 0;
}

void Namespace::Entry::clear()
{
   if(mCode)
   {
      mCode->decRefCount();
      mCode = NULL;
   }

   // Clean up usage strings generated for script functions.
   if( ( mType == Namespace::Entry::ScriptFunctionType ) && mUsage )
   {
      delete mUsage;
      mUsage = NULL;
   }
}

Namespace::Namespace( Package *parent, StringTableEntry name )
{
   mUsage = NULL;
   mCleanUpUsage = false;
   mName = name;
   mOwner = NULL;
   mParent = NULL;
   mChild = NULL;

   mRefCountToParent = 0;
   mClassRep = 0;
   mDictionary = NULL;

   mChildEntryList = new tHashTable<StringTableEntry,Entry*>;
   mOwner = NULL;
   //-Mat ALWAYS add to a Package
   //maybe allow for parent to be NULL and add to Mainpackage
   parent->addNamespace( this );
}

Namespace::~Namespace()
{
   if( mUsage && mCleanUpUsage )
   {
      delete mUsage;
      mUsage = NULL;
      mCleanUpUsage = false;
   }
   if(mDictionary){
	   delete mDictionary;
	   mDictionary = NULL;
   }
}


Namespace *Namespace::find(StringTableEntry name, StringTableEntry packageName)
{
	Namespace *ret = NULL;
	Package *package = NULL;
	//-Mat if we can't find it, we need to check all the active packages
	if( packageName == NULL ) {
		//both NULL, we want the current Package's default Namespace
		package = Package::getMainNamespacePackage();
		} else {
		package = Package::findPackage( packageName );
	}

	if( !package ) {
		Con::errorf( "Error: Package %s does not exist", packageName );
		return NULL;
	}

	ret = package->findNamespace( name );
	return ret;
}

extern Vector<Package *> gActivePackages;

Namespace *Namespace::findAndCreate(StringTableEntry name, StringTableEntry packageName)
{
	Namespace *ret = NULL;
	Package *package = NULL;

	//get package if we have packageName, else use main Package
	if( packageName == NULL ) {
		//don't know what Package we want, check the active ones, most recent first
			package = Package::getMainNamespacePackage();
	} else {
		//if we have a package name we DO want to create it if it doesn't exist
		package = Package::findAndCreatePackage( packageName );
	}

	if( name == NULL ) {
		//-Mat we should create a MainNamespace for each Package here, and look it up subsequently
		name = StringTable->insert( DEFAULT_PACKAGE_NAMESPACE_NAME );
	}

	ret = package->findAndCreateNamespace( name );
	return ret;
}


bool Namespace::unlinkClass(Namespace *parent)
{

	//-Mat sometimes we unlink twice in a row, so if mParent is NULL, that's OK
	if( mParent && mParent != parent ) {
		Con::errorf(ConsoleLogEntry::General, "Namespace::unlinkClass - cannot unlink namespace parent linkage for %s for %s.",
		mName, mParent->mName);
		return false;
	}

	mRefCountToParent--;
	AssertFatal(mRefCountToParent >= 0, "Namespace::unlinkClass - reference count to parent is less than 0");

	//-Mat see if mChild == this
	if( mParent ) {
		mParent->mChild = NULL;
	}

	if(mRefCountToParent==0){	
		mParent = NULL;
	}

	return true;
}


bool Namespace::classLinkTo(Namespace *parent)
{
	if( mParent && mParent != parent) {
		//sometimes Torque likes to re-link a Namespace back to it's current parent
		Con::errorf(ConsoleLogEntry::General, "Error: cannot change namespace parent linkage of %s from %s to %s (unlink first).",
		mName, mParent->mName, parent->mName);
		return false;
	}

	mRefCountToParent++;
	mParent = parent;
	mParent->mChild = this;
	return true;
}

Namespace *Namespace::globalFunctionNamespace() {
	return Package::getMainNamespace();
}

const char *Namespace::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
	//-Mat implement tabComplete
	/*
   const char *bestMatch = NULL;
   for( int i = 0; i < mChildEntryList->getSize(); i++ ) {
	   Entry* current = mChildEntryList->getElement( i );
	   if(current && canTabComplete(prevText, bestMatch, current->mFunctionName, baseLen, fForward)) {
		   bestMatch = current->mFunctionName;
	   }
   }
   */
	return NULL;
}

Namespace::Entry *Namespace::lookup(StringTableEntry name)
{

	U32 startPackage = mOwner->mActivePosition;
	U32 packageListSize = gActivePackages.size()-1;
	
	Package *currentPackage = NULL;
	Namespace *currentNS = this;
	Entry *ret = NULL;

	while( currentNS ) {
		//go through each active package, starting from our owner	
		for( S32 i = packageListSize; i >= 0; i-- ) {
			//if(i==startPackage) continue;
			currentPackage = gActivePackages[i];
			//find "this" namespace in the current package
			Namespace *ns = currentPackage->findNamespace( currentNS->mName );
			if(ns) ret = ns->mChildEntryList->find( name ).getValue();
			if( ret ) {
				return ret;
	}
}

		//keep going up the parent tree
		currentNS = currentNS->mParent;
	}

	return ret;
}

Namespace::Entry *Namespace::lookupMethod(StringTableEntry name, SimObject *obj)
{
	//look it up normal first
	Entry *ret = lookup( name );
	//-Mat if we can't find it here, check the parent class of this object
	if( !ret ) {
		//-Mat this needs work
		//use the AbstractClassRep to find the parent class namespace
		AbstractClassRep *currentRep = obj->getClassRep();
		Namespace *parent = NULL;
		//go up the inheritance tree to find this Entry
		while( currentRep ) {
			parent = currentRep->getNameSpace();
			ret = parent->mChildEntryList->find( name ).getValue();
			if( ret ) {
				return ret;
			} else {
				//get the next one
				currentRep = currentRep->getParentClass();
			}
		}
	}
	return ret;
}


void Namespace::printAllEntries() {
	//same as above, but this doesn't look in Active Packages
	//-Mat implement printAllEntries
	/*
	U32 size = mChildEntryList->getSize();
	Con::printf("\n\nNamespace %s Entries:\n", mName);
	for( int i = 0; i < size; i++ ) {
		Entry *current = mChildEntryList->getElement( i );
		Con::printf( "%s()", current->mFunctionName );
	}
	*/
}


static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   const Namespace::Entry* fa = *((Namespace::Entry**)a);
   const Namespace::Entry* fb = *((Namespace::Entry**)b);

   return dStricmp(fa->mFunctionName, fb->mFunctionName);
}

void Namespace::getEntryList(Vector<Entry *> *vec)
{
	//-Mat implement getEntryList()
	/*
	for( S32 i = 0; i < mChildEntryList->getSize(); i++ ) {
		Entry *current = mChildEntryList->getElement( i );
		if( current ) {
			vec->push_back( current );
		}
	}
	//return list in alphabetical order
   dQsort(vec->address(),vec->size(),sizeof(Namespace::Entry *),compareEntries);
	*/
}

Namespace::Entry *Namespace::createLocalEntry(StringTableEntry name)
{
	name = StringTable->insert( name );
	//check HashTable before creating a new entry
	Entry *ret = mChildEntryList->find( name ).getValue();
	if( ret) {
		return ret;
	}

   Entry *ent = new Entry;
   constructInPlace(ent);

   ent->mNamespace = this;
   ent->mFunctionName = StringTable->insert( name );
   ent->mToolOnly = false;

   mChildEntryList->insertUnique( ent->mFunctionName, ent );
   return ent;
}

void Namespace::addFunction(StringTableEntry name, CodeBlock *cb, U32 functionOffset, const char* usage)
{
   Entry *ent = createLocalEntry(name);

   ent->mUsage = usage;
   ent->mCode = cb;
   ent->mFunctionOffset = functionOffset;
   ent->mCode->incRefCount();
   ent->mType = Entry::ScriptFunctionType;
}

void Namespace::addCommand(StringTableEntry name,StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::StringCallbackType;
   ent->cb.mStringCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::IntCallbackType;
   ent->cb.mIntCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::VoidCallbackType;
   ent->cb.mVoidCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::FloatCallbackType;
   ent->cb.mFloatCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::BoolCallbackType;
   ent->cb.mBoolCallbackFunc = cb;
}

void Namespace::addOverload(const char * name, const char *altUsage)
{
   static U32 uid=0;
   char buffer[1024];
   char lilBuffer[32];
   dStrcpy(buffer, name);
   dSprintf(lilBuffer, 32, "_%d", uid++);
   dStrcat(buffer, lilBuffer);

   Entry *ent = createLocalEntry(StringTable->insert( buffer ));

   ent->mUsage = altUsage;
   ent->mMinArgs = -1;
   ent->mMaxArgs = -2;

   ent->mType = Entry::OverloadMarker;
   ent->cb.mGroupName = name;
}

void Namespace::markGroup(const char* name, const char* usage)
{
   static U32 uid=0;
   char buffer[1024];
   char lilBuffer[32];
   dStrcpy(buffer, name);
   dSprintf(lilBuffer, 32, "_%d", uid++);
   dStrcat(buffer, lilBuffer);

   Entry *ent = createLocalEntry(StringTable->insert( buffer ));

   if(usage != NULL)
      lastUsage = (char*)(ent->mUsage = usage);
   else
      ent->mUsage = lastUsage;

   ent->mMinArgs = -1; // Make sure it explodes if somehow we run this entry.
   ent->mMaxArgs = -2;

   ent->mType = Entry::GroupMarker;
   ent->cb.mGroupName = name;
}

const char *Namespace::Entry::execute(S32 argc, const char **argv)
{
	ExprEvalState *state = &gEvalState;
   if(mType == ScriptFunctionType)
   {
	   if(mFunctionOffset) {
		   //-Mat pass in recursionCount, the number of times this entry has been called
		   recursionCount++;
		   //-Mat may need to change parent call here
		   const char * ret = mCode->exec(mFunctionOffset, argv[0], mNamespace, argc, argv, false, mNamespace->getOwner()->mName, recursionCount );
		   recursionCount--;

		   return ret;
	   } else {
         return "";
	   }
   }

#ifndef TORQUE_DEBUG
   // [tom, 12/13/2006] This stops tools functions from working in the console,
   // which is useful behavior when debugging so I'm ifdefing this out for debug builds.
   if(mToolOnly && ! Con::isCurrentScriptToolScript())
   {
      Con::errorf(ConsoleLogEntry::Script, "%s::%s - attempting to call tools only function from outside of tools", mNamespace->mName, mFunctionName);
      return "";
   }
#endif

   if((mMinArgs && argc < mMinArgs) || (mMaxArgs && argc > mMaxArgs))
   {
      Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", mNamespace->mName, mFunctionName);
      Con::warnf(ConsoleLogEntry::Script, "usage: %s", mUsage);
      return "";
   }

   static char returnBuffer[32];
   switch(mType)
   {
      case StringCallbackType:
         return cb.mStringCallbackFunc(state->thisObject, argc, argv);
      case IntCallbackType:
         dSprintf(returnBuffer, sizeof(returnBuffer), "%d",
            cb.mIntCallbackFunc(state->thisObject, argc, argv));
         return returnBuffer;
      case FloatCallbackType:
         dSprintf(returnBuffer, sizeof(returnBuffer), "%g",
            cb.mFloatCallbackFunc(state->thisObject, argc, argv));
         return returnBuffer;
      case VoidCallbackType:
         cb.mVoidCallbackFunc(state->thisObject, argc, argv);
         return "";
      case BoolCallbackType:
         dSprintf(returnBuffer, sizeof(returnBuffer), "%d",
            (U32)cb.mBoolCallbackFunc(state->thisObject, argc, argv));
         return returnBuffer;
   }

   return "";
}

#else //normal TGB

#include "platform/platform.h"
#include "console/console.h"

#include "console/ast.h"
#include "core/tAlgorithm.h"
#include "core/resManager.h"

#include "core/findMatch.h"
#include "console/consoleInternal.h"
#include "core/fileStream.h"
#include "console/compiler.h"

#include "Namespace.h"
#include "Dictionary.h"

U32 Namespace::mCacheSequence = 0;
DataChunker Namespace::mCacheAllocator;
DataChunker Namespace::mAllocator;
Namespace *Namespace::mNamespaceList = NULL;
Namespace *Namespace::mGlobalNamespace = NULL;


extern bool canTabComplete(const char *prevText, const char *bestMatch,
               const char *newText, S32 baseLen, bool fForward);

//used for creating our vector of parameter entries
#define DEFAULT_MAX_ARGS	10


Namespace::Entry::Entry()
{
   mCode = NULL;
   mType = InvalidFunctionType;
   recursionCount = 0;
   mMinArgs = 0;
   mMaxArgs = 0;
}

void Namespace::Entry::clear()
{
   if(mCode)
   {
      mCode->decRefCount();
      mCode = NULL;
   }

   // Clean up usage strings generated for script functions.
   if( ( mType == Namespace::Entry::ScriptFunctionType ) && mUsage )
   {
      delete mUsage;
      mUsage = NULL;
   }
}

Namespace::Namespace()
{
   mPackage = NULL;
   mUsage = NULL;
   mCleanUpUsage = false;
   mName = NULL;
   mParent = NULL;
   mNext = NULL;
   mEntryList = NULL;
   mHashSize = 0;
   mHashTable = 0;
   mHashSequence = 0;
   mRefCountToParent = 0;
   mClassRep = 0;
   mDictionary = NULL;
}

Namespace::~Namespace()
{
   if( mUsage && mCleanUpUsage )
   {
      delete mUsage;
      mUsage = NULL;
      mCleanUpUsage = false;
   }
}

void Namespace::clearEntries()
{
   for(Entry *walk = mEntryList; walk; walk = walk->mNext)
      walk->clear();
}

Namespace *Namespace::find(StringTableEntry name, StringTableEntry package)
{
   for(Namespace *walk = mNamespaceList; walk; walk = walk->mNext)
      if(walk->mName == name && walk->mPackage == package)
         return walk;

   Namespace *ret = (Namespace *) mAllocator.alloc(sizeof(Namespace));
   constructInPlace(ret);
   ret->mPackage = package;
   ret->mName = name;
   ret->mNext = mNamespaceList;
   mNamespaceList = ret;
   return ret;
}

bool Namespace::unlinkClass(Namespace *parent)
{
   Namespace *walk = this;
   while(walk->mParent && walk->mParent->mName == mName)
      walk = walk->mParent;

   if(walk->mParent && walk->mParent != parent)
   {
      Con::errorf(ConsoleLogEntry::General, "Namespace::unlinkClass - cannot unlink namespace parent linkage for %s for %s.",
         walk->mName, walk->mParent->mName);
      return false;
   }

   mRefCountToParent--;
   AssertFatal(mRefCountToParent >= 0, "Namespace::unlinkClass - reference count to parent is less than 0");

   if(mRefCountToParent == 0)
      walk->mParent = NULL;

   trashCache();

   return true;
}


bool Namespace::classLinkTo(Namespace *parent)
{
   Namespace *walk = this;
   while(walk->mParent && walk->mParent->mName == mName)
      walk = walk->mParent;

   if(walk->mParent && walk->mParent != parent)
   {
      Con::errorf(ConsoleLogEntry::General, "Error: cannot change namespace parent linkage of %s from %s to %s.",
         walk->mName, walk->mParent->mName, parent->mName);
      return false;
   }
   mRefCountToParent++;
   walk->mParent = parent;

   trashCache();

   return true;
}

void Namespace::buildHashTable()
{
   if(mHashSequence == mCacheSequence)
      return;

   if(!mEntryList && mParent)
   {
      mParent->buildHashTable();
      mHashTable = mParent->mHashTable;
      mHashSize = mParent->mHashSize;
      mHashSequence = mCacheSequence;
      return;
   }

   U32 entryCount = 0;
   Namespace * ns;
   for(ns = this; ns; ns = ns->mParent)
      for(Entry *walk = ns->mEntryList; walk; walk = walk->mNext)
         if(lookupRecursive(walk->mFunctionName) == walk)
            entryCount++;

   mHashSize = entryCount + (entryCount >> 1) + 1;

   if(!(mHashSize & 1))
      mHashSize++;

   mHashTable = (Entry **) mCacheAllocator.alloc(sizeof(Entry *) * mHashSize);
   for(U32 i = 0; i < mHashSize; i++)
      mHashTable[i] = NULL;

   for(ns = this; ns; ns = ns->mParent)
   {
      for(Entry *walk = ns->mEntryList; walk; walk = walk->mNext)
      {
         U32 index = HashPointer(walk->mFunctionName) % mHashSize;
         while(mHashTable[index] && mHashTable[index]->mFunctionName != walk->mFunctionName)
         {
            index++;
            if(index >= mHashSize)
               index = 0;
         }

         if(!mHashTable[index])
            mHashTable[index] = walk;
      }
   }

   mHashSequence = mCacheSequence;
}

void Namespace::init()
{
   // create the global namespace
   mGlobalNamespace = find(NULL);
}

Namespace *Namespace::global()
{
   return mGlobalNamespace;
}

void Namespace::shutdown()
{
   for(Namespace *walk = mNamespaceList; walk; walk = walk->mNext)
      walk->clearEntries();
}

void Namespace::trashCache()
{
   mCacheSequence++;
   mCacheAllocator.freeBlocks();
}

const char *Namespace::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   if(mHashSequence != mCacheSequence)
      buildHashTable();

   const char *bestMatch = NULL;
   for(U32 i = 0; i < mHashSize; i++)
      if(mHashTable[i] && canTabComplete(prevText, bestMatch, mHashTable[i]->mFunctionName, baseLen, fForward))
         bestMatch = mHashTable[i]->mFunctionName;
   return bestMatch;
}

Namespace::Entry *Namespace::lookupRecursive(StringTableEntry name)
{
   for(Namespace *ns = this; ns; ns = ns->mParent)
      for(Entry *walk = ns->mEntryList; walk; walk = walk->mNext)
         if(walk->mFunctionName == name)
            return walk;

   return NULL;
}

Namespace::Entry *Namespace::lookup(StringTableEntry name)
{
   if(mHashSequence != mCacheSequence)
      buildHashTable();

   U32 index = HashPointer(name) % mHashSize;
   while(mHashTable[index] && mHashTable[index]->mFunctionName != name)
   {
      index++;
      if(index >= mHashSize)
         index = 0;
   }
   return mHashTable[index];
}

static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   const Namespace::Entry* fa = *((Namespace::Entry**)a);
   const Namespace::Entry* fb = *((Namespace::Entry**)b);

   return dStricmp(fa->mFunctionName, fb->mFunctionName);
}

void Namespace::getEntryList(Vector<Entry *> *vec)
{
   if(mHashSequence != mCacheSequence)
      buildHashTable();

   for(U32 i = 0; i < mHashSize; i++)
      if(mHashTable[i])
         vec->push_back(mHashTable[i]);

   dQsort(vec->address(),vec->size(),sizeof(Namespace::Entry *),compareEntries);
}

Namespace::Entry *Namespace::createLocalEntry(StringTableEntry name)
{
   for(Entry *walk = mEntryList; walk; walk = walk->mNext)
   {
      if(walk->mFunctionName == name)
      {
         walk->clear();
         return walk;
      }
   }

   Entry *ent = (Entry *) mAllocator.alloc(sizeof(Entry));
   constructInPlace(ent);

   ent->mNamespace = this;
   ent->mFunctionName = name;
   ent->mNext = mEntryList;
   ent->mPackage = mPackage;
   ent->mToolOnly = false;
   mEntryList = ent;
   return ent;
}

void Namespace::addFunction(StringTableEntry name, CodeBlock *cb, U32 functionOffset, const char* usage)
{
   Entry *ent = createLocalEntry(name);
   trashCache();

   ent->mUsage = usage;
   ent->mCode = cb;
   ent->mFunctionOffset = functionOffset;
   ent->mCode->incRefCount();
   ent->mType = Entry::ScriptFunctionType;
}

void Namespace::addCommand(StringTableEntry name,StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);
   trashCache();

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::StringCallbackType;
   ent->cb.mStringCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);
   trashCache();

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::IntCallbackType;
   ent->cb.mIntCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);
   trashCache();

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::VoidCallbackType;
   ent->cb.mVoidCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);
   trashCache();

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::FloatCallbackType;
   ent->cb.mFloatCallbackFunc = cb;
}

void Namespace::addCommand(StringTableEntry name,BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs, bool isToolOnly /* = false */)
{
   Entry *ent = createLocalEntry(name);
   trashCache();

   ent->mUsage = usage;
   ent->mMinArgs = minArgs;
   ent->mMaxArgs = maxArgs;
   ent->mToolOnly = isToolOnly;

   ent->mType = Entry::BoolCallbackType;
   ent->cb.mBoolCallbackFunc = cb;
}

void Namespace::addOverload(const char * name, const char *altUsage)
{
   static U32 uid=0;
   char buffer[1024];
   char lilBuffer[32];
   dStrcpy(buffer, name);
   dSprintf(lilBuffer, 32, "_%d", uid++);
   dStrcat(buffer, lilBuffer);

   Entry *ent = createLocalEntry(StringTable->insert( buffer ));
   trashCache();

   ent->mUsage = altUsage;
   ent->mMinArgs = -1;
   ent->mMaxArgs = -2;

   ent->mType = Entry::OverloadMarker;
   ent->cb.mGroupName = name;
}

void Namespace::markGroup(const char* name, const char* usage)
{
   static U32 uid=0;
   char buffer[1024];
   char lilBuffer[32];
   dStrcpy(buffer, name);
   dSprintf(lilBuffer, 32, "_%d", uid++);
   dStrcat(buffer, lilBuffer);

   Entry *ent = createLocalEntry(StringTable->insert( buffer ));
   trashCache();

   if(usage != NULL)
      lastUsage = (char*)(ent->mUsage = usage);
   else
      ent->mUsage = lastUsage;

   ent->mMinArgs = -1; // Make sure it explodes if somehow we run this entry.
   ent->mMaxArgs = -2;

   ent->mType = Entry::GroupMarker;
   ent->cb.mGroupName = name;
}

extern S32 executeBlock(StmtNode *block, ExprEvalState *state);

const char *Namespace::Entry::execute(S32 argc, const char **argv, ExprEvalState *state)
{
	if(mType == ScriptFunctionType)
	{
		if(mFunctionOffset) {
			recursionCount++;
			//pass in recursionCount, the number of times this entry has been called
			const char * ret = mCode->exec(mFunctionOffset, argv[0], mNamespace, argc, argv, false, mPackage, recursionCount );
			recursionCount--;

			//Luma:	We need to return the return value here, but it must be in a static variable to remain valid
			// 1.4.1 - The return buffer was previously set to 32, which was reported as a problem
			// in the forums. Increased the size, switched to dStrncpy, and provided a rather harsh "warning"
		    static char returnBuffer2[4096];
			AssertFatal( dStrlen(ret) < sizeof(returnBuffer2), "Namespace::Entry::execute -> Size of returned string too large for return buffer" );
			dStrncpy(returnBuffer2, ret, (sizeof(returnBuffer2)-1));
			return returnBuffer2;
		}
		else
			return "";
	}

#ifndef TORQUE_DEBUG
   // [tom, 12/13/2006] This stops tools functions from working in the console,
   // which is useful behavior when debugging so I'm ifdefing this out for debug builds.
   if(mToolOnly && ! Con::isCurrentScriptToolScript())
   {
      Con::errorf(ConsoleLogEntry::Script, "%s::%s - attempting to call tools only function from outside of tools", mNamespace->mName, mFunctionName);
      return "";
   }
#endif

   if((mMinArgs && argc < mMinArgs) || (mMaxArgs && argc > mMaxArgs))
   {
      Con::warnf(ConsoleLogEntry::Script, "%s::%s - wrong number of arguments.", mNamespace->mName, mFunctionName);
      Con::warnf(ConsoleLogEntry::Script, "usage: %s", mUsage);
      return "";
   }

   static char returnBuffer[32];
   switch(mType)
   {
      case StringCallbackType:
         return cb.mStringCallbackFunc(state->thisObject, argc, argv);
      case IntCallbackType:
         dSprintf(returnBuffer, sizeof(returnBuffer), "%d",
            cb.mIntCallbackFunc(state->thisObject, argc, argv));
         return returnBuffer;
      case FloatCallbackType:
         dSprintf(returnBuffer, sizeof(returnBuffer), "%g",
            cb.mFloatCallbackFunc(state->thisObject, argc, argv));
         return returnBuffer;
      case VoidCallbackType:
         cb.mVoidCallbackFunc(state->thisObject, argc, argv);
         return "";
      case BoolCallbackType:
         dSprintf(returnBuffer, sizeof(returnBuffer), "%d",
            (U32)cb.mBoolCallbackFunc(state->thisObject, argc, argv));
         return returnBuffer;
   }

   return "";
}


StringTableEntry Namespace::mActivePackages[Namespace::MaxActivePackages];
U32 Namespace::mNumActivePackages = 0;
U32 Namespace::mOldNumActivePackages = 0;

bool Namespace::isPackage(StringTableEntry name)
{
   for(Namespace *walk = mNamespaceList; walk; walk = walk->mNext)
      if(walk->mPackage == name)
         return true;
   return false;
}

void Namespace::activatePackage(StringTableEntry name)
{
   if(mNumActivePackages == MaxActivePackages)
   {
      Con::printf("ActivatePackage(%s) failed - Max package limit reached: %d", name, MaxActivePackages);
      return;
   }
   if(!name)
      return;

   // see if this one's already active
   for(U32 i = 0; i < mNumActivePackages; i++)
      if(mActivePackages[i] == name)
         return;

   // kill the cache
   trashCache();

   // find all the package namespaces...
   for(Namespace *walk = mNamespaceList; walk; walk = walk->mNext)
   {
      if(walk->mPackage == name)
      {
         Namespace *parent = Namespace::find(walk->mName);
         // hook the parent
         walk->mParent = parent->mParent;
         parent->mParent = walk;

         // now swap the entries:
         Entry *ew;
         for(ew = parent->mEntryList; ew; ew = ew->mNext)
            ew->mNamespace = walk;

         for(ew = walk->mEntryList; ew; ew = ew->mNext)
            ew->mNamespace = parent;

         ew = walk->mEntryList;
         walk->mEntryList = parent->mEntryList;
         parent->mEntryList = ew;
		
		//-Mat swap Dictionaries too
		 Dictionary *temp = walk->mDictionary;
		 walk->mDictionary = parent->mDictionary;
		 parent->mDictionary = temp;
		 //set this as well so the Dictionaries know who's boss
		 if( walk->mDictionary ) {
			 walk->mDictionary->scopeNamespace = walk;
		 }
		 if( parent->mDictionary ) {
			 parent->mDictionary->scopeNamespace = parent;
		 }

      }
   }
   mActivePackages[mNumActivePackages++] = name;
}

void Namespace::deactivatePackage(StringTableEntry name)
{
   S32 i, j;
   for(i = 0; i < mNumActivePackages; i++)
      if(mActivePackages[i] == name)
         break;
   if(i == mNumActivePackages)
      return;

   trashCache();

   for(j = mNumActivePackages - 1; j >= i; j--)
   {
      // gotta unlink em in reverse order...
      for(Namespace *walk = mNamespaceList; walk; walk = walk->mNext)
      {
         if(walk->mPackage == mActivePackages[j])
         {
            Namespace *parent = Namespace::find(walk->mName);
            // hook the parent
            parent->mParent = walk->mParent;
            walk->mParent = NULL;

            // now swap the entries:
            Entry *ew;
            for(ew = parent->mEntryList; ew; ew = ew->mNext)
               ew->mNamespace = walk;

            for(ew = walk->mEntryList; ew; ew = ew->mNext)
               ew->mNamespace = parent;

            ew = walk->mEntryList;
            walk->mEntryList = parent->mEntryList;
            parent->mEntryList = ew;

			//-Mat swap Dictionaries too
			Dictionary *temp = walk->mDictionary;
			walk->mDictionary = parent->mDictionary;
			parent->mDictionary = temp;
			//set this crap TOO so the Dictionaries know who's boss
			if( walk->mDictionary ) {
				walk->mDictionary->scopeNamespace = walk;
			}
			if( parent->mDictionary ) {
				parent->mDictionary->scopeNamespace = parent;
			}

         }
      }
   }
   mNumActivePackages = i;
}

void Namespace::unlinkPackages()
{
   mOldNumActivePackages = mNumActivePackages;
   if(!mNumActivePackages)
      return;
   deactivatePackage(mActivePackages[0]);
}

void Namespace::relinkPackages()
{
   if(!mOldNumActivePackages)
      return;
   for(U32 i = 0; i < mOldNumActivePackages; i++)
      activatePackage(mActivePackages[i]);
}

ConsoleFunctionGroupBegin( Packages, "Functions relating to the control of packages.");

ConsoleFunction(isPackage,bool,2,2,"( packageName ) Use the isPackage function to check if the name or ID specified in packageName is a valid package.\n"
																"@param packagename The name or ID of an existing package.\n"
																"@return Returns true if packageName is a valid package, false otherwise.\n"
																"@sa activatePackage, deactivatePackage")
{
   argc;
   StringTableEntry packageName = StringTable->insert(argv[1]);
   return Namespace::isPackage(packageName);
}

ConsoleFunction(activatePackage, void,2,2,"( packageName ) Use the activatePackage function to activate a package definition and to re-define all functions named within this package with the definitions provided in the package body.\n"
																"This pushes the newly activated package onto the top of the package stack.\n"
																"@param packagename The name or ID of an existing package.\n"
																"@return No return value.\n"
																"@sa deactivatePackage, isPackage")
{
   argc;
   StringTableEntry packageName = StringTable->insert(argv[1]);
   Namespace::activatePackage(packageName);
}

ConsoleFunction(deactivatePackage, void,2,2,"( packageName ) Use the deactivatePackage function to deactivate a package definition and to pop any definitions from this package off the package stack.\n"
																"This also causes any subsequently stacked packages to be popped. i.e. If any packages were activated after the one specified in packageName, they too will be deactivated and popped.\n"
																"@param packagename The name or ID of an existing package.\n"
																"@return No return value.\n"
																"@sa activatePackage, isPackage")
{
   argc;
   StringTableEntry packageName = StringTable->insert(argv[1]);
   Namespace::deactivatePackage(packageName);
}

ConsoleFunctionGroupEnd( Packages );



#endif //PUAP_NAMESPACE_CHANGE

