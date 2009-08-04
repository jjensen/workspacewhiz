///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindTagEdit.cpp $
// $Archive: /WWhizReg/FindTagEdit.cpp $
// $Date:: 4/12/01 11:26p  $ $Revision:: 8    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizReg.h"
#include "FindTagEdit.h"

CFindTagEdit::CFindTagEdit()
{
}

CFindTagEdit::~CFindTagEdit()
{
}


BEGIN_MESSAGE_MAP(CFindTagEdit, CComboBoxEx)
	//{{AFX_MSG_MAP(CFindTagEdit)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnChange)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTagEdit message handlers

void CFindTagEdit::OnChange() 
{
	if (HasExpiredHelper())
		return;

	NMCHAR nmChar;
	nmChar.hdr.hwndFrom = this->GetSafeHwnd();
	nmChar.hdr.idFrom = GetDlgCtrlID();
	nmChar.hdr.code = NM_CHAR;
	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmChar);
//	((CFindTagDialog*)GetParent())->OnChangeFuncname();
}

BOOL CFindTagEdit::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		bool bSendToListBox = true;
		bool bSendToEdit = true;
		
		if (!HasExpiredHelper())
		{
			switch (pMsg->wParam)
			{
				case VK_LEFT:
				case VK_RIGHT:
					bSendToListBox = FALSE;
					break;
					
				case VK_NEXT:
				case VK_PRIOR:
				case VK_DOWN:
				case VK_UP:
					bSendToEdit = FALSE;
					break;
					
				case VK_HOME:
				case VK_END:
					if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000)
						bSendToEdit = FALSE;
					else
						bSendToListBox = FALSE;
					break;
					
				case VK_F5:
					GetParent()->SendMessage(WM_USER_REFRESH, GetDlgCtrlID(), 0);
					bSendToListBox = FALSE;
					bSendToEdit = FALSE;
					break;
			}
		}
		
		if (!GetDroppedState())
		{
			if (bSendToListBox)
			{
				m_buddyList->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
			}

			if (!bSendToEdit)
				return TRUE;
		}
	}
	
	return CComboBoxEx::PreTranslateMessage(pMsg);
}
