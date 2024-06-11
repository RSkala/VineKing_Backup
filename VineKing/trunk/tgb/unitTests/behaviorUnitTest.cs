function UnitTestBehavior::test( %this )
{
   return ( %this.unitTestField $= "foo" );
}

function UnitTestBehaviorParent::parent_test( %this )
{
   return true;
}