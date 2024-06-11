#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Scene Object Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderTextObjectTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderTextObjectTool);

LevelBuilderTextObjectTool::LevelBuilderTextObjectTool() : LevelBuilderCreateTool(),
                                                           mDefaultFont( StringTable->insert( "Arial" ) ),
                                                           mDefaultSize( 96 ),
                                                           mDefaultHeight( 10 ),
                                                           mDefaultAlignment( t2dTextObject::CENTER ),
                                                           mDefaultWordWrap( true ),
                                                           mDefaultClipText( true ),
                                                           mDefaultAutoSize( true ),
                                                           mDefaultAspectRatio( 1.0f ),
                                                           mDefaultLineSpacing( 0.0f ),
                                                           mDefaultCharacterSpacing( 0.0f )
{
   // Set our tool name
   mToolName = StringTable->insert("Text Object Tool");
}

LevelBuilderTextObjectTool::~LevelBuilderTextObjectTool()
{
}

t2dSceneObject* LevelBuilderTextObjectTool::createObject()
{
   t2dTextObject* textObject = dynamic_cast<t2dTextObject*>(ConsoleObject::create("t2dTextObject"));
   // So no funny resizing happens
   textObject->setEditing( true );
   return textObject;
}

ConsoleMethod(LevelBuilderTextObjectTool, setFontDB, void, 3, 3, "Sets font Datablock for the Text Object.")
{
}

void LevelBuilderTextObjectTool::showObject()
{
   mCreatedObject->setVisible(true);
}

t2dVector LevelBuilderTextObjectTool::getDefaultSize(LevelBuilderSceneWindow* sceneWindow)
{
   return t2dVector( 1.0f, static_cast<t2dTextObject*>( mCreatedObject )->getLineHeight() );
}

void LevelBuilderTextObjectTool::onObjectCreated()
{
   t2dTextObject* textObject = dynamic_cast<t2dTextObject*>( mCreatedObject );
   AssertFatal( textObject, "LevelBuilderTextObjectTool::onObjectCreated - Created object is not a text object." );

   textObject->setEditing( false );

   Parent::onObjectCreated();
}

ConsoleFunction( enumerateFonts, const char*, 1, 1, "() Retrieves a list of all fonts on the system.\n"
              "@return A tab delimited list of the fonts." )
{
   Vector<StringTableEntry> fonts;
   PlatformFont::enumeratePlatformFonts( fonts );

   if( fonts.empty() )
      return "";

   S32 bufferSize = 0;
   for( Vector<StringTableEntry>::const_iterator iter = fonts.begin(); iter != fonts.end(); iter++ )
      bufferSize += dStrlen( *iter ) + 1;

   char* fontList = Con::getReturnBuffer( bufferSize );
   dStrcpy( fontList, fonts[0] );
   for( Vector<StringTableEntry>::const_iterator iter = fonts.begin() + 1; iter != fonts.end(); iter++ )
   {
      dStrcat( fontList, "\t" );
      dStrcat( fontList, *iter );
   }

   S32 length = dStrlen( fontList );

   return fontList;
}

#endif // TORQUE_TOOLS
