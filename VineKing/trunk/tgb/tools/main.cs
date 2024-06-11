//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Space separated list of editors to load.
//---------------------------------------------------------------------------------------------
$editors = "editorClasses debugger guiEditor particleEditor leveleditor tileLayerEditor projectWizard animationEditor imageEditor behaviorEditor localPointEditor";

$Tools::resourcePath = "tools/";

// Hoping this will fix the startup problem -MP
// $reloadWithStartup = false;

//---------------------------------------------------------------------------------------------
// Tools Package.
//---------------------------------------------------------------------------------------------
package Tools
{
   function loadKeybindings()
   {
      Parent::loadKeybindings();
      
      // [neo, 6/12/2007 - #3222]
      // Try load this, if it is empty or does not exist it will fall through below
      loadCustomEditorbindings();
      
      if (isObject(levelEditorMap))
         return;
         
      new ActionMap(levelEditorMap);      
                  
      levelEditorMap.bind(keyboard, "lbracket", layerSelectionDown, "Move Selection One Layer Closer To Camera");
      levelEditorMap.bind(keyboard, "rbracket", layerSelectionUp, "Move Selection One Layer Away From Camera");
      
      levelEditorMap.bind(keyboard, "escape", levelBuilderSetSelectionTool, "Activate the Selection Tool");
      
      levelEditorMap.bind(keyboard, "e", levelBuilderSetEditPanel, "Show the Edit Panel");
      levelEditorMap.bind(keyboard, "c", levelBuilderSetCreatePanel, "Show the Create Panel");
      levelEditorMap.bind(keyboard, "p", levelBuilderSetProjectPanel, "Show the Project Panel");
      
      levelEditorMap.bind(keyboard, "home", levelBuilderHomeView, "Home the View");
      levelEditorMap.bind(keyboard, "end", levelBuilderZoomToFit, "Show All Scene Contents");
      levelEditorMap.bind(keyboard, "equals", levelBuilderZoomViewIn, "Zoom the View Window In");
      levelEditorMap.bind(keyboard, "minus", levelBuilderZoomViewOut, "Zoom the View Window Out");
      levelEditorMap.bind(keyboard, "left", levelBuilderViewLeft, "Move the View Window Left");
      levelEditorMap.bind(keyboard, "right", levelBuilderViewRight, "Move the View Window Right");
      levelEditorMap.bind(keyboard, "up", levelBuilderViewUp, "Move the View Window Up");
      levelEditorMap.bind(keyboard, "down", levelBuilderViewDown, "Move the View Window Down");
      levelEditorMap.bindCmd(keyboard, "F5", "runGame();", "");
      
      if($platform $= "macos")
      {
         levelEditorMap.bind(keyboard, "cmd z",       levelBuilderUndo, "Undo The Last Action");
         levelEditorMap.bind(keyboard, "cmd-shift z", levelBuilderRedo, "Redo an Undone Action");
         levelEditorMap.bind(keyboard, "cmd c",       levelBuilderCopy, "Copy the Selected Objects");
         levelEditorMap.bind(keyboard, "cmd x",       levelBuilderCut, "Cut the Selected Objects");
         levelEditorMap.bind(keyboard, "cmd v",       levelBuilderPaste, "Paste Cut or Copied Objects");
      }
      else
      {
         levelEditorMap.bind(keyboard, "ctrl z", levelBuilderUndo, "Undo The Last Action");
         levelEditorMap.bind(keyboard, "ctrl y", levelBuilderRedo, "Redo an Undone Action");
         levelEditorMap.bind(keyboard, "ctrl c", levelBuilderCopy, "Copy the Selected Objects");
         levelEditorMap.bind(keyboard, "ctrl x", levelBuilderCut, "Cut the Selected Objects");
         levelEditorMap.bind(keyboard, "ctrl v", levelBuilderPaste, "Paste Cut or Copied Objects");
      }

   }
   
   // [neo, 6/12/2007 - #3222]
   // Save out key bindings if they have changed
   function saveCustomEditorBindings()
   {      
      %bindFile = getPrefsPath( "customEditorBind.cs" );
      
      levelEditorMap.save( %bindFile );
   }

   // [neo, 6/12/2007 - #3222]
   // Save out key bindings if they have changed
   function loadCustomEditorbindings()
   {
      %bindFile = getPrefsPath( "customEditorBind.cs" );
   
      if( isFile( %bindFile ) )
      {
         exec( %bindFile );
      
         return true;
      }
      
      return false;
   }

   
   // [neo, 6/1/2007 - #3168]
   // On windows platform make sure tool and game executables 
   function checkPlatformAssociations()
   {
      if( $platform $= "windows" )
      {
         %tgbr = new TGBRegistryManager();
   
         if( isObject( %tgbr ) )
         {
            //if( !%tgbr.fileAssociationExists() )
               %tgbr.setFileAssociations();
      
            //if( !%tgbr.executablePathsAreRegistered() )
               %tgbr.registerExecutablePaths();
         
            %tgbr.delete();
         }
      }
   }
   
   // Start-up.
   function onStart()
   {
      Parent::onStart();
      
      //%toggle = $Scripts::ignoreDSOs;
      //$Scripts::ignoreDSOs = true;
       
      echo(" % - Initializing Tools");
      
      // Common GUI stuff.
      exec("./gui/cursors.ed.cs");
      exec("./gui/profiles.ed.cs");
   
      $ignoredDatablockSet = new SimSet();
   
      // [neo, 6/1/2007 - #3166]
      checkPlatformAssociations();
   
      %count = getWordCount($editors);
      for (%i = 0; %i < %count; %i++)
      {
         exec("./" @ getWord($editors, %i) @ "/main.cs");
         call("initialize" @ getWord($editors, %i));
      }

      // Make sure we get editor profiles before any GUI's
      exec("./gui/guiDialogs.ed.cs");

      // Load up the tools resources. All the editors are initialized at this point, so
      // resources can override, redefine, or add functionality.
      Tools::LoadResources( $Tools::resourcePath );
      
      //$Scripts::ignoreDSOs = %toggle;
   }
   
   // Shutdown.
   function onExit()
   {
      GuiFormManager::SaveLayout(LevelBuilder, Default, User);
      
      %count = getWordCount($editors);
      for (%i = 0; %i < %count; %i++)
      {
         call("destroy" @ getWord($editors, %i));
      }
      
      // Export Preferences.
      echo("Exporting Gui preferences.");
      export("$Pref::FileDialogs::*", "fileDialogPrefs.cs", false);
	
      // Call Parent.
      Parent::onExit();
   }
};

function Tools::LoadResources( %path )
{
   %resourcesPath = %path @ "resources/";
   %resourcesList = getDirectoryList( %resourcesPath );
   
   %wordCount = getFieldCount( %resourcesList );
   for( %i = 0; %i < %wordCount; %i++ )
   {
      %resource = GetField( %resourcesList, %i );
      if( isFile( %resourcesPath @ %resource @ "/resourceDatabase.cs") )
         ResourceObject::load( %path, %resource );
   }
}


//-----------------------------------------------------------------------------
// Activate Package.
//-----------------------------------------------------------------------------
activatePackage(Tools);

