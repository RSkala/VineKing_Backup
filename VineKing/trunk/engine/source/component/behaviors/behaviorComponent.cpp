#include "component/behaviors/behaviorComponent.h"
#include "component/behaviors/behaviorTemplate.h"
#include "core/stream.h"
#include "util/stringUnit.h"
#include "core/frameAllocator.h"

// Needed to be able to directly call execute on a Namespace::Entry
extern ExprEvalState gEvalState;


IMPLEMENT_CONOBJECT( BehaviorComponent );

bool BehaviorComponent::addBehavior( BehaviorInstance *bi, int delay/* = 0*/  )
{
   if( bi == NULL || !bi->isProperlyAdded() )
      return false;

   mBehaviors.pushObject( bi );

   // Register the component with this owner.
   bi->setBehaviorOwner( this );

   // May want to look @ the return value here and optionally pushobject etc
   if( delay == 0 ) {
	   if( bi->isMethod("onBehaviorAdd") ) {
		   Con::executef( bi , 1, "onBehaviorAdd" );
	   }
   } else {
	   //sometimes we want to delay these callbakcs
	   if( bi->isMethod("onBehaviorAdd") ) {
		   Con::executef( bi , 1, "onBehaviorAdd" );
		   const char *argv = "onBehaviorAdd";
		   Sim::schedule( delay, bi, 1, &argv );
	   }
   }

   return true;
}

ConsoleMethod( BehaviorComponent, addBehavior, bool, 3, 3, "(BehaviorInstance bi) - Add a behavior to the object\n"
														   "@param bi The behavior instance to add"
														   "@return (bool success) Whether or not the behavior was successfully added")
{
   return object->addBehavior( dynamic_cast<BehaviorInstance *>( Sim::findObject( argv[2] ) ) );
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorComponent::removeBehavior( BehaviorInstance *bi, bool deleteBehavior )
{
   for( SimSet::iterator nItr = mBehaviors.begin(); nItr != mBehaviors.end(); nItr++ )
   {
      if( *nItr == bi )
      {
         mBehaviors.removeObject( *nItr );

         AssertFatal( bi->isProperlyAdded(), "Don't know how but a behavior instance is not registered w/ the sim" );

         if( bi->isMethod("onBehaviorRemove") )
           Con::executef( bi , 1, "onBehaviorRemove" );

         if (deleteBehavior)
            bi->deleteObject();

         return true;
      }
   }

   return false;
}

ConsoleMethod( BehaviorComponent, removeBehavior, bool, 3, 4, "(BehaviorInstance bi, [bool deleteBehavior = true])\n"
															  "@param bi The behavior instance to remove\n"
															  "@param deleteBehavior Whether or not to delete the behavior\n"
															  "@return (bool success) Whether the behavior was successfully removed")
{
   bool deleteBehavior = true;
   if (argc > 3)
      deleteBehavior = dAtob(argv[3]);

   return object->removeBehavior( dynamic_cast<BehaviorInstance *>( Sim::findObject( argv[2] ) ), deleteBehavior );
}

//////////////////////////////////////////////////////////////////////////

void BehaviorComponent::clearBehaviors()
{
   while( mBehaviors.size() > 0 )
   {
      BehaviorInstance *bi = dynamic_cast<BehaviorInstance *>( mBehaviors.first() );
      removeBehavior( bi );
   }
}

ConsoleMethod( BehaviorComponent, clearBehaviors, void, 2, 2, "() - Clear all behavior instances\n"
			  "@return No return value")
{
   object->clearBehaviors();
}

//////////////////////////////////////////////////////////////////////////

const char *BehaviorComponent::_callMethod( U32 argc, const char *argv[], bool callThis /* = true  */ )
{   
   if( mBehaviors.empty() )   
      return Parent::_callMethod( argc, argv, callThis );
   
   const char *cbName = StringTable->insert(argv[0]);

   // [neo, 5/10/2007 - #3010]
   // We don't want behaviors to call a method on its owner which would recursively call it
   // again on the behavior and cause an infinite loop so we mark it when calling the behavior
   // method and trap it if it reenters and force it to call the method on this object.
   // This is a quick fix for now and I will review this before the end of the release.
   if( mInBehaviorCallback ) 
      return Parent::_callMethod( argc, argv, true );
   
   // CodeReview The tools ifdef here is because we don't want behaviors getting calls during
   //            design time.  For example an onCollision call when an object with collision
   //            enabled and an 'explode on collision' behavior is dragged over another object
   //            with collision enable in the scene while designing.   Is this incorrect? [2/27/2007 justind]
   // CodeReview [tom, 3/9/2007] That seems semi-sensible, unless you want some kind of 
   // behavior preview functionality in the editor. Something feels slightly wrong
   // about ifdef'ing this out, though.

   // Copy the arguments to avoid weird clobbery situations.
   FrameTemp<char *> argPtrs (argc);
   
   U32 strdupWatermark = FrameAllocator::getWaterMark();
   for( S32 i = 0; i < argc; i++ )
   {
      argPtrs[i] = reinterpret_cast<char *>( FrameAllocator::alloc( dStrlen( argv[i] ) + 1 ) );
      dStrcpy( argPtrs[i], argv[i] );
   }

   for( SimSet::iterator i = mBehaviors.begin(); i != mBehaviors.end(); i++ )
   {
      BehaviorInstance *pBehavior = dynamic_cast<BehaviorInstance *>( *i );
      AssertFatal( pBehavior, "BehaviorComponent::_callMethod - Bad behavior instance in list." );
      AssertFatal( pBehavior->getId() > 0, "Invalid id for behavior component" );

      // Use the BehaviorInstance's namespace
      Namespace *pNamespace = pBehavior->getNamespace();
      if(!pNamespace)
         continue;

      // Lookup the Callback Namespace entry and then splice callback
      Namespace::Entry *pNSEntry = pNamespace->lookup(cbName);
      if( pNSEntry )
      {
         // Set %this to our BehaviorInstance's Object ID
         argPtrs[1] = const_cast<char *>( pBehavior->getIdString() );

         // [neo, 5/10/2007 - #3010]
         // Set flag so we can call the method on this object directly, should the
         // behavior have 'overloaded' a method on the owner object.
         mInBehaviorCallback = true;

         // Change the Current Console object, execute, restore Object
         SimObject *save = gEvalState.thisObject;
         gEvalState.thisObject = pBehavior;
#ifdef PUAP_NAMESPACE_CHANGE
         const char *ret = pNSEntry->execute(argc, const_cast<const char **>( ~argPtrs ));
#else
         const char *ret = pNSEntry->execute(argc, const_cast<const char **>( ~argPtrs ), &gEvalState);
#endif //PUAP_NAMESPACE_CHANGE
         gEvalState.thisObject = save;

         // [neo, 5/10/2007 - #3010]
         // Reset flag
         mInBehaviorCallback = false;
      }
   }

   // Pass this up to the parent since a BehaviorComponent is still a DynamicConsoleMethodComponent
   // it needs to be able to contain other components and behave properly
   const char* fnRet = Parent::_callMethod( argc, argv, callThis );

   // Clean up.
   FrameAllocator::setWaterMark( strdupWatermark );

   return fnRet;
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorComponent::handlesConsoleMethod( const char *fname, S32 *routingId )
{

   // CodeReview [6/25/2007 justind]
   // If we're deleting the BehaviorComponent, don't forward the call to the
   // behaviors, the parent onRemove will handle freeing them
   // This should really be handled better, and is in the Parent implementation
   // but behaviors are a special case because they always want to be called BEFORE
   // the parent to act.
   if( dStricmp( fname, "delete" ) == 0 )
      return Parent::handlesConsoleMethod( fname, routingId );

   // [neo, 5/10/2007 - #3010]
   // Make sure we honor the flag!
   if( !mInBehaviorCallback )
   {
      for( SimSet::iterator nItr = mBehaviors.begin(); nItr != mBehaviors.end(); nItr++ )
      {
         SimObject *pComponent = dynamic_cast<SimObject *>(*nItr);
         if( pComponent != NULL && pComponent->isMethod( fname ) )
         {
            *routingId = -2; // -2 denotes method on component
            return true;
         }
      }
   }

   // Let parent handle it
   return Parent::handlesConsoleMethod( fname, routingId );
}


//////////////////////////////////////////////////////////////////////////

static void writeTabs(Stream &stream, U32 count)
{
   char tab[] = "   ";
   while(count--)
      stream.write(3, (void*)tab);
}

void BehaviorComponent::write( Stream &stream, U32 tabStop, U32 flags /* = 0 */ )
{
   // Do *not* call parent on this

   VectorPtr<SimComponent *> &componentList = lockComponentList();
   // export selected only?
   if( ( flags & SelectedOnly ) && !isSelected() )
   {
      for( SimComponentIterator i = componentList.begin(); i != componentList.end(); i++ )
         (*i)->write(stream, tabStop, flags);

      goto write_end;
   }

   writeTabs( stream, tabStop );
   char buffer[1024];
   dSprintf( buffer, sizeof(buffer), "new %s(%s) {\r\n", getClassName(), getName() ? getName() : "" );
   stream.write( dStrlen(buffer), buffer );
   writeFields( stream, tabStop + 1 );

   // Write behavior fields
   if( mBehaviors.size() > 0 )
   {
      // Pack out the behaviors into fields
      U32 i = 0;
      for( SimSet::iterator b = mBehaviors.begin(); b != mBehaviors.end(); b++ )
      {
         dSprintf( buffer, sizeof( buffer ), "_behavior%d = \"", i++ );
         writeTabs( stream, tabStop + 1 );
         stream.write( dStrlen( buffer ), buffer );
   
         dynamic_cast<BehaviorInstance *>( *b )->packToStream( stream, tabStop + 1, flags );

         stream.write(4, "\";\r\n" );
      }
   }

   writeTabs( stream, tabStop );
   stream.write( 4, "};\r\n" );

write_end:
   unlockComponentList();
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorComponent::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   if( !deferAddingBehaviors() )
      addBehaviors();
   
   return true;
}

//[neo, 5/11/2007]
// Refactored code here from onAdd so we can defer calling Behavior::onAdd() should
// we need to e.g. to wait for t2dSceneObject to be added to a scene graph first or
// until a fully copy was made in clone() etc.
void BehaviorComponent::addBehaviors()
{
   AssertFatal( mBehaviors.size() == 0, "BehaviorComponent::addBehaviors() already called!" );

   const char *bField = "";

   // Check for data fields which contain packed behaviors, and instantiate them
   // As a side note, this is the most obfuscated conditional block I think I've ever written   
   for( int i = 0; dStrcmp( bField = getDataField( StringTable->insert( avar( "_behavior%d", i ) ), NULL ), "" ) != 0; i++ )
   {
      AssertFatal( ( StringUnit::getUnitCount( bField, "\t" ) - 1 ) % 2 == 0, "Fields should always be in sets of two!" );

      // Grab the template name, make sure the sim knows about it or we are hosed anyway
      StringTableEntry templateName = StringTable->insert( StringUnit::getUnit( bField, 0, "\t" ) );
      BehaviorTemplate *tpl = dynamic_cast<BehaviorTemplate *>( Sim::findObject( templateName ) );
      if( tpl == NULL )
      {
         // If anyone wants to know, let them.
         if( isMethod( "onBehaviorMissing" ) )
            Con::executef( this, 2, "onBehaviorMissing", templateName );
         else
            Con::warnf("BehaviorComponent::addBehaviors - Missing Behavior %s", templateName );

         // Skip it, it's invalid.
         setDataField( StringTable->insert( avar( "_behavior%d", i ) ), NULL, "" );

         continue;
      }

      // create instance
      BehaviorInstance *inst = tpl->createInstance();

      // Sub loop to set up the fields with the values that got written out
      S32 index = 1;
      while( index < StringUnit::getUnitCount( bField, "\t" ) )
      {
         StringTableEntry slotName = StringTable->insert( StringUnit::getUnit( bField, index++, "\t" ) );
         const char* slotValue = StringUnit::getUnit( bField, index++, "\t" );
         inst->setDataField( slotName, NULL, slotValue );
      }

      // Add to behaviors
      addBehavior( inst );

      setDataField( StringTable->insert( avar( "_behavior%d", i ) ), NULL, "" );
   }
}

void BehaviorComponent::onRemove()
{
   // Remove all behaviors and notify
   clearBehaviors();

   // Call parent
   Parent::onRemove();
}


//////////////////////////////////////////////////////////////////////////

void BehaviorComponent::registerInterfaces( SimComponent *owner )
{
   Parent::registerInterfaces( owner );

   owner->registerCachedInterface( NULL, "behaviors", this, &mPublicBehaviorInterface );
}

//////////////////////////////////////////////////////////////////////////
BehaviorInstance *BehaviorComponent::getBehavior( S32 index )
{
   if( index < mBehaviors.size() )
      return reinterpret_cast<BehaviorInstance *>(mBehaviors[index]);

   return NULL;
}

ConsoleMethod( BehaviorComponent, getBehaviorByIndex, S32, 3, 3, "(int index) - Gets a particular behavior\n"
																 "@param index The index of the behavior to get\n"
																 "@return (BehaviorInstance bi) The behavior instance you requested")
{
   BehaviorInstance *bInstance = object->getBehavior( dAtoi(argv[2]) );

   return ( bInstance != NULL ) ? bInstance->getId() : 0;
}

BehaviorInstance *BehaviorComponent::getBehavior( StringTableEntry behaviorTemplateName )
{
   for( SimSet::iterator b = mBehaviors.begin(); b != mBehaviors.end(); b++ )
   {
      // We can do this because both are in the string table
      if( reinterpret_cast<BehaviorInstance *>(*b)->getTemplateName() == behaviorTemplateName )
         return reinterpret_cast<BehaviorInstance *>( *b );
   }

   return NULL;
}

ConsoleMethod( BehaviorComponent, getBehavior, S32, 3, 3, "(string BehaviorTemplateName) - gets a behavior\n"
														  "@param BehaviorTemplateName The name of the template of the behavior instance you want\n"
														  "@return (BehaviorInstance bi) The behavior instance you requested")
{
   BehaviorInstance *bInstance = object->getBehavior( StringTable->insert( argv[2] ) );

   return ( bInstance != NULL ) ? bInstance->getId() : 0;
}

//////////////////////////////////////////////////////////////////////////

bool BehaviorComponent::reOrder( BehaviorInstance *obj, U32 desiredIndex /* = 0 */ )
{
   if( desiredIndex > mBehaviors.size() )
      return false;

   SimObject *target = mBehaviors.at( desiredIndex );
   return mBehaviors.reOrder( obj, target );
}

ConsoleMethod( BehaviorComponent, reOrder, bool, 3, 3, "(BehaviorInstance inst, [int desiredIndex = 0])\n"
													   "@param inst The behavior instance you want to reorder\n"
													   "@param desiredIndex The index you want the behavior instance to be reordered to\n"
													   "@return (bool success) Whether or not the behavior instance was successfully reordered" )
{
   BehaviorInstance *inst = dynamic_cast<BehaviorInstance *>( Sim::findObject( argv[1] ) );

   if( inst == NULL )
      return false;

   U32 idx = 0;
   if( argc > 2 )
      idx = dAtoi( argv[2] );

   return object->reOrder( inst, idx );
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod( BehaviorComponent, getBehaviorCount, S32, 2, 2, "() - Get the count of behaviors on an object\n"
															   "@return (int count) The number of behaviors on an object")
{
   return object->getBehaviorCount();
}
