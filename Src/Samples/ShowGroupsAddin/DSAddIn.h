///////////////////////////////////////////////////////////////////////////////
// $Workfile: DSAddIn.h $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowGroupsAddin/DSAddIn.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_DSADDIN_H__4E3AD43A_091A_4BA6_9B66_5FBCA37B2F97__INCLUDED_)
#define AFX_DSADDIN_H__4E3AD43A_091A_4BA6_9B66_5FBCA37B2F97__INCLUDED_

#include "commands.h"

// {149CCF4D-F302-4F48-A442-9E8B99158021}
DEFINE_GUID(CLSID_DSAddIn,
0x149ccf4d, 0xf302, 0x4f48, 0xa4, 0x42, 0x9e, 0x8b, 0x99, 0x15, 0x80, 0x21);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "ShowGroupsAddin.DSAddIn.1",
		"SHOWGROUPSADDIN Developer Studio Add-in", IDS_SHOWGROUPSADDIN_LONGNAME,
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

#endif // !defined(AFX_DSADDIN_H__4E3AD43A_091A_4BA6_9B66_5FBCA37B2F97__INCLUDED)
