///////////////////////////////////////////////////////////////////////////////
// $Workfile: HistoryListCtrl.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/HistoryListCtrl.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "HistoryListCtrl.h"
#include "HistoryDialog.h"
#include "History.h"

BEGIN_MESSAGE_MAP(CHistoryListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CHistoryListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CHistoryListCtrl::CHistoryListCtrl()
{
	m_nNextFree = 0;
}

void CHistoryListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM* item = &pDispInfo->item;

	item->mask = LVIF_TEXT;

	int index = item->iItem;
	History::Info* info = History::GetAt(index);

	CString strField;
	if (item->iSubItem == 0)
		strField.Format("%d", info->m_y);
	else if (item->iSubItem == 1)
		strField.Format("%d", info->m_x);
	else if (item->iSubItem == 2)
		strField = info->m_filename;

	LPTSTR pstrBuffer = AddPool(&strField);
	item->pszText = pstrBuffer;

	*pResult = 0;
}

LPTSTR CHistoryListCtrl::AddPool(CString* pstr)
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

