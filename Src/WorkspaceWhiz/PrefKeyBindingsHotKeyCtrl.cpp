///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefKeyBindingsHotKeyCtrl.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefKeyBindingsHotKeyCtrl.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 6    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "workspacewhiz.h"
#include "PrefKeyBindingsHotKeyCtrl.h"
#include "PrefKeyBindingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefKeyBindingsHotKeyCtrl

CPrefKeyBindingsHotKeyCtrl::CPrefKeyBindingsHotKeyCtrl()
{
}

CPrefKeyBindingsHotKeyCtrl::~CPrefKeyBindingsHotKeyCtrl()
{
}


BEGIN_MESSAGE_MAP(CPrefKeyBindingsHotKeyCtrl, CHotKeyCtrl)
	//{{AFX_MSG_MAP(CPrefKeyBindingsHotKeyCtrl)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefKeyBindingsHotKeyCtrl message handlers

void CPrefKeyBindingsHotKeyCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	((CPrefKeyBindingsPage*)GetParent())->SetModified(TRUE);
	
	CHotKeyCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CPrefKeyBindingsHotKeyCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN  &&  pMsg->wParam == VK_RETURN)
	{
		WORD vk;
		WORD mod;
		GetHotKey(vk, mod);
		SetHotKey(VK_RETURN, mod);
		return TRUE;
	}

	return CHotKeyCtrl::PreTranslateMessage(pMsg);
}
