///////////////////////////////////////////////////////////////////////////////
// $Workfile: DSAddIn.h $
// $Archive: /WorkspaceWhiz/Src/Samples/BackupProjectsAddin/DSAddIn.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 2    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_DSADDIN_H__9F560B75_4507_4904_9CE4_3AB2FCA095D6__INCLUDED_)
#define AFX_DSADDIN_H__9F560B75_4507_4904_9CE4_3AB2FCA095D6__INCLUDED_

#include "commands.h"

// {957CD3AD-F43D-4507-AD80-56CF889AA3BB}
DEFINE_GUID(CLSID_DSAddIn,
0x957cd3ad, 0xf43d, 0x4507, 0xad, 0x80, 0x56, 0xcf, 0x88, 0x9a, 0xa3, 0xbb);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "BackupProjectsAddin.DSAddIn.1",
		"BACKUPPROJECTSADDIN Developer Studio Add-in", IDS_BACKUPPROJECTSADDIN_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);

protected:
	CCommandsObj* m_pCommands;
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSADDIN_H__9F560B75_4507_4904_9CE4_3AB2FCA095D6__INCLUDED)
