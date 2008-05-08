///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindTagListCtrl.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/FindTagListCtrl.h $
// $Date:: 2/15/00 10:49p  $ $Revision:: 5    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#ifndef FINDTAGLISTCTRL_H
#define FINDTAGLISTCTRL_H

#include <afxcmn.h>

class CFindTagListCtrl : public CListCtrl
{
public:
	CFindTagListCtrl();
	TagArray* m_tagArray;

protected:
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFindTagListCtrl)
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif FINDTAGLISTCTRL_H
