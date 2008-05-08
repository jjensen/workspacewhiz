///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceCommands.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/WorkspaceCommands.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 11   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WorkspaceWhiz.h"
#include "WorkspaceCommands.h"
#include "FindFileDialog.h"
#include "History.h"

extern CFindFileDialog g_wfoDialog;			// Workspace file open dialog.
extern CFindFileDialog g_cfoDialog;			// Compiler file open dialog.

int WorkspaceCommands::PutTemplate(WWhizTemplate& code)
{
	ObjModelHelper objModel;
	if (objModel.GetActiveDocument())
	{
		code.SetEntry("sel", objModel.GetText());
	}
	else
	{
		code.SetEntry("sel", "");
	}

	code.Parse();

	return 0;
}

	
int WorkspaceCommands::HeaderFlipDlg(CString fullName)
{
	// Exit if no filename was passed in.
	if (fullName.IsEmpty())
	{
		g_wwhizInterface->GetCurrentFilename(fullName);
		if (fullName.IsEmpty())
		{
			return -1;
		}
	}

	// Find the dot.
	int dotPos = fullName.ReverseFind('.');
	if (dotPos != -1)
		fullName = fullName.Left(dotPos);

	// Find the last backslash.
	int slashPos = fullName.ReverseFind('\\');
	if (slashPos != -1)
		fullName = fullName.Mid(slashPos + 1);

	g_wwhizInterface->RefreshFileList();

	CString saveLastFilename = g_wfoDialog.m_lastFilename;
	g_wfoDialog.m_lastFilename = fullName;
	
	g_wfoDialog.m_fileList = &g_wwhizInterface->GetActiveFileList();
	g_wfoDialog.m_workspaceOpen = true;
	g_wfoDialog.DoModal();

	// Update the workspace, in case it changed.
	g_wwhizInterface->RefreshFileList();

	return 0;
}

