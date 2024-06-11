#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Chunked Sprite Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderChunkedSpriteTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderChunkedSpriteTool);

LevelBuilderChunkedSpriteTool::LevelBuilderChunkedSpriteTool() : LevelBuilderCreateTool(),
                                                                 mImageMapName(NULL)
{
   // Set our tool name
   mToolName = StringTable->insert("Chunked Sprite Tool");
}

LevelBuilderChunkedSpriteTool::~LevelBuilderChunkedSpriteTool()
{
}

t2dSceneObject* LevelBuilderChunkedSpriteTool::createObject()
{
   t2dChunkedSprite* staticSprite = dynamic_cast<t2dChunkedSprite*>(ConsoleObject::create("t2dChunkedSprite"));

   if (staticSprite)
      staticSprite->setChunkedImage(mImageMapName);

   return staticSprite;
}

void LevelBuilderChunkedSpriteTool::showObject()
{
   mCreatedObject->setVisible(true);
}

t2dVector LevelBuilderChunkedSpriteTool::getDefaultSize( LevelBuilderSceneWindow *window )
{
   t2dChunkedSprite* chunkedSprite = dynamic_cast<t2dChunkedSprite*>(mCreatedObject);
   if (chunkedSprite)
   {
      // No way to get size from the actual object. We'll do it this way for now.
      t2dChunkedImageDatablock* chunkedImage = dynamic_cast<t2dChunkedImageDatablock*>(Sim::findObject(mImageMapName));
      if (chunkedImage)
      {
         ChunkedTextureHandle texture = chunkedImage->getChunkedImageTexture();
         return t2dVector(texture.getWidth() * 0.1f, texture.getHeight() * 0.1f);
      }
   }

   return t2dVector(10.0f, 10.0f);
}

ConsoleMethod(LevelBuilderChunkedSpriteTool, setImageMap, void, 3, 3, "(map) Sets the image map for the created chunked sprites.\n"
			  "@return No return value.")
{
   t2dChunkedImageDatablock* chunkedImage = dynamic_cast<t2dChunkedImageDatablock*>(Sim::findObject(argv[2]));
   if (chunkedImage)
      object->setImageMapName(argv[2]);
   else
      Con::warnf("LevelBuilderChunkedSpriteTool::setImageMap - Invalid image map: %s", argv[2]);
}


#endif // TORQUE_TOOLS
