//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Base Datablock Object.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "./t2dBaseDatablock.h"
#include "./t2dSceneObject.h"

//------------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(t2dBaseDatablock);

//------------------------------------------------------------------------------

// Datablock Set.
// NOTE:-   We use this to enumerate available T2D datablocks.
SimSet  datablockSet2D;


//------------------------------------------------------------------------------
// Get datablock Set for T2D.
//------------------------------------------------------------------------------
ConsoleFunction( getT2DDatablockSet, S32, 1, 1, "() Retrieves the datablock-set for T2D."
				"@return Returns the datablockset ID as an integer")
{
    // Return Datablock Set.
    return datablockSet2D.getId();
}
// Get datablock Set for T2D.
SimSet* t2dBaseDatablock::get2DDatablockSet()
{
   return &datablockSet2D;
}



//------------------------------------------------------------------------------
// Initialise the Sanity ID.
//------------------------------------------------------------------------------
t2dBaseDatablock::t2dBaseDatablock() : mValid(false)
{
   mDefaultConfigDatablock = StringTable->insert("");
}

void t2dBaseDatablock::initPersistFields()
{
   addField("defaultConfig",   TypeString, Offset(mDefaultConfigDatablock,   t2dBaseDatablock));
}

//------------------------------------------------------------------------------
// On Add.
//------------------------------------------------------------------------------
bool t2dBaseDatablock::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Add Datablock-Set if not added.
    if ( !datablockSet2D.isProperlyAdded() )
        datablockSet2D.registerObject();

    // Add ourselves to datablock-set.
    datablockSet2D.addObject( this );

    // Validate Datablock.
    setIsValid(true);

    // Return Okay.
    return true;
}


//------------------------------------------------------------------------------
// On Remove.
//------------------------------------------------------------------------------
void t2dBaseDatablock::onRemove()
{
    // Call Parent.
    Parent::onRemove();

    // Remove ourselves from datablock-set.
    datablockSet2D.removeObject( this );

    // Remove Datablock-Set if empty and added.
    if ( datablockSet2D.size() == 0 && datablockSet2D.isProperlyAdded() )
        datablockSet2D.unregisterObject();

    // Invalidate Datablock.
    setIsValid(false);
}

ConsoleMethod(t2dBaseDatablock, setDefaultConfigDatablock, void, 3, 3, "(block) Sets the datablock to be applied to objects using this image map."
			  "@param block The block you wish to set."
			  "@return No return value.")
{
   object->setDefaultConfigDatablock(argv[2]);
}

ConsoleMethod(t2dBaseDatablock, getDefaultConfigDatablock, const char*, 2, 2, "() Gets the datablock being applied to objects using this image map."
			  "@return Returns the block as a string.")
{
   return object->getDefaultConfigDatablock();
}

void t2dBaseDatablock::setDefaultConfigDatablock(const char* config)
{
   mDefaultConfigDatablock = StringTable->insert(config);
}

StringTableEntry t2dBaseDatablock::getDefaultConfigDatablock()
{
   return mDefaultConfigDatablock;
}


//------------------------------------------------------------------------------
// Get Datablock Class Name.
//------------------------------------------------------------------------------
ConsoleMethod(t2dBaseDatablock, getDatablockClassName, const char*, 2, 2, "() Gets datablocks unique classname.\n"
			  "@return The class name as a string.")
{
    return object->getClassName();
}

ConsoleMethod(t2dBaseDatablock, getIsValid, bool, 2, 2, "() \n @return Returns a boolean value for whether or not the datablock is valid")
{
   return object->getIsValid();
}


//------------------------------------------------------------------------------
// Register t2d datablock type
//------------------------------------------------------------------------------

IMPLEMENT_CONSOLETYPE(t2dBaseDatablock)
IMPLEMENT_GETDATATYPE(t2dBaseDatablock)
IMPLEMENT_SETDATATYPE(t2dBaseDatablock)
