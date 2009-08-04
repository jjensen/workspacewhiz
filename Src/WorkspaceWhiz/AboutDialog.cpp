///////////////////////////////////////////////////////////////////////////////
// $Workfile: AboutDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/AboutDialog.cpp $
// $Date: 2003/01/07 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "AboutDialog.h"
#include <exdisp.h>

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog


CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: ABOUT_DIALOG(CAboutDialog::IDD, pParent),
	m_pBrowserApp(NULL)
{
	//{{AFX_DATA_INIT(CAboutDialog)
	//}}AFX_DATA_INIT
}


CAboutDialog::~CAboutDialog()
{
	if (m_pBrowserApp)
		m_pBrowserApp->Release();
}


void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	ABOUT_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDialog)
	DDX_Control(pDX, IDCANCEL, m_butOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDialog, ABOUT_DIALOG)
	//{{AFX_MSG_MAP(CAboutDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_AB_REGISTER, OnAbRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(CAboutDialog, cdxCDynamicDialog)
	DYNAMIC_MAP_ENTRY(IDCANCEL,				mdRelative,	mdRepos)
	DYNAMIC_MAP_ENTRY(IDC_AB_REGISTER,		mdRelative,	mdRepos)
END_DYNAMIC_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog message handlers

BOOL CAboutDialog::OnInitDialog()
{
	ABOUT_DIALOG::OnInitDialog();

	CRect dlgClient;
	GetClientRect(dlgClient);
	ClientToScreen(dlgClient);

	CRect rectClient;
	GetDlgItem(IDC_AB_HTML)->GetWindowRect(rectClient);
	rectClient.OffsetRect(-dlgClient.left, -dlgClient.top);
	GetDlgItem(IDC_AB_HTML)->DestroyWindow();

	m_wndBrowser.SetWindowPos(NULL, rectClient.left, rectClient.top,
		rectClient.Width(), rectClient.Height(), SWP_NOZORDER);

	// Resizing stuff.
	AddSzControl(m_wndBrowser,						mdResize,	mdResize);

	// Get the module name.
	TCHAR moduleName[_MAX_PATH];
	::GetModuleFileName(AfxGetResourceHandle(), (TCHAR*)&moduleName, _MAX_PATH);

	CString resName;
	resName.Format("res://%s/About.html", moduleName);

	DWORD dwFlags = 0;
	COleSafeArray vPostData;
	m_pBrowserApp->Navigate(CComBSTR(resName),
		COleVariant((long) dwFlags, VT_I4),
		COleVariant((LPCTSTR)NULL, VT_BSTR),
		vPostData,
		COleVariant((LPCTSTR)NULL, VT_BSTR));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CAboutDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (ABOUT_DIALOG::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	AfxEnableControlContainer();

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, "",
				WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}
	
	return 0;
}

void CAboutDialog::OnAbRegister()
{
	if (g_wwhizReg)
		g_wwhizReg->RegistrationDialog(true);
/*	if (AfxMessageBox("If you wish to contribute to the further development of Workspace Whiz "
			"press OK to connect to PayPal and send a donation to 'sales@workspacewhiz.com'.  Any amount "
			"would be appreciated.\nPress Cancel to continue using Workspace Whiz.\n",
			MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}

	ShellExecute(NULL, "open", "http://www.paypal.com/", NULL, NULL, SW_SHOW);
*/
}
