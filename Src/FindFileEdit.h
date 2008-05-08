#ifndef FINDFILEEDIT_H
#define FINDFILEEDIT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CFindFileEdit window

class CFindFileDialog;

class CFindFileEdit : public CEdit
{
// Construction
public:
	CFindFileEdit();
	CFindFileDialog *m_pParent;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFileEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFindFileEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindFileEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif FINDFINDEDIT
