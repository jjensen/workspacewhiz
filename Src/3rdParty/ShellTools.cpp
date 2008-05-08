/***************************************************************************/
/* NOTE:                                                                   */
/* This document is copyright (c) by Oz Solomonovich.  All non-commercial  */
/* use is allowed, as long as this document not altered in any way, and    */
/* due credit is given.                                                    */
/***************************************************************************/

#include "stdafx.h"
#include "ShellTools.h"


/////////////////////////////////////////////////////////////////////////////
// CInitializer 
// private class used for automatic initialization and cleanup of local 
// variables

static class CInitializer
{
public:
    CInitializer();
    ~CInitializer();

    static LPSHELLFOLDER   m_sfDesktop;
    static LPMALLOC        m_pAllocator;
    static HIMAGELIST      m_hShellImageList;
} __init;

LPSHELLFOLDER CInitializer::m_sfDesktop  = NULL;
LPMALLOC      CInitializer::m_pAllocator = NULL;
HIMAGELIST    CInitializer::m_hShellImageList = NULL;

CInitializer::CInitializer()
{
    SHGetDesktopFolder(&m_sfDesktop);
    SHGetMalloc(&m_pAllocator);

    SHFILEINFO  sfi;
    m_hShellImageList = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\", 0, 
        &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
}


CInitializer::~CInitializer()
{
    m_sfDesktop->Release();
    m_pAllocator->Release();
}


                         // == PIDL Utilities == //

void GetPIDL(LPCSTR path, LPITEMIDLIST& pidl)
{
    OLECHAR olePath[MAX_PATH];
    ULONG   chEaten;
    ULONG   dwAttributes;
    CString cDir;

    pidl = NULL;
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, path, -1, olePath, 
        MAX_PATH);
    CInitializer::m_sfDesktop->ParseDisplayName(NULL, NULL, olePath, 
        &chEaten, &pidl, &dwAttributes);
}

// given a path to a name-space object (such as a file or directory),
// this function will return two pidls:
// 1. the pidl of the object's parent
// 2. the pidl of the object, relative to it's parent
// i.e.: given the path: "c:\windows\system\foo", will return:
// pidlParent = pidl to "c:\windows\system"
// pidlObject = pidl to "foo"   (relative to pidlParent)
#define CB_SIZE  (sizeof(piid->cb))

void GetSplitPIDL(LPITEMIDLIST& pidl, LPITEMIDLIST& pidlParent, 
                  LPITEMIDLIST& pidlObject)
{
    int         size;
    SHITEMID    *piid, *piidLast;

    // calculate total size of pidl
    size = 0;
    piid = piidLast = &pidl->mkid;
    while (piid->cb)
    {
        piidLast = piid;
        size += (piid->cb);
        piid =  (SHITEMID *)((LPBYTE)piid + (piid->cb));
    }

    // copy "parent" portion
    // "parent" doesn't include the last SHITEMID, so reduce that from
    // the total size
    size -= piidLast->cb;
    pidlParent = 
        (LPITEMIDLIST)CInitializer::m_pAllocator->Alloc(size + CB_SIZE);
    CopyMemory(pidlParent, pidl, size);
    ZeroMemory((LPBYTE)pidlParent + size, CB_SIZE);

    // copy "object" portion
    size = piidLast->cb;
    //make sure to allocate enough space for a pending zero cb.
    pidlObject = 
        (LPITEMIDLIST)CInitializer::m_pAllocator->Alloc(size + CB_SIZE); 
    ZeroMemory(pidlObject, size);
    CopyMemory(pidlObject, piidLast, size);
    ZeroMemory((LPBYTE)pidlObject + size, CB_SIZE);
}

void FreePIDL(LPITEMIDLIST pidl)
{
    if (!pidl) return;
    CInitializer::m_pAllocator->Free(pidl);
}

LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
   OLECHAR szOleChar[MAX_PATH];
   LPITEMIDLIST  lpifq;
   ULONG ulEaten, ulAttribs;
   HRESULT hr;
   STRRET str;

    lpsf->GetDisplayNameOf(lpi, SHGDN_FORPARSING, &str);
    STRRET2Str(&str, lpi);

    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.cStr, -1,
        (USHORT *)szOleChar, sizeof(szOleChar));
    
    hr = CInitializer::m_sfDesktop->ParseDisplayName(NULL,
        NULL, szOleChar, &ulEaten, &lpifq, &ulAttribs);
    
   if (FAILED(hr))
      return NULL;

   return lpifq;
}

HRESULT GetUIObjectOfAbsPIDL(HWND hWnd, LPITEMIDLIST pidl, REFIID riid, 
    LPVOID *ppvOut)
{
    LPITEMIDLIST    pidlParent, pidlObject;
    LPSHELLFOLDER   pSF = NULL;
    HRESULT         hr;
    
    GetSplitPIDL(pidl, pidlParent, pidlObject);
    hr = CInitializer::m_sfDesktop->BindToObject(pidlParent, NULL, 
        IID_IShellFolder, (LPVOID *)&pSF);

    if (SUCCEEDED(hr))
    {
        hr = pSF->GetUIObjectOf(hWnd, 1, 
            (const struct _ITEMIDLIST **)&pidlObject, riid, 0, ppvOut);
        pSF->Release();
    }

    FreePIDL(pidlParent);
    FreePIDL(pidlObject);

    return hr;
}

// Take a STRRET structure and put the result into the cStr field.
void STRRET2Str(LPSTRRET pStrRet, LPITEMIDLIST pidl)
{
    LPWSTR pOleStr;

    switch (pStrRet->uType) 
    {
        case STRRET_WSTR:
            // The pOleStr points to a WCHAR string that we convert
            // to ANSI.  We need to stash pStrRet->pOleStr into a safe
            // place, because pStrRet->cStr lives in the same place.
            pOleStr = pStrRet->pOleStr;
            WideCharToMultiByte(CP_ACP, 0, pOleStr, -1,
                                pStrRet->cStr, MAX_PATH, NULL, NULL);
            CInitializer::m_pAllocator->Free(pOleStr);
            break;

        case STRRET_OFFSET:
            // The string lives inside the pidl, so copy it out.
            lstrcpyn(pStrRet->cStr, (LPSTR)((LPBYTE)pidl + pStrRet->uOffset), 
                MAX_PATH);
            break;

        case STRRET_CSTR:
            // The string already is in the right place.
            break;

    }
}

                        // == Icon Extraction == //

typedef struct
{
    RECEIVERFUNC pReceiver;
    LPVOID       pContext;
    LPCTSTR      pszPath;
    UINT         uFlags;
} gfi_thread_data, *pgfi_thread_data;

static UINT gfiExecutor(LPVOID pParam)
{
    pgfi_thread_data pData = (pgfi_thread_data)pParam;

    HICON theIcon = GetFileIcon(pData->pszPath, pData->uFlags);

    pData->pReceiver(theIcon, pData->pContext);
    delete pData;

    return 0;
}

// GetFileIcon - returns the icon associated with a namespace object.
HICON GetFileIcon(LPCTSTR pszPath, UINT uFlags, RECEIVERFUNC pReceiver, 
                  LPVOID pContext)
{
    if (pReceiver == NULL)
    {
        SHFILEINFO  sfi;

        SHGetFileInfo(pszPath, 0, &sfi, sizeof(SHFILEINFO),
            uFlags | SHGFI_ATTRIBUTES | SHGFI_DISPLAYNAME | SHGFI_ICON | 
            SHGFI_SMALLICON);

        return ImageList_GetIcon(CInitializer::m_hShellImageList, 
            sfi.iIcon, 0);
    }
    else
    {
        pgfi_thread_data pData = new gfi_thread_data;
        pData->pReceiver = pReceiver;
        pData->pContext  = pContext;
        pData->pszPath   = pszPath;
        pData->uFlags    = uFlags;

        return (HICON)AfxBeginThread(gfiExecutor, pData);
    }
}


                          // == Open Folder == //

void OpenLocation(CString &cLocation, OpenMethods aMethod)
{
    if (aMethod == omDefaultAction)
    {
        ShellExecute(AfxGetMainWnd()->m_hWnd, NULL, cLocation, NULL, NULL, 
            SW_SHOWDEFAULT);
    }
    else
    {
        CString             cExecString = "", cWinDir;
        PROCESS_INFORMATION pi;
        STARTUPINFO         si;

        switch (aMethod)
        {
            case omExplore:
                cExecString += "/e,";
                break;
            
            case omExploreFrom:
                cExecString = "/e,/root,";
                break;
        }
        
        GetStartupInfo(&si);
        GetWindowsDirectory(cWinDir.GetBuffer(256), 256);
        cWinDir.ReleaseBuffer();
        cExecString = cWinDir + "\\Explorer.exe " + cExecString + cLocation;

        CreateProcess(NULL, (char *)(const char *)(cExecString),  NULL,
            NULL, FALSE, 0, NULL, NULL, &si, &pi);
    }
    
}

