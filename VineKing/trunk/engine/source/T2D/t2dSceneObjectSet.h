//---------------------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//---------------------------------------------------------------------------------------------

#include "console/simBase.h"
#include "core/tVector.h"
#include "T2D/t2dVector.h"
#include "T2D/t2dPhysics.h"
#include "core/color.h"

class t2dSceneObject;

#ifndef  _T2DSCENEOBJECTSET_H_
#define  _T2DSCENEOBJECTSET_H_

class t2dSceneObjectSet : public SimSet
{
private:
   typedef SimSet Parent;

   RectF mObjectRect;
   F32 mRotation;

   bool mFlipX;
   bool mFlipY;
   S32 mGraphGroup;
   S32 mLayer;

public:
   DECLARE_CONOBJECT(t2dSceneObjectSet);

   t2dSceneObjectSet();
   ~t2dSceneObjectSet();

   void setPosition(t2dVector position);
   void setSize(t2dVector size);
   void setRotation(F32 rotation);
   void rotate(F32 rotation);
   void flipX();
   void flipY();
   void setGraphGroup(S32 group);
   void setLayer(S32 layer);

   void addObject(SimObject* object);
   void removeObject(SimObject* object);
   
   void getSceneObjects(Vector<t2dSceneObject*>& objects) const;

   void calculateObjectRect();
   RectF getBoundingRect() const { return mObjectRect; };
   t2dVector getPosition() const { return mObjectRect.point + (mObjectRect.extent * 0.5); };
   t2dVector getSize() const { return mObjectRect.extent; };
   F32 getRotation() const;
   bool getFlipX() const { return mFlipX; };
   bool getFlipY() const { return mFlipY; };
   S32 getGraphGroup();
   S32 getLayer();
};

#endif
