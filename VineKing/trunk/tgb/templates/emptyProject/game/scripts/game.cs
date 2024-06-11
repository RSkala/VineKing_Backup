//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// startGame
// All game logic should be set up here. This will be called by the level builder when you
// select "Run Game" or by the startup process of your game to load the first level.
//---------------------------------------------------------------------------------------------
function startGame(%level)
{
   Canvas.setContent(mainScreenGui);
   
   new ActionMap(moveMap);   
     //  moveMap.bind(joystick0, xaxis, "joystickMoveX" );
     //  moveMap.bind(joystick0, yaxis, "joystickMoveY" );
     //  moveMap.bind(joystick0, Zaxis, "joystickMoveZ" );
   moveMap.push();
   
   
   $enableDirectInput = true;
   activateDirectInput();
   enableJoystick();
   
   //Time to start game is measured from the very entry point, until now
   %runTime = getRealTime() - $Debug::loadStartTime;
   echo(" % - Game load time : " @ %runTime @ " ms");
   
   sceneWindow2D.loadLevel(%level);
}

//---------------------------------------------------------------------------------------------
// endGame
// Game cleanup should be done here.
//---------------------------------------------------------------------------------------------
function endGame()
{
   sceneWindow2D.endLevel();
   moveMap.pop();
   moveMap.delete();
}

function oniPhoneChangeOrientation(%newOrientation)
{
	%new = "Unkown";
	if(%newOrientation == $iDevice::constant::OrientationLandscapeLeft)
	{
		%new = "Landscape Left (Home Button on the right)";
	}
	else if(%newOrientation == $iDevice::constant::OrientationLandscapeRight)
	{
		%new = "Landscape Right (Home Button on the left)";
	}
	else if(%newOrientation == $iDevice::constant::OrientationPortrait)
	{
		%new = "Portrait (Home Button on the bottom)";
	}
	else if(%newOrientation == $iDevice::constant::OrientationPortraitUpsideDown)
	{
		%new = "Portrait Upside Down (Home Button on the top)";
	}
		
	echo("newOrientation: " @ %new);
}

function oniPhoneTouchDown( %touchCount, %touchX, %touchY ) 
{
}

function oniPhoneTouchUp( %touchCount, %touchX, %touchY ) 
{
}

function oniPhoneTouchMove( %touchCount, %touchX, %touchY ) 
{

}

//Luma: Tap support
function oniPhoneTouchTap ( %touchCount, %touchX, %touchY )
{ 
}