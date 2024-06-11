
// Create Resource Descriptor
$instantResource = new ScriptObject()
{
   Class = "particlesArt";
   Name = "particlesArt";
   User = "TGB";
   LoadFunction = "particlesArt::LoadResource";
   UnloadFunction = "particlesArt::UnloadResource";
};

// Load Resource Function - Hooks into game
function particlesArt::LoadResource( %this )
{
}


// Unload Resource Function - Remove from game Sim.
function particlesArt::UnloadResource( %this )
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
   
   new t2dImageMapDatablock(particles1ImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/particles1";
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
      cellWidth = "64";
      cellHeight = "64";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(particles2ImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/particles2";
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
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(particles3ImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/particles3";
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
      cellHeight = "128";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(particles4ImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/particles4";
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
      cellWidth = "64";
      cellHeight = "64";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(particles5ImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/particles5";
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
      cellWidth = "64";
      cellHeight = "64";
      preload = "1";
      allowUnload = "0";
   };
   new t2dImageMapDatablock(particles6ImageMap) {
      canSaveDynamicFields = "1";
      imageName = "./images/particles6";
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
      cellWidth = "64";
      cellHeight = "64";
      preload = "1";
      allowUnload = "0";
   };
      
   };