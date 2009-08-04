///////////////////////////////////////////////////////////////////////////////
// $Workfile: HtmlHelpDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/HtmlHelpDialog.cpp $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
/***************************************************************************/
/* NOTE: Parts of                                                          */
/* This document is copyright (c) by Oz Solomonovich.  All non-commercial  */
/* use is allowed, as long as this document is not altered in any way, and */
/* due credit is given.                                                    */
/***************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include "HtmlHelpDialog.h"
#include "HtmlHelp.h"
#include <io.h>

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD s_tooltipHelpList[] = 
{
	IDC_COM_ABOUT,
	IDC_COM_EXTRAFILES,
	IDHELP,
	IDC_COM_CURRENTFILE,
	IDC_FF_FILES,
	IDC_FF_FILENAME,
	IDC_FF_REFRESH,
	IDC_FF_SCOPE_PROJECT,
	IDC_FF_SCOPE_WORKSPACE,
	IDC_FF_PREFIX,
	IDC_EP_LIST,
	IDC_EP_ADD,
	IDC_EP_REMOVE,
	IDC_EP_FILENAME,
	IDC_EP_BROWSE,
	IDC_FT_TAGS,
	IDC_FT_PARENT,
	IDC_FT_NAME,
	IDC_FT_REBUILDTAGS,
	IDC_FT_SCOPE_FILE,
	IDC_FT_SCOPE_PROJECT,
	IDC_FT_SCOPE_WORKSPACE,
	IDC_FT_INFO_FILENAME,
	IDC_FT_INFO_MISC,
	IDC_FT_INFO_SEARCHSTRING,
	IDC_FT_SHOW,
	IDC_FT_TYPECOMBO,
	IDC_FT_REFRESHTAGS,
	IDC_H_RESET,
	IDC_H_FILES,
	IDC_TS_FILELIST,
	IDC_TS_TREE,
	IDC_TS_MEMO,
	IDC_PKB_KEYCHECK1,
	IDC_PKB_KEYCHECK2,
	IDC_PKB_KEYCHECK3,
	IDC_PKB_KEYCHECK4,
	IDC_PKB_KEYCHECK5,
	IDC_PKB_KEYCHECK6,
	IDC_PKB_KEYCHECK7,
	IDC_PKB_KEYCHECK8,
	IDC_PKB_KEYCHECK9,
	IDC_PKB_KEYCHECK10,
	IDC_PKB_KEYCHECK11,
	IDC_PKB_KEYCHECK12,
	IDC_PKB_KEYCHECK13,
	IDC_PKB_KEYCHECK14,
	IDC_PKB_KEYCHECK15,
	IDC_PKB_KEYCHECK16,
	IDC_PKB_KEYCHECK17,
	IDC_PKB_KEYCHECK18,
	IDC_PKB_KEYCHECK19,
	IDC_PKB_KEYCHECK20,
	IDC_PKB_KEY1,
	IDC_PKB_KEY2,
	IDC_PKB_KEY3,
	IDC_PKB_KEY4,
	IDC_PKB_KEY5,
	IDC_PKB_KEY6,
	IDC_PKB_KEY7,
	IDC_PKB_KEY8,
	IDC_PKB_KEY9,
	IDC_PKB_KEY10,
	IDC_PKB_KEY11,
	IDC_PKB_KEY12,
	IDC_PKB_KEY13,
	IDC_PKB_KEY14,
	IDC_PKB_KEY15,
	IDC_PKB_KEY16,
	IDC_PKB_KEY17,
	IDC_PKB_KEY18,
	IDC_PKB_KEY19,
	IDC_PKB_KEY20,
	IDC_PKB_ASSIGN,
	IDC_PKB_RESET,
	IDC_PHF_EXTLIST,
	IDC_PHF_EXTEDIT,
	IDC_PHF_ADD,
	IDC_PHF_REMOVE,
	IDC_PHF_RESET,
	IDC_PC_EXTEDIT,
	IDC_PC_ADD,
	IDC_PC_REMOVE,
	IDC_PC_RESET,
	IDC_PC_EXTLIST,
	IDC_PC_AUTOPACKCHANGES,
	IDC_PC_AUTOSAVECHANGES,
	IDC_PC_USETAGCURSORDIALOG,
	IDC_PC_AUTOREFRESHTAGS,
	IDC_PC_AUTOSAVEALL,
	IDC_PC_TOKENEDIT,
	IDC_PC_TOKENADD,
	IDC_PC_TOKENREMOVE,
	IDC_PC_TOKENRESET,
	IDC_PC_TOKENLIST,
};

class CHtmlHelp
{
public:
    CHtmlHelp();
    ~CHtmlHelp();

    const CString& GetHelpFilename();
    void ShowHelp(HWND hWnd, int ctrlID, DWORD helpID,
        LPCTSTR pszHelpWndType = NULL);
    DWORD FindHelpID(const DWORD *ids, DWORD ctrlID, bool bAllowImplicit);
    BOOL OnHelpInfo(CWnd *pDlg, const DWORD *pHelpIDs, HELPINFO* pHelpInfo,
        bool bAllowImplicit);
    void OnContextMenu(CWnd *pDlg, const DWORD *pHelpIDs, CWnd *pWnd,
        CPoint point, bool bAllowImplicit);

#ifndef UNICODE
    typedef HWND (WINAPI * fnHtmlHelp)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData);
#else
    typedef HWND (WINAPI * fnHtmlHelp)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData);
#endif //UNICODE
    HINSTANCE	m_htmlHelpInstance;
    fnHtmlHelp	m_htmlHelpFunction;

    CMenu       WhatsThis;

    CMap<int, int, CString, LPCTSTR> m_WinTypeMap;
};


CHtmlHelp g_htmlHelp;

CHtmlHelp::CHtmlHelp()
{
    m_htmlHelpInstance = AfxLoadLibrary("HHCTRL.OCX");
    if (m_htmlHelpInstance)
#ifndef UNICODE
        m_htmlHelpFunction = (fnHtmlHelp)::GetProcAddress(m_htmlHelpInstance, "HtmlHelpA");
#else
        m_htmlHelpFunction = (fnHtmlHelp)::GetProcAddress(m_htmlHelpInstance, "HtmlHelpW");
#endif //UNICODE
    else
        m_htmlHelpFunction = NULL;
}


CHtmlHelp::~CHtmlHelp()
{
    if (m_htmlHelpInstance)
    {
        AfxFreeLibrary(m_htmlHelpInstance);
    }
}


const CString& CHtmlHelp::GetHelpFilename()
{
    static CString CachedName;

	if (CachedName.IsEmpty())
	{
		CachedName = AfxGetApp()->m_pszHelpFilePath;
		if (_access(CachedName, 0) == -1)
			CachedName.Empty();
	}

    return CachedName;
}


void CHtmlHelp::ShowHelp(HWND hWnd, int ctrlID, DWORD helpID,
                         LPCTSTR pszHelpWndType /*= NULL*/)
{
    if (!m_htmlHelpFunction)
        return;

	pszHelpWndType = "WorkspaceWhiz";
    if (GetHelpFilename().IsEmpty())
    {
        CString msg, file;
        int p;
        file = AfxGetApp()->m_pszHelpFilePath;
        if ((p = file.ReverseFind('\\')) >= 0) file = file.Mid(p + 1);
        msg.Format("Help file [%s] not found." , file);
        AfxMessageBox(msg, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (ctrlID)
    {
		for (DWORD i = 0; i < _countof(s_tooltipHelpList); i++)
		{
			if (helpID == s_tooltipHelpList[i])
			{
//				pszHelpWndType = "CsPopup";
				DWORD ids[] = { (DWORD)ctrlID, helpID, 0, 0 };
	            (*m_htmlHelpFunction)(hWnd, GetHelpFilename(), HH_TP_HELP_WM_HELP,
	                (DWORD)ids);
	            return;
//				break;
			}
		}
    }

    if (!pszHelpWndType)
    {
        if (helpID < 0x30000000)
        {
//            pszHelpWndType = pszPopupWinTypeName;
			pszHelpWndType = "CsPopup";
        }
        else
        {
            if (helpID < 0x40000000)
            {
                int n = (helpID >> 16) & 0xFF;
                if (n)
                {
                    CString val;
                    if (m_WinTypeMap.Lookup(n, val) != 0)
                    {
                        pszHelpWndType = m_WinTypeMap[n];
                    }
                    else
                    {
                        ASSERT(false);
                    }
                }
            }
        }
    }

    CString szHelpFile = GetHelpFilename();
    if (pszHelpWndType)
    {
        szHelpFile += ">";
        szHelpFile += pszHelpWndType;
    }

	if (helpID == 0)
	    (*m_htmlHelpFunction)(hWnd, szHelpFile, HH_DISPLAY_TOC, NULL);
	else
	    (*m_htmlHelpFunction)(hWnd, szHelpFile, HH_HELP_CONTEXT, helpID);
}


#define NO_HELP_TXT     "No help information is available for this item."
#define NO_HELP_FNT     "MS Sans Serif,8,,"

BOOL CHtmlHelp::OnHelpInfo(CWnd *pDlg, const DWORD *pHelpIDs,
                               HELPINFO* pHelpInfo, bool bAllowImplicit)
{
    HWND    hWnd = (HWND)pHelpInfo->hItemHandle;
    CString cName;
    DWORD   id;
    int     ctrlID;

    // check to see if user pressed F1 from inside a popup help window 
    ::GetClassName(hWnd, cName.GetBuffer(512), 512);
    cName.ReleaseBuffer();
    if (cName == _T("hh_popup"))
    {
        // we're inside a popup window, so exit
        return FALSE;
    }

    if (pHelpInfo->iCtrlId >= 0)
        ctrlID = pHelpInfo->iCtrlId;
    else
        ctrlID = (unsigned short)::GetDlgCtrlID(hWnd);

//    id = FindHelpID(pHelpIDs, (DWORD)ctrlID, bAllowImplicit);
	id = ctrlID;

    if (id == 0)
    {
        // there's no help message for this item.

        // does user want to show a message when there's no default help?
//        if (!g_bHTMLHelp_ShowNoHelpMessage)
//        {
//            return FALSE;
//        }

        // Either:
        // 1. The user pressed F1  - or -
        // 2. The user pressed the dialog's '?' and used the mouse to click
        //    on an item.
        // For the latter, we want to avoid displaying messages for items
        // that aren't tab stops, otherwise every static control and the
        // likes would generate a message
        if ((::GetWindowLong(hWnd, GWL_STYLE) & WS_TABSTOP) != WS_TABSTOP)
        {
            return FALSE;
        }

        HH_POPUP    hhp;
        RECT        r;

        hhp.cbStruct = sizeof(hhp);
        hhp.idString = 0;
        hhp.pszText  = NO_HELP_TXT;
        hhp.pszFont  = NO_HELP_FNT;
        hhp.clrBackground = -1;
        hhp.clrForeground = -1;
        ::SetRect(&hhp.rcMargins, -1, -1, -1, -1);

        ::GetWindowRect(hWnd, &r);
        hhp.pt.x = (r.right + r.left) / 2;
        hhp.pt.y = (r.bottom + r.top) / 2;

        (*g_htmlHelp.m_htmlHelpFunction)(pDlg->m_hWnd, NULL, HH_DISPLAY_TEXT_POPUP,
            (DWORD)&hhp);
        return TRUE;
    }

    ShowHelp(hWnd, ctrlID, id);

    return TRUE;
}



#define IMPLEMENT_CLASS(classname, baseclass)                               \
                                                                            \
IMPLEMENT_DYNCREATE(classname, baseclass)                                   \
                                                                            \
BEGIN_MESSAGE_MAP(classname, baseclass)                                     \
	ON_WM_CONTEXTMENU()                                                     \
	ON_WM_HELPINFO()                                                        \
    ON_COMMAND(IDHELP, OnHelp)                                              \
END_MESSAGE_MAP()                                                           \
                                                                            \
void classname::OnHelp()                                                    \
{                                                                           \
    g_htmlHelp.ShowHelp(NULL, GetDlgCtrlID(), m_id);				\
}                                                                           \
                                                                            \
void classname::OnContextMenu(CWnd *pWnd, CPoint point)                     \
{                                                                           \
}                                                                           \
                                                                            \
BOOL classname::OnHelpInfo(HELPINFO* pHelpInfo)                             \
{                                                                           \
	g_htmlHelp.OnHelpInfo(this, NULL, pHelpInfo, true);						\
	return baseclass::OnHelpInfo(pHelpInfo);								\
}


IMPLEMENT_CLASS(CHtmlHelpDialog,   cdxCDynamicDialog);
IMPLEMENT_CLASS(CHtmlHelpPropPage, CPropertyPage);
IMPLEMENT_DYNAMIC(CHtmlHelpPropSheet, CPropertySheet)


BOOL CHtmlHelpPropSheet::OnInitDialog()
{
    BOOL bResult = CPropertySheet::OnInitDialog();
    ModifyStyleEx(0, WS_EX_CONTEXTHELP);
    return bResult;
}


