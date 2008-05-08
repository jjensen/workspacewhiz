#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "FindTagEdit.h"
#include "FindTagDialog.h"

CFindTagEdit::CFindTagEdit()
{
	m_pParent = NULL;
}

CFindTagEdit::~CFindTagEdit()
{
}


BEGIN_MESSAGE_MAP(CFindTagEdit, CEdit)
	//{{AFX_MSG_MAP(CFindTagEdit)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTagEdit message handlers

void CFindTagEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bSendToListBox = TRUE, bSendToEdit = TRUE;

	switch (nChar)
	{
		case VK_LEFT:
		case VK_RIGHT:
			bSendToListBox = FALSE;
			break;
	
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
			m_pParent->OnFiRefresh();
			bSendToListBox = FALSE;
			bSendToEdit = FALSE;
			break;
	}

	if (bSendToListBox == TRUE)
		m_pParent->m_tags.SendMessage(WM_KEYDOWN, nChar, nFlags << 16 | nRepCnt);

	if (bSendToEdit == TRUE)
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFindTagEdit::OnChange() 
{
	m_pParent->OnChangeFuncname();
}
