setlocal
set SAVE_CD=%cd%
cd %~dp0
for /f "tokens=1,2,3*" %%i in ('reg query HKLM\software\NSIS') do set NSISDir=%%l
"%NSISDir%\MakeNSIS" WWhizInterface_VC6.nsi
"%NSISDir%\MakeNSIS" WWhizInterface_NET.nsi
cd %SAVE_CD%
endlocal

