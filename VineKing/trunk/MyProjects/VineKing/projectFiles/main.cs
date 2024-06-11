//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

//enableDebugOutput( true );
dbgSetParameters( 6060, "password", false );

/// Player Initialization Procedure
/// 
function onStart()
{   
}

function onExit()
{
}

//---------------------------------------------------------------------------------------------
// Load the paths we need access to
//---------------------------------------------------------------------------------------------
function loadPath( %path )
{
   setModPaths( getModPaths() @ ";" @ %path );
   exec(%path @ "/main.cs");

}

//---------------------------------------------
// Do some bootstrap voodoo to get the game to 
// the initializeProject phase of loading and 
// pass off to the user
//---------------------------------------------

 $Debug::loadStartTime = getRealTime();

// Output a console log
setLogMode(6);

loadPath( "common" );

loadPath( "game" );

onStart();

// Initialized
echo("\nTorque Game Builder (" @ getT2DVersion() @ ") initialized...");
echo("Torque2D for iPhone " @ getiPhoneToolsVersion() @ " is active...");


if( !isFunction( "initializeProject" ) || !isFunction( "_initializeProject" ) )
{
   messageBox( "Game Startup Error", "'initializeProject' function could not be found." @
               "\nThis could indicate a bad or corrupt common directory for your game." @
               "\n\nThe Game will now shutdown because it cannot properly function", "Ok", "MIStop" );
   quit();
}

_initializeProject();

// Startup the project
initializeProject();


