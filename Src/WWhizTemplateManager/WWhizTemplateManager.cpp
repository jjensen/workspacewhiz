///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizTemplateManager.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/WWhizTemplateManager.cpp $
// $Date:: 1/03/01 12:14a  $ $Revision:: 4    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <afx.h>
#include "WWhizTemplateManager.h"
#include <atlbase.h>
#include <initguid.h>
#include "ObjModelGUID.h"
//#include <ObjModel\addguid.h>
//#include <ObjModel\appguid.h>
//#include <ObjModel\bldguid.h>
//#include <ObjModel\textguid.h>
//#include <ObjModel\dbgguid.h>
#pragma warning (disable : 4355)
#include <afxtempl.h>
#include "AfxTemplateEx.h"
#include <process.h>
#include "TemplateManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IApplication* g_pApplication;
extern TemplateManager g_templateManager;
HINSTANCE g_instance;
CString g_modulePath;

extern "C" __declspec(dllexport)
WWhizTemplateManager* WWhizTemplateManagerCreate(IApplication* pApplication)
{
	g_pApplication = pApplication;

	g_templateManager.Create();

	return &g_templateManager;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Get the add-in directory.
		TCHAR modulePath[_MAX_PATH];
		modulePath[0] = 0;
		::GetModuleFileName(hinstDLL, (TCHAR*)&modulePath, _MAX_PATH);
		TCHAR* ptr = _tcsrchr(modulePath, '\\');
		if (ptr)
		{
			ptr++;
			*ptr = 0;
		}

		g_modulePath = modulePath;
	}

	return TRUE;
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
