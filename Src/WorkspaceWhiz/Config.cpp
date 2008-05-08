///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/Config.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 18   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "WorkspaceWhiz.h"
#include "Config.h"

Config::Config()
{
	m_keys.SetSize(GetCommandCount());
}


void Config::LoadRegistry()
{
	// Get DevStudio version.
	HMODULE hModule = GetModuleHandle("MSDEV.EXE");
	if (hModule)
	{
		CComBSTR bstrVersion;
		g_pApplication->get_Version(&bstrVersion);
		CString strVersion = bstrVersion;
		if (strVersion != "5.0")
		{
			m_dsFirstTime = AfxGetApp()->GetProfileInt("Config", "DS6FirstTime", true) != 0;
		}
		else
		{
			m_dsFirstTime = AfxGetApp()->GetProfileInt("Config", "DS5FirstTime", true) != 0;
		}
	}
	else
	{
		hModule = GetModuleHandle("EVC.EXE");
		if (hModule)
		{
			m_dsFirstTime = AfxGetApp()->GetProfileInt("Config", "EVC3FirstTime", true) != 0;
		}
	}
	
	const CommandInfo* comList = GetCommandList();
	int comCount = GetCommandCount();

	for (int i = 0; i < comCount; i++)
	{
		CString keyStr = AfxGetApp()->GetProfileString("Config\\Keys", comList[i].m_name, "");
		if (keyStr.IsEmpty())
		{
			// Grab the default.
			m_keys[i].m_keyStr = comList[i].m_keyStr;
			m_keys[i].m_assignKey = m_keys[i].m_keyStr.IsEmpty() != TRUE;
		}
		else
		{
			// Find the pipe symbol.
			int sepPos = keyStr.ReverseFind('|');

			if (keyStr[sepPos + 1] == '1')
				m_keys[i].m_assignKey = true;
			else
				m_keys[i].m_assignKey = false;

			m_keys[i].m_keyStr = keyStr.Left(sepPos);
		}
	}

	m_useFindTagAtCursorDialog = AfxGetApp()->GetProfileInt("Config", "UseFindTagAtCursorDialog", 1) == 1;

	///////////////////////////////////////////////////////////////////////////
	// Load WWhizInterface info.
	///////////////////////////////////////////////////////////////////////////
	WWhizConfig& config = g_wwhizInterface->GetConfig();

	// Read in the extension list.
	CString extStr = AfxGetApp()->GetProfileString("Config", "ExtensionList", "");
	if (extStr.IsEmpty())
	{
		config.FilesExtReset();
	}
	else
	{
		config.FilesExtRemoveAll();

		int curPos = 0;
		int nextPos;
		while (true)
		{
			// See if there is a comma.
			nextPos = CStringFind(extStr, ',', curPos);
			if (nextPos == -1)
				break;

			CString ext = extStr.Mid(curPos, nextPos - curPos);
			curPos = nextPos + 1;
			config.FilesExtAdd(ext);
		}

		CString lastExt = extStr.Mid(curPos);
		if (!lastExt.IsEmpty())
			config.FilesExtAdd(extStr.Mid(curPos));
	}

	// Read in the tags extension list.
	extStr = AfxGetApp()->GetProfileString("Config", "TagsExtensionList", "");
	if (extStr.IsEmpty())
	{
		config.TagsExtReset();
	}
	else
	{
		config.TagsExtRemoveAll();

		int curPos = 0;
		int nextPos;
		while (true)
		{
			// See if there is a comma.
			nextPos = CStringFind(extStr, ',', curPos);
			if (nextPos == -1)
				break;

			CString ext = extStr.Mid(curPos, nextPos - curPos);
			curPos = nextPos + 1;
			config.TagsExtAdd(ext);
		}

		CString lastExt = extStr.Mid(curPos);
		if (!lastExt.IsEmpty())
			config.TagsExtAdd(extStr.Mid(curPos));
	}

	// Read in the ignore tokens list.
	CString tokenStr = AfxGetApp()->GetProfileString("Config", "IgnoreTokensList", "");
	if (tokenStr.IsEmpty())
	{
		config.IgnoreTokensReset();
	}
	else
	{
		config.IgnoreTokensRemoveAll();

		int curPos = 0;
		int nextPos;
		while (true)
		{
			// See if there is a comma.
			nextPos = CStringFind(tokenStr, ',', curPos);
			if (nextPos == -1)
				break;

			CString token = tokenStr.Mid(curPos, nextPos - curPos);
			curPos = nextPos + 1;
			config.IgnoreTokensAdd(token);
		}

		CString lastToken = tokenStr.Mid(curPos);
		if (!lastToken.IsEmpty())
			config.IgnoreTokensAdd(tokenStr.Mid(curPos));
	}

	config.SetTagAutoPackAmount(AfxGetApp()->GetProfileInt("Config", "AutoPackTagsAmount", 1000));
	config.SetAutoRefreshTags(AfxGetApp()->GetProfileInt("Config", "AutoRefreshTags", 1) == 1);
	config.SetScanFilesTaskSwitch(AfxGetApp()->GetProfileInt("Config", "ScanFilesTaskSwitch", 1) == 1);
	m_autoSaveAllPerTagCommand = AfxGetApp()->GetProfileInt("Config", "AutoSaveAllPerTagCommand", 1) == 1;
	config.SetTagAutoSaveAmount(AfxGetApp()->GetProfileInt("Config", "AutoSaveTagsAmount", 100));
}


void Config::SaveRegistry()
{
	int i;
	
	const CommandInfo* comList = GetCommandList();
	int comCount = GetCommandCount();

	for (i = 0; i < comCount; i++)
	{
		CString writeStr = m_keys[i].m_keyStr;
		if (m_keys[i].m_assignKey)
			writeStr += "|1";
		else
			writeStr += "|0";
		AfxGetApp()->WriteProfileString("Config\\Keys", comList[i].m_name, writeStr);
	}

	AfxGetApp()->WriteProfileInt("Config", "UseFindTagAtCursorDialog", m_useFindTagAtCursorDialog);

	///////////////////////////////////////////////////////////////////////////
	// Save WWhizInterface info.
	///////////////////////////////////////////////////////////////////////////
	// Write the extension list.
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	CString extStr;
	int numExts = config.FilesExtGetCount();
	for (i = 0; i < numExts; i++)
	{
		extStr += config.FilesExtGet(i) + ",";
	}
	AfxGetApp()->WriteProfileString("Config", "ExtensionList", extStr);

	// Write the tags extension list.
	extStr = "";
	numExts = config.TagsExtGetCount();
	for (i = 0; i < numExts; i++)
	{
		extStr += config.TagsExtGet(i) + ",";
	}
	AfxGetApp()->WriteProfileString("Config", "TagsExtensionList", extStr);

	// Write the tags extension list.
	CString tokenStr = "";
	int numTokens = config.IgnoreTokensGetCount();
	for (i = 0; i < numTokens; i++)
	{
		tokenStr += config.IgnoreTokensGet(i) + ",";
	}
	AfxGetApp()->WriteProfileString("Config", "IgnoreTokensList", tokenStr);

	AfxGetApp()->WriteProfileInt("Config", "AutoPackTagsAmount", config.GetTagAutoPackAmount());
	AfxGetApp()->WriteProfileInt("Config", "AutoRefreshTags", config.GetAutoRefreshTags());
	AfxGetApp()->WriteProfileInt("Config", "ScanFilesTaskSwitch", config.GetScanFilesTaskSwitch());
	AfxGetApp()->WriteProfileInt("Config", "AutoSaveAllPerTagCommand", m_autoSaveAllPerTagCommand);
	AfxGetApp()->WriteProfileInt("Config", "AutoSaveTagsAmount", config.GetTagAutoSaveAmount());
}


void Config::SaveRegistry_FirstTime()
{
	// Get DevStudio version.
	HMODULE hModule = GetModuleHandle("MSDEV.EXE");
	if (hModule)
	{
		CComBSTR bstrVersion;
		g_pApplication->get_Version(&bstrVersion);
		CString strVersion = bstrVersion;
		if (strVersion != "5.0")
		{
			AfxGetApp()->WriteProfileInt("Config", "DS6FirstTime", m_dsFirstTime);
		}
		else
		{
			AfxGetApp()->WriteProfileInt("Config", "DS5FirstTime", m_dsFirstTime);
		}
	}
	else
	{
		hModule = GetModuleHandle("EVC.EXE");
		if (hModule)
		{
			AfxGetApp()->WriteProfileInt("Config", "EVC3FirstTime", m_dsFirstTime);
		}
	}
}
	

