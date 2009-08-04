///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindFileEdit.cpp $
// $Archive: /WWhizReg/FindFileEdit.cpp $
// $Date:: 4/12/01 11:26p  $ $Revision:: 10   $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizReg.h"
#include "FindFileEdit.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindFileEdit

CFindFileEdit::CFindFileEdit()
{
}

CFindFileEdit::~CFindFileEdit()
{
}


BEGIN_MESSAGE_MAP(CFindFileEdit, CComboBoxEx)
	//{{AFX_MSG_MAP(CFindFileEdit)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnChange)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFileEdit message handlers

void CFindFileEdit::OnChange() 
{
	if (HasExpiredHelper())
		return;

	NMCHAR nmChar;
	nmChar.hdr.hwndFrom = this->GetSafeHwnd();
	nmChar.hdr.idFrom = GetDlgCtrlID();
	nmChar.hdr.code = NM_CHAR;
	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmChar);
}

BOOL CFindFileEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		bool bSendToList = true;
		BOOL bSendToEdit = TRUE;
		
		if (!HasExpiredHelper())
		{
			switch (pMsg->wParam)
			{
				case VK_NEXT:
				case VK_PRIOR:
				case VK_DOWN:
				case VK_UP:
					bSendToEdit = FALSE;
					break;
					
				case VK_LEFT:
				case VK_RIGHT:
					bSendToList = FALSE;
					break;
					
				case VK_HOME:
				case VK_END:
					if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000)
					{
						int curSel = m_buddyList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
						if (curSel != -1)
							m_buddyList->SetItemState(curSel, 0, LVIS_SELECTED | LVIS_FOCUSED);
						int whichSel = 0;
						if (pMsg->wParam == VK_END)
							whichSel = m_buddyList->GetItemCount() - 1;
						m_buddyList->SetItemState(whichSel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
						m_buddyList->EnsureVisible(whichSel, FALSE);
						bSendToEdit = FALSE;
					}
					
					bSendToList = false;
					break;
			}
		}
		
		if (!GetDroppedState())
		{
			if (bSendToList)
			{
				m_buddyList->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
			}
		
			if (!bSendToEdit)
				return TRUE;
		}
	}
	
	return CComboBoxEx::PreTranslateMessage(pMsg);
}
