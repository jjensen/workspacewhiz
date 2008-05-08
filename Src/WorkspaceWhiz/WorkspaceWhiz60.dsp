# Microsoft Developer Studio Project File - Name="WorkspaceWhiz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WorkspaceWhiz - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WorkspaceWhiz60.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WorkspaceWhiz60.mak" CFG="WorkspaceWhiz - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WorkspaceWhiz - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WorkspaceWhiz - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/WorkspaceWhiz/Src/WorkspaceWhiz", HSAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WorkspaceWhiz - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WorkspaceWhiz\Release60"
# PROP Intermediate_Dir "..\..\Output\WorkspaceWhiz\Release60"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\Shared" /I "." /I "..\3rdParty" /FI"StdAfx.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib version.lib /nologo /base:"0x60000000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Bin\WorkspaceWhiz.dll"

!ELSEIF  "$(CFG)" == "WorkspaceWhiz - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Output\WorkspaceWhiz\Debug"
# PROP Intermediate_Dir "..\..\Output\WorkspaceWhiz\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "." /I "..\3rdParty" /FI"StdAfx.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib version.lib /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /out:"..\..\Bin\WorkspaceWhizD.dll" /pdbtype:sept
# SUBTRACT LINK32 /verbose

!ENDIF 

# Begin Target

# Name "WorkspaceWhiz - Win32 Release"
# Name "WorkspaceWhiz - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AddInInt.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoCompleteTagsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtraFilesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTagDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\History.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlHelpDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefHeaderFlipListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefHeaderFlipPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefKeyBindingsHotKeyCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefKeyBindingsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefTagsListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefTagsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TemplateFileListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateSelectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateWizardDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TextLine.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolbarWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceCommands.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWhiz.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWhiz.def
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWhiz.odl
# ADD MTL /h "WorkspaceWhizTypes.h"
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWhiz.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=.\AutoCompleteTagsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Commands.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\DSAddIn.h
# End Source File
# Begin Source File

SOURCE=.\ExtraFilesDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\FindTagDialog.h
# End Source File
# Begin Source File

SOURCE=.\History.h
# End Source File
# Begin Source File

SOURCE=.\HistoryDialog.h
# End Source File
# Begin Source File

SOURCE=.\HistoryListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HtmlHelpDialog.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesDialog.h
# End Source File
# Begin Source File

SOURCE=.\PrefHeaderFlipListBox.h
# End Source File
# Begin Source File

SOURCE=.\PrefHeaderFlipPage.h
# End Source File
# Begin Source File

SOURCE=.\PrefKeyBindingsHotKeyCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PrefKeyBindingsPage.h
# End Source File
# Begin Source File

SOURCE=.\PrefTagsListBox.h
# End Source File
# Begin Source File

SOURCE=.\PrefTagsPage.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TemplateFileListDialog.h
# End Source File
# Begin Source File

SOURCE=.\TemplateSelectDialog.h
# End Source File
# Begin Source File

SOURCE=.\TemplateWizardDialog.h
# End Source File
# Begin Source File

SOURCE=.\TextLine.h
# End Source File
# Begin Source File

SOURCE=.\ToolbarWnd.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceCommands.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWhiz.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AboutTitle.JPG
# End Source File
# Begin Source File

SOURCE=.\res\Back.GIF
# End Source File
# Begin Source File

SOURCE=.\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Logo.jpg
# End Source File
# Begin Source File

SOURCE=.\res\TBarLrge.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TBarMedm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WorkspaceWhiz.rc2
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Third Party"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\3rdParty\AddInComm.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\AICLoader.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\3rdParty\AICLoader.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\BCMenu.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCDynamicDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCDynamicDialog.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCDynamicWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCDynamicWnd.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCDynamicWndEx.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCDynamicWndEx.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCSizeIconCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\cdxCSizeIconCtrl.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ChoiceWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ChoiceWindow.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ChoiceWindowBtn.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ChoiceWindowBtn.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\MemFile.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\MemFile.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ObjModelHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ObjModelHelper.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ShellContextMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ShellContextMenu.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ShellTools.cpp
# End Source File
# Begin Source File

SOURCE=..\3rdParty\ShellTools.h
# End Source File
# Begin Source File

SOURCE=..\3rdParty\TreeCtrlEx.h
# End Source File
# End Group
# Begin Group "Shared"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Shared\WWhizInterface2.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizInterface2Loader.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizInterface2Loader.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizRegLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizRegLoader.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizTemplateManager.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizTemplateManagerLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WWhizTemplateManagerLoader.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\About.html
# End Source File
# End Target
# End Project
