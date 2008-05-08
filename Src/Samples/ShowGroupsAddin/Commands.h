///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.h $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowGroupsAddin/Commands.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_COMMANDS_H__E24059AF_7C46_4DF4_9087_94BB83E715A0__INCLUDED_)
#define AFX_COMMANDS_H__E24059AF_7C46_4DF4_9087_94BB83E715A0__INCLUDED_

#include "ShowGroupsAddinTypes.h"

class CCommands : 
	public CComDualImpl<ICommands, &IID_ICommands, &LIBID_ShowGroupsAddin>, 
	public CComObjectRoot,
	public CComCoClass<CCommands, &CLSID_Commands>
{
protected:
	IApplication* m_pApplication;

public:
	CCommands();
	~CCommands();
	void SetApplicationObject(IApplication* m_pApplication);

	BEGIN_COM_MAP(CCommands)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ICommands)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CCommands)

public:
// ICommands methods
	STDMETHOD(ShowGroupsAddinCommandMethod)(THIS);
};

typedef CComObject<CCommands> CCommandsObj;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDS_H__E24059AF_7C46_4DF4_9087_94BB83E715A0__INCLUDED)
