///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindTagEdit.h $
// $Archive: /WWhizReg/FindTagEdit.h $
// $Date:: 4/12/01 8:19p   $ $Revision:: 5    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#ifndef FINDTAGEDIT_H
#define FINDTAGEDIT_H

class CFindTagDialog;

class CFindTagEdit : public CComboBoxEx
{
// Construction
public:
	CFindTagEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTagEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFindTagEdit();

	CListCtrl* m_buddyList;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindTagEdit)
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif FINDTAGEDIT_H

