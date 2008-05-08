///////////////////////////////////////////////////////////////////////////////
// $Workfile: ToolbarWnd.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/ToolbarWnd.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 9    $ $Author: Jjensen $
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
#include "ToolbarWnd.h"
#include "PreferencesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolbarWnd

CToolbarWnd::CToolbarWnd()
{
}

CToolbarWnd::~CToolbarWnd()
{
}


BEGIN_MESSAGE_MAP(CToolbarWnd, CWnd)
	//{{AFX_MSG_MAP(CToolbarWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_DOTOOLBAR, OnDoToolbar)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarWnd message handlers

extern void AddToolbar();

LRESULT CToolbarWnd::OnDoToolbar(WPARAM wParam, LPARAM lParam)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (wParam == 0x1234  &&  lParam == 0x56789012)
    {
		AddToolbar();

		CPreferencesDialog dlg;
		dlg.DoModal();
		DestroyWindow();
    }

    return Default();
}

