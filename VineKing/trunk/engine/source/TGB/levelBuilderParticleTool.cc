#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Particle Effect Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderParticleTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderParticleTool);

LevelBuilderParticleTool::LevelBuilderParticleTool() : LevelBuilderCreateTool(),
                                                       mEffectName(NULL)
{
   // Set our tool name
   mToolName = StringTable->insert("Particle Effect Tool");
}

LevelBuilderParticleTool::~LevelBuilderParticleTool()
{
}

t2dSceneObject* LevelBuilderParticleTool::createObject()
{
   t2dParticleEffect* effect = dynamic_cast<t2dParticleEffect*>(ConsoleObject::create("t2dParticleEffect"));

   return effect;
}

void LevelBuilderParticleTool::showObject()
{
   mCreatedObject->setVisible(true);
   t2dParticleEffect* effect = dynamic_cast<t2dParticleEffect*>(mCreatedObject);
   if (effect)
   {
      // Loading and playing an effect messes with size and position so we need to save and reset.
      t2dVector size = effect->getSize();
      t2dVector position = effect->getPosition();

      effect->loadEffect(mEffectName);
      effect->playEffect(true);

      effect->setSize(size);
      effect->setPosition(position);
   }
}

ConsoleMethod(LevelBuilderParticleTool, setEffect, void, 3, 3, "Sets the effect file for the created particle effects.")
{
   if (Platform::isFile(argv[2]))
      object->setEffect(argv[2]);
   else
      Con::warnf("LevelBuilderParticleTool::setEffect - Invalid effect file: %s", argv[2]);
}


#endif // TORQUE_TOOLS
