<?php

echo("Welcome to projectGenerator 0.6\n");
echo("(c) GarageGames.Com, Inc.\n");
echo("\n");
error_reporting(E_ERROR);

function generateFileList(&$projectFiles, &$proj, $projName, $outputName, &$output)
{
    $projectFiles[$projName] = array();

   foreach($proj['paths'] as $dir)
   {
      // Build the path.
      $curPath = $output['baseDir'] . $dir;

      // Get the array we'll be adding things to...
      $pathParts = explode('/', $dir);
      $pathWalk = &$projectFiles[$projName];
      foreach($pathParts as $part)
      {
         if(!is_array($pathWalk[$part]))
            $pathWalk[$part] = array();

         $pathWalk = &$pathWalk[$part];
      }

      // Open directory.
      $dirHdl = opendir($curPath);
      if(! $dirHdl)
      {
         echo("Path " . $curPath . " not found, giving up.\n");
         return false;
      }

      // Iterate over all the files.
      while($curFile = readdir($dirHdl))
      {
         // Skip out if it's an uninteresting dir...
         if($curFile == '.' || $curFile == '..' || $curFile == '.svn' || $curFile == 'CVS')
            continue;

         // See if we need to reject it based on our rules..
         $mustReject = false;
         foreach($output['rejectPatterns'] as $rule)
         {
            if(preg_match($rule, $curFile))
            {
               $mustReject = true;
               break;
            }
         }

         if($mustReject)
            continue;

         // Get the extension - is it one of our allowed values?
         $gotMatch = false;
         foreach($output['fileExtensions'] as $ext)
         {
            $extLen = strlen($ext);
            $possibleMatch = substr($curFile, -$extLen, $extLen);

            if($possibleMatch == $ext)
            {
               $gotMatch = true;
               break;
            }
         }

         // No match, skip it!
         if(!$gotMatch)
            continue;

         // Cool - note in the list!
         $pathWalk[] = $curFile;
      }

      // Clean up after yourself!
      closedir($dirHdl);
   }

   return true;
}

echo("   - Loading Smarty...\n");
require_once("smarty/Smarty.class.php");

// Set the working directory to the path of this script.
chdir(dirname($argv[0]));

$tpl = new Smarty();
$tpl->template_dir  = getcwd() . "/templates";
$tpl->compile_dir   = getcwd() . "/templates_c";

// Alright, first we need to build our file & project database.

// Read our config file.
echo("   - Loading config file '$argv[1]'\n");
require($argv[1]);

// Alright, for each project scan and generate the file list.
$projectFiles = array();
$rootProjectGeneratorDir = getcwd();

foreach($g_config_projects as $projName => $proj)
{
   echo("   - Processing project '$projName'...\n");

   // Iterate over this project's outputs.
   foreach($proj['outputs'] as $outputName => $output)
   {

      // Get to the right working directory (first go back to root, then to relative)
      chdir($rootProjectGeneratorDir);
      echo("      - Changing CWD to " . $output['outputDir'] . "\n");
      // echo("        (From: " . getcwd() . ")\n");

      if(!chdir($output['outputDir']))
      {
         echo("Couldn't change directory\n");
         continue;
      }

      echo("      - Scanning directory for output  '$outputName'...\n");
      if(! generateFileList($projectFiles, $proj, $projName, $outputName, $output))
      {
         echo("File list generation failed. Giving up on this project.\n");
         continue;
      }

      echo("      o Writing project file " . $output['outputFile'] . "\n");

      // Evaluate template into a file.
      $tpl->assign_by_ref('fileArray',    $projectFiles[$projName]);
      $tpl->assign_by_ref('projName',     $projName);
      $tpl->assign_by_ref('projSettings', $proj);
      $tpl->assign_by_ref('projOutput',   $output);
      $tpl->assign_by_ref('gameFolder', $proj['gameDir']);

      // Todo: Change this to a more agnostic way.
      // CodeReview: This should be in the template. -- BJG, 3/13/2007
      // The ";" seperated is the VC way of doing it -patw
      $defineString = "";
      foreach($proj['defines'] as $defineName => $define)
         $defineString = $defineString . $define . ";";

      $tpl->assign_by_ref('projDefines', $defineString);

      // To put a bandaid on the tools/player output dir problem
      // CodeReview: This should be in the template. -- BJG, 3/13/2007
      $addPath = "";
      if( $proj['additionalExePath'] )
         $addPath = $proj['additionalExePath'];

      $tpl->assign_by_ref('additionalExePath', $addPath );

      // Write file
      if($hdl = fopen($output['outputFile'], 'w'))
      {
         fputs($hdl, $tpl->fetch($output['template']));
         fclose($hdl);
      }
      else
         echo("Could not write output file: " . $output['outputFile']);
   }

}

?>
