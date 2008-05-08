///////////////////////////////////////////////////////////////////////////////
// $Workfile: StdAfx.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/StdAfx.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 7    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "atlimpl.cpp"

template<> inline void AFXAPI ConstructElementsEx<CString> (CString* pElements, int nCount)
{
	for (; nCount--; ++pElements)
		memcpy(pElements, &afxEmptyString, sizeof(*pElements));
}

template<> inline void AFXAPI DestructElementsEx<CString> (CString* pElements, int nCount)
{
	for (; nCount--; ++pElements)
		pElements->~CString();
}

template<> inline void AFXAPI CopyElementsEx<CString> (CString* pDest, const CString* pSrc, int nCount)
{
	for (; nCount--; ++pDest, ++pSrc)
		*pDest = *pSrc;
}


BOOL CListCtrl_SetItemState(CListCtrl& listCtrl, int nItem, UINT nState, UINT nStateMask)
{
	ASSERT(::IsWindow(listCtrl.m_hWnd));
	LVITEM lvi;
	lvi.stateMask = nStateMask;
	lvi.state = nState;
	return (BOOL) ::SendMessage(listCtrl.m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)&lvi);
}


BOOL CListCtrl_SetItemCountEx(CListCtrl& listCtrl, int iCount, DWORD dwFlags /* = LVSICF_NOINVALIDATEALL */)
{
	ASSERT(::IsWindow(listCtrl.m_hWnd));

	// can't have dwFlags on a control that isn't virutal
	ASSERT(dwFlags == 0 || (listCtrl.GetStyle() & LVS_OWNERDATA));

	return (BOOL) ::SendMessage(listCtrl.m_hWnd, LVM_SETITEMCOUNT, (WPARAM) iCount,
		(LPARAM) dwFlags);
}


POSITION CListCtrl_GetFirstSelectedItemPosition(CListCtrl& listCtrl)
{
	ASSERT(::IsWindow(listCtrl.m_hWnd));
	return (POSITION) (1 + listCtrl.GetNextItem(-1, LVIS_SELECTED));
}


int CListCtrl_GetNextSelectedItem(CListCtrl& listCtrl, POSITION& pos)
{
	ASSERT(::IsWindow(listCtrl.m_hWnd));
	int nOldPos = (int)pos-1;
	pos = (POSITION) (1+listCtrl.GetNextItem(nOldPos, LVIS_SELECTED));
	return nOldPos;
}


int CToolTipCtrl_SetMaxTipWidth(CToolTipCtrl& toolTipCtrl, int iWidth)
{
	ASSERT(::IsWindow(toolTipCtrl.m_hWnd));
	return (int) ::SendMessage(toolTipCtrl.m_hWnd, TTM_SETMAXTIPWIDTH, 0, iWidth);
}


void CToolTipCtrl_SetDelayTime(CToolTipCtrl& toolTipCtrl, DWORD dwDuration, int iTime)
{
	ASSERT(::IsWindow(toolTipCtrl.m_hWnd));
	::SendMessage(toolTipCtrl.m_hWnd, TTM_SETDELAYTIME, dwDuration, MAKELPARAM(iTime, 0));
}


