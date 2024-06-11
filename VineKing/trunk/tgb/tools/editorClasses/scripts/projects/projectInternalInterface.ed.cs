//-----------------------------------------------------------------------------
// Project Base Internal API's
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

///
/// Internal Project Events
/// 

Projects::GetEventManager().registerEvent( "_ProjectCreate" );
Projects::GetEventManager().registerEvent( "_ProjectOpen" );
Projects::GetEventManager().registerEvent( "_ProjectClose" );
Projects::GetEventManager().registerEvent( "_ProjectAddFile" );
Projects::GetEventManager().registerEvent( "_ProjectRemoveFile" );

/// 
/// Project Context Methods
///

function ProjectBase::isActive( %this ) 
{
   if( Projects::GetEventManager().activeProject == %this.getId() )
      return true;
   else
      return false;
}

function ProjectBase::getActiveProject( %this ) 
{
   return Projects::GetEventManager().activeProject;   
}

function ProjectBase::setActive( %this ) 
{
   %activeProject = %this.getActiveProject();
   
   if( isObject( %activeProject ) )
   {
      // If another is active, properly post a close event for now THEN
      // and only then should we change the .activeProject field on the evtmgr
   }
   
   Projects::GetEventManager().activeProject = %this;
}

function ProjectBase::onAdd( %this )
{
   // Subscribe to base events
   Projects::GetEventManager().subscribe( %this, "_ProjectCreate", "_onProjectCreate" );
   Projects::GetEventManager().subscribe( %this, "_ProjectOpen", "_onProjectOpen" );
   Projects::GetEventManager().subscribe( %this, "_ProjectClose", "_onProjectClose" );
   Projects::GetEventManager().subscribe( %this, "_ProjectAddFile", "_onProjectAddFile" );
   Projects::GetEventManager().subscribe( %this, "_ProjectRemoveFile", "_onProjectRemoveFile" );
   
}

function ProjectBase::onRemove( %this )
{
   // Remove subscriptions to base events
   Projects::GetEventManager().remove( %this, "_ProjectCreate" );
   Projects::GetEventManager().remove( %this, "_ProjectOpen" );
   Projects::GetEventManager().remove( %this, "_ProjectClose" );
   Projects::GetEventManager().remove( %this, "_ProjectAddFile" );
   Projects::GetEventManager().remove( %this, "_ProjectRemoveFile" );
}

function ProjectBase::promptUpdate(%this)
{
   // rdbnote: prompt user if their project is out of date
   %compareVal = %this.CompareVersions(%this.projectVersion, getT2DVersion());
   if (%compareVal == 1)
   {
      %result = messageBox("Scene Builder", "This project is from an older version and may not function correctly unless you update it. Would you like to update now?", "OkCancel", "Warning");
      if (%result == $MROk)
      {
         echo("Updating Project..." SPC %this.gamePath);
         
         %templateCommonPath = expandFilename("^tool/gameData/T2DProject/common");
         %templateGamePath = expandFilename("^tool/gameData/T2DProject/templates/base");
         %commonPath = expandFilename("^project/common");
         %gamePath = expandFilename("^project/game");
         
         // rdbhack: it's done this way so we don't overwrite commonConfig.xml
         pathCopy(%templateCommonPath @ "/main.cs", %commonPath @ "/main.cs", false);
         pathCopy(%templateCommonPath @ "/data", %commonPath @ "/data", false);
         pathCopy(%templateCommonPath @ "/scripts", %commonPath @ "/scripts", false);
         pathCopy(%templateCommonPath @ "/gui", %commonPath @ "/gui", false);
         
         pathCopy(%templateGamePath @ "/scripts/guiProfiles.cs", %gamePath @ "/scripts/guiProfiles.cs", false);
         
         %this.SaveProject( %this.projectFile );
         
         messageBox("Scene Builder", "The project was successfully updated.", "Ok", "Information");
         //messageBox("Scene Builder", "The project was successfully updated and will now reload!", "Ok", "Information");
         //reloadProject();
      }
      
      // return that we do need an update
      return false;
   }
   
   // return that we didn't need an update
   return true;
}

///
/// Internal ProjectOpen Event Handler
/// - %data is the project file path to be opened 

function ProjectBase::_onProjectOpen( %this, %data )
{  
   $currentProjectPath = filePath(%data);
   
   //If these dont exist, we can set them here
   if(!$pref::lastProject)
   {
      $pref::startupProject = %data;
      $pref::lastProject = %data;
   }
   
   // Sanity check calling of this 
   if( !%this.isMethod( "onProjectOpen" ) )
   {
      error("Incomplete Project Interface - onProjectOpen method is non-existent!");
      return false;
   }
     
   if( !%this.LoadProject( %data ) )
   {
      messageBox("Error", "Unable to Open Project.","Ok","Error");
      return false;
   }
  
   // Setup the project expandos
   setScriptPathExpando("project", $currentProjectPath );
   setScriptPathExpando("game", $currentProjectPath @ "/game" );
   
   // check for update
   if ( !%this.promptUpdate() )
   {
       // Reload the project data.
       %this.LoadProject( %data );
   }

   %this.gamePath = filePath( %data );
   %this.projectFile = %data;
      
   %toggle = $Scripts::ignoreDSOs;
   $Scripts::ignoreDSOs = true;
      
   %this.gameResPath = %this.gamePath @ "/*";
         
   // Set current dir to game
   setCurrentDirectory( %this.gamePath );
      
   // Index in ResManager
   addResPath( %this.gameResPath, true );

   $Gui::fontCacheDirectory = expandFileName("^project/common/data/fonts");

   //// Set Mod Paths. 

   //%modPaths = getModPaths();
   //if( strstr( %modPaths, %this.gamePath ) == -1 ) 
      //setModPaths( %modPaths @ ";" @ %this.gamePath );
   
   %this.onProjectOpen( %data );
   %this.setActive();
   
   Projects::GetEventManager().postEvent( "ProjectOpened", %this );
   
   $Scripts::ignoreDSOs = %toggle;
   $pref::lastProject = %data;
   
   // Eat this message because we processed the open event
   return false;
}

///
/// Internal ProjectClose Event Handler
/// 

function ProjectBase::_onProjectClose( %this, %data )
{
   
   Projects::GetEventManager().postEvent( "ProjectClosed", %this );
   
   // Sanity check calling of this 
   if( !%this.isMethod( "onProjectClose" ) )
      error("Incomplete Project Interface - onProjectClose method is non-existent!");
   else
      %this.onProjectClose( %data );
   
   // Reset to tools directory
   setCurrentDirectory( getMainDotCsDir() );
   
   // Remove expandos
   removeScriptPathExpando( "game" );
   removeScriptPathExpando( "project" );

   // Remove res indexing   
   removeResPath( %this.gameResPath );
}

///
/// Internal ProjectCreate Event Handler (Optionally Inherited by public interface)
/// 

function ProjectBase::_onProjectCreate( %this, %data )
{
   // Force a write out of the project file
   if( !%this.SaveProject( %data ) )
      return false;
      
   // Sanity check calling of this 
   if( %this.isMethod( "onProjectCreate" ) )
      %this.onProjectCreate( %data );
}


///
/// Internal ProjectAddFile Event Handler
/// 

function ProjectBase::_onProjectAddFile( %this, %data )
{
   // Sanity check calling of this 
   if( !%this.isMethod( "onProjectAddFile" ) )
      error("Incomplete Project Interface - onProjectAddFile method is non-existent!");
   else
      %this.onProjectAddFile( %data );

}

///
/// Internal ProjectRemoveFile Event Handler
/// 

function ProjectBase::_onProjectRemoveFile( %this, %data )
{
   // Sanity check calling of this 
   if( !%this.isMethod( "onProjectRemoveFile" ) )
      error("Incomplete Project Interface - onProjectRemoveFile method is non-existent!");
   else
      %this.onProjectRemoveFile( %data );

}
