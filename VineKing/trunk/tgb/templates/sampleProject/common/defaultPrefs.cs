//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

///
/// Network
///
$pref::Net::LagThreshold = 400;
$pref::Net::Port = 28000;

///
/// Input
///
$pref::Input::LinkMouseSensitivity = 1;
$pref::Input::MouseEnabled = 0;
$pref::Input::JoystickEnabled = 0;
$pref::Input::KeyboardTurnSpeed = 0.1;

///
/// Audio
///
$pref::Audio::driver = "OpenAL";
$pref::Audio::forceMaxDistanceUpdate = 0;
$pref::Audio::environmentEnabled = 0;
$pref::Audio::masterVolume   = 0.8;
$pref::Audio::channelVolume1 = 0.8;
$pref::Audio::channelVolume2 = 0.8;
$pref::Audio::channelVolume3 = 0.8;
$pref::Audio::channelVolume4 = 0.8;
$pref::Audio::channelVolume5 = 0.8;
$pref::Audio::channelVolume6 = 0.8;
$pref::Audio::channelVolume7 = 0.8;
$pref::Audio::channelVolume8 = 0.8;

// Audio profiles for default
$pref::Audio::sfxVolume = 0.8;
$pref::Audio::musicVolume = 0.8;

///
/// TGB
///
$pref::T2D::dualCollisionCallbacks = 1;
$pref::T2D::imageMapDumpTextures = 0;
$pref::T2D::imageMapEchoErrors = 1;
$pref::T2D::imageMapFixedMaxTextureError = 1;
$pref::T2D::imageMapFixedMaxTextureSize = 0;
$pref::T2D::imageMapShowPacking = 0;
$pref::T2D::imageMapPreloadDefault = true;
$pref::T2D::imageMapAllowUnloadDefault = false;
$pref::T2D::particleEngineQuantityScale = 1.0;
$pref::T2D::renderContactChange = 0.5;
$pref::T2D::renderContactMax = 16;
$pref::T2D::warnFileDeprecated = 1;
$pref::T2D::warnSceneOccupancy = 1;

///
/// Server
///
$pref::Server::Name = "TGB Server";
$pref::Player::Name = "TGB Player";
$pref::Server::port = 28000;
$pref::Server::MaxPlayers = 32;
$pref::Server::RegionMask = 2;
$pref::Net::RegionMask = 2;
$pref::Master0 = "2:master.garagegames.com:28002";

///
/// Video
///

$pref::ts::detailAdjust = 0.45;

$pref::Video::appliedPref = 0;
$pref::Video::disableVerticalSync = 1;
$pref::Video::monitorNum = 0;
$pref::Video::screenShotFormat = "PNG";

$pref::OpenGL::gammaCorrection = 0.5;
$pref::OpenGL::force16BitTexture = "0";
$pref::OpenGL::forcePalettedTexture = "0";
$pref::OpenGL::maxHardwareLights = 3;
$pref::VisibleDistanceMod = 1.0;

//platform
if( $platform $= "iphone" ) 
{
   $Project::Game::CompanyName         = "Independent";
   $Project::Game::ProductName         = "defaultName";
   $Project::Game::Resolution          = "480 320 32";
   $Project::Game::FullScreen          = 0;
   $Project::Game::UsesAudio           = 1;
   $Project::Game::DefaultScene        = "~/data/levels/untitled.t2d";
   
   //set default iDevice settings.
   
   $pref::iDevice::DeviceType          = $iDevice::constant::iPhone;
   $pref::iDevice::ScreenOrientation   = $iDevice::constant::Landscape;
   $pref::iDevice::ScreenResolution    = $iDevice::constant::ResolutionSmall;
   $pref::iDevice::ScreenDepth		   = 32;
   
   //Turn all features of by default
   
   $pref::iDevice::UseGameKit          = 0;
   $pref::iDevice::UseLocation         = 0;
   $pref::iDevice::UseMultitouch       = 0;
   $pref::iDevice::UseMusic            = 0;
   $pref::iDevice::UseNetwork          = 0;
   $pref::iDevice::UseStoreKit         = 0;
   $pref::iDevice::UseMoviePlayer      = 0;
   $pref::iDevice::UseAutoRotate       = 0;
   $pref::iDevice::UseOrientation      = 0;   
   $pref::iDevice::StatusBarType       = 0;
}

// Default to OpenGL
$pref::Video::displayDevice = "OpenGL";
$pref::Video::preferOpenGL = 1;

// But allow others
$pref::Video::allowOpenGL = 1;
$pref::Video::allowD3D = 0;

// And not full screen
$pref::Video::fullScreen = 0;

echo( "\nUsing " @  $pref::Video::displayDevice @ " rendering. Fullscreen: " @ $pref::Video::fullScreen @ "\n");
