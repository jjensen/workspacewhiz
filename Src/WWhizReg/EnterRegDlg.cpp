///////////////////////////////////////////////////////////////////////////////
// $Workfile: EnterRegDlg.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/EnterRegDlg.cpp $
// $Date:: 7/14/00 2:23a   $ $Revision:: 6    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizReg.h"
#include "WWhizReg.h"
#include "EnterRegDlg.h"
#include "ValidateRegistrationCode.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnterRegDlg dialog


CEnterRegDlg::CEnterRegDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterRegDlg::IDD, pParent),
	m_gotCode(false)
{
	//{{AFX_DATA_INIT(CEnterRegDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEnterRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterRegDlg)
	DDX_Control(pDX, IDC_WWRR_USERNAME, m_fullName);
	DDX_Control(pDX, IDC_WWRR_REGCODE, m_regCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnterRegDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterRegDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterRegDlg message handlers

void CEnterRegDlg::OnOK()
{
	CString regName;
	m_fullName.GetWindowText(regName);
	regName.TrimLeft();
	regName.TrimRight();

	CString regCode;
	m_regCode.GetWindowText(regCode);
	regCode.TrimLeft();
	regCode.TrimRight();

	time_t registrationTime;
	bool decoded = ValidateRegistrationCode(regCode, regName, registrationTime);
	if (!decoded)
	{
		AfxMessageBox("Registration code incorrect.\n");
		return;
	}

	CTime time(registrationTime);
	CString timeStr = time.Format("%A, %B %d, %Y");

	CString str;
	str.Format("Workspace Whiz is registered to:\n\n%s\n\non:\n\n%s", (LPCTSTR)regName, timeStr);
	AfxMessageBox(str);

	AfxGetApp()->WriteProfileString("Config", "RegName", regName);
	AfxGetApp()->WriteProfileString("Config", "RegCode", regCode);

	m_gotCode = true;

	CDialog::OnOK();
}
