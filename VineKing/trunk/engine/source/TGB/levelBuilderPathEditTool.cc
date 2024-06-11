#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Path editing tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderPathEditTool.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderPathEditTool);

LevelBuilderPathEditTool::LevelBuilderPathEditTool() : LevelBuilderBaseEditTool(),
                                                       mPath(NULL),
                                                       mSceneWindow(NULL),
                                                       mUndoAction(NULL),
                                                       mAddUndo(false),
                                                       mDraggingNode(-1),
                                                       mDraggingHandle(0)
{
   // Set our tool name
   mToolName            = StringTable->insert("Path Tool");
}

LevelBuilderPathEditTool::~LevelBuilderPathEditTool()
{
}

bool LevelBuilderPathEditTool::onActivate(LevelBuilderSceneWindow* sceneWindow)
{
   if (!Parent::onActivate(sceneWindow))
      return false;

   mSceneWindow = sceneWindow;
   mPath = NULL;

   return true;
}

void LevelBuilderPathEditTool::onDeactivate()
{
   finishEdit();

   mUndoAction = NULL;

   mPath = NULL;
   mSceneWindow = NULL;
   Parent::onDeactivate();
}

void LevelBuilderPathEditTool::editObject(t2dPath* object)
{
   if (!mSceneWindow)
      return;

   mPath = object;
}

ConsoleMethod(LevelBuilderPathEditTool, editObject, void, 3, 3, "(obj) Selects an object for editing.\n"
			  "@param The object you wish to edit.\n"
			  "@return No return value.")
{
   t2dPath* obj = dynamic_cast<t2dPath*>(Sim::findObject(argv[2]));
   if (obj)
      object->editObject(obj);
   else
      Con::warnf("Invalid object passed to LevelBuilderPathedTool::editObject");
}

void LevelBuilderPathEditTool::finishEdit()
{
   if (!mPath || !mSceneWindow)
      return;

   mPath->updateSize();
   mSceneWindow->getSceneEdit()->onObjectSpatialChanged(mPath);
   mPath = NULL;
}

ConsoleMethod(LevelBuilderPathEditTool, finishEdit, void, 2, 2, "() Applies changes and ends editing of an object.\n"
			  "@return No return value.")
{
   object->finishEdit();
}

bool LevelBuilderPathEditTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mPath || (sceneWindow != mSceneWindow))
      return Parent::onMouseMove(sceneWindow, mouseStatus);

   mAddUndo = false;
   mUndoAction = NULL;
   mDraggingNode = -1;
   mDraggingHandle = 0;

   // Try to find a bezier handle first.
   if (mPath->getPathType() == T2D_FOLLOW_BEZIER)
   {
      for (S32 i = 0; i < mPath->getNodeCount(); i++)
      {
         t2dPath::PathNode& node = mPath->getNode(i);
         mDraggingHandle = findBezierHandle(node, mouseStatus.mousePoint2D);
         if (mDraggingHandle)
         {
            mDraggingNode = i;
            mStartRotation = node.rotation;
            mRotationVector = mouseStatus.mousePoint2D - node.position;

            UndoPathNodeRotateAction* undo = new UndoPathNodeRotateAction(sceneWindow->getSceneEdit(), "Rotate Path Node");
            undo->setStartRotation(mPath, mDraggingNode, node.rotation, node.weight);
            mUndoAction = (UndoAction*)undo;

            break;
         }
      }
   }

   // Bezier handle not grabbed, find a node to drag.
   if (mDraggingNode == -1)
   {
      mDraggingNode = findPathNode(mPath, mouseStatus.mousePoint2D);
      if (mDraggingNode != -1)
      {
         UndoPathNodeMoveAction* undo = new UndoPathNodeMoveAction(mSceneWindow->getSceneEdit(), "Move Path Node");
         undo->setStartPosition(mPath, mDraggingNode, mPath->getNode(mDraggingNode).position);
         mUndoAction = (UndoAction*)undo;
      }
   }

   // No dragging node, so add a new one.
   if (mDraggingNode == -1)
   {
      S32 addIndex = findClosestNode(mPath, mouseStatus.mousePoint2D);
      if (mPath->getNodeCount() > 1)
      {
         S32 previous = addIndex - 1;
         if (previous < 0)
            previous = mPath->getNodeCount() - 1;

         S32 next = addIndex + 1;
         if (next >= mPath->getNodeCount())
            next = 0;

         F32 addAngle = mAtan(mouseStatus.mousePoint2D.mX - mPath->getNode(addIndex).position.mX,
                              mouseStatus.mousePoint2D.mY - mPath->getNode(addIndex).position.mY);

         F32 prevAngle = mAtan(mouseStatus.mousePoint2D.mX - mPath->getNode(previous).position.mX,
                              mouseStatus.mousePoint2D.mY - mPath->getNode(previous).position.mY);

         F32 nextAngle = mAtan(mouseStatus.mousePoint2D.mX - mPath->getNode(next).position.mX,
                              mouseStatus.mousePoint2D.mY - mPath->getNode(next).position.mY);

         if (mFabs(addAngle - prevAngle) < mFabs(addAngle - nextAngle))
            addIndex = next;
         else
            addIndex = previous + 1;
      }

      mPath->addNode(mouseStatus.mousePoint2D, 0.0f, 10.0f, addIndex);
      mDraggingNode = addIndex;
      
      mAddUndo = true;
      UndoPathNodeAddAction* undo = new UndoPathNodeAddAction(mSceneWindow->getSceneEdit(), "Add Path Node");
      undo->setNode(mPath, addIndex, mouseStatus.mousePoint2D, 0.0f, 10.0f);
      mUndoAction = (UndoAction*)undo;
   }

   return true;
}

bool LevelBuilderPathEditTool::onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mPath || (sceneWindow != mSceneWindow))
      return Parent::onMouseMove(sceneWindow, mouseStatus);

   if (mDraggingHandle)
   {
      mAddUndo = true;
      t2dPath::PathNode& node = mPath->getNode(mDraggingNode);
      rotate(sceneWindow->getSceneEdit(), mStartRotation, mRotationVector, mouseStatus.mousePoint2D - node.position, node.rotation);
      node.weight = (mouseStatus.mousePoint2D - node.position).len() * 2.0f;
   }

   else if (mDraggingNode != -1)
   {
      mAddUndo = true;
      move(sceneWindow->getSceneEdit(), t2dVector(0.0f, 0.0f), mouseStatus.mousePoint2D, mPath->getNode(mDraggingNode).position);
   }

   return true;
}

bool LevelBuilderPathEditTool::onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mPath || (sceneWindow != mSceneWindow))
      return Parent::onMouseMove(sceneWindow, mouseStatus);

   if (mAddUndo)
   {
      UndoPathNodeMoveAction* undoMove = dynamic_cast<UndoPathNodeMoveAction*>(mUndoAction);
      UndoPathNodeAddAction* undoAdd = dynamic_cast<UndoPathNodeAddAction*>(mUndoAction);
      UndoPathNodeRotateAction* undoRotate = dynamic_cast<UndoPathNodeRotateAction*>(mUndoAction);
      if (undoMove)
      {
         undoMove->setEndPosition(mPath->getNode(mDraggingNode).position);
         undoMove->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
      }
      else if (undoAdd)
      {
         t2dPath::PathNode& node = mPath->getNode(mDraggingNode);
         undoAdd->setNode(mPath, mDraggingNode, node.position, node.rotation, node.weight);
         undoAdd->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
      }
      else if (undoRotate)
      {
         t2dPath::PathNode& node = mPath->getNode(mDraggingNode);
         undoRotate->setEndRotation(node.rotation, node.weight);
         undoRotate->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
      }
      else if (mUndoAction)
         delete mUndoAction;
   }
   else if (mUndoAction)
      delete mUndoAction;

   mAddUndo = false;
   mUndoAction = NULL;
   mDraggingNode = -1;

   return true;
}

bool LevelBuilderPathEditTool::onRightMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!mPath || (sceneWindow != mSceneWindow))
      return Parent::onMouseMove(sceneWindow, mouseStatus);

   S32 nodeIndex = findPathNode(mPath, mouseStatus.mousePoint2D);
   if (nodeIndex != -1)
   {
      UndoPathNodeRemoveAction* undo = new UndoPathNodeRemoveAction(sceneWindow->getSceneEdit(), "Remove Path Node");
      t2dPath::PathNode& node = mPath->getNode(nodeIndex);
      undo->setNode(mPath, nodeIndex, node.position, node.rotation, node.weight);

      mPath->removeNode(nodeIndex);

      undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());
   }

   return true;
}

void LevelBuilderPathEditTool::onRenderGraph( LevelBuilderSceneWindow* sceneWindow )
{
   // Render Parent
   Parent::onRenderGraph( sceneWindow );

   if (!mPath || (sceneWindow != mSceneWindow))
      return;

   if (mPath->getPathType() == T2D_FOLLOW_BEZIER)
   {
      for (S32 i = 0; i < mPath->getNodeCount(); i++)
      {
         t2dPath::PathNode& node = mPath->getNode(i);

         // Draw a line representing the weight and rotation.
         F32 rotation = mDegToRad(node.rotation - 90.0f);
         t2dVector point1 = node.position - (t2dVector(mCos(rotation), mSin(rotation)) * node.weight * 0.5f);
         t2dVector point2 = node.position + (t2dVector(mCos(rotation), mSin(rotation)) * node.weight * 0.5f);
         
         t2dVector windowPoint1, windowPoint2;
         sceneWindow->sceneToWindowCoord(point1, windowPoint1);
         sceneWindow->sceneToWindowCoord(point2, windowPoint2);

         Point2I window1 = sceneWindow->localToGlobalCoord(Point2I(windowPoint1.mX, windowPoint1.mY));
         Point2I window2 = sceneWindow->localToGlobalCoord(Point2I(windowPoint2.mX, windowPoint2.mY));
         dglDrawLine(window1, window2, ColorI(255, 255, 255));
         dglDrawRectFill(window1 - Point2I(2, 2), window1 + Point2I(2, 2), ColorI(255, 255, 255));
         dglDrawRectFill(window2 - Point2I(2, 2), window2 + Point2I(2, 2), ColorI(255, 255, 255));
      }
   }
}

S32 LevelBuilderPathEditTool::findClosestNode(t2dPath *path, t2dVector position)
{
   S32 closestNode = -1;
   F32 closestLength;
   for (S32 i = 0; i < path->getNodeCount(); i++)
   {
      t2dPath::PathNode node = path->getNode(i);
      t2dVector nodePosition = node.position;
      F32 length = (nodePosition - position).len();

      if ((i == 0) || (length < closestLength))
      {
         closestNode = i;
         closestLength = length;
      }
   }
   return closestNode;
}

S32 LevelBuilderPathEditTool::findPathNode(t2dPath* path, t2dVector position)
{
   for (S32 i = 0; i < path->getNodeCount(); i++)
   {
      t2dPath::PathNode node = path->getNode(i);
      t2dVector nodeSize = t2dVector(path->getNodeRenderSize(), path->getNodeRenderSize());
      t2dVector nodePosition = node.position;
      t2dVector upperLeft = nodePosition - nodeSize;
      t2dVector lowerRight = nodePosition + nodeSize;

      if ((position.mX > upperLeft.mX) && (position.mY > upperLeft.mY) &&
          (position.mX < lowerRight.mX) && (position.mY < lowerRight.mY))
      {
         return i;
      }
   }
   return -1;
}

S32 LevelBuilderPathEditTool::findBezierHandle(t2dPath::PathNode& node, t2dVector position)
{
   F32 rotation = mDegToRad(node.rotation - 90.0f);
   t2dVector point1 = node.position - (t2dVector(mCos(rotation), mSin(rotation)) * node.weight * 0.5f);
   t2dVector point2 = node.position + (t2dVector(mCos(rotation), mSin(rotation)) * node.weight * 0.5f);

   if ((position - point1).len() < 3.0f)
      return -1;
   else if ((position - point2).len() < 3.0f)
      return 1;

   return 0;
}


#endif // TORQUE_TOOLS
