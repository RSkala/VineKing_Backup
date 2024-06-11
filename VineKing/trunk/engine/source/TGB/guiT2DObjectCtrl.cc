//-----------------------------------------------------------------------------
// T2D Object Renderer
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "gui/core/guiDefaultControlRender.h"
#include "guiT2DObjectCtrl.h"
#include "platform/profiler.h"

// -----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(guiT2DObjectCtrl);

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
guiT2DObjectCtrl::guiT2DObjectCtrl(void)
{
   // Reset Scene Object Name.
   mSceneObjectName = StringTable->insert("");

   // Default to no render margin
   mMargin = 0;

   mCaption = StringTable->insert("");

   mStateOn = false;
   mButtonType = ButtonTypeRadio;

}

// -----------------------------------------------------------------------------
// Persistant Fields.
// -----------------------------------------------------------------------------
void guiT2DObjectCtrl::initPersistFields()
{
   // Call Parent.
   Parent::initPersistFields();

   // Define Fields.
   addGroup("guiT2DObjectCtrl");
   addField("renderMargin", TypeS32, Offset(mMargin, guiT2DObjectCtrl));
   addField("sceneObject", TypeString, Offset(mSceneObjectName, guiT2DObjectCtrl));
   endGroup("guiT2DObjectCtrl");
}


// -----------------------------------------------------------------------------
// Wake!
// -----------------------------------------------------------------------------
bool guiT2DObjectCtrl::onWake()
{
   // Call Parent.
   if (!Parent::onWake())
      return false;

   if( mProfile->constructBitmapArray() >= 36 )
      mHasTexture = true;
   else
      mHasTexture = false;

   // Activate.
   setActive(true);

   // All Okay.
   return true;
}


// -----------------------------------------------------------------------------
// Sleep!
// -----------------------------------------------------------------------------
void guiT2DObjectCtrl::onSleep()
{
   Parent::onSleep();
}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void guiT2DObjectCtrl::inspectPostApply()
{
   // Call Parent.
   Parent::inspectPostApply();

   // Set the Scene Object.
   setSceneObject( mSceneObjectName );
}


// -----------------------------------------------------------------------------
// Set Scene Object.
// -----------------------------------------------------------------------------
ConsoleMethod( guiT2DObjectCtrl, setSceneObject, void, 3, 3, "(string obj) Set the T2D sceneObject displayed in the control."
			  "@param obj Either the object's ID or its name."
			  "@return No return value.")
{
   // Set Scene Object.
   object->setSceneObject( argv[2] );
}
// Set Scene Object.
void guiT2DObjectCtrl::setSceneObject( const char* name )
{
   // Reset existing object.
   mSelectedSceneObject = NULL;

   // Get Scene Name.
   mSceneObjectName = StringTable->insert( name ? name : "" );

   // Valid Scene Object Name?
   if ( *mSceneObjectName )
   {
      // Fetch Scene Object.
      t2dSceneObject* pSceneObject = dynamic_cast<t2dSceneObject*>(Sim::findObject( name ));
      // Valid?
      if ( pSceneObject )
      {
         // Yes, so set Scene Object.
         mSelectedSceneObject = pSceneObject;
      }
   }

   else
      mSelectedSceneObject = NULL;

   // Do an update.
   setUpdate();
}

ConsoleMethod( guiT2DObjectCtrl, getSceneObject, const char*, 2,2, "() \n@return Returns displaying sceneobject id")
{
   t2dSceneObject *sceneObject = object->getSceneObject();
   if( !sceneObject )
      return "";

   return sceneObject->getIdString();
}



ConsoleMethod( guiT2DObjectCtrl, setCaption, void, 3, 3, "(string caption) Sets the object caption to specified string.\n"
			  "@return No return value.")
{
   object->setCaption( argv[2] );
}

void guiT2DObjectCtrl::setCaption( const char* caption )
{
   if( caption != NULL )
      mCaption = StringTable->insert( caption );
}

//
//
//
void guiT2DObjectCtrl::onMouseUp(const GuiEvent &event)
{
   if( mDepressed && ( event.mouseClickCount % 2 ) == 0 )
         Con::executef( this, 2, "onDoubleClick" );    

   Parent::onMouseUp( event );
}

void guiT2DObjectCtrl::onMouseLeave( const GuiEvent &event )
{
   Con::executef( this, 2, "onMouseLeave" );

   Parent::onMouseLeave( event );
}

void guiT2DObjectCtrl::onMouseEnter( const GuiEvent &event )
{
   Con::executef( this, 2, "onMouseEnter" );

   Parent::onMouseEnter( event );
}

void guiT2DObjectCtrl::onMouseDragged( const GuiEvent &event )
{
   Con::executef( this, 2, "onMouseDragged" );

   Parent::onMouseDragged( event );
}

// -----------------------------------------------------------------------------
// Render any selected Scene Object.
// -----------------------------------------------------------------------------
void guiT2DObjectCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	PROFILE_SCOPE(guiT2DObjectCtrl_onRender);

   RectI ctrlRect( offset, mBounds.extent );

   // Draw Background
   if( mProfile->mOpaque )
   {
      if( mDepressed || mStateOn )
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( ctrlRect, 3, mProfile );
         else
            dglDrawRectFill( ctrlRect, mProfile->mFillColorHL );
      }
      else if ( mMouseOver )
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( ctrlRect, 2, mProfile );
         else
            dglDrawRectFill( ctrlRect, mProfile->mFillColorHL );
      }
      else
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( ctrlRect, 1, mProfile );
         else
            dglDrawRectFill( ctrlRect, mProfile->mFillColor );
      }
   }

   //// Render Border.
   //if( mProfile->mBorder || mStateOn )    
   //   dglDrawRect(ctrlRect, mProfile->mBorderColor);


   // Valid Scene Object?
   if ( !mSelectedSceneObject.isNull() )
   {
      RectI objRect = updateRect;
      objRect.inset( mMargin, mMargin );
      RectI ctrlRectInset = ctrlRect;
      ctrlRectInset.inset( mMargin, mMargin); 

      // Draw Canvas color for object
      if ( mProfile->mOpaque )
      {
         if( mHasTexture )
            renderSizableBitmapBordersFilled( objRect, 4, mProfile );
         else
            dglDrawRectFill( objRect, mProfile->mFillColorNA );
      }

      // Yes, so fetch object clip boundary.
      const t2dVector* pClipBoundary = mSelectedSceneObject->getWorldClipBoundary();

      // Calculate the GUI-Control Clip Boundary.
      const F32 xscale = (pClipBoundary[1].mX - pClipBoundary[0].mX) / ctrlRectInset.extent.x;
      const F32 yscale = (pClipBoundary[2].mY - pClipBoundary[0].mY) / ctrlRectInset.extent.y;

      F32 x1 = pClipBoundary[0].mX + ( objRect.point.x - ctrlRectInset.point.x) * xscale;
      F32 x2 = pClipBoundary[1].mX + ( objRect.point.x + objRect.extent.x - ctrlRectInset.extent.x - ctrlRectInset.point.x) * xscale;
      F32 y1 = pClipBoundary[0].mY + ( objRect.point.y - ctrlRectInset.point.y) * yscale;
      F32 y2 = pClipBoundary[2].mY + ( objRect.point.y + objRect.extent.y - ctrlRectInset.extent.y - ctrlRectInset.point.y) * yscale;

      t2dVector size = mSelectedSceneObject->getSize();
      if (size.mX > size.mY)
      {
         S32 center = objRect.point.y + (objRect.extent.y / 2);
         objRect.extent.y *= size.mY / size.mX;
         objRect.point.y = center - (objRect.extent.y / 2);
      }
      else
      {
         S32 center = objRect.point.x + (objRect.extent.x / 2);
         objRect.extent.x *= size.mX / size.mY;
         objRect.point.x = center - (objRect.extent.x / 2);
      }

      // Setup new logical coordinate system.
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      RectI viewport;
      dglGetViewport(&viewport);

      if (x1 > x2)
      {
         F32 temp = x1;
         x1 = x2;
         x2 = temp;
      }
      if (y1 > y2)
      {
         F32 temp = y1;
         y1 = y2;
         y2 = temp;
      }

      // Setup Orthographic Projection for Object Area.
#ifdef TORQUE_OS_IPHONE
      glOrthof( x1, x2, y2, y1, 0.0f, t2dSceneGraph::maxLayersSupported );
#else
      glOrtho( x1, x2, y2, y1, 0.0f, t2dSceneGraph::maxLayersSupported );
#endif
	  // Setup new viewport.
      dglSetViewport(objRect);

      // Set ModelView.
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      // Enable Alpha Test.
      glEnable        ( GL_ALPHA_TEST );
      glAlphaFunc     ( GL_GREATER, 0.0f );

      // Calculate maximal clip bounds.
      RectF clipBounds( -x1,-y1, x2-x1, y2-y1 );

      // Render Object in GUI-space.
      // by the way - the arguments to t2dSceneObject::renderObject() are not used by any existing t2dSceneObject.
      mSelectedSceneObject->renderObject( clipBounds, clipBounds );

      // Restore Standard Settings.
      glDisable       ( GL_DEPTH_TEST );
      glDisable       ( GL_ALPHA_TEST );

      // Restore Matrices.
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
   }
   else
   {
      // No Object so reset name.
      mSceneObjectName = NULL;
   }


   RectI captionRect = ctrlRect;
   captionRect.point.y += (captionRect.extent.y / 8);


   captionRect.inset(1, 1);
   dglSetBitmapModulation( ColorI(0,0,0,255) );
   renderJustifiedText(captionRect.point, captionRect.extent, mCaption);
   captionRect.inset(1, 1);   
   dglSetBitmapModulation( mProfile->mFontColor );
   renderJustifiedText(captionRect.point, captionRect.extent, mCaption);

   

   // Render Child Controls.
   renderChildControls(offset, updateRect);
}
