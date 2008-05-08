///////////////////////////////////////////////////////////////////////////////
// $Workfile: pchWWhizInterface.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/pchWWhizInterface.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 19   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning (disable : 4786)

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
//extern CComModule _Module;
//#include <atlcom.h>

// Developer Studio Object Model
#include <ObjModel\appauto.h>
extern IApplication* g_pApplication;
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

int CStringFind(const CString& str, LPCTSTR lpszSub, int nStart);
int CStringFind(const CString& str, TCHAR ch, int nStart);

template<> inline void AFXAPI ConstructElementsEx<CString> (CString* pElements, int nCount)
{
	for (; nCount--; ++pElements)
		memcpy(pElements, &afxEmptyString, sizeof(*pElements));
}

template<> inline void AFXAPI DestructElementsEx<CString> (CString* pElements, int nCount)
{
	for (; nCount--; ++pElements)
		pElements->~CString();
}

template<> inline void AFXAPI CopyElementsEx<CString> (CString* pDest, const CString* pSrc, int nCount)
{
	for (; nCount--; ++pDest, ++pSrc)
		*pDest = *pSrc;
}

template<> inline UINT AFXAPI HashKey<CString> (CString _key)
{
	LPCSTR key = _key;
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

template<> inline UINT AFXAPI HashKey<const CString&> (const CString& _key)
{
	LPCSTR key = _key;
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif _countof

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef bool (*pfnGetWorkspaceName)(LPTSTR buffer, int bufSize);

#include "RCBase.h"
#include "WWhizInterface2.h"
#include "WWhizReg.h"
#include "Auto.h"

extern WWhizInterface* g_wwhizInterface;
extern WWhizReg* g_wwhizReg;
extern HINSTANCE g_wwhizInterfaceInstance;

//#include "AggressiveOptimize.h"
