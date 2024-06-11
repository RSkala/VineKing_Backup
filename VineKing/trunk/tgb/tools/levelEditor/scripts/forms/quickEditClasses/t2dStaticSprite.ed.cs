//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dStaticSprite Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dStaticSprite", "LBQEStaticSprite::CreateContent", "LBQEStaticSprite::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQEStaticSprite::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQEStaticSpriteClass", %quickEditObj);
   
   //Static sprite options
   %rollout = %base.createRolloutStack("Static Sprite", true);
   %imageMapList = %rollout.createT2DDatablockList("ImageMap", "Image Map", "t2dImageMapDatablock", "The Image to Display on This Sprite");
   %srcRectList = %rollout.createDropDownList("UseSourceRect", "Use SourceRect", "","NO\tYES", "Enable using SourceRect rendering on this sprite.");
   %srcRectValue = %rollout.createTextEdit("SpriteSourceRect", "TEXT", "Source Rect", "Four components - The source rect (topLeft topRight width height) ie : 10 10 100 100");
   %frameContainer = %rollout.createHideableStack(%base @ ".object.getImageMap().getFrameCount() < 2;");
   %frameContainer.addControlDependency(%imageMapList);
   %frameContainer.createLeftRightEdit("Frame", "0;", %base @ ".object.getImageMap().getFrameCount() - 1;", 1, "Frame", "The Image Map Frame To Display");
   
   // Return Ref to Base.
   return %base;

}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQEStaticSprite::SaveContent( %contentCtrl )
{
   // Nothing.
}
