#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "FindTagListCtrl.h"
#include "WorkspaceTags.h"

BEGIN_MESSAGE_MAP(CFindTagListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFindTagListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFindTagListCtrl::CFindTagListCtrl()
{
	m_nNextFree = 0;
}

void CFindTagListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		const WorkspaceTags::Tag* tag = (WorkspaceTags::Tag*)pDispInfo->item.lParam;

		CString strField;
		if (pDispInfo->item.iSubItem == 0)
		{
			strField = tag->GetIdent();
		}
		else
		{
			strField = tag->GetParentIdent();
		}
		LPTSTR pstrBuffer = AddPool(&strField); 
 
		pDispInfo->item.pszText = pstrBuffer;
	}       
	*pResult = 0;
}

LPTSTR CFindTagListCtrl::AddPool(CString* pstr)
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

