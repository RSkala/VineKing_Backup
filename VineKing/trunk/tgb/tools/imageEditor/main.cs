//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

function initializeImageEditor()
{   
   echo(" % - Initializing ImageMap Builder");

   execPrefs("imageEditorPrefs.cs");
   
   exec("./data/content/datablocks.cs");

   exec("./scripts/editor.ed.cs");
   exec("./scripts/editorSettings.ed.cs");
   exec("./scripts/editorPreviewWindow.ed.cs");
   
   //exec("./gui/OpenFileDlgExwPreview.ed.gui");
   exec("./gui/ImageBuilderGui.ed.gui");
}

function destroyImageEditor()
{
   // Export Preferences.
   echo("Exporting Image Editor preferences.");
   export("$imageEditor::*", "imageEditorPrefs.cs", false);
}
