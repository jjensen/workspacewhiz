// FIPreferences.cpp : implementation file
//

#include "stdafx.h"
#include "workspaceutils.h"
#include "FIPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFIPreferences dialog


CFIPreferences::CFIPreferences(CWnd* pParent /*=NULL*/)
	: CDialog(CFIPreferences::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFIPreferences)
	m_classes = FALSE;
	m_constants = FALSE;
	m_functions = FALSE;
	//}}AFX_DATA_INIT
}


void CFIPreferences::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFIPreferences)
	DDX_Check(pDX, IDC_FI_CLASSES, m_classes);
	DDX_Check(pDX, IDC_FI_CONSTANTS, m_constants);
	DDX_Check(pDX, IDC_FI_FUNCTIONS, m_functions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFIPreferences, CDialog)
	//{{AFX_MSG_MAP(CFIPreferences)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFIPreferences message handlers
