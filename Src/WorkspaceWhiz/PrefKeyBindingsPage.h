///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefKeyBindingsPage.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefKeyBindingsPage.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 12   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PREFKEYBINDINGSPAGE_H__0AD96FA2_8999_11D3_9B27_444553540000__INCLUDED_)
#define AFX_PREFKEYBINDINGSPAGE_H__0AD96FA2_8999_11D3_9B27_444553540000__INCLUDED_

#pragma once
// PrefKeyBindingsPage.h : header file
//

#include "PrefKeyBindingsHotKeyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefKeyBindingsPage dialog

class CPrefKeyBindingsPage : public CHtmlHelpPropPage
{
	DECLARE_DYNCREATE(CPrefKeyBindingsPage)

// Construction
public:
	CPrefKeyBindingsPage();
	CPrefKeyBindingsPage(int whichPage);
	~CPrefKeyBindingsPage();

// Dialog Data
	//{{AFX_DATA(CPrefKeyBindingsPage)
	enum { IDD = IDD_PREF_KEYBINDINGS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefKeyBindingsPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CArray<CPrefKeyBindingsHotKeyCtrl, CPrefKeyBindingsHotKeyCtrl&> m_subclassedHotKeys;
	
	void FillInControls();
	void TranslateHKToDS(CString& commandStr, WORD vk, WORD mod);
	void TranslateDSToHK(CString commandStr, WORD& vk, WORD& mod);

	int m_firstItem;
	int m_lastItem;

	// Generated message map functions
	afx_msg void OnPkbKeycheck(UINT nID);
	//{{AFX_MSG(CPrefKeyBindingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPkbAssign();
	afx_msg void OnPkbReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFKEYBINDINGSPAGE_H__0AD96FA2_8999_11D3_9B27_444553540000__INCLUDED_)
