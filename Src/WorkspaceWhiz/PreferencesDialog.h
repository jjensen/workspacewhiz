///////////////////////////////////////////////////////////////////////////////
// $Workfile: PreferencesDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PreferencesDialog.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PREFERENCESDIALOG_H__0AD96FA3_8999_11D3_9B27_444553540000__INCLUDED_)
#define AFX_PREFERENCESDIALOG_H__0AD96FA3_8999_11D3_9B27_444553540000__INCLUDED_

#pragma once
// PreferencesDialog.h : header file
//

#include "PrefKeyBindingsPage.h"
#include "PrefHeaderFlipPage.h"
#include "PrefTagsPage.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog

class CPreferencesDialog : public CHtmlHelpPropSheet
{
	DECLARE_DYNAMIC(CPreferencesDialog)

// Construction
public:
	CPreferencesDialog(UINT iSelectPage = 0, CWnd* pParentWnd = NULL);

// Attributes
public:
	CPrefKeyBindingsPage m_keyBindingsDlg;
	CPrefKeyBindingsPage m_keyBindingsDlg2;
	CPrefKeyBindingsPage m_keyBindingsDlg3;
	CPrefHeaderFlipPage m_headerFlipPage;
	CPrefTagsPage m_tagsPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDialog)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreferencesDialog();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreferencesDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDIALOG_H__0AD96FA3_8999_11D3_9B27_444553540000__INCLUDED_)
