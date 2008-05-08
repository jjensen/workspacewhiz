setlocal
set SAVE_CD=%cd%
cd %~dp0
rem call MakeWWhizInterface.bat
for /f "tokens=1,2,3,4,5*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Inno Setup 5_is1" /v "Inno Setup: App Path"') do set INNO=%%n
"%INNO%\iscc" WWhizNet.iss
"%INNO%\iscc" WWhizVC6.iss
cd %SAVE_CD%
endlocal

