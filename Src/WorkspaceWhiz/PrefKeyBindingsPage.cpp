///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefKeyBindingsPage.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefKeyBindingsPage.cpp $
// $Date: 2003/01/08 $ $Revision: #12 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "PrefKeyBindingsPage.h"
#include "PrefKeyBindingsHotKeyCtrl.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefKeyBindingsPage property page

IMPLEMENT_DYNCREATE(CPrefKeyBindingsPage, CPropertyPage)

CPrefKeyBindingsPage::CPrefKeyBindingsPage()
{
	m_subclassedHotKeys.SetCount(WWhizCommands::GetCommandCount());
}

CPrefKeyBindingsPage::CPrefKeyBindingsPage(int which) :
	CHtmlHelpPropPage(CPrefKeyBindingsPage::IDD + which)
{
	//{{AFX_DATA_INIT(CPrefKeyBindingsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_subclassedHotKeys.SetCount(WWhizCommands::GetCommandCount());
}

CPrefKeyBindingsPage::~CPrefKeyBindingsPage()
{
}

void CPrefKeyBindingsPage::DoDataExchange(CDataExchange* pDX)
{
	CHtmlHelpPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefKeyBindingsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefKeyBindingsPage, CHtmlHelpPropPage)
	//{{AFX_MSG_MAP(CPrefKeyBindingsPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PKB_ASSIGN, OnPkbAssign)
	ON_BN_CLICKED(IDC_PKB_RESET, OnPkbReset)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PKB_KEYCHECK1, IDC_PKB_KEYCHECK21, OnPkbKeycheck)
	ON_BN_CLICKED(IDC_PKB_ASSIGNALLPAGES, OnBnClickedPkbAssignallpages)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefKeyBindingsPage message handlers

BOOL CPrefKeyBindingsPage::OnInitDialog() 
{
	CHtmlHelpPropPage::OnInitDialog();
	
	m_firstItem = 0;
	m_lastItem = -1;
	for (int i = 1; i < WWhizCommands::GetCommandCount(); i++)
	{
		CWnd* wnd = GetDlgItem(IDC_PKB_KEY1 + i - 1);
		if (wnd)
		{
			m_firstItem = i;
			break;
		}
	}
	
	for (i = m_firstItem + 1; i < WWhizCommands::GetCommandCount(); i++)
	{
		CWnd* wnd = GetDlgItem(IDC_PKB_KEY1 + i - 1);
		if (!wnd)
		{
			m_lastItem = i;
			break;
		}
	}
	if (m_lastItem == -1)
		m_lastItem = WWhizCommands::GetCommandCount();

	for (i = m_firstItem; i < m_lastItem; i++)
	{
		m_subclassedHotKeys[i].SubclassDlgItem(IDC_PKB_KEY1 + i - 1, this);
	}
		
	FillInControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPrefKeyBindingsPage::FillInControls()
{
	for (int i = m_firstItem; i < m_lastItem; i++)
	{
		bool assignKey = g_config.m_keys[i].m_assignKey;

		((CButton*)GetDlgItem(IDC_PKB_KEYCHECK1 + i - 1))->SetCheck(assignKey);

		CHotKeyCtrl* hkCtrl = (CHotKeyCtrl*)GetDlgItem(IDC_PKB_KEY1 + i - 1);
		hkCtrl->EnableWindow(assignKey);

		WORD vk;
		WORD mod;
		TranslateDSToHK(g_config.m_keys[i].m_keyStr, vk, mod);

		hkCtrl->SetHotKey(vk, mod);
	}
}


void CPrefKeyBindingsPage::OnDestroy() 
{
	CHtmlHelpPropPage::OnDestroy();
}


BOOL CPrefKeyBindingsPage::OnApply() 
{
	return CHtmlHelpPropPage::OnApply();
}


void CPrefKeyBindingsPage::OnPkbKeycheck(UINT nID) 
{
	int which = nID - IDC_PKB_KEYCHECK1;
	BOOL checked = ((CButton*)GetDlgItem(IDC_PKB_KEYCHECK1 + which))->GetCheck();
	GetDlgItem(IDC_PKB_KEY1 + which)->EnableWindow(checked);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Borrowed, in part, from Window Tabs.
struct VKName
{
    UINT    m_vk;
    LPCTSTR m_name;
	bool	m_extended;
};

#define DEF_VK1(vk)         { VK_##vk, #vk, false }
#define DEF_VK2(vk, name, ext)   { VK_##vk, name, ext }
#define DEF_VK3(code, name) { code, name }

// This list translates VKs to their DevStudio names.
VKName VKNames[] =
{
    DEF_VK1(BACK),
    DEF_VK1(TAB),
    DEF_VK1(CLEAR),
    DEF_VK2(RETURN,			"Enter", false),
    DEF_VK1(PAUSE),
    DEF_VK2(CAPITAL,        "Caps Lock", false),
    DEF_VK2(ESCAPE,         "Esc", false),
    DEF_VK1(SPACE),
    DEF_VK1(END),
    DEF_VK1(HOME),
    DEF_VK2(LEFT,           "Left Arrow", true),
    DEF_VK2(UP,             "Up Arrow", true),
    DEF_VK2(RIGHT,          "Right Arrow", true),
    DEF_VK2(DOWN,           "Down Arrow", true),
    DEF_VK1(INSERT),
    DEF_VK2(DELETE,         "Del", true),
    DEF_VK2(NUMPAD0,        "Num0", false),
    DEF_VK2(NUMPAD1,        "Num1", false),
    DEF_VK2(NUMPAD2,        "Num2", false),
    DEF_VK2(NUMPAD3,        "Num3", false),
    DEF_VK2(NUMPAD4,        "Num4", false),
    DEF_VK2(NUMPAD5,        "Num5", false),
    DEF_VK2(NUMPAD6,        "Num6", false),
    DEF_VK2(NUMPAD7,        "Num7", false),
    DEF_VK2(NUMPAD8,        "Num8", false),
    DEF_VK2(NUMPAD9,        "Num9", false),
    DEF_VK2(MULTIPLY,       "Num *", false),
    DEF_VK2(ADD,            "Num +", false),
    DEF_VK2(SUBTRACT,       "Num -", false),
    DEF_VK2(DECIMAL,        "Num .", false),
    DEF_VK2(DIVIDE,         "Num /", false),
    DEF_VK1(F1),
    DEF_VK1(F2),
    DEF_VK1(F3),
    DEF_VK1(F4),
    DEF_VK1(F5),
    DEF_VK1(F6),
    DEF_VK1(F7),
    DEF_VK1(F8),
    DEF_VK1(F9),
    DEF_VK1(F10),
    DEF_VK1(F11),
    DEF_VK1(F12),
    DEF_VK2(NUMLOCK,        "Num Lock", true),
    DEF_VK2(SCROLL,         "Scroll Lock", true),
    DEF_VK2(NEXT,           "Page Down", true),
    DEF_VK2(PRIOR,          "Page Up", true),

    // undocumented VKs
    DEF_VK3(186,            ";"),
    DEF_VK3(187,            "="),
    DEF_VK3(188,            ","),
    DEF_VK3(189,            "-"),
    DEF_VK3(190,            "."),
    DEF_VK3(191,            "/"),
    DEF_VK3(192,            "`"),
    DEF_VK3(219,            "["),
    DEF_VK3(220,            "\\"),
    DEF_VK3(221,            "]"),
    DEF_VK3(222,            "'")
};
const int vkNamesCount = _countof(VKNames);

void CPrefKeyBindingsPage::TranslateHKToDS(CString& keyStr, WORD vk, WORD mod)
{
    if (vk == 0)  // no key?
        return;

    // start with a fresh string
    int      i;

    keyStr.Empty();

    // translate modifiers:
    if (mod & HOTKEYF_CONTROL)  keyStr += "CTRL+";
    if (mod & HOTKEYF_ALT)      keyStr += "ALT+";
    if (mod & HOTKEYF_SHIFT)    keyStr += "SHIFT+";

    // translate m_vk:
	if (isalnum(vk))
	{
		keyStr += (TCHAR)vk;
	}
    else
    {
        // use m_vk map
        for (i = 0; i < vkNamesCount; i++)
        {
            if (VKNames[i].m_vk == vk)
                break;
        }

        if (i >= vkNamesCount)
        {
            _RPTF1(_CRT_ERROR,
                "Could not identify virtual key (m_vk=%d).", vk);
            return;
        }

        keyStr += VKNames[i].m_name;
    }
}


void CPrefKeyBindingsPage::TranslateDSToHK(CString keyStr, WORD& vk, WORD& mod)
{
	mod = 0;
	vk = 0;

	if (keyStr.IsEmpty())
		return;
	
	// Build the modifier by cycling through the + symbols.
	int lastPlusPos = 0;
	int plusPos = keyStr.Find('+', lastPlusPos);
	while (plusPos != -1)
	{
		CString str = keyStr.Mid(lastPlusPos, plusPos - lastPlusPos);
		str.MakeUpper();
		if (str == "CTRL")
			mod |= HOTKEYF_CONTROL;
		else if (str == "ALT")
			mod |= HOTKEYF_ALT;
		else if (str == "SHIFT")
			mod |= HOTKEYF_SHIFT;
		
		lastPlusPos = plusPos + 1;
		plusPos = keyStr.Find('+', lastPlusPos);
	}
    
	// Now, build the virtual key code.
	CString vkStr = keyStr.Mid(lastPlusPos);

	// Potentially a letter or number.
	if (vkStr.GetLength() == 1)
	{
		if (isalnum(vkStr[0]))
			vk = (WORD)vkStr[0];
	}

	// Translated yet?
	if (vk == 0)
	{
        // use m_vk map
        for (int i = 0; i < vkNamesCount; i++)
        {
            if (vkStr.CompareNoCase(VKNames[i].m_name) == 0)
                break;
        }

        if (i >= vkNamesCount)
        {
            _RPTF1(_CRT_ERROR,
                "Could not identify virtual key (m_vk=%d).", vk);
            return;
        }

		vk = VKNames[i].m_vk;

		// Is it extended?
		mod |= (VKNames[i].m_extended ? HOTKEYF_EXT : 0);
//		UINT scanCode = MapVirtualKey(vk, 0);
//		scanCode = scanCode;
	}
}



void CPrefKeyBindingsPage::OnPkbAssign() 
{
	const WWhizCommands::CommandInfo* comInfo = WWhizCommands::GetCommandList();
	for (int i = m_firstItem; i < m_lastItem; i++)
	{
		BOOL checked = ((CButton*)GetDlgItem(IDC_PKB_KEYCHECK1 + i - 1))->GetCheck();
		g_config.m_keys[i].m_assignKey = (checked != FALSE);

		if (g_config.m_keys[i].m_assignKey)
		{
			WORD vk;
			WORD mod;
			m_subclassedHotKeys[i].GetHotKey(vk, mod);

			CString keyStr;
			TranslateHKToDS(keyStr, vk, mod);

			g_config.m_keys[i].m_keyStr = keyStr;
		}
	}

	for (i = m_firstItem; i < m_lastItem; i++)
	{
		if (!g_config.m_keys[i].m_assignKey)
			continue;
		CString keyStr = g_config.m_keys[i].m_keyStr;
		int plusPos = keyStr.ReverseFind('+');
		CString endPart = keyStr.Mid(plusPos + 1);
#ifdef WWHIZ_VC6
		if (endPart.GetLength() == 1)
		{
			// DevStudio only supports alphanumeric for some oddball reason.
			if (!isalnum(endPart[0]))
			{
				AfxMessageBox(
					"Unfortunately, Workspace Whiz is not able to add the "
					"key binding [ " + keyStr + " ] for command " + comInfo[i].m_name +
					" due to a bug in Developer Studio.  Please assign this key "
					"by hand through the Tools menu->Customize->Keyboard dialog.");

				continue;
			}
		}
#endif WWHIZ_VC6

		ObjModelHelper::SetKeyBinding(comInfo[i].m_justText, CString(comInfo[i].m_name), keyStr);
	}

	// Save the new configuration.
	g_config.SaveRegistry();
}

void CPrefKeyBindingsPage::OnPkbReset() 
{
	const WWhizCommands::CommandInfo* comList = WWhizCommands::GetCommandList();
	int comCount = WWhizCommands::GetCommandCount();

	for (int i = 1; i < comCount; i++)
	{
		// Grab the default.
		g_config.m_keys[i].m_keyStr = comList[i].m_keyStr;
		g_config.m_keys[i].m_assignKey = g_config.m_keys[i].m_keyStr.IsEmpty() != TRUE;
	}

	FillInControls();
}

void CPrefKeyBindingsPage::OnBnClickedPkbAssignallpages()
{
	const WWhizCommands::CommandInfo* comInfo = WWhizCommands::GetCommandList();
	for (int i = m_firstItem; i < m_lastItem; i++)
	{
		BOOL checked = ((CButton*)GetDlgItem(IDC_PKB_KEYCHECK1 + i - 1))->GetCheck();
		g_config.m_keys[i].m_assignKey = (checked != FALSE);
		if (g_config.m_keys[i].m_assignKey)
		{
			WORD vk;
			WORD mod;
			m_subclassedHotKeys[i].GetHotKey(vk, mod);

			CString keyStr;
			TranslateHKToDS(keyStr, vk, mod);

			g_config.m_keys[i].m_keyStr = keyStr;
		}
	}

	for ( i = 1; i < (int)g_config.m_keys.GetCount(); ++i )
	{
		if (!g_config.m_keys[i].m_assignKey)
			continue;
		CString keyStr = g_config.m_keys[i].m_keyStr;
		int plusPos = keyStr.ReverseFind('+');
		CString endPart = keyStr.Mid(plusPos + 1);
#ifdef WWHIZ_VC6
		if (endPart.GetLength() == 1)
		{
			// DevStudio only supports alphanumeric for some oddball reason.
			if (!isalnum(endPart[0]))
			{
				AfxMessageBox(
					"Unfortunately, Workspace Whiz is not able to add the "
					"key binding [ " + keyStr + " ] for command " + comInfo[i].m_name +
					" due to a bug in Developer Studio.  Please assign this key "
					"by hand through the Tools menu->Customize->Keyboard dialog.");

				continue;
			}
		}
#endif WWHIZ_VC6

		if (!ObjModelHelper::SetKeyBinding(comInfo[i].m_justText, CString(comInfo[i].m_name), keyStr))
		{
#ifdef WWHIZ_VSNET			
			AfxMessageBox("Workspace Whiz is unable to add the key binding [ " + keyStr + " ] "
					"for command " + comInfo[i].m_name + ".  The most likely reason for this "
					"is Visual Studio .NET maintains a set of read-only default key bindings.  "
					"Go to the Tools menu->Options->Keyboard and press the Save As button to "
					"generate a set of editable key bindings.");
			break;
#endif WWHIZ_VSNET
		}
	}

	// Save the new configuration.
	g_config.SaveRegistry();
}
