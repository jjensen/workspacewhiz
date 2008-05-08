///////////////////////////////////////////////////////////////////////////////
// $Workfile: FileList.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/FileList.cpp $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "FileList.h"
#include "WorkspaceInfo.h"
#include <algorithm>

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// FileMap
///////////////////////////////////////////////////////////////////////////////

FileMap::~FileMap()
{
	RemoveAll();
}


File* FileMap::Get(CString fullName) const
{
	fullName.MakeLower();
	File* file;
	if (m_fileMap.Lookup(fullName, file))
		return file;
	return NULL;
}


bool FileMap::Get(File* file) const
{
	if (m_fileMap.Lookup(file->GetFullName(), file))
		return true;
	return false;
}

	
bool FileMap::Add(File* file)
{
	m_fileMap[file->GetFullName()] = file;
	file->AddRef();

	return true;
}


bool FileMap::Remove(File* file)
{
	m_fileMap.RemoveKey(file->GetFullName());
	file->Release();

	return true;
}


void FileMap::RemoveAll()
{
	CString fullName;
	File* file;

	POSITION pos = m_fileMap.GetStartPosition();
	while (pos)
	{
		m_fileMap.GetNextAssoc(pos, fullName, file);
		file->Release();
	}

	m_fileMap.RemoveAll();
}


void FileMap::InitHashTable(DWORD hashSize)
{
	m_fileMap.InitHashTable(hashSize);
}


FileMap g_globalFileMap;

/**
	Only valid for the global file map.
**/
void FileMap::CleanUp()
{
	if (this != &g_globalFileMap)
		return;

	POSITION pos = m_fileMap.GetStartPosition();
	while (pos)
	{
		CString fullName;
		File* file;
		m_fileMap.GetNextAssoc(pos, fullName, file);
		if (file->GetRefCount() == 1)
			Remove(file);
	}
}


///////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////

WWHIZ_DEFINE_REFCOUNT(File)

/**
**/
bool File::Create(File& file, const CString& filenameConst)
{
	CString filename = filenameConst;

	// Find the path.
	int pathEndPosition = filename.ReverseFind('\\');
	if (pathEndPosition == -1)
		return false;
	file.SetPath(filename.Left(pathEndPosition + 1));
	filename = filename.Mid(pathEndPosition + 1);

	// Find the extension.
	int extPosition = filename.ReverseFind('.');
	if (extPosition != -1)
	{
		file.m_ext = filename.Mid(extPosition + 1);
		file.m_ext.MakeLower();
	}
	else
		extPosition = filename.GetLength();

	// Find the file title.
	file.m_title = filename.Left(extPosition);

	// Build the shortened name (no symbols).
	char shortName[200];
	int sLen = 0;
	for (int i = 0; i < file.m_title.GetLength(); i++)
		if (isalnum(file.m_title[i]))
			shortName[sLen++] = (char)tolower(file.m_title[i]);
	shortName[sLen] = 0;
	file.m_shortName = shortName;

	file.m_fullName = file.m_path + file.m_title;
	if (!file.m_ext.IsEmpty())
		file.m_fullName += "." + file.m_ext;

	file.m_fullName.MakeLower();

	file.m_timeStamp = CTime(0);

	return true;
}


/**
**/
File* File::Create(const CString& filename)
{
	// Check the map.
	File* file = g_globalFileMap.Get(filename);
	if (file)
		return file;

	// Create a new file.
	file = WNEW File;
	File::Create(*file, filename);

	// Add to map.
	g_globalFileMap.Add(file);

	return file;
}


/**
**/
File::File() :
	m_touched(0),
	m_changed(false),
	m_tagBufferSize(0),
	m_tags(NULL),
	m_workspaceFile(false)
{
}


File::~File()
{
	ClearTags();
}


void File::ClearTags()
{
	// Clean up the tag lists and tag buffer.
	m_tagBuffer = NULL;
	m_tagBufferSize = 0;
	delete [] m_tags;
	m_tags = NULL;
	m_orderedTagList.RemoveAll();
	m_tagList.RemoveAll();
}

	
///////////////////////////////////////////////////////////////////////////////
// FileList
///////////////////////////////////////////////////////////////////////////////

FileList::FileList()
{
	m_files.SetCount(0, 100);
}


FileList::~FileList()
{
	RemoveAll();
}


/**
	Remove all files from the container.
**/
void FileList::RemoveAll()
{
	m_files.RemoveAll();
	m_fileMap.RemoveAll();
}


WWhizFile& FileList::Create(const CString& filename)
{
	File* file = WNEW File;
	File::Create(*file, filename);
	file->AddRef();
	return *file;
}


/*
static int __cdecl CompareArray(const void* elem1, const void* elem2)
{
	File* file1 = *(File**)elem1;
	File* file2 = *(File**)elem2;

	int ret = file1->GetShortName().Compare(file2->GetShortName());
	if (ret == 0)
	{
		// If the names match, compare against the file extensions.
		ret = file1->GetExt().Compare(file2->GetExt());
		if (ret == 0)
		{
			// If the extensions match, compare against the path.
			ret = file1->GetPath().CompareNoCase(file2->GetPath());
		}
	}
	return ret;
}
*/	

// Return whether first element is greater than the second
static bool CompareElements( File* file1, File* file2 )
{
	int ret = file1->GetShortName().Compare(file2->GetShortName());
	if (ret == 0)
	{
		// If the names match, compare against the file extensions.
		ret = file1->GetExt().Compare(file2->GetExt());
		if (ret == 0)
		{
			// If the extensions match, compare against the path.
			ret = file1->GetPath().CompareNoCase(file2->GetPath());
		}
	}

	return ret < 0;
}


void FileList::Sort()
{
	// Sort the file array.
	std::sort(m_files.GetData(), m_files.GetData() + m_files.GetCount(), CompareElements);
//	qsort(m_files.GetData(), m_files.GetCount(), sizeof(File*), CompareArray);
}

// Find exact file index.
int FileList::FindExact(WWhizFile& file) const
{
	// Scan the file list.
	for (int i = 0; i < GetCount(); i++)
	{
		WWhizFile* fileCmp = Get(i);

		// Compare the extension.
		if (!file.GetExt().IsEmpty()  &&  file.GetExt() != fileCmp->GetExt())
			continue;

		// Compare the file titles.
		if (file.GetTitle().CompareNoCase(fileCmp->GetTitle()) != 0)
			continue;

		// Compare the path.
		if (file.GetPath().CompareNoCase(fileCmp->GetPath()) == 0)
		{
			return i;
		}
	}

	return -1;
}


// Find next file index.
int FileList::FindNext(int startPos, WWhizFile& file) const
{
	// Scan the file list.
	for (int i = startPos + 1; i < GetCount(); i++)
	{
		WWhizFile* fileCmp = Get(i);

		// Compare the file titles.
		if (file.GetTitle().CompareNoCase(fileCmp->GetTitle()) == 0)
			return i;
	}

	for (i = 0; i < startPos; i++)
	{
		WWhizFile* fileCmp = Get(i);

		// Compare the file titles.
		if (file.GetTitle().CompareNoCase(fileCmp->GetTitle()) == 0)
			return i;
	}

	return -1;
}


/**
	Find previous file index.
**/
int FileList::FindPrevious(int startPos, WWhizFile& file) const
{
	// Scan the file list.
	for (int i = startPos - 1; i >= 0; i--)
	{
		WWhizFile* fileCmp = Get(i);

		// Compare the file titles.
		if (file.GetTitle().CompareNoCase(fileCmp->GetTitle()) == 0)
			return i;
	}

	for (i = GetCount() - 1; i > startPos; i--)
	{
		WWhizFile* fileCmp = Get(i);

		// Compare the file titles.
		if (file.GetTitle().CompareNoCase(fileCmp->GetTitle()) == 0)
			return i;
	}

	return -1;
}


/**
**/
File* FileList::Add(const CString& fullPath)
{
	File* file = File::Create(fullPath);
	m_fileMap.Add(file);
	return file;
}


/**
	Add file to the end of the file list.
**/
bool FileList::Add(File* file)
{
	if (!m_fileMap.Get(file))
	{
		m_files.Add(file);
		m_fileMap.Add(file);
		return true;
	}

	return false;
}

	
/**
**/
void FileList::Remove(int index)
{
	m_fileMap.Remove(m_files[index]);
	m_files.RemoveAt(index);
}

	
