// FindFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "FindFileDialog.h"
#include "FindFileEdit.h"
#include "ExtraProjectsDialog.h"
#include "PreferencesDialog.h"
#include "WorkspaceInfo.h"
#include "CompilerFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindFileDialog dialog

IApplication *CFindFileDialog::m_pApplication = NULL;

CFindFileDialog::CFindFileDialog(CWnd* pParent /*=NULL*/)
:	m_created(false),
	m_lastPosition(-1),
	CDialog(CFindFileDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindFileDialog)
	//}}AFX_DATA_INIT
}

CFindFileDialog::~CFindFileDialog()
{
}

void CFindFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindFileDialog)
	DDX_Control(pDX, IDC_FF_FILES, m_files);
	DDX_Control(pDX, IDC_FF_FILENAME, m_edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindFileDialog, CDialog)
	//{{AFX_MSG_MAP(CFindFileDialog)
	ON_BN_CLICKED(IDC_FF_ABOUT, OnPfAbout)
	ON_NOTIFY(NM_DBLCLK, IDC_FF_FILES, OnDblclkFfFiles)
	ON_BN_CLICKED(IDC_FF_EXTRAPROJECTS, OnFfExtraprojects)
	ON_BN_CLICKED(IDC_FF_PREFERENCES, OnFfPreferences)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_FF_REFRESH, OnFfRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFileDialog message handlers

BOOL CFindFileDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Subclass the edit with our own.
	m_edit.m_pParent = this;
    m_edit.SetWindowText(m_lastFilename);
	m_edit.SetSel(0, -1, FALSE);

	// Set the column names of the list control.
	CRect rect;
	DWORD dwStyle;
	m_files.m_parent = this;
	dwStyle = m_files.SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES ;
	m_files.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0,dwStyle );
#if _MSC_VER > 1100
	m_files.SetItemCountEx(m_fileList->GetCount());
#else
	m_files.SetItemCount(m_fileList->GetCount());
#endif
	m_files.GetClientRect(&rect);
	m_files.InsertColumn(0, "Filename", LVCFMT_LEFT, 175, 0);
	m_files.InsertColumn(1, "Path", LVCFMT_LEFT, rect.Width() - 175 - 20, 1);

	// Hide some stuff.
	if (m_workspaceOpen)
	{
		GetDlgItem(IDC_FF_REFRESH)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_FF_EXTRAPROJECTS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FF_PREFERENCES)->ShowWindow(SW_HIDE);
	}
	
	m_created = true;
	
	if (m_lastPosition != -1)
	{
		m_oldFilename = m_lastFilename;
	}
	RefreshList(m_lastFilename);
	if (m_files.GetItemCount() > 0  &&  m_lastPosition != -1)
	{
		m_files.SetItemState(m_lastPosition, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_files.EnsureVisible(m_lastPosition, FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFindFileDialog::RefreshList(LPCTSTR name) 
{
	if (m_created == false)
		return;
	
	// Turn off any redraw while we fill the list control.
	m_files.SetRedraw(FALSE);
	
	// Remove all items from the list.
	m_files.DeleteAllItems();
	m_foundFiles.RemoveAll();
	m_foundFiles.SetSize(m_fileList->GetCount());

	// We're going to modify the text, so copy it.
	char filename[200];
	strcpy(filename, name);

	// Is there an extension in the filename?
	char* ext = strrchr(filename, '.');
	int extLen = 0;
	if (ext != NULL)
	{
		*ext++ = 0;
		strlwr(ext);
		extLen = strlen(ext);
	}
	
	// Now build the shortened name.
	int pLen = 0;
	int len = strlen(filename);
	int splitPos = -1;
	for (int i = 0; i < len; i++)
	{
		if (isalnum(filename[i]))
			filename[pLen++] = tolower(filename[i]);
		else if (filename[i] == '/')
			splitPos = pLen;
	}
	filename[pLen] = 0;

	// Last part.
	char lastPart[200];
	int lastPartLen = 0;
	if (splitPos != -1)
	{
		strcpy(lastPart, (char *)&filename[splitPos]);
		lastPartLen = strlen(lastPart);
		filename[splitPos] = 0;
		pLen = splitPos;
	}

	// Start the search.
	int topOne = -1;
	int curPos = 0;
	const FileList& fileList = *m_fileList;
	for (i = 0; i < fileList.GetCount(); i++)
	{
		File& fileCmp = fileList[i];

		// Grab the extension, if we're doing it.
		if (extLen != 0)
		{
			// Do the compare.
			if (fileCmp.GetExt().GetLength() < extLen)
				continue;
			if (strncmp(ext, fileCmp.GetExt(), extLen) != 0)
				continue;
		}

		if (fileCmp.GetShortName().GetLength() >= pLen)
		{
			int result;
			if (pLen == 0)
				result = 0;
			else
				result = strncmp(fileCmp.GetShortName(), filename, pLen);

			// Do last part check.
			if (result == 0  &&  lastPartLen != 0)
			{
				if (fileCmp.GetShortName().GetLength() - pLen - lastPartLen >= 0)
				{
					if (strstr((LPCTSTR)fileCmp.GetShortName() + pLen, lastPart) != NULL)
						result = 0;
					else
						result = -1;
				}
				else
					result = -1;
			}

			if (result == 0)
			{
				m_foundFiles[curPos] = i;

				if (fileCmp.GetShortName().GetLength() == pLen  &&  topOne == -1)
					topOne = curPos;

				curPos++;
			}
		}
	}

#if _MSC_VER > 1100
	m_files.SetItemCountEx(curPos);
#else
	m_files.SetItemCount(curPos);
#endif
	if (topOne == -1)
		topOne = 0;
	if ((m_files.GetItemCount() > 0  &&  m_lastPosition == -1)  ||  m_oldFilename != name)
		m_files.SetItemState(topOne, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	m_files.SetRedraw(TRUE);

	m_oldFilename = name;
}



void CFindFileDialog::OnChangeFfFilename() 
{
	// Get the filename.
	CString filename;
	m_edit.GetWindowText(filename);
	RefreshList(filename);

}

void CFindFileDialog::OnOK() 
{
	if (m_files.GetItemCount() != 0)
	{
		CString filename;
		if (m_files.GetSelectedCount() == 0)
			m_files.SetItemState(0, 0, LVIS_SELECTED | LVIS_FOCUSED);
		int nCount = m_files.GetSelectedCount();

		CComPtr<IDispatch> p_Documents;
		m_pApplication->get_Documents(&p_Documents);
		CComQIPtr<IDocuments, &IID_IDocuments> pDocuments(p_Documents);

		int curSel = m_files.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		while (curSel != -1)
		{
			const File& file = (*m_fileList)[m_foundFiles[curSel]];

			CString filename = file.GetPath() + file.GetTitle() + "." + file.GetExt();

			CComPtr<IDispatch> p_Document;
			pDocuments->Open(CComBSTR(filename), CComVariant("Auto"), CComVariant(VARIANT_FALSE), &p_Document);

			curSel = m_files.GetNextItem(curSel, LVNI_ALL | LVNI_SELECTED);
		}
	}
	
	m_lastPosition = m_files.GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
	m_edit.GetWindowText(CFindFileDialog::m_lastFilename);
	CDialog::OnOK();
}

void CFindFileDialog::OnCancel() 
{
	m_lastPosition = m_files.GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
	m_edit.GetWindowText(CFindFileDialog::m_lastFilename);
	
	CDialog::OnCancel();
}

void CFindFileDialog::OnPfAbout() 
{
	CDialog dlg(IDD_ABOUTBOX);
	dlg.DoModal();
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}


void CFindFileDialog::OnDblclkFfFiles(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
	*pResult = 0;
}


void CFindFileDialog::OnFfExtraprojects() 
{
	CExtraProjectsDialog dlg;
	dlg.workspaceLocation = WorkspaceInfo::GetWorkspaceLocation();
	if (dlg.DoModal() == IDOK)
	{
		WorkspaceInfo::Refresh();
		OnChangeFfFilename();
	}
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}

void CFindFileDialog::OnFfPreferences() 
{
	CPreferencesDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		WorkspaceInfo::RemoveAll();
		WorkspaceInfo::Refresh();
		OnChangeFfFilename();
	}
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}


void CFindFileDialog::OnFfRefresh() 
{
	CompilerFiles::Refresh();
	OnChangeFfFilename();
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}
