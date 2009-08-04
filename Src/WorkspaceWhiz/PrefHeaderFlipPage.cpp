///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefHeaderFlipPage.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefHeaderFlipPage.cpp $
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
#include "PrefHeaderFlipPage.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefHeaderFlipPage property page

IMPLEMENT_DYNCREATE(CPrefHeaderFlipPage, CPropertyPage)

CPrefHeaderFlipPage::CPrefHeaderFlipPage() : CHtmlHelpPropPage(CPrefHeaderFlipPage::IDD)
{
	//{{AFX_DATA_INIT(CPrefHeaderFlipPage)
	//}}AFX_DATA_INIT
}

CPrefHeaderFlipPage::~CPrefHeaderFlipPage()
{
}

void CPrefHeaderFlipPage::DoDataExchange(CDataExchange* pDX)
{
	CHtmlHelpPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefHeaderFlipPage)
	DDX_Control(pDX, IDC_PHF_EXTLIST, m_extList);
	DDX_Control(pDX, IDC_PHF_EXTEDIT, m_extEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefHeaderFlipPage, CHtmlHelpPropPage)
	//{{AFX_MSG_MAP(CPrefHeaderFlipPage)
	ON_BN_CLICKED(IDC_PHF_ADD, OnPhfAdd)
	ON_BN_CLICKED(IDC_PHF_REMOVE, OnPhfRemove)
	ON_BN_CLICKED(IDC_PHF_RESET, OnPhfReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefHeaderFlipPage message handlers

BOOL CPrefHeaderFlipPage::OnInitDialog() 
{
	CHtmlHelpPropPage::OnInitDialog();
	
	FillInControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefHeaderFlipPage::FillInControls()
{
	WWhizConfig& config = g_wwhizInterface->GetConfig();

	m_extList.ResetContent();

	int numExts = config.FilesExtGetCount();
	for (int i = 0; i < numExts; i++)
	{
		m_extList.AddString(config.FilesExtGet(i));
	}
}

BOOL CPrefHeaderFlipPage::OnApply() 
{
	// Remove previous extensions.
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	config.FilesExtRemoveAll();

	// Add them to the config.
	int count = m_extList.GetCount();
	for (int i = 0; i < count; i++)
	{
		CString item;
		m_extList.GetText(i, item);

		config.FilesExtAdd(item);
	}

	g_config.SaveRegistry();
	
	return CHtmlHelpPropPage::OnApply();
}

void CPrefHeaderFlipPage::OnPhfAdd() 
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

void CPrefHeaderFlipPage::OnPhfRemove() 
{
	int sel = m_extList.GetCurSel();
	if (sel == LB_ERR)
		return;

	m_extList.DeleteString(sel);

	SetModified(TRUE);
}

void CPrefHeaderFlipPage::OnPhfReset() 
{
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	config.FilesExtReset();

	FillInControls();

	SetModified(TRUE);
}
