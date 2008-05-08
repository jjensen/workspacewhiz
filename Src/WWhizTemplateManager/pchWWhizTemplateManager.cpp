///////////////////////////////////////////////////////////////////////////////
// $Workfile: pchWWhizTemplateManager.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/pchWWhizTemplateManager.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 3    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizTemplateManager.h"

#include "atlimpl.cpp"

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

/**
	Stolen from the MFC 6.0 source.  See CString::Find().
**/
int CStringFind(const CString& str, LPCTSTR lpszSub, int nStart)
{
	ASSERT(AfxIsValidString(lpszSub));

	int nLength = str.GetLength();
	if (nStart > nLength)
		return -1;

	// find first matching substring
	LPTSTR lpsz = _tcsstr((LPCTSTR)str + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)str);
}


/**
	Stolen from the MFC 6.0 source.  See CString::Find().
**/
int CStringFind(const CString& str, TCHAR ch, int nStart)
{
	int nLength = str.GetLength();
	if (nStart >= nLength)
		return -1;

	// find first single character
	LPTSTR lpsz = _tcschr((LPCTSTR)str + nStart, (_TUCHAR)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)str);
}


