#if !defined(AFX_EXTRAPROJECTSDIALOG_H__C59ABF61_B178_11D1_84EF_444553540000__INCLUDED_)
#define AFX_EXTRAPROJECTSDIALOG_H__C59ABF61_B178_11D1_84EF_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExtraProjectsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtraProjectsDialog dialog

class CExtraProjectsDialog : public CDialog
{
// Construction
public:
	CExtraProjectsDialog(CWnd* pParent = NULL);   // standard constructor

	CString workspaceLocation;

// Dialog Data
	//{{AFX_DATA(CExtraProjectsDialog)
	enum { IDD = IDD_EXTRAPROJECTS };
	CEdit	m_filename;
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtraProjectsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtraProjectsDialog)
	afx_msg void OnEpAdd();
	afx_msg void OnEpRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnEpBrowse();
	virtual void OnOK();
	afx_msg void OnDblclkEpList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRAPROJECTSDIALOG_H__C59ABF61_B178_11D1_84EF_444553540000__INCLUDED_)
