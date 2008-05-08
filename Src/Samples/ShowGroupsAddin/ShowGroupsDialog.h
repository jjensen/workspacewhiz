///////////////////////////////////////////////////////////////////////////////
// $Workfile: ShowGroupsDialog.h $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowGroupsAddin/ShowGroupsDialog.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SHOWGROUPSDIALOG_H__8B9C5FE1_8A97_44FF_A15B_25A770A129C7__INCLUDED_)
#define AFX_SHOWGROUPSDIALOG_H__8B9C5FE1_8A97_44FF_A15B_25A770A129C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowGroupsDialog.h : header file
//

#include "TreeCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CShowGroupsDialog dialog

class CShowGroupsDialog : public CDialog
{
// Construction
public:
	CShowGroupsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowGroupsDialog)
	enum { IDD = IDD_SHOWGROUPS };
	CTreeCtrlEx	m_tree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowGroupsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowGroupsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWGROUPSDIALOG_H__8B9C5FE1_8A97_44FF_A15B_25A770A129C7__INCLUDED_)
