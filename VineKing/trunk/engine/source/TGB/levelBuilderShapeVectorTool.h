#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Scene Object Creation tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERSHAPEVECTORTOOL_H_
#define _LEVELBUILDERSHAPEVECTORTOOL_H_

#ifndef _T2DSCENEWINDOW_H_
#include "T2D/t2dSceneWindow.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _T2DSHAPEVECTOR_H_
#include "T2D/t2dShapeVector.h"
#endif

#ifndef _LEVELBUILDERCREATETOOL_H_
#include "TGB/levelBuilderCreateTool.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderShapeVectorTool
//-----------------------------------------------------------------------------
class LevelBuilderShapeVectorTool : public LevelBuilderCreateTool
{
   typedef LevelBuilderCreateTool Parent;

protected:
   virtual t2dShapeVector* createObject();
   virtual void showObject();
  
public:
   LevelBuilderShapeVectorTool();
   ~LevelBuilderShapeVectorTool();

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderShapeVectorTool);
};

#endif


#endif // TORQUE_TOOLS
