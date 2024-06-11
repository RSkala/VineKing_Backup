//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "component/behaviors/behaviorInstance.h"
#include "component/behaviors/behaviorTemplate.h"
#include "console/consoleTypes.h"
#include "console/consoleInternal.h"
#include "core/stream.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

BehaviorInstance::BehaviorInstance( BehaviorTemplate *btemplate ) : mTemplate( btemplate )
{
   mBehaviorOwner = NULL;
}

BehaviorInstance::~BehaviorInstance()
{
}

IMPLEMENT_CONOBJECT(BehaviorInstance);

//////////////////////////////////////////////////////////////////////////

void BehaviorInstance::initPersistFields()
{
   addGroup("Behavior");
      addField("template", TypeSimObjectName, Offset(mTemplate, BehaviorInstance), "Template this instance was created from");
      // Read-only field, set always returns false
      addProtectedField( "Owner", TypeSimObjectPtr, Offset(mBehaviorOwner, BehaviorInstance), &setOwner, &defaultProtectedGetFn, "" );
   endGroup("Behavior");

   Parent::initPersistFields();
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorInstance::onAdd()
{
   if(! Parent::onAdd())
      return false;

   // Store this object's namespace
#ifdef PUAP_NAMESPACE_CHANGE
   mNameSpace = Namespace::findAndCreate(getTemplateName());
#else
   mNameSpace = Namespace::global()->find(getTemplateName());
#endif //PUAP_NAMESPACE_CHANGE
   return true;
}

void BehaviorInstance::onRemove()
{
   Parent::onRemove();
}

//////////////////////////////////////////////////////////////////////////

void BehaviorInstance::packToStream( Stream &stream, U32 tabStop, U32 flags /* = 0  */ )
{
   char buffer[1024];

   // This is a specialized write that just wants a single line which will represent the behavior
   // so it can be serialized. The stream should already be set up so all we have to do is write
   // out a string to the stream, and the calling method will take care of the rest.

   // Write out common info
   dSprintf( buffer, sizeof( buffer ), "%s", getTemplateName() );
   stream.write( dStrlen( buffer ), buffer );

   // Write out the fields which the behavior template knows about
   for( int i = 0; i < mTemplate->getBehaviorFieldCount(); i++ )
   {
      BehaviorTemplate::BehaviorField *field = mTemplate->getBehaviorField( i );
      const char *objFieldValue = getDataField( field->mFieldName, NULL );

      // If the field holds the same value as the template's default value than it
      // will get initialized by the template, and so it won't be included just
      // to try to keep the object files looking as non-horrible as possible.
      if( dStrcmp( field->mDefaultValue, objFieldValue ) != 0 )
      {
         // Write out a field/value pair
         dSprintf( buffer, sizeof( buffer ), "\t%s\t%s", field->mFieldName, ( dStrlen( objFieldValue ) > 0 ? objFieldValue : "0" ) );
         stream.write( dStrlen( buffer ), buffer );
      }
   }
}

const char * BehaviorInstance::getTemplateName()
{
   return mTemplate ? mTemplate->getName() : NULL;
}

//////////////////////////////////////////////////////////////////////////
// Console Methods
//////////////////////////////////////////////////////////////////////////

ConsoleMethod(BehaviorInstance, getTemplateName, const char *, 2, 2, "() - Get the template name of this behavior\n"
																	 "@return (string name) The name of the template this behaivor was created from")
{
   const char *name = object->getTemplateName();
   return name ? name : "";
}
