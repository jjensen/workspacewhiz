; The name of the installer
Name "WWhizInterfaceNET 4.0 Beta Build 1105"

; The file to write
OutFile "WWhizInterface40NET_1105.exe"

; The default installation directory
InstallDir $PROGRAMFILES\WWhizInterface\2005

ShowInstDetails nevershow

; The stuff to install
Section "Ignored"
  SetDetailsView hide
  SetDetailsPrint none
  SetAutoClose true

  SetOutPath $INSTDIR
  File "..\BinVSNET\WWhizInterface2.mod"
  File "..\BinVSNET\CTAGS.exe"

  WriteRegStr HKLM SOFTWARE\WWhizInterface\2005 "Path" $INSTDIR
SectionEnd ; end the section

; eof
