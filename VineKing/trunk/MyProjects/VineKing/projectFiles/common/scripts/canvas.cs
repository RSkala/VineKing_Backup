//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------


function iDeviceResolutionFromSetting( %deviceType, %deviceScreenOrientation, %deviceResolution )
{
      //A helper function to get a string based resolution from the settings given.
      
   %x = 0;
   %y = 0;
   
   %fail = 0;
   if( (%deviceType == $iDevice::constant::iPhone) && ( %deviceResolution == $iDevice::constant::ResolutionFull ) ) 
   {
      //No thanks, 
      %fail = 1;
   }
   
   //Handle common first, as we dont care what device its on (all currently support it)
   if( (%deviceResolution == $iDevice::constant::ResolutionSmall) || (%fail == 1) )
   {
      //Landscape and Portrait , watch the switch on width / height.
      if(%deviceScreenOrientation == $iDevice::constant::Landscape)
      {
         %x =  $iDevice::constant::ResolutionSmallWidth;
         %y =  $iDevice::constant::ResolutionSmallHeight;
      }
      else
      {
         %x =  $iDevice::constant::ResolutionSmallHeight;
         %y =  $iDevice::constant::ResolutionSmallWidth;
      }
   }
   else
   if(%deviceResolution == $iDevice::constant::ResolutionFull)
   {
         //If a larger resolution was selected, 
         //we cant allow that on iPhone/iPod at all. 
         //Only iPad can handle higher resolution
         if(%deviceType == $iDevice::constant::iPad)
         {
            if(%deviceScreenOrientation == $iDevice::constant::Landscape)
            {
               %x = $iDevice::constant::ResolutionFullWidth;
               %y = $iDevice::constant::ResolutionFullHeight;
            }
            else
            {
               %x = $iDevice::constant::ResolutionFullHeight;
               %y = $iDevice::constant::ResolutionFullWidth;
            }
         }
   }

   return %x @ " " @ %y;
}

//---------------------------------------------------------------------------------------------
// initializeCanvas
// Constructs and initializes the default canvas window.
//---------------------------------------------------------------------------------------------
$canvasCreated = false;
function initializeCanvas(%windowName)
{
   // Don't duplicate the canvas.
   if($canvasCreated)
   {
      error("Cannot instantiate more than one canvas!");
      return;
   }
   
   videoSetGammaCorrection($pref::OpenGL::gammaCorrection);
   
   if (!createCanvas(%windowName))
   {
      error("Canvas creation failed. Shutting down.");
      quit();
   }

   //Luma: Apply the settings to the game from the commonConfig iDeviceSettings section.
   //For the resolution options, its two sided. 1) Landscape/Portrait and 2) the chosen size (full or small).
      //Todo : dont hardcore this, add it as an invisible setting. default should be 16
      
   $pref::iDevice::ScreenDepth = 32;
   %res = iDeviceResolutionFromSetting($pref::iDevice::DeviceType, $pref::iDevice::ScreenOrientation, $pref::iDevice::ScreenResolution);
      
   setScreenMode( GetWord( %res , 0 ), GetWord( %res, 1 ), $pref::iDevice::ScreenDepth, false );

/*
	If the above is giving issues, for now i have kept the working original code here.
    %goodres = $Project::Game::Resolution;
    setScreenMode( GetWord( %goodres, 0), GetWord( %goodres,1), GetWord( %goodres,2), false );
*/
   $canvasCreated = true;
}

//---------------------------------------------------------------------------------------------
// resetCanvas
// Forces the canvas to redraw itself.
//---------------------------------------------------------------------------------------------
function resetCanvas()
{
   if (isObject(Canvas))
      Canvas.repaint();
}

//---------------------------------------------------------------------------------------------
// GuiDefaultProfile is a special profile that all other profiles inherit defaults from. It
// must exist.
//---------------------------------------------------------------------------------------------
if(!isObject(GuiDefaultProfile)) new GuiControlProfile (GuiDefaultProfile)
{
   tab = false;
   canKeyFocus = false;
   hasBitmapArray = false;
   mouseOverSelected = false;

   // fill color
   opaque = true;
   fillColor = "0 0 0";
   fillColorHL = "244 244 244";
   fillColorNA = "244 244 244";

   // border color
	// RKS: I disabled the border
	border = 0;
   //border = 1;
   //borderColor   = "40 40 40 100";
   //borderColorHL = "128 128 128";
   //borderColorNA = "64 64 64";

   // font
   fontType = "Arial";
   fontSize = 14;

   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
   fontColorNA = "0 0 0";
   fontColorSEL= "80 80 80";

   // bitmap information
   bitmap = "";
   bitmapBase = "";
   textOffset = "0 0";

   // used by guiTextControl
   modal = true;
   justify = "left";
   autoSizeWidth = false;
   autoSizeHeight = false;
   returnTab = false;
   numbersOnly = false;
   cursorColor = "0 0 0 255";

   // sounds
   soundButtonDown = "";
   soundButtonOver = "";
};

new GuiCursor(DefaultCursor)
{
   hotSpot = "4 4";
   renderOffset = "0 0";
   bitmapName = "~/data/images/cursor";
};


// RKS:  I added this Gui Profile, so I can create Gui objects that are NON-Modal (do not respond to mouse events)
if(!isObject(GuiDefaultProfileNonModal)) new GuiControlProfile (GuiDefaultProfileNonModal)
{
	tab = false;
	canKeyFocus = false;
	hasBitmapArray = false;
	mouseOverSelected = false;
	
	// fill color
	opaque = true;
	fillColor = "0 0 0";
	fillColorHL = "244 244 244";
	fillColorNA = "244 244 244";
	
	// border color
	border = 0;
	
	// font
	fontType = "Times New Roman Bold Italic";
	//fontSize = 14;
	fontSize = 32;
	//fontSize = 24;
	
	//fontColor = "0 0 0";
	fontColor = "246 171 0";
	fontColorHL = "32 100 100";
	fontColorNA = "0 0 0";
	fontColorSEL= "80 80 80";
	
	// bitmap information
	bitmap = "";
	bitmapBase = "";
	textOffset = "0 0";
	
	// used by guiTextControl
	modal = false;	// RKS: So a Gui object that uses this profile does not respond to touch/mouse events (
	justify = "left";
	autoSizeWidth = false;
	autoSizeHeight = false;
	returnTab = false;
	numbersOnly = false;
	cursorColor = "0 0 0 255";
	
	// sounds
	soundButtonDown = "";
	soundButtonOver = "";
};
