///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindFileListCtrl.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/FindFileListCtrl.h $
// $Date:: 2/15/00 10:49p  $ $Revision:: 4    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#ifndef FINDFILELISTCTRL_H
#define FINDFILELISTCTRL_H

#include <afxcmn.h>
#include "WCollection.h"
#include "WWhizInterface3.h"

class CFindFileDialog;

class CFindFileListCtrl : public CListCtrl
{
public:
	CFindFileListCtrl();

	WArray<int>* m_foundFiles;
	WWhizFileList** m_fileList;

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
