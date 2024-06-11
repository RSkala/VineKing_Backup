//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _iPhoneCONSOLE_H_
#define _iPhoneCONSOLE_H_

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif
#ifndef _EVENT_H_
#include "Platform/event.h"
#endif


class iPhoneConsole
{
private:
   bool consoleEnabled;
	bool debugOutputEnabled;
   
   U32   inBufPos;
   char  inBuf[MaxConsoleLineSize];
   ConsoleEvent postEvent;
   
   void clearInBuf();
   
public:
   static void create();
   static void destroy();
   static bool isEnabled();

   iPhoneConsole();
   ~iPhoneConsole();
   void enable(bool);
	//%PUAP%
	void enableDebugOutput( bool );

   void processConsoleLine(const char *consoleLine);
   
   void  inputLoop();

};

extern iPhoneConsole *gConsole;

#endif
