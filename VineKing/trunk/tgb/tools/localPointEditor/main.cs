//------------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//------------------------------------------------------------------------------

exec("./localPointPrefs.cs");
exec("./gui/LocalPointEditor.ed.gui");
exec("./localPointEditor.cs");
exec("./localPointMouse.cs");
exec("./localPointDragAndDrop.cs");
exec("./linkpointEditor.cs");
exec("./collisionPolyEditor.cs");
exec("./shapeVectorEditor.cs");
exec("./behaviorPolyEditor.cs");
exec("./behaviorLocalPointListEditor.cs");


function initializeLocalPointEditor()
{
   if (!isObject(LinkpointEditor))
   {
      new ScriptObject(LinkpointEditor) {
         class = LocalPointEditor;
         showEditObject = true;
         showPolygon = false;
         showHull = false;
         showConvexViolations = false;
         clampToBounds = false;
         insertBetweenMode = false;
         showBackground = false;
         zoomEnable = true;
         polyControlsEnable = false;
         title = "Linkpoint Editor";
      };
   }
   
   if (!isObject(CollisionPolyEditor))
   {
      new ScriptObject(CollisionPolyEditor) {
         class = LocalPointEditor;
         showEditObject = true;
         showPolygon = true;
         showHull = true;
         showConvexViolations = true;
         clampToBounds = true;
         insertBetweenMode = true;
         showBackground = false;
         zoomEnable = false;
         polyControlsEnable = true;
         title = "Collision Editor";
      };
   }

   if (!isObject(ShapeVectorEditor))
   {
      new ScriptObject(ShapeVectorEditor) {
         class = LocalPointEditor;
         showEditObject = false;
         showPolygon = true;
         showHull = false;
         showConvexViolations = false;
         clampToBounds = true;
         insertBetweenMode = false;
         showBackground = false;
         zoomEnable = false;
         polyControlsEnable = true;
         title = "Polygon Editor";
      };
   }

   if (!isObject(BehaviorPolyEditor))
   {
      new ScriptObject(BehaviorPolyEditor) {
         class = LocalPointEditor;
         showEditObject = true;
         showPolygon = true;
         showHull = false;
         showConvexViolations = false;
         clampToBounds = true;
         insertBetweenMode = false;
         showBackground = false;
         zoomEnable = false;
         polyControlsEnable = true;
         title = "Behavior Polygon Editor";
      };
   }

   if (!isObject(BehaviorLocalPointListEditor))
   {
      new ScriptObject(BehaviorLocalPointListEditor) {
         class = LocalPointEditor;
         showEditObject = true;
         showPolygon = false;
         showHull = false;
         showConvexViolations = false;
         clampToBounds = false;
         insertBetweenMode = false;
         showBackground = false;
         zoomEnable = true;
         polyControlsEnable = false;
         title = "Behavior Local Point List Editor";
      };
   }
}

function destroyLocalPointEditor()
{
   if (isObject(LinkpointEditor))
   {
      // close it down, if it's open.
      if (LinkpointEditor.open)
         LinkpointEditor.close();
         
      // clean up the object.
      LinkpointEditor.delete();
   }

   if (isObject(CollisionPolyEditor))
   {
      // close it down, if it's open.
      if (CollisionPolyEditor.open)
         CollisionPolyEditor.close();
         
      // clean up the object.
      CollisionPolyEditor.delete();
   }

   if (isObject(ShapeVectorEditor))
   {
      // close it down, if it's open.
      if (ShapeVectorEditor.open)
         ShapeVectorEditor.close();
         
      // clean up the object.
      ShapeVectorEditor.delete();
   }

   if (isObject(BehaviorPolyEditor))
   {
      // close it down, if it's open.
      if (BehaviorPolyEditor.open)
         BehaviorPolyEditor.close();
         
      // clean up the object.
      BehaviorPolyEditor.delete();
   }

   if (isObject(BehaviorLocalPointListEditor))
   {
      // close it down, if it's open.
      if (BehaviorLocalPointListEditor.open)
         BehaviorLocalPointListEditor.close();
         
      // clean up the object.
      BehaviorLocalPointListEditor.delete();
   }
}


