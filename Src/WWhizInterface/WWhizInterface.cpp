///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizInterface.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WWhizInterface.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 19   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <afx.h>
#include "WWhizInterface2.h"
#include <atlbase.h>
#include <initguid.h>
#include <ObjModel\appguid.h>
#include "ObjModelGUID.h"
#pragma warning (disable : 4355)
#include <afxtempl.h>
#include "AfxTemplateEx.h"
#include "RCBase.h"
#include "WorkspaceInfo.h"
#include "WorkspaceTags.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IApplication* g_pApplication;
WWhizInterface* g_wwhizInterface;
WWhizReg* g_wwhizReg;

pfnGetWorkspaceName g_fnGetWorkspaceName;
HINSTANCE g_wwhizHelperInstance;
HINSTANCE g_wwhizInterfaceInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	g_wwhizInterfaceInstance = hinstDLL;

	return TRUE;
}

extern "C" __declspec(dllexport) WWhizInterface* WWhizInterfaceCreate(IApplication* pApplication)
{
	g_pApplication = pApplication;

	g_wwhizInterface = &WorkspaceInfo::GetWWhizInterface();
	static bool initialized = false;
	if (!initialized)
	{
		WorkspaceTags::Initialize();
		initialized = true;

		g_wwhizHelperInstance = AfxLoadLibrary("WWhizInterfaceHelper.pkg");
		if (g_wwhizHelperInstance)
		{
			g_fnGetWorkspaceName = (pfnGetWorkspaceName)GetProcAddress(g_wwhizHelperInstance, "GetWorkspaceName");
		}
	}
	return g_wwhizInterface;
}


extern "C" __declspec(dllexport) void WWhizInterfaceDestroy()
{
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
