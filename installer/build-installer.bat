@echo off
SETLOCAL
SET QTIBIN=C:\dev\Qt\Tools\QtInstallerFramework\3.0\bin

%QTIBIN%\binarycreator.exe --offline-only -c config\config.xml -p packages Videocutter-Installer.exe
