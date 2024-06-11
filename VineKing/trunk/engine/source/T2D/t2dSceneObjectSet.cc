//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

#include "T2D/t2dSceneObjectSet.h"
#include "T2D/t2dSceneObject.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(t2dSceneObjectSet);

extern EnumTable collisionDetectionTable;
extern EnumTable collisionResponseTable;
extern EnumTable srcBlendFactorTable;
extern EnumTable dstBlendFactorTable;

//---------------------------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------------------------
t2dSceneObjectSet::t2dSceneObjectSet() : mObjectRect(0.0f, 0.0f, 0.0f, 0.0f),
                                         mGraphGroup(0),
                                         mLayer(0),
                                         mFlipX(false),
                                         mFlipY(false),
                                         mRotation(0.0f)
{
}

//---------------------------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------------------------
t2dSceneObjectSet::~t2dSceneObjectSet()
{
}

void t2dSceneObjectSet::addObject(SimObject* object)
{
   Parent::addObject(object);

   mRotation = 0.0f;
   calculateObjectRect();
}

void t2dSceneObjectSet::removeObject(SimObject* object)
{
   Parent::removeObject(object);

   mRotation = 0.0f;
   calculateObjectRect();
}

void t2dSceneObjectSet::calculateObjectRect()
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   if (objects.size())
      mObjectRect = objects[0]->getWorldClipRectangle();
   else
      mObjectRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);

   F32 minX = mObjectRect.point.x;
   F32 minY = mObjectRect.point.y;
   F32 maxX = mObjectRect.point.x + mObjectRect.extent.x;
   F32 maxY = mObjectRect.point.y + mObjectRect.extent.y;

   for (S32 i = 1; i < objects.size(); i++)
   {
      t2dVector upperLeft = objects[i]->getWorldClipRectangle().point;
      t2dVector lowerRight = objects[i]->getWorldClipRectangle().point + objects[i]->getWorldClipRectangle().extent;
      minX = getMin(upperLeft.mX, minX);
      maxX = getMax(lowerRight.mX, maxX);
      minY = getMin(upperLeft.mY, minY);
      maxY = getMax(lowerRight.mY, maxY);
   }

   mObjectRect = RectF(minX, minY, maxX - minX, maxY - minY);
}

//---------------------------------------------------------------------------------------------
// getSceneObjects
//---------------------------------------------------------------------------------------------
void t2dSceneObjectSet::getSceneObjects(Vector<t2dSceneObject*>& objects) const
{
   for(S32 i = 0; i < size(); i++)
   {
      SimObject* object = at(i);

      t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
      if(sceneObject)
         objects.push_back(sceneObject);

      else
      {
         t2dSceneObjectGroup* sceneObjectGroup = dynamic_cast<t2dSceneObjectGroup*>(object);
         if(sceneObjectGroup)
            sceneObjectGroup->getSceneObjects(objects);

         else
         {
            t2dSceneObjectSet* sceneObjectSet = dynamic_cast<t2dSceneObjectSet*>(object);
            if(sceneObjectSet)
               sceneObjectSet->getSceneObjects(objects);
         }
      }
   }
}

void t2dSceneObjectSet::setPosition(t2dVector position)
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   t2dVector midPoint = mObjectRect.point + (mObjectRect.extent * 0.5);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];
      object->setPosition((object->getPosition() - midPoint) + position);
      object->updateSpatialConfig();
   }

   calculateObjectRect();
}

void t2dSceneObjectSet::setSize(t2dVector size)
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   Vector<t2dVector> positions;

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      // Objects rotated 90 or 270 need the axes switched.
      t2dVector newSize = object->getSize().div(mObjectRect.extent).mult(size);
      if (mIsEqual(object->getRotation(), 90.0f) || mIsEqual(object->getRotation(), 270.0f))
      {
         t2dVector rotatedObjectSize;
         rotatedObjectSize.mX = object->getSize().mY;
         rotatedObjectSize.mY = object->getSize().mX;
         newSize = rotatedObjectSize.div(mObjectRect.extent).mult(size);
         F32 temp = newSize.mX;
         newSize.mX = newSize.mY;
         newSize.mY = temp;
      }

      positions.push_back((object->getPosition() - getPosition()).div(mObjectRect.extent));
      object->setPosition(getPosition());
      object->setSize(newSize);
      object->updateSpatialConfig();
   }

   calculateObjectRect();
   
   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      object->setPosition(positions[i].mult(size) + getPosition());
      object->updateSpatialConfig();
   }
   
   calculateObjectRect();
}

void t2dSceneObjectSet::setRotation(F32 rotation)
{
   rotate(mDegToRad(rotation - mRotation));
}

void t2dSceneObjectSet::rotate(F32 rotation)
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      t2dVector offset = object->getPosition() - getPosition();
      F32 distance = offset.len();
      F32 rotationOffset = mAtan(offset.mX, offset.mY);
      t2dVector position = t2dVector(distance * mSin(rotationOffset - rotation), distance * mCos(rotationOffset - rotation));
      object->setPosition(getPosition() + position);

      object->setRotation(object->getRotation() + mRadToDeg(rotation));
      object->updateSpatialConfig();
   }
   
   mRotation += mRadToDeg(rotation);
   calculateObjectRect();
}

void t2dSceneObjectSet::flipX()
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      object->setFlip(!object->getFlipX(), object->getFlipY());
      t2dVector offset = object->getPosition() - getPosition();
      offset.mX = -offset.mX;
      object->setPosition(getPosition() + offset);

      object->updateSpatialConfig();
   }

   mFlipX = !mFlipX;
}

void t2dSceneObjectSet::flipY()
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      object->setFlip(object->getFlipX(), !object->getFlipY());
      t2dVector offset = object->getPosition() - getPosition();
      offset.mY = -offset.mY;
      object->setPosition(getPosition() + offset);

      object->updateSpatialConfig();
   }

   mFlipY = !mFlipY;
}

void t2dSceneObjectSet::setGraphGroup(S32 group)
{
   // Don't want to update unless a change actually occurs. Otherwise, object specific
   // changes could be reset.
   if (mGraphGroup == group)
      return;

   mGraphGroup = group;

   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];
      object->setGraphGroup(group);
   }
}

void t2dSceneObjectSet::setLayer(S32 layer)
{
   if (mLayer == layer)
      return;

   mLayer = layer;

   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];
      object->setLayer(layer);
   }
}

S32 t2dSceneObjectSet::getGraphGroup()
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      if (i == 0) mGraphGroup = object->getGraphGroup();
      else
      {
         if (object->getGraphGroup() != mGraphGroup)
            mGraphGroup = 0;
      }
   }

   return mGraphGroup;
}

S32 t2dSceneObjectSet::getLayer()
{
   Vector<t2dSceneObject*> objects;
   getSceneObjects(objects);

   for (S32 i = 0; i < objects.size(); i++)
   {
      t2dSceneObject* object = objects[i];

      if (i == 0) mLayer = object->getLayer();
      else
      {
         if (object->getLayer() != mLayer)
            mLayer = 0;
      }
   }

   return mLayer;
}

F32 t2dSceneObjectSet::getRotation() const
{
   if (size() == 1)
   {
      t2dSceneObject* object = dynamic_cast<t2dSceneObject*>(at(0));
      if (object)
         return object->getRotation();
   }
   return mRotation;
}

ConsoleMethod(t2dSceneObjectSet, setPositionX, void, 3, 3, "(xpos) Set the x component of the position of the object\n"
			  "@return No return value.")
{
   object->setPosition(t2dVector(dAtof(argv[2]), object->getPosition().mY));
}

ConsoleMethod(t2dSceneObjectSet, setPositionY, void, 3, 3, "(ypos) Set the y component of the position of the object\n"
			  "@return No return value.")
{
   object->setPosition(t2dVector(object->getPosition().mX, dAtof(argv[2])));
}

ConsoleMethod(t2dSceneObjectSet, setWidth, void, 3, 3, "(width) Set the width of the object\n"
			  "@return No return value.")
{
   object->setSize(t2dVector(dAtof(argv[2]), object->getSize().mY));
}

ConsoleMethod(t2dSceneObjectSet, setHeight, void, 3, 3, "(height) Set the height of the object\n"
			  "@return No return value.")
{
   object->setSize(t2dVector(object->getSize().mX, dAtof(argv[2])));
}

ConsoleMethod(t2dSceneObjectSet, setRotation, void, 3, 3, "(angle) Set the rotation angle of the object\n"
			  "@param The rotation angle in degrees"
			  "@return No return value.")
{
   object->setRotation(dAtof(argv[2]));
}

ConsoleMethod(t2dSceneObjectSet, setFlipX, void, 3, 3, "(flipX) Set the flipX flag\n"
			  "@return No return value.")
{
   object->flipX();
}

ConsoleMethod(t2dSceneObjectSet, setFlipY, void, 3, 3, "(flipY) Set the flipY flag\n"
			  "@return No return value.")
{
   object->flipY();
}

ConsoleMethod(t2dSceneObjectSet, setGraphGroup, void, 3, 3, "(graphgroup) Sets the graphgroup of the set.\n"
			  "@return No return value.")
{
   object->setGraphGroup(dAtoi(argv[2]));
}

ConsoleMethod(t2dSceneObjectSet, setLayer, void, 3, 3, "(layer) Sets the layer of the set.\n"
			  "@return No return value.")
{
   object->setLayer(dAtoi(argv[2]));
}

ConsoleMethod(t2dSceneObjectSet, getPosition, const char*, 2, 2, "()\n @return Returns position.")
{
   char* buffer = Con::getReturnBuffer( 32 );
   t2dVector pos = object->getPosition();
   dSprintf( buffer, 32, "%f %f", pos.mX, pos.mY );
   return buffer;
}

ConsoleMethod(t2dSceneObjectSet, getPositionX, F32, 2, 2, "()\n Returns x position.")
{
   return object->getPosition().mX;
}

ConsoleMethod(t2dSceneObjectSet, getPositionY, F32, 2, 2, "()\n @return Returns y position.")
{
   return object->getPosition().mY;
}

ConsoleMethod(t2dSceneObjectSet, getSize, const char*, 2, 2, "()\n @return Returns size.")
{
   char* buffer = Con::getReturnBuffer( 32 );
   t2dVector size = object->getSize();
   dSprintf( buffer, 32, "%f %f", size.mX, size.mY );
   return buffer;
}

ConsoleMethod(t2dSceneObjectSet, getWidth, F32, 2, 2, "()\n @return Returns width.")
{
   return object->getSize().mX;
}

ConsoleMethod(t2dSceneObjectSet, getHeight, F32, 2, 2, "()\n @return Returns height.")
{
   return object->getSize().mY;
}

ConsoleMethod(t2dSceneObjectSet, getRotation, F32, 2, 2, "()\n @return Returns rotation.")
{
   return object->getRotation();
}

ConsoleMethod(t2dSceneObjectSet, getFlipX, bool, 2, 2, "() \n @return Returns flip x.")
{
   return object->getFlipX();
}

ConsoleMethod(t2dSceneObjectSet, getFlipY, bool, 2, 2, "() \n @return Returns flip y.")
{
   return object->getFlipY();
}

ConsoleMethod(t2dSceneObjectSet, getGraphGroup, S32, 2, 2, "() \n @return Returns the graph group.")
{
   return object->getGraphGroup();
}

ConsoleMethod(t2dSceneObjectSet, getLayer, S32, 2, 2, "() \n @return Returns the layer.")
{
   return object->getLayer();
}
