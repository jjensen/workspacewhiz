///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizRegLoader.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizRegLoader.cpp $
// $Date: 2003/01/05 $ $Revision: #8 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "WWhizInterfaceLoader.h"
#include "WWhizRegLoader.h"
#include "WWhizReg.h"

struct WWhizReg;

static HINSTANCE s_wwhizRegInst;
static WWhizReg* (*s_wwhizRegCreate)(MyApplicationType, WWhizInterface*);
static void (*s_wwhizRegDestroy)(WWhizReg*);
static WWhizReg* s_wwhizReg;

WWhizReg* __cdecl WWhizRegCreate(
		MyApplicationType application, HINSTANCE hInstance, WWhizInterface* wwhizInterface )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Only allow one interface per instance.
	if (s_wwhizReg)
		return s_wwhizReg;

#ifdef _DEBUG
	LPCTSTR filename = "WWhizRegD.mod";
#else
	LPCTSTR filename = "WWhizReg.mod";
#endif //_DEBUG
	s_wwhizRegInst = LoadLibrary(filename);
	if (!s_wwhizRegInst)
	{
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
		s_wwhizRegInst = LoadLibrary(moduleName);
	}
	if (!s_wwhizRegInst)
		return NULL;

	s_wwhizRegCreate = (WWhizReg* (*)(MyApplicationType, WWhizInterface*))
			GetProcAddress(s_wwhizRegInst, "WWhizRegCreate");
	if (!s_wwhizRegCreate)
	{
		WWhizRegDestroy();
		return NULL;
	}

	s_wwhizRegDestroy = (void (*)(WWhizReg*))GetProcAddress(s_wwhizRegInst, "WWhizRegDestroy");
	if (!s_wwhizRegDestroy)
	{
		WWhizRegDestroy();
		return NULL;
	}

	s_wwhizReg = (*s_wwhizRegCreate)(application, wwhizInterface);

	return s_wwhizReg;
}


void __cdecl WWhizRegDestroy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (s_wwhizRegInst)
	{
		if (s_wwhizRegDestroy)
			(*s_wwhizRegDestroy)(s_wwhizReg);
		BOOL ret = ::FreeLibrary(s_wwhizRegInst);
		s_wwhizRegInst = NULL;
		s_wwhizRegCreate = NULL;
		s_wwhizRegDestroy = NULL;
		s_wwhizReg = NULL;
	}
}

