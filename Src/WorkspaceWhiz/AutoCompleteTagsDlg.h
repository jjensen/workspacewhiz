///////////////////////////////////////////////////////////////////////////////
// $Workfile: AutoCompleteTagsDlg.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/AutoCompleteTagsDlg.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUTOCOMPLETETAGSDLG_H__A5DD52A4_D386_11D3_BE56_D365DEF62C67__INCLUDED_)
#define AFX_AUTOCOMPLETETAGSDLG_H__A5DD52A4_D386_11D3_BE56_D365DEF62C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoCompleteTagsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoCompleteTagsDlg dialog

class CAutoCompleteTagsDlg : public CHtmlHelpDialog
{
// Construction
public:
	CAutoCompleteTagsDlg(CWnd* pParent = NULL);   // standard constructor
	void SetTagName(CString tag);

	int m_selectedIndex;
	const WWhizTag* m_selectedTag;
	CString m_editText;
	CString m_autoParent;

// Dialog Data
	//{{AFX_DATA(CAutoCompleteTagsDlg)
	enum { IDD = IDD_AUTOCOMPLETETAG };
	//}}AFX_DATA

	CComboBoxEx*	m_tagParent;
	CComboBoxEx*	m_edit;
	CListCtrl*		m_tagListCtrl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoCompleteTagsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RefreshList(LPCTSTR name);
	TagArray m_tagArray;
	int m_lastPosition;

	// Generated message map functions
	//{{AFX_MSG(CAutoCompleteTagsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkFtTags(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFtName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOCOMPLETETAGSDLG_H__A5DD52A4_D386_11D3_BE56_D365DEF62C67__INCLUDED_)
