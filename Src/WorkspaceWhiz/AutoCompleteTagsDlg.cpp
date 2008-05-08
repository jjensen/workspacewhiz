///////////////////////////////////////////////////////////////////////////////
// $Workfile: AutoCompleteTagsDlg.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/AutoCompleteTagsDlg.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 19   $ $Author: Jjensen $
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
#include "AutoCompleteTagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoCompleteTagsDlg dialog


CAutoCompleteTagsDlg::CAutoCompleteTagsDlg(CWnd* pParent /*=NULL*/)
	: CHtmlHelpDialog(CAutoCompleteTagsDlg::IDD, pParent),
	m_selectedIndex(-1),
	m_selectedTag(NULL),
	m_tagParent(NULL),
	m_edit(NULL),
	m_tagListCtrl(NULL)
{
	//{{AFX_DATA_INIT(CAutoCompleteTagsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAutoCompleteTagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CHtmlHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoCompleteTagsDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoCompleteTagsDlg, CHtmlHelpDialog)
	//{{AFX_MSG_MAP(CAutoCompleteTagsDlg)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_FT_TAGS, OnDblclkFtTags)
	ON_NOTIFY(NM_CHAR, IDC_FT_NAME, OnChangeFtName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CAutoCompleteTagsDlg, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDC_FT_TAGS,		mdResize,	mdResize)
END_DYNAMIC_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoCompleteTagsDlg message handlers

BOOL CAutoCompleteTagsDlg::OnInitDialog() 
{
	CHtmlHelpDialog::OnInitDialog();
	
	// Registered tag controls.
	g_wwhizReg->CreateTagControls(this, IDD, m_tagParent, m_edit, m_tagListCtrl, &m_tagArray );
	
	m_lastPosition = -1;
	
	RefreshList("");
	
	CRect listWindowRect;
	m_tagListCtrl->GetWindowRect(listWindowRect);
	SetWindowPos(NULL, 0, 0, listWindowRect.Width(), listWindowRect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAutoCompleteTagsDlg::RefreshList(LPCTSTR pName) 
{
	CString name = pName;
	if (name.Find(';') == -1)
	{
		if (!m_autoParent.IsEmpty())
			name += m_autoParent;
	}

	// Turn off any redraw while we fill the list control.
	m_tagListCtrl->SetRedraw(FALSE);
	
	const WWhizTagList* tags = &g_wwhizInterface->GetTagList();

	// Perform the matching.
	m_tagArray.RemoveAll();
	g_wwhizInterface->MatchTag(*tags, name);
	if (g_wwhizInterface->GetTagMatchCount() == 0)
	{
		// Remove all items from the list.
		CListCtrl_SetItemCountEx(*m_tagListCtrl, 0);
		m_tagListCtrl->SetRedraw(TRUE);
		m_tagListCtrl->Invalidate();

		return;
	}

	// Set the number of items to the maximum.
	m_tagArray.SetSize(g_wwhizInterface->GetTagMatchCount());

	// Start looking for it.
	int curPos = 0;
	const WWhizTag* tag = g_wwhizInterface->GetTagMatchHead();
	CString lastIdent;
	for (; tag; tag = tag->GetMatchNext())
	{
		// Skip redundant tag identifiers.
		if (tag->GetIdent() == lastIdent)
			continue;
		if (!g_wwhizInterface->GetTagUseType(tag->GetType()))
			continue;

		m_tagArray[curPos] = tag;
		lastIdent = tag->GetIdent();

		curPos++;
	}

	m_tagArray.SetSize(curPos);
	CListCtrl_SetItemCountEx(*m_tagListCtrl, curPos);

	POSITION pos = CListCtrl_GetFirstSelectedItemPosition(*m_tagListCtrl);
	while (pos)
	{
		int nItem = CListCtrl_GetNextSelectedItem(*m_tagListCtrl, pos);
		CListCtrl_SetItemState(*m_tagListCtrl, nItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
	}

	if (m_lastPosition >= m_tagListCtrl->GetItemCount())
		m_lastPosition = -1;
	if (m_tagListCtrl->GetItemCount() > 0)
	{
		int setPos;
		if (m_lastPosition == -1)
			setPos = 0;
		else
			setPos = m_lastPosition;
		CListCtrl_SetItemState(*m_tagListCtrl, setPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_tagListCtrl->EnsureVisible(setPos, FALSE);
	}
	else
	{
		m_lastPosition = -1;
	}

	m_tagListCtrl->SetRedraw(TRUE);
	m_tagListCtrl->Invalidate();
}


void CAutoCompleteTagsDlg::OnDestroy() 
{
	CHtmlHelpDialog::OnDestroy();
	
	g_wwhizReg->DestroyTagControls();
}

void CAutoCompleteTagsDlg::OnOK() 
{
	if (m_tagListCtrl->GetItemCount() != 0)
	{
		if (m_tagListCtrl->GetSelectedCount() == 0)
			CListCtrl_SetItemState(*m_tagListCtrl, 0, 0, LVIS_SELECTED | LVIS_FOCUSED);
		int nCount = m_tagListCtrl->GetSelectedCount();

		m_selectedIndex = m_tagListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		if (m_selectedIndex != -1)
			m_selectedTag = m_tagArray[m_selectedIndex];
		m_edit->GetWindowText(m_editText);
	}
	
	CHtmlHelpDialog::OnOK();
}


void CAutoCompleteTagsDlg::OnDblclkFtTags(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CAutoCompleteTagsDlg::OnChangeFtName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString tag;
	m_edit->GetWindowText(tag);
	RefreshList(tag);
}

void CAutoCompleteTagsDlg::SetTagName(CString tag)
{
	m_edit->SetWindowText(tag);
	int lineLength = m_edit->LineLength();
	m_edit->SetSel(lineLength, -1);
	RefreshList(tag);
}
