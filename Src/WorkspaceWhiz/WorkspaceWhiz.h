///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceWhiz.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/WorkspaceWhiz.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 11   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_WORKSPACEWHIZ_H__039FB62B_6DD0_11D3_9B27_835709CADA52__INCLUDED_)
#define AFX_WORKSPACEWHIZ_H__039FB62B_6DD0_11D3_9B27_835709CADA52__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\dbgguid.h>

#define WM_DOTOOLBAR             (WM_APP + 1)

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceWhizApp

class CWorkspaceWhizApp : public CWinApp
{
public:
	CWorkspaceWhizApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceWhizApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWorkspaceWhizApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};


// The global application.
extern CWorkspaceWhizApp theApp;

extern Config g_config;

struct CommandInfo
{
	LPCTSTR m_name;
	LPCTSTR m_keyStr;
	bool m_hasIcon;
	bool m_justText;
};

const CommandInfo* GetCommandList();
int GetCommandCount();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEWHIZ_H__039FB62B_6DD0_11D3_9B27_835709CADA52__INCLUDED)
