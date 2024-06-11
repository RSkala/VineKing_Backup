/*
   Takes 2 argument:
   1) CompileScriptsMain.cs (didn't name it main .cs to ensure it doesn't get exec()ed by the editor
   2) directory to compile

*/

$DeleteCount = 0;
$CompileCount = 0;
$Scripts::OverrideDSOPath  = %path;
$Scripts::ignoreDSOs       = false;
function onExit() {}

function deleteDSOs(%path, %echo)
{
   %path = filePath( %path @ "/" );
   addresPath(%path);
   %pattern = %path @ "*.dso";
   for( %file = findFirstFile( %pattern ); %file !$= ""; %file = findNextFile( %pattern ) ) {
      if( %echo == true ) {
         echo( "" TAB "Deleting" SPC %file );
      }
      $DeleteCount++;
      fileDelete(%file);
   }
   
   removeResPath(%path);
}

function compileFiles(%path, %echo)
{
   $Scripts::OverrideDSOPath  = %path;
   $Scripts::ignoreDSOs       = false;
   %path = filePath( %path @ "/" );
   addResPath(%path);
   %pattern = %path @ "*.cs";
   for( %file = findFirstFile( %pattern ); %file !$= ""; %file = findNextFile( %pattern ) ) {
      if( %echo == true ) {
         echo( "" TAB "Compiling" SPC %file );
      }
      $CompileCount++;
      %ret = compile(%file);
   }
   %pattern = %path @ "*.gui";
   for( %file = findFirstFile( %pattern ); %file !$= ""; %file = findNextFile( %pattern ) ) {
      if( %echo == true ) {
         echo( "" TAB "Compiling" SPC %file );
      }
      $CompileCount++;
      %ret = compile(%file);
   }
   %pattern = %path @ "*.t2d";
   for( %file = findFirstFile( %pattern ); %file !$= ""; %file = findNextFile( %pattern ) ) {
      if( %echo == true ) {
         echo( "" TAB "Compiling" SPC %file );
      }
      $CompileCount++;
      %ret = compile(%file);
   }   
   removeResPath(%path);
}


function deleteFiles(%path, %echo)
{
   %path = filePath( %path @ "/" );
   addResPath(%path);
   %pattern = %path @ "*.dso";
   for( %file = findFirstFile( %pattern ); %file !$= ""; %file = findNextFile( %pattern ) ) {
      if( %echo == true ) {
         //echo( "" TAB "Compiling" SPC %file );
      }
      $DeleteCount++;
      %ret = fileDelete(%file);
   }  
   removeResPath(%path);
}


function deleteDirectoryDSOs( %path, %echo )
{     
   // Get a list of direct sub-directories in directory if any
   // (We do this recursively so depth is set to 0)
   %dirs = getDirectoryList( %path, 0 );   
   %cnt  = getWordCount( %dirs );
   
   // First handle sub dirs
   for( %i = 0; %i < %cnt; %i++ )
   {
      // Get the directory path
      %dir = %path @ "/" @ getWord( %dirs, %i );
      
      // Recursively delete it      
      deleteDirectoryDSOs( %dir, %echo );
         //return false;
   }

   if( %echo == true ) {
      echo( "Deleting DSOs in folder:" SPC %path );   
   }
   // Delete all the DSOs in directory
   deleteDSOs( %path, true );
}




$GameDir = $Game::argv2;
echo( "GameDir" SPC $GameDir );

echo( "%       ----------  Deleting DSOs  ----------" );
deleteDSOs( $GameDir, true );
echo( "\nDeleted" SPC $DeleteCount SPC "DSOs" );

echo( "%       ----------  Compiling Script Files  ----------" );
compileFiles( $GameDir, true );
echo( "\nCompiled" SPC $CompileCount SPC "DSOs" );

//done, shutdown
quit();
