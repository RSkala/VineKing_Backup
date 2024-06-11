#include "console/console.h"
#include "console/consoleTypes.h"
#include "TGB/gui2DDatablockDropDown.h"
#include "T2D/t2dBaseDatablock.h"

IMPLEMENT_CONOBJECT( T2DDatablockDropDownCtrl );

T2DDatablockDropDownCtrl::T2DDatablockDropDownCtrl():GuiPopUpMenuCtrlEx()
{
   // Default to all t2d datablocks
   mFilter = StringTable->insert("t2dBaseDatablock");
 
   mDatablocks = t2dBaseDatablock::get2DDatablockSet();
   mBounds.extent.set( 125, 20 );

}

bool T2DDatablockDropDownCtrl::onAdd()
{
   if(!Parent::onAdd())
      return false;
   
   populateList();

   return true;
}

bool T2DDatablockDropDownCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   return true;
}

void T2DDatablockDropDownCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField( "DatablockFilter", TypeString, Offset( mFilter, T2DDatablockDropDownCtrl));
}

void T2DDatablockDropDownCtrl::onStaticModified( const char* slotName )
{
   Parent::onStaticModified(slotName);
   // onStaticModified is called when one of our persistent fields has been modified in the GUI editor.
   // I suppose this is little known functionality, mostly because it was broken until i fixed it recently
   // so basically when we get this notification it means the user has specified a different filter for our
   // datablock list and we should re-populate the list
   populateList();
}


ConsoleMethod( T2DDatablockDropDownCtrl, setFilter, void, 2, 3, "([datablock classname]) Sets the datablock filter. No parameters for all t2d datablocks"
			  "@param classname The datablock you want to set(default \"t2dBaseDatablock\"\n"
			  "@return No return value.")
{
   object->setFilter( argv[2] );
}

void T2DDatablockDropDownCtrl::setFilter( StringTableEntry className )
{
   if( dStricmp( className, "" ) == 0 )
      mFilter = StringTable->insert("t2dBaseDatablock");
   else
      mFilter = StringTable->insert( className );

   populateList();

}

void T2DDatablockDropDownCtrl::populateList()
{
   clear();

   SimSet::iterator iter = mDatablocks->begin();

   S32 i = 1;
   while (iter != mDatablocks->end() )
   {
      t2dBaseDatablock *block = dynamic_cast<t2dBaseDatablock*>((*iter));
      if( block )
      {
         AbstractClassRep* pRep = block->getClassRep();
         while(pRep)
         {
            if(!dStricmp(pRep->getClassName(), mFilter))
            {
               addEntry( block->getName(), i++ );
               pRep = NULL;
            }
            else
            {
               pRep	=	pRep->getParentClass();
            }
         }

      }
      iter++;
   }
   
   // If there are no entries, bail
   if( mEntries.empty() )
      return;

   // Sort the entries
   sort();


   setText( mEntries[0].buf );


}