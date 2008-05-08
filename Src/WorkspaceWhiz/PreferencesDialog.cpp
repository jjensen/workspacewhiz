///////////////////////////////////////////////////////////////////////////////
// $Workfile: PreferencesDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PreferencesDialog.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 13   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "workspacewhiz.h"
#include "PreferencesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW							
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog

IMPLEMENT_DYNAMIC(CPreferencesDialog, CHtmlHelpPropSheet)

CPreferencesDialog::CPreferencesDialog(UINT iSelectPage, CWnd* pParentWnd)
	:CHtmlHelpPropSheet(IDS_PREFDLG_CAPTION, pParentWnd, iSelectPage),
	m_keyBindingsDlg(0),
	m_keyBindingsDlg2(1),
	m_keyBindingsDlg3(2)
{
	AddPage(&m_keyBindingsDlg);
	AddPage(&m_keyBindingsDlg2);
	AddPage(&m_keyBindingsDlg3);
	AddPage(&m_headerFlipPage);
	AddPage(&m_tagsPage);
}

CPreferencesDialog::~CPreferencesDialog()
{
}


BEGIN_MESSAGE_MAP(CPreferencesDialog, CHtmlHelpPropSheet)
	//{{AFX_MSG_MAP(CPreferencesDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog message handlers

