//-----------------------------------------------------------------------------
// Platformer Starter Kit
// Copyright (C) Phillip O'Shea
//-----------------------------------------------------------------------------

#include "component/dynamicConsoleMethodComponent.h"
#include "T2D/t2dSceneObject.h"

//-----------------------------------------------------------------------------

class FaceObjectComponent : public DynamicConsoleMethodComponent
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
	/*
	sometimes the object has not been created when we init this, so save the name and
	lookup the value the first time we need it. if we try to grab the object  at first, we
	may miss it on init and never have.
	-Mat
	*/

   const char * mObjectName;
   t2dSceneObject *mObject;
   F32 mTurnSpeed;   
   F32 mRotationOffset;

public:

	DECLARE_CONOBJECT(FaceObjectComponent);

	FaceObjectComponent();

	bool onAdd();
	void onRemove();

	static void initPersistFields();

	//
	virtual bool onComponentAdd(SimComponent *target);
	virtual void onComponentRemove(SimComponent *target);

	//
	virtual void onAddToScene();
	virtual void onUpdate();
};