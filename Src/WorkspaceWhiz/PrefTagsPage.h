///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefTagsPage.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefTagsPage.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 12   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PREFTAGSPAGE_H__7DCD48E1_8B26_11D3_9B27_444553540000__INCLUDED_)
#define AFX_PREFTAGSPAGE_H__7DCD48E1_8B26_11D3_9B27_444553540000__INCLUDED_

#pragma once
// PrefTagsPage.h : header file
//

#include "PrefTagsListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefTagsPage dialog

class CPrefTagsPage : public CHtmlHelpPropPage
{
	DECLARE_DYNCREATE(CPrefTagsPage)

// Construction
public:
	CPrefTagsPage();
	~CPrefTagsPage();

// Dialog Data
	//{{AFX_DATA(CPrefTagsPage)
	enum { IDD = IDD_PREF_CTAGS };
	CEdit	m_tokenEdit;
	CListCtrl	m_tokenList;
	CPrefTagsListBox	m_extList;
	CEdit	m_extEdit;
	UINT	m_autoPackChangesEdit;
	BOOL	m_useTagCursorDialog;
	BOOL	m_autoRefreshTags;
	BOOL	m_autoSaveAll;
	BOOL	m_scanFilesTaskSwitch;
	UINT	m_autoSaveChanges;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefTagsPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillInControls();

	// Generated message map functions
	//{{AFX_MSG(CPrefTagsPage)
	afx_msg void OnPcAdd();
	afx_msg void OnPcRemove();
	afx_msg void OnPcReset();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePcAutopackchanges();
	afx_msg void OnPcAutorefreshtags();
	afx_msg void OnPcUsetagcursordialog();
	afx_msg void OnPcTokenadd();
	afx_msg void OnPcTokenremove();
	afx_msg void OnPcTokenreset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFTAGSPAGE_H__7DCD48E1_8B26_11D3_9B27_444553540000__INCLUDED_)
