///////////////////////////////////////////////////////////////////////////////
// $Workfile: Commands.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/Commands.h $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_COMMANDS_H__039FB637_6DD0_11D3_9B27_835709CADA52__INCLUDED_)
#define AFX_COMMANDS_H__039FB637_6DD0_11D3_9B27_835709CADA52__INCLUDED_

#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\dbgguid.h>

#include "WorkspaceWhizTypes.h"

class CCommands :
	public CComDualImpl<ICommands, &IID_ICommands, &LIBID_WorkspaceWhiz>,
	public CComObjectRoot,
	public CComCoClass<CCommands, &CLSID_Commands>
{
protected:
	IApplication* m_pApplication;

public:
	CCommands();
	~CCommands();
	void SetApplicationObject(IApplication* m_pApplication);
	IApplication* GetApplicationObject() { return m_pApplication; }
	void UnadviseFromEvents();

	BEGIN_COM_MAP(CCommands)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ICommands)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CCommands)

protected:
	// This class template is used as the base class for the Application
	//  event handler object and the Debugger event handler object,
	//  which are declared below.
	template <class IEvents, const IID* piidEvents, const GUID* plibid,
		class XEvents, const CLSID* pClsidEvents>
	class XEventHandler :
		public CComDualImpl<IEvents, piidEvents, plibid>,
		public CComObjectRoot,
		public CComCoClass<XEvents, pClsidEvents>
	{
	public:
		BEGIN_COM_MAP(XEvents)
			COM_INTERFACE_ENTRY(IDispatch)
			COM_INTERFACE_ENTRY_IID(*piidEvents, IEvents)
		END_COM_MAP()
		DECLARE_NOT_AGGREGATABLE(XEvents)
		void Connect(IUnknown* pUnk)
		{ VERIFY(SUCCEEDED(AtlAdvise(pUnk, this, *piidEvents, &m_dwAdvise))); }
		void Disconnect(IUnknown* pUnk)
		{ AtlUnadvise(pUnk, *piidEvents, m_dwAdvise); }

		CCommands* m_pCommands;

	protected:
		DWORD m_dwAdvise;
	};

	// This object handles events fired by the Application object
	class XApplicationEvents : public XEventHandler<IApplicationEvents,
		&IID_IApplicationEvents, &LIBID_WorkspaceWhiz,
		XApplicationEvents, &CLSID_ApplicationEvents>
	{
	public:
		// IApplicationEvents methods
		STDMETHOD(BeforeBuildStart)(THIS);
		STDMETHOD(BuildFinish)(THIS_ long nNumErrors, long nNumWarnings);
		STDMETHOD(BeforeApplicationShutDown)(THIS);
		STDMETHOD(DocumentOpen)(THIS_ IDispatch * theDocument);
		STDMETHOD(BeforeDocumentClose)(THIS_ IDispatch * theDocument);
		STDMETHOD(DocumentSave)(THIS_ IDispatch * theDocument);
		STDMETHOD(NewDocument)(THIS_ IDispatch * theDocument);
		STDMETHOD(WindowActivate)(THIS_ IDispatch * theWindow);
		STDMETHOD(WindowDeactivate)(THIS_ IDispatch * theWindow);
		STDMETHOD(WorkspaceOpen)(THIS);
		STDMETHOD(WorkspaceClose)(THIS);
		STDMETHOD(NewWorkspace)(THIS);
	};
	typedef CComObject<XApplicationEvents> XApplicationEventsObj;
	XApplicationEventsObj* m_pApplicationEventsObj;

	// This object handles events fired by the Application object
	class XDebuggerEvents : public XEventHandler<IDebuggerEvents,
		&IID_IDebuggerEvents, &LIBID_WorkspaceWhiz,
		XDebuggerEvents, &CLSID_DebuggerEvents>
	{
	public:
		// IDebuggerEvents method
	    STDMETHOD(BreakpointHit)(THIS_ IDispatch * pBreakpoint);
	};
	typedef CComObject<XDebuggerEvents> XDebuggerEventsObj;
	XDebuggerEventsObj* m_pDebuggerEventsObj;

public:
	STDMETHOD(WWOptions)();
	STDMETHOD(WWFileOpen)();
	STDMETHOD(WWFileGlobalOpen)();
	STDMETHOD(WWHeaderFlip)();
	STDMETHOD(WWFileFindPrev)();
	STDMETHOD(WWFileFindNext)();
	STDMETHOD(WWHistoryBack)();
	STDMETHOD(WWHistoryForward)();
	STDMETHOD(WWHistoryDlg)();
	STDMETHOD(WWTagFindDlg)();
	STDMETHOD(WWTagFindSpecialDlg)();
	STDMETHOD(WWTagFlip)();
	STDMETHOD(WWTagFindAtCursor)();
	STDMETHOD(WWTagFindAtCursorDlg)();
	STDMETHOD(WWTagFindPrev)();
	STDMETHOD(WWTagFindNext)();
	STDMETHOD(WWTagCompletePrev)();
	STDMETHOD(WWTagCompleteNext)();
	STDMETHOD(WWTagCompleteRestore)();
	STDMETHOD(WWTagCompleteDlg)();
	STDMETHOD(WWTemplateSelect)();
	STDMETHOD(WWTemplateComplete)();
	STDMETHOD(WWPasteFix)();
// ICommands methods
};

typedef CComObject<CCommands> CCommandsObj;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDS_H__039FB637_6DD0_11D3_9B27_835709CADA52__INCLUDED)
