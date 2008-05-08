///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceTags.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceTags.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "WorkspaceInfo.h"

/**
**/
class WorkspaceTags
{
public:
// Constructors
	static void Initialize();

// Attributes
	static const Tag* GetMatchHead(void);
	static const Tag* GetMatchTail(void);
	static int GetMatchCount(void);

	static const TagList& GetTagList(void);

// Operations
	static void RemoveAll(void);

	static bool ParseTextTags(BYTE* mem, DWORD memSize);

	static void MatchTag(const TagList& tags, LPCTSTR name, WWhizTag::Type forceType = WWhizTag::LAST_TYPE);

	static void SetCallback(WWhizInterface::TagRefreshCallback callback, void* userData);
	static void Refresh(bool forceRefresh = false, bool forceSave = false);

	static bool GetShowType(WWhizTag::Type type);
	static void SetShowType(WWhizTag::Type type, bool show);

	static void Load(bool newProjectsOnly);
	static bool LoadProjects(bool newProjectsOnly);
	static bool LoadProjectTags(Project* project);
	static void Save(bool forceSave);

	static void AddChangedFile(CString filename);
	static void CheckActiveFilesForChanges();
	static void CheckDateTimeStamps();

	static const WWhizTag* GetTagScope(const CString& filename, UINT y);
}; //WorkspaceTags

