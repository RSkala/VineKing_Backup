//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dSceneObject Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dSceneObject", "LBQESceneObject::CreateContent", "LBQESceneObject::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQESceneObject::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQESceneObjectClass", %quickEditObj);
   
   %sceneObjectRollout = %base.createRolloutStack("Scene Object", true);
   %sceneObjectRollout.createTextEdit2("PositionX", "PositionY", 3, "Position", "X", "Y", "Position", true);
   %sceneObjectRollout.createTextEdit2("Width", "Height", 3, "Size", "Width", "Height", "Size", true);
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dPath\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dShape3D\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %sceneObjectRollout.createHideableStack( %hiddenCheck );
   %hidden.createTextEdit ("rotation", 3, "Rotation", "Rotation", true);
   %hidden.createTextEdit ("autorotation", 3, "Auto Rotation", "Automatic Rotation");

   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dTileLayer\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dParticleEffect\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dTrigger\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dPath\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dShape3D\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %sceneObjectRollout.createHideableStack( %hiddenCheck );
   %hidden.createCheckBox ("FlipX", "Flip Horizontal", "Flip Horizontal", true);
   %hidden.createCheckBox ("FlipY", "Flip Vertical", "Flip Vertical", true);
   
   %sceneObjectRollout.createTextEdit2("SortPointX", "SortPointY", 3, "Sort Point", "X", "Y", "The layer draw order sorting point.", true);
   %sceneObjectRollout.createLeftRightEdit("Layer", "0;", "31;", 1, "Layer", "Rendering Layer");
   %sceneObjectRollout.createLeftRightEdit("GraphGroup", "0;", "31;", 1, "Group", "Graph Group");
   %sceneObjectRollout.createLeftRight("moveBackwardInLayer", "moveForwardInLayer", "Back/Forward", "Move the object within its layer.");
   %sceneObjectRollout.createCheckBox ("Visible", "Visible", "Visibility");
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dParticleEffect\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dTileLayer\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dPath\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %sceneObjectRollout.createHideableStack( %hiddenCheck );
   %hidden.createTextEdit ("Lifetime", 3, "Lifetime", "Lifetime");
   
   %alignRollout = %base.createRolloutStack( "Align", false );
   %alignRollout.createAlignTools( true );
   
   %scriptingRollout = %base.createRolloutStack("Scripting");
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dTileLayer\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %scriptingRollout.createHideableStack( %hiddenCheck );
   %hidden.createCheckBox("Persistent", "Persist", "Set This Object to Persist Across All Levels.");
   
   %scriptingRollout.createT2DDatablockList("ConfigDatablock", "Config Datablock", "t2dSceneObjectdatablock", "The Configuration Datablock to associate this sceneobject with");   
   %scriptingRollout.createTextEdit("Name", "TEXT", "Name", "Name the Object for Referencing in Script");
   %scriptingRollout.createTextEdit("ClassNamespace", "TEXT", "Class", "Link this Object to a Class");
   %scriptingRollout.createTextEdit("SuperClassNamespace", "TEXT", "Super Class", "Link this Object to a Parent Class");
   %scriptingRollout.createCheckBox("UseMouseEvents", "Use Mouse Events", "Enable mouse callbacks on the object.");
   
   %pathNodeCountCommand = %base @ ".object.getAttachedToPath().getNodeCount() - 1;";
   %pathedStack = %base.createHideableStack("!" @ %base @ ".object.getAttachedToPath();");
   %pathedRollout = %pathedStack.createRolloutStack("Pathing");
   %pathedRollout.createLeftRightEdit("PathStartNode", "0;", %pathNodeCountCommand, 1, "Start Node", "The Node the Object Starts At.");
   %pathedRollout.createLeftRightEdit("PathEndNode", "0;", %pathNodeCountCommand, 1, "End Node", "The Node the Object Ends At.");
   %pathedRollout.createTextEdit("PathSpeed", 3, "Speed", "The Speed at Which to Follow the Path.");
   %pathedRollout.createCheckBox("PathMoveForward", "Move Forward", "Uncheck to Change the Direction of the object.");
   %pathedRollout.createCheckBox("PathOrient", "Orient To Path", "Rotate the Object With the Direction it is Facing.");
   %pathedRollout.createTextEdit("PathRotationOffset", 3, "Rotation Offset", "The offset of the rotation when using orient to path.");
   %pathedRollout.createTextEdit("PathLoops", 0, "Loops", "The number of Loops Along the Path to Take.");
   %pathedRollout.createEnumList("PathFollowMode", false, "Follow Mode", "The Response of the Object Upon Reaching the End of the Path.", "t2dPath", "pathModeEnum");
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dPath\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %base.createHideableStack( %hiddenCheck );
   %collisionRollout = %hidden.createRolloutStack("Collision");
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dTileLayer\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %collisionRollout.createHideableStack( %hiddenCheck );
   %hidden.createCheckBox("collisionActiveSend", "Send Collision", "Send Collisions Into The Collision Detection System.");
   
   %collisionRollout.createCheckBox("collisionActiveReceive", "Receive Collision", "Receive Collisions From The Collision Detection System.");
   %collisionRollout.createCheckBox("collisionPhysicsSend", "Send Physics", "Use Physics When Sending Collisions.");
   %collisionRollout.createCheckBox("collisionPhysicsReceive", "Receive Physics", "Use Physics When Receiving Collisions.");
   %collisionRollout.createCheckBox("collisionCallback", "Callback", "Receive Script Notifications of Collision Events.");
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dTileLayer\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %collisionRollout.createHideableStack( %hiddenCheck );
   %collisionDectionList = %hidden.createEnumList("collisionDetection", false, "Detection Mode", "Change the Collision Detection Mode.", "t2dSceneObject", "collisionDetectionMode");
      
   %circleCollisionContainer = %collisionRollout.createHideableStack(%base @ ".object.getCollisionDetection() !$= \"CIRCLE\";");
   %circleCollisionContainer.addControlDependency(%collisionDectionList);
   %circleCollisionContainer.createTextEdit("collisionCircleScale", 3, "Circle Scale", "Scale of the Collision Circle.");
   %circleCollisionContainer.createCheckBox("collisionCircleSuperscribed", "Superscribe Ellipse", "Set the Collision Circle to Surround the Bounding Box.");
   
   %collisionRollout.createDropDownList("collisionResponse", "Collision Response", "", "CLAMP\tBOUNCE\tSTICKY\tKILL\tRIGID", "Change the collision response mode.");
   %collisionRollout.createMask        ("CollisionLayers", "Collision Layers", 0, 31, "Change the Layers of Objects With Which This Can Collide.");
   %collisionRollout.createMask        ("CollisionGroups", "Collision Groups", 0, 31, "Change the Groups of Objects With Which This Can Collide.");

   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dPath\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %base.createHideableStack( %hiddenCheck );
   %physicsRollout = %hidden.createRolloutStack("Physics");
   
   %physicsRollout.createCheckBox("UsesPhysics", "Uses Physics", "Enables/disables all physics for an object");
   %physicsRollout.createTextEdit2("linearVelocityX", "linearVelocityY", 3, "Velocity", "X", "Y", "Linear Velocity");
   %physicsRollout.createTextEdit2("minLinearVelocity", "maxLinearVelocity", 3, "", "Min", "Max", "Minimum Linear Velocity");
   %physicsRollout.createTextEdit ("AngularVelocity", 3, "Angular Velocity", "Angular Velocity");
   %physicsRollout.createTextEdit2("minAngularVelocity", "maxAngularVelocity", 3, "", "Min", "Max", "Minimum Angular Velocity");
   %physicsRollout.createCheckBox ("immovable", "Immovable", "Allow or Disallow the Object to Move.");
   %physicsRollout.createCheckBox ("forwardMovementOnly", "Forward Only", "Allows the Object to Only Move in its Forward Direction.");
   %physicsRollout.createTextEdit2("constantForceX", "constantForceY", 3, "Constant Force", "X", "Y", "Constant Force");
   %physicsRollout.createCheckBox ("graviticConstantForce", "Gravitic", "Gravitic Constant Forces Disregard Mass When Applying the Force.");
   %physicsRollout.createTextEdit ("forceScale", 3, "Force Scale", "Scales the Magnitude of Forces Applied to the Object.");
   
   %autoMassInertiaCheck = %physicsRollout.createCheckBox ("autoMassInertia", "Calculate Mass and Inertia", "Forces the Engine to Calculate Mass and Inertia Based on Size and Density.");
   
   %autoMassInertiaContainer = %physicsRollout.createHideableStack(%base @ ".object.getAutoMassInertia();");
   %autoMassInertiaContainer.addControlDependency(%autoMassInertiaCheck);
   %autoMassInertiaContainer.createTextEdit("mass", 3, "Mass", "Mass");
   %autoMassInertiaContainer.createTextEdit("inertialMoment", 3, "Inertia", "Inertial Moment");
   
   %physicsRollout.createTextEdit("density", 3, "Density", "Density");
   %physicsRollout.createTextEdit("damping", 3, "Damping", "Damping");
   %physicsRollout.createTextEdit("friction", 3, "Friction", "Friction");
   %physicsRollout.createTextEdit("restitution", 3, "Restitution", "Restitution");
   
   %mountingRollout = %base.createRolloutStack("Mounting");
   %mountingRollout.createTextEdit("mountRotation", 3, "Rotation", "Mount Rotation");
   %mountingRollout.createTextEdit("autoMountRotation", 3, "Auto Rotation", "Automatic Mount Rotation");
   
   %mountingContainer = %mountingRollout.createHideableStack("!" @ %base @ ".object.getIsMounted();");
   %mountingContainer.createTextEdit("mountForce", 3, "Mount Force", "Mount Tracking Force");
   %mountingContainer.createCheckBox("mountTrackRotation", "Track Rotation", "Track Mount Rotation.");
   %mountingContainer.createCheckBox("mountOwned", "Owned By Mount", "Owned By Mount.");
   %mountingContainer.createCheckBox("mountInheritAttributes", "Inherit Attributes", "Inherits the Attributes of the Mount Object.");
   
   %worldLimitRollout = %base.createRolloutStack("World Limits");
   %worldLimitList = %worldLimitRollout.createDropDownList("worldLimitMode", "Limit Mode", "", "OFF\tNULL\tCLAMP\tBOUNCE\tSTICKY\tKILL", "Change the world limit mode.");
   
	  %worldLimitContainer = %worldLimitRollout.createHideableStack("(" @ %base @ ".object.getWorldLimitMode() $= \"OFF\");");
   %worldLimitContainer.addControlDependency(%worldLimitList);
   %worldLimitContainer.createTextEdit2("WorldLimitMinX", "WorldLimitMinY", 3, "Min. Bounds", "X", "Y", "World Limit Minimum Bounds.", true);
   %worldLimitContainer.createTextEdit2("WorldLimitMaxX", "WorldLimitMaxY", 3, "Max. Bounds", "X", "Y", "World Limit Maximum Bounds.", true);
   %worldLimitContainer.createCheckBox ("WorldLimitCallback", "Callback", "Receive Notifications of World Limit Collisions.");
   
   %hiddenCheck = %base @ ".object.getClassName() $= \"t2dPath\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dParticleEffect\"";
   %hiddenCheck = %hiddenCheck @ " || " @ %base @ ".object.getClassName() $= \"t2dTrigger\"";
   %hiddenCheck = %hiddenCheck @ ";";
   %hidden = %base.createHideableStack( %hiddenCheck );
   %blendingRollout = %hidden.createRolloutStack("Blending");
   %blendingEnabledCheck = %blendingRollout.createCheckBox("BlendingStatus", "Enabled", "Enable Blending");
   
   %blendingContainer = %blendingRollout.createHideableStack("!" @ %base @ ".object.getBlendingStatus();");
   %blendingContainer.addControlDependency(%blendingEnabledCheck);
   %blendingContainer.createEnumList("SrcBlendFactor", false, "Source Factor", "Source Blend Factor", "t2dSceneObject", "srcBlendFactor");
   %blendingContainer.createEnumList("DstBlendFactor", false, "Destination Factor", "Destination Blend Factor", "t2dSceneObject", "dstBlendFactor");
   %blendingContainer.createColorPicker("BlendColor", "Blend Color", "Blend Color");
   
   %dynamicFieldRollout = %base.createRolloutStack("Dynamic Fields");
   %dynamicFieldRollout.createDynamicFieldStack();
   
   // Return Ref to Base.
   return %base;
}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQESceneObject::SaveContent( %contentCtrl )
{
   // Nothing.
}

function t2dSceneObject::getWorldLimitMode(%this)
{
   %worldLimit = %this.getWorldLimit();
   return getWord(%worldLimit, 0);
}

function t2dSceneObject::setWorldLimitMode(%this, %mode)
{
   %worldLimit = %this.getWorldLimit();
   %this.setWorldLimit(%mode, %this.getWorldLimitMinX(), %this.getWorldLimitMinY(), %this.getWorldLimitMaxX(), %this.getWorldLimitMaxY(), %this.getWorldLimitCallback());
}

function t2dSceneObject::getWorldLimitMinX(%this)
{
   %worldLimit = %this.getWorldLimit();
   return getWord(%worldLimit, 1);
}

function t2dSceneObject::setWorldLimitMinX(%this, %val)
{
   %worldLimit = %this.getWorldLimit();
   %this.setWorldLimit(%this.getWorldLimitMode(), %val, %this.getWorldLimitMinY(), %this.getWorldLimitMaxX(), %this.getWorldLimitMaxY(), %this.getWorldLimitCallback());
}

function t2dSceneObject::getWorldLimitMinY(%this)
{
   %worldLimit = %this.getWorldLimit();
   return getWord(%worldLimit, 2);
}

function t2dSceneObject::setWorldLimitMinY(%this, %val)
{
   %worldLimit = %this.getWorldLimit();
   %this.setWorldLimit(%this.getWorldLimitMode(), %this.getWorldLimitMinX(), %val, %this.getWorldLimitMaxX(), %this.getWorldLimitMaxY(), %this.getWorldLimitCallback());
}

function t2dSceneObject::getWorldLimitMaxX(%this)
{
   %worldLimit = %this.getWorldLimit();
   return getWord(%worldLimit, 3);
}

function t2dSceneObject::setWorldLimitMaxX(%this, %val)
{
   %worldLimit = %this.getWorldLimit();
   %this.setWorldLimit(%this.getWorldLimitMode(), %this.getWorldLimitMinX(), %this.getWorldLimitMinY(), %val, %this.getWorldLimitMaxY(), %this.getWorldLimitCallback());
}

function t2dSceneObject::getWorldLimitMaxY(%this)
{
   %worldLimit = %this.getWorldLimit();
   return getWord(%worldLimit, 4);
}

function t2dSceneObject::setWorldLimitMaxY(%this, %val)
{
   %worldLimit = %this.getWorldLimit();
   %this.setWorldLimit(%this.getWorldLimitMode(), %this.getWorldLimitMinX(), %this.getWorldLimitMinY(), %this.getWorldLimitMaxX(), %val, %this.getWorldLimitCallback());
}

function t2dSceneObject::getWorldLimitCallback(%this)
{
   %worldLimit = %this.getWorldLimit();
   return getWord(%worldLimit, 5);
}

function t2dSceneObject::setWorldLimitCallback(%this, %callback)
{
   %worldLimit = %this.getWorldLimit();
   %this.setWorldLimit(%this.getWorldLimitMode(), %this.getWorldLimitMinX(), %this.getWorldLimitMinY(), %this.getWorldLimitMaxX(), %this.getWorldLimitMaxY(), %callback);
}

function t2dSceneObject::setPathStartNode(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setStartNode(%this, %value);
}

function t2dSceneObject::getPathStartNode(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getStartNode(%this);
   else
      return 0;
}

function t2dSceneObject::setPathEndNode(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setEndNode(%this, %value);
}

function t2dSceneObject::getPathEndNode(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getEndNode(%this);
   else
      return 0;
}

function t2dSceneObject::setPathSpeed(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setSpeed(%this, %value);
}

function t2dSceneObject::getPathSpeed(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getSpeed(%this);
   else
      return 0;
}

function t2dSceneObject::setPathMoveForward(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setMoveForward(%this, %value);
}

function t2dSceneObject::getPathMoveForward(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getMoveForward(%this);
   else
      return 0;
}

function t2dSceneObject::setPathOrient(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setOrient(%this, %value);
}

function t2dSceneObject::getPathOrient(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getOrient(%this);
   else
      return 0;
}

function t2dSceneObject::setPathRotationOffset(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setRotationOffset(%this, %value);
}

function t2dSceneObject::getPathRotationOffset(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getRotationOffset(%this);
   else
      return 0;
}

function t2dSceneObject::setPathLoops(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setLoops(%this, %value);
}

function t2dSceneObject::getPathLoops(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getLoops(%this);
   else
      return 0;
}

function t2dSceneObject::setPathFollowMode(%this, %value)
{
   if (%this.getAttachedToPath())
      %this.getAttachedToPath().setFollowMode(%this, %value);
}

function t2dSceneObject::getPathFollowMode(%this)
{
   if (%this.getAttachedToPath())
      return %this.getAttachedToPath().getFollowMode(%this);
   else
      return 0;
}

function t2dSceneObject::moveForwardInLayer(%this)
{
   %scenegraph = ToolManager.getLastWindow().getSceneGraph();
   return %scenegraph.setLayerDrawOrder(%this, "FORWARD");
}

function t2dSceneObject::moveBackwardInLayer(%this)
{
   %scenegraph = ToolManager.getLastWindow().getSceneGraph();
   return %scenegraph.setLayerDrawOrder(%this, "BACKWARD");
}
