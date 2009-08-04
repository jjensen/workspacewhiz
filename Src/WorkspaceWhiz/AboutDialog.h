///////////////////////////////////////////////////////////////////////////////
// $Workfile: AboutDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/AboutDialog.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ABOUTDIALOG_H__CC7F62A0_6198_11D3_9B27_444553540000__INCLUDED_)
#define AFX_ABOUTDIALOG_H__CC7F62A0_6198_11D3_9B27_444553540000__INCLUDED_

#pragma once
// AboutDialog.h : header file
//

interface IWebBrowser2;

#define ABOUT_DIALOG cdxCDynamicDialog

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog

class CAboutDialog : public ABOUT_DIALOG
{
// Construction
public:
	CAboutDialog(CWnd* pParent = NULL);   // standard constructor
	~CAboutDialog();

// Dialog Data
	//{{AFX_DATA(CAboutDialog)
	enum { IDD = IDD_ABOUTBOX };
	CButton	m_butOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd m_wndBrowser;
	IWebBrowser2* m_pBrowserApp;

	// Generated message map functions
	//{{AFX_MSG(CAboutDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAbRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDIALOG_H__CC7F62A0_6198_11D3_9B27_444553540000__INCLUDED_)
