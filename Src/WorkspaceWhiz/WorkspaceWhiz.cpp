///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceWhiz.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/WorkspaceWhiz.cpp $
// $Date: 2003/07/08 $ $Revision: #14 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#ifdef WWHIZ_VC6
#include <initguid.h>
#endif WWHIZ_VC6
#include "ObjModelGUID.h"
#include "resource.h"

#ifdef WWHIZ_VSNET

#include "WWhizNet.h"

CAddInModule _AtlModule;

#endif WWHIZ_VSNET

#ifdef WWHIZ_VC6

#include "Commands.h"
#include "DSAddIn.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif _DEBUG

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_DSAddIn, CDSAddIn)
END_OBJECT_MAP()

#endif WWHIZ_VC6

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceWhizApp

class CWorkspaceWhizApp : public CWinApp
{
public:
	CWorkspaceWhizApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceWhizApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWorkspaceWhizApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};


// The global application.
extern CWorkspaceWhizApp theApp;

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
	free((void*)m_pszProfileName);
#ifdef WWHIZ_VC6
	m_pszRegistryKey = _tcsdup(_T("Workspace Whiz"));
	m_pszProfileName = _strdup("VC6");
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	m_pszRegistryKey = _tcsdup(_T("Workspace Whiz"));
	m_pszProfileName = _strdup(".NET");
#endif WWHIZ_VSNET

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

#ifdef WWHIZ_VC6
	_Module.Init(ObjectMap, m_hInstance);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	_AtlModule.SetResourceInstance(m_hInstance);
	_AtlModule.DllMain(DLL_PROCESS_ATTACH, NULL);
#endif WWHIZ_VSNET

	return CWinApp::InitInstance();
}

int CWorkspaceWhizApp::ExitInstance()
{
#ifdef WWHIZ_VC6
	_Module.Term();
#endif WWHIZ_VC6
	return CWinApp::ExitInstance();
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

	LONG filenameFileSize = (LONG)filenameFile.GetLength();
	BYTE* filenameFileMem = filenameFile.Detach();
	AfxGetApp()->WriteProfileBinary("Templates", "FileList",
		filenameFileMem, filenameFileSize);
	free(filenameFileMem);
}

#ifdef WWHIZ_VSNET
// DLL Entry Point
//extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
//{
//	_AtlModule.SetResourceInstance(hInstance);
//	AfxSetResourceHandle(hInstance);
//	return _AtlModule.DllMain(dwReason, lpReserved); 
//}
#endif WWHIZ_VSNET


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
#ifdef WWHIZ_VC6
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	return _AtlModule.DllCanUnloadNow();
#endif WWHIZ_VSNET
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef WWHIZ_VC6
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _Module.GetClassObject(rclsid, riid, ppv);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
#endif WWHIZ_VSNET
}

static CString s_regWWhizName = "Workspace Whiz";
static CString s_regWWhizBasePath = "Software\\" + s_regWWhizName;

#ifdef WWHIZ_VC6
static CString s_regBasePath = "Software\\Microsoft\\DevStudio\\6.0";
static CString s_regWhichWWhizPath = "\\VC6";
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
static CString s_regBasePath = "Software\\Microsoft\\VisualStudio";
static CString s_regWhichWWhizPath = "\\.NET";
#endif WWHIZ_VSNET

static CString s_regWWhizPath = s_regWWhizBasePath + s_regWhichWWhizPath;

#ifdef WWHIZ_VSNET

extern void UnregisterAllCommands(CComPtr<EnvDTE::_DTE>& pDTE);

void ConnectAndUnregisterAllCommands()
{
	// Create the shell and get an interface to it.
	CComPtr<EnvDTE::_DTE> pDTE;
	HRESULT hr = CoCreateInstance(__uuidof(EnvDTE::DTE), NULL, CLSCTX_LOCAL_SERVER,
		__uuidof(EnvDTE::_DTE), reinterpret_cast<void**>(&pDTE));
	if (pDTE)
	{
		UnregisterAllCommands(pDTE);

		CComPtr<Office::_CommandBars> pCommandBars;
		CComPtr<Office::CommandBar> pCommandBar;

		// Get the set of command bars for the application.
		pDTE->get_CommandBars(&pCommandBars);
		if (pCommandBars)
		{
			// See if the Workspace Whiz toolbar has been created.
			hr = pCommandBars->get_Item(CComVariant(L"Workspace Whiz"), &pCommandBar);
			if (SUCCEEDED(hr))
			{
				hr = pCommandBar->Delete();
				int hi = 5;
			}
		}
	}
}

#endif WWHIZ_VSNET

// by exporting DllRegisterServer, you can use regsvr32.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef WWHIZ_VC6
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

				lRes = 0;

				displayName = szDesc;
			}

			key.SetKeyValue(_T("Description"), strDescription);
		}
		CoTaskMemFree(lpOleStr);
	}
	if (lRes != ERROR_SUCCESS)
		hRes = HRESULT_FROM_WIN32(lRes);
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
//	ConnectAndUnregisterAllCommands();

	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
#endif WWHIZ_VSNET

	// Get the module name.
	TCHAR moduleName[_MAX_PATH];
	moduleName[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&moduleName, _MAX_PATH);

	// Get the module path.
	TCHAR modulePath[_MAX_PATH];
	_tcscpy(modulePath, moduleName);
	TCHAR* ptr = _tcsrchr(modulePath, '\\');
	ptr++;
	*ptr++ = 0;

	// Get the short module name.
	TCHAR moduleShortName[_MAX_PATH];
	ptr = _tcsrchr(moduleName, '\\');
	_tcscpy(moduleShortName, ptr + 1);

#ifdef WWHIZ_VC6
	// Does DevStudio 6.0 exist?
	CRegKey devKey;
	if (devKey.Open(HKEY_CURRENT_USER, s_regBasePath) == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_CURRENT_USER, s_regBasePath + "\\Addins") == ERROR_SUCCESS)
		{
			// Create the WorkspaceWhiz.DSAddin.1 key.
			if (devKey.Create(HKEY_CURRENT_USER, s_regBasePath + "\\Addins\\WorkspaceWhiz.DSAddin.1") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetValue("1");
				devKey.SetValue(strDescription, _T("Description"));
				devKey.SetValue(displayName, _T("DisplayName"));

				devKey.SetValue(moduleName, _T("Filename"));
			}
		}
	}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	// Register the add-in?
	CRegKey devKey;
	if (devKey.Open(HKEY_LOCAL_MACHINE, s_regBasePath + "\\7.0") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_LOCAL_MACHINE, s_regBasePath + "\\7.0\\AddIns") == ERROR_SUCCESS)
		{
			// Create the WorkspaceWhiz.DSAddin.1 key.
			if (devKey.Create(HKEY_LOCAL_MACHINE, s_regBasePath + "\\7.0\\AddIns\\WWhizNet.Connect") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetStringValue("SatelliteDLLPath", modulePath);
				devKey.SetStringValue("SatelliteDLLName", moduleShortName);
				devKey.SetDWORDValue("LoadBehavior", 3);
				devKey.SetStringValue("FriendlyName", "Workspace Whiz - A Visual Studio .NET Add-in");
				devKey.SetStringValue("Description", "Adds useful code navigation and completion capabilities to Visual Studio");
				devKey.SetDWORDValue("CommandPreload", 1);
			}
		}
	}

	if (devKey.Open(HKEY_CURRENT_USER, s_regBasePath + "\\7.0\\PreloadAddinState") == ERROR_SUCCESS)
	{
		devKey.SetDWORDValue("WWhizNet.Connect", 1);
	}

	if (devKey.Open(HKEY_LOCAL_MACHINE, s_regBasePath + "\\7.1") == ERROR_SUCCESS)
	{
		// Auto create the addins key if it isn't already there.
		if (devKey.Create(HKEY_LOCAL_MACHINE, s_regBasePath + "\\7.1\\AddIns") == ERROR_SUCCESS)
		{
			// Create the WorkspaceWhiz.DSAddin.1 key.
			if (devKey.Create(HKEY_LOCAL_MACHINE, s_regBasePath + "\\7.1\\AddIns\\WWhizNet.Connect") == ERROR_SUCCESS)
			{
				// Remove all old entries.
				devKey.SetStringValue("SatelliteDLLPath", modulePath);
				devKey.SetStringValue("SatelliteDLLName", moduleShortName);
				devKey.SetDWORDValue("LoadBehavior", 3);
				devKey.SetStringValue("FriendlyName", "Workspace Whiz - A Visual Studio .NET Add-in");
				devKey.SetStringValue("Description", "Adds useful code navigation and completion capabilities to Visual Studio");
				devKey.SetDWORDValue("CommandPreload", 1);
			}
		}
	}

	if (devKey.Open(HKEY_CURRENT_USER, s_regBasePath + "\\7.1\\PreloadAddinState") == ERROR_SUCCESS)
	{
		devKey.SetDWORDValue("WWhizNet.Connect", 1);
	}
#endif WWHIZ_VSNET

	// Add a few Workspace Whiz defaults.
	CRegKey whizKey;
	bool overwriteTemplates = true;
	if (whizKey.Open(HKEY_CURRENT_USER, s_regWWhizPath + "\\Templates") == ERROR_SUCCESS)
	{
		overwriteTemplates = false;
/*
		if (AfxMessageBox("The Workspace Whiz installation found an already existing list of "
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

	// Do the Ignore Tokens List
/*	if (whizKey.Open(HKEY_CURRENT_USER, s_regWWhizPath + "\\Config") == ERROR_SUCCESS)
	{
#ifdef WWHIZ_VC6
		DWORD count;
		if (whizKey.QueryValue(NULL, "IgnoreTokensList", &count) == ERROR_SUCCESS)
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
		ULONG count;
		if (whizKey.QueryStringValue("IgnoreTokensList", NULL, &count) == ERROR_SUCCESS)
#endif WWHIZ_VSNET
		{
			if (AfxMessageBox("The Workspace Whiz installation found an already existing list of "
				"tag tokens to ignore.\n\n" 
				"This version of Workspace Whiz contains an enhanced list of those tokens.  Do "
				"you want Workspace Whiz to overwrite your list with the defaults?  (Select yes if "
				"you are unsure.)", MB_YESNO | MB_DEFBUTTON2) == IDYES)
			{
				whizKey.DeleteValue("IgnoreTokensList");
			}
		}
	}
*/
#ifdef WWHIZ_VC6
	// Erase defunct key bindings.
	CRegKey keyKey;
	if (keyKey.Open(HKEY_CURRENT_USER, s_regWWhizPath + "\\Config\\Keys") == ERROR_SUCCESS)
	{
		keyKey.DeleteValue("WWFileOpenAtCursorDlg");
	}

	return hRes;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
    return hr;
#endif WWHIZ_VSNET
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRegKey key;

#ifdef WWHIZ_VC6
	// Remove entries from DevStudio 6.0.
	if (key.Open(HKEY_CURRENT_USER, s_regBasePath + "\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WorkspaceWhiz.DSAddin.1");

		if (key.Open(HKEY_CURRENT_USER, s_regBasePath + "\\Keyboard\\Aut") == ERROR_SUCCESS)
		{
			const WWhizCommands::CommandInfo* commandList = WWhizCommands::GetCommandList();
			int commandCount = WWhizCommands::GetCommandCount();
			for (int i = 0; i < commandCount; i++)
			{
				key.DeleteValue(CString(commandList[i].m_name));
			}
		}
	}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	// Remove entries.
	if (key.Open(HKEY_LOCAL_MACHINE, s_regBasePath + "\\AddIns") == ERROR_SUCCESS)
	{
		// Remove all old entries.
		key.RecurseDeleteKey("WWhizNet.Connect");
	}
#endif WWHIZ_VSNET

	// Remove the registry entry.
	bool removeAllSettings = false;
	if (key.Open(HKEY_CURRENT_USER, s_regWWhizBasePath) == ERROR_SUCCESS)
	{
		if (AfxMessageBox("Do you want to remove your custom settings?  (If uninstalling "
				"Workspace Whiz permanently, select Yes.)", MB_YESNO | MB_DEFBUTTON2) == IDYES)
		{
			removeAllSettings = true;
			key.RecurseDeleteKey(s_regWhichWWhizPath);

/*			if (key.Open(HKEY_CURRENT_USER, "Software") == ERROR_SUCCESS)
			{
				key.DeleteSubKey(s_regWWhizName);
			}*/
		}
	}
	
	if (removeAllSettings  &&
		key.Open(HKEY_LOCAL_MACHINE, s_regWWhizBasePath) == ERROR_SUCCESS)
	{
		key.RecurseDeleteKey(s_regWhichWWhizPath);

		if (key.Open(HKEY_LOCAL_MACHINE, "Software") == ERROR_SUCCESS)
		{
			key.DeleteSubKey(s_regWWhizName);
		}
	}
	
#ifdef WWHIZ_VC6
	// Unregister the server.
	HRESULT hRes = S_OK;
#if _MSC_VER > 1100
	_Module.UnregisterServer(TRUE, NULL);
#else
	_Module.UnregisterServer();
#endif

	return hRes;
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	HRESULT hr = _AtlModule.DllUnregisterServer();

	ConnectAndUnregisterAllCommands();

	return S_OK;
#endif WWHIZ_VSNET
}

