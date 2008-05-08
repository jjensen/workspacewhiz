///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceWhiz.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/WorkspaceWhiz.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 18   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <initguid.h>
#include "ObjModelGUID.h"
#include "WorkspaceWhiz.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Bad place to put this...
Config g_config;

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_DSAddIn, CDSAddIn)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceWhizApp

BEGIN_MESSAGE_MAP(CWorkspaceWhizApp, CWinApp)
	//{{AFX_MSG_MAP(CWorkspaceWhizApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CWorkspaceWhizApp object

CWorkspaceWhizApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceWhizApp construction

CWorkspaceWhizApp::CWorkspaceWhizApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceWhizApp initialization

BOOL CWorkspaceWhizApp::InitInstance()
{
	free((void*)m_pszRegistryKey);
	m_pszRegistryKey = _tcsdup(_T("Workspace Whiz!"));

	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(_T("2.00"));

	// Set the help filename.
	TCHAR modulePath[_MAX_PATH];
	modulePath[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&modulePath, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(modulePath, '\\');
	if (ptr)
	{
		ptr++;
		*ptr = 0;
	}
	_tcscat(modulePath, "WorkspaceWhiz.chm");
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(_T(modulePath));

	_Module.Init(ObjectMap, m_hInstance);

	return CWinApp::InitInstance();
}

int CWorkspaceWhizApp::ExitInstance()
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

/**
**/
static void SaveTemplateRegistryDefaults()
{
	// Get the module name.
	TCHAR moduleName[_MAX_PATH];
	moduleName[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&moduleName, _MAX_PATH);

	// Get the module path.
	TCHAR modulePath[_MAX_PATH];
	_tcscpy(modulePath, moduleName);
	TCHAR* ptr = _tcsrchr(modulePath, '\\');
	*(ptr + 1) = 0;

	CString filename;
	WORD len;
	WORD count = 2;
	MemFile filenameFile;
	filenameFile.Write(&count, sizeof(count));

	filename = modulePath + CString("WWhizTemplateTest.WWTpl");
	len = filename.GetLength();
	filenameFile.Write(&len, sizeof(WORD));
	filenameFile.Write((LPCTSTR)filename, len);
	time_t time = 0;
	filenameFile.Write(&time, sizeof(time_t));

	filename = modulePath + CString("UsefulSamples.WWTpl");
	len = filename.GetLength();
	filenameFile.Write(&len, sizeof(WORD));
	filenameFile.Write((LPCTSTR)filename, len);
	time = 0;
	filenameFile.Write(&time, sizeof(time_t));

	LONG filenameFileSize = filenameFile.GetLength();
	BYTE* filenameFileMem = filenameFile.Detach();
	AfxGetApp()->WriteProfileBinary("Templates", "FileList",
		filenameFileMem, filenameFileSize);
	free(filenameFileMem);
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

	CString strDescription;
	strDescription.LoadString(IDS_WORKSPACEWHIZ_DESCRIPTION);

	// Register description of this add-in object in its own
	//  "/Description" subkey.
	// TODO:  If you add more add-ins to this module, you need
	//  to register all of their descriptions, each description
	//  in each add-in object's registry CLSID entry:
	// HKEY_CLASSES_ROOT\Clsid\{add-in CLSID}\Description="add-in description"
	_ATL_OBJMAP_ENTRY* pEntry = _Module.m_pObjMap;
	CString displayName;
	CRegKey key;
	LONG lRes = key.Open(HKEY_CLASSES_ROOT, _T("CLSID"), KEY_READ);
	if (lRes == ERROR_SUCCESS)
	{
		USES_CONVERSION;
		LPOLESTR lpOleStr;
		StringFromCLSID(*pEntry->pclsid, &lpOleStr);
		LPTSTR lpsz = OLE2T(lpOleStr);

		lRes = key.Open(key, lpsz, KEY_READ);
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
				// We must be on NT.
				displayName.ReleaseBuffer(0);

				TCHAR szDesc[256];
				LoadString(AfxGetInstanceHandle(), IDS_WORKSPACEWHIZ_LONGNAME, szDesc, 256);

				lRes = key.Open(key, lpsz);
				if (lRes == ERROR_SUCCESS)
				{
					// We must be on NT.
					key.SetValue(szDesc);
				}

				displayName = szDesc;
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

	// Does DevStudio 6.0 exist?
	CRegKey devKey;
	if (devKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\Addins") == ERROR_SUCCESS)
		{
			// Create the WorkspaceWhiz.DSAddin.1 key.
			if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\Addins\\WorkspaceWhiz.DSAddin.1") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetValue("1");
				devKey.SetValue(strDescription, _T("Description"));
				devKey.SetValue(displayName, _T("DisplayName"));

				devKey.SetValue(moduleName, _T("Filename"));
			}
		}
	}

	// Does DevStudio 5.0 exist?
	if (devKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0\\Addins") == ERROR_SUCCESS)
		{
			// Create the WorkspaceWhiz.DSAddin.1 key.
			if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0\\Addins\\WorkspaceWhiz.DSAddin.1") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetValue("1");
				devKey.SetValue(strDescription, _T("Description"));
				devKey.SetValue(displayName, _T("DisplayName"));

				devKey.SetValue(moduleName, _T("Filename"));
			}
		}
	}

	// Does Embedded Visual C++ 3.0 exist?
	if (devKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\CEStudio\\3.0\\evc") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\CEStudio\\3.0\\evc\\AddIns") == ERROR_SUCCESS)
		{
			// Create the WorkspaceWhiz.DSAddin.1 key.
			if (devKey.Create(HKEY_CURRENT_USER, "Software\\Microsoft\\CEStudio\\3.0\\evc\\AddIns\\WorkspaceWhiz.DSAddin.1") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetValue("1");
				devKey.SetValue(strDescription, _T("Description"));
				devKey.SetValue(displayName, _T("DisplayName"));

				devKey.SetValue(moduleName, _T("Filename"));
			}
		}
	}

	// Add a few Workspace Whiz! defaults.
	CRegKey whizKey;
	bool overwriteTemplates = true;
	if (whizKey.Open(HKEY_CURRENT_USER, "Software\\Workspace Whiz!\\2.00\\Templates") == ERROR_SUCCESS)
	{
		overwriteTemplates = false;
/*
		if (AfxMessageBox("The Workspace Whiz! installation found an already existing list of "
						"active template files.\n\n" 
						"Do you want to overwrite them with the defaults?  (Select yes if "
						"you are unsure.)", MB_YESNO | MB_DEFBUTTON2) == IDNO)
		{
			overwriteTemplates = false;
		}
*/
	}

	if (overwriteTemplates)
		SaveTemplateRegistryDefaults();

	// Erase defunct key bindings.
	CRegKey keyKey;
	if (keyKey.Open(HKEY_CURRENT_USER, "Software\\Workspace Whiz!\\2.00\\Config\\Keys") == ERROR_SUCCESS)
	{
		keyKey.DeleteValue("WWFileOpenAtCursorDlg");
	}

	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Remove entries from DevStudio 6.0.
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WorkspaceWhiz.DSAddin.1");

		if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\6.0\\Keyboard\\Aut") == ERROR_SUCCESS)
		{
			const CommandInfo* commandList = GetCommandList();
			int commandCount = GetCommandCount();
			for (int i = 0; i < commandCount; i++)
			{
				key.DeleteValue(commandList->m_name);
				commandList++;
			}
		}
	}

	// Remove entries from DevStudio 5.0.
	if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Devstudio\\5.0\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WorkspaceWhiz.DSAddin.1");
	}

	// Remove entries from Embedded Visual C++ 3.0.
	if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\CEStudio\\3.0\\evc\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WorkspaceWhiz.DSAddin.1");

		if (key.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\CEStudio\\3.0\\evc\\Keyboard\\Aut") == ERROR_SUCCESS)
		{
			const CommandInfo* commandList = GetCommandList();
			int commandCount = GetCommandCount();
			for (int i = 0; i < commandCount; i++)
			{
				key.DeleteValue(commandList->m_name);
				commandList++;
			}
		}
	}

	// Remove the registry entry.
	bool removeAllSettings = false;
	if (key.Open(HKEY_CURRENT_USER, "Software\\Workspace Whiz!") == ERROR_SUCCESS)
	{
		if (AfxMessageBox("Do you want to remove your custom settings?  (If uninstalling "
				"Workspace Whiz! permanently, select Yes.)", MB_YESNO | MB_DEFBUTTON2) == IDYES)
		{
			removeAllSettings = true;
			key.RecurseDeleteKey("2.00");

			if (key.Open(HKEY_CURRENT_USER, "Software") == ERROR_SUCCESS)
			{
				key.DeleteSubKey("Workspace Whiz!");
			}
		}
	}
	
	if (removeAllSettings  &&
		key.Open(HKEY_LOCAL_MACHINE, "Software\\Workspace Whiz!") == ERROR_SUCCESS)
	{
		key.RecurseDeleteKey("2.00");
		if (key.Open(HKEY_LOCAL_MACHINE, "Software") == ERROR_SUCCESS)
		{
			key.DeleteSubKey("Workspace Whiz!");
		}
	}
	
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
