#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Path Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderPathTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderPathTool);

LevelBuilderPathTool::LevelBuilderPathTool() : LevelBuilderCreateTool()
{
   // Set our tool name
   mToolName = StringTable->insert("Path Tool");
}

LevelBuilderPathTool::~LevelBuilderPathTool()
{
}

t2dSceneObject* LevelBuilderPathTool::createObject()
{
   t2dPath* path = dynamic_cast<t2dPath*>(ConsoleObject::create("t2dPath"));

   return path;
}

void LevelBuilderPathTool::showObject()
{
   mCreatedObject->setDebugOn(BIT(1));
   mCreatedObject->setVisible(true);
}

t2dVector LevelBuilderPathTool::getDefaultSize( LevelBuilderSceneWindow *window )
{
   return t2dVector(10.0f, 10.0f);
}


#endif // TORQUE_TOOLS
