///////////////////////////////////////////////////////////////////////////////
// $Workfile: RegHelperWnd.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/RegHelperWnd.h $
// $Date:: 7/14/00 11:11p  $ $Revision:: 3    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRegHelperWnd window

class CRegHelperWnd : public CWnd
{
// Construction
public:
	CRegHelperWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegHelperWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegHelperWnd();

	// Generated message map functions
protected:
	LRESULT OnDoRegSetup(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CRegHelperWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

