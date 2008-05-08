set rp="\machine\software\Microsoft\VisualStudio\6.0\Setup\VsCommonDir"
for /f "tokens=1*" %%i in ('dtreg -lv %rp%') do set VSDir=%%j
start /D"%VSDIR%\MSDev98\Bin" msdev.exe %*

