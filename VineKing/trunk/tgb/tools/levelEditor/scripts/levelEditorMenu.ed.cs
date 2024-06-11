
//set up $cmdctrl variable so that it matches OS standards
$cmdCtrl = $platform $= "macos" ? "Cmd" : "Ctrl";

function LevelBuilderMenu::initialize( %this )
{
   if( isObject( LevelBuilderBase.menuGroup ) )
      LevelBuilderBase.menuGroup.delete();
      
   LevelBuilderBase.menuGroup = new SimGroup();
   
   //-----------------------------------------------------------------------------
   // File Menu
   //-----------------------------------------------------------------------------    
   %nonMacMenu = 0;
   if( $platform $= "macos" )
      %nonMacMenu = -1000;

   %fileMenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      
      barPosition = 0;
      barName     = "File";      
      
      item[0] = "New Scene..." TAB $cmdCtrl SPC "N" TAB "ToolManager.getLastWindow().setFirstResponder(); LBProjectObj.newLevel();";
      item[1] = "Open Scene..." TAB $cmdCtrl SPC "O" TAB "ToolManager.getLastWindow().setFirstResponder(); LBProjectObj.openLevel();";
      item[2] = "-";
      item[3] = "New Project..." TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); newProject();";
      item[4] = "Open Project..." TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); TGBWorkspace.OpenProject();";
      item[5] = "-";
      item[6] = "Save" TAB $cmdCtrl SPC "S" TAB "ToolManager.getLastWindow().setFirstResponder(); LBProjectObj.saveLevel();";
      item[7] = "Save As..." TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); LBProjectObj.saveLevelAs();";
      
	  //item[5] = "Build Project..." TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); buildProject();";
      
      // the mac os application menu already has a quit item, yay! no need to duplicate it here!
      // we therefore hide these next two entries
      //item[%nonMacMenu + 9] = "-";
      //item[%nonMacMenu + 10] = "Quit" TAB "" TAB "quit();";
   };

   //-----------------------------------------------------------------------------
   // Edit Menu
   //-----------------------------------------------------------------------------
   %editMenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      
      barPosition = 1;
      barName     = "Edit";      

      item[0] = "Undo" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); levelBuilderUndo(1);";
      item[1] = "Redo" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); levelBuilderRedo(1);";
      item[2] = "-";
      item[3] = "Cut" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); levelBuilderCut(1);";
      item[4] = "Copy" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); levelBuilderCopy(1);";
      item[5] = "Paste" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder();levelBuilderPaste(1);";
      item[6] = "-";
      item[7] = "Group Objects" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); ToolManager.groupObjects();";
      item[8] = "Ungroup Objects" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); ToolManager.breakApart();";
      item[9] = "-";
      item[10] = "Send to back" TAB $cmdCtrl SPC "B" TAB "ToolManager.getLastWindow().setFirstResponder(); sendToBack(true);";
      item[11] = "Bring to front" TAB $cmdCtrl SPC "F" TAB "ToolManager.getLastWindow().setFirstResponder(); bringToFront(true);";
      item[12] = "-";
      item[13] = "Preferences..." TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); Canvas.pushDialog(optionsDlg);";
   };


   //-----------------------------------------------------------------------------
   // Project Menu
   //-----------------------------------------------------------------------------
   %projectMenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      
      barPosition = 2;
      barName     = "Project";      

      item[0] = "Show Start Page" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); Canvas.setContent(TGBStartPage);";
      item[1] = "Run Game" TAB "" TAB "ToolManager.getLastWindow().setFirstResponder(); runGame();";
      item[2] = "-";
      item[3] = "GUI Builder..." TAB "" TAB "GuiEdit(0);";
      item[4] = "-";
      item[5] = "Image Map Builder..." TAB "" TAB "launchNewImageMap();";
      item[6] = "Animation Builder..." TAB "" TAB "AnimationBuilder.createAnimation();";
      item[7] = "-";
      item[8] = "Refresh Object Library" TAB "" TAB "GuiFormManager::SendContentMessage($LBCreateSiderBar, %this, \"refreshall 0\");";
      item[9] = "Reload Textures" TAB "" TAB "reloadImageMaps();";
};      

   //-----------------------------------------------------------------------------
   // View Menu
   //-----------------------------------------------------------------------------
   %viewMenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      
      barPosition = 3;
      barName     = "View";

      
      item[0] = "Home" TAB "" TAB "levelBuilderHomeView(1);";
      item[1] = "Show All" TAB "" TAB "levelBuilderZoomToFit(1);";
      item[2] = "Show Selected" TAB "" TAB "levelBuilderZoomToSelected(1);";
      item[3] = "-";
      item[4] = "Zoom 25%" TAB "" TAB "levelBuilderZoomView(0.25);";
      item[5] = "Zoom 50%" TAB "" TAB "levelBuilderZoomView(0.5);";
      item[6] = "Zoom 100%" TAB "" TAB "levelBuilderZoomView(1.0);";
      item[7] = "Zoom 200%" TAB "" TAB "levelBuilderZoomView(2.0);";
      item[8] = "Zoom 400%" TAB "" TAB "levelBuilderZoomView(4.0);";
   };      

   //-----------------------------------------------------------------------------
   // Help Menu
   //-----------------------------------------------------------------------------
   %helpFile = expandFilename("^tools/help.html");
   %helpMenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      
      barPosition = 4;
      barName     = "Help";
      
      item[0] = "Documentation" TAB "" TAB "gotowebpage(\"" @ %helpFile @ "\");";
      item[1] = "-";
      item[2] = "Keyboard Shortcuts..." TAB "" TAB "Canvas.pushDialog(optionsdlg);OptionsTabBook.selectPage(1);";
      item[3] = "-";
      item[4] = "iTorque 2D Community" TAB "" TAB "gotoWebpage(\"http://www.garagegames.com/community/forums/56\");";
      item[5] = "-";
      item[6] = "About..." TAB "" TAB "TGBInsiderDlg.showAbout();";
   };
      
   // Submenus will be deleted when the menu they are in is deleted
   LevelBuilderBase.menuGroup.add(%fileMenu);
   LevelBuilderBase.menuGroup.add(%editMenu);
   LevelBuilderBase.menuGroup.add(%projectMenu);
   LevelBuilderBase.menuGroup.add(%viewMenu);
   LevelBuilderBase.menuGroup.add(%helpMenu);

}

function LevelBuilderMenu::destroy( %this )
{
   LevelBuilderBase.menuGroup.delete();
}

// [neo, 5/31/2007 - #3174]
// Refactored menu attach stuff so we can call it as needed
// e.g. before and after display changes, etc.
function LevelBuilderBase::attachMenuGroup( %this )
{
   if( !isObject( %this.menuGroup ) ) 
      return;

   for( %i = 0; %i < %this.menuGroup.getCount(); %i++ )
     %this.menuGroup.getObject( %i ).attachToMenuBar();
}

// [neo, 5/31/2007 - #3174]
// Refactored menu detach stuff so we can call it as needed
// e.g. before and after display changes, etc.
function LevelBuilderBase::detachMenuGroup( %this )
{
   if( !isObject( %this.menuGroup ) ) 
      return;
      
   for( %i = 0; %i < %this.menuGroup.getCount(); %i++ )
      %this.menuGroup.getObject( %i ).removeFromMenuBar();
}

// This is a component of the LevelBuilder GUI so this is called when the GUI Sleeps.
function LevelBuilderBase::onSleep( %this )
{
   // [neo, 5/31/2007 - #3174]
   // Refactored code to detachMenuGroup();
   detachMenuBars();
}

function LevelBuilderBase::onWake( %this )
{
   if( %this.getID() != Canvas.getContent().getID() )
      return;
 
   // [neo, 5/31/2007 - #3174]
   // Refactored to attachMenuGroup();
   attachMenuBars();   
}