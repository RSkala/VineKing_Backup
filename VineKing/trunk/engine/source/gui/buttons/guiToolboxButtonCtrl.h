//-----------------------------------------------------------------------------
// Toolbox Button Control 
// Justin DuJardin
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUITOOLBOXBUTTON_H_
#define _GUITOOLBOXBUTTON_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif

class GuiToolboxButtonCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

protected:
   StringTableEntry mNormalBitmapName;
   StringTableEntry mLoweredBitmapName;
   StringTableEntry mHoverBitmapName;

   TextureHandle mTextureNormal;
   TextureHandle mTextureLowered;
   TextureHandle mTextureHover;

   void renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect);
   void renderStateRect( TextureHandle &texture, const RectI& rect );

public:   
   DECLARE_CONOBJECT(GuiToolboxButtonCtrl);
   GuiToolboxButtonCtrl();

   static void initPersistFields();

   //Parent methods
   bool onWake();
   void onSleep();
   void inspectPostApply();

   void setNormalBitmap( StringTableEntry bitmapName );
   void setLoweredBitmap( StringTableEntry bitmapName );
   void setHoverBitmap( StringTableEntry bitmapName );
   

   void onRender(Point2I offset, const RectI &updateRect);
};


#endif //_GUITOOLBOXBUTTON_H_
