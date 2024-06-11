#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Scroller Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderScrollerTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderScrollerTool);

LevelBuilderScrollerTool::LevelBuilderScrollerTool() : LevelBuilderCreateTool(),
                                                       mImageMapName(NULL)
{
   // Set our tool name
   mToolName = StringTable->insert("Scroller Tool");
}

LevelBuilderScrollerTool::~LevelBuilderScrollerTool()
{
}

t2dSceneObject* LevelBuilderScrollerTool::createObject()
{
   t2dScroller* scroller = dynamic_cast<t2dScroller*>(ConsoleObject::create("t2dScroller"));

   if (scroller)
      scroller->setImageMap(mImageMapName);

   return scroller;
}

Point2I LevelBuilderScrollerTool::getPixelSize()
{
   t2dScroller* scroller = dynamic_cast<t2dScroller*>(mCreatedObject);
   if (scroller)
   {
      // No way to get size from the actual object. We'll do it this way for now.
      t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(mImageMapName));
      if (imageMap)
      {
         const t2dImageMapDatablock::cFramePixelArea& area = imageMap->getImageMapFramePixelArea(0);
         return Point2I(area.mWidth, area.mHeight);
      }
   }

   return Parent::getPixelSize();
}

ConsoleMethod(LevelBuilderScrollerTool, setImageMap, void, 3, 3, "Sets the image map for the created scrollers.")
{
   t2dImageMapDatablock* imageMap = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject(argv[2]));
   if (imageMap)
      object->setImageMapName(argv[2]);
   else
      Con::warnf("LevelBuilderScrollerTool::setImageMap - Invalid image map: %s", argv[2]);;
}


#endif // TORQUE_TOOLS
