#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CommandInfo commandList[] =
{
	CommandInfo(
		"WUFileOpen",
		"\nFile Open\nIncremental workspace-wide file open\n"
		"Incremental file open", true),

	CommandInfo(
		"WUHeaderFlip",
		"\nHeader Flip\nWorkspace-wide .cpp/.h flipper\n"
		"Header flip", true ),

	CommandInfo(
		"WUFindTagDlg",
		"\nFind Tag Dialog\nFind a tag within the workspace\n"
		"Find tag dialog", true),

	CommandInfo(
		"WUCompilerFileOpen",
		"\nCompiler File Open\nIncremental compiler-wide file open\n"
		"Compiler file open", true),

	CommandInfo("WUFindTagAtCursor",
		"\nFind Tag At Cursor\nMatches the tag under the cursor\n"
		"Find tag at cursor", true),

	CommandInfo("WUFindTagAtCursorDlg",
		"\nFind Tag At Cursor Dialog\nTries to match the tag under the cursor. "
		"Displays a dialog of all matching tags\n"
		"Find tag at cursor dialog", true),

	CommandInfo("WUFindPrevTag",
		"\nFind Previous Tag\nGoes to the previous matching tag\n"
		"Find previous tag", true),

	CommandInfo("WUFindNextTag",
		"\nFind Next Tag\nGoes to the next matching tag\n"
		"Find next tag", true),

	CommandInfo("WUFindPrevFile",
		"\nFind Previous File\nGoes to the previous matching file with the same title\n"
		"Find previous file", true),

	CommandInfo("WUFindNextFile",
		"\nFind Next File\nGoes to the next matching file with the same title\n"
		"Find next file", true),

	CommandInfo(
		"WUFixMultiplePrecompiledHeaders",
		"\nFix precompiled headers\nFixes the .dsp file to allow multiple precompiled headers\n"
		"Fix precompiled headers", true),

	CommandInfo("", "", false)
};

const CommandInfo* GetCommandList()
{
	return commandList;
}

// This is called when the user first loads the add-in, and on start-up
//  of each subsequent Developer Studio session
STDMETHODIMP CDSAddIn::OnConnection(IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// Store info passed to us
	IApplication* pApplication = NULL;
	if (FAILED(pApp->QueryInterface(IID_IApplication, (void**) &pApplication))
		|| pApplication == NULL)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	m_dwCookie = dwCookie;

	// Create command dispatch, send info back to DevStudio
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();

	// The QueryInterface above AddRef'd the Application object.  It will
	//  be Release'd in CCommand's destructor.
	m_pCommands->SetApplicationObject(pApplication);

	VERIFY_OK(pApplication->SetAddInInfo((long) AfxGetInstanceHandle(),
		(LPDISPATCH) m_pCommands, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, m_dwCookie));

	int curCommand = 0;
	while (!commandList[curCommand].m_name.IsEmpty())
	{
		VARIANT_BOOL bRet;
		VERIFY_OK(pApplication->AddCommand(
			CComBSTR(commandList[curCommand].m_name + commandList[curCommand].m_desc),
			CComBSTR(commandList[curCommand].m_name),
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
		if (bFirstTime == VARIANT_TRUE  &&  commandList[curCommand].m_toolbar)
		{
			VERIFY_OK(pApplication->
				AddCommandBarButton(dsGlyph, CComBSTR(commandList[curCommand].m_name), m_dwCookie));
		}

		curCommand++;
	}

	*OnConnection = VARIANT_TRUE;
	return S_OK;
}

// This is called on shut-down, and also when the user unloads the add-in
STDMETHODIMP CDSAddIn::OnDisconnection(VARIANT_BOOL bLastTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pCommands->UnadviseFromEvents();
	m_pCommands->Release();
	m_pCommands = NULL;

	// TODO: Perform any cleanup work here

	return S_OK;
}
