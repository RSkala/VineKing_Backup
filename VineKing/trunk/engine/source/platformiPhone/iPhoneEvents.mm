//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformiPhone/platformiPhone.h"
#include "platform/event.h"
#include "platform/platformInput.h"
#include "platform/gameInterface.h"
#include "platform/threads/thread.h"
#include "platform/platformVideo.h"
#include <pthread.h>

#include "platformiPhone/iPhoneUtil.h"
#include "platformiPhone/iPhoneEvents.h"
#include "platformiPhone/iPhoneAlerts.h"
#include "gui/core/guiCanvas.h"



/*
EventHandlerRef gWinMouseEventHandlerRef  = NULL;
EventHandlerRef gAppMouseEventHandlerRef  = NULL;
*/



//-----------------------------------------------------------------------------
static void _OnActivate(bool activating)
{
   if(activating)
   {
      Input::activate();
      Game->refreshWindow();
      platState.backgrounded = false;
      pthread_kill((pthread_t)platState.torqueThreadId, SIGALRM);
   }
   else
   {
      Input::deactivate();
      platState.backgrounded = true;
   }
}

//-----------------------------------------------------------------------------
// here we manually poll for events, and send them to the dispatcher.
// we only use this in single-threaded mode.
static void _iPhonePollEvents()
{
}



//-----------------------------------------------------------------------------
void Platform::enableKeyboardTranslation(void)
{
	platState.tsmActive=true;
}

//-----------------------------------------------------------------------------
void Platform::disableKeyboardTranslation(void)
{
	platState.tsmActive=false;
}


void Platform::setWindowLocked(bool locked)
{
}




//-----------------------------------------------------------------------------
void Platform::process()
{
	// TODO: HID input
	
	// ProcessMessages() manually polls for events when we are single threaded
	//   if(ThreadManager::isCurrentThread(platState.firstThreadId))
	//      _iPhonePollEvents();
	
	// Some things do not get carbon events, we must always poll for them.
	// HID ( usb gamepad et al ) input, for instance.
	Input::process();
	
	if(platState.ctxNeedsUpdate)
	{
		//aglUpdateContext(platState.ctx);
		platState.ctxNeedsUpdate=false;
	}
}

