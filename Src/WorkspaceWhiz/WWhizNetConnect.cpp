// Connect.cpp : Implementation of CConnect
#include "stdafx.h"
#include "WWhizNet.h"
#include "WWhizNetConnect.h"
#include "WWhizInterfaceLoader.h"
#include "WWhizTemplateManagerLoader.h"
#include "WWhizRegLoader.h"
#include "WWhizCommands.h"
#include "TemplateWizardDialog.h"
#include <io.h>

#include "dte80.tlh"
#include "mso.tlh"
#include "vsmso.tlh"

static WWhizTemplateDialogImpl s_templateDialogImpl;

extern CAddInModule _AtlModule;

void PrintStatusBar(LPCTSTR message)
{
	CComPtr<EnvDTE::StatusBar> pStatusBar;
	g_pDTE->get_StatusBar(&pStatusBar);
	pStatusBar->put_Text(CComBSTR(message));
}


class SolutionEventsSink70 : public IDispEventImpl<1, SolutionEventsSink70, &__uuidof(EnvDTE::_dispSolutionEvents), &EnvDTE::LIBID_EnvDTE, 7, 0>
{
public:
	BEGIN_SINK_MAP(SolutionEventsSink70)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispSolutionEvents), 1, Opened)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispSolutionEvents), 2, BeforeClosing)
	END_SINK_MAP()

	void __stdcall Opened()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		WWhizCommands::OnWorkspaceOpen();
	}

	void __stdcall BeforeClosing()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		WWhizCommands::OnWorkspaceClose();
	}
};


class SolutionEventsSink80 : public IDispEventImpl<1, SolutionEventsSink80, &__uuidof(EnvDTE::_dispSolutionEvents), &EnvDTE::LIBID_EnvDTE, 8, 0>
{
public:
	BEGIN_SINK_MAP(SolutionEventsSink80)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispSolutionEvents), 1, Opened)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispSolutionEvents), 2, BeforeClosing)
	END_SINK_MAP()

	void __stdcall Opened()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		WWhizCommands::OnWorkspaceOpen();
	}

	void __stdcall BeforeClosing()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		WWhizCommands::OnWorkspaceClose();
	}
};


class DocumentEventsSink70 : public IDispEventImpl<1, DocumentEventsSink70, &__uuidof(EnvDTE::_dispDocumentEvents), &EnvDTE::LIBID_EnvDTE, 7, 0>
{
public:
	BEGIN_SINK_MAP(DocumentEventsSink70)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispDocumentEvents), 1, DocumentSaved)
	END_SINK_MAP()

	void __stdcall DocumentSaved(EnvDTE::Document *Document)
	{
		CComBSTR bstrName;
		Document->get_FullName( &bstrName );

		g_wwhizInterface->AddChangedFile(CString(bstrName));
	}
};


class DocumentEventsSink80 : public IDispEventImpl<1, DocumentEventsSink80, &__uuidof(EnvDTE::_dispDocumentEvents), &EnvDTE::LIBID_EnvDTE, 8, 0>
{
public:
	BEGIN_SINK_MAP(DocumentEventsSink80)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispDocumentEvents), 1, DocumentSaved)
	END_SINK_MAP()

	void __stdcall DocumentSaved(EnvDTE::Document *Document)
	{
		CComBSTR bstrName;
		Document->get_FullName( &bstrName );

		g_wwhizInterface->AddChangedFile(CString(bstrName));
	}
};


/*class WindowEventsSink : public IDispEventImpl<1, WindowEventsSink, &__uuidof(EnvDTE::_dispWindowEvents), &EnvDTE::LIBID_EnvDTE, 8, 0>
{
public:
	BEGIN_SINK_MAP(WindowEventsSink)
		SINK_ENTRY_EX(1, __uuidof(EnvDTE::_dispWindowEvents), 3, WindowActivated)
	END_SINK_MAP()

	void __stdcall WindowActivated(EnvDTE::Window* GotFocus, EnvDTE::Window* LostFocus)
	{
		extern CComPtr<EnvDTE::Window> g_pLastDocumentWindow;
		if (GotFocus == g_pLastDocumentWindow)
		{
			g_pLastDocumentWindow = NULL;
			g_pDTE->ExecuteCommand(CComBSTR("WWhizNet.Connect.InternalPutTemplate"), CComBSTR()); 
		}
	}
};
*/

// When run, the Add-in wizard prepared the registry for the Add-in.
// At a later time, if the Add-in becomes unavailable for reasons such as:
//   1) You moved this project to a computer other than which is was originally created on.
//   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
//   3) Registry corruption.
// you will need to re-register the Add-in by building the MyAddin21Setup project 
// by right clicking the project in the Solution Explorer, then choosing install.
void UnregisterCommand(CComPtr<EnvDTE::Commands>& pCommands, CStringW commandName)
{
	CComPtr<EnvDTE::Command> pCommand;
	HRESULT hr = pCommands->Item(CComVariant(L"WWhizNet.Connect." + commandName), -1,
		&pCommand);

	if (SUCCEEDED(hr))
	{
		pCommand->Delete();
	}

	pCommand = NULL;
	hr = pCommands->Item(CComVariant(L"WWhizNet8.Connect." + commandName), -1,
		&pCommand);

	if (SUCCEEDED(hr))
	{
		pCommand->Delete();
	}
}


/////////////////////////////////////////////////////////////////////////////

BOOL AFXAPI AfxExtractSubString(CString& rString, LPCTSTR lpszFullString,
	int iSubString, TCHAR chSep)
{
	if (lpszFullString == NULL)
		return FALSE;

	while (iSubString--)
	{
		lpszFullString = _tcschr(lpszFullString, chSep);
		if (lpszFullString == NULL)
		{
			rString.Empty();        // return empty string as well
			return FALSE;
		}
		lpszFullString++;       // point past the separator
	}
	LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
	int nLen = (lpchEnd == NULL) ?
		lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
	ASSERT(nLen >= 0);
	memcpy(rString.GetBufferSetLength(nLen), lpszFullString, nLen*sizeof(TCHAR));
	return TRUE;
}


SolutionEventsSink70* m_SolutionEventsSink70;
SolutionEventsSink80* m_SolutionEventsSink80;
DocumentEventsSink70* m_DocumentEventsSink70;
DocumentEventsSink80* m_DocumentEventsSink80;
//WindowEventsSink m_WindowEventsSink;

CComPtr<EnvDTE::_SolutionEvents> pSolutionEvents;
CComPtr<EnvDTE::_DocumentEvents> pDocumentEvents;
CComPtr<EnvDTE::_WindowEvents> pWindowEvents;


void UnregisterAllCommands70(CComPtr<EnvDTE::_DTE>& pDTE)
{
	CComPtr<EnvDTE::Commands> pCommands;
	pDTE->get_Commands(&pCommands);

	// Unregister all previously registered commands.
	UnregisterCommand(pCommands, L"WorkspaceFileOpen");
	UnregisterCommand(pCommands, L"WorkspaceFileOpen2");
	UnregisterCommand(pCommands, L"InternalTemplatePut");

	for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
	{
		const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
		CStringW commandName = commandInfo->m_name;
		UnregisterCommand(pCommands, commandName);
	}
}


void UnregisterAllCommands80(CComPtr<EnvDTE80::DTE2>& pDTE)
{
	CComPtr<EnvDTE::Commands> pCommands;
	pDTE->get_Commands(&pCommands);

	// Unregister all previously registered commands.
	UnregisterCommand(pCommands, L"WorkspaceFileOpen");
	UnregisterCommand(pCommands, L"WorkspaceFileOpen2");
	UnregisterCommand(pCommands, L"InternalTemplatePut");

	for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
	{
		const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
		CStringW commandName = commandInfo->m_name;
		UnregisterCommand(pCommands, commandName);
	}
}


void UnregisterAllCommands90(CComPtr<EnvDTE80::DTE2>& pDTE)
{
	CComPtr<EnvDTE::Commands> pCommands;
	pDTE->get_Commands(&pCommands);

	// Unregister all previously registered commands.
	UnregisterCommand(pCommands, L"WorkspaceFileOpen");
	UnregisterCommand(pCommands, L"WorkspaceFileOpen2");
	UnregisterCommand(pCommands, L"InternalTemplatePut");

	for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
	{
		const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
		CStringW commandName = commandInfo->m_name;
		UnregisterCommand(pCommands, commandName);
	}
}


void UnregisterAllCommands100(CComPtr<EnvDTE80::DTE2>& pDTE)
{
	CComPtr<EnvDTE::Commands> pCommands;
	pDTE->get_Commands(&pCommands);

	// Unregister all previously registered commands.
	UnregisterCommand(pCommands, L"WorkspaceFileOpen");

	for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
	{
		const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
		CStringW commandName = commandInfo->m_name;
		UnregisterCommand(pCommands, commandName);
	}
}


CComBSTR version;


void CConnect::FreeEvents()
{
	if ( pSolutionEvents )
	{
		if (version == "7.00"  ||  version == "7.10")
		{
			m_SolutionEventsSink70->DispEventUnadvise((IUnknown*)pSolutionEvents.p);
			delete m_SolutionEventsSink70;
			m_SolutionEventsSink70 = NULL;
		}
		else if (version = "8.0"  ||  version == "9.0"  ||  version == "10.0")
		{
			m_SolutionEventsSink80->DispEventUnadvise((IUnknown*)pSolutionEvents.p);
			delete m_SolutionEventsSink80;
			m_SolutionEventsSink80 = NULL;
		}
		pSolutionEvents = NULL;
	}

	if ( pDocumentEvents )
	{
		if (version == "7.00"  ||  version == "7.10")
		{
			m_DocumentEventsSink70->DispEventUnadvise((IUnknown*)pDocumentEvents.p);
			delete m_DocumentEventsSink70;
			m_DocumentEventsSink70 = NULL;
		}
		else if (version = "8.0"  ||  version == "9.0"  ||  version == "10.0")
		{
			m_DocumentEventsSink80->DispEventUnadvise((IUnknown*)pDocumentEvents.p);
			delete m_DocumentEventsSink80;
			m_DocumentEventsSink80 = NULL;
		}
		pDocumentEvents = NULL;
	}
}


HWND g_devStudioWnd;
HHOOK s_wndProcHook;

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


// IDTExtensibility2 implementation:
STDMETHODIMP CConnect::OnConnection(IDispatch *pApplication,
		AddInDesignerObjects::ext_ConnectMode ConnectMode, IDispatch *pAddInInst,
		SAFEARRAY ** /*custom*/ )
{
	try
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// Extra error checking, because I still don't understand the source of
		// the startup and shutdown crashes.
		g_pDTE = NULL;
		m_pAddInInstance = NULL;

		HRESULT hr = S_OK;
		pApplication->QueryInterface(__uuidof(EnvDTE::_DTE), (LPVOID*)&g_pDTE);
		pAddInInst->QueryInterface(__uuidof(EnvDTE::AddIn), (LPVOID*)&m_pAddInInstance);

		CComPtr<EnvDTE80::DTE2> pDTE2;
		pApplication->QueryInterface(__uuidof(EnvDTE80::DTE2), (LPVOID*)&pDTE2);

		g_pDTE->get_Version(&version);

		// Chances are, this won't work.
		if ( ConnectMode == AddInDesignerObjects::ext_cm_CommandLine )
			return S_OK;

		CComPtr<IDispatch> pDisp;
		CComPtr<EnvDTE::Commands> pCommands;
		hr = g_pDTE->get_Commands( &pCommands );
		if ( FAILED( hr ) )
			return hr;
		if ( !pCommands )
			return E_FAIL;

		if (version == "7.00"  ||  version == "7.10")
		{
			CComPtr<Office::_CommandBars> pCommandBars;
			CComPtr<Office::CommandBar> pCommandBar;
			// Get the set of command bars for the application.
			hr = g_pDTE->get_CommandBars( (EnvDTE::_CommandBars**)&pCommandBars );
			if ( FAILED( hr ) )
				return hr;

			// See if the Workspace Whiz toolbar has been created.
			bool forceRecreateOfToolbarAndCommands = false;
			hr = pCommandBars->get_Item( CComVariant( L"Workspace Whiz" ), &pCommandBar );
			if ( SUCCEEDED( hr ) )
			{
				CComPtr<Office::CommandBarControls> pCommandBarControls;
				pCommandBar->get_Controls( &pCommandBarControls );

				int count;
				pCommandBarControls->get_Count( &count );

				if ( count == 0 )
				{
					CString str;
					str.LoadString( IDS_MISSINGTOOLBARCONTENTS );
					if ( AfxMessageBox( IDS_MISSINGTOOLBARCONTENTS, MB_YESNO ) == IDYES )
					{
						forceRecreateOfToolbarAndCommands = true;
					}
				}

				pCommandBar = NULL;
			}
			else
			{
				forceRecreateOfToolbarAndCommands = true;
			}

			// Build the toolbar.
			if ( ConnectMode == 5  ||  forceRecreateOfToolbarAndCommands ) //5 == AddInDesignerObjects::ext_cm_UISetup)
			{
				// See if the Workspace Whiz toolbar has been created.
				hr = pCommandBars->get_Item( CComVariant( L"Workspace Whiz" ), &pCommandBar );
				if ( FAILED( hr ) )
				{
					CComPtr<IDispatch> pDispCommandBar;

					// The toolbar hasn't been created yet.  Add it.
					hr = pCommands->AddCommandBar(CComBSTR(L"Workspace Whiz"),
						EnvDTE::vsCommandBarTypeToolbar, NULL, 
						1, &pDispCommandBar);

					// Yes, this code is unnecessary, but it serves to prove that
					// the command bar creation actually worked.
					hr = pCommandBars->get_Item(CComVariant(L"Workspace Whiz"), &pCommandBar);
				}

				pCommandBar->put_Visible(VARIANT_TRUE);

				// When run, the Add-in wizard prepared the registry for the Add-in.
				// At a later time, the Add-in or its commands may become unavailable for reasons such as:
				//   1) You moved this project to a computer other than which is was originally created on.
				//   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
				//   3) You add new commands or modify commands already defined.
				// You will need to re-register the Add-in by building the WWhizNetSetup project,
				// right-clicking the project in the Solution Explorer, and then choosing install.
				// Alternatively, you could execute the ReCreateCommands.reg file the Add-in Wizard generated in 
				// the project directory, or run 'devenv /setup' from a command prompt.

				// Unregister all previously registered commands.
				UnregisterCommand(pCommands, L"WorkspaceFileOpen");
				UnregisterCommand(pCommands, L"WorkspaceFileOpen2");
				UnregisterCommand(pCommands, L"InternalTemplatePut");

				for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
				{
					CComPtr<EnvDTE::Command> pCreatedCommand;

					const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
					CStringW commandName = commandInfo->m_name;
					UnregisterCommand(pCommands, commandName);

		//			if (s_commandList[curCommand].m_removeOnly)
		//				continue;

					CString regStr;
					regStr.LoadString(IDS_CI_WWOPTIONS + curCommand);

					CString buttonText;
					CString toolTip;
					AfxExtractSubString(buttonText, regStr, 1, '\n');
					AfxExtractSubString(toolTip, regStr, 2, '\n');

					pCreatedCommand = NULL;
					hr = pCommands->AddNamedCommand(m_pAddInInstance, CComBSTR(commandName),
							CComBSTR(buttonText), CComBSTR(toolTip), VARIANT_FALSE,
							commandInfo->m_bitmapID, NULL,
							EnvDTE::vsCommandStatusSupported+EnvDTE::vsCommandStatusEnabled+
							EnvDTE::vsCommandStatusTextWantedNone,
							&pCreatedCommand);
					if(SUCCEEDED(hr) && (pCreatedCommand))
					{
						// Add a button to the tools menu bar.
						CComPtr<Office::CommandBarControl> pCommandBarControl;
						hr = pCreatedCommand->AddControl( pCommandBar, curCommand + 1, (EnvDTE::CommandBarControl**)&pCommandBarControl );
						if ( FAILED( hr ) )
							return hr;
						if ( !pCommandBar )
							return E_FAIL;

						CComQIPtr<Office::_CommandBarButton> pCommandBarButton(pCommandBarControl);
						pCommandBarButton->put_Style(Office::msoButtonIcon);
					}
				}
			}
		}
		else if (version == "8.0"  ||  version == "9.0"  ||  version == "10.0")
		{
			CComQIPtr<Microsoft_VisualStudio_CommandBars::_CommandBars> pCommandBars;
			CComPtr<Microsoft_VisualStudio_CommandBars::CommandBar> pCommandBar;
			// Get the set of command bars for the application.
			hr = pDTE2->get_CommandBars( (EnvDTE::_CommandBars**)&pDisp );
			if ( FAILED( hr ) )
				return hr;
			pCommandBars = pDisp;
			if ( !pCommandBars )
				return E_FAIL;

			// See if the Workspace Whiz toolbar has been created.
			bool forceRecreateOfToolbarAndCommands = false;
			hr = pCommandBars->get_Item( CComVariant( L"Workspace Whiz" ), &pCommandBar );
			if ( SUCCEEDED( hr ) )
			{
				CComPtr<Microsoft_VisualStudio_CommandBars::CommandBarControls> pCommandBarControls;
				pCommandBar->get_Controls( &pCommandBarControls );

				int count;
				pCommandBarControls->get_Count( &count );

				if ( count == 0 )
				{
					CString str;
					str.LoadString( IDS_MISSINGTOOLBARCONTENTS );
					if ( AfxMessageBox( IDS_MISSINGTOOLBARCONTENTS, MB_YESNO ) == IDYES )
					{
						forceRecreateOfToolbarAndCommands = true;
					}
				}

				pCommandBar = NULL;
			}
			else
			{
				forceRecreateOfToolbarAndCommands = true;
			}

			// Build the toolbar.
			if ( ConnectMode == 5  ||  forceRecreateOfToolbarAndCommands ) //5 == AddInDesignerObjects::ext_cm_UISetup)
			{
				// See if the Workspace Whiz toolbar has been created.
				hr = pCommandBars->get_Item( CComVariant( L"Workspace Whiz" ), &pCommandBar );
				if ( FAILED( hr ) )
				{
					CComPtr<IDispatch> pDispCommandBar;

					// The toolbar hasn't been created yet.  Add it.
					hr = pCommands->AddCommandBar(CComBSTR(L"Workspace Whiz"),
						EnvDTE::vsCommandBarTypeToolbar, NULL, 
						1, &pDispCommandBar);

					// Yes, this code is unnecessary, but it serves to prove that
					// the command bar creation actually worked.
					hr = pCommandBars->get_Item(CComVariant(L"Workspace Whiz"), &pCommandBar);
				}

				pCommandBar->put_Visible(VARIANT_TRUE);

				// When run, the Add-in wizard prepared the registry for the Add-in.
				// At a later time, the Add-in or its commands may become unavailable for reasons such as:
				//   1) You moved this project to a computer other than which is was originally created on.
				//   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
				//   3) You add new commands or modify commands already defined.
				// You will need to re-register the Add-in by building the WWhizNetSetup project,
				// right-clicking the project in the Solution Explorer, and then choosing install.
				// Alternatively, you could execute the ReCreateCommands.reg file the Add-in Wizard generated in 
				// the project directory, or run 'devenv /setup' from a command prompt.

				// Unregister all previously registered commands.
				UnregisterCommand(pCommands, L"WorkspaceFileOpen");
				UnregisterCommand(pCommands, L"WorkspaceFileOpen2");
				UnregisterCommand(pCommands, L"InternalTemplatePut");

				for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
				{
					CComPtr<EnvDTE::Command> pCreatedCommand;

					const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
					CStringW commandName = commandInfo->m_name;
					UnregisterCommand(pCommands, commandName);

		//			if (s_commandList[curCommand].m_removeOnly)
		//				continue;

					CString regStr;
					regStr.LoadString(IDS_CI_WWOPTIONS + curCommand);

					CString buttonText;
					CString toolTip;
					AfxExtractSubString(buttonText, regStr, 1, '\n');
					AfxExtractSubString(toolTip, regStr, 2, '\n');

					pCreatedCommand = NULL;
					CComPtr<EnvDTE80::Commands2> pCommands2;
					pCommands2 = pCommands;
					if ( !pCommands2 )
						return E_FAIL;
					hr = pCommands2->AddNamedCommand2(m_pAddInInstance, CComBSTR(commandName),
							CComBSTR(buttonText), CComBSTR(toolTip), VARIANT_FALSE,
							CComVariant(commandInfo->m_bitmapID), NULL,
							EnvDTE::vsCommandStatusSupported+EnvDTE::vsCommandStatusEnabled,
							EnvDTE80::vsCommandStylePict, EnvDTE80::vsCommandControlTypeButton, 
							&pCreatedCommand);
 					if(SUCCEEDED(hr) && (pCreatedCommand))
					{
						// Add a button to the tools menu bar.
						CComPtr<Microsoft_VisualStudio_CommandBars::CommandBarControl> pCommandBarControl;
						pDisp = NULL;
						hr = pCreatedCommand->AddControl( pCommandBar, curCommand + 1, (EnvDTE::CommandBarControl**)&pDisp );
						if ( FAILED( hr ) )
							return hr;
						pCommandBarControl = pDisp;
						if ( !pCommandBar )
							return E_FAIL;

						CComQIPtr<Microsoft_VisualStudio_CommandBars::_CommandBarButton> pCommandBarButton(pCommandBarControl);
						pCommandBarButton->put_Style(Microsoft_VisualStudio_CommandBars::msoButtonIcon);
					}
				}
			}
		}

		// Find the Visual Studio main window.
		CComPtr<EnvDTE::Window> pMainWindow;
		ObjModelHelper::GetInterface()->get_MainWindow(&pMainWindow);
		if (!pMainWindow)
		{
			::MessageBeep(MB_ICONEXCLAMATION);
			//????
			return S_OK;
		}

		pMainWindow->get_HWnd((long*)&g_devStudioWnd);

		s_wndProcHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc,
			AfxGetInstanceHandle(), AfxGetThread()->m_nThreadID);

		FreeEvents();

		// Retrieve the list of events.
		CComPtr<EnvDTE::Events> pEvents;
		hr = g_pDTE->get_Events( &pEvents );
		if ( FAILED( hr ) )
			return hr;

		// If there are events registered and we got here, then VStudio is
		// really messing with us...
/*		if (pWindowEvents)
		{
			m_WindowEventsSink.DispEventUnadvise((IUnknown*)pWindowEvents.p);
			pWindowEvents = NULL;
		}*/

		if (version == "7.00"  ||  version == "7.10")
		{
			// Register the event systems.
			if(SUCCEEDED(pEvents->get_SolutionEvents((EnvDTE::_SolutionEvents**)&pSolutionEvents)))
			{
				m_SolutionEventsSink70 = new SolutionEventsSink70;
				m_SolutionEventsSink70->DispEventAdvise((IUnknown*)pSolutionEvents.p);
			}

			if(SUCCEEDED(pEvents->get_DocumentEvents(NULL, (EnvDTE::_DocumentEvents**)&pDocumentEvents)))
			{
				m_DocumentEventsSink70 = new DocumentEventsSink70;
				m_DocumentEventsSink70->DispEventAdvise((IUnknown*)pDocumentEvents.p);
			}

	/*		if(SUCCEEDED(pEvents->get_WindowEvents(NULL, (EnvDTE::_WindowEvents**)&pWindowEvents)))
			{
				m_WindowEventsSink.DispEventAdvise((IUnknown*)pWindowEvents.p);
			}*/
		}
		else if (version == "8.0"  ||  version == "9.0"  ||  version == "10.0")
		{
			// Register the event systems.
			if(SUCCEEDED(pEvents->get_SolutionEvents((EnvDTE::_SolutionEvents**)&pSolutionEvents)))
			{
				m_SolutionEventsSink80 = new SolutionEventsSink80;
				m_SolutionEventsSink80->DispEventAdvise((IUnknown*)pSolutionEvents.p);
			}

			if(SUCCEEDED(pEvents->get_DocumentEvents(NULL, (EnvDTE::_DocumentEvents**)&pDocumentEvents)))
			{
				m_DocumentEventsSink80 = new DocumentEventsSink80;
				m_DocumentEventsSink80->DispEventAdvise((IUnknown*)pDocumentEvents.p);
			}
		}

		g_wwhizInterface = WWhizInterfaceCreate(_AtlModule.GetResourceInstance(), g_pDTE);
		g_wwhizReg = WWhizRegCreate(g_pDTE, _AtlModule.GetResourceInstance(), g_wwhizInterface);
		g_wwhizInterface->SetWWhizReg(g_wwhizReg);
		g_wwhizTemplateManager = WWhizTemplateManagerCreate(g_pDTE, &s_templateDialogImpl);

		// Check the first time.
		g_config.LoadRegistry();

		WWhizCommands::SetDefaultWWhizDirectory();

		// Load the template stuff from the registry.
		g_wwhizTemplateManager->LoadRegistry();
	}
	catch ( ... )
	{
	}

	return S_OK;
}

// 
STDMETHODIMP CConnect::OnDisconnection(AddInDesignerObjects::ext_DisconnectMode RemoveMode, SAFEARRAY ** /*custom*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	try
	{
		FreeEvents();
	}
	catch ( ... )
	{
	}
		
	::UnhookWindowsHookEx(s_wndProcHook);

	try
	{
		WWhizCommands::OnShutdown();
	}
	catch(...)
	{
		AfxMessageBox("Workspace Whiz has thrown an exception.");
	}

	try
	{
		m_pAddInInstance = NULL;
		g_pDTE = NULL;
	}
	catch(...)
	{
	}

	return S_OK;
}

// 
STDMETHODIMP CConnect::OnAddInsUpdate (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}


// 
STDMETHODIMP CConnect::OnStartupComplete (SAFEARRAY ** /*custom*/ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	try
	{
		if (g_config.m_dsFirstTime)
		{
			WWhizCommands::WWOptions();
		}

		g_config.m_dsFirstTime = false;
		g_config.SaveRegistry_FirstTime();
	}
	catch ( ... )
	{
	}

	return S_OK;
}

// 
STDMETHODIMP CConnect::OnBeginShutdown (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

// IDTCommandTarget implementation:
STDMETHODIMP CConnect::QueryStatus(BSTR bstrCmdName, EnvDTE::vsCommandStatusTextWanted NeededText, EnvDTE::vsCommandStatus *pStatusOption, VARIANT *pvarCommandText)
{
	if(NeededText == EnvDTE::vsCommandStatusTextWantedNone)
	{
		for (int curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
		{
			const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
			CStringW commandName = commandInfo->m_name;
			if(!_wcsicmp(bstrCmdName, L"WWhizNet.Connect." + commandName))
			{
				*pStatusOption = (EnvDTE::vsCommandStatus)(EnvDTE::vsCommandStatusEnabled+EnvDTE::vsCommandStatusSupported);
			}
		}
	}
	return S_OK;
}

// 
STDMETHODIMP CConnect::Exec(BSTR bstrCmdName, EnvDTE::vsCommandExecOption ExecuteOption, VARIANT * /*pvarVariantIn*/, VARIANT * /*pvarVariantOut*/, VARIANT_BOOL *pvbHandled)
{
	*pvbHandled = VARIANT_FALSE;

	try
	{
		if(ExecuteOption == EnvDTE::vsCommandExecOptionDoDefault)
		{
			CStringW commandName = bstrCmdName;

		if (commandName.Left(17) == L"WWhizNet.Connect.")
				commandName = commandName.Mid(17);

			int curCommand;
			for (curCommand = 0; curCommand < WWhizCommands::GetCommandCount(); ++curCommand)
			{
				const WWhizCommands::CommandInfo* commandInfo = WWhizCommands::GetCommand(curCommand);
				if (commandInfo->m_name == commandName)
					break;
			}

			if (curCommand == WWhizCommands::GetCommandCount())
			{
				return S_OK;
			}

			switch (curCommand)
			{
				case WWhizCommands::Command_WWOptions:
					WWhizCommands::WWOptions();
					break;
				case WWhizCommands::Command_WWFileOpen:
					WWhizCommands::WWFileOpen();
					break;
				case WWhizCommands::Command_WWFileGlobalOpen:
					WWhizCommands::WWFileGlobalOpen();
					break;
				case WWhizCommands::Command_WWHeaderFlip:
					WWhizCommands::WWHeaderFlip();
					break;
				case WWhizCommands::Command_WWFileFindPrev:
					WWhizCommands::WWFileFindPrev();
					break;
				case WWhizCommands::Command_WWFileFindNext:
					WWhizCommands::WWFileFindNext();
					break;
				case WWhizCommands::Command_WWHistoryBack:
					WWhizCommands::WWHistoryBack();
					break;
				case WWhizCommands::Command_WWHistoryForward:
					WWhizCommands::WWHistoryForward();
					break;
				case WWhizCommands::Command_WWHistoryDlg:
					WWhizCommands::WWHistoryDlg();
					break;
				case WWhizCommands::Command_WWTagFindDlg:
					WWhizCommands::WWTagFindDlg();
					break;
				case WWhizCommands::Command_WWTagFindSpecialDlg:
					WWhizCommands::WWTagFindSpecialDlg();
					break;
				case WWhizCommands::Command_WWTagFlip:
					WWhizCommands::WWTagFlip();
					break;
				case WWhizCommands::Command_WWTagFindAtCursor:
					WWhizCommands::WWTagFindAtCursor();
					break;
				case WWhizCommands::Command_WWTagFindAtCursorDlg:
					WWhizCommands::WWTagFindAtCursorDlg();
					break;
				case WWhizCommands::Command_WWTagFindPrev:
					WWhizCommands::WWTagFindPrev();
					break;
				case WWhizCommands::Command_WWTagFindNext:
					WWhizCommands::WWTagFindNext();
					break;
				case WWhizCommands::Command_WWTagCompletePrev:
					WWhizCommands::WWTagCompletePrev();
					break;
				case WWhizCommands::Command_WWTagCompleteNext:
					WWhizCommands::WWTagCompleteNext();
					break;
				case WWhizCommands::Command_WWTagCompleteRestore:
					WWhizCommands::WWTagCompleteRestore();
					break;
				case WWhizCommands::Command_WWTagCompleteDlg:
					WWhizCommands::WWTagCompleteDlg();
					break;
				case WWhizCommands::Command_WWTemplateSelect:
					WWhizCommands::WWTemplateSelect();
					break;
				case WWhizCommands::Command_WWTemplateComplete:
					WWhizCommands::WWTemplateComplete();
					break;
			}

			*pvbHandled = VARIANT_TRUE;
			return S_OK;
		}
	}
	catch (...)
	{
		AfxMessageBox("Workspace Whiz crashed.");
	}

	return S_OK;
}


