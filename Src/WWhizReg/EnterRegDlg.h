///////////////////////////////////////////////////////////////////////////////
// $Workfile: EnterRegDlg.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/EnterRegDlg.h $
// $Date:: 5/09/00 8:00p   $ $Revision:: 3    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ENTERREGDLG_H__FA0DEBDF_48F7_47FD_9F77_232CADC05165__INCLUDED_)
#define AFX_ENTERREGDLG_H__FA0DEBDF_48F7_47FD_9F77_232CADC05165__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnterRegDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnterRegDlg dialog

class CEnterRegDlg : public CDialog
{
// Construction
public:
	CEnterRegDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterRegDlg)
	enum { IDD = IDD_WWR_ENTERREG };
	CEdit	m_fullName;
	CEdit	m_regCode;
	//}}AFX_DATA

	bool m_gotCode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterRegDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterRegDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTERREGDLG_H__FA0DEBDF_48F7_47FD_9F77_232CADC05165__INCLUDED_)
