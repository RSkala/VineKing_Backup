//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Graph Window.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "gui/core/guiTypes.h"
#include "gui/core/guiCanvas.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "math/mMathFn.h"
#include "./t2dSceneObject.h"
#include "./t2dUtility.h"
#include "./t2dSceneWindow.h"

#include "component/ComponentGlobals.h"					// RKS: I added this, so I could notify the Global Component Interface of Mouse Events
#include "component/ComponentInGameTutorialObject.h"	// RKS: I added this so I could enable input while an in-game tutorial is active
#include "component/ComponentLevelBeginSequence.h"		// RKS: I added this so I could disable input during "Ready Set Go"
#include "component/MouseEventEnums.h"

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif

#ifdef PUAP_OPTIMIZE
//<--%PUAP%   gCurrentSceneWindow   used by particles to shut off when off-screen
t2dSceneWindow *gCurrentSceneWindow;
//%PUAP% -->
#endif

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dSceneWindow);


//-----------------------------------------------------------------------------
// Interpolation Mode Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums interpolationModeLookup[] =
                {
                { t2dSceneWindow::LINEAR,   "LINEAR" },
                { t2dSceneWindow::SIGMOID,  "SIGMOID" },
                };

static EnumTable interpolationModeTable(sizeof(interpolationModeLookup) / sizeof(EnumTable::Enums), &interpolationModeLookup[0]);

//-----------------------------------------------------------------------------
// Interpolation Mode Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dSceneWindow::eCameraInterpolationMode getInterpolationMode(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(interpolationModeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(interpolationModeLookup[i].label, label) == 0)
            return((t2dSceneWindow::eCameraInterpolationMode)interpolationModeLookup[i].index);

    // Invalid Interpolation Mode!
    AssertFatal(false, "t2dSceneWindow::getInterpolationMode() - Invalid Interpolation Mode!");
    // Bah!
    return t2dSceneWindow::SIGMOID;
}

//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dSceneWindow::t2dSceneWindow() :  mpSceneGraph2D(NULL),
                                    mLockMouse(false),
                                    mWindowDirty(true),
                                    mRenderLayerMask(T2D_MASK_ALL),
                                    mRenderGroupMask(T2D_MASK_ALL),
									mpDontRenderObject(NULL),
                                    mDebugBannerColour(0,0,1,0.5f),
                                    mDebugTextColour(1,1,1,1),
                                    mCameraInterpolationMode(SIGMOID),
                                    mMaxQueueItems(64),
                                    mCameraTransitionTime(2.0f),
                                    mMovingCamera(false),
                                    mpMountedTo(NULL),
                                    mCameraMounted(false),
                                    mCameraShaking(false),
                                    mCameraShakeOffset(0.0f,0.0f),
                                    mViewLimitActive(false),
                                    mUseWindowMouseEvents(true),
                                    mUseObjectMouseEvents(false),
                                    mMouseEventGroupMaskFilter(T2D_MASK_ALL),
                                    mMouseEventLayerMaskFilter(T2D_MASK_ALL),
                                    mMouseEventInvisibleFilter( false )
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mCameraQueue );

#ifdef PUAP_OPTIMIZE
	//%PUAP%
	gCurrentSceneWindow = this;
#endif
    // Turn-on Tick Processing.
    setProcessTicks( true );
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dSceneWindow::~t2dSceneWindow()
{
}


//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dSceneWindow::onAdd()
{
    if(!Parent::onAdd())
        return false;

    // Reset Current Camera Position.
    setCurrentCameraPosition( Point2F(0,0), 100, 75 );
    // Reset Current Camera Zoom.
    setCurrentCameraZoom( 1.0f );

    // Zero Camera Time.
    zeroCameraTime();

    // Set Default Font.
    setDefaultFont();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onRemove()
{
    // Reset Scene Graph.
    resetSceneGraph();

    // Call Parent.
    Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Initialise Persistent Fields.
//-----------------------------------------------------------------------------
void t2dSceneWindow::initPersistFields()
{
    // Call Parent.
   Parent::initPersistFields();

   // Add Fields.
   addField( "lockMouse",               TypeBool, Offset(mLockMouse, t2dSceneWindow) );
   addField( "useWindowMouseEvents",    TypeBool, Offset(mUseWindowMouseEvents, t2dSceneWindow) );
   addField( "useObjectMouseEvents",    TypeBool, Offset(mUseObjectMouseEvents, t2dSceneWindow) );

   // Set Event-Modifier Script Variables.
   Con::setIntVariable("$EventModifier::LSHIFT",      SI_LSHIFT);
   Con::setIntVariable("$EventModifier::RSHIFT",      SI_RSHIFT);
   Con::setIntVariable("$EventModifier::SHIFT",       SI_SHIFT);
   Con::setIntVariable("$EventModifier::LCTRL",       SI_LCTRL);
   Con::setIntVariable("$EventModifier::RCTRL",       SI_RCTRL);
   Con::setIntVariable("$EventModifier::CTRL",        SI_CTRL);
   Con::setIntVariable("$EventModifier::LALT",        SI_LALT);
   Con::setIntVariable("$EventModifier::RALT",        SI_RALT);
   Con::setIntVariable("$EventModifier::ALT",         SI_ALT);
}


//-----------------------------------------------------------------------------
// Set Default Font.
//-----------------------------------------------------------------------------
void t2dSceneWindow::setDefaultFont( void )
{
    // Get Debug Font.
#if defined(TORQUE_OS_MAC)
   mpDebugFont = GFont::create("Monaco", 14, GuiControlProfile::sFontCacheDirectory);
#else
   mpDebugFont = GFont::create("Lucida Console", 12, GuiControlProfile::sFontCacheDirectory);
#endif
}


//-----------------------------------------------------------------------------
// Get Window Extents.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getWindowExtents, const char*, 2, 2, "() Fetch Window Extents (Position/Size)."
			  "@return Returns the window dimensions as a string formatted as follows: <position.x> <position.y> <width> <height>")
{
    // Get Size Argument Buffer.
    char* pExtentsBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf( pExtentsBuffer, 64, "%d %d %d %d", object->getPosition().x, object->getPosition().y, object->getExtent().x, object->getExtent().y );
    // Return Buffer.
    return pExtentsBuffer;
}


//-----------------------------------------------------------------------------
// Get Scene Graph.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getSceneGraph, const char*, 2, 2, "() - Returns the t2dSceneGraph associated with this window."
			  "@return Returns the scenegraph ID as a string")
{
   t2dSceneGraph* sceneGraph = object->getSceneGraph();

   char* id = Con::getReturnBuffer(8);
   if (sceneGraph) {
#ifdef PUAP_OPTIMIZE
		dItoa(sceneGraph->getId(), id );//-Mat put itoa instead of dsprintf
#else
		dSprintf(id, 8, "%d", sceneGraph->getId());
#endif
   } else {
      id[0] = '\0';
   }

   return id;
}



//-----------------------------------------------------------------------------
// Set Scene Graph.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setSceneGraph, void, 2, 3, "(t2dSceneGraph) Associates t2dSceneGraph Object."
			  "@param t2dScenegraph The scenegraph ID or name.\n"
			  "@return No return value.")
{
    // No Scenegraph specified?
    if ( argc < 3 )
    {
        // No, so reset the scenegraph.
        object->resetSceneGraph();
        // Finish here.
        return;
    }

    // Find t2dSceneGraph Object.
    t2dSceneGraph* pT2DSceneGraph = (t2dSceneGraph*)(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pT2DSceneGraph )
    {
        Con::warnf("t2dSceneWindow::setSceneGraph() - Couldn't find object '%s'.", argv[2]);
        return;
    }

    // Set Scene Graph.
    object->setSceneGraph( pT2DSceneGraph );
}   
// Set Scene Graph.
void t2dSceneWindow::setSceneGraph( t2dSceneGraph* pT2DSceneGraph )
{
    // Detach (if needed)
    resetSceneGraph();
   // Clear last pick list
   while( mLastPickVector.size() )
      mLastPickVector.popObject();

    // Attach the Window.
    pT2DSceneGraph->attachSceneWindow( this );

    // Set SceneGraph2D Object.
    mpSceneGraph2D = pT2DSceneGraph;
}


//-----------------------------------------------------------------------------
// Reset Scene Graph.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, resetSceneGraph, void, 2, 2, "() Detaches the window from any t2dSceneGraph Object.\n"
			  "@return No return value")
{
    // Reset Scene Graph.
    object->resetSceneGraph();
}   
// Reset Scene Graph.
void t2dSceneWindow::resetSceneGraph( void )
{
    // Detach from scene (if attached).
    if ( getSceneGraph() )
        getSceneGraph()->detachSceneWindow( this );

    // Are we mounted to an object?
    if ( isCameraMounted() )
        // Yes, so dismount object.
        dismount();

    // Reset SceneGraph2D Object.
    mpSceneGraph2D = NULL;
}


//-----------------------------------------------------------------------------
// Set Current Camera Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setCurrentCameraArea, void, 3, 6, "(x1 / y1 / x2 / y2) - Set current camera area."
			  "@param x1,y1,x2,y2 The coordinates of the minimum and maximum points (top left, bottom right)\n"
			  "The input can be formatted as either \"x1 y1 x2 y2\", \"x1 y1, x2 y2\", \"x1, y1, x2, y2\"\n"
			  "@return No return value.")
{
   // Upper left bound.
   t2dVector v1;
   // Lower right bound.
   t2dVector v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = t2dSceneObject::getStringElementVector(argv[2]);
       v2 = t2dSceneObject::getStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = t2dSceneObject::getStringElementVector(argv[2]);
      v2 = t2dSceneObject::getStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       v2 = t2dVector(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::setCurrentCameraArea() - Invalid number of parameters!");
      return;
   }

    // Calculate Normalised Rectangle.
    t2dVector topLeft( (v1.mX <= v2.mX) ? v1.mX : v2.mX, (v1.mY <= v2.mY) ? v1.mY : v2.mY );
    t2dVector bottomRight( (v1.mX > v2.mX) ? v1.mX : v2.mX, (v1.mY > v2.mY) ? v1.mY : v2.mY );

    // Set Current Camera Area.
    object->setCurrentCameraArea( RectF(topLeft.mX, topLeft.mY, bottomRight.mX-topLeft.mX, bottomRight.mY-topLeft.mY) );
}   
// Set Current Camera Area.
void t2dSceneWindow::setCurrentCameraArea( const RectF& cameraWindow )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("t2dSceneWindow::setCurrentCameraArea - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraCurrent.mCameraWindow = cameraWindow;

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;
}


//-----------------------------------------------------------------------------
// Get Current Camera Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getCurrentCameraArea, const char*, 2, 2, "() Get current camera Area.\n"
			  "@return The camera area formatted as \"x1 y1 x2 y2\"")
{
    // Fetch Camera Window.
    const RectF cameraWindow = object->getCurrentCameraArea();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f %f %f", cameraWindow.point.x, cameraWindow.point.y, cameraWindow.point.x+cameraWindow.extent.x, cameraWindow.point.y+cameraWindow.extent.y);
    // Return Buffer.
    return pBuffer;
}   


//-----------------------------------------------------------------------------
// Get Current Camera Size.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getCurrentCameraSize, const char*, 2, 2, "() Get current camera Size.\n"
			  "@return Returns the cameras width and height as a string formatted as \"width height\"")
{
    // Fetch Camera Window.
    const RectF cameraWindow = object->getCurrentCameraArea();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f", cameraWindow.extent.x, cameraWindow.extent.y);
    // Return Buffer.
    return pBuffer;
}   


//-----------------------------------------------------------------------------
// Set Current Camera Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setCurrentCameraPosition, void, 3, 6, "(x / y / [width / height]) - Set current camera position.\n"
			  "@param There are 5 possible formats for input: (x1, y1), (\"x1 y1\"), (x, y, width, height) (\"x1 y1\", \"width height\") (x1, y1, x2, y2)"
			  "@return No return value.")
{
   // Position.
   t2dVector position;
   // Dimensions.
   F32 width = object->getCurrentCameraWidth();
   F32 height = object->getCurrentCameraHeight();

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 width height")
   if ((elementCount1 == 4) && (argc == 3))
   {
       position = t2dSceneObject::getStringElementVector(argv[2]);
       width = dAtof(t2dSceneObject::getStringElement(argv[2], 2));
       height = dAtof(t2dSceneObject::getStringElement(argv[2], 3));
   }
   
   // ("x1 y1", "width height")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
       position = t2dSceneObject::getStringElementVector(argv[2]);
       width = dAtof(t2dSceneObject::getStringElement(argv[3], 0));
       height = dAtof(t2dSceneObject::getStringElement(argv[3], 1));
   }

   // ("x1 y1")
   else if ((elementCount1 == 2) && (argc == 3))
   {
       position = t2dSceneObject::getStringElementVector(argv[2]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       position = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       width = dAtof(argv[4]);
       height = dAtof(argv[5]);
   }

   // (x1, y1)
   else if (argc == 4)
   {
       position = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setArea() - Invalid number of parameters!");
      return;
   }

    // Set Current Camera Position.
    object->setCurrentCameraPosition( position, width, height );
}
// Set Current Camera Position.
void t2dSceneWindow::setCurrentCameraPosition( t2dVector centerPosition, F32 width, F32 height )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("t2dSceneWindow::setCurrentCameraPosition - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraCurrent.mCameraWindow = RectF( centerPosition.mX - width/2, centerPosition.mY - height/2, width, height );

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;
}


//-----------------------------------------------------------------------------
// Get Current Camera Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getCurrentCameraPosition, const char*, 2, 2, "() Get current camera Position.\n"
			  "@return Returns the camera's position as a string formatted as \"x y\"")
{
    // Fetch Camera Position.
    const t2dVector cameraPosition = object->getCurrentCameraPosition();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf( pBuffer, 32, "%f %f", cameraPosition.mX, cameraPosition.mY );
    // Return Buffer.
    return pBuffer;
}   
// Get Current Camera Position.
const t2dVector t2dSceneWindow::getCurrentCameraPosition( void ) const
{
    // Fetch Camera Window.
    const RectF cameraWindow = getCurrentCameraArea();
    // Calculate Position.
    return t2dVector( cameraWindow.point.x + (cameraWindow.len_x()*0.5f), cameraWindow.point.y + (cameraWindow.len_y()*0.5f) );
}



//-----------------------------------------------------------------------------
// Set Current Camera Zoom.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setCurrentCameraZoom, void, 3, 3, "(zoomFactor) - Set current camera Zoom Factor.\n"
			  "@param zoomFactor A float value representing the zoom factor\n"
			  "@return No return value.")
{
    // Set Current Camera Zoom.
    object->setCurrentCameraZoom( dAtof(argv[2]) );
}
// Set Current Camera Zoom.
void t2dSceneWindow::setCurrentCameraZoom( F32 zoomFactor )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraCurrent.mCameraZoom = zoomFactor;

    // Set Camera Target to Current.
    mCameraTarget = mCameraCurrent;
}


//-----------------------------------------------------------------------------
// Get Current Camera Zoom.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getCurrentCameraZoom, F32, 2, 2, "() Get current camera Zoom.\n"
			  "@return Returns the camera zoom factor as a 32-bit floating point value.")
{
    // Get Current Camera Zoom.
    return object->getCurrentCameraZoom();
}   


//-----------------------------------------------------------------------------
// Set Target Camera Area.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setTargetCameraArea, void, 3, 6, "(x / y / width / height) - Set target camera area."
			  "@return No return value.")
{
   // Upper left bound.
   t2dVector v1;
   // Lower right bound.
   t2dVector v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = t2dSceneObject::getStringElementVector(argv[2]);
       v2 = t2dSceneObject::getStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = t2dSceneObject::getStringElementVector(argv[2]);
      v2 = t2dSceneObject::getStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       v2 = t2dVector(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::setTargetCameraArea() - Invalid number of parameters!");
      return;
   }

    // Calculate Normalised Rectangle.
    t2dVector topLeft( (v1.mX <= v2.mX) ? v1.mX : v2.mX, (v1.mY <= v2.mY) ? v1.mY : v2.mY );
    t2dVector bottomRight( (v1.mX > v2.mX) ? v1.mX : v2.mX, (v1.mY > v2.mY) ? v1.mY : v2.mY );

    // Set Target Camera Area.
    object->setTargetCameraArea( RectF(topLeft.mX, topLeft.mY, bottomRight.mX-topLeft.mX+1, bottomRight.mY-topLeft.mY+1) );
}
// Set Target Camera Area.
void t2dSceneWindow::setTargetCameraArea( const RectF& cameraWindow )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("t2dSceneWindow::setTargetCameraArea - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraTarget.mCameraWindow = cameraWindow;
}


//-----------------------------------------------------------------------------
// Set Target Camera Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setTargetCameraPosition, void, 3, 6, "(x / y / [width / height]) - Set target camera position."
			  "@return No return value.")
{
   // Position.
   t2dVector position;
   // Dimensions.
   F32 width = object->getCurrentCameraWidth();
   F32 height = object->getCurrentCameraHeight();

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 width height")
   if ((elementCount1 == 4) && (argc == 3))
   {
       position = t2dSceneObject::getStringElementVector(argv[2]);
       width = dAtof(t2dSceneObject::getStringElement(argv[2], 2));
       height = dAtof(t2dSceneObject::getStringElement(argv[2], 3));
   }
   
   // ("x1 y1", "width height")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
       position = t2dSceneObject::getStringElementVector(argv[2]);
       width = dAtof(t2dSceneObject::getStringElement(argv[3], 0));
       height = dAtof(t2dSceneObject::getStringElement(argv[3], 1));
   }

   // ("x1 y1")
   else if ((elementCount1 == 2) && (argc == 3))
   {
       position = t2dSceneObject::getStringElementVector(argv[2]);
   }
   
   // (x1, y1, width, height)
   else if (argc == 6)
   {
       position = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       width = dAtof(argv[4]);
       height = dAtof(argv[5]);
   }
   
   // (x1, y1)
   else if (argc == 4)
   {
       position = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setArea() - Invalid number of parameters!");
      return;
   }

    // Set Target Camera Position.
    object->setTargetCameraPosition( position, width, height );
}
// Set Target Camera Position.
void t2dSceneWindow::setTargetCameraPosition( t2dVector centerPosition, F32 width, F32 height )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("t2dSceneWindow::setTargetCameraPosition - Cannot use this command when camera is mounted!");
        return;
    }

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraTarget.mCameraWindow = RectF( centerPosition.mX - width/2, centerPosition.mY - height/2, width, height );
}


//-----------------------------------------------------------------------------
// Set Target Camera Zoom.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setTargetCameraZoom, void, 3, 3, "(zoomFactor) - Set target camera Zoom Factor."
			  "@return No return value.")
{
    // Set Target Camera Zoom.
    object->setTargetCameraZoom( dAtof(argv[2]) );
}
// Set Target Camera Zoom.
void t2dSceneWindow::setTargetCameraZoom( F32 zoomFactor )
{
    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Target.
    mCameraTarget.mCameraZoom = zoomFactor;
}


//-----------------------------------------------------------------------------
// Set Camera Interpolation Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setCameraInterpolationTime, void, 3, 3, "(interpolationTime) - Set camera interpolation time."
			  "@return No return value")
{
    // Set Camera Interpolation Time.
    object->setCameraInterpolationTime( dAtof(argv[2]) );
}
// Set Camera Interpolation Time.
void t2dSceneWindow::setCameraInterpolationTime( F32 interpolationTime )
{
    // Set Interpolation Time.
    mCameraTransitionTime = interpolationTime;
}


//-----------------------------------------------------------------------------
// Set Camera Interpolation Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setCameraInterpolationMode, void, 3, 3, "(interpolationMode) - Set camera interpolation mode."
			  "@return No return value.")
{
    // Set Camera Interpolation Mode.
    object->setCameraInterpolationMode( getInterpolationMode(argv[2]) );
}
// Set Camera Interpolation Mode.
void t2dSceneWindow::setCameraInterpolationMode( eCameraInterpolationMode interpolationMode )
{
    // Set Interpolation Mode.
    mCameraInterpolationMode = interpolationMode;
}


//-----------------------------------------------------------------------------
// Start Camera Move.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, startCameraMove, void, 2, 3, "([interpolationTime]) - Start Camera Move."
			  "@return No return value.")
{
    F32 interpolationTime;

    // Interpolation Time?
    if ( argc >= 3 )
        interpolationTime = dAtof(argv[2]);
    else
        interpolationTime = object->getCameraInterpolationTime();

    // Start Camera Move.
    object->startCameraMove( interpolationTime );
}
// Start Camera Move.
void t2dSceneWindow::startCameraMove( F32 interpolationTime )
{
    // Are we mounted to an object and trying to move?
    if ( isCameraMounted() )
    {
        if ( ( mCameraCurrent.mCameraWindow.point != mCameraTarget.mCameraWindow.point ) ||
             ( mCameraCurrent.mCameraWindow.extent != mCameraTarget.mCameraWindow.extent ) )
        {
           // Yes, so cannot use this command.
           Con::warnf("t2dSceneWindow::startCameraMove - Cannot use this command when camera is mounted!");
           return;
        }
    }

    // Stop move if we're at target already.
    if (    mCameraCurrent.mCameraWindow.point == mCameraTarget.mCameraWindow.point &&
            mCameraCurrent.mCameraWindow.extent == mCameraTarget.mCameraWindow.extent &&
            mCameraCurrent.mCameraZoom == mCameraTarget.mCameraZoom )
    {
        // Reset Camera Move.
        mMovingCamera = false;

        // Return here.
        return;
    }
    else
    {
        // Stop Camera Move ( if any ).
        if ( mMovingCamera ) stopCameraMove();
    }

    // Set Camera Interpolation Time.
    setCameraInterpolationTime( interpolationTime );

    // Zero Camera Time.
    zeroCameraTime();

    // Set Source Camera.
    mCameraSource = mCameraCurrent;

    // Set Camera Move.
    mMovingCamera = true;

    // Complete camera move if interpolate time is zero.
    if ( mIsZero(mCameraTransitionTime) ) completeCameraMove();

    // Queue Current Camera.
    mCameraQueue.push_back( mCameraCurrent );
    // Clamp Queue Size.
    if ( mCameraQueue.size() > mMaxQueueItems ) mCameraQueue.pop_front();
}


//-----------------------------------------------------------------------------
// Stop Camera Move.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, stopCameraMove, void, 2, 2, "() Stops current camera movement"
			  "@return No return value.")
{
    // Stop Camera Move.
    object->stopCameraMove();
}
// Stop Camera Move.
void t2dSceneWindow::stopCameraMove( void )
{
    // Quit if we're not moving.
    if ( !mMovingCamera ) return;

    // Reset Tick Camera Time.
    resetTickCameraTime();

    // Set target to Current.
    mCameraTarget = mCameraCurrent;

    // Reset Camera Move.
    mMovingCamera = false;
}



//-----------------------------------------------------------------------------
// Complete Camera Move.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, completeCameraMove, void, 2, 2, "() Moves camera directly to target.\n"
			  "@return No return value.")
{
    // Complete Camera Move.
    object->completeCameraMove();
}
// Complete Camera Move.
void t2dSceneWindow::completeCameraMove( void )
{
    // Quit if we're not moving.
    if ( !mMovingCamera ) return;

    // Reset Tick Camera Time.
    resetTickCameraTime();

    // Move straight to target.
    mCameraCurrent = mCameraTarget;

    // Reset Camera Move.
    mMovingCamera = false;
}


//-----------------------------------------------------------------------------
// Undo Camera Move.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, undoCameraMove, void, 2, 3, "([interpolationTime]) - Reverses previous camera movement."
			  "@return No return value.")
{
    F32 interpolationTime;

    // Interpolation Time?
    if ( argc >= 3 )
        interpolationTime = dAtof(argv[2]);
    else
        interpolationTime = object->getCameraInterpolationTime();

    // Undo Camera Move.
    object->undoCameraMove( interpolationTime );
}
// Undo Camera Move.
void t2dSceneWindow::undoCameraMove( F32 interpolationTime )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so cannot use this command.
        Con::warnf("t2dSceneWindow::undoCameraMove - Cannot use this command when camera is mounted!");
        return;
    }

    // Quit if we've got no queued targets.
    if ( mCameraQueue.size() == 0 ) return;

    // Stop Camera Move ( if any ).
    if ( mMovingCamera ) stopCameraMove();

    // Set Camera Interpolation Time.
    setCameraInterpolationTime( interpolationTime );

    // Zero Camera Time.
    zeroCameraTime();

    // Set Source Camera.
    mCameraSource = mCameraCurrent;

    // Set Camera Move.
    mMovingCamera = true;

    // Fetch Last Queued Camera Target.
    mCameraTarget = mCameraQueue.last();
    // Remove Last Target.
    mCameraQueue.pop_back();

    // Complete camera move if interpolate time is zero.
    if ( mIsZero(mCameraTransitionTime) ) completeCameraMove();
}


//-----------------------------------------------------------------------------
// Is Camera Moving.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getIsCameraMoving, bool, 2, 2, "() Check the camera moving status.\n"
			  "@return Returns a boolean value as to whether or not the camera is moving.")
{
    // Is Camera Moving?
    return object->isCameraMoving();
}


//-----------------------------------------------------------------------------
// Is Camera Mounted.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getIsCameraMounted, bool, 2, 2, "() Check the camera mounted status.\n"
			  "@return Returns a boolean value as to whether or not the camera is mounted.")
{
    // Is Camera Mounted.
    return object->isCameraMounted();
}


//-----------------------------------------------------------------------------
// Update Camera.
//-----------------------------------------------------------------------------
void t2dSceneWindow::updateCamera( void )
{
    // Calculate Normalised Time.
    const F32 normCameraTime = mRenderCameraTime / mCameraTransitionTime;

    // Have we finished the interpolation?
    if ( mGreaterThanOrEqual(normCameraTime, 1.0f) )
    {
        // Yes, so complete camera move.
        completeCameraMove();
        // Finish here.
        return;
    }

    // Interpolate Camera Window/Zoom.
    mCameraCurrent.mCameraWindow.point.x = interpolate( mCameraSource.mCameraWindow.point.x, mCameraTarget.mCameraWindow.point.x, normCameraTime );
    mCameraCurrent.mCameraWindow.point.y = interpolate( mCameraSource.mCameraWindow.point.y, mCameraTarget.mCameraWindow.point.y, normCameraTime );
    mCameraCurrent.mCameraWindow.extent.x = interpolate( mCameraSource.mCameraWindow.extent.x, mCameraTarget.mCameraWindow.extent.x, normCameraTime );
    mCameraCurrent.mCameraWindow.extent.y = interpolate( mCameraSource.mCameraWindow.extent.y, mCameraTarget.mCameraWindow.extent.y, normCameraTime );
    mCameraCurrent.mCameraZoom = interpolate( mCameraSource.mCameraZoom, mCameraTarget.mCameraZoom, normCameraTime );
}




//-----------------------------------------------------------------------------
// Interpolate Arbitrator.
//-----------------------------------------------------------------------------
F32 t2dSceneWindow::interpolate( F32 from, F32 to, F32 delta )
{
    // Linear.
    if ( mCameraInterpolationMode == LINEAR )
        return linearInterpolate( from, to, delta );
    // Sigmoid.
    else if ( mCameraInterpolationMode == SIGMOID )
        return sigmoidInterpolate( from, to, delta );
    // Hmmm...
    else
        return from;
}


//-----------------------------------------------------------------------------
// Linear Interpolate.
// Standard Linear Ramp Interpolation Function.
//-----------------------------------------------------------------------------
F32 t2dSceneWindow::linearInterpolate( F32 from, F32 to, F32 delta )
{
    // Clamp if we're over/under time.
    if ( delta <= 0.0f )
        return from;
    else if ( delta >= 1.0f )
        return to;

    // Calculate resultant interpolation.
    return ( from * ( 1.0f - delta ) ) + ( to * delta );
}



//-----------------------------------------------------------------------------
// Sigmoid Interpolate.
// Slow-in / Slow-out Sigmoid Interpolation Function.
//-----------------------------------------------------------------------------
F32 t2dSceneWindow::sigmoidInterpolate( F32 from, F32 to, F32 delta )
{
    // Range Expand/Clamp Delta to (-1 -> +1).
    delta = mClampF( (delta - 0.5f) * 2.0f, -1.0f, 1.0f );

    // Calculate interpolator value using sigmoid function.
    F32 sigmoid = mClampF ( 1.0f / (1.0f + mPow(2.718282f, -15.0f * delta)), 0.0f, 1.0f );

    // Calculate resultant interpolation.
    return ( from * ( 1.0f - sigmoid ) ) + ( to * sigmoid );
}


//-----------------------------------------------------------------------------
// Start Camera Shake.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, startCameraShake, void, 4, 4, "(shakeMagnitude, time) - Starts the camera shaking."
			  "@param shakeMagnitude The intensity of the shaking\n"
			  "@param time The length of the shake"
			  "@return No return value")
{
    // Start Camera Shake.
    object->startCameraShake( dAtof(argv[2]), dAtof(argv[3]) );
}
// Start Camera Shake.
void t2dSceneWindow::startCameraShake( F32 magnitude, F32 time )
{
    // Is the time zero?
    if ( mIsZero( time ) && mIsZero( magnitude ) )
    {
        // Yes, so simply stop the camera shaking.
        stopCameraShake();
        // Finish here.
        return;
    }

    // Set Current Shake.
    mCurrentShake = mFabs(magnitude);
    // Set Shake Life.
    mShakeLife = time;
    // Calculate Shake Ramp.
    mShakeRamp = mCurrentShake / mShakeLife;
    // Flag camera shaking.
    mCameraShaking = true;
}


//-----------------------------------------------------------------------------
// Stop Camera Shake.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, stopCameraShake, void, 2, 2, "() Stops the camera shaking."
			  "@return No return value")
{
    // Stop Camera Shake.
    object->stopCameraShake();
}
// Stop Camera Shake.
void t2dSceneWindow::stopCameraShake( void )
{
    // Flag camera not shaking.
    mCameraShaking = false;

    // Reset Shake Offset.
    mCameraShakeOffset.zero();
}


//-----------------------------------------------------------------------------
// Mount Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, mount, void, 3, 7, "(t2dSceneObject, [offsetX / offsetY], [mountForce], [sendToMount?]) - Mounts Camera onto a specified object."
			  "@return No return value")
{
    // Grab the object. Always specified.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if (!pSceneObject2D)
    {
        Con::warnf("t2dSceneWindow::mount() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Reset Element Count.
    U32 elementCount = 2;
    // Calculate Mount-Offset.
    t2dVector mountOffset(0.0f, 0.0f);

    if (argc > 3)
    {
        // Fetch Element Count.
        elementCount = t2dSceneObject::getStringElementCount(argv[3]);

        // (object, "offsetX offsetY", ...)
        if ((elementCount == 2) && (argc < 7))
            mountOffset = t2dSceneObject::getStringElementVector(argv[3]);

        // (object, offsetX, offsetY, ...)
        else if ((elementCount == 1) && (argc > 4))
            mountOffset = t2dVector(dAtof(argv[3]), dAtof(argv[4]));

        // Invalid.
        else
        {
            Con::warnf("t2dSceneWindow::mount() - Invalid number of parameters!");
            return;
        }

    }

    // Set the next arg index.
    // The argv index of the first parameter after the offset.
    U32 firstArg = 6 - elementCount;

    // Grab the mount force - if it's specified.
    F32 mountForce = 0.0f;
    if (argc > firstArg)
        mountForce = dAtof(argv[firstArg]);

    // Grab the send to mount flag.
    bool sendToMount = true;
    if (argc > (firstArg + 1))
        sendToMount = dAtob(argv[firstArg + 1]);

    // Mount Object.
    object->mount( pSceneObject2D, mountOffset, mountForce, sendToMount );
}
// Mount Object.
void t2dSceneWindow::mount( t2dSceneObject* pSceneObject2D, t2dVector mountOffset, F32 mountForce, bool sendToMount )
{
    // Are we mounted to an object?
    if ( isCameraMounted() )
    {
        // Yes, so dismount object.
        dismount();
    }
    else
    {
        // No, so stop any Camera Move.
        if ( mMovingCamera ) stopCameraMove();
    }

    // Set Mount Object Reference.
    mpMountedTo = pSceneObject2D;
    // Store Mount Offset.
    mMountOffset = mountOffset;
    // Set Mount Force.
    mMountForce = mountForce;
    // Create a Mount Node.
    mMountToID = mpMountedTo->addLinkPoint( mountOffset );

    mpMountedTo->getMountNode( mMountToID )->mMountReferenceCount++;

    // Add Camera Mount Reference.
    pSceneObject2D->addCameraMountReference( this );

    // Flag Camera mounted.
    mCameraMounted = true;

    // Send directly to mount (if selected).
    if ( sendToMount )
    {
        // Fetch Mount Position.
        const t2dVector& mountPos = mpMountedTo->getMountNode(mMountToID)->mWorldMountPosition;

        // Calculate Window Half-Dimensions.
        const F32 halfWidth = mCameraCurrent.mCameraWindow.len_x() * 0.5f;
        const F32 halfHeight = mCameraCurrent.mCameraWindow.len_y() * 0.5f;

        // Set Current View to Object Position.
        mCameraCurrent.mCameraWindow.point.set( mountPos.mX - halfWidth, mountPos.mY - halfHeight );
    }

    // Reset Tick Camera Position.
    resetTickCameraPosition();
}


//-----------------------------------------------------------------------------
// Dismount Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, dismount, void, 2, 2, "() Dismounts Camera from object."
			  "@return No return value")
{
    // Dismount Object.
    object->dismount();
}
// Dismount Object.
void t2dSceneWindow::dismount( void )
{
    // Nothing to do if we're not mounted!
    if (!isCameraMounted() )
        return;

    // Remove Object Mount Node.
    mpMountedTo->removeLinkPoint( mMountToID );

    // Remove Camera Mount Reference.
    mpMountedTo->removeCameraMountReference();

    // Reset Camera Object Mount.
    mpMountedTo = NULL;

    // Flag Camera not mounted.
    mCameraMounted = false;

    // Reset Tick Camera Position.
    resetTickCameraPosition();
}


//-----------------------------------------------------------------------------
// Dismount Specific Object.
//-----------------------------------------------------------------------------
void t2dSceneWindow::dismountMe( t2dSceneObject* pSceneObject2D )
{
    // Are we mounted to the specified object?
    if ( isCameraMounted() && pSceneObject2D != mpMountedTo )
    {
        // No, so warn.
        Con::warnf("t2dSceneWindow::dismountMe() - Object is not mounted by the camera!");
        return;
    }

    // Dismount Object.
    dismount();
}


//-----------------------------------------------------------------------------
// Set View Limit On.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setViewLimitOn, void, 3, 6, "([minX / minY / maxX / maxY]) - Set View Limit On."
			  "@return No return value")
{
   // Upper left bound.
   t2dVector v1;
   // Lower right bound.
   t2dVector v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = t2dSceneObject::getStringElementVector(argv[2]);
       v2 = t2dSceneObject::getStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = t2dSceneObject::getStringElementVector(argv[2]);
      v2 = t2dSceneObject::getStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       v2 = t2dVector(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::setViewLimitOn() - Invalid number of parameters!");
      return;
   }

   // Calculate Normalised Rectangle.
   t2dVector topLeft((v1.mX <= v2.mX) ? v1.mX : v2.mX, (v1.mY <= v2.mY) ? v1.mY : v2.mY);
   t2dVector bottomRight((v1.mX > v2.mX) ? v1.mX : v2.mX, (v1.mY > v2.mY) ? v1.mY : v2.mY);

    // Set the View Limit On.
    object->setViewLimitOn(topLeft, bottomRight);
}
// Set World Limit.
void t2dSceneWindow::setViewLimitOn( const t2dVector& limitMin, const t2dVector& limitMax )
{
    // Activate View Limit.
    mViewLimitActive = true;

    // Set View Limit Min/Max.
    mViewLimitMin = limitMin;
    mViewLimitMax = limitMax;

    // Calculate Camera Area.
    mViewLimitArea = mViewLimitMax - mViewLimitMin;
}


//-----------------------------------------------------------------------------
// Set View Limit Off.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setViewLimitOff, void, 2, 2, "() Set View Limit Off."
			  "@return No return value")
{
    // Set View Limit Off.
    object->setViewLimitOff();
}

//-----------------------------------------------------------------------------
// Set Collision Groups.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setRenderGroups, void, 3, 2 + T2D_MASK_BITCOUNT, "(groups$) - Sets the render group(s).\n"
			  "@param groups The list of groups you wish to set.\n"
			  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneWindow::setRenderGroups() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setRenderGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setRenderGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setRenderGroups(mask);
}

void t2dSceneWindow::setRenderGroups(U32 groupMask)
{
   mRenderGroupMask = groupMask;
}

//-----------------------------------------------------------------------------
// Set Render Layers.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setRenderLayers, void, 3, 2 + T2D_MASK_BITCOUNT, "(layers$) - Sets the render layers(s)."
			  "@param The layer numbers you wish to set.\n"
			  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneWindow::setRenderLayers() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setRenderLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setRenderLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setRenderLayers(mask);
}

void t2dSceneWindow::setRenderLayers(U32 layerMask)
{
   mRenderLayerMask = layerMask;
}

//-----------------------------------------------------------------------------
// Set Render Masks.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setRenderMasks, void, 3, 4, "(layerMask, groupMask) - Sets the layer/group mask which control what is rendered."
			  "@param layermask The bitmask for setting the layers to render\n"
			  "@param groupmask The bitmask for setting the groups to render\n"
			  "@return No return value.")
{
    // Set Render Masks.
   if( argc < 4 )
      object->setRenderLayers( dAtoi( argv[2] ) );
   else
      object->setRenderMasks( dAtoi(argv[2]), dAtoi(argv[3]) );
}   
// Set Render Masks.
void t2dSceneWindow::setRenderMasks( U32 layerMask, U32 groupMask )
{
    // Set Render Layer/Group Masks.
    mRenderLayerMask = layerMask;
    mRenderGroupMask = groupMask;
}

//-----------------------------------------------------------------------------
// Get Render Masks.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getRenderLayerMask, S32, 2, 2, "() - Gets the layer mask which controls what is rendered."
			  "@returns The bit mask corresponding to the layers which are to be rendered")
{
   return object->getRenderLayerMask();
}   
// Set Render Masks.
S32 t2dSceneWindow::getRenderLayerMask()
{
    // Set Render Layer/Group Masks.
    return mRenderLayerMask;
}

ConsoleMethod(t2dSceneWindow, getRenderGroupMask, S32, 2, 2, "() - Gets the group mask which controls what is rendered."
			  "@returns The bit mask corresponding to the groups which are to be rendered")
{
   return object->getRenderGroupMask();
}   
// Set Render Masks.
S32 t2dSceneWindow::getRenderGroupMask()
{
    // Set Render Layer/Group Masks.
    return mRenderGroupMask;
}

//-----------------------------------------------------------------------------
// Get "Don't Render Object"
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getDontRenderObject, S32, 2, 2, "() Gets the object that won't be rendered in this scene window."
			  "@return The ID of the object that won't be rendered.")
{
	t2dSceneObject* dontRender = object->getDontRenderObject();
	if (dontRender != NULL)
		return dontRender->getId();
	else
		return 0;
}

t2dSceneObject* t2dSceneWindow::getDontRenderObject()
{
	return mpDontRenderObject;
}

//-----------------------------------------------------------------------------
// Set "Don't Render Object"
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setDontRenderObject, bool, 3, 3, "(t2dSceneObject obj) - Sets obj as to not render in this scene window.\n"
			  "@param obj The object you wish not to render in the window."
			  "@return Always returns true.")
{
	t2dSceneObject* pDontRenderMe = dynamic_cast<t2dSceneObject*>(Sim::findObject( argv[2] ));
	object->setDontRenderObject(pDontRenderMe);
	return true;
}
	
void t2dSceneWindow::setDontRenderObject(t2dSceneObject* obj)
{
	mpDontRenderObject = obj;
}


//-----------------------------------------------------------------------------
// Set Use Window Mouse Events.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setUseWindowMouseEvents, void, 3, 3, "(mouseStatus) Sets whether mouses-events are monitored by the window.\n"
			  "@param mouseStatus A boolean value. If true mouse events won't be monitored by the window, if false they will\n"
			  "@return No return value.")
{
    // Set Use Window Mouse Events.
    object->setUseWindowMouseEvents( dAtob(argv[2]) );
}   


//-----------------------------------------------------------------------------
// Get Use Window Mouse Events.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getUseWindowMouseEvents, bool, 2, 2, "() Gets whether mouses-events are monitored by the window.\n"
			  "@return A boolean value. If true mouse events are being monitored by the window, if false they aren't")
{
    // Get Use Window Mouse Events.
    return object->getUseWindowMouseEvents();
}   


//-----------------------------------------------------------------------------
// Set Use Object Mouse Events.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setUseObjectMouseEvents, void, 3, 3, "(mouseStatus) Sets whether mouses-events are monitored by the scene-Objects.\n"
			  "@param mouseStatus A boolean value. If true mouse events won't be monitored by the object, if false they will\n")
{
    // Set Use Object Mouse Events.
    object->setUseObjectMouseEvents( dAtob(argv[2]) );
}


//-----------------------------------------------------------------------------
// Set Object Mouse Events.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getUseObjectMouseEvents, bool, 2, 2, "() Gets whether mouses-events are monitored by the scene-objects.\n"
			  "@return Returns a boolean value. If true mouse events are being monitored by the object, if false they aren't")
{
    // Get Use Window Mouse Events.
    return object->getUseObjectMouseEvents();
}

ConsoleMethod(t2dSceneWindow, setObjectMouseEventGroupFilter, void, 3, 2 + T2D_MASK_BITCOUNT, "(groups$) Sets the group filter to the specified mask.\n"
			  "@param List of groups to set.\n"
			  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneWindow::setObjectMouseEventGroupFilter() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setObjectMouseEventGroupFilter() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setObjectMouseEventGroupFilter() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Groups.
   object->setObjectMouseEventGroupFilter(mask);
}

void t2dSceneWindow::setObjectMouseEventGroupFilter(U32 groupMask)
{
   mMouseEventGroupMaskFilter = groupMask;
}

//-----------------------------------------------------------------------------
// Set Collision Layers.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setObjectMouseEventLayerFilter, void, 3, 2 + T2D_MASK_BITCOUNT, "(layers$) Sets the layer filter."
			  "@param The list of layers to set.\n"
			  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneWindow::setObjectMouseEventLayerFilter() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setObjectMouseEventLayerFilter() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_MASK_BITCOUNT))
         {
            Con::warnf("t2dSceneWindow::setObjectMouseEventLayerFilter() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setObjectMouseEventLayerFilter(mask);
}

void t2dSceneWindow::setObjectMouseEventLayerFilter(U32 layerMask)
{
   mMouseEventLayerMaskFilter = layerMask;
}

ConsoleMethod(t2dSceneWindow, setObjectMouseEventInvisibleFilter, void, 3, 3, "(bool useInvisible) Sets the use invisible flag")
{
   object->setObjectMouseEventInvisibleFilter(dAtob(argv[2]));
}

void t2dSceneWindow::setObjectMouseEventInvisibleFilter(bool useInvisible)
{
   mMouseEventInvisibleFilter = useInvisible;
}

//-----------------------------------------------------------------------------
// Set Object Mouse Event Filter.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setObjectMouseEventFilter, void, 4, 5, "(groupMask, layerMask, [useInvisible?]) Sets Filter for Object Mouse-Events.")
{
    // Calculate Use Invisible Flag.
    bool useInvisible = argc >= 5 ? dAtob(argv[4]) : false;

    // Set Object Mouse Event Filter.
    object->setObjectMouseEventFilter( dAtoi(argv[2]), dAtoi(argv[3]), useInvisible );
}
// Set Object Mouse Event Filter.
void t2dSceneWindow::setObjectMouseEventFilter( U32 groupMask, U32 layerMask, bool useInvisible )
{
    // Set Object Mouse Event Filter.
    mMouseEventGroupMaskFilter = groupMask;
    mMouseEventLayerMaskFilter = layerMask;
    mMouseEventInvisibleFilter = useInvisible;
}


//-----------------------------------------------------------------------------
// Set Lock Mouse.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setLockMouse, void, 3, 3, "(bool lockSet) Sets the window mouse-lock status."
			  "@return No return value.")
{
    // Set Lock Mouse.
    object->setLockMouse( dAtob(argv[2]) );
}


//-----------------------------------------------------------------------------
// Get Lock Mouse.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getLockMouse, bool, 2, 2, "() Gets the window mouse-lock status.")
{
    // Get Lock Mouse.
    return object->getLockMouse();
}

//-----------------------------------------------------------------------------
// Set Debug Banner.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setDebugBanner, void, 4, 12, "(fontName, fontSize, textR/G/B/[A]$, backgroundR/G/B/[A]$) - Set Debug Font/Size/textColour/backgroundColour")
{
   // Note: If only one of the two alphas are passed in a comma separated list, it is assumed to
   // be for the background. i.e (fontName, fontSize, r, g, b, r, g, b, a).

   ColorF debugTextColor, debugBannerColor;
   if (argc > 4)
   {
      U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[4]);
      U32 elementCount2 = 0;

      // (fontName, fontSize, "R G B [A]", "R G B [A]")
      if ((elementCount1 > 2) && (argc < 7))
      {
         debugTextColor.red = dAtof(t2dSceneObject::getStringElement(argv[4], 0));
         debugTextColor.green = dAtof(t2dSceneObject::getStringElement(argv[4], 1));
         debugTextColor.blue = dAtof(t2dSceneObject::getStringElement(argv[4], 2));

         // Alpha specified?
         if (elementCount1 > 3)
            debugTextColor.alpha = dAtof(t2dSceneObject::getStringElement(argv[4], 3));
         else
            debugTextColor.alpha = 1.0f;

         // Banner color specified?
         if (argc > 5)
            elementCount2 = t2dSceneObject::getStringElementCount(argv[5]);
         else
            debugBannerColor.set(0.5f, 0.5f, 0.5f, 0.2f);

         if (elementCount2 > 2)
         {
            debugBannerColor.red = dAtof(t2dSceneObject::getStringElement(argv[5], 0));
            debugBannerColor.green = dAtof(t2dSceneObject::getStringElement(argv[5], 1));
            debugBannerColor.blue = dAtof(t2dSceneObject::getStringElement(argv[5], 2));
   
            // Alpha specified?
            if (elementCount2 > 3)
               debugBannerColor.alpha = dAtof(t2dSceneObject::getStringElement(argv[5], 3));
            else
               debugBannerColor.alpha = 1.0f;
         }
         else
         {
            // Warn.
            Con::warnf("t2dSceneWindow::setDebugBanner() - Invalid debug banner colour specified; Must be at least three elements! (%s)", argv[4]);
            return;
         }
      }

      // (fontName, fontSize, R, G, B, [A], R, G, B, [A])
      else if ((elementCount1 == 1) && (argc > 6))
      {
         debugTextColor.red = dAtof(argv[4]);
         debugTextColor.green = dAtof(argv[5]);
         debugTextColor.blue = dAtof(argv[6]);

         U32 firstBannerArg = 0;

         // Alpha specified?
         if ((argc == 8) || (argc == 12))
         {
            firstBannerArg = 8;
            debugTextColor.alpha = dAtof(argv[7]);
         }
         else
         {
            firstBannerArg = 7;
            debugTextColor.alpha = 1.0f;
         }

         // Banner color specified?
         if (argc > 9)
         {
            debugBannerColor.red = dAtof(argv[firstBannerArg]);
            debugBannerColor.green = dAtof(argv[firstBannerArg + 1]);
            debugBannerColor.blue = dAtof(argv[firstBannerArg + 2]);

            // Alpha specified?
            if (argc == 11)
               debugBannerColor.alpha = dAtof(argv[10]);
            else if (argc == 12)
               debugBannerColor.alpha = dAtof(argv[11]);
            else
               debugBannerColor.alpha = 1.0f;
         }
         else
            debugBannerColor.set(0.5f, 0.5f, 0.5f, 0.2f);
      }

      // Invalid
      else
      {
         // Warn.
         Con::warnf("t2dSceneWindow::setDebugBanner() - Invalid debug text colour specified; Must be at least three elements! (%s)", argv[4]);
         return;
      }
   }
   else
   {
      debugTextColor.set(0.0f, 0.0f, 0.0f, 1.0f);
      debugBannerColor.set(0.5f, 0.5f, 0.5f, 0.2f);
   }

   // Set Debug Banner.
   object->setDebugBanner( argv[2], dAtoi(argv[3]), debugTextColor, debugBannerColor );
}
// Set Debug Banner.
void t2dSceneWindow::setDebugBanner( const char* pFontname, const U32 fontSize, const ColorF& debugTextColour, const ColorF& debugBannerColour )
{
    // Get Debug Font.
    mpDebugFont = GFont::create( pFontname, fontSize, GuiControlProfile::sFontCacheDirectory);

    // Was the font valid?
    if ( mpDebugFont.isNull() )
    {
        // No, so set default font.
        setDefaultFont();

        // Warn.
        Con::warnf("t2dSceneWindow::setDebugFont() - Invalid font selected! '%s' (%d). (Using default)", pFontname, fontSize );
    }

    // Set Debug Text/Banner Colours.
    mDebugTextColour = debugTextColour;
    mDebugBannerColour = debugBannerColour;
}

//remove from the SceneWindow last picker
void t2dSceneWindow::removeFromLastPicker(t2dSceneObject *psObject)
{
	mLastPickVector.removeObject(psObject);
}

//-----------------------------------------------------------------------------
// Set World Mouse Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, setMousePosition, void, 3, 4, "(x/y) Sets Current Mouse Position."
			  "@param x,y The coordinates to set the mousecursor. Accepts either (x,y) or (\"x y\")")
{
   // The new position.
   t2dVector position;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      position = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      position = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::setMousePosition() - Invalid number of parameters!");
      return;
   }

    // Set Mouse Position.
    object->setMousePosition( position );
}
// Set World Mouse Position.
void t2dSceneWindow::setMousePosition( const t2dVector& mousePosition )
{
    // Fetch Canvas.
    GuiCanvas* pCanvas = getRoot();

    // Canvas available?
    if ( pCanvas )
    {
        // Are we bound to a scenegraph?
        if ( getSceneGraph() )
        {
            t2dVector windowMousePosition;

            // Yes, so convert window into scene coordinates...
            sceneToWindowCoord( mousePosition, windowMousePosition );

            // Copy into a compatible format for TGE.
            Point2I localWindowPosition( S32(windowMousePosition.mX), S32(windowMousePosition.mY) );

            // Set Cursor Position.
            pCanvas->setCursorPos( localToGlobalCoord(localWindowPosition) );
        }
        else
        {
            // No, so error.
            Con::warnf("t2dSceneObject::setMousePosition() - No scene attached to window!");
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Get World Mouse Position.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getMousePosition, const char*, 2, 2, "() Gets Current Mouse Position."
			  "@return Returns a string with the current mouse cursor coordinates formatted as \"x y\"")
{
    // Fetch Mouse Position.
    t2dVector worldMousePoint = object->getMousePosition();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Generate Script Parameters.
    dSprintf(pBuffer, 32, "%f %f", worldMousePoint.mX, worldMousePoint.mY);

    // Return Buffer.
    return pBuffer;
}
// Get World Mouse Position.
t2dVector t2dSceneWindow::getMousePosition( void )
{
    // Calculate Current Camera View.
    calculateCameraView( &mCameraCurrent );

    // Fetch Canvas.
    GuiCanvas* pCanvas = getRoot();

    // World Mouse Position.
    t2dVector worldMousePoint(0, 0);

    // Canvas available?
    if ( pCanvas )
    {
        // Yes, so fetch local GUI coordinates.
        const t2dVector localGuiPoint = globalToLocalCoord( pCanvas->getCursorPos() );

        // Are we bound to a scenegraph?
        if ( getSceneGraph() )
        {
            // Yes, so convert window into scene coordinates...
            windowToSceneCoord(localGuiPoint, worldMousePoint);
        }
        else
        {
            // No, so use window screen coordinates.
            worldMousePoint = localGuiPoint;
        }
    }
    else
    {
        //  No, so warn
        //Con::warnf("t2dSceneWindow::getMousePosition() - Window not attached to canvas!" );
    }

    // Return World Mouse Position.
    return worldMousePoint;
}


//-----------------------------------------------------------------------------
// Convert Window Coordinate to Scene Coordinate.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getWorldPoint, const char*, 3, 4, "(X / Y) - Returns World coordinate of Window coordinate."
			  "@param x,y The coordinates in window coordinates you wish to convert to world coordinates. Accepts either (x,y) or (\"x y\")"
			  "@return Returns the desired world coordinates as a string formatted as \"x y\"")
{
   // The new position.
   t2dVector srcPoint;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::getWorldPoint() - Invalid number of parameters!");
      return false;
   }
   
   // Desintation Point.
    t2dVector dstPoint;

    // Do Conversion.
    object->windowToSceneCoord( srcPoint, dstPoint );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", dstPoint.mX, dstPoint.mY);
    // Return buffer.
    return pBuffer;
}
// Convert Window Coordinate to Scene Coordinate.
void t2dSceneWindow::windowToSceneCoord( const t2dVector& srcPoint, t2dVector& dstPoint ) const
{
    // Return Conversion.
    dstPoint.set( (srcPoint.mX * mCameraCurrent.mSceneWindowScale.x) + mCameraCurrent.mSceneMin.x, (srcPoint.mY * mCameraCurrent.mSceneWindowScale.y) + mCameraCurrent.mSceneMin.y );
}


//-----------------------------------------------------------------------------
// Convert Scene Coordinate to Window Coordinate.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getWindowPoint, const char*, 3, 4, "(X / Y) - Returns Window coordinate of World coordinate."
			  "@param x,y The coordinates in world coordinates you wish to convert to window coordinates. Accepts either (x,y) or (\"x y\")"
			  "@return Returns the desired window coordinates as a string formatted as \"x y\"")
{
   // The new position.
   t2dVector srcPoint;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::getWindowPoint() - Invalid number of parameters!");
      return NULL;
   }
   
   // Desintation Point.
    t2dVector dstPoint;

    // Do Conversion.
    object->sceneToWindowCoord( srcPoint, dstPoint );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f", dstPoint.mX, dstPoint.mY);
    // Return buffer.
    return pBuffer;
}
// Convert Scene Coordinate to Window Coordinate.
void t2dSceneWindow::sceneToWindowCoord( const t2dVector& srcPoint, t2dVector& dstPoint ) const
{
    // Return Conversion.
    dstPoint.set( (srcPoint.mX - mCameraCurrent.mSceneMin.x) / mCameraCurrent.mSceneWindowScale.x, (srcPoint.mY - mCameraCurrent.mSceneMin.y) / mCameraCurrent.mSceneWindowScale.y );
}


//-----------------------------------------------------------------------------
// Get Canvas Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getCanvasPoint, const char*, 3, 4, "(X / Y) - Returns Canvas coordinate of Window coordinate."
			   "@param x,y The coordinates in world coordinates you wish to convert to window coordinates. Accepts either (x,y) or (\"x y\")"
			  "@return Returns the desired canvas coordinates as a string formatted as \"x y\"")
{
   // The new position.
   Point2I srcPoint;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
   {
      srcPoint.x = dAtoi(t2dSceneObject::getStringElement(argv[2], 0));
      srcPoint.y = dAtoi(t2dSceneObject::getStringElement(argv[2], 1));
   }

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Point2I(dAtoi(argv[2]), dAtoi(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::getCanvasPoint() - Invalid number of parameters!");
      return NULL;
   }

    // Do Conversion.
    Point2I dstPoint = object->localToGlobalCoord( srcPoint );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", dstPoint.x, dstPoint.y);
    // Return buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Is Point in Window?
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneWindow, getIsWindowPoint, bool, 3, 4, "(X / Y) Checks if World coordinate is inside Window."
			   "@param x,y The coordinates in world coordinates you wish to check. Accepts either (x,y) or (\"x y\")"
			  "@return Returns true if the coordinates are within the window, and false otherwise.")
{
   // The new position.
   t2dVector srcPoint;

   // Elements in the first argument.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = t2dSceneObject::getStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("t2dSceneWindow::getIsWindowPoint() - Invalid number of parameters!");
      return false;
   }

   // Desintation Point.
    t2dVector dstPoint;
    // Do Conversion.
    object->sceneToWindowCoord( srcPoint, dstPoint );

    // Check if point is in window bounds.
    return object->mBounds.pointInRect( Point2I( S32(mFloor(dstPoint.mX)+object->mBounds.point.x), S32(mFloor(dstPoint.mY)+object->mBounds.point.y )) );
}


//-----------------------------------------------------------------------------
// Dispatch Mouse Events.
//-----------------------------------------------------------------------------
void t2dSceneWindow::dispatchMouseEvent( const char* name, const GuiEvent& event )
{
	//printf( "t2dSceneWindow::dispatchMouseEvent - %s\n", name ); // RKS:
	//if( strcmp( name, "onMouseUp" ) == 0 )
	//{
	//	int stophere = 0;
	//	stophere++;
	//}
	
    // Dispatch Mouse-Event to Window (if active).
    if ( getUseWindowMouseEvents() )
        sendWindowMouseEvent( name, event );
 
	// RKS:  I added this if-statement, so that objects will not respond to mouse events until after "Ready Set Go"
	if( CComponentLevelBeginSequence::DoesLevelHaveBeginSequence() == false )
	{
		if ( getUseObjectMouseEvents() )
			sendObjectMouseEvent( name, event );
	}
	else 
	{
		
		
		if( CComponentLevelBeginSequence::GetInstance().HasLevelBeginSequenceFinished())
		{
			if ( getUseObjectMouseEvents() )
			{
				sendObjectMouseEvent( name, event );
				
				// RKS: 2010/12/19 - Testing to see if this fixes the bug where touching an enemy causes the user to not be able to draw a line on the next touch.
				CComponentGlobals::GetInstance().SetPlayerNotTouchingEnemy();
			}
		}
		else if( CComponentInGameTutorialObject::IsInGameTutorialActive() )
		{
			if ( getUseObjectMouseEvents() )
				sendObjectMouseEvent( name, event );
		}

	}
	
    // Dispatch Mouse-Event to Scene-Object(s) (if active).
//    if ( getUseObjectMouseEvents() )
//        sendObjectMouseEvent( name, event );  // RKS: This is commented out because of the stuff above this
}


//-----------------------------------------------------------------------------
// Send Window Mouse Event to Scripts.
//-----------------------------------------------------------------------------
void t2dSceneWindow::sendWindowMouseEvent( const char* name, const GuiEvent& event )
{
    t2dVector   worldMousePoint;

    // Calculate Current Camera View.
    calculateCameraView( &mCameraCurrent );

    // Convert to local gui coordinates.
    const t2dVector localGuiPoint = globalToLocalCoord(event.mousePoint);

    // Are we bound to a scenegraph?
    if ( getSceneGraph() )
    {
        // Yes, so convert window into scene coordinates...
        windowToSceneCoord(localGuiPoint, worldMousePoint);
    }
    else
    {
        // No, so use window screen coordinates.
        worldMousePoint = localGuiPoint;
    }


    // Argument Buffers.
    char argBuffer[3][64];

     // Format Event-Modifier Buffer.
    dSprintf(argBuffer[0], 64, "%d", event.modifier);

    // Format Mouse-Position Buffer.
    dSprintf(argBuffer[1], 64, "%f %f", worldMousePoint.mX, worldMousePoint.mY);

    // Format Mouse-Click Count Buffer.
    dSprintf(argBuffer[2], 64, "%d", event.mouseClickCount);

    // Call Scripts.
    //Con::executef(this, 4, name, argBuffer[0], argBuffer[1], argBuffer[2]); // RKS TODO:  Remove this script call, as it's unnecessary for my purposes.
}

//-----------------------------------------------------------------------------
// Send Object Mouse Event to Scripts.
//-----------------------------------------------------------------------------
void t2dSceneWindow::sendObjectMouseEvent( const char* name, const GuiEvent& event ) // RKS NOTE:  The only events sent here now are MOUSE_DOWN and MOUSE_UP
{
	EMouseEvent eMouseEvent;
	if( strcmp( name, "onMouseDown" ) == 0 )
	{
		eMouseEvent = MOUSE_EVENT_DOWN;   
	}
	else if (strcmp( name, "onMouseUp" ) == 0 )
	{
		eMouseEvent = MOUSE_EVENT_UP;
		CComponentGlobals::GetInstance().ClearFirstTouchDownObject();
	}
	else if( strcmp( name, "onMouseEnter" ) == 0 )
	{
		eMouseEvent = MOUSE_EVENT_ENTER;
	}
	else if( strcmp( name, "onMouseLeave" ) == 0 )
	{
		eMouseEvent = MOUSE_EVENT_LEAVE;
	}
	else
	{
		eMouseEvent = MOUSE_EVENT_NONE;
		printf( "t2dSceneObject::onMouseEvent - Unhandled mouse event: %s\n", name );
	}
	
   // Finish if we're not bound to a scenegraph?
   if ( !getSceneGraph() ) return;

   // Convert Event-Position into scene coordinates.
   t2dVector worldMousePoint;
   windowToSceneCoord(t2dVector(globalToLocalCoord(event.mousePoint)), worldMousePoint);

   // If an object is locked, we just send it all the events regardless of where it is.
   const SimSet& lockedObjects = getSceneGraph()->getMouseLockedObjectSet();
   for (U32 i = 0; i < lockedObjects.size(); i++)
   {
      t2dSceneObject* lockedObject = static_cast<t2dSceneObject*>( lockedObjects.at(i) );
      lockedObject->onMouseEvent(name, event, worldMousePoint, eMouseEvent); // RKS: I added the eMouseEvent
   }

   // Finish if we've no objects at the mouse-event point.
   if ( getSceneGraph()->pickPoint( worldMousePoint, mMouseEventGroupMaskFilter, mMouseEventLayerMaskFilter, mMouseEventInvisibleFilter ) == 0 )
   {
      if( !mLastPickVector.empty() )
      {         
         for( U32 i = 0; i < mLastPickVector.size(); i++ )
         {
            t2dSceneObject* lastObject = static_cast<t2dSceneObject*>( mLastPickVector[i] );
            if( !lastObject )
               continue;

            if( lastObject->isProperlyAdded() && lastObject->getUseMouseEvents() )
               lastObject->onMouseEvent( "onMouseLeave" , event, worldMousePoint, MOUSE_EVENT_LEAVE ); // RKS: I added the MOUSE_EVENT_LEAVE
         }
         while( mLastPickVector.size() )
            mLastPickVector.popObject();
      }
      return;
   }

   // Fetch Pick Vector.
   typeSceneObjectVector pickVector = getSceneGraph()->getPickList();
   SimSet lastPickVector;
   for( S32 i = 0; i < mLastPickVector.size(); i++ )
      lastPickVector.addObject( mLastPickVector[i] );
	

   // We have to dance a bit to get onMouseLeave to work
   for( U32 i = 0; i < lastPickVector.size(); i++ )
   {
      t2dSceneObject* lastObject = static_cast<t2dSceneObject*>( lastPickVector[i] );
      if( !lastObject )
         continue;

      bool bFoundNew = true;
      for( U32 j = 0; j < pickVector.size(); j++ )
      {
         t2dSceneObject* object = pickVector[j];
         if(!object)
            continue;

         if( object == lastObject )
         {
            bFoundNew = false;
            break;
         }         
      }

	   
	   //Luma: Svenfix : This is messed up, with multitouch theres issues with mouseLeave, as mouse events happen in 2 places
      //                and logically there is only one mouse. If you are using the iPhone touch systems you need to cater for
      //                multitouch yourself. If you design with mouse events it is your own choice to flaw the design.
      //                Documentation will explain this nicely, dont expect best results if you use mouse events and multitouch.

	  if( lastObject && lastObject->isProperlyAdded() && bFoundNew == true && lastObject->getUseMouseEvents() )
		  lastObject->onMouseEvent( "onMouseLeave" , event, worldMousePoint, MOUSE_EVENT_LEAVE ); // RKS: I added the MOUSE_EVENT_LEAVE

   }
	

   // We have to dance a bit to get onMouseEnter to work
   for( U32 i = 0; i < pickVector.size(); i++ )
   {
      t2dSceneObject* object = pickVector[i];
      if(!object)
         continue;

      bool bFoundLast = false;
      for( U32 j = 0; j < lastPickVector.size(); j++ )
      {
         t2dSceneObject* lastObject = static_cast<t2dSceneObject*>( lastPickVector[j] );
         if(!lastObject)
            continue;

         if( lastObject == object )
         {
            bFoundLast = true;
            break;
         }         
      }

      if( bFoundLast == false && object->getUseMouseEvents() )
         object->onMouseEvent( "onMouseEnter" , event, worldMousePoint, MOUSE_EVENT_ENTER ); // RKS: I added the MOUSE_EVENT_ENTER
   }
	
	
	// RKS: I added this so we can get the first object touched
//	if( eMouseEvent == MOUSE_EVENT_DOWN && pickVector.size() > 0 )
//	{
//		t2dSceneObject* pFirstObject = pickVector[0];
//		if( pFirstObject )
//		{
//			if( CComponentGlobals::GetInstance().GetFirstTouchDownObject() == NULL )
//			{
//				printf( "First object touched: %s\n", pFirstObject->getName() );
//				CComponentGlobals::GetInstance().SetFirstTouchDownObject( pFirstObject );
//			}
//		}
//	}
	

   // Action Objects if they're monitoring mouse-events.  // RKS: THIS IS WHERE OBJECTS HANDLE MOUSE EVENTS
   for( U32 n = 0; n < pickVector.size(); n++ )
   {
      t2dSceneObject* object = pickVector[n];
      if(!object)
         continue;

      // Monitoring Mouse Events?
      if( object->getUseMouseEvents() && (!object->getMouseLocked()))
      {
		  ////////////////////////////////////////////////////////////////////////////////////////////////
		  // RKS: I added this so we can get the FIRST object touched
		  if( CComponentGlobals::GetInstance().GetFirstTouchDownObject() == NULL && 
			  CComponentGlobals::GetInstance().IsTouchingDown() == false &&
			  eMouseEvent == MOUSE_EVENT_DOWN )
		  {
			  //printf( "First object touched: %s\n", object->getName() );
			  CComponentGlobals::GetInstance().SetFirstTouchDownObject( object );
		  }
		  ////////////////////////////////////////////////////////////////////////////////////////////////
		  
         // Yes, so do callback.
         object->onMouseEvent( name, event, worldMousePoint, eMouseEvent ); // RKS: I added the eMouseEvent
      }
   }
	

	if( CComponentGlobals::GetInstance().IsTouchingObject() == false )
		CComponentGlobals::GetInstance().HandleGlobalMouseInput( eMouseEvent, worldMousePoint );
	
	
	
	// RKS NOTE: This needs to be done after handling all object mouse inputs
	if( eMouseEvent == MOUSE_EVENT_UP )
	{
		// RKS: If this is a mouse up event, clear the global touch flag
		CComponentGlobals::GetInstance().SetNotTouchingDown();
		CComponentGlobals::GetInstance().ClearTouchedObject();
	}
	else if( eMouseEvent == MOUSE_EVENT_DOWN )
	{
		// RKS NOTE: If this is a mouse down event, set the global touch flag
		CComponentGlobals::GetInstance().SetIsTouchingDown();
	}
	
	
   //Luma:	re-obtain the pickVector as some objects may have been deleted!
   getSceneGraph()->pickPoint( worldMousePoint, mMouseEventGroupMaskFilter, mMouseEventLayerMaskFilter, mMouseEventInvisibleFilter );
   pickVector = getSceneGraph()->getPickList();
	

   // Prune Pick Vector of non-mouse event controls
   //Luma:	U32 will cause issues if 1st element is removed... so change to S32 instead
   for( S32 n = 0; n < pickVector.size(); n++ )
   {
      t2dSceneObject* object = pickVector[n];
      if(!object)
         continue;

      // Monitoring Mouse Events?
      if( !object->getUseMouseEvents() )
      {
         pickVector.erase( &pickVector[n] );
         n--;
      }
   }


   // Store last pick list (Necessary for onMouseEnter/Leave callbacks)
   while( mLastPickVector.size() )
      mLastPickVector.popObject();
   
   while( lastPickVector.size() )
      lastPickVector.popObject();

   for( S32 i = 0; i < pickVector.size(); i++ )
      mLastPickVector.addObject( pickVector[i] );


   // Clear Pick-List.
   if(getSceneGraph())
      getSceneGraph()->clearPickList();

}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onMouseDown( const GuiEvent& event )
{
	//printf( "t2dSceneWindow::onMouseDown\n" ); //
	
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseLock();

    // Dispatch Mouse Event.
    dispatchMouseEvent("onMouseDown", event); // RKS TODO: Modify this so it also passes the MOUSE_EVENT into it, so I can avoid doing string compares
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onMouseUp( const GuiEvent& event )
{
	//printf( "t2dSceneWindow::onMouseUp\n" ); // 
	
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseUnlock();

    // Dispatch Mouse Event.
    dispatchMouseEvent("onMouseUp", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onMouseMove( const GuiEvent& event )
{
    // Dispatch Mouse Event.
    dispatchMouseEvent("onMouseMove", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onMouseDragged( const GuiEvent& event )
{
    // Dispatch Mouse Event.
    dispatchMouseEvent("onMouseDragged", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
// NOTE:-   This isn't handled by the event-dispatcher as it's only really
//          relevant to the window-events, not potential object-events.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onMouseEnter( const GuiEvent& event )
{
    // Send Windows Mouse Event.
    sendWindowMouseEvent("onMouseEnter", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
// NOTE:-   This isn't handled by the event-dispatcher as it's only really
//          relevant to the window-events, not potential object-events.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onMouseLeave( const GuiEvent& event )
{
    // Send Windows Mouse Event.
    sendWindowMouseEvent("onMouseLeave", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onRightMouseDown( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseLock();

    // Dispatch Mouse Event.
    dispatchMouseEvent("onRightMouseDown", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onRightMouseUp( const GuiEvent& event )
{
    // Lock Mouse (if necessary).
    if(mLockMouse)
        mouseUnlock();

    // Dispatch Mouse Event.
    dispatchMouseEvent("onRightMouseUp", event);
}


//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onRightMouseDragged( const GuiEvent& event )
{
    // Dispatch Mouse Event.
    dispatchMouseEvent("onRightMouseDragged", event);
}

//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
bool t2dSceneWindow::onMouseWheelUp( const GuiEvent& event )
{
   // Call Parent.
   Parent::onMouseWheelUp( event );

   // Dispatch Mouse Event.
   dispatchMouseEvent("onMouseWheelUp", event);

   // Return Success.
   return true;
}

//-----------------------------------------------------------------------------
// Mouse Event Handler.
//-----------------------------------------------------------------------------
bool t2dSceneWindow::onMouseWheelDown( const GuiEvent& event )
{
   // Call Parent.
   Parent::onMouseWheelDown( event );

   // Dispatch Mouse Event.
   dispatchMouseEvent("onMouseWheelDown", event);

   // Return Success.
   return true;
}


//-----------------------------------------------------------------------------
// Calculate Camera Mount.
//-----------------------------------------------------------------------------
void t2dSceneWindow::calculateCameraMount( const F32 elapsedTime )
{
   // Fetch Mount Position.
   const t2dVector& mountPos = mpMountedTo->getMountNode(mMountToID)->mWorldMountPosition;

   // Set Pre-Tick Position.
   mPreTickPosition = mPostTickPosition;

   // Set Current Camera Position.
   mCameraCurrent.mCameraWindow.point = mPreTickPosition;

   // Calculate Window Half-Dimensions.
   const F32 halfWidth = mCameraCurrent.mCameraWindow.len_x() * 0.5f;
   const F32 halfHeight = mCameraCurrent.mCameraWindow.len_y() * 0.5f;

   // Calculate Target Position.
   const Point2F targetPos = Point2F( mountPos.mX - halfWidth, mountPos.mY - halfHeight );

   // Rigid Mount?
   if ( mIsZero( mMountForce ) )
   {
      // Yes, so easy post-tick position.
      mPostTickPosition = targetPos;
      return;
   }

   // Calculate Time/Force Product.
   const F32 timeForce = elapsedTime * mMountForce;

   // Will we exceed our step?
   if ( timeForce > 1.0f )
   {
      // Yes, so clamp at step.
      mPostTickPosition = targetPos;
      return;
   }
   else
   {
      // No, so calculate Direction to move.
      const Point2F direction = (targetPos - mPreTickPosition) * timeForce;
      // Calculate post-tick position.
      mPostTickPosition = mPreTickPosition + direction;
      return;
   }
}


//-----------------------------------------------------------------------------
// Calculate Camera View.
//-----------------------------------------------------------------------------
void t2dSceneWindow::calculateCameraView( tCameraView* pCameraView )
{
    // Calculate Zoom Reciprocal.
    F32 zoomRecip = (pCameraView->mCameraZoom>0.0f) ? 1.0f/pCameraView->mCameraZoom : pCameraView->mCameraZoom;
    // Calculate Zoom X/Y Factors.
    F32 zoomFactorX = (pCameraView->mCameraWindow.len_x() - (pCameraView->mCameraWindow.len_x() * zoomRecip))/2;
    F32 zoomFactorY = (pCameraView->mCameraWindow.len_y() - (pCameraView->mCameraWindow.len_y() * zoomRecip))/2;
    // Fetch Camera View.
    pCameraView->mZoomedWindow = pCameraView->mCameraWindow;
    // Inset Window by Zoom Factor.
    pCameraView->mZoomedWindow.inset( zoomFactorX, zoomFactorY );
    // Ensure we've got a valid window.
    if ( !pCameraView->mZoomedWindow.isValidRect() )
        // Make it real!
        pCameraView->mZoomedWindow.extent = Point2F(1,1);

    // Calculate Scene Min/Max.
    pCameraView->mSceneMin.x = pCameraView->mZoomedWindow.point.x;
    pCameraView->mSceneMin.y = pCameraView->mZoomedWindow.point.y;
    pCameraView->mSceneMax.x = pCameraView->mZoomedWindow.point.x + pCameraView->mZoomedWindow.len_x();
    pCameraView->mSceneMax.y = pCameraView->mZoomedWindow.point.y + pCameraView->mZoomedWindow.len_y();

    // Is View Limit Active?
    if ( mViewLimitActive )
    {
        // Yes, so is the limit area X less than the current view X?
        if ( mViewLimitArea.mX < pCameraView->mZoomedWindow.len_x() )
        {
            // Yes, so calculate center of view.
            F32 viewCenterX = mViewLimitMin.mX + ( mViewLimitArea.mX * 0.5f );
            // Half Camera Width.
            F32 halfCameraX = pCameraView->mZoomedWindow.len_x() * 0.5f;

            // Calculate Min/Max X.
            pCameraView->mSceneMin.x = viewCenterX - halfCameraX;
            pCameraView->mSceneMax.x = viewCenterX + halfCameraX;
        }
        else
        {
            // No, so calculate window min overlap (positive)
            F32 windowMinOverlapX = getMax(0.0f, mViewLimitMin.mX - pCameraView->mSceneMin.x);
            // Calculate window max overlap (negative).
            F32 windowMaxOverlapX = getMin(0.0f, mViewLimitMax.mX - pCameraView->mSceneMax.x);

            // Adjust Window.
            pCameraView->mSceneMin.x += windowMinOverlapX + windowMaxOverlapX;
            pCameraView->mSceneMax.x += windowMinOverlapX + windowMaxOverlapX;
        }

        // Is the limit area Y less than the current view Y?
        if ( mViewLimitArea.mY < pCameraView->mZoomedWindow.len_y() )
        {
            // Yes, so calculate center of view.
            F32 viewCenterY = mViewLimitMin.mY + ( mViewLimitArea.mY * 0.5f );
            // Half Camera Height.
            F32 halfCameraY = pCameraView->mZoomedWindow.len_y() * 0.5f;

            // Calculate Min/Max Y.
            pCameraView->mSceneMin.y = viewCenterY - halfCameraY;
            pCameraView->mSceneMax.y = viewCenterY + halfCameraY;
        }
        else
        {
            // No, so calculate window min overlap (positive)
            F32 windowMinOverlapY = getMax(0.0f, mViewLimitMin.mY - pCameraView->mSceneMin.y);
            // Calculate window max overlap (negative).
            F32 windowMaxOverlapY = getMin(0.0f, mViewLimitMax.mY - pCameraView->mSceneMax.y);

            // Adjust Window.
            pCameraView->mSceneMin.y += windowMinOverlapY + windowMaxOverlapY;
            pCameraView->mSceneMax.y += windowMinOverlapY + windowMaxOverlapY;
        }

        // Recalculate Zoomed Window.
        pCameraView->mZoomedWindow.point = pCameraView->mSceneMin;
        pCameraView->mZoomedWindow.extent = pCameraView->mSceneMax - pCameraView->mSceneMin;
    }

    // Calculate Scene Window Scale.
    pCameraView->mSceneWindowScale.x = (pCameraView->mSceneMax.x - pCameraView->mSceneMin.x) / mBounds.len_x();
    pCameraView->mSceneWindowScale.y = (pCameraView->mSceneMax.y - pCameraView->mSceneMin.y) / mBounds.len_y();
}


//-----------------------------------------------------------------------------
// Handle View Resize.
//-----------------------------------------------------------------------------
void t2dSceneWindow::resize(const Point2I &newPosition, const Point2I &newExtent)
{
    // Resize Parent.
    Parent::resize( newPosition, newExtent);

    // Argument Buffer.
    char argBuffer[64];
    // Format Buffer.
    dSprintf( argBuffer, 64, "%d %d %d %d", newPosition.x, newPosition.y, newExtent.x, newExtent.y );

    // Resize Callback.
    Con::executef( this, 2, "onExtentChange", argBuffer );
}


//-----------------------------------------------------------------------------
// Process Tick.
//-----------------------------------------------------------------------------
void t2dSceneWindow::processTick( void )
{
    // Are we moving the camera.
    if ( mMovingCamera )
    {
        // Yes, so add Elapsed Time (scaled appropriately).
        mCurrentCameraTime += ITickable::smTickSec;

        // Update Tick Camera Time.
        updateTickCameraTime();

        // Update Camera.
        updateCamera();
    } 

    // [neo, 5/7/2007 - #2978]
    //else  <---- no else we always want to update camera shake even when mounted

    // Is the Camera Shaking?
    if ( mCameraShaking )
    {
        // Reduce Shake Life.
        mShakeLife -= ITickable::smTickSec;

        // Is the Shake still active?
        if ( mShakeLife > 0.0f )
        {
            // Calculate Current Shake.
            mCurrentShake -= mShakeRamp * ITickable::smTickSec;
            // Clamp Shake.
            mCurrentShake = getMax(mCurrentShake, 0.0f);
        }
        else
        {
            // No, so stop shake.
            stopCameraShake();
        }
    }    
}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dSceneWindow::interpolateTick( F32 timeDelta )
{
    // Are we moving the camera.
    if ( mMovingCamera )
    {
        // Calculate Render Tick Position.
        mRenderCameraTime = (mPreCameraTime * timeDelta) + ((1.0f-timeDelta) * mPostCameraTime);

        // Update Camera.
        updateCamera();
    }
}


//-----------------------------------------------------------------------------
// Interpolate Camera Mount.
//-----------------------------------------------------------------------------
void t2dSceneWindow::interpolateCameraMount( const F32 timeDelta )
{
    // Camera Mounted?
    if ( !isCameraMounted() ) return;

    // Calculate position.
    mCameraCurrent.mCameraWindow.point = (mPreTickPosition * timeDelta) + ((1.0f-timeDelta) * mPostTickPosition);  
}


//-----------------------------------------------------------------------------
// Reset Tick Camera Time.
//-----------------------------------------------------------------------------
void t2dSceneWindow::zeroCameraTime( void )
{
    // Reset Camera Time.
    mRenderCameraTime = mPreCameraTime = mPostCameraTime = mCurrentCameraTime = 0.0f;
}



//-----------------------------------------------------------------------------
// Reset Tick Camera Time.
//-----------------------------------------------------------------------------
void t2dSceneWindow::resetTickCameraTime( void )
{
    // Reset Camera Time.
    mRenderCameraTime = mPreCameraTime = mPostCameraTime = mCurrentCameraTime;
}


//-----------------------------------------------------------------------------
// Update Tick Camera Time.
//-----------------------------------------------------------------------------
void t2dSceneWindow::updateTickCameraTime( void )
{
    // Store Pre Camera Time.
    mPreCameraTime = mPostCameraTime;

    // Store Current Camera Time.
    mPostCameraTime = mCurrentCameraTime;

    // Render Camera Time is at Pre-Tick Time.
    mRenderCameraTime = mPreCameraTime;
}


//-----------------------------------------------------------------------------
// Reset Tick Camera Position.
//-----------------------------------------------------------------------------
void t2dSceneWindow::resetTickCameraPosition( void )
{
    mPreTickPosition = mPostTickPosition = mCameraCurrent.mCameraWindow.point;
}


//-----------------------------------------------------------------------------
// Render Scene Window.
//-----------------------------------------------------------------------------
void t2dSceneWindow::onRender( Point2I offset, const RectI& updateRect )
{
    // Fetch Scenegraph.
    t2dSceneGraph* pT2DSceneGraph = getSceneGraph();

    // Cannot render without scenegraph!
    if ( !pT2DSceneGraph )
        return;

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneWindow_onRender);
#endif

    // Get Debug Stats.
    CDebugStats& debugStats = pT2DSceneGraph->getDebugStats();

    // Set Screen Window.
    setScreenWindow( offset, updateRect );

    // Set Current Render Window.
    pT2DSceneGraph->setCurrentRenderWindow( this );

    // Calculate current camera View ( if needed ).
    calculateCameraView( &mCameraCurrent );

    // Setup new logical coordinate system.
    glMatrixMode(GL_PROJECTION);
#ifdef TORQUE_OS_IPHONE
    glLoadIdentity();
    RectI viewport;
    dglGetViewport(&viewport);

    // Is the Camera Shaking?
    if ( mCameraShaking )
    {
        // Yes, so calculate the Screen Shake-Ratio.
        Point2F shakeRatio( mCameraCurrent.mZoomedWindow.len_x() / F32(updateRect.len_x()), mCameraCurrent.mZoomedWindow.len_y() / F32(updateRect.len_y()) );
        // Calculate the Camera Shake Magnitude based upon the Screen Shake-Ratio.
        F32 shakeMagnitudeX = mCurrentShake * shakeRatio.x * 0.5f;
        F32 shakeMagnitudeY = mCurrentShake * shakeRatio.y * 0.5f;
        // Choose a random Shake.
        mCameraShakeOffset.set( mGetT2DRandomF( -shakeMagnitudeX, shakeMagnitudeX ), mGetT2DRandomF( -shakeMagnitudeY, shakeMagnitudeY ) );

        // Calculate Shaking Scene Portal.
        Point2F windowToScene( (mCameraCurrent.mSceneMax.x-mCameraCurrent.mSceneMin.x) / F32(mBounds.len_x()),(mCameraCurrent.mSceneMax.y-mCameraCurrent.mSceneMin.y) / F32(mBounds.len_y()) );
        Point2F sceneMin( (-offset.x + updateRect.point.x) * windowToScene.x + mCameraCurrent.mSceneMin.x + mCameraShakeOffset.mX, (-offset.y + updateRect.point.y) * windowToScene.y + mCameraCurrent.mSceneMin.y + mCameraShakeOffset.mY );
        Point2F sceneMax( (-offset.x + updateRect.point.x + updateRect.extent.x) * windowToScene.x + mCameraCurrent.mSceneMin.x + mCameraShakeOffset.mX, (-offset.y + updateRect.point.y + updateRect.extent.y) * windowToScene.y + mCameraCurrent.mSceneMin.y + mCameraShakeOffset.mY );
		glOrthof( sceneMin.x, sceneMax.x, sceneMax.y, sceneMin.y, 0, t2dSceneGraph::maxLayersSupported );
   
	}
    else
    {
        // No, so calculate standard Scene Portal.
        Point2F windowToScene( (mCameraCurrent.mSceneMax.x-mCameraCurrent.mSceneMin.x) / F32(mBounds.len_x()),(mCameraCurrent.mSceneMax.y-mCameraCurrent.mSceneMin.y) / F32(mBounds.len_y()) );
        Point2F sceneMin( (-offset.x + updateRect.point.x) * windowToScene.x + mCameraCurrent.mSceneMin.x, (-offset.y + updateRect.point.y) * windowToScene.y + mCameraCurrent.mSceneMin.y );
        Point2F sceneMax( (-offset.x + updateRect.point.x + updateRect.extent.x) * windowToScene.x + mCameraCurrent.mSceneMin.x, (-offset.y + updateRect.point.y + updateRect.extent.y) * windowToScene.y + mCameraCurrent.mSceneMin.y );
        glOrthof( sceneMin.x, sceneMax.x, sceneMax.y, sceneMin.y, 0.0f, t2dSceneGraph::maxLayersSupported );
 
	}

    // Setup new viewport.
    dglSetViewport(updateRect);
 	
    // Set ModelView.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Enable Alpha Test. // RKS NOTE: The two following lines were commented out according to this optimization: http://www.garagegames.com/community/forums/viewthread/127309
    //glEnable        ( GL_ALPHA_TEST );		// RKS: Commented
    //glAlphaFunc     ( GL_GREATER, 0.0f );		// RKS: Commented
    
    glDisable       ( GL_DEPTH_TEST );
    //glEnable      ( GL_DEPTH_TEST );
    //glDepthFunc       ( GL_LEQUAL );

	// implement "Don't Render Object" functionality.  Hide it before we render, unhide it afterwards.
	bool previousDontRenderObjectVisibility = false;
	if ((mpDontRenderObject != NULL) && mpDontRenderObject->mVisible)
	{
		previousDontRenderObjectVisibility = true;
		mpDontRenderObject->mVisible = false;
	}
	
    // Render View.
    pT2DSceneGraph->renderView( mCameraCurrent.mZoomedWindow, mRenderLayerMask, mRenderGroupMask, &debugStats );
	
	// restore visibility of hidden object.
	if ((mpDontRenderObject != NULL) && previousDontRenderObjectVisibility)
		mpDontRenderObject->mVisible = previousDontRenderObjectVisibility;

    // Disable Alpha/Depth-Tests.
    glDisable       ( GL_ALPHA_TEST );
    //glDisable     ( GL_DEPTH_TEST );

    // Restore Matrices.
	glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
	
	
	glEnable        ( GL_BLEND );
	glBlendFunc     ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
	

    // Output Debug Statistics?
    if ( pT2DSceneGraph->getDebugMask() & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_STATISTICS )
    {
        // Yes, so output Debug Strings...

// -Mat Works, but text drawn is too big to show on screen

        // Blending for banner background.
        glEnable        ( GL_BLEND );
        glBlendFunc     ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
        // Set banner background colour.
        glColor4f( mDebugBannerColour.red, mDebugBannerColour.green, mDebugBannerColour.blue, mDebugBannerColour.alpha );

        // Calculate Banner Height.
        U32 bannerHeight = 10 * mpDebugFont->getHeight();

        // Calculate Debug Banner Offset.
        

        // Draw Banner Background.
		GLfloat verts[] = {
            updateRect.point.x, updateRect.point.y,
            updateRect.point.x + updateRect.extent.x, updateRect.point.y,
            updateRect.point.x, updateRect.point.y + bannerHeight,
            updateRect.point.x + updateRect.extent.x, updateRect.point.y + bannerHeight,
		};
		glVertexPointer(2, GL_FLOAT, 0, verts);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Disable Banner Blending.
        glDisable       ( GL_BLEND );
		
		Point2I bannerOffset = updateRect.point + Point2I(8,8);
		
#else
    glPushMatrix();
    glLoadIdentity();
    RectI viewport;
    dglGetViewport(&viewport);

    // Is the Camera Shaking?
    if ( mCameraShaking )
    {
        // Yes, so calculate the Screen Shake-Ratio.
        Point2F shakeRatio( mCameraCurrent.mZoomedWindow.len_x() / F32(updateRect.len_x()), mCameraCurrent.mZoomedWindow.len_y() / F32(updateRect.len_y()) );
        // Calculate the Camera Shake Magnitude based upon the Screen Shake-Ratio.
        F32 shakeMagnitudeX = mCurrentShake * shakeRatio.x * 0.5f;
        F32 shakeMagnitudeY = mCurrentShake * shakeRatio.y * 0.5f;
        // Choose a random Shake.
        mCameraShakeOffset.set( mGetT2DRandomF( -shakeMagnitudeX, shakeMagnitudeX ), mGetT2DRandomF( -shakeMagnitudeY, shakeMagnitudeY ) );

        // Calculate Shaking Scene Portal.
        Point2F windowToScene( (mCameraCurrent.mSceneMax.x-mCameraCurrent.mSceneMin.x) / F32(mBounds.len_x()),(mCameraCurrent.mSceneMax.y-mCameraCurrent.mSceneMin.y) / F32(mBounds.len_y()) );
        Point2F sceneMin( (-offset.x + updateRect.point.x) * windowToScene.x + mCameraCurrent.mSceneMin.x + mCameraShakeOffset.mX, (-offset.y + updateRect.point.y) * windowToScene.y + mCameraCurrent.mSceneMin.y + mCameraShakeOffset.mY );
        Point2F sceneMax( (-offset.x + updateRect.point.x + updateRect.extent.x) * windowToScene.x + mCameraCurrent.mSceneMin.x + mCameraShakeOffset.mX, (-offset.y + updateRect.point.y + updateRect.extent.y) * windowToScene.y + mCameraCurrent.mSceneMin.y + mCameraShakeOffset.mY );
        glOrtho( sceneMin.x, sceneMax.x, sceneMax.y, sceneMin.y, 0, t2dSceneGraph::maxLayersSupported );
    }
    else
    {
        // No, so calculate standard Scene Portal.
        Point2F windowToScene( (mCameraCurrent.mSceneMax.x-mCameraCurrent.mSceneMin.x) / F32(mBounds.len_x()),(mCameraCurrent.mSceneMax.y-mCameraCurrent.mSceneMin.y) / F32(mBounds.len_y()) );
        Point2F sceneMin( (-offset.x + updateRect.point.x) * windowToScene.x + mCameraCurrent.mSceneMin.x, (-offset.y + updateRect.point.y) * windowToScene.y + mCameraCurrent.mSceneMin.y );
        Point2F sceneMax( (-offset.x + updateRect.point.x + updateRect.extent.x) * windowToScene.x + mCameraCurrent.mSceneMin.x, (-offset.y + updateRect.point.y + updateRect.extent.y) * windowToScene.y + mCameraCurrent.mSceneMin.y );
        glOrtho( sceneMin.x, sceneMax.x, sceneMax.y, sceneMin.y, 0.0f, t2dSceneGraph::maxLayersSupported );
    }

    // Setup new viewport.
    dglSetViewport(updateRect);

    // Set ModelView.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Enable Alpha Test.
    glEnable        ( GL_ALPHA_TEST );
    glAlphaFunc     ( GL_GREATER, 0.0f );
    
    glDisable       ( GL_DEPTH_TEST );
    //glEnable      ( GL_DEPTH_TEST );
    //glDepthFunc       ( GL_LEQUAL );

	// implement "Don't Render Object" functionality.  Hide it before we render, unhide it afterwards.
	bool previousDontRenderObjectVisibility = false;
	if ((mpDontRenderObject != NULL) && mpDontRenderObject->mVisible)
	{
		previousDontRenderObjectVisibility = true;
		mpDontRenderObject->mVisible = false;
	}

    // Render View.
    pT2DSceneGraph->renderView( mCameraCurrent.mZoomedWindow, mRenderLayerMask, mRenderGroupMask, &debugStats );

	// restore visibility of hidden object.
	if ((mpDontRenderObject != NULL) && previousDontRenderObjectVisibility)
		mpDontRenderObject->mVisible = previousDontRenderObjectVisibility;

    // Disable Alpha/Depth-Tests.
    glDisable       ( GL_ALPHA_TEST );
    //glDisable     ( GL_DEPTH_TEST );

    // Restore Matrices.
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Output Debug Statistics?
    if ( pT2DSceneGraph->getDebugMask() & t2dSceneGraph::T2D_SCENEGRAPH_DEBUG_STATISTICS )
    {
        // Yes, so output Debug Strings...

        // Blending for banner background.
        glEnable        ( GL_BLEND );
        glBlendFunc     ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
        // Set banner background colour.
        glColor4fv( (GLfloat*)&mDebugBannerColour );

        // Calculate Banner Height.
        U32 bannerHeight = 10 * mpDebugFont->getHeight();

        // Calculate Debug Banner Offset.
        Point2I bannerOffset = updateRect.point + Point2I(8,8);

        // Draw Banner Background.
        glBegin(GL_QUADS);
            glVertex2i( updateRect.point.x, updateRect.point.y );
            glVertex2i( updateRect.point.x + updateRect.extent.x, updateRect.point.y );
            glVertex2i( updateRect.point.x + updateRect.extent.x, updateRect.point.y + bannerHeight + 16);
            glVertex2i( updateRect.point.x, updateRect.point.y + bannerHeight + 16);
        glEnd();

        // Disable Banner Blending.
        glDisable       ( GL_BLEND );
#endif
        // Set Debug Text Colour.
        dglSetBitmapModulation( mDebugTextColour );

        // ****************************************************************
        // Draw Banner Text.
        // ****************************************************************

        // Scene Objects.
        dSprintf( mDebugText, sizeof( mDebugText ), "TotalObj : %7.1d /", pT2DSceneGraph->getSceneObjectCount() );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*2), mDebugText, NULL );

        // Scene Timing.
        dSprintf( mDebugText, sizeof( mDebugText ), "SceneTime: %7.1f / ActualFPS: %7.1f /    MinFPS: %7.1f  /    MaxFPS: %7.1f / DeltaFPS: %7.1f /", pT2DSceneGraph->getSceneTime(), Con::getFloatVariable("fps::real",0.0f), pT2DSceneGraph->getMinFPS(), pT2DSceneGraph->getMaxFPS(), pT2DSceneGraph->getMaxFPS()-pT2DSceneGraph->getMinFPS() );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*3), mDebugText, NULL );

        // Camera Window.
        t2dVector cameraPosition = getCurrentCameraPosition();
        dSprintf( mDebugText, sizeof( mDebugText ), "X-Pos    : %7.1f / Y-Pos    : %7.1f / X-Width  : %7.1f  / Y-Height : %7.1f / Zoom    : %7.1f /", cameraPosition.mX, cameraPosition.mY, mCameraCurrent.mCameraWindow.extent.x, mCameraCurrent.mCameraWindow.extent.y, mCameraCurrent.mCameraZoom );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*4), mDebugText, NULL );

        dSprintf( mDebugText, sizeof( mDebugText ), "X-Min    : %7.1f / Y-Min    : %7.1f / X-Max    : %7.1f  / Y-Max    : %7.1f /", mCameraCurrent.mCameraWindow.point.x, mCameraCurrent.mCameraWindow.point.y, mCameraCurrent.mCameraWindow.point.x + mCameraCurrent.mCameraWindow.extent.x, mCameraCurrent.mCameraWindow.point.y + mCameraCurrent.mCameraWindow.extent.y );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*5), mDebugText, NULL );

        // Container System.
        dSprintf( mDebugText, sizeof( mDebugText ), "BinReloc : %7.1d / MaxReloc : %7.1d / BinCollis: %7.1d  / MaxCollis: %7.1d / BinSearch: %7.1d", debugStats.objectBinRelocations, pT2DSceneGraph->getMaxBinRelocations(), debugStats.objectBinCollisions, pT2DSceneGraph->getMaxBinCollisions(), debugStats.objectBinSearches );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*6), mDebugText, NULL );

        // Collisions.
        dSprintf( mDebugText, sizeof( mDebugText ), "PotCol   : %7.1d / ActCol   : %7.1d / ColHit   : %7.1f%% / Contacts : %7.1d /", debugStats.objectPotentialCollisions, debugStats.objectActualCollisions, debugStats.objectHitPercCollisions, debugStats.objectContactPairs );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*7), mDebugText, NULL );

        // Rendering.
        dSprintf( mDebugText, sizeof( mDebugText ), "PotRender: %7.1d / ActRender: %7.1d / RenderHit: %7.1f%% / SortedObj: %7.1d /", debugStats.objectsPotentialRender, debugStats.objectsActualRendered, debugStats.objectsHitPercRendered, debugStats.objectsLayerSorted );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*8), mDebugText, NULL );

        // Particles.
        dSprintf( mDebugText, sizeof( mDebugText ), "ParFree  : %7.1d / ParUsed  : %7.1d /", debugStats.objectParticlesAvailable, debugStats.objectParticlesActive );
        dglDrawText( mpDebugFont, bannerOffset + Point2I(0,mpDebugFont->getHeight()*9), mDebugText, NULL );

        // Clear Bitmap Modulation.
        dglClearBitmapModulation();
    }

    // Reset Current Render Window.
    // NOTE:-   We should do this before the children render in-case
    //          the children are other t2dSceneWindow objects.
    pT2DSceneGraph->setCurrentRenderWindow( NULL );

    // Render Children.
    renderChildControls(offset, updateRect);

    // Update Window.
    setUpdate();

    // Restore old viewport.
    dglSetViewport(viewport);

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneWindow_onRender
#endif
}


