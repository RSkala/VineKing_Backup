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
   startGame( expandFilename($Game::DefaultScene) );
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
