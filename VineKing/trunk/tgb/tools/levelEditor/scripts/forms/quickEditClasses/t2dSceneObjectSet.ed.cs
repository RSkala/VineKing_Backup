//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dSceneObject Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dSceneObjectSet", "LBQESceneObjectSet::CreateContent", "LBQESceneObjectSet::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQESceneObjectSet::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQESceneObjectClass", %quickEditObj);
   
   %sceneObjectRollout = %base.createRolloutStack("Selected Object Set", true);
   %sceneObjectRollout.createTextEdit2("PositionX", "PositionY", 3, "Position", "X", "Y", "Position", true);
   %sceneObjectRollout.createTextEdit2("Width", "Height", 3, "Size", "Width", "Height", "Size", true);
   %sceneObjectRollout.createTextEdit ("rotation", 3, "Rotation", "Rotation", true);
   %sceneObjectRollout.createCheckBox ("FlipX", "Flip Horizontal", "Flip Horizontal", true);
   %sceneObjectRollout.createCheckBox ("FlipY", "Flip Vertical", "Flip Vertical", true);
   %sceneObjectRollout.createLeftRightEdit("Layer", "0;", "31;", 1, "Layer", "Rendering Layer");
   %sceneObjectRollout.createLeftRightEdit("GraphGroup", "0;", "31;", 1, "Group", "Graph Group");
   
   %alignRollout = %base.createRolloutStack( "Align", true );
   %alignRollout.createAlignTools( false );
   
   // Return Ref to Base.
   return %base;
}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQESceneObjectSet::SaveContent( %contentCtrl )
{
   // Nothing.
}
