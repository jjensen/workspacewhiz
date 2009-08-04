# Microsoft Developer Studio Project File - Name="WWhizPickDirTemplate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WWhizPickDirTemplate - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WWhizPickDirTemplate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WWhizPickDirTemplate.mak" CFG="WWhizPickDirTemplate - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WWhizPickDirTemplate - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWhizPickDirTemplate - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WorkspaceWhiz/Src/WWhizPickDirTemplate", GBBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WWhizPickDirTemplate - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WWhizPickDirTemplate\Release60"
# PROP Intermediate_Dir "..\..\Output\WWhizPickDirTemplate\Release60"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizPickDirTemplate.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\Shared" /I "..\3rdParty" /FI"pchWWhizPickDirTemplate.h" /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZ_VC6" /Yu"pchWWhizPickDirTemplate.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /base:"0x60300000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Bin\TemplateCommands\WWhizPickDirTemplate.tpl"
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "WWhizPickDirTemplate - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\WWhizPickDirTemplate\Debug"
# PROP Intermediate_Dir "..\..\Output\WWhizPickDirTemplate\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizPickDirTemplate.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Shared" /I "..\3rdParty" /FI"pchWWhizPickDirTemplate.h" /D "_DEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZ_VC6" /Yu"pchWWhizPickDirTemplate.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\..\Bin\TemplateCommands\WWhizPickDirTemplate.dtpl" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WWhizPickDirTemplate - Win32 Release"
# Name "WWhizPickDirTemplate - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pchWWhizPickDirTemplate.cpp
# ADD CPP /Yc"pchWWhizPickDirTemplate.h"
# End Source File
# Begin Source File

SOURCE=.\WWhizPickDirTemplate.cpp
# ADD CPP /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\pchWWhizPickDirTemplate.h
# End Source File
# End Group
# Begin Group "Third Party"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\3rdParty\AggressiveOptimize.h
# End Source File
# Begin Source File

SOURCE=.\PickDirDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PickDirDlg.h
# End Source File
# End Group
# End Target
# End Project
