#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Particle Effect Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERPARTICLETOOL_H_
#define _LEVELBUILDERPARTICLETOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DPARTICLEEFFECT_H_
#include "T2D/t2dParticleEffect.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderParticleTool
//-----------------------------------------------------------------------------
class LevelBuilderParticleTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

private:
   StringTableEntry mEffectName;

protected:
   virtual t2dSceneObject* createObject();
   virtual void showObject();
  
public:
   LevelBuilderParticleTool();
   ~LevelBuilderParticleTool();

   void setEffect(const char* name) { mEffectName = StringTable->insert(name); };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderParticleTool);
};

#endif


#endif // TORQUE_TOOLS
