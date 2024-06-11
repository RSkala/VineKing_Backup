
#include "core/resManager.h"
#include "core/findMatch.h"
#include "console/consoleInternal.h"
#include "Dictionary.h"
#include "Namespace.h"

extern ExprEvalState gEvalState;

#define ST_INIT_SIZE 15
static char scratchBuffer[1024];

static S32 QSORT_CALLBACK varCompare(const void* a,const void* b)
{
   return dStricmp( (*((Dictionary::Entry **) a))->name, (*((Dictionary::Entry **) b))->name );
}



void Dictionary::exportVariables(const char *varString, const char *fileName, bool append)
{
   const char *searchStr = varString;
   Vector<Entry *> sortList(__FILE__, __LINE__);

   entryIterator it;
   for( it = mHashTable->begin(); it != mHashTable->end(); it++ ) {

	  Entry *walk = (it->value);
	  if(walk)
	  {
		 if(FindMatch::isMatch((char *) searchStr, (char *) walk->name))
			sortList.push_back(walk);
	   }   
   }

   if(!sortList.size())
      return;

   dQsort((void *) &sortList[0], sortList.size(), sizeof(Entry *), varCompare);

   Vector<Entry *>::iterator s;
   char expandBuffer[1024];
   FileStream strm;

   if(fileName)
   {
      if(!ResourceManager->openFileForWrite(strm, fileName, append ? FileStream::ReadWrite : FileStream::Write))
      {
         Con::errorf(ConsoleLogEntry::General, "Unable to open file '%s for writing.", fileName);
         return;
      }
      if(append)
         strm.setPosition(strm.getStreamSize());
   }

   char buffer[1024];
   const char *cat = fileName ? "\r\n" : "";

   for(s = sortList.begin(); s != sortList.end(); s++)
   {
      switch((*s)->type)
      {
         case Entry::TypeInternalInt:
            dSprintf(buffer, sizeof(buffer), "%s = %d;%s", (*s)->name, (*s)->ival, cat);
            break;
         case Entry::TypeInternalFloat:
            dSprintf(buffer, sizeof(buffer), "%s = %g;%s", (*s)->name, (*s)->fval, cat);
            break;
         default:
            expandEscape(expandBuffer, (*s)->getStringValue());
            dSprintf(buffer, sizeof(buffer), "%s = \"%s\";%s", (*s)->name, expandBuffer, cat);
            break;
      }
      if(fileName)
         strm.write(dStrlen(buffer), buffer);
      else
         Con::printf("%s", buffer);
   }
   if(fileName)
      strm.close();
}

void Dictionary::deleteVariables(const char *varString)
{
   const char *searchStr = varString;

   for(S32 i = 0; i < mHashTable->size(); i++)
   {
      Entry *walk = (mHashTable->find( searchStr )->value);
      while(walk)
      {
         Entry *matchedEntry = (FindMatch::isMatch((char *) searchStr, (char *) walk->name)) ? walk : NULL;
         walk = walk->nextEntry;
         if (matchedEntry)
            remove(matchedEntry); // assumes remove() is a stable remove (will not reorder entries on remove)
	   }   
   }
}



Dictionary::Entry *Dictionary::lookup(StringTableEntry name, StringTableEntry parentName) {
#ifdef PUAP_SCRIPT_CHANGE
	//-Mat if we have no parent name, use the most recent one
	if( !parentName && name[0] != '$' ) {//only for locals
		parentName = getCurrentNamespaceName();
	}
#endif// PUAP_SCRIPT_CHANGE
	Entry *ret = lookupLocal( name, parentName );
	
	if( !ret && scopeNamespace ) {
		Dictionary *curDictionary = NULL;
		Namespace *curNamespace = scopeNamespace->mParent;
		while( curNamespace && !ret ) {
			curDictionary = curNamespace->mDictionary;
			if( curDictionary ) {
				ret = curDictionary->lookupLocal( name, parentName );
			}
			curNamespace = curNamespace->mParent;
		}
	}

	return ret;
}


Dictionary::Entry *Dictionary::lookupLocal(StringTableEntry name, StringTableEntry parentName)
{
   Entry *pEntry = NULL;
   entryIterator it = mHashTable->find( name );

#ifdef PUAP_SCRIPT_CHANGE
   /*
	   -Mat if we have a parent and it matches the entry's, return it. if it doesn't match, keep going until we find one that does
	   or has no parent. if an entry has no parent, that it is "shared" by all functions in this namespace, including us
	   so if we are at the first one and it doesn't match, iterate though all that we have until we find a suitable one
	   Globals will have the parentName of "GlobalVar"
   */
   StringTableEntry globalParentName = StringTable->insert( "GlobalVar" );
#endif //PUAP_SCRIPT_CHANGE

   while( it.getValue() ) {
	   pEntry = it.getValue();

#ifdef PUAP_SCRIPT_CHANGE
	   //-Mat first check for a global variable
		if( parentName == globalParentName ) {
			if( pEntry->name == name ) {
				//global variable have no parent, so we give them all this name for looking up
				return pEntry;
			}
		}
#endif //PUAP_SCRIPT_CHANGE

	   if( pEntry && pEntry->parentName != NULL  ) {
		   if( pEntry->name == name && pEntry->parentName == parentName ) {
			   return pEntry;
		   } else {
			   //get the next one
			   it++;
		   }
	   } else {
		   //if there is no parent name, we belong to this Dictionary's Namespace, not any function
		   //so if we are part of this dictionary without a parent we're OK to return
		   //special case for % variable outside of functions
		   if( pEntry->name == name ) {
			   return pEntry;
		   } else {
			   return NULL;
		   }
	   }
   }

   return NULL;
}

Dictionary::Entry *Dictionary::add(StringTableEntry name,StringTableEntry parentName, bool lookupFirst/* = true */)
{
   Entry *ret = NULL;
   if( lookupFirst ) {
	   ret = lookup( name, parentName );
   }
   if( ret ) {
	   return ret;//see if we already have it before creating it
   }
   ret = new Entry(name,parentName);
   ret->mOwner = this;//set owning Dictionary
   //we may have multiple variables with the same name, the parent will determine which we want, so allow duplicates
   mHashTable->insertEqual( ret->name, ret );

   return ret;
}

// deleteVariables() assumes remove() is a stable remove (will not reorder entries on remove)
void Dictionary::remove(Dictionary::Entry *ent)
{
	mHashTable->erase( ent->name );
}

Dictionary::Dictionary()
   :  exprState( NULL ),
      scopeName( NULL ),
      scopeNamespace( NULL ),
      code( NULL ),
      ip( 0 )
{
	mHashTable = new tHashTable<StringTableEntry,Dictionary::Entry*>;
	recursiveDictionary = false;
}

Dictionary::Dictionary(ExprEvalState *state, Dictionary* ref)
   :  exprState( NULL ),
      scopeName( NULL ),
      scopeNamespace( NULL ),
      code( NULL ),
      ip( 0 )
{
	mHashTable = new tHashTable<StringTableEntry,Dictionary::Entry*>;
   setState(state,ref);
   recursiveDictionary = false;
}

void Dictionary::setState(ExprEvalState *state, Dictionary* ref)
{
   exprState = state;

   //copy hashTable from master
   if (ref)
      mHashTable = ref->mHashTable;
   else
   {
	   //make a new one, but no memory leaks
	   if( mHashTable ) {
		   delete mHashTable;
	   }
	   mHashTable = new tHashTable<StringTableEntry,Dictionary::Entry*>;
   }
}

Dictionary::~Dictionary()
{
#ifdef PUAP_SCRIPT_CHANGE
   delete mHashTable;
#endif
}

void Dictionary::reset()
{
   //make a new one, but no memory leaks
   if( mHashTable ) {
	   delete mHashTable;
   }
   mHashTable = new tHashTable<StringTableEntry,Dictionary::Entry*>;
}




bool canTabComplete(const char *prevText, const char *bestMatch,
               const char *newText, S32 baseLen, bool fForward)
{
   // test if it matches the first baseLen chars:
   if(dStrnicmp(newText, prevText, baseLen))
      return false;

   if (fForward)
   {
      if(!bestMatch)
         return dStricmp(newText, prevText) > 0;
      else
         return (dStricmp(newText, prevText) > 0) &&
                (dStricmp(newText, bestMatch) < 0);
   }
   else
   {
      if (dStrlen(prevText) == (U32) baseLen)
      {
         // look for the 'worst match'
         if(!bestMatch)
            return dStricmp(newText, prevText) > 0;
         else
            return dStricmp(newText, bestMatch) > 0;
      }
      else
      {
         if (!bestMatch)
            return (dStricmp(newText, prevText)  < 0);
         else
            return (dStricmp(newText, prevText)  < 0) &&
                   (dStricmp(newText, bestMatch) > 0);
      }
   }
}

const char *Dictionary::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   // UNUSED: JOSEPH THOMAS -> S32 i;
   const char *bestMatch = NULL;
   // UNUSED: JOSEPH THOMAS -> U32 size = mHashTable->size();
   Entry *pEntry = NULL;
   //-Mat run through all elements from start to finish
   entryIterator it;
   for( it = mHashTable->begin(); it != mHashTable->end(); it++ ) {
	  pEntry = (it->value);
	   if( pEntry ) {
         if(canTabComplete(prevText, bestMatch, pEntry->name, baseLen, fForward))
            bestMatch = pEntry->name;
	   }
   }

   return bestMatch;
}

char *typeValueEmpty = "";

Dictionary::Entry::Entry(StringTableEntry in_name,StringTableEntry in_parentName)
{
   dataPtr = NULL;
   nextEntry = NULL;
   name = in_name;
   parentName = in_parentName;
   mOwner = NULL;
   ival = 0;
   fval = 0;
   sval = typeValueEmpty;
   //-Mat important, for uninitializd variable, setting this means that their defualt value is "", not 0
   type = TypeInternalString;//be a string by default
   bufferLen = 0;
   nextEntry = NULL;
}

Dictionary::Entry::~Entry()
{
   if(sval != typeValueEmpty)
      dFree(sval);
}

const char *Dictionary::getVariable(StringTableEntry name, bool *entValid)
{
   Entry *ent = lookup(name);
   if(ent)
   {
      if(entValid)
         *entValid = true;
      return ent->getStringValue();
   }
   if(entValid)
      *entValid = false;

   // Warn users when they access a variable that isn't defined.
   if(gWarnUndefinedScriptVariables)
      Con::warnf(" *** Accessed undefined variable '%s'", name);

   return "";
}

void Dictionary::Entry::setStringValue(const char * value)
{
   if(type <= TypeInternalString)
   {
      // Let's not remove empty-string-valued global vars from the dict.
      // If we remove them, then they won't be exported, and sometimes
      // it could be necessary to export such a global.  There are very
      // few empty-string global vars so there's no performance-related
      // need to remove them from the dict.
/*
      if(!value[0] && name[0] == '$')
      {
         gEvalState.globalVars.remove(this);
         return;
      }
*/

      U32 stringLen = dStrlen(value);

      // If it's longer than 256 bytes, it's certainly not a number.
      //
      // (This decision may come back to haunt you. Shame on you if it
      // does.)
      if(stringLen < 256)
      {
         fval = dAtof(value);
         ival = dAtoi(value);
      }
      else
      {
         fval = 0.f;
         ival = 0;
      }

      type = TypeInternalString;

      // may as well pad to the next cache line
      U32 newLen = ((stringLen + 1) + 15) & ~15;
      
      if(sval == typeValueEmpty)
         sval = (char *) dMalloc(newLen);
      else if(newLen > bufferLen)
         sval = (char *) dRealloc(sval, newLen);

      bufferLen = newLen;
      dStrcpy(sval, value);
   }
   else
      Con::setData(type, dataPtr, 0, 1, &value);
}

void Dictionary::setVariable(StringTableEntry name, const char *value)
{
   Entry *ent = add(name);
   if(!value)
      value = "";
   ent->setStringValue(value);
}

void Dictionary::addVariable(const char *name, S32 type, void *dataPtr)
{
   if(name[0] != '$')
   {
      scratchBuffer[0] = '$';
      dStrcpy(scratchBuffer + 1, name);
      name = scratchBuffer;
   }
   Entry *ent = add(StringTable->insert(name));
   ent->type = type;
   if(ent->sval != typeValueEmpty)
   {
      dFree(ent->sval);
      ent->sval = typeValueEmpty;
   }
   ent->dataPtr = dataPtr;
}

bool Dictionary::removeVariable(StringTableEntry name)
{
   if( Entry *ent = lookup(name) ){
      remove( ent );
      return true;
   }
   return false;
}
