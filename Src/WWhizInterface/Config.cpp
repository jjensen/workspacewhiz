///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/Config.cpp $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizInterface.h"
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
	"DECLARE_MESSAGE_MAP+",
	"BEGIN_MESSAGE_MAP+",
	"END_MESSAGE_MAP+",
	"BEGIN_OBJECT_MAP+",
	"END_OBJECT_MAP+",
	"BEGIN_DISPATCH_MAP+",
	"END_DISPATCH_MAP+",
	"DECLARE_EVENTSINK_MAP+",
	"BEGIN_EVENTSINK_MAP+",
	"END_EVENTSINK_MAP+",
	"ATL_NO_VTABLE",
	"BEGIN_CONNECTION_POINT_MAP+",
	"END_CONNECTION_POINT_MAP+",
	"THIS",
	"THIS_",
	"__declspec+"
	"_declspec+"
};

///////////////////////////////////////////////////////////////////////////////
// File Extensions
///////////////////////////////////////////////////////////////////////////////
void WWhizConfigImpl::FilesExtReset()
{
	m_filesExtList.RemoveAll();
	for (size_t i = 0; i < _countof(s_extList); i++)
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
	for (size_t i = 0; i < m_filesExtList.GetCount(); i++)
	{
		if (ext == m_filesExtList[i])
			return;
	}

	m_filesExtList.Add(ext);
}


void WWhizConfigImpl::FilesExtRemove(size_t index)
{
	if (index >= m_filesExtList.GetCount())
		return;
	m_filesExtList.RemoveAt(index);
}


static CString s_emptyStr;

const CString& WWhizConfigImpl::FilesExtGet(size_t index)
{
	if (index >= m_filesExtList.GetCount())
		return s_emptyStr;
	return m_filesExtList[index];
}


size_t WWhizConfigImpl::FilesExtGetCount()
{
	return m_filesExtList.GetCount();
}


///////////////////////////////////////////////////////////////////////////////
// Tag Extensions
///////////////////////////////////////////////////////////////////////////////
void WWhizConfigImpl::TagsExtReset()
{
	m_tagsExtList.RemoveAll();
	for (size_t i = 0; i < _countof(s_extList); i++)
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
	for (size_t i = 0; i < (int)m_tagsExtList.GetCount(); i++)
	{
		if (ext == m_tagsExtList[i])
			return;
	}

	m_tagsExtList.Add(ext);
}


void WWhizConfigImpl::TagsExtRemove(size_t index)
{
	if (index >= m_tagsExtList.GetCount())
		return;
	m_tagsExtList.RemoveAt(index);
}


const CString& WWhizConfigImpl::TagsExtGet(size_t index)
{
	if (index >= m_tagsExtList.GetCount())
		return s_emptyStr;
	return m_tagsExtList[index];
}


size_t WWhizConfigImpl::TagsExtGetCount()
{
	return m_tagsExtList.GetCount();
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
	for (size_t i = 0; i < _countof(s_ignoreTokensList); i++)
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
	for (size_t i = 0; i < m_ignoreTokensList.GetCount(); i++)
	{
		if (ext == m_ignoreTokensList[i])
			return;
	}

	m_ignoreTokensList.Add(ext);
}


void WWhizConfigImpl::IgnoreTokensRemove(size_t index)
{
	if (index >= m_ignoreTokensList.GetCount())
		return;
	m_ignoreTokensList.RemoveAt(index);
}


const CString& WWhizConfigImpl::IgnoreTokensGet(size_t index)
{
	if (index >= m_ignoreTokensList.GetCount())
		return s_emptyStr;
	return m_ignoreTokensList[index];
}


size_t WWhizConfigImpl::IgnoreTokensGetCount()
{
	return m_ignoreTokensList.GetCount();
}


UINT WWhizConfigImpl::GetTagAutoSaveAmount()
{
	return m_tagAutoSaveAmount;
}


void WWhizConfigImpl::SetTagAutoSaveAmount(UINT amount)
{
	m_tagAutoSaveAmount = amount;
}


