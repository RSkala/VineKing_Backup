#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level builder edit base tool.
//-----------------------------------------------------------------------------
#ifndef _LEVELBUILDERBASEEDITTOOL_H_
#define _LEVELBUILDERBASEEDITTOOL_H_

#ifndef _LEVELBUILDERBASETOOL_H_
#include "TGB/levelBuilderBaseTool.h"
#endif

#ifndef _LEVELBUILDERSCENEEDIT_H_
#include "TGB/levelBuilderSceneEdit.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

//-----------------------------------------------------------------------------
// LevelBuilderBaseEditTool
// This tool provides common editing functionality like scaling or rotating.
//-----------------------------------------------------------------------------
class LevelBuilderBaseEditTool : public LevelBuilderBaseTool
{
protected:
   typedef LevelBuilderBaseTool Parent;

   // Sizing States
   enum objectSizingModes
   {
      SizingNone = 0,
      SizingLeft = 1,
      SizingRight = 2,
      SizingTop = 4,
      SizingBottom = 8
   };

   U32 mSizingState;
   
   virtual void nudge(t2dVector pos, S32 directionX, S32 directionY, bool fast, t2dVector& newPos);
   virtual void move(LevelBuilderSceneEdit* sceneEdit, t2dVector size, t2dVector mousePoint2D, t2dVector& finalPosition);
   virtual void rotate(LevelBuilderSceneEdit* sceneEdit, F32 rotation, t2dVector rotationVector, t2dVector newVector, F32& newRotation);
   virtual void scale(LevelBuilderSceneEdit* sceneEdit, t2dVector size, t2dVector pos, t2dVector mousePoint2D, bool uniform, bool maintainAR, F32 ar,
                      t2dVector& newSize, t2dVector& newPosition, bool& flipX, bool& flipY);
  
public:
   LevelBuilderBaseEditTool();
   virtual ~LevelBuilderBaseEditTool();

   virtual void drawSizingNuts(LevelBuilderSceneWindow* sceneWindow, const RectF& rect);
   virtual S32  getSizingState(LevelBuilderSceneWindow* sceneWindow, const Point2I &pt, const RectF &rect);

   // Declare our Console Object
   DECLARE_CONOBJECT(LevelBuilderBaseEditTool);
};

#endif

#endif // TORQUE_TOOLS
