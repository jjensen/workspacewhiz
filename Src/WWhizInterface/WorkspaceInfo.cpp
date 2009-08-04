///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceInfo.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceInfo.cpp $
// $Date: 2003/01/07 $ $Revision: #11 $ $Author: Joshua $
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
#include "XmlData.h"
#include "MemFile.h"
#include "FileGlobList.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTime g_lastFileRefresh;

CString WorkspaceInfo::s_windowListFileName;
CString WorkspaceInfo::s_workspacePath;
CString WorkspaceInfo::s_workspaceFullPath;
CString WorkspaceInfo::s_extraFilename;
ProjectList WorkspaceInfo::s_projects;
FileList WorkspaceInfo::s_fileList;
FileList WorkspaceInfo::s_activeFileList;

int g_numRefs;

extern WMap<CString, int> g_filesChangedFileMap;

///////////////////////////////////////////////////////////////////////////////
void WorkspaceInfo::ResolveFilename(const CString& rootDir, CString& filename)
{
	// Initially resolve all environment variables.
	int position = -1;
	while (1)
	{
		// Search for $ symbols, denoting an environment variable.
		position = filename.Find('$', position + 1);
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

	CString noEnvFileName = filename;

	// Now resolve relative paths.
	if (filename[0] == '.'  ||
		((filename[0] != '\\'  &&  filename[0] != '/')  &&  filename[1] != ':')
		)
	{
		int nRootLastCharPos = rootDir.GetLength()-1;
		CString strSep;

		if( filename[0] == '.' && 
			!rootDir.IsEmpty() && 
			nRootLastCharPos >= 0 &&
			rootDir[nRootLastCharPos] != '\\' &&
			rootDir[nRootLastCharPos] != '/' )
		{
			strSep = '\\';
		}
		filename = rootDir + strSep + filename;
	}

	CFileStatus fileStatus;
	CFile::GetStatus(filename, fileStatus);
	filename = fileStatus.m_szFullName;
	if (filename.IsEmpty())
		filename = noEnvFileName;
}


void WorkspaceInfo::SetWorkspaceLocation(void)
{
	// Retrieve the workspace name.
	s_workspaceFullPath = g_wwhizInterface->GetWorkspaceName();

	// Is it empty?
	if (s_workspaceFullPath.IsEmpty()  ||  s_workspaceFullPath == "!!WWhizSolution!!.sln")
	{
		// If so, then there is no workspace open.
		// Call the OS for the current directory.
		::GetCurrentDirectory(_MAX_PATH, s_workspaceFullPath.GetBuffer(_MAX_PATH));
		s_workspaceFullPath.ReleaseBuffer();

		// Make sure it ends in a closing backslash.
		s_workspaceFullPath.TrimRight('\\');
		s_workspaceFullPath += "\\!!!WWhizSolution!!!.sln";
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
	if (!ObjModelHelper::VStudioExists())
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
	ObjModelHelper objModelHelper;
	CString projectName = objModelHelper.GetCurrentProjectName();
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

	MemFile xmlMemFile;
	WriteString(xmlMemFile, 0, "<VisualStudioProject\n  ProjectType=\"Visual C++\"\n"
		"  Version=\"6.00\"\n  Name = \"Test\">\n");
	
	// Begin reading the file.
	bool localListRefreshed = false;
	CString line;
	UINT numSpaces = 4;
	int inGroup = 0;		// Hack to fix a CMake generation bug.
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
				inGroup++;
			}

			// Check for # End Group lines
			else if (line.GetLength() >= 11  &&  _tcsncmp(line, "# End Group", 11) == 0  &&  inGroup > 0)
			{
				type = END_GROUP;
				line = line.Mid(11);
				inGroup--;
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
	char* buffer = WNEW char[size + 1];
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
		bool& localListRefreshed, WList<CString>& projectsToAdd)
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

				WList<CString> filenameList;

				// Does it have a wildcard in it?
				if (filename.Find('*') != -1  ||  filename.Find('?') != -1)
				{
					// Yes.  Run the globber.
					FileGlobList glob;
					glob.MatchPattern(filename);

					for (FileGlobList::iterator it = glob.begin(); it != glob.end(); ++it)
					{
						filenameList.AddTail((*it).c_str());
					}
				}
				else
				{
					filenameList.AddTail(filename);
				}

				POSITION pos = filenameList.GetHeadPosition();
				while (pos)
				{
					CString filename = filenameList.GetNext(pos);
					WorkspaceInfo::ResolveFilename(rootPath, filename);
					if (!filename.IsEmpty())
					{
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
							if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||  ext == "vcw"  ||
								ext == "vcproj"  ||  ext == "csproj"  ||  ext == "vbproj"  ||  ext == "stproj"  ||
								ext == "sln")
								projectsToAdd.AddTail(filename);
						}
					}
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
	WList<CString> projectsToAdd;
	bool prjIsExtraFiles = false;
	if (prj.GetName().CompareNoCase(GetExtraFilename() + ".dsp") != 0)
		projectsToAdd.AddTail(const_cast<CString&>(prj.GetName()));
	else
		prjIsExtraFiles = true;


	// Make sure no files have been touched yet.
	int fileListCount = fileList.GetCount();
	int i;
	for (i = 0; i < fileListCount; i++)
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


void WorkspaceInfo::RecurseCSProjNode(
		XmlNode* parentNode, const CString& rootPath, FileList& fileList,
		bool& localListRefreshed, WList<CString>& projectsToAdd)
{
	if (!parentNode)
		return;

    XmlNode* node = (XmlNode*)parentNode->GetFirstChildNode();
    while (node)
    {
		if (node->GetName() == "ItemGroup")
		{
			XmlNode* childNode = (XmlNode*)node->GetFirstChildNode();
			while (childNode)
			{
				if (childNode->GetName() == "Compile"  ||  childNode->GetName() == "Content"  ||  
					childNode->GetName() == "EmbeddedResource"  ||  childNode->GetName() == "None"  ||
					childNode->GetName() == "Ruby"  ||  childNode->GetName() == "EmbeddedRuby")
				{
					XmlNode::Attribute* attr = childNode->FindAttribute("Include");
					if (attr)
					{
						CString filename = attr->GetValue();
						WorkspaceInfo::ResolveFilename(rootPath, filename);
						if (!filename.IsEmpty())
						{
							FileGlobList glob;

							// Does it have a wildcard in it?
							if (filename.Find('*') != -1  ||  filename.Find('?') != -1)
							{
								// Yes.  Run the globber.
								glob.MatchPattern(filename);
							}
							else
							{
								glob.push_back(std::string(filename));
							}

							for (FileGlobList::iterator it = glob.begin(); it != glob.end(); ++it)
							{
								filename = (*it).c_str();
								filename.Replace('/', '\\');

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
									if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||
										ext == "vcw"  ||  ext == "vcproj"  ||  ext == "csproj"  ||
										ext == "vbproj"  ||  ext == "stproj"  ||  ext == "sln")
										projectsToAdd.AddTail(filename);
								}
							}
						}
					}
				}
				
				childNode = (XmlNode*)childNode->GetNextSiblingNode();
			}
		}

		// Is it a File node?
		else if (node->GetName() == "File")
		{
			// Create and resolve the filename.
			XmlNode::Attribute* attr = node->FindAttribute("RelPath");
			if (attr)
			{
				CString filename = attr->GetValue();
				WorkspaceInfo::ResolveFilename(rootPath, filename);
				if (!filename.IsEmpty())
				{
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
						if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||
							ext == "vcw"  ||  ext == "vcproj"  ||  ext == "csproj"  ||
							ext == "vbproj"  ||  ext == "stproj"  ||  ext == "sln")
							projectsToAdd.AddTail(filename);
					}
				}
			}
		}
		else
		{
        	RecurseCSProjNode(node, rootPath, fileList, localListRefreshed, projectsToAdd);
		}

		node = (XmlNode*)node->GetNextSiblingNode();
	}
}


void WorkspaceInfo::ReadCSProjFile(Project& prj, CFile* inFile)
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
	WList<CString> projectsToAdd;
	bool prjIsExtraFiles = false;
	if (prj.GetName().CompareNoCase(GetExtraFilename() + ".dsp") != 0)
		projectsToAdd.AddTail(const_cast<CString&>(prj.GetName()));
	else
		prjIsExtraFiles = true;


	// Make sure no files have been touched yet.
	int fileListCount = fileList.GetCount();
	int i;
	for (i = 0; i < fileListCount; i++)
	{
		File* file = (File*)fileList.Get(i);
		file->m_touched = false;
	}

	bool localListRefreshed = false;

	// Determine the version of the project file.
	bool vs2005 = false;
	XmlNode* rootNode = prj.GetXmlData().GetRootNode()->Find("Project");
	if (rootNode)
	{
		XmlNode::Attribute* attr = rootNode->FindAttribute("xmlns");
		if (attr)
		{
			CString xmlns = attr->GetValue();
			if (xmlns == "http://schemas.microsoft.com/developer/msbuild/2003")
			{
				vs2005 = true;
			}
		}
	}

	if (vs2005)
	{
		RecurseCSProjNode(prj.GetXmlData().GetRootNode(), rootPath, fileList, localListRefreshed, projectsToAdd);
	}
	else
	{
		XmlNode* filesNode = prj.GetXmlData().Find("Files");
		RecurseCSProjNode(filesNode, rootPath, fileList, localListRefreshed, projectsToAdd);
	}

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
			endPos = line.Find('"', startPos);
			if (endPos == -1)
				continue;
		}
		else //if (line[namePos] == '"')
		{
			// Find a space, since that should denote the end of the filename.
			endPos = line.Find(' ', startPos);
		}

		// Got the name isolated.  Add it.
		CString projectPath = line.Mid(startPos, endPos - startPos);
		ResolveFilename(rootPath, projectPath);
		Project* newlyAddedProject = AddHelper(projectPath, "", prj.IsActive());
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


// Read in a .dsw file.
void WorkspaceInfo::ReadSlnFile(Project& prj)
{
	// Open the .sln file.
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
		// Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "WWhizNet", "WWhizNet.vcproj", "{4C0121D7-16F7-41E1-AEF1-C044693AFF30}"
		if (line.GetLength() <= 8  ||  strncmp(line, "Project(", 8) != 0)
			continue;

		// Search for the first ,.
		int endPos;		// Will be one past the last letter.
		int startPos = line.Find(',');
		if (startPos == -1)
			continue;

		// Find the first quote.
		startPos++;		// Move to the beginning of the name.
		while (line[startPos] != '"')
			startPos++;

		// See if the name is quoted.
		if (line[startPos] == '"')
		{
			// Move past the quote.
			startPos++;
			
			// Find the closing quote.
			endPos = line.Find('"', startPos);
			if (endPos == -1)
				continue;
		}

		// Got the name isolated.  Add it.
		CString projectPath = line.Mid(startPos, endPos - startPos);
		ResolveFilename(rootPath, projectPath);
		Project* newlyAddedProject = AddHelper(projectPath, "", prj.IsActive());
		if (newlyAddedProject)
		{
			newlyAddedProject->m_parent = &prj;
			File* vcprojFile = File::Create(projectPath);
			prj.m_fileList.Add(vcprojFile);
		}
	} //while(1)

	// Close the .sln file.
	file.Close();

	// Add the .sln file.
	File* slnFile = File::Create(prj.m_name);
	prj.m_fileList.Add(slnFile);

	prj.m_fileList.Sort();
}


// Add a new project or workspace to the list of projects.
Project* WorkspaceInfo::AddProject(CString projectName, bool active, bool noRefresh)
{
	if (projectName.IsEmpty())
		return NULL;

	return AddHelper(projectName, "", active, noRefresh);
}


POSITION FindProject(const CString& projectName)
{
	// Is it already in the list?
	ProjectList& projectList = WorkspaceInfo::GetProjectList();
	POSITION pos = projectList.GetHeadPosition();
	while (pos)
	{
		POSITION lastPos = pos;
		Project* compProject = projectList.GetNext(pos);

		if (projectName.CompareNoCase(compProject->GetName()) == 0)
			return lastPos;
	}

	return pos;
}

	
// Internal helper function.
Project* WorkspaceInfo::AddHelper(CString projectName, CString ext, bool active, bool noRefresh)
{
	// Resolve the filename.
	ResolveFilename(GetWorkspaceLocation(), projectName);
	
	// Make sure there is an extension.
	int dotPosition = projectName.ReverseFind('.');
	if (dotPosition == -1  &&  ext.IsEmpty())
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
		project = WNEW Project;
		project->m_name = projectName;
		project->m_newProject = true;
		project->SetActive(active);

		// Add it to the end of the projects list.
		projectPos = s_projects.AddTail(project);

		// Automatic refresh for a new project.
		noRefresh = false;
	}

	// Assign the project's refresh flag.
	project->m_noRefresh = noRefresh;

	// Workspace projects are assigned later.
	project->m_workspaceProject = false;

	// Determine which type of file this is:
	if (ext.IsEmpty())
	{
		ext = project->m_name.Mid(dotPosition + 1);
		ext.MakeLower();
	}

	// Check the project time stamp.
	if (noRefresh)
	{
		// Check any projects in the list.
		for (int i = 0; i < project->m_fileList.GetCount(); i++)
		{
			File* file = (File*)project->m_fileList.Get(i);
			file->m_touched = true;

			const CString& fullExt = file->GetExt();
			int dotPos = fullExt.ReverseFind('.');
			CString ext = dotPos == -1 ? fullExt : fullExt.Mid(dotPos + 1);
			if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||
				ext == "vcw"  ||  ext == "vcproj"  ||  ext == "csproj"  ||
				ext == "vbproj"  ||  ext == "stproj"  ||  ext == "sln")
			{
				if (projectName.CompareNoCase(file->GetFullName()) != 0)
					AddProject(file->GetCaseFullName(), project->IsActive(), noRefresh);
			}
		}
	}
	else
	{
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
			else if (ext == "csproj"  ||  ext == "vbproj"  ||  ext == "stproj")
			{
				ReadCSProjFile(*project);
			}
			else if (ext == "sln")
			{
				ReadSlnFile(*project);
			}
		}
		else
		{
			// Check any projects in the list.
			for (int i = 0; i < project->m_fileList.GetCount(); i++)
			{
				File* file = (File*)project->m_fileList.Get(i);
				const CString& fullExt = file->GetExt();
				int dotPos = fullExt.ReverseFind('.');
				CString ext = dotPos == -1 ? fullExt : fullExt.Mid(dotPos + 1);
				if (ext == "dsp"  ||  ext == "dsw"  ||  ext == "vcp"  ||
					ext == "vcw"  ||  ext == "vcproj"  ||  ext == "csproj"  ||
					ext == "vbproj"  ||  ext == "stproj"  ||  ext == "sln")
				{
					if (projectName.CompareNoCase(file->GetFullName()) != 0)
						AddProject(file->GetCaseFullName(), project->IsActive(), noRefresh);
				}
			}
		}
	}

	project->m_touched = true;
	
	return project;
}


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
	if (ObjModelHelper::VStudioExists())
	{
		CString workspaceName = g_wwhizInterface->GetWorkspaceName();
		if (!workspaceName.IsEmpty())
		{
			Project* workspace = AddProject(workspaceName, true);

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
				if (project)
				{
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

#if 0
		// This works, but it isn't complete.
#ifdef WWHIZ_VSNET
		if (s_windowListFileName.IsEmpty())
		{
			// Generate a unique temporary name.
			char* asciiTempName = _tempnam(NULL, "WW300WINDOWLIST_");
			s_windowListFileName = asciiTempName;
			free(asciiTempName);
		}

		FILE* f = fopen(s_windowListFileName, "wt");
		if (f)
		{
			fprintf(f, "<Files>\n");

			CComPtr<EnvDTE::Documents> pDocuments;
			g_pDTE->get_Documents(&pDocuments);

			long count;
			pDocuments->get_Count(&count);
			for (int i = 1; i <= count; ++i)
			{
				CComPtr<EnvDTE::Document> pDocument;
				pDocuments->Item(CComVariant(i), &pDocument);

				CComBSTR bstrFullName;
				pDocument->get_FullName(&bstrFullName);

				CString fullName(bstrFullName);
				fprintf(f, "\t<File RelativePath=\"%s\"/>\n", fullName);
			}

			fprintf(f, "</Files>\n");
			fclose(f);

			AddHelper(s_windowListFileName, "vcproj", true, false);
		}

		_unlink(s_windowListFileName);
#endif WWHIZ_VSNET
#endif 0
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


