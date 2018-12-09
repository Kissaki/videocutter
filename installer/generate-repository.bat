@echo off
SETLOCAL
SET QTIBIN=C:\dev\Qt\Tools\QtInstallerFramework\3.0\bin

%QTIBIN%\repogen.exe -p packages repository
