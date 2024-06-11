
// Create Resource Descriptor
$instantResource = new ScriptObject()
{
   Class = "fishArt";
   Name = "fishArt";
   User = "TGB";
   LoadFunction = "fishArt::LoadResource";
   UnloadFunction = "fishArt::UnloadResource";
};

// Load Resource Function - Hooks into game
function fishArt::LoadResource( %this )
{
}


// Unload Resource Function - Remove from game Sim.
function fishArt::UnloadResource( %this )
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
}

// Resource Data
$instantResource.Data = new SimGroup() 
   {
      canSaveDynamicFields = "1";
      new t2dImageMapDatablock(backgroundImage) {
      canSaveDynamicFields = "1";
      imageName = "./backgrounds/background";
      imageMode = "FULL";
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
      cellWidth = "0";
      cellHeight = "0";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(rocksfarImage) {
      canSaveDynamicFields = "1";
      imageName = "./backgrounds/rocksfar";
      imageMode = "FULL";
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
      cellWidth = "0";
      cellHeight = "0";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(rocksnearImage) {
      canSaveDynamicFields = "1";
      imageName = "./backgrounds/rocksnear";
      imageMode = "FULL";
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
      cellWidth = "0";
      cellHeight = "0";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(waveImage) {
      canSaveDynamicFields = "1";
      imageName = "./backgrounds/wave";
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
   new t2dImageMapDatablock(fishimagesImage) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fishimages";
      imageMode = "KEY";
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
      cellWidth = "249";
      cellHeight = "336";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(angelfish1sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/angelfish1sheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dAnimationDatablock(angelfish1sheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "angelfish1sheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dImageMapDatablock(seahorse2sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/seahorse2sheet";
      imageMode = "CELL";
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
      cellWidth = "128";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dAnimationDatablock(seahorse2sheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "seahorse2sheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.5";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dImageMapDatablock(angelfish2sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/angelfish2sheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(butterflyfishsheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/butterflyfishsheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(eelsheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/eelsheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "64";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(pufferfishpuffsheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/pufferfishpuffsheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(pufferfishswimsheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/pufferfishswimsheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(rockfish1sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/rockfish1sheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(rockfish2sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/rockfish2sheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(seahorse1sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/seahorse1sheet";
      imageMode = "CELL";
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
      cellWidth = "128";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(triggerfish1sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/triggerfish1sheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(triggerfish2sheetImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./fish/fish/triggerfish2sheet";
      imageMode = "CELL";
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
      cellWidth = "256";
      cellHeight = "256";
      preload = "1";
      allowUnload = "0";
   };
   new t2dAnimationDatablock(angelfish2sheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "angelfish2sheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(butterflyfishsheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "butterflyfishsheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(eelsheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "eelsheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(pufferfishpuffsheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "pufferfishpuffsheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(pufferfishswimsheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "pufferfishswimsheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
   new t2dAnimationDatablock(triggerfish1sheetImageMapAnimation) {
      canSaveDynamicFields = "1";
      imageMap = "triggerfish1sheetImageMap";
      animationFrames = "0 1 2 3 2 1";
      animationTime = "0.6";
      animationCycle = "1";
      randomStart = "0";
      startFrame = "0";
   };
};