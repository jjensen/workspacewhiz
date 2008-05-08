///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.cpp $
// $Archive: /WorkspaceWhiz/Src/Samples/BackupProjectsAddin/Commands.cpp $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BackupProjectsAddin.h"
#include "Commands.h"
#include "WWhizInterface2.h"
#include "WWhizInterface2Loader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands()
{
	m_pApplication = NULL;
}

CCommands::~CCommands()
{
	ASSERT (m_pApplication != NULL);
	m_pApplication->Release();
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	//  for us, which CDSAddIn did in its QueryInterface call
	//  just before it called us.
	m_pApplication = pApplication;
}


/////////////////////////////////////////////////////////////////////////////
// CCommands methods

STDMETHODIMP CCommands::BackupProjectsAddinCommandMethod() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Create the WWhizInterface.
	WWhizInterface* g_wwhizInterface = WWhizInterface2Create(AfxGetInstanceHandle(), m_pApplication);

	// Refresh all the files.
	g_wwhizInterface->RefreshFileList();

	// Get the project list.
	WWhizProjectList& projectList = g_wwhizInterface->GetProjectList();

	// Iterate the projects in the list.
	for (int i = 0; i < projectList.GetProjectCount(); ++i)
	{
		// Get the project at the specified index.
	    WWhizProject* project = projectList.GetProjectByIndex(i);

		// Is it a member of the workspace?  If not, then skip it.
		if (!project->IsWorkspaceProject())
			continue;

		// Ask if we should backup the project.
		if (AfxMessageBox("Backup the project " + project->GetName() + "?", MB_YESNO) == IDNO)
			continue;

		// Get the project's file list.
		WWhizFileList& fileList = project->GetFileList();

		// Iterate all members of the file list.
		for (int j = 0; j < fileList.GetCount(); ++j)
		{
			// Get the file at the specified index.
			WWhizFile* file = fileList.Get(j);

			// Make a copy of it.
			CString existingFilename = file->GetCaseFullName();
			CString newFilename = existingFilename + ".bak";
			::CopyFile(existingFilename, newFilename, FALSE);
		}
	}

	// Destroy the WWhizInterface.
	WWhizInterface2Destroy();
		
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}
