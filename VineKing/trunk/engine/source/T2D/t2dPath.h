#ifndef _T2DPATH_H_
#define _T2DPATH_H_

#include "T2D/t2dSceneObject.h"

class t2dPath;

//---------------------------------------------------------------------------------------------
// Enumeration of the actions to take upon reaching the end node of the path.
//---------------------------------------------------------------------------------------------
enum ePathMode
{
   T2D_PATH_WRAP,                      // Loop.
   T2D_PATH_REVERSE,                   // Reverse directions at the end node.
   T2D_PATH_RESTART                    // Warp to the start node and restart.
};

//---------------------------------------------------------------------------------------------
// Enumeration of the different path following methods.
//---------------------------------------------------------------------------------------------
enum eFollowMethod
{
   T2D_FOLLOW_LINEAR,
   T2D_FOLLOW_BEZIER,
   T2D_FOLLOW_CATMULL,
   T2D_FOLLOW_CUSTOM
};

class PathedObject
{
public:
   PathedObject();
   ~PathedObject() {};

   // Set Attributes
   void setSpeed(F32 speed, bool reset = false) { mSpeed = speed; if (reset) resetObject(); };
   void setDirection(S32 direction, bool reset = false) { mDirection = direction; if (reset) resetObject(); };
   void setStartDirection(S32 direction, bool reset = false) { mStartDirection = direction; if (reset) resetObject(); };
   void setLoop(S32 loop, bool reset = false) { mLoopCounter = loop; if (reset) resetObject(); };
   void setTotalLoops(S32 loops, bool reset = false) { mTotalLoops = loops; if (reset) resetObject(); };
   void setOrientToPath(bool orientToPath, bool reset = false) { mOrientToPath = orientToPath; if (reset) resetObject(); };
   void setPathMode(ePathMode pathMode, bool reset = false) { mPathMode = pathMode; if (reset) resetObject(); };
   void setRotationOffset(F32 offset, bool reset = false) { mRotationOffset = offset; if (reset) resetObject(); };
   inline void resetObject();
   inline bool sendToNode(S32 index);
   inline void setStartNode(S32 startNode, bool reset = false);
   inline void setEndNode(S32 endNode, bool reset = false);

   // Get attribute methods.
   inline F32 getSpeed() const { return mSpeed; };
   inline S32 getDirection() const { return mDirection; };
   inline S32 getStartNode() const { return mStartNode; };
   inline S32 getEndNode() const { return mEndNode; };
   inline S32 getCurrentNode() const { return mSourceNode; };
   inline S32 getDestinationNode() const { return mDestinationNode; };
   inline S32 getLoop() const { return mLoopCounter; };
   inline S32 getTotalLoops() const { return mTotalLoops; };
   inline bool getOrientToPath() const { return mOrientToPath; };
   inline ePathMode getPathMode() const { return mPathMode; };
   inline F32 getRotationOffset() const { return mRotationOffset; };

private:
   friend class t2dPath;

   void setCurrentNode(S32 node);
   void setDestinationNode(S32 node);

   t2dPath* mPath;                       // The path this is attached to.
   SimObjectPtr<t2dSceneObject> mObject; // The object this is wrapping.
   S32 mSourceNode;                      // The most recently passed node.
   S32 mDestinationNode;                 // The node being headed toward.
   S32 mStartNode;                       // The node at the beginning of the path.
   S32 mEndNode;                         // The node at the end of the path.
   F32 mSpeed;                           // The speed to move the object at.
   S32 mStartDirection;                  // The direction the object is initially traveling.
   S32 mDirection;                       // The direction the object is traveling (-1 or 1).
   bool mOrientToPath;                   // Follow the orientation of the path.
   S32 mLoopCounter;                     // The current loop the object is on.
   S32 mTotalLoops;                      // The number of loops to take around the path.
   F32 mTime;                            // The parametric time of the location on the path.
   ePathMode mPathMode;                  // The action to take upon path completion.
   F32 mRotationOffset;					     // The rotation offset of the object when using orient to path.

   //[neo, 5/22/2007 - #3139]
   // Used as a marker so we can map the object to this instance
   // By the time onDeleteNotify is called the mObject ref has already
   // been zeroed by the notification code so we can't use that.
   SimObjectId mObjectId;                       
};

//---------------------------------------------------------------------------------------------
// t2dPath
//---------------------------------------------------------------------------------------------
class t2dPath : public t2dSceneObject
{
   typedef t2dSceneObject Parent;

public:
   struct PathNode
   {
      // Constructor.
      PathNode(t2dVector _position, F32 _rotation, F32 _weight)
      {
         position = _position;
         rotation = _rotation;
         weight = _weight;
         bezierLength = 0.0f;
         catmullLength = 0.0f;
      };

      t2dVector position;                 // The position of the node.
      F32 rotation;                       // The rotation of the node.
      F32 weight;                         // The weight of the node.
      F32 bezierLength;                   // The bezier length from this node to the next.
      F32 catmullLength;                  // The catmull length from this node to the next.
   };

   // Constructor/Destructor.
   t2dPath();
   virtual ~t2dPath();

   virtual void onDeleteNotify( SimObject* object );

   static void initPersistFields();
   static bool setPathType(void* obj, const char* data);

   ePathMode mPathModeField;
   eFollowMethod mPathTypeField;

   virtual void onRemove();

   // Handles the path following.
   virtual void preIntegrate(const F32 sceneTime, const F32 elapsedTime, CDebugStats *pDebugStats);
   // For editor support. Resets pathed objects.
   virtual void integrateObject(const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats);
   // Renders the path when debug modes are set.
   virtual void renderObject(const RectF& viewPort, const RectF& viewIntersection);

   virtual void setPosition(const t2dVector& position);
   virtual void setSize(const t2dVector& size);

   // Update the size and position to encompass all nodes.
   void updateSize();

   // Node management.
   S32 addNode(t2dVector position, F32 rotation, F32 weight, S32 location = -1);
   // Returns the node index or -1 if no matches
   S32 findNode( t2dVector position, F32 rotation, F32 weight );
   S32 removeNode(U32 index);
   void clear();
   S32 getNodeCount() const { return mNodes.size(); };

   void setPathType(eFollowMethod pathType) { mPathType = pathType; };
   eFollowMethod getPathType() const        { return mPathType; };

   void setNodeRenderSize(F32 size) { mNodeRenderSize = size; };
   F32 getNodeRenderSize() const { return mNodeRenderSize; };

   // Add and remove objects from the path.
   void attachObject(t2dSceneObject* object, F32 speed, S32 direction, bool orientToPath,
                     S32 startNode, S32 endNode, ePathMode pathMode, S32 loops, bool sendToStart);
   void detachObject(t2dSceneObject* object);

   S32 getPathedObjectCount()
   {
      return mObjects.size();
   };

   t2dSceneObject* getPathedObject( U32 index )
   {
      if( index < mObjects.size() )
         return mObjects[index]->mObject;

      return NULL;
   };

   // Get the pathed object that represents a scene object.
   inline PathedObject* getPathedObject(const t2dSceneObject* obj)
   {
      Vector<PathedObject*>::iterator i;
      for (i = mObjects.begin(); i != mObjects.end(); ++i)
         if ((*i)->mObject == obj) return *i;

      return NULL;
   };

   // Set Properties on Pathed Objects.
   void setStartNode(t2dSceneObject* object, S32 node)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setStartNode(node, true);
   }
   void setEndNode(t2dSceneObject* object, S32 node)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setEndNode(node, true);
   }
   void setSpeed(t2dSceneObject* object, F32 speed, bool resetObject)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setSpeed(speed, resetObject);
   }
   void setMoveForward(t2dSceneObject* object, bool forward, bool resetObject)
   {
      setDirection(object, forward ? 1 : -1, resetObject);
   }
   void setDirection(t2dSceneObject* object, S32 direction, bool resetObject)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setStartDirection(direction, resetObject);
   }
   void setOrient(t2dSceneObject* object, bool orient, bool resetObject)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setOrientToPath(orient, resetObject);
   }
   void setRotationOffset(t2dSceneObject* object, F32 offset, bool resetObject)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setRotationOffset(offset, resetObject);
   }
   void setLoops(t2dSceneObject* object, S32 loops, bool resetObject)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setTotalLoops(loops, resetObject);
   }
   void setFollowMode(t2dSceneObject* object, ePathMode followMode, bool resetObject)
   {
      PathedObject* pathedObject = getPathedObject(object);
      if (pathedObject) pathedObject->setPathMode(followMode, resetObject);
   }

   // Get Properties on Pathed Objects.
   S32 getStartNode(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getStartNode() : -1;
   }
   S32 getEndNode(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getEndNode() : -1;
   }
   F32 getSpeed(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getSpeed() : 0.0f;
   }
   bool getMoveForward(t2dSceneObject* object)
   {
      return getDirection(object) == 1 ? true : false;
   }
   S32 getDirection(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getDirection() : 0;
   }
   bool getOrient(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getOrientToPath() : false;
   }
   F32 getRotationOffset(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getRotationOffset() : false;
   }
   S32 getLoops(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getTotalLoops() : -1;
   }
   ePathMode getFollowMode(t2dSceneObject* object)
   {
      PathedObject* pathedObject = getPathedObject(object);
      return pathedObject ? pathedObject->getPathMode() : T2D_PATH_WRAP;
   }

   inline PathNode& getNode(S32 index)
   {
      if (isValidNode(index)) return mNodes[index];
      return mNodes[0];
   };

   inline bool isValidNode(S32 index)
   {
      if (mNodes.empty()) return false;
      if ((index >= 0) && (index < mNodes.size())) return true;
      return false;
   };
    
    // RKS NOTE: I added this so The Finger could be reattached in the right place (2012/07/02)
    void GetNodePosition( S32 index, t2dVector& _outVector );

   DECLARE_CONOBJECT(t2dPath);

   virtual void readFieldsPostLoad();
   virtual void writeFields(Stream &stream, U32 tabStop);

private:
   // Make sure all the PathedObjects still reference valid nodes.
   void checkObjectNodes();

   // Traversal methods.
   void linear(PathedObject& object);
   void bezier(PathedObject& object, F32 dt);
   void catmull(PathedObject& object, F32 dt);
   void custom(PathedObject& object);

   // Calculate the length from one node to the next based on the interpolation type.
   void calculateBezierLength(S32 node);
   void calculateCatmullLength(S32 node);

   // The type of path.
   eFollowMethod mPathType;
   bool mNodesLoaded;
   Vector<S32> mObjectsLoaded;
   S32 mMountOffset;

   bool mFinished;

   // The size to render the path nodes at.
   F32 mNodeRenderSize;

   // The objects and nodes.
   // [neo, 5/22/2007 - #3139]
   // PathedObject::mObject is a SimObjectPtr and calling erase_fast() etc or any reallocation
   // will invalidate the reference stored with the simobject it points to and so the object
   // will have a reference to garbage memory. We could just manually copy it in the specified
   // cases but this way we wont miss anything by accident.
   //Vector<PathedObject> mObjects;
   Vector<PathedObject*> mObjects;
   Vector<PathNode>      mNodes;
};

inline void PathedObject::resetObject()
{
   sendToNode(mStartNode);

   mTime = 0.0f;
   mLoopCounter = 0;
   mSourceNode = mStartNode;
   mDestinationNode = mStartNode;
   mDirection = mStartDirection;
}

inline bool PathedObject::sendToNode(S32 index)
{
   t2dPath::PathNode node = mPath->getNode(index);

   mObject->setPosition(node.position);
   if (mOrientToPath)
      mObject->setRotation(node.rotation);

   return true;
}

inline void PathedObject::setCurrentNode(S32 node)
{
   if (mPath->isValidNode(node))
      mSourceNode = node;
}

inline void PathedObject::setDestinationNode(S32 node)
{
   if (mPath->isValidNode(node))
      mDestinationNode = node;
}

inline void PathedObject::setStartNode(S32 startNode, bool reset)
{
   if (!mPath->isValidNode(startNode))
      return;

   mStartNode = startNode;
   if (reset) resetObject();
};

inline void PathedObject::setEndNode(S32 endNode, bool reset)
{
   if (!mPath->isValidNode(endNode))
      return;

   mEndNode = endNode;
   if (reset) resetObject();
}

#endif
