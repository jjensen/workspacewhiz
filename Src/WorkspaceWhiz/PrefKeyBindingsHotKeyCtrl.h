///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefKeyBindingsHotKeyCtrl.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefKeyBindingsHotKeyCtrl.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PREFKEYBINDINGSHOTKEYCTRL_H__0AD96FA9_8999_11D3_9B27_444553540000__INCLUDED_)
#define AFX_PREFKEYBINDINGSHOTKEYCTRL_H__0AD96FA9_8999_11D3_9B27_444553540000__INCLUDED_

#pragma once
// PrefKeyBindingsHotKeyCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefKeyBindingsHotKeyCtrl window

class CPrefKeyBindingsHotKeyCtrl : public CHotKeyCtrl
{
// Construction
public:
	CPrefKeyBindingsHotKeyCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefKeyBindingsHotKeyCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrefKeyBindingsHotKeyCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrefKeyBindingsHotKeyCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFKEYBINDINGSHOTKEYCTRL_H__0AD96FA9_8999_11D3_9B27_444553540000__INCLUDED_)
