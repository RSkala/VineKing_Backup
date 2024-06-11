//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// Base Datablock Object.
//-----------------------------------------------------------------------------

#ifndef _T2DBASEDATABLOCK_H_
#define _T2DBASEDATABLOCK_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif


///-----------------------------------------------------------------------------
/// Base Datablock 2D.
///
/// This serves as the root for all T2D datablocks.  All T2D datablocks should
/// inherit from this and use the validity checking provided.
//-----------------------------------------------------------------------------
class t2dBaseDatablock : public SimDataBlock
{
private:
    typedef SimDataBlock Parent;

    /// Validity.
    bool    mValid;

    // Default configuration datablock - set for any object this datablock is applied to.
    StringTableEntry mDefaultConfigDatablock;

protected:
   // This holds the fields that were actually specified in the datablock. It is updated
   // by onStaticModified. This list is maintained so that only properties of a scene object
   // whose corresponding datablock entry has been specified will be changed.
   Vector<StringTableEntry> mSpecifiedFields;

public:
    /// Members.
    t2dBaseDatablock();
    virtual ~t2dBaseDatablock() {};
    virtual bool onAdd();
    virtual void onRemove();

    /// Validity Check.
    bool getIsValid(void) const { return mValid; };
    void setIsValid(bool status) { mValid = status; };

    // Retrieve Datablock Set.
    static SimSet* get2DDatablockSet();

    void setDefaultConfigDatablock(const char* config);
    StringTableEntry getDefaultConfigDatablock();

    static void initPersistFields();

    /// Declare Console Object.
    DECLARE_CONOBJECT(t2dBaseDatablock);
};
DECLARE_CONSOLETYPE(t2dBaseDatablock)


///-----------------------------------------------------------------------------
/// Check FX Datablock.
///
/// Use this when you've got a datablock and you want to ensure that it's
///-----------------------------------------------------------------------------
template<class T> bool t2dCheckDatablock( T* pDatablock )
{
    return ( pDatablock != NULL && pDatablock->getIsValid() );
}

// Adds compatibility when using SimObjectPtr<> Template.
template<class T> bool t2dCheckDatablock( T pDatablock )
{
    return ( pDatablock != NULL && pDatablock->getIsValid() );
}


#endif // _T2DBASEDATABLOCK_H_

