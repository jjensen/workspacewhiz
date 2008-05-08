///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefHeaderFlipPage.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefHeaderFlipPage.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 7    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PREFHEADERFLIPPAGE_H__36B38223_8A5A_11D3_9B27_444553540000__INCLUDED_)
#define AFX_PREFHEADERFLIPPAGE_H__36B38223_8A5A_11D3_9B27_444553540000__INCLUDED_

#pragma once
// PrefHeaderFlipPage.h : header file
//

#include "PrefHeaderFlipListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefHeaderFlipPage dialog

class CPrefHeaderFlipPage : public CHtmlHelpPropPage
{
	DECLARE_DYNCREATE(CPrefHeaderFlipPage)

// Construction
public:
	CPrefHeaderFlipPage();
	~CPrefHeaderFlipPage();

// Dialog Data
	//{{AFX_DATA(CPrefHeaderFlipPage)
	enum { IDD = IDD_PREF_HEADERFLIP };
	CPrefHeaderFlipListBox	m_extList;
	CEdit	m_extEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefHeaderFlipPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillInControls();
	
	// Generated message map functions
	//{{AFX_MSG(CPrefHeaderFlipPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPhfAdd();
	afx_msg void OnPhfRemove();
	afx_msg void OnPhfReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFHEADERFLIPPAGE_H__36B38223_8A5A_11D3_9B27_444553540000__INCLUDED_)
