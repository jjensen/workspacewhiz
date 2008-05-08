///////////////////////////////////////////////////////////////////////////////
// $Workfile: DSAddIn.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/DSAddIn.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 44   $ $Author: Jjensen $
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
#include "DSAddIn.h"
#include "Commands.h"
#include "AICLoader.h"
#include "WWhizInterface2Loader.h"
#include "WWhizRegLoader.h"
#include "WWhizTemplateManagerLoader.h"
#include "TemplateWizardDialog.h"
#include "ToolbarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WWhizInterface* g_wwhizInterface;
WWhizReg* g_wwhizReg;
WWhizTemplateManager* g_wwhizTemplateManager;

extern int AddinCallback200(LPCTSTR pCmd);

static CommandInfo s_commandList[] =
{
	{ "WWFileOpen",						"Ctrl+O",				true,  false },
	{ "WWFileGlobalOpen",				"Ctrl+Shift+O",			true,  false },
	{ "WWHeaderFlip",					"Ctrl+H",				true,  false },
	{ "WWFileFindPrev",					"",						true,  false },
	{ "WWFileFindNext",					"",						true,  false },
	{ "WWHistoryBack",					"Alt+Left Arrow",		true,  false },
	{ "WWHistoryForward",				"Alt+Right Arrow",		true,  false },
	{ "WWHistoryDlg",					"",						true,  false },
	{ "WWTagFindDlg",					"Ctrl+D",				true,  false },
	{ "WWTagFindSpecialDlg",			"Ctrl+Shift+D",			true,  false },
	{ "WWTagFlip",						"Ctrl+Shift+Space",		true,  false },
	{ "WWTagFindAtCursor",				"Ctrl+Space",			true,  false },
	{ "WWTagFindAtCursorDlg",			"",						true,  false },
	{ "WWTagFindPrev",					"Alt+Shift+Left Arrow",	true,  false },
	{ "WWTagFindNext",					"Alt+Shift+Right Arrow",	true,  false },
	{ "WWTagCompletePrev",				"Ctrl+Shift+Enter",		true,  false },
	{ "WWTagCompleteNext",				"Ctrl+Enter",			true,  false },
	{ "WWTagCompleteRestore",			"Ctrl+Alt+Enter",		true,  false },
	{ "WWTagCompleteDlg",				"Ctrl+Alt+Shift+Enter",	true,  false },
	{ "WWTemplateSelect",				"Ctrl+Shift+T",			true,  false },
	{ "WWTemplateComplete",				"Ctrl+T",				true,  false },
	{ "WWPasteFix",						"Ctrl+V",				false, true },
};

int GetCommandCount()
{
	return _countof(s_commandList);
}

const CommandInfo* GetCommandList()
{
	return s_commandList;
}

IApplication* g_pApplication;
HWND g_devStudioWnd;
HWND g_mdiWnd;
static HINSTANCE s_aicModule;

typedef HADDIN (*AICREGISTERADDIN)(LPCTSTR pName, int iVerMaj,
			int iVerMin, int iVerExtra, AddinCmdHandler_t *pCmdFn);
typedef bool (*AICUNREGISTERADDIN)(HADDIN hAddIn);
static HADDIN s_aicAddinHandle;
		
static long s_dwCookie;

static bool QueryValueBinary(CRegKey& key, LPCTSTR lpszValueName, void* szValue, DWORD& pdwCount)
{
	DWORD dwType = NULL;
	LONG lRes = RegQueryValueEx(key.m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
		(LPBYTE)szValue, &pdwCount);

	if (dwType != REG_BINARY)
		return false;
	if (lRes != ERROR_SUCCESS  &&  lRes != ERROR_MORE_DATA)
		return false;

	return true;
}

void AddToolbar()
{
	if (g_config.m_dsFirstTime)
	{
		bool foundWWhizToolbar = false;

		// Get DevStudio version.
		CComBSTR bstrVersion;
		g_pApplication->get_Version(&bstrVersion);
		CString strVersion = bstrVersion;

		// Open the proper DevStudio key.
		CString whichEnvironment;
		CString devStudioKeyName;
		HMODULE hModule = GetModuleHandle("MSDEV.EXE");
		if (hModule)
		{
			devStudioKeyName = "Software\\Microsoft\\Devstudio\\" + strVersion + "\\";
			whichEnvironment = "DS";
		}
		else
		{
			hModule = GetModuleHandle("EVC.EXE");
			if (hModule)
			{
				devStudioKeyName = "Software\\Microsoft\\CEStudio\\" + strVersion + "\\evc\\";
				whichEnvironment = "EVC";
			}
		}

		CRegKey devKey;
		if (devKey.Open(HKEY_CURRENT_USER, devStudioKeyName + "Layout") == ERROR_SUCCESS)
		{
			// Okay, we have the key open.  Walk all keys...
			DWORD dwSize = 256;
			TCHAR szName[256];
			DWORD dwIndex = 0;
			LONG result;
			DWORD type;

			while (true)
			{
				result = RegEnumValue(devKey.m_hKey, dwIndex, szName, &dwSize, NULL,
					&type, NULL, NULL);
				if (result != ERROR_SUCCESS)
					break;
				dwSize = 256;

				CString name = szName;
				if (name.Left(6) == "Window")
				{
					// Read the key.
					DWORD count = 0;
					if (QueryValueBinary(devKey, name, NULL, count))
					{
						AutoBasic<BYTE> buf(new BYTE[count + 1]);
						buf[count] = 0;
						if (QueryValueBinary(devKey, name, buf, count))
						{
							// Okay, we have the binary data.  Do a scan for
							// Workspace Whiz! commands.
							BYTE* ptr = buf;
							BYTE* ptrEnd = ptr + count;
							while (ptr != ptrEnd)
							{
								// "WWFileOpen"
								if (ptr[0] == 'W'  &&  ptr[1] == 'W')
								{
									if (strcmp((LPCTSTR)ptr, "WWFileOpen") == 0)
									{
										foundWWhizToolbar = true;
										goto DoneSearch;
									}
								}
								ptr++;
							}
						}
					}
				}

				dwIndex++;
			}
		}

DoneSearch:
		const DWORD thisToolbarVersion = 0x02000002;

		// Get the toolbar version.
		DWORD curToolbarVersion = AfxGetApp()->GetProfileInt("Config", "Toolbar" + whichEnvironment + strVersion, 0);
		if (curToolbarVersion != thisToolbarVersion)
		{
			foundWWhizToolbar = false;
			AfxGetApp()->WriteProfileInt("Config", "Toolbar" + whichEnvironment + strVersion, thisToolbarVersion);;
		}
		
		// Does the toolbar possibly exist?
		bool buildToolbar = true;
		if (foundWWhizToolbar)
		{
			if (AfxMessageBox("A toolbar (potentially turned off) was found which appears to contain "
					"Workspace Whiz! commands.\n\nPress Yes to create a new Workspace "
					"Whiz! toolbar.  Press No to keep the existing toolbar.", MB_YESNO) == IDNO)
			{
				buildToolbar = false;
			}
		}

		if (buildToolbar)
		{
			for (int curCommand = 0; curCommand < GetCommandCount(); curCommand++)
			{
				// Add toolbar buttons only if this is the first time the add-in
				//  is being loaded.  Toolbar buttons are automatically remembered
				//  by Developer Studio from session to session, so we should only
				//  add the toolbar buttons once.
				if (s_commandList[curCommand].m_hasIcon)
				{
					VERIFY_OK(g_pApplication->
						AddCommandBarButton(
							dsGlyph, CComBSTR(s_commandList[curCommand].m_name), s_dwCookie));
				}
			}

			AfxMessageBox("Please exit Developer Studio and reload to save toolbar changes.");
		}
		g_config.m_dsFirstTime = false;
		g_config.SaveRegistry_FirstTime();
	}
}

static CToolbarWnd s_hiddenWnd;
CWnd g_devStudioCWnd;
HWND g_statusBarWnd;
CWnd g_statusBar;
static WWhizTemplateDialogImpl s_templateDialogImpl;

void PrintStatusBar(LPCTSTR message)
{
	g_statusBar.SetWindowText(message);
	g_statusBar.UpdateWindow();
}

// This is called when the user first loads the add-in, and on start-up
//  of each subsequent Developer Studio session
STDMETHODIMP CDSAddIn::OnConnection(IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HMODULE hModule = GetModuleHandle("MSDEV.EXE");
	hModule = GetModuleHandle("EVC.EXE");

	g_pApplication = pApp;
	
	// Find the Developer Studio window.  (borrowed from Oz Solomonovich)
	HWND hDesktopWnd = ::GetDesktopWindow();

    pApp->put_Active(VARIANT_FALSE);
    pApp->put_Active(VARIANT_TRUE);
    HWND hWnd = ::GetActiveWindow();
    while (hWnd  &&  hWnd != hDesktopWnd)
    {
        g_devStudioWnd = hWnd;
        hWnd = ::GetParent(hWnd);
    }
	g_devStudioCWnd.Attach(g_devStudioWnd);

	// Figure out the global DevStudio window "type".
	char classNameBuffer[_MAX_PATH];
	::GetClassName(g_devStudioWnd, classNameBuffer, _MAX_PATH);

	// Find the third colon.
	char* className;
	char* endColonPtr;
	className = strchr(classNameBuffer, ':');
	className = strchr(className + 1, ':');
	className = strchr(className + 1, ':');
	className++;
	endColonPtr = strchr(className + 1, ':');
	*endColonPtr = 0;

	// Find the status bar.
	char statusBarName[_MAX_PATH];
	strcpy(statusBarName, "Afx:400000:0:");
	strcat(statusBarName, className);
	strcat(statusBarName, ":10:0");
	g_statusBarWnd = ::FindWindowEx(g_devStudioWnd, NULL, statusBarName, NULL);
	g_statusBar.Attach(g_statusBarWnd);

	// Find the MDI client window
	g_mdiWnd = ::FindWindowEx(g_devStudioWnd, NULL, "MDIClient", NULL);

	// Store info passed to us
	IApplication* pApplication = NULL;
	if (FAILED(pApp->QueryInterface(IID_IApplication, (void**) &pApplication))
		|| pApplication == NULL)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	m_dwCookie = dwCookie;
	s_dwCookie = dwCookie;

	g_wwhizInterface = WWhizInterface2Create(AfxGetInstanceHandle(), pApplication);
	g_wwhizReg = WWhizRegCreate(pApplication, g_wwhizInterface);
	g_wwhizInterface->SetWWhizReg(g_wwhizReg);
	g_wwhizTemplateManager = WWhizTemplateManagerCreate(pApplication, &s_templateDialogImpl);

	// Check the first time.
	g_config.LoadRegistry();

    if (bFirstTime == VARIANT_TRUE)
	{
		g_config.m_dsFirstTime = false;
		g_config.SaveRegistry_FirstTime();
	}

	// Create command dispatch, send info back to DevStudio
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();

	// The QueryInterface above AddRef'd the Application object.  It will
	//  be Release'd in CCommand's destructor.
	m_pCommands->SetApplicationObject(pApplication);

	// (see stdafx.h for the definition of VERIFY_OK)

	VERIFY_OK(pApplication->SetAddInInfo((long) AfxGetInstanceHandle(),
		(LPDISPATCH) m_pCommands, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, m_dwCookie));

	*OnConnection = VARIANT_TRUE;
	for (int curCommand = 0; curCommand < GetCommandCount(); curCommand++)
	{
		CString desc;
		desc.LoadString(IDS_CI_WWFILEOPEN + curCommand);

		VARIANT_BOOL bRet;
		VERIFY_OK(pApplication->AddCommand(
			CComBSTR(s_commandList[curCommand].m_name + desc),
			CComBSTR(s_commandList[curCommand].m_name),
			curCommand,
			m_dwCookie,
			&bRet));
		if (bRet == VARIANT_FALSE)
		{
			// AddCommand failed because a command with this name already
			//  exists.  You may try adding your command under a different name.
			//  Or, you can fail to load as we will do here.
			*OnConnection = VARIANT_FALSE;
			return S_OK;
		}

		// Add toolbar buttons only if this is the first time the add-in
		//  is being loaded.  Toolbar buttons are automatically remembered
		//  by Developer Studio from session to session, so we should only
		//  add the toolbar buttons once.
		if (bFirstTime == VARIANT_TRUE)
		{
			if (s_commandList[curCommand].m_hasIcon)
			{
				VERIFY_OK(pApplication->
					AddCommandBarButton(
						dsGlyph, CComBSTR(s_commandList[curCommand].m_name), m_dwCookie));
			}
		}
	}

	// Load the AddInComm module, if available.
	aiclUseDebugLibrary(FALSE);
	aiclLoadAICLibrary(AfxGetInstanceHandle());
	s_aicAddinHandle = AICRegisterAddIn("WorkspaceWhiz", 2, 12, 1, AddinCallback200);

	// Load the template stuff from the registry.
	g_wwhizTemplateManager->LoadRegistry();

	s_hiddenWnd.Create(NULL, "", 0, CRect(0, 0, 0, 0), &g_devStudioCWnd, 0, NULL);
	if (g_config.m_dsFirstTime)
	{
		s_hiddenWnd.PostMessage(WM_DOTOOLBAR, 0x1234, 0x56789012);
	}
	
	return S_OK;
}

// This is called on shut-down, and also when the user unloads the add-in
STDMETHODIMP CDSAddIn::OnDisconnection(VARIANT_BOOL bLastTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pCommands->UnadviseFromEvents();
	m_pCommands->Release();
	m_pCommands = NULL;

	if (s_aicAddinHandle)
		AICUnregisterAddIn(s_aicAddinHandle);

	g_wwhizInterface->RemoveAllFiles();
	g_wwhizInterface->RemoveAllTags();
	g_wwhizTemplateManager->SaveRegistry();		// Save out the registry on shutdown.

	WWhizTemplateManagerDestroy();
	WWhizRegDestroy();
	WWhizInterface2Destroy();

	g_statusBar.Detach();
	s_hiddenWnd.DestroyWindow();
	g_devStudioCWnd.Detach();

	return S_OK;
}
