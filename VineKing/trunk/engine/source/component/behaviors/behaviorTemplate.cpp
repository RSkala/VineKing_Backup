//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/simBase.h"
#include "console/consoleTypes.h"
#include "component/simComponent.h"
#include "component/behaviors/behaviorTemplate.h"
#include "util/safeDelete.h"
#include "core/resManager.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

BehaviorTemplate::BehaviorTemplate()
{
   mFriendlyName = StringTable->lookup("");
   mFromResource = StringTable->lookup("");
   mBehaviorType = StringTable->lookup("");

   // [tom, 1/12/2007] We manage the memory for the description since it
   // could be loaded from a file and thus massive. This is accomplished with
   // protected fields, but since they still call Con::getData() the field
   // needs to always be valid. This is pretty lame.
   mDescription = new char [1];
   ((char *)mDescription)[0] = 0;
}

BehaviorTemplate::~BehaviorTemplate()
{
   for(S32 i = 0;i < mFields.size();++i)
   {
      BehaviorField &field = mFields[i];
      SAFE_DELETE_ARRAY(field.mFieldDescription);
   }

   SAFE_DELETE_ARRAY(mDescription);
}

IMPLEMENT_CONOBJECT(BehaviorTemplate);

//////////////////////////////////////////////////////////////////////////

void BehaviorTemplate::initPersistFields()
{
   addGroup("Behavior");
      addField("friendlyName", TypeCaseString, Offset(mFriendlyName, BehaviorTemplate), "Human friendly name of this behavior");
      addProtectedField("description", TypeCaseString, Offset(mDescription, BehaviorTemplate), &setDescription, &getDescription, 
         "The description of this behavior which can be set to a \"string\" or a fileName\n");
      addField("behaviorType", TypeString, Offset(mBehaviorType, BehaviorTemplate), "?? Organizational keyword ??");
   endGroup("Behavior");

   Parent::initPersistFields();
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorTemplate::setDescription(void* obj, const char* data)
{
   BehaviorTemplate *object = static_cast<BehaviorTemplate *>(obj);
   SAFE_DELETE_ARRAY(object->mDescription);
   object->mDescription = object->getDescriptionText(data);

   // We return false since we don't want the console to mess with the data
   return false;
}

const char * BehaviorTemplate::getDescription(void* obj, const char* data)
{
   BehaviorTemplate *object = static_cast<BehaviorTemplate *>(obj);

   return object->mDescription ? object->mDescription : "";
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorTemplate::onAdd()
{
   if(! Parent::onAdd())
      return false;

   Sim::gBehaviorSet->addObject(this);

   return true;
}

void BehaviorTemplate::onRemove()
{
   Sim::gBehaviorSet->removeObject(this);

   Parent::onRemove();
}

//////////////////////////////////////////////////////////////////////////

 bool BehaviorTemplate::setupFields( BehaviorInstance *bi )
 {
    for(S32 i = 0;i < mFields.size();++i)
    {
       BehaviorField &field = mFields[i];
       bi->setDataField(field.mFieldName, NULL, field.mDefaultValue);
    }
 
    return true;
 }

//////////////////////////////////////////////////////////////////////////

void BehaviorTemplate::addBehaviorField(const char *fieldName, const char *desc, const char *type, const char *defaultValue /* = NULL */, const char *userData /* = NULL */)
{
   StringTableEntry stFieldName = StringTable->insert(fieldName);

   for(S32 i = 0;i < mFields.size();++i)
   {
      if(mFields[i].mFieldName == stFieldName)
         return;
   }

   BehaviorField field;
   field.mFieldName = stFieldName;
   field.mFieldType = StringTable->insert(type ? type : "");
   field.mUserData = StringTable->insert(userData ? userData : "");
   field.mDefaultValue = StringTable->insert(defaultValue ? defaultValue : "");
   field.mFieldDescription = getDescriptionText(desc);

   mFields.push_back(field);
}

//////////////////////////////////////////////////////////////////////////

const char * BehaviorTemplate::getDescriptionText(const char *desc)
{
   if(desc == NULL)
      return NULL;

   char *newDesc;

   // [tom, 1/12/2007] If it isn't a file, just do it the easy way
   if(! Platform::isFile(desc))
   {
      newDesc = new char [dStrlen(desc) + 1];
      dStrcpy(newDesc, desc);

      return newDesc;
   }

   Stream *stream = ResourceManager->openStream(desc);
   if(stream == NULL)
      return NULL;

   U32 size = stream->getStreamSize();
   if(size > 0)
   {
      newDesc = new char [size + 1];
      if(stream->read(size, (void *)newDesc))
         newDesc[size] = 0;
      else
      {
         SAFE_DELETE_ARRAY(newDesc);
      }
   }

   ResourceManager->closeStream(stream);

   return newDesc;
}

//////////////////////////////////////////////////////////////////////////

BehaviorInstance *BehaviorTemplate::createInstance()
{
   BehaviorInstance *instance = new BehaviorInstance(this);

   setupFields( instance );

   if(instance->registerObject())
      return instance;

   delete instance;
   return NULL;
}

//////////////////////////////////////////////////////////////////////////
// Console Methods
//////////////////////////////////////////////////////////////////////////

ConsoleMethod(BehaviorTemplate, addBehaviorField, void, 5, 7, "(fieldName, desc, type, [defaultValue, userData])\n"
              "Adds a named BehaviorField to a Behavior Template\n"
              "@param fieldName The name of this field\n"
              "@param desc The Description of this field\n"
			  "@param type The DataType for this field (default, int, float, Point2F, bool, enum, Object, keybind, color)\n"
              "@param defaultValue The Default value for this field\n"
			  "@param userData An extra data field that can be used for custom data on a per-field basis<br>Usage for default types<br>"
			  "-enum: a TAB separated list of possible values<br>"
			  "-object: the T2D object type that are valid choices for the field.  The object types observe inheritance, so if you have a t2dSceneObject field you will be able to choose t2dStaticSrpites, t2dAnimatedSprites, etc.\n"
              "@return Nothing\n")
{
   const char *defValue = argc > 5 ? argv[5] : NULL;
   const char *typeInfo = argc > 6 ? argv[6] : NULL;
   
   object->addBehaviorField(argv[2], argv[3], argv[4], defValue, typeInfo);
}

ConsoleMethod(BehaviorTemplate, getBehaviorFieldCount, S32, 2, 2, "() - Get the number of BehaviorField's on this object\n"
              "@return Returns the number of BehaviorFields as a nonnegative integer\n")
{
   return object->getBehaviorFieldCount();
}

// [tom, 1/12/2007] Field accessors split into multiple methods to allow space
// for long descriptions and type data.

ConsoleMethod(BehaviorTemplate, getBehaviorField, const char *, 3, 3, "(int index) - Gets a Tab-Delimited list of information about a BehaviorField specified by Index\n"
              "@param index The index of the behavior\n"
			  "@return FieldName, FieldType and FieldDefaultValue, each separated by a TAB character.\n")
{
   BehaviorTemplate::BehaviorField *field = object->getBehaviorField(dAtoi(argv[2]));
   if(field == NULL)
      return "";

   char *buf = Con::getReturnBuffer(1024);
   dSprintf(buf, 1024, "%s\t%s\t%s", field->mFieldName, field->mFieldType, field->mDefaultValue);

   return buf;
}

ConsoleMethod(BehaviorTemplate, getBehaviorFieldUserData, const char *, 3, 3, "(int index) - Gets the UserData associated with a field by index in the field list\n"
			  "@param index The index of the behavior\n"
			  "@return Returns a string representing the user data of this field\n")
{
   BehaviorTemplate::BehaviorField *field = object->getBehaviorField(dAtoi(argv[2]));
   if(field == NULL)
      return "";

   return field->mUserData;
}

ConsoleMethod(BehaviorTemplate, getBehaviorFieldDescription, const char *, 3, 3, "(int index) - Gets a field description by index\n"
			  "@param index The index of the behavior\n"
              "@return Returns a string representing the description of this field\n")
{
   BehaviorTemplate::BehaviorField *field = object->getBehaviorField(dAtoi(argv[2]));
   if(field == NULL)
      return "";

   return field->mFieldDescription ? field->mFieldDescription : "";
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(BehaviorTemplate, createInstance, S32, 2, 2, "() - Create an instance of this behavior\n"
			  "@return (BehaviorInstance inst) The behavior instance created")
{
   BehaviorInstance *inst = object->createInstance();
   return inst ? inst->getId() : 0;
}
