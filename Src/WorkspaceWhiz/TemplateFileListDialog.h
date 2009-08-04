///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateFileListDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TemplateFileListDialog.h $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TEMPLATEFILELISTDIALOG_H__55678086_6611_11D3_A0BF_00104B639721__INCLUDED_)
#define AFX_TEMPLATEFILELISTDIALOG_H__55678086_6611_11D3_A0BF_00104B639721__INCLUDED_

#pragma once
// TemplateFileListDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateFileListDialog dialog

#define TEMPLATE_FILE_LIST_DIALOG CHtmlHelpDialog

class CTemplateFileListDialog : public TEMPLATE_FILE_LIST_DIALOG
{
// Construction
public:
	CTemplateFileListDialog(CWnd* pParent = NULL);   // standard constructor
	static bool s_autoCloseParent;		// A hack to close the parent.

// Dialog Data
	//{{AFX_DATA(CTemplateFileListDialog)
	enum { IDD = IDD_TEMPLATEFILELIST };
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butRemove;
	CButton	m_butEdit;
	CButton	m_butAdd;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateFileListDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Refresh();

	// Generated message map functions
	//{{AFX_MSG(CTemplateFileListDialog)
	afx_msg void OnTflAdd();
	afx_msg void OnTflRemove();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTflEdit();
	afx_msg void OnDblclkTflList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnComAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEFILELISTDIALOG_H__55678086_6611_11D3_A0BF_00104B639721__INCLUDED_)
