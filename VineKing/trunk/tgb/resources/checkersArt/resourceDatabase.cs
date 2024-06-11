
// Create Resource Descriptor
$instantResource = new ScriptObject()
{
   Class = "checkersArt";
   Name = "checkersArt";
   User = "TGB";
   LoadFunction = "checkersArt::LoadResource";
   UnloadFunction = "checkersArt::UnloadResource";
};

// Load Resource Function - Hooks into game
function checkersArt::LoadResource( %this )
{
}


// Unload Resource Function - Remove from game Sim.
function checkersArt::UnloadResource( %this )
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
   
   new t2dImageMapDatablock(Checkers_boardImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/Checkers_board";
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
   new t2dImageMapDatablock(moonBaseImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/Moonbase";
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
   new t2dImageMapDatablock(moon_angryImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/moon_angry";
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
   new t2dImageMapDatablock(moon_gladImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/moon_glad";
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
   new t2dImageMapDatablock(moon_irisImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/moon_iris";
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
   new t2dImageMapDatablock(moon_neutralImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/moon_neutral";
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
   new t2dImageMapDatablock(sun_angryImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/sun_angry";
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
   new t2dImageMapDatablock(sun_gladImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/sun_glad";
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
   new t2dImageMapDatablock(sun_irisImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/sun_iris";
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
   new t2dImageMapDatablock(sun_neutralImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/sun_neutral";
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

      
   };