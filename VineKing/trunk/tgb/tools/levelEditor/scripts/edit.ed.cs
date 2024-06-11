//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

$levelBuilderClipboardFile = getPrefsPath( "levelEditor/clipboard.cs" );

function reloadImageMaps()
{
   flushTextureCache();
   recompileAllImageMaps();
}

function reloadProject()
{
   //[neo, 4/6/2007 - #3192]
   // Track when we are reloading a project
   $pref::reloadLastProject = true;
   
   //The reload with startup is set whenever reload is called with a startupProject rather than
   //a lastProject path. We reset both and make startup null so that the engine can find the game.   
   if($reloadWithStartup)
   {
      $pref::lastProject = $Pref::startupProject;
      $Pref::startupProject = "";
   }
   else
   {
      $Pref::startupProject = "";
   }
   
   setCurrentDirectory( expandFilename("^tool/") );
   
   restartInstance();
}


function openProjectFolder()
{
   echo("Opening Project Folder At " @ expandFileName("^project/")) ;
   openFolder(expandFileName("^project/"));   
}

function openVS2008Project()
{
   if($platform $= "windows")
   {
      %pathToProject = expandFileName("^project/../../buildFiles/VisualStudio2008/iTorque2DGame.sln");
      echo(%pathToProject);
      
      if(!shellExecute(%pathToProject))
      {
         messageBox("Error", "Cannot find the project file, Sorry!");
      }
   }
   else
   {
       messageBox("Error", "Cannot open Visual Studio Projects in OSX, Sorry!");
   }
}
function openVS2005Project()
{
   if($platform $= "windows")
   {
      
      %pathToProject = expandFileName("^project/../../buildFiles/VisualStudio2005/iTorque2DGame.sln");
      echo(%pathToProject);
      
      if(!shellExecute(%pathToProject))
      {
         messageBox("Error", "Cannot find the project file, Sorry!");
      }
   }
   else
   {
      messageBox("Error", "Cannot open Visual Studio Projects in OSX, Sorry!");
   }
}
function openXCodeProject()
{
   if($platform $= "macos")
   {
       %pathToProject = expandFileName("^project/../../buildFiles/XCode_iPhone/iTorque2D.xcodeproj");
       %batchFile = expandFileName("./openXcode.sh");
	%cmds = expandFileName("./openXcode.sh") @ "; " @ %pathToProject;
	echo("Doing : " @ %cmds);
       runBatchFile("sh", %cmds, true);
       echo("Finished running batchfile");   
}
   else
   {
       messageBox("Error", "Cannot open XCode projects on Windows, Sorry!");
   }
}

function LevelBuilderSceneEdit::groupObjects(%this)
{
   %this.groupAcquiredObjects();
   RefreshTreeView();
}

function LevelBuilderSceneEdit::breakApart(%this)
{
   %this.breakApartAcquiredObjects();
   RefreshTreeView();
}

function RefreshTreeView()
{
   GuiFormManager::SendContentMessage($LBTreeViewContent, "", "openCurrentGraph");
}

function levelBuilderUndo(%val)
{
   if (%val)
      ToolManager.undo();
}

function levelBuilderRedo(%val)
{
   if (%val)
      ToolManager.redo();
}

function levelBuilderCut(%val)
{
   if (%val)
   {
      levelBuilderCopy(1);
      // Undo will be handled by this call.
      ToolManager.deleteAcquiredObjects();
   }
}

function levelBuilderCopy(%val)
{
   if (%val)
      saveSelectedObjectsCallback($levelBuilderClipboardFile);
}

function levelBuilderPaste(%val)
{
   if (%val)
   {
      %undo = new UndoScriptAction() { actionName = "Paste"; class = UndoPasteAction; };
      %undo.objectList = new SimSet();
      
      addToLevelCallback($levelBuilderClipboardFile);
      %newObjectList = $LevelManagement::newObjects;
      
      if( isObject( %newObjectList ) )
      {
         ToolManager.clearAcquisition();
         
         if( %newObjectList.isMemberOfClass( "t2dSceneObject" ) )
         {
            %undo.objectList.add( %newObjectList );
            ToolManager.acquireObject( %newObjectList );
         }
         
         else
         {
            %count = %newObjectList.getCount();
            for (%i = 0; %i < %count; %i++)
            {
               %obj = %newObjectList.getObject( %i );
               ToolManager.acquireObject( %obj );
               %undo.objectList.add(%obj);
            }
         }
      }
      
      if (%undo.objectList.getCount() > 0)
         %undo.addToManager(LevelBuilderUndoManager);
      else
      {
         %undo.objectList.delete();
         %undo.delete();
      }
   }
}

function levelBuilderEditLevelDatablocks()
{
   //Shows a little gui to allow per datablock selection from the managed set,
   //and copy these to the current level datablocks file...
   
   //First, collect the already existing datablocks in the level - 
   
      LDListLevel.clearItems();
      
      %ldfilename = filePath($levelEditor::LastLevel) @ "/datablocks/" @ fileBase($levelEditor::LastLevel) @ "_datablocks.cs";
      echo("level datablocks location : " @ %ldfilename);
      
      LDELevelNameText.text = fileBase($levelEditor::LastLevel) @ " Datablocks";
      
      //Load the level datablocks file , clearing the values first      
      if(isObject($levelDatablocks))
      {
         $levelDatablocks.delete();
      }
      
      exec(%ldfilename);
      if(isObject($levelDatablocks))
      {
         %lcount = $levelDatablocks.getCount();
         if(isObject($ld_editorDatablocks))
           $ld_editorDatablocks.clear();
            else
           $ld_editorDatablocks = new SimSet();
            
         if(%lcount)
         {
            for(%ii = 0; %ii < %lcount; %ii++)
            {
                 %class = $levelDatablocks.getObject(%ii).getClassName();
                 %name = $levelDatablocks.getObject(%ii).getName();
                 LDListLevel.addItem(%class @ " [ " @ %name @ " ]");                 
                 $levelDatablocks.getObject(%ii).setName($levelDatablocks.getObject(%ii).getName() @ "_ldeditor");
                     //So the editor doesnt try to use this datablock
                 $ignoredDatablockSet.add($levelDatablocks.getObject(%ii));
                 $ld_editorDatablocks.add($levelDatablocks.getObject(%ii));
            }      
         }
      }
   
   //Second, show the list of all of the datablocks type(name) - 
   
      LDListAll.clearItems();
      
      //Store previous datablocks for now, so we can clean up properly
      $previousManagedDatablocks = '';
      $previousManagedDatablocks = $managedDatablockSet;
      if(isObject($managedDatablockSet))
      {
            echo("Managed set is valid, enumerating.");
         %count = $managedDatablockSet.getCount();
            echo("Found " @ %count @ " datablocks, adding to list.");
         for(%i = 0; %i < %count; %i++)
         {
              %class = $managedDatablockSet.getObject(%i).getClassName();
              %name = $managedDatablockSet.getObject(%i).getName();
              echo("adding : " @ %name);
              LDListAll.addItem(%class @ " [ " @ %name @ " ]");
         }         
      }
      
   //Show the user the dialog
   Canvas.PushDialog(levelBuilderLevelDatablocksEditor);
}

function LDEonAddObject()
{
   %selcount = LDListAll.getSelCount();
   if(%selcount)
   {
      %list = LDListAll.getSelectedItems();
      for (%i = 0; %i < %selcount; %i++)
      {
         %item = getWord(%list, %i);
         %itemlistname = LDListAll.getItemText(%item);
         if(LDListLevel.findItemText(%itemlistname, false) == -1)
         {
            LDListLevel.addItem(%itemlistname);   
         }
      }
   }
}

function LDEonRemoveObject()
{
   %selcount = LDListLevel.getSelCount();
   if(%selcount)
   {
      %list = LDListlevel.getSelectedItems();
      %item = getWord(%list, 0);
      LDListLevel.deleteItem(%item);
   }
}

function LDEonApply()
{
   //Store state first, and then hide dialog

   //Take the newly selected one from the managed side, 
   //copy them to a new set, and save that as the level datablocks
   //instead.
   
   %newLevelDatablocks = new SimSet()
   {
      canSaveDynamicFields = "1";
      setType = "Datablocks";
      
   };
   
   %count = LDListlevel.getItemCount();
   for (%i = 0; %i < %count; %i++)
   {
      %itemname = "";
      %itemlistname = "";
      %itemlistname = LDListlevel.getItemText(%i);
      %itemnamestart = strpos(%itemlistname ,"[");
      %itemnameend = strpos(%itemlistname ,"]");
      if(%itemnamestart != -1 && %itemnameend != -1 && %itemlistname !$= "")
      {
         %strlen = strlen(%itemlistname);
         %itemname = getSubStr(%itemlistname, %itemnamestart+2, (%strlen - (%itemnamestart + 2)) - 2);
         if(%itemname !$= "")
         {
            %newLevelDatablocks.add(%itemname);
         }
         
         echo(%itemname @ " added to level datablocks!" ); 
      }
      
      //Once we have added the new set of items, we can persist it to disk.
      %ldfilename = filePath($levelEditor::LastLevel) @ "/datablocks/" @ fileBase($levelEditor::LastLevel) @ "_datablocks.cs";
      if( isWriteableFileName( %ldfilename ) )
      {
         %fo = new FileObject();
         if( %fo.openForWrite( %ldfilename ) )
         {      
            %fo.writeObject(%newLevelDatablocks, "$levelDatablocks = ");
            %fo.close();
         }
         %fo.delete();
      }
   }
   
   
   //Cleanup used items here
   //For all ignored items
   for(%i = 0; %i < $ignoredDatablockSet.getCount(); %i++)
   {
      //Check against all the ld editors last copies
      for(%ii = 0; %ii < $ld_editorDatablocks.getCount(); %ii++)
      {         
         %name = $ld_editorDatablocks.getObject(%ii).getName();
         if($ignoredDatablockSet.getObject(%i).getName() $= %name)
         {
            %obj = $ignoredDatablockSet.getObject(%i);
            %obj.delete();
         }
      }
   }
   
   $ld_editorDatablocks.delete();
   $ld_editorDatablocks = 0;
   
   Canvas.popDialog(levelBuilderLevelDatablocksEditor);
}

function LDEonCancel()
{
   Canvas.popDialog(levelBuilderLevelDatablocksEditor);
}

function UndoPasteAction::undo(%this)
{
   ToolManager.clearAcquisition();
   for (%i = 0; %i < %this.objectList.getCount(); %i++)
      ToolManager.moveToRecycleBin(%this.objectList.getObject(%i));
}

function UndoPasteAction::redo(%this)
{
   for (%i = 0; %i < %this.objectList.getCount(); %i++)
      ToolManager.moveFromRecycleBin(%this.objectList.getObject(%i));
}

function levelBuilderSetSelectionTool(%val)
{
   if (%val && (ToolManager.getActiveTool() != LevelEditorSelectionTool.getId()))
      LevelBuilderToolManager::setTool(LevelEditorSelectionTool);
}

function levelBuilderHomeView(%val)
{
   if (%val)
   {
      %sceneWindow = ToolManager.getLastWindow();
      %scenegraph = %sceneWindow.getSceneGraph();
      %cameraPosition = "0 0";
      %cameraSize = $levelEditor::DefaultCameraSize;
      if (%scenegraph.cameraPosition)
         %cameraPosition = %scenegraph.cameraPosition;
      if (%scenegraph.cameraSize)
         %cameraSize = %scenegraph.cameraSize;
         
      %sceneWindow.setTargetCameraPosition(%cameraPosition, %cameraSize);
      %sceneWindow.setTargetCameraZoom(1.0);
      %sceneWindow.startCameraMove(0.5);
   }
}

function levelBuilderZoomView(%amount)
{
   %sceneWindow = ToolManager.getLastWindow();
   %scenegraph = %sceneWindow.getSceneGraph();
   %cameraPosition = ToolManager.getLastWindow().getCurrentCameraPosition();
   %cameraSize = $levelEditor::DefaultCameraSize;
   if (%scenegraph.cameraSize)
      %cameraSize = %scenegraph.cameraSize;
   
   %windowX = getWord( %sceneWindow.getExtent(), 0 );
   %windowY = getWord( %sceneWindow.getExtent(), 1 );
   
   %newX = getWord( %cameraSize, 0 ) * %windowX / $levelEditor::DesignResolutionX;
   %newY = getWord( %cameraSize, 1 ) * %windowY / $levelEditor::DesignResolutionY;
   
   %sceneWindow.setTargetCameraPosition(%cameraPosition, %newX SPC %newY);
   %sceneWindow.setTargetCameraZoom(%amount);
   %sceneWindow.startCameraMove(0.5);
}

function levelBuilderZoomToFit(%val)
{
   if (%val)
   {
      %sceneWindow = ToolManager.getLastWindow();
      %sceneGraph = %sceneWindow.getSceneGraph();
      if (!isObject(%sceneWindow) || !isObject(%sceneGraph))
         return;
      
      %count = %sceneGraph.getSceneObjectCount();
      if (%count < 1)
         return;
      
      %minX = 0;
      %minY = 0;
      %maxX = 1;
      %maxY = 1;
      for (%i = 0; %i < %count; %i++)
      {
         %object = %sceneGraph.getSceneObject(%i);
         if (%i == 0)
         {
            %minX = %object.getPositionX() - (%object.getWidth() / 2);
            %minY = %object.getPositionY() - (%object.getHeight() / 2);
            %maxX = %object.getPositionX() + (%object.getWidth() / 2);
            %maxY = %object.getPositionY() + (%object.getHeight() / 2);
         }
         else
         {
            %newMinX = %object.getPositionX() - (%object.getWidth() / 2);
            %newMinY = %object.getPositionY() - (%object.getHeight() / 2);
            %newMaxX = %object.getPositionX() + (%object.getWidth() / 2);
            %newMaxY = %object.getPositionY() + (%object.getHeight() / 2);
            if (%newMinX < %minX)
               %minX = %newMinX;
            if (%newMaxX > %maxX)
               %maxX = %newMaxX;
            if (%newMinY < %minY)
               %minY = %newMinY;
            if (%newMaxY > %maxY)
               %maxY = %newMaxY;
         }
      }
      
      %sceneWindow.setTargetCameraArea(%minX, %minY, %maxX, %maxY);
      %sceneWindow.setTargetCameraZoom(1.0);
      %sceneWindow.startCameraMove(0.5);
   }
}

function levelBuilderZoomToSelected(%val)
{
   if (%val)
   {
      %sceneWindow = ToolManager.getLastWindow();
      if (!isObject(%sceneWindow))
         return;
      
      %selectedObjects = ToolManager.getAcquiredObjects();
      %count = %selectedObjects.getCount();
      if (%count < 1)
         return;
      
      %minX = 0;
      %minY = 0;
      %maxX = 1;
      %maxY = 1;
      for (%i = 0; %i < %count; %i++)
      {
         %object = %selectedObjects.getObject(%i);
         if (%i == 0)
         {
            %minX = %object.getPositionX() - (%object.getWidth() / 2);
            %minY = %object.getPositionY() - (%object.getHeight() / 2);
            %maxX = %object.getPositionX() + (%object.getWidth() / 2);
            %maxY = %object.getPositionY() + (%object.getHeight() / 2);
         }
         else
         {
            %newMinX = %object.getPositionX() - (%object.getWidth() / 2);
            %newMinY = %object.getPositionY() - (%object.getHeight() / 2);
            %newMaxX = %object.getPositionX() + (%object.getWidth() / 2);
            %newMaxY = %object.getPositionY() + (%object.getHeight() / 2);
            if (%newMinX < %minX)
               %minX = %newMinX;
            if (%newMaxX > %maxX)
               %maxX = %newMaxX;
            if (%newMinY < %minY)
               %minY = %newMinY;
            if (%newMaxY > %maxY)
               %maxY = %newMaxY;
         }
      }
      
      %sceneWindow.setTargetCameraArea(%minX, %minY, %maxX, %maxY);
      %sceneWindow.setTargetCameraZoom(1.0);
      %sceneWindow.startCameraMove(0.5);
   }
}

function levelBuilderSetEditPanel(%val)
{
   if (%val)
      GuiFormManager::SendContentMessage($LBSideBarContent, "", "setTabPage" SPC "edit");
}

function levelBuilderSetProjectPanel(%val)
{
   if (%val)
      GuiFormManager::SendContentMessage($LBSideBarContent, "", "setTabPage" SPC "project");
}

function levelBuilderSetCreatePanel(%val)
{
   if (%val)
      GuiFormManager::SendContentMessage($LBSideBarContent, "", "setTabPage" SPC "create");
}

function levelBuilderZoomViewIn(%val)
{
   if (%val)
   {
      %camera = ToolManager.getLastWindow();
      %zoom = %camera.getCurrentCameraZoom();
      %amount = 120 * 0.001 * %zoom;
      %camera.setTargetCameraZoom(%zoom + %amount);
      %camera.startCameraMove(0.1);
   }
}

function levelBuilderZoomViewOut(%val)
{
   if (%val)
   {
      %camera = ToolManager.getLastWindow();
      %zoom = %camera.getCurrentCameraZoom();
      %amount = -120 * 0.001 * %zoom;
      %camera.setTargetCameraZoom(%zoom + %amount);
      %camera.startCameraMove(0.1);
   }
}

function levelBuilderViewLeft(%val)
{
   if (%val && (ToolManager.getAcquiredObjectCount() == 0))
   {
      %camera = ToolManager.getLastWindow();
      %position = %camera.getCurrentCameraPosition();
      %zoom = %camera.getCurrentCameraZoom();
      %amount = (-10 + %zoom) SPC 0;
      %camera.setTargetCameraPosition(t2dVectorAdd(%position, %amount));
      %camera.startCameraMove(0.1);
   }
}

function levelBuilderViewRight(%val)
{
   if (%val && (ToolManager.getAcquiredObjectCount() == 0))
   {
      %camera = ToolManager.getLastWindow();
      %position = %camera.getCurrentCameraPosition();
      %zoom = %camera.getCurrentCameraZoom();
      %amount = (10 - %zoom) SPC 0;
      %camera.setTargetCameraPosition(t2dVectorAdd(%position, %amount));
      %camera.startCameraMove(0.1);
   }
}

function levelBuilderViewUp(%val)
{
   if (%val && (ToolManager.getAcquiredObjectCount() == 0))
   {
      %camera = ToolManager.getLastWindow();
      %position = %camera.getCurrentCameraPosition();
      %zoom = %camera.getCurrentCameraZoom();
      %amount = 0 SPC (-10 + %zoom);
      %camera.setTargetCameraPosition(t2dVectorAdd(%position, %amount));
      %camera.startCameraMove(0.1);
   }
}

function levelBuilderViewDown(%val)
{
   if (%val && (ToolManager.getAcquiredObjectCount() == 0))
   {
      %camera = ToolManager.getLastWindow();
      %position = %camera.getCurrentCameraPosition();
      %zoom = %camera.getCurrentCameraZoom();
      %amount = 0 SPC (10 - %zoom);
      %camera.setTargetCameraPosition(t2dVectorAdd(%position, %amount));
      %camera.startCameraMove(0.1);
   }
}
