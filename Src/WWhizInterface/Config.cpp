///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/Config.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 17   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "Config.h"

WWhizConfigImpl::WWhizConfigImpl()
:	m_tagAutoPackAmount(1000),
	m_autoRefreshTags(true),
	m_scanFilesTaskSwitch(true),
	m_tagAutoSaveAmount(100)
{
	FilesExtReset();
	TagsExtReset();
	IgnoreTokensReset();
}


// Set up our known extensions list.
static char* s_extList[] = { "cpp", "c", "cxx", "cc", "h", "hpp", "hh", "inl" };

// Set up our ignore tokens list.
static char* s_ignoreTokensList[] =
{
	"DECLARE_DYNAMIC+",
	"IMPLEMENT_DYNAMIC+",
	"DECLARE_DYNCREATE+",
	"IMPLEMENT_DYNCREATE+",
	"DECLARE_SERIAL+",
	"IMPLEMENT_SERIAL+",
	"_AFXWIN_INLINE",
};

///////////////////////////////////////////////////////////////////////////////
// File Extensions
///////////////////////////////////////////////////////////////////////////////
void WWhizConfigImpl::FilesExtReset()
{
	m_filesExtList.RemoveAll();
	for (int i = 0; i < _countof(s_extList); i++)
	{
		m_filesExtList.Add(CString(s_extList[i]));
	}
}


void WWhizConfigImpl::FilesExtRemoveAll()
{
	m_filesExtList.RemoveAll();
}


void WWhizConfigImpl::FilesExtAdd(CString ext)
{
	if (ext == "")
		return;
	ext.MakeLower();
	for (int i = 0; i < m_filesExtList.GetSize(); i++)
	{
		if (ext == m_filesExtList[i])
			return;
	}

	m_filesExtList.Add(ext);
}


void WWhizConfigImpl::FilesExtRemove(int index)
{
	if (index < 0  ||  index >= m_filesExtList.GetSize())
		return;
	m_filesExtList.RemoveAt(index);
}


static CString s_emptyStr;

const CString& WWhizConfigImpl::FilesExtGet(int index)
{
	if (index < 0  ||  index >= m_filesExtList.GetSize())
		return s_emptyStr;
	return m_filesExtList[index];
}


int WWhizConfigImpl::FilesExtGetCount()
{
	return m_filesExtList.GetSize();
}


///////////////////////////////////////////////////////////////////////////////
// Tag Extensions
///////////////////////////////////////////////////////////////////////////////
void WWhizConfigImpl::TagsExtReset()
{
	m_tagsExtList.RemoveAll();
	for (int i = 0; i < _countof(s_extList); i++)
	{
		m_tagsExtList.Add(CString(s_extList[i]));
	}
}


void WWhizConfigImpl::TagsExtRemoveAll()
{
	m_tagsExtList.RemoveAll();
}


void WWhizConfigImpl::TagsExtAdd(CString ext)
{
	if (ext == "")
		return;
	ext.MakeLower();
	for (int i = 0; i < m_tagsExtList.GetSize(); i++)
	{
		if (ext == m_tagsExtList[i])
			return;
	}

	m_tagsExtList.Add(ext);
}


void WWhizConfigImpl::TagsExtRemove(int index)
{
	if (index < 0  ||  index >= m_tagsExtList.GetSize())
		return;
	m_tagsExtList.RemoveAt(index);
}


const CString& WWhizConfigImpl::TagsExtGet(int index)
{
	if (index < 0  ||  index >= m_tagsExtList.GetSize())
		return s_emptyStr;
	return m_tagsExtList[index];
}


int WWhizConfigImpl::TagsExtGetCount()
{
	return m_tagsExtList.GetSize();
}


///////////////////////////////////////////////////////////////////////////////
// Misc
///////////////////////////////////////////////////////////////////////////////
UINT WWhizConfigImpl::GetTagAutoPackAmount()
{
	return m_tagAutoPackAmount;
}


void WWhizConfigImpl::SetTagAutoPackAmount(UINT amount)
{
	m_tagAutoPackAmount = amount;
}


bool WWhizConfigImpl::GetAutoRefreshTags()
{
	return m_autoRefreshTags;
}


void WWhizConfigImpl::SetAutoRefreshTags(bool autoRefresh)
{
	m_autoRefreshTags = autoRefresh;
}

	
bool WWhizConfigImpl::GetScanFilesTaskSwitch()
{
	return m_scanFilesTaskSwitch;
}


void WWhizConfigImpl::SetScanFilesTaskSwitch(bool scanFiles)
{
	m_scanFilesTaskSwitch = scanFiles;
}

	
///////////////////////////////////////////////////////////////////////////////
// Ignore Tokens List
///////////////////////////////////////////////////////////////////////////////
void WWhizConfigImpl::IgnoreTokensReset()
{
	m_ignoreTokensList.RemoveAll();
	for (int i = 0; i < _countof(s_ignoreTokensList); i++)
	{
		m_ignoreTokensList.Add(CString(s_ignoreTokensList[i]));
	}
}


void WWhizConfigImpl::IgnoreTokensRemoveAll()
{
	m_ignoreTokensList.RemoveAll();
}


void WWhizConfigImpl::IgnoreTokensAdd(CString ext)
{
	if (ext == "")
		return;
	for (int i = 0; i < m_ignoreTokensList.GetSize(); i++)
	{
		if (ext == m_ignoreTokensList[i])
			return;
	}

	m_ignoreTokensList.Add(ext);
}


void WWhizConfigImpl::IgnoreTokensRemove(int index)
{
	if (index < 0  ||  index >= m_ignoreTokensList.GetSize())
		return;
	m_ignoreTokensList.RemoveAt(index);
}


const CString& WWhizConfigImpl::IgnoreTokensGet(int index)
{
	if (index < 0  ||  index >= m_ignoreTokensList.GetSize())
		return s_emptyStr;
	return m_ignoreTokensList[index];
}


int WWhizConfigImpl::IgnoreTokensGetCount()
{
	return m_ignoreTokensList.GetSize();
}


UINT WWhizConfigImpl::GetTagAutoSaveAmount()
{
	return m_tagAutoSaveAmount;
}


void WWhizConfigImpl::SetTagAutoSaveAmount(UINT amount)
{
	m_tagAutoSaveAmount = amount;
}


