//-----------------------------------------------------------------------------
// LevelBuilder Quick Edit t2dStaticSprite Class
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Register Form Content.
//-----------------------------------------------------------------------------
GuiFormManager::AddFormContent( "LevelBuilderQuickEditClasses", "t2dScroller", "LBQEScroller::CreateContent", "LBQEScroller::SaveContent", 2 );

//-----------------------------------------------------------------------------
// Form Content Creation Function
//-----------------------------------------------------------------------------
function LBQEScroller::CreateContent( %contentCtrl, %quickEditObj )
{
   %base = %contentCtrl.createBaseStack("LBQEScrollerClass", %quickEditObj);
   %rollout = %base.createRolloutStack("Scroller", true);
   %rollout.createT2DDatablockList("ImageMap", "Image Map", "t2dImageMapDatablock", "The Image to Display on This Sprite");
   %srcRectList = %rollout.createDropDownList("UseSourceRect", "Use SourceRect", "","NO\tYES", "Enable using SourceRect rendering on this object.");
   %srcRectValue = %rollout.createTextEdit("SpriteSourceRect", "TEXT", "Source Rect", "Four components - The source rect (topLeft topRight width height) ie : 10 10 100 100");
   %rollout.createTextEdit2("RepeatX", "RepeatY", 3, "Repeat", "X", "Y", "Number of Times to Repeat the Texture");
   %rollout.createTextEdit2("ScrollX", "ScrollY", 3, "Scroll Speed", "X", "Y", "The Speed to Scroll the Texture");
   %rollout.createTextEdit2("ScrollPositionX", "ScrollPositionY", 3, "Scroll Position", "X", "Y", "The Scroll Position of the Texture");
   
   // Return Ref to Base.
   return %base;

}

//-----------------------------------------------------------------------------
// Form Content Save Function
//-----------------------------------------------------------------------------
function LBQEScroller::SaveContent( %contentCtrl )
{
   // Nothing.
}
