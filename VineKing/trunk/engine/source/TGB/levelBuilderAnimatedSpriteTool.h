#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Animated Sprite Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERANIMATEDSPRITETOOL_H_
#define _LEVELBUILDERANIMATEDSPRITETOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DANIMATEDSPRITE_H_
#include "T2D/t2dAnimatedSprite.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderAnimatedSpriteTool
//-----------------------------------------------------------------------------
class LevelBuilderAnimatedSpriteTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

private:
   StringTableEntry mAnimationName;

protected:
   virtual t2dSceneObject* createObject();
   virtual Point2I getPixelSize();
  
public:
   LevelBuilderAnimatedSpriteTool();
   ~LevelBuilderAnimatedSpriteTool();

   void setAnimationName(const char* name) { mAnimationName = StringTable->insert(name); };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderAnimatedSpriteTool);
};

#endif

#endif // TORQUE_TOOLS
