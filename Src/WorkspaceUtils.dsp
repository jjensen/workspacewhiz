# Microsoft Developer Studio Project File - Name="WorkspaceUtils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WorkspaceUtils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WorkspaceUtils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WorkspaceUtils.mak" CFG="WorkspaceUtils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WorkspaceUtils - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WorkspaceUtils - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Addins/WorkspaceUtils", CAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WorkspaceUtils - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "WorkspaceUtils - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WorkspaceUtils - Win32 Release"
# Name "WorkspaceUtils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\CompilerFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtraProjectsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FileList.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FIPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WorkspaceInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceTags.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.def
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.odl
# ADD MTL /h "WorkspaceUtilsTypes.h"
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Commands.h
# End Source File
# Begin Source File

SOURCE=.\CompilerFiles.h
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.h
# End Source File
# Begin Source File

SOURCE=.\ExtraProjectsDialog.h
# End Source File
# Begin Source File

SOURCE=.\FileList.h
# End Source File
# Begin Source File

SOURCE=.\FindFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindFileEdit.h
# End Source File
# Begin Source File

SOURCE=.\FindFileListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FindTagDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindTagEdit.h
# End Source File
# Begin Source File

SOURCE=.\FindTagListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FIPreferences.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceInfo.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceTags.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\TBarLrge.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TBarMedm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WorkspaceUtils.rc2
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
