//-----------------------------------------------------------------------------
// GarageGames Library
// Copyright (c) GarageGames, All Rights Reserved
//-----------------------------------------------------------------------------

//Sven - need to include the config for the define below it
#include "core/torqueConfig.h"
#ifdef TORQUE_ALLOW_UNIT_TEST_DEFINES

#include "unit/test.h"
#include <stdio.h>
#include <string.h>

#include "console/console.h"

namespace UnitTesting
{

//-----------------------------------------------------------------------------

TestRegistry *TestRegistry::_list = 0;


//-----------------------------------------------------------------------------

static const int MaxMarginCount = 32;
static const int MaxMarginValue = 128;
static int _Margin[MaxMarginCount] = { 3 };
static int* _MarginPtr = _Margin;
static char _MarginString[MaxMarginValue];

static void _printMargin()
{
   if (*_MarginPtr)
      ::fwrite(_MarginString,1,*_MarginPtr,stdout);
}

void UnitMargin::Push(int margin)
{
   if (_MarginPtr < _Margin + MaxMarginCount) {
      *++_MarginPtr = (margin < MaxMarginValue)? margin: MaxMarginValue;
      memset(_MarginString,' ',*_MarginPtr);
   }
}

void UnitMargin::Pop()
{
   if (_MarginPtr > _Margin) {
      _MarginPtr--;
      memset(_MarginString,' ',*_MarginPtr);
   }
}

int UnitMargin::Current()
{
   return *_MarginPtr;
}

void UnitPrint(const char* str)
{
   static bool lineStart = true;
   Platform::outputDebugString(str);

   // Need to scan for '\n' in order to support margins
   const char* ptr = str, *itr = ptr;
   for (; *itr != 0; itr++)
      if (*itr == '\n') 
      {
         if (lineStart)
            _printMargin();
         ::fwrite(ptr,1,itr - ptr + 1,stdout);
         ptr = itr + 1;
         lineStart = true;
      }

   // End the line with a carriage return unless the
   // line ends with a line continuation char.
   if (ptr != itr) {
      if (lineStart)
         _printMargin();
      if (itr[-1] == '\\') {
         ::fwrite(ptr,1,itr - ptr - 1,stdout);
         lineStart = false;
      }
      else {
         ::fwrite(ptr,1,itr - ptr,stdout);
         ::fwrite("\n",1,1,stdout);
         lineStart = true;
      }
   }
   else {
      ::fwrite("\n",1,1,stdout);
      lineStart = true;
   }
   ::fflush(stdout);
}


//-----------------------------------------------------------------------------

UnitTest::UnitTest() {
   _testCount = 0;
   _failureCount = 0;
   _warningCount = 0;
}

void UnitTest::fail(const char* msg)
{
   Con::printf("** Failed: %s",msg);
   _failureCount++;
}

void UnitTest::warn(const char* msg)
{
   Con::warnf("** Warning: %s",msg);
   _warningCount++;
}


//-----------------------------------------------------------------------------

TestRegistry::TestRegistry(const char* name)
{
   _next = _list;
   _list = this;
   _name = name;
}

DynamicTestRegistration::DynamicTestRegistration( const char *name, UnitTest *test ) : TestRegistry( name ), mUnitTest( test )
{

}

DynamicTestRegistration::~DynamicTestRegistration()
{
   // Un-link ourselves from the test registry
   TestRegistry *walk = _list;

   // Easy case!
   if( walk == this )
      _list = _next;
   else
   {
      // Search for us and remove
      while( ( walk != 0 ) && ( walk->_next != 0 ) && ( walk->_next != this ) )
         walk = walk->_next;

      // When this loop is broken, walk will be the unit test in the list previous to this one
      if( walk != 0 && walk->_next != 0 )
         walk->_next = walk->_next->_next;
   }
}


//-----------------------------------------------------------------------------

TestRun::TestRun()
{
   _subCount = 0;
   _testCount = 0;
   _failureCount = 0;
   _warningCount = 0;
}

void TestRun::printStats()
{
   Con::printf("-- %d test%s run (with %d sub-test%s)",
      _testCount,(_testCount != 1)? "s": "",
      _subCount,(_subCount != 1)? "s": "");

   if (_testCount)
   {
      if (_failureCount)
         Con::printf("** %d reported failure%s",
            _failureCount,(_failureCount != 1)? "s": "");
      else if (_warningCount)
         Con::printf("** %d reported warning%s",
            _warningCount,(_warningCount != 1)? "s": "");
      else
         Con::printf("-- No reported failures");
   }
}

void TestRun::test(TestRegistry* reg)
{
   Con::printf("-- Testing: %s",reg->getName());

   UnitMargin::Push(_Margin[0]);

   // Run the test.
   UnitTest* test = reg->newTest();
   test->run();

   UnitMargin::Pop();

   // Update stats.
   _failureCount += test->getFailureCount();
   _subCount += test->getTestCount();
   _warningCount += test->getWarningCount();
   _testCount++;

   // Don't forget to delete the test!
   delete test;
}

// [tom, 2/5/2007] To provide a predictable environment for the tests, this
// now changes the current directory to the executable's directory before
// running the tests. The previous current directory is restored on exit.

bool TestRun::test(const char* module)
{
   StringTableEntry cwdSave = Platform::getCurrentDirectory();

   int len = strlen(module);
   if (!len)
      Con::printf("-- Running all unit tests");
   else
      Con::printf("-- Running %s tests",module);
   
   for (TestRegistry* itr = TestRegistry::getFirst(); itr; itr = itr->getNext())
   {
      if (!len || !dStrnicmp(module,itr->getName(),len))
      {
         Platform::setCurrentDirectory(Platform::getMainDotCsDir());
         test(itr);
      }
   }

   printStats();

   Platform::setCurrentDirectory(cwdSave);

   return !_failureCount;
}

} // Namespace

#endif //TORQUE_ALLOW_UNIT_TEST_DEFINES