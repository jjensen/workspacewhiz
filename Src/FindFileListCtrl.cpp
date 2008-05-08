#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "FindFileListCtrl.h"
#include "WorkspaceInfo.h"
#include "FindFileDialog.h"

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
	LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		int index = m_parent->m_foundFiles[pDispInfo->item.iItem];
		const File& file = (*m_parent->m_fileList)[index];

		CString strField;
		if (pDispInfo->item.iSubItem == 0)
		{
			strField = file.GetTitle() + "." + file.GetExt();
		}
		else
		{
			strField = file.GetPath();
		}
		LPTSTR pstrBuffer = AddPool(&strField);

		pDispInfo->item.pszText = pstrBuffer;
	}
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

