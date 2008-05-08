#ifndef WORKSPACEINFO_H
#define WORKSPACEINFO_H

#include "FileList.h"

/**
**/
class WorkspaceInfo
{
public:
	/**
	**/
	class Project
	{
		friend class WorkspaceInfo;
	public:
		const CString& GetName(void) const;
		bool IsActive(void) const;
		const CTime& GetTimeStamp(void) const;

		void SetActive(bool active);
	
	protected:
		CString m_name;			// The name of the project or workspace.
		bool m_active;			// Is the project active?
		CTime m_timeStamp;		// The time stamp of the project.
		Project* m_parent;		// Parent project.
	}; //Project

	typedef CList<Project, Project&> ProjectList;

public:
// Attributes
	static ProjectList& GetProjectList(void);	// Not const on purpose.
	static FileList& GetFileList(void);			// Not const on purpose.
	static const CString& GetWorkspaceLocation(void);
	static const CString& GetExtraFilename(void);
	static const CString& GetExcludeFilename(void);
	static void GetCurrentFilename(CString &filename);

// Operations
	static void ResolveFilename(const CString& rootDir, CString& filename);
	static void SplitPath(const CString& fullPath, CString& path, CString& title, CString& ext);
	static void SetWorkspaceLocation(void);		// Do once on Workspace Open.
	static bool CheckIntegrity(void);
	static bool Refresh(void);
	static void Add(CString projectName);
	static void RemoveAll(void);

	static IApplication *s_pApplication;

protected:
	static bool CheckProjectIntegrity(CString projectName);
	static void ReadDSPFile(Project& prj);
	static void ReadDSWFile(Project& prj);
	static Project* AddHelper(CString projectName);

	static CString s_workspaceLocation;
	static CString s_extraFilename;
	static CString s_excludeFilename;

	static ProjectList s_projects;
	static FileList s_fileList;
}; //WorkspaceInfo


inline const CString& WorkspaceInfo::Project::GetName(void) const
{  return m_name;  }
inline bool WorkspaceInfo::Project::IsActive(void) const
{  return m_active;  }
inline const CTime& WorkspaceInfo::Project::GetTimeStamp(void) const
{  return m_timeStamp;  }
inline void WorkspaceInfo::Project::SetActive(bool active)
{  m_active = active;  }


inline WorkspaceInfo::ProjectList& WorkspaceInfo::GetProjectList(void)
{  return s_projects;  }
inline FileList& WorkspaceInfo::GetFileList(void)
{  return s_fileList;  }
inline const CString& WorkspaceInfo::GetWorkspaceLocation(void)
{  return s_workspaceLocation;  }
inline const CString& WorkspaceInfo::GetExtraFilename(void)
{  return s_extraFilename;  }
inline const CString& WorkspaceInfo::GetExcludeFilename(void)
{  return s_excludeFilename;  }

#endif WORKSPACEINFO_H

