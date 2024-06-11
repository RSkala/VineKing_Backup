//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

$templatesDirectory = expandFilename("~/../templates");
$resourcesDirectory = expandFilename("^tool/resources/");

// To catch the text from the choice in the box, returns a number
$newProjectTemplate["Empty Project (main.cs entry point)"] = 0;
//$newProjectTemplate["Sample Project (gameplay example)"] = 1;

// Eventually to be replaced by iPhone template -MP
//The number above is used now, to get the folder for the template
$newProjectTemplateLocation[0] = $templatesDirectory @ "/emptyProject";

// Deprecated from 1.4.1. Eventually to be replaced with iPad template -MP
//$newProjectTemplateLocation[1] = $templatesDirectory @ "/sampleProject";

$newProjectTemplateCommonFileFolder = $templatesDirectory @ "/commonFiles";

$batch::CreateXCode = "createNewXcodeProject.sh";

function initializeProjectWizard()
{
   exec("./gui/newProjectDlg.ed.gui");
   exec("./gui/newProjectDlg.ed.cs");
   exec("./gui/openProjectDlg.ed.gui");
   exec("./gui/projectOptionsDlg.ed.gui");
   exec("./gui/projectOptionsDlg.ed.cs");
   exec("./gui/superTooltipDlg.ed.cs");
   exec("./gui/superTooltipDlg.ed.gui");
}

function destroyProjectWizard()
{
}

function newProject()
{
   Canvas.pushDialog(NewProjectDlg);
   
   templateListBox.clear();
   templateListBox.add("Empty Project (main.cs entry point)", 0);

   // Deprecated from 1.4.1. Eventually to be replaced with iPad template -MP   
   //templateListBox.add("Sample Project (gameplay example)", 1);
   
   /*%templates = getDirectoryList( $templatesDirectory );
   %templateCount = getWordCount( %templates );
   %num = 3;
   for( %i = 0; %i < %templateCount; %i++ )
   {
      %template = getWord( %templates, %i );
      %name = %template;
      if( (%index = strrchr( %name, "/" )) !$= "")
         %name = getSubStr( %name, %index );
      
      if( %name !$= "base" )
      {
         templateListBox.add( %name, %num );
         $newProjectTemplate[%name] = $templatesDirectory @ %name;
         %num++;
      }
   }*/
   
   //templateListBox.sort();
   templateListBox.setSelected( 0 );
}


// Return true/false
function findProject( %name ) 
{
   %projects = getDirectoryList( "/" );
   %projectCount = getWordCount( %projects );
   
   for( %i = 0; %i < %projectCount; %i++ )
   {
      if( getWord( %projects, %i ) $= %name )
         return true;
   }
   
   return false;
}

function createNewProjectiPhone(%location, %name)
{

   %templateDir = expandFilename("^/../templateXcodeProject/iPhoneDefault");
   
   // The name must not be empty
   if( %location $= "")
   {
      MessageBoxOK("Error Creating Project", "Please choose a folder for the project" );
      return false;
   }   
   
   %gameLocation = %location @ "/" @ %name @ "/";
   
   if( isFile( %gameLocation @ "project.t2dProj" ) )
   {
      %result = messageBox( "Invalid Name", "A game already exists at this location. Choose a new name or delete the old game before continuing.", "Ok" );
      return false;
   }
      
   // Template Hierarchy
   //  force overwrite
   if(!pathCopy(%templateDir, %gameLocation , false))
   {
      messageBox( "Error Creating Project", "An error occurred while trying to create your project.\n\nCould not create the output path, Access Denied.", "Ok", "Error" );
      return;
   }
   
   
   // Pop this dialog
   Canvas.popDialog(NewProjectDlg);
   

   copyProjectGameBinaries( %gameLocation, %name );
      
   // Torsion default project (will check for platform stuff etc)
   t2dTorsion::copyProject( %gameLocation, %name, %cbCopy.getValue() );
   
   %projectFile = %gameLocation @ "project.t2dProj";
   
   // Post Create Project
   Projects::GetEventManager().postEvent( "_ProjectCreate", %projectFile );   
  
   // Post Open Event
   if(! LBProjectObj.isActive())
      Projects::GetEventManager().postEvent( "_ProjectOpen", %projectFile );
   else 
   {
      $pref::lastProject = $pref::startupProject = %projectFile;
      reloadProject();
   }
}


function createNewProject(%name, %template)
{
   %projectLocation = getMainDotCsDir();
   //Cut out the tgb for this project.
     %projectLocation =  strreplace(%projectLocation, "/tgb", "");
     %projectLocation = %projectLocation @ "/MyProjects/" @ %name;
     
   %projectLocation = expandFilename(%projectLocation);
   
   // The name must not be empty
   if( isDirectory(%projectLocation) )
   {
      MessageBoxOK("Error Creating Project", "There is already a project with this name" );
      return false;
   }   
   
   %gameLocation = expandFilename(%projectLocation @ "/");
   
   if( isFile( %gameLocation @ "project.t2dProj" ) )
   {
      %result = messageBox( "Invalid Name", "A game already exists at this location. Choose a new name or delete the old game before continuing.", "Ok" );
      return false;
   }
      
   // Template Hierarchy
   //  force overwrite
   %templateLocation = $newProjectTemplateLocation[$newProjectTemplate[%template]];
   //if(!pathCopy(%templateLocation, %gameLocation @ "game/", false))
  // {
  //    messageBox( "Error Creating Project", "An error occurred while trying to create your project.\n\nCould not create the output path, Access Denied.", "Ok", "Error" );
  //    return;
  // }
 //  
   // Any resources
   copyResourceIncludes(%gameLocation, %templateLocation);
   
   
   // Pop this dialog
   Canvas.popDialog(NewProjectDlg);
   
   // Common Hierarchy
   //  force overwrite
   pathCopy( expandFilename(%templateLocation @ "/common"), %gameLocation @ "projectFiles/common/", false );
   // Game Hierarchy
   //  force overwrite
   pathCopy( expandFilename(%templateLocation @ "/game"), %gameLocation @ "projectFiles/game/", false );

   // Main Script
   pathCopy( expandFileName(%templateLocation @ "/main.cs"), %gameLocation @ "projectFiles/main.cs" );

   //Project File
   pathCopy( expandFileName(%templateLocation @ "/project.t2dProj"), %gameLocation @ "projectFiles/project.t2dProj" );

   //Torsion Project
   //pathCopy( expandFileName(%templateLocation @ "/iTorque2DGame.torsion"), %gameLocation @ "projectFiles/iTorque2DGame.torsion" );
   t2dTorsion::copyProject( %gameLocation @ "projectFiles/", "iTorque2DGame", "" );
   
   //Executables iTorque2DGame.exe and iTorque2DGame_debug.exe
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/iTorque2DGame.exe"), %gameLocation @ "/projectFiles/iTorque2DGame.exe" );
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/iTorque2DGame_debug.exe"), %gameLocation @ "/projectFiles/iTorque2DGame_debug.exe" );
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/iTorque2DGame.app"), %gameLocation @ "/projectFiles/iTorque2DGame.app" );
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/iTorque2DGame_debug.app"), %gameLocation @ "/projectFiles/iTorque2DGame_debug.app" );

   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/Default.png"), %gameLocation @ "/projectFiles/Default.png" );
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/cleandso.command"), %gameLocation @ "/projectFiles/cleandso.command" );
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/cleandso.bat"), %gameLocation @ "/projectFiles/cleandso.bat" );

   //We need to copy Build Files as well
   pathCopy( expandFileName($newProjectTemplateCommonFileFolder @ "/buildFiles"), %gameLocation @ "/buildFiles/" );
   
   
   %projectFile = %gameLocation @ "projectFiles/project.t2dProj";
   
   // Post Create Project
   Projects::GetEventManager().postEvent( "_ProjectCreate", %projectFile );   
  
   // Post Open Event
   if(! LBProjectObj.isActive())
   {
      Projects::GetEventManager().postEvent( "_ProjectOpen", %projectFile );
      $Game::ProductName = %name;
      _saveGameConfigurationData(%gameLocation @ "/common/commonConfig.xml");
   }
   else 
   {
      $pref::lastProject = $pref::startupProject = %projectFile;
      reloadProject();
   }
}

function createNewXcodeProject()
{
   createNewProjectiPhone( NewProjectNameText.getText());
   
   runBatchFile("sh", expandFileName($batch::CreateXCode)@ ";" @ NewProjectLocationText.getText() @ "/" @ NewProjectNameText.getText() @ ";" @ NewProjectNameText.getText(), true);
   $Game::XcodeDir = NewProjectLocationText.getText() @ "/" @ NewProjectNameText.getText()@ "/iPhoneXcodeProject/engine/compilers/Xcode_iPhone/Torque2D.xcodeproj";
}

// [neo, 6/1/2007 - #3167]
// Copy over game executable and any other binaries needed for a project
// Rename the game executable so it matches the project name (non mac only)
function copyProjectGameBinaries( %gameLocation, %name )
{
      %exename = %gameLocation @ %name @ ".exe";
      %appname = %gameLocation @ %name @ ".app";
      %srcpath = expandFileName( "^tool/templates/commonFiles/" );
      
      pathCopy( %srcpath @ "iTorque2DGame.exe",    %exename, false );
      pathCopy( %srcpath @ "iTorque2DGame.app",    %appname, false );
      pathCopy( %srcpath @ "unicows.dll",    %gameLocation @ "unicows.dll" );
      pathCopy( %srcpath @ "openAL32.dll",   %gameLocation @ "openAL32.dll" );
}

function copyResourceIncludes(%gameLocation, %template)
{
   %resInclude = expandFilename($templatesDirectory @ %template @ ".ggres");
   if (!isFile(%resInclude))
      return;
      
   %outputDir = %gameLocation @ "resources/";
   
   %fileObject = new FileObject();
   %blah = %fileObject.openForRead(%resInclude);

   while(!%fileObject.isEOF())
   {
      %resource = %fileObject.readLine();
      if (%resource $= "")
         continue;
      
      %resourceDir = expandFilename($resourcesDirectory @ %resource);
      if(!pathCopy(%resourceDir, %outputDir @ %resource, false))
      {
         %errorMsg = "An error occurred while trying to copy some resources to your project.\n\nCould not find the resource <" SPC %resource SPC ">!";
         messageBox( "Error Copying Resources", %errorMsg, "Ok", "Error" );
         continue;
      }
   }

   %fileObject.close();
   %fileObject.delete();
}

function isValidProject(%project)
{
   // Should be more robust check.
   return isFile(%project @ "/project.t2dproj");
}
