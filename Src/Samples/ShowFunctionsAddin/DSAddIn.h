///////////////////////////////////////////////////////////////////////////////
// $Workfile: DSAddIn.h $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowFunctionsAddin/DSAddIn.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_DSADDIN_H__BB1DC8AA_2271_4CAF_B4E2_171BC0EAC3AD__INCLUDED_)
#define AFX_DSADDIN_H__BB1DC8AA_2271_4CAF_B4E2_171BC0EAC3AD__INCLUDED_

#include "commands.h"

// {684AF377-81C2-4B9D-A551-B0D174BEA4FD}
DEFINE_GUID(CLSID_DSAddIn,
0x684af377, 0x81c2, 0x4b9d, 0xa5, 0x51, 0xb0, 0xd1, 0x74, 0xbe, 0xa4, 0xfd);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "ShowFunctionsAddin.DSAddIn.1",
		"SHOWFUNCTIONSADDIN Developer Studio Add-in", IDS_SHOWFUNCTIONSADDIN_LONGNAME,
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

#endif // !defined(AFX_DSADDIN_H__BB1DC8AA_2271_4CAF_B4E2_171BC0EAC3AD__INCLUDED)
