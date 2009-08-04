///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindTagListCtrl.cpp $
// $Archive: /WWhizReg/FindTagListCtrl.cpp $
// $Date:: 11/09/00 9:47p  $ $Revision:: 12   $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizReg.h"
#include "FindTagListCtrl.h"

BEGIN_MESSAGE_MAP(CFindTagListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFindTagListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFindTagListCtrl::CFindTagListCtrl()
	: m_tagArray(NULL)
	, m_nNextFree(0)

{
}

void CFindTagListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (HasExpiredHelper())
		return;

	LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM* item = &pDispInfo->item;

	const WWhizTag* tag = (*m_tagArray)[item->iItem];

	item->mask = LVIF_TEXT;
	if (item->iSubItem == 0)
	{
		switch (tag->GetType())
		{
			case WWhizTag::CLASS:		item->pszText = "Class";		break;
			case WWhizTag::DECLARATION:	item->pszText = "Decl";			break;
			case WWhizTag::DEFINE:		item->pszText = "Define";		break;
			case WWhizTag::ENUM:		item->pszText = "Enum";			break;
			case WWhizTag::ENUMMEMBER:	item->pszText = "EnumVal";		break;
			case WWhizTag::FUNCTION:	item->pszText = "Function";		break;
			case WWhizTag::MEMBER:		item->pszText = "MemVar";		break;
			case WWhizTag::NAMESPACE:	item->pszText = "NSpace";		break;
			case WWhizTag::STRUCTURE:	item->pszText = "Struct";		break;
			case WWhizTag::TYPEDEF:		item->pszText = "Typedef";		break;
			case WWhizTag::UNION:		item->pszText = "Union";		break;
			case WWhizTag::VARIABLE:	item->pszText = "Variable";		break;
			case WWhizTag::FILE:		item->pszText = "File";		break;
		}
	}
	else if (item->iSubItem == 1)
	{
		LPCTSTR text = tag->GetNamespace();
		if (item->pszText[0] == 0  &&  tag->GetBuddy())
		{
			item->pszText = (LPTSTR)(LPCTSTR)tag->GetBuddy()->GetNamespace();
		}
		item->pszText = (LPTSTR)text;
	}
	else if (item->iSubItem == 2)
	{
		item->pszText = (LPTSTR)(LPCTSTR)tag->GetParentIdent();
	}
	else if (item->iSubItem == 3)
	{
		item->pszText = (LPTSTR)(LPCTSTR)tag->GetIdent();
	}
	else if (item->iSubItem == 4)
	{
		CString fileName = tag->GetFile()->GetTitle();
		if (!tag->GetFile()->GetExt().IsEmpty())
			fileName += "." + tag->GetFile()->GetExt();

		LPTSTR pstrBuffer = AddPool(&fileName);
		item->pszText = pstrBuffer;
	}
/*	else if (item->iSubItem == 5)
	{
		CString fileName = tag->GetFilename();
		int lastSlash = fileName.ReverseFind('\\');
		fileName = fileName.Left(lastSlash + 1);
		LPTSTR pstrBuffer = AddPool(&fileName);
		item->pszText = pstrBuffer;
	}*/
	else if (item->iSubItem == 5)
	{
		CString searchString = tag->GetSearchString();
		searchString.TrimLeft("^");
		searchString.TrimRight("$");
		searchString.Replace('\t', ' ');
		LPTSTR pstrBuffer = AddPool(&searchString);
		item->pszText = pstrBuffer;
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

