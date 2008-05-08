/***************************************************************************/
/* NOTE:                                                                   */
/* This document is copyright (c) by Oz Solomonovich.  All non-commercial  */
/* use is allowed, as long as this document not altered in any way, and    */
/* due credit is given.                                                    */
/***************************************************************************/

// ShellContextMenu.h : header file
//

#if !defined(AFX_SHELLCONTEXTMENU_H__24BAC666_2B03_11D3_B9C1_0000861DFCE7__INCLUDED_)
#define AFX_SHELLCONTEXTMENU_H__24BAC666_2B03_11D3_B9C1_0000861DFCE7__INCLUDED_

#pragma once

class BCMenu;

#include <shlobj.h>

class CShellContextMenu
{
    LPCONTEXTMENU           m_lpcm;
    CString                 m_cFullPath;
    HWND                    m_hWnd;
    BCMenu *                m_pSendToMenu;

public:    
    CShellContextMenu(HWND m_hWnd, CString cFullPath);
    ~CShellContextMenu();

    bool IsMenuCommand(int iCmd) const;
    void InvokeCommand(int iCmd) const;

    void CShellContextMenu::SetMenu(BCMenu *pMenu, BCMenu *pMenu2);

protected:    
    void CShellContextMenu::FillSendToMenu(BCMenu *pMenu, 
        LPSHELLFOLDER pSF, UINT &idm);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLCONTEXTMENU_H__24BAC666_2B03_11D3_B9C1_0000861DFCE7__INCLUDED_)
