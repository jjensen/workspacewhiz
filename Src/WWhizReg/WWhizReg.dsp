# Microsoft Developer Studio Project File - Name="WWhizReg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WWhizReg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WWhizReg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WWhizReg.mak" CFG="WWhizReg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WWhizReg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWhizReg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WWhizReg", RABAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WWhizReg - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WWhizReg\Release60"
# PROP Intermediate_Dir "..\..\Output\WWhizReg\Release60"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizReg.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "..\Shared" /I "..\3rdParty" /D "NDEBUG" /D "_AFXEXT" /D "WWHIZINTERFACEBUILDDLL" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZ_VC6" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib /nologo /base:"0x60400000" /subsystem:windows /dll /machine:I386 /out:"..\..\Bin\WWhizReg.mod"
# SUBTRACT LINK32 /verbose /debug

!ELSEIF  "$(CFG)" == "WWhizReg - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\WWhizReg\Debug"
# PROP Intermediate_Dir "..\..\Output\WWhizReg\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizReg.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\Shared" /I "..\3rdParty" /D "_DEBUG" /D "_MBCS" /D "_AFXEXT" /D "WWHIZINTERFACEBUILDDLL" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZ_VC6" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\..\Bin\WWhizRegD.mod" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WWhizReg - Win32 Release"
# Name "WWhizReg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Base32.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterRegDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpiredDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtraFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\md5c.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\pchWWhizReg.cpp
# ADD CPP /Yc"pchWWhizReg.h"
# End Source File
# Begin Source File

SOURCE=.\RegHelperWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RegThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Subclass.cpp
# End Source File
# Begin Source File

SOURCE=.\ValidateRegistrationCode.cpp
# End Source File
# Begin Source File

SOURCE=.\WWhizReg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\WWhizReg.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Base32.h
# End Source File
# Begin Source File

SOURCE=.\EnterRegDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExpiredDlg.h
# End Source File
# Begin Source File

SOURCE=.\FindFileEdit.h
# End Source File
# Begin Source File

SOURCE=.\FindFileListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FindTagEdit.h
# End Source File
# Begin Source File

SOURCE=.\FindTagListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\md5global.h
# End Source File
# Begin Source File

SOURCE=.\pchWWhizReg.h
# End Source File
# Begin Source File

SOURCE=.\RegHelperWnd.h
# End Source File
# Begin Source File

SOURCE=.\RegThread.h
# End Source File
# Begin Source File

SOURCE=.\Subclass.h
# End Source File
# Begin Source File

SOURCE=.\ValidateRegistrationCode.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizReg.h
# End Source File
# Begin Source File

SOURCE=.\WWhizRegExt.h
# End Source File
# End Group
# Begin Group "Third Party"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\3rdParty\AggressiveOptimize.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\MemFile.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\MemFile.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\WCollection.h
# End Source File
# End Group
# End Target
# End Project
