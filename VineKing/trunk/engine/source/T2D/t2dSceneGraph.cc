//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Scene Graph Object.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "./t2dSceneObject.h"
#include "./t2dTileMap.h"
#include "./t2dSceneGraph.h"
#include "T2D/t2dQuadBatch.h"

#include "component/ComponentGlobals.h"		// RKS: I added this
#include "component/ComponentDoubleBrick.h" // RKS: I added this
#include "component/ComponentLandBrick.h"	// RKS: I added this
#include "component/SoundManager.h"			// RKS: I added this

// T2D Debug Profiling.
#ifdef TORQUE_ENABLE_PROFILER
#include "platform/profiler.h"
#endif

//------------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(t2dSceneGraphDatablock);
IMPLEMENT_CONOBJECT(t2dSceneGraph);


//------------------------------------------------------------------------------

// Container System Defaults.
static const F32 scmContainerBinSize    = 16.0f;
static const U32 scmContainerBinCount   = 32;//-Mat was 256;

// Layer Sorting Defaults.
static const U32 scmUseLayerSorting     = true;
static const U32 scmLastInFrontSorting  = true;


//-----------------------------------------------------------------------------
// Find Objects Callback.
//-----------------------------------------------------------------------------
void findObjectsCallback(t2dSceneObject* pSceneObject, void* storage)
{  
    // Cast Callback List.
    Vector<t2dSceneObject*>* callbackList = (Vector<t2dSceneObject*>*)storage;
    // Add Object to List.
    callbackList->push_back(pSceneObject);
}


//-----------------------------------------------------------------------------
// Find Layered Objects Callback.
//-----------------------------------------------------------------------------
void findLayeredObjectsCallback(t2dSceneObject* pSceneObject, void* storage)
{  
    // Cast Callback List.
    Vector<t2dSceneObject*>* callbackList = (Vector<t2dSceneObject*>*)storage;
    // Add Object to List.
    callbackList[pSceneObject->getLayer()].push_back(pSceneObject);
}


//-----------------------------------------------------------------------------
// Draw-Order Command Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums drawOrderCommandLookup[] =
                {
                { t2dSceneGraph::T2D_ORDER_FRONT,       "FRONT" },
                { t2dSceneGraph::T2D_ORDER_BACK,        "BACK" },
                { t2dSceneGraph::T2D_ORDER_FORWARD,     "FORWARD" },
                { t2dSceneGraph::T2D_ORDER_BACKWARD,    "BACKWARD" },
                };

//-----------------------------------------------------------------------------
// Draw-Order Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dSceneGraph::eDrawOrderCommand getDrawOrderCommand(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(drawOrderCommandLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(drawOrderCommandLookup[i].label, label) == 0)
            return((t2dSceneGraph::eDrawOrderCommand)drawOrderCommandLookup[i].index);

    // Invalid Alignment!
    AssertFatal(false, "t2dSceneGraph::getDrawOrderCommand() - Invalid Draw-Order Command!");
    // Bah!
    return t2dSceneGraph::T2D_ORDER_INVALID;
}


//-----------------------------------------------------------------------------
// Draw-Order Sort Table.
//-----------------------------------------------------------------------------
static EnumTable::Enums drawOrderSortLookup[] =
                {
                { t2dSceneGraph::T2D_SORT_NORMAL,       "Normal" },
                { t2dSceneGraph::T2D_SORT_YAXIS,        "Y Axis" },
                { t2dSceneGraph::T2D_SORT_XAXIS,        "X Axis" },
                { t2dSceneGraph::T2D_SORT_NEGYAXIS,        "-Y Axis" },
                { t2dSceneGraph::T2D_SORT_NEGXAXIS,        "-X Axis" },
                };

static EnumTable drawOrderSortTable(sizeof(drawOrderSortLookup) /  sizeof(EnumTable::Enums), &drawOrderSortLookup[0]);

//-----------------------------------------------------------------------------
// Draw-Order Sort Script-Enumerator.
//-----------------------------------------------------------------------------
static t2dSceneGraph::eDrawOrderSort getDrawOrderSort(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(drawOrderSortLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(drawOrderSortLookup[i].label, label) == 0)
            return((t2dSceneGraph::eDrawOrderSort)drawOrderSortLookup[i].index);

    // Invalid Alignment!
    AssertFatal(false, "t2dSceneGraph::getDrawOrderSort() - Invalid Draw-Order Sort!");
    // Bah!
    return t2dSceneGraph::T2D_SORT_NORMAL;
}

//------------------------------------------------------------------------------

t2dSceneGraphDatablock::t2dSceneGraphDatablock() :  mContainerBinSize(scmContainerBinSize),
                                                    mContainerBinCount(scmContainerBinCount),
                                                    mUseLayerSorting(scmUseLayerSorting),
                                                    mLastInFrontSorting(scmLastInFrontSorting)
{
}

//------------------------------------------------------------------------------

t2dSceneGraphDatablock::~t2dSceneGraphDatablock()
{
}

//------------------------------------------------------------------------------

void t2dSceneGraphDatablock::initPersistFields()
{
    Parent::initPersistFields();

    // Fields.
    addField("containerBinSize",            TypeF32,    Offset(mContainerBinSize,           t2dSceneGraphDatablock));
    addField("containerBinCount",           TypeS32,    Offset(mContainerBinCount,          t2dSceneGraphDatablock));
    addField("useLayerSorting",             TypeBool,   Offset(mUseLayerSorting,            t2dSceneGraphDatablock));
    addField("lastInFrontSorting",          TypeBool,   Offset(mLastInFrontSorting,         t2dSceneGraphDatablock));
}

//------------------------------------------------------------------------------

bool t2dSceneGraphDatablock::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void t2dSceneGraphDatablock::packData(BitStream* stream)
{
    // Parent packing.
    Parent::packData(stream);

    // Write Datablock.
    //stream->write( mContainerBinSize );
    //stream->write( mContainerBinCount );
    //stream->write( mUseLayerSorting );
    //stream->write( mLastInFrontSorting );
}

//------------------------------------------------------------------------------

void t2dSceneGraphDatablock::unpackData(BitStream* stream)
{
    // Parent unpacking.
    Parent::unpackData(stream);

    // Read Datablock.
    //stream->read( &mContainerBinSize );
    //stream->read( &mContainerBinCount );
    //stream->read( &mUseLayerSorting );
    //stream->read( &mLastInFrontSorting );
}


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
t2dSceneGraph::t2dSceneGraph() :    T2D_Stream_HeaderID(makeFourCCTag('2','D','S','G')),
                                    mConfigDataBlock(NULL),
                                    mRenderSortFn(layeredRenderSort),
                                    mUpdateSequenceKey(1),
                                    mSceneObjectCount(0),
                                    mSceneTime(0.0f),
                                    mScenePause(false),
                                    mMinFPS(10000),
                                    mMaxFPS(0.0f),
                                    mMaxObjectBinRelocations(0),
                                    mMaxObjectBinCollisions(0),
                                    mConstantForce(0.0f,0.0f),
                                    mGraviticConstantForce(false),
                                    mConstantForceActive(false),
                                    mLayerOrder(0),
                                    mDebugMask(0X00000000),
                                    mUseLayerSorting(scmUseLayerSorting),
                                    mLastInFrontSorting(scmLastInFrontSorting),
                                    mLocalSerialiseID(1),
                                    mSceneSerialiseID(2),
                                    mSerialiseMode(false),
                                    mpCurrentRenderWindow(NULL),
                                    mIsEditorScene(0),
                                    mSceneGraphInitialised(false),
                                    mGlobalTileMap(NULL)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mDeleteRequests );
    VECTOR_SET_ASSOCIATION( mDeleteRequestsTemp );
    VECTOR_SET_ASSOCIATION( mLinearPickList );

    // Set Vector-Array Associations.
    for ( U32 n = 0; n < t2dSceneGraph::maxLayersSupported; n++ )
    {
        VECTOR_SET_ASSOCIATION( mLayeredPickList[n] );
        VECTOR_SET_ASSOCIATION( mLayeredRenderList[n] );
    }

    for ( U32 n = 0; n < t2dSceneGraph::maxLayersSupported; n++ )
       mLayerSortModes[n] = T2D_SORT_NORMAL;

    // Turn-on Tick Processing.
    setProcessTicks( true );

    mNSLinkMask = LinkSuperClassName | LinkClassName;
	mUseUpdateCallback = false;
	
	//S32 iObjectID = getId();
	//dPrintf( "CREATING SCENE GRAPH WITH OBJECT ID: %d\n", iObjectID ); // RKS - DEBUG SHIT
}

//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
t2dSceneGraph::~t2dSceneGraph()
{
	// RKS:  Testing to to see how I can PROPERLY get the correct Scene Graph in CComponentGlobals...
	//S32 iObjectID = getId();
	//dPrintf( "DELETING SCENE GRAPH WITH OBJECT ID: %d\n", iObjectID );
}


//-----------------------------------------------------------------------------
// Layered Render Sort.
//
// This is optional and is used to ensure that objects that are added to the
// scene last are drawn on-top of everything added prior.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK t2dSceneGraph::layeredRenderSort(const void* a, const void* b)
{
    // Layer-Order Sort.
    return ((*((t2dSceneObject**)a))->mLayerOrder) - ((*((t2dSceneObject**)b))->mLayerOrder);
}


//-----------------------------------------------------------------------------
// Layered Y SortPoint Sort.
//
// Here we're sorting the lists by the sort point y.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK t2dSceneGraph::layeredYSortPointSort(const void* a, const void* b)
{
   t2dSceneObject* o1 = *(t2dSceneObject**)a;
   t2dSceneObject* o2 = *(t2dSceneObject**)b;

   F32 y1 = o1->getPosition().mY + ( o1->getSortPoint().mY * o1->getHalfSize().mY );
   F32 y2 = o2->getPosition().mY + ( o2->getSortPoint().mY * o2->getHalfSize().mY );

   // We sort lower y values before higher values.
   return y1 < y2 ? -1 : y1 > y2 ? 1 : 0;
}

S32 QSORT_CALLBACK t2dSceneGraph::layeredXSortPointSort(const void* a, const void* b)
{
   t2dSceneObject* o1 = *(t2dSceneObject**)a;
   t2dSceneObject* o2 = *(t2dSceneObject**)b;

   F32 y1 = o1->getPosition().mX + ( o1->getSortPoint().mX * o1->getHalfSize().mX );
   F32 y2 = o2->getPosition().mX + ( o2->getSortPoint().mX * o2->getHalfSize().mX );

   // We sort lower y values before higher values.
   return y1 < y2 ? -1 : y1 > y2 ? 1 : 0;
}

S32 QSORT_CALLBACK t2dSceneGraph::layeredNegYSortPointSort(const void* a, const void* b)
{
   t2dSceneObject* o1 = *(t2dSceneObject**)a;
   t2dSceneObject* o2 = *(t2dSceneObject**)b;

   F32 y1 = o1->getPosition().mY + ( o1->getSortPoint().mY * o1->getHalfSize().mY );
   F32 y2 = o2->getPosition().mY + ( o2->getSortPoint().mY * o2->getHalfSize().mY );

   // We sort lower y values before higher values.
   return y1 < y2 ? 1 : y1 > y2 ? -1 : 0;
}

S32 QSORT_CALLBACK t2dSceneGraph::layeredNegXSortPointSort(const void* a, const void* b)
{
   t2dSceneObject* o1 = *(t2dSceneObject**)a;
   t2dSceneObject* o2 = *(t2dSceneObject**)b;

   F32 y1 = o1->getPosition().mX + ( o1->getSortPoint().mX * o1->getHalfSize().mX );
   F32 y2 = o2->getPosition().mX + ( o2->getSortPoint().mX * o2->getHalfSize().mX );

   // We sort lower y values before higher values.
   return y1 < y2 ? 1 : y1 > y2 ? -1 : 0;
}


//-----------------------------------------------------------------------------
// Layered Pick Sort.
//
// Here we're sorting the lists by layer-order.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK t2dSceneGraph::layeredPickListSort(const void* a, const void* b)
{
    // Layer-Order Sort.
    return ((*((t2dSceneObject**)b))->mLayerOrder) - ((*((t2dSceneObject**)a))->mLayerOrder);
}


//-----------------------------------------------------------------------------
// Linear Pick Sort ( by Collision-Time ).
//
// Here we're sorting the list by Collision-Time.
//-----------------------------------------------------------------------------
S32 QSORT_CALLBACK t2dSceneGraph::linearPickListSortKeyCollisionTime(const void* a, const void* b)
{
    // Fetch Keys.
    const F32 keyA = (*((t2dSceneObject**)a))->mSortKeyCollisionTime;
    const F32 keyB = (*((t2dSceneObject**)b))->mSortKeyCollisionTime;

    // Return Sort Order.
    if ( keyA < keyB )
        return -1;
    else if ( keyA > keyB )
        return 1;
    else
        return 0;
}


//-----------------------------------------------------------------------------
// OnAdd
//-----------------------------------------------------------------------------
bool t2dSceneGraph::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Cast the Datablock.
    mConfigDataBlock = dynamic_cast<t2dSceneGraphDatablock*>(mConfigDataBlock);

    // Transfer Datablock (if we've got one).
    if ( t2dCheckDatablock( mConfigDataBlock ) )
    {
        // Initialise.
        initialise( mConfigDataBlock->mContainerBinSize, mConfigDataBlock->mContainerBinCount, mConfigDataBlock->mUseLayerSorting, mConfigDataBlock->mLastInFrontSorting );
    }

    // Synchronize Namespace's
    linkNamespaces();


    // Make sure we're initialised.
    if ( !isInitialised() )
        defaultInitialise();

    // Generate Process Head (Dummy Scene Object).
    mpProcessHead = new t2dSceneObject;

    // Clear Pick-List.
    clearPickList();

    // Create a global tilemap.
    t2dTileMap* tileMap = dynamic_cast<t2dTileMap*>(ConsoleObject::create("t2dTileMap"));
    if (tileMap && tileMap->registerObject())
    {
       tileMap->setModStaticFields(true);
       tileMap->setModDynamicFields(true);

       addToScene(tileMap);

       setGlobalTileMap(tileMap);
    }
    else if (tileMap)
       delete tileMap;

    gLoadingSceneGraph = this;

    // Tell the scripts
    //Con::executef(this, 1, "onAdd"); // RKS: I disabled this script call

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// OnRemove.
//-----------------------------------------------------------------------------
void t2dSceneGraph::onRemove()
{
    // tell the scripts
    //Con::executef(this, 1, "onRemove"); // RKS: I disabled this script call

    // Call Parent. Clear scene handles all the object removal, so we can skip
    // that part and just do the simobject stuff.
    SimObject::onRemove();

    // Clear Scene.
    clearScene();

    // Restore NameSpace's
    unlinkNamespaces();

    // Process Delete Requests.
    processDeleteRequests(true);

    // Detach All Scene Windows.
    detachAllSceneWindows();

    // Destroy Process Head (Dummy Scene Object).
    delete mpProcessHead;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RKS: I added this for testing shit -- (see the .h file) -- I have to make the functions virtual, so fuck it :(
//bool t2dSceneGraph::registerObject()
//{
//	dPrintf( "t2dSceneGraph::registerObject()\n" );
//	
//	int aaaa = 0;
//	aaaa++;
//	
//	bool bRet = Parent::registerObject();
//	
//	return bRet;
//	//return Parent::registerObject();
//}
//
////-----------------------------------------------------
//
//bool t2dSceneGraph::registerObject(U32 id)
//{
//	dPrintf( "t2dSceneGraph::registerObject(U32 id)\n" );
//	
//	int aaaa = 0;
//	aaaa++;
//	
//	bool bRet = Parent::registerObject( id );;
//	
//	return bRet;
//	//return Parent::registerObject( id );
//}
//
////-----------------------------------------------------
//
//bool t2dSceneGraph::registerObject(const char *name)
//{
//	dPrintf( "t2dSceneGraph::registerObject(const char *name)\n" );
//	
//	int aaaa = 0;
//	aaaa++;
//	
//	bool bRet = Parent::registerObject( name );
//	
//	return bRet;
//	//return Parent::registerObject( name );
//}
////-----------------------------------------------------
//
//bool t2dSceneGraph::registerObject(const char *name, U32 id)
//{
//	dPrintf( "t2dSceneGraph::registerObject(const char *name, U32 id)\n" );
//	
//	int aaaa = 0;
//	aaaa++;
//	
//	bool bRet = Parent::registerObject( name, id );
//	
//	aaaa++;
//	
//	return bRet;
//	//return Parent::registerObject( name, id );
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//-----------------------------------------------------------------------------
// Initialise Persistent Fields.
//-----------------------------------------------------------------------------
void t2dSceneGraph::initPersistFields()
{
    // Call Parent.
    Parent::initPersistFields();

    // Datablock.
    addField("config", Typet2dBaseDatablockPtr, Offset(mConfigDataBlock, t2dSceneGraph));

    // Layer Sorting.
    addField("UseLayerSorting", TypeBool, Offset(mUseLayerSorting, t2dSceneGraph));

    // Layer sorts... hack!
    char buffer[64];
    for ( U32 n = 0; n < t2dSceneGraph::maxLayersSupported; n++ )
    {
       dSprintf( buffer, 64, "layerSortMode%d", n );
       addField( buffer, TypeEnum, OffsetNonConst(mLayerSortModes[n], t2dSceneGraph), 1, &drawOrderSortTable, "");
    }

	// Debug rendering
	addField("DebugRendering", TypeS32, Offset(mDebugMask, t2dSceneGraph)); 
	addField("UseUpdateCallback", TypeBool, Offset(mUseUpdateCallback, t2dSceneGraph));
}

ConsoleMethod(t2dSceneGraph, getGlobalTileMap, S32, 2, 2, "() \n @return Returns the ID of the scene graph's global tile map.")
{
   if (object->getGlobalTileMap())
      return object->getGlobalTileMap()->getId();

   return 0;
}

//-----------------------------------------------------------------------------
// Initialise
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, initialise, void, 6, 6, "(containerBinSize, containerBinCount, useLayerSorting, lastInFrontSorting) Initialise Scene Graph.\n"
			  "@param containerBinSize The desired size of the scene graph's bins.\n"
			  "@param containerBinCount The number of bins to create\n"
			  "@param useLayerSorting Boolean flag to set whether to use by-layer sorting.\n"
			  "@param lastInFrontSorting Boolean flag to set whather to set the last item in the list to render in front of those before it\n"
			  "@return No return value.")
{
    // Initialise.
    object->initialise( dAtof(argv[2]), dAtoi(argv[3]), dAtob(argv[4]), dAtob(argv[5]) );
}
// Initialise.
void t2dSceneGraph::initialise( const F32 containerBinSize, const U32 containerBinCount, const bool useLayerSorting, const bool lastInFrontSorting )
{
    // Clear the scene and reset the scene container if we've
    // already been initialised.
    if ( mSceneGraphInitialised )
    {
        // Clear the scene.
        clearScene();

        // Reset Scene Container.
        mSceneContainer.destroyContainerSystem();

        // Reset Scene Initialise Flag.
        mSceneGraphInitialised = false;
    }

    // Initialise Container System
    mSceneContainer.initialise( this, containerBinSize, containerBinCount );

    // Set Layer Sorting Option.
    mUseLayerSorting    = useLayerSorting;
    mLastInFrontSorting = lastInFrontSorting;

    // Flag as Initialised.
    mSceneGraphInitialised = true;
}


//-----------------------------------------------------------------------------
// Default SceneGraph Initialise.
//-----------------------------------------------------------------------------
void t2dSceneGraph::defaultInitialise( void )
{
    // Do Default Initialisation.
    initialise( scmContainerBinSize, scmContainerBinCount, scmUseLayerSorting, scmLastInFrontSorting );
}


//-----------------------------------------------------------------------------
// Add to Scene.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, addToScene, void, 3, 3, "(object) Add t2dSceneObject to Scene.\n"
			  "@param object The t2dSceneObject's ID\n"
			  "@return No return value.")
{
    // Find t2dSceneObject Object.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject2D )
    {
        // Maybe it's a group.
        t2dSceneObjectGroup* pSceneObjectGroup = dynamic_cast<t2dSceneObjectGroup*>(Sim::findObject(argv[2]));

        if (!pSceneObjectGroup)
        {
           Con::warnf("t2dSceneGraph::addToScene - Couldn't find/Invalid object '%s'.", argv[2]);
           return;
        }
        object->addToScene( pSceneObjectGroup );
        return;
    }

    // Add to Scene.
    object->addToScene( pSceneObject2D );
}   
// Add to Scene.
void t2dSceneGraph::addToScene( t2dSceneObject* pSceneObject2D, bool triggerCallback )
{
    // Check that object is not already in a scene graph.
    if ( pSceneObject2D->getSceneGraph() )
    {
        // Are we warning about scene occupancy.
#ifdef PUAP_OPTIMIZE
        if ( !pSceneObject2D->getIsChild() && Con::getBoolVariable( "$pref::T2D::warnSceneOccupancy" ) )
        {
            // Yes, so warn.
            Con::warnf("t2dSceneGraph::addToScene() - Object '%s' is already in a SceneGraph!.", pSceneObject2D->getIdString());
        }
#endif
        // Remove from scene.
        pSceneObject2D->getSceneGraph()->removeFromScene( pSceneObject2D );
    }

    // Check Layer.
    if ( pSceneObject2D->getLayer() >= maxLayersSupported )
    {
        Con::warnf("t2dSceneGraph::addToScene() - Object '%s' has invalid layer!", pSceneObject2D->getIdString());
        return;
    }

    // Add Object to Container.
    //
    // NOTE:-   This is only here because we cannot control the order of when the user will
    //          set the properties and then finally add the object to the container system. 
    //          Therefore we're forced to wedge the scene-object into the scene so that when
    //          the container system is updated, it recognised it's in the scene but isn't
    //          yet in the bin-system.  This removes the now legacy "calculateSpatials()" call.
    pSceneObject2D->mpSceneGraph = this;

    // Update the Container Config.
    pSceneObject2D->updateSpatialConfig( true );

    // Add to Process List.
    addToProcessList( pSceneObject2D );

    // Set Layer Ordering.
    //
    // NOTE:-   We set the layer ordering here but don't manipulate it if we're
    //          in serialise mode.  This is because both the Scene and Objects
    //          contained will have their Layer-Order streamed from disk and
    //          this would mess that up.
    if ( !mSerialiseMode )
        pSceneObject2D->setLayerOrder( mLastInFrontSorting ? mLayerOrder++ : --mLayerOrder );

    // Do an initial integration.
    //
    // NOTE:-   We do this so that objects have a change to initialise themselves correctly.
    //
    //          We'll do a full pre/mid/post integration just in-case objects have some
    //          special requirement to do so.
    const F32 sceneTime = getSceneTime();
    CDebugStats* pDebugStats = &getDebugStats();
    pSceneObject2D->preIntegrate( sceneTime, 0.0f, pDebugStats );
    pSceneObject2D->integrateObject( getSceneTime(), 0.0f, pDebugStats );
    pSceneObject2D->postIntegrate( getSceneTime(), 0.0f, pDebugStats );

    // Add the object to our set if it's not already there.
    if(!findChildObject(pSceneObject2D))
       Parent::addObject(pSceneObject2D);

#ifndef TORQUE_TOOLS
#ifdef USE_COMPONENTS
	if( pSceneObject2D->isProperlyAdded() ) 
	{
		//-Mat adding C onAddtoScene to replace script one
		pSceneObject2D->onAddToScene();
		
		// RKS: I disabled this script call
		//if( triggerCallback ) 
		//{		
		//   Con::executef(pSceneObject2D, 2, "onAddToScene", getIdString());
		//}
	}
#else
    if (triggerCallback && pSceneObject2D->isProperlyAdded())
       Con::executef(pSceneObject2D, 2, "onAddToScene", getIdString());
#endif
#endif
}

void t2dSceneGraph::addToScene(t2dSceneObjectGroup* pSceneObjectGroup)
{
   // Check that object is not already in a scene graph.
   if ( pSceneObjectGroup->getSceneGraph() )
   {
      // Remove from scene.
      pSceneObjectGroup->getSceneGraph()->removeFromScene( pSceneObjectGroup );
   }

   pSceneObjectGroup->mSceneGraph = this;

   if (!findChildObject(pSceneObjectGroup))
      Parent::addObject(pSceneObjectGroup);

   for (S32 i = 0; i < pSceneObjectGroup->size(); i++)
      addToScene(pSceneObjectGroup->at(i));
}

void t2dSceneGraph::addToScene(SimObject* object)
{
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
   if (sceneObject)
      addToScene(sceneObject);

   else
   {
      t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (group)
         addToScene(group);
   }
}

//-----------------------------------------------------------------------------
// Remove from Scene.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, removeFromScene, void, 3, 3, "(object) Remove t2dSceneObject from Scene.\n"
			  "@param object The ID of the t2dSceneObject.\n"
			  "@return No return value.")
{
    // Find t2dSceneGraph Object.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject2D )
    {
        t2dSceneObjectGroup* pSceneObjectGroup = dynamic_cast<t2dSceneObjectGroup*>(Sim::findObject(argv[2]));

        if (!pSceneObjectGroup)
        {
           // Are we warning about scene occupancy.
           if ( Con::getBoolVariable( "$pref::T2D::warnSceneOccupancy" ) )
           {
               // Yes, so warn.
               Con::warnf("t2dSceneGraph::removeFromScene() - Couldn't find/Invalid object '%s'.", argv[2]);
           }

           return;
        }

        object->removeFromScene(pSceneObjectGroup);

        // Finish Here.
        return;
    }

    // Remove from Scene.
    object->removeFromScene( pSceneObject2D );
}  
// Remove from Scene.
void t2dSceneGraph::removeFromScene( t2dSceneObject* pSceneObject2D )
{
    // Check if object is actually in a scene graph.
    if ( !pSceneObject2D->getSceneGraph() )
    {
        Con::warnf("t2dSceneGraph::removeFromScene() - Object '%s' is not in a SceneGraph!.", pSceneObject2D->getIdString());
        return;
    }

    // Process Destroy Notifications.
    pSceneObject2D->processDestroyNotifications();

    // Dismount Any Camera.
    pSceneObject2D->dismountCamera();

    // Remove from Process List.
    removeFromProcessList( pSceneObject2D );

    // Remove Scene Object from Scene Container.
    mSceneContainer.removeSceneObject( pSceneObject2D );

	//remove from the SceneWindow last pickers
	for(S32 i=0;i<mAttachedSceneWindows.size();i++)
	{
		(dynamic_cast<t2dSceneWindow*>(mAttachedSceneWindows[i]))->removeFromLastPicker(pSceneObject2D);
	}

    // Reset SceneGraph Reference.
    pSceneObject2D->mpSceneGraph = NULL;

    // Remove from Set, When Appropriate.
    SimSet::iterator pObjectLookup = find(begin(), end(), pSceneObject2D);
    if (pObjectLookup != end())
       Parent::removeObject(pSceneObject2D);

    else if (pSceneObject2D->getSceneObjectGroup())
    {
       if (pSceneObject2D->getSceneObjectGroup()->getSceneGraph() == this)
          pSceneObject2D->getSceneObjectGroup()->removeObject(pSceneObject2D);
    }

    //Con::executef(pSceneObject2D, 2, "onRemoveFromScene", this); // RKS: I disabled this script call
}

void t2dSceneGraph::removeFromScene(t2dSceneObjectGroup* pSceneObjectGroup)
{
   // Check if object is actually in a scene graph.
   if ( !pSceneObjectGroup->getSceneGraph() )
   {
      Con::warnf("t2dSceneGraph::removeFromScene() - Object '%s' is not in a SceneGraph!.", pSceneObjectGroup->getIdString());
      return;
   }

   pSceneObjectGroup->mSceneGraph = NULL;
   
   for (S32 i = 0; i < pSceneObjectGroup->size(); i++)
      removeFromScene(pSceneObjectGroup->at(i));

    SimSet::iterator pObjectLookup = find(begin(), end(), pSceneObjectGroup);
    if (pObjectLookup != end())
       Parent::removeObject(pSceneObjectGroup);

    else if (pSceneObjectGroup->getSceneObjectGroup())
    {
       if (pSceneObjectGroup->getSceneObjectGroup()->getSceneGraph() == this)
          pSceneObjectGroup->getSceneObjectGroup()->removeObject(pSceneObjectGroup);
    }
}

void t2dSceneGraph::removeFromScene(SimObject* object)
{
   t2dSceneObject* sceneObject = dynamic_cast<t2dSceneObject*>(object);
   if (sceneObject)
      removeFromScene(sceneObject);

   else
   {
      t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (group)
         removeFromScene(group);
   }
}

bool t2dSceneGraph::findChildObject(SimObject* searchObject)
{
   for (S32 i = 0; i < size(); i++)
   {
      SimObject* object = at(i);

      if (object == searchObject)
         return true;

      t2dSceneObjectGroup* group = dynamic_cast<t2dSceneObjectGroup*>(object);
      if (group)
      {
         if (group->findChildObject(searchObject))
            return true;
      }
   }

   // If we make it here, no.
   return false;
}

// These just need to be redirected to the scene object management versions of the function.
void t2dSceneGraph::addObject( SimObject* object )
{
   t2dSceneObjectGroup* parentGroup = t2dSceneObjectGroup::getSceneObjectGroup(object);
   t2dSceneGraph* parentGraph = t2dSceneObjectGroup::getSceneGraph(object);

   if (parentGraph == this)
   {
      if (parentGroup)
         parentGroup->removeObject(object);

      Parent::addObject(object);
   }

   else
   {
      if (parentGraph)
         parentGraph->removeFromScene(object);

      else if (parentGroup)
         parentGroup->removeObject(object);

      addToScene(object);
   }
}

void t2dSceneGraph::removeObject(SimObject* object)
{
   Parent::removeObject(object);
}


//-----------------------------------------------------------------------------
// Clear Scene.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, clearScene, void, 2, 3, "([deleteObjects]) Clear Scene of all t2dSceneObject objects.\n"
			  "@param deleteObjects A boolean flag that sets whether to delete the objects as well as remove them from the scene (default is true).\n"
			  "@return No return value.")
{
    // Calculate 'Delete Objects' flag.
    bool deleteObjects;
    if ( argc >= 3 )
        deleteObjects = dAtob( argv[2] );
    else
        deleteObjects = true;

    // Clear Scene.
    object->clearScene( deleteObjects );
}
// Clear Scene.
void t2dSceneGraph::clearScene( bool deleteObjects )
{
    // Process until all objects are removed.
    while ( (getSceneObjectCount()) > 0 )
    {
        // Fetch Object.
        t2dSceneObject* pSceneObject = getProcessHead()->getProcessPrevious();

        // Remove Object from Scene.
        removeFromScene( pSceneObject );

        // Queue Object for deletion.
        if ( deleteObjects )
            pSceneObject->safeDelete();
    }
}

//-----------------------------------------------------------------------------
// Set Layer Draw-Order.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setLayerDrawOrder, bool, 4, 4, "(object, orderCommand) Sets the Layer Draw Order for Specified Object.\n"
			  "@param object The ID of the desired t2dSceneObject\n"
			  "@param orderCommand A mnemonic representing the ordering option (FRONT, BACK, FORWARD, BACKWARD)\n"
			  "@return Returns true on success.")
{
    // Find t2dSceneObject Object.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject2D )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::setLayerDrawOrder - Couldn't find/Invalid object '%s'.", argv[2]);
        // Return Not-Moved.
        return false;
    }

    // Calculate Draw-Order Command.
    t2dSceneGraph::eDrawOrderCommand command = getDrawOrderCommand(argv[3]);

    // Check Draw-Order Command.
    if ( command == t2dSceneGraph::T2D_ORDER_INVALID )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::setLayerDrawOrder - Invalid Draw-Order Command! '%s'.", argv[3]);
        // Return Not-Moved.
        return false;
    }

    // Set Layer Draw-Order.
    return object->setLayerDrawOrder( pSceneObject2D, command );
}
// Set Layer Draw-Order.
bool t2dSceneGraph::setLayerDrawOrder( t2dSceneObject* pSceneObject2D, eDrawOrderCommand command )
{
    // Check it's in this scene.
    if ( pSceneObject2D->getSceneGraph() != this )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::setLayerDrawOrder - Selected object is not in specified scene! '%s'.", pSceneObject2D->getIdString());
        // Return Not-Moved.
        return false;
    }

    // Nothing to do if object is alone!
    if ( getSceneObjectCount() == 1 )
        // Return Not-Moved.
        return false;

    // Scene Objects.
    typeSceneObjectVector tempSceneList;
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( tempSceneList );

    // Note Source Layer.
    U32 srcLayer = pSceneObject2D->getLayer();

    // Fetch First Object.
    t2dSceneObject* pSceneObjectRef2D = getProcessHead();

    // Process Objects until all actioned.
    while ( 1 )
    {
        // Fetch Next Object.
        pSceneObjectRef2D = pSceneObjectRef2D->getProcessNext();

        // Is this Object in the same layer?
        if ( pSceneObjectRef2D->getLayer() == srcLayer )
            // Yes, so store in List.
            tempSceneList.push_back( pSceneObjectRef2D );

        // Finish if we've actioned everything.
        if ( pSceneObjectRef2D == getProcessHead() )
            break;
    };

    // Quick-Sort the layer.
    dQsort(tempSceneList.address(), tempSceneList.size(), sizeof(t2dSceneObject*), layeredRenderSort);

    // Source Object-Index.
    U32 srcObjectIndex = 0;

    // Find Source-Object Index.
    for ( U32 n = 0; n < tempSceneList.size(); n++ )
        // Is this the Source-Object?
        if ( tempSceneList[n] == pSceneObject2D )
        {
            // Yes, so set Source-Object Index.
            srcObjectIndex = n;
            // Finish.
            break;
        }

    // Handle Command.
    switch( command )
    {
        // Move to Front of Layer.
        case T2D_ORDER_FRONT:
        {
            // *********************************************************************************
            // We need to note the front-objects' layer-order then cascade the layer-orders
            // backwards through the list up to the source-object and finally assign the
            // source-object with the previous front layer-order.
            // *********************************************************************************

            // Finish if already at front of layer.
            if ( srcObjectIndex == (tempSceneList.size()-1) )
                // Return Not-Moved.
                return false;

            // Fetch Front Layer-Order.
            U32 frontLayerOrder = tempSceneList.last()->getLayerOrder();

            // Cascade the Layer-Orders backwards.
            for ( U32 n = tempSceneList.size()-1; n > srcObjectIndex; n-- )
                tempSceneList[n]->setLayerOrder( tempSceneList[n-1]->getLayerOrder() );

            // Assign Front Layer-Order.
            pSceneObject2D->setLayerOrder( frontLayerOrder );

            // The front of the layer is the last object created, so we need to reorder this object
            // to the back of the simset.
            pushObjectToBack(pSceneObject2D);

        } break;

        // Move to Back of Layer.
        case T2D_ORDER_BACK:
        {
            // *********************************************************************************
            // We need to note the back-objects' layer-order then cascade the layer-orders
            // forwards through the list up to the source-object and finally assign the
            // source-object with the previous back layer-order.
            // *********************************************************************************

            // Finish if already at back of layer.
            if ( srcObjectIndex == 0 )
                // Return Not-Moved.
                return false;

            // Fetch Back Layer-Order.
            U32 backLayerOrder = tempSceneList.first()->getLayerOrder();

            // Cascade the Layer-Orders backwards.
            for ( U32 n = 0; n < srcObjectIndex; n++ )
                tempSceneList[n]->setLayerOrder( tempSceneList[n+1]->getLayerOrder() );

            // Assign Front Layer-Order.
            pSceneObject2D->setLayerOrder( backLayerOrder );

            // The back of the layer is the last object created, so we need to reorder this object
            // to the front of the simset.
            bringObjectToFront(pSceneObject2D);

        } break;

        // Move forward within Layer.
        case T2D_ORDER_FORWARD:
        {
            // *********************************************************************************
            // We need to swap the front-objects' layer-order with the source-object.
            // *********************************************************************************

            // Finish if already at front of layer.
            if ( srcObjectIndex == (tempSceneList.size()-1) )
                // Return Not-Moved.
                return false;

            // Swap Source-Object with Object in-front of it.
            U32 srcLayerOrder = tempSceneList[srcObjectIndex]->getLayerOrder();
            tempSceneList[srcObjectIndex]->setLayerOrder( tempSceneList[srcObjectIndex+1]->getLayerOrder() );
            tempSceneList[srcObjectIndex+1]->setLayerOrder( srcLayerOrder );

            // The reorder command puts the source object in front of the target object so we need
            // to reorder this before srcObjectIndex + 1.
            if (srcObjectIndex == (tempSceneList.size() - 2))
               pushObjectToBack(tempSceneList[srcObjectIndex]);
            else
               reOrder(tempSceneList[srcObjectIndex], tempSceneList[srcObjectIndex + 2]);


        } break;

        // Move backward within Layer.
        case T2D_ORDER_BACKWARD:
        {
            // *********************************************************************************
            // We need to swap the back-objects' layer-order with the source-object.
            // *********************************************************************************

            // Finish if already at back of layer.
            if ( srcObjectIndex == 0 )
                // Return Not-Moved.
                return false;

            // Swap Source-Object with Object in-front of it.
            U32 srcLayerOrder = tempSceneList[srcObjectIndex]->getLayerOrder();
            tempSceneList[srcObjectIndex]->setLayerOrder( tempSceneList[srcObjectIndex-1]->getLayerOrder() );
            tempSceneList[srcObjectIndex-1]->setLayerOrder( srcLayerOrder );

            // The reorder command puts the source object in front of the target object so we need
            // to reorder this before srcObjectIndex - 1.
            reOrder(tempSceneList[srcObjectIndex], tempSceneList[srcObjectIndex - 1]);

        } break;
    };

    // Return Moved.
    return true;
}


//-----------------------------------------------------------------------------
// Set Scene Draw-Order.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setSceneDrawOrder, bool, 4, 4, "(object, orderCommand) Sets the Scene Draw Order for Specified Object.\n"
			  "@param object The ID of the desired t2dSceneObject\n"
			  "@param orderCommand A mnemonic representing the ordering option (FRONT, BACK, FORWARD, BACKWARD)\n"
			  "@return Return true on success.")
{
    // Find t2dSceneObject Object.
    t2dSceneObject* pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject2D )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::setSceneDrawOrder - Couldn't find/Invalid object '%s'.", argv[2]);
        // Return Not-Moved.
        return false;
    }

    // Calculate Draw-Order Command.
    t2dSceneGraph::eDrawOrderCommand command = getDrawOrderCommand(argv[3]);

    // Check Draw-Order Command.
    if ( command == t2dSceneGraph::T2D_ORDER_INVALID )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::setSceneDrawOrder - Invalid Draw-Order Command! '%s'.", argv[3]);
        // Return Not-Moved.
        return false;
    }

    // Set Scene Draw-Order.
    return object->setSceneDrawOrder( pSceneObject2D, command );
}
// Set Scene Draw-Order.
bool t2dSceneGraph::setSceneDrawOrder( t2dSceneObject* pSceneObject2D, eDrawOrderCommand command )
{
    // Check it's in this scene.
    if ( pSceneObject2D->getSceneGraph() != this )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::setSceneDrawOrder - Selected object is not in specified scene! '%s'.", pSceneObject2D->getIdString());
        // Return Not-Moved.
        return false;
    }

    // Reset Object-Moved Flag.
    bool objectMoved = false;

    // Handle Command.
    switch( command )
    {
        // Move to Front of Layer.
        case T2D_ORDER_FRONT:
        {
            // Assign Front Layer.
            if ( pSceneObject2D->getLayer() != 0 )
            {
                // Move to Front-Layer.
                pSceneObject2D->setLayer( 0 );
                // Flag Moved.
                objectMoved = true;
            }

            // Set Layer-Order to Front.
            if ( setLayerDrawOrder( pSceneObject2D, T2D_ORDER_FRONT ) )
                // Flag Moved.
                objectMoved = true;

        } break;

        // Move to Back of Layer.
        case T2D_ORDER_BACK:
        {
            // Assign Back Layer.
            if ( pSceneObject2D->getLayer() != (maxLayersSupported-1) )
            {
                // Move to Back Layer.
                pSceneObject2D->setLayer( maxLayersSupported-1 );
                // Flag Moved.
                objectMoved = true;
            }

            // Set Layer-Order to Back.
            if ( setLayerDrawOrder( pSceneObject2D, T2D_ORDER_BACK ) )
                // Flag Moved.
                objectMoved = true;

        } break;

        // Move forward within Layer.
        case T2D_ORDER_FORWARD:
        {
            // Move Object Forward until it moves or hits the front.
            while(1)
            {
                // Set Layer-Order Forward.
                if ( setLayerDrawOrder( pSceneObject2D, T2D_ORDER_FORWARD ) )
                {
                    // Flag Moved.
                    objectMoved = true;
                    // Finish.
                    break;
                }
                else
                {
                    // Are we at the front layer?
                    if ( pSceneObject2D->getLayer() == 0 )
                    {
                        // Yes, so finish here as we can't go any further forward.
                        break;
                    }
                    else
                    {
                        // Move forward a layer.
                        pSceneObject2D->setLayer( pSceneObject2D->getLayer()-1 );
                        // Move to Back of Layer.
                        setLayerDrawOrder( pSceneObject2D, T2D_ORDER_BACK );
                        // Flag Moved.
                        objectMoved = true;
                        // Finish.
                        //break;
                    }
                }
            };

        } break;

        // Move backward within Layer.
        case T2D_ORDER_BACKWARD:
        {
            // Move Object Backward until it moves or hits the back.
            while(1)
            {
                // Set Layer-Order Backward.
                if ( setLayerDrawOrder( pSceneObject2D, T2D_ORDER_BACKWARD ) )
                {
                    // Flag Moved.
                    objectMoved = true;
                    // Finish.
                    break;
                }
                else
                {
                    // Are we at the back layer?
                    if ( pSceneObject2D->getLayer() == (maxLayersSupported-1) )
                    {
                        // Yes, so finish here as we can't go any further backward.
                        break;
                    }
                    else
                    {
                        // Move backward a layer.
                        pSceneObject2D->setLayer( pSceneObject2D->getLayer()+1 );
                        // Move to Front of Layer.
                        setLayerDrawOrder( pSceneObject2D, T2D_ORDER_FRONT );
                        // Flag Moved.
                        objectMoved = true;
                        // Finish.
                        //break;
                    }
                }
            };

        } break;
    };

    // Return Moved Status.
    return objectMoved;
}


//-----------------------------------------------------------------------------
// Set Constant Force.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setConstantForce, void, 3, 4, "(forceX / forceY, [gravitic]) Apply a continuous force.\n"
			  "@param forceX/forceY The direction and magnitude of the force in each direction. Formatted as either (\"forceX forceY\") or (forceX, forceY)\n"
			  "@param gravitic An optional boolean flag for whether the force is gravitic in nature (default is false).\n"
			  "@return No return value.\n")
{
   // The force.
   t2dVector force;
   // Gravitic flag.
   bool gravitic = false;

   // Grab the element count.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("forceX forceY", [gravitic])
   if ((elementCount == 2) && (argc < 5))
   {
      force = t2dSceneObject::getStringElementVector(argv[2]);

      if (argc > 3)
         gravitic = dAtob(argv[3]);
   }

   // (forceX, forceY, [gravitic])
   else if ((elementCount == 1) && (argc > 3))
   {
      force = t2dVector(dAtof(argv[2]), dAtof(argv[3]));

      if (argc > 4)
         gravitic = dAtob(argv[4]);
   }

   // Invalid
   else
   {
      Con::warnf("t2dSceneObject::setImpulseForce() - Invalid number of parameters!");
      return;
   }

    // Set Constant Force.
    object->setConstantForce(force, gravitic);
}
// Set Constant Force.
void t2dSceneGraph::setConstantForce( t2dVector force, bool gravitic )
{
    // Add Constant-Force to Physics.
    mConstantForce = force;

    // Set Gravitic Flag.
    mGraviticConstantForce = gravitic;
    // Usage Flag.
    mConstantForceActive = mNotZero(mConstantForce.lenSquared());
}


//-----------------------------------------------------------------------------
// Set Constant Force Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setConstantForcePolar, void, 4, 5, "(angle, force, [gravitic?]) Apply a continuous polar force.\n"
			  "@param angle The angle component of the force (polar form)\n"
			  "@param force The polar magnitude of the force.\n"
			  "@param gravitic An optional boolean flag for whether the force is gravitic in nature (default is false).\n"
			  "@return No return value.\n")
{
    // Renormalise Angle.
    F32 angle = mFmod(dAtof(argv[2]), 360.0f);
    // Fetch Force.
    F32 force = dAtof(argv[3]);

    // Fetch Gravitic Flag.
    bool gravitic = false;
    if ( argc >= 5 )
        gravitic = dAtob(argv[4]);

    // Set Constant Force Polar.
    object->setConstantForce( t2dVector( mSin(mDegToRad(angle))*force, -mCos(mDegToRad(angle))*force ), gravitic );
}


//-----------------------------------------------------------------------------
// Get Constant Force.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, getConstantForce, const char*, 2, 2, "() Gets the scene constant-force.\n"
			  "@return Returns the scene's set constant force in Cartesian coordinatesas a string formatted as \"forceX forceY\"")
{
    // Get Constant Force.
    t2dVector constantForce = object->getConstantForce();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", constantForce.mX, constantForce.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Stop Constant Force Polar.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, stopConstantForce, void, 2, 2, "() Stops any continuous force.\n"
			  "@return No return value.")
{
    // Reset Constant Force.
    object->setConstantForce( t2dVector::getZero(), false );
}


//-----------------------------------------------------------------------------
// Set Scene Pause.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setScenePause, void, 3, 3, "(status) Sets scene pause status.\n"
			  "@return No return value.")
{
    // Set Scene Pause.
    object->setScenePause( dAtob(argv[2]) );
}


//-----------------------------------------------------------------------------
// Get Scene Pause.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, getScenePause, bool, 2, 2, "() Gets scene pause status.\n"
			  "@return Returns a boolean value. True if pause status, false otherwise.")
{
    // Get Scene Pause.
    return object->getScenePause();
}


//-----------------------------------------------------------------------------
// Load Scene from Disk.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, loadScene, bool, 3, 3, "(file) Loads a Scene from a Disk File.\n"
			  "@param file The name of the scene file to load.\n"
			  "@return Returns true on success, false otherwise (ie filenotfound or unable to open for read)")
{
    // Load Scene from Disk.
    return object->loadScene( argv[2] );
}

// Load Scene from Disk.
bool t2dSceneGraph::loadScene( const char* sceneFile )
{
    // Critical Section for Serialisation Mode.
    //
    // NOTE:-   This seemingly elaborate construction is to ensure
    //          that the serialise mode is reset if we decide to
    //          abort early out of this function.
    t2dSerialiseSection serialSection( &mSerialiseMode );

    // Expand relative paths.
    char buffer[1024];
    if ( sceneFile )
        if ( Con::expandScriptFilename( buffer, sizeof( buffer ), sceneFile ) )
            sceneFile = buffer;

    // Open Scene File.
    Stream* fileStream = ResourceManager->openStream( sceneFile );
    // Check Stream.
    if ( !fileStream )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::loadScene() - Could not Open File '%s' for Scene-Load.", sceneFile);
        // Return Error.
        return false;
    }

    // Scene Objects.
    typeSceneObjectVector ObjReferenceList;
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( ObjReferenceList );

    // Load SceneGraph Stream.
    if ( !loadStream( *fileStream, this, ObjReferenceList, false ) )
    {
        // Close Stream.
        ResourceManager->closeStream( fileStream );
        // Warn.
        Con::warnf("t2dSceneGraph::loadScene() - Could not read SceneGraph contents from file '%s' for Scene-Load.", sceneFile);
        // Clear the Scene.
        clearScene();
        // Return Error.
        return false;
    }

    // Close Stream.
    ResourceManager->closeStream( fileStream );

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Save Scene to Disk.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, saveScene, bool, 3, 3, "(file) Saves a Scene to a Disk File.\n"
			  "@param file Name of file to write to.\n"
			  "@return Returns true on success, false otherwise.")
{
    // Save Scene to Disk.
    return object->saveScene( argv[2] );
}

// Save Scene to Disk.
bool t2dSceneGraph::saveScene( const char* sceneFile )
{
    // Expand relative paths.
    char buffer[1024];
    if ( sceneFile )
        if ( Con::expandToolScriptFilename( buffer, sizeof( buffer ), sceneFile ) )
            sceneFile = buffer;

    // Open Scene File.
    FileStream fileStream;
    if ( !ResourceManager->openFileForWrite( fileStream, sceneFile, FileStream::Write ) )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::saveScene() - Could not open File '%s' for Scene-Save.", sceneFile);
        return false;
    }

    // Process Any Pending Delete Requests.
    processDeleteRequests(true);

    // Reset Serialise Key.
    const U32 serialiseKey = 1;

    // Save the scene.
    if ( !saveStream( fileStream, getNextSerialiseID(), serialiseKey ) )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::saveScene() - Could not write SceneGraph contents to file '%s' for Scene-Save.", sceneFile);
        // Return Error.
        return false;
    }

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Get Child Count.
//-----------------------------------------------------------------------------
U32 t2dSceneGraph::getChildCount( void )
{
    // No children if scene is empty!
    if ( getSceneObjectCount() == 0 )
        return 0;

    // Reset Child Count.
    U32 childCount = 0;

    // Fetch First Object.
    t2dSceneObject* pSceneObject2D = getProcessHead();

    // Process Objects until all actioned.
    while ( 1 )
    {
        // Fetch Next Object.
        pSceneObject2D = pSceneObject2D->getProcessNext();

        // Finish if we've actioned everything.
        if ( pSceneObject2D == getProcessHead() )
            break;

        // Count Child Objects.
        if ( pSceneObject2D->getIsChild() )
            childCount++;
    };

    // Return Child Count.
    return childCount;
}


//-----------------------------------------------------------------------------
// Add to Process List.
//-----------------------------------------------------------------------------
void t2dSceneGraph::addToProcessList( t2dSceneObject* pSceneObject2D )
{
    // Check we're not already linked.
    if ( pSceneObject2D->processIsLinked() )
        return;

    // Add to Process List.
    pSceneObject2D->processLinkBefore( getProcessHead() );

    // Increase Scene Object Count.
    mSceneObjectCount++;
}


//-----------------------------------------------------------------------------
// Remove from Process List.
//-----------------------------------------------------------------------------
void t2dSceneGraph::removeFromProcessList( t2dSceneObject* pSceneObject2D )
{
    // Check we're linked.
    if ( !pSceneObject2D->processIsLinked() )
        return;

    // Is this object mounted?
    if ( pSceneObject2D->processIsMounted() )
    {
        // Yes, so dismount.
        pSceneObject2D->dismount();
    }

    // Remove from Process List.
    pSceneObject2D->processUnLink();

    // Decrease Scene Object Count.
    mSceneObjectCount--;
}


//-----------------------------------------------------------------------------
// Get Scene Time.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, getSceneTime, F32, 2, 2, "() Gets the Scene Graph Time.\n"
			  "@return Returns the time as a floating point number\n")
{
    // Get Scene Time.
    return object->getSceneTime();
}   


//-----------------------------------------------------------------------------
// Get Scene Object-Count.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, getSceneObjectCount, S32, 2, 2, "() Gets the Scene Object-Count.\n"
			  "@return Returns the number of scene objects in current scenegraph as an integer.")
{
    // Get Scene Object-Count.
    return object->getSceneObjectCount();
}  


//-----------------------------------------------------------------------------
// Get Scene Object.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, getSceneObject, S32, 3, 3, "(objectIndex) Gets the selected Scene Object.\n"
			  "@param objectIndex The index of the desired object\n"
			  "@return The ID of the object.")
{
    // Fetch Object Index.
    const U32 objectIndex = dAtoi(argv[2]);

    // Fetch Scene Object.
    const t2dSceneObject* pSceneObject = object->getSceneObject( objectIndex );
    // Check Object.
    if ( pSceneObject != NULL )
    {
        // No error so return object id.
        return pSceneObject->getId();
    }
    else
    {
        // Error so warn.
        Con::warnf("t2dSceneGraph::getSceneObject() - Cannot retrieve specified object index (%d)!", objectIndex);
        // Return no object.
        return 0;
    }
}
// Get Scene Object.
t2dSceneObject* t2dSceneGraph::getSceneObject( const U32 objectIndex )
{
   // No objects if scene is empty!
   if ( getSceneObjectCount() == 0 )
      return NULL;

   // Reset object index.
   U32 objectCount = 0;

   // Fetch First Object.
   t2dSceneObject* pSceneObject2D = getProcessHead()->getProcessNext();

   // Process Objects until all actioned.
   while ( pSceneObject2D != getProcessHead() )
   {
       // Is this our object?
       if ( objectIndex == objectCount )
       {
           // Yes, so return object.
           return pSceneObject2D;
       }

        // Fetch Next Object.
        pSceneObject2D = pSceneObject2D->getProcessNext();

        // Increase Object Count.
        objectCount++;
   };

   // Return Error.
   return NULL;
}

//-----------------------------------------------------------------------------
// Get Scene Object-List.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, getSceneObjectList, const char*, 2, 2, "() Gets the Scene Object-List.\n"
			  "@return Returns a string with a list of object IDs")
{
    // Scene Object-List.
    Vector<t2dSceneObject*> objList;

    // Finish here if there are no scene objects.
    U32 objCount = object->getSceneObjectList( objList );
    if( objCount == 0 )
        return NULL;

    // Our return bugger will be 6 times the size of our object list (4 for Id (+1 for future size?) + 1 for space).
    U32 maxBufferSize = objCount * 12;
    // Create Returnable Buffer.
    char *pBuffer = Con::getReturnBuffer( maxBufferSize ); 
    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Iterate through the list and generate an id string list to return
    for ( S32 n = 0; n < objList.size(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", objList[n]->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneGraph::getSceneObjectList() - Not enough space to return all %d objects!", objList.size());
            break;
        }
    }

    // Return buffer.
    return pBuffer;
}  
// Get Scene Object-List.
U32 t2dSceneGraph::getSceneObjectList( Vector<t2dSceneObject*> &objectList )
{
   // No objects if scene is empty!
   if ( getSceneObjectCount() == 0 )
      return 0;

   // Reset object Count.
   U32 objectCount = 1;

   // Fetch First Object.
   t2dSceneObject* pSceneObject2D = getProcessHead()->getProcessNext();

   // Push the first object onto the list
   objectList.push_back( pSceneObject2D );

   // Process Objects until all actioned.
   while ( pSceneObject2D != getProcessHead() )
   {
        // Fetch Next Object.
        pSceneObject2D = pSceneObject2D->getProcessNext();

        // Finish if we've actioned everything.
        if ( pSceneObject2D == getProcessHead() )
            break;

        // Push the next object onto the list
        objectList.push_back( pSceneObject2D );

        // Increase Object Count.
        objectCount++;
   };

   // Return Child Count.
   return objectCount;
}


//-----------------------------------------------------------------------------
// Set Debug On Mode(s).
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setDebugOn, void, 3, 2 + T2D_DEBUGMODE_COUNT, "(debugMask) Sets Debug On Mode(s).\n"
			  "@param debugMask Either a list of debug modes (comma separated), or a string with the modes (space separated)\n"
			  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneGraph::setDebugOn() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneGraph::setDebugOn() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneGraph::setDebugOn() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   object->setDebugOn(mask);
}   
// Set Debug On Mode(s).
void t2dSceneGraph::setDebugOn( U32 debugModeMask )
{
    // Set Debug On Mask.
    mDebugMask |= debugModeMask;
}

ConsoleMethod(t2dSceneGraph, getDebugOn, bool, 3, 3, "(debugMode) Gets the state of the debug mode\n"
			  "@param The specific debug mode to check active state of.\n"
			  "@return Returns true if active, false if not.")
{
   U32 mask = 1 << dAtoi(argv[2]);
   return object->getDebugOn(mask);
}

bool t2dSceneGraph::getDebugOn(U32 debugMask)
{
   return mDebugMask & debugMask;
}


//-----------------------------------------------------------------------------
// Set Debug Off Mode(s).
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, setDebugOff, void, 3, 2 + T2D_DEBUGMODE_COUNT, "(debugMask) Sets Debug Off Mode(s).\n"
			  "@param debugMask Either a list of debug modes to turn off (comma separated) or a string (space separated)\n"
			  "@return No return value.")
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("t2dSceneGraph::setDebugOn() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         U32 bit = dAtoi(t2dSceneObject::getStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneGraph::setDebugOn() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for (U32 i = 2; i < argc; i++)
      {
         U32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= T2D_DEBUGMODE_COUNT))
         {
            Con::warnf("t2dSceneGraph::setDebugOn() - Invalid debug mode specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   object->setDebugOff(mask);
}   
// Set Debug Off Mode(s).
void t2dSceneGraph::setDebugOff( U32 debugMask )
{
    // Set Debug Off Mask.
    mDebugMask &= ~debugMask;
}


//-----------------------------------------------------------------------------
// Clear Pick-List.
//-----------------------------------------------------------------------------
void t2dSceneGraph::clearPickList( void )
{
    // Clear Linear Pick List.
	mLinearPickList.clear();

    // Clear Layered Pick List.
    for ( U32 i = 0; i < t2dSceneGraph::maxLayersSupported; i++ )
        mLayeredPickList[i].clear();
}


//-----------------------------------------------------------------------------
// Pick Point.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, pickPoint, const char*, 3, 8, "(x / y, [groupMask], [layerMask], [showInvisible?], [excludeObject] ) Picks objects intersecting point with optional group/layer masks.\n"
			  "@param x/y The coordinate of the point as either (\"x y\") or (x,y)\n"
			  "@param groupMask Optional group mask.\n"
			  "@param layerMask Optional layer mask.\n"
			  "@param showInvisible Object visibility setting\n"
			  "@param excludeObject Object to exclude from check\n"
			  "@return Returns list of object IDs or NULL on failure")
{
   // The point.
   t2dVector point;
   // The index of the first optional parameter.
   U32 firstArg;

   // Grab the number of elements in the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc < 8))
   {
      point = t2dSceneObject::getStringElementVector(argv[2]);
      firstArg = 3;
   }
   
   // (x, y)
   else if ((elementCount == 1) && (argc > 3))
   {
       point = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       firstArg = 4;
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneGraph::pickPoint() - Invalid number of parameters!");
      return NULL;
   }

   // Calculate Group Mask.
   U32 groupMask = T2D_MASK_ALL;
   if (argc > firstArg)
      groupMask = dAtoi(argv[firstArg]);

   // Calculate Group Mask.
   U32 layerMask = T2D_MASK_ALL;
   if (argc > (firstArg + 1))
      layerMask = dAtoi(argv[firstArg + 1]);

   // Calculate Group Mask.
   bool showInvisible = false;
   if (argc > (firstArg + 2))
      showInvisible = dAtob(argv[firstArg + 2]);

    // Get Optional Exclude Object.
    t2dSceneObject* pSceneObject2D = NULL;
    if ( argc > (firstArg + 3) )
    {
        // Find t2dSceneObject Object.
        pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[firstArg + 3]));

        // Validate Object.
        if ( !pSceneObject2D )
        {
            Con::warnf("t2dSceneGraph::pickPoint() - Couldn't find/Invalid object '%s'.", argv[firstArg + 3]);
            return NULL;
        }
    }

    // Pick Point.
    if ( object->pickPoint( point, groupMask, layerMask, showInvisible, pSceneObject2D  ) == 0 )
    {
        // No Objects!
        return NULL;
    }

    // Fetch Pick Vector.
    typeSceneObjectVectorConstRef pickVector = object->getPickList();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < pickVector.size(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", pickVector[n]->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneGraph::pickPoint() - Too many collisions to return to scripts!");
            break;
        }
    }

    // Clear Pick-List.
    object->clearPickList();

    // Return buffer.
    return pBuffer;
}
//-----------------------------------------------------------------------------
// Pick Point (C++).
// NOTE:-   Use "getPickList()" to retrieve picked objects.
//-----------------------------------------------------------------------------
U32 t2dSceneGraph::pickPoint( const t2dVector& pickPoint, const U32 groupMask, const U32 layerMask, const bool showInvisible, const t2dSceneObject* pIgnoreObject, bool ignorePhysics )
{
    // Clear Pick-List.
    clearPickList();

    // Pick Objects.
    const U32 pickCount = getSceneContainer().pickPoint( pickPoint, layerMask, groupMask, showInvisible, findLayeredObjectsCallback, &mLayeredPickList, pIgnoreObject, ignorePhysics );

    // Finish if no objects were picked.
    if ( pickCount == 0 )
        return 0;

    // At this point, objects are trivially selected but will require a further check to see if they are actually
    // overlapping the pick-point...

    // Step through layers and sort them.
    for ( S32 layer = 0; layer < t2dSceneGraph::maxLayersSupported ; layer++ )
    {
        // Fetch Layer Size.
        U32 layerSize = mLayeredPickList[layer].size();

        // Are there any objects to render in this layer?
        if ( layerSize > 0 )
        {
            // Are we using layer sorting?
            if ( getUseLayeredSorting() && layerSize > 1 )
                // Yes, so Quick-Sort the layer.
                dQsort(mLayeredPickList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredPickListSort);

            // Add sorted layer to linear list.
            for ( U32 n = 0; n < layerSize; n++ )
            {
                // Refine the search by doing a more accurate check for the selected point and
                // only add the objects that succeed.
                bool inObject = false;
                if (ignorePhysics)
                {
                    t2dSceneObject* object = mLayeredPickList[layer][n];
					RectF objectRect = object->getWorldClipRectangle();
					Point2F lowerRight = objectRect.point + objectRect.extent;
					if ((objectRect.point.x < pickPoint.mX) && (objectRect.point.y < pickPoint.mY) &&
						(lowerRight.x > pickPoint.mX) && (lowerRight.y > pickPoint.mY))
					{
						inObject = true;
					}
				}
                else
                  inObject = mLayeredPickList[layer][n]->getIsPointInObject( pickPoint );

                if ( inObject )
                    mLinearPickList.push_back( mLayeredPickList[layer][n] );
            }
        }
    }

    // Return Picked Object Count.
    return mLinearPickList.size();
}


//-----------------------------------------------------------------------------
// Pick Line.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, pickLine, const char*, 3, 10, "(startx/y, endx/y, [groupMask], [layerMask], [showInvisible?], [excludeObject] ) Picks objects intersecting line with optional group/layer masks.\n"
			  "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
			  "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
			  "@param groupMask Optional group mask.\n"
			  "@param layerMask Optional layer mask.\n"
			  "@param showInvisible Object visibility setting\n"
			  "@param excludeObject Object to exclude from check\n"
			  "@return Returns list of object IDs or NULL on failure")
{
   // Upper left and lower right bound.
   t2dVector v1, v2;
   // The index of the first optional parameter.
   U32 firstArg;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc < 9))
   {
       v1 = t2dSceneObject::getStringElementVector(argv[2]);
       v2 = t2dSceneObject::getStringElementVector(argv[2], 2);
       firstArg = 3;
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 10))
   {
      v1 = t2dSceneObject::getStringElementVector(argv[2]);
      v2 = t2dSceneObject::getStringElementVector(argv[3]);
      firstArg = 4;
   }
   
   // (x1, y1, x2, y2)
   else if (argc > 5)
   {
       v1 = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       v2 = t2dVector(dAtof(argv[4]), dAtof(argv[5]));
       firstArg = 6;
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneGraph::pickLine() - Invalid number of parameters!");
      return NULL;
   }

   // Calculate Group Mask.
   U32 groupMask = T2D_MASK_ALL;
   if (argc > firstArg)
      groupMask = dAtoi(argv[firstArg]);

   // Calculate Group Mask.
   U32 layerMask = T2D_MASK_ALL;
   if (argc > (firstArg + 1))
      layerMask = dAtoi(argv[firstArg + 1]);

   // Calculate Group Mask.
   bool showInvisible = false;
   if (argc > (firstArg + 2))
      showInvisible = dAtob(argv[firstArg + 2]);

    // Get Optional Exclude Object.
    t2dSceneObject* pSceneObject2D = NULL;
    if ( argc > (firstArg + 3) )
    {
        // Find t2dSceneObject Object.
        pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[firstArg + 3]));

        // Validate Object.
        if ( !pSceneObject2D )
        {
            Con::warnf("t2dSceneGraph::pickLine() - Couldn't find/Invalid object '%s'.", argv[firstArg + 3]);
            return NULL;
        }
    }

    // Pick Line.
    // NOTE:-   "pickLine" refines the search via the objects' physics so there's no need to search further!
    if ( object->pickLine(v1, v2, groupMask, layerMask, showInvisible, pSceneObject2D ) == 0 )
        // No Objects!
        return NULL;

    // Fetch Pick Vector.
    typeSceneObjectVectorConstRef pickVector = object->getPickList();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < pickVector.size(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", pickVector[n]->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneGraph::pickLine() - Too many collisions to return to scripts!");
            break;
        }
    }

    // Clear Pick-List.
    object->clearPickList();

    // Return buffer.
    return pBuffer;
}
//-----------------------------------------------------------------------------
// Pick Line (C++).
// NOTE:-   Use "getPickList()" to retrieve picked objects.
//-----------------------------------------------------------------------------
U32 t2dSceneGraph::pickLine( const t2dVector& startPickPoint, const t2dVector& endPickPoint, const U32 groupMask, const U32 layerMask, const bool showInvisible, const t2dSceneObject* pIgnoreObject )
{
    // Clear Pick-List.
    clearPickList();

    // Pick Objects.
    const U32 pickCount = getSceneContainer().pickLine( startPickPoint, endPickPoint, layerMask, groupMask, showInvisible, findObjectsCallback, &mLinearPickList, pIgnoreObject );

    // Sort Time-Keys if more than one object.
    if ( pickCount > 1 )
    {
        // Quick-Sort by the Collision Time.
        dQsort(mLinearPickList.address(), mLinearPickList.size(), sizeof(t2dSceneObject*), linearPickListSortKeyCollisionTime );
    }

    // Return Picked Object Count.
    return pickCount;
}


//-----------------------------------------------------------------------------
// Pick Rectangle.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, pickRect, const char*, 3, 10, "(startx/y, endx/y, [groupMask], [layerMask], [showInvisible?], [excludeObject] ) Picks objects intersecting rectangle with optional group/layer masks.\n"
			  "@param startx/y The coordinates of the start point as either (\"x y\") or (x,y)\n"
			  "@param endx/y The coordinates of the end point as either (\"x y\") or (x,y)\n"
			  "@param groupMask Optional group mask.\n"
			  "@param layerMask Optional layer mask.\n"
			  "@param showInvisible Object visibility setting\n"
			  "@param excludeObject Set whether to exclude object from list.\n"
			  "@return Returns list of object IDs or NULL on failure")
{
   // Upper left and lower right bound.
   t2dVector v1, v2;
   // The index of the first optional parameter.
   U32 firstArg;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = t2dSceneObject::getStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = t2dSceneObject::getStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc < 9))
   {
       v1 = t2dSceneObject::getStringElementVector(argv[2]);
       v2 = t2dSceneObject::getStringElementVector(argv[2], 2);
       firstArg = 3;
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 10))
   {
      v1 = t2dSceneObject::getStringElementVector(argv[2]);
      v2 = t2dSceneObject::getStringElementVector(argv[3]);
      firstArg = 4;
   }
   
   // (x1, y1, x2, y2)
   else if (argc > 5)
   {
       v1 = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       v2 = t2dVector(dAtof(argv[4]), dAtof(argv[5]));
       firstArg = 6;
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneGraph::pickRect() - Invalid number of parameters!");
      return NULL;
   }

   // Calculate Group Mask.
   U32 groupMask = T2D_MASK_ALL;
   if (argc > firstArg)
      groupMask = dAtoi(argv[firstArg]);

   // Calculate Group Mask.
   U32 layerMask = T2D_MASK_ALL;
   if (argc > (firstArg + 1))
      layerMask = dAtoi(argv[firstArg + 1]);

   // Calculate Group Mask.
   bool showInvisible = false;
   if (argc > (firstArg + 2))
      showInvisible = dAtob(argv[firstArg + 2]);

    // Get Optional Exclude Object.
    t2dSceneObject* pSceneObject2D = NULL;
    if ( argc > (firstArg + 3) )
    {
        // Find t2dSceneObject Object.
        pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[firstArg + 3]));

        // Validate Object.
        if ( !pSceneObject2D )
        {
            Con::warnf("t2dSceneGraph::pickRect() - Couldn't find/Invalid object '%s'.", argv[firstArg + 3]);
            return NULL;
        }
    }

    // Pick Rectangle.
    if ( object->pickRect(v1, v2, groupMask, layerMask, showInvisible, pSceneObject2D) == 0)
        // No Objects!
        return NULL;

    // Fetch Pick Vector.
    typeSceneObjectVectorConstRef pickVector = object->getPickList();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < pickVector.size(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", pickVector[n]->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneGraph::pickRect() - Too many collisions to return to scripts!");
            break;
        }
    }

    // Clear Pick-List.
    object->clearPickList();

    // Return buffer.
    return pBuffer;
}
//-----------------------------------------------------------------------------
// Pick Rect (C++).
// NOTE:-   Use "getPickList()" to retrieve picked objects.
//-----------------------------------------------------------------------------
U32 t2dSceneGraph::pickRect( const t2dVector& startPickPoint, const t2dVector& endPickPoint, const U32 groupMask, const U32 layerMask, const bool showInvisible, const t2dSceneObject* pIgnoreObject, bool ignorePhysics )
{
    // Clear Pick-List.
    clearPickList();

    // Renormalise Area.
    t2dVector normStartPickPoint = startPickPoint;
    t2dVector normEndPickPoint = endPickPoint;
    if ( startPickPoint.mX > endPickPoint.mX ) mSwap(normStartPickPoint.mX, normEndPickPoint.mX);
    if ( startPickPoint.mY > endPickPoint.mY ) mSwap(normStartPickPoint.mY, normEndPickPoint.mY);

    // Calculate Pick Area.
    const RectF pickRect(normStartPickPoint.mX, normStartPickPoint.mY, normEndPickPoint.mX-normStartPickPoint.mX, normEndPickPoint.mY-normStartPickPoint.mY);

    // Pick Objects.
    const U32 pickCount = getSceneContainer().pickArea( pickRect, layerMask, groupMask, showInvisible, findLayeredObjectsCallback, &mLayeredPickList, pIgnoreObject, ignorePhysics );

    // Finish if no objects were picked.
    if ( pickCount == 0 )
        return NULL;

    // Setup Physics to check against.
    t2dPhysics pickAreaPhysics;
    // Do a dummy initialisation.
    pickAreaPhysics.initialise( NULL, "" );
    // Setup the Position.
    pickAreaPhysics.setPosition( (normStartPickPoint + normEndPickPoint)/2.0f );
    // Set the poly count (in case we change the default in the future).
    pickAreaPhysics.setCollisionPolyPrimitive( 4 );
    // Setup the area.
    pickAreaPhysics.setSize( t2dVector(pickRect.len_x(), pickRect.len_y()) );

    // Dummy Collision Status.
    t2dPhysics::cCollisionStatus collisionStatus;

    // Setup Source Physics Mode.
    collisionStatus.mpSrcPhysics = &pickAreaPhysics;

    // Step through layers and sort them.
    for ( S32 layer = 0; layer < t2dSceneGraph::maxLayersSupported ; layer++ )
    {
        // Fetch Layer Size.
        U32 layerSize = mLayeredPickList[layer].size();

        // Are there any objects to render in this layer?
        if ( layerSize > 0 )
        {
            // Are we using layer sorting?
            if ( getUseLayeredSorting() && layerSize > 1 )
                // Yes, so Quick-Sort the layer.
                dQsort(mLayeredPickList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredPickListSort);

            // Add sorted layer to linear list.
            for ( U32 n = 0; n < layerSize; n++ )
            {
                // Refine the search by doing a more accurate check for the selected point and
                // only add the objects that succeed.
                bool inObject = true;
                if (ignorePhysics)
                {
                    // If we are ignoring physics, scope always objects will be picked regardless. We
                    // need to remedy that.
                    t2dSceneObject* object = mLayeredPickList[layer][n];
                    if (object->getIsAlwaysScope())
                       inObject = object->getWorldClipRectangle().intersect(pickRect);
                }
                else
                {
                   // Setup Destination Physics Mode.
                   collisionStatus.mpDstPhysics = &(mLayeredPickList[layer][n]->getParentPhysics());
                   // Perform collision check.
                   // NOTE:- We're using a small but finite time here at current max FPS limit.
                   inObject = t2dPhysics::calculateCollision( 1.0f/300.0f, &collisionStatus );
                }
                if ( inObject )
                    // Collisions!
                    mLinearPickList.push_back( mLayeredPickList[layer][n] );
            }
        }
    }

    // Return Picked Object Count.
    return mLinearPickList.size();
}


//-----------------------------------------------------------------------------
// Pick Radius.
//-----------------------------------------------------------------------------
ConsoleMethod(t2dSceneGraph, pickRadius, const char*, 4, 9, "(x / y, radius, [groupMask], [layerMask], [showInvisible?], [excludeObject] ) Picks objects intersecting circle with optional group/layer masks.\n"
			  "@param x/y The coordinates of the center point as either (\"x y\") or (x,y)\n"
			  "@param radius The radius of the circular region\n"
			  "@param groupMask Optional group mask.\n"
			  "@param layerMask Optional layer mask.\n"
			  "@param showInvisible Object visibility setting\n"
			  "@param excludeObject Set whether to exclude object from list.\n"
			  "@return Returns list of object IDs or NULL on failure")
{
   // The point.
   t2dVector point;
   // The radius.
   F32 radius;
   // The index of the first optional parameter.
   U32 firstArg;

   // Grab the number of elements in the first parameter.
   U32 elementCount = t2dSceneObject::getStringElementCount(argv[2]);

   // ("x y", radius)
   if ((elementCount == 2) && (argc < 9))
   {
      point = t2dSceneObject::getStringElementVector(argv[2]);
      radius = dAtof(argv[3]);
      firstArg = 4;
   }
   
   // (x, y, radius)
   else if ((elementCount == 1) && (argc > 4))
   {
       point = t2dVector(dAtof(argv[2]), dAtof(argv[3]));
       radius = dAtof(argv[4]);
       firstArg = 5;
   }
   
   // Invalid
   else
   {
      Con::warnf("t2dSceneGraph::pickLine() - Invalid number of parameters!");
      return NULL;
   }

   // Calculate Group Mask.
   U32 groupMask = T2D_MASK_ALL;
   if (argc > firstArg)
      groupMask = dAtoi(argv[firstArg]);

   // Calculate Group Mask.
   U32 layerMask = T2D_MASK_ALL;
   if (argc > (firstArg + 1))
      layerMask = dAtoi(argv[firstArg + 1]);

   // Calculate Group Mask.
   bool showInvisible = false;
   if (argc > (firstArg + 2))
      showInvisible = dAtob(argv[firstArg + 2]);

    t2dSceneObject* pSceneObject2D = NULL;
    if ( argc > firstArg + 3 )
    {
        // Find t2dSceneObject Object.
        pSceneObject2D = dynamic_cast<t2dSceneObject*>(Sim::findObject(argv[firstArg + 3]));

        // Validate Object.
        if ( !pSceneObject2D )
        {
            Con::warnf("t2dSceneGraph::pickRadius() - Couldn't find/Invalid object '%s'.", argv[firstArg + 3]);
            return NULL;
        }
    }

    // Pick Radius.
    if ( object->pickRadius( point, radius, groupMask, layerMask, showInvisible, pSceneObject2D ) == 0 )
    {
        // No Objects!
        return NULL;
    }

    // Fetch Pick Vector.
    typeSceneObjectVectorConstRef pickVector = object->getPickList();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);
    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < pickVector.size(); n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", pickVector[n]->getId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("t2dSceneGraph::pickRadius() - Too many collisions to return to scripts!");
            break;
        }
    }

    // Clear Pick-List.
    object->clearPickList();

    // Return buffer.
    return pBuffer;
}
//-----------------------------------------------------------------------------
// Pick Radius (C++).
// NOTE:-   Use "getPickList()" to retrieve picked objects.
//-----------------------------------------------------------------------------
U32 t2dSceneGraph::pickRadius( const t2dVector& pickPoint, const F32 radius, const U32 groupMask, const U32 layerMask, const bool showInvisible, const t2dSceneObject* pIgnoreObject )
{
    // Check Radii.
    if ( mLessThanOrEqual( radius, 0 ) )
    {
        // Warn.
        Con::warnf("t2dSceneGraph::pickRadius() - Radius must be greater than zero! (%f)", radius);
        return 0;
    }

    // Clear Pick-List.
    clearPickList();

    // Calculate Pick Area.
    const F32 diameter = radius * 2.0f;
    const RectF pickRect(pickPoint.mX-radius, pickPoint.mY-radius, diameter, diameter);

    // Pick Objects.
    const U32 pickCount = getSceneContainer().pickArea( pickRect, layerMask, groupMask, showInvisible, findLayeredObjectsCallback, &mLayeredPickList, pIgnoreObject );

    // Finish if no objects were picked.
    if ( pickCount == 0 )
        return 0;

    // Setup Physics to check against.
    t2dPhysics pickAreaPhysics;
    // Do a dummy initialisation.
    pickAreaPhysics.initialise( NULL, "" );
    // Setup the Position.
    pickAreaPhysics.setPosition( pickPoint );
    // Set Size.
    pickAreaPhysics.setSize( t2dVector(diameter,diameter) );
    // Use Circle Detection.
    pickAreaPhysics.setCollisionDetectionMode( t2dPhysics::T2D_DETECTION_CIRCLE_ONLY );
    // Set Circle Mode.
    // NOTE:-   We can use the stock circle detection system but we need to ensure that we're in subscribe circle mode!
    pickAreaPhysics.setCollisionCircleSuperscribed( false );

    // Dummy Collision Status.
    t2dPhysics::cCollisionStatus collisionStatus;

    // Setup Source Physics Mode.
    collisionStatus.mpSrcPhysics = &pickAreaPhysics;

    // Step through layers and sort them.
    for ( S32 layer = 0; layer < t2dSceneGraph::maxLayersSupported ; layer++ )
    {
        // Fetch Layer Size.
        U32 layerSize = mLayeredPickList[layer].size();

        // Are there any objects to render in this layer?
        if ( layerSize > 0 )
        {
            // Are we using layer sorting?
            if ( getUseLayeredSorting() && layerSize > 1 )
            {
                // Yes, so Quick-Sort the layer.
                dQsort(mLayeredPickList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredPickListSort);
            }

            // Add sorted layer to linear list.
            for ( U32 n = 0; n < layerSize; n++ )
            {
                // Refine the search by doing a more accurate check for the selected point and
                // only add the objects that succeed.

                // Setup Destination Physics Mode.
                collisionStatus.mpDstPhysics = &(mLayeredPickList[layer][n]->getParentPhysics());
                // Perform collision check.
                // NOTE:- We're using a small but finite time here at current max FPS limit.
                if ( t2dPhysics::calculateCollision( 1.0f/300.0f, &collisionStatus ) )
                {
                    // Collisions!
                    mLinearPickList.push_back( mLayeredPickList[layer][n] );
                }
            }
        }
    }

    // Return Picked Object Count.
    return mLinearPickList.size();
}


//-----------------------------------------------------------------------------
// Check Scene Object Bins.
//-----------------------------------------------------------------------------
void t2dSceneGraph::checkSceneObjectBins( t2dSceneObject* pSceneObject2D )
{
    // Ignore if disabled.
    if ( !pSceneObject2D->isEnabled() )
        return;

    // Get the Scene-Container to Check the Object Bins.
    mSceneContainer.checkSceneObjectBins( pSceneObject2D );
}


//-----------------------------------------------------------------------------
// Add Delete Request.
//-----------------------------------------------------------------------------
void t2dSceneGraph::addDeleteRequest( t2dSceneObject* pSceneObject2D )
{
    // Ignore if it's already being safe-deleted.
    if ( pSceneObject2D->isBeingDeleted() )
        return;

    // Populate Delete Request.
    tDeleteRequest deleteRequest = { pSceneObject2D->getId(), NULL, false };
    // Push Delete Request.
    mDeleteRequests.push_back( deleteRequest );

    // Flag Delete in Progress.
    pSceneObject2D->mBeingSafeDeleted = true;
}


//-----------------------------------------------------------------------------
// Process Delete Requests.
//
// NOTE:-   The t2dSceneObject contains a member which allows the actual
//          deletion to be deferred until the object is 'ready'.
//          For example, a t2dParticleEffect may want to be deleted only
//          when all particles have expired otherwise, particles would simply
//          disappear which would look bad!
//          When the original 'safeDelete' request comes in, the system allows
//          the object to decide how to proceed by the object itself overriding
//          the 'onSafeDelete' function, where it can control the safe-deletion
//          status appropriately.
//-----------------------------------------------------------------------------
void t2dSceneGraph::processDeleteRequests( const bool forceImmediate )
{
    // Ignore if there's no delete requests!
    if ( mDeleteRequests.size() == 0 )
        return;

    // Validate All Delete Requests.
    U32 safeDeleteReadyCount = 0;
    for ( U32 requestIndex = 0; requestIndex < mDeleteRequests.size(); )
    {
        // Fetch Reference to Delete Request.
        tDeleteRequest& deleteRequest = mDeleteRequests[requestIndex];

        // Fetch Object.
        // NOTE:- Let's be safer and check that it's definately a scene-object.
        t2dSceneObject* pSceneObject = dynamic_cast<t2dSceneObject*>( Sim::findObject( deleteRequest.mObjectId ) );

        // Does this object exist?
        if ( pSceneObject )
        {
            // Yes, so write object.
            deleteRequest.mpSceneObject = pSceneObject;

            // Calculate Safe-Ready Flag.
            deleteRequest.mSafeDeleteReady = forceImmediate || pSceneObject->getSafeDelete();

            // Is it ready to safe-delete?
            if ( deleteRequest.mSafeDeleteReady )
            {
                // Yes, so increase safe-ready count.
                ++safeDeleteReadyCount;
            }         
        }
        else
        {
            // No, so it looks like the object got deleted prematurely; let's just remove
            // the request instead.
            mDeleteRequests.erase_fast( requestIndex );
            
            // Repeat this item.
            continue;
        }

        // Skip to next request index.
        ++requestIndex;
    }

    // Stop if there's no delete requests!
    if ( mDeleteRequests.size() == 0 )
        return;

    // Transfer Delete-Requests to Temporary version.
    // NOTE:-   We do this because we may delete objects which have dependancies.  This would
    //          cause objects to be added to the safe-delete list.  We don't want to work on
    //          the list whilst this is happening so we'll transfer it to a temporary list.
    mDeleteRequestsTemp = mDeleteRequests;

    // Can we process all remaining delete-requests?
    if ( safeDeleteReadyCount == mDeleteRequestsTemp.size() )
    {
        // Yes, so process ALL safe-ready delete-requests.
        for ( U32 requestIndex = 0; requestIndex < mDeleteRequestsTemp.size(); ++requestIndex )
        {
            // Yes, so fetch object.
            t2dSceneObject* pSceneObject = mDeleteRequestsTemp[requestIndex].mpSceneObject;

            // Do script callback.
            //Con::executef(this, 2, "onSafeDelete", pSceneObject->getIdString() ); // RKS: I disabled this script call

            // Destroy the object.
            pSceneObject->deleteObject();
        }

        // Remove All delete-requests.
        mDeleteRequestsTemp.clear();
    }
    else
    {
        // No, so process only safe-ready delete-requests.
        for ( U32 requestIndex = 0; requestIndex < mDeleteRequestsTemp.size(); )
        {
            // Fetch Reference to Delete Request.
            tDeleteRequest& deleteRequest = mDeleteRequestsTemp[requestIndex];

            // Is the Object Safe-Ready?
            if ( deleteRequest.mSafeDeleteReady )
            {
                // Yes, so fetch object.
                t2dSceneObject* pSceneObject = deleteRequest.mpSceneObject;

                // Do script callback.
                //Con::executef(this, 2, "onSafeDelete", pSceneObject->getIdString() ); // RKS: I disabled this script call

                // Destroy the object.
                pSceneObject->deleteObject();

                // Quickly remove delete-request.
                mDeleteRequestsTemp.erase_fast( requestIndex );

                // Repeat this item.
                continue;
            }

            // Skip to next request index.
            ++requestIndex;
        }
    }
}



//-----------------------------------------------------------------------------
// Attach Scene Window.
//-----------------------------------------------------------------------------
void t2dSceneGraph::attachSceneWindow( t2dSceneWindow* pSceneWindow2D )
{
    // Ignore if already attached.
    if ( isSceneWindowAttached( pSceneWindow2D ) )
        return;

    // Add to Attached List.
    mAttachedSceneWindows.addObject( pSceneWindow2D );
}


//-----------------------------------------------------------------------------
// Detach Scene Window.
//-----------------------------------------------------------------------------
void t2dSceneGraph::detachSceneWindow( t2dSceneWindow* pSceneWindow2D )
{
    // Ignore if not attached.
    if ( !isSceneWindowAttached( pSceneWindow2D ) )
        return;

    // Add to Attached List.
    mAttachedSceneWindows.removeObject( pSceneWindow2D );
}


//-----------------------------------------------------------------------------
// Detach All Scene Windows.
//-----------------------------------------------------------------------------
void t2dSceneGraph::detachAllSceneWindows( void )
{
    // Detach All Scene Windows.
    while( mAttachedSceneWindows.size() > 0 )
        dynamic_cast<t2dSceneWindow*>(mAttachedSceneWindows[mAttachedSceneWindows.size()-1])->resetSceneGraph();
}


//-----------------------------------------------------------------------------
// Is Scene Window Attached?
//-----------------------------------------------------------------------------
bool t2dSceneGraph::isSceneWindowAttached( t2dSceneWindow* pSceneWindow2D )
{
    for( SimSet::iterator itr = mAttachedSceneWindows.begin(); itr != mAttachedSceneWindows.end(); itr++ )
        if ( pSceneWindow2D == dynamic_cast<t2dSceneWindow*>(*itr) )
            // Found.
            return true;

    // Not Found.
    return false;
}


//--------------------------------------------------------------------------------
// Calculate Debug Stats.
//--------------------------------------------------------------------------------
void t2dSceneGraph::calculateDebugStats( void )
{
    // Calculate 'Max' Stats.

    // Relocations...
    if ( mDebugStats.objectBinRelocations > mMaxObjectBinRelocations )
        mMaxObjectBinRelocations = mDebugStats.objectBinRelocations;

    // Collisions.
    if ( mDebugStats.objectBinCollisions > mMaxObjectBinCollisions )
        mMaxObjectBinCollisions = mDebugStats.objectBinCollisions;
}


//-----------------------------------------------------------------------------
// Process Tick.
//-----------------------------------------------------------------------------
void t2dSceneGraph::processTick( void )
{
    // Process Delete Requests.
    processDeleteRequests(false);

    // Reset Debug Stats.
    mDebugStats.reset();

    // Finish if scene is paused.
    if ( getScenePause() ) return;

#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneGraph_processTick);
#endif

    // Debug Status Reference.
    CDebugStats* pDebugStats = &mDebugStats;

    // Update Graph Time.
    mSceneTime += ITickable::smTickSec;

    // Only Process if we've got objects in the scene!
    if ( getSceneObjectCount() > 0 )
    {
        // Increment Update Sequence.
        nextUpdateSequence();

        // Fetch First Object.
        t2dSceneObject* pSceneObject2D = getProcessHead()->getProcessNext();

        // ****************************************************
        // Pre-Integrate Stage.
        // ****************************************************
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneGraph_preIntegrate);
#endif

       // Pre integration is a physics processing step, so editor scenes need not perform it.
       if ( !getIsEditorScene() )
       {
           while ( pSceneObject2D != getProcessHead() )
           {
               // Is the object enabled/not-paused/not being deleted?
               // NOTE:-    We also need to check if we're disabled but owned by a mount that is now enabled.
               //           We also need to check if we're paused but owned by a mount that is now not-paused.
               if ( !pSceneObject2D->getInitialUpdate() ||
                   ( pSceneObject2D->isBeingDeleted() && !pSceneObject2D->getSafeDelete() ) ||
                   ( !pSceneObject2D->isBeingDeleted() &&
                   (pSceneObject2D->isEnabled() || (pSceneObject2D->getIsMounted() && pSceneObject2D->mMountOwned && pSceneObject2D->getProcessMount()->isEnabled()) ) &&
                   (!pSceneObject2D->getPaused() || (pSceneObject2D->getIsMounted() && pSceneObject2D->mMountOwned && !pSceneObject2D->getProcessMount()->getPaused()) ) ) )
               {
                   // Pre-Inegrate.
                   pSceneObject2D->preIntegrate( mSceneTime, ITickable::smTickSec, pDebugStats );
               }

               // Fetch Next Object.
               pSceneObject2D = pSceneObject2D->getProcessNext();
           }

           // Reset to first object.
           pSceneObject2D = pSceneObject2D->getProcessNext();
       }

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneGraph_preIntegrate
#endif


        // ****************************************************
        // Integrate Object Stage.
        // ****************************************************
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneGraph_integrateObject);
#endif

        while ( pSceneObject2D != getProcessHead() )
        {
           // Is the object enabled/not-paused/not being deleted?
           // NOTE:-    We also need to check if we're disabled but owned by a mount that is now enabled.
           //           We also need to check if we're paused but owned by a mount that is now not-paused.

			//Luma: attempt to unroll this in order to optimise as this logic seems to be quite slow in profiles... just due to number of objects to update possibly?
			t2dSceneObject	*psProcessObject = pSceneObject2D;
            pSceneObject2D = pSceneObject2D->getProcessNext();

            // Post-Update.
			if(!psProcessObject->getInitialUpdate())
			{
				psProcessObject->integrateObject( mSceneTime, ITickable::smTickSec, pDebugStats );
			}
			else if(psProcessObject->isBeingDeleted())
			{
				if(!psProcessObject->getSafeDelete())
				{
					psProcessObject->integrateObject( mSceneTime, ITickable::smTickSec, pDebugStats );
				}
			}
			else
			{
				bool	bMountOwned = psProcessObject->getIsMounted() && psProcessObject->mMountOwned;
				t2dSceneObject	*psProcessMount = psProcessObject->getProcessMount();
				if((psProcessObject->isEnabled() || (bMountOwned && psProcessMount->isEnabled())) && (!psProcessObject->getPaused() || (bMountOwned && !psProcessMount->getPaused()) ) )
				{
					psProcessObject->integrateObject( mSceneTime, ITickable::smTickSec, pDebugStats );
				}
            }          
		}

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneGraph_integrateObject
#endif

        // ****************************************************
        // Post-Integrate Stage.
        // ****************************************************
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneGraph_postIntegrate);
#endif

        // Fetch Next Object.
        pSceneObject2D = pSceneObject2D->getProcessNext();

        if (!getIsEditorScene())
        {
           while ( pSceneObject2D != getProcessHead() )
           {
               // Is the object enabled/not-paused/not being deleted?
               // NOTE:-    We also need to check if we're disabled but owned by a mount that is now enabled.
               //           We also need to check if we're paused but owned by a mount that is now not-paused.
               if ( !pSceneObject2D->getInitialUpdate() ||
                   ( pSceneObject2D->isBeingDeleted() && !pSceneObject2D->getSafeDelete() ) ||
                   ( !pSceneObject2D->isBeingDeleted() &&
                   (pSceneObject2D->isEnabled() || (pSceneObject2D->getIsMounted() && pSceneObject2D->mMountOwned && pSceneObject2D->getProcessMount()->isEnabled()) ) &&
                   (!pSceneObject2D->getPaused() || (pSceneObject2D->getIsMounted() && pSceneObject2D->mMountOwned && !pSceneObject2D->getProcessMount()->getPaused()) ) ) )
               {
                   // Pre-Inegrate.
                   pSceneObject2D->postIntegrate( mSceneTime, ITickable::smTickSec, pDebugStats );
               }

               // Fetch Next Object.
               pSceneObject2D = pSceneObject2D->getProcessNext();
           }
        }

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneGraph_postIntegrate
#endif

    }

    // Calculate Collision Hit Percentage ( if any ).
    if ( mDebugStats.objectPotentialCollisions != 0 )
        mDebugStats.objectHitPercCollisions = mDebugStats.objectActualCollisions * ( 100.0f / mDebugStats.objectPotentialCollisions  );
    else
        mDebugStats.objectHitPercCollisions = 100.0f;

    // Update Debug Stats.
    calculateDebugStats();


#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneGraph_onUpdateSceneTick);
#endif

	if( mUseUpdateCallback ) {
    // Expose a kind of main-loop to scripts.
		if (!getIsEditorScene()) {
		   Con::executef( this, 1, "onUpdateSceneTick" );
		}
	}

#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();  //T2D_t2dSceneGraph_onUpdateSceneTick
#endif


#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneGraph_processTick
#endif
}


//-----------------------------------------------------------------------------
// Interpolate Tick.
//-----------------------------------------------------------------------------
void t2dSceneGraph::interpolateTick( F32 timeDelta )
{
    // Finish if scene is paused.
    if ( getScenePause() ) return;

#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneGraph_interpolateTick);
#endif

    // Fetch First Object.
    t2dSceneObject* pSceneObject2D = getProcessHead()->getProcessNext();

    // Advance All Objects.
    while ( pSceneObject2D != getProcessHead() )
    {
       // Is the object enabled/not-paused/not being deleted?
       if ( pSceneObject2D->isEnabled() &&
            !pSceneObject2D->getPaused() &&
            !pSceneObject2D->isBeingDeleted() )
        {
            // Post-Update.
            pSceneObject2D->interpolateTick( timeDelta );
        }   

       // Fetch Next Object.
       pSceneObject2D = pSceneObject2D->getProcessNext();
    }

#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();   // T2D_t2dSceneGraph_interpolateTick
#endif
}



//-----------------------------------------------------------------------------
// Render View.
//-----------------------------------------------------------------------------
void t2dSceneGraph::renderView( const RectF viewWindow, const U32 layerMask, const U32 groupMask, CDebugStats* pDebugStats )
{

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneGraph_renderView);
#endif

#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_START(T2D_t2dSceneGraph_onUpdateSceneScript);
#endif

	if( mUseUpdateCallback ) {
    // Expose a kind of main-loop to scripts.
    if (!getIsEditorScene())
       Con::executef( this, 1, "onUpdateScene" );
	}

#ifdef TORQUE_ENABLE_PROFILER
    PROFILE_END();  //T2D_t2dSceneGraph_onUpdateSceneScript
#endif

    // Reset Render Stats.
    pDebugStats->objectsLayerSorted = 0;
    pDebugStats->objectsPotentialRender = 0;
    pDebugStats->objectsActualRendered = 0;

    // Clear Layered Render-List.
    for ( U32 n = 0; n < t2dSceneGraph::maxLayersSupported; n++ )
        mLayeredRenderList[n].clear();

    // Find Objects in View Window.
    if ( mSceneContainer.findObjects( viewWindow, layerMask, groupMask, false, false, findLayeredObjectsCallback, &mLayeredRenderList, NULL ) > 0 )
    {
        // Step through layers.
        for ( S32 layer = t2dSceneGraph::maxLayersSupported-1; layer >= 0 ; layer-- )
        {
            // Fetch Layer Size.
            const U32 layerSize = mLayeredRenderList[layer].size();

            // Are there any objects to render in this layer?
            if ( layerSize > 0 )
            {
                // Are we using layer sorting?
                if ( mUseLayerSorting && layerSize > 1 )
                {
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneGraph_layerSorting);
#endif
                    // Yes, so Quick-Sort the layer.
                    const eDrawOrderSort& mode = mLayerSortModes[layer];
                    switch( mode )
                    {
                        case T2D_SORT_YAXIS:
                            dQsort(mLayeredRenderList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredYSortPointSort);
                            break;

                        case T2D_SORT_XAXIS:
                            dQsort(mLayeredRenderList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredXSortPointSort);
                            break;

                        case T2D_SORT_NEGYAXIS:
                            dQsort(mLayeredRenderList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredNegYSortPointSort);
                            break;

                        case T2D_SORT_NEGXAXIS:
                            dQsort(mLayeredRenderList[layer].address(), layerSize, sizeof(t2dSceneObject*), layeredNegXSortPointSort);
                            break;

                        case T2D_SORT_NORMAL:
                        default:
                            dQsort(mLayeredRenderList[layer].address(), layerSize, sizeof(t2dSceneObject*), mRenderSortFn);
                    };

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneGraph_layerSorting
#endif
                    // Increase Layer Sorts.
                    pDebugStats->objectsLayerSorted += layerSize;
                }

                // Viewport/Object View Intersection.
                RectF viewIntersection;

                // Yes, so step through objects.
                for (   typeSceneObjectVector::iterator itr = mLayeredRenderList[layer].begin();
                        itr < mLayeredRenderList[layer].end();
                        itr++ )
                {
                    // Increase Potential Renders.
                    pDebugStats->objectsPotentialRender++;

                    // Fetch more direct reference.
                    t2dSceneObject* pSceneObject2D = (*itr);

                    // Is the object in view or always scoped?
                    //
                    // NOTE:-   Object must have been initially updated.
                    if ( pSceneObject2D->getInitialUpdate() && (pSceneObject2D->getIsAlwaysScope() || pSceneObject2D->getIsInViewport(viewWindow, viewIntersection)) )
                    {                      
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_START(T2D_t2dSceneGraph_renderObjects);
#endif
                        // Yes, so render Object.
                        pSceneObject2D->renderObject( viewWindow, viewIntersection );
#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneGraph_renderObjects
#endif
                        // Increase Actual Renders.
                        pDebugStats->objectsActualRendered++;
                    }

                }
            }
        }

        // Reset Blend Options.
        t2dSceneObject::resetBlendOptions();
    }

    // Calculate Render Hit Percentage ( if any ).
    if ( pDebugStats->objectsPotentialRender != 0 )
        pDebugStats->objectsHitPercRendered = pDebugStats->objectsActualRendered * ( 100.0f / pDebugStats->objectsPotentialRender  );
    else
        pDebugStats->objectsHitPercRendered = 100.0f;

#ifdef TORQUE_ENABLE_PROFILER
        PROFILE_END();   // T2D_t2dSceneGraph_renderView
#endif
}

//-----------------------------------------------------------------------------
// Serialisation.
//-----------------------------------------------------------------------------

// Register Handlers.
REGISTER_SERIALISE_START( t2dSceneGraph )
    REGISTER_SERIALISE_VERSION( t2dSceneGraph, 2, false )
    REGISTER_SERIALISE_VERSION( t2dSceneGraph, 3, false )
    REGISTER_SERIALISE_END()

// Implement Leaf Serialisation.
IMPLEMENT_T2D_SERIALISE_LEAF( t2dSceneGraph, 3 )


//-----------------------------------------------------------------------------
// Load v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dSceneGraph, 2 )
{
    F32                         binSize;
    U32                         binCount;
    bool                        useLayerSorting;
    bool                        lastInFrontSorting;
    bool                        scenePause;
    t2dVector                   constantForce;
    bool                        graviticConstantForce;
    F32                         scenePhysicsLimitFPS;
    F32                         scenePhysicsTargetFPS;
    U32                         scenePhysicsMaxIterations;

    // Object Info.
    if  (   !stream.read( &binSize ) ||
            !stream.read( &binCount ) ||
            !stream.read( &useLayerSorting ) ||
            !stream.read( &lastInFrontSorting ) ||
            !stream.read( &object->mLayerOrder ) ||
            !stream.read( &object->mSceneTime ) ||
            !stream.read( &scenePause ) ||
            !stream.read( &constantForce.mX ) ||
            !stream.read( &constantForce.mY ) ||
            !stream.read( &graviticConstantForce ) ||
            !stream.read( &scenePhysicsLimitFPS ) ||
            !stream.read( &scenePhysicsTargetFPS ) ||
            !stream.read( &scenePhysicsMaxIterations ) )
        return false;

    // Initialise Scene Graph.
    object->initialise( binSize, binCount, useLayerSorting, lastInFrontSorting );

    // Set Scene Pause.
    object->setScenePause( scenePause );

    // Set Scene Constant Force.
    object->setConstantForce( constantForce, graviticConstantForce );

    // NOTE:-   The following calls are now obsolete but left here purely for information.
    // Set Physics Simulation Constraints.
    //object->setScenePhysicsLimitFPS( scenePhysicsLimitFPS );
    //object->setScenePhysicsTargetFPS( scenePhysicsTargetFPS );
    //object->setScenePhysicsMaxIterations( scenePhysicsMaxIterations );

    // Read object count.
    U32 objectsToLoad;
    if ( !stream.read( &objectsToLoad ) )
        return false;
	
	// RKS: This is to see if I can get a loading bar working
	dPrintf( " t2dSceneGraph - LOAD_METHOD v2 -- objectsToLoad: %u\n", objectsToLoad );

    if ( objectsToLoad > 0 )
    {
        // Read Objects.
        for ( U32 n = 0; n < objectsToLoad; n++ )
        {
            // Read Object Classname.
            char className[256];
            stream.readString( className );

            // Create Scene Object.
            t2dSceneObject* pSceneObject2D = (t2dSceneObject*)ConsoleObject::create( className );

            // Valid Scene-Object?
            if ( !pSceneObject2D )
            {
                // No, so warn.
                Con::warnf("t2dSceneGraph::loadScene() - Could not create Classname '%s' for Scene-Load.", className);
                // Clear the Scene.
                object->clearScene();
                // Return Error.
                return false;
            }

            // Register Object.
            if ( !pSceneObject2D->registerObject() )
            {
                // Warn.
                Con::warnf("t2dSceneGraph::loadScene() - Could not register object-class '%s' for Scene Load.", className);
                // Destroy Object.
                delete pSceneObject2D;
                // Clear the Scene.
                object->clearScene();
                // Return Error.
                return false;
            }

            // Add it to Scene Graph.
            object->addToScene( pSceneObject2D );

            // Add Object to Object Reference List.
            ObjReferenceList.push_back( pSceneObject2D );

            // Do Object Self-Serialisation.
            if ( !pSceneObject2D->loadStream( stream, object, ObjReferenceList, false ) )
            {
                // Warn.
                Con::warnf("t2dSceneGraph::loadScene() - Could not read object-class '%s' for Scene-Load.", className);
                // Destroy Object.
                pSceneObject2D->deleteObject();
                // Clear the Scene.
                object->clearScene();
                // Return Error.
                return false;
            }
        }
    }

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v2
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dSceneGraph, 2 )
{
    // Object Info.
    if  (   !stream.write( object->mSceneContainer.getBinSize() ) ||
            !stream.write( object->mSceneContainer.getBinCount() ) ||
            !stream.write( object->mUseLayerSorting ) ||
            !stream.write( object->mLastInFrontSorting ) ||
            !stream.write( object->mLayerOrder ) ||
            !stream.write( object->getSceneTime() ) ||
            !stream.write( object->mScenePause ) ||
            !stream.write( object->mConstantForce.mX ) ||
            !stream.write( object->mConstantForce.mY ) ||
            !stream.write( object->mGraviticConstantForce ) ||
            // NOTE:-   The following calls are now obsolete but left here purely for information.
            !stream.write( 0.0f ) ||
            !stream.write( 0.0f ) ||
            !stream.write( S32(1) ) )
        return false;

    // Calculate Objects to Save.
    const U32 objectsToSave = object->getSceneObjectCount() - object->getChildCount();

    // Write Objects to Save.
    if ( !stream.write( objectsToSave ) )
        // Return Error.
        return false;

    // Fetch First Object.
    t2dSceneObject* pSceneObject2D = object->getProcessHead();

    // Process Objects until all actioned.
    while ( 1 )
    {
        // Fetch Next Object.
        pSceneObject2D = pSceneObject2D->getProcessNext();

        // Finish if we've actioned everything.
        if ( pSceneObject2D == object->getProcessHead() )
            break;

        // Is this a child object?
        if ( !pSceneObject2D->getIsChild() )
        {
            // No, so Save ClassName.
            stream.writeString( pSceneObject2D->getClassName() );

            // No, so do Object Self-Serialisation.
            if ( !pSceneObject2D->saveStream( stream, object->getCurrentSerialiseID(), serialiseKey ) )
            {
                // Warn.
                Con::warnf("t2dSceneGraph::saveScene() - Could not Write Object Class '%s'(%d) to File for Scene-Save.", pSceneObject2D->getClassName(), pSceneObject2D->getId());
                // Return Error.
                return false;
            }

            // Next Serialise Key.
            serialiseKey++;
        }
    };

    // Return Okay.
    return true;
}


//-----------------------------------------------------------------------------
// Load v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_LOAD_METHOD( t2dSceneGraph, 3 )
{
    F32                         binSize;
    U32                         binCount;
    bool                        useLayerSorting;
    bool                        lastInFrontSorting;
    bool                        scenePause;
    t2dVector                   constantForce;
    bool                        graviticConstantForce;

    // Object Info.
    if  (   !stream.read( &binSize ) ||
        !stream.read( &binCount ) ||
        !stream.read( &useLayerSorting ) ||
        !stream.read( &lastInFrontSorting ) ||
        !stream.read( &object->mLayerOrder ) ||
        !stream.read( &object->mSceneTime ) ||
        !stream.read( &scenePause ) ||
        !stream.read( &constantForce.mX ) ||
        !stream.read( &constantForce.mY ) ||
        !stream.read( &graviticConstantForce ) )
        return false;

    // Initialise Scene Graph.
    object->initialise( binSize, binCount, useLayerSorting, lastInFrontSorting );

    // Set Scene Pause.
    object->setScenePause( scenePause );

    // Set Scene Constant Force.
    object->setConstantForce( constantForce, graviticConstantForce );

    // Read object count.
    U32 objectsToLoad;
    if ( !stream.read( &objectsToLoad ) )
        return false;
	
	// RKS: This is to see if I can get a loading bar working
	dPrintf( " t2dSceneGraph - LOAD_METHOD v3 -- objectsToLoad: %u\n", objectsToLoad );

    if ( objectsToLoad > 0 )
    {
        // Read Objects.
        for ( U32 n = 0; n < objectsToLoad; n++ )
        {
            // Read Object Classname.
            char className[256];
            stream.readString( className );

            // Create Scene Object.
            t2dSceneObject* pSceneObject2D = (t2dSceneObject*)ConsoleObject::create( className );

            // Valid Scene-Object?
            if ( !pSceneObject2D )
            {
                // No, so warn.
                Con::warnf("t2dSceneGraph::loadScene() - Could not create Classname '%s' for Scene-Load.", className);
                // Clear the Scene.
                object->clearScene();
                // Return Error.
                return false;
            }

            // Register Object.
            if ( !pSceneObject2D->registerObject() )
            {
                // Warn.
                Con::warnf("t2dSceneGraph::loadScene() - Could not register object-class '%s' for Scene Load.", className);
                // Destroy Object.
                delete pSceneObject2D;
                // Clear the Scene.
                object->clearScene();
                // Return Error.
                return false;
            }

            // Add it to Scene Graph.
            object->addToScene( pSceneObject2D );

            // Add Object to Object Reference List.
            ObjReferenceList.push_back( pSceneObject2D );

            // Do Object Self-Serialisation.
            if ( !pSceneObject2D->loadStream( stream, object, ObjReferenceList, false ) )
            {
                // Warn.
                Con::warnf("t2dSceneGraph::loadScene() - Could not read object-class '%s' for Scene-Load.", className);
                // Destroy Object.
                pSceneObject2D->deleteObject();
                // Clear the Scene.
                object->clearScene();
                // Return Error.
                return false;
            }
        }
    }

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------
// Save v3
//-----------------------------------------------------------------------------
IMPLEMENT_T2D_SAVE_METHOD( t2dSceneGraph, 3 )
{
    // Object Info.
    if  (   !stream.write( object->mSceneContainer.getBinSize() ) ||
        !stream.write( object->mSceneContainer.getBinCount() ) ||
        !stream.write( object->mUseLayerSorting ) ||
        !stream.write( object->mLastInFrontSorting ) ||
        !stream.write( object->mLayerOrder ) ||
        !stream.write( object->getSceneTime() ) ||
        !stream.write( object->mScenePause ) ||
        !stream.write( object->mConstantForce.mX ) ||
        !stream.write( object->mConstantForce.mY ) ||
        !stream.write( object->mGraviticConstantForce ) )
        return false;

    // Calculate Objects to Save.
    const U32 objectsToSave = object->getSceneObjectCount() - object->getChildCount();

    // Write Objects to Save.
    if ( !stream.write( objectsToSave ) )
        // Return Error.
        return false;

    // Fetch First Object.
    t2dSceneObject* pSceneObject2D = object->getProcessHead();

    // Process Objects until all actioned.
    while ( 1 )
    {
        // Fetch Next Object.
        pSceneObject2D = pSceneObject2D->getProcessNext();

        // Finish if we've actioned everything.
        if ( pSceneObject2D == object->getProcessHead() )
            break;

        // Is this a child object?
        if ( !pSceneObject2D->getIsChild() )
        {
            // No, so Save ClassName.
            stream.writeString( pSceneObject2D->getClassName() );

            // No, so do Object Self-Serialisation.
            if ( !pSceneObject2D->saveStream( stream, object->getCurrentSerialiseID(), serialiseKey ) )
            {
                // Warn.
                Con::warnf("t2dSceneGraph::saveScene() - Could not Write Object Class '%s'(%d) to File for Scene-Save.", pSceneObject2D->getClassName(), pSceneObject2D->getId());
                // Return Error.
                return false;
            }

            // Next Serialise Key.
            serialiseKey++;
        }
    };

    // Return Okay.
    return true;
}



void t2dSceneGraph::write(Stream &stream, U32 tabStop, U32 flags)
{
   // Don't save the global tile map.
   if (getGlobalTileMap())
      removeObject(getGlobalTileMap());

   // Set a mount id on all objects.
   U32 mountID = 1;
   t2dSceneObject* pSceneObject2D = getProcessHead()->getProcessNext();
   while ( pSceneObject2D != getProcessHead() )
   {
      char idString[8];
      dSprintf(idString, 8, "%d", mountID);
      pSceneObject2D->setDataField(StringTable->insert("mountID"), NULL, idString);
      pSceneObject2D = pSceneObject2D->getProcessNext();
      mountID++;
   }

   Parent::write(stream, tabStop, flags);
   
   pSceneObject2D = getProcessHead()->getProcessNext();
   while ( pSceneObject2D != getProcessHead() )
   {
      pSceneObject2D->setDataField(StringTable->insert("mountID"), NULL, "");
      pSceneObject2D->setDataField(StringTable->insert("mountToID"), NULL, "");
      pSceneObject2D = pSceneObject2D->getProcessNext();
   }

   if (getGlobalTileMap())
      addObject(getGlobalTileMap());
}

ConsoleMethod(t2dSceneGraph, performPostInit, void, 2, 2, "() Performs mount and other post initialization tasks\n"
			  "@return No return value.")
{
   object->performPostInit();
}

void t2dSceneGraph::performPostInit()
{
   // We can't just loop through the scene objects based on process order because mounting will
   // disrupt it. So, we have to grab all the objects first, and then process each one.
   Vector<t2dSceneObject*> sceneObjectList;
   sceneObjectList.reserve(getSceneObjectCount());
   t2dSceneObject* pSceneObject2D = getProcessHead()->getProcessNext();
   while ( pSceneObject2D != getProcessHead() )
   {
      sceneObjectList.push_back(pSceneObject2D);
      pSceneObject2D = pSceneObject2D->getProcessNext();
   }

	// RKS NOTE: I use the uNumSceneObjects variable so I can iterate through the objects and call PostInit()
	U32 uNumSceneObjects = sceneObjectList.size();
   //for (U32 i = 0; i < sceneObjectList.size(); i++)
	for (U32 i = 0; i < uNumSceneObjects; i++)
      sceneObjectList[i]->readFieldsPostLoad();

   // We have to clear the mountID field on all objects when we're done with it so as
   // to not interfere with addToLevel situations.
   pSceneObject2D = getProcessHead()->getProcessNext();
   while ( pSceneObject2D != getProcessHead() )
   {
      pSceneObject2D->setDataField(StringTable->insert("mountID"), NULL, "");
      pSceneObject2D->setDataField(StringTable->insert("mountToID"), NULL, "");
      pSceneObject2D = pSceneObject2D->getProcessNext();
   }
	
	// RKS NOTE: I added this so we can do a PostInit on all objects and their components
	uNumSceneObjects = sceneObjectList.size();
	for( U32 i = 0; i < uNumSceneObjects; ++i )
	{
		// Perform post initialization on the scene objects
		pSceneObject2D = sceneObjectList[i];
		if( pSceneObject2D )
			pSceneObject2D->OnPostInit();
	}
	
	CComponentGlobals::GetInstance().OnPostInit();
	CComponentLandBrick::AddLandBrickDataToPathGrid();
	
	// Pair a Double Brick with a Land Brick.  This needs to be done here as there is no guarantee that the Land Brick data will be set up before the Double Bricks
	CComponentDoubleBrick::SetLandBricksForAllDoubleBricks();
	
	//CSoundManager::Initialize();

   gLoadingSceneGraph = NULL;
}
