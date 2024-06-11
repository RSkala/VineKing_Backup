//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/simBase.h"
#include "core/stringTable.h"
#include "console/console.h"
#include "core/fileStream.h"
#include "sim/actionMap.h"
#include "core/resManager.h"
#include "core/fileObject.h"
#include "console/consoleInternal.h"
#include "platform/profiler.h"
#include "console/typeValidators.h"
#include "core/frameAllocator.h"

#ifdef PUAP_NAMESPACE_CHANGE
#else
extern ExprEvalState gEvalState;
#endif //PUAP_NAMESPACE_CHANGE
namespace Sim
{
   // Don't forget to InstantiateNamed* in simManager.cc - DMM
   ImplementNamedSet(ActiveActionMapSet)
   ImplementNamedSet(GhostAlwaysSet)
   ImplementNamedSet(LightSet)
   ImplementNamedSet(WayPointSet)
   ImplementNamedSet(fxReplicatorSet)
   ImplementNamedSet(fxFoliageSet)
   ImplementNamedSet(BehaviorSet)
   ImplementNamedGroup(ActionMapGroup)
   ImplementNamedGroup(ClientGroup)
   ImplementNamedGroup(GuiGroup)
   ImplementNamedGroup(GuiDataGroup)
   ImplementNamedGroup(TCPGroup)

   //groups created on the client
   ImplementNamedGroup(ClientConnectionGroup)
   ImplementNamedGroup(ChunkFileGroup)
}   

//---------------------------------------------------------------------------

void SimObjectList::pushBack(SimObject* obj)
{
   if (find(begin(),end(),obj) == end())
      push_back(obj);
}	

void SimObjectList::pushBackForce(SimObject* obj)
{
   iterator itr = find(begin(),end(),obj);
   if (itr == end()) 
   {
      push_back(obj);
   }
   else 
   {
      // Move to the back...
      //
      SimObject* pBack = *itr;
      removeStable(pBack);
      push_back(pBack);
   }
}	

void SimObjectList::pushFront(SimObject* obj)
{
   if (find(begin(),end(),obj) == end())
      push_front(obj);
}	

void SimObjectList::remove(SimObject* obj)
{
   iterator ptr = find(begin(),end(),obj);
   if (ptr != end()) 
      erase(ptr);
}

void SimObjectList::removeStable(SimObject* obj)
{
   iterator ptr = find(begin(),end(),obj);
   if (ptr != end()) 
      erase(ptr);
}

S32 QSORT_CALLBACK SimObjectList::compareId(const void* a,const void* b)
{
   return (*reinterpret_cast<const SimObject* const*>(a))->getId() -
      (*reinterpret_cast<const SimObject* const*>(b))->getId();
}

void SimObjectList::sortId()
{
   dQsort(address(),size(),sizeof(value_type),compareId);
}	


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

SimFieldDictionary::Entry *SimFieldDictionary::mFreeList = NULL;

static Chunker<SimFieldDictionary::Entry> fieldChunker;

SimFieldDictionary::Entry *SimFieldDictionary::allocEntry()
{
   if(mFreeList)
   {
      Entry *ret = mFreeList;
      mFreeList = ret->next;
      return ret;
   }
   else
      return fieldChunker.alloc();
}

void SimFieldDictionary::freeEntry(SimFieldDictionary::Entry *ent)
{
   ent->next = mFreeList;
   mFreeList = ent;
}

SimFieldDictionary::SimFieldDictionary()
{
   for(U32 i = 0; i < HashTableSize; i++)
      mHashTable[i] = 0;

   mVersion = 0;
}

SimFieldDictionary::~SimFieldDictionary()
{
   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i]; walk;)
      {
         Entry *temp = walk;
         walk = temp->next;

         dFree(temp->value);
         freeEntry(temp);
      }
   }
}

void SimFieldDictionary::setFieldValue(StringTableEntry slotName, const char *value)
{
   U32 bucket = HashPointer(slotName) % HashTableSize;
   Entry **walk = &mHashTable[bucket];
   while(*walk && (*walk)->slotName != slotName)
      walk = &((*walk)->next);

   Entry *field = *walk;
   if(!*value)
   {
      if(field)
      {
         mVersion++;

         dFree(field->value);
         *walk = field->next;
         freeEntry(field);
      }
   }
   else
   {
      if(field)
      {
         dFree(field->value);
         field->value = dStrdup(value);
      }
      else
      {
         mVersion++;

         field = allocEntry();
         field->value = dStrdup(value);
         field->slotName = slotName;
         field->next = NULL;
         *walk = field;
      }
   }
}

const char *SimFieldDictionary::getFieldValue(StringTableEntry slotName)
{
   U32 bucket = HashPointer(slotName) % HashTableSize;

   for(Entry *walk = mHashTable[bucket];walk;walk = walk->next)
      if(walk->slotName == slotName)
         return walk->value;

   return NULL;
}

//---------------------------------------------------------------------------

SimObject::SimObject( const U8 namespaceLinkMask ) : mNSLinkMask( namespaceLinkMask )
{
   objectName            = NULL;
   mInternalName          = NULL;
   nextNameObject        = (SimObject*)-1;
   nextManagerNameObject = (SimObject*)-1;
   nextIdObject          = NULL;

   mId           = 0;
   mGroup        = 0;
   mNameSpace    = NULL;
   mNotifyList   = NULL;
   mFlags.set( ModStaticFields | ModDynamicFields );
   mTypeMask             = 0;

   mFieldDictionary = NULL;
   mCanSaveFieldDictionary	=	true;

   mClassName = NULL;
   mSuperClassName = NULL;
}

void SimFieldDictionary::assignFrom(SimFieldDictionary *dict)
{
   mVersion++;

   for(U32 i = 0; i < HashTableSize; i++)
      for(Entry *walk = dict->mHashTable[i];walk; walk = walk->next)
         setFieldValue(walk->slotName, walk->value);
}

static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   SimFieldDictionary::Entry *fa = *((SimFieldDictionary::Entry **)a);
   SimFieldDictionary::Entry *fb = *((SimFieldDictionary::Entry **)b);
   return dStricmp(fa->slotName, fb->slotName);
}

void SimFieldDictionary::writeFields(SimObject *obj, Stream &stream, U32 tabStop)
{

   const AbstractClassRep::FieldList &list = obj->getFieldList();
   Vector<Entry *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i];walk; walk = walk->next)
      {
         // make sure we haven't written this out yet:
         U32 i;
         for(i = 0; i < list.size(); i++)
            if(list[i].pFieldname == walk->slotName)
               break;

         if(i != list.size())
            continue;


         if (!obj->writeField(walk->slotName, walk->value))
            continue;

         flist.push_back(walk);
      }
   }

   // Sort Entries to prevent version control conflicts
   dQsort(flist.address(),flist.size(),sizeof(Entry *),compareEntries);

   // Save them out
   for(Vector<Entry *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      U32 nBufferSize = (dStrlen( (*itr)->value ) * 2) + dStrlen( (*itr)->slotName ) + 16;
      FrameTemp<char> expandedBuffer( nBufferSize );

      stream.writeTabs(tabStop+1);

      dSprintf(expandedBuffer, nBufferSize, "%s = \"", (*itr)->slotName);
      expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), (*itr)->value);
      dStrcat(expandedBuffer, "\";\r\n");

      stream.write(dStrlen(expandedBuffer),expandedBuffer);
   }

}
void SimFieldDictionary::printFields(SimObject *obj)
{
   const AbstractClassRep::FieldList &list = obj->getFieldList();
   char expandedBuffer[4096];
   Vector<Entry *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i];walk; walk = walk->next)
      {
         // make sure we haven't written this out yet:
         U32 i;
         for(i = 0; i < list.size(); i++)
            if(list[i].pFieldname == walk->slotName)
               break;

         if(i != list.size())
            continue;

         flist.push_back(walk);
      }
   }
   dQsort(flist.address(),flist.size(),sizeof(Entry *),compareEntries);

   for(Vector<Entry *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", (*itr)->slotName);
      expandEscape(expandedBuffer + dStrlen(expandedBuffer), (*itr)->value);
      Con::printf("%s\"", expandedBuffer);
   }
}

//------------------------------------------------------------------------------
SimFieldDictionaryIterator::SimFieldDictionaryIterator(SimFieldDictionary * dictionary)
{
   mDictionary = dictionary;
   mHashIndex = -1;
   mEntry = 0;
   operator++();
}

SimFieldDictionary::Entry* SimFieldDictionaryIterator::operator++()
{
   if(!mDictionary)
      return(mEntry);

   if(mEntry)
      mEntry = mEntry->next;

   while(!mEntry && (mHashIndex < (SimFieldDictionary::HashTableSize-1)))
      mEntry = mDictionary->mHashTable[++mHashIndex];

   return(mEntry);
}

SimFieldDictionary::Entry* SimFieldDictionaryIterator::operator*()
{
   return(mEntry);
}

void SimObject::assignDynamicFieldsFrom(SimObject* parent)
{
   if(parent->mFieldDictionary)
   {
      if( mFieldDictionary == NULL )
         mFieldDictionary = new SimFieldDictionary;
      mFieldDictionary->assignFrom(parent->mFieldDictionary);
   }
}

void SimObject::assignFieldsFrom(SimObject *parent)
{
   // only allow field assigns from objects of the same class:
   if(getClassRep() == parent->getClassRep())
   {
      const AbstractClassRep::FieldList &list = getFieldList();

      // copy out all the fields:
      for(U32 i = 0; i < list.size(); i++)
      {
         const AbstractClassRep::Field* f = &list[i];
         S32 lastField = f->elementCount - 1;
         for(S32 j = 0; j <= lastField; j++)
         {
             const char* fieldVal = (*f->getDataFn)( this,  Con::getData(f->type, (void *) (((const char *)parent) + f->offset), j, f->table, f->flag));
            //if(fieldVal)
            //   Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            if(fieldVal)
            {
               // code copied from SimObject::setDataField().
               // TODO: paxorr: abstract this into a better setData / getData that considers prot fields.
               FrameTemp<char> buffer(2048);
               FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data 
               ConsoleBaseType *cbt = ConsoleBaseType::getType( f->type );
               const char* szBuffer = cbt->prepData( fieldVal, buffer, 2048 );
               dMemset( bufferSecure, 0, 2048 );
               dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

               if((*f->setDataFn)( this, bufferSecure ) )
                  Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            }
         }
      }
   }

   assignDynamicFieldsFrom(parent);
}

bool SimObject::writeField(StringTableEntry fieldname, const char* value)
{
   // Don't write empty fields.
   if (!value || !*value)
      return false;

   // Don't write ParentGroup
   if( fieldname == StringTable->insert("parentGroup") )
      return false;


   return true;
}

void SimObject::writeFields(Stream &stream, U32 tabStop)
{
   const AbstractClassRep::FieldList &list = getFieldList();
   // UNUSED: JOSEPH THOMAS -> const char *docRoot = Con::getVariable("$DocRoot");
   // UNUSED: JOSEPH THOMAS -> const char *modRoot = Con::getVariable("$ModRoot");
   // UNUSED: JOSEPH THOMAS -> S32 docRootLen = dStrlen(docRoot);
   // UNUSED: JOSEPH THOMAS -> S32 modRootLen = dStrlen(modRoot);

   for(U32 i = 0; i < list.size(); i++)
   {
      const AbstractClassRep::Field* f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
		  f->type == AbstractClassRep::StartGroupFieldType ||
		  f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         char array[8];
         dSprintf( array, 8, "%d", j );
         const char *val = getDataField(StringTable->insert( f->pFieldname ), array );

         // Make a copy for the field check.
         if (!val)
            continue;

         U32 nBufferSize = dStrlen( val ) + 1;
         FrameTemp<char> valCopy( nBufferSize );
         dStrcpy( (char *)valCopy, val );

         if (!writeField(f->pFieldname, valCopy))
            continue;

         val = valCopy;

         U32 expandedBufferSize = ( nBufferSize  * 2 ) + 32;
         FrameTemp<char> expandedBuffer( expandedBufferSize );
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, expandedBufferSize, "%s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, expandedBufferSize, "%s[%d] = \"", f->pFieldname, j);

         // detect and collapse relative path information
         char fnBuf[1024];
         if (f->type == TypeFilename)
         {
            Con::collapseScriptFilename(fnBuf, 1024, val);
            val = fnBuf;
         }

         expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), val);
         dStrcat(expandedBuffer, "\";\r\n");

         stream.writeTabs(tabStop);
         stream.write(dStrlen(expandedBuffer),expandedBuffer);
      }
   }
   if(mFieldDictionary && mCanSaveFieldDictionary)
      mFieldDictionary->writeFields(this, stream, tabStop);
}

void SimObject::write(Stream &stream, U32 tabStop, U32 flags)
{
   // Only output selected objects if they want that.
   if((flags & SelectedOnly) && !isSelected())
      return;

   stream.writeTabs(tabStop);
   char buffer[1024];
   dSprintf(buffer, sizeof(buffer), "new %s(%s) {\r\n", getClassName(), getName() ? getName() : "");
   stream.write(dStrlen(buffer), buffer);
   writeFields(stream, tabStop + 1);
   stream.writeTabs(tabStop);
   stream.write(4, "};\r\n");
}

bool SimObject::save(const char* pcFileName, bool bOnlySelected)
{
   static const char *beginMessage = "//--- OBJECT WRITE BEGIN ---";
   static const char *endMessage = "//--- OBJECT WRITE END ---";
   FileStream stream;
   FileObject f;
   f.readMemory(pcFileName);

   // check for flags <selected, ...>
   U32 writeFlags = 0;
   if(bOnlySelected)
      writeFlags |= SimObject::SelectedOnly;

   if(!ResourceManager->openFileForWrite(stream, pcFileName)) 
      return false;

   char docRoot[256];
   char modRoot[256];

   dStrcpy(docRoot, pcFileName);
   char *p = dStrrchr(docRoot, '/');
   if (p) *++p = '\0';
   else  docRoot[0] = '\0';

   dStrcpy(modRoot, pcFileName);
   p = dStrchr(modRoot, '/');
   if (p) *++p = '\0';
   else  modRoot[0] = '\0';

   Con::setVariable("$DocRoot", docRoot);
   Con::setVariable("$ModRoot", modRoot);

   const char *buffer;
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, beginMessage))
         break;
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }
   stream.write(dStrlen(beginMessage), beginMessage);
   stream.write(2, "\r\n");
   write(stream, 0, writeFlags);
   stream.write(dStrlen(endMessage), endMessage);
   stream.write(2, "\r\n");
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, endMessage))
         break;
   }
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }

   Con::setVariable("$DocRoot", NULL);
   Con::setVariable("$ModRoot", NULL);

   return true;

}

ConsoleFunctionGroupBegin ( SimFunctions, "Functions relating to Sim.");

ConsoleFunction(nameToID, S32, 2, 2, "( objectName ) Use the nameToID function to convert an object name into an object ID.\n"
																"This function is a helper for those odd cases where a string will not covert properly, but generally this can be replaced with a statement like: (\"someName\")\n"
																"@param objectName A string containing the name of an object.\n"
																"@return Returns a positive non-zero value if the name corresponds to an object, or a -1 if it does not.")
{
   argc;
   SimObject *obj = Sim::findObject(argv[1]);
   if(obj)
      return obj->getId();
   else
      return -1;
}

ConsoleFunction(isObject, bool, 2, 2, "( handle ) Use the isObject function to check if the name or ID specified in handle is a valid object.\n"
																"@param handle A name or ID of a possible object.\n"
																"@return Returns true if handle refers to a valid object, false otherwise")
{
   argc;
   if (!dStrcmp(argv[1], "0") || !dStrcmp(argv[1], ""))
      return false;
   else
      return (Sim::findObject(argv[1]) != NULL);
}

ConsoleFunction(cancel,void,2,2,"( eventID ) Use the cancel function to cancel a previously scheduled event as specified by eventID.\n"
																"@param eventID The numeric ID of a previously scheduled event.\n"
																"@return No return value.\n"
																"@sa getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule")
{
   argc;
   Sim::cancelEvent(dAtoi(argv[1]));
}

ConsoleFunction(isEventPending, bool, 2, 2, "( eventID ) Use the isEventPending function to see if the event associated with eventID is still pending.\n"
																"When an event passes, the eventID is removed from the event queue, becoming invalid, so there is no discnerable difference between a completed event and a bad event ID.\n"
																"@param eventID The numeric ID of a previously scheduled event.\n"
																"@return Returns true if this event is still outstanding and false if it has passed or eventID is invalid.\n"
																"@sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, schedule, obj.schedule")
{
   argc;
   return Sim::isEventPending(dAtoi(argv[1]));
}

ConsoleFunction(getEventTimeLeft, S32, 2, 2, "( eventID ) Use the getEventTimeLeft function to determine how much time remains until the event specified by eventID occurs.\n"
																"@param eventID The numeric ID of a previously scheduled event.\n"
																"@return Returns a non-zero integer value equal to the milliseconds until the event specified by eventID will occur. However, if eventID is invalid, or the event has passed, this function will return zero.\n"
																"@sa cancel, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule")
{
   return Sim::getEventTimeLeft(dAtoi(argv[1]));
}

ConsoleFunction(getScheduleDuration, S32, 2, 2, " ( eventID ) Use the getScheduleDuration function to determine how long the event associated with eventID was scheduled for.\n"
																"@param eventID The numeric ID of a previously scheduled event.\n"
																"@return Returns a non-zero integer value equal to the milliseconds used in the schedule call that created this event. However, if eventID is invalid, this function will return zero.\n"
																"@sa cancel, getEventTimeLeft, getTimeSinceStart, isEventPending, schedule, obj.schedule")
{
   argc;   S32 ret = Sim::getScheduleDuration(dAtoi(argv[1]));
   return ret;
}

ConsoleFunction(getTimeSinceStart, S32, 2, 2, "( eventID ) Use the getTimeSinceStart function to determine how much time has passed since the event specified by eventID was scheduled.\n"
																"@param eventID The numeric ID of a previously scheduled event.\n"
																"@return Returns a non-zero integer value equal to the milliseconds that have passed since this event was scheduled. However, if eventID is invalid, or the event has passed, this function will return zero.\n"
																"@sa cancel, getEventTimeLeft, getScheduleDuration, isEventPending, schedule, obj.schedule")
{
   argc;   S32 ret = Sim::getTimeSinceStart(dAtoi(argv[1]));
   return ret;
}


//-Mat global schedule function
U32 Sim::schedule( U32 time, SimObject *object, U32 argc, const char **argv, bool onObject/* = true*/ ) {
   if(!object)
   {
      object = Sim::getRootGroup();
	  onObject = false;
   }

   SimConsoleEvent *evt = new SimConsoleEvent(argc, argv, onObject);

   U32 ret = Sim::postEvent(object, evt, Sim::getCurrentTime() + time);
   return ret;
}


ConsoleFunction(schedule, S32, 4, 0, "( t , objID || 0 , functionName, arg0, ... , argN ) Use the schedule function to schedule functionName to be executed with optional arguments at time t (specified in milliseconds) in the future. This function may be associated with an object ID or not. If it is associated with an object ID and the object is deleted prior to this event occurring, the event is automatically canceled.\n"
																"@param t The time to wait (in milliseconds) before executing functionName.\n"
																"@param objID An optional ID to associate this event with.\n"
																"@param functionName An unadorned (flat) function name.\n"
																"@param arg0, .. , argN – Any number of optional arguments to be passed to functionName.\n"
																"@return Returns a non-zero integer representing the event ID for the scheduled event.\n"
																"@sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, obj.schedule")
{
   U32 timeDelta = U32(dAtof(argv[1]));
   SimObject *refObject = Sim::findObject(argv[2]);
   if(!refObject)
   {
      if(argv[2][0] != '0')
         return 0;

      refObject = Sim::getRootGroup();
   }
   //SimConsoleEvent *evt = new SimConsoleEvent(argc - 3, argv + 3, false);
   //S32 ret = Sim::postEvent(refObject, evt, Sim::getCurrentTime() + timeDelta);
   S32 ret = Sim::schedule( timeDelta, refObject, argc - 3, argv + 3, false );
// #ifdef DEBUG
//    Con::printf("ref %s schedule(%s) = %d", argv[2], argv[3], ret);
//    Con::executef(1, "backtrace");
// #endif
   return ret;
}

ConsoleFunctionGroupEnd( SimFunctions );

ConsoleMethod(SimObject, save, bool, 3, 4, "obj.save(fileName, [selectedOnly])")
{
   bool bSelectedOnly	=	false;
   if(argc > 3)
      bSelectedOnly	= dAtob(argv[3]);

   const char* filename = NULL;

#ifdef TORQUE_PLAYER
   char temp[1024];
   Con::expandScriptFilename(temp, sizeof(temp), argv[2]);
   filename = Platform::getPrefsPath(temp);
#else
   filename = argv[2];
#endif

   if(filename == NULL || *filename == 0)
      return false;

   return object->save(filename, bSelectedOnly);

}

ConsoleMethod(SimObject, setName, void, 3, 3, "obj.setName(newName)")
{
   argc;
   object->assignName(argv[2]);
}

ConsoleMethod(SimObject, getName, const char *, 2, 2, "obj.getName()")
{
   argc; argv;
   const char *ret = object->getName();
   return ret ? ret : "";
}

ConsoleMethod(SimObject, getClassName, const char *, 2, 2, "obj.getClassName()")
{
   argc; argv;
   const char *ret = object->getClassName();
   return ret ? ret : "";
}

ConsoleMethod(SimObject, getFieldValue, const char *, 3, 3, "obj.getFieldValue(fieldName);")
{
   argc; argv;

   const char *fieldName = StringTable->insert( argv[2] );
   return object->getDataField( fieldName, NULL );
}


ConsoleMethod(SimObject, setFieldValue, bool, 4, 4, "obj.setFieldValue(fieldName,value);")
{
   argc; argv;
   const char *fieldName = StringTable->insert(argv[2]);
   const char *value = argv[3];

   object->setDataField( fieldName, NULL, value );

   return true;

}

ConsoleMethod(SimObject, getFieldType, const char *, 3, 3, "obj.getFieldType(fieldName);")
{
   argc; argv;

   const char *fieldName = StringTable->insert( argv[2] );
   U32 typeID = object->getDataFieldType( fieldName, NULL );
   ConsoleBaseType* type = ConsoleBaseType::getType( typeID );
   
   if( type )
      return type->getTypeClassName();

   return "";
}

ConsoleMethod( SimObject, call, const char*, 2, 0, "( %args ) - Dynamically call a method on an object." )
{
   argv[1] = argv[2];
   return Con::execute( object, argc - 1, argv + 1 );
}

//-----------------------------------------------------------------------------
//	Set the internal name, can be used to find child objects
//	in a meaningful way, usually from script, while keeping
//	common script functionality together using the controls "Name" field.
//-----------------------------------------------------------------------------
ConsoleMethod( SimObject, setInternalName, void, 3, 3, "string InternalName")
{
   object->setInternalName(argv[2]);
}

void SimObject::setInternalName(const char* newname)
{
   if(newname)
      mInternalName = StringTable->insert(newname);
}

ConsoleMethod( SimObject, getInternalName, const char*, 2, 2, "getInternalName returns the objects internal name")
{
   return object->getInternalName();
}

StringTableEntry SimObject::getInternalName()
{
   return mInternalName;
}


ConsoleMethod(SimObject, dumpClassHierarchy, void, 2, 2, "obj.dumpClassHierarchy()")
{
   object->dumpClassHierarchy();
}
ConsoleMethod(SimObject, isMemberOfClass, bool, 3, 3, " isMemberOfClass(string classname) -- returns true if this object is a member of the specified class")
{

   AbstractClassRep* pRep = object->getClassRep();
   while(pRep)
   {
      if(!dStricmp(pRep->getClassName(), argv[2]))
      {
         //matches
         return true;
      }

      pRep	=	pRep->getParentClass();
   }

   return false;
}
ConsoleMethod(SimObject, getId, S32, 2, 2, "() Use the getId method to get the numeric ID of this shape.\n"
																"@return Returns the unique numeric ID of this shape.\n"
																"@sa getName, setName")
{
   argc; argv;
   return object->getId();
}

ConsoleMethod(SimObject, getGroup, S32, 2, 2, "() Use the getGroup method to determine if this object is contained in a SimGroup and if so, which one.\n"
																"@return Returns the ID of the SimGroup this shape is in or zero if the shape is not contained in a SimGroup")
{
   argc; argv;
   SimGroup *grp = object->getGroup();
   if(!grp)
      return -1;
   return grp->getId();
}

ConsoleMethod(SimObject, delete, void, 2, 2,"() Use the delete method to delete this object.\n"
																"When an object is deleted, it automatically: Unregisters its ID and name (if it has one) with the engine. Removes itself from any SimGroup or SimSet it may be a member of. (eventually) returns the memory associated with itself and its non-dynamic members. Cancels all pending %obj.schedule() events. For objects in the GameBase, ScriptObject, or GUIControl hierarchies, an object will first: Call the onRemove() method for the object's namespace\n"
																"@return No return value.")
{
   argc;argv;
   object->deleteObject();
}

ConsoleMethod(SimObject,schedule, S32, 4, 0, "(time , command , <arg1 ... argN> ) Use the schedule method to schedule an action to be executed upon this object time milliseconds in the future.\n"
																"The major difference between this and the schedule console function is that if this object is deleted prior to the scheduled event, the event is automatically canceled. times should not be treated as exact since some 'simulation delay' is to be expected. The minimum resolution for a scheduled event is \"32 ms, or one tick. schedule does not validate the existence of command. i.e. If you pass an invalid console method name, the schedule() method will still return a schedule ID, but the subsequent event will fail silently.\n"
																"@param time Time in milliseconds till action is scheduled to occur.\n"
																"@param command Name of the command to execute. This command must be scoped to this object (i.e. It must exist in the namespace of the object), otherwise the schedule call will fail.\n"
																"@param arg1...argN These are optional arguments which will be passed to command. This version of schedule automatically passes the ID of %obj as arg0 to command.\n"
																"@return Returns an integer schedule ID.\n"
																"@sa See the schedule console function and its corresponding helper functions.")
{
   U32 timeDelta = U32(dAtof(argv[2]));
   argv[2] = argv[3];
   argv[3] = argv[1];
   //SimConsoleEvent *evt = new SimConsoleEvent(argc - 2, argv + 2, true);
   //S32 ret = Sim::postEvent(object, evt, Sim::getCurrentTime() + timeDelta);
   S32 ret = Sim::schedule( timeDelta, object, argc - 2, argv + 2, true );
// #ifdef DEBUG
//    Con::printf("obj %s schedule(%s) = %d", argv[3], argv[2], ret);
//    Con::executef(1, "backtrace");
// #endif
   return ret;
}

static S32 QSORT_CALLBACK compareFields(const void* a,const void* b)
{
   const AbstractClassRep::Field* fa = *((const AbstractClassRep::Field**)a);
   const AbstractClassRep::Field* fb = *((const AbstractClassRep::Field**)b);

   return dStricmp(fa->pFieldname, fb->pFieldname);
}

ConsoleMethod(SimObject, getDynamicFieldCount, S32, 2, 2, "obj.getDynamicFieldCount()")
{
   S32 count = 0;
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
      count++;

   return count;
}

ConsoleMethod(SimObject, getDynamicField, const char*, 3, 3, "obj.getDynamicField(index)")
{
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   SimFieldDictionaryIterator itr(fieldDictionary);
   S32 index = dAtoi(argv[2]);
   for (S32 i = 0; i < index; i++)
   {
      if (!(*itr))
      {
         Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
         return NULL;
      }
      ++itr;
   }

   char* buffer = Con::getReturnBuffer(256);
   if (*itr)
   {
      SimFieldDictionary::Entry* entry = *itr;
      dSprintf(buffer, 256, "%s", entry->slotName);
      return buffer;
   }

   Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
   return NULL;
}

ConsoleMethod( SimObject, getFieldCount, S32, 2, 2, "() - Gets the number of persistent fields on the object." )
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   const AbstractClassRep::Field* f;
   U32 numDummyEntries = 0;
   for(int i = 0; i < list.size(); i++)
   {
      f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType )
      {
         numDummyEntries++;
      }
   }

   return list.size() - numDummyEntries;
}

ConsoleMethod( SimObject, getField, const char*, 3, 3, "(int index) - Gets the name of the field at the given index." )
{
   S32 index = dAtoi( argv[2] );
   const AbstractClassRep::FieldList &list = object->getFieldList();
   if( ( index < 0 ) || ( index >= list.size() ) )
      return "";

   const AbstractClassRep::Field* f;
   S32 currentField = 0;
   for(int i = 0; i < list.size() && currentField <= index; i++)
   {
      f = &list[i];
      
      // skip any dummy fields   
      if(f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType)
      {
         continue;
      }

      if(currentField == index)
         return f->pFieldname;
      
      currentField++;
   }  

   // if we found nada, return nada.
   return "";
}

ConsoleMethod(SimObject,dump, void, 2, 2, "() Use the dump method to display the following information about this object: All engine and script registered console methods (including parent methods) for this object, all Non-Dynamic Fields AND all Dynamic Fields\n"
																"@return No return value")
{
   argc; argv;
   const AbstractClassRep::FieldList &list = object->getFieldList();
   char expandedBuffer[4096];

   Con::printf("Member Fields:");
   Vector<const AbstractClassRep::Field *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < list.size(); i++)
      flist.push_back(&list[i]);

   dQsort(flist.address(),flist.size(),sizeof(AbstractClassRep::Field *),compareFields);

   for(Vector<const AbstractClassRep::Field *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      const AbstractClassRep::Field* f = *itr;
      if( f->type == AbstractClassRep::DepricatedFieldType ||
		  f->type == AbstractClassRep::StartGroupFieldType ||
		  f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         // [neo, 07/05/2007 - #3000]
         // Some objects use dummy vars and projected fields so make sure we call the get functions 
         //const char *val = Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag);                          
         const char *val = (*f->getDataFn)( object, Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag) );// + typeSizes[fld.type] * array1));

         if(!val /*|| !*val*/)
            continue;
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s[%d] = \"", f->pFieldname, j);
         expandEscape(expandedBuffer + dStrlen(expandedBuffer), val);
         Con::printf("%s\"", expandedBuffer);
      }
   }

   Con::printf("Tagged Fields:");
   if(object->getFieldDictionary())
      object->getFieldDictionary()->printFields(object);

   Con::printf("Methods:");
   Namespace *ns = object->getNamespace();
   Vector<Namespace::Entry *> vec(__FILE__, __LINE__);

   if(ns)
      ns->getEntryList(&vec);

   for(Vector<Namespace::Entry *>::iterator j = vec.begin(); j != vec.end(); j++)
      Con::printf("  %s() - %s", (*j)->mFunctionName, (*j)->mUsage ? (*j)->mUsage : "");

}

ConsoleMethod(SimObject, getType, S32, 2, 2, "() Use the getType method to get the type for this object. This type is an integer value composed of bitmasks. For simplicity, these bitmasks are defined in the engine and exposed for our use as global variables.\n"
																"To simplify the writing of scripts, a set of globals has been provided containing the bit setting for each class corresponding to a particular type. For a complete list of the bit masks, see the 'Shape Type Bitmasks' table. \n-$TypeMasks::GameBaseObjectType \n-- $TypeMasks::EnvironmentObjectType \n-- $TypeMasks::ExplosionObjectType \n-- $TypeMasks::ProjectileObjectType \n-- $TypeMasks::ShapeBaseObjectType \n--- $TypeMasks::CameraObjectType \n--- $TypeMasks::ItemObjectType \n--- $TypeMasks::MarkerObjectType \n--- $TypeMasks::PlayerObjectType \n--- $TypeMasks::StaticShapeObjectType \n--- $TypeMasks::VehicleObjectType \n-- $TypeMasks::TriggerObjectType \n- $TypeMasks::InteriorObjectType \n- $TypeMasks::StaticObjectType \n- $TypeMasks::TerrainObjectType \n- $TypeMasks::VehicleBlockerObjectType \n- $TypeMasks::WaterObjectType Two interesting general masks are: \n$TypeMasks::EnvironmentObjectType – Matches sky, sun, lightning, particle emitter nodes. \n$TypeMasks::StaticObjectType – Matches – fxFoliageReplicator, fxLight, fxShapeReplicator, fxSunlight, interiorInstance, lightning, mirrorSubObject, missionMarker, staticShape, terrain, tsStatic\n"
																"@return Returns a bit mask containing one or more set bits.\n"
																"@sa getClassName")
{
   argc; argv;
   return((S32)object->getType());
}

bool SimObject::isMethod( const char* methodName )
{
   if( !methodName || !methodName[0] )
      return false;

   StringTableEntry stname = StringTable->insert( methodName );

   if( getNamespace() )
      return ( getNamespace()->lookup( stname ) != NULL );

   return false;
}

ConsoleMethod(SimObject, isMethod, bool, 3, 3, "obj.isMethod(string method name)")
{
   return object->isMethod( argv[2] );
}


const char *SimObject::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   return mNameSpace->tabComplete(prevText, baseLen, fForward);
}

void SimObject::setDataField(StringTableEntry slotName, const char *array, const char *value)
{
   // first search the static fields if enabled
   if(mFlags.test(ModStaticFields))
   {
      const AbstractClassRep::Field *fld = findField(slotName);
      if(fld)
      {
         if( fld->type == AbstractClassRep::DepricatedFieldType ||
            fld->type == AbstractClassRep::StartGroupFieldType ||
            fld->type == AbstractClassRep::EndGroupFieldType) 
            return;

         S32 array1 = array ? dAtoi(array) : 0;

         if(array1 >= 0 && array1 < fld->elementCount && fld->elementCount >= 1)
         {
            // If the set data notify callback returns true, then go ahead and
            // set the data, otherwise, assume the set notify callback has either
            // already set the data, or has deemed that the data should not
            // be set at all.
            FrameTemp<char> buffer(2048);
            FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data 
            // so that if the prep functions or any other functions use the string stack, the data
            // is not corrupted.

            ConsoleBaseType *cbt = ConsoleBaseType::getType( fld->type );
            AssertFatal( cbt != NULL, "Could not resolve Type Id." );

            const char* szBuffer = cbt->prepData( value, buffer, 2048 );
            dMemset( bufferSecure, 0, 2048 );
            dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

            if( (*fld->setDataFn)( this, bufferSecure ) )
               Con::setData(fld->type, (void *) (((const char *)this) + fld->offset), array1, 1, &value, fld->table);

            onStaticModified( slotName, value );

            return;
         }

         if(fld->validator)
            fld->validator->validateType(this, (void *) (((const char *)this) + fld->offset));

         onStaticModified( slotName, value );
         return;
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         mFieldDictionary = new SimFieldDictionary;

      if(!array)
         mFieldDictionary->setFieldValue(slotName, value);
      else
      {
         char buf[256];
         dStrcpy(buf, slotName);
         dStrcat(buf, array);
         mFieldDictionary->setFieldValue(StringTable->insert(buf), value);
      }
   }
}


void  SimObject::dumpClassHierarchy()
{
   AbstractClassRep* pRep = getClassRep();
   while(pRep)
   {
      Con::warnf("%s ->", pRep->getClassName());
      pRep	=	pRep->getParentClass();
   }
}

const char *SimObject::getDataField(StringTableEntry slotName, const char *array)
{
   if(mFlags.test(ModStaticFields))
   {
      S32 array1 = array ? dAtoi(array) : -1;
      const AbstractClassRep::Field *fld = findField(slotName);
   
      if(fld)
      {
         if(array1 == -1 && fld->elementCount == 1)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), 0, fld->table, fld->flag) );
         if(array1 >= 0 && array1 < fld->elementCount)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), array1, fld->table, fld->flag) );// + typeSizes[fld.type] * array1));
         return "";
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         return "";

      if(!array) 
      {
         if (const char* val = mFieldDictionary->getFieldValue(slotName))
            return val;
      }
      else
      {
         static char buf[256];
         dStrcpy(buf, slotName);
         dStrcat(buf, array);
         if (const char* val = mFieldDictionary->getFieldValue(StringTable->insert(buf)))
            return val;
      }
   }

   return "";
}

U32 SimObject::getDataFieldType( StringTableEntry slotName, const char* array )
{
   const AbstractClassRep::Field* field = findField( slotName );
   if( field )
      return field->type;

   return 0;
}

SimObject::~SimObject()
{
   delete mFieldDictionary;

   AssertFatal(nextNameObject == (SimObject*)-1,avar(
                  "SimObject::~SimObject:  Not removed from dictionary: name %s, id %i",
                  objectName, mId));
   AssertFatal(nextManagerNameObject == (SimObject*)-1,avar(
                  "SimObject::~SimObject:  Not removed from manager dictionary: name %s, id %i",
                  objectName,mId));
   AssertFatal(mFlags.test(Added) == 0, "SimObject::object "
               "missing call to SimObject::onRemove");
}

//---------------------------------------------------------------------------

bool SimObject::isLocked()
{
   if(!mFieldDictionary)
      return false;

   const char * val = mFieldDictionary->getFieldValue( StringTable->insert( "locked", false ) );

   return( val ? dAtob(val) : false );
}

void SimObject::setLocked( bool b = true )
{
   setDataField(StringTable->insert("locked", false), NULL, b ? "true" : "false" );
}

bool SimObject::isHidden()
{
   if(!mFieldDictionary)
      return false;

   const char * val = mFieldDictionary->getFieldValue( StringTable->insert( "hidden", false ) );
   return( val ? dAtob(val) : false );
}

void SimObject::setHidden(bool b = true)
{
   setDataField(StringTable->insert("hidden", false), NULL, b ? "true" : "false" );
}

const char* SimObject::getIdString()
{
   static char IDbuffer[64];
   dSprintf(IDbuffer, sizeof(IDbuffer), "%d", mId);
   return IDbuffer;
}

//---------------------------------------------------------------------------

bool SimObject::onAdd()
{
   mFlags.set(Added);

   if(getClassRep())
      mNameSpace = getClassRep()->getNameSpace();

   linkNamespaces();

   // onAdd() should return FALSE if there was an error
   return true;
}

void SimObject::onRemove()
{
   mFlags.clear(Added);

   unlinkNamespaces();
}

void SimObject::onGroupAdd()
{
}

void SimObject::onGroupRemove()
{
}

void SimObject::onDeleteNotify(SimObject*)
{
}

void SimObject::onNameChange(const char*)
{
}

void SimObject::onStaticModified(const char* slotName, const char* newValue)
{
}

bool SimObject::processArguments(S32 argc, const char**)
{
   return argc == 0;
}

bool SimObject::isChildOfGroup(SimGroup* pGroup)
{
   if(!pGroup)
      return false;

   //if we *are* the group in question,
   //return true:
   if(pGroup == dynamic_cast<SimGroup*>(this))
      return true;

   SimGroup* temp	=	mGroup;
   while(temp)
   {
      if(temp == pGroup)
         return true;
      temp = temp->mGroup;
   }

   return false;
}


ConsoleMethod(SimObject, isChildOfGroup, bool, 3,3," returns true, if we are in the specified simgroup - or a subgroup thereof")
{
   SimGroup* pGroup = dynamic_cast<SimGroup*>(Sim::findObject(dAtoi(argv[2])));
   if(pGroup)
   {
      return object->isChildOfGroup(pGroup);
   }

   return false;
}
//---------------------------------------------------------------------------

static Chunker<SimObject::Notify> notifyChunker(128000);
SimObject::Notify *SimObject::mNotifyFreeList = NULL;

SimObject::Notify *SimObject::allocNotify()
{
   if(mNotifyFreeList)
   {
      SimObject::Notify *ret = mNotifyFreeList;
      mNotifyFreeList = ret->next;
      return ret;
   }
   return notifyChunker.alloc();
}

void SimObject::freeNotify(SimObject::Notify* note)
{
   AssertFatal(note->type != SimObject::Notify::Invalid, "Invalid notify");
   note->type = SimObject::Notify::Invalid;
   note->next = mNotifyFreeList;
   mNotifyFreeList = note;
}

//------------------------------------------------------------------------------

SimObject::Notify* SimObject::removeNotify(void *ptr, SimObject::Notify::Type type)
{
   Notify **list = &mNotifyList;
   while(*list)
   {
      if((*list)->ptr == ptr && (*list)->type == type)
      {
         SimObject::Notify *ret = *list;
         *list = ret->next;
         return ret;
      }
      list = &((*list)->next);
   }
   return NULL;
}

void SimObject::deleteNotify(SimObject* obj)
{
   AssertFatal(!obj->isDeleted(),
               "SimManager::deleteNotify: Object is being deleted");
   Notify *note = allocNotify();
   note->ptr = (void *) this;
   note->next = obj->mNotifyList;
   note->type = Notify::DeleteNotify;
   obj->mNotifyList = note;

   note = allocNotify();
   note->ptr = (void *) obj;
   note->next = mNotifyList;
   note->type = Notify::ClearNotify;
   mNotifyList = note;

   //obj->deleteNotifyList.pushBack(this);
   //clearNotifyList.pushBack(obj);
}

void SimObject::registerReference(SimObject **ptr)
{
   Notify *note = allocNotify();
   note->ptr = (void *) ptr;
   note->next = mNotifyList;
   note->type = Notify::ObjectRef;
   mNotifyList = note;
}

void SimObject::unregisterReference(SimObject **ptr)
{
   Notify *note = removeNotify((void *) ptr, Notify::ObjectRef);
   if(note)
      freeNotify(note);
}

void SimObject::clearNotify(SimObject* obj)
{
   Notify *note = obj->removeNotify((void *) this, Notify::DeleteNotify);
   if(note)
      freeNotify(note);

   note = removeNotify((void *) obj, Notify::ClearNotify);
   if(note)
      freeNotify(note);
}

void SimObject::processDeleteNotifies()
{
   // clear out any delete notifies and
   // object refs.

   while(mNotifyList)
   {
      Notify *note = mNotifyList;
      mNotifyList = note->next;

      AssertFatal(note->type != Notify::ClearNotify, "Clear notes should be all gone.");

      if(note->type == Notify::DeleteNotify)
      {
         SimObject *obj = (SimObject *) note->ptr;
         Notify *cnote = obj->removeNotify((void *)this, Notify::ClearNotify);
         obj->onDeleteNotify(this);
         freeNotify(cnote);
      }
      else
      {
         // it must be an object ref - a pointer refs this object
         *((SimObject **) note->ptr) = NULL;
      }
      freeNotify(note);
   }
}

void SimObject::clearAllNotifications()
{
   for(Notify **cnote = &mNotifyList; *cnote; )
   {
      Notify *temp = *cnote;
      if(temp->type == Notify::ClearNotify)
      {
         *cnote = temp->next;
         Notify *note = ((SimObject *) temp->ptr)->removeNotify((void *) this, Notify::DeleteNotify);
         freeNotify(temp);
         freeNotify(note);
      }
      else
         cnote = &(temp->next);
   }
}

//---------------------------------------------------------------------------

void SimObject::initPersistFields()
{
   Parent::initPersistFields();
   //add the canSaveDynamicFields property:
   addGroup("SimBase");
   addField("canSaveDynamicFields",		TypeBool,		Offset(mCanSaveFieldDictionary, SimObject));
   addField("internalName",            TypeString,       Offset(mInternalName, SimObject));
   addProtectedField("parentGroup",        TypeSimObjectPtr, Offset(mGroup, SimObject), &setProtParent, &defaultProtectedGetFn, "Group hierarchy parent of the object." );
   endGroup("SimBase");

   // Namespace Linking.
   addGroup("Namespace Linking");
   addProtectedField("superclass", TypeString, Offset(mSuperClassName, SimObject), &setSuperClass, &defaultProtectedGetFn, "Script Class of object.");
   addProtectedField("class",      TypeString, Offset(mClassName,      SimObject), &setClass,      &defaultProtectedGetFn, "Script SuperClass of object.");
   endGroup("Namespace Linking");
}

void SimObject::copyTo(SimObject* object)
{
   object->mClassName = mClassName;
   object->mSuperClassName = mSuperClassName;

   linkNamespaces();
}

bool SimObject::setProtParent(void* obj, const char* data)
{
   SimGroup *parent = NULL;
   SimObject *object = static_cast<SimObject*>(obj);

   if(Sim::findObject(data, parent))
      parent->addObject(object);
   
   // always return false, because we've set mGroup when we called addObject
   return false;
}


bool SimObject::addToSet(SimObjectId spid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(spid);
   if (ptr) 
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
                  "SimObject::addToSet: "
                  "ObjectId does not refer to a set object");
      sp->addObject(this);
      return true;
   }
   return false;
}

bool SimObject::addToSet(const char *ObjectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(ObjectName);
   if (ptr) 
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
                  "SimObject::addToSet: "
                  "ObjectName does not refer to a set object");
      sp->addObject(this);
      return true;
   }
   return false;
}

bool SimObject::removeFromSet(SimObjectId sid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(sid, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

bool SimObject::removeFromSet(const char *objectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(objectName, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

void SimObject::inspectPreApply()
{
}

void SimObject::inspectPostApply()
{
}

void SimObject::linkNamespaces()
{
   if( mNameSpace )
      unlinkNamespaces();
   
   StringTableEntry parent = StringTable->insert( getClassName() );
   if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
   {
      if( Con::linkNamespaces( parent, mSuperClassName ) )
         parent = mSuperClassName;
      else
         mSuperClassName = StringTable->insert( "" ); // CodeReview Is this behavior that we want?
                                                      // CodeReview This will result in the mSuperClassName variable getting hosed
                                                      // CodeReview if Con::linkNamespaces returns false. Looking at the code for
                                                      // CodeReview Con::linkNamespaces, and the call it makes to classLinkTo, it seems
                                                      // CodeReview like this would only fail if it had bogus data to begin with, but
                                                      // CodeReview I wanted to note this behavior which occurs in some implementations
                                                      // CodeReview but not all. -patw
   }

   // ClassName -> SuperClassName
   if ( ( mNSLinkMask & LinkClassName ) && mClassName && mClassName[0] )
   {
      if( Con::linkNamespaces( parent, mClassName ) )
         parent = mClassName;
      else
         mClassName = StringTable->insert( "" ); // CodeReview (See previous note on this code)
   }

   // ObjectName -> ClassName
   StringTableEntry objectName = getName();
   if( objectName && objectName[0] )
   {
      if( Con::linkNamespaces( parent, objectName ) )
         parent = objectName;
   }

   // Store our namespace.
#ifdef PUAP_NAMESPACE_CHANGE
   mNameSpace = Con::lookupAndCreateNamespace( parent );
#else //normal TGB
   mNameSpace = Con::lookupNamespace( parent );
#endif //PUAP_NAMESPACE_CHANGE
}

void SimObject::unlinkNamespaces()
{
   if (!mNameSpace)
      return;

   // Restore NameSpace's
   StringTableEntry child = getName();
   if( child && child[0] )
   {
      if( ( mNSLinkMask & LinkClassName ) && mClassName && mClassName[0])
      {
         if( Con::unlinkNamespaces( mClassName, child ) )
            child = mClassName;
      }

      if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
      {
         if( Con::unlinkNamespaces( mSuperClassName, child ) )
            child = mSuperClassName;
      }

      Con::unlinkNamespaces( getClassName(), child );
   }
   else
   {
      child = mClassName;
      if( child && child[0] )
      {
         if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
         {
            if( Con::unlinkNamespaces( mSuperClassName, child ) )
               child = mSuperClassName;
         }

         Con::unlinkNamespaces( getClassName(), child );
      }
      else
      {
         if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
            Con::unlinkNamespaces( getClassName(), mSuperClassName );
      }
   }

#ifdef PUAP_NAMESPACE_CHANGE
   //-Mat we always want to swapNamespace(), not NULLify them
   //mNameSpace = NULL;
#else //normal TGB
   mNameSpace = NULL;
#endif //PUAP_NAMESPACE_CHANGE
}

void SimObject::setClassNamespace( const char *classNamespace )
{
   mClassName = StringTable->insert( classNamespace );
}

void SimObject::setSuperClassNamespace( const char *superClassNamespace )
{  
   mSuperClassName = StringTable->insert( superClassNamespace );
}

ConsoleMethod(SimObject, getClassNamespace, const char*, 2, 2, "")
{
   return object->getClassNamespace();
}

ConsoleMethod(SimObject, getSuperClassNamespace, const char*, 2, 2, "")
{
   return object->getSuperClassNamespace();
}

ConsoleMethod(SimObject, setClassNamespace, void, 2, 3, "")
{
   object->setClassNamespace(argv[2]);
}

ConsoleMethod(SimObject, setSuperClassNamespace, void, 2, 3, "")
{
   object->setSuperClassNamespace(argv[2]);
}

IMPLEMENT_CONOBJECT(SimObject);

//---------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(SimDataBlock);
SimObjectId SimDataBlock::sNextObjectId = DataBlockObjectIdFirst;
S32 SimDataBlock::sNextModifiedKey = 0;

//---------------------------------------------------------------------------

SimDataBlock::SimDataBlock()
{
   setModDynamicFields(true);
   setModStaticFields(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RKS: DO NOT LEAVE THIS HERE -- DELETE THIS DESTRUCTOR -- THIS IS ONLY HERE TO DEBUG HOW DATABLOCKS ARE REMOVED (IF AT ALL)
SimDataBlock::~SimDataBlock()
{ 
	//dPrintf( "DELETING %s\n", getName() ); 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SimDataBlock::onAdd()
{
   Parent::onAdd();

   // This initialization is done here, and not in the constructor,
   // because some jokers like to construct and destruct objects
   // (without adding them to the manager) to check what class
   // they are.
   modifiedKey = ++sNextModifiedKey;
   AssertFatal(sNextObjectId <= DataBlockObjectIdLast,
               "Exceeded maximum number of data blocks");

   // add DataBlock to the DataBlockGroup unless it is client side ONLY DataBlock
   if (getId() >= DataBlockObjectIdFirst && getId() <= DataBlockObjectIdLast)
      if (SimGroup* grp = Sim::getDataBlockGroup())
         grp->addObject(this);
   return true;
}

void SimDataBlock::assignId()
{
   // We don't want the id assigned by the manager, but it may have
   // already been assigned a correct data block id.
   if (getId() < DataBlockObjectIdFirst || getId() > DataBlockObjectIdLast)
      setId(sNextObjectId++);
}

void SimDataBlock::onStaticModified(const char* slotName, const char* newValue)
{
   modifiedKey = sNextModifiedKey++;

}

/*void SimDataBlock::setLastError(const char*)
{
} */

void SimDataBlock::packData(BitStream*)
{
}

void SimDataBlock::unpackData(BitStream*)
{
}

bool SimDataBlock::preload(bool, char[256])
{
   return true;
}

ConsoleFunction(deleteDataBlocks, void, 1, 1, "() Use the deleteDataBlocks function to cause a server to delete all datablocks that have thus far been loaded and defined.\n"
																"This is usually done in preparation of downloading a new set of datablocks, such as occurs on a mission change, but it's also good post-mission cleanup\n"
																"@return No return value.")
{
   argc; argv;
   // delete from last to first:
   SimGroup *grp = Sim::getDataBlockGroup();
   for(S32 i = grp->size() - 1; i >= 0; i--)
   {
      SimObject *obj = (*grp)[i];
      obj->deleteObject();
   }
   SimDataBlock::sNextObjectId = DataBlockObjectIdFirst;
   SimDataBlock::sNextModifiedKey = 0;
}

//------------------------------------------------------------------------------

SimConsoleEvent::SimConsoleEvent(S32 argc, const char **argv, bool onObject)
{
   mOnObject = onObject;
   mArgc = argc;
   U32 totalSize = 0;
   S32 i;
   for(i = 0; i < argc; i++)
      totalSize += dStrlen(argv[i]) + 1;
   totalSize += sizeof(char *) * argc;

   mArgv = (char **) dMalloc(totalSize);
   char *argBase = (char *) &mArgv[argc];

   for(i = 0; i < argc; i++)
   {
      mArgv[i] = argBase;
      dStrcpy(mArgv[i], argv[i]);
      argBase += dStrlen(argv[i]) + 1;
   }
}

SimConsoleEvent::~SimConsoleEvent()
{
   dFree(mArgv);
}

void SimConsoleEvent::process(SimObject* object)
{
// #ifdef DEBUG
//    Con::printf("Executing schedule: %d", sequenceCount);
// #endif
    if(mOnObject)
      Con::execute(object, mArgc, const_cast<const char**>( mArgv ));
   else
   {
      // Grab the function name. If '::' doesn't exist, then the schedule is
      // on a global function.
      char* func = dStrstr( mArgv[0], (char*)"::" );
      if( func )
      {
         // Set the first colon to NULL, so we can reference the namespace.
         // This is okay because events are deleted immediately after
         // processing. Maybe a bad idea anyway?
         func[0] = '\0';

         // Move the pointer forward to the function name.
         func += 2;

         // Lookup the namespace and function entry.
		 //-Mat this Namespace should exist, if we create it, there will not be a function anyway
		 //-Mat may break scheduling
         Namespace* ns = Namespace::find( StringTable->insert( mArgv[0] ) );
         if( ns )
         {
            Namespace::Entry* nse = ns->lookup( StringTable->insert( func ) );
            if( nse )
               // Execute.
#ifdef PUAP_NAMESPACE_CHANGE
               nse->execute( mArgc, (const char**)mArgv);
#else //normal TGB
               nse->execute( mArgc, (const char**)mArgv, &gEvalState );
#endif //PUAP_NAMESPACE_CHANGE
         }
      }

      else
         Con::execute(mArgc, const_cast<const char**>( mArgv ));
   }
}

//-----------------------------------------------------------------------------

SimConsoleThreadExecCallback::SimConsoleThreadExecCallback() : retVal(NULL)
{
   sem = Semaphore::createSemaphore(0);
}

SimConsoleThreadExecCallback::~SimConsoleThreadExecCallback()
{
   Semaphore::destroySemaphore(sem);
}

void SimConsoleThreadExecCallback::handleCallback(const char *ret)
{
   retVal = ret;
   Semaphore::releaseSemaphore(sem);
}

const char *SimConsoleThreadExecCallback::waitForResult()
{
   if(Semaphore::acquireSemaphore(sem, true))
   {
      return retVal;
   }

   return NULL;
}

//-----------------------------------------------------------------------------

SimConsoleThreadExecEvent::SimConsoleThreadExecEvent(S32 argc, const char **argv, bool onObject, SimConsoleThreadExecCallback *callback) : 
   SimConsoleEvent(argc, argv, onObject),
   cb(callback)
{
}

void SimConsoleThreadExecEvent::process(SimObject* object)
{
   const char *retVal;
   if(mOnObject)
      retVal = Con::execute(object, mArgc, const_cast<const char**>( mArgv ));
   else
      retVal = Con::execute(mArgc, const_cast<const char**>( mArgv ));

   if(cb)
      cb->handleCallback(retVal);
}

//-----------------------------------------------------------------------------

inline void SimSetIterator::Stack::push_back(SimSet* set)
{
   increment();
   last().set = set;
   last().itr = set->begin();
}


//-----------------------------------------------------------------------------

SimSetIterator::SimSetIterator(SimSet* set)
{
   VECTOR_SET_ASSOCIATION(stack);

   if (!set->empty())
      stack.push_back(set);
}


//-----------------------------------------------------------------------------

SimObject* SimSetIterator::operator++()
{
   SimSet* set;
   if ((set = dynamic_cast<SimSet*>(*stack.last().itr)) != 0) 
   {
      if (!set->empty()) 
      {
         stack.push_back(set);
         return *stack.last().itr;
      }
   }

   while (++stack.last().itr == stack.last().set->end()) 
   {
      stack.pop_back();
      if (stack.empty())
         return 0;
   }
   return *stack.last().itr;
}	

SimObject* SimGroupIterator::operator++()
{
   SimGroup* set;
   if ((set = dynamic_cast<SimGroup*>(*stack.last().itr)) != 0) 
   {
      if (!set->empty()) 
      {
         stack.push_back(set);
         return *stack.last().itr;
      }
   }

   while (++stack.last().itr == stack.last().set->end()) 
   {
      stack.pop_back();
      if (stack.empty())
         return 0;
   }
   return *stack.last().itr;
}	

