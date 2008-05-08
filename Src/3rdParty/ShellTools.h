/***************************************************************************/
/* NOTE:                                                                   */
/* This document is copyright (c) by Oz Solomonovich.  All non-commercial  */
/* use is allowed, as long as this document not altered in any way, and    */
/* due credit is given.                                                    */
/***************************************************************************/

// ShellTools.h

#ifndef SHELLTOOLS_H
#define SHELLTOOLS_H

#include <shlobj.h>

// PIDL utilities
void GetPIDL(LPCSTR path, LPITEMIDLIST& pidl);
LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);
void FreePIDL(LPITEMIDLIST pidl);
void GetSplitPIDL(LPITEMIDLIST& pidl, LPITEMIDLIST& pidlParent, 
                  LPITEMIDLIST& pidlObject);
HRESULT GetUIObjectOfAbsPIDL(HWND hWnd, LPITEMIDLIST pidl, REFIID riid, 
                             LPVOID *ppvOut);
void STRRET2Str(LPSTRRET pStrRet, LPITEMIDLIST pidl);

// icon extraction
typedef void (*RECEIVERFUNC)(HICON, LPVOID);
HICON GetFileIcon(LPCTSTR pszPath, UINT uFlags = 0, 
                  RECEIVERFUNC pReceiver = NULL, LPVOID pContext = 0);

// open folder
enum OpenMethods { omDefaultAction, omOpen, omExplore, omExploreFrom };

void OpenLocation(CString& cLocation, OpenMethods aMethod);



#endif  // SHELLTOOLS_H
