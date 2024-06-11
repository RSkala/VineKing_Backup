//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "component/dynamicConsoleMethodComponent.h"
#include "T2D/t2dSceneObject.h"

//-----------------------------------------------------------------------------

class MoveWaveComponent : public DynamicConsoleMethodComponent
{
	typedef DynamicConsoleMethodComponent Parent;

public:
	struct GroundCheck
	{
		F32 mMaxNormal;
		F32 mThreshold;
		F32 mBuffer;
	};

protected:

	//
	t2dSceneObject *mOwner;

	//
	const char *mDirectionName;
	int mDirection; // LEFT or RIGHT
	float mSpeed;
	bool mUseWaveOffset;
	int mWaveOffset;

	F32 mSinStep;
	F32 mAdjustVal;

public:

	DECLARE_CONOBJECT(MoveWaveComponent);

	MoveWaveComponent();

	bool onAdd();
	void onRemove();

	static void initPersistFields();

	//
	virtual bool onComponentAdd(SimComponent *target);
	virtual void onComponentRemove(SimComponent *target);

	//
	virtual void onAddToScene();
	virtual void onUpdate();
	void updateMove();
};