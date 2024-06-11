//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Gun Turret Object.
//-----------------------------------------------------------------------------


#include "./t2dGunTurret.h"


//------------------------------------------------------------------------------


IMPLEMENT_CONOBJECT(t2dGunTurret);


//-----------------------------------------------------------------------------
//
//  t2dGunTurret.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dGunTurret::t2dGunTurret() :      T2D_Stream_HeaderID(makeFourCCTag('2','D','A','T')),
                                    mImageMapDataBlock(NULL),
                                    mFrame(0),
                                    mpSceneObject2D(NULL)

{
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dGunTurret::~t2dGunTurret()
{
}


//-----------------------------------------------------------------------------
// OnAdd.
//-----------------------------------------------------------------------------
bool t2dGunTurret::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // No Default Parameters for now !!!!!
    return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dGunTurret::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}


//-----------------------------------------------------------------------------
// Set ImageMap.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dGunTurret, setImageMap, bool, 3, 4, "(imageMapName, [int frame]) - Sets imageMap/Frame.")
{
    // Calculate Frame.
    U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;

    // Set ImageMap.
    return object->setImageMap( argv[2], frame );
}   
// Set ImageMap/Frame.
bool t2dGunTurret::setImageMap( const char* imageMapName, U32 frame )
{
    // Invalid ImageMap Name.
    if ( !imageMapName || imageMapName == StringTable->insert("") )
        return false;

    // Find ImageMap Datablock.
    t2dImageMapDatablock* pImageMapDataBlock = dynamic_cast<t2dImageMapDatablock*>(Sim::findObject( imageMapName ));

    // Set Datablock.
    if ( !t2dCheckDatablock( pImageMapDataBlock ) )
    {
        // Warn.
        Con::warnf("t2dGunTurret::setImageMap() - t2dImageMapDatablock Datablock is invalid! (%s)", imageMapName);
        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= pImageMapDataBlock->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dGunTurret::setImageMap() - Invalid Frame #%d for t2dImageMapDatablock Datablock! (%s)", frame, imageMapName);
        // Return Here.
        return false;
    }

    // Set ImageMap Datablock.
    mImageMapDataBlock = pImageMapDataBlock;

    // Set Frame.
    mFrame = frame;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Set ImageMap Frame.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dGunTurret, setFrame, bool, 3, 3, "(frame) - Sets imageMap frame.")
{
    // Set ImageMap Frame.
    return object->setFrame( dAtoi(argv[2]) );
}   
// Set ImageMap/Frame.
bool t2dGunTurret::setFrame( U32 frame )
{
    // Check Existing ImageMap.
    if ( !mImageMapDataBlock )
    {
        // Warn.
        Con::warnf("t2dGunTurret::setFrame() - Cannot set Frame without existing t2dImageMapDatablock Datablock!");
        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= mImageMapDataBlock->getImageMapFrameCount() )
    {
        // Warn.
        Con::warnf("t2dGunTurret::setFrame() - Invalid Frame #%d for t2dImageMapDatablock Datablock! (%s)", frame, getIdString());
        // Return Here.
        return false;
    }

    // Set Frame.
    mFrame = frame;

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Set Track Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dGunTurret, setTrackObject, bool, 3, 3, "(t2dSceneObject) - Sets Object to Track.")
{
    // Find Track Object.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject( argv[2] ));

    // Check Scene Object.
    if ( !pSceneObject2D )
    {
        // Warn.
        Con::warnf("t2dGunTurret::setTrackObject() - Track Object is invalid! (%s)", argv[2]);
        // Return Here.
        return false;
    }

    // Set Track Object.
    return object->setTrackObject( pSceneObject2D );
}   
// Set Track Object.
bool t2dGunTurret::setTrackObject( t2dSceneObject* pSceneObject2D )
{
    // Setup Reference to Scene Object.
    mpSceneObject2D = pSceneObject2D;

    // Register Scene Object Reference.
    pSceneObject2D->registerReference( (SimObject**)&mpSceneObject2D );

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Reset Tile.
// NOTE:-   This is called by T2D when a tile-layer is reset.
//          "t2dTileLayer::resetLayer()"
//-----------------------------------------------------------------------------
void t2dGunTurret::resetTile()
{
}


//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dGunTurret )
    REGISTER_SERIALISE_VERSION( t2dGunTurret, 1, false )
REGISTER_SERIALISE_END()

// Implement Base Serialisation.
IMPLEMENT_T2D_SERIALISE_PARENT( t2dGunTurret, 1 )


//-----------------------------------------------------------------------------
// Load v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dGunTurret, 1 )
{
    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Save v1
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dGunTurret, 1 )
{
    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Integrate Object.
//-----------------------------------------------------------------------------
void t2dGunTurret::integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats )
{
}


//-----------------------------------------------------------------------------
// Render Object.
//-----------------------------------------------------------------------------
void t2dGunTurret::renderObject( const RectF& viewPort, const RectF& viewIntersection, const t2dVector tileWorldClipBoundary[4] )
{
    // Cannot render without Texture.
    if ( !mImageMapDataBlock || !mpSceneObject2D )
        return;

    glEnable        ( GL_TEXTURE_2D );
    mImageMapDataBlock->bindImageMapFrame( mFrame );
    glTexEnvi       ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Fetch Current Frame Area.
    const t2dImageMapDatablock::cFrameTexelArea& frameArea = mImageMapDataBlock->getImageMapFrameArea( mFrame );
    // Fetch Positions.
    const F32& minX = frameArea.mX;
    const F32& minY = frameArea.mY;
    const F32& maxX = frameArea.mX2;
    const F32& maxY = frameArea.mY2;

    // Calculate Center Pivot-Point.
    t2dVector pivotPoint( (tileWorldClipBoundary[2].mX + tileWorldClipBoundary[0].mX)*0.5f, (tileWorldClipBoundary[2].mY + tileWorldClipBoundary[0].mY)*0.5f );

    // Fetch Target Center Point.
    const t2dVector targetPoint = mpSceneObject2D->getRenderPosition();

    // Do Vector Operation.
    F32 angle = mRadToDeg( mAtan( targetPoint.mX-pivotPoint.mX, pivotPoint.mY-targetPoint.mY ) );

    // Calculate Transform.
    t2dMatrix xForm( -angle );

    // Rotate Clip-Boundary around pivot-point.
    t2dVector renderTileWorldClipBoundary[4];
    t2dSceneObject::pivotTransformPoint2D( xForm, tileWorldClipBoundary[0], pivotPoint, renderTileWorldClipBoundary[0] );
    t2dSceneObject::pivotTransformPoint2D( xForm, tileWorldClipBoundary[1], pivotPoint, renderTileWorldClipBoundary[1] );
    t2dSceneObject::pivotTransformPoint2D( xForm, tileWorldClipBoundary[2], pivotPoint, renderTileWorldClipBoundary[2] );
    t2dSceneObject::pivotTransformPoint2D( xForm, tileWorldClipBoundary[3], pivotPoint, renderTileWorldClipBoundary[3] );
	
#ifdef TORQUE_OS_IPHONE
	//NOTE: Torque expects to use quads, but we use a triangle strip, so the last two vertices are switched
	GLfloat vertices[] = {
		(GLfloat)(renderTileWorldClipBoundary[0].mX),(GLfloat)(renderTileWorldClipBoundary[0].mY),
		(GLfloat)(renderTileWorldClipBoundary[1].mX),(GLfloat)(renderTileWorldClipBoundary[1].mY),
		(GLfloat)(renderTileWorldClipBoundary[3].mX),(GLfloat)(renderTileWorldClipBoundary[3].mY),
		(GLfloat)(renderTileWorldClipBoundary[2].mX),(GLfloat)(renderTileWorldClipBoundary[2].mY),
	};		
	GLfloat texCoord[] = {
		minX, minY,
		maxX, minY,
		minX, maxY,
		maxX, maxY,
	};
	
	//2 for 2-point coordinates
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
#else
    // Draw Object.
    glBegin(GL_QUADS);
        glTexCoord2f( minX, minY );
        glVertex2fv ( (GLfloat*)&(renderTileWorldClipBoundary[0]) );
        glTexCoord2f( maxX, minY );
        glVertex2fv ( (GLfloat*)&(renderTileWorldClipBoundary[1]) );
        glTexCoord2f( maxX, maxY );
        glVertex2fv ( (GLfloat*)&(renderTileWorldClipBoundary[2]) );
        glTexCoord2f( minX, maxY );
        glVertex2fv ( (GLfloat*)&(renderTileWorldClipBoundary[3]) );
    glEnd();
#endif
    // Disable Texturing.
    glDisable       ( GL_TEXTURE_2D );
}

