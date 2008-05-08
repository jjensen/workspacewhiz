///////////////////////////////////////////////////////////////////////////////
// $Workfile: ProjectList.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/ProjectList.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "ProjectList.h"
#include "WorkspaceInfo.h"
#include "Timer.h"

extern void ODS(LPCTSTR msg, ...);

/**
	Destructor.
**/
ProjectList::~ProjectList()
{
	RemoveAll();

}
		
/**
	Destroy the projects in the list.
**/
void ProjectList::RemoveAll( )
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		Project* project = GetNext(pos);
		delete project;
	}

	WList<Project*>::RemoveAll();
}


Project::Project() :
	m_active(true),
	m_lastActive(true),
	m_changed(false),
	m_parent(NULL),
	m_touched(false),
	m_workspaceProject(false),
	m_newProject(false),
	m_noRefresh(false)
{
}


void Project::BuildTags(void)
{
	m_tagList.RemoveAll();

	CTimer addTimer;
	addTimer.Start();
	for (int i = 0; i < m_fileList.GetCount(); ++i)
	{
		WWhizFile* file = m_fileList.Get(i);
		TagList& tagList = (TagList&)file->GetTagList();

		for (int j = 0; j < tagList.GetCount(); ++j)
		{
			Tag* tag = (Tag*)tagList.Get(j);
			m_tagList.Add(tag);
		}
	}
	addTimer.Stop();

	CTimer sortTimer;
	sortTimer.Start();
	m_tagList.SortByTag_Parent_Namespace();
	sortTimer.Stop();
	ODS("WWhiz: Project %s Add: %0.2f  Sort: %0.2f\n", m_name, addTimer.GetMillisecs(),
		sortTimer.GetMillisecs());
}

	
int ProjectList::GetProjectCount() const
{
	return GetCount();
}


WWhizProject* ProjectList::GetProjectByIndex(int index) const
{
	POSITION pos = FindIndex(index);
	if (!pos)
		return NULL;
	return (WWhizProject*)GetAt(pos);
}


WWhizProject* ProjectList::Find(CString projectName) const
{
	// Resolve the filename.
	WorkspaceInfo::ResolveFilename(WorkspaceInfo::GetWorkspaceLocation(), projectName);
	
	// Find the file.
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		WWhizProject* project = GetNext(pos);
		if (project->GetName().CompareNoCase(projectName) == 0)
			return project;
	}

	return NULL;
}


bool Project::IsWorkspaceProject(void) const
{
	return m_workspaceProject;
}


static WMap<Project*, Project*> s_recursionMap;

void Project::SetActive(bool active)
{
	Project* temp;
	if (s_recursionMap.Lookup(this, temp))
	{
		// Already in here.
		return;
	}

	s_recursionMap[this] = this;

	m_changed = true;
	m_lastActive = m_active;
	m_active = active;

	int fileCount = m_fileList.GetCount();
	for (int i = 0; i < fileCount; i++)
	{
		File* file = (File*)m_fileList.Get(i);
		const CString& ext = file->GetExt();
		if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||  ext == "vcw")
		{
			Project* project = (Project*)g_wwhizInterface->GetProjectList().Find(file->GetFullName());
			if (project)
				project->SetActive(active);
		}
	}

	s_recursionMap.RemoveKey(this);
}

