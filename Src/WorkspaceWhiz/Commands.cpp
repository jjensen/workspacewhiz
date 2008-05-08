///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/Commands.cpp $
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
#include "Commands.h"
#include "TemplateSelectDialog.h"
#include "FindFileDialog.h"
#include "FindTagDialog.h"
#include "WorkspaceCommands.h"
#include "History.h"
#include "HistoryDialog.h"
#include <io.h>
#include "TextLine.h"
#include "AutoCompleteTagsDlg.h"
#include "TemplateWizardDialog.h"
	 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWnd g_devStudioCWnd;

extern int OnHeaderFlip(LPCTSTR pParams);

bool GotoTag(const WWhizTag* tag)
{
	bool didAdd = History::PreAdd();
	bool ret = g_wwhizInterface->GotoTag(tag);
	if (ret)
		History::Add();
	else if (didAdd)
		History::UndoAdd();
	return ret;
}


void TagCallback(const WWhizInterface::TagCallbackInfo& info);


void LoadTags()
{
	g_wwhizInterface->SetTagCallback(TagCallback, NULL);
	g_wwhizInterface->LoadTags();
}


void SaveTags()
{
	g_wwhizInterface->SetTagCallback(TagCallback, NULL);
	g_wwhizInterface->SaveTags();
}


/**
	Stolen from the MFC 6.0 source.  See CString::Find().
**/
int CStringFind(const CString& str, LPCTSTR lpszSub, int nStart)
{
	ASSERT(AfxIsValidString(lpszSub));

	int nLength = str.GetLength();
	if (nStart > nLength)
		return -1;

	// find first matching substring
	LPTSTR lpsz = _tcsstr((LPCTSTR)str + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)str);
}


/**
	Stolen from the MFC 6.0 source.  See CString::Find().
**/
int CStringFind(const CString& str, TCHAR ch, int nStart)
{
	int nLength = str.GetLength();
	if (nStart >= nLength)
		return -1;

	// find first single character
	LPTSTR lpsz = _tcschr((LPCTSTR)str + nStart, (_TUCHAR)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)str);
}


/**
	Set the default Workspace Whiz! directory to the add-in directory.
**/
void SetDefaultWWhizDirectory()
{
	// Call the OS for the current directory.
	CString oldDirectory;
	::GetCurrentDirectory(_MAX_PATH, oldDirectory.GetBuffer(_MAX_PATH));
	oldDirectory.ReleaseBuffer();

	// Get the add-in directory.
	TCHAR modulePath[_MAX_PATH];
	modulePath[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&modulePath, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(modulePath, '\\');
	if (ptr)
	{
		ptr++;
		*ptr = 0;
	}

	::SetCurrentDirectory(modulePath);

	// Set the workspace location to the add-in directory.
	g_wwhizInterface->SetWorkspaceLocation();

	// Restore the previous directory.
	::SetCurrentDirectory(oldDirectory);
}


/////////////////////////////////////////////////////////////////////////////
// CCommands

CFindFileDialog g_wfoDialog;			// Workspace file open dialog.
CFindFileDialog g_cfoDialog;			// Compiler file open dialog.

CCommands::CCommands()
{
	m_pApplication = NULL;
	m_pApplicationEventsObj = NULL;
	m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands()
{
	ASSERT (m_pApplication != NULL);
	m_pApplication->Release();
}

HHOOK s_wndProcHook;
extern HWND g_devStudioWnd;

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT* cwp = (CWPSTRUCT*)lParam;
	if (cwp->hwnd == g_devStudioWnd  &&  cwp->message == WM_ACTIVATEAPP)
	{
		if (cwp->wParam)
		{
			if (g_wwhizInterface->GetConfig().GetScanFilesTaskSwitch())
				g_wwhizInterface->CheckActiveFilesForChanges();
		}
	}

	return ::CallNextHookEx(s_wndProcHook, nCode, wParam, lParam);
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	//  for us, which CDSAddIn did in its QueryInterface call
	//  just before it called us.
	m_pApplication = pApplication;

	// Create Application event handlers
	XApplicationEventsObj::CreateInstance(&m_pApplicationEventsObj);
	m_pApplicationEventsObj->AddRef();
	m_pApplicationEventsObj->Connect(m_pApplication);
	m_pApplicationEventsObj->m_pCommands = this;

	// Create Debugger event handler
	CComPtr<IDispatch> pDebugger;
	if (SUCCEEDED(m_pApplication->get_Debugger(&pDebugger))
		&& pDebugger != NULL)
	{
		XDebuggerEventsObj::CreateInstance(&m_pDebuggerEventsObj);
		m_pDebuggerEventsObj->AddRef();
		m_pDebuggerEventsObj->Connect(pDebugger);
		m_pDebuggerEventsObj->m_pCommands = this;
	}

	s_wndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc,
		AfxGetInstanceHandle(), AfxGetThread()->m_nThreadID);
	
	SetDefaultWWhizDirectory();

//	LoadTags();
	History::Reset();
}

void CCommands::UnadviseFromEvents()
{
	SaveTags();

	::UnhookWindowsHookEx(s_wndProcHook);

	ASSERT (m_pApplicationEventsObj != NULL);
	m_pApplicationEventsObj->Disconnect(m_pApplication);
	m_pApplicationEventsObj->Release();
	m_pApplicationEventsObj = NULL;

	if (m_pDebuggerEventsObj != NULL)
	{
		// Since we were able to connect to the Debugger events, we
		//  should be able to access the Debugger object again to
		//  unadvise from its events (thus the VERIFY_OK below--see stdafx.h).
		CComPtr<IDispatch> pDebugger;
		VERIFY_OK(m_pApplication->get_Debugger(&pDebugger));
		ASSERT (pDebugger != NULL);
		m_pDebuggerEventsObj->Disconnect(pDebugger);
		m_pDebuggerEventsObj->Release();
		m_pDebuggerEventsObj = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Event handlers

// Application events

HRESULT CCommands::XApplicationEvents::BeforeBuildStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentOpen(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString filename;
	if (theDocument)
	{
		if (g_wwhizInterface->GetCurrentFilename(filename))
		{
			g_wwhizInterface->AddChangedFile(filename);
		}
	}

	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SaveTags();
	g_wwhizInterface->RemoveAllFiles();
	g_wwhizInterface->RemoveAllTags();
	g_wwhizInterface->SetWorkspaceLocation();
	LoadTags();
	g_wwhizInterface->CheckActiveFilesForChanges();
	History::Reset();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SaveTags();
	g_wwhizInterface->RemoveAllFiles();
	g_wwhizInterface->RemoveAllTags();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	History::Reset();
	SetDefaultWWhizDirectory();

	g_wwhizInterface->RefreshFileList();
	LoadTags();;
	History::Reset();
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_wwhizInterface->RemoveAllFiles();
	g_wwhizInterface->RemoveAllTags();
	g_wwhizInterface->RefreshFileList();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	History::Reset();

	return S_OK;
}

// Debugger event

HRESULT CCommands::XDebuggerEvents::BreakpointHit(IDispatch* pBreakpoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCommands methods

STDMETHODIMP CCommands::WWFileOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	//Get the number of projects in the collection
	g_wwhizInterface->RefreshFileList();

	g_wfoDialog.m_fileList = &g_wwhizInterface->GetActiveFileList();
	g_wfoDialog.m_workspaceOpen = true;
	g_wfoDialog.DoModal();

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}


STDMETHODIMP CCommands::WWFileGlobalOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	g_wwhizInterface->RefreshGlobalFileList();

	g_cfoDialog.m_fileList = &g_wwhizInterface->GetGlobalFileList();
	g_cfoDialog.m_workspaceOpen = false;
	g_cfoDialog.DoModal();

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


static bool MatchFromFileList(int whichExt, WWhizFile& file, const WWhizFileList& fileList,
							  ObjModelHelper& objModel)
{
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	int numExts = config.FilesExtGetCount();

	// Set up the next extension.
	int nextExt = whichExt + 1;
	if (nextExt == numExts)
		nextExt = 0;

	// Find the exact file.
	int startPos = fileList.FindExact(file);
	if (startPos == -1)
		return false;					// Can't match.

	// Search until we've found an extension match and an extension AND path
	// match.
	int foundExtPos = -1;
	int foundExtPathPos = -1;
	int curPos = startPos;
	while (true)
	{
		// Now, try and go to the next one.
		curPos = fileList.FindNext(curPos, file);
		if (curPos == -1  ||  curPos == startPos)
			break;

		// Get the found file.
		WWhizFile* foundFile = (WWhizFile*)fileList.Get(curPos);

		// Cycle through the extensions, looking for a match.
		int curExt = nextExt;
		do
		{
			if (foundFile->GetExt() == config.FilesExtGet(curExt))
			{
				// See if the file exists.
				if (_access(foundFile->GetFullName(), 00) != -1)
				{
					if (foundExtPos == -1)
						foundExtPos = curPos;

					// Yep.  Check the path.
					if (file.GetPath() == foundFile->GetPath())
					{
						foundExtPathPos = curPos;
						goto done;
					}

					break;
				}
			}

			curExt++;
			if (curExt == numExts)
				curExt = 0;
		} while (curExt != whichExt);
	}

done:
	int usePos = (foundExtPathPos == -1) ? foundExtPos : foundExtPathPos;
	if (usePos != -1)
	{
		WWhizFile* foundFile = fileList.Get(usePos);
		CString fullNameWithPath = foundFile->GetFullName();

		return objModel.OpenDocument(fullNameWithPath, "Auto");
	}

	return false;
}


int HeaderFlip(LPCTSTR fullNameStr)
{
	// Exit if no filename was passed in.
	CString fullName = fullNameStr;
	fullName.MakeLower();
	if (fullName.IsEmpty())
	{
		g_wwhizInterface->GetCurrentFilename(fullName);
		if (fullName.IsEmpty())
		{
			return -1;
		}
	}

	// Update the workspace, in case it changed.
	g_wwhizInterface->RefreshFileList();

	// Create the file structure.
	WWhizFile& file = g_wwhizInterface->GetActiveFileList().Create(fullName);

	// There must be a file extension to perform the flip.
	if (file.GetExt().IsEmpty())
	{
		file.Release();
		return -1;
	}

	// Retrieve the number of extensions.
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	int numExts = config.FilesExtGetCount();

	// Figure out which extension we should start on.
	for (int whichExt = 0; whichExt < numExts; ++whichExt)
	{
		if (file.GetExt() == config.FilesExtGet(whichExt))
			break;
	}

	// Did we match ANY extension at all?  If not, then leave.  We can't properly
	// do a flip.
	if (whichExt == numExts)
	{
		file.Release();
		return -1;
	}

	// Get a pointer to the dispatch for the Documents collection.
	ObjModelHelper objModel;

	// Start searching one past the current extension.
	int nextExt = whichExt + 1;
	if (nextExt == numExts)
		nextExt = 0;
	int curExt;

	WWhizProject* project;
	
	// Try on disk within the same directory.
	curExt = nextExt;
	do
	{
		CString fullNameWithPath = file.GetPath() + file.GetTitle() + "." + 
			config.FilesExtGet(curExt);
		if (_access(fullNameWithPath, 00) != -1)
		{
			if (objModel.OpenDocument(fullNameWithPath, "Auto"))
				goto done;
		}

		curExt++;
		if (curExt == numExts)
			curExt = 0;
	} while (curExt != whichExt);

	// Try the current project.
	project = g_wwhizInterface->GetCurrentProject();
	if (project)
	{
		if (MatchFromFileList(whichExt, file, project->GetFileList(), objModel))
			goto done;
	}

	// Try the workspace.
	if (MatchFromFileList(whichExt, file, g_wwhizInterface->GetActiveFileList(), objModel))
		goto done;

	// Refresh the compiler files.
	g_wwhizInterface->RefreshGlobalFileList();

	// Match within the compiler include and source directories.
	if (MatchFromFileList(whichExt, file, g_wwhizInterface->GetGlobalFileList(), objModel))
		goto done;

	file.Release();
	return -1;

done:
	file.Release();
	return 0;
}


STDMETHODIMP CCommands::WWHeaderFlip()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the filename of the active file.
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	
	if (OnHeaderFlip("") == -1)
	{
		// Get the currently active filename.
		CString filename;
		if (g_wwhizInterface->GetCurrentFilename(filename))
		{
			// Create the file structure.
			WWhizFile& file = g_wwhizInterface->GetActiveFileList().Create(filename);

			int whichPos = g_wwhizInterface->GetActiveFileList().FindExact(file);
			if (whichPos != -1)
			{
				g_wfoDialog.m_fileList = &g_wwhizInterface->GetActiveFileList();
				g_wfoDialog.m_workspaceOpen = true;

				CFindFileDialog::FileScope saveScope = CFindFileDialog::s_fileScope;
				int saveLastPosition = g_wfoDialog.m_lastPosition;
				CString saveLastFilename = g_wfoDialog.m_lastFilename;

				CFindFileDialog::s_fileScope = CFindFileDialog::ScopeEntireWorkspace;
				g_wfoDialog.m_lastPosition = whichPos;
				g_wfoDialog.m_lastFilename = "";

				g_wfoDialog.DoModal();

				g_wfoDialog.m_lastFilename = saveLastFilename;
				g_wfoDialog.m_lastPosition = saveLastPosition;
				CFindFileDialog::s_fileScope = saveScope;
			}

			file.Release();
		}
	}

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}


STDMETHODIMP CCommands::WWFileFindPrev()
{
	// Get the currently active filename.
	CString filename;
	if (!g_wwhizInterface->GetCurrentFilename(filename))
	{
		return S_OK;
	}
		
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	g_wwhizInterface->RefreshFileList();

	// Create the file structure.
	WWhizFile& file = g_wwhizInterface->GetActiveFileList().Create(filename);

	// Find the exact file.
	const WWhizFileList& fileList = g_wwhizInterface->GetActiveFileList();
	int startPos = fileList.FindExact(file);
	if (startPos == -1)
	{
		// Try without the extension.
		file.SetExt("");
		startPos = fileList.FindExact(file);
	}

	if (startPos == -1)
	{
		file.Release();
		MessageBeep(0xFFFFFFFF);
		return S_OK;
	}

	// Now, go to the next one.
	int prevPos = fileList.FindPrevious(startPos, file);
	if (prevPos == -1)
	{
		file.Release();
		return S_OK;
	}

	file.Release();

	WWhizFile* fileFound = fileList.Get(prevPos);

	CString fullNameWithPath = fileFound->GetFullName();

	History::PreAdd();
	
	ObjModelHelper objModel;
	if (objModel.OpenDocument(fullNameWithPath))
		History::Add();

	return S_OK;
}


STDMETHODIMP CCommands::WWFileFindNext()
{
	// Get the currently active filename.
	CString filename;
	if (!g_wwhizInterface->GetCurrentFilename(filename))
	{
		return S_OK;
	}
		
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	g_wwhizInterface->RefreshFileList();

	// Create the file structure.
	WWhizFile& file = g_wwhizInterface->GetActiveFileList().Create(filename);

	// Find the exact file.
	const WWhizFileList& fileList = g_wwhizInterface->GetActiveFileList();
	int startPos = fileList.FindExact(file);
	if (startPos == -1)
	{
		// Try without the extension.
		file.SetExt("");
		startPos = fileList.FindExact(file);
	}

	if (startPos == -1)
	{
		MessageBeep(0xFFFFFFFF);
		file.Release();
		return S_OK;
	}

	// Now, go to the next one.
	int nextPos = fileList.FindNext(startPos, file);
	if (nextPos == -1)
	{
		file.Release();
		return S_OK;
	}

	file.Release();

	WWhizFile* fileFound = fileList.Get(nextPos);

	History::PreAdd();

	CString fullNameWithPath = fileFound->GetFullName();

	ObjModelHelper objModel;
	if (objModel.OpenDocument(fullNameWithPath))
		History::Add();

	return S_OK;
}


STDMETHODIMP CCommands::WWHistoryBack()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (History::Back())
		History::Goto();
	else
		MessageBeep(MB_ICONEXCLAMATION);

	return S_OK;
}


STDMETHODIMP CCommands::WWHistoryForward()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (History::Forward())
		History::Goto();
	else
		MessageBeep(MB_ICONEXCLAMATION);

	return S_OK;
}


STDMETHODIMP CCommands::WWHistoryDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CHistoryDialog dlg;
	dlg.DoModal();

	return S_OK;
}


STDMETHODIMP CCommands::WWTagFindDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Save all files.
	if (g_config.m_autoSaveAllPerTagCommand)
	{
		ObjModelHelper objModel;
		objModel.SaveAll();
	}
	
	// Simulate a modal dialog.
	CFindTagDialog dlg;
	g_devStudioCWnd.EnableWindow(FALSE);
	if (dlg.Create(CFindTagDialog::IDD))
	{
		dlg.ShowWindow(SW_SHOW);
		dlg.Refresh();
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	g_devStudioCWnd.EnableWindow(TRUE);

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


STDMETHODIMP CCommands::WWTagFindSpecialDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Save all files.
	if (g_config.m_autoSaveAllPerTagCommand)
	{
		ObjModelHelper objModel;
		objModel.SaveAll();
	}
	
	int oldTypeFilter = CFindTagDialog::s_typeFilter;
	CFindTagDialog::TagScope oldTagScope = CFindTagDialog::s_tagScope;
	CString oldFunction = CFindTagDialog::m_lastFunction;
	int oldPosition = CFindTagDialog::m_lastPosition;

	CFindTagDialog::m_lastFunction = "";
	CFindTagDialog::m_lastPosition = 0;
	CFindTagDialog::s_typeFilter = WWhizTag::FUNCTION + 1;
	CFindTagDialog::s_tagScope = CFindTagDialog::ScopeCurrentFile;

	// Simulate a modal dialog.
	CFindTagDialog dlg;
	g_devStudioCWnd.EnableWindow(FALSE);
	if (dlg.Create(CFindTagDialog::IDD))
	{
		dlg.ShowWindow(SW_SHOW);
		dlg.Refresh();
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	g_devStudioCWnd.EnableWindow(TRUE);

	CFindTagDialog::s_typeFilter = oldTypeFilter;
	CFindTagDialog::s_tagScope = oldTagScope;
	CFindTagDialog::m_lastFunction = oldFunction;
	CFindTagDialog::m_lastPosition = oldPosition;

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


STDMETHODIMP CCommands::WWTagFindAtCursorDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));
	
	// Save all files.
	if (g_config.m_autoSaveAllPerTagCommand)
	{
		ObjModelHelper objModel;
		objModel.SaveAll();
	}
	
	TextLine line;
	CString foundIdent = line.GetIdent();

	CString parentIdent;
	int semiPos = foundIdent.ReverseFind(';');
	if (semiPos != -1)
	{
		parentIdent = foundIdent.Mid(semiPos + 1);
		foundIdent = foundIdent.Left(semiPos);
	}

	CFindTagDialog::m_lastFunction = foundIdent;
	CFindTagDialog::m_lastFunction.MakeLower();

	// Simulate a modal dialog.
	g_devStudioCWnd.EnableWindow(FALSE);
	CFindTagDialog dlg;
	if (dlg.Create(CFindTagDialog::IDD))
	{
		// Hack.
		dlg.m_tagParent->SetWindowText(parentIdent);
		dlg.ShowWindow(SW_SHOW);
		dlg.Refresh();
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	g_devStudioCWnd.EnableWindow(TRUE);

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}

STDMETHODIMP CCommands::WWTagFindAtCursor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// This command only works if a file is open.
	CString currentFilename;
	if (!g_wwhizInterface->GetCurrentFilename(currentFilename))
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return S_OK;
	}

	// Update the workspace.
	{
		CWaitCursor cursor;

		// Save all files.
		if (g_config.m_autoSaveAllPerTagCommand)
		{
			ObjModelHelper objModel;
			objModel.SaveAll();
		}
		
		g_wwhizInterface->SetTagCallback(TagCallback, NULL);
		g_wwhizInterface->RefreshFileList();
		g_wwhizInterface->RefreshTagList();
	}

	TextLine line;
	CString foundText = line.GetIdent(true);
	if (!foundText.IsEmpty())
	{
		foundText = "=" + foundText + "=";
		g_wwhizInterface->MatchTag(g_wwhizInterface->GetTagList(), foundText);

		// Is there a match?
		if (g_wwhizInterface->GetTagMatchCount() == 0)
		{
			// Is there a parent identifier?
			int semiPos = foundText.ReverseFind(';');
			if (semiPos != -1)
			{
				// Remove it and try again.
				foundText = foundText.Left(semiPos + 1);// + "/" + foundText.Mid(semiPos + 1);
				g_wwhizInterface->MatchTag(g_wwhizInterface->GetTagList(), foundText);
			}
		}

		if (g_wwhizInterface->GetTagMatchCount() > 0)
		{
			if (g_wwhizInterface->GetTagMatchCount() > 1  && 
				g_config.m_useFindTagAtCursorDialog)
			{
				CFindTagDialog::m_lastFunction = foundText;

				// Simulate a modal dialog.
				g_devStudioCWnd.EnableWindow(FALSE);
				CFindTagDialog dlg;
				if (dlg.Create(CFindTagDialog::IDD))
				{
					dlg.ShowWindow(SW_SHOW);
					dlg.Refresh();
					dlg.RunModalLoop(MLF_NOIDLEMSG);
					dlg.DestroyWindow();
				}
				g_devStudioCWnd.EnableWindow(TRUE);
			}
			else
			{
				GotoTag(g_wwhizInterface->GetTagMatchHead());
			}
		}
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}

	return S_OK;
}


STDMETHODIMP CCommands::WWTagFlip()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// This command only works if a file is open.
	CString currentFilename;
	if (!g_wwhizInterface->GetCurrentFilename(currentFilename))
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return S_OK;
	}

	// Update the workspace.
	{
		CWaitCursor cursor;

		// Save all files.
		if (g_config.m_autoSaveAllPerTagCommand)
		{
			ObjModelHelper objModel;
			objModel.SaveAll();
		}
		
		g_wwhizInterface->SetTagCallback(TagCallback, NULL);
		g_wwhizInterface->RefreshFileList();
		g_wwhizInterface->RefreshTagList();
	}

	ObjModelHelper objModel;
	objModel.GetActiveDocument();

	const WWhizTag* scopeTag;

	long curX;
	long curY;
	// If this isn't a text document, then abort.
	if (!objModel.IsTextDocument())
	{
		goto Done;
	}

	// Save the last coordinate.
	objModel.GetPosition(curX, curY);

	scopeTag = g_wwhizInterface->GetTagScope(currentFilename, curY);
	if (scopeTag)
	{
		if (scopeTag->GetBuddy())
			scopeTag = scopeTag->GetBuddy();
		g_wwhizInterface->GotoTag(scopeTag);
	}

Done:
	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


STDMETHODIMP CCommands::WWTagFindNext()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (g_wwhizInterface->GetLastMatchedTag())
	{
		const WWhizTag* tag = g_wwhizInterface->GetLastMatchedTag();
		if (tag->GetMatchNext())
			tag = tag->GetMatchNext();
		else
			tag = g_wwhizInterface->GetTagMatchHead();
		GotoTag(tag);
	}

	return S_OK;
}

STDMETHODIMP CCommands::WWTagFindPrev()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (g_wwhizInterface->GetLastMatchedTag() != NULL)
	{
		const WWhizTag* tag = g_wwhizInterface->GetLastMatchedTag();
		if (tag->GetMatchPrev() != NULL)
			tag = tag->GetMatchPrev();
		else
			tag = g_wwhizInterface->GetTagMatchTail();
		GotoTag(tag);
	}

	return S_OK;
}


// tc = Tag complete.
static CString s_tcOrigIdent;
static CString s_tcLastMatchedIdent("@@@");
static CString s_tcLastTagHash("@@@");
static POSITION s_tcLastTagPos;
CListEx<const WWhizTag*, const WWhizTag*> g_tcMatchedTagList(100);
static CTime s_tcLastTagRefreshTime;

// Not really a hash, but could be...
CString GenerateTagHash(const WWhizTag* tag)
{
	CString filename = tag->GetFilename();
	filename.MakeLower();
	return CString(tag->GetType() + 'A') +
			tag->GetIdent() + tag->GetParentIdent() +
			filename + tag->GetSearchString();
}
			

static bool TagCompleteHelper(TextLine& line, bool goForward, bool writeTag, LPCTSTR identStr = NULL)
{
	// Get the identifier under the cursor to autocomplete.
	CString ident;
	if (identStr)
		ident = identStr;
	else
		ident = line.GetIdent(true, false);

	// Check for a different identifier.  If there is one, refresh the
	// workspace.
	if (ident != s_tcLastMatchedIdent)
	{
		// Update the workspace.
		{
			CWaitCursor cursor;

			// Save all files.
			if (g_config.m_autoSaveAllPerTagCommand)
			{
				ObjModelHelper objModel;
				objModel.SaveAll();
			}
			
			g_wwhizInterface->SetTagCallback(TagCallback, NULL);
			g_wwhizInterface->RefreshFileList();
			g_wwhizInterface->RefreshTagList();
		}
	}

	// Check the last update time.
	bool tagsUpdated = false;
	if (g_wwhizInterface->GetLastTagRefreshTime() != s_tcLastTagRefreshTime)
	{
		s_tcLastTagRefreshTime = g_wwhizInterface->GetLastTagRefreshTime();
		tagsUpdated = true;

		// Reset everything.
		s_tcLastMatchedIdent = "@@@";
		g_tcMatchedTagList.RemoveAll();
		s_tcLastTagPos = NULL;
	}
	
	POSITION lastTagPos = s_tcLastTagPos;
	const WWhizTag* matchedTag = NULL;
	if (ident != s_tcLastMatchedIdent)
	{
		// Reset everything.
		s_tcLastTagHash = "@@@";
		s_tcLastMatchedIdent = "@@@";
		s_tcOrigIdent.Empty();
		g_tcMatchedTagList.RemoveAll();
		s_tcLastTagPos = NULL;

		// New identifier.
		// Do we want to case sensitive match the identifier?
		bool useFileType = g_wwhizInterface->GetTagUseType(WWhizTag::FILE);
		g_wwhizInterface->SetTagUseType(WWhizTag::FILE, false);
		g_wwhizInterface->MatchTag(g_wwhizInterface->GetTagList(), ident);

		// Is there a match?
		if (g_wwhizInterface->GetTagMatchCount() == 0)
		{
			// Is there a parent identifier?
			int semiPos = ident.ReverseFind(';');
			if (semiPos != -1)
			{
				// Remove it and try again.
				ident = ident.Left(semiPos + 1) + "/" + ident.Mid(semiPos + 1);
				g_wwhizInterface->MatchTag(g_wwhizInterface->GetTagList(), ident);
			}
		}

		g_wwhizInterface->SetTagUseType(WWhizTag::FILE, useFileType);

		// Were any tags found?
		if (g_wwhizInterface->GetTagMatchCount() == 0)
		{
//			MessageBeep(MB_ICONEXCLAMATION);
		}
		else
		{
			s_tcOrigIdent = ident;

			// Found a match.  Show the first one.
			matchedTag = g_wwhizInterface->GetTagMatchHead();

			// Add all to the linked list.
			const WWhizTag* tag = g_wwhizInterface->GetTagMatchHead();
			CString lastIdent;
			while (tag)
			{
				// Skip redundant tag identifiers.
				if (tag->GetIdent() != lastIdent)
				{
					lastIdent = tag->GetIdent();
					
					g_tcMatchedTagList.AddTail(tag);
					if (tagsUpdated  &&  !s_tcLastTagPos)
					{
						CString hash = GenerateTagHash(tag);
						if (hash == s_tcLastTagHash)
							s_tcLastTagPos = g_tcMatchedTagList.GetTailPosition();
					}
				}

				// Go to the next tag.
				tag = tag->GetMatchNext();
			}

		}

		char s[200];
		sprintf(s, "Workspace Whiz! Autocomplete Tag: %d tags found.", g_wwhizInterface->GetTagMatchCount());
		PrintStatusBar(s);
	}
	else
	{
		// Old identifier.
		if (goForward)
			g_tcMatchedTagList.GetNext(s_tcLastTagPos);
		else
			g_tcMatchedTagList.GetPrev(s_tcLastTagPos);
	}

	if (!s_tcLastTagPos)
	{
		if (goForward)
			s_tcLastTagPos = g_tcMatchedTagList.GetHeadPosition();
		else
			s_tcLastTagPos = g_tcMatchedTagList.GetTailPosition();
	}

	if (writeTag  &&  s_tcLastTagPos  &&  lastTagPos != s_tcLastTagPos)
	{
		const WWhizTag* matchedTag = g_tcMatchedTagList.GetAt(s_tcLastTagPos);

		// Select the current line.
		line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, dsExtend);
		line.m_objModel.PutText(matchedTag->GetIdent());

		s_tcLastTagHash = GenerateTagHash(matchedTag);
		s_tcLastMatchedIdent = line.GetIdent(true, false);	//matchedTag->GetIdent();
	}

	return S_OK;
}


STDMETHODIMP CCommands::WWTagCompleteNext()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// This command only works if a file is open.
	CString currentFilename;
	if (!g_wwhizInterface->GetCurrentFilename(currentFilename))
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return S_OK;
	}

	TextLine line;
	TagCompleteHelper(line, true, true);

	return S_OK;
}


STDMETHODIMP CCommands::WWTagCompletePrev()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// This command only works if a file is open.
	CString currentFilename;
	if (!g_wwhizInterface->GetCurrentFilename(currentFilename))
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return S_OK;
	}

	TextLine line;
	TagCompleteHelper(line, false, true);

	return S_OK;
}


STDMETHODIMP CCommands::WWTagCompleteRestore()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// This command only works if a file is open.
	CString currentFilename;
	if (!g_wwhizInterface->GetCurrentFilename(currentFilename))
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return S_OK;
	}

	TextLine line;
	CString ident = line.GetIdent(true);

	if (!ident.IsEmpty())
	{
		if (ident == s_tcLastMatchedIdent)
		{
			// Select the current line.
			line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, dsExtend);
			line.m_objModel.PutText(s_tcOrigIdent);

			s_tcLastTagHash = "@@@";
			s_tcLastMatchedIdent = "@@@";
			s_tcOrigIdent.Empty();
			g_tcMatchedTagList.RemoveAll();
			s_tcLastTagPos = NULL;
		}
	}

	return S_OK;
}


STDMETHODIMP CCommands::WWTagCompleteDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString currentFilename;
	if (!g_wwhizInterface->GetCurrentFilename(currentFilename))
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		return S_OK;
	}

	TextLine line;
	TagCompleteHelper(line, true, false);
	TagCompleteHelper(line, false, false);

extern HWND g_mdiWnd;
	HWND activeMDIWnd = (HWND)::SendMessage(g_mdiWnd, WM_MDIGETACTIVE, 0, 0);
	CPoint caretPos = CWnd::GetCaretPos();
	ClientToScreen(activeMDIWnd, &caretPos);

	CAutoCompleteTagsDlg dlg;
	g_devStudioCWnd.EnableWindow(FALSE);
	if (dlg.Create(CAutoCompleteTagsDlg::IDD))
	{
		CString autoParent;
		CString tagName;

		int semiPos = s_tcOrigIdent.Find(';');
		if (semiPos == -1)
			tagName = s_tcOrigIdent;
		else
		{
			tagName = s_tcOrigIdent.Left(semiPos);
			autoParent = s_tcOrigIdent.Mid(semiPos);
		}
		dlg.m_autoParent = autoParent;
		dlg.SetTagName(tagName);

		dlg.SetWindowPos(NULL, caretPos.x, caretPos.y + 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		CRect devStudioWindowRect;
		g_devStudioCWnd.GetWindowRect(devStudioWindowRect);
		CRect dlgWindowRect;
		dlg.GetWindowRect(dlgWindowRect);
		dlg.ShowWindow(SW_SHOW);
		if (dlgWindowRect.bottom > devStudioWindowRect.bottom)
			dlg.SetWindowPos(NULL, caretPos.x, caretPos.y - dlgWindowRect.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	g_devStudioCWnd.EnableWindow(TRUE);

	// Write tag.
	if (dlg.m_selectedTag)
	{
//		POSITION pos = g_tcMatchedTagList.FindIndex(dlg.m_selectedIndex);
//		const WWhizTag* matchedTag = g_tcMatchedTagList.GetAt(pos);
		const WWhizTag* matchedTag = dlg.m_selectedTag;

		// Select the current line.
		line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, dsExtend);
		line.m_objModel.PutText(matchedTag->GetIdent());

		TagCompleteHelper(line, true, false, dlg.m_editText);

		s_tcLastTagHash = GenerateTagHash(matchedTag);
		s_tcLastMatchedIdent = line.GetIdent(true, false);	//matchedTag->GetIdent();
	}

	return S_OK;
}


STDMETHODIMP CCommands::WWTemplateSelect()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	CTemplateSelectDialog::DoDialog();

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


STDMETHODIMP CCommands::WWTemplateComplete()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	TextLine line;

	// Get the identifier under the cursor to autocomplete.
	CString ident = line.GetIdent(true, false);
	if (ident.IsEmpty())
		return S_OK;
   
	// Refresh the template files.
	g_wwhizTemplateManager->Refresh();

	int templateGroupCount = g_wwhizTemplateManager->GetCount();
	for (int i = 0; i < templateGroupCount; i++)
	{
		WWhizTemplateGroup* templateGroup = g_wwhizTemplateManager->GetTemplateGroup(i);
		int templateCount = templateGroup->GetCount();
		for (int j = 0; j < templateCount; j++)
		{
			WWhizTemplate* theTemplate = templateGroup->Get(j);
			const CString& key = theTemplate->GetKey();
			if (key.IsEmpty())
				continue;

			if (ident == key)
			{
				VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

				theTemplate->ResetDictionaryDefaults();

				// If there are dialog wizard pages, then pop up the wizard dialog.
				if (theTemplate->GetPageCount() > 0)
				{
					CTemplateWizardDialog dlg(*theTemplate);
					if (dlg.DoModal() == IDCANCEL)
					{
						VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
						return S_OK;
					}
				}

				// Select the current line.
				line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, dsExtend);
				line.m_objModel.PutText("");

				// Finally, put it into the text document.
				WorkspaceCommands::PutTemplate(*theTemplate);

				VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

				return S_OK;
			}
		}
	}
	
	return S_OK;
}

STDMETHODIMP CCommands::WWPasteFix()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (OpenClipboard(NULL))
	{
		HANDLE hData = ::GetClipboardData(CF_TEXT);
		if (hData)
		{
			char* data = (char*)::GlobalLock(hData);

			// Get the active document.  If there is one, set up the m_helper->m_pTextSelection
			// pointer.
			ObjModelHelper objModel;
			objModel.GetActiveDocument();
			objModel.PutText(data);

			::GlobalUnlock(hData);
		}
		CloseClipboard();
	}

	return S_OK;
}

