//==============================================================================================================================================================
// THIS FILE CONTAINS ALL COMPONENTS USED IN iTGB 
// This file containts all components used in iTGB. iTGB is more efficient when there are less script files
//==============================================================================================================================================================

//==============================================================================================================================================================
// NOTES
// Possible values for 'addBehaviorField'
//	%template.addBehaviorField( DefaultTest,	"",		default );
//	%template.addBehaviorField( intTest,		"",		int		);
//	%template.addBehaviorField( floatTest,		"",		float	);
//	%template.addBehaviorField( Point2FTest,	"",		Point2F );
//	%template.addBehaviorField( boolTest,		"",		bool	);
//	%template.addBehaviorField( keybindTest,	"",		keybind );
//	%template.addBehaviorField( colorTest,		"",		color	);
//	%template.addBehaviorField( enumTest,		"",		enum	);
//==============================================================================================================================================================


//==============================================================================================================================================================
// ComponentAchievementBanner

if( isObject( ComponentAchievementBanner ) == false )
{
	%template = new BehaviorTemplate( ComponentAchievementBanner );
	
	%template.friendlyName = "Achievement Banner";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to the Achievement Banner";
}

//--------------------------------------------------------

function ComponentAchievementBanner::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentAchievementBanner(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentAchievementBanner::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentBossMountain

%linkedSmokePositions = "-" TAB "01" TAB "02" TAB "03" TAB "04" TAB "05" TAB "06" TAB "07" TAB "08" TAB "09" TAB "10";

if( isObject( ComponentBossMountain ) == false )
{
	%template = new BehaviorTemplate( ComponentBossMountain );
	
	%template.friendlyName = "Boss Mountain";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to assign a Boss";
	
	%template.addBehaviorField( LinkedSmokeIndex_01, "Smoke position disabled when this Boss dies", enum, "-", %linkedSmokePositions );
	%template.addBehaviorField( LinkedSmokeIndex_02, "Smoke position disabled when this Boss dies", enum, "-", %linkedSmokePositions );
	%template.addBehaviorField( LinkedSmokeIndex_03, "Smoke position disabled when this Boss dies", enum, "-", %linkedSmokePositions );
}

//--------------------------------------------------------

function ComponentBossMountain::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentBossMountain(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentBossMountain::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemyProjectileBrickAttack

if( isObject( ComponentEnemyProjectileBrickAttack ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemyProjectileBrickAttack );
	
	%template.friendlyName = "Brick Attack Projectile";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to the t2dAnimatedSprite that will be used as the projectile attack template for attacking bricks";
}

//--------------------------------------------------------

function ComponentEnemyProjectileBrickAttack::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentEnemyProjectileBrickAttack(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemyProjectileBrickAttack::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentDoubleBrick

if( isObject( ComponentDoubleBrick ) == false )
{
	%template = new BehaviorTemplate( ComponentDoubleBrick );
	
	%template.friendlyName = "Double Brick";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as a Double Brick";
}

//--------------------------------------------------------

function ComponentDoubleBrick::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentDoubleBrick(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentDoubleBrick::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEndLevelScreen

if( isObject( ComponentEndLevelScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentEndLevelScreen );
	
	%template.friendlyName = "End Level Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to an object that handles End Level situatons";
}

//--------------------------------------------------------

function ComponentEndLevelScreen::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentEndLevelScreen(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEndLevelScreenElement

%endLevelScreenElementTypes = "YOU_WIN" TAB "GAMEOVER" TAB "OK_BUTTON";

if( isObject( ComponentEndLevelScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentEndLevelScreenElement );
	
	%template.friendlyName = "End Level Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to an object will be a used as an End Level Screen Element";
	
	%template.addBehaviorField( ElementType, "End Level Screen Element Type", enum, "", %endLevelScreenElementTypes );
}

//--------------------------------------------------------

function ComponentEndLevelScreenElement::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentEndLevelScreenElement(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemyBlob

//%attackVulnerabilityTypes   = "Tap" TAB "Swipe" TAB "Tap Then Swipe" TAB "Tap Then Shake";

if( isObject( ComponentEnemyBlob ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemyBlob );
	
	%template.friendlyName = "Enemy Blob";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to an Enemy Blob";
}

//--------------------------------------------------------

function ComponentEnemyBlob::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemyBlob(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemyBlob::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemySerpent

if( isObject( ComponentEnemySerpent ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemySerpent );
	
	%template.friendlyName = "Enemy Serpent";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to an Enemy Serpent";
}

//--------------------------------------------------------

function ComponentEnemySerpent::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemySerpent(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemySerpent::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemySmoke

if( isObject( ComponentEnemySmoke ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemySmoke );
	
	%template.friendlyName = "Enemy Smoke";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to an Enemy Smoke Monster";
}

//--------------------------------------------------------

function ComponentEnemySmoke::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemySmoke(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemySmoke::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemySmokePositionList

if( isObject( ComponentEnemySmokePositionList ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemySmokePositionList );
	
	%template.friendlyName = "Enemy Smoke Position List";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to create a list of Start and End Positions for Enemy Smoke Monsters";
	
	%template.addBehaviorField( StartPos_01, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_02, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_03, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_04, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_05, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_06, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_07, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_08, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_09, "Smoke Start Point", Point2F );
	%template.addBehaviorField( StartPos_10, "Smoke Start Point", Point2F );
	
	%template.addBehaviorField( EndPos_01, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_02, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_03, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_04, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_05, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_06, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_07, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_08, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_09, "Smoke End Point", Point2F );
	%template.addBehaviorField( EndPos_10, "Smoke End Point", Point2F );
}

//--------------------------------------------------------

function ComponentEnemySmokePositionList::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemySmokePositionList(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemySmokePositionList::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemySpawnController

%timeSlotNames =	"-" TAB 
					"TimeSlot01" TAB "TimeSlot02" TAB "TimeSlot03" TAB "TimeSlot04" TAB "TimeSlot05" TAB "TimeSlot06" TAB "TimeSlot07" TAB "TimeSlot08" TAB
					"TimeSlot09" TAB "TimeSlot10" TAB "TimeSlot11" TAB "TimeSlot12" TAB "TimeSlot13" TAB "TimeSlot14" TAB "TimeSlot15" TAB "TimeSlot16";

%loopBehavior = "LOOP TO START" TAB "LOOP LAST TIME SLOT";

if( isObject( ComponentEnemySpawnController ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemySpawnController );
	
	%template.friendlyName = "Enemy Spawn Controller";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to create an Enemy Spawn Controller";
	
	%template.addBehaviorField( LoopBehavior, "Spawn Controller loop point", enum, "LOOP TO START", %loopBehavior );
	
	%template.addBehaviorField( UseTimeSlot01,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot02,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot03,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot04,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot05,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot06,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot07,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot08,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot09,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot10,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot11,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot12,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot13,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot14,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot15,	"Use Time Slot?", bool, false );
	%template.addBehaviorField( UseTimeSlot16,	"Use Time Slot?", bool, false );
}

//--------------------------------------------------------

function ComponentEnemySpawnController::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemySpawnController(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemySpawnController::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemySpawnPosition

//%spawnedEnemyTypes = "-" TAB "BLOB" TAB "SERPENT";

if( isObject( ComponentEnemySpawnPosition ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemySpawnPosition );
	
	%template.friendlyName = "Enemy Spawn Position";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to create a Spawn Point for Serpents";
	
	//%template.addBehaviorField( EnemySpawnType, "Type of Enemy Spawner (Blob or Serpent)", enum, "-", %spawnedEnemyTypes );
}

//--------------------------------------------------------

function ComponentEnemySpawnPosition::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemySpawnPosition(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemySpawnPosition::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentEnemySpawnTimeSlot

%enemyTypes = "-" TAB "BLOB" TAB "SUPER BLOB" TAB "SERPENT" TAB "SUPER SERPENT" TAB "SMOKE" TAB "SUPER SMOKE";

%smokePositionIndex = "-" TAB "ITERATE" TAB "01" TAB "02" TAB "03" TAB "04" TAB "05" TAB "06" TAB "07" TAB "08" TAB "09" TAB "10";

if( isObject( ComponentEnemySpawnTimeSlot ) == false )
{
	%template = new BehaviorTemplate( ComponentEnemySpawnTimeSlot );
	
	%template.friendlyName = "Enemy Spawn Time Slot";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to create an Enemy Spawn Time Slot";
	
	%template.addBehaviorField( StartTime,		"Start Time",         float, 0.0 );
	
	// Entry 01
	%template.addBehaviorField( UseEntry_01,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_01,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_01,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_01,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_01,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_01,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_01,	"Smoke Positions (SMOKE ONLY)",	enum,	"-", %smokePositionIndex );
	
	// Entry 02
	%template.addBehaviorField( UseEntry_02,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_02,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_02,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_02,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_02,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_02,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_02,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 03
	%template.addBehaviorField( UseEntry_03,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_03,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_03,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_03,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_03,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_03,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_03,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 04
	%template.addBehaviorField( UseEntry_04,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_04,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_04,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_04,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_04,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_04,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_04,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 05
	%template.addBehaviorField( UseEntry_05,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_05,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_05,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_05,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_05,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_05,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_05,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 06
	%template.addBehaviorField( UseEntry_06,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_06,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_06,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_06,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_06,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_06,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_06,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 07
	%template.addBehaviorField( UseEntry_07,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_07,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_07,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_07,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_07,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_07,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_07,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 08
	%template.addBehaviorField( UseEntry_08,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_08,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_08,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_08,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_08,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_08,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_08,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 09
	%template.addBehaviorField( UseEntry_09,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_09,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_09,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_09,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_09,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_09,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_09,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 10
	%template.addBehaviorField( UseEntry_10,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_10,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_10,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_10,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_10,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_10,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_10,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 11
	%template.addBehaviorField( UseEntry_11,		"Use Entry",					bool,	false );
 	%template.addBehaviorField( EnemyType_11,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_11,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_11,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_11,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_11,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_11,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
	
	// Entry 12
	%template.addBehaviorField( UseEntry_12,		"Use Entry",					bool,	false );
	%template.addBehaviorField( EnemyType_12,		"Enemy Type",					enum,	"-", %enemyTypes );
	%template.addBehaviorField( NumEnemies_12,		"Number of Enemies",			int,	0 );
	%template.addBehaviorField( StaggerTime_12,		"Stagger Time",					float,	0.0 );
	%template.addBehaviorField( Path_12,			"Path (BLOB ONLY)",             object, "", t2dSceneObject );
	%template.addBehaviorField( SpawnPoint_12,		"Spawn Point (SERPENT ONLY)",	object, "", t2dSceneObject );
	%template.addBehaviorField( SmokePosIndex_12,	"Smoke Positions (SMOKE ONLY)", enum,	"-", %smokePositionIndex );
}

//--------------------------------------------------------

function ComponentEnemySpawnTimeSlot::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentEnemySpawnTimeSlot(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentEnemySpawnTimeSlot::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentHealthBar

if (!isObject(ComponentHealthBar))
{
	%template = new BehaviorTemplate(ComponentHealthBar);
	
	%template.friendlyName = "Health Bar";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to an object that will be used as the Health Bar";
	
	%template.addBehaviorField( FrameLeft,		"Left part of the Frame",	object, "", t2dSceneObject );
	%template.addBehaviorField( FrameCenter,	"Center part of the Frame",	object, "", t2dSceneObject );
	%template.addBehaviorField( FrameRight,		"Bottom part of the Frame",	object, "", t2dSceneObject );
}

//--------------------------------------------------------

function ComponentHealthBar::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentHealthBar(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}


//==============================================================================================================================================================
// ComponentHeartScreen

if( isObject( ComponentHeartScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentHeartScreen );
	
	%template.friendlyName = "Heart Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as the Heart Screen";
}

//--------------------------------------------------------

function ComponentHeartScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentHeartScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentHeartScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentHeartScreenElement

if( isObject( ComponentHeartScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentHeartScreenElement );
	
	%template.friendlyName = "Heart Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as the Heart Screen Element";
}

//--------------------------------------------------------

function ComponentHeartScreenElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentHeartScreenElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentHeartScreenElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentInGameTutorialObject

//%tutorialObjectTypes = "TOP" TAB "BOTTOM" TAB "NEXT_BUTTON";
//%tutorialObjectTypes = "IMAGE" TAB "BUTTON";

if( isObject( ComponentInGameTutorialObject ) == false )
{
	%template = new BehaviorTemplate( ComponentInGameTutorialObject );
	
	%template.friendlyName = "In Game Tutorial Object";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a In-Game Tutorial object";
	
	//%template.addBehaviorField( TutObjType,	"The type of tutorial object", enum, "", %tutorialObjectTypes );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentInGameTutorialObject::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentInGameTutorialObject(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentInGameTutorialObject::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLandBrick

%grassFrameNumbers = "0" TAB "1" TAB "2" TAB "3" TAB "4" TAB "5" TAB "6" TAB "7" TAB "8";

if( isObject( ComponentLandBrick ) == false )
{
	%template = new BehaviorTemplate( ComponentLandBrick );
	
	%template.friendlyName = "Land Brick";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as a Land Brick";
	
	//%template.addBehaviorField( GrassFrameNumber,	"The frame # that corresponds to the grass image map that will be used for this brick",	enum, "0", %grassFrameNumbers );
	
	%template.addBehaviorField( DangerBrick,		"Whether this brick is a Danger Brick (Causes damage to player)",						bool );
	%template.addBehaviorField( Unbreakable,		"Whether this brick is unbreakable",													bool );
}

//--------------------------------------------------------

function ComponentLandBrick::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentLandBrick(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentLandBrick::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLevelSelectButton

%levelNumbers		= "01" TAB "02" TAB "03" TAB "04" TAB "05" TAB "06" TAB "07" TAB "08" TAB "09" TAB "10" TAB "11" TAB "Boss";
%subLevelNumbers	= "01" TAB "02" TAB "03" TAB "04" TAB "05" TAB "06" TAB "07" TAB "08" TAB "09" TAB "10" TAB "11" TAB 
                      "12" TAB "13" TAB "14" TAB "15" TAB "16" TAB "17" TAB "18" TAB "19" TAB "20" TAB "21";


if (!isObject(ComponentLevelSelectButton))
{
	%template = new BehaviorTemplate(ComponentLevelSelectButton);
	
	%template.friendlyName = "Level Select Button";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to a t2dStaticSprite. When touched, it will load a level.";
	
	%template.addBehaviorField( LevelNumber,		"The level number",		enum, "",	%levelNumbers		);
	%template.addBehaviorField( SubLevelNumber,		"The sub-level number",	enum, "",	%subLevelNumbers	);
	%template.addBehaviorField( BossLevel,			"Whether this level is a boss level", bool );
	%template.addBehaviorField( LevelName,			"Direct level name to load. NOTE: This will take precedence over the Level Numbers.", string );
	%template.addBehaviorField( BackButton,			"Whether this is a Back Button (will not load a level)", bool );
	%template.addBehaviorField( RPGButton,			"Whether this is an RPG Button (will not load a level)", bool );
	%template.addBehaviorField( ExitToMainButton,	"Whether this is the 'Return to Main Menu' button (will not load a level)", bool );
	%template.addBehaviorField( MapHelpButton,		"Map Help Button?", bool );
}

//--------------------------------------------------------

function ComponentLevelSelectButton::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentLevelSelectButton(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLevelBeginSequence

if( isObject( ComponentLevelBeginSequence ) == false )
{
	%template = new BehaviorTemplate( ComponentLevelBeginSequence );
	
	%template.friendlyName = "Level Begin Sequence";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to the object that displays the 'Ready Set Go' images";
}

//--------------------------------------------------------

function ComponentLevelBeginSequence::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentLevelBeginSequence(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLevelUpScreen

if( isObject( ComponentLevelUpScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentLevelUpScreen );
	
	%template.friendlyName = "Level Up Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to instantiate a Level Up screen";
}

//--------------------------------------------------------

function ComponentLevelUpScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentLevelUpScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentLevelUpScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLevelUpScreenElement

//%levelUpScreenElementTypes = "BACK_BUTTON" TAB "BACKGROUND" TAB "HEALTH_BAR" TAB "MANA_BAR" TAB "SPEED_BAR" TAB "TOTAL_XP_BG" TAB "TOTAL_XP_TEXT" TAB "HEALTH_BUTTON" TAB
//							 "MANA_BUTTON" TAB "SPEED_BUTTON" TAB "HEALTH_BAR_POSITION_MARKER" TAB "SPEED_BAR_POSITION_MARKER" TAB "MANA_BAR_POSITION_MARKER";

if( isObject( ComponentLevelUpScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentLevelUpScreenElement );
	
	%template.friendlyName = "Level Up Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject a Level Up screen element";
	
	//%template.addBehaviorField( ElementType, "Level Up screen Element Type", enum, "BACK_BUTTON", %levelUpScreenElementTypes );
}

//--------------------------------------------------------

function ComponentLevelUpScreenElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentLevelUpScreenElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentLevelUpScreenElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLineDrawAttack

if( isObject( ComponentLineDrawAttack ) == false )
{
	%template = new BehaviorTemplate( ComponentLineDrawAttack );
	
	%template.friendlyName = "Line Draw Attack";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will handle the Line Draw Attack";
	
	%template.addBehaviorField( LineObject,			"The t2dSceneObject that is used to create lines",					object, "", t2dSceneObject	);
	%template.addBehaviorField( ConnectionObject,	"The t2dSceneObject that is used to connect the lines",			object, "", t2dSceneObject	);
	%template.addBehaviorField( PathObject,			"The t2dPath that is used to draw to Catmull-Rom spline",			object, "", t2dPath			);
	%template.addBehaviorField( TestObject,			"The t2dSceneObject that will be used to test path traversal",	object, "", t2dSceneObject	);
}

//--------------------------------------------------------

function ComponentLineDrawAttack::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentLineDrawAttack(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		error( "ComponentLineDrawAttack::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLineDrawAttackObject

if( isObject( ComponentLineDrawAttackObject ) == false )
{
	%template = new BehaviorTemplate( ComponentLineDrawAttackObject );
	
	%template.friendlyName = "Line Draw Attack Object";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will traverse the drawn path to attack the tiles";
}

//--------------------------------------------------------

function ComponentLineDrawAttackObject::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentLineDrawAttackObject(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		error( "ComponentLineDrawAttackObject::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentLogoScreen

if( isObject( ComponentLogoScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentLogoScreen );
	
	%template.friendlyName = "Logo Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to assign the Logo Screen";
}

//--------------------------------------------------------

function ComponentLogoScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentLogoScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentLogoScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMainMenuScreen
//
if( isObject( ComponentMainMenuScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentMainMenuScreen );
	
	%template.friendlyName = "Main Menu Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to instantiate a Main Menu Screen";
}

//--------------------------------------------------------

function ComponentMainMenuScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMainMenuScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMainMenuScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMainMenuScreenElement

%mainMenuScreenElementTypes = 
	"NONE"					TAB 
	"MAINMENUSCREEN_BG"		TAB
	"OPTIONSSCREEN_BG"		TAB
	"CREDITSSCREEN_BG"		TAB
	"CLEARDATASCREEN_BG"	TAB
	"NEWGAME_TEXT"			TAB
	"CONTINUE_TEXT"			TAB
	"TUTORIAL_TEXT"			TAB
	"OPTIONS_TEXT"			TAB
	"CLEARDATA_TEXT"		TAB
	"CREDITS_TEXT"			TAB
	"CLEARDATA_YES_TEXT"	TAB
	"CLEARDATA_NO_TEXT"		TAB
	"MUTE_BUTTON"			TAB
	"BACK_BUTTON";

if( isObject( ComponentMainMenuScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentMainMenuScreenElement );
	
	%template.friendlyName = "Main Menu Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to use as a Main Menu Screen Element";
	
	%template.addBehaviorField( ElementType, "Main Menu screen Element Type", enum, "NONE", %mainMenuScreenElementTypes );
}

//--------------------------------------------------------

function ComponentMainMenuScreenElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMainMenuScreenElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMainMenuScreenElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentManaBar

if (!isObject(ComponentManaBar))
{
	%template = new BehaviorTemplate(ComponentManaBar);
	
	%template.friendlyName = "Mana Bar";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to an object that will be used as the Mana Bar";
	
	%template.addBehaviorField( FrameTop,		"Top part of the Frame",	object, "", t2dSceneObject );
	%template.addBehaviorField( FrameCenter,	"Center part of the Frame",	object, "", t2dSceneObject );
	%template.addBehaviorField( FrameBottom,	"Bottom part of the Frame",	object, "", t2dSceneObject );
}

//--------------------------------------------------------

function ComponentManaBar::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentManaBar(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentManaSeedItemDrop

if (!isObject(ComponentManaSeedItemDrop))
{
	%template = new BehaviorTemplate(ComponentManaSeedItemDrop);
	
	%template.friendlyName = "Mana Seed Item Drop";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to an object that will be duplicated into Mana Seed Item Drops";
	
	%template.addBehaviorField( GrassDropAnim, "The t2dAnimationDatablock played when this seed is dropped from a fully grown grass tile", object, "", t2dAnimationDatablock );
	%template.addBehaviorField( GrassDropManaValue, "Amount of mana units a seed gives when dropped from grass", int, 2 );
	
	%template.addBehaviorField( EnemyDropAnim, "The t2dAnimationDatablock played when this seed is dropped from a killed enemy", object, "", t2dAnimationDatablock );
	%template.addBehaviorField( EnemyDropManaValue, "Amount of mana units a seed gives when dropped from an enemy", int, 3 );
}

//--------------------------------------------------------

function ComponentManaSeedItemDrop::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentManaSeedItemDrop(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMapSelectMenu

if( isObject( ComponentMapSelectMenu ) == false )
{
	%template = new BehaviorTemplate( ComponentMapSelectMenu );
	
	%template.friendlyName = "Map Select Menu";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as a Map Select Menu";
	
	%template.addBehaviorField( GrayBox,	"The gray box that is used to dim the background",	object, "", t2dSceneObject );
	%template.addBehaviorField( BackButton, "The button used to back out of a map",				object, "", t2dSceneObject );
	%template.addBehaviorField( RPGButton,	"The button used to open the RPG level up screen",	object, "", t2dSceneObject );
}

//--------------------------------------------------------

function ComponentMapSelectMenu::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMapSelectMenu(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMapSelectMenu::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMapSelectButton

if( isObject( ComponentMapSelectButton ) == false )
{
	%template = new BehaviorTemplate( ComponentMapSelectButton );
	
	%template.friendlyName = "Map Select Button";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as a Map Select Button";
	
	%template.addBehaviorField( Map, "The map object that is opened from this button", object, "", t2dSceneObject );
}

//--------------------------------------------------------

function ComponentMapSelectButton::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMapSelectButton(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMapSelectButton::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMapSelection (a map that contains a group of buttons)

if( isObject( ComponentMapSelection ) == false )
{
	%template = new BehaviorTemplate( ComponentMapSelection );
	
	%template.friendlyName = "Map Selection Object";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as a Map Selection (group of level select buttons)";
	
	%template.addBehaviorField( LevelButton01, "Level Select Button 01", object, "", t2dSceneObject );
	%template.addBehaviorField( LevelButton02, "Level Select Button 02", object, "", t2dSceneObject );
	%template.addBehaviorField( LevelButton03, "Level Select Button 03", object, "", t2dSceneObject );
	%template.addBehaviorField( LevelButton04, "Level Select Button 04", object, "", t2dSceneObject );
}

//--------------------------------------------------------

function ComponentMapSelection::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMapSelection(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMapSelection::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMyScoreScreen

if( isObject( ComponentMyScoreScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentMyScoreScreen );
	
	%template.friendlyName = "My Score Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as the MyScore Screen";
}

//--------------------------------------------------------

function ComponentMyScoreScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMyScoreScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMyScoreScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentMyScoreScreenElement

if( isObject( ComponentMyScoreScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentMyScoreScreenElement );
	
	%template.friendlyName = "My Score Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be used as an element of the MyScore screen";
}

//--------------------------------------------------------

function ComponentMyScoreScreenElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentMyScoreScreenElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentMyScoreScreenElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// PathGridHandlerComp

if( isObject( PathGridHandlerComp ) == false )
{
	%template = new BehaviorTemplate( PathGridHandlerComp );
	
	%template.friendlyName = "Path Grid Handler";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Path Grid Handler Component";
}

//--------------------------------------------------------

function PathGridHandlerComp::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentPathGridHandler(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		error( "PathGridHandlerComp::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentPauseScreen

if( isObject( ComponentPauseScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentPauseScreen );
	
	%template.friendlyName = "Pause Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to instantiate a Pause Screen";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentPauseScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentPauseScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentPauseScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentPauseScreenElement

//%pauseScreenElementTypes = "PAUSE_BUTTON" TAB "PAUSE_TEXT" TAB "RESUME_TEXT" TAB "EXIT_TEXT";

%pauseScreenElementTypes = "PAUSE_MENU_IMAGE" TAB "CONFIRM_QUIT_MENU_IMAGE" TAB "PAUSE_BUTTON" TAB "RESUME_BUTTON" TAB "MUTE_BUTTON" TAB "QUIT_BUTTON" 
							TAB "BUTTON_PRESSED_IMAGE" TAB "MUTE_ONOFF_IMAGE" TAB "CONFIRM_QUIT_BUTTON_YES" TAB "CONFIRM_QUIT_BUTTON_NO"
							TAB "RESUME_PRESSED_IMAGE" TAB "QUIT_PRESSED_IMAGE" TAB "CONFIRM_YES_PRESSED_IMAGE" TAB "CONFIRM_NO_PRESSED_IMAGE"
							TAB "RESUME_TEXT" TAB "MUTE_TEXT" TAB "QUIT_TEXT" TAB "QUIT_CONFIRM_TEXT_1" TAB "QUIT_CONFIRM_TEXT_2"
							TAB "QUIT_CONFIRM_YES_TEXT" TAB "QUIT_CONFIRM_NO_TEXT" TAB

							// NEW SHIT
							"RESUME_BUTTON_NEW"				TAB
							"MAP_BUTTON_NEW"				TAB
							"MUTE_BUTTON_NEW"				TAB
							"CONFIRM_QUIT_BG_NEW"			TAB
							"CONFIRM_QUIT_BUTTON_YES_NEW"	TAB
							"CONFIRM_QUIT_BUTTON_NO_NEW";

if( isObject( ComponentPauseScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentPauseScreenElement );
	
	%template.friendlyName = "Pause Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to use as a Pause Screen Element";
	
	%template.addBehaviorField( ElementType, "Level Up screen Element Type", enum, "", %pauseScreenElementTypes );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentPauseScreenElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentPauseScreenElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentPauseScreenElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// PlayerHomeBaseComponent

if( isObject( PlayerHomeBaseComponent ) == false )
{
	%template = new BehaviorTemplate( PlayerHomeBaseComponent );
	
	%template.friendlyName = "Player Home Base Component";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this Component to a t2dSceneObject to use it to place the player's home base";
	
	%template.addBehaviorField( IdleLoopAnim,		"The t2dAnimationDatablock that will be used as the Idle Loop Animation",	object, "", t2dAnimationDatablock );
	%template.addBehaviorField( GrowingStartAnim,	"The t2dAnimationDatablock that will be used as the Grow Start Animation",	object, "", t2dAnimationDatablock );
	%template.addBehaviorField( GrowingLoopAnim,	"The t2dAnimationDatablock that will be used as the Grow Loop Animation",	object, "", t2dAnimationDatablock );
	%template.addBehaviorField( MoveLoopAnim,		"The t2dAnimationDatablock that will be used as the Move Loop Animation",	object, "", t2dAnimationDatablock );
	%template.addBehaviorField( EatAnim,			"The t2dAnimationDatablock that will be used as the Eat Animation",			object, "", t2dAnimationDatablock );
	%template.addBehaviorField( WinAnim,			"The t2dAnimationDatablock that will be used as the Win Animation",			object, "", t2dAnimationDatablock );
	%template.addBehaviorField( DeadAnim,			"The t2dAnimationDatablock that will be used as the Death Animation",		object, "", t2dAnimationDatablock );
	%template.addBehaviorField( HitAnim,			"The t2dAnimationDatablock that will be used when hit by an enemy",			object, "", t2dAnimationDatablock );
	
	%template.addBehaviorField( VineObjectA, "The object that will be used as the vine attack", object, "", t2dSceneObject );
	%template.addBehaviorField( VineObjectB, "The object that will be used as the vine attack", object, "", t2dSceneObject );
	%template.addBehaviorField( VineObjectC, "The object that will be used as the vine attack", object, "", t2dSceneObject );
}

//--------------------------------------------------------

function PlayerHomeBaseComponent::onBehaviorAdd( %this )
{	
	// Create the Component in C++
	%this.Component = new CComponentPlayerHomeBase(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentScoreScreenFacebookButton

if( isObject( ComponentScoreScreenFacebookButton ) == false )
{
	%template = new BehaviorTemplate( ComponentScoreScreenFacebookButton );
	
	%template.friendlyName = "Score Screen Facebook Button";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to the object that will be the Facebook button";
}

//------------------------------------------------------------------------------------------------

function ComponentScoreScreenFacebookButton::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentScoreScreenFacebookButton(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentScoringManager

if( isObject( ComponentScoringManager ) == false )
{
	%template = new BehaviorTemplate( ComponentScoringManager );
	
	%template.friendlyName = "Scoring Manager";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to instantiate a CComponentScoringManager";
	
	%template.addBehaviorField( ScoreScreenBG,			"ScoreScreenBG",			object, "", t2dSceneObject );
	
	%template.addBehaviorField( NumKilledA,				"NumKilledA",				object, "", t2dTextObject );
	%template.addBehaviorField( ScoreEnemyA,			"ScoreEnemyA",				object, "", t2dTextObject );
	
	%template.addBehaviorField( NumKilledB,				"NumKilledB",				object, "", t2dTextObject );
	%template.addBehaviorField( ScoreEnemyB,			"ScoreEnemyB",				object, "", t2dTextObject );
	
	
	%template.addBehaviorField( NumKilledSmoke,			"NumKilledSmoke",			object, "", t2dTextObject );
	%template.addBehaviorField( ScoreSmokeMonster,		"ScoreSmokeMonster",		object, "", t2dTextObject );
	
	%template.addBehaviorField( NumSeedsEaten,			"NumSeedsEaten",			object, "", t2dTextObject );
	%template.addBehaviorField( ScoreSeedsEaten,		"ScoreSeedsEaten",			object, "", t2dTextObject );
	
	
	%template.addBehaviorField( NumGrassGrown,			"NumGrassGrown",			object, "", t2dTextObject );
	%template.addBehaviorField( ScoreGrassTilesGrown,	"ScoreGrassTilesGrown",		object, "", t2dTextObject );
	
	%template.addBehaviorField( NumDestroyedCrystals,	"NumDestroyedCrystals",		object, "", t2dTextObject );
	%template.addBehaviorField( ScoreCrystalsDestroyed,	"ScoreCrystalsDestroyed",	object, "", t2dTextObject );
	
	%template.addBehaviorField( HealthRemaining,		"HealthRemaining",			object, "", t2dTextObject );
	%template.addBehaviorField( ScoreHealthRemaining,	"ScoreHealthRemaining",		object, "", t2dTextObject );
	
	%template.addBehaviorField( ManaRemaining,			"ManaRemaining",			object, "", t2dTextObject );
	%template.addBehaviorField( ScoreManaRemaining,		"ScoreManaRemaining",		object, "", t2dTextObject );
	
	//%template.addBehaviorField( ScoreTotalLabel,		"ScoreTotalLabel",			object, "", t2dSceneObject );
	%template.addBehaviorField( ScoreTotal,				"ScoreTotal",				object, "", t2dTextObject );
	
	//%template.addBehaviorField( XPLabel,				"XPLabel",					object, "", t2dTextObject );
	//%template.addBehaviorField( XPTotal,				"XPTotal",					object, "", t2dTextObject );
	
	//%template.addBehaviorField( ScoreScreenBlackBox,	"ScoreScreenBlackBox",		object, "", t2dSceneObject );
	//%template.addBehaviorField( ScoreScreenItemImage,	"ScoreScreenItemImage",		object, "", t2dSceneObject );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentScoringManager::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentScoringManager(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentScoringManager::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentSeedMagnetButton

if( isObject( ComponentSeedMagnetButton ) == false )
{
	%template = new BehaviorTemplate( ComponentSeedMagnetButton );
	
	%template.friendlyName = "Seed Magnet Button";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to the object that will be a Seed Magnet";
}

//------------------------------------------------------------------------------------------------

function ComponentSeedMagnetButton::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentSeedMagnetButton(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentStaticPath

%staticPathNodes =	"-" TAB
					"01_01" TAB "01_02" TAB "01_03" TAB "01_04" TAB "01_05" TAB "01_06" TAB "01_07" TAB "01_08" TAB
					"02_01" TAB "02_02" TAB "02_03" TAB "02_04" TAB "02_05" TAB "02_06" TAB "02_07" TAB "02_08" TAB
					"03_01" TAB "03_02" TAB "03_03" TAB "03_04" TAB "03_05" TAB "03_06" TAB "03_07" TAB "03_08" TAB
					"04_01" TAB "04_02" TAB "04_03" TAB "04_04" TAB "04_05" TAB "04_06" TAB "04_07" TAB "04_08" TAB
					"05_01" TAB "05_02" TAB "05_03" TAB "05_04" TAB "05_05" TAB "05_06" TAB "05_07" TAB "05_08" TAB
					"06_01" TAB "06_02" TAB "06_03" TAB "06_04" TAB "06_05" TAB "06_06" TAB "06_07" TAB "06_08" TAB
					"07_01" TAB "07_02" TAB "07_03" TAB "07_04" TAB "07_05" TAB "07_06" TAB "07_07" TAB "07_08" TAB
					"08_01" TAB "08_02" TAB "08_03" TAB "08_04" TAB "08_05" TAB "08_06" TAB "08_07" TAB "08_08" TAB
					"09_01" TAB "09_02" TAB "09_03" TAB "09_04" TAB "09_05" TAB "09_06" TAB "09_07" TAB "09_08" TAB
					"10_01" TAB "10_02" TAB "10_03" TAB "10_04" TAB "10_05" TAB "10_06" TAB "10_07" TAB "10_08" TAB
					"11_01" TAB "11_02" TAB "11_03" TAB "11_04" TAB "11_05" TAB "11_06" TAB "11_07" TAB "11_08" TAB
					"12_01" TAB "12_02" TAB "12_03" TAB "12_04" TAB "12_05" TAB "12_06" TAB "12_07" TAB "12_08" TAB
					"13_01" TAB "13_02" TAB "13_03" TAB "13_04" TAB "13_05" TAB "13_06" TAB "13_07" TAB "13_08";

if( isObject( ComponentStaticPath ) == false )
{
	%template = new BehaviorTemplate( ComponentStaticPath );
	
	%template.friendlyName = "Static Path";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject that will be a container of path nodes";
	
	// NEW WAY (No node objects)
	%template.addBehaviorField( Node01, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node02, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node03, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node04, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node05, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node06, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node07, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node08, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node09, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node10, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node11, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node12, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node13, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node14, "", enum, "-", %staticPathNodes );
	%template.addBehaviorField( Node15, "", enum, "-", %staticPathNodes );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentStaticPath::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentStaticPath(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "CComponentStaticPath::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentStoryEndingScreen

if( isObject( ComponentStoryEndingScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentStoryEndingScreen );
	
	%template.friendlyName = "Story Ending Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to the Achievement Banner";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentStoryEndingScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentStoryEndingScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentStoryEndingScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentStoryOpeningScreen

if( isObject( ComponentStoryOpeningScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentStoryOpeningScreen );
	
	%template.friendlyName = "Story Opening Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to assign the Story Opening Screen";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentStoryOpeningScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentStoryOpeningScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentStoryOpeningScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentStoryOpeningScreenElement

if( isObject( ComponentStoryOpeningScreenElement ) == false )
{
	%template = new BehaviorTemplate( ComponentStoryOpeningScreenElement );
	
	%template.friendlyName = "Story Opening Screen Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to assign a Story Opening Screen Element";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentStoryOpeningScreenElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentStoryOpeningScreenElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentStoryOpeningScreenElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentTutorialOpeningScreen

if( isObject( ComponentTutorialOpeningScreen ) == false )
{
	%template = new BehaviorTemplate( ComponentTutorialOpeningScreen );
	
	%template.friendlyName = "Tutorial Opening Screen";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to assign the Story Opening Screen";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentTutorialOpeningScreen::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentTutorialOpeningScreen(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentTutorialOpeningScreen::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// ComponentWinLevel

if (!isObject(ComponentWinLevel))
{
	%template = new BehaviorTemplate(ComponentWinLevel);
	
	%template.friendlyName = "Win Level";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Attach this component to an object that handles the 'Win Level sequence";
	
	%template.addBehaviorField( YouWinObject, "The t2dSceneObject that says 'YOU WIN'",		object, "", t2dSceneObject );
	%template.addBehaviorField( GameOverObject, "The t2dSceneObject that says 'GAME OVER'",  object, "", t2dSceneObject );
}

//------------------------------------------------------------------------------------------------

function ComponentWinLevel::onBehaviorAdd(%this)
{
	// Create the Component
	%this.Component = new CComponentWinLevel(); // This creates the component in C++, so the name MUST match the C++ class name.
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent(%this, %this.Component);
	
	// Add the component
	if (!%this.Owner.addComponents(%this.Component))
	{
		%this.Component.safeDelete();
		return;
	}
}

//------------------------------------------------------------------------------------------------

function ComponentWinLevel::OnWinLevel( %this ) 
{
	echo( "ComponentWinLevel::OnWinLevel" );
	
	//schedule( 2000, 0, "ComponentWinLevel_LoadLevel", "Screen_FrontEndTest" );
	
	//schedule( 2000, 0, "ComponentWinLevel_LoadLevel", "Screen_LevelSelectTest" );
	schedule( 2000, 0, "ComponentWinLevel_LoadLevel", "Screen_LevelSelectIsland" );
	
	//schedule( 2000, 0, "TakesDamageAdvBehavior_loadLevel", %this.nextLevel );
	//%this.schedule(%this.respawnTime * 1000, "spawn");
}

//------------------------------------------------------------------------------------------------

function ComponentWinLevel::PlayLevelWinMusic( %this ) 
{
	//$currentBGM01 = alxPlay( BackgroundMusic01 );
	//alxStop( $currentBGM01 );
	echo( "ComponentWinLevel::PlayLevelWinMusic" );
	
	if( alxIsPlaying( $currentBGM01 ) )
	{
		echo( "- Stopping current BGM" );
		alxStop( $currentBGM01 );
		$currentBGM01 = 0;
	}
	
	//$currentBGM01 = alxPlay( LevelWinMusic );
}

//------------------------------------------------------------------------------------------------

function ComponentWinLevel::PlayLevelLoseMusic( %this ) 
{
	//$currentBGM01 = alxPlay( BackgroundMusic01 );
	//alxStop( $currentBGM01 );
	echo( "ComponentWinLevel::PlayLevelLoseMusic" );
	
	if( alxIsPlaying( $currentBGM01 ) )
	{
		echo( "- Stopping current BGM" );
		alxStop( $currentBGM01 );
		$currentBGM01 = 0;
	}
	
	//$currentBGM01 = alxPlay( LevelLoseMusic );
}



//==============================================================================================================================================================
// 

if( isObject( ComponentTutorialLevel ) == false )
{
	%template = new BehaviorTemplate( ComponentTutorialLevel );
	
	%template.friendlyName = "Tutorial Level";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to create the Tutorial Level";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentTutorialLevel::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentTutorialLevel(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentTutorialLevel::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// 

if( isObject( ComponentTutorialLevelElement ) == false )
{
	%template = new BehaviorTemplate( ComponentTutorialLevelElement );
	
	%template.friendlyName = "Tutorial Level Element";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to add an element to the Tutorial Level";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentTutorialLevelElement::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentTutorialLevelElement(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentTutorialLevelElement::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}

//==============================================================================================================================================================
// 

if( isObject( ComponentTutorialPathHandler ) == false )
{
	%template = new BehaviorTemplate( ComponentTutorialPathHandler );
	
	%template.friendlyName = "Tutorial Path Handler";
	%template.behaviorType = "C++ Component Template";
	%template.description = "Add this component to a t2dSceneObject to create the Tutorial Path Handler";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

function ComponentTutorialPathHandler::onBehaviorAdd( %this )
{
	// Create the Component in C++
	%this.Component = new CComponentTutorialPathHandler(); // NOTE: Make sure this name matches the C++ class name, otherwise it will not be created
	
	// Copy all of the behavior information to the component
	copyBehaviorToComponent( %this, %this.Component );
	
	// Add the component
	if( %this.Owner.addComponents( %this.Component ) == false )
	{
		echo( "ComponentTutorialPathHandler::onBehaviorAdd - Failed to register component" );
		%this.Component.safeDelete();
		return;
	}
}
