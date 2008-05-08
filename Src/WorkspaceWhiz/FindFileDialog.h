///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindFileDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/FindFileDialog.h $
// $Date: 2003/01/05 $ $Revision: #9 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_FINDFILEDIALOG_H__16CF1ABA_78AA_11D1_B8E6_00C06C466074__INCLUDED_)
#define AFX_FINDFILEDIALOG_H__16CF1ABA_78AA_11D1_B8E6_00C06C466074__INCLUDED_

#pragma once

#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CFindFileDialog dialog

#define FIND_FILE_DIALOG CHtmlHelpDialog

class CFindFileDialog : public FIND_FILE_DIALOG
{
// Construction
public:
	typedef enum { ScopeCurrentProject, ScopeEntireWorkspace, ScopeGlobal } FileScope;
	static FileScope s_fileScope;

	CFindFileDialog(CWnd* pParent = NULL);   // standard constructor
	 ~CFindFileDialog();

	CString m_lastFilename;
	CString m_lastPrefix;
	int m_lastPosition;
	CString m_oldFilename;
	CString m_oldPrefix;
	int m_created;
	WWhizFileList* m_fileList;
	WWhizProject* m_curProject;
	WArray<int> m_foundFiles;

	void RefreshList(LPCTSTR filename);

	static int FilePopup(const CString& fullName, CWnd* thisWnd, CPoint* inPoint);

// Dialog Data
	//{{AFX_DATA(CFindFileDialog)
	enum { IDD = IDD_FINDFILE };
	CButton	m_butCurrentFile;
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_butHelp;
	CButton	m_butExtraFiles;
	CButton	m_butAbout;
	//}}AFX_DATA

	CListCtrl*		m_files;
	CComboBoxEx*	m_edit;

	CString m_saveOldFilename;
	CString m_saveOldPrefix;
	int m_saveLastPosition;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void RefreshFromFilename();

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CFindFileDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPfAbout();
	afx_msg void OnDblclkFfFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFfExtraprojects();
	virtual void OnCancel();
//	afx_msg void OnFfRefresh();
	afx_msg void OnDestroy();
	afx_msg void OnRclickFfFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFfCurrentfile();
	afx_msg void OnFfScopeProject();
	afx_msg void OnFfScopeWorkspace();
	afx_msg void OnFfScopeGlobal();
	afx_msg void OnChangeFfFilename(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
public:
	afx_msg void OnCbnCloseupFfFilename();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDFILEDIALOG_H__16CF1ABA_78AA_11D1_B8E6_00C06C466074__INCLUDED_)
