///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizRegLoader.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizRegLoader.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 6    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "WWhizRegLoader.h"
#include "WWhizReg.h"

struct WWhizReg;

static HINSTANCE s_wwhizRegInst;
static WWhizReg* (*s_wwhizRegCreate)(IApplication*, WWhizInterface*);
static void (*s_wwhizRegDestroy)(WWhizReg*);
static WWhizReg* s_wwhizReg;

WWhizReg* __cdecl WWhizRegCreate(
		IApplication* pApplication, WWhizInterface* wwhizInterface )
{
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
		::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&moduleName, _MAX_PATH);
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

	s_wwhizRegCreate = (WWhizReg* (*)(IApplication*, WWhizInterface*))
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

	s_wwhizReg = (*s_wwhizRegCreate)(pApplication, wwhizInterface);

	return s_wwhizReg;
}


void __cdecl WWhizRegDestroy()
{
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
