///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/Commands.cpp $
// $Date: 2003/01/05 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "Commands.h"
#include "WWhizCommands.h"
#include "History.h"
	 
#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommands

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
	
	WWhizCommands::SetDefaultWWhizDirectory();

//	LoadTags();
	History::Reset();
}

void CCommands::UnadviseFromEvents()
{
	WWhizCommands::SaveTags();

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
		m_pApplication->get_Debugger(&pDebugger);
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

	if (theDocument)
	{
		WWhizCommands::OnDocumentSave();
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

	WWhizCommands::OnWorkspaceOpen();
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	WWhizCommands::OnWorkspaceClose();
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	WWhizCommands::OnWorkspaceNew();
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
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWFileOpen();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWFileGlobalOpen()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWFileGlobalOpen();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWHeaderFlip()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWHeaderFlip();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWFileFindPrev()
{
	return WWhizCommands::WWFileFindPrev();
}


STDMETHODIMP CCommands::WWFileFindNext()
{
	return WWhizCommands::WWFileFindNext();
}


STDMETHODIMP CCommands::WWHistoryBack()
{
	return WWhizCommands::WWHistoryBack();
}


STDMETHODIMP CCommands::WWHistoryForward()
{
	return WWhizCommands::WWHistoryForward();
}


STDMETHODIMP CCommands::WWHistoryDlg()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWHistoryDlg();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWTagFindDlg()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWTagFindDlg();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWTagFindSpecialDlg()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWTagFindSpecialDlg();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWTagFindAtCursorDlg()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWTagFindAtCursorDlg();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}

STDMETHODIMP CCommands::WWTagFindAtCursor()
{
	return WWhizCommands::WWTagFindAtCursor();
}


STDMETHODIMP CCommands::WWTagFlip()
{
	return WWhizCommands::WWTagFlip();
}


STDMETHODIMP CCommands::WWTagFindNext()
{
	return WWhizCommands::WWTagFindNext();
}

STDMETHODIMP CCommands::WWTagFindPrev()
{
	return WWhizCommands::WWTagFindPrev();
}


STDMETHODIMP CCommands::WWTagCompleteNext()
{
	return WWhizCommands::WWTagCompleteNext();
}


STDMETHODIMP CCommands::WWTagCompletePrev()
{
	return WWhizCommands::WWTagCompletePrev();
}


STDMETHODIMP CCommands::WWTagCompleteRestore()
{
	return WWhizCommands::WWTagCompleteRestore();
}


STDMETHODIMP CCommands::WWTagCompleteDlg()
{
	return WWhizCommands::WWTagCompleteDlg();
}


STDMETHODIMP CCommands::WWTemplateSelect()
{
	m_pApplication->EnableModeless(VARIANT_FALSE);
	HRESULT hr = WWhizCommands::WWTemplateSelect();
	m_pApplication->EnableModeless(VARIANT_TRUE);

	return hr;
}


STDMETHODIMP CCommands::WWTemplateComplete()
{
	return WWhizCommands::WWTemplateComplete();
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


STDMETHODIMP CCommands::WWOptions()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	WWhizCommands::WWOptions();

	return S_OK;
}
