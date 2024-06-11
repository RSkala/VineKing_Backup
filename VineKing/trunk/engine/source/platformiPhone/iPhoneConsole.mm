//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformiPhone/platformiPhone.h"
#include "platformiPhone/iPhoneConsole.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "platform/threads/thread.h"

#include <stdio.h>

// TODO: convert this to use ncurses.

iPhoneConsole *gConsole = NULL;

ConsoleFunction(enableWinConsole, void, 2, 2, "(bool enable)")
{
   argc;
   if (gConsole)
      gConsole->enable(dAtob(argv[1]));
}

ConsoleFunction(enableDebugOutput, void, 2, 2, "(bool enable)")
{
	argc;
	if (gConsole)
		gConsole->enableDebugOutput(dAtob(argv[1]));
}

static void iPhoneConsoleConsumer(ConsoleLogEntry::Level, const char *line)
{
   if (gConsole)
      gConsole->processConsoleLine(line);
}

static void iPhoneConsoleInputLoopThread(S32 *arg)
{
   if(!gConsole)
      return;
   gConsole->inputLoop();
}

void iPhoneConsole::create()
{
   gConsole = new iPhoneConsole();
}

void iPhoneConsole::destroy()
{
   if (gConsole)
      delete gConsole;
   gConsole = NULL;
}

void iPhoneConsole::enable(bool enabled)
{
   if (gConsole == NULL) return;
   
   consoleEnabled = enabled;
   if(consoleEnabled)
   {
      printf("Initializing Console...\n");
      new Thread((ThreadRunFunction)iPhoneConsoleInputLoopThread,0,true);
      printf("Console Initialized.\n");

      printf("%s", Con::getVariable("Con::Prompt"));
   }
   else
   {
      printf("Deactivating Console.");
   }
}

//%PUAP%
void iPhoneConsole::enableDebugOutput(bool enabled)
{
	if (gConsole == NULL) return;
	debugOutputEnabled = enabled;
}


bool iPhoneConsole::isEnabled()
{
   if ( !gConsole )
      return false;

   return gConsole->consoleEnabled;
}


iPhoneConsole::iPhoneConsole()
{
   consoleEnabled = false;
   clearInBuf();
   
   Con::addConsumer(iPhoneConsoleConsumer);
}

iPhoneConsole::~iPhoneConsole()
{
   Con::removeConsumer(iPhoneConsoleConsumer);
}

void iPhoneConsole::processConsoleLine(const char *consoleLine)
{
   if(consoleEnabled)
   {
         printf("%s\n", consoleLine);
   }
	//%PUAP%
	if(debugOutputEnabled)
	{
		printf("%s\n", consoleLine);
	}

}

void iPhoneConsole::clearInBuf()
{
   dMemset(inBuf, 0, MaxConsoleLineSize);
   inBufPos=0;
}

void iPhoneConsole::inputLoop()
{
   Con::printf("Console Input Thread Started");
   unsigned char c;
   while(consoleEnabled)
   {
      c = fgetc(stdin);
      if(c == '\n')
      {
         // exec the line
         dStrcpy(postEvent.data, inBuf);
         postEvent.size = ConsoleEventHeaderSize + dStrlen(inBuf) + 1;
         Con::printf("=> %s",postEvent.data);
         Game->postEvent(postEvent);
         // clear the buffer
         clearInBuf();
         // display the prompt. Note that we're using real printf, not Con::printf...
         printf("=> ");
      }
      else
      {
         // add it to the buffer.
         inBuf[inBufPos++] = c;
         // if we're full, clear & warn.
         if(inBufPos >= MaxConsoleLineSize-1)
         {
            clearInBuf();
            Con::warnf("Line to long, discarding the last 512 bytes...");
         }
      }
   }
   Con::printf("Console Input Thread Stopped");
}
