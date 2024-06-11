//A message as a reminder to create datablocks per level!
//Every time you create a scene/level - You need to create a corresponding datablocks file inside the ./datablocks/levelname_datablocks.cs folder

echo("::: --------" @ $CurrentLevel @ " --- Datablocks being loaded!");  

$levelDatablocks = new SimSet() {
   canSaveDynamicFields = "1";
      setType = "Datablocks";
      new t2dImageMapDatablock(spritesImageMap) {
      imageName = "~/data/images/sprites";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "0";
      preferPerf = "1";
      cellRowOrder = "1";
      cellOffsetX = "0";
      cellOffsetY = "0";
      cellStrideX = "0";
      cellStrideY = "0";
      cellCountX = "-1";
      cellCountY = "-1";
      cellWidth = "256";
      cellHeight = "256";
      preload = "0";
      allowUnload = "1";
      compressPVR = "0";
      optimised = "1";
      force16bit = "0";
   };
   new t2dImageMapDatablock(sparkleImageMap) {
      imageName = "~/data/images/sparkle";
      imageMode = "FULL";
      frameCount = "-1";
      filterMode = "SMOOTH";
      filterPad = "0";
      preferPerf = "1";
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
      compressPVR = "0";
      optimised = "1";
      force16bit = "0";
   };
};