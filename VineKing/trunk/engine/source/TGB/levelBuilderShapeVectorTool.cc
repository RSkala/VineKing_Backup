#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Scene Object Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderShapeVectorTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderShapeVectorTool);

LevelBuilderShapeVectorTool::LevelBuilderShapeVectorTool() : LevelBuilderCreateTool()
{
   // Set our tool name
   mToolName = StringTable->insert("Shape Vector Tool");
}

LevelBuilderShapeVectorTool::~LevelBuilderShapeVectorTool()
{
}

t2dShapeVector* LevelBuilderShapeVectorTool::createObject()
{
   t2dShapeVector* shapeVector = dynamic_cast<t2dShapeVector*>(ConsoleObject::create("t2dShapeVector"));

   return shapeVector;
}

void LevelBuilderShapeVectorTool::showObject()
{
   mCreatedObject->setVisible(true);
}


#endif // TORQUE_TOOLS
