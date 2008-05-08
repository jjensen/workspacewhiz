///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceInfo.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceInfo.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 39   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "WorkspaceInfo.h"
#include "WorkspaceTags.h"
#include "TagList.h"
#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\bldguid.h>
#include <ObjModel\textguid.h>
#include <ObjModel\dbgguid.h>
#include "CompilerFiles.h"
#include "XmlData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString s_workspacePath;
static CString s_workspaceFullPath;
static CString s_extraFilename;
CTime g_lastFileRefresh;

static ProjectList s_projects;
static FileList s_fileList;
static FileList s_activeFileList;

int g_numRefs;

extern CMap<CString, LPCTSTR, int, int> g_filesChangedFileMap;

///////////////////////////////////////////////////////////////////////////////
ProjectList& WorkspaceInfo::GetProjectList(void)
{
	return s_projects;
}


FileList& WorkspaceInfo::GetFileList(void)
{
	return s_fileList;
}


FileList& WorkspaceInfo::GetActiveFileList(void)
{
	return s_activeFileList;
}


const CString& WorkspaceInfo::GetWorkspaceLocation(void)
{
	return s_workspacePath;
}


const CString& WorkspaceInfo::GetWorkspaceFullPath(void)
{
	return s_workspaceFullPath;
}


const CString& WorkspaceInfo::GetExtraFilename(void)
{
	return s_extraFilename;
}


void WorkspaceInfo::ResolveFilename(const CString& rootDir, CString& filename)
{
	// Initially resolve all environment variables.
	int position = -1;
	while (1)
	{
		// Search for $ symbols, denoting an environment variable.
		position = CStringFind(filename, '$', position + 1);
		if (position == -1)
			break;

		// Okay, there is an environment variable in there... resolve it.
		if (filename[position + 1] == '(')
		{
			int lastpos = filename.Find(')');
			CString env = filename.Mid(position + 2, lastpos - (position + 2));

			// See if we can resolve it.  If not, then exit.
			char buffer[_MAX_PATH];
			if (::GetEnvironmentVariable(env, buffer, _MAX_PATH) == 0)
				continue;

			// Okay, rebuild the string.
			filename = filename.Left(position) + buffer +
						filename.Right(filename.GetLength() - lastpos - 1);
		}
	}

	// Now resolve relative paths.
	if (filename[0] == '.'  ||
		((filename[0] != '\\'  &&  filename[0] != '/')  &&  filename[1] != ':')
		)
	{
		filename = rootDir + filename;
	}
	CFileStatus fileStatus;
	CFile::GetStatus(filename, fileStatus);
	filename = fileStatus.m_szFullName;
}


void WorkspaceInfo::SetWorkspaceLocation(void)
{
	// Retrieve the workspace name.
	s_workspaceFullPath = g_wwhizInterface->GetWorkspaceName();

	// Is it empty?
	if (s_workspaceFullPath.IsEmpty())
	{
		// If so, then there is no workspace open.
		// Call the OS for the current directory.
		::GetCurrentDirectory(_MAX_PATH, s_workspaceFullPath.GetBuffer(_MAX_PATH));
		s_workspaceFullPath.ReleaseBuffer();

		// Make sure it ends in a closing backslash.
		s_workspaceFullPath.TrimRight('\\');
		s_workspaceFullPath += "\\!!!WWhizVirtualWorkspace!!!.dsw";
	}

	int slashPos = s_workspaceFullPath.ReverseFind('\\');
	if (slashPos != -1)
	{
		s_workspacePath = s_workspaceFullPath.Left(slashPos + 1);
	}

	// Assign the extra filename.
	CString pathNoExt = s_workspaceFullPath;
	int dotPos = pathNoExt.ReverseFind('.');
	if (dotPos != -1)
		pathNoExt = pathNoExt.Left(dotPos);
	s_extraFilename = pathNoExt + ".ExtraFiles.WW";
}


bool WorkspaceInfo::GetCurrentFilename(CString& filename)
{
	// Is there an application?
	if (!g_pApplication)
	{
		filename.Empty();
		return false;
	}

	ObjModelHelper objModel;
	if (objModel.GetActiveDocument())
	{
		filename = objModel.GetFilename();
		return !filename.IsEmpty();
	}

	filename.Empty();
	return false;
}


Project* WorkspaceInfo::GetCurrentProject()
{
	if (!g_pApplication)
	{
		return NULL;
	}

	// Get the current project
	CComPtr<IDispatch> pActiveProject;
	g_pApplication->get_ActiveProject(&pActiveProject);
	if (pActiveProject)
	{
		CComQIPtr<IGenericProject, &IID_IGenericProject> pProject(pActiveProject);
		if (pProject)
		{
			CComBSTR bstrProjectName;
			pProject->get_FullName(&bstrProjectName);
			CString projectName = bstrProjectName;

			ProjectList& projectList = WorkspaceInfo::GetProjectList();
			POSITION pos = projectList.GetHeadPosition();
			while (pos)
			{
				Project* project = projectList.GetNext(pos);
				if (project->GetName().CompareNoCase(projectName) == 0)
				{
					return project;
				}
			}
		}
	}

	return NULL;
}


bool g_filesRefreshed;


// Clean the projects and filenames lists.
void WorkspaceInfo::RemoveAll(void)
{
	// Clean the projects list.
	s_projects.RemoveAll();

	// Clean the filenames list.
	s_fileList.RemoveAll();

	WorkspaceInfo::GetGlobalFileMap().RemoveAll();

	g_filesRefreshed = true;
}


static void WriteString(CFile& file, UINT numSpaces, LPCTSTR msg, ...)
{
	va_list args;
	char textBuffer[1024];

	va_start(args, msg);
	_vsnprintf(textBuffer, 1023, msg, args);
	va_end(args);

	// Write out indentation.
	char spaces[500];
	memset(spaces, ' ', numSpaces);
	file.Write(spaces, numSpaces);
	file.Write(textBuffer, strlen(textBuffer));
}


void WorkspaceInfo::ReadDSPFile(Project& prj)
{
	// Open the .dsp file.
	CStdioFile file;
	if (!file.Open(prj.GetName(), CFile::modeRead))
	{
		// Huh?
		return;
	}

	enum ParseState
	{
		FINDTARGET,
		PARSETARGET,
	};

	ParseState state = FINDTARGET;

	CMemFile xmlMemFile;
	WriteString(xmlMemFile, 0, "<VisualStudioProject\n  ProjectType=\"Visual C++\"\n"
		"  Version=\"6.00\"\n  Name = \"Test\">\n");
	
	// Begin reading the file.
	bool localListRefreshed = false;
	CString line;
	UINT numSpaces = 4;
	while (true)
	{
		// Read in a line from the file.
		if (!file.ReadString(line))
			break;

		if (line.IsEmpty())
			continue;

		// Check the state.
		if (state == FINDTARGET)
		{
			if (line.CompareNoCase("# Begin Target") == 0)
			{
				WriteString(xmlMemFile, 2, "<Files>\n");

				state = PARSETARGET;				
			}
		}

		else if (state == PARSETARGET)
		{
			enum Types
			{
				NONE,
				BEGIN_GROUP,
				END_GROUP,
				END_TARGET,
				SOURCE,
			};

			Types type = NONE;
			
			// Check for # Begin Group lines.
			if (line.GetLength() > 13  &&  _tcsncmp(line, "# Begin Group", 13) == 0)
			{
				type = BEGIN_GROUP;
				line = line.Mid(14);
			}

			// Check for # End Group lines
			else if (line.GetLength() >= 11  &&  _tcsncmp(line, "# End Group", 11) == 0)
			{
				type = END_GROUP;
				line = line.Mid(11);
			}

			// Check for SOURCE= lines.  (Do _tcsncmp() for speed)
			else if (line.GetLength() > 7  &&  _tcsncmp(line, "SOURCE=", 7) == 0)
			{
				type = SOURCE;
				line = line.Mid(7);
			}

			// Check for # End Group lines
			else if (line.GetLength() >= 12  &&  _tcsncmp(line, "# End Target", 12) == 0)
			{
				type = END_TARGET;
				line = line.Mid(12);
			}

			if (type == NONE)
				continue;
			if (type == END_GROUP)
			{
				numSpaces -= 2;
				WriteString(xmlMemFile, numSpaces, "</Filter>\n");
				continue;
			}
			else if (type == END_TARGET)
			{
				WriteString(xmlMemFile, 2, "</Files>\n");
				state = FINDTARGET;
				continue;
			}

			///////////////////////////////////////////////////////////////////////
			// Start the pointer just after the SOURCE=, but strip the beginning
			// and end quotes if they exist.
			int startPos = 0;
			if (line[0] == '"')
				startPos = 1;

			int endPos = line.GetLength();
			if (line[endPos - 1] == '"')
				endPos--;

			// Strip spaces, just in case.
			while (startPos < endPos  &&  line[startPos] == ' ')
				startPos++;

			// Create and resolve the filename.
			CString text = line.Mid(startPos, endPos - startPos);
			if (type == BEGIN_GROUP)
			{
				WriteString(xmlMemFile, numSpaces, "<Filter Name=\"%s\">\n", text);
				numSpaces += 2;
			}

			if (type == SOURCE)
			{
				WriteString(xmlMemFile, numSpaces, "<File RelativePath=\"%s\">\n", text);
				WriteString(xmlMemFile, numSpaces, "</File>\n", text);
			}
		}
	} //while(1)

	WriteString(xmlMemFile, 0, "</VisualStudioProject>\n");

#ifdef DUMP_FILE
	FILE* textFile = fopen("c:\\test.dsp", "wt");
	DWORD size = xmlMemFile.GetLength();
	char* buffer = new char[size + 1];
	xmlMemFile.SeekToBegin();
	xmlMemFile.Read(buffer, size);
	buffer[size]  = 0;

	fputs(buffer, textFile);
	fclose(textFile);
	delete [] buffer;
#endif DUMP_FILE
	
	// Close the .dsp file.
	file.Close();

	xmlMemFile.SeekToBegin();
	ReadVCProjFile(prj, &xmlMemFile);
}


void WorkspaceInfo::RecurseVCProjNode(
		XmlNode* parentNode, const CString& rootPath, FileList& fileList,
		bool& localListRefreshed, CList<CString, CString&>& projectsToAdd)
{
	if (!parentNode)
		return;

    XmlNode* node = (XmlNode*)parentNode->GetFirstChildNode();
    while (node)
    {
		// Is it a File node?
		if (node->GetName() == "File")
		{
			// Create and resolve the filename.
			XmlNode::Attribute* attr = node->FindAttribute("RelativePath");
			if (attr)
			{
				CString filename = attr->GetValue();
				WorkspaceInfo::ResolveFilename(rootPath, filename);

				File* file = File::Create(filename);

				// Insert it into the current project.
				if (fileList.Add(file))
				{
					g_filesRefreshed = true;
					localListRefreshed = true;
				}
				file->m_touched = true;

				// Test the file to see if it is a project or workspace.
				int dotPos = filename.ReverseFind('.');
				if (dotPos != -1)
				{
					CString ext = filename.Mid(dotPos + 1);
					ext.MakeLower();
					if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||  ext == "vcw"  ||  ext == "vcproj")
						projectsToAdd.AddTail(filename);
				}
			}
		}
		else if (node->GetName() == "Filter")
		{
			XmlNode::Attribute* attr = node->FindAttribute("Filter");
        	RecurseVCProjNode(node, rootPath, fileList, localListRefreshed, projectsToAdd);
		}

		node = (XmlNode*)node->GetNextSiblingNode();
	}
}


void WorkspaceInfo::ReadVCProjFile(Project& prj, CFile* inFile)
{
	if (!inFile)
	{
		// Parse the .vcproj file.
		if (!prj.GetXmlData().ParseXmlFile(prj.GetName()))
			return;
	}
	else
	{
		// Parse the .vcproj file.
		if (!prj.GetXmlData().ParseXmlFile(*inFile))
			return;
	}

	FileList& fileList = (FileList&)prj.GetFileList();

	// Build the root path to resolve filenames from.
	CString rootPath = prj.GetName().Left(prj.GetName().ReverseFind('\\') + 1);

	// Build the projectsToAdd list.
	CList<CString, CString&> projectsToAdd;
	bool prjIsExtraFiles = false;
	if (prj.GetName().CompareNoCase(GetExtraFilename() + ".dsp") != 0)
		projectsToAdd.AddTail(const_cast<CString&>(prj.GetName()));
	else
		prjIsExtraFiles = true;


	// Make sure no files have been touched yet.
	int fileListCount = fileList.GetCount();
	for (int i = 0; i < fileListCount; i++)
	{
		File* file = (File*)fileList.Get(i);
		file->m_touched = false;
	}

	bool localListRefreshed = false;
	XmlNode* filesNode = prj.GetXmlData().Find("Files");
	RecurseVCProjNode(filesNode, rootPath, fileList, localListRefreshed, projectsToAdd);

	// Remove unused files.
	fileListCount = fileList.GetCount();
	for (i = 0; i < fileListCount; i++)
	{
		File* file = (File*)fileList.Get(i);
		if (!file->m_touched)
		{
			// The file doesn't exist in the project anymore.
			fileList.Remove(i);
			i--;
			fileListCount--;
			g_filesRefreshed = true;
			localListRefreshed = true;
		}
	}

	if (localListRefreshed)
	{
		// Sort it.
		fileList.Sort();
	}

	// Add the .dsp and .dsw files.
	POSITION pos = projectsToAdd.GetHeadPosition();
	if (!prjIsExtraFiles)
		projectsToAdd.GetNext(pos);
	while (pos)
	{
		const CString& projectFilename = projectsToAdd.GetNext(pos);
		AddProject(projectFilename, prj.IsActive());
	}
}





// Read in a .dsw file.
void WorkspaceInfo::ReadDSWFile(Project& prj)
{
	// Open the .dsw file.
	CStdioFile file;
	if (!file.Open(prj.GetName(), CFile::modeRead))
	{
		// Huh?
		return;
	}

	// Build the root path to resolve filenames from.
	CString rootPath = prj.GetName().Left(prj.GetName().ReverseFind('\\') + 1);

	// Begin reading the file.
	CString line;
	while (1)
	{
		// Read in a line from the file.
		if (!file.ReadString(line))
			break;

		// Look for something that looks like this.
		// Project: "!MyLib"=".\Prj\!MyLib.dsp" - Package Owner=<4>
		// Project: "Gfx"=.\Prj\Gfx.dsp - Package Owner=<4>
		if (line.GetLength() <= 8  ||  strncmp(line, "Project:", 8) != 0)
			continue;

		// Search for the =.
		int endPos;		// Will be one past the last letter.
		int startPos = line.Find('=');
		if (startPos == -1)
			continue;
		startPos++;		// Move to the beginning of the name.

		// See if the name is quoted.
		if (line[startPos] == '"')
		{
			// Move past the quote.
			startPos++;
			
			// Find the closing quote.
			endPos = CStringFind(line, '"', startPos);
			if (endPos == -1)
				continue;
		}
		else //if (line[namePos] == '"')
		{
			// Find a space, since that should denote the end of the filename.
			endPos = CStringFind(line, ' ', startPos);
		}

		// Got the name isolated.  Add it.
		CString projectPath = line.Mid(startPos, endPos - startPos);
		ResolveFilename(rootPath, projectPath);
		Project* newlyAddedProject = AddHelper(projectPath, prj.IsActive());
		if (newlyAddedProject)
		{
			newlyAddedProject->m_parent = &prj;
			File* dspFile = File::Create(projectPath);
			prj.m_fileList.Add(dspFile);
		}
	} //while(1)

	// Close the .dsp file.
	file.Close();

	// Add the .dsw file.
	File* dswFile = File::Create(prj.m_name);
	prj.m_fileList.Add(dswFile);

	prj.m_fileList.Sort();
}


// Add a new project or workspace to the list of projects.
Project* WorkspaceInfo::AddProject(CString projectName, bool active)
{
	if (projectName.IsEmpty())
		return NULL;

	return AddHelper(projectName, active);
}


POSITION FindProject(const CString& projectName)
{
	// Is it already in the list?
	POSITION pos = s_projects.GetHeadPosition();
	while (pos)
	{
		POSITION lastPos = pos;
		Project* compProject = s_projects.GetNext(pos);

		if (projectName.CompareNoCase(compProject->GetName()) == 0)
			return lastPos;
	}

	return pos;
}

	
// Internal helper function.
Project* WorkspaceInfo::AddHelper(CString projectName, bool active)
{
	// Resolve the filename.
	ResolveFilename(GetWorkspaceLocation(), projectName);
	
	// Make sure there is an extension.
	int dotPosition = projectName.ReverseFind('.');
	if (dotPosition == -1)
	{
		// What?!
		return NULL;
	}

	// Is it already in the list?
	Project* project = NULL;
	POSITION projectPos = FindProject(projectName);
	if (projectPos)
	{
		project = s_projects.GetAt(projectPos);
	}
	
	// Make sure the file exists.
	CFileStatus fileStatus;
	if (!CFile::GetStatus(projectName, fileStatus))
	{
		if (projectPos)
		{
			// The project no longer exists.  Destroy it.
			delete project;
			s_projects.RemoveAt(projectPos);

			return NULL;
		}
	}

	// If there isn't a project, create a new project structure.
	if (!project)
	{
		project = new Project;
		project->m_name = projectName;
		project->m_newProject = true;
		project->SetActive(active);

		// Add it to the end of the projects list.
		projectPos = s_projects.AddTail(project);
	}

	project->m_workspaceProject = false;

	// Determine which type of file this is:
	CString ext = project->m_name.Mid(dotPosition + 1);
	ext.MakeLower();

	// Check the project time stamp.
	if (project->GetTimeStamp() != fileStatus.m_mtime)
	{
		// Set the project time stamp.
		project->m_timeStamp = fileStatus.m_mtime;

		if (ext == "dsw"  ||  ext == "vcw")
		{
			// This is a workspace file.
			ReadDSWFile(*project);
		}

		else if (ext == "dsp"  ||  ext == "vcp")
		{
			TRY
			{
				// Assume it is a project file.
				ReadDSPFile(*project);
			}
			CATCH_ALL(e)
			{
				e->Delete();
			}
			END_CATCH_ALL
		}
		else if (ext == "vcproj")
		{
			ReadVCProjFile(*project);
		}
	}
	else
	{
		// Check any projects in the list.
		for (int i = 0; i < project->m_fileList.GetCount(); i++)
		{
			File* file = (File*)project->m_fileList.Get(i);
			const CString& ext = file->GetExt();
			if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||  ext == "vcw"  ||  ext == "vcproj")
			{
				if (projectName.CompareNoCase(file->GetFullName()) != 0)
					AddProject(file->GetCaseFullName(), project->IsActive());
			}
		}
	}

	project->m_touched = true;
	
	return project;
}


static CString s_lastWorkspaceName;

// Refresh the projects list.
bool WorkspaceInfo::Refresh(void)
{
	// Turn off the workspace project flag.
	POSITION pos = GetProjectList().GetHeadPosition();
	while (pos)
	{
		Project* project = GetProjectList().GetNext(pos);
		project->m_workspaceProject = false;
	}
	
	for (int i = 0; i < s_fileList.GetCount(); ++i)
	{
		File* file = (File*)s_fileList.Get(i);
		file->m_workspaceFile = true;
	}

	// Only do this part if DevStudio exists.
	if (g_pApplication)
	{
		CString workspaceName = g_wwhizInterface->GetWorkspaceName();
		if (!workspaceName.IsEmpty())
		{
			bool doWorkspaceRead = true;

			// Did we read this workspace last time?
			if (workspaceName.CompareNoCase(s_lastWorkspaceName) == 0)
			{
//	g_filesChangedFileMap[workspaceName] = 0;
				// See if it changed.
//				int value;
//				if (!g_filesChangedFileMap.Lookup(workspaceName, value))
//					doWorkspaceRead = false;
//				else
//				{
/*					// See if the file is currently in the file list.
					File* file = (File*)GetActiveFileList().Find(workspaceName);
					if (file)
					{
						// Get the time stamp.
						CFileStatus fileStatus;
						if (CFile::GetStatus(workspaceName, fileStatus))
						{
							// If it is the same, then don't read the workspace information in.
							if (file->GetTimeStamp() == fileStatus.m_mtime)
							{
								doWorkspaceRead = false;
							}
						}
					}
*/
//				}
			}

			if (doWorkspaceRead)
			{
				Project* workspace = AddProject(workspaceName, true);
				s_lastWorkspaceName = workspaceName;

				// Assign the workspace projects.
				FileList& workspaceFileList = (FileList&)workspace->GetFileList();
				workspace->m_workspaceProject = true;
				for (int i = 0; i < workspaceFileList.GetCount(); ++i)
				{
					// Get the project name.
					File* projectFile = (File*)workspaceFileList.Get(i);
					projectFile->m_workspaceFile = true;

					Project* project =
						static_cast<Project*>(GetProjectList().Find(
							projectFile->GetCaseFullName()));
					project->m_workspaceProject = true;

					FileList& projectFileList = (FileList&)project->GetFileList();

					for (int j = 0; j < projectFileList.GetCount(); ++j)
					{
						File* file = (File*)projectFileList.Get(j);
						file->m_workspaceFile = true;
					}

					project->m_active = true;
				}
			}
		}
/*
		// First, get a pointer to the dispatch for the Projects collection
		CComPtr<IDispatch> pDispProjects;
		VERIFY_OK(g_pApplication->get_Projects(&pDispProjects));
		CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

		// Get the number of projects in the collection
		long projectCount;
		VERIFY_OK(pProjects->get_Count(&projectCount));

		// Iterate all the projects.
		for (long i = 1; i <= projectCount; i++)
		{
			CComVariant Vari = i;

			// Get the next project
			CComPtr<IGenericProject> pGenProject;
			VERIFY_OK(pProjects->Item(Vari, &pGenProject));
			CComQIPtr<IGenericProject, &IID_IGenericProject> pProject(pGenProject);

			// Get the project name.
			CComBSTR bszStr;
			VERIFY_OK(pProject->get_FullName(&bszStr));
			CString projectName = bszStr;

			Project* project = AddProject(projectName);
			if (project)
				project->m_workspaceProject = true;
		}
*/
	}

	// Remove unused projects.
	pos = s_projects.GetHeadPosition();
	while (pos)
	{
		POSITION oldPos = pos;
		Project* project = s_projects.GetNext(pos);
		if (!project->m_touched)
		{
			s_projects.RemoveAt(oldPos);
			delete project;
			g_filesRefreshed = true;
		}
		else
		{
			project->m_touched = false;

			if (project->m_changed)
			{
				project->m_changed = false;
				g_filesRefreshed = true;
			}

			project->m_lastActive = project->m_active;
		}
	}
	
	if (g_filesRefreshed)
	{
		// Add all files to global file list.
		s_fileList.RemoveAll();
		s_activeFileList.RemoveAll();
		pos = s_projects.GetHeadPosition();
		while (pos)
		{
			Project* project = s_projects.GetNext(pos);
			WWhizFileList& fileList = project->GetFileList();
			int fileListCount = fileList.GetCount();
			bool projectActive = project->IsActive();
			for (int i = 0; i < fileListCount; i++)
			{
				File* file = (File*)fileList.Get(i);
				if (projectActive)
					s_activeFileList.Add(file);
				s_fileList.Add(file);
			}
		}

		// Sort the global file array.
		s_activeFileList.Sort();
		s_fileList.Sort();

extern FileMap g_globalFileMap;
		g_globalFileMap.CleanUp();
		
		g_lastFileRefresh = CTime::GetCurrentTime();
	}
	
	g_filesChangedFileMap.RemoveAll();

	// Rebuilt stuff.
	return false;
}


extern FileMap g_globalFileMap;

FileMap& WorkspaceInfo::GetGlobalFileMap()
{
	return g_globalFileMap;
}


