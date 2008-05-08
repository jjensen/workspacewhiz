///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateSelectDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TemplateSelectDialog.cpp $
// $Date: 2003/01/16 $ $Revision: #7 $ $Author: Joshua $
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
#include "TemplateSelectDialog.h"
#include "TemplateFileListDialog.h"
#include "TemplateWizardDialog.h"
#include "AboutDialog.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateSelectDialog dialog

/**
	Static function to create a template select dialog.
**/
void CTemplateSelectDialog::DoDialog()
{
#ifdef WWHIZ_VSNET
	CComPtr<EnvDTE::Document> document;
	g_pDTE->get_ActiveDocument(&document);
#endif WWHIZ_VSNET

	while (g_wwhizTemplateManager->GetCount() == 0)
	{
		CTemplateFileListDialog dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;
	}

	CTemplateSelectDialog dlg;
	if (dlg.DoModal() == IDCANCEL)
		return;

	if (dlg.m_code)
	{
#ifdef WWHIZ_VSNET
		if (document)
			document->Activate();
#endif WWHIZ_VSNET

		WWhizCommands::PutTemplate(*dlg.m_code);
	}
}


CTemplateSelectDialog::CTemplateSelectDialog(CWnd* pParent /*=NULL*/)
	: TEMPLATE_SELECT_DIALOG(CTemplateSelectDialog::IDD, pParent),
	m_code(NULL)
{
	//{{AFX_DATA_INIT(CTemplateSelectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTemplateSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	TEMPLATE_SELECT_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateSelectDialog)
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_TS_FILELIST, m_butFileList);
	DDX_Control(pDX, IDC_COM_ABOUT, m_butAbout);
	DDX_Control(pDX, IDC_TS_MEMO, m_memo);
	DDX_Control(pDX, IDC_TS_TREE, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateSelectDialog, TEMPLATE_SELECT_DIALOG)
	//{{AFX_MSG_MAP(CTemplateSelectDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TS_TREE, OnSelchangedCtTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TS_TREE, OnDblclkCtTree)
	ON_BN_CLICKED(IDC_TS_FILELIST, OnTfFilelist)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_COM_ABOUT, OnTsAbout)
	ON_NOTIFY(NM_RCLICK, IDC_TS_TREE, OnRclickTsTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CTemplateSelectDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDOK,					mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDHELP,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_ABOUT,		mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_TS_FILELIST,		mdRepos,	mdNone)
	
	DYNAMIC_MAP_ENTRY(IDC_TS_TREE,			mdResize,	mdResize)
	DYNAMIC_MAP_ENTRY(IDC_TS_MEMO,			mdResize,	mdRepos)
END_DYNAMIC_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTemplateSelectDialog message handlers

static CTreeCursor FindTreeBranch(CTreeCursor cursor, LPCTSTR branchName)
{
	cursor = cursor.GetChild();
	while (cursor != NULL)
	{
		if (cursor.GetText() == branchName)
		{
			return cursor;
		}

		cursor = cursor.GetNextSibling();
	}

	return cursor;
}


static CTreeCursor AddTreeBranch(CTreeCursor cursor, const CString& branchName)
{
	CTreeCursor foundCursor = FindTreeBranch(cursor, branchName);
	if (foundCursor)
		return foundCursor;

	foundCursor = cursor.AddTail(branchName);
	foundCursor.SetData(NULL);

	return foundCursor;
}


static void AddTreeItem(CTreeCursor cursor, UINT id, CString name)
{
	int curPos = 0;
	int nextPos;

	// Autoexpand.
	bool autoExpand = false;
	if (name[0] == '+')
	{
		curPos++;
		autoExpand = true;
	}

	while (true)
	{
		// See if there is a sub-tree.
		nextPos = name.Find('|', curPos);
		if (nextPos == -1)
			break;

		CString branchName = name.Mid(curPos, nextPos - curPos);
		curPos = nextPos + 1;
		cursor = AddTreeBranch(cursor, branchName);
		if (autoExpand)
			cursor.GetParent().Expand();

		cursor.SortChildren();
	}

	CTreeCursor lastBranchCursor;
	lastBranchCursor = cursor.AddTail(name.Mid(curPos));
	lastBranchCursor.SetData(id);

	if (autoExpand)
	{
		lastBranchCursor.Expand();
		lastBranchCursor.GetParent().Expand();
	}

	cursor.SortChildren();
}


void CTemplateSelectDialog::Refresh()
{
	// Try refreshing the template files.
	bool changed = g_wwhizTemplateManager->Refresh();

	// Delete all the items in the tree.
	m_tree.DeleteAllItems();

	// Get the root tree item.
	CTreeCursor rootCursor = m_tree.GetRootItem();

	// Walk each template file.
	int count = g_wwhizTemplateManager->GetCount();
	for (int i = 0; i < count; i++)
	{
		// Add each template file to the root of the tree.
		WWhizTemplateGroup* file = g_wwhizTemplateManager->GetTemplateGroup(i);
		if (!file->IsActive())
			continue;
		CTreeCursor topCursor = rootCursor.AddTail(file->GetName());
		topCursor.SetData(NULL);

		// Add each template stored in the template file as children.
		for (int index = 0; index < file->GetCount(); index++)
		{
			WWhizTemplate* code = file->Get(index);
			if (code->GetName().GetAt(0) != '-')		// - is private.
				AddTreeItem(topCursor, (UINT)code, code->GetName());
			topCursor.SortChildren();
		}
	}

	// Sort the root.
	rootCursor.SortChildren();

	// Set the input focus to the tree.
	m_tree.SetFocus();
	m_tree.GetRootItem().Select();

	// If there wasn't a refresh, then check the tree state.
	if (!changed)
		changed |= g_wwhizTemplateManager->RestoreTreeState(m_tree);
}


BOOL CTemplateSelectDialog::OnInitDialog() 
{
	TEMPLATE_SELECT_DIALOG::OnInitDialog();
	
	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	// Refresh the tree.
	Refresh();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplateSelectDialog::OnSelchangedCtTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
//	int treeIndex = pNMTreeView->itemNew.lParam;						// Get new index
	CTreeCursor cursor;
	cursor = m_tree.GetSelectedItem();
	WWhizTemplate* code = (WWhizTemplate*)cursor.GetData();
	if (code)
	{
		m_memo.SetWindowText(code->GetMemo());
	}
	else
	{
		m_memo.SetWindowText("");
	}

	*pResult = 0;
}

void CTemplateSelectDialog::OnDblclkCtTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTreeCursor cursor = m_tree.GetSelectedItem();
	WWhizTemplate* code = (WWhizTemplate*)cursor.GetData();
	if (code)
		OnOK();

	*pResult = 0;
}


void CTemplateSelectDialog::OnOK() 
{
	// Get the template, if possible.
	CTreeCursor cursor = m_tree.GetSelectedItem();
	if (cursor != NULL)
	{
		m_code = (WWhizTemplate*)cursor.GetData();
	}
	
	// Save the tree state.
	g_wwhizTemplateManager->SaveTreeState(m_tree);
	
	// Close the dialog.
	TEMPLATE_SELECT_DIALOG::OnOK();

	// If there wasn't a template, exit.
	if (!m_code)
		return;

	m_code->ResetDictionaryDefaults();

	// If there are dialog wizard pages, then pop up the wizard dialog.
	if (m_code->GetPageCount() > 0)
	{
		CTemplateWizardDialog dlg(*m_code);
		if (dlg.DoModal() == IDCANCEL)
			return;
	}
}


void CTemplateSelectDialog::OnCancel() 
{
	// Save the tree state.
	g_wwhizTemplateManager->SaveTreeState(m_tree);
	
	TEMPLATE_SELECT_DIALOG::OnCancel();
}

void CTemplateSelectDialog::OnTfFilelist() 
{
	CTemplateFileListDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (CTemplateFileListDialog::s_autoCloseParent)
		{
			OnCancel();
			return;
		}

		// Refresh, if necessary.
		Refresh();
	}
}

void CTemplateSelectDialog::OnDestroy() 
{
	TEMPLATE_SELECT_DIALOG::OnDestroy();
	
//	cdxCDynamicControlsManager::StoreWindowPosition(*this, _T("Dialogs\\TemplateSelectDialog"));
}

void CTemplateSelectDialog::OnTsAbout() 
{
	CAboutDialog dlg;
	dlg.DoModal();
	GetDlgItem(IDC_TS_TREE)->SetFocus();
}

void CTemplateSelectDialog::OnRclickTsTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTreeCursor cursor = m_tree.GetDropHilightItem();
	if (!cursor)
		cursor = m_tree.GetSelectedItem();
	if (!cursor)
		return;
	WWhizTemplate* code = (WWhizTemplate*)cursor.GetData();

	if (code)
	{
		// Open the document.
		ObjModelHelper objModel;
		if (objModel.OpenDocument(code->GetParent().GetFilename(), "Auto"))
		{
			objModel.PutLanguage("cpp");

			objModel.MoveTo(code->GetLineNumber(), 1, false);
			objModel.ScrollToCenter();

			OnCancel();
		}
	}

	*pResult = 0;
}
