///////////////////////////////////////////////////////////////////////////////
// $Workfile: CompilerFiles.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/CompilerFiles.cpp $
// $Date: 2003/06/23 $ $Revision: #8 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizInterface.h"
#include "CompilerFiles.h"
#include "FileList.h"

#ifdef WWHIZ_VSNET

#include <atlsafe.h>
#include "VCProjectEngine90.tlh"
#include "VCProjectEngine70.tlh"
#include "VCProjectEngine71.tlh"
#include "VCProjectEngine80.tlh"

#endif // WWHIZ_VSNET

static FileList s_fileList;
static bool s_updated = false;

/**
**/
FileList& CompilerFiles::GetFileList(void)
{
	return s_fileList;
}


/**
	Clean the projects and filenames lists.
**/
void CompilerFiles::RemoveAll(void)
{
	// Clean the filenames list.
	s_fileList.RemoveAll();
}


/**
	Add a new project or workspace to the list of projects.
**/
void CompilerFiles::Add(CString pathName)
{
	WIN32_FIND_DATA fd;
	HANDLE handle = ::FindFirstFile(pathName + "*.*", &fd);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(fd.cFileName, ".") != 0  &&  strcmp(fd.cFileName, "..") != 0)
				Add(pathName + fd.cFileName + "\\");
		}
		else
		{
			// Create the file structure.
			File* file = WNEW File;
			file->AddRef();
			if (File::Create(*file, pathName + fd.cFileName) == false)
			{
				file->Release();
				if (!FindNextFile(handle, &fd))
					break;
				continue;
			}

			// Insert it at the end.
			s_fileList.Add(file);
			file->Release();
		}

		if (!FindNextFile(handle, &fd))
			break;
	}
}


/**
	The buffer will be modified.
**/
void CompilerFiles::ProcessPaths(char* buffer)
{
#ifdef WWHIZ_VSNET
	CComBSTR version;
	g_pDTE->get_Version(&version);

	CComPtr<EnvDTE::Project> pProject;
	CComPtr<IDispatch> pID;
	CComPtr<IDispatch> pDispProjEngine;

	ObjModelHelper objModelHelper;
	objModelHelper.GetActiveProject(pProject);

	CComQIPtr<VCProjectEngineLibrary70::VCProjectEngine> pProjEng70;
	CComQIPtr<VCProjectEngineLibrary70::VCProject> cpProject70;
	CComQIPtr<VCProjectEngineLibrary71::VCProjectEngine> pProjEng71;
	CComQIPtr<VCProjectEngineLibrary71::VCProject> cpProject71;
	CComQIPtr<VCProjectEngineLibrary80::VCProjectEngine> pProjEng80;
	CComQIPtr<VCProjectEngineLibrary80::VCProject> cpProject80;
	CComQIPtr<VCProjectEngineLibrary90::VCProjectEngine> pProjEng90;
	CComQIPtr<VCProjectEngineLibrary90::VCProject> cpProject90;

	if (pProject)
	{
		pProject->get_Object( &pID );
		if (version == "7.00")
		{
			cpProject70 = pID;
			cpProject70->get_VCProjectEngine(&pDispProjEngine);
			pProjEng70 = pDispProjEngine;
			if (!pProjEng70)
				return;
		}
		else if (version == "7.10")
		{
			cpProject71 = pID;
			cpProject71->get_VCProjectEngine(&pDispProjEngine);
			pProjEng71 = pDispProjEngine;
			if (!pProjEng71)
				return;
		}
		else if (version == "8.0")
		{
			cpProject80 = pID;
			cpProject80->get_VCProjectEngine(&pDispProjEngine);
			pProjEng80 = pDispProjEngine;
			if (!pProjEng80)
				return;
		}
		else if (version == "9.0")
		{
			cpProject90 = pID;
			cpProject90->get_VCProjectEngine(&pDispProjEngine);
			pProjEng90 = pDispProjEngine;
			if (!pProjEng90)
				return;
		}
	}
#endif WWHIZ_VSNET

	char* ptr;
	char* lastPtr = buffer;
	do
	{
		ptr = strchr(lastPtr, ';');
		if (ptr)
		{
			*ptr++ = 0;
		}

#ifdef WWHIZ_VC6
		Add(CString(lastPtr) + "\\");
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
		CComBSTR bstrResolvedDir;
		if (pProjEng70)
		{
			pProjEng70->Evaluate(CComBSTR(lastPtr), &bstrResolvedDir);
		}
		else if (pProjEng71)
		{
			pProjEng71->Evaluate(CComBSTR(lastPtr), &bstrResolvedDir);
		}
		else if (pProjEng80)
		{
			pProjEng80->Evaluate(CComBSTR(lastPtr), &bstrResolvedDir);
		}

		if (bstrResolvedDir == "")
			bstrResolvedDir = lastPtr;

		CString dir(bstrResolvedDir);
		dir.TrimRight('\\');
		dir += "\\";

		Add(dir);
#endif WWHIZ_VSNET

		lastPtr = ptr;
	} while (ptr);
}


bool QueryValueString(HKEY key, LPCTSTR lpszValueName, LPTSTR szValue, DWORD& pdwCount)
{
	DWORD dwType = NULL;
	LONG lRes = RegQueryValueEx(key, (LPTSTR)lpszValueName, NULL, &dwType,
		(LPBYTE)szValue, &pdwCount);

	if (dwType != REG_SZ)
	{
		return false;
	}
	if (lRes != ERROR_SUCCESS)
	{
		return false;
	}

	return true;
}


static CString s_lastConfigName;


// Refresh the path list.
bool CompilerFiles::Refresh(bool force)
{
	ObjModelHelper objModel;
	CString configName = objModel.GetCurrentConfigName();
	if (configName != s_lastConfigName)
		force = true;
	
	if (s_updated  &&  !force)
		return true;

	// Delete everything.
	RemoveAll();

	// Read the registry to get the paths.
	CComBSTR bstrIncludeDirectories;
	CComBSTR bstrSourceDirectories;

#ifdef WWHIZ_VC6
Top:
	HKEY key;
	LONG lRes =
		RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Devstudio\\6.0\\Build System\\Components\\Platforms\\" + configName + "\\Directories",
		0, KEY_ALL_ACCESS, &key);
	if (lRes != ERROR_SUCCESS)
	{
		lRes =
			RegOpenKeyEx(HKEY_CURRENT_USER,
			"Software\\Microsoft\\Devstudio\\5.0\\Build System\\Components\\Platforms\\" + configName + "\\Directories",
			0, KEY_ALL_ACCESS, &key);
		if (lRes != ERROR_SUCCESS)
		{
			if (configName != "Win32 (x86)")
			{
				configName = "Win32 (x86)";
				goto Top;
			}
			return true;
		}
	}

	char buffer[5000];
	DWORD len = 5000;
	QueryValueString(key, "Include Dirs", buffer, len);
	bstrIncludeDirectories = buffer;

	len = 5000;
	QueryValueString(key, "Source Dirs", buffer, len);
	bstrSourceDirectories = buffer;

	RegCloseKey(key);

#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	CString version = ObjModelHelper::GetVersion();

	// Look up the DTE.Solution object.
	CComPtr<EnvDTE::_Solution> pSolution;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
	{
		return false;
	}

	// Look up the DTE.Solution.SolutionBuild object.
	CComPtr<EnvDTE::SolutionBuild> m_pSolutionBuild;
	pSolution->get_SolutionBuild(&m_pSolutionBuild);
	if (!m_pSolutionBuild)
	{
		return false;
	}

	// DTE.Solution.SolutionBuild.ActiveConfiguration
	CComPtr<EnvDTE::SolutionConfiguration> m_pSolutionConfiguration;
	m_pSolutionBuild->get_ActiveConfiguration(&m_pSolutionConfiguration);
	if (!m_pSolutionConfiguration)
	{
		return false;
	}

	// DTE.Solution.SolutionBuild.ActiveConfiguration.SolutionContexts
	CComPtr<EnvDTE::SolutionContexts> m_pSolutionContexts;
	m_pSolutionConfiguration->get_SolutionContexts(&m_pSolutionContexts);
	if (!m_pSolutionContexts)
	{
		return false;
	}

	// Look up the DTE.Solution.SolutionBuild.StartupProjects array.
	CComVariant pProjectVariant;
	if (FAILED(m_pSolutionBuild->get_StartupProjects(&pProjectVariant)))
	{
		return false;
	}

	// If the array has nothing in it, then they don't have any startup projects.
	CComSafeArray<VARIANT> startupProjects;
	startupProjects.Attach(pProjectVariant.parray);
	if (startupProjects.GetCount() == 0)
	{
		return false;
	}

	VARIANT& startupProjectPath = startupProjects.GetAt(0);
	startupProjects.Detach();

	// DTE.Solution.Projects
	CComPtr<EnvDTE::Projects> pProjects;
	pSolution->get_Projects(&pProjects);
	if (!pProjects)
	{
		return false;
	}

	// Look up the project object for the startup project.
	CComPtr<EnvDTE::Project> pActiveProject;
	pProjects->Item(startupProjectPath, &pActiveProject);
	if (!pActiveProject)
	{
		return false;
	}

	// Test the GUID and make sure we're only looking at C/C++ projects.
	CComBSTR cppKind = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
	CComBSTR bstrKind;
	pActiveProject->get_Kind(&bstrKind);
	if (bstrKind != cppKind)
	{
		return false;
	}

	// VCProject
	CComPtr<IDispatch> pDispatch;
	pActiveProject->get_Object(&pDispatch);

	// Get the project's unique name to look up its solution context.
	CComBSTR projectUniqueName;
	pActiveProject->get_UniqueName(&projectUniqueName);

	// Look up the solution context.
	CComPtr<EnvDTE::SolutionContext> pSolutionContext;
	m_pSolutionContexts->Item(CComVariant(projectUniqueName), &pSolutionContext);

	CComBSTR bstrPlatformName;
	pSolutionContext->get_PlatformName(&bstrPlatformName);
/*
	CComPtr<EnvDTE::Properties> props;
	g_pDTE->get_Properties(CComBSTR("Projects"), CComBSTR("VCDirectories"), &props);
	
	CComPtr<EnvDTE::Property> pItem;
	props->Item(CComVariant("Platforms"), &pItem);

	CComPtr<IDispatch> pDispatch;
	pItem->get_Object(&pDispatch);
*/
	if (version == "7.00")
	{
		CComQIPtr<VCProjectEngineLibrary70::IVCCollection> pVCCollection(pDispatch);
		pDispatch = NULL;

		pVCCollection->Item(CComVariant(bstrPlatformName), &pDispatch);

		CComQIPtr<VCProjectEngineLibrary70::VCPlatform> pVCPlatform(pDispatch);

		pVCPlatform->get_IncludeDirectories(&bstrIncludeDirectories);
		pVCPlatform->get_SourceDirectories(&bstrSourceDirectories);
	}
	else if (version == "7.10")
	{
		CComQIPtr<VCProjectEngineLibrary71::IVCCollection> pVCCollection(pDispatch);
		pDispatch = NULL;

		pVCCollection->Item(CComVariant(bstrPlatformName), &pDispatch);

		CComQIPtr<VCProjectEngineLibrary71::VCPlatform> pVCPlatform(pDispatch);

		pVCPlatform->get_IncludeDirectories(&bstrIncludeDirectories);
		pVCPlatform->get_SourceDirectories(&bstrSourceDirectories);
	}
	else if (version == "8.0")
	{
		CComQIPtr<VCProjectEngineLibrary80::VCProject> pVCProject(pDispatch);
		pDispatch = NULL;

		// Get the platform pointer...
		CComPtr<IDispatch> pDispPlatforms;
		pVCProject->get_Platforms(&pDispPlatforms);
		CComQIPtr<VCProjectEngineLibrary80::IVCCollection> pPlatforms(pDispPlatforms);
		CComPtr<IDispatch> pDispPlatform;
		pPlatforms->Item(CComVariant(bstrPlatformName), &pDispPlatform);

		CComQIPtr<VCProjectEngineLibrary80::VCPlatform> pVCPlatform(pDispPlatform);

		pVCPlatform->get_IncludeDirectories(&bstrIncludeDirectories);
		pVCPlatform->get_SourceDirectories(&bstrSourceDirectories);
	}
	else if (version == "9.0")
	{
		CComQIPtr<VCProjectEngineLibrary90::VCProject> pVCProject(pDispatch);
		pDispatch = NULL;

		// Get the platform pointer...
		CComPtr<IDispatch> pDispPlatforms;
		pVCProject->get_Platforms(&pDispPlatforms);
		CComQIPtr<VCProjectEngineLibrary90::IVCCollection> pPlatforms(pDispPlatforms);
		CComPtr<IDispatch> pDispPlatform;
		pPlatforms->Item(CComVariant(bstrPlatformName), &pDispPlatform);

		CComQIPtr<VCProjectEngineLibrary90::VCPlatform> pVCPlatform(pDispPlatform);

		pVCPlatform->get_IncludeDirectories(&bstrIncludeDirectories);
		pVCPlatform->get_SourceDirectories(&bstrSourceDirectories);
	}

#endif WWHIZ_VSNET

	s_lastConfigName = configName;
	
	CString dir = CString(bstrIncludeDirectories);
	ProcessPaths((char*)(const char*)dir);
	dir = bstrSourceDirectories;
	ProcessPaths((char*)(const char*)dir);

	s_fileList.Sort();

	s_updated = true;
	
	// Rebuilt stuff.
	return false;
}


