#if !defined(AFX_FINDFILEDIALOG_H__16CF1ABA_78AA_11D1_B8E6_00C06C466074__INCLUDED_)
#define AFX_FINDFILEDIALOG_H__16CF1ABA_78AA_11D1_B8E6_00C06C466074__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CFindFileDialog dialog

#include "FindFileEdit.h"
#include "FindFileListCtrl.h"
#include "WorkspaceInfo.h"

class CFindFileDialog : public CDialog
{
// Construction
public:
	 ~CFindFileDialog();
	static IApplication * m_pApplication;
	CString m_lastFilename;
	int m_lastPosition;
	CString m_oldFilename;
	int m_created;
	FileList* m_fileList;
	bool m_workspaceOpen;
	CArray<int, int> m_foundFiles;

	void RefreshList(LPCTSTR filename);

	CFindFileDialog(CWnd* pParent = NULL);   // standard constructor
	void OnChangeFfFilename();

// Dialog Data
	//{{AFX_DATA(CFindFileDialog)
	enum { IDD = IDD_FINDFILE };
	CFindFileListCtrl	m_files;
	CFindFileEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindFileDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPfAbout();
	afx_msg void OnDblclkFfFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFfExtraprojects();
	afx_msg void OnFfPreferences();
	virtual void OnCancel();
	afx_msg void OnFfRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDFILEDIALOG_H__16CF1ABA_78AA_11D1_B8E6_00C06C466074__INCLUDED_)
