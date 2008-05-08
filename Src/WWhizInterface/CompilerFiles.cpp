///////////////////////////////////////////////////////////////////////////////
// $Workfile: CompilerFiles.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/CompilerFiles.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 8    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "CompilerFiles.h"
#include "FileList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
			File* file = new File;
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
	char* ptr;
	char* lastPtr = buffer;
	do
	{
		ptr = strchr(lastPtr, ';');
		if (ptr)
		{
			*ptr++ = 0;
		}

		Add(CString(lastPtr) + "\\");

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


// Refresh the path list.
bool CompilerFiles::Refresh(bool force)
{
	if (s_updated  &&  !force)
		return true;

	// Delete everything.
	RemoveAll();

	// Read the registry to get the paths.
	HKEY key;
	LONG lRes =
		RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Devstudio\\6.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories",
		0, KEY_ALL_ACCESS, &key);
	if (lRes != ERROR_SUCCESS)
	{
		lRes =
			RegOpenKeyEx(HKEY_CURRENT_USER,
			"Software\\Microsoft\\Devstudio\\5.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories",
			0, KEY_ALL_ACCESS, &key);
		if (lRes != ERROR_SUCCESS)
		{
			return true;
		}
	}

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

