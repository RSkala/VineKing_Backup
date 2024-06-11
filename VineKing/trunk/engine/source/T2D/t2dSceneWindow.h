//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Graph Window.
//-----------------------------------------------------------------------------

#ifndef _T2DSCENEWINDOW_H_
#define _T2DSCENEWINDOW_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _T2DSCENEGRAPH_H_
#include "./t2dSceneGraph.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _T2DVECTOR_H_
#include "t2dVector.h"
#endif



///-----------------------------------------------------------------------------
/// Scene Window 2D.
///-----------------------------------------------------------------------------
class t2dSceneWindow : public GuiControl, public virtual ITickable
{
    typedef GuiControl Parent;

public:
    /// Camera View.
    struct tCameraView
    {
        /// Targets.
        RectF           mCameraWindow;
        F32             mCameraZoom;

        /// Factors.
        RectF           mZoomedWindow;
        Point2F         mSceneMin;
        Point2F         mSceneMax;
        Point2F         mSceneWindowScale;
        
        tCameraView()
        {
           mCameraWindow = RectF(0.0f, 0.0f, 10.0f, 10.0f);
           mCameraZoom = 1.0f;
           mZoomedWindow = RectF(0.0f, 0.0f, 10.0f, 10.0f);
           mSceneMin = Point2F(0.0f, 0.0f);
           mSceneMax = Point2F(10.0f, 10.0f);
           mSceneWindowScale = Point2F(1.0f, 1.0f);
        }
    };

private:
    /// Cameras.
    tCameraView mCameraCurrent, mCameraSource, mCameraTarget;

    // Camera Interpolation.
    Vector<tCameraView> mCameraQueue;
    S32                 mMaxQueueItems;
    F32                 mCameraTransitionTime;
    F32                 mPreCameraTime;
    F32                 mPostCameraTime;
    F32                 mRenderCameraTime;
    F32                 mCurrentCameraTime;
    bool                mMovingCamera;

    /// Tick Properties.
    Point2F             mPreTickPosition;
    Point2F             mPostTickPosition;

    /// Camera Attachment.
    bool                mCameraMounted;
    t2dSceneObject*     mpMountedTo;
    Point2F             mMountOffset;
    U32                 mMountToID;
    F32                 mMountForce;

    /// View Limit.
    bool                mViewLimitActive;
    t2dVector           mViewLimitMin;
    t2dVector           mViewLimitMax;
    t2dVector           mViewLimitArea;

    /// Camera Shaking.
    bool                mCameraShaking;
    F32                 mShakeLife;
    F32                 mCurrentShake;
    F32                 mShakeRamp;
    t2dVector           mCameraShakeOffset;

    /// Misc.
    t2dSceneGraph*      mpSceneGraph2D;
    S32                 mLastRenderTime;
    bool                mLockMouse;
    bool                mWindowDirty;

    // Mouse Events.
    bool                mUseWindowMouseEvents;
    bool                mUseObjectMouseEvents;
    U32                 mMouseEventGroupMaskFilter;
    U32                 mMouseEventLayerMaskFilter;
    bool                mMouseEventInvisibleFilter;
    SimSet mLastPickVector;

    // Render Masks.
    U32                 mRenderLayerMask;
    U32                 mRenderGroupMask;
	t2dSceneObject*		mpDontRenderObject;

    Resource<GFont>     mpDebugFont;
    char                mDebugText[256];
    ColorF              mDebugTextColour;
    ColorF              mDebugBannerColour;


    /// Default Font.
    void setDefaultFont( void );

    /// Handling Mouse Events.
    void dispatchMouseEvent( const char* name, const GuiEvent& event );
    void sendWindowMouseEvent( const char* name, const GuiEvent& event );
    void sendObjectMouseEvent( const char* name, const GuiEvent& event );

    inline void calculateCameraView( tCameraView* pCameraView );


public:

    /// Camera Interpolation Mode.
    enum eCameraInterpolationMode
    {
        LINEAR,             ///< Standard Linear.
        SIGMOID             ///< Slow Start / Slow Stop.

    } mCameraInterpolationMode;

    // Screen Window.
    struct tScreenWindow
    {
        Point2I offset;
        RectI updateRect;

    } mScreenWindow;


    t2dSceneWindow();
    virtual ~t2dSceneWindow();

    virtual bool onAdd();
    virtual void onRemove();

    /// Initialisation.
    virtual void setSceneGraph( t2dSceneGraph* pT2DSceneGraph );
    virtual void resetSceneGraph( void );
    void setRenderGroups( U32 groupMask);
    void setRenderLayers( U32 layerMask);
    void setRenderMasks( U32 layerMask, U32 groupMask );
	t2dSceneObject* getDontRenderObject( void );
	void setDontRenderObject( t2dSceneObject* );
    void setDebugBanner( const char* pFontname, const U32 fontSize, const ColorF& debugTextColour, const ColorF& debugBannerColour );

    S32 getRenderLayerMask();
    S32 getRenderGroupMask();

    /// Get SceneGraph.
    inline t2dSceneGraph* getSceneGraph( void ) const { return mpSceneGraph2D; };

    /// Screen Window.
    void setScreenWindow( const Point2I& offset, const RectI& updateRect ) { mScreenWindow.offset = offset; mScreenWindow.updateRect = updateRect; };
    const tScreenWindow& getScreenWindow( void ) const { return mScreenWindow; };

    /// Mouse.
    void setLockMouse( bool lockStatus ) { mLockMouse = lockStatus; };
    bool getLockMouse( void ) { return mLockMouse; };
    void setObjectMouseEventInvisibleFilter(bool useInvisible);
    void setObjectMouseEventGroupFilter(U32 groupMask);
    void setObjectMouseEventLayerFilter(U32 layerMask);
    void setObjectMouseEventFilter( U32 groupMask, U32 layerMask, bool useInvisible = false );
    void setUseWindowMouseEvents( bool windowMouseStatus ) { mUseWindowMouseEvents = windowMouseStatus; };
    void setUseObjectMouseEvents( bool objectMouseStatus ) { mUseObjectMouseEvents = objectMouseStatus; };
    bool getUseWindowMouseEvents( void ) const { return mUseWindowMouseEvents; };
    bool getUseObjectMouseEvents( void ) const { return mUseObjectMouseEvents; };
    t2dVector getMousePosition( void );
    void setMousePosition( const t2dVector& mousePosition );
	void removeFromLastPicker(t2dSceneObject *psObject);

    /// Coordinate Conversion.
    void windowToSceneCoord( const t2dVector& srcPoint, t2dVector& dstPoint ) const;
    void sceneToWindowCoord( const t2dVector& srcPoint, t2dVector& dstPoint ) const;

    /// Mounting.
    void mount( t2dSceneObject* pSceneObject2D, t2dVector mountOffset, F32 mountForce, bool sendToMount );
    void dismount( void );
    void dismountMe( t2dSceneObject* pSceneObject2D );
    void calculateCameraMount( const F32 elapsedTime );
    void interpolateCameraMount( const F32 timeDelta );

    /// View Limit.
    void setViewLimitOn( const t2dVector& limitMin, const t2dVector& limitMax );
    void setViewLimitOff( void ) { mViewLimitActive = false; };

    /// Tick Processing.
    void zeroCameraTime( void );
    void resetTickCameraTime( void );
    void updateTickCameraTime( void );
    void resetTickCameraPosition( void );

    virtual void interpolateTick( F32 delta );
    virtual void processTick();
    virtual void advanceTime( F32 timeDelta ) {};

    /// Current Camera,
    virtual void setCurrentCameraArea( const RectF& cameraWindow );
    virtual void setCurrentCameraPosition( t2dVector centerPosition, F32 width, F32 height );
    void setCurrentCameraZoom( F32 zoomFactor );

    /// Target Camera.
    virtual void setTargetCameraArea( const RectF& cameraWindow );
    virtual void setTargetCameraPosition( t2dVector centerPosition, F32 width, F32 height );
    void setTargetCameraZoom( F32 zoomFactor );

    /// Camera Interpolation Time/Mode.
    void setCameraInterpolationTime( F32 interpolationTime );
    void setCameraInterpolationMode( eCameraInterpolationMode interpolationMode );

    /// Camera Movement.
    void startCameraMove( F32 interpolationTime );
    void stopCameraMove( void );
    void completeCameraMove( void );
    void undoCameraMove( F32 interpolationTime );
    F32 interpolate( F32 from, F32 to, F32 delta );
    F32 linearInterpolate( F32 from, F32 to, F32 delta );
    F32 sigmoidInterpolate( F32 from, F32 to, F32 delta );
    void updateCamera( void );

    /// Camera Accessors.
    const t2dVector getCurrentCameraPosition( void ) const;
    const RectF getCurrentCameraArea( void ) const              { return mCameraCurrent.mCameraWindow; };
    F32 getCameraInterpolationTime( void )                      { return mCameraTransitionTime; };
    F32 getCurrentCameraWidth( void ) const                     { return mCameraCurrent.mCameraWindow.len_x(); };
    F32 getCurrentCameraHeight( void ) const                    { return mCameraCurrent.mCameraWindow.len_y(); };
    F32 getCurrentCameraZoom( void ) const                      { return mCameraCurrent.mCameraZoom; };
    const t2dVector getCurrentCameraWindowScale( void ) const   { return mCameraCurrent.mSceneWindowScale; }
    const tCameraView& getCurrentCamera(void) const             { return mCameraCurrent; };
    const t2dVector& getCameraShake(void) const                 { return mCameraShakeOffset; };
    bool isCameraMounted( void ) const                          { return mCameraMounted; };
    bool isCameraMoving( void ) const                           { return mMovingCamera; };

    /// Camera Shake.
    void startCameraShake( F32 magnitude, F32 time );
    void stopCameraShake( void );

    static void initPersistFields();

    /// GuiControl
    virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
    void onMouseDown( const GuiEvent& event );
    void onMouseUp( const GuiEvent& event );
    void onMouseMove( const GuiEvent& event );
    void onMouseDragged( const GuiEvent& event );
    void onMouseEnter( const GuiEvent& event );
    void onMouseLeave( const GuiEvent& event );
    void onRightMouseDown( const GuiEvent& event );
    void onRightMouseUp( const GuiEvent& event );
    void onRightMouseDragged( const GuiEvent& event );
    bool onMouseWheelDown( const GuiEvent &event );
    bool onMouseWheelUp( const GuiEvent &event );
    void onRender( Point2I offset, const RectI& updateRect );

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dSceneWindow);
};

#endif // _T2DSCENEWINDOW_H_
