///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizInterface2Loader.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizInterface2Loader.cpp $
// $Date: 2003/05/01 $ $Revision: #5 $ $Author: jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <afx.h>
#include "WWhizInterface3.h"
#include "WWhizInterfaceLoader.h"

#ifdef WWHIZ_VC6
#define USE_WWHIZ_AICLOADER 0
#if USE_WWHIZ_AICLOADER
#include "AICLoader.h"
#include "AddInComm.h"
#endif //USE_WWHIZ_AICLOADER

const char* REG_ENTRY = "WWhizInterface\\VC6";
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
const char* REG_ENTRY = "WWhizInterface\\NET";
#endif WWHIZ_VSNET

class WWhizInterface;

typedef WWhizInterface* (*pfnWWhizCreate)(MyApplicationType application);
typedef void (*pfnWWhizDestroy)();

static HINSTANCE s_wwhizInst;
static pfnWWhizCreate s_wwhizInterfaceCreate;
static pfnWWhizDestroy s_wwhizInterfaceDestroy;
static WWhizInterface* s_wwhizInterface;

// this object will automatically clean up after the loader by calling
// WWhizDestroy() when the program exists.
static struct auto_cleanup { ~auto_cleanup() { WWhizInterfaceDestroy(); } } autoc;

static void GetWWhizDirectory(LPTSTR wwhizPath)
{
    HKEY hKey;
    DWORD type;
	DWORD pathLen = _MAX_PATH;

	wwhizPath[0] = 0;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\" + CString(REG_ENTRY),
			0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
#ifdef _DEBUG
		const char* pathStr = "DebugPath";
#else
		const char* pathStr = "Path";
#endif _DEBUG
        if (::RegQueryValueEx(hKey, pathStr, 0, &type, 
            (LPBYTE)wwhizPath, &pathLen) == ERROR_SUCCESS)
        {
        }

        ::RegCloseKey(hKey);
    }
}


WWhizInterface* __cdecl WWhizInterfaceCreate(HINSTANCE hInstance, MyApplicationType application)
{
	// Only allow one interface per instance.
	if (s_wwhizInterface)
		return s_wwhizInterface;

	TCHAR wwhizPath[_MAX_PATH];
	GetWWhizDirectory(wwhizPath);
	if (wwhizPath[_tcslen(wwhizPath) - 1] != '\\')
		_tcscat(wwhizPath, "\\");

#ifdef _DEBUG
	LPCTSTR filename = "WWhizInterface2D.mod";
#else
	LPCTSTR filename = "WWhizInterface2.mod";
#endif //_DEBUG

	// Get the module name and strip the module filename from it, leaving the
	// module path.
	TCHAR moduleName[_MAX_PATH];
	moduleName[0] = 0;
	::GetModuleFileName(hInstance, (TCHAR*)&moduleName, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(moduleName, '\\');
	if (ptr)
	{
		ptr++;
		*ptr = 0;
	}

	_tcscat(moduleName, _T(filename));
	s_wwhizInst = LoadLibrary(moduleName);
	if (!s_wwhizInst)
	{
		_tcscat(wwhizPath, filename);
		s_wwhizInst = LoadLibrary(wwhizPath);
		if (!s_wwhizInst)
		{
				return NULL;
		}
	}

	s_wwhizInterfaceCreate = (pfnWWhizCreate)GetProcAddress(s_wwhizInst, "WWhizInterfaceCreate");
	s_wwhizInterfaceDestroy = (pfnWWhizDestroy)GetProcAddress(s_wwhizInst, "WWhizInterfaceDestroy");
	if (!s_wwhizInterfaceCreate)
	{
		WWhizInterfaceDestroy();
		return NULL;
	}

	s_wwhizInterface = (*s_wwhizInterfaceCreate)(application);

	return s_wwhizInterface;
}


void __cdecl WWhizInterfaceDestroy()
{
	if (s_wwhizInst)
	{
		if (s_wwhizInterfaceDestroy)
		{
			(*s_wwhizInterfaceDestroy)();
		}
		BOOL ret = ::FreeLibrary(s_wwhizInst);
		s_wwhizInst = NULL;
	}
	s_wwhizInterfaceCreate = NULL;
	s_wwhizInterfaceDestroy = NULL;
	s_wwhizInterface = NULL;
}
