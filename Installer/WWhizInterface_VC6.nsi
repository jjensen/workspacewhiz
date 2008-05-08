; The name of the installer
Name "WWhizInterfaceVC6 3.0 Beta Build 1026"

; The file to write
OutFile "WWhizInterface30VC6_1026.exe"

; The default installation directory
InstallDir $PROGRAMFILES\WWhizInterface\VC6

ShowInstDetails nevershow

; The stuff to install
Section "Ignored"
  SetDetailsView hide
  SetDetailsPrint none
  SetAutoClose true

  SetOutPath $INSTDIR
  File "..\Bin\WWhizInterface2.mod"
  File "..\Bin\CTAGS.exe"

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  StrCpy $1 $0\MSDev98\Bin\IDE

  SetOutPath $1
  File "..\Bin\WWhizInterfaceHelper.pkg"
  WriteRegStr HKLM SOFTWARE\WWhizInterface\VC6 "Path" $INSTDIR
  WriteRegStr HKLM SOFTWARE\WWhizInterface "Path" $INSTDIR
SectionEnd ; end the section

; eof
