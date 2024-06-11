//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "core/resManager.h"
#include "core/fileStream.h"
#include "console/compiler.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "platform/platformInput.h"
#include "core/torqueConfig.h"
#include "core/frameAllocator.h"

// Buffer for expanding script filenames.
static char scriptFilenameBuffer[1024];

//-------------------------------------- Helper Functions
static void forwardslash(char *str)
{
   while(*str)
   {
      if(*str == '\\')
         *str = '/';
      str++;
   }
}

static void backslash(char *str)
{
   while(*str)
   {
      if(*str == '/')
         *str = '\\';
      str++;
   }
}


//----------------------------------------------------------------
ConsoleFunctionGroupBegin( FileSystem, "Functions allowing you to search for files, read them, write them, and access their properties.");

static ResourceObject *firstMatch = NULL;

ConsoleFunction(findFirstFile, const char *, 2, 2, "( strPattern )\n"
				"@param strPattern The string pattern to search for.\n"
				"@return Returns a string representing the first file in the directory system matching the given pattern.")
{
   argc;
   const char *fn;
   firstMatch = NULL;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatch(scriptFilenameBuffer, &fn, NULL);

   if(firstMatch)
   {
#ifdef TORQUE_PLAYER
      // stripBasePath should NOT be using a StringTableEntry
      StringTableEntry ret = Platform::stripBasePath(fn);
      return ret;
#else
      return fn;
#endif
   }
   else
      return "";
}

ConsoleFunction(findNextFile, const char *, 2, 2, "( strPattern )\n" 
				"@param strPattern The string pattern to search for.\n"
				"@return Returns the next file matching a search begun in findFirstFile.")
{
   argc;
   const char *fn;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatch(scriptFilenameBuffer, &fn, firstMatch);
   else
      firstMatch = NULL;

   if(firstMatch)
   {
#ifdef TORQUE_PLAYER
      // stripBasePath should NOT be using a StringTableEntry
      StringTableEntry ret = Platform::stripBasePath(fn);
      return ret;
#else
      return fn;
#endif
   }
   else
      return "";
}

ConsoleFunction(getFileCount, S32, 2, 2, "(strPattern) \n" 
				"@param strPattern The string pattern to search for.\n"
				"@return Returns the number of files in the directory tree that match the given pattern")
{
   argc;
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatch(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatch(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

ConsoleFunction(findFirstFileMultiExpr, const char *, 2, 2, "(strPattern)\n" 
				"@param strPattern The string pattern to search for.\n"
				"@return Returns the first file in the directory system matching the given pattern.")
{
   argc;
   const char *fn;
   firstMatch = NULL;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatchMultiExprs(scriptFilenameBuffer, &fn, NULL);

   if(firstMatch)
   {
#ifdef TORQUE_PLAYER
      // stripBasePath should NOT be using a StringTableEntry
      StringTableEntry ret = Platform::stripBasePath(fn);
      return ret;
#else
      return fn;
#endif
   }
   else
      return "";
}

ConsoleFunction(findNextFileMultiExpr, const char *, 2, 2, "(string pattern) Returns the next file matching a search begun in findFirstFile.")
{
   argc;
   const char *fn;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatchMultiExprs(scriptFilenameBuffer, &fn, firstMatch);
   else
      firstMatch = NULL;

   if(firstMatch)
   {
#ifdef TORQUE_PLAYER
      // stripBasePath should NOT be using a StringTableEntry
      StringTableEntry ret = Platform::stripBasePath(fn);
      return ret;
#else
      return fn;
#endif
   }
   else
      return "";
}

ConsoleFunction(getFileCountMultiExpr, S32, 2, 2, "(strPattern) \n" 
				"@param strPattern The string pattern to search for.\n"
				"@return Returns the number of files in the directory tree that match the given pattern")
{
   argc;
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

ConsoleFunction(getFileCRC, S32, 2, 2, "(filename)\n"
				"@param filename The string representing the file from which to get the CRC\n"
				"@return An integer")
{
   argc;
   U32 crcVal;
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);

   if(!ResourceManager->getCrc(scriptFilenameBuffer, crcVal))
      return(-1);
   return(S32(crcVal));
}

ConsoleFunction(isDirectory, bool, 2, 2, "(path)\n @param pathName Path to check.\n @return Returns true if the given path is a folder/directory, false otherwise")
{
	bool doesExist = Platform::isDirectory(argv[1]);
	if(doesExist)
		return true;

	return false;
}

ConsoleFunction(isFile, bool, 2, 2, "(fileName)\n"
				"@param fileName Filename to check.\n"
				"@return Returns true if the given filename is an existing file or false otherwise")
{
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   bool doesExist = bool(ResourceManager->find(scriptFilenameBuffer));
   if (doesExist)
      return true;

#ifdef TORQUE_PLAYER
   StringTableEntry prefPath = Platform::getPrefsPath(Platform::stripBasePath(scriptFilenameBuffer));
   if(prefPath == NULL || *prefPath == 0)
      return false;

   // test again
   return bool(ResourceManager->find(prefPath));
#else
   return false;
#endif
}

ConsoleFunction(isWriteableFileName, bool, 2, 2, "(fileName)\n"
				"@param fileName Filename to check.\n"
				"@return Returns true if the given filename is an existing file and is not Read-Only or false otherwise")
{
   argc;

#ifdef TORQUE_PLAYER
   const char* filename = NULL;
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   filename = Platform::getPrefsPath(scriptFilenameBuffer);
#else
   char filename[1024];
   Con::expandScriptFilename(filename, sizeof(filename), argv[1]);
#endif

   if (filename == NULL || *filename == 0)
      return false;

   // in a writeable directory?
   if(!ResourceManager->isValidWriteFileName(filename))
      return(false);

   // exists?
   FileStream fs;
   if(!fs.open(filename, FileStream::Read))
      return(true);

   // writeable? (ReadWrite will create file if it does not exist)
   fs.close();
   if(!fs.open(filename, FileStream::ReadWrite))
      return(false);

   return(true);
}

ConsoleFunction(getDirectoryList, const char*, 2, 3, "(strPath, intDepth)\n"
				"@param strPath The path in which to check\n"
				"@param intDepth The depth in which to return (default 0 if not specified)\n"
				"@return The directory contents")
{
   // Grab the full path.
   char path[1024];
   Platform::makeFullPathName(dStrcmp(argv[1], "/") == 0 ? "" : argv[1], path, sizeof(path));

   //dSprintf(path, 511, "%s/%s", Platform::getWorkingDirectory(), argv[1]);

   // Append a trailing backslash if it's not present already.
   if (path[dStrlen(path) - 1] != '/')
   {
      S32 pos = dStrlen(path);
      path[pos] = '/';
      path[pos + 1] = '\0';
   }

   // Grab the depth to search.
   S32 depth = 0;
   if (argc > 2)
      depth = dAtoi(argv[2]);

   // Dump the directories.
   Vector<StringTableEntry> directories;
   Platform::dumpDirectories(path, directories, depth, true);

   if( directories.empty() )
      return "";

   // Grab the required buffer length.
   S32 length = 0;

   for (S32 i = 0; i < directories.size(); i++)
      length += dStrlen(directories[i]) + 1;

   // Get a return buffer.
   char* buffer = Con::getReturnBuffer(length);
   char* p = buffer;

   // Copy the directory names to the buffer.
   for (S32 i = 0; i < directories.size(); i++)
   {
      dStrcpy(p, directories[i]);
      p += dStrlen(directories[i]);
      // Tab separated.
      p[0] = '\t';
      p++;
   }
   p--;
   p[0] = '\0';

   return buffer;
}

ConsoleFunction(fileSize, S32, 2, 2, "(fileName)\n"
				"@param fileName The name of the file to check.\n" 
				"@return Returns the size of the file as an integer or -1 if file not found")
{
   argc;
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return Platform::getFileSize( scriptFilenameBuffer );
}

ConsoleFunction(fileDelete, bool, 2,2, "(fileName)\n"
				"@param fileName Name of the file to remove"
				"@return Returns true on success and flase on failure")
{
   static char fileName[1024];
   static char sandboxFileName[1024];

   Con::expandScriptFilename( fileName, sizeof( fileName ), argv[1] );
   Platform::makeFullPathName(fileName, sandboxFileName, sizeof(sandboxFileName));

   return dFileDelete(sandboxFileName);
}


//----------------------------------------------------------------

ConsoleFunction(fileExt, const char *, 2, 2, "(fileName)\n "
				"@param Name of file from which to extract extension\n"
				"@return A string containing the file's extension (assuming all data after first '.' is the extension) or returns empty string on failure")
{
   argc;
   const char *ret = dStrrchr(argv[1], '.');
   if(ret)
      return ret;
   return "";
}

ConsoleFunction(fileBase, const char *, 2, 2, "fileBase(fileName)")
{

   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);
   
   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   argc;
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      path = szPathCopy;
   else
      path++;
   char *ret = Con::getReturnBuffer(dStrlen(path) + 1);
   dStrcpy(ret, path);
   char *ext = dStrrchr(ret, '.');
   if(ext)
      *ext = 0;
   return ret;
}

ConsoleFunction(fileName, const char *, 2, 2, "(filePathName) Extract the filename from the full path description")
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   argc;
   const char *name = dStrrchr(szPathCopy, '/');
   if(!name)
      name = szPathCopy;
   else
      name++;
   char *ret = Con::getReturnBuffer(dStrlen(name));
   dStrcpy(ret, name);
   return ret;
}

ConsoleFunction(filePath, const char *, 2, 2, "(fileName) Extract the file path from a file's full desciption")
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   argc;
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      return "";
   U32 len = path - (char*)szPathCopy;
   char *ret = Con::getReturnBuffer(len + 1);
   dStrncpy(ret, szPathCopy, len);
   ret[len] = 0;
   return ret;
}

//////////////////////////////////////////////////////////////////////////
// Tools Only Funcitons
//////////////////////////////////////////////////////////////////////////

#ifdef TORQUE_TOOLS

ConsoleToolFunction(openFolder, void, 2 ,2,"openFolder(%path);")
{
   Platform::openFolder( argv[1] );
}

ConsoleToolFunction(pathCopy, bool, 3, 4, "pathCopy(fromFile, toFile [, nooverwrite = true])")
{
   bool nooverwrite = true;

   if( argc > 3 )
      nooverwrite = dAtob( argv[3] );

   static char fromFile[1024];
   static char toFile[1024];

   static char qualifiedFromFile[1024];
   static char qualifiedToFile[1024];

   Con::expandScriptFilename( fromFile, sizeof( fromFile ), argv[1] );
   Con::expandScriptFilename( toFile, sizeof( toFile ), argv[2] );

   Platform::makeFullPathName(fromFile, qualifiedFromFile, sizeof(qualifiedFromFile));
   Platform::makeFullPathName(toFile, qualifiedToFile, sizeof(qualifiedToFile));

   return dPathCopy( qualifiedFromFile, qualifiedToFile, nooverwrite );
}

ConsoleToolFunction(getWorkingDirectory, const char *, 1, 1, "alias to getCurrentDirectory()")
{
   return Platform::getCurrentDirectory();
}

ConsoleToolFunction(getCurrentDirectory, const char *, 1, 1, "getCurrentDirectory()")
{
   return Platform::getCurrentDirectory();
}

ConsoleToolFunction( setCurrentDirectory, bool, 2, 2, "setCurrentDirectory(absolutePathName)" )
{
   return Platform::setCurrentDirectory( StringTable->insert( argv[1] ) );

}

ConsoleToolFunction(getExecutableName, const char *, 1, 1, "getExecutableName()")
{
   return Platform::getExecutableName();
}

ConsoleToolFunction(getMainDotCsDir, const char *, 1, 1, "getExecutableName()")
{
   return Platform::getMainDotCsDir();
}

ConsoleToolFunction(makeFullPath, const char *, 2, 3, "(string path, [string currentWorkingDir])")
{
   char *buf = Con::getReturnBuffer(512);
   Platform::makeFullPathName(argv[1], buf, 512, argc > 2 ? argv[2] : NULL);
   return buf;
}

ConsoleToolFunction(makeRelativePath, const char *, 3, 3, "(string path, string to)")
{
   return Platform::makeRelativePathName(argv[1], argv[2]);
}

ConsoleToolFunction(pathConcat, const char *, 3, 0, "(string path, string file1, [... fileN])")
{
   char *buf = Con::getReturnBuffer(1024);
   char pathBuf[1024];
   dStrcpy(buf, argv[1]);

   for(S32 i = 2;i < argc;++i)
   {
      Platform::makeFullPathName(argv[i], pathBuf, 1024, buf);
      dStrcpy(buf, pathBuf);
   }
   return buf;
}

ConsoleToolFunction(restartInstance, void, 1, 1, "restartInstance()")
{
   Game->setRestart( true );
   Platform::postQuitMessage( 0 );
}

ConsoleToolFunction( createPath, bool, 2,2, "createPath(\"file name or path name\");  creates the path or path to the file name")
{
   static char pathName[1024];

   Con::expandScriptFilename( pathName, sizeof( pathName ), argv[1] );

   return Platform::createPath( pathName );
}

#endif // TORQUE_TOOLS

//////////////////////////////////////////////////////////////////////////

ConsoleFunctionGroupEnd( FileSystem );
