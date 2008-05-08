#if !defined(AFX_FINDTAGDIALOG_H__5C39DF21_24DB_11D2_A392_444553540000__INCLUDED_)
#define AFX_FINDTAGDIALOG_H__5C39DF21_24DB_11D2_A392_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FindTagDialog.h : header file
//

#include <afxcmn.h>

#include "FindTagEdit.h"
#include "FindTagListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CFindTagDialog dialog

class CFindTagDialog : public CDialog
{
// Construction
public:
	typedef enum { eProjectScope, eFileScope } TagScope;

	CFindTagDialog(CWnd* pParent = NULL);   // standard constructor
	~CFindTagDialog();
	void OnChangeFuncname();
	void RefreshList(const char *name);

	static IApplication * m_pApplication;
	static CString m_lastFunction;
	static int m_lastPosition;
	static CString m_oldFunction;
	bool m_created;
	TagScope m_eTagScope;

// Dialog Data
	//{{AFX_DATA(CFindTagDialog)
	enum { IDD = IDD_FINDTAG };
	CFindTagEdit	m_edit;
	CFindTagListCtrl	m_tags;
	//}}AFX_DATA
	afx_msg void OnFiRefresh();



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTagDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LPTSTR AddPool(CString* pstr);

	// Generated message map functions
	//{{AFX_MSG(CFindTagDialog)
	afx_msg void OnFtAbout();
	afx_msg void OnFiPreferences();
	afx_msg void OnDblclkFtTags(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFilescopeRadio();
	afx_msg void OnProjectscopeRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTAGDIALOG_H__5C39DF21_24DB_11D2_A392_444553540000__INCLUDED_)
