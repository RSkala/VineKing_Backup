//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "platformiPhone/platformiPhone.h"
#include "platformiPhone/iPhoneEvents.h"
#include "platformiPhone/iPhoneUtil.h"
#include "platform/threads/thread.h"
#include "platform/gameInterface.h"
#include "core/fileio.h"

//-------------------------------------------------------------------------------------------------------
// RKS NOTE: I added this to fix the issue where sound stops working after receiving a phone call.
#include "platform/platformAudio.h" 
#include "component/ComponentGlobals.h" // This was added so the game could be paused when resigning active
#include "component/SoundManager.h"

void _RestartAudioAfterBecomingActive();
static bool g_bDeviceWasInactive = false;

//-------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _iPhoneRunTorqueMain() starts the Game.main() loop
//-----------------------------------------------------------------------------

static BOOL CAdisplayLinkSupported = NO;
id displayLink = nil;

int _iPhoneRunTorqueMain( id appID, UIView *Window, UIApplication *app )
{
	platState.appID = appID;	
	platState.firstThreadId = ThreadManager::getCurrentThreadId();
	platState.Window = Window;
	platState.application = app;
	
	//Hidden by default.
	platState.application.statusBarHidden = YES;
	
#if !defined(TORQUE_MULTITHREAD)
	printf("performing mainInit()\n");
	platState.lastTimeTick = Platform::getRealMilliseconds();
	if(!Game->mainInit(platState.argc, platState.argv))
	{
		return 0;
	}
	
	//Luma: CADisplayLink main loop support
	//Currently needs to move to app delegate or platstate.
	NSString *reqSysVer = @"3.1";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
		CAdisplayLinkSupported = YES;

	// start the game going
    if(!CAdisplayLinkSupported)
    {
        iPhoneRunEventLoopTimer(sgTimeManagerProcessInterval);
    }
    else
    {
        //We can use the CADisplayLink to update the game now.
		//Luma: The magic number 1 below is because of what the docs say.
		
		/*
		 The default value is 1, which results in your application being notified at the refresh rate of the display.
		 If the value is set to a value larger than 1, the display link notifies your application at a fraction of the
		 native refresh rate. For example, setting the interval to 2 causes the display link to fire every other frame, 
		 providing half the frame rate.
         
		 Setting this value to less than 1 results in undefined behavior and is a programmer error.
		 */
        
        NSInteger updateDelay = 1;
		
		displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:appID selector:@selector(runMainLoop)];
		[displayLink setFrameInterval:updateDelay];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
	
	//Need a true or it will think its crashed, and fail.
	return true;
	
#else
	
#endif
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool g_bDoNotRun = false; // RKS NOTE: I added this according to this post: http://www.torquepowered.com/community/forums/viewthread/118571

//--------------------------------------------------------------------------------------------------------------------------------------------

S32 gLastStart = 0;
void _iPhoneGameInnerLoop()
{
	if( g_bDoNotRun ) // RKS NOTE: I added this
		return;
	
	if(Game->isRunning())
	{
		//if( g_bDoNotRun ) // RKS NOTE: I added this
		//	return;
		
		S32 start = Platform::getRealMilliseconds();
		Game->mainLoop();
		S32 time = sgTimeManagerProcessInterval - (start - gLastStart);
		gLastStart = start;
        if(!CAdisplayLinkSupported)
        {
            iPhoneRunEventLoopTimer(time);
        }
	}
	else
	{
		printf( "_iPhoneGameInnerLoop() - Shutting down game!\n" ); // RKS NOTE: I added this
		Game->mainShutdown();

		//Luma:	Need to actually exit the application now
		exit(0);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void _iPhoneGameResignActive()
{
	printf( "IN _iPhoneGameResignActive()\n" );
	
	g_bDoNotRun = true;
	g_bDeviceWasInactive = true;
	
	CComponentGlobals::OnDeviceResignActive();
	
	alxStopAll(); // RKS NOTE: Stop all sounds after resigning active.
	
	//Con::executef( 1, "oniPhoneResignActive" ); // RKS NOTE: I disabled this script call
}

//--------------------------------------------------------------------------------------------------------------------------------------------


void _iPhoneGameBecomeActive()
{
	printf( "IN _iPhoneGameBecomeActive()\n" );
	
	g_bDoNotRun = false;
	
	if( g_bDeviceWasInactive )
		_RestartAudioAfterBecomingActive();
	
	g_bDeviceWasInactive = false;
	
	//Con::executef( 1, "oniPhoneBecomeActive" ); // RKS NOTE: I disabled this script call
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void _RestartAudioAfterBecomingActive()
{
	if( Audio::OpenALInit() )
	{
		CSoundManager::GetInstance().PlayBGMAfterBecomingActive();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void _iPhoneGameWillTerminate()
{
	printf( "IN _iPhoneGameWillTerminate()\n" );
	
	//Con::executef( 1, "oniPhoneWillTerminate" ); // RKS NOTE: I disabled this script call

	Con::executef( 1, "onExit" );
	//-Mat make sure we quit

	Game->mainShutdown();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//Luma: Store current orientation for easy access
void _iPhoneGameChangeOrientation(S32 newOrientation)
{
	printf( "IN _iPhoneGameChangeOrientation()\n" ); // RKS NOTE: I added this
	
	_iPhoneGameSetCurrentOrientation(newOrientation);
	Con::executef( 2, "oniPhoneChangeOrientation", Con::getIntArg(newOrientation) );
	
//Sven: adding landscape auto rotation support, 
#ifdef TORQUE_ALLOW_AUTOROTATE
	
	if(Con::getIntVariable("$pref::iDevice::ScreenOrientation") != 1)
	{
	
		//Start "collecting animations"
		[UIView beginAnimations: nil context: nil];
			
		switch(newOrientation)
		{
			case iPhoneOrientationLandscapeLeft:
				platState.Window.transform = CGAffineTransformMakeRotation(mDegToRad(0.0f));
			break;
				
			case iPhoneOrientationLandscapeRight:
				platState.Window.transform = CGAffineTransformMakeRotation(mDegToRad(180.0f));	
			break;
			
				//Sventodo: This is for reference. You can also resize the view dynamically with
				//no stretching, etc. 
			/*case iPhoneOrientationPortrait:
				platState.Window.transform = CGAffineTransformMakeRotation(mDegToRad(270.0f));
			break;
				
			case iPhoneOrientationPortraitUpsideDown:
				platState.Window.transform = CGAffineTransformMakeRotation(mDegToRad(90.0f));	
			break;*/
			default:
				break;
		}
	
		//Show them
		[UIView commitAnimations];
		
	}
	else {
		//Only handling landscape orientation rotations. The other makes no sense by default.
	}

#endif //TORQUE_ALLOW_AUTOROTATE

}


void iPhoneRunEventLoopTimer(S32 intervalMs)
{
	
	//Luma: We only want to support NSTimer method, if below 3.1 OS.
	if(!CAdisplayLinkSupported)
	{
		if( intervalMs < 4 ) {
			intervalMs = 4;
		}
		NSTimeInterval interval = intervalMs / 1000.0; // EventTimerInterval is a double.
		platState.mainLoopTimer = [NSTimer scheduledTimerWithTimeInterval:interval target:platState.appID selector:@selector(runMainLoop) userInfo:nil repeats:NO];
		platState.sleepTicks = intervalMs;
	}
	else {
		//We can use the CADisplayLink to update the game now.
		//Luma: The magic number 1 below is because of what the docs say.
		
		/*
		 The default value is 1, which results in your application being notified at the refresh rate of the display.
		 If the value is set to a value larger than 1, the display link notifies your application at a fraction of the
		 native refresh rate. For example, setting the interval to 2 causes the display link to fire every other frame, 
		 providing half the frame rate.
		
		 Setting this value to less than 1 results in undefined behavior and is a programmer error.
		 */
		
		//NSInteger updateDelay = 1;
		
		//displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(runMainLoop)];
		//[displayLink setFrameInterval:updateDelay];
		//[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	}

}


//-----------------------------------------------------------------------------
// Handler stub for bsd signals.
//-----------------------------------------------------------------------------
static void _iPhoneSignalHandler(int )
{
   // we send the torque thread a SIGALRM to wake it up from usleep()
   // when transitioning from background - forground.
	printf( "IN _iPhoneSignalHandler()\n" ); // RKS NOTE: I added this
}


#pragma mark -

//-----------------------------------------------------------------------------
// command line arg processing
//-----------------------------------------------------------------------------
static void _iPhoneGetTxtFileArgs(int &argc, char** argv, int maxargc)
{
   argc = 0;
   
   const U32 kMaxTextLen = 2048; // arbitrary
   U32 textLen;
   char* text = new char[kMaxTextLen];   

   // open the file, kick out if we can't
   File cmdfile;
   File::Status err = cmdfile.open("iPhoneCmdLine.txt", cmdfile.Read);
   
   //Luma: Re-organise function to handle memory deletion better
   if(err == File::Ok)
   {
	   // read in the first kMaxTextLen bytes, kick out if we get errors or no data
	   err = cmdfile.read(kMaxTextLen-1, text, &textLen);
	   if(((err == File::Ok || err == File::EOS) || textLen > 0))
	   {
		   // null terminate
		   text[textLen++] = '\0';
		   // truncate to the 1st line of the file
		   for(int i = 0; i < textLen; i++)
		   {
			  if( text[i] == '\n' || text[i] == '\r' )
			  {
				 text[i] = '\0';
				 textLen = i+1;
				 break;
			  }
		   }

		   // tokenize the args with nulls, save them in argv, count them in argc
		   char* tok;
		   for(tok = dStrtok(text, " "); tok && argc < maxargc; tok = dStrtok(NULL, " "))
		   {
			  argv[argc++] = tok;
		   }
		}
	}
	
	//Luma: Close file and delete memory before returning
    cmdfile.close();
	delete[] text;
	text = NULL;
}

#pragma mark -

//-----------------------------------------------------------------------------
// main() - the real one - this is the actual program entry point.
//-----------------------------------------------------------------------------


//int main(int argc, char *argv[]) {
S32 main(S32 argc, char *argv[])
{	
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	int kMaxCmdlineArgs = 32; //Arbitrary
	
	printf("Initial Command Line\n");
	for( int i = 0; i < argc; i++ ) {
		printf("%i : %s", i, argv[i] );
	}
	
	NSString *nsStrVersion = [UIDevice currentDevice ].systemVersion;
	const char *strVersion = [nsStrVersion UTF8String ];
	platState.osVersion = dAtof( strVersion); 
	
   // Find Main.cs .
   const char* cwd = Platform::getMainDotCsDir();
   // Change to the directory that contains main.cs
   Platform::setCurrentDirectory(cwd);
   
   // get the actual command line args
   S32   newArgc = argc;
   const char* newArgv[kMaxCmdlineArgs];
   for(int i=0; i < argc && i < kMaxCmdlineArgs; i++)
      newArgv[i] = argv[i];

   // get the text file args
   S32 textArgc;
   char* textArgv[kMaxCmdlineArgs];
   _iPhoneGetTxtFileArgs(textArgc, textArgv, kMaxCmdlineArgs);
      
   // merge them
   int i=0;
   while(i < textArgc && newArgc < kMaxCmdlineArgs)
       newArgv[newArgc++] = textArgv[i++];
   
   // store them in platState
   platState.argc = newArgc;
   platState.argv = newArgv;
    
	printf("\nMerged Command Line\n");
	for( int i = 0; i < platState.argc; i++ ) {
		printf("%i : %s", i, platState.argv[i] );
	}
	printf("\n");
	
   // now, we run UIApplication which calls back and starts thread or timer
	platState.appReturn = UIApplicationMain(argc, argv, nil, nil);

	printf("exiting...\n");   
	[pool release];
	return(platState.appReturn);
}



