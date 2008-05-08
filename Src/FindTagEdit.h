#ifndef FINDTAGEDIT_H
#define FINDTAGEDIT_H

class CFindTagDialog;

class CFindTagEdit : public CEdit
{
// Construction
public:
	CFindTagEdit();
	CFindTagDialog *m_pParent;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTagEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFindTagEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindTagEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif FINDTAGEDIT_H

