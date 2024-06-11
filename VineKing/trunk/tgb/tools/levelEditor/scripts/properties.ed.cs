//This doesnt seem like a logical place for this, i know  -Sven

$iDevice::constant::iPhone = 0;
$iDevice::constant::iPad = 1;
$iDevice::constant::Landscape = 0;
$iDevice::constant::Portrait = 1;
$iDevice::constant::ResolutionFull = 0;
$iDevice::constant::ResolutionSmall = 1;

$iDevice::constant::ResolutionSmallWidth = 480;
$iDevice::constant::ResolutionSmallHeight = 320;

$iDevice::constant::ResolutionFullWidth = 1024;
$iDevice::constant::ResolutionFullHeight = 768;
//
// Save config data
//
function _saveGameConfigurationData( %projectFile )
{
   %xml = new ScriptObject() { class = "XML"; };
   if( %xml.beginWrite( %projectFile ) )
   {
      %xml.writeClassBegin( "TorqueGameConfiguration" );
      
         %xml.writeField( "CommonVer", $Game::CommonVersion );
         %xml.writeField( "Company", $Game::CompanyName );
         %xml.writeField( "GameName", $Game::ProductName );
         %xml.writeField( "Resolution", $Game::Resolution );
         %xml.writeField( "FullScreen", $Game::FullScreen );
         %xml.writeField( "UsesAudio", $Game::UsesAudio );
         %xml.writeField( "DefaultScene", $Game::DefaultScene );
         
      %xml.writeClassEnd();
         
      %xml.writeClassBegin( "iDeviceSettings" );
      
         //Write device preference settings    
         %xml.writeField( "iDeviceType",              $pref::iDevice::DeviceType        );
         %xml.writeField( "iDeviceScreenOrientation", $pref::iDevice::ScreenOrientation );
         %xml.writeField( "iDeviceScreenResolution",  $pref::iDevice::ScreenResolution  );

         //Write build feature settings
         // Deprecated -MP %xml.writeField( "iFeatureUseStoreKit",      $pref::iDevice::UseStoreKit   );
         %xml.writeField( "iFeatureUseNetwork",       $pref::iDevice::UseNetwork    );
         // Deprecated -MP %xml.writeField( "iFeatureUseGameKit",       $pref::iDevice::UseGameKit    );
         %xml.writeField( "iFeatureUseMusic",         $pref::iDevice::UseMusic      );
         // Deprecated -MP %xml.writeField( "iFeatureUseLocation",      $pref::iDevice::UseLocation   );
         %xml.writeField( "iFeatureUseMultitouch",    $pref::iDevice::UseMultitouch );
         %xml.writeField( "iFeatureUseMoviePlayer",   $pref::iDevice::UseMoviePlayer );
         %xml.writeField( "iFeatureUseAutoRotate",    $pref::iDevice::UseOrientation );
         %xml.writeField( "iFeatureUseOrientation",   $pref::iDevice::UseAutoRotate );          
         %xml.writeField( "iFeatureStatusBarType",    $pref::iDevice::StatusBarType ); 
         
      %xml.writeClassEnd();
         
      /* %xml.writeField( "StatusBarHidden", $pref::iPhone::StatusBarHidden );
         %xml.writeField( "UsesNetwork", $Game::UsesNetwork );
         %xml.writeField( "ScreenOrientation", $pref::iPhone::ScreenOrientation );
         %xml.writeField( "ScreenUpsideDown", $pref::iPhone::ScreenUpsideDown );
         %xml.writeField( "ScreenAutoRotate", $pref::iPhone::ScreenAutoRotate );
         %xml.writeField( "MultiTouch", $pref::iPhone::EnableMultipleTouch);

         %xml.writeField( "Platform", ProjectBuilderPlatformList.getSelected() );
         %xml.writeField( "BuildOption", ProjectBuilderBuildOptionsList.getSelected() );
         %xml.writeField( "XcodeOption", ProjectBuilderBuildTypeList.getSelected() );
      */
      
      %xml.endWrite();
   }   
   else
   {
      error( "saveGameConfigurationData - Failed to write to file: " @ %projectFile );
      return false;
   }
   
   // Delete the object
   %xml.delete();
   
   //Once we have written the xml configuration, we write the xcode xcconfig file for the build target
   //so that XCode knows what it wants to do when it hits the target.
   
   //the file has 2 lines, OTHER_LDFLAGS and IT2D_EDITOR_FLAGS
   %configpath = expandFileName("^project/../../buildFiles/XCode_iPhone/T2DBuild.xcconfig");

   %fo = new FileObject();
   %fo.openForWrite(%configpath);

      //Write some lines in
      %fo.writeLine("IT2D_LDFLAGS = " @ makeLDFlags());
      %fo.writeLine("IT2D_EDITOR_FLAGS =" @ makePreprocessorFlags());
      
   %fo.close();
   %fo.delete();
   
   return true;
}

//
// Load config data
//
function _loadGameConfigurationData( %projectFile )
{
   %xml = new ScriptObject() { class = "XML"; };
   
   //For catching any errors, we can default to some
   // settings. 
   %hasTorqueConfig = 0;
   %hasiDeviceConfig = 0;
   
   if( %xml.beginRead( %projectFile ) )
   {
      
      if( %xml.readClassBegin( "TorqueGameConfiguration" ) )
      {
         
         $Game::CommonVersion     = %xml.readField( "CommonVer" );
         $Game::CompanyName       = %xml.readField( "Company" );
         $Game::ProductName       = %xml.readField( "GameName" );

         $Game::Resolution        = %xml.readField( "Resolution" );
         $Game::FullScreen        = %xml.readField( "FullScreen" );

         $Game::UsesAudio         = %xml.readField( "UsesAudio" );
         $Game::DefaultScene      = %xml.readField( "DefaultScene" );
         
         %xml.readClassEnd();
         %hasTorqueConfig = 1;

      }  //TorqueGameConfiguration
      else
      {
         %hasTorqueConfig = 0;
      }
      
      if( %xml.readClassBegin( "iDeviceSettings" ) )
      {
         //Read device settings
         $pref::iDevice::DeviceType        = %xml.readField( "iDeviceType" );
         $pref::iDevice::ScreenOrientation = %xml.readField( "iDeviceScreenOrientation" );
         $pref::iDevice::ScreenResolution  = %xml.readField( "iDeviceScreenResolution" );
                  
         //Read build feature settings
         // Deprecated -MP $pref::iDevice::UseStoreKit       = %xml.readField( "iFeatureUseStoreKit" );
         $pref::iDevice::UseNetwork        = %xml.readField( "iFeatureUseNetwork" );
         // Deprecated -MP $pref::iDevice::UseGameKit        = %xml.readField( "iFeatureUseGameKit" );
         $pref::iDevice::UseMusic          = %xml.readField( "iFeatureUseMusic" );
         // Deprecated -MP $pref::iDevice::UseLocation       = %xml.readField( "iFeatureUseLocation" );
         $pref::iDevice::UseMultitouch     = %xml.readField( "iFeatureUseMultitouch" );
         $pref::iDevice::UseMoviePlayer    = %xml.readField( "iFeatureUseMoviePlayer" );
         $pref::iDevice::UseAutoRotate     = %xml.readField( "iFeatureUseAutoRotate" );
         $pref::iDevice::UseOrientation    = %xml.readField( "iFeatureUseOrientation" );            
         $pref::iDevice::StatusBarType     = %xml.readField( "iFeatureStatusBarType" );  
         
         %xml.readClassEnd();
         %hasiDeviceConfig = 1;
         
      } //iDevice Settings
      else
      {
         %hasiDeviceConfig = 0;
      }
      
      //Close the xml if we got this far.
      %xml.endRead();

      //Notify new users about the new features, if they are to convert a project they would most likely ignore
      //the documentation, no matter how awesome.
      if(%hasiDeviceConfig == 0)
      {
         warn("1.4 Warning >> This " @ %projectFile @ " might be from an earlier version of iTorque2D . Be sure to convert your settings in the project tab and click apply.");
      }
      
      //Perhaps this can happen, but hey.
      if(%hasTorqueConfig == 0)
      {
         warn("1.4 Error   >> This " @ %projectFile @ " is probably not valid at all.");
      }
         
   } //begin read
   else
   {

      //we couldnt read the xml for some reason, 
      _defaultGameConfiguration();
      
   }
   
   // Cleanup after we are done.
   %xml.delete();
   
      /* $pref::iPhone::ScreenOrientation  = %xml.readField( "ScreenOrientation" );
         $pref::iPhone::ScreenUpsideDown   = %xml.readField( "ScreenUpsideDown" );
         $pref::iPhone::ScreenAutoRotate   = %xml.readField( "ScreenAutoRotate" ) $= ""? 0 : %xml.readField( "ScreenAutoRotate");
         $pref::iPhone::EnableMultipleTouch  = %xml.readField( "MultiTouch") $= ""? 0 : %xml.readField( "MultiTouch");
      */   
}

//
// Load config data
//
function _defaultGameConfigurationData()
{
   //Set default torque configuration 
   
   $Game::CompanyName         = "Independent";
   $Game::ProductName         = "defaultName";
   $Game::Resolution          = "480 320 32";
   $Game::FullScreen          = 0;
   $Game::UsesAudio           = 1;
   $Game::DefaultScene        = "~/data/levels/untitled.t2d";
   
   //set default iDevice settings.
   
   $pref::iDevice::DeviceType          = $iDevice::constant::iPhone;
   $pref::iDevice::ScreenOrientation   = $iDevice::constant::Landscape;
   $pref::iDevice::ScreenResolution    = $iDevice::constant::ResolutionSmall;
   
   //Turn all features of by default
   
   // Deprecated -MP $pref::iDevice::UseGameKit          = 0;
   // Deprecated -MP $pref::iDevice::UseLocation         = 0;
   $pref::iDevice::UseMultitouch       = 0;
   $pref::iDevice::UseMusic            = 0;
   $pref::iDevice::UseNetwork          = 0;
   // Deprecated -MP $pref::iDevice::UseStoreKit         = 0;
   $pref::iDevice::UseAutoRotate       = 0;
   $pref::iDevice::StatusBarType       = 0;
   
}
