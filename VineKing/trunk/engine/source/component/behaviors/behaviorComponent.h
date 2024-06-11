#ifndef _BEHAVIOR_COMPONENT_H_
#define _BEHAVIOR_COMPONENT_H_

#include "component/dynamicConsoleMethodComponent.h"
#include "component/behaviors/behaviorInstance.h"
#include "component/componentInterface.h"

class BehaviorComponent;

//////////////////////////////////////////////////////////////////////////

class BehaviorInterface : public ComponentInterface
{
   typedef ComponentInterface Parent;

public:
   virtual bool addBehavior( BehaviorInstance *bi );
   virtual bool removeBehavior( BehaviorInstance *bi );
   virtual const SimSet &getBehaviors() const;
   virtual BehaviorInstance *getBehavior( StringTableEntry behaviorTemplateName );
   virtual BehaviorInstance *getBehavior( S32 index );
   virtual void clearBehaviors();
   virtual bool reOrder( BehaviorInstance *obj, U32 desiredIndex /* = 0 */ );
   virtual U32 getBehaviorCount() const;
   virtual const char *callMethodOnBehaviors( S32 argc, const char *argv[] );
};

//////////////////////////////////////////////////////////////////////////

class BehaviorComponent : public DynamicConsoleMethodComponent
{
   friend class BehaviorInterface;
   typedef DynamicConsoleMethodComponent Parent;

private:
   SimSet mBehaviors;

protected:
   BehaviorInterface mPublicBehaviorInterface;
   
   // [neo, 5/10/2007 - #3010]
   bool  mInBehaviorCallback; ///<  

   virtual const char* _callMethod( U32 argc, const char *argv[], bool callThis = true );

   // [neo, 5/11/2007]
   // Refactored onAdd() code into this method so it can be deferred by derived classes.
   /// Attach behaviors listed as special fields (derived classes can override this and defer!)
   virtual void addBehaviors();

   /// Should adding of behaviors be deferred from onAdd() to be called by derived classe manually?
   virtual bool deferAddingBehaviors() const { return false; }
  
public:
   DECLARE_CONOBJECT( BehaviorComponent );

   BehaviorComponent() : mInBehaviorCallback( false )
   {
      SIMSET_SET_ASSOCIATION( mBehaviors );
   }


   //////////////////////////////////////////////////////////////////////////
   /// Behavior interface  (Move this to protected?)
   virtual bool addBehavior( BehaviorInstance *bi, int delay = 0 );
   virtual bool removeBehavior( BehaviorInstance *bi, bool deleteBehavior = true );
   virtual const SimSet &getBehaviors() const { return mBehaviors; };
   virtual BehaviorInstance *getBehavior( StringTableEntry behaviorTemplateName );
   virtual BehaviorInstance *getBehavior( S32 index );
   virtual void clearBehaviors();
   virtual bool reOrder( BehaviorInstance *obj, U32 desiredIndex /* = 0 */ );
   virtual U32 getBehaviorCount() const { return mBehaviors.size(); }

   //////////////////////////////////////////////////////////////////////////
   // DynamicConsoleMethodComponent Overrides
   virtual bool handlesConsoleMethod( const char *fname, S32 *routingId );

   //////////////////////////////////////////////////////////////////////////
   // SimComponent overrides
   virtual void write( Stream &stream, U32 tabStop, U32 flags = 0  );
   virtual void registerInterfaces( SimComponent *owner );

   //////////////////////////////////////////////////////////////////////////
   // SimObject overrides
   virtual bool onAdd();
   virtual void onRemove();

};


//////////////////////////////////////////////////////////////////////////
// Inline interface implementation

inline bool BehaviorInterface::addBehavior( BehaviorInstance *bi )
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<BehaviorComponent *>( getOwner() )->addBehavior( bi );
}

inline bool BehaviorInterface::removeBehavior( BehaviorInstance *bi )
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<BehaviorComponent *>( getOwner() )->removeBehavior( bi );
}

inline const SimSet &BehaviorInterface::getBehaviors() const
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<const BehaviorComponent *>( getOwner() )->getBehaviors();
}

inline BehaviorInstance *BehaviorInterface::getBehavior( StringTableEntry behaviorTemplateName )
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<BehaviorComponent *>( getOwner() )->getBehavior( behaviorTemplateName );
}

inline BehaviorInstance *BehaviorInterface::getBehavior( S32 index )
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<BehaviorComponent *>( getOwner() )->getBehavior( index );
}

inline void BehaviorInterface::clearBehaviors()
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   reinterpret_cast<BehaviorComponent *>( getOwner() )->clearBehaviors();
}

inline bool BehaviorInterface::reOrder( BehaviorInstance *obj, U32 desiredIndex /* = 0 */ )
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<BehaviorComponent *>( getOwner() )->reOrder( obj, desiredIndex );
}

inline U32 BehaviorInterface::getBehaviorCount() const
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<const BehaviorComponent *>( getOwner() )->getBehaviorCount();
}

inline const char *BehaviorInterface::callMethodOnBehaviors( S32 argc, const char *argv[] )
{
   VALID_INTERFACE_ASSERT(BehaviorComponent);
   return reinterpret_cast<BehaviorComponent *>( getOwner() )->_callMethod( argc, argv, false );
}

#endif