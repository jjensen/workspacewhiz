// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__DEACBAD6_24DD_11D2_A392_444553540000__INCLUDED_)
#define AFX_DSADDIN_H__DEACBAD6_24DD_11D2_A392_444553540000__INCLUDED_

#include "commands.h"

// {DEACBAC3-24DD-11D2-A392-444553540000}
DEFINE_GUID(CLSID_DSAddIn,
0xdeacbac3, 0x24dd, 0x11d2, 0xa3, 0x92, 0x44, 0x45, 0x53, 0x54, 0, 0);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "WorkspaceUtils.DSAddIn.3",
		"WORKSPACEUTILS Developer Studio Add-in", IDS_WORKSPACEUTILS_LONGNAME,
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
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// Inform DevStudio of the commands we implement
struct CommandInfo
{
	CString m_name;
	CString m_desc;
	bool m_toolbar;

	CommandInfo(CString name, CString desc, bool toolbar)
	{  m_name = name;  m_desc = desc;  m_toolbar = toolbar;  }
};

const CommandInfo* GetCommandList();

#endif // !defined(AFX_DSADDIN_H__DEACBAD6_24DD_11D2_A392_444553540000__INCLUDED)
