#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Chunked Sprite Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERCHUNKEDSPRITETOOL_H_
#define _LEVELBUILDERCHUNKEDSPRITETOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DCHUNKEDSPRITE_H_
#include "T2D/t2dChunkedSprite.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderChunkedSpriteTool
//-----------------------------------------------------------------------------
class LevelBuilderChunkedSpriteTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

private:
   StringTableEntry mImageMapName;

protected:
   virtual t2dSceneObject* createObject();
   virtual void showObject();
   virtual t2dVector getDefaultSize( LevelBuilderSceneWindow *window );
  
public:
   LevelBuilderChunkedSpriteTool();
   ~LevelBuilderChunkedSpriteTool();

   void setImageMapName(const char* name) { mImageMapName = StringTable->insert(name); };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderChunkedSpriteTool);
};

#endif


#endif // TORQUE_TOOLS
