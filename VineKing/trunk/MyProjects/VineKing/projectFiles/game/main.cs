//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// initializeProject
// Perform game initialization here.
//---------------------------------------------------------------------------------------------
function initializeProject()
{
	//GenerateFonts(); // RKS NOTE: This should be disabled unless generating UTF font files!
	
   // Load up the in game gui.
   //A little more practical, load a mainscreen and set its size according to the orientation
   exec("~/gui/mainScreen.gui");
   
   if(isObject(mainScreenGui) && isObject(sceneWindow2D))
   {
      if($pref::iDevice::ScreenOrientation == 1) //portrait
      {
         if(($pref::iDevice::DeviceType != $iDevice::constant::iPad) && ($pref::iDevice::ScreenResolution != $iDevice::constant::ResolutionFull))
         {
            echo(" % - MainScreen set to Portrait! 320x480");
            mainScreenGui.Extent = "320 480";
            sceneWindow2D.Extent = "320 480";
         }
         else
         {
            echo(" % - MainScreen set to Portrait! 768x1024");
            mainScreenGui.Extent = "768 1024";
            sceneWindow2D.Extent = "768 1024";
         }
      }
      else
      {
         if(($pref::iDevice::DeviceType != $iDevice::constant::iPad) && ($pref::iDevice::ScreenResolution != $iDevice::constant::ResolutionFull))
         {
            echo(" % - MainScreen set to Landscape! 480x320 ");
            mainScreenGui.Extent = "480 320";
            sceneWindow2D.Extent = "480 320";
         }
         else
         {
            echo(" % - MainScreen set to Landscape! 1024x768 ");
            mainScreenGui.Extent = "1024 768";
            sceneWindow2D.Extent = "1024 768";
         }
      }
   }

   // Exec game scripts.
   exec("./scripts/game.cs");

   // This is where the game starts. Right now, we are just starting the first level. You will
   // want to expand this to load up a splash screen followed by a main menu depending on the
   // specific needs of your game. Most likely, a menu button will start the actual game, which
   // is where startGame should be called from.
   startGame( expandFilename($Project::Game::DefaultScene) );
}

//---------------------------------------------------------------------------------------------
// shutdownProject
// Clean up your game objects here.
//---------------------------------------------------------------------------------------------
function shutdownProject()
{
   endGame();
}

//---------------------------------------------------------------------------------------------
// setupKeybinds
// Bind keys to actions here..
//---------------------------------------------------------------------------------------------
function setupKeybinds()
{
   new ActionMap(moveMap);
   //moveMap.bind("keyboard", "a", "doAction", "Action Description");
}



//=====================================================================================================================================================
// RKS NOTE: I added this to create fonts
// RKS NOTE: The generated UTF files will be placed in ~/Library/Application Support/GarageGames/Torque Game Builder(iPhone)/common/data/fonts
function GenerateFonts()
{
	echo( "GenerateFonts() - THIS SHOULD ONLY BE RUN WHEN CREATING FONTS!" );
	
	// Generate font .utf files...  
	//%fontsizes = "16 18 20 28 32 80"; // include every size you want to use for this font
	//%fontSizes = "18 24 32";
	%fontSizes = "10 16 18 20 24 28 32 38 40 80";
	//%fontName = "Lambada LET";
	//%fontName = "Times New Roman Bold";
	%fontName = "Times New Roman Bold Italic";
	for( %i = 0; %i < getWordCount( %fontSizes ); %i++ )
	{
		// String is the name of the font (NOT the name of the TTF file)
		//populateFontCacheRange( "Bleeding Cowboys", getWord( %fontsizes, %i ), 32, 126 );
		populateFontCacheRange( %fontName, getWord( %fontsizes, %i ), 32, 126 );
	}
	writeFontCache();
}

//ConsoleFunction(populateFontCacheRange, void, 5, 5, "(faceName, size, rangeStart, rangeEnd) - "
// -> Note: rangeStart and rangeEnd are the start and end points for the Unicode points.

//=====================================================================================================================================================
