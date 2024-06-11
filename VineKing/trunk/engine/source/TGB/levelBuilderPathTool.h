#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Path Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERPATHTOOL_H_
#define _LEVELBUILDERPATHTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DPATH_H_
#include "T2D/t2dPath.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderPathTool
//-----------------------------------------------------------------------------
class LevelBuilderPathTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

protected:
   virtual t2dSceneObject* createObject();
   virtual void showObject();
   virtual t2dVector getDefaultSize( LevelBuilderSceneWindow *window );
  
public:
   LevelBuilderPathTool();
   ~LevelBuilderPathTool();

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderPathTool);
};

#endif


#endif // TORQUE_TOOLS
