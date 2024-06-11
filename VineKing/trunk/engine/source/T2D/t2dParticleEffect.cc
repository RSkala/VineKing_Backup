//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Particle Effect Object.
//-----------------------------------------------------------------------------

#include "core/resManager.h"
#include "./t2dParticleEffect.h"
#include "console/consoleTypes.h"

#ifdef PUAP_OPTIMIZE
//%PUAP% for integrateObject
#include "math/mrect.h"
#endif

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif

//------------------------------------------------------------------------------


IMPLEMENT_CONOBJECT(t2dParticleEffect);


//-----------------------------------------------------------------------------
// Effect-Life Mode Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums effectLifeLookup[] =
{
   { t2dParticleEffect::INFINITE,  "INFINITE" },
   { t2dParticleEffect::CYCLE,     "CYCLE" },
   { t2dParticleEffect::KILL,      "KILL" },
   { t2dParticleEffect::STOP,      "STOP" },
};
static EnumTable gEffectMode(4, &effectLifeLookup[0]);

//-----------------------------------------------------------------------------
// Effect-Life Mode Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dParticleEffect::eEffectLifeMode getEffectMode(const char* label)
{
   // Search for Mnemonic.
   for(U32 i = 0; i < (sizeof(effectLifeLookup) / sizeof(EnumTable::Enums)); i++)
      if( dStricmp(effectLifeLookup[i].label, label) == 0)
         return((t2dParticleEffect::eEffectLifeMode)effectLifeLookup[i].index);

   // Invalid Effect Life-Mode!
   AssertFatal(false, "t2dParticleEffect::getEffectMode() - Invalid Effect-Life Mode!");
   // Bah!
   return t2dParticleEffect::INFINITE;
}

bool t2dParticleEffect::setEffectMode(void* obj, const char* data)
{ 
   t2dParticleEffect *object = static_cast<t2dParticleEffect*>(obj);

   object->setEffectLifeMode( getEffectMode( data ), object->getEffectLifeTime() ); 

   return false; 
};


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dParticleEffect::t2dParticleEffect() :    T2D_Stream_HeaderID(makeFourCCTag('2','D','P','F')),
                                            mpCurrentGraph(NULL),
                                            mCurrentGraphName(StringTable->insert("")),
                                            mEffectPlaying(false),
                                            mWaitingForParticles(false),
                                            mWaitingForDelete(false),
                                            mInitialised(false),
                                            mUseEffectCollisions(false),
                                            mEffectLifeMode(INFINITE),
                                            mEffectLifetime(0.0f),
                                            mEmitterSerial(1),
                                            mEffectFile(StringTable->insert("")),
											mWasPlaying(false)
{
   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( mParticleEmitterList );

   // Picking is Allowed!
   Parent::mPickingAllowed = true;
   // Enable Always Scope.
   Parent::mEnableAlwaysScope = true;

   mDisableParticleInterpolation = Con::getBoolVariable( "$pref::T2D::disableParticleInterpolation", false );

   // Initialise Effect.
   if ( !mInitialised )
      initialise();
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dParticleEffect::~t2dParticleEffect()
{
   // Clear Emitters.
   clearEmitters();

   // Clear Graph Selections.
   clearGraphSelections();
}


//-----------------------------------------------------------------------------
// Safe Delete.
//-----------------------------------------------------------------------------
void t2dParticleEffect::safeDelete()
{
   // Are we already waiting for delete?
   if ( mWaitingForDelete )
      // Yes, nothing to do!
      return;

   // Is effect playing?
   if ( mEffectPlaying )
   {
      // Yes, so stop the effect and allow it to kill itself.
      stopEffect(true, true);
   }
   else
   {
       // Call parent which will deal with the deletion.
       Parent::safeDelete();
   }
}


//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dParticleEffect::onAdd()
{
   // Eventually, we'll need to deal with Server/Client functionality!

   // Call Parent.
   if(!Parent::onAdd())
      return false;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dParticleEffect::onRemove()
{
   // Call Parent.
   Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Clear Graph Selection.
//-----------------------------------------------------------------------------
void t2dParticleEffect::clearGraphSelections( void )
{
   // Destroy Graph Selections.
   for ( U32 n = 0; n < mGraphSelectionList.size(); n++ )
      delete mGraphSelectionList[n];

   // Clear List.
   mGraphSelectionList.clear();
}


//-----------------------------------------------------------------------------
// Add Graph Selection.
//-----------------------------------------------------------------------------
void t2dParticleEffect::addGraphSelection( const char* graphName, t2dGraphField* pGraphObject )
{
   // Generate new Graph Selection.
   tGraphSelection* pGraphSelection = new tGraphSelection;

   // Populate Graph Selection.
   pGraphSelection->mGraphName = StringTable->insert( graphName );
   pGraphSelection->mpGraphObject = pGraphObject;

   // Put into Graph Selection List.
   mGraphSelectionList.push_back( pGraphSelection );
}


//-----------------------------------------------------------------------------
// Find Graph Selection.
//-----------------------------------------------------------------------------
t2dGraphField* t2dParticleEffect::findGraphSelection( const char* graphName ) const
{
   // Search For Selected Graph and return if found.
   for ( U32 n = 0; n < mGraphSelectionList.size(); n++ )
      if ( mGraphSelectionList[n]->mGraphName == StringTable->insert(graphName) )
         return mGraphSelectionList[n]->mpGraphObject;

   // Return "Not Found".
   return NULL;
}


//-----------------------------------------------------------------------------
// Initialise.
//-----------------------------------------------------------------------------
void t2dParticleEffect::initialise( void )
{
   // ****************************************************************************
   // Initialise Graph Selections.
   // ****************************************************************************
   addGraphSelection( "particlelife_scale", &mParticleLife.t2dGraphField_Base );
   addGraphSelection( "quantity_scale", &mQuantity.t2dGraphField_Base );
   addGraphSelection( "sizex_scale", &mSizeX.t2dGraphField_Base );
   addGraphSelection( "sizey_scale", &mSizeY.t2dGraphField_Base );
   addGraphSelection( "speed_scale", &mSpeed.t2dGraphField_Base );
   addGraphSelection( "spin_scale", &mSpin.t2dGraphField_Base );
   addGraphSelection( "fixedforce_scale", &mFixedForce.t2dGraphField_Base );
   addGraphSelection( "randommotion_scale", &mRandomMotion.t2dGraphField_Base );
   addGraphSelection( "visibility_scale", &mVisibility.t2dGraphField_Base );
   addGraphSelection( "emissionforce_base", &mEmissionForce.t2dGraphField_Base );
   addGraphSelection( "emissionforce_var", &mEmissionForce.t2dGraphField_Variation );
   addGraphSelection( "emissionangle_base", &mEmissionAngle.t2dGraphField_Base );
   addGraphSelection( "emissionangle_var", &mEmissionAngle.t2dGraphField_Variation );
   addGraphSelection( "emissionarc_base", &mEmissionArc.t2dGraphField_Base );
   addGraphSelection( "emissionarc_var", &mEmissionArc.t2dGraphField_Variation );


   // ****************************************************************************
   // Initialise Graphs.
   // ****************************************************************************
   mParticleLife.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f );
   mQuantity.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f ); 
   mSizeX.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f ); 
   mSizeY.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f ); 
   mSpeed.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f ); 
   mSpin.t2dGraphField_Base.setValueBounds( 1000.0f, -100.0f, 100.0f, 1.0f ); 
   mFixedForce.t2dGraphField_Base.setValueBounds( 1000.0f, -100.0f, 100.0f, 1.0f ); 
   mRandomMotion.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f ); 
   mVisibility.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 100.0f, 1.0f ); 
   mEmissionForce.t2dGraphField_Base.setValueBounds( 1000.0f, -1000.0f, 1000.0f, 5.0f );
   mEmissionForce.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 2000.0f, 0.0f );
   mEmissionAngle.t2dGraphField_Base.setValueBounds( 1000.0f, -3600.0f, 3600.0f, 0.0f ); 
   mEmissionAngle.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 720.0f, 0.0f );
   mEmissionArc.t2dGraphField_Base.setValueBounds( 1000.0f, 0.0f, 360.0f, 360.0f );
   mEmissionArc.t2dGraphField_Variation.setValueBounds( 1000.0f, 0.0f, 720.0f, 0.0f );

   // Stop Effect.
   stopEffect(false, false);

   // Flag Initialised.
   mInitialised = true;
}


//-----------------------------------------------------------------------------
// Is Emitter Valid?
//-----------------------------------------------------------------------------
bool t2dParticleEffect::isEmitterValid( SimObjectId emitterID ) const
{
   // Search for emitter ID.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      if ( mParticleEmitterList[n].mObjectId == emitterID )
         // Return "Valid".
         return true;

   // Return "Not Valid".
   return false;
}


//-----------------------------------------------------------------------------
// Find Emitter Index.
//-----------------------------------------------------------------------------
S32 t2dParticleEffect::findEmitterIndex( SimObjectId emitterID ) const
{
   // Search for emitter ID.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      if ( mParticleEmitterList[n].mObjectId == emitterID )
         // Return Index.
         return n;

   // Return "Not Found".
   return -1;
}


//-----------------------------------------------------------------------------
// Add Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, addEmitter, S32, 2, 3, "(emitter) - Adds an Emitter to the Effect creating a new one if necessary.\n"
			  "@return On success it returns the ID of the emitter, or 0 if failed.")
{
   t2dParticleEmitter* emitter = NULL;
   if (argc > 2)
      emitter = dynamic_cast<t2dParticleEmitter*>(Sim::findObject(argv[2]));

   // Add Emitter.
   t2dParticleEmitter* newEmitter = object->addEmitter(emitter);
   return newEmitter ? newEmitter->getId() : 0;
}

// Add Emitter.
t2dParticleEmitter* t2dParticleEffect::addEmitter( t2dParticleEmitter* pEmitter )
{
   // Stop the Effect (if it's playing).
   if ( mEffectPlaying )
      stopEffect(false, false);

   if (!pEmitter)
   {
      // Create new Particle Emitter.
      pEmitter = new t2dParticleEmitter();

      // Register the Object.
      pEmitter->registerObject();

      // Initialise.
      pEmitter->initialise( this );

      // Format and Set Default Emitter Name.
      char emitterName[256];
      dSprintf( emitterName, 256, "Emitter_%d", mEmitterSerial++ );
      pEmitter->setEmitterName( emitterName );
   }

   // Add to Emitter List.
   tEmitterHandle emitterItem = { pEmitter->getId(), (SimObject*)pEmitter };
   mParticleEmitterList.push_back( emitterItem );

   // Return Emitter ID.
   return pEmitter;
}


//-----------------------------------------------------------------------------
// Remove Emitter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, removeEmitter, void, 3, 4, "(emitterObject, [bool deleteEmitter]) - Removes an Emitter from the Effect.\n"
			  "@return No return value.")
{
   bool deleteEmitter = true;
   if (argc > 3)
      deleteEmitter = dAtob(argv[3]);

   // Remove Emitter.
   object->removeEmitter( dAtoi(argv[2]), deleteEmitter );
}
// Remove Emitter.
void t2dParticleEffect::removeEmitter( SimObjectId emitterID, bool deleteEmitter )
{
   // Check Emitter Index.
   if ( !isEmitterValid(emitterID) )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::removeEmitter() - Invalid Emitter Id! (%d)", emitterID);
      return;
   }

   // Stop the Effect (if it's playing).
   if ( mEffectPlaying )
      stopEffect(false, false);

   // Find Emitter Index.
   S32 index = findEmitterIndex(emitterID);

   // Stop if we can't find this emitter.
   if ( index == -1 )
      return;

   // Destroy Emitter.
   if (deleteEmitter)
   {
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[index].mObjectId ) );
      if( pEmitter )
         pEmitter->deleteObject();
   }

   // Remove from Emitter List.
   mParticleEmitterList.erase( index );
}


//-----------------------------------------------------------------------------
// Clear Emitters.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, clearEmitters, void, 2, 2, "() Clear all Emitters from the Effect.\n"
			  "@return No return Value.")
{
   // Clear Emitters.
   object->clearEmitters();
}
// Clear Emitters.
void t2dParticleEffect::clearEmitters( void )
{
   // Stop Effect.
   stopEffect(false, false);

   // Destroy All Emitters.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
   {
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
      if( pEmitter )
         pEmitter->deleteObject();
   }

   // Clear Emitter List.
   mParticleEmitterList.clear();
}

//-----------------------------------------------------------------------------
// Get Emitter Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getEmitterCount, S32, 2, 2, "() Gets Emitter Count for Effect.\n"
			  "@return Returns the number of emitters as an integer.")
{
   // Get Emitter Count.
   return object->getEmitterCount();
}
// Get Emitter Count.
S32 t2dParticleEffect::getEmitterCount( void ) const
{
   // Get Emitter Count.
   return mParticleEmitterList.size();
}


//-----------------------------------------------------------------------------
// Find Emitter Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, findEmitterObject, S32, 3, 3, "(emitterName$) Finds Emitter Object by name from Effect.\n"
			  "@param emitterName The name of the desired effect emitter\n"
			  "@return The emitter's name as a string.")
{
   // Find Emitter Object.
   return object->findEmitterObject( argv[2] );
}
// Find Emitter Object.
SimObjectId t2dParticleEffect::findEmitterObject( const char* emitterName ) const
{
   // Any Emitters?
   if ( getEmitterCount() > 0 )
   {
      // Yes, so insert search name into String-Table.
      StringTableEntry searchName = StringTable->insert( emitterName );

      // Search for Emitter.
      for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      {
         t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
         if( pEmitter && searchName == pEmitter->getEmitterName() )
            // Yes, so return Emitter ID.
            return mParticleEmitterList[n].mObjectId;
      }
   }

   // No Emitter Found.
   return -1;
}


//-----------------------------------------------------------------------------
// Get Emitter Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getEmitterObject, S32, 3, 3, "(emitterIndex) Gets Emitter Object from Effect.\n"
			  "@param emitterIndex The index value for the desired emitter\n"
			  "@return The object ID")
{
   // Get Emitter Object.
   return object->getEmitterObject( dAtoi(argv[2]) );
}
// Get Emitter Object.
SimObjectId t2dParticleEffect::getEmitterObject( S32 index ) const
{
   // Check Emitter Index.
   if ( index >= getEmitterCount() )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::getEmitterId() - Invalid Emitter Index (%d)", index);
      return 0;
   }

   // Return Emitter ID.
   return mParticleEmitterList[index].mObjectId;
}


//-----------------------------------------------------------------------------
// Move Emitter Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, moveEmitter, void, 4, 4, "(fromEmitterIndex, toEmitterIndex) Moves the Emitter Object.\n"
			  "@param fromEmitterIndex Original index of desired emitter\n"
			  "@param toEmitterIndex Desired destination index.\n"
			  "@return No return value.")
{
   // Move Emitter Object.
   object->moveEmitter( dAtoi(argv[2]), dAtoi(argv[3]) );
}
// Move Emitter Object.
void t2dParticleEffect::moveEmitter( S32 fromIndex, S32 toIndex )
{
   // Check From Emitter Index.
   if ( fromIndex < 0 || fromIndex >= getEmitterCount() )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::moveEmitter() - Invalid From-Emitter-Index (%d)", fromIndex);
      return;
   }

   // Check To Emitter Index.
   if ( toIndex < 0 || toIndex >= getEmitterCount() )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::moveEmitter() - Invalid To-Emitter-Index (%d)", toIndex);
      return;
   }


   // We need to skip an object if we're inserting above the object.
   if ( toIndex > fromIndex )
      toIndex++;
   else
      fromIndex++;

   // Fetch Emitter to be moved.
   typeEmitterVector::iterator fromItr = (mParticleEmitterList.address()+fromIndex);

   // Fetch Emitter to be inserted at.
   typeEmitterVector::iterator toItr = (mParticleEmitterList.address()+toIndex);

   // Insert Object at new Position.
   mParticleEmitterList.insert( toItr, (*fromItr) );

   // Remove Original Reference.
   mParticleEmitterList.erase( fromItr );
}



//-----------------------------------------------------------------------------
// Select Graph.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, selectGraph, void, 3, 3, "(graphName) Select Graph by Name.\n"
			  "@param graphName Name of desired graph\n"
			  "@return No return value.")
{
   // Select Graph.
   object->selectGraph( argv[2] );
}
// Select Graph.
void t2dParticleEffect::selectGraph( const char* graphName )
{
   // Find and Selected Graph.
   mpCurrentGraph = findGraphSelection( graphName );

   // Was it a registered graph?
   if ( mpCurrentGraph )
   {
      // Yes, so store name.
      mCurrentGraphName = StringTable->insert( graphName );
   }
   else
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::selectGraph() - Invalid Graph Selected! (%s)", graphName );
      return;
   }
}


//-----------------------------------------------------------------------------
// Add Data Key.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, addDataKey, S32, 4, 4, "(time, value) Add Data-Key to Graph.\n"
			  "@param time The time key\n"
			  "@param value The value at given time\n"
			  "@return Returns the index of the Data-Key or -1 on failure.")
{
   // Add Data Key.
   return object->addDataKey( dAtof(argv[2]), dAtof(argv[3]) );
}
// Add Data Key.
S32 t2dParticleEffect::addDataKey( F32 time, F32 value )
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::addDataKey() - No Graph Selected!" );
      return -1;
   }

   // Add Data Key.
   return mpCurrentGraph->addDataKey( time, value );
}


//-----------------------------------------------------------------------------
// Remove Data Key.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, removeDataKey, bool, 3, 3, "(keyIndex) Remove Data-Key from Graph.\n"
			  "@param keyIndex The index of the data-key you want to remove\n"
			  "@return Returns true on success and false otherwise.")
{
   // Remove Data Key.
   return object->removeDataKey( dAtoi(argv[2]) );
}
// Remove Data Key.
bool t2dParticleEffect::removeDataKey( S32 index )
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::removeDataKey() - No Graph Selected!" );
      return false;
   }

   // Remove Data Key.
   return mpCurrentGraph->removeDataKey( index );
}


//-----------------------------------------------------------------------------
// Clear Data Keys.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, clearDataKeys, bool, 2, 2, "() Clears all Data-Key(s) from Graph.\n"
			  "@return Returns true on success (false means you do not have a graph selected).")
{
   // Clear Data Keys
   return object->clearDataKeys();
}
// Clear Data Keys
bool t2dParticleEffect::clearDataKeys( void )
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::clearDataKeys() - No Graph Selected!" );
      return false;
   }

   // Clear Data Keys
   mpCurrentGraph->clearDataKeys();

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Set Data Key.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, setDataKeyValue, bool, 4, 4, "(keyIndex, value) Set Data-Key Value in Graph.\n"
			  "@param keyIndex The index of the key you wish to modify.\n"
			  "@param value The value you wish to reset the given data-key to.\n"
			  "@return Returns true on success and false otherwise.")
{
   // Set Data Key.
   return object->setDataKey( dAtoi(argv[2]), dAtof(argv[3]) );
}
// Set Data Key.
bool t2dParticleEffect::setDataKey( S32 index, F32 value )
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::setDataKey() - No Graph Selected!" );
      return false;
   }

   // Set Data Key.
   return mpCurrentGraph->setDataKeyValue( index, value );
}



//-----------------------------------------------------------------------------
// Get Data Key
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getDataKey, const char*, 3, 3, "(keyIndex) Get Data-Key Time/Value from Graph.\n"
			  "@param keyIndex The index of the desired data-key\n"
			  "@return Returns the data-key as a string formatted as \"time value\" or false if failed")
{
   // Fetch Key Index.
   S32 keyIndex = dAtoi(argv[2]);

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);
   // Format Buffer.
   dSprintf(pBuffer, 32, "%f %f", object->getDataKeyTime( keyIndex ), object->getDataKeyValue( keyIndex ) );
   // Return buffer.
   return pBuffer;
}
// Get Data Key Value.
F32 t2dParticleEffect::getDataKeyValue( S32 index ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getDataKeyValue() - No Graph Selected!" );
      return false;
   }

   // Get Data Key Value.
   return mpCurrentGraph->getDataKeyValue( index );
}
// Get Data Key Time.
F32 t2dParticleEffect::getDataKeyTime( S32 index ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getDataKeyTime() - No Graph Selected!" );
      return false;
   }

   // Get Data Key Time.
   return mpCurrentGraph->getDataKeyTime( index );
}



//-----------------------------------------------------------------------------
// Get Data Key Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getDataKeyCount, S32, 2, 2, "() \n @return Returns the current Data-Key count from selected graph (or false if failed).")
{
   // Get Data Key Count.
   return object->getDataKeyCount();
}
// Get Data Key Count.
U32 t2dParticleEffect::getDataKeyCount( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getDataKeyCount() - No Graph Selected!" );
      return false;
   }

   // Get Data Key Count.
   return mpCurrentGraph->getDataKeyCount();
}


//-----------------------------------------------------------------------------
// Get Min Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getMinValue, F32, 2, 2, "() \n @return Returns Min-Value from Graph (or false if failed).")
{
   // Get Min Value.
   return object->getMinValue();
}
// Get Min Value.
F32 t2dParticleEffect::getMinValue( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getMinValue() - No Graph Selected!" );
      return false;
   }

   // Get Min Value.
   return mpCurrentGraph->getMinValue();
}


//-----------------------------------------------------------------------------
// Get Max Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getMaxValue, F32, 2, 2, "() \n @return Max-Value from Graph (or false if failed).")
{
   // Get Max Value.
   return object->getMaxValue();
}
// Get Max Value.
F32 t2dParticleEffect::getMaxValue( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getMaxValue() - No Graph Selected!" );
      return false;
   }

   // Get Max Value.
   return mpCurrentGraph->getMaxValue();
}


//-----------------------------------------------------------------------------
// Get Min Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getMinTime, F32, 2, 2, "() \n @return Returns Min-Time from Graph (or false if failed).")
{
   // Get Min Time.
   return object->getMinTime();
}
// Get Min Time.
F32 t2dParticleEffect::getMinTime( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getMinTime() - No Graph Selected!" );
      return false;
   }

   // Get Min Time.
   return mpCurrentGraph->getMinTime();
}


//-----------------------------------------------------------------------------
// Get Max Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getMaxTime, F32, 2, 2, "() \n @return Returns Max-Value from Graph (or false if failed).")
{
   // Get Max Time.
   return object->getMaxTime();
}
// Get Max Time.
F32 t2dParticleEffect::getMaxTime( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getMaxTime() - No Graph Selected!" );
      return false;
   }

   // Get Max Time.
   return mpCurrentGraph->getMaxTime();
}


//-----------------------------------------------------------------------------
// Get Graph Value.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getGraphValue, F32, 3, 3, "(time) Get value at given time from selected graph.\n"
			  "@param time The desired graph time step\n"
			  "@return Returns the value set at given time (or false if failed).")
{
   // Get Graph Value.
   return object->getGraphValue( dAtof(argv[2]) );
}
// Get Graph Value.
F32 t2dParticleEffect::getGraphValue( F32 time ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getGraphValue() - No Graph Selected!" );
      return false;
   }

   // Get Graph Value.
   return mpCurrentGraph->getGraphValue( time );
}


//-----------------------------------------------------------------------------
// Set Time Repeat.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, setTimeRepeat, bool, 3, 3, "(timeRepeat) Set Time-Repeat For Graph.\n"
			  "@return Returns true on success (or false if failed).")
{
   // Set Time Repeat.
   return object->setTimeRepeat( dAtof(argv[2]) );
}
// Set Time Repeat.
bool t2dParticleEffect::setTimeRepeat( const F32 timeRepeat )
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::setTimeRepeat() - No Graph Selected!" );
      return false;
   }

   // Set Time Repeat.
   return mpCurrentGraph->setTimeRepeat( timeRepeat );
}


//-----------------------------------------------------------------------------
// Get Time Repeat.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getTimeRepeat, F32, 2, 2, "() \n @return Returns Time-Repeat for Graph (or false if failed).")
{
   // Get Time Repeat.
   return object->getTimeRepeat();
}
// Get Time Repeat.
F32 t2dParticleEffect::getTimeRepeat( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getTimeRepeat() - No Graph Selected!" );
      return false;
   }

   // Get Time Repeat.
   return mpCurrentGraph->getTimeRepeat();
}


//-----------------------------------------------------------------------------
// Set Value Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, setValueScale, bool, 3, 3, "(valueScale) - Set Value-Scale For Graph.\n"
			  "@return Returns true on success (or false if failed).")
{
   // Set Value Scale.
   return object->setValueScale( dAtof(argv[2]) );
}
// Set Value Scale.
bool t2dParticleEffect::setValueScale( const F32 valueScale )
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::setValueScale() - No Graph Selected!" );
      return false;
   }

   // Set Value Scale.
   return mpCurrentGraph->setValueScale( valueScale );
}


//-----------------------------------------------------------------------------
// Get Value Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getValueScale, F32, 2, 2, "() \n @return Returns Value-Scale for Graph (or false if failed).")
{
   // Get Value Scale.
   return object->getValueScale();
}
// Get Value Scale.
F32 t2dParticleEffect::getValueScale( void ) const
{
   // Have we got a valid Graph Selected?
   if ( !mpCurrentGraph )
   {
      // No, so warn.
      Con::warnf( "t2dParticleEffect::getValueScale() - No Graph Selected!" );
      return false;
   }

   // Get Value Scale.
   return mpCurrentGraph->getValueScale();
}


//-----------------------------------------------------------------------------
// Find Particle Peak.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, findParticlePeak, const char*, 5, 5, "(searchTime, timeStep, peakLimit) Finds the time of a particle effect at which there are a peak number of particles emitted.\n"
			  "@param searchTime The amount of time into the effect to search.\n"
			  "@param timeStep The time step at which the effect will be checked. Smaller time steps will give more accurate results, but take more time.\n"
			  "@param peakLimit The maximum number of particles to find. The search will stop when this number of particles is found.\n"
			  "@return On success it returns a string formatted as \"true peakCount peakTime\". On fail it returns \"false\"")
{
   // Fetch Move-Time.
   const F32 moveTime = dAtof(argv[2]);
   // Fetch Time-Step.
   const F32 timeStep = dAtof(argv[3]);
   // Fetch Peak-Limit.
   const U32 peakLimit = dAtoi(argv[4]);

   // Reset Maximum Particle Tracking.
   U32 peakCount;
   F32 peakTime;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);

   // Find Particle Peak.
   if ( object->findParticlePeak( moveTime, timeStep, peakLimit, peakCount, peakTime ) )
   {
      // Okay, so format Buffer.
      dSprintf(pBuffer, 32, "%s %d %f", "true", peakCount, peakTime);
   }
   else
   {
      // Problem, so format Buffer.
      dSprintf(pBuffer, 32, "false");
   }

   // Return Buffer.
   return pBuffer;
}
// Find Particle Peak.
bool t2dParticleEffect::findParticlePeak( const F32 searchTime, const F32 timeStep, const U32 peakLimit, U32& peakCount, F32& peakTime )
{
   // Cannot do anything if we've not got any emitters!
   if ( mParticleEmitterList.size() == 0 )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::findParticlePeak() - Cannot Play; no emitters!");
      return false;
   }

   // Are we in a Scene?
   if ( getSceneGraph() == NULL )
   {
      // No, so warn.
      Con::warnf("t2dParticleEffect::findParticlePeak() - Cannot Play; not in a scene!");
      return false;
   }

   // Check Search-time.
   if ( mLessThanZero( searchTime ) )
   {
      // Problem, so warn.
      Con::warnf("t2dParticleEffect::findParticlePeak() - Search-Time is invalid!");
      return false;
   }

   // Check Time-Step.
   if ( mIsZero( timeStep ) || mGreaterThan( timeStep, searchTime ) )
   {
      // Problem, so warn.
      Con::warnf("t2dParticleEffect::findParticlePeak() - Time-Step is invalid!");
      return false;
   }

   // Reset Effect Age.
   mEffectAge = 0.0f;

   // Play All Emitters.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
   {
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
      if( pEmitter )
         pEmitter->playEmitter( true );
   }

   // Reset Waiting for Particles.
   mWaitingForParticles = false;
   // Reset Waiting for delete.
   mWaitingForDelete = false;

   // Flag as Playing.
   mEffectPlaying = true;

   // Set Unsafe Delete!
   setSafeDelete(false);

   // Now we'll move to the selected position using the selected intervals...

   // Calculate Number of intervals.
   const U32 intervalCount = U32(mFloor(searchTime / timeStep));
   // Calculate Final sub-interval.
   const F32 finalIntervalTime = searchTime - ( intervalCount * timeStep );

   // Debug Statistics.
   CDebugStats debugStats;
   // Get Quicker Reference to active-particle count.
   U32& particlesActive = debugStats.objectParticlesActive;

   // Reset Scene Time.
   F32 sceneTime = 0.0f;

   // Reset "Found" Flag.
   bool limitFound = false;

   // Reset Maximum Particle Tracking.
   peakCount = 0;
   peakTime = 0.0f;

   // Step the appropriate number of intervals.
   for ( U32 i = 0; i < intervalCount; ++i )
   {
      // Move Scene Time.
      sceneTime += timeStep;

      // Reset Particles Active.
      particlesActive = 0;

      // Update All Emitters for the main intervals.
      for ( U32 n = 0; n < mParticleEmitterList.size(); ++n )
      {
         // Integrate Object.
         t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
         if( pEmitter )
            pEmitter->integrateObject( sceneTime, timeStep, &debugStats );
      }

      // Check Peak Particle Count.
      if ( particlesActive > peakCount )
      {
         // Check we don't go over the limit.
         if ( particlesActive > peakLimit )
         {
            // We have so flag "found".
            limitFound = true;
            break;
         }

         // Set peak-count/time.
         peakCount = particlesActive;
         peakTime = sceneTime;
      }
   }

   // Only continue searching if we've not found the limit.
   if ( !limitFound )
   {
      // Move Scene Time.
      sceneTime += finalIntervalTime;

      // Reset Particles Active.
      particlesActive = 0;

      // Update All Emitters for the final sub-interval.
      for ( U32 n = 0; n < mParticleEmitterList.size(); ++n )
      {
         // Integrate Object.
         t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
         if( pEmitter )
            pEmitter->integrateObject( sceneTime, finalIntervalTime, &debugStats );
      }

      // Check Peak Particle Count.
      if ( particlesActive > peakCount )
      {
         // Check we don't go over the limit.
         if ( particlesActive > peakLimit )
         {
            // We have so flag "found".
            limitFound = true;
         }
         else
         {
            // Set peak-count/time.
            peakCount = particlesActive;
            peakTime = sceneTime;
         }
      }
   }

   // Stop All Emitters.
   stopEffect( false, false );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Move Effect To.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, moveEffectTo, const char*, 4, 4, "(moveTime, timeStep) Moves an effect's playback to a particular time.\n"
			  "@param moveTime The time to move the effect to.\n"
			  "@param timeStep The time steps at which the effect will be processed in moving to the move time.\n"
			  "@return On success it returns a string formatted as \"true peakCount peakTime\". On fail it returns \"false\"")
{
   // Fetch Move-Time.
   const F32 moveTime = dAtof(argv[2]);
   // Fetch Time-Step.
   const F32 timeStep = dAtof(argv[3]);

   U32 peakCount;
   F32 peakTime;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);

   // Move Effect To.
   if ( object->moveEffectTo( moveTime, timeStep, peakCount, peakTime ) )
   {
      // Okay, so format Buffer.
      dSprintf(pBuffer, 32, "%s %d %f", "true", peakCount, peakTime);
   }
   else
   {
      // Problem, so format Buffer.
      dSprintf(pBuffer, 32, "false");
   }

   // Return Buffer.
   return pBuffer;
}
// Move Effect To.
bool t2dParticleEffect::moveEffectTo( const F32 moveTime, const F32 timeStep, U32& peakCount, F32& peakTime )
{
   // Cannot do anything if we've not got any emitters!
   if ( mParticleEmitterList.size() == 0 )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::MoveEffectTo() - Cannot Play; no emitters!");
      return false;
   }

   // Are we in a Scene?
   if ( getSceneGraph() == NULL )
   {
      // No, so warn.
      Con::warnf("t2dParticleEffect::MoveEffectTo() - Cannot Play; not in a scene!");
      return false;
   }

   // Check Move-time.
   if ( mLessThanZero( moveTime ) )
   {
      // Problem, so warn.
      Con::warnf("t2dParticleEffect::MoveEffectTo() - Move-Time is invalid!");
      return false;
   }

   // Check Time-Step.
   if ( mIsZero( timeStep ) || mGreaterThan( timeStep, moveTime ) )
   {
      // Problem, so warn.
      Con::warnf("t2dParticleEffect::MoveEffectTo() - Time-Step is invalid!");
      return false;
   }

   // Reset Effect Age.
   mEffectAge = 0.0f;

   // Play All Emitters.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
   {
      t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
      if( pEmitter )
         pEmitter->playEmitter( true );
   }

   // Reset Waiting for Particles.
   mWaitingForParticles = false;
   // Reset Waiting for delete.
   mWaitingForDelete = false;

   // Flag as Playing.
   mEffectPlaying = true;

   // Set Unsafe Delete!
   setSafeDelete(false);

   // Now we'll move to the selected position using the selected intervals...

   // Calculate Number of intervals.
   const U32 intervalCount = U32(mFloor(moveTime / timeStep));
   // Calculate Final sub-interval.
   const F32 finalIntervalTime = moveTime - ( intervalCount * timeStep );

   // Debug Statistics.
   CDebugStats debugStats;
   // Get Quicker Reference to active-particle count.
   U32& particlesActive = debugStats.objectParticlesActive;

   // Reset Scene Time.
   F32 sceneTime = 0.0f;

   // Reset Maximum Particle Tracking.
   peakCount = 0;
   peakTime = 0.0f;

   // Step the appropriate number of intervals.
   for ( U32 i = 0; i < intervalCount; ++i )
   {
      // Move Scene Time.
      sceneTime += timeStep;

      // Reset Particles Active.
      particlesActive = 0;

      // Update All Emitters for the main intervals.
      for ( U32 n = 0; n < mParticleEmitterList.size(); ++n )
      {
         // Integrate Object.
         t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
         if( pEmitter )
            pEmitter->integrateObject( sceneTime, timeStep, &debugStats );
      }

      // Check Peak Particle Count.
      if ( particlesActive > peakCount )
      {
         peakCount = particlesActive;
         peakTime = sceneTime;
      }
   }

   // Move Scene Time.
   sceneTime += finalIntervalTime;

   // Reset Particles Active.
   particlesActive = 0;

   // Update All Emitters for the final sub-interval.
   for ( U32 n = 0; n < mParticleEmitterList.size(); ++n )
   {
      // Integrate Object.
      t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
      if( pEmitter )
         pEmitter->integrateObject( sceneTime, finalIntervalTime, &debugStats );
   }

   // Check Peak Particle Count.
   if ( particlesActive > peakCount )
   {
      peakCount = particlesActive;
      peakTime = sceneTime;
   }

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Play Effect.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, playEffect, bool, 2, 3, "([resetParticles]) Plays the Particle Effect.\n"
			  "@param resetParticles Flag for whether to reset particles before playback (default true)\n"
			  "@return Returns true on success and false otherwise")
{
   // Calculate Reset-Particles Flag.
   bool resetParticles = argc >= 3 ? dAtob(argv[2]) : true;

   // Play Effect.
   return object->playEffect( resetParticles );
}
// Play Effect.
bool t2dParticleEffect::playEffect( bool resetParticles )
{
   // Cannot do anything if we've not got any emitters!
   if ( mParticleEmitterList.size() == 0 )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::playEffect() - Cannot Play; no emitters!");
      return false;
   }

   // Are we in a Scene?
   if ( getSceneGraph() == NULL )
   {
      // No, so warn.
      Con::warnf("t2dParticleEffect::playEffect() - Cannot Play; not in a scene!");
      return false;
   }

   // Reset Effect Age.
   mEffectAge = 0.0f;

   // Play All Emitters.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
   {
      t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
      if( pEmitter )
         pEmitter->playEmitter( resetParticles );
   }

   // Reset Waiting for Particles.
   mWaitingForParticles = false;
   // Reset Waiting for delete.
   mWaitingForDelete = false;

   // Flag as Playing.
   mEffectPlaying = true;

   // Set Unsafe Delete!
   setSafeDelete(false);

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Stop Effect.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, stopEffect, void, 2, 4, "([waitForParticles?, killEffect?]) - Stops the Particle Effect.\n"
			  "@param waitForParticles Whether or not the effect should wait until all of its particles have run their course, or just stop immediately and delete the particles (default true).\n"
			  "@param killEffect Whether or not the effect should be deleted after it has stopped (default false).\n"
			  "@return No return value.")
{
   // Calculate Options.
   bool waitForParticles   = argc >= 3 ? dAtob(argv[2]) : true;
   bool killEffect         = argc >= 4 ? dAtob(argv[3]) : false;

   // Stop Effect.
   object->stopEffect( waitForParticles, killEffect );
}
// Stop Effect.
void t2dParticleEffect::stopEffect( bool waitForParticles, bool killEffect )
{
   // Ignore if we're not playing and there's no kill command.
   if ( !mEffectPlaying && !killEffect )
      return;

   // Are we waiting for particles to end?
   if ( waitForParticles )
   {
      // Yes, so pause all emitters.
      for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      {
         t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
         if( pEmitter )
            pEmitter->pauseEmitter();
      }

      // Flag Waiting for Particles.
      mWaitingForParticles = true;

      // Flag as waiting for deletion if killing effect.
      if ( killEffect )
         mWaitingForDelete = true;
   }
   else
   {
      // No, so stop All Emitters.
      for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      {
         // Fetch Particle Emitter Pointer.
         t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( mParticleEmitterList[n].mObjectId ) );
         if( pEmitter )
            pEmitter->stopEmitter();
      }

      // Reset Effect Age.
      mEffectAge = 0.0f;

      // Flag as Stopped and not waiting.
      mEffectPlaying = mWaitingForParticles = mWaitingForDelete = false;

      // Set Safe Delete.
      setSafeDelete(true);

      // Perform "OnStopEffect" Callback.
      if( isMethod( "onStopEffect" ) )
         Con::executef( this, 1, "onStopEffect" );

      // Flag for immediate Deletion if killing.
      if ( killEffect )
         safeDelete();
   }
}

ConsoleMethod(t2dParticleEffect, getIsEffectPlaying, bool, 2, 2, "() \n @return Returns true if effect is playing")
{
   return object->getIsEffectPlaying();
}

//-----------------------------------------------------------------------------
// Set Effect-Life Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, setEffectLifeMode, void, 3, 4, "(lifeMode, [time]) - Sets the Effect Life Mode/Time.\n"
			  "@param lifeMode The lifemode of the effect (either INFINITE CYCLE KILL or STOP)\n"
			  "@param time The lifetime of the effect. This has different effects depending on the life mode (required for CYCLE or KILL)."
			  "@return No return value.")
{
   // Fetch Effect-Life Mode.
   t2dParticleEffect::eEffectLifeMode lifeMode = getEffectMode( argv[2] );

   // Check for Invalid Arguments.
   if (    (lifeMode == t2dParticleEffect::CYCLE || lifeMode == t2dParticleEffect::KILL) &&
      (argc < 4) )
   {
      // Missing Parameter so Warn.
      Con::warnf( "t2dParticleEffect::setEffectLifeMode() - Missing 'time' parameter for selected mode!" );
      return;
   }

   // Check for Invalid Life Argument.
   if (    (lifeMode == t2dParticleEffect::CYCLE || lifeMode == t2dParticleEffect::KILL) &&
      dAtof(argv[3]) <= 0.0f )
   {
      // Missing Parameter so Warn.
      Con::warnf( "t2dParticleEffect::setEffectLifeMode() - 'time' parameter has to be greater than zero!" );
      return;
   }

   // Handle Effect-Life Mode Appropriately.
   switch( lifeMode )
   {
      // Infinite-Life Mode.
   case t2dParticleEffect::INFINITE:
      {
         // Set Effect Life Mode.
         object->setEffectLifeMode( t2dParticleEffect::INFINITE, 0.0f );

      } break;

      // Cycle-Life Mode.
   case t2dParticleEffect::CYCLE:
      {
         // Set Effect Life Mode.
         object->setEffectLifeMode( t2dParticleEffect::CYCLE, dAtof(argv[3]) );

      } break;

      // Kill-Life Mode.
   case t2dParticleEffect::KILL:
      {
         // Set Effect Life Mode.
         object->setEffectLifeMode( t2dParticleEffect::KILL, dAtof(argv[3]) );

      } break;

      // Stop-Life Mode.
   case t2dParticleEffect::STOP:
      {
         // Set Effect Life Mode.
         object->setEffectLifeMode( t2dParticleEffect::STOP, dAtof(argv[3]) );

      } break;
   }
}
// Set Effect-Life Mode.
void t2dParticleEffect::setEffectLifeMode( eEffectLifeMode lifeMode, F32 time )
{
   // Set Effect-Life Mode.
   mEffectLifeMode = lifeMode;
   // Set Effect Lifetime.
   mEffectLifetime = time;
}


//-----------------------------------------------------------------------------
// Get Effect-Life Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getEffectLifeMode, const char*, 2, 2, "() \n @return Returns the Effect Life Mode (either INFINITE CYCLE KILL or STOP).")
{
   // Fetch Effect Life Mode.
   t2dParticleEffect::eEffectLifeMode lifeMode = object->getEffectLifeMode();

   // Search for Mnemonic.
   for(U32 i = 0; i < (sizeof(effectLifeLookup) / sizeof(effectLifeLookup[0])); i++)
      if( effectLifeLookup[i].index == lifeMode )
      {
         // Create Returnable Buffer.
         char* pBuffer = Con::getReturnBuffer(128);
         // Format Buffer.
         dSprintf(pBuffer, 128, "%s %f", effectLifeLookup[i].label, object->getEffectLifeTime() == t2dParticleEffect::INFINITE ? 0.0f : object->getEffectLifeTime());
         // Return buffer.
         return pBuffer;
      }

      // Bah!
      return NULL;
}


//-----------------------------------------------------------------------------
// Set Effect Collision Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, setEffectCollisionStatus, void, 3, 3, "(status) Set the effect collision status.\n"
			  "@param status True if the effect as a whole is processing collisions, false if the individual particles are processing collisions.\n"
			  "@return No return value.")
{
   // Set Effect Collision Status.
   object->setEffectCollisionStatus( dAtob(argv[2]) );
}
// Set Effect Collision Status.
void t2dParticleEffect::setEffectCollisionStatus( const bool status )
{
   // Set Effect Collision Status.
   mUseEffectCollisions = status;
}


//-----------------------------------------------------------------------------
// Get Effect Collision Status.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, getEffectCollisionStatus, bool, 2, 2, "() \n @return Returns the effect collision status (true if effect processes collisions, or false if individual particles are).")
{
   // Get Effect Collision Status.
   return object->getEffectCollisionStatus();
}
// Get Effect Collision Status.
bool t2dParticleEffect::getEffectCollisionStatus( void )
{
   // Get Effect Collision Status.
   return mUseEffectCollisions;
}


//-----------------------------------------------------------------------------
// Load Effect.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, loadEffect, bool, 3, 3, "(effectFile$) Loads a Particle Effect.\n"
			  "@param effectFile File name from which to load desired effect\n"
			  "@return Returns true on success.")
{
   // Load Effect.
   return object->loadEffect( argv[2] );
}
// Load Effect.
bool t2dParticleEffect::loadEffect( const char* effectFile )
{
   // Are we in a Scene?
   if ( getSceneGraph() == NULL )
   {
      // No, so warn.
      Con::warnf("t2dParticleEffect::loadEffect() - Cannot Load-Effect; not in a scene!");
      // Return Error.
      return false;
   }

   // Expand relative paths.
   char buffer[1024];
   if ( effectFile )
      if ( Con::expandScriptFilename( buffer, sizeof( buffer ), effectFile ) )
         effectFile = buffer;

   // Open Effect File.
   Stream* pStream = ResourceManager->openStream( effectFile );
   // Check Stream.
   if ( !pStream )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::loadEffect() - Could not Open File '%s' for Effect Load.", effectFile);
      // Return Error.
      return false;
   }

   // Scene Objects.
   Vector<t2dSceneObject*> ObjReferenceList;

   // Set Vector Associations.
   VECTOR_SET_ASSOCIATION( ObjReferenceList );

   // Load Stream.
   if ( !loadStream( *pStream, getSceneGraph(), ObjReferenceList, true  ) )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::loadEffect() - Error Loading Effect/Emitter(s)!");
      // Return Error.
      return false;
   }

   // Reset Any Lifetime Counter.
   setLifetime( 0.0f );

   // Close Stream.
   ResourceManager->closeStream( pStream );

   mEffectFile = StringTable->insert(effectFile);

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Save Effect.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dParticleEffect, saveEffect, bool, 3, 3, "(effectFile$) Saves a Particle Effect to file.\n"
			  "@param effectFile The name of the file to save the effect to.\n"
			  "@return Returns true on success and false otherwise.")
{
   // Save Effect.
   return object->saveEffect( argv[2] );
}
// Save Effect.
bool t2dParticleEffect::saveEffect( const char* effectFile )
{
   // Are we in a Scene?
   if ( getSceneGraph() == NULL )
   {
      // No, so warn.
      Con::warnf("t2dParticleEffect::saveEffect() - Cannot Save-Effect; not in a scene!");
      // Return Error.
      return false;
   }

   // Expand relative paths.
   char buffer[1024];
   if ( effectFile )
      if ( Con::expandToolScriptFilename( buffer, sizeof( buffer ), effectFile ) )
         effectFile = buffer;

   // Open Effect File.
   FileStream fileStream;
   if ( !ResourceManager->openFileForWrite( fileStream, effectFile, FileStream::Write ) )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::saveEffect() - Could not open File '%s' for Effect Save.", effectFile);
      // Return Error.
      return false;
   }

   // Stop Effect.
   // NOTE:-   We do this so that we don't save active emitter particles.
   stopEffect(false, false);

   // Save Stream.
   if ( !saveStream( fileStream, getSceneGraph()->getNextSerialiseID(), 1 ) )
   {
      // Warn.
      Con::warnf("t2dParticleEffect::saveEffect() - Error Saving Effect/Emitter(s)!");
      // Return Error.
      return false;
   }

   Con::executef( this, 1, "onEffectSaved" );

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Check Collision Send.
//-----------------------------------------------------------------------------
bool t2dParticleEffect::checkCollisionSend( const F32 elapsedTime, t2dPhysics::cCollisionStatus& sendCollisionStatus, CDebugStats* pDebugStats )
{
   // Use Effect Collisions?
   if ( mUseEffectCollisions )
   {
      // Yes, so use standard core-collision.
      return Parent::checkCollisionSend( elapsedTime, sendCollisionStatus, pDebugStats );
   }


   // T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
   PROFILE_START(T2D_t2dParticleEffect_checkCollisionSend);
#endif

   // Reset Initial Collision Status.
   bool collisionStatus = false;

   // Update all emitters.
   for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
   {
      // Fetch Particle Emitter Reference.
      t2dParticleEmitter* pEmitter = dynamic_cast<t2dParticleEmitter*>( mParticleEmitterList[n].mpSceneObject );

      // Integrate Emitter (if visible).
      if ( pEmitter->getVisible() && pEmitter->getEmitterCollisionStatus() )
      {
         // Check Particle Collision.
         if ( pEmitter->checkParticleCollisions( this, elapsedTime, sendCollisionStatus, pDebugStats ) )
         {
            // Flag Collision Occurred.
            collisionStatus = true;
         }
      }
   }

   // Did a collision occur.
   if ( collisionStatus )
   {
      // Yes, so perform callback.
      Con::executef( this, 1, "onParticleCollision" );
   }

   // T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
   PROFILE_END();   // T2D_t2dParticleEffect_checkCollisionSend
#endif

   // Return No Collision Always.
   // NOTE:-   This stops the particle-effect object from ever receiving collisions itself!
   return false;
}

#ifdef PUAP_OPTIMIZE
bool RectIntersect( RectF a, RectF b ) {

	if(b.point.x > a.point.x + a.extent.x)	return false;
	if(b.point.x + b.extent.x < a.point.x)	return false;
	
	
	if(b.point.y > a.point.y + a.extent.y)	return false;
	if(b.point.y + b.extent.y < a.point.y)	return false;
	
	return true;
}   

extern t2dSceneWindow *gCurrentSceneWindow;
#endif

//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dParticleEffect::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{

#ifdef PUAP_OPTIMIZE
	//%PUAP%   optimizer  kill particles that are not on-screen
	t2dVector CameraPOS = gCurrentSceneWindow->getCurrentCameraPosition();
	t2dVector CameraWH(gCurrentSceneWindow->getCurrentCameraWidth(),
						gCurrentSceneWindow->getCurrentCameraHeight());

	CameraPOS.mX -= CameraWH.mX/2;
	CameraPOS.mY -= CameraWH.mY/2;


//now check our rect with this one
	RectF ourRect = getWorldClipRectangle();

	RectF cameraRect = RectF(CameraPOS.mX, CameraPOS.mY, CameraWH.mX, CameraWH.mY);
	
	
	if( !RectIntersect( ourRect, cameraRect ) ) {
		mWasPlaying = mEffectPlaying;
		stopEffect( false, false );
	} else {
		//if we were playing, continue
		if( mWasPlaying ) {
			mWasPlaying = false;
			if( mParticleEmitterList.size() != 0  ) {
			playEffect( false );
		}
	}
	}

	//%PUAP%
#endif

   // Call Parent.
   Parent::integrateObject( sceneTime, elapsedTime, pDebugStats );

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dParticleEffect_integrateObject);
#endif

   // Is the Effect Playing?
   if ( mEffectPlaying )
   {
      // Yes, so update Effect Age.
      mEffectAge += elapsedTime;

      // Reset Active Particle Count.
      U32 activeParticles = 0;

      // Update all emitters.
      for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      {
         // Integrate Emitter (if visible).
         t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( mParticleEmitterList[n].mpSceneObject );
         if ( pEmitter && pEmitter->getVisible() )
         {
            // Integrate Emitter.
            pEmitter->integrateObject( sceneTime, elapsedTime, pDebugStats );
            // Total Particles.
            activeParticles += pEmitter->getActiveParticles();
         }
      }

      // Only handle modes if we're not waiting for particles.
      if ( !mWaitingForParticles )
      {
         // Handle Effect-Life Mode Appropriately.
         switch( mEffectLifeMode )
         {
            // Cycle-Life Mode.
         case t2dParticleEffect::CYCLE:
            {
               // Have we expired?
               if ( mEffectAge >= mEffectLifetime )
                  // Yes, so restart effect (reset existing particles).
                  playEffect(true);

            } break;

            // Kill-Life Mode.
         case t2dParticleEffect::KILL:
            {
               // Have we expired?
               if ( mEffectAge >= mEffectLifetime )
               {
                  if( getSceneGraph() && getSceneGraph()->getIsEditorScene() )
                     stopEffect( true, false );
                  else
                     // Yes, so stop Effect and Kill.
                     stopEffect( true, true );
               }

            } break;

            // Stop-Life Mode.
         case t2dParticleEffect::STOP:
            {
               // Have we expired?
               if ( mEffectAge >= mEffectLifetime )
                  // Yes, so stop Effect.
                  stopEffect( true, false );

            } break;
         }
      }

      // Are we waiting for particles and none are active?
      if ( mWaitingForParticles && activeParticles == 0 )
      {
         // Yes, so stop effect ( take note of 'killEffect' flag ).
         stopEffect( false, mWaitingForDelete );
      }
   }

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dParticleEffect_integrateObject
#endif
}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dParticleEffect::interpolateTick( const F32 timeDelta )
{
   // Call Parent.
   Parent::interpolateTick( timeDelta );

   // Disable Particle Interpolation?
   if ( mDisableParticleInterpolation ) return;

   // Is the Effect Playing?
   if ( mEffectPlaying )
   {
      // Yes, so interpolate all emitters.
      for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      {
         // Integrate Emitter (if visible).
         t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( mParticleEmitterList[n].mpSceneObject );
         if ( pEmitter && pEmitter->getVisible() )
         {
            // Interpolate Emitter.
            pEmitter->interpolateTick( timeDelta );
         }
      }
   }
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dParticleEffect::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
   // Is the Effect Playing?
   if ( mEffectPlaying )
   {
      // Yes, so render all emitters.
      //for ( U32 n = 0; n < mParticleEmitterList.size(); n++ )
      for ( S32 n = mParticleEmitterList.size()-1; n >= 0 ; n-- )
      {
         t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( mParticleEmitterList[n].mpSceneObject );
         if ( pEmitter && pEmitter->getVisible() )
            pEmitter->renderObject( viewPort, viewIntersection );
      }

      // Call Parent.
      Parent::renderObject( viewPort, viewIntersection ); // Always use for Debug Support!
   }
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dParticleEffect )
REGISTER_SERIALISE_VERSION( t2dParticleEffect, 1, false )
REGISTER_SERIALISE_VERSION( t2dParticleEffect, 2, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dParticleEffect, 2 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dParticleEffect, 1 )
{
   // Clear Emitters.
   object->clearEmitters();

   // Load Graph Count.
   S32 graphCount;
   if ( !stream.read( &graphCount ) )
      return false;

   // Load Graphs.
   for ( U32 n = 0; n < graphCount; n++ )
   {
      // Load/Find Graph Name.
      t2dGraphField* pGraphField = object->findGraphSelection( stream.readSTString() );
      // Check Graph Field.
      if ( !pGraphField )
         return false;

      // Load Graph Object.
      if ( !pGraphField->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
         return false;
   }

   // Load Graph Selection Flag.
   bool graphSelection;
   if ( !stream.read( &graphSelection ) )
      return false;

   // Do we have a Graph Selection?
   if ( graphSelection )
      // Yes, so Read Graph Name and Select it.
      object->selectGraph( stream.readSTString() );

   // Load Emitter Count.
   S32 emitterCount;
   if ( !stream.read( &emitterCount ) )
      return false;

   // Load Emitters.
   for ( U32 n = 0; n < emitterCount; n++ )
   {
      // Add Emitter.
      object->addEmitter();

      // Find Emitter.
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( object->mParticleEmitterList[n].mObjectId ) );

      // Error?
      if( !pEmitter )
      {
         Con::warnf("t2dParticleEmitter::loadStream - Unable to properly add emitter from disk" );
         continue;
      }

      // Load Emitter.
      pEmitter->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS );

      // Is Emitter Name Empty?
      if ( dStrlen( pEmitter->getEmitterName() ) == 0 )
      {
         // Yes, so format and Set Default Emitter Name.
         char emitterName[256];
         dSprintf( emitterName, 256, "Emitter_%d", n );
         pEmitter->setEmitterName( emitterName );
      }
   }

   // Load Effect Playing Flag / Age.
   if (    !stream.read( &object->mEffectPlaying ) ||
      !stream.read( &object->mEffectAge ) ||
      !stream.read( &object->mWaitingForParticles ) ||
      !stream.read( &object->mWaitingForDelete ) ||
      !stream.read( (S32*)&object->mEffectLifeMode ) ||
      !stream.read( &object->mEffectLifetime ) )
      return false;

   // Return Okay.
   return true;
}

//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dParticleEffect, 1 )
{
   // Save Graph Count.
   if ( !stream.write( object->mGraphSelectionList.size() ) )
      return false;

   // Save Graphs.
   for ( U32 n = 0; n < object->mGraphSelectionList.size(); n++ )
   {
      // Write Graph Name.
      stream.writeString( object->mGraphSelectionList[n]->mGraphName );
      // Write Graph Object.
      if ( !object->mGraphSelectionList[n]->mpGraphObject->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
         return false;
   }

   // Save Graph Selection Flag.
   if ( !stream.write( (object->mpCurrentGraph != NULL) ) )
      return false;

   // Do we have a Graph Selection?
   if ( object->mpCurrentGraph )
      // Yes, so save Graph Selection.
      stream.writeString( object->mCurrentGraphName );

   // Save Emitter Count.
   if ( !stream.write( object->mParticleEmitterList.size() ) )
      return false;

   // Save Emitters.
   for ( U32 n = 0; n < object->mParticleEmitterList.size(); n++ )
   {
      // Find Emitter.
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( object->mParticleEmitterList[n].mObjectId ) );

      // Save Emitter.
      if( pEmitter )
         pEmitter->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS );
   }

   // Save Effect Playing Flag / Age.
   if (    !stream.write( object->mEffectPlaying ) ||
      !stream.write( object->mEffectAge ) ||
      !stream.write( object->mWaitingForParticles ) ||
      !stream.write( object->mWaitingForDelete ) ||
      !stream.write( (S32)object->mEffectLifeMode ) ||
      !stream.write( object->mEffectLifetime ) )
      return false;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// Load v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dParticleEffect, 2 )
{
   // Clear Emitters.
   object->clearEmitters();

   // Load Graph Count.
   S32 graphCount;
   if ( !stream.read( &graphCount ) )
      return false;

   // Load Graphs.
   for ( U32 n = 0; n < graphCount; n++ )
   {
      // Load/Find Graph Name.
      t2dGraphField* pGraphField = object->findGraphSelection( stream.readSTString() );
      // Check Graph Field.
      if ( !pGraphField )
         return false;

      // Load Graph Object.
      if ( !pGraphField->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) )
         return false;
   }

   // Load Graph Selection Flag.
   bool graphSelection;
   if ( !stream.read( &graphSelection ) )
      return false;

   // Do we have a Graph Selection?
   if ( graphSelection )
      // Yes, so Read Graph Name and Select it.
      object->selectGraph( stream.readSTString() );

   // Load Effect Collision Flag.
   bool effectCollisionStatus;
   if ( !stream.read( &effectCollisionStatus ) )
      return false;

   // Set Effect Collision Status.
   object->setEffectCollisionStatus( effectCollisionStatus );

   // Load Emitter Count.
   S32 emitterCount;
   if ( !stream.read( &emitterCount ) )
      return false;

   // Load Emitters.
   for ( U32 n = 0; n < emitterCount; n++ )
   {
      // Add Emitter.
      object->addEmitter();

      // Find Emitter.
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( object->mParticleEmitterList[n].mObjectId ) );

      // Error?
      if( !pEmitter )
      {
         Con::warnf("t2dParticleEmitter::loadStream - Unable to properly add emitter from disk" );
         continue;
      }

      // Load Emitter.
      pEmitter->loadStream( T2D_SERIALISE_LOAD_ARGS_PASS );

      // Is Emitter Name Empty?
      if ( dStrlen( pEmitter->getEmitterName() ) == 0 )
      {
         // Yes, so format and Set Default Emitter Name.
         char emitterName[256];
         dSprintf( emitterName, 256, "Emitter_%d", n );
         pEmitter->setEmitterName( emitterName );
      }
   }

   // Load Effect Playing Flag / Age.
   if (    !stream.read( &object->mEffectPlaying ) ||
      !stream.read( &object->mEffectAge ) ||
      !stream.read( &object->mWaitingForParticles ) ||
      !stream.read( &object->mWaitingForDelete ) ||
      !stream.read( (S32*)&object->mEffectLifeMode ) ||
      !stream.read( &object->mEffectLifetime ) )
      return false;

   // Return Okay.
   return true;
}

//-----------------------------------------------------------------------------
// Save v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dParticleEffect, 2 )
{
   // Save Graph Count.
   if ( !stream.write( object->mGraphSelectionList.size() ) )
      return false;

   // Save Graphs.
   for ( U32 n = 0; n < object->mGraphSelectionList.size(); n++ )
   {
      // Write Graph Name.
      stream.writeString( object->mGraphSelectionList[n]->mGraphName );
      // Write Graph Object.
      if ( !object->mGraphSelectionList[n]->mpGraphObject->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )
         return false;
   }

   // Save Graph Selection Flag.
   if ( !stream.write( (object->mpCurrentGraph != NULL) ) )
      return false;

   // Do we have a Graph Selection?
   if ( object->mpCurrentGraph )
      // Yes, so save Graph Selection.
      stream.writeString( object->mCurrentGraphName );

   // Save Effect Collision Status.
   if ( !stream.write( object->getEffectCollisionStatus() ) )
      return false;

   // Save Emitter Count.
   if ( !stream.write( object->mParticleEmitterList.size() ) )
      return false;

   // Save Emitters.
   for ( U32 n = 0; n < object->mParticleEmitterList.size(); n++ )
   {
      t2dParticleEmitter *pEmitter = dynamic_cast<t2dParticleEmitter*>( Sim::findObject( object->mParticleEmitterList[n].mObjectId ) );
      if( pEmitter )
         pEmitter->saveStream( T2D_SERIALISE_SAVE_ARGS_PASS );
   }

   // Save Effect Playing Flag / Age.
   if (    !stream.write( object->mEffectPlaying ) ||
      !stream.write( object->mEffectAge ) ||
      !stream.write( object->mWaitingForParticles ) ||
      !stream.write( object->mWaitingForDelete ) ||
      !stream.write( (S32)object->mEffectLifeMode ) ||
      !stream.write( object->mEffectLifetime ) )
      return false;

   // Return Okay.
   return true;
}

void t2dParticleEffect::initPersistFields()
{
   addProtectedField("effectFile", TypeFilename, Offset(mEffectFile, t2dParticleEffect), &setEffectFile, &defaultProtectedGetFn, "");
   addProtectedField("useEffectCollisions", TypeBool, Offset(mUseEffectCollisions, t2dParticleEffect), &setUseEffectCollisions, &defaultProtectedGetFn, "");
   addProtectedField("effectMode", TypeEnum, Offset(mEffectLifeMode, t2dParticleEffect), &setEffectMode, &defaultProtectedGetFn, 1, &gEffectMode);
   addProtectedField("effectTime", TypeF32, Offset(mEffectLifetime, t2dParticleEffect), &setEffectModeTime, &defaultProtectedGetFn, "" );
   Parent::initPersistFields();
}

void t2dParticleEffect::copyTo(SimObject* object)
{
   Parent::copyTo(object);
	
   AssertFatal(dynamic_cast<t2dParticleEffect*>(object), "t2dParticleEffect::copyTo - Copy object is not a t2dParticleEffect!");
   t2dParticleEffect* effect = static_cast<t2dParticleEffect*>(object);

   effect->mUseEffectCollisions = mUseEffectCollisions;
   effect->mEffectLifeMode = mEffectLifeMode;
   effect->mEffectLifetime = mEffectLifetime;

   for (S32 i = 0; i < getEmitterCount(); i++)
   {
      SimObject* emitter = mParticleEmitterList[i].mpSceneObject;
      t2dParticleEmitter* newEmitter = effect->addEmitter();
      emitter->copyTo(newEmitter);
   }

   mParticleLife.copyTo(effect->mParticleLife);
   mQuantity.copyTo(effect->mQuantity);
   mSizeX.copyTo(effect->mSizeX);
   mSizeY.copyTo(effect->mSizeY);
   mSpeed.copyTo(effect->mSpeed);
   mSpin.copyTo(effect->mSpin);
   mFixedForce.copyTo(effect->mFixedForce);
   mRandomMotion.copyTo(effect->mRandomMotion);
   mEmissionForce.copyTo(effect->mEmissionForce);
   mEmissionAngle.copyTo(effect->mEmissionAngle);
   mEmissionArc.copyTo(effect->mEmissionArc);
   mVisibility.copyTo(effect->mVisibility);

   if (getIsEffectPlaying() && effect->getSceneGraph())
      effect->playEffect(true);
}

bool t2dParticleEffect::setEffectFile(void *obj, const char *data)
{
   t2dParticleEffect* effect = static_cast<t2dParticleEffect*>(obj);
   if (effect->getSceneGraph())
   {
      effect->loadEffect(data);
      return false;
   }

   else if (gLoadingSceneGraph)
   {
      gLoadingSceneGraph->addToScene(effect);
      effect->loadEffect(data);
      return false;
   }

   return true;
}

void t2dParticleEffect::readFieldsPostLoad()
{
   playEffect(true);

   Parent::readFieldsPostLoad();
}
