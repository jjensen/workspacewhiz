///////////////////////////////////////////////////////////////////////////////
// $Workfile: HistoryDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/HistoryDialog.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 9    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_HISTORYDIALOG_H__F13988A4_8CC2_11D3_9B28_444553540000__INCLUDED_)
#define AFX_HISTORYDIALOG_H__F13988A4_8CC2_11D3_9B28_444553540000__INCLUDED_

#pragma once
// HistoryDialog.h : header file
//

#include "HistoryListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CHistoryDialog dialog

#define HISTORY_DIALOG CHtmlHelpDialog

class CHistoryDialog : public HISTORY_DIALOG
{
// Construction
public:
	CHistoryDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHistoryDialog)
	enum { IDD = IDD_HISTORY };
	CHistoryListCtrl	m_files;
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butReset;
	CButton	m_butPreferences;
	CButton	m_butHelp;
	CButton	m_butAbout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RefreshList();

	// Generated message map functions
	//{{AFX_MSG(CHistoryDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHAbout();
	afx_msg void OnHHelp();
	afx_msg void OnHPreferences();
	afx_msg void OnHReset();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkHFiles(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYDIALOG_H__F13988A4_8CC2_11D3_9B28_444553540000__INCLUDED_)
