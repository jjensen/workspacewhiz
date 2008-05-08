///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizInterfaceImpl.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WWhizInterfaceImpl.cpp $
// $Date: 2003/01/05 $ $Revision: #10 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizInterface.h"
#include "WorkspaceInfo.h"
#include "WorkspaceTags.h"
#include "CompilerFiles.h"
#include "Config.h"

static DWORD s_wwhizVersion = 0x04000459;

class WWhizInterfaceImpl : public WWhizInterface
{
public:
	virtual void Reserved0();
	
	virtual WWhizProjectList& GetProjectList(void) const;
	virtual WWhizFileList& GetFileList(void) const;
	virtual CString GetWorkspaceName(void) const;
	virtual const CString& GetWorkspaceLocation(void) const;
	virtual void SetWorkspaceLocation(void);		// Do once on Workspace Open.
	virtual const CString& GetExtraFilename(void);
	virtual bool GetCurrentFilename(CString& filename) const;
	virtual WWhizProject* GetCurrentProject() const;
	virtual void ResolveFilename(const CString& rootDir, CString& filename);

	virtual bool RefreshFileList(void);
	virtual void AddProject(CString projectName);
	virtual void RemoveAllFiles(void);
	virtual void AddProject2(CString projectName, bool noRefresh);
		
	// Tags
	virtual const WWhizTag* GetTagMatchHead(void) const;
	virtual const WWhizTag* GetTagMatchTail(void) const;
	virtual int GetTagMatchCount(void) const;
	virtual const WWhizTag* GetLastMatchedTag(void) const;

	virtual const WWhizTagList& GetTagList(void) const;

	virtual void RemoveAllTags(void);

	virtual void MatchTag(const WWhizTagList& tags, LPCTSTR name, WWhizTag::Type forceType = WWhizTag::LAST_TYPE) const;
	virtual bool GotoTag(const WWhizTag* ident) const;

	virtual void SetTagCallback(TagRefreshCallback callback, void* userData);
	virtual void RefreshTagList(bool forceRefresh = false, bool forceSave = false);

	virtual void LoadTags();
	virtual void SaveTags();

	virtual bool GetTagUseType(WWhizTag::Type type);
	virtual void SetTagUseType(WWhizTag::Type type, bool show);

	// Global files
	virtual WWhizFileList& GetGlobalFileList(void) const;
	virtual bool RefreshGlobalFileList(void);

	// Misc
	virtual WWhizConfig& GetConfig();

	// More stuff appended to the end.
	virtual CTime GetLastFileRefreshTime() const;
	virtual CTime GetLastTagRefreshTime() const;
	virtual void SetWWhizReg(WWhizReg* wwhizReg);
	virtual WWhizFileList& GetActiveFileList(void) const;

	virtual DWORD GetVersion(void) const;

	virtual void AddChangedFile(CString filename);
	virtual void CheckActiveFilesForChanges();

	virtual const WWhizTag* GetTagScope(const CString& filename, UINT y);

	virtual const CString& GetWorkspaceFullPath(void) const;


protected:
	TagRefreshCallback m_tagRefreshCallback;

	WWhizConfigImpl m_config;
};


WWhizInterfaceImpl g_wwhizInterfaceImpl;

WWhizInterface& WorkspaceInfo::GetWWhizInterface()
{
	return g_wwhizInterfaceImpl;
}

	
void WWhizInterfaceImpl::Reserved0()
{
}

	
WWhizProjectList& WWhizInterfaceImpl::GetProjectList(void) const
{
	return WorkspaceInfo::GetProjectList();
}


WWhizFileList& WWhizInterfaceImpl::GetFileList(void) const
{
	return WorkspaceInfo::GetFileList();
}


CString WWhizInterfaceImpl::GetWorkspaceName(void) const
{
#ifdef WWHIZ_VC6
extern pfnGetWorkspaceName g_fnGetWorkspaceName;
	if (g_fnGetWorkspaceName)
	{
		CString str;
		LPTSTR buf = str.GetBuffer(_MAX_PATH);
		(*g_fnGetWorkspaceName)(buf, _MAX_PATH);
		str.ReleaseBuffer();
		OutputDebugString(str);

		return str;
	}

	CString empty;
	return empty;
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	if (!g_pDTE)
		return "";

	CComPtr<EnvDTE::_Solution> pSolution;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
	{
		return "";
	}

	CComBSTR solutionName;
	pSolution->get_FullName(&solutionName);
	return CString(solutionName);
#endif WWHIZ_VSNET
}

	
const CString& WWhizInterfaceImpl::GetWorkspaceLocation(void) const
{
	return WorkspaceInfo::GetWorkspaceLocation();
}


void WWhizInterfaceImpl::SetWorkspaceLocation(void)
{
	WorkspaceInfo::SetWorkspaceLocation();
}


const CString& WWhizInterfaceImpl::GetExtraFilename(void)
{
	return WorkspaceInfo::GetExtraFilename();
}


bool WWhizInterfaceImpl::GetCurrentFilename(CString& filename) const
{
	return WorkspaceInfo::GetCurrentFilename(filename);
}


WWhizProject* WWhizInterfaceImpl::GetCurrentProject() const
{
	return WorkspaceInfo::GetCurrentProject();
}


void WWhizInterfaceImpl::ResolveFilename(const CString& rootDir, CString& filename)
{
	WorkspaceInfo::ResolveFilename(rootDir, filename);
}


bool WWhizInterfaceImpl::RefreshFileList(void)
{
	if (g_wwhizReg)
		g_wwhizReg->ReadExtraFiles(true);
	return WorkspaceInfo::Refresh();
}


void WWhizInterfaceImpl::AddProject(CString projectName)
{
	WorkspaceInfo::AddProject(projectName);
}


void WWhizInterfaceImpl::AddProject2(CString projectName, bool noRefresh)
{
	WorkspaceInfo::AddProject(projectName, true, noRefresh);
}

	
void WWhizInterfaceImpl::RemoveAllFiles(void)
{
	WorkspaceInfo::RemoveAll();
}

	
const WWhizTag* WWhizInterfaceImpl::GetTagMatchHead(void) const
{
	return WorkspaceTags::GetMatchHead();
}

const WWhizTag* WWhizInterfaceImpl::GetTagMatchTail(void) const
{
	return WorkspaceTags::GetMatchTail();
}

int WWhizInterfaceImpl::GetTagMatchCount(void) const
{
	return WorkspaceTags::GetMatchCount();
}

const WWhizTag* WWhizInterfaceImpl::GetLastMatchedTag(void) const
{
	if (g_wwhizReg)
		return g_wwhizReg->GetLastMatchedTag();
	return NULL;
}

const WWhizTagList& WWhizInterfaceImpl::GetTagList(void) const
{
	return WorkspaceTags::GetTagList();
}

void WWhizInterfaceImpl::RemoveAllTags(void)
{
	WorkspaceTags::RemoveAll();
}

void WWhizInterfaceImpl::MatchTag(const WWhizTagList& tags, LPCTSTR name, WWhizTag::Type forceType) const
{
	WorkspaceTags::MatchTag((TagList&)tags, name, forceType);
}

bool WWhizInterfaceImpl::GotoTag(const WWhizTag* tag) const
{
	if (g_wwhizReg)
		return g_wwhizReg->GotoTag(tag);
	return false;
}

void WWhizInterfaceImpl::SetTagCallback(TagRefreshCallback callback, void* userData)
{
	WorkspaceTags::SetCallback(callback, userData);
}

void WWhizInterfaceImpl::RefreshTagList(bool forceRefresh, bool forceSave)
{
	WorkspaceTags::Refresh(forceRefresh, forceSave);
}

void WWhizInterfaceImpl::LoadTags()
{
	WorkspaceTags::Load(false);
}

void WWhizInterfaceImpl::SaveTags()
{
	WorkspaceTags::Save(false);
}

bool WWhizInterfaceImpl::GetTagUseType(WWhizTag::Type type)
{
	return WorkspaceTags::GetShowType(type);
}

void WWhizInterfaceImpl::SetTagUseType(WWhizTag::Type type, bool show)
{
	WorkspaceTags::SetShowType(type, show);
}

WWhizFileList& WWhizInterfaceImpl::GetGlobalFileList(void) const
{
	return CompilerFiles::GetFileList();
}

bool WWhizInterfaceImpl::RefreshGlobalFileList(void)
{
	return CompilerFiles::Refresh();
}

WWhizConfig& WWhizInterfaceImpl::GetConfig()
{
	return m_config;
}

CTime WWhizInterfaceImpl::GetLastFileRefreshTime() const
{
extern CTime g_lastFileRefresh;
	return g_lastFileRefresh;
}


CTime WWhizInterfaceImpl::GetLastTagRefreshTime() const
{
extern CTime g_lastTagRefresh;
	return g_lastTagRefresh;
}

void WWhizInterfaceImpl::SetWWhizReg(WWhizReg* wwhizReg)
{
	g_wwhizReg = wwhizReg;
}

WWhizFileList& WWhizInterfaceImpl::GetActiveFileList(void) const
{
	return WorkspaceInfo::GetActiveFileList();
}


DWORD WWhizInterfaceImpl::GetVersion(void) const
{
	return s_wwhizVersion;
}


void WWhizInterfaceImpl::AddChangedFile(CString filename)
{
	WorkspaceTags::AddChangedFile(filename);
}


void WWhizInterfaceImpl::CheckActiveFilesForChanges()
{
	WorkspaceTags::CheckActiveFilesForChanges();
}


const WWhizTag* WWhizInterfaceImpl::GetTagScope(const CString& filename, UINT y)
{
	return WorkspaceTags::GetTagScope(filename, y);
}


const CString& WWhizInterfaceImpl::GetWorkspaceFullPath(void) const
{
	return WorkspaceInfo::GetWorkspaceFullPath();
}

