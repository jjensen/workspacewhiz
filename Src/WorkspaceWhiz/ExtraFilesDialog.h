///////////////////////////////////////////////////////////////////////////////
// $Workfile: ExtraFilesDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/ExtraFilesDialog.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 13   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_EXTRAFILESDIALOG_H__C59ABF61_B178_11D1_84EF_444553540000__INCLUDED_)
#define AFX_EXTRAFILESDIALOG_H__C59ABF61_B178_11D1_84EF_444553540000__INCLUDED_

#pragma once
// ExtraFilesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtraFilesDialog dialog

class CExtraFilesDialog : public CHtmlHelpDialog
{
// Construction
public:
	CExtraFilesDialog(CWnd* pParent = NULL);   // standard constructor

	CString workspaceLocation;

// Dialog Data
	//{{AFX_DATA(CExtraFilesDialog)
	enum { IDD = IDD_EXTRAFILES };
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butRemove;
	CButton	m_butBrowse;
	CButton	m_butAdd;
	CEdit	m_filename;
	CCheckListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtraFilesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ResolveFilename(CString& filename);
	void AddFileToList(CString& filename);
	
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CExtraFilesDialog)
	afx_msg void OnEpAdd();
	afx_msg void OnEpRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnEpBrowse();
	virtual void OnOK();
	afx_msg void OnDblclkEpList();
	afx_msg void OnComAbout();
	afx_msg void OnComPreferences();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRAFILESDIALOG_H__C59ABF61_B178_11D1_84EF_444553540000__INCLUDED_)
