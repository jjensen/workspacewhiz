///////////////////////////////////////////////////////////////////////////////
// $Workfile: DSAddIn.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/DSAddIn.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 5    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_DSADDIN_H__039FB635_6DD0_11D3_9B27_835709CADA52__INCLUDED_)
#define AFX_DSADDIN_H__039FB635_6DD0_11D3_9B27_835709CADA52__INCLUDED_

#include "commands.h"

// {039FB622-6DD0-11D3-9B27-835709CADA52}
DEFINE_GUID(CLSID_DSAddIn,
0x39fb622, 0x6dd0, 0x11d3, 0x9b, 0x27, 0x83, 0x57, 0x9, 0xca, 0xda, 0x52);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn :
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "WorkspaceWhiz.DSAddIn.1",
		"WORKSPACEWHIZ Developer Studio Add-in", IDS_WORKSPACEWHIZ_LONGNAME,
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

#endif // !defined(AFX_DSADDIN_H__039FB635_6DD0_11D3_9B27_835709CADA52__INCLUDED)
