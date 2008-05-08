///////////////////////////////////////////////////////////////////////////////
// $Workfile: PickDirDlg.h $
// $Archive: /WorkspaceWhiz/Src/WWhizPickDirTemplate/PickDirDlg.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 2    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#ifndef __PICKDIRDLG_H_
#define __PICKDIRDLG_H_

#include <afx.h>

class CPickDirDlg
{
public:
	CPickDirDlg(void) {}  // default constructor
	CPickDirDlg(CString strTitle, CString strStartDir);
	CString GetDirectory();
	void SetTitle(CString& strTitle) { m_strTitle = strTitle; }
	void SetStartDirectory(CString& strStartDir) { m_strStartDir = strStartDir; }

private:
	CString m_strTitle;
	CString m_strStartDir;
	static int WINAPI BrowseProc( HWND hwnd, UINT uMsg, LPARAM lParam,LPARAM lpData);
};

#endif
