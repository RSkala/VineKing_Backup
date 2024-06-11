
//This file houses the XCode xcconfig file output 

   //We only actually set 2 things in the file, 
   //   
   //   IT2D_EDITOR_FLAGS (which GCC_PREPROCESSOR_DEFINITIONS uses)
   //   IT2D_LDFLAGS = -framework MediaPlayer etc.
   // 

//The file is kept in /MyProjects/<yourProject>/buildFiles/XCode_iPhone/T2DBuild.xcconfig

function makeLDFlags()
{
   //Returns a string for use in the xcconfig, based on current inclusion settings.
   //This removes things like, the mediaplayer when it is not in use.
   
   %result = "";
   
   // Deprecated -MP if($pref::iDevice::UseGameKit)
   // Deprecated -MP    { %result = %result @ " -framework GameKit"; } 
   // Deprecated -MP if($pref::iDevice::UseLocation)
   // Deprecated -MP    { %result = %result @ " -framework CoreLocation"; }
   // Deprecated -MP if($pref::iDevice::UseStoreKit)
   // Deprecated -MP    { %result = %result @ " -framework StoreKit"; }
   if($pref::iDevice::UseMusic)
      { %result = %result @ " -framework MediaPlayer"; }
   if($pref::iDevice::UseMoviePlayer && $pref::iDevice::UseMusic == 0)
      { %result = %result @ " -framework MediaPlayer"; }
      
   return %result;
}

function makePreprocessorFlags()
{
   //Returns a string for use in the xcconfig file, based on settings in editor.
   //The settings are for removing code/features from the code base based on selection
   //in the editor. 
   
   %result = "";
   
      //TORQUE_ALLOW_GAMEKIT
      //TORQUE_ALLOW_STOREKIT
      //TORQUE_ALLOW_LOCATION
      //TORQUE_ALLOW_MUSICPLAYER
      //TORQUE_ALLOW_AUTOROTATE
      //TORQUE_ALLOW_MOVIEPLAYER
      //TORQUE_ALLOW_ORIENTATIONS
      
   // Deprecated -MP if($pref::iDevice::UseGameKit)
   // Deprecated -MP    { %result = %result @ " TORQUE_ALLOW_GAMEKIT"; } 
   // Deprecated -MP if($pref::iDevice::UseLocation)
   // Deprecated -MP    { %result = %result @ " TORQUE_ALLOW_LOCATION"; }
   // Deprecated -MP if($pref::iDevice::UseStoreKit)
   // Deprecated -MP    { %result = %result @ " TORQUE_ALLOW_STOREKIT"; }
   if($pref::iDevice::UseMusic)
      { %result = %result @ " TORQUE_ALLOW_MUSICPLAYER"; }
   if($pref::iDevice::UseMoviePlayer)
      { %result = %result @ " TORQUE_ALLOW_MOVIEPLAYER"; }
   if($pref::iDevice::UseAutoRotate)
      { %result = %result @ " TORQUE_ALLOW_AUTOROTATE TORQUE_ALLOW_ORIENTATIONS" ; }
   if($pref::iDevice::UseOrientation && $pref::iDevice::UseAutoRotate == 0)
      { %result = %result @ " TORQUE_ALLOW_ORIENTATIONS"; }
      
   return %result;
}