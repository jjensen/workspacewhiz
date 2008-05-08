// WorkspaceUtils.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <initguid.h>
#include "WorkspaceUtils.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_DSAddIn, CDSAddIn)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceUtilsApp

BEGIN_MESSAGE_MAP(CWorkspaceUtilsApp, CWinApp)
	//{{AFX_MSG_MAP(CWorkspaceUtilsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CWorkspaceUtilsApp object

CWorkspaceUtilsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceUtilsApp construction

CWorkspaceUtilsApp::CWorkspaceUtilsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceUtilsApp initialization

BOOL CWorkspaceUtilsApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	return CWinApp::InitInstance();
}

int CWorkspaceUtilsApp::ExitInstance()
{
	_Module.Term();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _Module.GetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

// by exporting DllRegisterServer, you can use regsvr32.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hRes = S_OK;
	
	// Registers object, typelib and all interfaces in typelib
	hRes = _Module.RegisterServer(TRUE);
	if (FAILED(hRes))
		return hRes;

	CString displayName;
	CString strDescription;
	strDescription.LoadString(IDS_WORKSPACEUTILS_DESCRIPTION);

	// Register description of this add-in object in its own
	//  "/Description" subkey.
	// TODO:  If you add more add-ins to this module, you need
	//  to register all of their descriptions, each description
	//  in each add-in object's registry CLSID entry:
	// HKEY_CLASSES_ROOT\Clsid\{add-in CLSID}\Description="add-in description"
	_ATL_OBJMAP_ENTRY* pEntry = _Module.m_pObjMap;
	CRegKey key;
	LONG lRes = key.Open(HKEY_CLASSES_ROOT, _T("CLSID"), KEY_READ);
	if (lRes == ERROR_SUCCESS)
	{
		USES_CONVERSION;
		LPOLESTR lpOleStr;
		StringFromCLSID(*pEntry->pclsid, &lpOleStr);
		LPTSTR lpsz = OLE2T(lpOleStr);

		lRes = key.Open(key, lpsz);
		if (lRes == ERROR_SUCCESS)
		{
			LPTSTR buf = displayName.GetBuffer(_MAX_PATH);
			DWORD len = _MAX_PATH;
			if (key.QueryValue(buf, "", &len) == ERROR_SUCCESS)
			{
				displayName.ReleaseBuffer(len);
			}
			else
			{
				displayName.ReleaseBuffer(0);
			}

			key.SetKeyValue(_T("Description"), strDescription);
		}
		CoTaskMemFree(lpOleStr);
	}
	if (lRes != ERROR_SUCCESS)
		hRes = HRESULT_FROM_WIN32(lRes);

	// Get the module name.
	TCHAR moduleName[_MAX_PATH];
	moduleName[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&moduleName, _MAX_PATH);

	// Get the module path.
	TCHAR modulePath[_MAX_PATH];
	_tcscpy(modulePath, moduleName);
	TCHAR* ptr = _tcsrchr(modulePath, '\\');
	*(ptr + 1) = 0;

	CRegKey devKey;
#if _MSC_VER >= 1200
	// Does DevStudio 6.0 exist?
	if (devKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\Addins") == ERROR_SUCCESS)
		{
			// Create the WorkspaceUtils.DSAddin.3 key.
			if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\Addins\\WorkspaceUtils.DSAddin.3") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetValue("0");
				devKey.SetValue(strDescription, _T("Description"));
				devKey.SetValue(displayName, _T("DisplayName"));

				devKey.SetValue(moduleName, _T("Filename"));
			}
		}
	}

#else
	// Does DevStudio 5.0 exist?
	if (devKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0\\Addins") == ERROR_SUCCESS)
		{
			// Create the WorkspaceUtils.DSAddin.3 key.
			if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0\\Addins\\WorkspaceUtils.DSAddin.3") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetValue("0");
				devKey.SetValue(strDescription, _T("Description"));
				devKey.SetValue(displayName, _T("DisplayName"));

				devKey.SetValue(moduleName, _T("Filename"));
			}
		}
	}
#endif //_MSC_VER

	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Remove entries from DevStudio 6.0.
	CRegKey key;
#if _MSC_VER >= 1200
	if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WorkspaceUtils.DSAddin.1");
		key.RecurseDeleteKey("WorkspaceUtils.DSAddin.2");
		key.RecurseDeleteKey("WorkspaceUtils.DSAddin.3");
	}

	if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\Keyboard\\Aut") == ERROR_SUCCESS)
	{
		const CommandInfo* commandList = GetCommandList();
		while (!commandList->m_name.IsEmpty())
		{
			key.DeleteValue(commandList->m_name);
			commandList++;
		}
	}
#else
	// Remove entries from DevStudio 5.0.
	if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WorkspaceUtils.DSAddin.1");
		key.RecurseDeleteKey("WorkspaceUtils.DSAddin.2");
		key.RecurseDeleteKey("WorkspaceUtils.DSAddin.3");
	}
#endif //_MSC_VER

	// Unregister the server.
	HRESULT hRes = S_OK;
#if _MSC_VER > 1100
	_Module.UnregisterServer(TRUE, NULL);
#else
	_Module.UnregisterServer();
#endif

	
	return hRes;
}


/////////////////////////////////////////////////////////////////////////////
// Debugging support

// GetLastErrorDescription is used in the implementation of the VERIFY_OK
//  macro, defined in stdafx.h.

#ifdef _DEBUG

void GetLastErrorDescription(CComBSTR& bstr)
{
	CComPtr<IErrorInfo> pErrorInfo;
	if (GetErrorInfo(0, &pErrorInfo) == S_OK)
		pErrorInfo->GetDescription(&bstr);
}

#endif //_DEBUG
