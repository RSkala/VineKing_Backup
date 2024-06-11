//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dTileLayer Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dTileLayer", "LBQETileLayer::CreateContent", "LBQETileLayer::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQETileLayer::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQETileLayerClass", %quickEditObj);
   %rollout = %base.createRolloutStack("Tile Map", true);
   %rollout.createTextEdit2("autoPanX", "autoPanY", 3, "Auto Pan", "X", "Y", "Auto Pan", true);
   %rollout.createTextEdit2("panPositionX", "panPositionY", 3, "Pan Position", "X", "Y", "Pan Position", true);
   %rollout.createCheckBox("wrapX", "Wrap X");
   %rollout.createCheckBox("wrapY", "Wrap Y");
   %rollout.createTextEdit2("tileCountXAndWarn", "tileCountYAndWarn", 0, "Tile Count", "X", "Y", "Tile Count");
   %rollout.createTextEdit2("tileSizeX", "tileSizeY", 0, "Tile Size", "X", "Y", "Tile Size");
   %rollout.createCommandButton("TileBuilder::sizeObjectToLayer();", "Size Object to Layer");
   %rollout.createSpacer( 16 );
   
   %hiddenTest = "ToolManager.getActiveTool().getId() == LevelEditorTileMapEditTool.getId();";
   %hidden = %rollout.createHideableStack(%hiddenTest);
   %hidden.createCommandButton("TileBuilder::editSelectedTileLayer();", "Edit Tile Layer");
   
   %hiddenBrushesTest = "ToolManager.getActiveTool().getId() != LevelEditorTileMapEditTool.getId();";
   %hiddenBrushes = %rollout.createHideableStack(%hiddenBrushesTest);
   %brushRollout = %hiddenBrushes.createBrushStack("Tile Editing", true);
   
   // Create Layer Persistence Buttons
   %brushRollout.createLayerPersistence();
   
   %toolbar = %brushRollout.createToolbar();
   %toolbar.addSpacer();
   %toolbar.addTool("Select", "LevelEditorTileMapEditTool.setSelectTool();", "^tools/tileLayerEditor/selectTool");
   LevelEditorTileMapEditTool.paintTool = %toolbar.addTool("Paint", "LevelEditorTileMapEditTool.setPaintTool();", "^tools/tileLayerEditor/paintTool");
   %toolbar.addTool("Flood Fill", "LevelEditorTileMapEditTool.setFloodTool();", "^tools/tileLayerEditor/floodTool");
   %toolbar.addTool("Eye Dropper", "LevelEditorTileMapEditTool.setEyeTool();", "^tools/tileLayerEditor/eyeTool");
   %toolbar.addTool("Eraser", "LevelEditorTileMapEditTool.setEraserTool();", "^tools/tileLayerEditor/eraserTool");
   %toolbar.addSpacer();
   
   LevelEditorTileMapEditTool.paintTool.performClick();
   
   %brushRollout.createDropDownList("brush", "Brush", $brushSet, "BLANK", "", false);
   %brushRollout.createT2DDatablockList("image", "Image", "t2dImageMapDatablock t2dAnimationDatablock", "", "No Change\tNone", false);
   
   %frameStack = %brushRollout.createHideableStack("hideFrameQuickEdit();");
   %frameStack.createT2DFramePicker( "frame", "Frame", "", false );
   
   %brushRollout.createDropDownEditList("tileScript", "Tile Script", $tileScriptSet, "", "", false);
   %brushRollout.createDropDownEditList("customData", "Custom Data", $customDataSet, "", "", false);
   %brushRollout.createCheckBox("flipX", "Flip Horizontal", "", "", true, false);
   %brushRollout.createCheckBox("flipY", "Flip Vertical", "", "", true, false);
   %brushRollout.createCheckBox("collision", "Collision", "", "", true, false);
   %brushRollout.createSpacer(6);
   %brushRollout.createBrushPreview();
   %brushRollout.createSpacer(16);
   %brushRollout.createCommandButton("ActiveBrush.apply();", "Apply To Selection");
   %brushRollout.createSpacer(16);
   %brushRollout.createTextCommandButton("ActiveBrush.save", "Save Brush", "ActiveBrush.getBrush();");
   %brushRollout.createCommandButton("ActiveBrush.deleteBrush();", "Delete Brush", "", 148, 78);
   
   // Return Ref to Base.
   %base.hiddenStack1 = %hidden;
   %base.hiddenStack2 = %hiddenBrushes;
   %base.brushesRollout = %brushRollout;
   $TileEditor::QuickEditPane = %base;
   return %base;
}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQETileLayer::SaveContent( %contentCtrl )
{
   // Nothing.
}

$TileEditor::RequestedTileCountX = -1;
function t2dTileLayer::setTileCountXAndWarn(%this, %count)
{
   %currentCount = %this.getTileCountX();
   if (%count < %currentCount)
   {
      $TileEditor::RequestedTileCountX = %count;
      MessageBoxOKCancel("Warning", "Reducing the tile count will erase the tiles outside the bounds of the layer and cannot be undone. Proceed?",
                         "doSetTileCountX(" @ %this @ ", " @ %count @ ");", "refreshTileCount();");
   }
   
   else
      doSetTileCountX(%this, %count);
}

function doSetTileCountX(%layer, %count)
{
   $TileEditor::RequestedTileCountX = -1;
   %layer.setTileCountX(%count);
}

function t2dTileLayer::getTileCountXAndWarn(%this)
{
   %count = $TileEditor::RequestedTileCountX;
   if (%count < 0)
      %count = %this.getTileCountX();
   return %count;
}

$TileEditor::RequestedTileCountY = -1;
function t2dTileLayer::setTileCountYAndWarn(%this, %count)
{
   %currentCount = %this.getTileCountY();
   if (%count < %currentCount)
   {
      $TileEditor::RequestedTileCountY = %count;
      MessageBoxOKCancel("Warning", "Reducing the tile count will erase the tiles outside the bounds of the layer and cannot be undone. Proceed?",
                         "doSetTileCountY(" @ %this @ ", " @ %count @ ");", "refreshTileCount();");
   }
   
   else
      doSetTileCountY(%this, %count);
}

function doSetTileCountY(%layer, %count)
{
   $TileEditor::RequestedTileCountY = -1;
   %layer.setTileCountY(%count);
}

function t2dTileLayer::getTileCountYAndWarn(%this)
{
   %count = $TileEditor::RequestedTileCountY;
   if (%count < 0)
      %count = %this.getTileCountY();
      
   return %count;
}

function refreshTileCount()
{
   $TileEditor::RequestedTileCountY = -1;
   $TileEditor::RequestedTileCountX = -1;
   GuiFormManager::SendContentMessage( $LBQuickEdit, "", "inspectUpdate" );
}
