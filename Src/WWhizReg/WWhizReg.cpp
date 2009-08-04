///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizReg.cpp $
// $Archive: /WWhizReg/WWhizReg.cpp $
// $Date:: 4/12/01 8:19p   $ $Revision:: 26   $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
//#include <afxdllx.h>    // standard MFC Extension DLL routines
#include "pchWWhizReg.h"
#include "WWhizInterface3.h"
#include <atlbase.h>
#include <initguid.h>
#include "ObjModelGUID.h"
#pragma warning (disable : 4355)
#include "WWhizReg.h"
#include "FindFileEdit.h"
#include "FindFileListCtrl.h"
#include "FindTagEdit.h"
#include "FindTagListCtrl.h"
#include "RegHelperWnd.h"
#include "RegThread.h"
#include "ExpiredDlg.h"
#include "ValidateRegistrationCode.h"
#include <assert.h>
#include "Subclass.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef FindText

HINSTANCE g_instance;

/////////////////////////////////////////////////////////////////////////////
// Initialization of MFC Extension DLL

static AFX_EXTENSION_MODULE NEAR extensionDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            g_instance = hInstance;
            // Extension DLL one-time initialization.
            if (!AfxInitExtensionModule(extensionDLL, hInstance))
                return 0;
            WNEW CDynLinkLibrary(extensionDLL);
            break;

        case DLL_PROCESS_DETACH:
            // Extension DLL per-process termination
            AfxTermExtensionModule(extensionDLL);
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return 1;
}

/////////////////////////////////////////////////////////////////////////////

static CString MakeWindowID(UINT id)
{
	CString str;
	str.Format(_T("ID=0x%08lx"), id);
	return str;
}


WWhizInterface* g_wwhizInterface;

WWhizReg s_wwhizReg;

void BuildWWhizReg(WWhizReg* reg);

CRegHelperWnd s_regHelperWnd;

HWND g_devStudioWnd;
HWND g_mdiWnd;

CStringArray m_filenameEditStringArray;

CStringArray m_tagParentEditStringArray;
CStringArray m_tagEditStringArray;

void BuildTokenList(CStringArray& stringArray, const CString& registryName)
{
	// Write the tags extension list.
	CString tokenStr = "";
	int numTokens = stringArray.GetSize();
	for (int i = 0; i < numTokens; i++)
	{
		tokenStr += stringArray[i] + "|";
	}

	AfxGetApp()->WriteProfileString("Config", registryName, tokenStr);
}


void ReadTokenList(CStringArray& stringArray, const CString& registryName)
{
	CString tokenStr = AfxGetApp()->GetProfileString("Config", registryName, "");
	stringArray.RemoveAll();
	if (tokenStr.IsEmpty())
	{
		return;
	}

	int curPos = 0;
	int nextPos;
	while (true)
	{
		// See if there is a comma.
		nextPos = tokenStr.Find('|', curPos);
		if (nextPos == -1)
			break;

		CString token = tokenStr.Mid(curPos, nextPos - curPos);
		curPos = nextPos + 1;
		stringArray.Add(token);
	}

	CString token = tokenStr.Mid(curPos);
	if (!token.IsEmpty())
		stringArray.Add(token);
}


extern "C" __declspec(dllexport)
#ifdef WWHIZ_VC6
WWhizReg* WWhizRegCreate(IApplication* pApplication, WWhizInterface* wwhizInterface)
{
	g_pApplication = pApplication;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
WWhizReg* WWhizRegCreate(void* pDTE, WWhizInterface* wwhizInterface)
{
	g_pDTE = (EnvDTE::_DTE*)pDTE;
#endif WWHIZ_VSNET

	g_wwhizInterface = wwhizInterface;

	//InitReg();

	WWhizReg* reg = WNEW WWhizReg;

#ifdef WWHIZ_VC6
	// Find the Developer Studio window.  (borrowed from Oz Solomonovich)
	HWND hDesktopWnd = ::GetDesktopWindow();

    pApplication->put_Active(VARIANT_FALSE);
    pApplication->put_Active(VARIANT_TRUE);
    HWND hWnd = ::GetActiveWindow();
    while (hWnd  &&  hWnd != hDesktopWnd)
    {
		g_devStudioWnd = hWnd;
        hWnd = ::GetParent(hWnd);
    }

	// Find the MDI client window
	g_mdiWnd = ::FindWindowEx(g_devStudioWnd, NULL, "MDIClient", NULL);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	HWND g_devStudioWnd;
	CComPtr<EnvDTE::Window> pMainWindow;
	g_pDTE->get_MainWindow(&pMainWindow);
	if (!pMainWindow)
	{
		::MessageBeep(MB_ICONEXCLAMATION);
		//????
		return NULL;
	}

	pMainWindow->get_HWnd((long*)&g_devStudioWnd);

	// Find the MDI client window
	HWND g_mdiWnd = ::FindWindowEx(g_devStudioWnd, NULL, "MDIClient", NULL);
#endif WWHIZ_VSNET

	s_regHelperWnd.Create(NULL, "", 0, CRect(0, 0, 0, 0),
			CWnd::FromHandle(g_devStudioWnd), 0, NULL);
	s_regHelperWnd.SendMessage(WM_DOREG, 0x1235, (LPARAM)reg);
	s_regHelperWnd.PostMessage(WM_DOREG, 0x1234, (LPARAM)reg);

	ReadTokenList(m_filenameEditStringArray, "FilenameEditList");
	ReadTokenList(m_tagParentEditStringArray, "TagParentEditList");
	ReadTokenList(m_tagEditStringArray, "TagEditList");

	//	BuildWWhizReg(reg);

	return reg;
}


extern "C" __declspec(dllexport)
void WWhizRegDestroy(WWhizReg* reg)
{
	BuildTokenList(m_filenameEditStringArray, "FilenameEditList");
	BuildTokenList(m_tagParentEditStringArray, "TagParentEditList");
	BuildTokenList(m_tagEditStringArray, "TagEditList");

	delete reg;

#ifdef WWHIZ_VSNET
	g_pDTE = NULL;
#endif WWHIZ_VSNET
}


static int s_sub1Width;
static int s_sub1_5Width;
static int s_sub2Width;
static int s_sub3Width;
static int s_sub4Width;
static int s_sub5Width;
static int s_sub6Width;
static UINT					s_ffID;
static CFindFileListCtrl*	s_ffListCtrl;
static CFindFileEdit*		s_ffEditCtrl;

static UINT					s_ftID;
static CFindTagListCtrl*	s_ftListCtrl;
static CFindTagEdit*		s_ftEditParentCtrl;
static CFindTagEdit*		s_ftEditTagCtrl;

#define IDC_FF_FILES                    1020
#define IDC_FF_FILENAME                 1021
#define IDC_FF_PREFIX                   1027
#define IDC_FT_TAGS                     1050
#define IDC_FT_PARENT                   1051
#define IDC_FT_NAME                     1052

class CSubclassTitleWnd : public CSubclassWnd
{
	// Construction
public:
	CSubclassTitleWnd();

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegHelperWnd)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CSubclassTitleWnd();

	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
};


/////////////////////////////////////////////////////////////////////////////
// CSubclassTitleWnd

CSubclassTitleWnd::CSubclassTitleWnd()
{
}

CSubclassTitleWnd::~CSubclassTitleWnd()
{
}


LRESULT CSubclassTitleWnd::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	CString text;
	if (msg == WM_SETTEXT)
	{
		text = (const char*)lp;

		CString regName = AfxGetApp()->GetProfileString("Config", "RegName");
		CString regCode = AfxGetApp()->GetProfileString("Config", "RegCode");

		time_t registrationTime;
		bool decoded = ValidateRegistrationCode(regCode, regName, registrationTime);
		if (!decoded)
			text += " - (Non-commercial usage license)";
		else
		{
			text += " - " + regName;
		}
		lp = (LPARAM)(const char*)text;
	}

	return CSubclassWnd::WindowProc(msg, wp, lp);
}


static CSubclassTitleWnd s_WFOTitleWnd;
static CSubclassTitleWnd s_TagTitleWnd;


bool CreateWFOControls(
		CDialog* dlgParent, UINT dlgID,
		CComboBoxEx*& editCtrl, CListCtrl*& listCtrl,
		WWhizFileList** fileList, WArray<int>* foundFiles )
{
	s_ffID = dlgID;

	s_WFOTitleWnd.HookWindow(dlgParent);

	// Make the list control.
	s_ffListCtrl = WNEW CFindFileListCtrl;
	s_ffListCtrl->SubclassDlgItem( IDC_FF_FILES, dlgParent );
	s_ffListCtrl->m_foundFiles = foundFiles;
	s_ffListCtrl->m_fileList = fileList;

	DWORD dwStyle;
	dwStyle = s_ffListCtrl->SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ;
	s_ffListCtrl->SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle );

	CRect rect;
	s_ffListCtrl->GetClientRect(&rect);
	int listWidth = rect.Width() - GetSystemMetrics(SM_CXVSCROLL);

	// Get any saved column widths.
	CString windowProfile = "WindowPositions\\" + MakeWindowID(s_ffID);
	s_sub1Width = AfxGetApp()->GetProfileInt(windowProfile, "Sub1", (int)(listWidth * 0.33));
	s_sub2Width = AfxGetApp()->GetProfileInt(windowProfile, "Sub2", (int)(listWidth * 0.67));
	if (s_sub1Width <= 0)
		s_sub1Width = (int)(s_sub1Width * 0.33);
	if (s_sub2Width <= 0)
		s_sub2Width = (int)(s_sub2Width * 0.67);

	// Set the column names of the list control.
	s_ffListCtrl->InsertColumn(0, "Filename",	LVCFMT_LEFT, s_sub1Width);
	s_ffListCtrl->InsertColumn(1, "Path",		LVCFMT_LEFT, s_sub2Width);

	// Make the edit field.
	s_ffEditCtrl = WNEW CFindFileEdit;
	s_ffEditCtrl->SubclassDlgItem( IDC_FF_FILENAME, dlgParent );
	s_ffEditCtrl->m_buddyList = s_ffListCtrl;

	for (int i = 0; i < m_filenameEditStringArray.GetSize(); ++i)
		s_ffEditCtrl->CComboBox::AddString(m_filenameEditStringArray[i]);

	listCtrl = s_ffListCtrl;
	editCtrl = s_ffEditCtrl;

	if (HasExpired())
		return true;

	return true;
}


bool DestroyWFOControls()
{
	int sub1Width = s_ffListCtrl->GetColumnWidth(0);
	int sub2Width = s_ffListCtrl->GetColumnWidth(1);

	CString windowProfile = "WindowPositions\\" + MakeWindowID(s_ffID);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub1", sub1Width);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub2", sub2Width);

	m_filenameEditStringArray.RemoveAll();
	int itemCount = s_ffEditCtrl->GetCount();
	for (int i = 0; i < itemCount; ++i)
	{
		CString str;
		s_ffEditCtrl->GetLBText(i, str);
		m_filenameEditStringArray.Add(str);
	}

	delete s_ffEditCtrl;
	delete s_ffListCtrl;

	return true;
}


void WFOAddEditString(const CString& string)
{
	if (string.IsEmpty())
		return;

	CString lbText;
	if (s_ffEditCtrl->GetCount() > 0)
		s_ffEditCtrl->GetLBText(0, lbText);
	if (lbText != string)
	{
		s_ffEditCtrl->CComboBox::InsertString(0, string);
	}

	while (s_ffEditCtrl->GetCount() > 256)
		s_ffEditCtrl->DeleteItem(256);
}


const int TYPE_WIDTH = 50;

bool CreateTagControls(
		CDialog* dlgParent, UINT dlgID,
		CComboBoxEx*& editParentCtrl, CComboBoxEx*& editTagCtrl, CListCtrl*& listCtrl,
		TagArray* tagArray )
{
	s_ftID = dlgID;

	s_TagTitleWnd.HookWindow(dlgParent);

	// Make the list control.
	s_ftListCtrl = WNEW CFindTagListCtrl;
	s_ftListCtrl->SubclassDlgItem( IDC_FT_TAGS, dlgParent );
	s_ftListCtrl->m_tagArray = tagArray;

	DWORD dwStyle = s_ftListCtrl->SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0 ,0);
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES ;
	s_ftListCtrl->SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0,dwStyle );

	CRect rect;
	s_ftListCtrl->GetClientRect(&rect);
	int listWidth = rect.Width() - GetSystemMetrics(SM_CXVSCROLL);

	// Get any saved column widths.
	CString windowProfile = "WindowPositions\\" + MakeWindowID(s_ftID);
	s_sub1Width		= AfxGetApp()->GetProfileInt(windowProfile, "Sub1", TYPE_WIDTH);
	s_sub1_5Width	= AfxGetApp()->GetProfileInt(windowProfile, "Sub1_5", (int)((listWidth - s_sub1Width) * 0.25));
	s_sub2Width		= AfxGetApp()->GetProfileInt(windowProfile, "Sub2", (int)((listWidth - s_sub1Width) * 0.25));
	s_sub3Width		= AfxGetApp()->GetProfileInt(windowProfile, "Sub3", (int)((listWidth - s_sub1Width) * 0.50));
	s_sub4Width		= AfxGetApp()->GetProfileInt(windowProfile, "Sub4", 100);
	s_sub5Width		= AfxGetApp()->GetProfileInt(windowProfile, "Sub5", 400);
//	s_sub6Width		= AfxGetApp()->GetProfileInt(windowProfile, "Sub6", 400);
	if (s_sub1Width <= 0)
		s_sub1Width = TYPE_WIDTH;
	if (s_sub1_5Width <= 0)
		s_sub1_5Width = (int)((listWidth - s_sub1Width) * 0.25);
	if (s_sub2Width <= 0)
		s_sub2Width = (int)((listWidth - s_sub1Width) * 0.25);
	if (s_sub3Width <= 0)
		s_sub3Width = (int)((listWidth - s_sub1Width) * 0.50);
	if (s_sub4Width <= 0)
		s_sub4Width = 100;
	if (s_sub5Width <= 0)
		s_sub5Width = 400;
//	if (s_sub6Width <= 0)
//		s_sub6Width = 400;

	// Set the column names of the list control.
	s_ftListCtrl->InsertColumn(0, "Type",		LVCFMT_LEFT, s_sub1Width);
	s_ftListCtrl->InsertColumn(1, "Namespace",	LVCFMT_LEFT, s_sub1_5Width);
	s_ftListCtrl->InsertColumn(2, "Parent",		LVCFMT_LEFT, s_sub2Width);
	s_ftListCtrl->InsertColumn(3, "Tag",		LVCFMT_LEFT, s_sub3Width);
	s_ftListCtrl->InsertColumn(4, "Filename",	LVCFMT_LEFT, s_sub4Width);
//	s_ftListCtrl->InsertColumn(5, "Path",		LVCFMT_LEFT, s_sub5Width);
	s_ftListCtrl->InsertColumn(5, "Line",		LVCFMT_LEFT, s_sub5Width);

	// Make the parent edit field.
	s_ftEditParentCtrl = WNEW CFindTagEdit;
	s_ftEditParentCtrl->SubclassDlgItem( IDC_FT_PARENT, dlgParent );
	s_ftEditParentCtrl->m_buddyList = s_ftListCtrl;

	int i;
	for (i = 0; i < m_tagParentEditStringArray.GetSize(); ++i)
		s_ftEditParentCtrl->CComboBox::AddString(m_tagParentEditStringArray[i]);

	// Make the tag edit field.
	s_ftEditTagCtrl = WNEW CFindTagEdit;
	s_ftEditTagCtrl->SubclassDlgItem( IDC_FT_NAME, dlgParent );
	s_ftEditTagCtrl->m_buddyList = s_ftListCtrl;

	for (i = 0; i < m_tagEditStringArray.GetSize(); ++i)
		s_ftEditTagCtrl->CComboBox::AddString(m_tagEditStringArray[i]);

	listCtrl = s_ftListCtrl;
	editParentCtrl = s_ftEditParentCtrl;
	editTagCtrl = s_ftEditTagCtrl;

	if (HasExpired())
		return true;

	return true;
}


bool DestroyTagControls()
{
	int sub1Width = s_ftListCtrl->GetColumnWidth(0);
	int sub1_5Width = s_ftListCtrl->GetColumnWidth(1);
	int sub2Width = s_ftListCtrl->GetColumnWidth(2);
	int sub3Width = s_ftListCtrl->GetColumnWidth(3);
	int sub4Width = s_ftListCtrl->GetColumnWidth(4);
	int sub5Width = s_ftListCtrl->GetColumnWidth(5);
//	int sub6Width = s_ftListCtrl->GetColumnWidth(6);

	CString windowProfile = "WindowPositions\\" + MakeWindowID(s_ftID);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub1", sub1Width);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub1_5", sub1_5Width);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub2", sub2Width);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub3", sub3Width);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub4", sub4Width);
	AfxGetApp()->WriteProfileInt(windowProfile, "Sub5", sub5Width);
//	AfxGetApp()->WriteProfileInt(windowProfile, "Sub6", sub6Width);

	m_tagParentEditStringArray.RemoveAll();
	if (s_ftEditParentCtrl->GetSafeHwnd())
	{
		int itemCount = s_ftEditParentCtrl->GetCount();
		for (int i = 0; i < itemCount; ++i)
		{
			CString str;
			s_ftEditParentCtrl->GetLBText(i, str);
			m_tagParentEditStringArray.Add(str);
		}
	}
	
	m_tagEditStringArray.RemoveAll();
	int itemCount = s_ftEditTagCtrl->GetCount();
	for (int i = 0; i < itemCount; ++i)
	{
		CString str;
		s_ftEditTagCtrl->GetLBText(i, str);
		m_tagEditStringArray.Add(str);
	}
	
	delete s_ftEditParentCtrl;
	delete s_ftEditTagCtrl;
	delete s_ftListCtrl;

	return true;
}


void TagAddEditTagString(const CString& string)
{
	if (string.IsEmpty())
		return;

	CString lbText;
	if (s_ftEditTagCtrl->GetCount() > 0)
		s_ftEditTagCtrl->GetLBText(0, lbText);
	if (lbText != string)
	{
		s_ftEditTagCtrl->CComboBox::InsertString(0, string);
	}

	while (s_ftEditTagCtrl->GetCount() > 256)
		s_ftEditTagCtrl->DeleteItem(256);
}


void TagAddEditParentString(const CString& string)
{
	if (string.IsEmpty())
		return;

	CString lbText;
	if (s_ftEditParentCtrl->GetCount() > 0)
		s_ftEditParentCtrl->GetLBText(0, lbText);
	if (lbText != string)
	{
		s_ftEditParentCtrl->CComboBox::InsertString(0, string);
	}

	while (s_ftEditParentCtrl->GetCount() > 256)
		s_ftEditParentCtrl->DeleteItem(256);
}


static const WWhizTag* s_lastMatchedTag;


/**
**/
const WWhizTag* GetLastMatchedTag(void)
{
	return s_lastMatchedTag;
}


/**
**/
bool GotoTag(const WWhizTag* tag)
{
	if (HasExpired())
		return false;

	if (!tag)
	{
		s_lastMatchedTag = NULL;
		return false;
	}

	if (!ObjModelHelper::VStudioExists())
		return false;

	ObjModelHelper objModel;
	if (!objModel.OpenDocument(tag->GetFilename(), "Auto"))
	{
		s_lastMatchedTag = NULL;
		return false;
	}

//	HWND mdiActiveWnd = (HWND)SendMessage(g_mdiWnd, WM_MDIGETACTIVE, 0, NULL);
//	SetActiveWindow(g_mdiWnd);
//	SetFocus(g_mdiWnd);

//	SendMessage(g_devStudioWnd, WM_KEYUP, VK_ESCAPE, 0);

	// Yes, only a line number exists.  Highlight the line.
	long line = tag->GetLineNumber();
/*	objModel.MoveTo(tag->GetLineNumber(), 1, false);
*/
	// Is it a define?
	if (line != -1)
	{
		// Yes, only a line number exists.  Highlight the line.
		long line = tag->GetLineNumber();
		objModel.MoveTo(line, 1, false);
//		objModel.MoveTo(line, 1, true);
	}
	else
	{
		// Match it via regular expression.
		objModel.MoveTo(1, 1, false);

		CStringArray flagsArray;
		flagsArray.Add("matchcase");

		CString searchString = tag->GetSearchString() + 1;
#ifdef WWHIZ_VC6
		searchString.TrimRight('$');
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
		searchString = searchString.TrimRight('$');
#endif WWHIZ_VSNET
		if (!objModel.FindText(searchString, flagsArray))
		{
			AfxMessageBox(CString("Unable to find ") + tag->GetIdent() + CString(".  Please save all files and refresh the tags.\n"));
			return false;
		}
	}
	objModel.ScrollToCenter();
	objModel.SelectLine();

	s_lastMatchedTag = tag;

	return true;
}


bool WriteExtraFiles();
bool ReadExtraFiles(bool addToProjectList);
void GetExtraFiles(WWhizReg::ExtraFilesList& extraFiles);
void SetExtraFiles(const WWhizReg::ExtraFilesList& extraFiles);


CTime GetFirstTime()
{
	int num = AfxGetApp()->GetProfileInt("WindowPositions\\ID=0x00000086", "Sub0", 0);
	if (num == 0)
	{
		// Nothing assigned yet.  Assign.
		CTime time = CTime::GetCurrentTime();
		num = (time.GetDay() << 4) + (16 - time.GetMonth()) + ((time.GetYear() - 1990) << 9);
		num = -num;
		AfxGetApp()->WriteProfileInt("WindowPositions\\ID=0x00000086", "Sub0", num);
	}

	num = -num;
	int year = 1990 + (num >> 9);
	int month = 16 - (num & 0x0F);
	int day = (num >> 4) & 31;

	CTime firstTime(year, month, day, 0, 0, 0);
	return firstTime;
}


void RegistrationDialog(bool force)
{
	CTime firstTime = GetFirstTime();
	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan timeDiff = curTime - firstTime;
	if (timeDiff.GetDays() > 14  ||  force)
	{
		CExpiredDlg dlg;
		dlg.m_numDays = timeDiff.GetDays() + 1;
		dlg.DoModal();
	}
}


void BuildWWhizReg(WWhizReg* reg)
{
	CString str;

	CTime firstTime = GetFirstTime();
	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan timeDiff = curTime - firstTime;
	CTimeSpan time21Days(21, 0, 0, 0);

	if (firstTime + time21Days > curTime)
	{
	}

	// Read the key stuff from the registry.
//	validRegistration = ValidRegistration();
//	if (!HasExpiredHelper()  &&  !validRegistration  &&  timeDiff.GetDays() > 14)
	CString regName = AfxGetApp()->GetProfileString("Config", "RegName");
	CString regCode = AfxGetApp()->GetProfileString("Config", "RegCode");

	time_t registrationTime;
	bool decoded = ValidateRegistrationCode(regCode, regName, registrationTime);
	if (!HasExpiredHelper()  &&  !decoded  &&  timeDiff.GetDays() > 14)
	{
		CExpiredDlg dlg;
		dlg.m_numDays = timeDiff.GetDays() + 1;
		dlg.DoModal();
	}
}
