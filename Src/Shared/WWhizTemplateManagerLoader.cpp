///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizTemplateManagerLoader.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizTemplateManagerLoader.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 4    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "WWhizTemplateManagerLoader.h"
#include "WWhizTemplateManager.h"

typedef WWhizTemplateManager* (*pfnWWhizTemplateManagerCreate)(IApplication* pApplication);

static HINSTANCE s_wwhizTemplateManagerInst;
static pfnWWhizTemplateManagerCreate s_wwhizTemplateManagerCreate;
static WWhizTemplateManager* s_wwhizTemplateManager;

WWhizTemplateManager* __cdecl WWhizTemplateManagerCreate(
		IApplication* pApplication, WWhizTemplateDialog* templateDialog)
{
	// Only allow one interface per instance.
	if (s_wwhizTemplateManager)
		return s_wwhizTemplateManager;

#ifdef _DEBUG
	LPCTSTR filename = "WWhizTemplateManagerD.mod";
#else
	LPCTSTR filename = "WWhizTemplateManager.mod";
#endif //_DEBUG
	s_wwhizTemplateManagerInst = LoadLibrary(filename);
	if (!s_wwhizTemplateManagerInst)
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
		s_wwhizTemplateManagerInst = LoadLibrary(moduleName);
	}
	if (!s_wwhizTemplateManagerInst)
		return NULL;

	s_wwhizTemplateManagerCreate = (pfnWWhizTemplateManagerCreate)
			GetProcAddress(s_wwhizTemplateManagerInst, "WWhizTemplateManagerCreate");
	if (!s_wwhizTemplateManagerCreate)
	{
		WWhizTemplateManagerDestroy();
		return NULL;
	}

	s_wwhizTemplateManager = (*s_wwhizTemplateManagerCreate)(pApplication);
	s_wwhizTemplateManager->SetTemplateDialog(templateDialog);

	return s_wwhizTemplateManager;
}


void __cdecl WWhizTemplateManagerDestroy()
{
	if (s_wwhizTemplateManagerInst)
	{
		BOOL ret = ::FreeLibrary(s_wwhizTemplateManagerInst);
		s_wwhizTemplateManagerInst = NULL;
		s_wwhizTemplateManagerCreate = NULL;
		s_wwhizTemplateManager = NULL;
	}
}
