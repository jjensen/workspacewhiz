# Microsoft Developer Studio Project File - Name="WWhizTemplateManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WWhizTemplateManager - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WWhizTemplateManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WWhizTemplateManager.mak" CFG="WWhizTemplateManager - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WWhizTemplateManager - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWhizTemplateManager - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WorkspaceWhiz/Src/WWhizTemplateManager", IBBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WWhizTemplateManager - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WWhizTemplateManager\Release60"
# PROP Intermediate_Dir "..\..\Output\WWhizTemplateManager\Release60"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizTemplateManager.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\Shared" /I "..\3rdParty" /FI"pchWWhizTemplateManager.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZ_VC6" /Yu"pchWWhizTemplateManager.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /base:"0x60500000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Bin\WWhizTemplateManager.mod"
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "WWhizTemplateManager - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\WWhizTemplateManager\Debug"
# PROP Intermediate_Dir "..\..\Output\WWhizTemplateManager\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizTemplateManager.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Shared" /I "..\3rdParty" /FI"pchWWhizTemplateManager.h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZ_VC6" /Yu"pchWWhizTemplateManager.h" /FD /c
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
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"..\..\Bin\WWhizTemplateManagerD.mod" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WWhizTemplateManager - Win32 Release"
# Name "WWhizTemplateManager - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pchWWhizTemplateManager.cpp
# ADD CPP /Yc"pchWWhizTemplateManager.h"
# End Source File
# Begin Source File

SOURCE=.\Template.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WWhizTemplateManager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\WWhizTemplateManager.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\pchWWhizTemplateManager.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Template.h
# End Source File
# Begin Source File

SOURCE=.\TemplateGroup.h
# End Source File
# Begin Source File

SOURCE=.\TemplateManager.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizTemplateManager.h
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

SOURCE=..\3rdParty\ObjModelHelper.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\WCollection.h
# End Source File
# End Group
# End Target
# End Project
