///////////////////////////////////////////////////////////////////////////////
// $Workfile: pchWWhizTemplates.h $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplates/pchWWhizTemplates.h $
// $Date:: 1/03/01 12:14a  $ $Revision:: 6    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <afxwin.h>         // MFC core and standard components
#include <afxdisp.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxtempl.h>
#include <mmsystem.h>
#include "AfxTemplateEx.h"

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

// Developer Studio Object Model
//#include <ObjModel\addauto.h>
//#include <ObjModel\appdefs.h>
#include <ObjModel\appauto.h>
#include <ObjModel\blddefs.h>
#include <ObjModel\bldauto.h>
//#include <ObjModel\textdefs.h>
//#include <ObjModel\textauto.h>
//#include <ObjModel\dbgdefs.h>
//#include <ObjModel\dbgauto.h>
extern IApplication* g_pApplication;
#include "ObjModelHelper.h"

#include "WWhizTemplateManager.h"

extern WWhizTemplateManager* g_templateManager;

/////////////////////////////////////////////////////////////////////////////
// Debugging support

// Use VERIFY_OK around all calls to the Developer Studio objects which
//  you expect to return S_OK.
// In DEBUG builds of your add-in, VERIFY_OK displays an ASSERT dialog box
//  if the expression returns an HRESULT other than S_OK.  If the HRESULT
//  is a success code, the ASSERT box will display that HRESULT.  If it
//  is a failure code, the ASSERT box will display that HRESULT plus the
//  error description string provided by the object which raised the error.
// In RETAIL builds of your add-in, VERIFY_OK just evaluates the expression
//  and ignores the returned HRESULT.

#ifdef _DEBUG

void GetLastErrorDescription(CComBSTR& bstr);		// Defined in WorkspaceWhiz.cpp
#define VERIFY_OK(f) \
	{ \
		HRESULT hr = (f); \
		if (hr != S_OK) \
		{ \
			if (FAILED(hr)) \
			{ \
				CComBSTR bstr; \
				GetLastErrorDescription(bstr); \
				_RPTF2(_CRT_ASSERT, "Object call returned %lx\n\n%S", hr, (BSTR) bstr); \
			} \
			else \
				_RPTF1(_CRT_ASSERT, "Object call returned %lx", hr); \
		} \
	}

#else //_DEBUG

#define VERIFY_OK(f) (f);

#endif //_DEBUG

#include "AggressiveOptimize.h"
