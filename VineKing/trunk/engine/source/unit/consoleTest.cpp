#ifdef TORQUE_ALLOW_UNIT_TESTS

#include "unit/test.h"
#include "console/console.h"

using namespace UnitTesting;

ConsoleFunction(unitTest_runTests, void, 1, 2, "([searchString]) - run unit tests,"
                " or just the tests that prefix match against the searchString.\n"
				"@param searchString Optional specific search string for tests to execute\n"
				"@return No Return Value.")
{
   const char *searchString = (argc > 1 ? argv[1] : "");
   TestRun tr;
   tr.test(searchString);
}

#endif //TORQUE_ALLOW_UNIT_TESTS