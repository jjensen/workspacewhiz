///////////////////////////////////////////////////////////////////////////////
// $Workfile: ExpiredDlg.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/ExpiredDlg.cpp $
// $Date:: 7/14/00 2:23a   $ $Revision:: 10   $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizReg.h"
#include "WWhizReg.h"
#include "ExpiredDlg.h"
#include "EnterRegDlg.h"
#include "ValidateRegistrationCode.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExpiredDlg dialog


CExpiredDlg::CExpiredDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExpiredDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpiredDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CExpiredDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpiredDlg)
	DDX_Control(pDX, IDC_WWRE_TIMELEFTBAR, m_timeLeftBar);
	DDX_Control(pDX, IDC_WWRE_TIMELEFT, m_timeLeft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExpiredDlg, CDialog)
	//{{AFX_MSG_MAP(CExpiredDlg)
	ON_BN_CLICKED(IDC_WWRE_EVALUATE, OnWwreEvaluate)
	ON_BN_CLICKED(IDC_WWRE_ENTERCODE, OnWwreEntercode)
	ON_BN_CLICKED(IDC_WWRE_REGISTER, OnWwreRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpiredDlg message handlers

BOOL CExpiredDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_timeLeftBar.SetRange(0, 21);

	FillInControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExpiredDlg::FillInControls()
{
	// Read the key stuff from the registry.
	CString regName = AfxGetApp()->GetProfileString("Config", "RegName");
	CString regCode = AfxGetApp()->GetProfileString("Config", "RegCode");

	time_t registrationTime;
	bool decoded = ValidateRegistrationCode(regCode, regName, registrationTime);
	if (decoded)
	{
		CTime time(registrationTime);
		CString timeStr = time.Format("%A, %B %d, %Y");

		CString str;
		str.Format("Workspace Whiz is registered to:\n    %s\non:\n    %s", (LPCTSTR)regName, timeStr);
		GetDlgItem(IDC_WWRE_TEXT)->SetWindowText(str);

		SetWindowText("Workspace Whiz Registered Version");
		GetDlgItem(IDC_WWRE_EVALUATE)->SetWindowText("Continue");

		m_timeLeft.ShowWindow(SW_HIDE);
		m_timeLeftBar.ShowWindow(SW_HIDE);
	}
	else
	{
		HINSTANCE oldResourceHandle = AfxGetResourceHandle();
		AfxSetResourceHandle(g_instance);
		CString timeLeftStr;
		timeLeftStr.LoadString(IDS_WWR_TIMELEFT);
		CString timeLeftStr2;
		timeLeftStr2.Format(timeLeftStr, m_numDays);
		m_timeLeft.SetWindowText(timeLeftStr2);
		AfxSetResourceHandle(oldResourceHandle);
		m_timeLeftBar.SetPos(m_numDays);
	}

}

void CExpiredDlg::OnWwreEvaluate()
{
	OnOK();
}

void CExpiredDlg::OnWwreEntercode()
{
	if (HasExpired())
		return;

	CEnterRegDlg dlg;
	dlg.DoModal();
	if (dlg.m_gotCode)
		OnOK();
}

void CExpiredDlg::OnWwreRegister()
{
	if (AfxMessageBox("If you wish to order Workspace Whiz 4.00 at this time, "
			"press OK to connect to Plimus now.\nPress Cancel to "
			"continue using the unregistered Workspace Whiz 4.00.\n", MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}

	ShellExecute(NULL, "open", "https://www.plimus.com/jsp/buynow.jsp?contractId=1685607",
			NULL, NULL, SW_SHOW);
}
