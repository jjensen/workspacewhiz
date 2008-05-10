regsvr32 /u /s ..\Bin\WorkspaceWhiz.dll
regsvr32 /u /s ..\BinVSNet\WorkspaceWhiz.dll

setlocal
for /f "tokens=1,2*" %%i in ('reg query HKLM\software\Microsoft\VisualStudio\6.0\Setup /v VsCommonDir') do set VSDir=%%k
"%VSDIR%\MSDev98\Bin\msdev.com" %cd%\WorkspaceWhiz60.dsw /MAKE "All - Win32 Release"
endlocal

REM --------------------------
setlocal
for /f "tokens=1,2*" %%i in ('reg query HKLM\software\Microsoft\VisualStudio\9.0 /v InstallDir') do set VSDir=%%k
"%VSDIR%\devenv.com" WorkspaceWhizNet.sln /build Release
endlocal

REM --------------------------
call ..\Installer\MakeWWhizBuilds.bat

