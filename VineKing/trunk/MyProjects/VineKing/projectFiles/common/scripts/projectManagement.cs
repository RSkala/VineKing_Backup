//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------
$currentProject = "";

function exitDemo()
{
   if($runWithEditors)
      toggleLevelEditor();
   else
      quit();
}

function _initializeProject() // RKS:  This is where datablocks and shit are set (for persistent objects, etc)
{
	//echo( "==================================================================================================" );
	//echo( "IN projectManagement.cs, _initializeProject()\n" );
	
   // Managed File Paths
   //we dont want datablocks loaded for iPhone, but we do want to have them in the editor - sven
   if($LevelEditorActive)
   {
      %dbFile = expandFilename("game/managed/datablocks.cs");
      %persistFile = expandFilename("game/managed/persistent.cs");
      %brushFile = expandFilename("game/managed/brushes.cs");
   }
	else
	{
		// RKS: I added this, so I could have a global datablocks file and persistent objects!
		//%dbFile		= expandFilename( "game/managed/datablocks.cs" );
		//%dbFile			= expandFilename( "game/scripts/Datablocks_Global.cs" );
		//%persistFile	= expandFilename( "game/managed/persistent.cs");
	}
   
   %behaviorsDirectory = expandFilename("game/scripts/behaviors");   
   %userDatablockFile = expandFilename("game/scripts/datablocks.cs");
   %userGUIProfileFile = expandFilename("game/scripts/guiProfiles.cs");
   
   //---------------------------------------------------------------------------   
   // Project Resources
   //---------------------------------------------------------------------------
   // --  This MUST be done BEFORE datablocks and persistent objects are loaded.
   %resPath = expandFileName("resources");

   // Index resources
   addResPath( %resPath );
   
   
   if( !isObject( $dependentResourceGroup ) )
      $dependentResourceGroup = new SimGroup();
   
   %resList = getDirectoryList( %resPath, 0 );
   %resCount = getFieldCount( %resList );

   for( %i = 0; %i < %resCount; %i++ )
   {
      %resName = getField( %resList, %i );
      %resFile = %resPath @ "/" @ %resName;
      
      %resObject = ResourceObject::load( %resFile );
      if( !isObject( %resObject ) )
      {
         error(" % Game Resources : FAILED Loading Resource" SPC %resName );
         continue;
      }
      else
         echo(" % Game Resources : Loaded Resource" SPC %resName);
         
      // Create a dependentResourcegroup object
      %entry = new ScriptObject() { Name = %resName; };
      $dependentResourceGroup.add( %entry );
   }

   //---------------------------------------------------------------------------
   // Managed Datablocks
   //---------------------------------------------------------------------------
	//echo( " -> Checking the persist shit\n" ); // RKS
	
   if ( isFile( %dbFile ) || isFile( %dbFile @ ".dso" ) )
   {
	   //echo( " -> %dbFile EXISTS: " @ %dbFile ); // RKS
      exec( %dbFile );
   }
	//else
	//{
	//	//echo( " -> %dbFile DOES NOT EXIST: " @ %dbFile ); // RKS
	//}
     
   if( !isObject( $managedDatablockSet ) )   
      $managedDatablockSet = new SimSet();

   //---------------------------------------------------------------------------
   // Managed Persistent Objects
   //---------------------------------------------------------------------------
	
	//echo( " -> Checking the persist shit\n" ); // RKS
	
   if ( isFile( %persistFile ) || isFile( %persistFile @ ".dso" ) )
   {
	   //echo( "-> %persistFile EXISTS -- executing: " @ %persistFile ); // RKS
      exec( %persistFile );
   }
	//else
	//{
	//	echo( "-> %persistFile DOES NOT EXIST: " @ %persistFile ); // RKS
	//}

   if( !isObject( $persistentObjectSet ) )   
      $persistentObjectSet = new SimSet();
      
   //---------------------------------------------------------------------------
   // Managed Brushes
   //---------------------------------------------------------------------------
   if ( isFile( %brushFile ) || isFile( %brushFile @ ".dso" ) )
      exec( %brushFile );
   
   if( !isObject( $brushSet ) )   
      $brushSet = new SimSet();

   //---------------------------------------------------------------------------
   // User Defined Datablocks
   //---------------------------------------------------------------------------
   addResPath( %userDatablockFile );   
   if( isFile( %userDatablockFile ) )
      exec( %userDatablockFile );
      
   //---------------------------------------------------------------------------
   // Behaviors
   //---------------------------------------------------------------------------
   addResPath(%behaviorsDirectory);
   
   // Compile all the cs files.
   %behaviorsSpec = %behaviorsDirectory @ "/*.cs";
   for (%file = findFirstFile(%behaviorsSpec); %file !$= ""; %file = findNextFile(%behaviorsSpec))
      compile(%file);
   
   // And exec all the dsos.
   %behaviorsSpec = %behaviorsDirectory @ "/*.cs.dso";
   for (%file = findFirstFile(%behaviorsSpec); %file !$= ""; %file = findNextFile(%behaviorsSpec))
      exec(strreplace(%file, ".cs.dso", ".cs"));

   //---------------------------------------------------------------------------
   // User Defined GUI Profiles
   //---------------------------------------------------------------------------
   addResPath( %userGUIProfileFile );   
   if( isFile( %userGUIProfileFile ) )
      exec( %userGUIProfileFile );
}
