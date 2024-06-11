#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Trigger Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderTriggerTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderTriggerTool);

LevelBuilderTriggerTool::LevelBuilderTriggerTool() : LevelBuilderCreateTool()
{
   // Set our tool name
   mToolName = StringTable->insert("Trigger Tool");
}

LevelBuilderTriggerTool::~LevelBuilderTriggerTool()
{
}

t2dSceneObject* LevelBuilderTriggerTool::createObject()
{
   t2dTrigger* trigger = dynamic_cast<t2dTrigger*>(ConsoleObject::create("t2dTrigger"));

   return trigger;
}

void LevelBuilderTriggerTool::showObject()
{
   mCreatedObject->setDebugOn(BIT(1));
   mCreatedObject->setVisible(true);
}


#endif // TORQUE_TOOLS
