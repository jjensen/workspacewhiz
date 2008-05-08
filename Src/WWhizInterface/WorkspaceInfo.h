///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceInfo.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceInfo.h $
// $Date: 2003/01/07 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FileList.h"
#include "ProjectList.h"

class XmlNode;

/**
**/
class WorkspaceInfo
{
public:
// Attributes
	static ProjectList& GetProjectList(void)			{  return s_projects;  }
	static FileList& GetFileList(void)					{  return s_fileList;  }
	static FileList& GetActiveFileList(void)			{  return s_activeFileList;  }
	static const CString& GetWorkspaceFullPath(void)	{  return s_workspaceFullPath;  }
	static const CString& GetWorkspaceLocation(void)	{  return s_workspacePath;  }
	static const CString& GetExtraFilename(void)		{  return s_extraFilename;  }
	static bool GetCurrentFilename(CString &filename);
	static Project* GetCurrentProject();

// Operations
	static void ResolveFilename(const CString& rootDir, CString& filename);
	static void SplitPath(const CString& fullPath, CString& path, CString& title, CString& ext);
	static void SetWorkspaceLocation(void);		// Do once on Workspace Open.
	static bool Refresh(void);
	static Project* AddProject(CString projectName, bool active = true, bool noRefresh = false);
	static void RemoveAll(void);

	static WWhizInterface& GetWWhizInterface();
	static FileMap& GetGlobalFileMap();

protected:
	static void ReadDSPFile(Project& prj);
	static void ReadVCProjFile(Project& prj, CFile* inFile = NULL);
	static void ReadCSProjFile(Project& prj, CFile* inFile = NULL);
	static void ReadDSWFile(Project& prj);
	static void ReadSlnFile(Project& prj);
	static Project* AddHelper(CString projectName, CString ext, bool active, bool noRefresh = false);
	static void RecurseVCProjNode(
		XmlNode* node, const CString& rootPath, FileList& fileList, bool& localListRefreshed,
		WList<CString>& projectsToAdd);
	static void RecurseCSProjNode(
		XmlNode* node, const CString& rootPath, FileList& fileList, bool& localListRefreshed,
		WList<CString>& projectsToAdd);

	static CString s_windowListFileName;
	static CString s_workspacePath;
	static CString s_workspaceFullPath;
	static CString s_extraFilename;
	static ProjectList s_projects;
	static FileList s_fileList;
	static FileList s_activeFileList;
}; //WorkspaceInfo

