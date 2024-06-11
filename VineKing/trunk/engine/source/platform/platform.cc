#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "platform/threads/mutex.h"

//Added for the cprintf below
#include <stdarg.h>
#include <stdio.h>

// The tools prefer to allow the CPU time to process
#ifdef TORQUE_IPHONE
#ifndef TORQUE_TOOLS
S32 sgBackgroundProcessSleepTime = 16;
#else
S32 sgBackgroundProcessSleepTime = 200;
#endif
S32 sgTimeManagerProcessInterval = 16;
#else
#ifndef TORQUE_TOOLS
S32 sgBackgroundProcessSleepTime = 25;
#else
S32 sgBackgroundProcessSleepTime = 200;
#endif
S32 sgTimeManagerProcessInterval = 0;
#endif //TORQUE__IPHONE

void Platform::initConsole()
{
   Con::addVariable("Pref::backgroundSleepTime", TypeS32, &sgBackgroundProcessSleepTime);
   Con::addVariable("Pref::timeManagerProcessInterval", TypeS32, &sgTimeManagerProcessInterval);
}

S32 Platform::getBackgroundSleepTime()
{
   return sgBackgroundProcessSleepTime;
}

void Platform::cprintf( const char* str )
{
	printf( "%s \n", str );
}