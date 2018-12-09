@echo off
SETLOCAL
SET QTBIN=

cd %~dp0
copy /Y "build-release\bin\Videocutter.exe" "deploy/Videocutter.exe"
C:\dev\Qt\5.12.0\msvc2017_64\bin\windeployqt.exe -dir "%~dp0\deploy" "build-Videocutter-Desktop_Qt_5_10_0_MSVC2017_64bit-Release\bin\Videocutter.exe"
