///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateFileListDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TemplateFileListDialog.cpp $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "TemplateFileListDialog.h"
#include "History.h"
#include "AboutDialog.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateFileListDialog dialog

bool CTemplateFileListDialog::s_autoCloseParent;		// A hack to close the parent.

CTemplateFileListDialog::CTemplateFileListDialog(CWnd* pParent /*=NULL*/)
	: TEMPLATE_FILE_LIST_DIALOG(CTemplateFileListDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateFileListDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	s_autoCloseParent = false;
}


void CTemplateFileListDialog::DoDataExchange(CDataExchange* pDX)
{
	TEMPLATE_FILE_LIST_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateFileListDialog)
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_TFL_REMOVE, m_butRemove);
	DDX_Control(pDX, IDC_TFL_EDIT, m_butEdit);
	DDX_Control(pDX, IDC_TFL_ADD, m_butAdd);
	DDX_Control(pDX, IDC_TFL_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateFileListDialog, TEMPLATE_FILE_LIST_DIALOG)
	//{{AFX_MSG_MAP(CTemplateFileListDialog)
	ON_BN_CLICKED(IDC_TFL_ADD, OnTflAdd)
	ON_BN_CLICKED(IDC_TFL_REMOVE, OnTflRemove)
	ON_BN_CLICKED(IDC_TFL_EDIT, OnTflEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_TFL_LIST, OnDblclkTflList)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_COM_ABOUT, OnComAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CTemplateFileListDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDOK,					mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDHELP,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_ABOUT,		mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_TFL_ADD,			mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_TFL_REMOVE,		mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_TFL_EDIT,			mdRepos,	mdNone)
	
	DYNAMIC_MAP_ENTRY(IDC_TFL_LIST,			mdResize,	mdResize)
END_DYNAMIC_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateFileListDialog message handlers

void CTemplateFileListDialog::OnTflAdd() 
{
	CFileDialog fdlg(TRUE, "WWTpl", "*.WWTpl", OFN_HIDEREADONLY,
		"Code Template Files (*.WWTpl)|*.WWTpl||", NULL);
	fdlg.m_ofn.lpstrTitle = "Find Template Files";
	if(fdlg.DoModal() == IDCANCEL)  
		return;
	if (g_wwhizTemplateManager->Add(fdlg.GetPathName()) != -1)
	{
		Refresh();
	}
}

void CTemplateFileListDialog::OnTflRemove() 
{
	// Remove all selected items.
    int i = m_list.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
    while( i != -1 )
    {
		g_wwhizTemplateManager->Remove(m_list.GetItemData(i));
        i = m_list.GetNextItem( i, LVNI_ALL | LVNI_SELECTED);
    }

	Refresh();
}

void CTemplateFileListDialog::OnTflEdit() 
{
	if (m_list.GetItemCount() == 0  ||  m_list.GetSelectedCount() == 0)
	{
		AfxMessageBox("Please select a template file selected to edit first.\n");
	}

	CString filename;
	int curSel = m_list.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (curSel == -1)
		return;  //??

	int whichItem = m_list.GetItemData(curSel);
	WWhizTemplateGroup* file = g_wwhizTemplateManager->GetTemplateGroup(whichItem);

	History::PreAdd();
	
	// Open the document.
	ObjModelHelper objModel;
	if (objModel.OpenDocument(file->GetFilename(), "Auto"))
	{
		History::Add();

		objModel.PutLanguage("cpp");
	}

	s_autoCloseParent = true;

	OnOK();
}

BOOL CTemplateFileListDialog::OnInitDialog() 
{
	TEMPLATE_FILE_LIST_DIALOG::OnInitDialog();
	
	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	// Reload stuff from registry.
	g_wwhizTemplateManager->LoadRegistry();
	
	// Set the column names of the list control.
	CRect rect;
	DWORD dwStyle;
	dwStyle = m_list.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES;
	m_list.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);
	m_list.GetClientRect(&rect);
	m_list.InsertColumn(0, "Category", LVCFMT_LEFT, 125, 0);
	m_list.InsertColumn(1, "Path", LVCFMT_LEFT, rect.Width() - 125, 1);
	
	g_wwhizTemplateManager->Refresh();
	Refresh();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTemplateFileListDialog::Refresh() 
{
	// Turn off any redraw while we fill the list control.
	m_list.SetRedraw(FALSE);
	
	// Remove all items from the list.
	m_list.DeleteAllItems();

	for (int i = 0; i < g_wwhizTemplateManager->GetCount(); i++)
	{
		WWhizTemplateGroup* file = g_wwhizTemplateManager->GetTemplateGroup(i);
		int index = m_list.InsertItem(LVIF_TEXT | LVIF_PARAM, i, file->GetName(), 0, 0, 0, i);
		m_list.SetItem(index, 1, LVIF_TEXT, file->GetFilename(), 0, 0, 0, 0);
		m_list.SetCheck(index, file->IsActive());
	}

	m_list.SetRedraw(TRUE);
}



void CTemplateFileListDialog::OnOK() 
{
	// Set all active states.
	for (int i = 0; i < g_wwhizTemplateManager->GetCount(); i++)
	{
		UINT whichItem = m_list.GetItemData(i);
		WWhizTemplateGroup* file = g_wwhizTemplateManager->GetTemplateGroup(whichItem);
		file->SetActive(m_list.GetCheck(i) != FALSE);
	}

	// Save changes to registry.
	g_wwhizTemplateManager->SaveRegistry();

	TEMPLATE_FILE_LIST_DIALOG::OnOK();
}

void CTemplateFileListDialog::OnDblclkTflList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTflEdit();
	
	*pResult = 0;
}

void CTemplateFileListDialog::OnSize(UINT nType, int cx, int cy) 
{
	TEMPLATE_FILE_LIST_DIALOG::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_list.m_hWnd))
	{
		CRect rect;
		m_list.GetClientRect(rect);
		m_list.SetColumnWidth(0, (int)(rect.Width() * 0.33));
		m_list.SetColumnWidth(1, (int)(rect.Width() * 0.67));
	}
}

void CTemplateFileListDialog::OnComAbout() 
{
	CAboutDialog dlg;
	dlg.DoModal();
}

