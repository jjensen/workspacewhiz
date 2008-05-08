# Microsoft Developer Studio Project File - Name="WWhizInterface" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WWhizInterface - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WWhizInterface.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WWhizInterface.mak" CFG="WWhizInterface - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WWhizInterface - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WWhizInterface - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WorkspaceWhiz/Src/WWhizInterface", YVAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WWhizInterface - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WWhizInterface\Release60"
# PROP Intermediate_Dir "..\..\Output\WWhizInterface\Release60"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizInterface.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\Shared" /I "..\3rdParty" /FI"pchWWhizInterface.h" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZINTERFACEBUILDDLL" /Yu"pchWWhizInterface.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /base:"0x60100000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Bin\WWhizInterface2.mod"
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "WWhizInterface - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Output\WWhizInterface\Debug"
# PROP Intermediate_Dir "..\..\Output\WWhizInterface\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"pchWWhizInterface.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Shared" /I "..\3rdParty" /FI"pchWWhizInterface.h" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "WWHIZINTERFACEBUILDDLL" /Yu"pchWWhizInterface.h" /FD /c
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
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Bin\WWhizInterface2D.mod" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "WWhizInterface - Win32 Release"
# Name "WWhizInterface - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CompilerFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\FileList.cpp
# End Source File
# Begin Source File

SOURCE=.\pchWWhizInterface.cpp
# ADD CPP /Yc"pchWWhizInterface.h"
# End Source File
# Begin Source File

SOURCE=.\ProjectList.cpp
# End Source File
# Begin Source File

SOURCE=.\TagList.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceTags.cpp
# End Source File
# Begin Source File

SOURCE=.\WWhizInterface.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\WWhizInterface.rc
# End Source File
# Begin Source File

SOURCE=.\WWhizInterfaceImpl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CompilerFiles.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\FileList.h
# End Source File
# Begin Source File

SOURCE=.\pchWWhizInterface.h
# End Source File
# Begin Source File

SOURCE=.\ProjectList.h
# End Source File
# Begin Source File

SOURCE=.\RCBase.h
# End Source File
# Begin Source File

SOURCE=.\TagList.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceInfo.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceTags.h
# End Source File
# End Group
# Begin Group "Third Party"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\3rdParty\AfxTemplateEx.h
# End Source File
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

SOURCE=..\Shared\Timer.h
# End Source File
# End Group
# Begin Group "Shared"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Shared\Node.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Node.h
# End Source File
# Begin Source File

SOURCE=..\Shared\VirtualDrive.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\VirtualDrive.h
# End Source File
# Begin Source File

SOURCE=..\Shared\VirtualDriveManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\VirtualDriveManager.h
# End Source File
# Begin Source File

SOURCE=..\Shared\VirtualFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\VirtualFile.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizInterface2.h
# End Source File
# Begin Source File

SOURCE=..\Shared\XmlData.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\XmlData.h
# End Source File
# End Group
# End Target
# End Project
