#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Animated Sprite Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderAnimatedSpriteTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderAnimatedSpriteTool);

LevelBuilderAnimatedSpriteTool::LevelBuilderAnimatedSpriteTool() : LevelBuilderCreateTool(),
                                                                   mAnimationName(NULL)
{
   // Set our tool name
   mToolName = StringTable->insert("Animated Sprite Tool");
}

LevelBuilderAnimatedSpriteTool::~LevelBuilderAnimatedSpriteTool()
{
}

t2dSceneObject* LevelBuilderAnimatedSpriteTool::createObject()
{
   t2dAnimatedSprite* animatedSprite = dynamic_cast<t2dAnimatedSprite*>(ConsoleObject::create("t2dAnimatedSprite"));

   if (animatedSprite)
   {
      animatedSprite->playAnimation(mAnimationName, false);
      t2dAnimationDatablock* animation = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject(mAnimationName));
      if (animation && animation->getDefaultConfigDatablock() && *animation->getDefaultConfigDatablock())
         animatedSprite->setConfigDatablock(animation->getDefaultConfigDatablock());
   }

   return animatedSprite;
}

Point2I LevelBuilderAnimatedSpriteTool::getPixelSize()
{
   t2dAnimatedSprite* animatedSprite = dynamic_cast<t2dAnimatedSprite*>(mCreatedObject);
   if (animatedSprite)
   {
      const t2dImageMapDatablock::cFramePixelArea& area = animatedSprite->mAnimationController.getCurrentFramePixelArea();
      return Point2I(area.mWidth, area.mHeight);
   }

   return Parent::getPixelSize();
}

ConsoleMethod(LevelBuilderAnimatedSpriteTool, setAnimation, void, 3, 3, "(animationName) Sets the animation for the created animated sprites.\n"
			  "@param animationName The name of the desired animation."
			  "@return No return value.")
{
   t2dAnimationDatablock* animation = dynamic_cast<t2dAnimationDatablock*>(Sim::findObject(argv[2]));
   if (animation)
      object->setAnimationName(argv[2]);
   else
      Con::warnf("LevelBuilderAnimatedSpriteTool::setAnimation - Invalid animation: %s", argv[2]);
}

#endif // TORQUE_TOOLS
