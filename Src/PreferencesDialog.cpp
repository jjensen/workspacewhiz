// PreferencesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "PreferencesDialog.h"
#include "WorkspaceInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog dialog


CPreferencesDialog::CPreferencesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesDialog)
	//}}AFX_DATA_INIT
}


void CPreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDialog)
	DDX_Control(pDX, IDC_PR_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesDialog, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog message handlers

BOOL CPreferencesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set the proper check marks.
	const WorkspaceInfo::ProjectList& projectList = WorkspaceInfo::GetProjectList();
	POSITION pos = projectList.GetHeadPosition();
	while (pos != NULL)
	{
		WorkspaceInfo::Project& prj = projectList.GetNext(pos);
		int which = m_list.AddString(prj.GetName());
		m_list.SetCheck(which, prj.IsActive());
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesDialog::OnOK() 
{
	// Find an unchecked project.
	bool found = false;
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		if (m_list.GetCheck(i) == 0)
		{
			found = true;
			break;
		}
	}

	const CString& infoFile = WorkspaceInfo::GetExcludeFilename();
	if (found)
	{
		// Write the exclude list.
		CStdioFile file;
		if (file.Open(infoFile, CFile::modeCreate | CFile::modeWrite) == FALSE)
		{
			CDialog::OnOK();
			return;
		}

		const WorkspaceInfo::ProjectList& projectList = WorkspaceInfo::GetProjectList();
		POSITION pos = projectList.GetHeadPosition();
		for (int i = 0; i < m_list.GetCount(); i++)
		{
			WorkspaceInfo::Project& prj = projectList.GetNext(pos);
			prj.SetActive(m_list.GetCheck(i) == 1);
			if (m_list.GetCheck(i) == 0)
			{
				CString str;
				m_list.GetText(i, str);
				file.WriteString(str);
				file.WriteString("\n");
			}
		}

		file.Close();
	}
	else //found
	{
		_unlink(infoFile);
	}
	
	CDialog::OnOK();
}

