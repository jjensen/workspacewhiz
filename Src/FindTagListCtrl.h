#ifndef FINDTAGLISTCTRL_H
#define FINDTAGLISTCTRL_H

#include <afxcmn.h>

class CFindTagListCtrl : public CListCtrl
{
public:
	CFindTagListCtrl();

protected:
	LPTSTR AddPool(CString* pstr);

    CString m_strCPool[3];
    LPTSTR  m_pstrPool[3];
    int     m_nNextFree;
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindTagListCtrl)
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif FINDTAGLISTCTRL_H
