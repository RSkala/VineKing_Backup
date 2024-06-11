#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Static Sprite Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderStaticSpriteTool.h"
#include "T2D/t2dStaticSprite.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderStaticSpriteTool);

LevelBuilderStaticSpriteTool::LevelBuilderStaticSpriteTool() : LevelBuilderCreateTool(),
                                                               mImageMapName(NULL),
                                                               mImageMapFrame(0)
{
   // Set our tool name
   mToolName      = StringTable->insert("Static Sprite Tool");
}

LevelBuilderStaticSpriteTool::~LevelBuilderStaticSpriteTool()
{
}

t2dSceneObject* LevelBuilderStaticSpriteTool::createObject()
{
   t2dStaticSprite* staticSprite = dynamic_cast<t2dStaticSprite*>(ConsoleObject::create("t2dStaticSprite"));

   if (staticSprite)
   {
      staticSprite->setImageMap(mImageMapName, mImageMapFrame);
      t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(mImageMapName));
      if (imageMap && imageMap->getDefaultConfigDatablock() && *imageMap->getDefaultConfigDatablock())
         staticSprite->setConfigDatablock(imageMap->getDefaultConfigDatablock());
   }

   return staticSprite;
}

Point2I LevelBuilderStaticSpriteTool::getPixelSize()
{
   t2dStaticSprite* staticSprite = dynamic_cast<t2dStaticSprite*>(mCreatedObject);
   if (staticSprite)
   {
      t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(staticSprite->getImageMapName()));
      if (imageMap)
      {
         const U32 frame = staticSprite->getFrame();
         const t2dImageMapDatablock::cFramePixelArea& area = imageMap->getImageMapFramePixelArea(frame);
         return Point2I(area.mWidth, area.mHeight);
      }
   }

   return Parent::getPixelSize();
}

ConsoleMethod(LevelBuilderStaticSpriteTool, setImageMap, void, 3, 4, "Sets the image map for the created static sprites.")
{
   t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(argv[2]));
   if (imageMap)
   {
      S32 frame = 0;
      if( argc == 4 )
         frame = dAtoi( argv[3] );
      object->setImageMapName(argv[2], frame);
   }
   else
      Con::warnf("LevelBuilderStaticSpriteTool::setImageMap - Invalid image map: %s", argv[2]);;
}

void LevelBuilderStaticSpriteTool::setImageMapName( const char* name, S32 frame )
{
   mImageMapName = StringTable->insert(name); 
   mImageMapFrame = frame;
};


#endif // TORQUE_TOOLS
