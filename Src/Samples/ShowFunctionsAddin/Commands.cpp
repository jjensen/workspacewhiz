///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.cpp $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowFunctionsAddin/Commands.cpp $
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
#include "ShowFunctionsAddin.h"
#include "Commands.h"
#include "FunctionsDialog.h"

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

STDMETHODIMP CCommands::ShowFunctionsAddinCommandMethod() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Get the current file.
	CString currentFilename;
	g_wwhizInterface->GetCurrentFilename(currentFilename);

	// If there isn't a file currently open, then don't show the dialog.
	if (!currentFilename.IsEmpty())
	{
		CFunctionsDialog dlg;
		dlg.DoModal();
	}

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}
