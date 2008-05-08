///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindFileDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/FindFileDialog.cpp $
// $Date: 2003/01/05 $ $Revision: #10 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "FindFileDialog.h"
#include "History.h"
#include "AboutDialog.h"
#include "ExtraFilesDialog.h"
#include "ShellContextMenu.h"
#include "BCMenu.h"
#include "ShellTools.h"
#include <io.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString s_windowTitle;

/////////////////////////////////////////////////////////////////////////////
// CFindFileDialog dialog

CFindFileDialog::FileScope CFindFileDialog::s_fileScope = CFindFileDialog::ScopeEntireWorkspace;

CFindFileDialog::CFindFileDialog(CWnd* pParent /*=NULL*/)
:	m_created(false),
	m_lastPosition(-1),
	m_files(NULL),
	m_edit(NULL),
	FIND_FILE_DIALOG(CFindFileDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindFileDialog)
	//}}AFX_DATA_INIT
}

CFindFileDialog::~CFindFileDialog()
{
}

void CFindFileDialog::DoDataExchange(CDataExchange* pDX)
{
	FIND_FILE_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindFileDialog)
	DDX_Control(pDX, IDC_COM_CURRENTFILE, m_butCurrentFile);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
//	DDX_Control(pDX, IDC_FF_REFRESH, m_butRefresh);
	DDX_Control(pDX, IDHELP, m_butHelp);
	DDX_Control(pDX, IDC_COM_EXTRAFILES, m_butExtraFiles);
	DDX_Control(pDX, IDC_COM_ABOUT, m_butAbout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindFileDialog, FIND_FILE_DIALOG)
	//{{AFX_MSG_MAP(CFindFileDialog)
	ON_BN_CLICKED(IDC_COM_ABOUT, OnPfAbout)
	ON_NOTIFY(NM_DBLCLK, IDC_FF_FILES, OnDblclkFfFiles)
	ON_BN_CLICKED(IDC_COM_EXTRAFILES, OnFfExtraprojects)
//	ON_BN_CLICKED(IDC_FF_REFRESH, OnFfRefresh)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_FF_FILES, OnRclickFfFiles)
	ON_BN_CLICKED(IDC_COM_CURRENTFILE, OnFfCurrentfile)
	ON_BN_CLICKED(IDC_FF_SCOPE_PROJECT, OnFfScopeProject)
	ON_BN_CLICKED(IDC_FF_SCOPE_WORKSPACE, OnFfScopeWorkspace)
	ON_BN_CLICKED(IDC_FF_SCOPE_GLOBAL, OnFfScopeGlobal)
	ON_NOTIFY(NM_CHAR, IDC_FF_FILENAME, OnChangeFfFilename)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CHAR, IDC_FF_PREFIX, OnChangeFfFilename)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_CBN_CLOSEUP(IDC_FF_FILENAME, OnCbnCloseupFfFilename)
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CFindFileDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDC_COM_EXTRAFILES,	mdRepos,	mdNone)
//	DYNAMIC_MAP_ENTRY(IDC_FF_REFRESH,		mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_CURRENTFILE,	mdRepos,	mdNone)
	
	DYNAMIC_MAP_ENTRY(IDOK,					mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDHELP,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_ABOUT,		mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_FF_SCOPE_PROJECT,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FF_SCOPE_WORKSPACE,mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FF_SCOPE_GLOBAL,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FF_SCOPE,			mdRepos,	mdNone)
	
	DYNAMIC_MAP_ENTRY(IDC_FF_FILENAME,		mdResize,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FF_PREFIX,		mdNone,		mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FF_FILES,			mdResize,	mdResize)
END_DYNAMIC_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFileDialog message handlers

BOOL CFindFileDialog::OnInitDialog()
{
	FIND_FILE_DIALOG::OnInitDialog();

	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	GetWindowText(s_windowTitle);

	g_wwhizReg->CreateWFOControls(this, IDD, m_edit, m_prefixEdit, m_files, &m_fileList, &m_foundFiles );
	
	// Subclass the edit with our own.
    m_prefixEdit->SetWindowText(m_lastPrefix);
    m_edit->SetWindowText(m_lastFilename);
	m_edit->CComboBox::SetEditSel(0, -1);

	// Hide some stuff.
//	m_origFileList = m_fileList;

	if (m_lastPosition != -1)
	{
		m_oldFilename = m_lastFilename;
		m_oldPrefix = m_lastPrefix;
	}
	m_saveOldFilename = m_oldFilename;
	m_saveOldPrefix = m_oldPrefix;
	m_saveLastPosition = m_lastPosition;
//	RefreshList(m_lastPrefix + m_lastFilename);

	m_created = true;

	m_curProject = g_wwhizInterface->GetCurrentProject();
	if (!m_curProject)
	{
		GetDlgItem(IDC_FF_SCOPE_PROJECT)->EnableWindow(FALSE);
		if (s_fileScope == ScopeCurrentProject)
			s_fileScope = ScopeEntireWorkspace;
	}
	if (s_fileScope == ScopeCurrentProject)
		OnFfScopeProject();
	else if (s_fileScope == ScopeEntireWorkspace)
		OnFfScopeWorkspace();
	else if (s_fileScope == ScopeGlobal)
		OnFfScopeGlobal();

	if (m_files->GetItemCount() > 0  &&  m_lastPosition != -1)
	{
		if (m_lastPosition > m_files->GetItemCount())
			m_lastPosition = 0;
		m_files->SetItemState(m_lastPosition, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_files->SetItemState(m_lastPosition, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_files->EnsureVisible(m_lastPosition, FALSE);
	}

	CString filename;
	if (!g_wwhizInterface->GetCurrentFilename(filename))
	{
		GetDlgItem(IDC_COM_CURRENTFILE)->EnableWindow(FALSE);
	}

	CRect windowRect;
	GetWindowRect(windowRect);
	SetWindowPos(NULL, 0, 0, windowRect.Width(), windowRect.Height(), SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFindFileDialog::RefreshList(LPCTSTR name) 
{
	if (m_created == false)
		return;
	
	// Turn off any redraw while we fill the list control.
	m_files->SetRedraw(FALSE);
	
	// Remove all items from the list.
	m_files->DeleteAllItems();

	// We're going to modify the text, so copy it.
	char filename[200];
	strcpy(filename, name);

	// Is there a comma (line number) in the filename?
	char* comma = strrchr(filename, ',');
	if (!comma)
		comma = strrchr(filename, ':');
	if (comma != NULL)
	{
		*comma = 0;
	}
	
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
		else if (filename[i] == '/'  ||  filename[i] == '*')
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
	
	m_foundFiles.RemoveAll();
	m_foundFiles.SetCount(m_fileList->GetCount());

	const WWhizFileList& fileList = *m_fileList;
	int fileListCount = fileList.GetCount();
	for (i = 0; i < fileListCount; i++)
	{
		WWhizFile& fileCmp = *fileList.Get(i);

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

	m_files->SetItemCountEx(curPos);

	if (topOne == -1)
		topOne = 0;

	if ((m_files->GetItemCount() > 0  &&  m_lastPosition == -1)  ||  m_oldFilename != name)
		m_files->SetItemState(topOne, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	m_files->SetRedraw(TRUE);

	CString str;
	str.Format("%s (%d / %d files)", s_windowTitle, m_files->GetItemCount(), m_fileList->GetCount());
	SetWindowText(str);

	m_oldFilename = name;
}


void CFindFileDialog::RefreshFromFilename()
{
	// Get the filename.
	CString prefix;
	m_prefixEdit->GetWindowText(prefix);
	CString filename;
	m_edit->GetWindowText(filename);
	RefreshList(prefix + filename);
}


void CFindFileDialog::OnChangeFfFilename(NMHDR* pNMHDR, LRESULT* pResult) 
{
	RefreshFromFilename();
}

void CFindFileDialog::OnOK() 
{
	CString string;
	m_edit->GetWindowText(string);
	g_wwhizReg->WFOAddEditString(string);
	
	m_prefixEdit->GetWindowText(string);
	g_wwhizReg->WFOAddPrefixEditString(string);
	
	if (m_files->GetItemCount() != 0)
	{
		History::PreAdd();

		CString filename;
		if (m_files->GetSelectedCount() == 0)
			m_files->SetItemState(0, 0, LVIS_SELECTED | LVIS_FOCUSED);
		int nCount = m_files->GetSelectedCount();

		WArray<WWhizFile*> toOpenList;
		
		int lineNumber = -1;
		CString typedFilename;
		m_edit->GetWindowText(typedFilename);
		int commaPos = typedFilename.ReverseFind(',');
		if (commaPos == -1)
			commaPos = typedFilename.ReverseFind(':');
		if (commaPos != -1)
		{
			lineNumber = atoi(typedFilename.Mid(commaPos + 1));
		}

		int curSel = m_files->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		while (curSel != -1)
		{
			WWhizFile* file = m_fileList->Get(m_foundFiles[curSel]);
			toOpenList.Add(file);

			curSel = m_files->GetNextItem(curSel, LVNI_ALL | LVNI_SELECTED);
		}

		for (int i = toOpenList.GetCount() - 1; i >= 0; --i)
		{
			WWhizFile& file = *toOpenList[i];
			CString filename = file.GetPath() + file.GetTitle() + "." + file.GetExt();

			ObjModelHelper objModel;
			if (objModel.OpenDocument(filename, "Auto"))
				History::Add();

			if (lineNumber > 0)
			{
				objModel.MoveTo(lineNumber, 1, false);
				objModel.ScrollToCenter();
			}
		}
	}
	
	m_lastPosition = m_files->GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
	m_edit->GetWindowText(CFindFileDialog::m_lastFilename);
	m_prefixEdit->GetWindowText(CFindFileDialog::m_lastPrefix);
	FIND_FILE_DIALOG::OnOK();
}

void CFindFileDialog::OnCancel() 
{
//	m_lastPosition = m_files->GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
//	m_edit->GetWindowText(CFindFileDialog::m_lastFilename);
	m_lastPosition = m_saveLastPosition;
	m_oldFilename = m_saveOldFilename;
	m_oldPrefix = m_saveOldPrefix;
	
	FIND_FILE_DIALOG::OnCancel();
}

void CFindFileDialog::OnPfAbout() 
{
	CAboutDialog dlg;
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
	CExtraFilesDialog dlg;
	dlg.workspaceLocation = g_wwhizInterface->GetWorkspaceLocation();
	if (dlg.DoModal() == IDOK)
	{
		g_wwhizInterface->RefreshFileList();
		RefreshFromFilename();
	}
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}


/*void CFindFileDialog::OnFfRefresh() 
{
	g_wwhizInterface->RefreshGlobalFileList();
	RefreshFromFilename();
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}
*/

void CFindFileDialog::OnDestroy() 
{
	FIND_FILE_DIALOG::OnDestroy();
	
//	cdxCDynamicControlsManager::StoreWindowPosition(*this, _T("Dialogs\\FindFileDialog"));

	g_wwhizReg->DestroyWFOControls();

	m_created = false;
}

void CFindFileDialog::OnRclickFfFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* info = (NMLISTVIEW*)pNMHDR;
	
	if (info->iItem != -1)
	{
		const WWhizFile& file = *m_fileList->Get(m_foundFiles[info->iItem]);
		if (FilePopup(file.GetFullName(), this, NULL))
			OnCancel();
	}

	*pResult = 1;
}


int CFindFileDialog::FilePopup(const CString& fullName, CWnd* thisWnd, CPoint* inPoint)
{
	CPoint point;
	if (!inPoint)
		::GetCursorPos(&point);
	else
		point = *inPoint;

	CMenu menu;
	menu.LoadMenu(IDM_FINDFILEPOPUP);

    CShellContextMenu scm(::GetActiveWindow(), fullName);
    const int iShellContextPos = menu.GetSubMenu(0)->GetMenuItemCount() - 1;
    scm.SetMenu((BCMenu *)menu.GetSubMenu(0)->GetSubMenu(iShellContextPos), (BCMenu*)menu.GetSubMenu(0));

	CMenu* popupMenu = (BCMenu*)menu.GetSubMenu(0);
    if (GetFileAttributes(fullName) & FILE_ATTRIBUTE_READONLY)
    {
        popupMenu->ModifyMenu(IDC_FFP_READONLY, MF_BYCOMMAND, IDC_FFP_READONLY,
            "Ma&ke Writable");
    }
    else
    {
        popupMenu->ModifyMenu(IDC_FFP_READONLY, MF_BYCOMMAND, IDC_FFP_READONLY,
            "Ma&ke Read-Only");
    }

	BOOL ret = popupMenu->TrackPopupMenu(
		TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		point.x, point.y,
		thisWnd, NULL);
	if (ret == 0)
	{
		return 0;
	}
	
    if (scm.IsMenuCommand(ret))
    {
        scm.InvokeCommand(ret);
    }
	else switch (ret)
	{
		case IDC_FFP_HEADERFLIP:
		{
			WWhizCommands::OnHeaderFlip(fullName);
			return 1;
		}

		case IDC_FFP_OPENASTEXT:
		{
			History::PreAdd();
			
			ObjModelHelper objModel;
			if (objModel.OpenDocument(fullName))
				History::Add();

			return 1;
		}

		case IDC_FFP_READONLY:
		{
			bool doOpen = true;

			// See if we're dealing with a .dsw, .vcw, .dsp, or .vcp file.
			int dotPos = fullName.ReverseFind('.');
			if (dotPos != -1)
			{
				CString ext = fullName.Mid(dotPos + 1);
				ext.MakeLower();
				if (ext == "dsw"  ||  ext == "vcw"  ||  ext == "dsp"  ||
					ext == "vcp")
				{
					doOpen = false;
				}
			}

			History::PreAdd();

			ObjModelHelper objModel;
			if (!doOpen  ||  objModel.OpenDocument(fullName, "Auto"))
			{
				History::Add();

				bool readOnly = false;
				if (GetFileAttributes(fullName) & FILE_ATTRIBUTE_READONLY)
				{
					readOnly = true;
				}

				if (doOpen)
				{
					bool succeeded = objModel.SetReadOnly(readOnly);
				}
//				if (succeeded)
				{
					if (readOnly)
						_chmod(fullName, _S_IREAD | _S_IWRITE);
					else
						_chmod(fullName, _S_IREAD);
				}
			}

			return 1;
		}

		case IDC_FFP_EXPLORE:
		{
			int slashPos = fullName.ReverseFind('\\');
			CString path;
			if (slashPos == -1)
				path = fullName;
			else
				path = fullName.Left(slashPos);
			OpenLocation(path, omDefaultAction);

			return 1;
		}

		case IDC_FFP_COPYPATHTOCLIPBOARD:
		{
			int slashPos = fullName.ReverseFind('\\');
			CString path;
			if (slashPos == -1)
				path = fullName;
			else
				path = fullName.Left(slashPos + 1);

			CEdit edit;
			edit.Create(0, CRect(0, 0, 0, 0), thisWnd, 0);
			edit.SetWindowText(path);
			edit.SetSel(0, -1);
			edit.Copy();

			return 1;
		}

		case IDC_FFP_COPYFULLPATHTOCLIPBOARD:
		{
			CEdit edit;
			edit.Create(0, CRect(0, 0, 0, 0), thisWnd, 0);
			edit.SetWindowText(fullName);
			edit.SetSel(0, -1);
			edit.Copy();

			return 1;
		}
	}

	return false;
}

void CFindFileDialog::OnFfCurrentfile() 
{
	CString filename;
	if (g_wwhizInterface->GetCurrentFilename(filename))
	{
		if (FilePopup(filename, this, NULL))
			OnCancel();
	}
}

void CFindFileDialog::OnFfScopeProject() 
{
	s_fileScope = ScopeCurrentProject;
	m_fileList = &m_curProject->GetFileList();
	m_files->SetItemCountEx(m_fileList->GetCount());

	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
	((CButton*)GetDlgItem(IDC_FF_SCOPE_PROJECT))->SetCheck(1);
	RefreshFromFilename();
//	GetDlgItem(IDC_FF_REFRESH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}


void CFindFileDialog::OnFfScopeWorkspace() 
{
	s_fileScope = ScopeEntireWorkspace;
	g_wwhizInterface->RefreshFileList();
	m_fileList = &g_wwhizInterface->GetActiveFileList();
	m_files->SetItemCountEx(m_fileList->GetCount());

//	GetDlgItem(IDC_FF_REFRESH)->ShowWindow(SW_HIDE);
	((CButton*)GetDlgItem(IDC_FF_SCOPE_WORKSPACE))->SetCheck(1);
	RefreshFromFilename();
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}


void CFindFileDialog::OnFfScopeGlobal() 
{
	s_fileScope = ScopeGlobal;
	g_wwhizInterface->RefreshGlobalFileList();
	m_fileList = &g_wwhizInterface->GetGlobalFileList();
	m_files->SetItemCountEx(m_fileList->GetCount());

//	GetDlgItem(IDC_FF_REFRESH)->ShowWindow(SW_SHOW);
	((CButton*)GetDlgItem(IDC_FF_SCOPE_GLOBAL))->SetCheck(1);
	RefreshFromFilename();
	GetDlgItem(IDC_FF_FILENAME)->SetFocus();
}



void CFindFileDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd != m_files)
	{
		DefWindowProc(WM_CONTEXTMENU, (WPARAM)pWnd->GetSafeHwnd(), point.x + (point.y << 16));
		return;
	}

	if (m_files->GetSelectedCount() == 0)
		return;
	
	int curSel = m_files->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (curSel == -1)
		return;

	// Get the point for the current selection.
	m_files->GetItemPosition(curSel, &point);
	CPoint point2;
	m_files->GetItemPosition(curSel + 1, &point2);
	point.y += point2.y - point.y;
	m_files->ClientToScreen(&point);
	
	const WWhizFile& file = *m_fileList->Get(curSel);
	if (FilePopup(file.GetCaseFullName(), this, &point))
		OnCancel();
}



void CFindFileDialog::OnCbnCloseupFfFilename()
{
	RefreshFromFilename();
}
