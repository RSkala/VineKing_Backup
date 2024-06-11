#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Scene Object Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderSceneObjectTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderSceneObjectTool);

LevelBuilderSceneObjectTool::LevelBuilderSceneObjectTool() : LevelBuilderCreateTool()
{
   // Set our tool name
   mToolName = StringTable->insert("Scene Object Tool");
}

LevelBuilderSceneObjectTool::~LevelBuilderSceneObjectTool()
{
}

t2dSceneObject* LevelBuilderSceneObjectTool::createObject()
{
   t2dSceneObject* staticSprite = dynamic_cast<t2dSceneObject*>(ConsoleObject::create("t2dSceneObject"));

   return staticSprite;
}

void LevelBuilderSceneObjectTool::showObject()
{
   mCreatedObject->setDebugOn(BIT(1));
   mCreatedObject->setVisible(true);
}


#endif // TORQUE_TOOLS
