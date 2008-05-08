///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindTagDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/FindTagDialog.h $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_FINDTAGDIALOG_H__5C39DF21_24DB_11D2_A392_444553540000__INCLUDED_)
#define AFX_FINDTAGDIALOG_H__5C39DF21_24DB_11D2_A392_444553540000__INCLUDED_

#pragma once

#include <afxcmn.h>

#include "ChoiceWindowBtn.h"

/////////////////////////////////////////////////////////////////////////////
// CFindTagDialog dialog

#define FIND_TAG_DIALOG CHtmlHelpDialog

class CFindTagDialog : public FIND_TAG_DIALOG
{
// Construction
public:
	typedef enum { ScopeCurrentFile, ScopeCurrentProject, ScopeEntireWorkspace } TagScope;

	CFindTagDialog(CWnd* pParent = NULL);   // standard constructor
	~CFindTagDialog();

	void Refresh();
	void OnChangeFuncname();
	void RefreshList(LPCTSTR name);


	static CString m_lastFunction;
	static int m_lastPosition;
	static CString m_oldFunction;
	bool m_created;
	static TagScope s_tagScope;
	static int s_typeFilter;

	WWhizTagList* m_fileTagList;
	WWhizProject* m_project;

	TagArray m_tagArray;

// Dialog Data
	//{{AFX_DATA(CFindTagDialog)
	enum { IDD = IDD_FINDTAG };
	CEdit	m_infoMisc;
	CButton	m_butExtraFiles;
	CButton	m_butCurrentFile;
	CComboBox	m_typeCombo;
	CChoiceWindowBtn m_butShow;
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butRebuildTags;
	CButton	m_butHelp;
	CButton	m_butAbout;
	CStatic			m_infoSearchString;
	CStatic			m_infoFilename;
	//}}AFX_DATA
	afx_msg void OnFtRebuildTags();

	CComboBoxEx*	m_tagParent;
	CComboBoxEx*	m_edit;
	CListCtrl*		m_tagListCtrl;
	int m_saveLastPosition;
	CString m_saveOldFunction;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTagDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GotoTags(bool declaration);
	const WWhizTagList* GetCurTagList();
	int TagPopup(int item, CWnd* thisWnd, CPoint* inPoint);

	CToolTipCtrl m_tooltip;

	void RefreshFromEdit();

	// Generated message map functions
	//{{AFX_MSG(CFindTagDialog)
	afx_msg void OnFtAbout();
	afx_msg void OnDblclkFtTags(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFtScopeFile();
	afx_msg void OnFtScopeProject();
	afx_msg void OnItemchangedFtTags(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFtShow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFtTypecombo();
	afx_msg void OnDestroy();
	afx_msg void OnFtCurrentfile();
	afx_msg void OnFtExtraprojects();
	afx_msg void OnFtScopeWorkspace();
	afx_msg void OnFtRefreshtags();
	afx_msg void OnRclickFtTags(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFtTag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTAGDIALOG_H__5C39DF21_24DB_11D2_A392_444553540000__INCLUDED_)
