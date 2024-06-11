//-----------------------------------------------------------------------------
// Gui Scene TreeView Form Content
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
$LBTreeViewContent = GuiFormManager::AddFormContent( "LevelBuilderSidebarEdit", "Scene TreeView", "LBSceneTreeView::CreateForm", "LBSceneTreeView::SaveForm", 2 );

function ProjectiDevicePanelDeviceType::onChanged(%this)
{
   //If we have iPad selected, we can offer a resolution dropdown to
   //specify the target size of the game. This will be written to file
   //and applied code side to handle the different sizes and options.
   
   if(ProjectiDevicePanelDeviceType.getSelected() == $iDevice::constant::iPad)
   {
      //Reshow these if they were hidden before
      if( ProjectiDevicePanelScreenResolution.Visible == false )
      {
         ProjectiDevicePanelScreenResolution.Visible = true;
         ProjectiDevicePanelScreenResolutionLabel.Visible = true;
      }
   }
   else
   {
      //Hide the resolution controls, theres no option for iPhone iPad as of yet.
      if( ProjectiDevicePanelScreenResolution.Visible == true )
      {
         ProjectiDevicePanelScreenResolution.Visible = false;
         ProjectiDevicePanelScreenResolutionLabel.Visible = false;
      }
   }
   
      //Update the other settings now
      ProjectiDevicePanelScreenOrientation.onChanged();   
}

function loadiOSSettings()
{
   if(isObject(ProjectiFeaturePanel))
   {
      //Add the needed options to the gui on startup
      if(isObject(ProjectiFeaturePanelStatusBarType))
      {
          // Deprecated -MP ProjectiFeaturePanelEnableStorekit.setValue($pref::iDevice::UseStoreKit);
          ProjectiFeaturePanelEnableNetwork.setValue($pref::iDevice::UseNetwork);
          // Deprecated -MP ProjectiFeaturePanelEnableGamekit.setValue($pref::iDevice::UseGameKit);
          ProjectiFeaturePanelEnableMusic.setValue($pref::iDevice::UseMusic);
          // Deprecated -MP ProjectiFeaturePanelEnableLocation.setValue($pref::iDevice::UseLocation);
          ProjectiFeaturePanelEnableMoviePlayer.setValue($pref::iDevice::UseMoviePlayer);
          ProjectiFeaturePanelEnableAutorotate.setValue($pref::iDevice::UseAutoRotate);
          ProjectiFeaturePanelEnableOrientation.setValue($pref::iDevice::UseOrientation);
          ProjectiFeaturePanelEnableMultitouch.setValue($pref::iDevice::UseMultitouch);
         
         ProjectiFeaturePanelStatusBarType.add("Hidden", 0);
         ProjectiFeaturePanelStatusBarType.add("Black Opaque", 1);         
         ProjectiFeaturePanelStatusBarType.add("Black Translucent", 2);
         
         ProjectiFeaturePanelStatusBarType.setSelected($pref::iDevice::StatusBarType);
      }
   }
   
   ProjectiDevicePanelDeviceType.setSelected($pref::iDevice::DeviceType);
   
   if($pref::iDevice::DeviceType == $iDevice::constant::iPad)
   {
      if($pref::iDevice::ScreenOrientation == $iDevice::constant::Landscape)
      {
         ProjectiDevicePanelScreenResolution.clear();
         ProjectiDevicePanelScreenResolution.add("Full (1024x768)", $iDevice::constant::ResolutionFull);
         ProjectiDevicePanelScreenResolution.add("Small (480x320)", $iDevice::constant::ResolutionSmall);
      }
      else
      {
         ProjectiDevicePanelScreenResolution.clear();
         ProjectiDevicePanelScreenResolution.add("Full (768x1024)", $iDevice::constant::ResolutionFull);
         ProjectiDevicePanelScreenResolution.add("Small (320x480)", $iDevice::constant::ResolutionSmall);
      }
      ProjectiDevicePanelScreenResolution.Visible = true;
      ProjectiDevicePanelScreenResolutionLabel.Visible = true;     
      ProjectiDevicePanelScreenOrientation.setSelected($pref::iDevice::ScreenOrientation);
      ProjectiDevicePanelScreenResolution.setSelected($pref::iDevice::ScreenResolution);
   }
   else
   {
      ProjectiDevicePanelScreenOrientation.setSelected($pref::iDevice::ScreenOrientation);
      ProjectiDevicePanelScreenResolution.setSelected($pref::iDevice::ScreenResolution);
   }
   
   /* Settings
   $pref::iDevice::DeviceType
   $pref::iDevice::ScreenOrientation
   $pref::iDevice::ScreenResolution
   */
   
   /* Constants
   $iDevice::constant::Landscape
   */
}

function ProjectiDevicePanelScreenOrientation::onChanged(%this)
{
   // iDevice Settings tab for resolutions are first set here. This happens once
   // So when a project loads these do not change to reflect the project settings
   // Starting trace and fix -MP
   
   //In order to handle this nicely, we have an on change to update the controls 
   //according to the settings we made in the device drop down. If the device is
   //not an iPad, there is no resolution option (it disabled it on change) and then
   //it will call this explicity to update the values on the resolution.
   //also this depends on the current device type 
   if(ProjectiDevicePanelDeviceType.getSelected() == $iDevice::constant::iPad)
   {
      //Only care for the resolution of the device is iPad
      %val = ProjectiDevicePanelScreenOrientation.getSelected();
      %was = ProjectiDevicePanelScreenResolution.getSelected();
      
      //Redo the items in the list so they make sense according to the resolution
      if(%val == $iDevice::constant::Landscape)
      {
         ProjectiDevicePanelScreenResolution.clear();
            ProjectiDevicePanelScreenResolution.add("Full (1024x768)", $iDevice::constant::ResolutionFull);
            ProjectiDevicePanelScreenResolution.add("Small (480x320)", $iDevice::constant::ResolutionSmall); 
      }
      else
      {
         ProjectiDevicePanelScreenResolution.clear();
            ProjectiDevicePanelScreenResolution.add("Full (768x1024)", $iDevice::constant::ResolutionFull);
            ProjectiDevicePanelScreenResolution.add("Small (320x480)", $iDevice::constant::ResolutionSmall);     
      }
      
      //Set the value back to what it was before redoing the items.
      ProjectiDevicePanelScreenResolution.setSelected(%was); 
   }
   else
   {
      //Normal iPhone or iPod selected, update the resolutions invisibly for persistance
      //As its always 480x320 or 320x480 there is not much else but to update its index.
      ProjectiDevicePanelScreenResolution.setSelected($iDevice::constant::ResolutionSmall);
   }
   

}

function ProjectiFeaturePanelEnableAutorotate::onClick(%this)
{
   ProjectiFeaturePanelEnableOrientation.setValue(%this.getValue());
}

function ProjectiFeaturePanelEnableOrientation::onClick(%this)
{
   if(ProjectiFeaturePanelEnableAutorotate.getValue())
      %this.setValue(1);
}

function ProjectiDeviceSettings::setiDeviceOptions()
{
   //This manages build configuration
   
   //old values to replace
         //$pref::iPhone::EnableMultipleTouch
         //$pref::iPhone::StatusBarType
                  
         //New values include Device Type, Device Screen Orientation and Device Resolution if it applies.
         $pref::iDevice::DeviceType          = ProjectiDevicePanelDeviceType.getSelected();
         $pref::iDevice::ScreenOrientation   = ProjectiDevicePanelScreenOrientation.getSelected();
         $pref::iDevice::ScreenResolution    = ProjectiDevicePanelScreenResolution.getSelected();
         
         LBQESceneGraph::updateCameraValues();
         
         //Save the new settings to the xml comfig
         %fname = expandFileName("^project/common/commonConfig.xml");
         echo("Saving file for iDevice Build settings ! " @ %fname);
         _saveGameConfigurationData( %fname );
         
         echo("iDevice settings applied!");
}

function ProjectiFeatureSettings::setiFeatureOptions()
{
   //Handle the xml updates for the device build config
   //Save the new settings to the xml comfig
   
   //collect some info. Things currently -  
      //status bar type,
      //checkboxes - In-app StoreKit Support
      //             Enable Network Support
      //             P2P GameKit Support
      //             iPod Music Support
      //             Location Support
      //             MultiTouch Support
      
   // Deprecated -MP $pref::iDevice::UseStoreKit     = ProjectiFeaturePanelEnableStorekit.getValue();
   $pref::iDevice::UseNetwork      = ProjectiFeaturePanelEnableNetwork.getValue();
   // Deprecated -MP $pref::iDevice::UseGameKit      = ProjectiFeaturePanelEnableGamekit.getValue();
   $pref::iDevice::UseMusic        = ProjectiFeaturePanelEnableMusic.getValue();
   // Deprecated -MP $pref::iDevice::UseLocation     = ProjectiFeaturePanelEnableLocation.getValue();
   $pref::iDevice::UseMultitouch   = ProjectiFeaturePanelEnableMultitouch.getValue();
   $pref::iDevice::UseMoviePlayer  = ProjectiFeaturePanelEnableMoviePlayer.getValue();
   $pref::iDevice::UseAutoRotate   = ProjectiFeaturePanelEnableAutorotate.getValue();
   $pref::iDevice::UseOrientation  = ProjectiFeaturePanelEnableOrientation.getValue();
   $pref::iDevice::StatusBarType   = ProjectiFeaturePanelStatusBarType.getSelected();
   
   %fname = expandFileName("^project/common/commonConfig.xml");
   echo("Saving file for iDevice Feature settings ! " @ %fname);
   _saveGameConfigurationData( %fname );
   
   echo("iDevice Feature settings applied!");
}

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBSceneTreeView::CreateForm( %formCtrl )
{   
   
   //Project manager base container rollout.
   %projectBase = new GuiRolloutCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorRolloutProfile";
      HorizSizing = "width";
      VertSizing = "height";
      Position = "0 0";
      Extent = "323 220";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      hovertime = "1000";
      Caption = "Project manager";
      Margin = "6 4";
      DragSizable = true;
      DefaultHeight = "280";
};
   
   //Project manager scroll container.
   %projScroll = new GuiScrollCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorTransparentScrollProfile";
      HorizSizing = "width";
      VertSizing = "bottom";
      position = "0 0";
      Extent = "214 220";
      MinExtent = "72 220";
      canSave = "1";
      visible = "1";
      hovertime = "1000";
      willFirstRespond = "1";
      hScrollBar = "dynamic";
      vScrollBar = "alwaysOn";
      constantThumbHeight = "0";
      childMargin = "0 0";
   };
   %projectBase.add(%projScroll);
   
   //iDevice Settings base container rollout.
   %iDeviceBase = new GuiRolloutCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorRolloutProfile";
      HorizSizing = "width";
      VertSizing = "height";
      Position = "0 0";
      Extent = "186 235";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      hovertime = "1000";
      Caption = "iDevice Settings";
      Margin = "6 4";
      DragSizable = true;
      DefaultHeight = "255";
   };
   
   //iDevice Settings scroll container.
   %iDeviceScroll = new GuiScrollCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorTransparentScrollProfile";
      HorizSizing = "width";
      VertSizing = "bottom";
      position = "0 0";
      Extent = "184 235";
      MinExtent = "72 235";
      canSave = "1";
      visible = "1";
      hovertime = "1000";
      willFirstRespond = "1";
      hScrollBar = "dynamic";
      vScrollBar = "alwaysOn";
      constantThumbHeight = "0";
      childMargin = "0 0";
   };
   %iDeviceBase.add(%iDeviceScroll);
   
   //iFeature Builder Settings base container rollout.
   %iDeviceFeatureBase = new GuiRolloutCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorRolloutProfile";
      HorizSizing = "width";
      VertSizing = "height";
      Position = "0 0";
      Extent = "186 300";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      hovertime = "1000";
      Caption = "iDevice Feature Builder";
      Margin = "6 4";
      DragSizable = true;
      DefaultHeight = "300";
   };
   
   //iFeature Builder Settings scroll container.
   %iDeviceFeatureScroll = new GuiScrollCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorTransparentScrollProfile";
      HorizSizing = "width";
      VertSizing = "bottom";
      position = "0 0";
      Extent = "150 350";
      MinExtent = "72 350";
      canSave = "1";
      visible = "1";
      hovertime = "1000";
      willFirstRespond = "1";
      //hScrollBar = "dynamic";
      //vScrollBar = "alwaysOn";
      constantThumbHeight = "0";
      childMargin = "0 0";
   };
   %iDeviceFeatureBase.add(%iDeviceFeatureScroll);
   
   %base = new GuiRolloutCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorRolloutProfile";
      HorizSizing = "width";
      VertSizing = "height";
      Position = "0 0";
      Extent = "323 231";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      hovertime = "1000";
      Caption = "Object Tree";
      Margin = "6 4";
      DragSizable = true;
      DefaultHeight = "280";
   };

   %scroll = new GuiScrollCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorTransparentScrollProfile";
      HorizSizing = "width";
      VertSizing = "bottom";
      position = "0 0";
      Extent = "323 231";
      MinExtent = "72 400";
      canSave = "1";
      visible = "1";
      hovertime = "1000";
      willFirstRespond = "1";
      hScrollBar = "dynamic";
      vScrollBar = "alwaysOn";
      constantThumbHeight = "0";
      childMargin = "0 0";
   };
   %base.add(%scroll);

   %treeObj = new GuiTreeViewCtrl() 
   {
      canSaveDynamicFields = "0";
      Profile = "EditorTreeViewProfile";
      class = "LevelBuilderSceneTreeView";
      HorizSizing = "width";
      VertSizing = "height";
      position = "2 2";
      Extent = "171 21";
      MinExtent = "8 2";
      canSave = "1";
      visible = "1";
      hovertime = "1000";
      tabSize = "16";
      textOffset = "2";
      fullRowSelect = "1";
      itemHeight = "21";
      destroyTreeOnSleep = "1";
      MouseDragging = "0";
      MultipleSelections = "1";
      DeleteObjectAllowed = "0";
      DragToItemAllowed = "0";
      scroll = %scroll;
      base = %base;
      owner = %formCtrl;     
   };
   

   //Load the project manager gui  
   exec("./gui/projectManPanel.gui");
   exec("./iDeviceTools/ProjectiDevicePanel.gui");
   exec("./iDeviceTools/ProjectiFeaturePanel.gui");
   
   if(isObject(ProjectManPanel))
   {
      %projScroll.add(ProjectManPanel);
   }
   
   if(isObject(ProjectiDevicePanel))
   {
      //First add the options to the drop down
         if(isObject(ProjectiDevicePanelDeviceType))
         {
            ProjectiDevicePanelDeviceType.add("iPhone / iPod Touch", $iDevice::constant::iPhone);
            ProjectiDevicePanelDeviceType.add("iPad", $iDevice::constant::iPad);
         }
         
         if(isObject(ProjectiDevicePanelScreenOrientation))
         {
            ProjectiDevicePanelScreenOrientation.add("Landscape", $iDevice::constant::Landscape);
            ProjectiDevicePanelScreenOrientation.add("Portrait", $iDevice::constant::Portrait);
         }

         //This is independant of the settings for now, index based.
         if(isObject(ProjectiDevicePanelScreenResolution))
         {
            ProjectiDevicePanelScreenResolution.add("Full (1024x768)", $iDevice::constant::ResolutionFull);
            ProjectiDevicePanelScreenResolution.add("Small (480x320)", $iDevice::constant::ResolutionSmall);            
         }
         
      //The apply the current setting, index based so text can change later
      
         ProjectiDevicePanelDeviceType.setSelected($pref::iDevice::DeviceType);
         ProjectiDevicePanelScreenOrientation.setSelected($pref::iDevice::ScreenOrientation);
         ProjectiDevicePanelScreenResolution.setSelected($pref::iDevice::ScreenResolution);
         
         ProjectiDevicePanelDeviceType.onChanged();
         ProjectiDevicePanelScreenOrientation.onChanged();
      
      %iDeviceScroll.add(ProjectiDevicePanel);
   }
   
   if(isObject(ProjectiFeaturePanel))
   {
      
      //Add the needed options to the gui on startup
      if(isObject(ProjectiFeaturePanelStatusBarType))
      {
          // Deprecated -MP ProjectiFeaturePanelEnableStorekit.setValue($pref::iDevice::UseStoreKit);
          ProjectiFeaturePanelEnableNetwork.setValue($pref::iDevice::UseNetwork);
          // Deprecated -MP ProjectiFeaturePanelEnableGamekit.setValue($pref::iDevice::UseGameKit);
          ProjectiFeaturePanelEnableMusic.setValue($pref::iDevice::UseMusic);
          // Deprecated -MP ProjectiFeaturePanelEnableLocation.setValue($pref::iDevice::UseLocation);
          ProjectiFeaturePanelEnableMoviePlayer.setValue($pref::iDevice::UseMoviePlayer);
          ProjectiFeaturePanelEnableAutorotate.setValue($pref::iDevice::UseAutoRotate);
          ProjectiFeaturePanelEnableOrientation.setValue($pref::iDevice::UseOrientation);
          ProjectiFeaturePanelEnableMultitouch.setValue($pref::iDevice::UseMultitouch);
         
         ProjectiFeaturePanelStatusBarType.add("Hidden", 0);
         ProjectiFeaturePanelStatusBarType.add("Black Opaque", 1);         
         ProjectiFeaturePanelStatusBarType.add("Black Translucent", 2);
         
         ProjectiFeaturePanelStatusBarType.setSelected($pref::iDevice::StatusBarType);
      }      

      %iDeviceFeatureScroll.add(ProjectiFeaturePanel);
   }
   
   %scroll.add( %treeObj );
   %formCtrl.add( %projectBase );
   %formCtrl.add( %iDeviceBase );
   %formCtrl.add( %iDeviceFeatureBase );
   %formCtrl.add( %base );

   // Open the Current SceneGraph, if any.
   %lastWindow = ToolManager.getLastWindow();
   if( isObject( %lastWindow ) && isObject( %lastWindow.getScenegraph() ) )
      %treeObj.open( %lastWindow.getSceneGraph() );


   // Specify Message Control (Override getObject(0) on new Content which is default message control)
   %base.MessageControl = %treeObj;

   //*** Return back the base control to indicate we were successful
   return %base;

}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBSceneTreeView::SaveForm( %formCtrl )
{
   // Nothing.
}

//-----------------------------------------------------------------------------
// Form Content Functionality
//-----------------------------------------------------------------------------
function LevelBuilderSceneTreeView::onContentMessage( %this, %sender, %message )
{

   %command = GetWord( %message, 0 );
   %value   = GetWord( %message, 1 );

   switch$( %command )
   {
      case "openCurrentGraph":
         // Open the Current SceneGraph, if any.
         %lastWindow = ToolManager.getLastWindow();
         if( isObject( %lastWindow ) && isObject( %lastWindow.getScenegraph() ) )
         {
            %this.open( %lastWindow.getSceneGraph() );
            %this.expandItem(1, true);
         }
      case "openObject":
         %this.open( %value );
         
      case "clearSelection":
         %item = %this.findItemByObjectId(%value);
         %this.removeSelection(%item);
      
      case "setSelection":
         %item = %this.findItemByObjectId(%value);
         %this.selectItem(%item);
      
      case "setSelections":
         %count = %value.getCount();
         for (%i = 0; %i < %count; %i++)
         {
            %item = %this.findItemByObjectId(%value.getObject(%i));
            %this.addSelection(%item);
         }
   }
   
   //ITGB-44 - Leaving line here for reference though.
   //%this.scroll.scrollToBottom();
   %this.base.sizeToContents();
   %this.owner.updateStack();
   
}


function LevelBuilderSceneTreeView::onWake( %this )
{
   // Open the Current SceneGraph, if any.
   %lastWindow = ToolManager.getLastWindow();
   if( isObject( %lastWindow ) && isObject( %lastWindow.getScenegraph() ) )
      %this.open( %lastWindow.getSceneGraph() );
}

function LevelBuilderSceneTreeView::onSelect(%this, %object)
{
   if (ToolManager.isAcquired(%object))
      return;
      
   ToolManager.clearAcquisition();

   %lastWindow = ToolManager.getLastWindow();  
   // Tell the Inspector to Inspect the SceneGraph
   if ( isObject( %lastWindow ) && %object == %lastWindow.getScenegraph())
   {
      GuiFormManager::SendContentMessage( $LBQuickEdit, %this, "inspect" SPC %object );
   }
   else 
   {
      ToolManager.acquireObject(%object);
      GuiFormManager::SendContentMessage( $LBQuickEdit, %this, "inspect" SPC %object );
   }
}

function LevelBuilderSceneTreeView::onUnSelect(%this, %object)
{
   if (!ToolManager.isAcquired(%object))
      return;
      
   ToolManager.clearAcquisition(%object);
}

function LevelBuilderSceneTreeView::onAddSelection(%this, %object)
{
   if (ToolManager.isAcquired(%object))
      return;
      
   ToolManager.acquireObject(%object);
}

function LevelBuilderSceneTreeView::onRemoveSelection(%this, %object)
{
   if (!ToolManager.isAcquired(%object))
      return;
      
   ToolManager.clearAcquisition(%object);
}

function LevelBuilderSceneTreeView::onClearSelection(%this)
{
   // if we pass nothing in, it will clear all objects
   ToolManager.clearAcquisition();
}
