//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------
$Game::CommonVersion = 114;

//---------------------------------------------------------------------------------------------
// initializeCommon
// Initializes common game functionality.
//---------------------------------------------------------------------------------------------
$Game::ConsoleBind = "ctrl tilde";
function initializeCommon()
{

   // Common keybindings.
   // GlobalActionMap doesn't get preference anymore so need special sequence to toggle console.
   // This also allows ~ to be used in the console now ;p
   // Example :  
	if($platform !$= "iphone")
	{
   		GlobalActionMap.bind(keyboard, $Game::ConsoleBind, ToggleConsole);
	}

   
   // Very basic functions used by everyone.
   exec("./audio.cs");
   exec("./canvas.cs");

   // Seed the random number generator.
   setRandomSeed();
   
   // Initialize the canvas.
   initializeCanvas( $Game::ProductName );
   
   // Start up the audio system.
   if( $Game::UsesAudio )
      initializeOpenAL();
   
   // Important Scripts.
   %startTime = getRealTime();
   
   exec("./keybindings.cs");
   exec("./projectManagement.cs");
   exec("./levelManagement.cs");
   exec("./projectResources.cs");
   exec("./SoundManager.cs");
   
   %runTime = getRealTime() - %startTime;
   echo(" % - Common scripts load time : " @ %runTime @ " ms");

   // Load client and server scripts.
   if( $pref::iDevice::UseNetwork )
   {
      setNetPort(0);
      initBaseClient();
      initBaseServer();
   }
   // Set a default cursor.
   Canvas.setCursor(DefaultCursor);
   Canvas.showCursor();
   
   loadKeybindings();

   $commonInitialized = true;

}

//---------------------------------------------------------------------------------------------
// _shutdownCommon
// Shuts down common game functionality.
//---------------------------------------------------------------------------------------------
function _shutdownCommon()
{
   if(isFunction("shutdownProject"))
      shutdownProject();
      
   shutdownOpenAL();
}

//---------------------------------------------------------------------------------------------
// dumpKeybindings
// Saves of all keybindings.
//---------------------------------------------------------------------------------------------
function dumpKeybindings()
{
   // Loop through all the binds.
   for (%i = 0; %i < $keybindCount; %i++)
   {
      // If we haven't dealt with this map yet...
      if (isObject($keybindMap[%i]))
      {
         // Save and delete.
         $keybindMap[%i].save("~/prefs/bind.cs", %i == 0 ? false : true);
         $keybindMap[%i].delete();
      }
   }
}

//---------------------------------------------------------------------------------------------
// initBaseClient
// Initializes necessary client functionality.
//---------------------------------------------------------------------------------------------
function initBaseClient()
{
   // Base client scripts.
   exec("./client/client.cs");
   exec("./client/message.cs");
   exec("./client/serverConnection.cs");
   exec("./client/chatClient.cs");
}

//---------------------------------------------------------------------------------------------
// initBaseServer
// Initializes necessary server functionality.
//---------------------------------------------------------------------------------------------
function initBaseServer()
{
   // Base server scripts.
   exec("./server/server.cs");
   exec("./server/message.cs");
   exec("./server/clientConnection.cs");
   exec("./server/kickban.cs");
   exec("./server/chatServer.cs");
}

//--------------------------------------------------------------------------
// Load a Mod Directory.
//--------------------------------------------------------------------------
function loadDir( %dir )
{
   // Set Mod Paths.
   setModPaths( getModPaths() @ ";" @ %dir );
   
   // Execute Boot-strap file.
   exec( %dir @ "/main.cs" );
}


//---------------------------------------------------------------------------------------------
// Cursor toggle functions.
//---------------------------------------------------------------------------------------------
$cursorControlled = false;
function showCursor()
{
   if ($cursorControlled)
      lockMouse(false);
   Canvas.cursorOn();
}

function hideCursor()
{
   if ($cursorControlled)
      lockMouse(true);
   Canvas.cursorOff();
}

//--------------------------------------------------------------------------
// Rounds a number
//--------------------------------------------------------------------------
function mRound(%num)
{
   if((%num-mFloor(%num)) >= 0.5)
   {
      %value = mCeil(%num);
   }
   else
   {
      %value = mFloor(%num);
   }    

   return %value;
}