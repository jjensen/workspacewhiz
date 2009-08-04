///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateSelectDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TemplateSelectDialog.h $
// $Date: 2003/01/16 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TEMPLATESELECTDIALOG_H__E9577443_648F_11D3_9B27_444553540000__INCLUDED_)
#define AFX_TEMPLATESELECTDIALOG_H__E9577443_648F_11D3_9B27_444553540000__INCLUDED_

#pragma once
// TemplateSelectDialog.h : header file
//

#include "TreeCtrlEx.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplateSelectDialog dialog

#define TEMPLATE_SELECT_DIALOG CHtmlHelpDialog

class CTemplateSelectDialog : public TEMPLATE_SELECT_DIALOG
{
// Construction
public:
	CTemplateSelectDialog(CWnd* pParent = NULL);   // standard constructor

	static void DoDialog();

// Dialog Data
	//{{AFX_DATA(CTemplateSelectDialog)
	enum { IDD = IDD_TEMPLATESELECT };
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butFileList;
	CButton	m_butAbout;
	CStatic	m_memo;
	CTreeCtrlEx	m_tree;
	//}}AFX_DATA

	WWhizTemplate* m_code;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateSelectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Refresh();

	// Generated message map functions
	//{{AFX_MSG(CTemplateSelectDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedCtTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkCtTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTfFilelist();
	afx_msg void OnDestroy();
	afx_msg void OnTsAbout();
	afx_msg void OnRclickTsTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATESELECTDIALOG_H__E9577443_648F_11D3_9B27_444553540000__INCLUDED_)
