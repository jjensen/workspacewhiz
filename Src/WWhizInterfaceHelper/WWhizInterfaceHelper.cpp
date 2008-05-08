///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizInterfaceHelper.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterfaceHelper/WWhizInterfaceHelper.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 3    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE WWhizInterfaceHelperDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(WWhizInterfaceHelperDLL, hInstance))
			return 0;

		new CDynLinkLibrary(WWhizInterfaceHelperDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(WWhizInterfaceHelperDLL);
	}
	return 1;
}


CWinApp* g_app;


extern "C" __declspec(dllexport) bool GetWorkspaceName(LPTSTR buffer, int bufSize)
{
	if (!g_app)
		return false;

	POSITION pos = g_app->GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate* dt = g_app->GetNextDocTemplate(pos);
		if (strcmp(dt->GetRuntimeClass()->m_lpszClassName, "CProjectWorkspaceDocTemplate") == 0)
		{
			POSITION docPos = dt->GetFirstDocPosition();
			if (!docPos)
				break;

			CDocument* doc = dt->GetNextDoc(docPos);
			if (!doc)
				break;

			strcpy(buffer, doc->GetPathName());

			return true;
		}
	}

	return false;
}


DWORD _stdcall InitPackage(HANDLE hInstPkg)
{
	g_app = AfxGetApp();

	return TRUE;
}


DWORD _stdcall ExitPackage(HANDLE hInstPkg)
{
	return TRUE;
}

