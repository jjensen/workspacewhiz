///////////////////////////////////////////////////////////////////////////////
// $Workfile: ExtraFilesDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/ExtraFilesDialog.cpp $
// $Date: 2003/01/05 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "afxdlgs.h"
#include "resource.h"
#include "ExtraFilesDialog.h"
#include <io.h>
#include <sys/stat.h>
#include "AboutDialog.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtraFilesDialog dialog


CExtraFilesDialog::CExtraFilesDialog(CWnd* pParent /*=NULL*/)
	: CHtmlHelpDialog(CExtraFilesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtraFilesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CExtraFilesDialog::DoDataExchange(CDataExchange* pDX)
{
	CHtmlHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtraFilesDialog)
	DDX_Control(pDX, IDC_EP_NEVERREFRESH, m_neverRefreshCheckbox);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_EP_REMOVE, m_butRemove);
	DDX_Control(pDX, IDC_EP_BROWSE, m_butBrowse);
	DDX_Control(pDX, IDC_EP_ADD, m_butAdd);
	DDX_Control(pDX, IDC_EP_FILENAME, m_filename);
	DDX_Control(pDX, IDC_EP_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtraFilesDialog, CHtmlHelpDialog)
	//{{AFX_MSG_MAP(CExtraFilesDialog)
	ON_BN_CLICKED(IDC_EP_ADD, OnEpAdd)
	ON_BN_CLICKED(IDC_EP_REMOVE, OnEpRemove)
	ON_BN_CLICKED(IDC_EP_BROWSE, OnEpBrowse)
	ON_LBN_DBLCLK(IDC_EP_LIST, OnDblclkEpList)
	ON_BN_CLICKED(IDC_COM_ABOUT, OnComAbout)
	ON_LBN_SELCHANGE(IDC_EP_LIST, OnSelchangeEpList)
	ON_BN_CLICKED(IDC_EP_NEVERREFRESH, OnEpNeverrefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CExtraFilesDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDC_EP_FILENAME,		mdResize,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_EP_LIST,			mdResize,	mdResize)
	DYNAMIC_MAP_ENTRY(IDC_EP_BROWSE,		mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_EP_ADD,			mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_EP_REMOVE,		mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDOK,					mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDHELP,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_ABOUT,		mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_EP_NEVERREFRESH,	mdRepos,	mdRepos)
END_DYNAMIC_MAP()
	
/////////////////////////////////////////////////////////////////////////////
// CExtraFilesDialog message handlers

BOOL CExtraFilesDialog::OnInitDialog() 
{
	CHtmlHelpDialog::OnInitDialog();
	
	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	// Check for the file being read-only.
	if (_access(g_wwhizInterface->GetExtraFilename(), 00) == 0  &&
		_access(g_wwhizInterface->GetExtraFilename(), 02) == -1)
	{
		int ret =
			AfxMessageBox("The ExtraFiles.WW file for storing extra projects is read-only.  In order to make"
				" changes, this file must be writable.  Do you want to make it writable?",
				MB_YESNO);
		if (ret == IDNO)
		{
			OnCancel();
			return TRUE;
		}
		else
		{
			_chmod(g_wwhizInterface->GetExtraFilename(), _S_IREAD | _S_IWRITE);
		}
	}

	// Is there an add-on file?
	g_wwhizReg->ReadExtraFiles(false);

	// Add the items from the extra files list to the list box.
	WWhizReg::ExtraFilesList extraFilesList;
	g_wwhizReg->GetExtraFiles(extraFilesList);
	POSITION pos = extraFilesList.GetHeadPosition();
	while (pos)
	{
		const WWhizReg::ExtraFilesInfo& info = extraFilesList.GetNext(pos);
		int index = m_list.AddString(info.m_name);
		m_list.SetCheck(index, info.m_active ? 1 : 0);
		m_list.SetItemData(index, info.m_noRefresh);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CExtraFilesDialog::ResolveFilename(CString& filename)
{
	char* origEnvList = (char*)::GetEnvironmentStrings();
	char* envList = origEnvList;

	// Loop through all of them.
	while (*envList)
	{
		CString env = envList;
		CString origEnv = env;
		int equalPos = env.Find('=');
		CString envName = env.Left(equalPos);
		env = env.Mid(equalPos + 1);
		if (!env.IsEmpty())
		{
			if (env[env.GetLength() - 1] == '\\')
				env = env.Left(env.GetLength() - 1);

			// Only try a match if the environment variable is less than the
			// filename's length.
			if (env.GetLength() < filename.GetLength()  &&
				filename[env.GetLength()] == '\\')
			{
				if (_tcsnicmp((LPCTSTR)env, (LPCTSTR)filename, env.GetLength()) == 0)
				{
					CString newFilename = "$(" + envName + ")" + filename.Mid(env.GetLength());

					// Matched.
					CString msg;
					msg.Format("The environment variable [%s] matches the [%s] portion "
						"of the path in [%s].  Do you want to substitute the current "
						"filename with [%s]?", envName, env, filename, newFilename);
					int ret = AfxMessageBox(msg, MB_YESNOCANCEL);
					if (ret == IDYES)
					{
						filename = newFilename;
						break;
					}
					else if (ret == IDCANCEL)
					{
						break;
					}
				}
			}
		}

		envList += origEnv.GetLength() + 1;
	}

	::FreeEnvironmentStrings(origEnvList);
}


void CExtraFilesDialog::AddFileToList(CString& filename)
{
//	ResolveFilename(filename);
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		CString name;
		m_list.GetText(i, name);
		if (name.CompareNoCase(filename) == 0)
			return;
	}
	int index = m_list.AddString(filename);
	m_list.SetCheck(index, 1);
}


void CExtraFilesDialog::OnEpAdd() 
{
	CString filename;
	m_filename.GetWindowText(filename);

	if (filename.GetLength() > 0)
	{
		AddFileToList(filename);
		m_filename.SetWindowText("");
	}
}

void CExtraFilesDialog::OnEpRemove() 
{
	if (m_list.GetCurSel() == LB_ERR)
		return;
	int sel = m_list.GetCurSel();
	m_list.DeleteString(sel);
	if (m_list.SetCurSel(sel) == LB_ERR)
		m_list.SetCurSel(sel - 1);
}


void CExtraFilesDialog::OnEpBrowse() 
{
	CFileDialog fdlg(TRUE, NULL, "*.*", OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
		"Workspaces and Project Files (.dsw,.sln,.dsp,.vcproj)|*.dsw;*.sln;*.dsp;*.vcproj|All Files (*.*)|*.*||", NULL);
	fdlg.m_ofn.lpstrTitle = "Select Extra Files";
	fdlg.m_ofn.nMaxFile = 10 * 1024;
	AutoBasic<char> fileBuffer(WNEW char[fdlg.m_ofn.nMaxFile]);
	fdlg.m_ofn.lpstrFile = fileBuffer;
	fdlg.m_ofn.lpstrFile[0] = 0;
	if(fdlg.DoModal() == IDCANCEL)  
		return;

	// Parse the current directory.
	char* ptr = fdlg.m_ofn.lpstrFile;
	CString curDir = ptr;
	ptr += curDir.GetLength() + 1;

	if (*ptr == 0)
	{
		AddFileToList(curDir);
	}
	else
	{
		if (curDir[curDir.GetLength() - 1] != '\\')
			curDir += "\\";

		while (*ptr)
		{
			CString filename = ptr;
			ptr += filename.GetLength() + 1;
			AddFileToList(curDir + filename);
		}
	}
	
	m_filename.SetWindowText("");
}

void CExtraFilesDialog::OnOK() 
{
	OnEpAdd();

	// Is there an add-on file?
	WWhizReg::ExtraFilesList extraFilesList;
	extraFilesList.RemoveAll();
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		WWhizReg::ExtraFilesInfo info;
		info.m_active = m_list.GetCheck(i) == 1;
		m_list.GetText(i, info.m_name);
		info.m_noRefresh = m_list.GetItemData(i) != 0;
		extraFilesList.AddTail(info);
	}
	g_wwhizReg->SetExtraFiles(extraFilesList);

	// Save the ExtraFiles.WW file.
	g_wwhizReg->WriteExtraFiles();
	
	CHtmlHelpDialog::OnOK();
}

void CExtraFilesDialog::OnDblclkEpList() 
{
	if (m_list.GetCurSel() == LB_ERR)
		return;
	CString str;
	m_list.GetText(m_list.GetCurSel(), str);
	m_filename.SetWindowText(str);

	OnEpRemove();
}

void CExtraFilesDialog::OnComAbout() 
{
	CAboutDialog dlg;
	dlg.DoModal();
}

void CExtraFilesDialog::OnSelchangeEpList() 
{
	int curSel = m_list.GetCurSel();
	if (curSel == LB_ERR)
		return;

	m_neverRefreshCheckbox.SetCheck(m_list.GetItemData(curSel));
}

void CExtraFilesDialog::OnEpNeverrefresh() 
{
	int curSel = m_list.GetCurSel();
	if (curSel == LB_ERR)
		return;

	m_list.SetItemData(curSel, m_neverRefreshCheckbox.GetCheck());
}
