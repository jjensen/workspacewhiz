#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "FindFileEdit.h"
#include "FindFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindFileEdit

CFindFileEdit::CFindFileEdit()
{
	m_pParent = NULL;
}

CFindFileEdit::~CFindFileEdit()
{
}


BEGIN_MESSAGE_MAP(CFindFileEdit, CEdit)
	//{{AFX_MSG_MAP(CFindFileEdit)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFileEdit message handlers

void CFindFileEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bSendToListBox = TRUE, bSendToEdit = TRUE;

	switch (nChar)
	{
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
	}

	if (bSendToListBox == TRUE)
		m_pParent->m_files.SendMessage(WM_KEYDOWN, nChar, nFlags << 16 | nRepCnt);

	if (bSendToEdit == TRUE)
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFindFileEdit::OnChange() 
{
	m_pParent->OnChangeFfFilename();
}
