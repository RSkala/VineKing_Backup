#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Scroller Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERSCROLLERTOOL_H_
#define _LEVELBUILDERSCROLLERTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCROLLER_H_
#include "T2D/t2dScroller.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderScrollerTool
//-----------------------------------------------------------------------------
class LevelBuilderScrollerTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

private:
   StringTableEntry mImageMapName;

protected:
   virtual t2dSceneObject* createObject();
   virtual Point2I getPixelSize();
  
public:
   LevelBuilderScrollerTool();
   ~LevelBuilderScrollerTool();

   void setImageMapName(const char* name) { mImageMapName = StringTable->insert(name); };

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderScrollerTool);
};

#endif


#endif // TORQUE_TOOLS
