; The name of the installer
Name "WWhizInterfaceNET 3.0 Beta Build 1026"

; The file to write
OutFile "WWhizInterface30NET_1026.exe"

; The default installation directory
InstallDir $PROGRAMFILES\WWhizInterface\NET

ShowInstDetails nevershow

; The stuff to install
Section "Ignored"
  SetDetailsView hide
  SetDetailsPrint none
  SetAutoClose true

  SetOutPath $INSTDIR
  File "..\BinVSNET\WWhizInterface2.mod"
  File "..\BinVSNET\CTAGS.exe"

  WriteRegStr HKLM SOFTWARE\WWhizInterface\NET "Path" $INSTDIR
SectionEnd ; end the section

; eof
