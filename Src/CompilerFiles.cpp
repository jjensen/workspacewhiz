#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "CompilerFiles.h"

FileList CompilerFiles::m_fileList;
bool CompilerFiles::m_updated = false;

// Clean the projects and filenames lists.
void CompilerFiles::RemoveAll(void)
{
	// Clean the filenames list.
	m_fileList.RemoveAll();
}


// Add a new project or workspace to the list of projects.
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
//			Add(pathName + fd.cFileName + "\\");
			if (!FindNextFile(handle, &fd))
				break;
			continue;
		}

		// Create the file structure.
		File file;
		if (File::Create(file, "", pathName + fd.cFileName, false) == false)
		{
			if (!FindNextFile(handle, &fd))
				break;
			continue;
		}

		// Insert it at the end.
		m_fileList.Add(file);

		if (!FindNextFile(handle, &fd))
			break;
	}
}


// The buffer will be modified.
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
	if (m_updated  &&  !force)
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

	m_fileList.Sort();

	m_updated = true;
	
	// Rebuilt stuff.
	return false;
}

