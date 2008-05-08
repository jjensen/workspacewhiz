set rp="\machine\software\Microsoft\VisualStudio\7.0\InstallDir"
for /f "tokens=1*" %%i in ('dtreg -lv %rp%') do set VSDir=%%j
start /D"%VSDIR%" devenv.exe %*

