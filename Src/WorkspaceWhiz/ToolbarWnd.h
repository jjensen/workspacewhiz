///////////////////////////////////////////////////////////////////////////////
// $Workfile: ToolbarWnd.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/ToolbarWnd.h $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TOOLBARWND_H__57F3DF62_CAE8_11D3_BE56_FE4DDAA68F61__INCLUDED_)
#define AFX_TOOLBARWND_H__57F3DF62_CAE8_11D3_BE56_FE4DDAA68F61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolbarWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolbarWnd window

class CToolbarWnd : public CWnd
{
// Construction
public:
	CToolbarWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolbarWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolbarWnd();

	// Generated message map functions
protected:
	LRESULT OnDoToolbar(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CToolbarWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBARWND_H__57F3DF62_CAE8_11D3_BE56_FE4DDAA68F61__INCLUDED_)
