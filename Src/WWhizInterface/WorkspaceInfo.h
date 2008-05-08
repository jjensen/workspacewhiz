///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceInfo.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceInfo.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 23   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
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
	static ProjectList& GetProjectList(void);
	static FileList& GetFileList(void);
	static FileList& GetActiveFileList(void);
	static const CString& GetWorkspaceFullPath(void);
	static const CString& GetWorkspaceLocation(void);
	static const CString& GetExtraFilename(void);
	static bool GetCurrentFilename(CString &filename);
	static Project* GetCurrentProject();

// Operations
	static void ResolveFilename(const CString& rootDir, CString& filename);
	static void SplitPath(const CString& fullPath, CString& path, CString& title, CString& ext);
	static void SetWorkspaceLocation(void);		// Do once on Workspace Open.
	static bool Refresh(void);
	static Project* AddProject(CString projectName, bool active = true);
	static void RemoveAll(void);

	static WWhizInterface& GetWWhizInterface();
	static FileMap& GetGlobalFileMap();

protected:
	static void ReadDSPFile(Project& prj);
	static void ReadVCProjFile(Project& prj, CFile* inFile = NULL);
	static void ReadDSWFile(Project& prj);
	static Project* AddHelper(CString projectName, bool active);
	static void RecurseVCProjNode(
		XmlNode* node, const CString& rootPath, FileList& fileList, bool& localListRefreshed,
		CList<CString, CString&>& projectsToAdd);
}; //WorkspaceInfo

