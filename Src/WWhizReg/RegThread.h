///////////////////////////////////////////////////////////////////////////////
// $Workfile: RegThread.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/RegThread.h $
// $Date:: 5/09/00 8:00p   $ $Revision:: 2    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_REGTHREAD_H__FF3B2C8E_1584_4749_9C45_66D3753B0989__INCLUDED_)
#define AFX_REGTHREAD_H__FF3B2C8E_1584_4749_9C45_66D3753B0989__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CRegThread thread

class CRegThread : public CWinThread
{
	DECLARE_DYNCREATE(CRegThread)
protected:
	CRegThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRegThread();

	// Generated message map functions
	//{{AFX_MSG(CRegThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
#ifdef WWHIZ_VC6
    afx_msg LRESULT OnInitCheck(WPARAM wParam, LPARAM lParam);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
    afx_msg void OnInitCheck(WPARAM wParam, LPARAM lParam);
#endif WWHIZ_VSNET

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGTHREAD_H__FF3B2C8E_1584_4749_9C45_66D3753B0989__INCLUDED_)


