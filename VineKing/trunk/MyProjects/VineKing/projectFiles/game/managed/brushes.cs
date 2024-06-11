$brushSet = new SimSet() {
   canSaveDynamicFields = "1";
      setType = "Brushes";

   new ScriptObject(BlankBoxWithCollision) {
      canSaveDynamicFields = "1";
      class = "TileBrush";
         collision = "1";
         customData = "None";
         displayName = "BlankBoxWithCollision";
         FlipX = "-1";
         FlipY = "-1";
         frame = "0";
         image = "InvisibleSquareImageMap";
         script = "None";
   };
   new ScriptObject(BlankBoxNoCollision) {
      canSaveDynamicFields = "1";
      class = "TileBrush";
         collision = "-1";
         customData = "None";
         displayName = "BlankBoxNoCollision";
         FlipX = "-1";
         FlipY = "-1";
         frame = "0";
         image = "InvisibleSquareImageMap";
         script = "None";
   };
};
