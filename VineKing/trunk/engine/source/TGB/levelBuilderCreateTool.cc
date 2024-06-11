#ifndef TORQUE_PLAYER

//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Level Object Creation tool.
//---------------------------------------------------------------------------------------------

#include "console/console.h"
#include "dgl/dgl.h"
#include "TGB/levelBuilderCreateTool.h"
#include "T2D/t2dSceneObject.h"

// Implement Our Console Object
IMPLEMENT_CONOBJECT(LevelBuilderCreateTool);

LevelBuilderCreateTool::LevelBuilderCreateTool() : LevelBuilderBaseEditTool(), 
                                                   mOutlineColor( 255, 255, 255 ),
                                                   mCreatedObject( NULL ),
                                                   mDragStart(0.0f, 0.0f),
                                                   mObjectHidden(false),
                                                   mAcquireCreatedObjects(true),
                                                   mMouseDownAR(1.0f)
{
   // Set our tool name
   mToolName            = StringTable->insert("Create Tool");
   mScriptClassName     = StringTable->insert("");
   mScriptSuperClassName= StringTable->insert("");
   mConfigDatablockName = StringTable->insert("");
}

LevelBuilderCreateTool::~LevelBuilderCreateTool()
{
}

ConsoleMethod(LevelBuilderCreateTool, createObject, S32, 4, 4, "(sceneWindow, position) Creates a new object at given position"
			  "@param sceneWindow The destination sceneWindow.\n"
			  "@param position The desired position for the object.\n"
			  "@return Returns the new objects ID or NULL in given sceneWindow is invalid")
{
   LevelBuilderSceneWindow* sceneWindow = dynamic_cast<LevelBuilderSceneWindow*>(Sim::findObject(argv[2]));
   if (sceneWindow)
   {
      t2dSceneObject* sceneObject = object->createFull(sceneWindow, t2dSceneObject::getStringElementVector(argv[3]));
      if (sceneObject)
         return sceneObject->getId();
   }

   return NULL;
}

ConsoleMethod(LevelBuilderCreateTool, startCreate, void, 4, 4, "(sceneWindow, position) ")
{
   LevelBuilderSceneWindow* sceneWindow = dynamic_cast<LevelBuilderSceneWindow*>(Sim::findObject(argv[2]));
   if (sceneWindow)
   {
      t2dEditMouseStatus mouseStatus;
      mouseStatus.mousePoint2D = t2dSceneObject::getStringElementVector( argv[3] );
      object->onMouseDown( sceneWindow, mouseStatus );
   }
}

bool LevelBuilderCreateTool::create(LevelBuilderSceneWindow* sceneWindow)
{
   if (!sceneWindow->getSceneGraph())
      return false;

   // Create the object.
   mCreatedObject = createObject();

   if (mCreatedObject == NULL)
      return false;

   // Link Class Namespace if specified.
   if( mScriptClassName != StringTable->lookup("") )
      mCreatedObject->setClassNamespace( StringTable->insert( mScriptClassName ) );

   // Link Class Namespace if specified.
   if( mScriptSuperClassName != StringTable->lookup("") )
      mCreatedObject->setSuperClassNamespace( StringTable->insert( mScriptSuperClassName ) );

   // Set Config Datablock
   if( mConfigDatablockName && *mConfigDatablockName )
      mCreatedObject->setConfigDatablock( mConfigDatablockName );

   if( !mCreatedObject->isProperlyAdded() && !mCreatedObject->registerObject())
   {
      delete mCreatedObject;
      return false;
   }

   mCreatedObject->setModStaticFields(true);
   mCreatedObject->setModDynamicFields(true);

   sceneWindow->getSceneGraph()->addToScene(mCreatedObject);
   mCreatedObject->setVisible(false);
   mObjectHidden = true;

   return true;
}

t2dSceneObject* LevelBuilderCreateTool::createFull(LevelBuilderSceneWindow* sceneWindow, t2dVector position)
{
   if (!create(sceneWindow))
      return NULL;

   if (!mCreatedObject || !sceneWindow->getSceneEdit())
      return NULL;

   showObject();
   mCreatedObject->setPosition(position);
   mCreatedObject->setSize(getDefaultSize(sceneWindow));

   if (mAcquireCreatedObjects)
   {
      sceneWindow->getSceneEdit()->clearAcquisition();
      sceneWindow->getSceneEdit()->requestAcquisition(mCreatedObject);
   }
   
   onObjectCreated();
   sceneWindow->getSceneEdit()->onObjectChanged(mCreatedObject);
   mCreatedObject->updateSpatialConfig();
   sceneWindow->getSceneEdit()->getAcquiredObjects().calculateObjectRect();

   UndoCreateAction* undo = new UndoCreateAction(sceneWindow->getSceneEdit(), "Create Object");
   undo->addObject(mCreatedObject);
   undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());

   t2dSceneObject* obj = mCreatedObject;
   mCreatedObject = NULL;

   return obj;
}

t2dVector LevelBuilderCreateTool::getDefaultSize(LevelBuilderSceneWindow* sceneWindow)
{
   if (mCreatedObject && mCreatedObject->mConfigDataBlock)
   {
      const char* sizeString = mCreatedObject->mConfigDataBlock->getDataField(StringTable->insert("size"), NULL);
      if (sizeString && (t2dSceneObject::getStringElementCount(sizeString) == 2))
         return t2dSceneObject::getStringElementVector(sizeString);
   }

   Point2I pixelSize = getPixelSize();
   Point2I designSize = sceneWindow->getSceneEdit()->getDesignResolution();
   t2dVector worldSize = t2dVector(100.0f, 75.0f);

   t2dSceneGraph* sceneGraph = sceneWindow->getSceneGraph();
   if (sceneGraph)
   {
      const char* cameraSize = sceneGraph->getDataField(StringTable->insert("cameraSize"), NULL);
      if (cameraSize && cameraSize[0])
         worldSize = t2dSceneObject::getStringElementVector(cameraSize);
   }

   t2dVector objectSize;
   objectSize.mX = ((F32)worldSize.mX / designSize.x) * pixelSize.x;
   objectSize.mY = ((F32)worldSize.mY / designSize.y) * pixelSize.y;

   return objectSize;
}

bool LevelBuilderCreateTool::onMouseDown( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   if (!sceneWindow->getSceneEdit() || !create(sceneWindow))
      return Parent::onMouseDown(sceneWindow, mouseStatus);

   if (!mCreatedObject)
      return Parent::onMouseDown(sceneWindow, mouseStatus);

   mCreatedObject->setSize(mouseStatus.mousePoint2D - mDragStart);
   mCreatedObject->setPosition(mDragStart + (mouseStatus.mousePoint2D - mDragStart));

   bool flipX, flipY;
   bool actualFlipX = false;
   bool actualFlipY = false;
   t2dVector newSize, newPosition;
   // Snap the mouse position to the grid.
   move(sceneWindow->getSceneEdit(), t2dVector(0.0f, 0.0f), mouseStatus.mousePoint2D, mDragStart);

   // Setup the sizing state.
   mSizingState = 0;
   if (mouseStatus.mousePoint2D.mX < mDragStart.mX)
   {
      actualFlipX = true;
      mSizingState |= SizingLeft;
   }
   else
      mSizingState |= SizingRight;

   if (mouseStatus.mousePoint2D.mY < mDragStart.mY)
   {
      actualFlipY = true;
      mSizingState |= SizingTop;
   }
   else
      mSizingState |= SizingBottom;

   t2dVector size = getDefaultSize( sceneWindow );
   mMouseDownAR = size.mX / size.mY;

   scale(sceneWindow->getSceneEdit(), t2dVector(0.0f, 0.0f), mDragStart, mouseStatus.mousePoint2D, mouseStatus.event.modifier & SI_CTRL,
         mouseStatus.event.modifier & SI_SHIFT, mMouseDownAR, newSize, newPosition, flipX, flipY);

   mCreatedObject->setSize(newSize);
   mCreatedObject->setPosition(newPosition);
   mCreatedObject->setFlip(actualFlipX, actualFlipY);

   if (mAcquireCreatedObjects)
   {
      sceneWindow->getSceneEdit()->clearAcquisition();
      sceneWindow->getSceneEdit()->requestAcquisition(mCreatedObject);
   }

   return true;
}

bool LevelBuilderCreateTool::onMouseDragged( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   // Maintain a small buffer zone so minor drags aren't actually interpreted as drags.
   if ( mouseStatus.dragRectNormal2D.extent.len() < 1.0f || mouseStatus.dragRectNormal.extent.len() < 4.0f)
      return Parent::onMouseDragged(sceneWindow, mouseStatus);

   if (!mCreatedObject || !sceneWindow->getSceneEdit())
      return Parent::onMouseDragged( sceneWindow, mouseStatus );

   // Show the object if it's not shown already.
   if (mObjectHidden)
   {
      showObject();
      mObjectHidden = false;
   }

   t2dVector newSize, newPosition;
   bool flipX, flipY;
   scale(sceneWindow->getSceneEdit(), mCreatedObject->getSize(), mCreatedObject->getPosition(), mouseStatus.mousePoint2D,
         mouseStatus.event.modifier & SI_CTRL, mouseStatus.event.modifier & SI_SHIFT, mMouseDownAR, newSize, newPosition, flipX, flipY);

   bool flipXValue = (mCreatedObject->getFlipX() || flipX) && !(mCreatedObject->getFlipX() && flipX);
   bool flipYValue = (mCreatedObject->getFlipY() || flipY) && !(mCreatedObject->getFlipY() && flipY);

   mCreatedObject->setFlip(flipXValue, flipYValue);
   mCreatedObject->setPosition(newPosition);
   mCreatedObject->setSize(newSize);

   sceneWindow->getSceneEdit()->onObjectSpatialChanged();

   return true;
}

bool LevelBuilderCreateTool::onMouseUp( LevelBuilderSceneWindow* sceneWindow, const t2dEditMouseStatus &mouseStatus )
{
   // Nothing to do if nothing was created.
   if (!mCreatedObject || !sceneWindow->getSceneEdit())
      return Parent::onMouseUp( sceneWindow, mouseStatus );

   // Show the object if it's not shown already.
   if (mObjectHidden)
   {
      showObject();
      mObjectHidden = false;
   }

   if ( mouseStatus.dragRectNormal2D.extent.len() < 1.0f || mouseStatus.dragRectNormal.extent.len() < 4.0f)
   {
      t2dVector size = getDefaultSize( sceneWindow );

      mCreatedObject->setSize(size);
      mCreatedObject->setPosition(mouseStatus.mousePoint2D);
      mCreatedObject->setFlip(false, false);
   }

   onObjectCreated();
   sceneWindow->getSceneEdit()->onObjectChanged();
   mCreatedObject->updateSpatialConfig();
   sceneWindow->getSceneEdit()->getAcquiredObjects().calculateObjectRect();

   UndoCreateAction* undo = new UndoCreateAction(sceneWindow->getSceneEdit(), "Create Object");
   undo->addObject(mCreatedObject);
   undo->addToManager(&sceneWindow->getSceneEdit()->getUndoManager());

   mCreatedObject = NULL;

   return true;
}

void LevelBuilderCreateTool::onObjectCreated()
{
   Con::executef(this, 4, "onObjectCreated", Con::getIntArg(mCreatedObject->getId()), mCreatedObject->getClassName() );
}

void LevelBuilderCreateTool::onRenderGraph( LevelBuilderSceneWindow* sceneWindow )
{
   // Render Parent
   Parent::onRenderGraph( sceneWindow );

   // Draw Object Outline
   if( mCreatedObject && !mObjectHidden )
   {
      RectI objRect = sceneWindow->getObjectBoundsWindow( mCreatedObject );
      objRect.point = sceneWindow->localToGlobalCoord(objRect.point);
      dglDrawRect( objRect, mOutlineColor );
   }
}

ConsoleMethod(LevelBuilderCreateTool, setAcquireCreatedObjects, void, 3, 3, "")
{
   object->setAcquireCreatedObjects(dAtob(argv[2]));
}

ConsoleMethod(LevelBuilderCreateTool, getAcquireCreatedObjects, bool, 2, 2, "")
{
   return object->getAcquireCreatedObjects();
}


//-----------------------------------------------------------------------------
// Creation Configuration (Config Datablock/Script Class/SuperClass Namespaces)
//-----------------------------------------------------------------------------
ConsoleMethod(LevelBuilderCreateTool, setConfigDatablock, void, 3, 3, "Sets the script class namespace to link the created object to.")
{
   object->setConfigDatablock( argv[2] );
}
void LevelBuilderCreateTool::setConfigDatablock( const char* datablockName )
{
   mConfigDatablockName = StringTable->insert( datablockName );
}

ConsoleMethod(LevelBuilderCreateTool, setClassName, void, 3, 3, "Sets the script class namespace to link the created object to.")
{
   object->setClassNamespace( argv[2] );
}
void LevelBuilderCreateTool::setClassNamespace( const char* classNamespace )
{
   mScriptClassName = StringTable->insert( classNamespace );
}

ConsoleMethod(LevelBuilderCreateTool, setSuperClassName, void, 3, 3, "Sets the script super class namespace to link the created object to.")
{
   object->setSuperClassNamespace( argv[2] );
}
void LevelBuilderCreateTool::setSuperClassNamespace( const char* superClassNamespace )
{
   mScriptSuperClassName = StringTable->insert( superClassNamespace );
}


#endif // TORQUE_TOOLS
