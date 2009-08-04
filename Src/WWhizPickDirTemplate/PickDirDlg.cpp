///////////////////////////////////////////////////////////////////////////////
// $Workfile: PickDirDlg.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizPickDirTemplate/PickDirDlg.cpp $
// $Date: 2003/01/30 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "PickDirDlg.h"

#include <shlobj.h>

CPickDirDlg::CPickDirDlg(CString strTitle, CString strStartDir)
{
	m_strTitle = strTitle;
	m_strStartDir = strStartDir;
}

CString CPickDirDlg::GetDirectory(void)
{
	LPMALLOC pMalloc;
	
	/* Gets the Shell's default allocator */
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;
		
		// Get help on BROWSEINFO struct - it's got all the bit settings.
		bi.hwndOwner = NULL;
		if ( m_strStartDir.IsEmpty() )
		{
			LPITEMIDLIST pidlStart;
			// get the pidl for the desktop - this will be used to initialize the folder
			SHGetSpecialFolderLocation( NULL, CSIDL_DESKTOP, &pidlStart);
			bi.pidlRoot = pidlStart;
		}
		else
			bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		if ( m_strTitle == _T("") )
			bi.lpszTitle = _T("Directory Picker");
		else
			bi.lpszTitle = m_strTitle;
		
		// Browse only file system directories, no "Control Panel" or "Printers" folders.
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseProc;
		if ( m_strStartDir.IsEmpty() )
			bi.lParam = NULL;
		else
			bi.lParam = (LPARAM)((const char *)m_strStartDir);
		
		// This next call issues the dialog box.
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, pszBuffer))
			{
				// At this point pszBuffer contains the selected path */.
				return pszBuffer;
			}
			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free(pidl);
		}
		// Release the shell's allocator.
		pMalloc->Release();
	}
	return _T("");
}

/*
* This proc. has been added just in order to set the starting directory.
* I looks shortsighted that the BROWSEINFO structure didn't have an
* easy way to set the starting place - it seems like such an obvious
* thing to provide.  The structure has a pidlRoot element, which takes a structure of
* type LPITEMIDLIST, but it has kno way to take a directory name string.
* To this function, we pass the lParam from the BROWSEINFO in the form of lpData, and
* while initializing, the shell browser will pick the directory whose name is in lpData.
*/
int WINAPI CPickDirDlg::BrowseProc( HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		if (lpData)
		{
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		}
	}
	return 0;
}


