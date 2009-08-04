///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindFileEdit.h $
// $Archive: /WWhizReg/FindFileEdit.h $
// $Date:: 4/12/01 8:19p   $ $Revision:: 6    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#ifndef FINDFILEEDIT_H
#define FINDFILEEDIT_H

#include <afxcmn.h>

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CFindFileEdit window

class CFindFileListCtrl;

class CFindFileEdit : public CComboBoxEx
{
// Construction
public:
	CFindFileEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFileEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFindFileEdit();

	CListCtrl* m_buddyList;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindFileEdit)
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif FINDFINDEDIT
