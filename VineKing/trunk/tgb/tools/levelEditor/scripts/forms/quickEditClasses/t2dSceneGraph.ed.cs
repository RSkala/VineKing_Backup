//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dStaticSprite Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dSceneGraph", "LBQESceneGraph::CreateContent", "LBQESceneGraph::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQESceneGraph::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQESceneObjectClass", %quickEditObj);
   %rollout = %base.createRolloutStack("Camera", true);
   %rollout.createTextEdit2("CameraPositionX", "CameraPositionY", 3, "Camera", "X", "Y", "Set the camera Position");
   %rollout.createTextEdit2("CameraSizeX", "CameraSizeY", 3, "", "Width", "Height", "Set the camera Size");
   %rollout.createTextEdit2("DesignResolutionX", "DesignResolutionY", 0, "Design Resolution", "X", "Y", "Set the design resolution");
   
   %scriptingRollout = %base.createRolloutStack("Scene Graph Scripting");
   %scriptingRollout.createTextEdit("Name", "TEXT", "Name", "Name the Object for Referencing in Script");
   %scriptingRollout.createTextEdit("Class", "TEXT", "Class", "Link this Object to a Class");
   %scriptingRollout.createTextEdit("SuperClass", "TEXT", "Super Class", "Link this Object to a Parent Class");
   
   %sortRollout = %base.createRolloutStack("Layer Management");
   //%sortRollout.createCheckBox("UseLayerSorting", "Use Layer Sorting", "Enables sorting of objects before the layer is rendered.");
   //%sortRollout.createLabel("Layer      L   H       Sort Mode");
   for ( %i = 0; %i < 32; %i++ )
      %sortRollout.createLayerManager( %i );

   %debugRollout = %base.createRolloutStack("Debug Rendering");
   %debugRollout.createCheckBox("ShowBoundingBoxes", "Bounding Boxes", "Show Object Bounding and Clipping Boxes");
   %debugRollout.createCheckBox("ShowLinkPoints", "Link Points", "Show Object Link Points");
   %debugRollout.createCheckBox("ShowWorldLimits", "World Limits", "Show Object World Limits");
   %debugRollout.createCheckBox("ShowCollisionBounds", "Collision Bounds", "Show Object Collision Poly or Ellipse");
   %debugRollout.createCheckBox("ShowContactHistory", "Contact History", "Show Object Collision Contact History");
   %debugRollout.createCheckBox("ShowSortPoints", "Sort Points", "Show Object Sorting Points");
   %debugRollout.createCheckBox("ShowDebugBanner", "Debug Banner", "Show Scene Debug Banner with Statistics");
   
   %dynamicFieldRollout = %base.createRolloutStack("Dynamic Fields");
   %dynamicFieldRollout.createDynamicFieldStack();
   
   // Return Ref to Base.
   return %base;

}


//-Mat for setting default values from the buttons
function LBQESceneGraph::setPortraitValues( %this ) {
   $currentSceneGraph.setCameraSizeX( 75 );
   $currentSceneGraph.setCameraSizeY( 100 );
   
   $currentSceneGraph.setDesignResolutionX( 320 );
   $currentSceneGraph.setDesignResolutionY( 480 );
   $pref::iPhone::ScreenOrientation = "Portrait";
}

function LBQESceneGraph::updateCameraValues( %this ) 
{
   //This needs to care about it being an iPad or an iPhone project.
   if($pref::iDevice::DeviceType == $iDevice::constant::iPad)
   {
      if($pref::iDevice::ScreenResolution == $iDevice::constant::ResolutionFull)
      {
         if($pref::iDevice::ScreenOrientation == $iDevice::constant::Landscape)
         {
            $currentSceneGraph.setCameraSizeX( $iDevice::constant::ResolutionFullWidth );
            $currentSceneGraph.setCameraSizeY( $iDevice::constant::ResolutionFullHeight );
            
            $currentSceneGraph.setDesignResolutionX( $iDevice::constant::ResolutionFullWidth );
            $currentSceneGraph.setDesignResolutionY( $iDevice::constant::ResolutionFullHeight );
         }
         else
         {
            $currentSceneGraph.setCameraSizeX( $iDevice::constant::ResolutionFullHeight );
            $currentSceneGraph.setCameraSizeY( $iDevice::constant::ResolutionFullWidth );
            
            $currentSceneGraph.setDesignResolutionX( $iDevice::constant::ResolutionFullHeight );
            $currentSceneGraph.setDesignResolutionY( $iDevice::constant::ResolutionFullWidth );            
         }
      }
      else
      {
         if($pref::iDevice::ScreenOrientation == $iDevice::constant::Landscape)
         {
            $currentSceneGraph.setCameraSizeX( $iDevice::constant::ResolutionSmallWidth );
            $currentSceneGraph.setCameraSizeY( $iDevice::constant::ResolutionSmallHeight );
            
            $currentSceneGraph.setDesignResolutionX( $iDevice::constant::ResolutionSmallWidth );
            $currentSceneGraph.setDesignResolutionY( $iDevice::constant::ResolutionSmallHeight );
         }
         else
         {
            $currentSceneGraph.setCameraSizeX( $iDevice::constant::ResolutionSmallHeight );
            $currentSceneGraph.setCameraSizeY( $iDevice::constant::ResolutionSmallWidth );
            
            $currentSceneGraph.setDesignResolutionX( $iDevice::constant::ResolutionSmallHeight );
            $currentSceneGraph.setDesignResolutionY( $iDevice::constant::ResolutionSmallWidth );
         }
      }
   }
   else
   {
      //iPhone only cares for 480x320 by res, so
      if($pref::iDevice::ScreenOrientation == $iDevice::constant::Landscape)
      {
         $currentSceneGraph.setCameraSizeX( $iDevice::constant::ResolutionSmallWidth );
         $currentSceneGraph.setCameraSizeY( $iDevice::constant::ResolutionSmallHeight );
         
         $currentSceneGraph.setDesignResolutionX( $iDevice::constant::ResolutionSmallWidth );
         $currentSceneGraph.setDesignResolutionY( $iDevice::constant::ResolutionSmallHeight );
      }
      else
      {
         $currentSceneGraph.setCameraSizeX( $iDevice::constant::ResolutionSmallHeight );
         $currentSceneGraph.setCameraSizeY( $iDevice::constant::ResolutionSmallWidth );
         
         $currentSceneGraph.setDesignResolutionX( $iDevice::constant::ResolutionSmallHeight );
         $currentSceneGraph.setDesignResolutionY( $iDevice::constant::ResolutionSmallWidth );
      }
   }
}


//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQESceneGraph::SaveContent( %contentCtrl )
{
   // Nothing.
}

function t2dSceneGraph::setCameraPositionX(%this, %value)
{
   %oldPos = %this.cameraPosition;
   %oldSize = %this.cameraSize;
   
   %this.cameraPosition = %value SPC %this.getCameraPositionY();
   if (ToolManager.getActiveTool().getID() == LevelEditorCameraTool.getID())
      LevelEditorCameraTool.setCameraPosition(%this.cameraPosition);
   
   %newPos = %this.cameraPosition;
   %newSize = %this.cameraSize;
   ToolManager.onCameraChanged( %oldPos, %oldSize, %newPos, %newSize );
}

function t2dSceneGraph::setCameraPositionY(%this, %value)
{
   %oldPos = %this.cameraPosition;
   %oldSize = %this.cameraSize;
   
   %this.cameraPosition = %this.getCameraPositionX() SPC %value;
   if (ToolManager.getActiveTool().getID() == LevelEditorCameraTool.getID())
      LevelEditorCameraTool.setCameraPosition(%this.cameraPosition);
   
   %newPos = %this.cameraPosition;
   %newSize = %this.cameraSize;
   ToolManager.onCameraChanged( %oldPos, %oldSize, %newPos, %newSize );
}

function t2dSceneGraph::getCameraPositionX(%this)
{
   return getWord(%this.cameraPosition, 0);
}

function t2dSceneGraph::getCameraPositionY(%this)
{
   return getWord(%this.cameraPosition, 1);
}

function t2dSceneGraph::setCameraSizeX(%this, %value)
{
   %oldPos = %this.cameraPosition;
   %oldSize = %this.cameraSize;
   
   %this.cameraSize = %value SPC %this.getCameraSizeY();
   if (ToolManager.getActiveTool().getID() == LevelEditorCameraTool.getID())
      LevelEditorCameraTool.setCameraSize(%this.cameraSize);
   
   %newPos = %this.cameraPosition;
   %newSize = %this.cameraSize;
   ToolManager.onCameraChanged( %oldPos, %oldSize, %newPos, %newSize );
}

function t2dSceneGraph::setCameraSizeY(%this, %value)
{
   %oldPos = %this.cameraPosition;
   %oldSize = %this.cameraSize;
   
   %this.cameraSize = %this.getCameraSizeX() SPC %value;
   if (ToolManager.getActiveTool().getID() == LevelEditorCameraTool.getID())
      LevelEditorCameraTool.setCameraSize(%this.cameraSize);
   
   %newPos = %this.cameraPosition;
   %newSize = %this.cameraSize;
   ToolManager.onCameraChanged( %oldPos, %oldSize, %newPos, %newSize );
}

function t2dSceneGraph::getCameraSizeX(%this)
{
   return getWord(%this.cameraSize, 0);
}

function t2dSceneGraph::getCameraSizeY(%this)
{
   return getWord(%this.cameraSize, 1);
}

function t2dSceneGraph::setClass(%this, %class)
{
   %this.class = %class;
}

function t2dSceneGraph::getClass(%this, %class)
{
   return %this.class;
}

function t2dSceneGraph::setSuperClass(%this, %class)
{
   %this.superClass = %class;
}

function t2dSceneGraph::getSuperClass(%this, %class)
{
   return %this.superClass;
}

function t2dSceneGraph::setUseLayerSorting(%this, %enable)
{
   %this.useLayerSorting = %enable;
}

function t2dSceneGraph::getUseLayerSorting(%this)
{
   return %this.useLayerSorting;
}


// ----------------------------------------------------------------------------------
// Debug Rendering.
// ----------------------------------------------------------------------------------

function t2dSceneGraph::setShowDebugBanner(%this, %value)
{
   if (%value)
      %this.setDebugOn(0);
   else
      %this.setDebugOff(0);
}


function t2dSceneGraph::setShowBoundingBoxes(%this, %value)
{
   if (%value)
      %this.setDebugOn(1);
   else
      %this.setDebugOff(1);
}

function t2dSceneGraph::setShowLinkPoints(%this, %value)
{
   if (%value)
      %this.setDebugOn(2);
   else
      %this.setDebugOff(2);
}

function t2dSceneGraph::setShowWorldLimits(%this, %value)
{
   if (%value)
      %this.setDebugOn(4);
   else
      %this.setDebugOff(4);
}

function t2dSceneGraph::setShowCollisionBounds(%this, %value)
{
   if (%value)
      %this.setDebugOn(5);
   else
      %this.setDebugOff(5);
}

function t2dSceneGraph::setShowContactHistory(%this, %value)
{
   if (%value)
      %this.setDebugOn(6);
   else
      %this.setDebugOff(6);
}


function t2dSceneGraph::setShowSortPoints(%this, %value)
{
   if (%value)
      %this.setDebugOn(7);
   else
      %this.setDebugOff(7);
}

function t2dSceneGraph::getShowDebugBanner(%this)
{
   return %this.getDebugOn(0);
}

function t2dSceneGraph::getShowBoundingBoxes(%this)
{
   return %this.getDebugOn(1);
}

function t2dSceneGraph::getShowLinkPoints(%this)
{
   return %this.getDebugOn(2);
}

function t2dSceneGraph::getShowWorldLimits(%this)
{
   return %this.getDebugOn(4);
}

function t2dSceneGraph::getShowCollisionBounds(%this)
{
   return %this.getDebugOn(5);
}

function t2dSceneGraph::getShowContactHistory(%this)
{
   return %this.getDebugOn(6);
}

function t2dSceneGraph::getShowSortPoints(%this)
{
   return %this.getDebugOn(7);
}
