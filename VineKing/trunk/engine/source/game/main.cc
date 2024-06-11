//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/platformVideo.h"
#include "platform/platformInput.h"
#include "platform/platformAudio.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "core/tVector.h"
#include "core/chunkFile.h"
#include "math/mMath.h"
#include "dgl/dgl.h"
#include "dgl/gBitmap.h"
#include "core/resManager.h"
#include "core/fileStream.h"
#include "dgl/gTexManager.h"
#include "console/console.h"
#include "console/simBase.h"
#include "gui/core/guiCanvas.h"
#include "sim/actionMap.h"
#include "core/dnet.h"
#include "core/bitStream.h"
#include "console/telnetConsole.h"
#include "console/telnetDebugger.h"
#include "console/consoleTypes.h"
#include "math/mathTypes.h"
#include "dgl/gTexManager.h"
#include "core/resManager.h"
#include "platform/platformVideo.h"
#include "sim/netStringTable.h"
#include "platform/platformRedBook.h"
#include "game/demoGame.h"
#include "core/frameAllocator.h"
#include "game/version.h"
#include "platform/profiler.h"
#include "game/net/serverQuery.h"
#include "game/demoGame.h"
#include "platform/nativeDialogs/msgBox.h"
#include <stdio.h>

#ifdef TORQUE_OS_IPHONE
//-Mat PUAP profiler 
#include "platformiPhone/iPhoneProfiler.h"

#include "platformiPhone/GameCenterWrapper.h" // RKS: I added this
#include "platformiPhone/iPhoneInterfaceWrapper.h" // RKS: I added this
#include "component/ComponentGlobals.h"

#ifdef _USE_STORE_KIT
#include "platformiPhone/iPhoneStoreKit.h"
#endif

#ifdef TORQUE_ALLOW_MUSICPLAYER
#include "platformiPhone/iPhoneUserMusicLibrary.h"
#endif

#endif

// OpenFileDialog dependancy can be nix'd in most Torque installs. 
//  It is used to locate the game to debug in to with the TGB player 
//  This should be moved to a seperate location for TGB.. [2/16/2007 justind]
#include "platform/nativeDialogs/fileDialog.h"
#include "util/safeDelete.h"

#ifdef TORQUE_OS_MAC
#include "TGB/levelBuilderiPhoneUtil.h"
#endif //TORQUE_OS_MAC

#ifndef BUILD_TOOLS
DemoGame GameObject;
DemoNetInterface GameNetInterface;
#endif

ConsoleFunctionGroupBegin( Platform , "General platform functions.");

ConsoleFunction( lockMouse, void, 2, 2, "( isLocked ) Use the lockMouse function to un/lock the mouse.\n"
																"@param isLocked A boolean value.\n"
																"@return No return value")
{
   Platform::setWindowLocked(dAtob(argv[1]));
}

ConsoleFunction( setNetPort, bool, 2, 2, "(int port)"
                "Set the network port for the game to use.\n"
				"@param The requested port as an integer\n"
				"@return Returns true on success, flase on fail")
{
   return Net::openPort(dAtoi(argv[1]));
}

#ifdef TORQUE_ALLOW_JOURNALING
ConsoleFunction( saveJournal, void, 2, 2, "( namedFile ) Use the saveJournal function to save a new journal of the current game.\n"
																"@param namedFile A full path specifying the file to save this journal to. Usually, journal names end with the extension .jrn.\n"
																"@return No return value.\n"
																"@sa playJournal")
{
   Game->saveJournal(argv[1]);
}

ConsoleFunction( playJournal, void, 2, 3, "( namedFile , doBreak ) Use the playJournal function to play back a journal from namedFile and to optionally break (into an active debugger) after loading the Journal. This allow us to debug engine bugs by reproducing them consistently repeatedly.\n"
																"The journaling system is a vital tool for debugging complex or hard to reproduce engine and script bugs.\n"
																"@param namedFile A full path to a valid journal file. Usually, journal names end with the extension .jrn.\n"
																"@param doBreak A boolean value. If true, the engine will load the journal and then assert a break (to break into an active debugger). If not true, the engine will play back the journal with no break.\n"
																"@return No return value.\n"
																"@sa saveJournal")
{
   bool jBreak = (argc > 2)? dAtob(argv[2]): false;
   Game->playJournal(argv[1],jBreak);
}
#endif //TORQUE_ALLOW_JOURNALING

extern void netInit();
extern void processConnectedReceiveEvent( ConnectedReceiveEvent * event );
extern void processConnectedNotifyEvent( ConnectedNotifyEvent * event );
extern void processConnectedAcceptEvent( ConnectedAcceptEvent * event );
extern void ShowInit();

/// Initalizes the components of the game like the TextureManager, ResourceManager
/// console...etc.
static bool initLibraries()
{
	if(!Net::init())
	{
		//Platform::AlertOK("Network Error", "Unable to initialize the network... aborting.");
		printf("Network Error : Unable to initialize the network... aborting.");
		return false;
	}
	
   // asserts should be created FIRST
   PlatformAssert::create();

#ifdef TORQUE_OS_IPHONE
   //3MB default is way too big for iPhone!!!
#ifdef	TORQUE_SHIPPING
   FrameAllocator::init(256 * 1024);	//256KB for now... but let's test and see!
#else
   FrameAllocator::init(512 * 1024);	//512KB for now... but let's test and see!
#endif	//TORQUE_SHIPPING
#else
   FrameAllocator::init(3 << 20);      // 3 meg frame allocator buffer
#endif	//TORQUE_OS_IPHONE

//   // Cryptographic pool next
//   CryptRandomPool::init();

   _StringTable::create();
   TextureManager::create();
   ResManager::create();

   // Register known file types here
#ifndef _NO_JPG_SUPPORT
   ResourceManager->registerExtension(".jpg", constructBitmapJPEG);
#endif // _NO_JPG_SUPPORT

#ifndef _NO_BMP_SUPPORT
   ResourceManager->registerExtension(".bmp", constructBitmapBMP);
#endif // _NO_BMP_SUPPORT

   ResourceManager->registerExtension(".png", constructBitmapPNG);
   ResourceManager->registerExtension(".uft", constructNewFont);

#ifdef __IPHONE__
	ResourceManager->registerExtension(".pvr", constructBitmapPVR);
#endif	
   
   Con::init();
   Platform::initConsole();
   NetStringTable::create();
   
   TelnetConsole::create();
   TelnetDebugger::create();

   Processor::init();
   Math::init();
   Platform::init();    // platform specific initialization
   RedBook::init();
    
#if defined(__IPHONE__) && defined(_USE_STORE_KIT)
    storeInit();
#endif // __IPHONE__ && _USE_STORE_KIT
   
   return true;
}

/// Destroys all the things initalized in initLibraries
static void shutdownLibraries()
{
   // Purge any resources on the timeout list...
   if (ResourceManager)
      ResourceManager->purge();

   RedBook::destroy();

   TextureManager::preDestroy();
   
   Platform::shutdown();
   TelnetDebugger::destroy();
   TelnetConsole::destroy();

   NetStringTable::destroy();
   Con::shutdown();

   ResManager::destroy();
   TextureManager::destroy();

   _StringTable::destroy();

   // asserts should be destroyed LAST
   FrameAllocator::destroy();

   PlatformAssert::destroy();
   Net::shutdown();
    
#ifdef _USE_STORE_KIT
	storeCleanup();
#endif // _USE_STORE_KIT
}

ConsoleFunction( getSimTime, S32, 1, 1, "() Use the getSimTime function to get the time, in ticks, that has elapsed since the engine started executing.\n"
																"@return Returns the time in ticks since the engine was started.\n"
																"@sa getRealTime")
{
   return Sim::getCurrentTime();
}

ConsoleFunction( getRealTime, S32, 1, 1, "() Use the getRealTime function to the computer time in milliseconds.\n"
																"@return Returns the current real time in milliseconds.\n"
																"@sa getSimTime")
{
   return Platform::getRealMilliseconds();
}

ConsoleFunctionGroupEnd(Platform);

static StringTableEntry sgCompanyName = NULL;
static StringTableEntry sgProductName = NULL;
ConsoleFunction(setCompanyAndProduct, void, 3, 3, "(company, product) Sets the company and product information.")
{
   sgCompanyName = StringTable->insert(argv[1]);
   sgProductName = StringTable->insert(argv[2]);

   Con::setVariable("$Game::CompanyName", sgCompanyName);
   Con::setVariable("$Game::ProductName", sgProductName);

   char appDataPath[1024];
   dSprintf(appDataPath, sizeof(appDataPath), "%s/%s/%s", Platform::getUserDataDirectory(), sgCompanyName, sgProductName);
   
   ResourceManager->addPath(appDataPath);
}

static F32 gTimeScale = 1.0;
static U32 gTimeAdvance = 0;
static U32 gFrameSkip = 0;
static U32 gFrameCount = 0;

// Executes an entry script; can be controlled by command-line options.
bool runEntryScript (int argc, const char **argv)
{

#ifdef TORQUE_PLAYER
   if(argc > 2 && dStricmp(argv[1], "-project") == 0)
   {
      char playerPath[1024];
      Platform::makeFullPathName(argv[2], playerPath, sizeof(playerPath));
      Platform::setCurrentDirectory(playerPath);

      argv += 2;
      argc -= 2;
   }

#endif

   ResourceManager->setWriteablePath(Platform::getCurrentDirectory());
   ResourceManager->addPath( Platform::getCurrentDirectory() );

   // Executes an entry script file. This is "main.cs"
   // by default, but any file name (with no whitespace
   // in it) may be run if it is specified as the first
   // command-line parameter. The script used, default
   // or otherwise, is not compiled and is loaded here
   // directly because the resource system restricts
   // access to the "root" directory.

#ifdef TORQUE_ENABLE_VFS
   Zip::ZipArchive *vfs = openEmbeddedVFSArchive();
   bool useVFS = vfs != NULL;
#endif

   Stream *mainCsStream;

   // The working filestream.
   FileStream str; 

   const char *defaultScriptName = "main.cs";
	//--------------------------------------------------------------------------------------
	//char szCurrentPath[ 256 ];
	//char* pszCurrentPath;
	//pszCurrentPath = getcwd(szCurrentPath, sizeof( szCurrentPath ) );
	//printf( "Current Working Directory: %s\n", szCurrentPath ); // RKS: I did this
	//--------------------------------------------------------------------------------------
   bool useDefaultScript = true;

   // Check if any command-line parameters were passed (the first is just the app name).
   if (argc > 1)
   {
      // If so, check if the first parameter is a file to open.
      if ( (str.open(argv[1], FileStream::Read)) && (argv[1] != "") )
      {
         // If it opens, we assume it is the script to run.
         useDefaultScript = false;
#ifdef TORQUE_ENABLE_VFS
         useVFS = false;
#endif
         mainCsStream = &str;
      }
   }

   if (useDefaultScript)
   {
      bool success = false;

#ifdef TORQUE_ENABLE_VFS
      if(useVFS)
         success = (mainCsStream = vfs->openFile(defaultScriptName, Zip::ZipArchive::Read)) != NULL;
      else
#endif
         success = str.open(defaultScriptName, FileStream::Read);

#ifdef TORQUE_DEBUG
      if (!success)
      {
         OpenFileDialog ofd;
         FileDialogData &fdd = ofd.getData();
         fdd.mFilters = StringTable->insert("Main Entry Script (main.cs)|main.cs|");
         fdd.mTitle   = StringTable->insert("Locate Game Entry Script");

         // Get the user's selection
         if( !ofd.Execute() )
            return false;

         // Process and update CWD so we can run the selected main.cs
         S32 pathLen = dStrlen( fdd.mFile );
         FrameTemp<char> szPathCopy( pathLen + 1);

         dStrcpy( szPathCopy, fdd.mFile );
         //forwardslash( szPathCopy );

         const char *path = dStrrchr(szPathCopy, '/');
         if(path)
         {
            U32 len = path - (const char*)szPathCopy;
            szPathCopy[len+1] = 0;

            Platform::setCurrentDirectory(szPathCopy);

            ResourceManager->setWriteablePath(Platform::getCurrentDirectory());
            ResourceManager->addPath( Platform::getCurrentDirectory() );

            success = str.open(fdd.mFile, FileStream::Read);
            if(success)
               defaultScriptName = fdd.mFile;
         }
      }
#endif
      if( !success )
      {
         char msg[1024];
         dSprintf(msg, sizeof(msg), "Failed to open \"%s\".", defaultScriptName);
		  //using printf because Con:: doesnt exist yet/being removed at the time. -Sven
		  printf(" Error : %s", msg); // RKS: Torque stops running here if files were not copied over from "Copy Bundle Resources"
         //Platform::AlertOK("Error", msg);
#ifdef TORQUE_ENABLE_VFS
         closeEmbeddedVFSArchive();
#endif

         return false;
      }

#ifdef TORQUE_ENABLE_VFS
      if(! useVFS)
#endif
         mainCsStream = &str;
   }

   U32 size = mainCsStream->getStreamSize();
   char *script = new char[size + 1];
   mainCsStream->read(size, script);

#ifdef TORQUE_ENABLE_VFS
   if(useVFS)
      vfs->closeFile(mainCsStream);
   else
#endif
      str.close();

   script[size] = 0;

   char buffer[1024], *ptr;
   Platform::makeFullPathName(useDefaultScript ? defaultScriptName : argv[1], buffer, sizeof(buffer), Platform::getCurrentDirectory());
   if(ptr = dStrrchr(buffer, '/'))
      *ptr = 0;
   Platform::setMainDotCsDir(buffer);
   Platform::setCurrentDirectory(buffer);

   Con::evaluate(script, false, useDefaultScript ? defaultScriptName : argv[1]); 
   delete[] script;

   return true;
}

/// Initalize game, run the specified startup script
bool initGame(int argc, const char **argv)
{
   Con::setFloatVariable("Video::texResidentPercentage", -1.0f);
   Con::setIntVariable("Video::textureCacheMisses", -1);
   Con::addVariable("timeScale", TypeF32, &gTimeScale);
   Con::addVariable("timeAdvance", TypeS32, &gTimeAdvance);
   Con::addVariable("frameSkip", TypeS32, &gFrameSkip);

   initMessageBoxVars();

   //
#ifdef TORQUE_GATHER_METRICS
   Con::addVariable("Video::numTexelsLoaded", TypeS32, &TextureManager::smTextureSpaceLoaded);
#else
   static U32 sBogusNTL = 0;
   Con::addVariable("Video::numTexelsLoaded", TypeS32, &sBogusNTL);
#endif
   netInit();
   //GameInit();
   //ShowInit();
   //MoveManager::init();

   Sim::init();

   ActionMap* globalMap = new ActionMap;
   globalMap->registerObject("GlobalActionMap");
   Sim::getActiveActionMapSet()->pushObject(globalMap);
   SimChunk::initChunkMappings();
	
	CComponentGlobals::GetInstance().SetIOSDeviceType(); // RKS: I added this

   // run the entry script and return.
   return runEntryScript(argc, argv);
}

/// Shutdown the game and delete core objects
void shutdownGame()
{

#ifdef TORQUE_TOOLS
   // Tools are given a chance to do pre-quit processing
   // - This is because for tools we like to do things such
   //   as prompting to save changes before shutting down
   //   and onExit is packaged which means we can't be sure
   //   where in the shutdown namespace chain we are when using
   //   onExit since some components of the tools may already be
   //   destroyed that may be vital to saving changes to avoid
   //   loss of work [1/5/2007 justind]
   if( Con::isFunction("onPreExit") )
      Con::executef(1, "onPreExit");

#endif
	dPrintf( "main.cc, shutdownGame(), Calling script \"onExit()\"\n" ); // RKS
   //exec the script onExit() function
	
	ActivityIndicatorWrapper::DeleteIndicator();
	GameCenterWrapper::Close(); // RKS: I added this
	
   Con::executef(1, "onExit");

   // Note: tho the SceneGraphs are created after the Manager, delete them after, rather
   //  than before to make sure that all the objects are removed from the graph.
   Sim::shutdown();
}

extern bool gDGLRender;
bool gShuttingDown   = false;

bool DemoGame::mainInit(int argc, const char **argv)
{
//   if (argc == 1) {
//      static const char* argvFake[] = { "dtest.exe", "-jload", "test.jrn" };
//      argc = 3;
//      argv = argvFake;
//   }

//   Memory::enableLogging("testMem.log");
//   Memory::setBreakAlloc(104717);
	
   if(!initLibraries())
      return false;
	
   // Set up the command line args for the console scripts...
   Con::setIntVariable("Game::argc", argc);
   U32 i;
   for (i = 0; i < argc; i++)
      Con::setVariable(avar("Game::argv%d", i), argv[i]);
   if (initGame(argc, argv) == false)
   { 
      //Using printf cos Con:: is not around here.
	   printf("\nApplication failed to start! Make sure your resources are in the correct place.");
      shutdownGame();
      shutdownLibraries();
      gShuttingDown = true;
      return false;
   }
	
#ifdef TORQUE_OS_IPHONE	
	//-Mat this is a bit of a hack, but if we don't want the network, we shut it off now. We can't do it until we've run the entry script, otherwise the script variable will not have ben loaded
	bool usesNet = dAtob( Con::getVariable( "$pref::iDevice::UseNetwork" ) );
	if( !usesNet ) {
		Net::shutdown();
	}
	
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerProfilerInit();
#endif
#endif

   return true;
}

/// Inner loop of the game engine
void DemoGame::mainLoop()
{	
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerStart("MAIN_LOOP");
#endif	
         PROFILE_START(MainLoop);
#ifdef TORQUE_ALLOW_JOURNALING
         PROFILE_START(JournalMain);
   Game->journalProcess();
         PROFILE_END();
#endif // TORQUE_ALLOW_JOURNALING
         PROFILE_START(NetProcessMain);
   Net::process();      // read in all events
         PROFILE_END();
         PROFILE_START(PlatformProcessMain);
	Platform::process(); // keys, etc.
         PROFILE_END();
         PROFILE_START(TelconsoleProcessMain);
   TelConsole->process();
         PROFILE_END();
         PROFILE_START(TelDebuggerProcessMain);
   TelDebugger->process();
         PROFILE_END();
         PROFILE_START(TimeManagerProcessMain);
   TimeManager::process(); // guaranteed to produce an event
         PROFILE_END();
         PROFILE_START(GameProcessEvents);
	Game->processEvents(); // process all non-sim posted events.
		 PROFILE_END();
         PROFILE_END();
	
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerEnd("MAIN_LOOP");
	if(iPhoneProfilerGetCount() >= 60){
		iPhoneProfilerPrintAllResults();
		iPhoneProfilerProfilerInit();
	}
#endif
}

void DemoGame::mainShutdown()
{
   shutdownGame();
   shutdownLibraries();

   if( Game->requiresRestart() )
      Platform::restartInstance();

   gShuttingDown = true;
}

// UNUSED: JOSEPH THOMAS -> static bool serverTick = false;

static F32 fpsRealStart;
static F32 fpsRealLast;
//static F32 fpsRealTotal;
static F32 fpsReal;
static F32 fpsVirtualStart;
static F32 fpsVirtualLast;
//static F32 fpsVirtualTotal;
static F32 fpsVirtual;
static F32 fpsFrames;
static F32 fpsNext;
static bool fpsInit = false;
const F32 UPDATE_INTERVAL = 0.25f;

//--------------------------------------

/// Resets the FPS variables
void fpsReset()
{
   fpsRealStart    = (F32)Platform::getRealMilliseconds()/1000.0f;      // Real-World Tick Count
   fpsVirtualStart = (F32)Platform::getVirtualMilliseconds()/1000.0f;   // Engine Tick Count (does not vary between frames)
   fpsNext         = fpsRealStart + UPDATE_INTERVAL;

//   fpsRealTotal= 0.0f;
   fpsRealLast = 0.0f;
   fpsReal     = 0.0f;
//   fpsVirtualTotal = 0.0f;
   fpsVirtualLast  = 0.0f;
   fpsVirtual      = 0.0f;
   fpsFrames = 0;
   fpsInit   = true;
}

//--------------------------------------

/// Updates the FPS variables
void fpsUpdate()
{
   if (!fpsInit)
      fpsReset();

   const float alpha  = 0.07f;
   F32 realSeconds    = (F32)Platform::getRealMilliseconds()/1000.0f;
   F32 virtualSeconds = (F32)Platform::getVirtualMilliseconds()/1000.0f;

   fpsFrames++;
   if (fpsFrames > 1)
   {
      fpsReal    = fpsReal*(1.0-alpha) + (realSeconds-fpsRealLast)*alpha;
      fpsVirtual = fpsVirtual*(1.0-alpha) + (virtualSeconds-fpsVirtualLast)*alpha;
   }
//   fpsRealTotal    = fpsFrames/(realSeconds - fpsRealStart);
//   fpsVirtualTotal = fpsFrames/(virtualSeconds - fpsVirtualStart);

   fpsRealLast    = realSeconds;
   fpsVirtualLast = virtualSeconds;

   // update variables every few frames
   F32 update = fpsRealLast - fpsNext;
   if (update > 0.5f)
   {
      //Con::setVariable("fps::realTotal",    avar("%4.1f", fpsRealTotal));
      //Con::setVariable("fps::virtualTotal", avar("%4.1f", fpsVirtualTotal));
      Con::setVariable("fps::real",    avar("%4.1f", 1.0f/fpsReal));
      Con::setVariable("fps::virtual", avar("%4.1f", 1.0f/fpsVirtual));
      if (update > UPDATE_INTERVAL)
         fpsNext  = fpsRealLast + UPDATE_INTERVAL;
      else
         fpsNext += UPDATE_INTERVAL;
   }
}


/// Process a mouse movement event, essentially pass to the canvas for handling
void DemoGame::processMouseMoveEvent(MouseMoveEvent * mEvent)
{
   if (Canvas)
      Canvas->processMouseMoveEvent(mEvent);
}

void DemoGame::processScreenTouchEvent(ScreenTouchEvent * mEvent)  
{
	if (Canvas)
		Canvas->processScreenTouchEvent(mEvent);
}

/// Process an input event, pass to canvas for handling
void DemoGame::processInputEvent(InputEvent *event)
{
   PROFILE_START(ProcessInputEvent);
   // [neo, 5/24/2007 - #2986]
   // Swapped around the order of call for global action map and canvas input 
   // handling to give canvas first go as GlobalActionMap will eat any input 
   // events meant for firstResponders only and as a "general" trap should really 
   // should only be called if any "local" traps did not take it, e.g. left/right 
   // in a text edit control should not be forwarded if the text edit has focus, etc. 
   // Any new issues regarding input should most probably start looking here first!
   if(!(Canvas && Canvas->processInputEvent(event)))
   {
      if(!ActionMap::handleEventGlobal(event))
      {
         // Other input consumers here...      
         ActionMap::handleEvent(event);
      }
   }
   PROFILE_END();
}

/// Process a quit event
void DemoGame::processQuitEvent()
{
   setRunning(false);
}

/// Refresh the game window, ask the canvas to set all regions to dirty (need to be updated)
void DemoGame::refreshWindow()
{
   if(Canvas)
      Canvas->resetUpdateRegions();
}

/// Process a console event
void DemoGame::processConsoleEvent(ConsoleEvent *event)
{
   char *argv[2];
   sprintf(argv[0], "eval");
   //argv[0] = "eval";
   argv[1] = event->data;
   Sim::postCurrentEvent(Sim::getRootGroup(), new SimConsoleEvent(2, const_cast<const char**>(argv), false));
}

/// Process a time event and update all sub-processes
void DemoGame::processTimeEvent(TimeEvent *event)
{
	PROFILE_START(ProcessTimeEvent);
   U32 elapsedTime = event->elapsedTime;
   // cap the elapsed time to one second
   // if it's more than that we're probably in a bad catch-up situation

   if(elapsedTime > 1024) {
      //Con::printf( "\nCapping elapsed time from %i to %i \n", elapsedTime, 1024 );
      elapsedTime = 1024;
   }

   U32 timeDelta;

   if(gTimeAdvance)
      timeDelta = gTimeAdvance;
   else
      timeDelta = (U32) (elapsedTime * gTimeScale);

   Platform::advanceTime(elapsedTime);
   bool tickPass;
	PROFILE_START(ServerProcess);
#ifdef TORQUE_OS_IPHONE_PROFILE
iPhoneProfilerStart("SERVER_PROC");
#endif
	
	tickPass = serverProcess(timeDelta);
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerEnd("SERVER_PROC");
#endif
	PROFILE_END();	
   PROFILE_START(ServerNetProcess);
   // only send packets if a tick happened
   if(tickPass)
      GNet->processServer();
   PROFILE_END();
	
   PROFILE_START(SimAdvanceTime);
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerStart("SIM_TIME");
#endif
	Sim::advanceTime(timeDelta);
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerEnd("SIM_TIME");
#endif
	PROFILE_END();

   PROFILE_START(ClientProcess);
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerStart("CLIENT_PROC");
#endif
	tickPass = clientProcess(timeDelta);
#ifdef TORQUE_OS_IPHONE_PROFILE
	iPhoneProfilerEnd("CLIENT_PROC");
#endif
	PROFILE_END();
   PROFILE_START(ClientNetProcess);
   if(tickPass)
      GNet->processClient();
   PROFILE_END();
	
   if(Canvas && gDGLRender)
   {
#ifdef TORQUE_OS_IPHONE_PROFILE	   
iPhoneProfilerStart("GL_RENDER");
#endif
      bool preRenderOnly = false;
      if(gFrameSkip && gFrameCount % gFrameSkip)
         preRenderOnly = true;

      PROFILE_START(RenderFrame);
      //ShapeBase::incRenderFrame();
      Canvas->renderFrame(preRenderOnly);
      PROFILE_END();
      gFrameCount++;
#ifdef TORQUE_OS_IPHONE_PROFILE
iPhoneProfilerEnd("GL_RENDER");
#endif
   }
   GNet->checkTimeouts();
   fpsUpdate();
	
#ifdef TORQUE_ALLOW_MUSICPLAYER
	updateVolume();
#endif
   PROFILE_END();

   // Update the console time
   Con::setFloatVariable("Sim::Time",F32(Platform::getVirtualMilliseconds()) / 1000);
}

/// Re-activate the game from, say, a minimized state
void GameReactivate()
{
   if ( !Input::isEnabled() )
      Input::enable();

   if ( !Input::isActive() )
      Input::reactivate();

   gDGLRender = true;
   if ( Canvas )
      Canvas->resetUpdateRegions();
}

/// De-activate the game in responce to, say, a minimize event
void GameDeactivate( bool noRender )
{
   if ( Input::isActive() )
      Input::deactivate();

   if ( Input::isEnabled() )
      Input::disable();

   if ( noRender )
      gDGLRender = false;
}

/// Invalidate all the textures
void DemoGame::textureKill()
{
   TextureManager::makeZombie();
}

/// Reaquire all textures
void DemoGame::textureResurrect()
{
   TextureManager::resurrect();
}

/// Process recieved net-packets
void DemoGame::processPacketReceiveEvent(PacketReceiveEvent * prEvent)
{
   GNet->processPacketReceiveEvent(prEvent);
}
