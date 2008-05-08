///////////////////////////////////////////////////////////////////////////////
// $Workfile: Main.cpp $
// $Archive: /WorkspaceWhiz/Src/Samples/TestWWhiz/Main.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <afx.h>
#include <afxwin.h>
#include "WWhizInterfaceLoader.h"
#include "WWhizInterface3.h"

WWhizInterface* g_wwhiz;

CWinApp winApp;

int main()
{
	g_wwhiz = WWhizInterface2Create(NULL, NULL);
	if (!g_wwhiz)
	{
		printf("WWhizInterface.mod not found.");
		return -1;
	}

	DWORD version = g_wwhiz->GetVersion();
	printf("WWhizInterface version: %d.%02d Build %4d\n",
		version >> 24, (version >> 16) & 0xFF, (version & 0xFFFF));

	g_wwhiz->AddProject("../Src/WorkspaceWhiz60.dsw");
	g_wwhiz->RefreshFileList();

	WWhizProjectList& projectList = g_wwhiz->GetProjectList();
	int projectCount = projectList.GetProjectCount();
	for (int i = 0; i < projectCount; i++)
	{
		WWhizProject* project = projectList.GetProjectByIndex(i);
		printf(project->GetName());
		CString projectName = project->GetName();
		printf("\n");

		WWhizFileList& fileList = project->GetFileList();
		int fileCount = fileList.GetCount();
		for (int j = 0; j < fileCount; j++)
		{
			WWhizFile* file = fileList.Get(j);
			CString str;
			str.Format("[%d] %s - %s - %s - %s - %s - %s\n", j,
				(LPCTSTR)file->GetFullName(), (LPCTSTR)file->GetCaseFullName(),
				(LPCTSTR)file->GetShortName(), (LPCTSTR)file->GetTitle(),
				(LPCTSTR)file->GetExt(), (LPCTSTR)file->GetPath());
			printf(str);
		}
	}

	WWhizInterface2Destroy();
}
