///////////////////////////////////////////////////////////////////////////////
// $Workfile: StdAfx.h $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowFunctionsAddin/StdAfx.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 2    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_STDAFX_H__250FFC74_8405_4A47_AA31_2AD50B12F470__INCLUDED_)
#define AFX_STDAFX_H__250FFC74_8405_4A47_AA31_2AD50B12F470__INCLUDED_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxdisp.h>
#include <afxcmn.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

// Developer Studio Object Model
#include <ObjModel\addauto.h>
#include <ObjModel\appdefs.h>
#include <ObjModel\appauto.h>
#include <ObjModel\blddefs.h>
#include <ObjModel\bldauto.h>
#include <ObjModel\textdefs.h>
#include <ObjModel\textauto.h>
#include <ObjModel\dbgdefs.h>
#include <ObjModel\dbgauto.h>

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

void GetLastErrorDescription(CComBSTR& bstr);		// Defined in ShowFunctionsAddin.cpp
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

#include "WWhizInterface2.h"

extern WWhizInterface* g_wwhizInterface;
extern IApplication* g_pApplication;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__250FFC74_8405_4A47_AA31_2AD50B12F470__INCLUDED)
