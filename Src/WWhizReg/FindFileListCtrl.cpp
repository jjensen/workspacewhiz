///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindFileListCtrl.cpp $
// $Archive: /WWhizReg/FindFileListCtrl.cpp $
// $Date:: 10/17/00 12:30a $ $Revision:: 9    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "FindFileListCtrl.h"

BEGIN_MESSAGE_MAP(CFindFileListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFindFileListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFindFileListCtrl::CFindFileListCtrl()
{
	m_nNextFree = 0;
}

void CFindFileListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (HasExpiredHelper())
		return;

	LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM* item = &pDispInfo->item;

	item->mask = LVIF_TEXT;

	int index = (*m_foundFiles)[item->iItem];
	WWhizFileList& fileList = **m_fileList;
	const WWhizFile& file = *fileList.Get(index);

	CString strField;
	if (item->iSubItem == 0)
	{
		strField = file.GetTitle();
		if (!file.GetExt().IsEmpty())
			strField += "." + file.GetExt();
	}
	else
		strField = file.GetPath();
	LPTSTR pstrBuffer = AddPool(&strField);
	item->pszText = pstrBuffer;

	*pResult = 0;
}

LPTSTR CFindFileListCtrl::AddPool(CString* pstr)
{
	LPTSTR pstrRetVal;
	int nOldest = m_nNextFree;

	m_strCPool[m_nNextFree] = *pstr;
	pstrRetVal = m_strCPool[m_nNextFree].LockBuffer();
	m_pstrPool[m_nNextFree++] = pstrRetVal;
	m_strCPool[nOldest].ReleaseBuffer();

	if (m_nNextFree == 3)
			m_nNextFree = 0;
	return pstrRetVal;
}

