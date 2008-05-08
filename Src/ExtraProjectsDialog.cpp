// ExtraProjectsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "afxdlgs.h"
#include "WorkspaceUtils.h"
#include "ExtraProjectsDialog.h"
#include "WorkspaceInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtraProjectsDialog dialog


CExtraProjectsDialog::CExtraProjectsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CExtraProjectsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtraProjectsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CExtraProjectsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtraProjectsDialog)
	DDX_Control(pDX, IDC_EP_FILENAME, m_filename);
	DDX_Control(pDX, IDC_EP_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtraProjectsDialog, CDialog)
	//{{AFX_MSG_MAP(CExtraProjectsDialog)
	ON_BN_CLICKED(IDC_EP_ADD, OnEpAdd)
	ON_BN_CLICKED(IDC_EP_REMOVE, OnEpRemove)
	ON_BN_CLICKED(IDC_EP_BROWSE, OnEpBrowse)
	ON_LBN_DBLCLK(IDC_EP_LIST, OnDblclkEpList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtraProjectsDialog message handlers

BOOL CExtraProjectsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Is there an add-on file?
	CStdioFile file;
	if (file.Open(WorkspaceInfo::GetExtraFilename(), CFile::modeRead) == TRUE)
	{
		CString line;
		
		// Count the number of extra projects.
		while (1)
		{
			// Read in a project name.
			if (!file.ReadString(line))
				break;

			// Add to the list box.
			m_list.AddString(line);
		} 

		// Close the file.
		file.Close();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExtraProjectsDialog::OnEpAdd() 
{
	CString filename;
	m_filename.GetWindowText(filename);

	if (filename.GetLength() > 0)
	{
		for (int i = 0; i < m_list.GetCount(); i++)
		{
			CString name;
			m_list.GetText(i, name);
			if (name.CompareNoCase(filename) == 0)
				return;
		}
		m_list.AddString(filename);
	}
}

void CExtraProjectsDialog::OnEpRemove() 
{
	if (m_list.GetCurSel() == LB_ERR)
		return;
	int sel = m_list.GetCurSel();
	m_list.DeleteString(sel);
	if (m_list.SetCurSel(sel) == LB_ERR)
		m_list.SetCurSel(sel - 1);
}


void CExtraProjectsDialog::OnEpBrowse() 
{
	CFileDialog fdlg(TRUE, "dsp", "*.dsp;*.dsw", OFN_HIDEREADONLY, "Project Files (*.dsp,*.dsw)", NULL);
	fdlg.m_ofn.lpstrTitle = "Find Project Files";
	if(fdlg.DoModal() == IDCANCEL)  
		return;

	m_filename.SetWindowText(fdlg.GetPathName());
}

void CExtraProjectsDialog::OnOK() 
{
	// Is there an add-on file?
	CStdioFile file;
	if (file.Open(WorkspaceInfo::GetExtraFilename(), 
			CFile::modeCreate | CFile::modeWrite) == TRUE)
	{
		for (int i = 0; i < m_list.GetCount(); i++)
		{
			CString str;
			m_list.GetText(i, str);
			file.WriteString(str);
			file.WriteString("\n");
		}

		file.Close();
	}
	
	CDialog::OnOK();
}

void CExtraProjectsDialog::OnDblclkEpList() 
{
	if (m_list.GetCurSel() == LB_ERR)
		return;
	CString str;
	m_list.GetText(m_list.GetCurSel(), str);
	m_filename.SetWindowText(str);
}
