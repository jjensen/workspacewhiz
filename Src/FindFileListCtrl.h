#ifndef FINDFILELISTCTRL_H
#define FINDFILELISTCTRL_H

#include <afxcmn.h>

class CFindFileDialog;

class CFindFileListCtrl : public CListCtrl
{
public:
	CFindFileListCtrl();

	CFindFileDialog* m_parent;

protected:
	LPTSTR AddPool(CString* pstr);

    CString m_strCPool[3];
    LPTSTR  m_pstrPool[3];
    int     m_nNextFree;


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindFileListCtrl)
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif FINDFILELISTCTRL_H
