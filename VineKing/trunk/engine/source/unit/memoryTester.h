#ifndef _UNIT_MEMORYTESTER_H_
#define _UNIT_MEMORYTESTER_H_

#ifdef TORQUE_ALLOW_UNIT_TESTS

namespace UnitTesting
{
   class MemoryTester
   {
   public:
      void mark();
      bool check();
   };
}


#endif
#endif //TORQUE_ALLOW_UNIT_TESTS