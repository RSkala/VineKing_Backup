//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// T2D Shape Vector.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "./t2dUtility.h"
#include "./t2dShapeVector.h"


//----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(t2dShapeVector);

//----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dShapeVector::t2dShapeVector() :	T2D_Stream_HeaderID(makeFourCCTag('2','D','S','V')),
                                    mLineColour(ColorF(1.0f,1.0f,1.0f,1.0f)),
                                    mFillColour(ColorF(0.5f,0.5f,0.5f,1.0f)),
                                    mFillMode( false ),
                                    mPolygonScale( 1.0f, 1.0f )
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mPolygonBasisList );
    VECTOR_SET_ASSOCIATION( mPolygonLocalList );
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dShapeVector::~t2dShapeVector()
{
}

//-----------------------------------------------------------------------------
// Persistent Fields
//-----------------------------------------------------------------------------
void t2dShapeVector::initPersistFields()
{
   Parent::initPersistFields();

	// UNUSED: JOSEPH THOMAS -> const char*  in_pFieldname = "PolyList";
    // UNUSED: JOSEPH THOMAS -> const U32 in_fieldType = TypePoint2FVector;
    // UNUSED: JOSEPH THOMAS -> const dsize_t in_fieldOffset = Offset(mPolygonBasisList, t2dShapeVector);
    // UNUSED: JOSEPH THOMAS -> AbstractClassRep::SetDataNotify in_setDataFn = &setLineColour;
    // UNUSED: JOSEPH THOMAS -> AbstractClassRep::GetDataNotify in_getDataFn = &defaultProtectedGetFn;
    // UNUSED: JOSEPH THOMAS -> const char* in_pFieldDocs = "";

   addProtectedField("PolyList", TypePoint2FVector, Offset(mPolygonBasisList, t2dShapeVector), &setPolyCustom, &defaultProtectedGetFn, "");
   addProtectedField("LineColor", TypeColorF, Offset(mLineColour, t2dShapeVector), &setLineColour, &defaultProtectedGetFn, "");
   addProtectedField("FillColor", TypeColorF, Offset(mFillColour, t2dShapeVector), &setFillColour, &defaultProtectedGetFn, "");
   addProtectedField("FillMode", TypeBool, Offset(mFillMode, t2dShapeVector), &setFillMode, &defaultProtectedGetFn, "");
}


//-----------------------------------------------------------------------------
// Clone With Behaviors Support.
//-----------------------------------------------------------------------------
void t2dShapeVector::copyTo(SimObject* obj)
{
   Parent::copyTo(obj);

   AssertFatal(dynamic_cast<t2dShapeVector*>(obj), "t2dShapeVector::copyTo - Copy object is not a t2dShapeVector!");
   t2dShapeVector* object = static_cast<t2dShapeVector*>(obj);

   // Copy fields
   object->mFillMode = mFillMode;
   object->mFillColour = mFillColour;
   object->mLineColour = mLineColour;
   if (getPolyVertexCount() > 0)
	   object->setPolyCustom(mPolygonBasisList.size(), getPoly());
}




//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dShapeVector::onAdd()
{
   // Call Parent.
   if(!Parent::onAdd())
      return false;

   // Return Okay.
   return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dShapeVector::onRemove()
{
   // Call Parent.
   Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Set Flip.
//-----------------------------------------------------------------------------
void t2dShapeVector::setFlip( const bool flipX, const bool flipY )
{
    // Call Parent.
    Parent::setFlip(flipX, flipY);

    // Generate Local Polygon.
    generateLocalPoly();
}


//-----------------------------------------------------------------------------
// Set Size.
//-----------------------------------------------------------------------------
void t2dShapeVector::setSize( const t2dVector& size )
{
    // Call Parent.
    Parent::setSize( size );

    // Generate Local Polygon.
    generateLocalPoly();
}


//-----------------------------------------------------------------------------
// Set Polygon Scale.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setPolyScale, void, 3, 3, "(widthScale / [heightScale]) - Sets the polygon scale.\n"
			  "@param width/heightScale The scale values of the given polygon. If no height is specified, the widthScale value is repeated.\n"
			  "@return No return value.")
{
    // Calculate Element Count.
    const U32 elementCount = t2dSceneObject::getStringElementCount( argv[2] );

    // Check Parameters.
    if ( elementCount < 1 )
    {
        Con::warnf("t2dShapeVector::setPolyScale() - Invalid number of parameters!");
        return;
    }

    // Fetch Width Scale.
    t2dVector scale;
    scale.mX = dAtof(t2dSceneObject::getStringElement(argv[2],0));
    // Use Fixed-Aspect for Scale if Height-Scale not specified.
    if ( elementCount == 2 )
    {
        // Specified Height Scale.
        scale.mY = dAtof(t2dSceneObject::getStringElement(argv[2],1));
    }
    else
    {
        // Fixed-Aspect Scale.
        scale.mY = scale.mX;
    }

    // Set Polygon Scale.
    object->setPolyScale( scale );
}   
// Set Polygon Scale.
void t2dShapeVector::setPolyScale( const t2dVector& scale )
{
    // Check Scales.
    if ( scale.mX <= 0.0f || scale.mY <= 0.0f )
    {
        Con::warnf("t2dShapeVector::setPolyScale() - Polygon Scales must be greater than zero! '%f,%f'.", scale.mX, scale.mY);
        return;
    }
    // Check Scales.
    if ( scale.mX > 1.0f || scale.mY > 1.0f )
    {
        Con::warnf("t2dShapeVector::setPolyScale() - Polygon Scales cannot be greater than one! '%f,%f'.", scale.mX, scale.mY);
        return;
    }

    // Set Polygon Scale.
    mPolygonScale = scale;

    // Generation Local Poly.
    generateLocalPoly();
}


//-----------------------------------------------------------------------------
// Set Polygon Primitive.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setPolyPrimitive, void, 3, 3, "(vertexCount) Sets a regular polygon primitive.\n"
			  "@return No return value.")
{
    // Set Polygon Primitive.
    object->setPolyPrimitive( dAtoi(argv[2]) );
}
// Set Polygon Primitive.
void t2dShapeVector::setPolyPrimitive( const U32 polyVertexCount )
{
    // Check it's not zero!
    if ( polyVertexCount == 0 )
    {
        // Warn.
        Con::warnf("t2dShapeVector::setPolyPrimitive() - Vertex count must be greater than zero!");
        // Finish Here.
        return;
    }

    // Clear Polygon List.
    mPolygonBasisList.clear();
    mPolygonBasisList.setSize( polyVertexCount );

    // Point?
    if ( polyVertexCount == 1 )
    {
        // Set Polygon Point.
        mPolygonBasisList[0].set(0.0f, 0.0f);
    }
    // Special-Case Quad?
    else if ( polyVertexCount == 4 )
    {
        // Yes, so set Quad.
        mPolygonBasisList[0].set(-1.0f, -1.0f);
        mPolygonBasisList[1].set(+1.0f, -1.0f);
        mPolygonBasisList[2].set(+1.0f, +1.0f);
        mPolygonBasisList[3].set(-1.0f, +1.0f);
    }
    else
    {
        // No, so calculate Regular (Primitive) Polygon Stepping.
        //
        // NOTE:-   The polygon sits on an circle that subscribes the interior
        //          of the collision box.
        F32 angle = M_PI_F / polyVertexCount;
        const F32 angleStep = M_2PI_F / polyVertexCount;

        // Calculate Polygon.
        for ( U32 n = 0; n < polyVertexCount; n++ )
        {
            // Calculate Angle.
            angle += angleStep;
            // Store Polygon Vertex.
            mPolygonBasisList[n].set(mCos(angle), mSin(angle));
        }
    }

    // Generation Local Poly.
    generateLocalPoly();
}


//-----------------------------------------------------------------------------
// Set Poly Custom.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setPolyCustom, void, 4, 4, "(poly-count, poly-Definition$) Sets Custom Polygon.\n"
			  "@return No return value.")
{
    // Set Collision Poly Custom.
    object->setPolyCustom( dAtoi(argv[2]), argv[3] );
}
// Set Collision Poly Custom.
void t2dShapeVector::setPolyCustom( const U32 polyVertexCount, const char* pCustomPolygon )
{
    // Validate Polygon.
    if ( polyVertexCount < 1 )
    {
        // Warn.
        Con::warnf("t2dShapeVector::setPolyCustom() - Vertex count must be greater than zero!");
        return;
    }

    // Fetch Custom Polygon Value Count.
    const U32 customCount = t2dSceneObject::getStringElementCount(pCustomPolygon);

    // Validate Polygon Custom Length.
    if ( customCount != polyVertexCount*2 )
    {
        // Warn.
        Con::warnf("t2dShapeVector::setPolyCustom() - Invalid Custom Polygon Items '%d'; expected '%d'!", customCount, polyVertexCount*2 );
        return;
    }
    
    // Validate Polygon Vertices.
    for ( U32 n = 0; n < customCount; n+=2 )
    {
        // Fetch Coordinate.
        const t2dVector coord = t2dSceneObject::getStringElementVector(pCustomPolygon, n);
        // Check Range.
        if ( coord.mX < -1.0f || coord.mX > 1.0f || coord.mY < -1.0f || coord.mY > 1.0f )
        {
            // Warn.
            Con::warnf("t2dShapeVector::setPolyCustom() - Invalid Polygon Coordinate range; Must be -1 to +1! '(%f,%f)'", coord.mX, coord.mY );
            return;
        }
    }

    // Clear Polygon Basis List.
    mPolygonBasisList.clear();
    mPolygonBasisList.setSize( polyVertexCount );

    // Validate Polygon Vertices.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        // Fetch Coordinate.
        const F32 x = dAtof(t2dSceneObject::getStringElement(pCustomPolygon, n*2));
        const F32 y = dAtof(t2dSceneObject::getStringElement(pCustomPolygon, n*2+1));

        // Store Polygon Vertex.
        mPolygonBasisList[n].set(x, y);
    }

    // Generation Local Poly.
    generateLocalPoly();
}

//-----------------------------------------------------------------------------
// Get Poly.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, getPoly, const char*, 2, 2, "() Gets Polygon.\n"
                                                          "@return (poly-Definition) The vertices of the polygon in object space.")
{
   // Get Collision Poly Count.
	
   return object->getPoly();
}
// Get Collision Poly.
const char* t2dShapeVector::getPoly( void )
{
    // Get Collision Polygon.
	const t2dVector* pPoly = (getPolyVertexCount() > 0) ? getPolyBasis() : NULL;

    // Set Max Buffer Size.
    const U32 maxBufferSize = getPolyVertexCount() * 18 + 1;

    // Get Return Buffer.
    char* pReturnBuffer = Con::getReturnBuffer( maxBufferSize );

    // Check Buffer.
    if( !pReturnBuffer )
    {
        // Warn.
        Con::printf("t2dShapeVector::getPoly() - Unable to allocate buffer!");
        // Exit.
        return NULL;
    }

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Polygon Edges.
    for ( U32 n = 0; n < getPolyVertexCount(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pReturnBuffer + bufferCount, maxBufferSize-bufferCount, "%0.5f %0.5f ", pPoly[n].mX, pPoly[n].mY );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dShapeVector::getPoly() - Error writing to buffer!");
            break;
        }
    }

    // Return Buffer.
    return pReturnBuffer;
}



//-----------------------------------------------------------------------------
// Generate Local Polygon.
//-----------------------------------------------------------------------------
void t2dShapeVector::generateLocalPoly( void )
{
    // Fetch Polygon Vertex Count.
    const U32 polyVertexCount = mPolygonBasisList.size();

    // Process Collision Polygon (if we've got one).
    if ( polyVertexCount > 0 )
    {
        // Clear Polygon List.
        mPolygonLocalList.clear();
        mPolygonLocalList.setSize( polyVertexCount );

        // Fetch Half Size.
        const t2dVector halfSize = getHalfSize();

        // Calculate Polygon Half-Size.
        const t2dVector polyHalfSize( halfSize.mX * mPolygonScale.mX, halfSize.mY * mPolygonScale.mY );

        // Scale/Orientate Polygon.
        for ( U32 n = 0; n < polyVertexCount; n++ )
        {
            // Fetch Polygon Basis.
            t2dVector polyVertex = mPolygonBasisList[n];
            // Scale.
			polyVertex.set( polyVertex.mX * polyHalfSize.mX * (mFlipX ? -1.0f : 1.0f), 
				            polyVertex.mY * polyHalfSize.mY * (mFlipY ? -1.0f : 1.0f));
            // Set Vertex.
            mPolygonLocalList[n] = polyVertex;
        }
    }
}


//-----------------------------------------------------------------------------
// Set Line Colour.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setLineColour, void, 3, 3, "(R / G / B / [A]) Sets the Rendering Line Colour (identical to setLineColor).\n"
			  "@param R/G/B/[A] Color values (0.0f - 1.0f) formatted as (\"Red Green Blue [Alpha]\"). Alpha is optional.\n"
			  "@return No return value.")
{
    // Set Line Colour.
    object->setLineColourString( argv[2] );
}
// Alias.
ConsoleMethod(t2dShapeVector, setLineColor, void, 3, 3, "(R / G / B / [A]) - Sets the Rendering Line Color(identical to setLineColour).\n"
			  "@param R/G/B/[A] Color values (0.0f - 1.0f) formatted as (\"Red Green Blue [Alpha]\"). Alpha is optional.\n"
			  "@return No return value.")
{
    // Set Line Colour.
    object->setLineColourString( argv[2] );
}
// Set Line Colour String.
void t2dShapeVector::setLineColourString( const char* lineColour )
{
    // Calculate Element Count.
    const U32 elementCount = t2dSceneObject::getStringElementCount( lineColour );

    // Check we've got enough arguments.
    if ( elementCount < 3 )
    {
        Con::warnf("t2dShapeVector::setLineColourString() - Invalid Number of Elements! (%s)", lineColour);
        return;
    }

    // Calculate Red, Green and Blue.
    const F32 red   = dAtof(t2dSceneObject::getStringElement( lineColour, 0 ));
    const F32 green = dAtof(t2dSceneObject::getStringElement( lineColour, 1 ));
    const F32 blue  = dAtof(t2dSceneObject::getStringElement( lineColour, 2 ));

    // Set Alpha (if specified).
    F32 alpha;
    if ( elementCount >= 4 )
        alpha = dAtof(t2dSceneObject::getStringElement( lineColour, 3 ));
    else alpha = 1.0f;

    // Set Line Colour.
    setLineColour( ColorF(red, green, blue, alpha) );
}
// Set Line Colour.
void t2dShapeVector::setLineColour( const ColorF& lineColour )
{
    // Set Line Colour.
    mLineColour = lineColour;
}

// Get Line Color
ConsoleMethod(t2dShapeVector, getLineColor, const char*, 2, 2, "() Gets the Rendering Line Color.\n"
			  "@return Returns the fill color as a string formatted with \"Red Green Blue Alpha\"")
{
	return object->getLineColor();
}
const char* t2dShapeVector::getLineColor()
{
	// Get Return Buffer.
    char* pReturnBuffer = Con::getReturnBuffer( 64 );
	dSprintf( pReturnBuffer, 64, "%0.5f %0.5f %0.5f %0.5f", mLineColour.red, mLineColour.green,
		                                                    mLineColour.blue, mLineColour.alpha);
	return pReturnBuffer;
}

//-----------------------------------------------------------------------------
// Set Line Alpha.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setLineAlpha, void, 3, 3, "(alpha) Sets the Rendering Line Alpha (transparency).\n"
			  "@param alpha The alpha value.\n"
			  "@return No return value.")
{
    // Set Line Alpha.
    object->setLineAlpha( dAtof(argv[2]) );
}
// Set Line Alpha.
void t2dShapeVector::setLineAlpha( const F32 alpha )
{
    // Set Line Alpha.
    mLineColour.alpha = alpha;
}


//-----------------------------------------------------------------------------
// Set Fill Colour.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setFillColour, void, 3, 3, "(R / G / B / [A]) Sets the Rendering Fill Colour (identical to setFillColor).\n"
			  "@param R/G/B/[A] Color values (0.0f - 1.0f) formatted as (\"Red Green Blue [Alpha]\"). Alpha is optional.\n"
			  "@return No return value."
			  )
{
    // Set Fill Colour.
    object->setLineColourString( argv[2] );
}
// Alias.
ConsoleMethod(t2dShapeVector, setFillColor, void, 3, 3, "(R / G / B / [A]) - Sets the Rendering Fill Color (identical to setFillColour).\n"
			  "@param R/G/B/[A] Color values (0.0f - 1.0f) formatted as (\"Red Green Blue [Alpha]\"). Alpha is optional.\n"
			  "@return No return value.")
{
    // Set Fill Colour.
    object->setFillColourString( argv[2] );
}
// Set Fill Colour String.
void t2dShapeVector::setFillColourString( const char* fillColour )
{
    // Calculate Element Count.
    const U32 elementCount = t2dSceneObject::getStringElementCount( fillColour );

    // Check we've got enough arguments.
    if ( elementCount < 3 )
    {
        Con::warnf("t2dShapeVector::setFillColourString() - Invalid Number of Elements! (%s)", fillColour);
        return;
    }

    // Calculate Red, Green and Blue.
    const F32 red   = dAtof(t2dSceneObject::getStringElement( fillColour, 0 ));
    const F32 green = dAtof(t2dSceneObject::getStringElement( fillColour, 1 ));
    const F32 blue  = dAtof(t2dSceneObject::getStringElement( fillColour, 2 ));

    // Set Alpha (if specified).
    F32 alpha;
    if ( elementCount >= 4 )
        alpha = dAtof(t2dSceneObject::getStringElement( fillColour, 3 ));
    else alpha = 1.0f;

    // Set Fill Colour.
    setFillColour( ColorF(red, green, blue, alpha) );
}
// Set Fill Colour.
void t2dShapeVector::setFillColour( const ColorF& fillColour )
{
    // Set Fill Colour.
    mFillColour = fillColour;
}

// Get Fill Color
ConsoleMethod(t2dShapeVector, getFillColor, const char*, 2, 2, "() Gets the Rendering Fill Color.\n"
			  "@return Returns the fill color as a string formatted with \"Red Green Blue Alpha\"")
{
	return object->getFillColor();
}
const char* t2dShapeVector::getFillColor()
{
	// Get Return Buffer.
    char* pReturnBuffer = Con::getReturnBuffer( 64 );
	dSprintf( pReturnBuffer, 64, "%0.5f %0.5f %0.5f %0.5f", mFillColour.red, mFillColour.green,
		                                                    mFillColour.blue, mFillColour.alpha);
	return pReturnBuffer;
}


//-----------------------------------------------------------------------------
// Set Fill Alpha.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setFillAlpha, void, 3, 3, "(alpha) Sets the Rendering Fill Alpha (transparency).\n"
			  "@param alpha The alpha value.\n"
			  "@return No return value.")
{
    // Set Fill Alpha.
    object->setFillAlpha( dAtof(argv[2]) );
}
// Set Fill Alpha.
void t2dShapeVector::setFillAlpha( const F32 alpha )
{
    // Set Fill Alpha.
    mFillColour.alpha = alpha;
}


//-----------------------------------------------------------------------------
// Set Fill Mode.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dShapeVector, setFillMode, void, 3, 3, "(fillMode?) Sets the Rendering Fill Mode.\n"
			  "@return No return value.")
{
    // Set Fill Mode.
    object->setFillMode( dAtob(argv[2]) );
}
// Set Fill Mode.
void t2dShapeVector::setFillMode( const bool fillMode )
{
    // Set Fill Mode.
    mFillMode = fillMode;
}
// Get Fill Mode.
ConsoleMethod(t2dShapeVector, getFillMode, bool, 2, 2, "() Gets the Rendering Fill Mode.\n"
			  "@return The fill mode as a boolean value.")
{
	return object->getFillMode();
}
bool t2dShapeVector::getFillMode()
{
	return mFillMode;
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dShapeVector::renderObject( const RectF& viewPort, const RectF& viewIntersection )
{
    // Fetch Vertex Count.
    const U32 vertexCount = mPolygonLocalList.size();

    // Have we got any vertices?
    if ( vertexCount == 0 )
    {
        // No, so finish here.
        return;
    }

    // Disable Texturing.
    glDisable       ( GL_TEXTURE_2D );

    // Save Modelview.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Fetch Position/Rotation.
    const t2dVector position = getRenderPosition();

    // Move into Vector-Space.
    glTranslatef( position.mX, position.mY, 0.0f );
    glRotatef( getRenderRotation(), 0.0f, 0.0f, 1.0f );

    // Set Blend Options.
    setBlendOptions();
	
#ifdef TORQUE_OS_IPHONE
    // Fill Mode?
    if ( mFillMode )
    {
        // Yes, so set polygon mode to FILL.
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Set Fill Colour.
        glColor4f( (GLfloat)mFillColour.red, (GLfloat)mFillColour.green, (GLfloat)mFillColour.blue, (GLfloat)mFillColour.alpha );

		GLfloat vert1[] = {//get first vert and make triangles based off of this one
			(GLfloat)(mPolygonLocalList[0].mX),
			(GLfloat)(mPolygonLocalList[0].mY),
		};
		GLfloat prevVert[] = {
			(GLfloat)(mPolygonLocalList[1].mX),
			(GLfloat)(mPolygonLocalList[1].mY),
		};
		
		
        // Draw Object.
            for ( U32 n = 2; n < vertexCount; n++ )
            {
                //glVertex2fv ( (GLfloat*)&(mPolygonLocalList[n]) );
				GLfloat vertex[] = {
									vert1[0], vert1[1],
									(GLfloat)(mPolygonLocalList[n].mX), (GLfloat)(mPolygonLocalList[n].mY),
									prevVert[0], prevVert[1],
				};
				
				glVertexPointer(2, GL_FLOAT, 0, vertex );
				glDrawArrays(GL_TRIANGLES, 0, 3 );
				prevVert[0] = (GLfloat)(mPolygonLocalList[n].mX);//save the current one's for nxt time
				prevVert[1] = (GLfloat)(mPolygonLocalList[n].mY);
            }
		//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        //glEnd();
 
    }

    // Set Line Colour.
	glColor4f(mLineColour.red, mLineColour.green, mLineColour.blue, mLineColour.alpha );
	
        for ( U32 n = 1; n <= vertexCount; n++ )
        {
			GLfloat verts[] = {
				(GLfloat)(mPolygonLocalList[n - 1].mX),
				(GLfloat)(mPolygonLocalList[n - 1].mY),
				(GLfloat)(mPolygonLocalList[n == vertexCount ? 0 : n].mX),
				(GLfloat)(mPolygonLocalList[n == vertexCount ? 0 : n].mY),
			};

			glVertexPointer(2, GL_FLOAT, 0, verts );			
			glDrawArrays(GL_LINE_LOOP, 0, 2);//draw last two
        }

#else
    // Fill Mode?
    if ( mFillMode )
    {
        // Yes, so set polygon mode to FILL.
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Set Fill Colour.
        glColor4fv( (GLfloat*)&mFillColour );

        // Draw Object.
        glBegin( GL_POLYGON );
            for ( U32 n = 0; n < vertexCount; n++ )
            {
                glVertex2fv ( (GLfloat*)&(mPolygonLocalList[n]) );
            }
        glEnd();
    }

    // Set Line Colour.
    glColor4fv( (GLfloat*)&mLineColour );

    // Draw Object.
    glBegin(GL_LINES);
        for ( U32 n = 1; n <= vertexCount; n++ )
        {
            glVertex2fv ( (GLfloat*)&(mPolygonLocalList[n - 1]) );
            glVertex2fv ( (GLfloat*)&(mPolygonLocalList[n == vertexCount ? 0 : n]) );
        }
    glEnd();

#endif
    // Restore Colour.
    glColor4f( 1,1,1,1 );

    // Restore Matrix.
    glPopMatrix();

	// Call Parent.
	Parent::renderObject( viewPort, viewIntersection );	// Always use for Debug Support!
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dShapeVector )
	REGISTER_SERIALISE_VERSION( t2dShapeVector, 1, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dShapeVector, 1 )




//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dShapeVector, 1 )
{
    U32                 polyVertexCount;
    t2dVector           polygonScale;
    ColorF              lineColour;
    ColorF              fillColour;
    bool                fillMode;

    // Read Vertex Count.
    if (    !stream.read( &polyVertexCount ) ||
            !stream.read( &polygonScale.mX ) ||
            !stream.read( &polygonScale.mY ) ||
            !stream.read( &fillMode ) ||
            !stream.read( &fillColour) ||
            !stream.read( &lineColour) )
        return false;

    // Clear Polygon Basis List.
    object->mPolygonBasisList.clear();
    object->mPolygonBasisList.setSize( polyVertexCount );

    // Read Collision Polygon.
    for ( U32 n = 0; n < polyVertexCount; n++ )
    {
        // Fetch Reference Object.
        t2dVector& polyVertex = object->mPolygonBasisList[n];

        if (    !stream.read( &(polyVertex.mX) ) ||
                !stream.read( &(polyVertex.mY) ) )
            // Error.
            return false;
    }

    // Generate Local Polygon.
    object->generateLocalPoly();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dShapeVector, 1 )
{
    // Write Vertex Count.
    if (    !stream.write( object->getPolyVertexCount() ) ||
            !stream.write( object->mPolygonScale.mX ) ||
            !stream.write( object->mPolygonScale.mY ) ||
            !stream.write( object->mFillMode ) ||
            !stream.write( object->mFillColour) ||
            !stream.write( object->mLineColour) )
        return false;



    ColorF                  mLineColour;
    ColorF                  mFillColour;
    // UNUSED: JOSEPH THOMAS -> bool                    mFillMode;
    t2dVector               mPolygonScale;          ///< Polygon Scale.


    // Write Polygon.
    for ( U32 n = 0; n < object->getPolyVertexCount(); n++ )
    {
        if (    !stream.write( object->mPolygonBasisList[n].mX ) ||
                !stream.write( object->mPolygonBasisList[n].mY ) )
            // Error.
            return false;
    }
    
    // Return Okay.
	return true;
}

