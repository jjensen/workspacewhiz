///////////////////////////////////////////////////////////////////////////////
// $Workfile: FindTagDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/FindTagDialog.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 47   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WorkspaceWhiz.h"
#include "FindTagDialog.h"
#include "AboutDialog.h"
#include "FindFileDialog.h"
#include "PreferencesDialog.h"
#include "ExtraFilesDialog.h"
#include "TextLine.h"
#include "BCMenu.h"

extern bool GotoTag(const WWhizTag* tag);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void LOG(LPCTSTR msg, ...)
{
	va_list args;
	char textBuffer[1024];

	va_start(args, msg);
	_vsnprintf(textBuffer, 1023, msg, args);
	va_end(args);

	OutputDebugString(textBuffer);
}

/////////////////////////////////////////////////////////////////////////////
// CFindTagDialog dialog

int CFindTagDialog::m_lastPosition = -1;
CString CFindTagDialog::m_lastFunction;
CString CFindTagDialog::m_oldFunction;
int CFindTagDialog::s_typeFilter;
CFindTagDialog::TagScope CFindTagDialog::s_tagScope = ScopeEntireWorkspace;

CFindTagDialog::CFindTagDialog(CWnd* pParent /*=NULL*/) :
	m_created(false),
	m_fileTagList(NULL),
	m_project(NULL),
	m_tagParent(NULL),
	m_edit(NULL),
	m_tagListCtrl(NULL),
	FIND_TAG_DIALOG(CFindTagDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindTagDialog)
	//}}AFX_DATA_INIT
}


CFindTagDialog::~CFindTagDialog()
{
}

void CFindTagDialog::DoDataExchange(CDataExchange* pDX)
{
	FIND_TAG_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTagDialog)
	DDX_Control(pDX, IDC_FT_INFO_MISC, m_infoMisc);
	DDX_Control(pDX, IDC_COM_EXTRAFILES, m_butExtraFiles);
	DDX_Control(pDX, IDC_COM_CURRENTFILE, m_butCurrentFile);
	DDX_Control(pDX, IDC_FT_TYPECOMBO, m_typeCombo);
	DDX_Control(pDX, IDC_FT_SHOW, m_butShow);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_FT_REBUILDTAGS, m_butRebuildTags);
	DDX_Control(pDX, IDC_COM_PREFERENCES, m_butPreferences);
	DDX_Control(pDX, IDHELP, m_butHelp);
	DDX_Control(pDX, IDC_COM_ABOUT, m_butAbout);
	DDX_Control(pDX, IDC_FT_INFO_SEARCHSTRING, m_infoSearchString);
	DDX_Control(pDX, IDC_FT_INFO_FILENAME, m_infoFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTagDialog, FIND_TAG_DIALOG)
	//{{AFX_MSG_MAP(CFindTagDialog)
	ON_BN_CLICKED(IDC_COM_ABOUT, OnFtAbout)
	ON_BN_CLICKED(IDC_COM_PREFERENCES, OnFtPreferences)
	ON_NOTIFY(NM_DBLCLK, IDC_FT_TAGS, OnDblclkFtTags)
	ON_BN_CLICKED(IDC_FT_SCOPE_FILE, OnFtScopeFile)
	ON_BN_CLICKED(IDC_FT_SCOPE_PROJECT, OnFtScopeProject)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FT_TAGS, OnItemchangedFtTags)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FT_SHOW, OnItemchangedFtShow)
	ON_CBN_SELCHANGE(IDC_FT_TYPECOMBO, OnSelchangeFtTypecombo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_COM_CURRENTFILE, OnFtCurrentfile)
	ON_BN_CLICKED(IDC_COM_EXTRAFILES, OnFtExtraprojects)
	ON_BN_CLICKED(IDC_FT_SCOPE_WORKSPACE, OnFtScopeWorkspace)
	ON_BN_CLICKED(IDC_FT_REFRESHTAGS, OnFtRefreshtags)
	ON_NOTIFY(NM_RCLICK, IDC_FT_TAGS, OnRclickFtTags)
	ON_NOTIFY(NM_CHAR, IDC_FT_PARENT, OnChangeFtTag)
	ON_BN_CLICKED(IDC_FT_REBUILDTAGS, OnFtRebuildTags)
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(NM_CHAR, IDC_FT_NAME, OnChangeFtTag)
	ON_CONTROL(WM_USER_REFRESH, IDC_FT_PARENT, OnFtRefreshtags)
	ON_CONTROL(WM_USER_REFRESH, IDC_FT_NAME, OnFtRefreshtags)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTagDialog message handlers

static CString s_windowTitle;


void ProcessEvents()
{
#if 0
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
		{
			AfxPostQuitMessage(0);
			return;
		}
	}
#endif
}


void TagCallback(const WWhizInterface::TagCallbackInfo& info)
{
	if (info.m_type == WWhizInterface::TagCallbackInfo::REFRESH)
	{
		CWnd* wnd = (CWnd*)info.m_userData;
		CString windowTitle = s_windowTitle;
		if (windowTitle.IsEmpty())
			windowTitle = "Workspace Whiz!";

		if (info.m_curFile == 0)
		{
			CString str;
			str.Format("%s - Checking for updates...", windowTitle);
			if (wnd)
				wnd->SetWindowText(str);
			PrintStatusBar(str);
			ProcessEvents();
		}

		else if (info.m_curFile == info.m_numFiles)
		{
			CString str;
			str.Format("%s - Done", windowTitle);
			if (wnd)
				wnd->SetWindowText(str);
			PrintStatusBar(str);
			ProcessEvents();
		}

		else if ((info.m_curFile % 20) == 0)
		{
			CString filename = info.m_curFilename;
			int slashPos = filename.ReverseFind('\\');
			CString path;
			if (slashPos != -1)
			{
				path = filename.Left(slashPos);
				filename = filename.Mid(slashPos + 1);
			}

			CString str;
			str.Format("%s - Parsing %d / %d (%s) (%s)", windowTitle, info.m_curFile,
				info.m_numFiles, filename, path);
			if (wnd)
				wnd->SetWindowText(str);
			PrintStatusBar(str);
			ProcessEvents();
		}
	}
	else if (info.m_type == WWhizInterface::TagCallbackInfo::LOAD)
	{
		if (info.m_curFile == info.m_numFiles)
		{
			PrintStatusBar("Loading tags - Done");
			ProcessEvents();
		}

		if ((info.m_curFile % 20) == 0)
		{
			CString str;
			str.Format("Loading tags - %d / %d (%s)", info.m_curFile, info.m_numFiles,
				info.m_curFilename);
			PrintStatusBar(str);
			ProcessEvents();
		}
	}
	else if (info.m_type == WWhizInterface::TagCallbackInfo::SAVE)
	{
		if (info.m_curFile == info.m_numFiles)
		{
			PrintStatusBar("Saving tags - Done");
			ProcessEvents();
		}

		if ((info.m_curFile % 20) == 0)
		{
			CString str;
			str.Format("Saving tags - %d / %d (%s)", info.m_curFile, info.m_numFiles,
				info.m_curFilename);
			PrintStatusBar(str);
			ProcessEvents();
		}
	}
}




BEGIN_DYNAMIC_MAP(CFindTagDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDOK,					mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_ABOUT,		mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDHELP,				mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_PREFERENCES,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_EXTRAFILES,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_COM_CURRENTFILE,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_REBUILDTAGS,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_REFRESHTAGS,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_SHOW,			mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_FT_SCOPE,			mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_SCOPE_FILE,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_SCOPE_PROJECT,	mdRepos,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_SCOPE_WORKSPACE,mdRepos,	mdNone)

	DYNAMIC_MAP_ENTRY(IDC_FT_PARENT,		mdNone,		mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_NAME,			mdResize,	mdNone)
	DYNAMIC_MAP_ENTRY(IDC_FT_TAGS,			mdResize,	mdResize)

	DYNAMIC_MAP_ENTRY(IDC_FT_INFO_FRAME,	mdResize,	mdRepos)
	DYNAMIC_MAP_ENTRY(IDC_FT_INFO_FILENAME,	mdResize,	mdRepos)
	DYNAMIC_MAP_ENTRY(IDC_FT_INFO_SEARCHSTRING,mdResize,mdRepos)
	DYNAMIC_MAP_ENTRY(IDC_FT_INFO_MISC,		mdResize,mdRepos)
END_DYNAMIC_MAP()


BOOL CFindTagDialog::OnInitDialog() 
{
	FIND_TAG_DIALOG::OnInitDialog();
	
	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	// Registered tag controls.
	g_wwhizReg->CreateTagControls(this, IDD, m_tagParent, m_edit, m_tagListCtrl, &m_tagArray );
	
	// Subclass the edit with our own.
	m_edit->SetWindowText(m_lastFunction);
	m_edit->SetSel(0, -1, FALSE);
	
	bool fileFound = false;
	CString filename;
	if (g_wwhizInterface->GetCurrentFilename(filename))
	{
		WWhizFile* file = g_wwhizInterface->GetActiveFileList().Find(filename);
		if (file)
		{
			fileFound = true;
			m_fileTagList = &file->GetTagList();

			if (s_tagScope == ScopeCurrentFile)
				((CButton*)GetDlgItem(IDC_FT_SCOPE_FILE))->SetCheck(1);
			else if (s_tagScope == ScopeCurrentProject)
				((CButton*)GetDlgItem(IDC_FT_SCOPE_PROJECT))->SetCheck(1);
			else if (s_tagScope == ScopeEntireWorkspace)
				((CButton*)GetDlgItem(IDC_FT_SCOPE_WORKSPACE))->SetCheck(1);
		}
	}

	if (!fileFound)
	{
		GetDlgItem(IDC_FT_SCOPE_FILE)->EnableWindow(FALSE);
		if (s_tagScope == ScopeCurrentFile)
			s_tagScope = ScopeEntireWorkspace;

		if (s_tagScope == ScopeCurrentProject)
			((CButton*)GetDlgItem(IDC_FT_SCOPE_PROJECT))->SetCheck(1);
		else if (s_tagScope == ScopeEntireWorkspace)
			((CButton*)GetDlgItem(IDC_FT_SCOPE_WORKSPACE))->SetCheck(1);
	}

	m_project = g_wwhizInterface->GetCurrentProject();
	if (!m_project)
	{
		GetDlgItem(IDC_FT_SCOPE_PROJECT)->EnableWindow(FALSE);
	}
	
/*	if (m_tagListCtrl->GetItemCount() > 0  &&  m_lastPosition != -1)
	{
		CListCtrl_SetItemState(*m_tagListCtrl, m_lastPosition, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_tagListCtrl->EnsureVisible(m_lastPosition, FALSE);
	}
*/	
	m_butShow.AddChoice("Classes", g_wwhizInterface->GetTagUseType(WWhizTag::CLASS));
	m_butShow.AddChoice("Declaration", g_wwhizInterface->GetTagUseType(WWhizTag::DECLARATION));
	m_butShow.AddChoice("Defines", g_wwhizInterface->GetTagUseType(WWhizTag::DEFINE));
	m_butShow.AddChoice("Enums", g_wwhizInterface->GetTagUseType(WWhizTag::ENUM));
	m_butShow.AddChoice("Enum Members", g_wwhizInterface->GetTagUseType(WWhizTag::ENUMMEMBER));
	m_butShow.AddChoice("Files", g_wwhizInterface->GetTagUseType(WWhizTag::FILE));
	m_butShow.AddChoice("Functions", g_wwhizInterface->GetTagUseType(WWhizTag::FUNCTION));
	m_butShow.AddChoice("Member Variables", g_wwhizInterface->GetTagUseType(WWhizTag::MEMBER));
	m_butShow.AddChoice("Namespaces", g_wwhizInterface->GetTagUseType(WWhizTag::NAMESPACE));
	m_butShow.AddChoice("Structures", g_wwhizInterface->GetTagUseType(WWhizTag::STRUCTURE));
	m_butShow.AddChoice("Typedefs", g_wwhizInterface->GetTagUseType(WWhizTag::TYPEDEF));
	m_butShow.AddChoice("Unions", g_wwhizInterface->GetTagUseType(WWhizTag::UNION));
	m_butShow.AddChoice("Variables", g_wwhizInterface->GetTagUseType(WWhizTag::VARIABLE));
	
	m_typeCombo.AddString("[All]");
	m_typeCombo.AddString("Classes");
	m_typeCombo.AddString("Declarations");
	m_typeCombo.AddString("Defines");
	m_typeCombo.AddString("Enums");
	m_typeCombo.AddString("Enum Members");
	m_typeCombo.AddString("File");
	m_typeCombo.AddString("Functions");
	m_typeCombo.AddString("Member Variables");
	m_typeCombo.AddString("Namespaces");
	m_typeCombo.AddString("Structures");
	m_typeCombo.AddString("Typedefs");
	m_typeCombo.AddString("Unions");
	m_typeCombo.AddString("Variables");
	m_typeCombo.SetCurSel(s_typeFilter);

	if (!g_wwhizInterface->GetCurrentFilename(filename))
	{
		GetDlgItem(IDC_COM_CURRENTFILE)->EnableWindow(FALSE);
	}

	m_saveLastPosition = m_lastPosition;
	m_saveOldFunction = m_oldFunction;

	GetWindowText(s_windowTitle);

	CRect windowRect;
	GetWindowRect(windowRect);
	SetWindowPos(NULL, 0, 0, windowRect.Width(), windowRect.Height(), SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFindTagDialog::Refresh()
{
	CWaitCursor cursor;
	
	g_wwhizInterface->SetTagCallback(TagCallback, this);
	
	SetWindowText(s_windowTitle + " - Checking for updates...");

	g_wwhizInterface->RefreshFileList();
	g_wwhizInterface->RefreshTagList(false);

	m_project = g_wwhizInterface->GetCurrentProject();
	if (m_project)
	{
		GetDlgItem(IDC_FT_SCOPE_PROJECT)->EnableWindow(TRUE);
	}
	
	SetWindowText(s_windowTitle);

	m_created = true;

	RefreshList(m_lastFunction);
}


void CFindTagDialog::OnFtRebuildTags()
{
	CWaitCursor cursor;

	SetWindowText(s_windowTitle + " - Deleting all items...");

	CListCtrl_SetItemCountEx(*m_tagListCtrl, 0);
	m_tagListCtrl->Invalidate();
	m_tagListCtrl->UpdateWindow();
	m_tagArray.RemoveAll();

	SetWindowText(s_windowTitle + " - Checking for updates...");

	g_wwhizInterface->SetTagCallback(TagCallback, this);

	g_wwhizInterface->RefreshFileList();
	g_wwhizInterface->RemoveAllTags();
	g_wwhizInterface->RefreshTagList(true, true);

	SetWindowText(s_windowTitle);
	
	m_created = true;

	// Get the filename.
	CString filename;
	m_edit->GetWindowText(filename);
	RefreshList(filename);
}


void CFindTagDialog::RefreshFromEdit()
{
	// Get the filename.
	CString tagParent;
	m_tagParent->GetWindowText(tagParent);
	CString filename;
	m_edit->GetWindowText(filename);
	if (!tagParent.IsEmpty())
		filename = filename + ";" + tagParent;
	RefreshList(filename);
}


void CFindTagDialog::OnChangeFtTag(NMHDR* pNMHDR, LRESULT* pResult)
{
	RefreshFromEdit();
}


void CFindTagDialog::RefreshList(LPCTSTR name) 
{
	if (!m_created)
		return;
	
	// Turn off any redraw while we fill the list control.
	m_tagListCtrl->SetRedraw(FALSE);
	
	// Get the current filename.
	const WWhizTagList* tags = GetCurTagList();
	
	// Perform the matching.
	m_tagArray.RemoveAll();
	g_wwhizInterface->MatchTag(*tags, name);
	if (g_wwhizInterface->GetTagMatchCount() == 0)
	{
		// Remove all items from the list.
		CListCtrl_SetItemCountEx(*m_tagListCtrl, 0);
		m_tagListCtrl->SetRedraw(TRUE);
		m_tagListCtrl->Invalidate();

		return;
	}

	// Set the number of items to the maximum.
	m_tagArray.SetSize(g_wwhizInterface->GetTagMatchCount());

	// Start looking for it.
	int curPos = 0;
	const WWhizTag* tag = g_wwhizInterface->GetTagMatchHead();
	for (; tag; tag = tag->GetMatchNext())
	{
		if (!g_wwhizInterface->GetTagUseType(tag->GetType()))
			continue;

		if (s_typeFilter != 0  &&  tag->GetType() != (WWhizTag::Type)(s_typeFilter - 1))
			continue;

		if (tag->GetType() == WWhizTag::DECLARATION)
		{
			// Check for a buddy.
			if (tag->GetBuddy())
			{
				// It has a buddy... skip it.
				continue;
			}
		}

		m_tagArray[curPos] = tag;

		curPos++;
	}

	m_tagArray.SetSize(curPos);
	CListCtrl_SetItemCountEx(*m_tagListCtrl, curPos);

	POSITION pos = CListCtrl_GetFirstSelectedItemPosition(*m_tagListCtrl);
	while (pos)
	{
		int nItem = CListCtrl_GetNextSelectedItem(*m_tagListCtrl, pos);
		CListCtrl_SetItemState(*m_tagListCtrl, nItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
	}

	if (m_lastPosition >= m_tagListCtrl->GetItemCount())
		m_lastPosition = -1;
	if (m_tagListCtrl->GetItemCount() > 0)
	{
		int setPos;
		if (m_lastPosition == -1  ||  m_oldFunction != name)
			setPos = 0;
		else
			setPos = m_lastPosition;
		CListCtrl_SetItemState(*m_tagListCtrl, setPos, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_tagListCtrl->EnsureVisible(setPos, FALSE);
	}
	else
	{
		m_lastPosition = -1;
	}

	m_tagListCtrl->SetRedraw(TRUE);
	m_tagListCtrl->Invalidate();

	CString str;
	str.Format("%s (%d / %d tags)", s_windowTitle, m_tagListCtrl->GetItemCount(), tags->GetCount());
	SetWindowText(str);

	m_oldFunction = name;

	CRect windowRect;
	GetWindowRect(windowRect);
	SetWindowPos(NULL, 0, 0, windowRect.Width(), windowRect.Height(), SWP_NOMOVE | SWP_NOZORDER);
}



void CFindTagDialog::OnOK() 
{
	GotoTags(false);

	FIND_TAG_DIALOG::OnOK();
}

void CFindTagDialog::GotoTags(bool declaration)
{
	if (m_tagListCtrl->GetItemCount() != 0)
	{
		CString filename;
		if (m_tagListCtrl->GetSelectedCount() == 0)
			CListCtrl_SetItemState(*m_tagListCtrl, 0, 0, LVIS_SELECTED | LVIS_FOCUSED);
		int nCount = m_tagListCtrl->GetSelectedCount();

		int curSel = m_tagListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		while (curSel != -1)
		{
			const WWhizTag* tag = m_tagArray[curSel];

			if (declaration  ||  GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				if (tag->GetBuddy())
					GotoTag(tag->GetBuddy());
				else
					GotoTag(tag);
			}
			else
			{
				GotoTag(tag);
			}

			curSel = m_tagListCtrl->GetNextItem(curSel, LVNI_ALL | LVNI_SELECTED);
		}
	}
	
	m_lastPosition = m_tagListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
	m_edit->GetWindowText(m_lastFunction);
}

void CFindTagDialog::OnCancel() 
{
	m_lastPosition = m_saveLastPosition;
	m_oldFunction = m_saveOldFunction;
	
	FIND_TAG_DIALOG::OnCancel();
}

void CFindTagDialog::OnFtAbout() 
{
	CAboutDialog dlg;
	dlg.DoModal();
	GetDlgItem(IDC_FT_NAME)->SetFocus();
}

void CFindTagDialog::OnDblclkFtTags(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

void CFindTagDialog::OnFtPreferences() 
{
	CPreferencesDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
	m_edit->SetFocus();
}




void CFindTagDialog::OnFtScopeFile() 
{
	s_tagScope = ScopeCurrentFile;
	RefreshFromEdit();
}

void CFindTagDialog::OnFtScopeProject() 
{
	s_tagScope = ScopeCurrentProject;
	RefreshFromEdit();
}

void CFindTagDialog::OnFtScopeWorkspace() 
{
	s_tagScope = ScopeEntireWorkspace;
	RefreshFromEdit();
}


void CFindTagDialog::OnItemchangedFtTags(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int iItem = pNMListView->iItem;
	if (iItem != -1  &&  iItem >= 0  &&  iItem < m_tagArray.GetSize())
	{
		const WWhizTag* tag = m_tagArray[iItem];
		const WWhizTag* accessTag = tag;
		if (tag->GetBuddy())
			accessTag = tag->GetBuddy();
		m_infoFilename.SetWindowText(tag->GetFilename());
		m_infoFilename.Invalidate();

		CString str;
		LPTSTR buf = str.GetBuffer(strlen(tag->GetSearchString()));
		LPCTSTR origBuf = tag->GetSearchString();
		if (*origBuf == '^')
			origBuf++;
		while (true)
		{
			if (origBuf[0] == '$'  &&  origBuf[1] == 0)
				break;
			if (origBuf[0] == 0)
				break;

			if (*origBuf == '\t')
				*buf = ' ';
			else
				*buf = *origBuf;

			buf++;
			origBuf++;
		}
		*buf = 0;
		str.ReleaseBuffer();
		str.TrimLeft();
		m_infoSearchString.SetWindowText(str);
		m_infoSearchString.Invalidate();

		CString temp;
		switch (accessTag->GetAccess())
		{
			case WWhizTag::PUBLIC:		temp = "Public";		break;
			case WWhizTag::PROTECTED:	temp = "Protected";		break;
			case WWhizTag::PRIVATE:		temp = "Private";		break;
			case WWhizTag::FRIEND:		temp = "Friend";		break;
		}

		CString implStr;
		switch (accessTag->GetImplementationType())
		{
			case WWhizTag::DEFAULT:		implStr = "Normal";			break;
			case WWhizTag::ABSTRACT:	implStr = "Abstract";		break;
			case WWhizTag::VIRTUAL:		implStr = "Virtual";		break;
			case WWhizTag::PUREVIRTUAL:	implStr = "Pure Virtual";	break;
		}
		
		str.Format("Line: %-6d   Access: %s  Implementation: %s", tag->GetLineNumber(), temp, implStr);
		m_infoMisc.SetWindowText(str);
		m_infoMisc.Invalidate();
	}
	else
	{
		m_infoFilename.SetWindowText("");
		m_infoFilename.Invalidate();
		m_infoMisc.SetWindowText("");
		m_infoMisc.Invalidate();
		m_infoSearchString.SetWindowText("");
		m_infoSearchString.Invalidate();
	}
	
	CRect dialogRect;
	GetClientRect(dialogRect);
	ClientToScreen(dialogRect);
	
	CRect rect;
	m_infoFilename.GetClientRect(rect);
	m_infoFilename.ClientToScreen(rect);
	rect.OffsetRect(-dialogRect.left, -dialogRect.top);
	InvalidateRect(rect);

	m_infoSearchString.GetClientRect(rect);
	m_infoSearchString.ClientToScreen(rect);
	rect.OffsetRect(-dialogRect.left, -dialogRect.top);
	InvalidateRect(rect);

	*pResult = 0;
}


void CFindTagDialog::OnItemchangedFtShow(NMHDR* pNMHDR, LRESULT* pResult)
{
	for (int i = 0; i < m_butShow.GetItemCount(); i++)
		g_wwhizInterface->SetTagUseType((WWhizTag::Type)i, m_butShow.GetCheck(i) != FALSE);

	RefreshFromEdit();
}


void CFindTagDialog::OnSelchangeFtTypecombo() 
{
	int sel = m_typeCombo.GetCurSel();
	if (sel == -1)
		return;

	s_typeFilter = sel;
	RefreshFromEdit();
}

void CFindTagDialog::OnDestroy() 
{
	FIND_TAG_DIALOG::OnDestroy();
	
	g_wwhizReg->DestroyTagControls();

	s_windowTitle.Empty();

	m_created = false;
}



void CFindTagDialog::OnFtCurrentfile() 
{
	CString filename;
	if (g_wwhizInterface->GetCurrentFilename(filename))
	{
		if (CFindFileDialog::FilePopup(filename, this, NULL))
			OnCancel();
	}
}

void CFindTagDialog::OnFtExtraprojects() 
{
	CExtraFilesDialog dlg;
	dlg.workspaceLocation = g_wwhizInterface->GetWorkspaceLocation();
	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor cursor;

		g_wwhizInterface->SetTagCallback(TagCallback, this);

		SetWindowText(s_windowTitle + " - Checking for updates...");

		g_wwhizInterface->RefreshFileList();
		g_wwhizInterface->RefreshTagList();

		SetWindowText(s_windowTitle);

		RefreshFromEdit();
	}
	m_edit->SetFocus();
}


void CFindTagDialog::OnFtRefreshtags() 
{
	CWaitCursor cursor;

	g_wwhizInterface->SetTagCallback(TagCallback, this);

	SetWindowText(s_windowTitle + " - Checking for updates...");

	ObjModelHelper objModel;
	objModel.SaveAll();

	g_wwhizInterface->RefreshFileList();
	g_wwhizInterface->RefreshTagList(true);

	SetWindowText(s_windowTitle);
	
	m_created = true;

	// Get the filename.
	CString filename;
	m_edit->GetWindowText(filename);
	RefreshList(filename);
}

const WWhizTagList* CFindTagDialog::GetCurTagList()
{
	const WWhizTagList* tags = NULL;

	if (s_tagScope == ScopeCurrentFile)
		tags = m_fileTagList;
	else if (s_tagScope == ScopeCurrentProject)
		tags = &m_project->GetTagList();
	else if (s_tagScope == ScopeEntireWorkspace)
		tags = &g_wwhizInterface->GetTagList();

	return tags;
}

void CFindTagDialog::OnRclickFtTags(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* info = (NMLISTVIEW*)pNMHDR;
	
	if (info->iItem != -1)
	{
		if (TagPopup(info->iItem, this, NULL) == 1)
			OnCancel();
	}
	
	*pResult = 1;
}


int CFindTagDialog::TagPopup(int item, CWnd* thisWnd, CPoint* inPoint)
{
	const WWhizTag* tag = m_tagArray[item];

	CPoint point;
	if (!inPoint)
		::GetCursorPos(&point);
	else
		point = *inPoint;

	CMenu menu;
	menu.LoadMenu(IDM_FINDTAG_POPUP);

	if (!tag->GetBuddy())
	{
		if (tag->GetType() == WWhizTag::DECLARATION)
		{
			menu.GetSubMenu(0)->EnableMenuItem(IDC_FTP_GOTOTAGDEFINITION, MF_DISABLED | MF_GRAYED);
		}
		else
		{
			menu.GetSubMenu(0)->EnableMenuItem(IDC_FTP_GOTOTAGDECLARATION, MF_DISABLED | MF_GRAYED);
		}
	}
	
	BOOL ret = menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		point.x, point.y,
		thisWnd, NULL);

	if (ret == 0)
		return 0;
	
	switch (ret)
	{
		case IDC_FTP_GOTOTAGDEFINITION:
		{
			OnOK();
			return 2;
		}

		case IDC_FTP_GOTOTAGDECLARATION:
		{
			GotoTags(true);
			FIND_TAG_DIALOG::OnOK();
			return 2;
		}

		case IDC_FTP_COPYTAG:
		case IDC_FTP_COPYPARENT:
		{
			// Hack to make this work.
			int subItem = 2;
			if (ret == IDC_FTP_COPYPARENT)
				subItem = 1;
			CString str = m_tagListCtrl->GetItemText(item, subItem);

			CEdit edit;
			edit.Create(0, CRect(0, 0, 0, 0), thisWnd, 0);
			edit.SetWindowText(str);
			edit.SetSel(0, -1);
			edit.Copy();
/*			if ( ::OpenClipboard(NULL) )
			{
				::EmptyClipboard();

				HANDLE mem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, str.GetLength() + 1);
				void *mymem = GlobalLock(mem);
				memcpy(mem, (LPCTSTR)str, str.GetLength() + 1);
				GlobalUnlock(mem);

				int err = 0;
				if ( ::SetClipboardData(CF_TEXT, mem) == NULL )
				{
					err = GetLastError();
				}

				::CloseClipboard();
				ASSERT(err == 0);
			}
*/
			return 1;
		}
	}

	return 0;
}


void CFindTagDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd != m_tagListCtrl)
	{
		DefWindowProc(WM_CONTEXTMENU, (WPARAM)pWnd->GetSafeHwnd(), point.x + (point.y << 16));
		return;
	}

	if (m_tagListCtrl->GetSelectedCount() == 0)
		return;
	
	int curSel = m_tagListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (curSel == -1)
		return;

	// Get the point for the current selection.
	m_tagListCtrl->GetItemPosition(curSel, &point);
	CPoint point2;
	m_tagListCtrl->GetItemPosition(curSel + 1, &point2);
	point.y += point2.y - point.y;
	m_tagListCtrl->ClientToScreen(&point);
	
	if (TagPopup(curSel, this, &point))
		OnCancel();
}
