#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Tile Map Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERTILEMAPTOOL_H_
#define _LEVELBUILDERTILEMAPTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DTILEMAP_H_
#include "T2D/t2dTileMap.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderTileMapTool
//-----------------------------------------------------------------------------
class LevelBuilderTileMapTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

private:
   StringTableEntry mTileLayerFile;
   t2dTileMap* mTileMap;

protected:
   virtual t2dSceneObject* createObject();
   virtual t2dVector getDefaultSize( LevelBuilderSceneWindow *window );
  
public:
   LevelBuilderTileMapTool();
   ~LevelBuilderTileMapTool();

   void setTileMap(t2dTileMap* map) { mTileMap = map; };
   void setTileLayerFile(const char* file) { mTileLayerFile = StringTable->insert(file); };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderTileMapTool);
};

#endif


#endif // TORQUE_TOOLS
