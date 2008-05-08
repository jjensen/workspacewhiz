///////////////////////////////////////////////////////////////////////////////
// $Workfile: ExpiredDlg.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/ExpiredDlg.h $
// $Date:: 5/09/00 8:00p   $ $Revision:: 5    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_EXPIREDDLG_H__ED392D2E_AB8B_4C30_B04D_B55995B05A3F__INCLUDED_)
#define AFX_EXPIREDDLG_H__ED392D2E_AB8B_4C30_B04D_B55995B05A3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExpiredDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExpiredDlg dialog

class CExpiredDlg : public CDialog
{
// Construction
public:
	CExpiredDlg(CWnd* pParent = NULL);   // standard constructor

	int m_numDays;

// Dialog Data
	//{{AFX_DATA(CExpiredDlg)
	enum { IDD = IDD_WWR_EXPIRED };
	CProgressCtrl	m_timeLeftBar;
	CStatic	m_timeLeft;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpiredDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillInControls();

	// Generated message map functions
	//{{AFX_MSG(CExpiredDlg)
	afx_msg void OnWwreEvaluate();
	afx_msg void OnWwreEntercode();
	virtual BOOL OnInitDialog();
	afx_msg void OnWwreRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPIREDDLG_H__ED392D2E_AB8B_4C30_B04D_B55995B05A3F__INCLUDED_)
