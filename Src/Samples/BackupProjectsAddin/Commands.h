///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.h $
// $Archive: /WorkspaceWhiz/Src/Samples/BackupProjectsAddin/Commands.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_COMMANDS_H__4BC5088B_0B26_4C4B_9C39_7A3BF9E260C4__INCLUDED_)
#define AFX_COMMANDS_H__4BC5088B_0B26_4C4B_9C39_7A3BF9E260C4__INCLUDED_

#include "BackupProjectsAddinTypes.h"

class CCommands : 
	public CComDualImpl<ICommands, &IID_ICommands, &LIBID_BackupProjectsAddin>, 
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
	STDMETHOD(BackupProjectsAddinCommandMethod)(THIS);
};

typedef CComObject<CCommands> CCommandsObj;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDS_H__4BC5088B_0B26_4C4B_9C39_7A3BF9E260C4__INCLUDED)
