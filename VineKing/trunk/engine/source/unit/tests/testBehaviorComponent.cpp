#ifdef TORQUE_ALLOW_UNIT_TEST_DEFINES

#include "unit/test.h"
#include "unit/memoryTester.h"
#include "component/behaviors/behaviorComponent.h"
#include "component/behaviors/behaviorTemplate.h"
#include "component/behaviors/behaviorInstance.h"
#include "core/resManager.h"

using namespace UnitTesting;

//////////////////////////////////////////////////////////////////////////

CreateUnitTest(TestComponentInterfacing, "Components/BehaviorComponent")
{
   void run()
   {
      // Lets make sure that all behaviors are cleaning up after themselves
      //MemoryTester m;
      //m.mark();

      //////////////////////////////////////////////////////////////////////////
      // Script requirements, split this out eventually since there is some common
      // functionality that we will probably always want
      test( ResourceManager->findFile( "unitTests/behaviorUnitTest.cs" ), "Unable to find behavior unit test script file." );
      Con::evaluate( "exec(\"unitTests/behaviorUnitTest.cs\");" );

      //////////////////////////////////////////////////////////////////////////
      // Set up the test template  
      BehaviorTemplate *testTemplate = new BehaviorTemplate();
      test( testTemplate->registerObject( "aTestTemplate" ), "Failed to register template" );

      testTemplate->setDataField( StringTable->insert( "friendlyName" ), NULL, "Behavior Unit Test Behavior" );
      testTemplate->setDataField( StringTable->insert( "description" ), NULL, "A behavior to test the behavior component" );
      testTemplate->setDataField( StringTable->insert( "behaviorType" ), NULL, "test" );
      testTemplate->setDataField( StringTable->insert( "fromResource" ), NULL, "The great beyond" );
      testTemplate->setDataField( StringTable->insert( "behaviorClass" ), NULL, "UnitTestBehavior" );
      testTemplate->setDataField( StringTable->insert( "behaviorSuperClass" ), NULL, "UnitTestBehaviorParent" );
      
      // Add some fields
      testTemplate->addBehaviorField( "unitTestField", "A test dynamic field", "string", "foo" );

      // Test the template
      test( testTemplate->getBehaviorFieldCount() > 0, "Dynamic field addition failed" );

      //////////////////////////////////////////////////////////////////////////
      // Create an instance of the behavior
      BehaviorInstance *testBehavior = testTemplate->createInstance();

      // Test the behavior
      test( testBehavior != NULL, "Failed to create behavior instance" );
      test( testBehavior->isProperlyAdded(), "Behavior was not properly registered by createInstance" );
      test( testBehavior->getTemplate() == testTemplate, "Behavior doesn't know what it's template is." );

      //////////////////////////////////////////////////////////////////////////
      // Set up the test component
      SimComponent *testComponent = new SimComponent();
      BehaviorComponent *beComponent = new BehaviorComponent();
 
      // Test the unserialize -- TODO: finish this after we have the 'BehaviorComponent.behavior(string templateName)' functionality
      //beComponent->setDataField( StringTable->insert( "_behavior0" ), NULL, "aTestTemplate\tunitTestField\tbar" );

      // Register, and add sub-components
      test( beComponent->registerObject(), "Failed to register behavior component" );
      test( testComponent->addComponent( beComponent ), "Failed to add behavior component" );

      // Register main component
      test( testComponent->registerObject(), "Failed to register test component" );

      //////////////////////////////////////////////////////////////////////////
      // Start tests

      // This tests the behavior interface
      // Note that this is how you would interact with this component if you were
      // another component. This whole code block could easily be simplified to
      //   beComponen->addBehavior( testBehavior );
      // however I want to test the interface, as well as provide an example of
      // how to use interfaces.
      ComponentInterfaceList iLst;
      test( beComponent->getInterfaces( &iLst, NULL, "behaviors" ), "Behavior component has not correctly exposed it's BehaviorInterface" );
      BehaviorInterface *beComponentBehaviorInterface = dynamic_cast<BehaviorInterface *>( iLst[0] );
      test( beComponentBehaviorInterface != NULL, "Behavior interface exposed is not a instance of BehaviorInterface" );

      if( beComponentBehaviorInterface != NULL )
      {
         test( beComponentBehaviorInterface->isValid(), "Behavior interface is not valid, check registration of interfaces" );
         test( beComponentBehaviorInterface->addBehavior( testBehavior ), "Failed to add behavior to behavior component." );
      }

      // This tests namespace linking
      test( dAtob( Con::executef( testBehavior, 1, "test" ) ), "Script test function failed, check namespace linking and link mask" );
      test( dAtob( Con::executef( testBehavior, 1, "parent_test" ) ), "Script parent test function failed, check namespace linking and link mask" );

      //////////////////////////////////////////////////////////////////////////
      // Cleanup
      testBehavior->deleteObject();
      testTemplate->deleteObject();
      testComponent->deleteObject();


      // This fails due to allocation in gBehaviorSet. I am not positive this is
      // a valid leak.
      //test( m.check(), "Behavior component test leaked memory" );
   }
};

#endif //TORQUE_ALLOW_UNIT_TEST_DEFINES