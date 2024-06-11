//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

// This holds the last scenegraph that was loaded.
$lastLoadedScene = "";
$CurrentLevelResources = "";
$CurrentLevel = "";
$CurrentLevelFileName = ""; // RKS NOTE: I added this so I could save the name of the level that was loaded

function getLastLoadedScene()
{
   return $lastLoadedScene;
}

// This variable determines whether the scenegraph in the level file or the existing scenegraph
// should be used.
$useNewSceneGraph = false;
//---------------------------------------------------------------------------------------------
// t2dSceneWindow.loadLevel
// Loads a level file into a scene window.
//---------------------------------------------------------------------------------------------
function t2dSceneWindow::loadLevel(%sceneWindow, %levelFile)
{
	//------------------------------------------------------------------------------------------
	// RKS NOTE: I added this so I could get the file name and load audio resources based on that
	echo( "t2dSceneWindow::loadLevel -- Loading " @ %levelFile );
	$CurrentLevelFileName = fileName( %levelFile );
	echo( "$CurrentLevelFileName = " @ $CurrentLevelFileName );
	%audioDataBlockFile = "";
	
	// Delete all the current level datablocks.  Idea from this forum post: http://www.torquepowered.com/community/forums/viewthread/86451/2#comments
	if( isObject( $levelDataBlocks ) )
	{
		%numLevelDataBlocks = $levelDatablocks.getCount();
		//echo( "DELETING LEVEL DATA BLOCKS; %numLevelDataBlocks = " @ %numLevelDataBlocks );
		//for( %i = 0; %i < %numLevelDataBlocks; %i++ ) 
		for( %i = %numLevelDataBlocks - 1; %i >= 0; %i-- )
		{
			%object = $levelDatablocks.getObject( %i );
			//echo( " - Deleting " @ %object.getName() @ " at index " @ %i );
			%object.delete();  
		} 
		%numLevelDataBlocks = $levelDatablocks.getCount();
		//echo( "Num datablocks after delete: " @ %numLevelDataBlocks );
		
		//echo( "attempting to delete $levelDatablocks..." );
		//$levelDatablocks.delete();
		//echo( "$levelDatablocks after delete: " @ $levelDatablocks );
	}
	//else
	//{
	//	//echo( "$levelDataBlocks does not exist" );
	//}
	//------------------------------------------------------------------------------------------
	
   // Clean up any previously loaded stuff.
   %sceneWindow.endLevel();
   
   // Load the level.
   $useNewSceneGraph = true;

   alxStopAll();//kill any leftovers
   
   
   //-Mat loadlevel stuff can go here for now
   hideCursor();

	////////////////////////////////////////////////////////////////////////////////////////////////
   //load level resources        -Mat
   %resPath = expandFileName("game");    

   //now get level specific resources 
   //Get just filename, then strip out extension         -Mat
   %levelFileName = fileName( %levelFile );
   %levelFileName = strreplace( %levelFileName, ".t2d", "" );
   
   $CurrentLevel = %levelFileName;
   
   //load Shared resources first
   %commonFile = %resPath @ "/PlatformerArt/SharedResources.cs";
   %commonObject = ResourceObject::load( %commonFile );
	
	//echo( "%commonFile   = " @ %commonFile );
	//echo( "%commonObject = " @ %commonObject );
   
   //load level-specific resources
   %resFile = %resPath @ "/PlatformerArt/" @ %levelFileName @"Resources.cs";   
   
   echo( "--------Loading Resource file" SPC %resFile );//-Mat iPhone debug output	   
   
   %resObject = ResourceObject::load( %resFile );

   if( strstr( %levelFile, "game/" ) != 0 ) 
   {
      %newlevelfile = %resPath @ stripChars( %levelFile, "~" );
   } 
   else 
   {
      %newlevelfile = stripChars( %levelFile, "~" );//just in case there is a stray char from a previous manipulation
   }
   
   echo( "--------Adding to Level" SPC %newlevelfile);//-Mat iPhone debug output
	////////////////////////////////////////////////////////////////////////////////////////////////
   
	
	// RKS NOTE: I commented out the way the set %dbFileName so I could use my own method
   //Get a datablock file name for this level - Sven
   //%dbFileName = %newlevelfile;
   //%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");  
	
	// RKS NOTE: This is my new method  game/data/levels/datablocks
	if( strcmp( $CurrentLevelFileName, "Screen_MainMenu.t2d" ) == 0 )
	{
		// Load the datablock file for the Main Menu
		
		//"game/scripts/AudioDataBlocks_MainMenu.cs"
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Screen_MapSelect_PowerUp.t2d" ) == 0 )
	{
		// Load the datablock file for the Map Select / Power Up Menus
		
		//"game/scripts/AudioDataBlocks_MapAndPowerUpScreen.cs"
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Screen_PixelVandalsLogo.t2d" ) == 0 )
	{
		// Load the datablock file for the Map Select / Power Up Menus
		
		//"game/scripts/AudioDataBlocks_MapAndPowerUpScreen.cs"
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Screen_StoryOpening.t2d" ) == 0 )
	{
		// Load the datablock file for the Story Opening Screen
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Screen_TutorialOpening.t2d" ) == 0 )
	{
		// Load the datablock file for the Tutorial Opening Screen
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Boss_01.t2d" ) == 0 )
	{
		// Load the datablock file for level Boss_01
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Boss_02.t2d" ) == 0 )
	{
		// Load the datablock file for level Boss_01
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Boss_03.t2d" ) == 0 )
	{
		// Load the datablock file for level Boss_01
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Level_00_00.t2d" ) == 0 )
	{
		// Load the datablock file for level Boss_01
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Screen_StoryEnding.t2d" ) == 0 )
	{
		// Load the datablock file for the Story Opening Screen
		
		%dbFileName = %newlevelfile;
		%dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");
	}
	else if( strcmp( $CurrentLevelFileName, "Level_01_01.t2d" ) == 0 )
	{
		%dbFileName = "game/data/levels/datablocks/Level_Tutorial_Datablocks.cs";
	}
    //else if( strcmp( $CurrentLevelFileName, "Level_NewTutorial.t2d" ) == 0 )
	//{
	//	%dbFileName = "game/data/levels/datablocks/Level_Tutorial_Datablocks.cs";
	//}
	else
	{
		// Load the generic datablock file
		
		//"game/scripts/AudioDataBlocks_Levels.cs"
		%dbFileName = "game/data/levels/datablocks/Level_Generic_Datablocks.cs";
	}
	
	echo( "%newlevelFile  = " @ %newlevelfile );
	echo( "%levelFileName = " @ %levelFileName );
	echo( "%dbFileName    = " @ %dbFileName );
	
	echo( "LOADING DATABLOCKS: " @ %dbFileName );
   
   //load level         -Mat
   %scenegraph = %sceneWindow.addToLevel(%newlevelfile, %dbFileName);
	
   
   //Load any level specific GUI
   %LevelGUIfunction = "load" @ %levelFileName @ "GUI";
   if( isFunction( %LevelGUIfunction ) ) 
   {
      eval( %LevelGUIfunction @ "();" );//call the function with no parameters
   }  
   
   
   if (!isObject(%scenegraph))
      return 0;
   
   %sceneWindow.setSceneGraph(%scenegraph);
   
   // Set the window properties from the scene graph if they are available.
   %cameraPosition = %sceneWindow.getCurrentCameraPosition();
   %cameraSize = t2dVectorSub(getWords(%sceneWindow.getCurrentCameraArea(), 2, 3),
                              getWords(%sceneWindow.getCurrentCameraArea(), 0, 1));
                              
   if (%scenegraph.cameraPosition !$= "")
      %cameraPosition = %scenegraph.cameraPosition;
   if (%scenegraph.cameraSize !$= "")
      %cameraSize = %scenegraph.cameraSize;
      
   %sceneWindow.setCurrentCameraPosition(%cameraPosition, %cameraSize);
   
   // Only perform "onLevelLoaded" callbacks when we're NOT editing a level
   //
   //  This is so that objects that may have script associated with them that
   //  the level builder cannot undo will not be executed while using the tool.
   //
          
   if (!$LevelEditorActive)
   {
      // Notify the scenegraph that it was loaded.
      if( %scenegraph.isMethod( "onLevelLoaded" ) )
         %scenegraph.onLevelLoaded( %levelFile );
      
      // And finally, notify all the objects that they were loaded.
      %sceneObjectList = %scenegraph.getSceneObjectList();
      %sceneObjectCount = getWordCount(%sceneObjectList);
      for (%i = 0; %i < %sceneObjectCount; %i++)
      {
         %sceneObject = getWord(%sceneObjectList, %i);
         //if( %sceneObject.isMethod( "onLevelLoaded" ) )
            %sceneObject.onLevelLoaded(%scenegraph);
      }
   }
   
   $lastLoadedScene = %scenegraph;
   return %scenegraph;
}

//---------------------------------------------------------------------------------------------
// t2dSceneWindow.addToLevel
// Adds a level file to a scene window's scenegraph.
//---------------------------------------------------------------------------------------------
function t2dSceneWindow::addToLevel(%sceneWindow, %levelFile, %dbFileName)
{
	//echo( "t2dSceneWindow::addToLevel" ); // RKS
	
   %scenegraph = %sceneWindow.getSceneGraph();
   if (!isObject(%scenegraph))
   {
      %scenegraph = new t2dSceneGraph();
      %sceneWindow.setSceneGraph(%scenegraph);
   }
   
   %newScenegraph = %scenegraph.addToLevel(%levelFile, %dbFileName);
   
   $lastLoadedScene = %newScenegraph;
   return %newScenegraph;
}

//---------------------------------------------------------------------------------------------
// t2dSceneGraph.loadLevel
// Loads a level file into a scenegraph.
//---------------------------------------------------------------------------------------------
function t2dSceneGraph::loadLevel(%sceneGraph, %levelFile)
{
   
   %sceneGraph.endLevel();
   
	echo( "t2dSceneGraph::loadLevel" ); // RKS
	
   //Get a datablock file name for this level - Sven
   %dbFileName = %levelFile;
   %dbFileName = strreplace(%dbFileName, %levelFileName @ ".t2d", "datablocks/" @ %levelFileName @ "_datablocks.cs");   
   
   %newScenegraph = %sceneGraph.addToLevel(%levelFile, %dbFileName);
   
   if (isObject(%newScenegraph) && !$LevelEditorActive)
   {
      // Notify the scenegraph that it was loaded.
      if( %newScenegraph.isMethod( "onLevelLoaded" ) )
         %newScenegraph.onLevelLoaded();
      
      // And finally, notify all the objects that they were loaded.
      %sceneObjectList = %newScenegraph.getSceneObjectList();
      %sceneObjectCount = getWordCount(%sceneObjectList);
      for (%i = 0; %i < %sceneObjectCount; %i++)
      {
         %sceneObject = getWord(%sceneObjectList, %i);
         //if( %sceneObject.isMethod( "onLevelLoaded" ) )
            %sceneObject.onLevelLoaded(%newScenegraph);
      }
   }
   
   $lastLoadedScene = %newScenegraph;
   return %newScenegraph;
}

//---------------------------------------------------------------------------------------------
// t2dSceneGraph.addToLevel
// Adds a level file to a scenegraph, passes an extra datablock parameter per level
//---------------------------------------------------------------------------------------------
function t2dSceneGraph::addToLevel(%scenegraph, %levelFile, %dbFileName)
{
	//echo( "IN SCRIPT:  t2dSceneGraph::addToLevel" ); // RKS
	
   // Reset this. It should always be false unless we are loading into a scenewindow.
   %useNewSceneGraph = $useNewSceneGraph;
   $useNewSceneGraph = false;
   
   // Prevent name clashes when loading a scenegraph with the same name as this one
   %scenegraph = %scenegraph.getId();
   
   // Make sure the file is valid.
   if ((!isFile(%levelFile)) && (!isFile(%levelFile @ ".dso")))
   {
      error("Error loading level " @ %levelFile @ ". Invalid file.");
      return 0;
   }
   
   //Before the level is loaded, load this levels datablocks - Sven
   	
   %dsofilename = strreplace(%dbFileName, ".cs", ".cs.dso");
   if(!isFile(%dbFileName) && !(isFile(%dsofilename)))
   {
      echo("--------::: No datablocks for this level? Level will still be loaded! Watch for errors.");
   }
   else
   {
      echo( "--------Exec()ing level datablocks" SPC %dbFileName );
      if(!isFile(%dbFileName))
      {
         if(isFile(%dsofilename))
         {
          exec(%dbFileName);
         }
      }
            
      exec(%dbFileName);
   }
   
   // Load up the level.
   echo( "--------Exec()ing level" SPC %levelFile );//-Mat iPhone debug output	
   exec(%levelFile);
   
   
   // The level file should have contained a scenegraph, which should now be in the instant
   // group. And, it should be the only thing in the group.
   if (!isObject(%levelContent))
   {
      error("Invalid level file specified: " @ %levelFile);
      return 0;
   }
   
   %newScenegraph = %scenegraph;
   %object = %levelContent;
   $LevelManagement::newObjects = "";
   
   
   
   
   if (%object.getClassName() $= "t2dSceneObjectGroup")
   {
      %newScenegraph.addToScene(%object);
      for (%i = 0; %i < %object.getCount(); %i++)
      {
         %obj = %object.getObject(%i);
         if (%obj.getClassName() $= "t2dParticleEffect")
         {
            %newScenegraph.addToScene(%obj);
            %oldPosition = %obj.getPosition();
            %oldSize = %obj.getSize();
            %obj.loadEffect(%obj.effectFile);
            %obj.setPosition(%oldPosition);
            %obj.setSize(%oldSize);
            %obj.playEffect();
         }
         
         else if (%obj.getClassName() $= "t2dTileLayer")
         {
            %oldPosition = %obj.getPosition();
            %oldSize = %obj.getSize();
            %tileMap = %newScenegraph.getGlobalTileMap();
            if (isObject(%tileMap))
            {
               %tileMap.addTileLayer(%obj);
               %obj.loadTileLayer(%obj.layerFile);
               %obj.setPosition(%oldPosition);
               %obj.setSize(%oldSize);
            }
            else
               error("Unable to find scene graph's global tile map.");
         }
      }
      $LevelManagement::newObjects = %object;
   }
   
   else if( %object.getClassName() $= "t2dSceneObjectSet" )
   {
      // Add each object in the set to the scene.
      for (%i = 0; %i < %object.getCount(); %i++)
      {
         %obj = %object.getObject(%i);
         %newScenegraph.addToScene( %obj );
         if (%obj.getClassName() $= "t2dParticleEffect")
         {
            %oldPosition = %obj.getPosition();
            %oldSize = %obj.getSize();
            %obj.loadEffect(%obj.effectFile);
            %obj.setPosition(%oldPosition);
            %obj.setSize(%oldSize);
            %obj.playEffect();
         }
         
         else if (%obj.getClassName() $= "t2dTileLayer")
         {
            %oldPosition = %obj.getPosition();
            %oldSize = %obj.getSize();
            %tileMap = %newScenegraph.getGlobalTileMap();
            if (isObject(%tileMap))
            {
               %tileMap.addTileLayer(%obj);
               %obj.loadTileLayer(%obj.layerFile);
               %obj.setPosition(%oldPosition);
               %obj.setSize(%oldSize);
            }
            else
               error("Unable to find scene graph's global tile map.");
         }
      }
      $LevelManagement::newObjects = %object;
   }
   
   else if (%object.isMemberOfClass("t2dSceneObject"))
   {
      if (%object.getClassName() $= "t2dParticleEffect")
      {
         %newScenegraph.addToScene(%object);
         %oldPosition = %object.getPosition();
         %oldSize = %object.getSize();
         %object.loadEffect(%object.effectFile);
         %object.setPosition(%oldPosition);
         %object.setSize(%oldSize);
         %object.playEffect();
      }
      
      else if (%object.getClassName() $= "t2dTileLayer")
      {
         %oldPosition = %object.getPosition();
         %oldSize = %object.getSize();
         %tileMap = %newScenegraph.getGlobalTileMap();
         if (isObject(%tileMap))
         {
            %tileMap.addTileLayer(%object);
            %object.loadTileLayer(%object.layerFile);
            %object.setPosition(%oldPosition);
            %object.setSize(%oldSize);
         }
         else
            error("Unable to find scene graph's global tile map.");
      }
      else
         %newScenegraph.addToScene(%object);
      
      $LevelManagement::newObjects = %object;
   }

   // If we got a scenegraph...
   else if (%object.getClassName() $= "t2dSceneGraph")
   {
      %fromSceneGraph = 0;
      %toSceneGraph = 0;
      
      // If we are supposed to use the new scenegraph, we need to copy from the existing scene
      // graph to the new one. Otherwise, we copy the loaded stuff into the existing one.
      if (%useNewSceneGraph)
      {
         %fromSceneGraph = %newScenegraph;
         %toSceneGraph = %object;
      }
      else
      {
         %fromSceneGraph = %object;
         %toSceneGraph = %newScenegraph;
      }

      if (isObject(%fromSceneGraph.getGlobalTileMap()))
         %fromSceneGraph.getGlobalTileMap().delete();

      // If the existing scenegraph has objects in it, then the new stuff should probably be
      // organized nicely in its own group.
      if ((%toSceneGraph.getCount() > 0) && (%fromSceneGraph.getCount() > 0))
      {
         %newGroup = new t2dSceneObjectGroup();
      
         while (%fromSceneGraph.getCount() > 0)
         {
            %obj = %fromSceneGraph.getObject(0);
            %fromSceneGraph.removeFromScene(%obj);
            %obj.setPosition(%obj.getPosition()); // This sets physics.dirty.... =)
            %newGroup.add(%obj);
         }
         
         %toSceneGraph.add(%newGroup);
         $LevelManagement::newObjects = %newGroup;
      }
      else
      // if it does not then simply move the objects over
      {
         while (%fromSceneGraph.getCount() > 0)
         {
            %obj = %fromSceneGraph.getObject(0);
            %fromSceneGraph.removeFromScene(%obj);
            %obj.setPosition(%obj.getPosition()); // This sets physics.dirty.... =)
            %toSceneGraph.addToScene(%obj);
         }
         $LevelManagement::newObjects = %toSceneGraph;
      }
      
      %newScenegraph = %toSceneGraph;
      %fromSceneGraph.delete();
   }
   
   // Unsupported object type.
   else
   {
      error("Error loading level " @ %levelFile @ ". " @ %object.getClassName() @
            " is not a valid level object type.");
      return 0;
   }
   
   if( isObject( $persistentObjectSet ) )
   {
	   //echo( " -> $persistentObjectSet EXISTS!\n" ); // RKS
	   
      // Now we need to move all the persistent objects into the scene.
      %count = $persistentObjectSet.getCount();
      for (%i = 0; %i < %count; %i++)
      {
         %object = $persistentObjectSet.getObject(%i);
         %sg = %object.getSceneGraph();
         if(%sg)
            %sg.removeFromScene(%object);
            
         %newScenegraph.addToScene(%object);
         %object.setPosition( %object.getPosition() );
      }
   }
	//else
	//{
	//	echo( " -> $persistentObjectSet DOES NOT EXIST!\n" ); // RKS - I added this else statement
	//}
   
   // And finally, perform any post creation actions
   %newScenegraph.performPostInit();
   
   $lastLoadedScene = %newScenegraph;
	
   return %newScenegraph;
}

//---------------------------------------------------------------------------------------------
// Level Load/unload Functions
//---------------------------------------------------------------------------------------------
// Load Resource Function
function PlatformerArt::LoadResource( %this )
{
//make loading GUI?	
}

// Unload Resource Function
function PlatformerArt::UnloadResource( %this )
{
   ResourceObject::Unload( %this );
   // Remove all of the objects we use
   if( isObject( %this.Data ) && %this.Data.GetCount() > 0 )
   {      
      while( %this.Data.getCount() > 0 )
      {
         %datablockObj = %this.Data.getObject( 0 );
         %this.Data.remove( %datablockObj );
         if( isObject( %datablockObj ) )
            %datablockObj.delete();
      }
   }
}
//---------------------------------------------------------------------------------------------
// t2dSceneWindow.endLevel
// Clears a scene window.
//---------------------------------------------------------------------------------------------
function t2dSceneWindow::endLevel(%sceneWindow)
{
	//echo( "t2dSceneWindow::endLevel" ); // RKS
	
   %scenegraph = %sceneWindow.getSceneGraph();
   
   if (!isObject(%scenegraph))
      return;
   
   %scenegraph.endLevel();
   
   if (isObject(%scenegraph))
   {
      if( isObject( %scenegraph.getGlobalTileMap() ) )
         %scenegraph.getGlobalTileMap().delete();
         
      %scenegraph.delete();
   }
   
   $lastLoadedScene = "";
}

//---------------------------------------------------------------------------------------------
// t2dSceneGraph.endLevel
// Clears a scenegraph.
//---------------------------------------------------------------------------------------------
function t2dSceneGraph::endLevel(%sceneGraph)
{
   if (!$LevelEditorActive)
   {
      %sceneObjectList = %sceneGraph.getSceneObjectList();
      // And finally, notify all the objects that they were loaded.
      for (%i = 0; %i < getWordCount(%sceneObjectList); %i++)
      {
         %sceneObject = getWord(%sceneObjectList, %i);
         //if( %sceneObject.isMethod( "onLevelEnded" ) )
            %sceneObject.onLevelEnded(%sceneGraph);
      }
      
      // Notify the scenegraph that the level ended.
      if( %sceneGraph.isMethod( "onLevelEnded" ) )
         %sceneGraph.onLevelEnded();
   }
	
   if( isObject( $persistentObjectSet ) )
   {
      %count = $persistentObjectSet.getCount();
      for (%i = 0; %i < %count; %i++)
      {
         %object = $persistentObjectSet.getObject(%i);
         %scenegraph.removeFromScene(%object);
      }
   }
	
   %globalTileMap = %sceneGraph.getGlobalTileMap();
   if (isObject(%globalTileMap))
   {
      %sceneGraph.removeFromScene(%globalTileMap);
   }
   
   %scenegraph.clearScene(true);
   
   if (isObject(%globalTileMap))
   {
      %sceneGraph.addToScene(%globalTileMap);
   }
	
   $lastLoadedScene = "";
   
   //Delete old resources    -Mat
   if( isObject( $CurrentLevelResources ) ) 
   {
      eval( $CurrentLevelResources.UnloadFunction @ "(" @ $CurrentLevelResources @ ");" );   
      $CurrentLevelResources = 0;
      $instantResource = 0;
   }
}

function t2dSceneObject::onLevelLoaded(%this, %scenegraph)
{
}

function t2dSceneObject::onLevelEnded(%this, %scenegraph)
{
}


function t2dSceneGraph::onLevelLoaded(%this)
{
   //showCursor(); // RKS: Hide the cursor
}
