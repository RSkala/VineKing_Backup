//-----------------------------------------------------------------------------
// T2D Object Renderer
//-----------------------------------------------------------------------------

#ifndef _guiT2DObjectCtrl_H_
#define _guiT2DObjectCtrl_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#include "T2D/t2dSceneObject.h"
#endif

#ifndef _T2DVECTOR_H_
#include "t2dVector.h"
#endif


/// Renders a T2D Object.
class guiT2DObjectCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

protected:
   StringTableEntry              mSceneObjectName;
   t2dVector                     mWorldClipBoundaryBackup[4];
   SimObjectPtr<t2dSceneObject>  mSelectedSceneObject;
   S32                           mMargin;
   StringTableEntry              mCaption;
   bool                          mHasTexture;
   
public:
    guiT2DObjectCtrl();
    static void initPersistFields();

    void setSceneObject( const char *name  );
    inline t2dSceneObject* getSceneObject() { return mSelectedSceneObject; };

    void setCaption( const char* caption );

    /// GuiControl
    bool onWake();
    void onSleep();
    void inspectPostApply();
    void onRender(Point2I offset, const RectI &updateRect);

    void onMouseEnter(const GuiEvent &event);
    void onMouseLeave(const GuiEvent &event);
    void onMouseUp(const GuiEvent &event);
    void onMouseDragged(const GuiEvent &event);

    /// Declare Console Object.
    DECLARE_CONOBJECT( guiT2DObjectCtrl );
};

#endif // _guiT2DObjectCtrl_H_

////EOF/////////////////////////////////////////////////////////////////////////
