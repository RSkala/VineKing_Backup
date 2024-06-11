//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scroller Object.
//-----------------------------------------------------------------------------

#ifndef _T2DSCROLLER_H_
#define _T2DSCROLLER_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif

#ifndef _T2DIMAGEMAPDATABLOCK_H_
#include "./t2dImageMapDatablock.h"
#endif


///-----------------------------------------------------------------------------
/// Scroller Object 2D.
///-----------------------------------------------------------------------------
class t2dScroller : public t2dSceneObject
{
    typedef t2dSceneObject      Parent;

private:
    F32                     mRepeatX;
    F32                     mRepeatY;
    F32                     mScrollX;
    F32                     mScrollY;

    t2dImageMapSmartPtr     mImageMapDataBlock;
    StringTableEntry        mImageMapDataBlockName;
    F32                     mTextureOffsetX;
    F32                     mTextureOffsetY;

    t2dVector               mPreTickTextureOffset;
    t2dVector               mPostTickTextureOffset;
    t2dVector               mRenderTickTextureOffset;


public:

    t2dScroller();
    virtual ~t2dScroller();

    static void initPersistFields();
    static bool setImageMap(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setImageMap(data); return false; };
    static bool setRepeatX(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setRepeatX(dAtof(data)); return false; };
    static bool setRepeatY(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setRepeatY(dAtof(data)); return false; };
    static bool setScrollX(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setScrollX(dAtof(data)); return false; };
    static bool setScrollY(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setScrollY(dAtof(data)); return false; };
    static bool setScrollPositionX(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setScrollPositionX(dAtof(data)); return false; };
    static bool setScrollPositionY(void* obj, const char* data) { static_cast<t2dScroller*>(obj)->setScrollPositionY(dAtof(data)); return false; };

    void setRepeatX( F32 repeatX ) { setRepeat(repeatX, getRepeatY()); };
    void setRepeatY( F32 repeatY ) { setRepeat(getRepeatX(), repeatY); };
    void setRepeat( F32 repeatX, F32 repeatY );
    void setScrollX(F32 x) { setScroll(x, getScrollY()); };
    void setScrollY(F32 y) { setScroll(getScrollX(), y); };
    void setScroll( F32 scrollX = 0.0f, F32 scrollY = 0.0f );
    void setImageMap( const char* imageMapName );
    void setScrollPositionX( F32 scrollX ) { setScrollPosition(scrollX, getScrollPositionY()); };
    void setScrollPositionY( F32 scrollY ) { setScrollPosition(getScrollPositionX(), scrollY); };
    void setScrollPosition( F32 scrollX, F32 scrollY );

    const char* getImageMapName( void ) const   { if (mImageMapDataBlock) return mImageMapDataBlock->getName(); else return NULL; };
    F32         getRepeatX() { return mRepeatX; };
    F32         getRepeatY() { return mRepeatY; };
    F32         getScrollX() { return mScrollX; };
    F32         getScrollY() { return mScrollY; };
    F32         getScrollPositionX() { return mTextureOffsetX; };
    F32         getScrollPositionY() { return mTextureOffsetY; };

    /// Tick Processing.
    void resetTickScrollPositions( void );
    void updateTickScrollPosition( void );
    virtual void interpolateTick( const F32 timeDelta );

    virtual bool onAdd();
    virtual void onRemove();
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );

    virtual void copyTo(SimObject* object);

	//Luma: need source rects here too
	void setSourceTextCoords(void);
	bool mUseSourceRect;
	RectF mSrcTexCoords;
	RectF mSrcRect;

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dScroller );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dScroller, 4 );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dScroller);
};

#endif // _T2DSCROLLER_H_
