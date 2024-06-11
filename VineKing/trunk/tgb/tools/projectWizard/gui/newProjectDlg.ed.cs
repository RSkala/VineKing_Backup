function TGBWorkspace::getGamesFolder( %this )
{
   if( $Pref::UserGamesFolder $= "" )
      return getUserHomeDirectory() @ "/MyProjects";
   
   return $Pref::UserGamesFolder;
}


function NewProjectDlg::onWake( %this )
{      
   NewProjectLocationText.Text = TGBWorkspace.getGamesFolder();
}