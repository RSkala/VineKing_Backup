#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level builder edit base tool.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "TGB/levelBuilderBaseEditTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderBaseEditTool);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LevelBuilderBaseEditTool::LevelBuilderBaseEditTool() : LevelBuilderBaseTool(),
                                                       mSizingState(0)
{
   // Set our tool name
   mToolName = StringTable->insert("Edit Base Tool");
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
LevelBuilderBaseEditTool::~LevelBuilderBaseEditTool()
{
}

//-----------------------------------------------------------------------------
// rotate
//-----------------------------------------------------------------------------
void LevelBuilderBaseEditTool::rotate(LevelBuilderSceneEdit* sceneEdit, F32 rotation, t2dVector rotationVector, t2dVector newVector, F32& newRotation)
{
   PROFILE_SCOPE(LevelBuilderBaseEditTool_rotate);

   AssertFatal(sceneEdit, "LevelBuilderBaseEditTool - Editing objects without a valid scene edit context.");

   F32 oldRotationOffset = -mAtan(rotationVector.mX, rotationVector.mY) - mDegToRad(rotation);
   newRotation = -mAtan(newVector.mX, newVector.mY) - oldRotationOffset;

   if (!sceneEdit->getRotationSnap())
      return;

   // Grab the snap values.
   F32 snapThreshold = mDegToRad(sceneEdit->getRotationSnapThreshold());
   F32 snapDegrees = mDegToRad(sceneEdit->getRotationSnapAngle());

   // Snap the new rotation.
   F32 closestSnap = mFloor((newRotation / snapDegrees) + 0.5) * snapDegrees;
   if (mFabs(newRotation - closestSnap) < snapThreshold)
      newRotation = closestSnap;

   // Always snap to 90 degree increments.
   F32 piOver2 = M_PI * 0.5f;
   if (mNotZero(piOver2 / snapDegrees))
   {
      F32 closest90 = mFloor((newRotation / piOver2) + 0.5) * piOver2;
      if (mFabs(newRotation - closest90) < snapThreshold)
         newRotation = closest90;
   }

   newRotation = mRadToDeg(newRotation);
}

//-----------------------------------------------------------------------------
// move
//-----------------------------------------------------------------------------
void LevelBuilderBaseEditTool::move(LevelBuilderSceneEdit* sceneEdit, t2dVector size, t2dVector position, t2dVector& finalPosition)
{
   PROFILE_SCOPE(LevelBuilderBaseEditTool_move);

   AssertFatal(sceneEdit, "LevelBuilderBaseEditTool - Editing objects without a valid scene edit context.");

   t2dVector halfSize = size * 0.5;
   t2dVector upperLeft = position - halfSize;
   t2dVector lowerRight = position + halfSize;

   // The final position is just the position, unless one of the edges of the
   // rectangle are within snapThreshold of a grid line.
   finalPosition = position;
   F32 snapThreshold = sceneEdit->getSnapThreshold();
   if (sceneEdit->isGridSnapX() && sceneEdit->getSnapToGridX())
   {
      F32 xSnap = sceneEdit->getGridSnapX();

      // Snap line closest to the left.
      F32 closestX = mFloor((upperLeft.mX / xSnap) + 0.5) * xSnap;
      F32 distance = mFabs(upperLeft.mX - closestX);
      
      // Snap line closest to the right.
      F32 closestX2 = mFloor((lowerRight.mX / xSnap) + 0.5) * xSnap;
      F32 distance2 = mFabs(lowerRight.mX - closestX2);

      // Guide closest to the left.
      F32 closestGuide = sceneEdit->getClosestXGuide( upperLeft.mX );
      F32 distance3 = mFabs( upperLeft.mX - closestGuide );

      // Guide closest to the right.
      F32 closestGuide2 = sceneEdit->getClosestXGuide( lowerRight.mX );
      F32 distance4 = mFabs( lowerRight.mX - closestGuide2 );

      // Grab the smallest distance.
      F32 smallest = distance;
      F32 offset = closestX;
      S32 side = 1;
      if( distance2 < smallest )
      {
         smallest = distance2;
         offset = closestX2;
         side = -1;
      }
      if( distance3 < smallest )
      {
         smallest = distance3;
         offset = closestGuide;
         side = 1;
      }
      if( distance4 < smallest )
      {
         smallest = distance4;
         offset = closestGuide2;
         side = -1;
      }

      if (smallest < snapThreshold)
         finalPosition.mX = offset + ( halfSize.mX * side );
   }
   
   if (sceneEdit->isGridSnapY() && sceneEdit->getSnapToGridY())
   {
      F32 ySnap = sceneEdit->getGridSnapY();
      F32 closestY = mFloor((upperLeft.mY / ySnap) + 0.5) * ySnap;
      F32 distance = mFabs(upperLeft.mY - closestY);
      
      F32 closestY2 = mFloor((lowerRight.mY / ySnap) + 0.5) * ySnap;
      F32 distance2 = mFabs(lowerRight.mY - closestY2);

      // Guide closest to the top.
      F32 closestGuide = sceneEdit->getClosestYGuide( upperLeft.mY );
      F32 distance3 = mFabs( upperLeft.mY - closestGuide );

      // Guide closest to the bottom.
      F32 closestGuide2 = sceneEdit->getClosestYGuide( lowerRight.mY );
      F32 distance4 = mFabs( lowerRight.mY - closestGuide2 );

      // Grab the smallest distance.
      F32 smallest = distance;
      F32 offset = closestY;
      S32 side = 1;
      if( distance2 < smallest )
      {
         smallest = distance2;
         offset = closestY2;
         side = -1;
      }
      if( distance3 < smallest )
      {
         smallest = distance3;
         offset = closestGuide;
         side = 1;
      }
      if( distance4 < smallest )
      {
         smallest = distance4;
         offset = closestGuide2;
         side = -1;
      }

      if (smallest < snapThreshold)
         finalPosition.mY = offset + ( halfSize.mY * side );
   }
}

//-----------------------------------------------------------------------------
// scale
//-----------------------------------------------------------------------------
void LevelBuilderBaseEditTool::scale(LevelBuilderSceneEdit* sceneEdit, t2dVector size, t2dVector pos, t2dVector mousePoint2D, bool uniform, bool maintainAR, F32 ar, t2dVector& newSize, t2dVector& newPosition, bool& flipX, bool& flipY)
{
   PROFILE_SCOPE(LevelBuilderBaseEditTool_scale);

   AssertFatal(sceneEdit, "LevelBuilderBaseEditTool - Editing objects without a valid scene edit context.");

   //F32 ar = size.mX / size.mY;

   // Snap To Grid
   t2dVector mousePoint = mousePoint2D;
   F32 snapThreshold = sceneEdit->getSnapThreshold();
   if (sceneEdit->getSnapToGridX())
   {
      F32 xSnap = sceneEdit->getGridSnapX();
      F32 closestX = mFloor((mousePoint.mX / xSnap) + 0.5) * xSnap;
      if (mFabs(mousePoint.mX - closestX) < snapThreshold)
         mousePoint.mX = closestX;
   }
   
   if (sceneEdit->getSnapToGridY())
   {
      F32 ySnap = sceneEdit->getGridSnapY();
      F32 closestY = mFloor((mousePoint.mY / ySnap) + 0.5) * ySnap;
      if (mFabs(mousePoint.mY - closestY) < snapThreshold)
         mousePoint.mY = closestY;
   }

   // Find The Scale Change
   t2dVector scaleDelta = t2dVector(0.0f, 0.0f);
   if( mSizingState & SizingLeft )
      scaleDelta.mX = mousePoint.mX - (pos.mX - (size.mX * 0.5));
   else if( mSizingState & SizingRight )
      scaleDelta.mX = mousePoint.mX - (pos.mX + (size.mX * 0.5));

   if( mSizingState & SizingTop )
      scaleDelta.mY = mousePoint.mY - (pos.mY - (size.mY * 0.5));
   else if( mSizingState & SizingBottom )
      scaleDelta.mY = mousePoint.mY - (pos.mY + (size.mY * 0.5));

   if (uniform)
      scaleDelta *= 2.0f;

   // Apply The Scale Change
   newSize = size;
   newPosition = pos;
   if( mSizingState & SizingLeft )
   {
      newSize.mX -= scaleDelta.mX;
      newPosition.mX += scaleDelta.mX * 0.5;
   }
   else if( mSizingState & SizingRight )
   {
      newSize.mX += scaleDelta.mX;
      newPosition.mX += scaleDelta.mX * 0.5;
   }

   if( mSizingState & SizingTop )
   {
      newSize.mY -= scaleDelta.mY;
      newPosition.mY += scaleDelta.mY * 0.5;
   }
   else if( mSizingState & SizingBottom )
   {
      newSize.mY += scaleDelta.mY;
      newPosition.mY += scaleDelta.mY * 0.5;
   }

   // If the new size is small, don't change anything. This needs some fixing up still.
   if (((mFabs(newSize.mX) < 0.001) && (sceneEdit->getSnapToGridX())) ||
       ((mFabs(newSize.mY) < 0.001) && (sceneEdit->getSnapToGridY())))
   {
      newSize = size;
      newPosition = pos;
      flipX = flipY = false;
      return;
   }

   // FlipX
   flipX = flipY = false;
   if (newSize.mX < 0.0f)
   {
      if (mSizingState & SizingLeft)
      {
         mSizingState &= ~SizingLeft;
         mSizingState |= SizingRight;
      }
      else if (mSizingState & SizingRight)
      {
         mSizingState &= ~SizingRight;
         mSizingState |= SizingLeft;
      }
      flipX = true;
      newSize.mX = -newSize.mX;
   }

   // FlipY
   if (newSize.mY < 0.0f)
   {
      if (mSizingState & SizingTop)
      {
         mSizingState &= ~SizingTop;
         mSizingState |= SizingBottom;
      }
      else if (mSizingState & SizingBottom)
      {
         mSizingState &= ~SizingBottom;
         mSizingState |= SizingTop;
      }
      flipY = true;
      newSize.mY = -newSize.mY;
   }

   // Maintain Aspect Ratio
   if (maintainAR)
   {
      F32 newAR = newSize.mX / newSize.mY;
      t2dVector oldNewSize = newSize;

      if (newAR < ar)
      {
         if ((newSize.mX < size.mX) && !(mSizingState & SizingBottom) && !(mSizingState & SizingTop))
            newSize.mY *= newAR / ar;
         else
            newSize.mX *= ar / newAR;
      }

      else
      {
         if ((newSize.mY < size.mY) && !(mSizingState & SizingLeft) && !(mSizingState & SizingRight))
            newSize.mX *= ar / newAR;
         else
            newSize.mY *= newAR / ar;
      }
      
      if( mSizingState & SizingLeft )
      {
         newPosition.mX -= (newSize.mX - oldNewSize.mX) * 0.5;
      }
      else if( mSizingState & SizingRight )
      {
         newPosition.mX += (newSize.mX - oldNewSize.mX) * 0.5;
      }

      if( mSizingState & SizingTop )
      {
         newPosition.mY -= (newSize.mY - oldNewSize.mY) * 0.5;
      }
      else if( mSizingState & SizingBottom )
      {
         newPosition.mY += (newSize.mY - oldNewSize.mY) * 0.5;
      }
   }

   // Uniform
   if (uniform)
      newPosition = pos;
}

//-----------------------------------------------------------------------------
// nudge
//-----------------------------------------------------------------------------
void LevelBuilderBaseEditTool::nudge(t2dVector pos, S32 directionX, S32 directionY, bool fast, t2dVector& newPos)
{
   PROFILE_SCOPE(LevelBuilderBaseEditTool_nudge);
   newPos = pos;
   t2dVector kDirection((F32)directionX, (F32)directionY);

   if (fast)
      kDirection *= 10.0f;
   
   newPos += kDirection;
}

//-----------------------------------------------------------------------------
// getSizingState
//-----------------------------------------------------------------------------
S32 LevelBuilderBaseEditTool::getSizingState(LevelBuilderSceneWindow* sceneWindow, const Point2I &pt, const RectF &rect)
{
   PROFILE_SCOPE(LevelBuilderBaseEditTool_getSizingState);
   if( !rect.isValidRect() )
      return SizingNone;

   t2dVector upperLeft = t2dVector(rect.point);
   t2dVector lowerRight = t2dVector(rect.point + rect.extent);

   // Convert to window coords.
   t2dVector windowUpperLeft, windowLowerRight;
   sceneWindow->sceneToWindowCoord(upperLeft, windowUpperLeft);
   sceneWindow->sceneToWindowCoord(lowerRight, windowLowerRight);

   RectI box = RectI(S32(windowUpperLeft.mX), S32(windowUpperLeft.mY),
                     S32(windowLowerRight.mX - windowUpperLeft.mX),
                     S32(windowLowerRight.mY - windowUpperLeft.mY));

   S32 lx = box.point.x, rx = box.point.x + box.extent.x - 1;
   S32 cx = (lx + rx) >> 1;
   S32 ty = box.point.y, by = box.point.y + box.extent.y - 1;
   S32 cy = (ty + by) >> 1;

   if (inNut(pt, lx, ty))
      return SizingLeft | SizingTop;
   if (inNut(pt, cx, ty))
      return SizingTop;
   if (inNut(pt, rx, ty))
      return SizingRight | SizingTop;
   if (inNut(pt, lx, by))
      return SizingLeft | SizingBottom;
   if (inNut(pt, cx, by))
      return SizingBottom;
   if (inNut(pt, rx, by))
      return SizingRight | SizingBottom;
   if (inNut(pt, lx, cy))
      return SizingLeft;
   if (inNut(pt, rx, cy))
      return SizingRight;

   return SizingNone;
}

//-----------------------------------------------------------------------------
// drawSizingNuts
//-----------------------------------------------------------------------------
void LevelBuilderBaseEditTool::drawSizingNuts(LevelBuilderSceneWindow* sceneWindow, const RectF& rect)
{
   PROFILE_SCOPE(LevelBuilderBaseEditTool_drawSizingNuts);
   t2dVector upperLeft = t2dVector(rect.point);
   t2dVector lowerRight = t2dVector(rect.point + rect.extent);

   // Convert to window coords.
   t2dVector windowUpperLeft, windowLowerRight;
   sceneWindow->sceneToWindowCoord(upperLeft, windowUpperLeft);
   sceneWindow->sceneToWindowCoord(lowerRight, windowLowerRight);
   windowUpperLeft = sceneWindow->localToGlobalCoord(Point2I(S32(windowUpperLeft.mX), S32(windowUpperLeft.mY)));
   windowLowerRight = sceneWindow->localToGlobalCoord(Point2I(S32(windowLowerRight.mX), S32(windowLowerRight.mY)));

   RectI selectionRect = RectI(S32(windowUpperLeft.mX), S32(windowUpperLeft.mY),
                               S32(windowLowerRight.mX - windowUpperLeft.mX),
                               S32(windowLowerRight.mY - windowUpperLeft.mY));

   // Top Left Sizing Knob
   drawNut( selectionRect.point);
   // Middle Left Sizing Knob
   drawNut( Point2I( selectionRect.point.x, selectionRect.point.y + ( selectionRect.extent.y / 2 ) ));
   // Bottom Left Sizing Knob
   drawNut( Point2I( selectionRect.point.x, selectionRect.point.y + selectionRect.extent.y ));
   // Bottom Right Sizing Knob
   drawNut( selectionRect.point + selectionRect.extent);
   // Middle Right Sizing Knob
   drawNut( Point2I( selectionRect.point.x + selectionRect.extent.x , selectionRect.point.y + ( selectionRect.extent.y / 2 ) ));
   // Top Right Sizing Knob
   drawNut( Point2I( selectionRect.point.x + selectionRect.extent.x , selectionRect.point.y ));
   // Top Middle Sizing Knob
   drawNut( Point2I( selectionRect.point.x + ( selectionRect.extent.x / 2) , selectionRect.point.y ));
   // Bottom Middle Sizing Knob
   drawNut( Point2I( selectionRect.point.x + ( selectionRect.extent.x / 2) , selectionRect.point.y + selectionRect.extent.y ));
}

#endif // TORQUE_TOOLS
