///////////////////////////////////////////////////////////////////////////////
// $Workfile: ProjectList.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/ProjectList.h $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
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
#include "XmlData.h"

class Project : public WWhizProject
{
	friend class WorkspaceInfo;
	friend class WorkspaceTags;
public:
	virtual const CString& GetName(void) const;
	virtual bool IsActive(void) const;
	virtual const CTime& GetTimeStamp(void) const;
	virtual WWhizFileList& GetFileList(void) const;
	virtual WWhizTagList& GetTagList(void) const;
	virtual bool IsWorkspaceProject(void) const;
	virtual void SetActive(bool active = true);
	virtual XmlData& GetXmlData()			{  return m_xmlData;  }
	virtual void SetNoRefresh(bool noRefresh)		{  m_noRefresh = noRefresh;  }
	
	void BuildTags(void);
	void RemoveAllTags(void);

protected:
	Project();
	Project(const Project& src);				// Hidden
	Project& operator=(const Project& src);		// Hidden

	CString m_name;			// The name of the project or workspace.
	bool m_active;			// Is the project active?
	bool m_workspaceProject;// Is the project part of the DevStudio workspace?
	CTime m_timeStamp;		// The time stamp of the project.
	Project* m_parent;		// Parent project.
	FileList m_fileList;	// List of all files in the project.
	TagList m_tagList;		// List of all tags in the project.

	bool m_touched:1;		// The file was touched the last refresh.
	bool m_changed:1;
	bool m_lastActive:1;
	bool m_newProject:1;
	bool m_noRefresh:1;

	XmlData m_xmlData;
};


class ProjectList : public WWhizProjectList, public WList<Project*>
{
public:
	virtual int GetProjectCount() const;
	virtual WWhizProject* GetProjectByIndex(int index) const;
	virtual WWhizProject* Find(CString projectName) const;

	~ProjectList();
	void RemoveAll();
};

inline const CString& Project::GetName(void) const
{
	return m_name;
}


inline bool Project::IsActive(void) const
{
	return m_active;
}


inline const CTime& Project::GetTimeStamp(void) const
{
	return m_timeStamp;
}


inline WWhizFileList& Project::GetFileList(void) const
{
	return (WWhizFileList&)m_fileList;
}


inline WWhizTagList& Project::GetTagList(void) const
{
	if (m_tagList.GetCount() == 0)
	{
		Project* prj = (Project*)this;
		prj->BuildTags();
	}
	return (WWhizTagList&)m_tagList;
}


inline void Project::RemoveAllTags(void)
{
	m_tagList.RemoveAll();
}


