//A message as a reminder to create datablocks per level!
//Every time you create a scene/level - You need to create a corresponding datablocks file inside the ./datablocks/levelname_datablocks.cs folder

echo("::: --------" @ $CurrentLevel @ " --- Datablocks being loaded!");  

$levelDatablocks = new SimSet() {
   canSaveDynamicFields = "1";
      setType = "Datablocks";
      
      //add any datablocks for this level below!
};