//-----------------------------------------------------------------------------
// GarageGames Library
// Copyright (c) GarageGames, All Rights Reserved
//-----------------------------------------------------------------------------

#ifndef UNIT_UNITTESTING_H
#define UNIT_UNITTESTING_H

#ifdef TORQUE_ALLOW_UNIT_TEST_DEFINES

namespace UnitTesting {

//-----------------------------------------------------------------------------

struct UnitMargin
{
static void Push(int margin);
static void Pop();
static int Current();
};

void UnitPrint(const char* msg);


//-----------------------------------------------------------------------------

class UnitTest {
   int _testCount;
   int _failureCount;
   int _warningCount;
public:
   UnitTest();
   virtual ~UnitTest() {};

   /// Test an assertion and note if it has failed.
   bool test(bool a,const char* msg) {
      _testCount++;
      if (!a)
         fail(msg);
      return a;
   }

   /// Report a failture condition.
   void fail(const char* msg);

   /// Report a warning
   void warn(const char* msg);

   int getTestCount() { return _testCount; }
   int getFailureCount() { return _failureCount; }
   int getWarningCount() { return _warningCount; }

   /// Implement this with the specific test.
   virtual void run() = 0;
};


//-----------------------------------------------------------------------------

class TestRegistry
{
   friend class DynamicTestRegistration; // Bless me, Father, for I have sinned, but this is damn cool

   static TestRegistry* _list;
   TestRegistry* _next;
   const char* _name;

public:
   TestRegistry(const char* name);
   virtual ~TestRegistry() {}
   static TestRegistry* getFirst() { return _list; }
   TestRegistry* getNext() { return _next; }
   const char* getName() { return _name; }
   virtual UnitTest* newTest() = 0;
};

template<class T>
class TestRegistration: public TestRegistry
{
public:
   virtual ~TestRegistration() {}
   TestRegistration(const char* name): TestRegistry(name) {}
   virtual UnitTest* newTest() { return new T; }
};

class DynamicTestRegistration : public TestRegistry
{

   UnitTest *mUnitTest;

public:
   DynamicTestRegistration( const char *name, UnitTest *test );

   virtual ~DynamicTestRegistration();

   virtual UnitTest *newTest() { return mUnitTest; }
};


//-----------------------------------------------------------------------------

class TestRun {
   int _testCount;
   int _subCount;
   int _failureCount;
   int _warningCount;
   void test(TestRegistry* reg);
public:
   TestRun();
   void printStats();
   bool test(const char* module);
};

#define CreateUnitTest(Class,Name) \
   class Class; \
   static UnitTesting::TestRegistration<Class> _UnitTester##Class (Name); \
   class Class : public UnitTest

} // Namespace

#endif

#endif //TORQUE_ALLOW_UNIT_TEST_DEFINES