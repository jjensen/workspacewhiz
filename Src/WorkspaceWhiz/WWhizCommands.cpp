#include "resource.h"
#include "WWhizCommands.h"
#include "WWhizInterfaceLoader.h"
#include "WWhizRegLoader.h"
#include "WWhizTemplateManagerLoader.h"
#include "WWhizInterface3.h"
#include "FindFileDialog.h"
#include "FindTagDialog.h"
#include "HistoryDialog.h"
#include "AutoCompleteTagsDlg.h"
#include "TemplateSelectDialog.h"
#include "TemplateWizardDialog.h"
#include "PreferencesDialog.h"
#include "History.h"
#include "TextLine.h"
#include <io.h>

WWhizInterface* g_wwhizInterface;
WWhizReg* g_wwhizReg;
WWhizTemplateManager* g_wwhizTemplateManager;
Config g_config;

static WWhizCommands::CommandInfo s_commandList[] =
{
	{ L"WWOptions",						"",						true,  false, IDB_CI_WWOPTIONS				  },
	{ L"WWFileOpen",					"Ctrl+O",				true,  false, IDB_CI_WWFILEOPEN               },
	{ L"WWFileGlobalOpen",				"Ctrl+Shift+O",			true,  false, IDB_CI_WWFILEGLOBALOPEN         },
	{ L"WWHeaderFlip",					"Ctrl+L",				true,  false, IDB_CI_WWHEADERFLIP             },
	{ L"WWFileFindPrev",				"",						true,  false, IDB_CI_WWFILEFINDPREV           },
	{ L"WWFileFindNext",				"",						true,  false, IDB_CI_WWFILEFINDNEXT           },
	{ L"WWHistoryBack",					"Alt+Left Arrow",		true,  false, IDB_CI_WWHISTORYBACK            },
	{ L"WWHistoryForward",				"Alt+Right Arrow",		true,  false, IDB_CI_WWHISTORYFORWARD         },
	{ L"WWHistoryDlg",					"",						true,  false, IDB_CI_WWHISTORYDLG             },
	{ L"WWTagFindDlg",					"Ctrl+D",				true,  false, IDB_CI_WWTAGFINDDLG             },
	{ L"WWTagFindSpecialDlg",			"Ctrl+Shift+D",			true,  false, IDB_CI_WWTAGFINDSPECIALDLG      },
	{ L"WWTagFlip",						"Ctrl+Shift+Space",		true,  false, IDB_CI_WWTAGFLIP                },
	{ L"WWTagFindAtCursor",				"Ctrl+Space",			true,  false, IDB_CI_WWTAGFINDATCURSOR        },
	{ L"WWTagFindAtCursorDlg",			"",						true,  false, IDB_CI_WWTAGFINDATCURSORDLG     },
	{ L"WWTagFindPrev",					"Alt+Shift+Left Arrow",	true,  false, IDB_CI_WWTAGFINDPREV            },
	{ L"WWTagFindNext",					"Alt+Shift+Right Arrow",true,  false, IDB_CI_WWTAGFINDNEXT            },
	{ L"WWTagCompletePrev",				"Ctrl+Shift+Enter",		true,  false, IDB_CI_WWTAGCOMPLETEPREV        },
	{ L"WWTagCompleteNext",				"Ctrl+Enter",			true,  false, IDB_CI_WWTAGCOMPLETENEXT        },
	{ L"WWTagCompleteRestore",			"Ctrl+Alt+Enter",		true,  false, IDB_CI_WWTAGCOMPLETERESTORE     },
	{ L"WWTagCompleteDlg",				"Ctrl+Alt+Shift+Enter",	true,  false, IDB_CI_WWTAGCOMPLETEDLG         },
	{ L"WWTemplateSelect",				"Ctrl+Shift+T",			true,  false, IDB_CI_WWTEMPLATESELECT         },
	{ L"WWTemplateComplete",			"Ctrl+T",				true,  false, IDB_CI_WWTEMPLATECOMPLETE       },
};


int WWhizCommands::GetCommandCount()
{
	return _countof(s_commandList);
}


const WWhizCommands::CommandInfo* WWhizCommands::GetCommandList()
{
	return s_commandList;
}


const WWhizCommands::CommandInfo* WWhizCommands::GetCommand(int index)
{
	return &s_commandList[index];
}


extern CWnd g_devStudioCWnd;

CFindFileDialog g_wfoDialog;			// Workspace file open dialog.
CFindFileDialog g_cfoDialog;			// Compiler file open dialog.


/**
	Set the default Workspace Whiz directory to the add-in directory.
**/
void WWhizCommands::SetDefaultWWhizDirectory()
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


void WWhizCommands::LoadTags()
{
	g_wwhizInterface->SetTagCallback(TagCallback, NULL);
	g_wwhizInterface->LoadTags();
}


void WWhizCommands::SaveTags()
{
	g_wwhizInterface->SetTagCallback(TagCallback, NULL);
	g_wwhizInterface->SaveTags();
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
	bool searchSameDir = true;			// Search for the same directory first.
	while (true)
	{
		// Now, try and go to the next one.
		curPos = fileList.FindNext(curPos, file);
		if (curPos == -1  ||  curPos == startPos)
		{
			if (searchSameDir)
			{
				// Restart, but don't search for same directory.
				searchSameDir = false;
				curPos = startPos;
				continue;
			}
			break;
		}

		// Get the found file.
		WWhizFile* foundFile = (WWhizFile*)fileList.Get(curPos);

		// If we're searching for the same directory, then compare the paths.
		if (searchSameDir  &&  foundFile->GetPath().CompareNoCase(file.GetPath()) != 0)
			continue;

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

		return objModel.OpenDocument(fullNameWithPath, NULL);
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
	
	// Update the workspace, in case it changed.
	g_wwhizInterface->RefreshFileList();
	
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


int WWhizCommands::OnHeaderFlip(LPCTSTR pParams)
{
	bool didAdd = History::PreAdd();
	int ret = HeaderFlip(pParams);
	if (ret == 0)
		History::Add();
	else if (didAdd)
		History::UndoAdd();
	return ret;
}


STDMETHODIMP WWhizCommands::WWOptions()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CPreferencesDialog dlg;
	dlg.DoModal();

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWFileOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//Get the number of projects in the collection
//	g_wwhizInterface->RefreshFileList();

//	g_wfoDialog.m_fileList = &g_wwhizInterface->GetActiveFileList();
//	g_wfoDialog.m_workspaceOpen = true;
	CFindFileDialog::s_fileScope = CFindFileDialog::ScopeEntireWorkspace;

	// Simulate a modal dialog.
	EnableMainWindow(FALSE);
	if (g_wfoDialog.Create(CFindFileDialog::IDD))
	{
		g_wfoDialog.ShowWindow(SW_SHOW);
		g_wfoDialog.RunModalLoop(MLF_NOIDLEMSG);
		g_wfoDialog.DestroyWindow();
	}
	EnableMainWindow(TRUE);

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWFileGlobalOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

//	g_wwhizInterface->RefreshGlobalFileList();

//	g_cfoDialog.m_fileList = &g_wwhizInterface->GetGlobalFileList();
//	g_cfoDialog.m_workspaceOpen = false;
	CFindFileDialog::s_fileScope = CFindFileDialog::ScopeGlobal;

	// Simulate a modal dialog.
	EnableMainWindow(FALSE);
	if (g_cfoDialog.Create(CFindFileDialog::IDD))
	{
		g_cfoDialog.ShowWindow(SW_SHOW);
		g_cfoDialog.RunModalLoop(MLF_NOIDLEMSG);
		g_cfoDialog.DestroyWindow();
	}
	EnableMainWindow(TRUE);

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWHeaderFlip()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the filename of the active file.
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
//				g_wfoDialog.m_fileList = &g_wwhizInterface->GetActiveFileList();
//				g_wfoDialog.m_workspaceOpen = true;

				CFindFileDialog::FileScope saveScope = CFindFileDialog::s_fileScope;
				int saveLastPosition = g_wfoDialog.m_lastPosition;
				CString saveLastFilename = g_wfoDialog.m_lastFilename;

				CFindFileDialog::s_fileScope = CFindFileDialog::ScopeEntireWorkspace;
				g_wfoDialog.m_lastPosition = whichPos;
				g_wfoDialog.m_lastFilename = "";

				// Simulate a modal dialog.
				EnableMainWindow(FALSE);
				if (g_wfoDialog.Create(CFindFileDialog::IDD))
				{
					g_wfoDialog.ShowWindow(SW_SHOW);
					g_wfoDialog.RunModalLoop(MLF_NOIDLEMSG);
					g_wfoDialog.DestroyWindow();
				}
				EnableMainWindow(TRUE);

				g_wfoDialog.m_lastFilename = saveLastFilename;
				g_wfoDialog.m_lastPosition = saveLastPosition;
				CFindFileDialog::s_fileScope = saveScope;
			}

			file.Release();
		}
	}

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWFileFindPrev()
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


STDMETHODIMP WWhizCommands::WWFileFindNext()
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


STDMETHODIMP WWhizCommands::WWHistoryBack()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (History::Back())
		History::Goto();
	else
		MessageBeep(MB_ICONEXCLAMATION);

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWHistoryForward()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (History::Forward())
		History::Goto();
	else
		MessageBeep(MB_ICONEXCLAMATION);

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWHistoryDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CHistoryDialog dlg;

	// Simulate a modal dialog.
	EnableMainWindow(FALSE);
	if (dlg.Create(CHistoryDialog::IDD))
	{
		dlg.ShowWindow(SW_SHOW);
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	EnableMainWindow(TRUE);

	return S_OK;
}




STDMETHODIMP WWhizCommands::WWTagFindDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Save all files.
	if (g_config.m_autoSaveAllPerTagCommand)
	{
		ObjModelHelper objModel;
		objModel.SaveAll();
	}
	
	// Simulate a modal dialog.
	CFindTagDialog dlg;
	EnableMainWindow(FALSE);
	if (dlg.Create(CFindTagDialog::IDD))
	{
		dlg.ShowWindow(SW_SHOW);
		dlg.Refresh();
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	EnableMainWindow(TRUE);

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWTagFindSpecialDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

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
	EnableMainWindow(FALSE);
	if (dlg.Create(CFindTagDialog::IDD))
	{
		dlg.ShowWindow(SW_SHOW);
		dlg.Refresh();
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	EnableMainWindow(TRUE);

	CFindTagDialog::s_typeFilter = oldTypeFilter;
	CFindTagDialog::s_tagScope = oldTagScope;
	CFindTagDialog::m_lastFunction = oldFunction;
	CFindTagDialog::m_lastPosition = oldPosition;

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWTagFindAtCursorDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

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
	EnableMainWindow(FALSE);
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
	EnableMainWindow(TRUE);

	return S_OK;
}

extern void TagCallback(const WWhizInterface::TagCallbackInfo& info);

STDMETHODIMP WWhizCommands::WWTagFindAtCursor()
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
				EnableMainWindow(FALSE);
				CFindTagDialog dlg;
				if (dlg.Create(CFindTagDialog::IDD))
				{
					dlg.ShowWindow(SW_SHOW);
					dlg.Refresh();
					dlg.RunModalLoop(MLF_NOIDLEMSG);
					dlg.DestroyWindow();
				}
				EnableMainWindow(TRUE);
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


STDMETHODIMP WWhizCommands::WWTagFlip()
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
	return S_OK;
}


STDMETHODIMP WWhizCommands::WWTagFindNext()
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

STDMETHODIMP WWhizCommands::WWTagFindPrev()
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
WList<const WWhizTag*> g_tcMatchedTagList(100);
static CTime s_tcLastTagRefreshTime;

// Not really a hash, but could be...
CString GenerateTagHash(const WWhizTag* tag)
{
	CString filename = tag->GetFilename();
	filename.MakeLower();
	return CString((char)(tag->GetType() + 'A')) +
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
		sprintf(s, "Workspace Whiz Autocomplete Tag: %d tags found.", g_wwhizInterface->GetTagMatchCount());
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
		line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, true);
		line.m_objModel.Delete();
		line.m_objModel.PutText(matchedTag->GetIdent());

		s_tcLastTagHash = GenerateTagHash(matchedTag);
		s_tcLastMatchedIdent = line.GetIdent(true, false);	//matchedTag->GetIdent();
	}

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWTagCompleteNext()
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


STDMETHODIMP WWhizCommands::WWTagCompletePrev()
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


STDMETHODIMP WWhizCommands::WWTagCompleteRestore()
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
	CString ident = line.GetIdent(true, false);

	if (!ident.IsEmpty())
	{
		if (ident == s_tcLastMatchedIdent)
		{
			// Select the current line.
			line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, true);
			line.m_objModel.Delete();
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


STDMETHODIMP WWhizCommands::WWTagCompleteDlg()
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

#ifdef WWHIZ_VSNET
	HWND g_devStudioWnd;
	CComPtr<EnvDTE::Window> pMainWindow;
	ObjModelHelper::GetInterface()->get_MainWindow(&pMainWindow);
	if (!pMainWindow)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		//????
		return S_OK;
	}

	pMainWindow->get_HWnd((long*)&g_devStudioWnd);

	// Find the MDI client window
	HWND g_mdiWnd = ::FindWindowEx(g_devStudioWnd, NULL, "MDIClient", NULL);
#endif WWHIZ_VSNET

#ifdef WWHIZ_VC6
	extern HWND g_mdiWnd;
	HWND g_devStudioWnd = g_devStudioCWnd.GetSafeHwnd();
#endif WWHIZ_VC6

	HWND activeMDIWnd = (HWND)::SendMessage(g_mdiWnd, WM_MDIGETACTIVE, 0, 0);
	CPoint caretPos = CWnd::GetCaretPos();
	ClientToScreen(activeMDIWnd, &caretPos);

	CAutoCompleteTagsDlg dlg;
	EnableMainWindow(FALSE);
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
		GetWindowRect(g_devStudioWnd, devStudioWindowRect);
		CRect dlgWindowRect;
		dlg.GetWindowRect(dlgWindowRect);
		dlg.ShowWindow(SW_SHOW);
		if (dlgWindowRect.bottom > devStudioWindowRect.bottom)
			dlg.SetWindowPos(NULL, caretPos.x, caretPos.y - dlgWindowRect.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		dlg.RunModalLoop(MLF_NOIDLEMSG);
		dlg.DestroyWindow();
	}
	EnableMainWindow(TRUE);
//	EnableWindow(g_devStudioWnd, TRUE);

	// Write tag.
	if (dlg.m_selectedTag)
	{
//		POSITION pos = g_tcMatchedTagList.FindIndex(dlg.m_selectedIndex);
//		const WWhizTag* matchedTag = g_tcMatchedTagList.GetAt(pos);
		const WWhizTag* matchedTag = dlg.m_selectedTag;

		// Select the current line.
		line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, true);
		line.m_objModel.Delete();
		line.m_objModel.PutText(matchedTag->GetIdent());

		TagCompleteHelper(line, true, false, dlg.m_editText);

		s_tcLastTagHash = GenerateTagHash(matchedTag);
		s_tcLastMatchedIdent = line.GetIdent(true, false);	//matchedTag->GetIdent();
	}

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWTemplateSelect()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CTemplateSelectDialog::DoDialog();

	return S_OK;
}


STDMETHODIMP WWhizCommands::WWTemplateComplete()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

#ifdef WWHIZ_VSNET
	CComPtr<EnvDTE::UndoContext> undoContext;
	g_pDTE->get_UndoContext(&undoContext);

	VARIANT_BOOL undoIsOpen;
	undoContext->get_IsOpen(&undoIsOpen);

	if (undoIsOpen == VARIANT_FALSE)
	{
		undoContext->Open(CComBSTR("Code Template"), VARIANT_FALSE);
	}
#endif WWHIZ_VSNET

	TextLine line;

	// Get the identifier under the cursor to autocomplete.
	CString ident = line.GetIdent(true, false);
	if (ident.IsEmpty())
	{
#ifdef WWHIZ_VSNET
		if (undoIsOpen == VARIANT_FALSE)
		{
			undoContext->Close();
		}
#endif WWHIZ_VSNET

		return S_OK;
	}

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
				theTemplate->ResetDictionaryDefaults();

				// If there are dialog wizard pages, then pop up the wizard dialog.
				if (theTemplate->GetPageCount() > 0)
				{
					CTemplateWizardDialog dlg(*theTemplate);

					if (dlg.DoModal() == IDCANCEL)
					{
#ifdef WWHIZ_VSNET
						if (undoIsOpen == VARIANT_FALSE)
						{
							undoContext->Close();
						}
#endif WWHIZ_VSNET

						return S_OK;
					}
				}

				// Select the current line.
				line.m_objModel.MoveTo(line.m_curY, line.m_startX + 1, true);
				line.m_objModel.Delete();

				// Finally, put it into the text document.
				PutTemplate(*theTemplate);

#ifdef WWHIZ_VSNET
				if (undoIsOpen == VARIANT_FALSE)
				{
					undoContext->Close();
				}
#endif WWHIZ_VSNET

				return S_OK;
			}
		}
	}

#ifdef WWHIZ_VSNET
	if (undoIsOpen == VARIANT_FALSE)
	{
		undoContext->Close();
	}
#endif WWHIZ_VSNET

	return S_OK;
}


void WWhizCommands::OnWorkspaceOpen()
{
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
}


void WWhizCommands::OnWorkspaceClose()
{
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
}


void WWhizCommands::OnWorkspaceNew()
{
	g_wwhizInterface->RemoveAllFiles();
	g_wwhizInterface->RemoveAllTags();
	g_wwhizInterface->RefreshFileList();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	History::Reset();
}


void WWhizCommands::OnDocumentSave()
{
	CString filename;
	if (g_wwhizInterface->GetCurrentFilename(filename))
	{
		g_wwhizInterface->AddChangedFile(filename);
	}
}


void WWhizCommands::EnableMainWindow(BOOL enable)
{
#ifdef WWHIZ_VC6
	g_devStudioCWnd.EnableWindow(enable);
#endif WWHIZ_VC6
}


int WWhizCommands::PutTemplate(WWhizTemplate& code)
{
	ObjModelHelper objModel;
	if (objModel.GetActiveDocument())
	{
		code.SetEntry("sel", objModel.GetText());
	}
	else
	{
		code.SetEntry("sel", "");
	}

#ifdef WWHIZ_VSNET
	CComPtr<EnvDTE::UndoContext> undoContext;
	g_pDTE->get_UndoContext(&undoContext);

	VARIANT_BOOL undoIsOpen;
	undoContext->get_IsOpen(&undoIsOpen);

	if (undoIsOpen == VARIANT_FALSE)
	{
		undoContext->Open(CComBSTR("Code Template"), VARIANT_FALSE);
	}
#endif WWHIZ_VSNET

	code.Parse();

#ifdef WWHIZ_VSNET
	if (undoIsOpen == VARIANT_FALSE)
	{
		undoContext->Close();
	}
#endif WWHIZ_VSNET

	return 0;
}


void WWhizCommands::OnShutdown()
{
	if ( g_wwhizInterface )
	{
		g_wwhizInterface->RemoveAllFiles();
		g_wwhizInterface->RemoveAllTags();
		g_wwhizTemplateManager->SaveRegistry();		// Save out the registry on shutdown.

		WWhizTemplateManagerDestroy();
		WWhizRegDestroy();
		WWhizInterfaceDestroy();

		g_wwhizTemplateManager = NULL;
		g_wwhizReg = NULL;
		g_wwhizInterface = NULL;
	}
}
