///////////////////////////////////////////////////////////////////////////////
// $Workfile: HistoryListCtrl.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/HistoryListCtrl.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#ifndef HISTORYLISTCTRL_H
#define HISTORYLISTCTRL_H

#include <afxcmn.h>

class CHistoryDialog;

class CHistoryListCtrl : public CListCtrl
{
public:
	CHistoryListCtrl();

protected:
	LPTSTR AddPool(CString* pstr);

    CString m_strCPool[3];
    LPTSTR  m_pstrPool[3];
    int     m_nNextFree;


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHistoryListCtrl)
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif HISTORYLISTCTRL_H
