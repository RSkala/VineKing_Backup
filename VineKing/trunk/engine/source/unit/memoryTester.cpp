#ifdef TORQUE_ALLOW_UNIT_TESTS

#include "platform/platform.h"
#include "unit/memoryTester.h"

using namespace UnitTesting;

void MemoryTester::mark()
{
#ifdef TORQUE_DEBUG_GUARD
   Memory::flagCurrentAllocs();
#endif
}

bool MemoryTester::check()
{
#ifdef TORQUE_DEBUG_GUARD
   return Memory::countUnflaggedAllocs(NULL) == 0;
#else
   //UnitTesting::UnitPrint("MemoryTester::check - unavailable w/o TORQUE_DEBUG_GUARD defined!");
   return true;
#endif
}

#endif //TORQUE_ALLOW_UNIT_TESTS