
// Create Resource Descriptor
$instantResource = new ScriptObject()
{
   Class = "moleArt";
   Name = "moleArt";
   User = "TGB";
   LoadFunction = "moleArt::LoadResource";
   UnloadFunction = "moleArt::UnloadResource";
};

// Load Resource Function - Hooks into game
function moleArt::LoadResource( %this )
{
   // Update Object library once datablocks are loaded.
   GuiFormManager::BroadcastContentMessage( "LevelBuilderSidebarCreate", 0, "refresh" );
}


// Unload Resource Function - Remove from game Sim.
function moleArt::UnloadResource( %this )
{
   // We must clean up all the mess we've made in the Sim.
   if( isObject( %this.Data ) && %this.Data.GetCount() > 0 )
   {      
      while( %this.Data.getCount() > 0 )
      {
         %datablockObj = %this.Data.getObject( 0 );
         %this.Data.remove( %datablockObj );
         if( isObject( %datablockObj ) )
            %datablockObj.delete();
      }
   }
   // Update Object library once datablocks are removed.
   GuiFormManager::BroadcastContentMessage( "LevelBuilderSidebarCreate", 0, "refresh" );
}

// Resource Data
$instantResource.Data = new SimGroup() 
   {
      canSaveDynamicFields = "1";
   
      new t2dImageMapDatablock(imageMoleRed) {
      canSaveDynamicFields = "1";
      imageName = "./images/molered";
      imageMode = "CELL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "1";
      preferPerf = "0";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "73";
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(imageMoleLilac) {
      canSaveDynamicFields = "1";
      imageName = "./images/molelilac";
      imageMode = "CELL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "1";
      preferPerf = "0";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "73";
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(imageMoleGreen) {
      canSaveDynamicFields = "1";
      imageName = "./images/molegreen";
      imageMode = "CELL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "1";
      preferPerf = "0";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "73";
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dAnimationDatablock(animMoleComeOutRed) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleRed";
      animationFrames = "0 1 2 3 4 4 3";
      animationTime = "0.3";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleDiveInRed) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleRed";
      animationFrames = "4 4 3 2 1 0";
      animationTime = "0.3";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleComeOutLilac) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleLilac";
      animationFrames = "0 1 2 3 4 4 3";
      animationTime = "0.25";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleDiveInLilac) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleLilac";
      animationFrames = "4 4 3 2 1 0";
      animationTime = "0.3";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleComeOutGreen) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleGreen";
      animationFrames = "0 1 2 3 4 4 3";
      animationTime = "0.3";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleDiveInGreen) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleGreen";
      animationFrames = "4 3 2 1 0";
      animationTime = "0.3";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dImageMapDatablock(imageMoleHole) {
      canSaveDynamicFields = "1";
      imageName = "./images/molehole";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "0";
      preferPerf = "0";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "0";
      cellHeight = "0";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(imageHammer) {
      canSaveDynamicFields = "1";
      imageName = "./images/molehammer";
      imageMode = "CELL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "1";
      preferPerf = "0";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "64";
      cellHeight = "64";
      preload = "1";
      allowUnload = "0";
   };
   new t2dAnimationDatablock(animHammer) {
      canSaveDynamicFields = "1";
      imageMap = "imageHammer";
      animationFrames = "0 1 2";
      animationTime = "0.2";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleWhackedLilac) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleLilac";
      animationFrames = "3 5 6 6 5 7 8";
      animationTime = "0.35";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleWhackedRed) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleRed";
      animationFrames = "3 5 6 6 5 7 8";
      animationTime = "0.35";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animMoleWhackedGreen) {
      canSaveDynamicFields = "1";
      imageMap = "imageMoleGreen";
      animationFrames = "3 6 6 5 7 8";
      animationTime = "0.35";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(animHammerIdle) {
      canSaveDynamicFields = "1";
      imageMap = "imageHammer";
      animationFrames = "2 2";
      animationTime = "1";
      animationCycle = "0";
      randomStart = "0";
      startFrame = "0";
   };



   };