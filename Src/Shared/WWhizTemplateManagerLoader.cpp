///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizTemplateManagerLoader.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizTemplateManagerLoader.cpp $
// $Date: 2003/01/05 $ $Revision: #8 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include <afx.h>
#include <afxwin.h>
#include "WWhizInterfaceLoader.h"
#include "WWhizTemplateManagerLoader.h"
#include "WWhizTemplateManager.h"

typedef WWhizTemplateManager* (*pfnWWhizTemplateManagerCreate)(MyApplicationType application);
typedef void (*pfnWWhizTemplateManagerDestroy)();

static HINSTANCE s_wwhizTemplateManagerInst;
static pfnWWhizTemplateManagerCreate s_wwhizTemplateManagerCreate;
static pfnWWhizTemplateManagerDestroy s_wwhizTemplateManagerDestroy;
static WWhizTemplateManager* s_wwhizTemplateManager;

WWhizTemplateManager* __cdecl WWhizTemplateManagerCreate(
		MyApplicationType application, WWhizTemplateDialog* templateDialog)
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
		TCHAR modulePath[_MAX_PATH];
		TCHAR moduleName[_MAX_PATH];
		modulePath[0] = 0;
		::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&modulePath, _MAX_PATH);
		TCHAR* ptr = _tcsrchr(modulePath, '\\');
		if (ptr)
		{
			ptr++;
			*ptr = 0;
		}

		_tcscpy(moduleName, modulePath);
		_tcscat(moduleName, filename);

		TCHAR currentDir[_MAX_PATH];
		GetCurrentDirectory(_MAX_PATH, currentDir);
		SetCurrentDirectory(modulePath);
		s_wwhizTemplateManagerInst = LoadLibrary(moduleName);
		SetCurrentDirectory(currentDir);
	}
	if (!s_wwhizTemplateManagerInst)
		return NULL;

	s_wwhizTemplateManagerDestroy = (pfnWWhizTemplateManagerDestroy)
			GetProcAddress(s_wwhizTemplateManagerInst, "WWhizTemplateManagerDestroy");
	s_wwhizTemplateManagerCreate = (pfnWWhizTemplateManagerCreate)
			GetProcAddress(s_wwhizTemplateManagerInst, "WWhizTemplateManagerCreate");
	if (!s_wwhizTemplateManagerCreate)
	{
		WWhizTemplateManagerDestroy();
		return NULL;
	}

	s_wwhizTemplateManager = (*s_wwhizTemplateManagerCreate)(application);
	s_wwhizTemplateManager->SetTemplateDialog(templateDialog);

	return s_wwhizTemplateManager;
}


void __cdecl WWhizTemplateManagerDestroy()
{
	if (s_wwhizTemplateManagerInst)
	{
		(*s_wwhizTemplateManagerDestroy)();
		BOOL ret = ::FreeLibrary(s_wwhizTemplateManagerInst);
		s_wwhizTemplateManagerInst = NULL;
		s_wwhizTemplateManagerCreate = NULL;
		s_wwhizTemplateManager = NULL;
	}
}
