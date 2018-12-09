@echo off
SETLOCAL
SET QTBIN=C:\dev\Qt\5.12.0\msvc2017_64\bin
SET TARGET=installer\packages\de.kcode.videocutter\data
SET SOURCEDIR=src\bin

PUSHD %~dp0
IF ERRORLEVEL 1 (PAUSE && EXIT 1)

PUSHD "%TARGET%"
IF ERRORLEVEL 1 (PAUSE && EXIT 1)
DEL /F /S /Q *
IF ERRORLEVEL 1 (PAUSE && EXIT 1)
POPD
IF ERRORLEVEL 1 (PAUSE && EXIT 1)

COPY "3rd-party\ffmpeg.exe" "%TARGET%\ffmpeg.exe"
IF ERRORLEVEL 1 (PAUSE && EXIT 1)
COPY "%SOURCEDIR%\Videocutter.exe" "%TARGET%/Videocutter.exe"
IF ERRORLEVEL 1 (PAUSE && EXIT 1)

%QTBIN%\windeployqt.exe -dir "%TARGET%" "%TARGET%\Videocutter.exe"
IF ERRORLEVEL 1 (PAUSE && EXIT 1)
