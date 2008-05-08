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
#include "CompilerFiles.h"
#include "FileList.h"

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
	bool vc70 = version == "7.00";

	CComPtr<EnvDTE::Project> pProject;
	CComPtr<IDispatch> pID;
	CComPtr<IDispatch> pDispProjEngine;

	ObjModelHelper objModelHelper;
	objModelHelper.GetActiveProject(pProject);

	CComQIPtr<VCProjectEngineLibrary70::VCProjectEngine> pProjEng70;
	CComQIPtr<VCProjectEngineLibrary70::VCProject> cpProject70;
	CComQIPtr<VCProjectEngineLibrary71::VCProjectEngine> pProjEng71;
	CComQIPtr<VCProjectEngineLibrary71::VCProject> cpProject71;

	if (pProject)
	{
		pProject->get_Object( &pID );
		if (vc70)
		{
			cpProject70 = pID;
			cpProject70->get_VCProjectEngine(&pDispProjEngine);
			pProjEng70 = pDispProjEngine;
		}
		else
		{
			cpProject71 = pID;
			cpProject71->get_VCProjectEngine(&pDispProjEngine);
			pProjEng71 = pDispProjEngine;
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
		if (vc70)
		{
			if (pProjEng70)
			{
				pProjEng70->Evaluate(CComBSTR(lastPtr), &bstrResolvedDir);
			}
		}
		else
		{
			if (pProjEng71)
			{
				pProjEng71->Evaluate(CComBSTR(lastPtr), &bstrResolvedDir);
			}
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
	HKEY key;
#ifdef WWHIZ_VC6
Top:
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
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	CString version = ObjModelHelper::GetVersion();

	LONG lRes;
	
	if (version == "7.00")
	{
		lRes =
			RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"Software\\Microsoft\\VisualStudio\\7.0\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories",
			0, KEY_ALL_ACCESS, &key);
	}
	else if (version == "7.10")
	{
		lRes =
			RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"Software\\Microsoft\\VisualStudio\\7.1\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories",
			0, KEY_ALL_ACCESS, &key);
	}

	if (lRes != ERROR_SUCCESS)
	{
		return true;
	}
#endif WWHIZ_VSNET

	s_lastConfigName = configName;
	
	char buffer[5000];
	DWORD len = 5000;
	QueryValueString(key, "Include Dirs", buffer, len);
	ProcessPaths(buffer);
	len = 5000;
	QueryValueString(key, "Source Dirs", buffer, len);
	ProcessPaths(buffer);

	RegCloseKey(key);

	s_fileList.Sort();

	s_updated = true;
	
	// Rebuilt stuff.
	return false;
}


