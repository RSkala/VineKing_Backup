//-----------------------------------------------------------------------------
// Gui Form Content Controller
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Declare the Project Target
Projects::DeclareProjectTarget( T2DProject, LBProjectObj );

//If there is no startup project, we are likely to be reloading a project.
//If there is neither startup or last project something else is horribly wrong,
//New project sets the startup project, reload sets the last project and reload = true;

function createProjectResPaths(%extra)
{
      echo("     % Project Game Path");
      echo("         % Projects (last, startup) : " @ $pref::lastProject @ "  ,  " @ $pref::startupProject);
      
   $gamepath = "";
   if(%extra !$= "")
   {
      $pref::startupProject = %extra;
   }

   if($pref::reloadLastProject)
   {
      //If we are reloading, we only care about startup project.
      echo("    % Reloading last project. " @ $pref::lastProject);
      $gamepath = filePath($pref::lastProject);
   } 
   else
   {
      //If we arent reloading a project, theres 2 choices. 
         //1) startup project path is set via cmd line or other method .
         //2) Load last project preference is set and is autoloadinga project.
         
      //In the case of 1, this overrides the load last preference anyway.
      //In the case of 2, it doesnt use the startup path as its null.
      
      if($pref::startupProject $= "")
      {
         //Likely option 2 above.
         echo("     % (2)Reloading last project. " @ $pref::lastProject);
         $gamepath = filePath($pref::lastProject);
      }
      else
      {
         //Likely that the startup is lingering, check the pref
         if($pref::loadLastProject)
         {
            echo("     % (1)Reloading last project. " @ $pref::lastProject);
            $gamepath = filePath($pref::lastProject);
         }
         else
         {
            echo("     % Reloading startup project. " @ $pref::startupProject);
            $gamepath = filePath($pref::startupProject);
         }
      }
   }

   if($gamepath !$= "")
   {
      echo("     % Path added : " @ $gamepath);
      addResPath( expandFilename($gamepath) );
   }
   else
   {
      echo("     % Game Path Missing. Datablocks and assets will be invalid.");
   }
}

//Call this 
createProjectResPaths("");

// Deal with non-Windows platforms
if($platform $= "windows")
   $LB::PlayerExecutable = expandFilename("^tool/templates/commonFiles/iTorque2DGame.exe");

if($platform $= "macos")
   $LB::PlayerExecutable = expandFilename("^tool/templates/commonFiles/ITorque2dGame.app/Contents/MacOS/ITorque2dGame");



function T2DProject::onAdd( %this )
{
   Parent::onAdd( %this );
   
   // Set flag to true to always persist objects when we're deleted
   %this.persistOnRemove = false;
   
}

function T2DProject::onRemove( %this ) 
{   
   // Persist at option
   if( %this.persistOnRemove == true )
      %this.persistToDisk( true, true, true );
      
   Parent::onRemove( %this );

}

///
/// ProjectOpen Event Handler
/// - %data is the project object to be opened 

function T2DProject::onProjectOpen( %this, %data )
{   
   echo("% Opening Project " @ %data @ " ...");
   
   // Load Project Data
   %this.LoadProjectData();
   
   showLevelEditor();
   
   if ($pref::loadLastLevel && isFile(%this.lastLevel))
      %this.openLevel(%this.lastLevel);
   else
   {
      //If we have no last scene, we might as well load the default scene (it seems unused anyway)
      if($Game::DefaultScene !$= "" && isFile((expandFilename($Game::DefaultScene))))
      {
        %this.openLevel(expandFilename($Game::DefaultScene));
      }
      else
      {
         //No default, so try open the existing empty level, 
         if(isFile(expandFilename("^game/data/levels/emptyLevel.t2d")))
         {
             %this.openLevel(expandFilename("^game/data/levels/emptyLevel.t2d"));
         }
         else
         {
            //Still no luck? then just make a new level.
            %this.newLevel();
         }
      }
   }
      
   //Make sure the mode of the screen is set because its annoying - Sven
   LBQESceneGraph::updateCameraValues();
   
   // Update object library
   GuiFormManager::SendContentMessage($LBCreateSiderBar, %this, "refreshAll 1");
   
   return true;
}

///
/// ProjectClose Event Handler
/// 

function T2DProject::onProjectClose( %this, %data )
{
   error("onProjectClose Handler not implemented for class -" SPC %this.class );   
}

///
/// ProjectAddFile Event Handler
/// 

function T2DProject::onProjectAddFile( %this, %data )
{
   error("onProjectAddFile Handler not implemented for class -" SPC %this.class );
}

///
/// ProjectRemoveFile Event Handler
/// 

function T2DProject::onProjectRemoveFile( %this, %data )
{
   error("onProjectRemoveFile Handler not implemented for class -" SPC %this.class );
}

/// Legacy Code Below, used only to prevent code changes to a currently stable base for now - JDD

//
// Open a project from the levelBuilder - returns True/False
//

function T2DProject::open( %this, %projectName )
{
   error("Deprecated LBProjectObj.open");
   if(%projectName $= "")
      return false;
}



