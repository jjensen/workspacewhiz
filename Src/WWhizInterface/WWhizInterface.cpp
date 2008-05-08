///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizInterface.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WWhizInterface.cpp $
// $Date: 2003/01/05 $ $Revision: #12 $ $Author: Joshua $
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
#include <atlbase.h>
#include <initguid.h>
#ifdef WWHIZ_VC6
#include <ObjModel\appguid.h>
#include "ObjModelGUID.h"
#endif WWHIZ_VC6
#pragma warning (disable : 4355)
#include <afxtempl.h>
#include "RCBase.h"
#include "WorkspaceInfo.h"
#include "WorkspaceTags.h"
#include <process.h>

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef WWHIZ_VSNET
#pragma warning( disable : 4278 )
#pragma warning( disable : 4146 )
	//The following #import imports MSO based on it's LIBID
	#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" version("2.2") lcid("0") raw_interfaces_only named_guids

	//The following #import imports DTE based on it's LIBID
	#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("7.0") lcid("0") raw_interfaces_only named_guids
#pragma warning( default : 4146 )
#pragma warning( default : 4278 )
#endif WWHIZ_VSNET

WWhizInterface* g_wwhizInterface;
WWhizReg* g_wwhizReg;

#ifdef WWHIZ_VC6
pfnGetWorkspaceName g_fnGetWorkspaceName;
#endif WWHIZ_VC6
HINSTANCE g_wwhizHelperInstance;
HINSTANCE g_wwhizInterfaceInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	g_wwhizInterfaceInstance = hinstDLL;

	return TRUE;
}

#ifdef WWHIZ_VC6
extern "C" __declspec(dllexport) WWhizInterface* WWhizInterfaceCreate(IApplication* pApplication)
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
extern "C" __declspec(dllexport) WWhizInterface* WWhizInterfaceCreate(void* pDTE)
#endif WWHIZ_VSNET
{
	try
	{
#ifdef WWHIZ_VC6
		g_pApplication = pApplication;
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
		// Just in case...
		g_pDTE = NULL;
		g_pDTE = (EnvDTE::_DTE*)pDTE;
#endif WWHIZ_VSNET

		g_wwhizInterface = &WorkspaceInfo::GetWWhizInterface();
		static bool initialized = false;
		if (!initialized)
		{
			WorkspaceTags::Initialize();
			initialized = true;

#ifdef WWHIZ_VC6
			g_wwhizHelperInstance = AfxLoadLibrary("WWhizInterfaceHelper.pkg");
			if ( g_wwhizHelperInstance )
			{
				g_fnGetWorkspaceName = (pfnGetWorkspaceName)GetProcAddress(g_wwhizHelperInstance, "GetWorkspaceName");
			}
			else
			{
				AfxMessageBox( "Workspace Whiz is missing the WWhizInterfaceHelper.pkg file.  "
						"This file should be installed in the Program Files\\Microsoft Visual Studio\\Common\\MSDev98\\Bin\\IDE\\ "
						"directory.  If it is not there, Workspace Whiz will not function properly\n\n" 
						"Please rerun the Workspace Whiz installer to solve this problem.\n" );
			}
#endif WWHIZ_VC6
		}
	}
	catch ( ... )
	{
		AfxMessageBox( "During Workspace Whiz startup, an exception was thrown.  This is a serious error.\n" );
		return NULL;
	}

	return g_wwhizInterface;
}


extern "C" __declspec(dllexport) void WWhizInterfaceDestroy()
{
#ifdef WWHIZ_VSNET
	try
	{
		g_pDTE = NULL;
	}
	catch ( ... )
	{
		AfxMessageBox( "An exception occurred during DTE shutdown in WWhizInterface.\n" );
	}
#endif WWHIZ_VSNET
}




