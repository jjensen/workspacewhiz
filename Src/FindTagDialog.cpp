// FindTagDialog.cpp : implementation file
//

#include "stdafx.h"
#include "workspaceutils.h"
#include "FindTagDialog.h"
//#include "EditFilename.h"
#include "WorkspaceTags.h"
#include "WorkspaceInfo.h"
#include "FindFileDialog.h"
#include "FIPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTagDialog dialog

IApplication *CFindTagDialog::m_pApplication = NULL;
int CFindTagDialog::m_lastPosition = -1;
CString CFindTagDialog::m_lastFunction;
CString CFindTagDialog::m_oldFunction;

CFindTagDialog::CFindTagDialog(CWnd* pParent /*=NULL*/)
	: m_created(false), CDialog(CFindTagDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindTagDialog)
	//}}AFX_DATA_INIT
}


CFindTagDialog::~CFindTagDialog()
{
	delete m_tags;
}

void CFindTagDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTagDialog)
	DDX_Control(pDX, IDC_FT_NAME, m_edit);
	DDX_Control(pDX, IDC_FT_TAGS, m_tags);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTagDialog, CDialog)
	//{{AFX_MSG_MAP(CFindTagDialog)
	ON_BN_CLICKED(IDC_FT_ABOUT, OnFtAbout)
	ON_BN_CLICKED(IDC_FI_PREFERENCES, OnFiPreferences)
	ON_NOTIFY(NM_DBLCLK, IDC_FT_TAGS, OnDblclkFtTags)
	ON_BN_CLICKED(IDC_FI_REFRESH, OnFiRefresh)
	ON_BN_CLICKED(IDC_FT_FILESCOPE_RADIO, OnFilescopeRadio)
	ON_BN_CLICKED(IDC_FT_PROJECTSCOPE_RADIO, OnProjectscopeRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTagDialog message handlers

BOOL CFindTagDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD time, time2;

	time = GetTickCount();
	if (WorkspaceInfo::Refresh() == false)
	{
	}
//		m_lastFilename.Empty();
	WorkspaceTags::Update();

	time2 = GetTickCount();
//	sprintf(s, "%d", time2 - time);
//	AfxMessageBox(s);

	// Subclass the edit with our own.
	m_edit.m_pParent = this;
	m_edit.SetWindowText(CFindTagDialog::m_lastFunction);
	m_edit.SetSel(0, -1, FALSE);
	
	// Set the column names of the list control.
	CRect rect;
//	m_tags.SubclassDlgItem(IDC_FT_FUNCTIONS, this);
//	m_tags.SetHighlightType(HIGHLIGHT_ROW);
	DWORD dwStyle;
	dwStyle = m_tags.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES ;
	m_tags.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0,dwStyle );
//	m_tags.SetItemCount(WorkspaceTags::m_idents.GetCount());
	m_tags.GetClientRect(&rect);
	m_tags.InsertColumn(0, "Identifier", LVCFMT_LEFT, 350);
	m_tags.InsertColumn(1, "Parent", LVCFMT_LEFT, 250);

	m_created = true;

	m_eTagScope = eFileScope;
	::SendMessage(::GetDlgItem(GetSafeHwnd(), IDC_FT_FILESCOPE_RADIO), BM_SETCHECK, TRUE, 0L);

	time = GetTickCount();
	RefreshList(m_lastFunction);
	time2 = GetTickCount();
//	sprintf(s, "%d", time2 - time);
//	AfxMessageBox(s);

	if (m_tags.GetItemCount() > 0  &&  m_lastPosition != -1)
	{
		m_tags.SetItemState(m_lastPosition, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_tags.EnsureVisible(m_lastPosition, FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindTagDialog::OnChangeFuncname()
{
	// Get the filename.
	CString filename;
	m_edit.GetWindowText(filename);
	RefreshList(filename);
}

void CFindTagDialog::RefreshList(LPCTSTR name) 
{
	if (m_created == false)
		return;
	
	CString currentFile;
		
	// Turn off any redraw while we fill the list control.
	m_tags.SetRedraw(FALSE);
	
	// Remove all items from the list.
	m_tags.DeleteAllItems();

	// Perform the matching.
	WorkspaceTags::MatchIdentifier(name);
	if (WorkspaceTags::GetMatchCount() == 0)
		return;

	// Set the number of items to the maximum.
	m_tags.SetItemCount(WorkspaceTags::GetMatchCount());

	// Start looking for it.
	int curPos = 0;
	const WorkspaceTags::Tag* tag = WorkspaceTags::GetMatchHead();
	WorkspaceInfo::GetCurrentFilename(currentFile);
	while (tag != NULL)
	{
		if (m_eTagScope == eFileScope && currentFile.CompareNoCase(tag->GetFilename()))
		{
			tag = tag->GetMatchNext();
			continue;
		}

		// Insert the class name.
		LV_ITEM item;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.iItem = curPos;
		item.iSubItem = 0;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.lParam = (LPARAM)tag;
		m_tags.InsertItem(&item);

		// Insert the path.
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.iItem = curPos;
		item.iSubItem = 1;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.lParam = (LPARAM)tag;
		m_tags.SetItem(&item);

		curPos++;
		if (curPos > 1000)
			break;

		tag = tag->GetMatchNext();
	}

	if ((m_tags.GetItemCount() > 0  &&  m_lastPosition == -1)  ||  m_oldFunction != name)
		m_tags.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_tags.SetRedraw(TRUE);

	m_oldFunction = name;
}



void CFindTagDialog::OnOK() 
{
	if (m_tags.GetItemCount() != 0)
	{
		CString filename;
		if (m_tags.GetSelectedCount() == 0)
			m_tags.SetItemState(0, 0, LVIS_SELECTED | LVIS_FOCUSED);
		int nCount = m_tags.GetSelectedCount();

		int curSel = m_tags.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		while (curSel != -1)
		{
			LV_ITEM item;
			item.mask = LVIF_PARAM;
			item.iItem = curSel;
			item.iSubItem = 0;
			m_tags.GetItem(&item);
			
			WorkspaceTags::Tag* tag = (WorkspaceTags::Tag*)item.lParam;
			WorkspaceTags::GotoTag(tag);

			curSel = m_tags.GetNextItem(curSel, LVNI_ALL | LVNI_SELECTED);
		}
	}
	
	m_lastPosition = m_tags.GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
	m_edit.GetWindowText(m_lastFunction);
	CDialog::OnOK();
}

void CFindTagDialog::OnCancel() 
{
	m_lastPosition = m_tags.GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
	m_edit.GetWindowText(m_lastFunction);
	
	CDialog::OnCancel();
}

void CFindTagDialog::OnFtAbout() 
{
	CDialog dlg(IDD_ABOUTBOX);
	dlg.DoModal();
	GetDlgItem(IDC_FT_NAME)->SetFocus();
}

void CFindTagDialog::OnDblclkFtTags(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CFindTagDialog::OnFiRefresh() 
{
	WorkspaceTags::RefreshAll();

	// Get the filename.
	CString filename;
	m_edit.GetWindowText(filename);
	RefreshList(filename);
}

void CFindTagDialog::OnFiPreferences() 
{
	CFIPreferences dlg;
	dlg.DoModal();
}




void CFindTagDialog::OnFilescopeRadio() 
{
	m_eTagScope = eFileScope;
	OnChangeFuncname();
}

void CFindTagDialog::OnProjectscopeRadio() 
{
	m_eTagScope = eProjectScope;
	OnChangeFuncname();
}


