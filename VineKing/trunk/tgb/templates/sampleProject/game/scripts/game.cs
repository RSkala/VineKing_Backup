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
   
      moveMap.bind(joystick0, xaxis, "joystickMoveX" );
      moveMap.bind(joystick0, yaxis, "joystickMoveY" );
      moveMap.bind(joystick0, Zaxis, "joystickMoveZ" );
      
      if($platform !$= "iphone")
      {
         moveMap.bindCmd(keyboard, "left", "leftDownHandler(1);","leftDownHandler(0);" );
         moveMap.bindCmd(keyboard, "right", "rightDownHandler(1);","rightDownHandler(0);");
         moveMap.bindCmd(keyboard, "up", "upDownHandler(1);","upDownHandler(0);");
      }
      
   moveMap.push();
   
   
   $enableDirectInput = true;
   activateDirectInput();
   enableJoystick();
   
   //Time to start game is measured from the very entry point, until now
   %runTime = getRealTime() - $Debug::loadStartTime;
   echo(" % - Game load time : " @ %runTime @ " ms");
   
   //Set some defaults.
   $onGround = false;
   $groundY = 33.3;
   $gravity = 0.6;
   $airDir = $gravity;
   
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

function upDownHandler(%val)
{
   if(%val == 1)
   {
      if($onGround)
      {
         $onGround = false;
         $airDir = -$gravity;  //to subtract jumping direction from y
      }
   }
   else
   {
      
   }
}

function leftDownHandler(%val)
{
   if(%val == 1)
   {
      leftArrow::onMouseDown(leftArrow);
   }
   else
   {
      leftArrow::onMouseUp(leftArrow);
      leftArrow::onMouseLeave(leftArrow);
   }
}

function rightDownHandler(%val)
{
   if(%val == 1)
   {
      rightArrow::onMouseDown(rightArrow);
   }
   else
   {
      rightArrow::onMouseUp(rightArrow);
      rightArrow::onMouseLeave(rightArrow);
   }
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

//Handle our simple gameplay
//Schedule an update

schedule(33, 0, updateGameplay);
function updateGameplay()
{
   
   //Only update if there is a need
   if($leftDown || $rightDown)
   {
      %pos = player.getPosition();
      %x = getWord(%pos, 0);
      %y = getWord(%pos, 1);
      
      
      //sprite is rotated 90 degrees, so flipY to see horizontal change
      
      if($leftDown)
      {
         if(player.FlipY)
            player.FlipY = false;
         
         if(%x > -48)
            %x -= 1;
      }
      else if($rightDown)
      {
         if(!player.FlipY)
            player.FlipY = true;
          
         if(%x < 48)
            %x += 1;
      }
         
      player.setPosition(%x , %y);
   }
   
   //Update jumping
   if(!$onGround)
   {
      //if we are off the ground, use the player position
      
      %pos = player.getPosition();
      %x = getWord(%pos, 0);
      %y = getWord(%pos, 1);  
      
      %y += $airDir;
       
      //am i in the air? 
      if($airDir == $gravity) //busy falling?
      {
         if(%y > $groundY)
         {
            //Done jumping
            %y = $groundY;
            $onGround = true;
            $airDir = -$gravity;
         }
      }
      else
      {
         //busy jumping? have we reached the peak?
         if(%y < ($groundY - 5))
         {
            $airDir = $gravity;
         }
      }

      player.setPosition(%x, %y );
        
   }
   
   //reschedule the update   
   schedule(33, 0, updateGameplay);   
}

function leftArrow::onMouseDown(%this, %modifier, %pos, %clickCount)
{
   $leftDown = true;
   %this.setBlendAlpha(1);
}

function leftArrow::onMouseLeave(%this, %modifier, %pos, %clickCount)
{
   $leftDown = false;
   %this.setBlendAlpha(0.3); 
}

function rightArrow::onMouseLeave(%this, %modifier, %pos, %clickCount)
{
   $rightDown = false;
   %this.setBlendAlpha(0.3);   
}

function rightArrow::onMouseDown(%this, %modifier, %pos, %clickCount)
{
   $rightDown = true;
   %this.setBlendAlpha(1);   
}

function rightArrow::onMouseUp(%this, %modifier, %pos, %clickCount)
{
   $rightDown = false;
   %this.setBlendAlpha(0.3);   
}

function leftArrow::onMouseUp(%this, %modifier, %pos, %clickCount)
{
   $leftDown = false;
   %this.setBlendAlpha(0.3);   
}