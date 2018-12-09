function Component()
{
}

Component.prototype.createOperations = function()
{
  try
  {
    component.createOperations();
  }
  catch (e)
  {
    print(e);
  }
  if (systemInfo.productType === "windows")
  {
    component.addOperation("CreateShortcut"
        , "@TargetDir@/Videocutter.exe"
        , "@StartMenuDir@/Videocutter.lnk"
        , "workingDirectory=@TargetDir@"
        , "description=Launch Videocutter"
        );
  }
}
