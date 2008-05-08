///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefTagsPage.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefTagsPage.cpp $
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
#include "PrefTagsPage.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefTagsPage dialog

IMPLEMENT_DYNCREATE(CPrefTagsPage, CPropertyPage)

CPrefTagsPage::CPrefTagsPage() : CHtmlHelpPropPage(CPrefTagsPage::IDD)
{
	//{{AFX_DATA_INIT(CPrefTagsPage)
	m_autoPackChangesEdit = 0;
	m_useTagCursorDialog = FALSE;
	m_autoRefreshTags = FALSE;
	m_autoSaveAll = FALSE;
	m_scanFilesTaskSwitch = FALSE;
	m_autoSaveChanges = 0;
	//}}AFX_DATA_INIT
}


CPrefTagsPage::~CPrefTagsPage()
{
}


void CPrefTagsPage::DoDataExchange(CDataExchange* pDX)
{
	CHtmlHelpPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefTagsPage)
	DDX_Control(pDX, IDC_PC_TOKENEDIT, m_tokenEdit);
	DDX_Control(pDX, IDC_PC_TOKENLIST, m_tokenList);
	DDX_Control(pDX, IDC_PC_EXTLIST, m_extList);
	DDX_Control(pDX, IDC_PC_EXTEDIT, m_extEdit);
	DDX_Text(pDX, IDC_PC_AUTOPACKCHANGES, m_autoPackChangesEdit);
	DDV_MinMaxUInt(pDX, m_autoPackChangesEdit, 0, 100000);
	DDX_Check(pDX, IDC_PC_USETAGCURSORDIALOG, m_useTagCursorDialog);
	DDX_Check(pDX, IDC_PC_AUTOREFRESHTAGS, m_autoRefreshTags);
	DDX_Check(pDX, IDC_PC_AUTOSAVEALL, m_autoSaveAll);
	DDX_Check(pDX, IDC_PC_SCANFILESTASKSWITCH, m_scanFilesTaskSwitch);
	DDX_Text(pDX, IDC_PC_AUTOSAVECHANGES, m_autoSaveChanges);
	DDV_MinMaxUInt(pDX, m_autoSaveChanges, 0, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefTagsPage, CHtmlHelpPropPage)
	//{{AFX_MSG_MAP(CPrefTagsPage)
	ON_BN_CLICKED(IDC_PC_ADD, OnPcAdd)
	ON_BN_CLICKED(IDC_PC_REMOVE, OnPcRemove)
	ON_BN_CLICKED(IDC_PC_RESET, OnPcReset)
	ON_EN_CHANGE(IDC_PC_AUTOPACKCHANGES, OnChangePcAutopackchanges)
	ON_BN_CLICKED(IDC_PC_AUTOREFRESHTAGS, OnPcAutorefreshtags)
	ON_BN_CLICKED(IDC_PC_USETAGCURSORDIALOG, OnPcUsetagcursordialog)
	ON_BN_CLICKED(IDC_PC_TOKENADD, OnPcTokenadd)
	ON_BN_CLICKED(IDC_PC_TOKENREMOVE, OnPcTokenremove)
	ON_BN_CLICKED(IDC_PC_TOKENRESET, OnPcTokenreset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefTagsPage message handlers

BOOL CPrefTagsPage::OnInitDialog() 
{
	CHtmlHelpPropPage::OnInitDialog();
	
	CRect clientRect;
	m_tokenList.GetClientRect(clientRect);
	m_tokenList.InsertColumn(0, "Token", LVCFMT_LEFT, clientRect.Width());
	FillInControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefTagsPage::FillInControls()
{
	WWhizConfig& config = g_wwhizInterface->GetConfig();

	m_extList.ResetContent();

	int numExts = config.TagsExtGetCount();
	int i;
	for (i = 0; i < numExts; i++)
	{
		m_extList.AddString(config.TagsExtGet(i));
	}

	// Fill in the ignore tokens list.
	m_tokenList.DeleteAllItems();
	int numTokens = config.IgnoreTokensGetCount();
	for (i = 0; i < numTokens; i++)
	{
		m_tokenList.InsertItem(i, config.IgnoreTokensGet(i));
	}

	m_autoPackChangesEdit = config.GetTagAutoPackAmount();
	m_useTagCursorDialog = g_config.m_useFindTagAtCursorDialog;
	m_autoRefreshTags = config.GetAutoRefreshTags();
	m_scanFilesTaskSwitch = config.GetScanFilesTaskSwitch();
	m_autoSaveAll = g_config.m_autoSaveAllPerTagCommand;
	m_autoSaveChanges = config.GetTagAutoSaveAmount();
	UpdateData(FALSE);
}


BOOL CPrefTagsPage::OnApply() 
{
	// Remove previous extensions.
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	config.TagsExtRemoveAll();

	// Add them to the config.
	int count = m_extList.GetCount();
	int i;
	for (i = 0; i < count; i++)
	{
		CString item;
		m_extList.GetText(i, item);

		config.TagsExtAdd(item);
	}

	// Add them to the config.
	config.IgnoreTokensRemoveAll();
	count = m_tokenList.GetItemCount();
	for (i = 0; i < count; i++)
	{
		CString item = m_tokenList.GetItemText(i, 0);

		config.IgnoreTokensAdd(item);
	}

	config.SetTagAutoPackAmount(m_autoPackChangesEdit);
	g_config.m_useFindTagAtCursorDialog = (m_useTagCursorDialog != FALSE);
	config.SetAutoRefreshTags((m_autoRefreshTags != FALSE));
	config.SetScanFilesTaskSwitch(m_scanFilesTaskSwitch != FALSE);
	g_config.m_autoSaveAllPerTagCommand = m_autoSaveAll != FALSE;
	config.SetTagAutoSaveAmount(m_autoSaveChanges);
	UpdateData(TRUE);

	g_config.SaveRegistry();
	
	return CHtmlHelpPropPage::OnApply();
}


void CPrefTagsPage::OnPcAdd() 
{
	CString str;
	m_extEdit.GetWindowText(str);

	if (str.IsEmpty())
		return;

	// Search for a duplicate.
	int count = m_extList.GetCount();
	for (int i = 0; i < count; i++)
	{
		CString item;
		m_extList.GetText(i, item);

		if (item == str)
			return;
	}

	// Okay, add it.
	m_extList.AddString(str);

	SetModified(TRUE);
}

void CPrefTagsPage::OnPcRemove() 
{
	int sel = m_extList.GetCurSel();
	if (sel == LB_ERR)
		return;

	m_extList.DeleteString(sel);

	SetModified(TRUE);
}

void CPrefTagsPage::OnPcReset() 
{
	g_wwhizInterface->GetConfig().TagsExtReset();

	FillInControls();

	SetModified(TRUE);
}


void CPrefTagsPage::OnChangePcAutopackchanges() 
{
	SetModified(TRUE);
}

void CPrefTagsPage::OnPcAutorefreshtags() 
{
	SetModified(TRUE);
}

void CPrefTagsPage::OnPcUsetagcursordialog() 
{
	SetModified(TRUE);
}

void CPrefTagsPage::OnPcTokenadd() 
{
	CString str;
	m_tokenEdit.GetWindowText(str);

	if (str.IsEmpty())
		return;

	// Search for a duplicate.
	int count = m_tokenList.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		CString item = m_tokenList.GetItemText(i, 0);

		if (item == str)
			return;
	}

	// Okay, add it.
	m_tokenList.InsertItem(count, str);

	SetModified(TRUE);
}

void CPrefTagsPage::OnPcTokenremove() 
{
	int sel = m_tokenList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (sel == -1)
		return;

	m_tokenList.DeleteItem(sel);

	SetModified(TRUE);
}

void CPrefTagsPage::OnPcTokenreset() 
{
	g_wwhizInterface->GetConfig().IgnoreTokensReset();

	FillInControls();

	SetModified(TRUE);
}
