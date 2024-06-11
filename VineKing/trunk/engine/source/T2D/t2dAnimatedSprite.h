//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Animated Sprite.
//-----------------------------------------------------------------------------

#ifndef _T2DANIMATEDSPRITE_H_
#define _T2DANIMATEDSPRITE_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif

#ifndef _T2DANIMATIONCONTROLLER_H_
#include "./t2dAnimationController.h"
#endif

///-----------------------------------------------------------------------------
/// Animated Object 2D.
///-----------------------------------------------------------------------------
class t2dAnimatedSprite : public t2dSceneObject
{
    typedef t2dSceneObject          Parent;

public:
    t2dAnimatedSprite();
    virtual ~t2dAnimatedSprite();

    static void initPersistFields();
    static bool setAnimation(void* obj, const char* data) { static_cast<t2dAnimatedSprite*>(obj)->playAnimation(data, false, -1, false); return false; };

    /// Play Animation.
    bool playAnimation( const char* animationName, const bool autoRestore, const S32 frameIndex = -1, const bool mergeTime = false );

    virtual bool onAdd();
    virtual void onRemove();
    virtual void integrateObject( const F32 sceneTime, const F32 elapsedTime, CDebugStats* pDebugStats );
    virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );

    t2dAnimationController          mAnimationController;
    bool                            mAnimationCallbackComplete;
    bool                            mFrameChangeCallback;

    virtual void copyTo(SimObject* object);

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dAnimatedSprite );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dAnimatedSprite, 4 );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dAnimatedSprite);
};

#endif // _T2DANIMATIONOBJECT2D_H_
