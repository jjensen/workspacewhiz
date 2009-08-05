///////////////////////////////////////////////////////////////////////////////
// $Workfile: StdAfx.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/StdAfx.h $
// $Date: 2003/01/05 $ $Revision: #9 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#define USE_SQLITE 1

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
//#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
//#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#include <afx.h>
#include <afxdisp.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxtempl.h>
#include <atlbase.h>
#include "WCollection.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// WWHIZ_VC6
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WWHIZ_VC6

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

// Developer Studio Object Model
#include <ObjModel\addauto.h>
#include <ObjModel\dbgdefs.h>
#include <ObjModel\dbgauto.h>
#include "ObjModelHelper.h"

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

#include "AddInComm.h"

#endif WWHIZ_VC6

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// WWHIZ_VSNET
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WWHIZ_VSNET

#include <atlcom.h>

//#define IfFailGo(x) { hr=(x); if (FAILED(hr)) goto Error; }
//#define IfFailGoCheck(x, p) { hr=(x); if (FAILED(hr)) goto Error; if(!p) {hr = E_FAIL; goto Error; } }

class DECLSPEC_UUID("8FB04AFE-EB33-4C47-9E1C-D90C6273DEE1") WWhizNetLib;

using namespace ATL;

class CAddInModule : public CAtlDllModuleT< CAddInModule >
{
public:
	CAddInModule()
	{
		m_hInstance = NULL;
	}

	DECLARE_LIBID(__uuidof(WWhizNetLib))

	inline HINSTANCE GetResourceInstance()
	{
		return m_hInstance;
	}

	inline void SetResourceInstance(HINSTANCE hInstance)
	{
		m_hInstance = hInstance;
	}

private:
	HINSTANCE m_hInstance;
};

extern CAddInModule _AtlModule;

#endif WWHIZ_VSNET


// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif _countof

extern void LOG(LPCTSTR msg, ...);

#include "ObjModelHelper.h"
#include "Auto.h"
#include "cdxCDynamicDialog.h"
#include "TreeCtrlEx.h"
#include "MemFile.h"
#include "Config.h"
#include "HtmlHelpDialog.h"
#include "WWhizInterface3.h"
#include "WWhizReg.h"
#include "WWhizTemplateManager.h"
#include "WWhizCommands.h"

extern void PrintStatusBar(LPCTSTR message);

#define WM_DOTOOLBAR             (WM_APP + 1)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
