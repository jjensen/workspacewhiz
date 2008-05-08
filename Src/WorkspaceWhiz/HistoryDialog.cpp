///////////////////////////////////////////////////////////////////////////////
// $Workfile: HistoryDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/HistoryDialog.cpp $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "HistoryDialog.h"
#include "AboutDialog.h"
#include "History.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryDialog dialog


CHistoryDialog::CHistoryDialog(CWnd* pParent /*=NULL*/)
	: HISTORY_DIALOG(CHistoryDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistoryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHistoryDialog::DoDataExchange(CDataExchange* pDX)
{
	HISTORY_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoryDialog)
	DDX_Control(pDX, IDC_H_FILES, m_files);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_H_RESET, m_butReset);
	DDX_Control(pDX, IDHELP, m_butHelp);
	DDX_Control(pDX, IDC_COM_ABOUT, m_butAbout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistoryDialog, HISTORY_DIALOG)
	//{{AFX_MSG_MAP(CHistoryDialog)
	ON_BN_CLICKED(IDC_COM_ABOUT, OnHAbout)
	ON_BN_CLICKED(IDC_H_RESET, OnHReset)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_H_FILES, OnDblclkHFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CHistoryDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDOK,					mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDHELP,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_ABOUT,		mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_H_RESET,			mdRepos,	mdNone)
	
	DYNAMIC_MAP_ENTRY(IDC_H_FILES,			mdResize,	mdResize)
END_DYNAMIC_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoryDialog message handlers

BOOL CHistoryDialog::OnInitDialog() 
{
	HISTORY_DIALOG::OnInitDialog();
	
	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	// Set the column names of the list control.
	CRect rect;
	DWORD dwStyle;
	dwStyle = m_files.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES ;
	m_files.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0,dwStyle );
	m_files.SetItemCountEx(History::GetCount());
	m_files.GetClientRect(&rect);
	m_files.InsertColumn(0, "Row", LVCFMT_LEFT, (int)(rect.Width() * 0.12), 0);
	m_files.InsertColumn(1, "Col", LVCFMT_LEFT, (int)(rect.Width() * 0.12), 1);
	m_files.InsertColumn(2, "Filename", LVCFMT_LEFT, (int)(rect.Width() * 0.76), 2);
	
	RefreshList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHistoryDialog::OnHAbout() 
{
	CAboutDialog dlg;
	dlg.DoModal();
}

void CHistoryDialog::OnHReset() 
{
	History::Reset();
	RefreshList();
}

void CHistoryDialog::OnOK() 
{
	if (m_files.GetItemCount() != 0)
	{
		CString filename;
		if (m_files.GetSelectedCount() == 0)
			m_files.SetItemState(0, 0, LVIS_SELECTED | LVIS_FOCUSED);

		int curSel = m_files.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

		History::Info info = *History::GetAt(curSel);
//		History::PreAdd();

		History::s_curPos = History::s_stack.FindIndex(curSel);
		History::Goto(&info);

		m_files.DeleteAllItems();
	}

	HISTORY_DIALOG::OnOK();
}

void CHistoryDialog::OnDestroy() 
{
	HISTORY_DIALOG::OnDestroy();
	
//	cdxCDynamicControlsManager::StoreWindowPosition(*this, _T("Dialogs\\HistoryDialog"));
}


void CHistoryDialog::RefreshList()
{
	// Turn off any redraw while we fill the list control.
	m_files.SetRedraw(FALSE);
	
	// Remove all items from the list.
	m_files.DeleteAllItems();

	m_files.SetItemCountEx(History::GetCount());
	if (History::GetCount() > 0)
	{
		// Find start index.
		int index = 0;
		POSITION curPos = History::s_stack.GetHeadPosition();
		while (curPos != History::s_curPos)
		{
			History::s_stack.GetNext(curPos);
			index++;
		}

		m_files.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}

	m_files.SetRedraw(TRUE);
}

void CHistoryDialog::OnSize(UINT nType, int cx, int cy) 
{
	HISTORY_DIALOG::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_files.m_hWnd))
	{
		CRect rect;
		m_files.GetClientRect(rect);
		m_files.SetColumnWidth(0, (int)(rect.Width() * 0.12));
		m_files.SetColumnWidth(1, (int)(rect.Width() * 0.12));
		m_files.SetColumnWidth(2, (int)(rect.Width() * 0.76));
	}
}

void CHistoryDialog::OnDblclkHFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}
