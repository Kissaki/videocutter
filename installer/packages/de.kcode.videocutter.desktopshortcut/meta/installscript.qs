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
  if (installer.value("os") === "win")
  {
    component.addOperation("CreateShortcut", "@TargetDir@/Videocutter.exe", "@DesktopDir@/Videocutter.lnk");
  }
}
