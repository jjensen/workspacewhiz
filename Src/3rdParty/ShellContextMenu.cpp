/***************************************************************************/
/* NOTE:                                                                   */
/* This document is copyright (c) by Oz Solomonovich.  All non-commercial  */
/* use is allowed, as long as this document not altered in any way, and    */
/* due credit is given.                                                    */
/***************************************************************************/

// ShellContextMenu.cpp : implementation file
//
// Handles the creation of the shell context menu, including the "Send To..."
// sub-menu.  
// Personal note: I think that MS should have made the code to populate and
// handle the "Send To..." sub-menu a part of the shell context menu.  But
// instead they forced us poor saps to write a whole lot of spaghetti COM 
// code to do what should have been a trivial part of the OS.  See the code 
// below and judge for yourself.
//

#include "BCMenu.h"
#include "ShellContextMenu.h"
#include "ShellTools.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDM_SHELLCTXFIRST      1
#define IDM_SHELLCTXLAST       29999
#define IDM_SENDTOFIRST        30000
#define IDM_SENDTOLAST         32767

static class CShCMInitializer
{
public:
    CShCMInitializer();
    ~CShCMInitializer();

    static LPSHELLFOLDER   m_sfDesktop;
    static LPSHELLFOLDER   m_sfSendTo;
    static LPITEMIDLIST    m_pidlSendTo;
} __init;

LPSHELLFOLDER CShCMInitializer::m_sfDesktop  = NULL;
LPSHELLFOLDER CShCMInitializer::m_sfSendTo   = NULL;
LPITEMIDLIST  CShCMInitializer::m_pidlSendTo = NULL;

CShCMInitializer::CShCMInitializer()
{
    HRESULT hr;

    SHGetDesktopFolder(&m_sfDesktop);

    hr = SHGetSpecialFolderLocation(NULL, CSIDL_SENDTO, &m_pidlSendTo);
    if (SUCCEEDED(hr)) 
    {
        hr = m_sfDesktop->BindToObject(m_pidlSendTo, NULL, IID_IShellFolder, 
            (LPVOID *)&m_sfSendTo);
        if (!SUCCEEDED(hr)) 
        {
            m_sfSendTo = NULL;
        }
    } 
    else 
    {
        m_sfSendTo = NULL;
    }
}

CShCMInitializer::~CShCMInitializer()
{
    m_sfSendTo->Release();
    m_sfDesktop->Release();
    FreePIDL(m_pidlSendTo);
}



CShellContextMenu::CShellContextMenu(HWND hWnd, CString m_cFullPath) : 
    m_hWnd(hWnd), m_cFullPath(m_cFullPath), m_pSendToMenu(NULL)
{
    m_lpcm = NULL;
}

CShellContextMenu::~CShellContextMenu()
{
    if (m_lpcm) m_lpcm->Release();

    if (m_pSendToMenu)
    {
        BCMenuData *pMenuData;
        int i = IDM_SENDTOFIRST;

        pMenuData = m_pSendToMenu->FindMenuListEx(i);
        while (pMenuData)
        {
            FreePIDL((LPITEMIDLIST)pMenuData->pContext);
            pMenuData = m_pSendToMenu->FindMenuListEx(++i);
        }
        delete m_pSendToMenu;
    }
}

bool CShellContextMenu::IsMenuCommand(int iCmd) const
{
    return (
            (IDM_SENDTOFIRST   <= iCmd  &&  iCmd <= IDM_SENDTOLAST) ||
            (IDM_SHELLCTXFIRST <= iCmd  &&  iCmd <= IDM_SHELLCTXLAST)
           );
}

void CShellContextMenu::InvokeCommand(int iCmd) const
{
    if (iCmd)
    {
        if (IDM_SENDTOFIRST <= iCmd  &&  iCmd <= IDM_SENDTOLAST)
        {
            // "Send To..." item

            LPITEMIDLIST pidlDrop, pidlFile;
            LPDROPTARGET pDT;
            LPDATAOBJECT pDO;
            HRESULT hr;

            GetPIDL(m_cFullPath, pidlFile);
            hr = GetUIObjectOfAbsPIDL(m_hWnd, pidlFile, IID_IDataObject,
                (LPVOID *)&pDO);
            if (SUCCEEDED(hr))
            {
                pidlDrop = (LPITEMIDLIST)
                    (m_pSendToMenu->FindMenuListEx(iCmd))->pContext;
                hr = GetUIObjectOfAbsPIDL(m_hWnd, pidlDrop, IID_IDropTarget,
                    (LPVOID *)&pDT);

                if (SUCCEEDED(hr))
                {
                    // do the drop
                    POINTL pt = { 0, 0 };
                    DWORD dwEffect = 
                        DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
                    hr = pDT->DragEnter(pDO, MK_LBUTTON, pt, &dwEffect);

                    if (SUCCEEDED(hr) && dwEffect) 
                    {
                        hr = pDT->Drop(pDO, MK_LBUTTON, pt, &dwEffect);
                    } 
                    else 
                    {
                        hr = pDT->DragLeave();
                    }

                    pDT->Release();
                }
            }
            
            FreePIDL(pidlFile);
        }
        else
        {
            // Shell command

            CMINVOKECOMMANDINFO cmi;
            cmi.cbSize       = sizeof(cmi);
            cmi.fMask        = 0;
            cmi.hwnd         = m_hWnd;
            cmi.lpVerb       = MAKEINTRESOURCE(iCmd - 1);
            cmi.lpParameters = NULL;
            cmi.lpDirectory  = NULL;
            cmi.nShow        = SW_SHOWNORMAL;
            cmi.dwHotKey     = 0;
            cmi.hIcon        = NULL;
            m_lpcm->InvokeCommand(&cmi);
        }
    }
}


// retrieves the shell context menu for a file
void CShellContextMenu::SetMenu(BCMenu *pMenu, BCMenu *pMenu2)
{
    HRESULT         hr;
    LPITEMIDLIST    pidl;

    m_lpcm = NULL;

    if (m_cFullPath.GetLength() == 0)
    {
        return;
    }

    GetPIDL(m_cFullPath, pidl);

	if (pidl == NULL)
		return;

    hr = GetUIObjectOfAbsPIDL(m_hWnd, pidl, IID_IContextMenu, 
        (LPVOID *)&m_lpcm);

    if (SUCCEEDED(hr))
    {
        pMenu->DeleteMenu(0, MF_BYPOSITION);
        hr = m_lpcm->QueryContextMenu(*pMenu, 0, IDM_SHELLCTXFIRST, 
            IDM_SHELLCTXLAST, CMF_NODEFAULT | CMF_EXPLORE);

        // find the "Send To" submenu: look for a one item submenu with
        // the same name as it's parent menu
        CMenu *pSubMenu;
        int count = pMenu->GetMenuItemCount();
        CString str1, str2;
        for (int i = 0; i < count; i++)
        {
            pSubMenu = pMenu->GetSubMenu(i);
            if (pSubMenu  &&  pSubMenu->GetMenuItemCount() == 1)
            {
                pMenu->GetMenuString(i, str1, MF_BYPOSITION);
                pSubMenu->GetMenuString(0, str2, MF_BYPOSITION);
                UINT idm = IDM_SENDTOFIRST;
                if (str1 == str2)
                {
                    // ok - found it.  now populate it
                    m_pSendToMenu = WNEW BCMenu();
                    m_pSendToMenu->CreateMenu();
                    pMenu->ModifyMenu(i, MF_BYPOSITION | MF_POPUP, 
                        (UINT)m_pSendToMenu->GetSafeHmenu(), str1);
                    FillSendToMenu(m_pSendToMenu, 
                        CShCMInitializer::m_sfSendTo, idm);
                    pSubMenu->DestroyMenu();
                    break;
                }
            }
        }
    }

    FreePIDL(pidl);

    return;
}


void CShellContextMenu::FillSendToMenu(BCMenu *pMenu, 
    LPSHELLFOLDER pSF, UINT &idm)
{
    USES_CONVERSION;
    LPITEMIDLIST pidl, pidl2;
    LPENUMIDLIST peidl;
    HRESULT hr;
    STRRET str;
    HICON hIcon;
    UINT idmStart = idm;
    BCMenu *pSubMenu;
    LPSHELLFOLDER pSubSF;

    if (pSF) 
    {
        pMenu->m_bDynIcons = TRUE;
        for (int i = 0; i < 2; i++)
        {
            // i == 0  -->  folders
            // i == 1  -->  other files

            hr = pSF->EnumObjects(m_hWnd, 
                SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &peidl);

            if (!SUCCEEDED(hr)) 
                continue;

            while (peidl->Next(1, &pidl, NULL) == S_OK &&
                  idm < IDM_SENDTOLAST) 
            {
                hr = pSF->GetDisplayNameOf(pidl, SHGDN_NORMAL, &str);
                if (SUCCEEDED(hr)) 
                {
                    ULONG ulAttrs = (unsigned)-1;
                    pSF->GetAttributesOf(1, 
                        (const struct _ITEMIDLIST **)&pidl, &ulAttrs);
                    pidl2 = GetFullyQualPidl(pSF, pidl);
                    hIcon = GetFileIcon((LPCTSTR)pidl2, SHGFI_PIDL);
                    STRRET2Str(&str, pidl);
                    if (ulAttrs & SFGAO_FOLDER) // folder?
                    {
                        if (i == 0)
                        {
                            // create new submenu & recurse
                            pSubMenu = WNEW BCMenu();
                            pSubMenu->CreateMenu();
                            pMenu->AppendODMenuA(str.cStr, 
                                MF_ENABLED | MF_OWNERDRAW | MF_POPUP, 
                                pSubMenu, (int)hIcon);
                            hr = pSF->BindToObject(pidl, NULL, 
                                IID_IShellFolder, (LPVOID *)&pSubSF);
                            if (!SUCCEEDED(hr)) pSubSF = NULL;
                            FillSendToMenu(pSubMenu, pSubSF, idm);
                            if (pSubSF) pSubSF->Release();
                            FreePIDL(pidl2);
                        }
                    }
                    else
                    {
                        if (i == 1)
                        {
                            pMenu->AppendODMenuA(str.cStr, 
                                MF_ENABLED | MF_OWNERDRAW, idm++, 
                                (int)hIcon);
                            pMenu->FindMenuOption(
                                A2W((const char *)&str.cStr))->pContext = 
                                (void *)pidl2;
                        }
                    }
                    FreePIDL(pidl);
                }
            }
            peidl->Release();
        }
    }

    // If the menu is still empty (the user has an empty SendTo folder),
    // then add a disabled "(empty)" item so we have at least something
    // to display.
    if (idm == idmStart) 
    {
        pMenu->AppendMenu(MF_GRAYED | MF_DISABLED | MF_STRING, idm, 
            "(empty)");
    }
}
