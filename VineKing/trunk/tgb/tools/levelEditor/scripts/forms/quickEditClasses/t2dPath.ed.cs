//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dSceneObject Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dPath", "LBQEPath::CreateContent", "LBQEPath::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQEPath::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQEPathClass", %quickEditObj);
   %rollout = %base.createRolloutStack("Path", true);
   %rollout.createEnumList("pathType", false, "Path Type", "The Type of Interpolation to Use for the Path", "t2dPath", "pathType");
   
   // Return Ref to Base.
   return %base;
}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQEPath::SaveContent( %contentCtrl )
{
   // Nothing.
}
