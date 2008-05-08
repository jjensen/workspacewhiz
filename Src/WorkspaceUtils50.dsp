# Microsoft Developer Studio Project File - Name="WorkspaceUtils50" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WorkspaceUtils50 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WorkspaceUtils50.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WorkspaceUtils50.mak" CFG="WorkspaceUtils50 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WorkspaceUtils50 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WorkspaceUtils50 - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Addins/WorkspaceUtils", CAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WorkspaceUtils50 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release50"
# PROP Intermediate_Dir "Release50"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release50/WorkspaceUtils.dll"

!ELSEIF  "$(CFG)" == "WorkspaceUtils50 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Workspac"
# PROP BASE Intermediate_Dir "Workspac"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug50"
# PROP Intermediate_Dir "Debug50"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug50/WorkspaceUtils.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WorkspaceUtils50 - Win32 Release"
# Name "WorkspaceUtils50 - Win32 Debug"
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\Commands.h
# End Source File
# Begin Source File

SOURCE=.\CompilerFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\CompilerFiles.h
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.cpp
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.h
# End Source File
# Begin Source File

SOURCE=.\ExtraProjectsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtraProjectsDialog.h
# End Source File
# Begin Source File

SOURCE=.\FileList.cpp
# End Source File
# Begin Source File

SOURCE=.\FileList.h
# End Source File
# Begin Source File

SOURCE=.\FindFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindFileEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileEdit.h
# End Source File
# Begin Source File

SOURCE=.\FindFileListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FindTagDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindTagEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagEdit.h
# End Source File
# Begin Source File

SOURCE=.\FindTagListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FIPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\FIPreferences.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Readme.html
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceInfo.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceTags.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceTags.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.def
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.dsp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.dsw
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.odl
# ADD MTL /h "WorkspaceUtilsTypes.h"
# End Source File
# Begin Source File

SOURCE=.\WorkspaceUtils.rc

!IF  "$(CFG)" == "WorkspaceUtils50 - Win32 Release"

!ELSEIF  "$(CFG)" == "WorkspaceUtils50 - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
