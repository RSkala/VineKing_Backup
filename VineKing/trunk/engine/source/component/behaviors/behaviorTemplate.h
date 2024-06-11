//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "console/simBase.h"
#include "component/behaviors/behaviorInstance.h"

#ifndef _BEHAVIORTEMPLATE_H_
#define _BEHAVIORTEMPLATE_H_

//////////////////////////////////////////////////////////////////////////
/// 
/// 
//////////////////////////////////////////////////////////////////////////
class BehaviorTemplate : public SimObject
{
   typedef SimObject Parent;

public:
   struct BehaviorField
   {
      StringTableEntry mFieldName;
      StringTableEntry mFieldDescription;

      StringTableEntry mFieldType;
      StringTableEntry mUserData;

      StringTableEntry mDefaultValue;
   };

protected:
   StringTableEntry mFriendlyName;
   StringTableEntry mDescription;
   
   StringTableEntry mFromResource;
   StringTableEntry mBehaviorType;

   Vector<BehaviorField> mFields;

public:
   BehaviorTemplate();
   virtual ~BehaviorTemplate();
   DECLARE_CONOBJECT(BehaviorTemplate);

   virtual bool onAdd();
   virtual void onRemove();
   static void initPersistFields();

   /// @name Creation Methods
   /// @{

   /// Create a BehaviorInstance from this template
   /// @return   BehaviorInstance   returns the newly created BehaviorInstance object
   BehaviorInstance *createInstance();

   bool setupFields( BehaviorInstance *bi );
   /// @}

   /// @name Adding Named Fields
   /// @{

   /// Adds a named field to a BehaviorTemplate that can specify a description, data type, default value and userData
   ///
   /// @param   fieldName    The name of the Field
   /// @param   desc         The Description of the Field
   /// @param   type         The Type of field that this is, example 'Text' or 'Bool'
   /// @param   defaultValue The Default value of this field
   /// @param   userData     An extra optional field that can be used for user data
   void addBehaviorField(const char *fieldName, const char *desc, const char *type, const char *defaultValue = NULL, const char *userData = NULL);

   /// Returns the number of BehaviorField's on this template
   inline S32 getBehaviorFieldCount() { return mFields.size(); };

   /// Gets a BehaviorField by its index in the mFields vector 
   /// @param idx  The index of the field in the mField vector
   inline BehaviorField *getBehaviorField(S32 idx)
   {
      if(idx < 0 || idx >= mFields.size())
         return NULL;

      return &mFields[idx];
   }

   const char *getDescriptionText(const char *desc);
   /// @}

   /// @name Description
   /// @{
   static bool setDescription(void* obj, const char* data);
   static const char* getDescription(void* obj, const char* data);

   /// @}

};

#endif // _BEHAVIORTEMPLATE_H_
