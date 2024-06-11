#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Tile Map Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderTileMapTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderTileMapTool);

LevelBuilderTileMapTool::LevelBuilderTileMapTool() : LevelBuilderCreateTool(),
                                                     mTileLayerFile(NULL),
                                                     mTileMap(NULL)
{
   // Set our tool name
   mToolName = StringTable->insert("Tile Map Tool");
}

LevelBuilderTileMapTool::~LevelBuilderTileMapTool()
{
}

t2dSceneObject* LevelBuilderTileMapTool::createObject()
{
   if (!mTileMap)
   {
      Con::warnf("LevelBuilderTileMapTool::createObject - No tile map for creating tile layers.");
      return NULL;
   }

   S32 id = mTileMap->createTileLayer(1, 1, 1, 1);
   t2dTileLayer* tileLayer = dynamic_cast<t2dTileLayer*>(Sim::findObject(id));
   if (tileLayer)
   {
      if (tileLayer->loadTileLayer(mTileLayerFile))
         return tileLayer;
   }

   return NULL;
}

t2dVector LevelBuilderTileMapTool::getDefaultSize( LevelBuilderSceneWindow *window )
{
   t2dTileLayer* tileMap = dynamic_cast<t2dTileLayer*>(mCreatedObject);

   if (tileMap)
   {
      return t2dVector(tileMap->getTileCountX() * tileMap->getTileSizeX(),
                       tileMap->getTileCountY() * tileMap->getTileSizeY());
   }
   return t2dVector(10.0f, 10.0f);
}

ConsoleMethod(LevelBuilderTileMapTool, setTileLayerFile, void, 3, 3, "Sets the tile layer file for the created tile layers.")
{
   if (Platform::isFile(argv[2]))
      object->setTileLayerFile(argv[2]);
   else
      Con::warnf("LevelBuilderTileMapTool::setTileLayer - Invalid tile layer file: %s", argv[2]);
}

ConsoleMethod(LevelBuilderTileMapTool, setTileMap, void, 3, 3, "Sets the tile map to place created layers in")
{
   t2dTileMap* tileMap = dynamic_cast<t2dTileMap*>(Sim::findObject(argv[2]));
   if (tileMap)
      object->setTileMap(tileMap);
}


#endif // TORQUE_TOOLS
