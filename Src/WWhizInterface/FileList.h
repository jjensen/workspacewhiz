///////////////////////////////////////////////////////////////////////////////
// $Workfile: FileList.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/FileList.h $
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

#include "TagList.h"

class WorkspaceInfo;

class File;

class File : public WWhizFile
{
	friend class WorkspaceInfo;
	friend class WorkspaceTags;

	WWHIZ_DECLARE_REFCOUNT()
public:
	File();
	~File();
	
	virtual const CString& GetFullName() const;
	virtual CString GetCaseFullName() const;
	virtual const CString& GetShortName(void) const;
	virtual const CString& GetTitle(void) const;
	virtual const CString& GetExt(void) const;
	virtual const CString& GetPath(void) const;
	virtual const CTime& GetTimeStamp(void) const;
	virtual WWhizTagList& GetTagList(void) const;

	virtual void SetShortName(const CString& shortName);
	virtual void SetTitle(const CString& title);
	virtual void SetExt(const CString& ext);
	virtual void SetPath(const CString& path);
	virtual void SetTimeStamp(CTime timeStamp);

	virtual WWhizTagList& GetOrderedTagList(void) const;

	virtual bool IsWorkspaceFile(void) const;

	static bool Create(File& file, const CString& filename);
	static File* Create(const CString& filename);

	const CTime& GetOldTimeStamp(void) const;		// Private.

	void ClearTags();

	AutoBasic<BYTE> m_tagBuffer;
	DWORD m_tagBufferSize;

	Tag* m_tags;

protected:
	CString m_shortName;
	CString m_title;
	CString m_ext;
	CString m_path;
	CString m_fullName;		// Lowercase

	CTime m_timeStamp;		// The time stamp of the file.
	CTime m_oldTimeStamp;	// The old time stamp, in case we need to restore it.
	bool m_touched:1;
	bool m_changed:1;
	bool m_workspaceFile:1;

	CTime m_lastTagUpdateTime;

	TagList m_tagList;
	TagList m_orderedTagList;
};


/**
**/
class FileMap
{
public:
	~FileMap();

	File* Get(CString fullName) const;
	bool Get(File* file) const;
	bool Add(File* file);
	bool Remove(File* file);
	void RemoveAll();
	void CleanUp();

	void InitHashTable(DWORD hashSize);

protected:
	WMap<CString, File*> m_fileMap;
};


class FileList : public WWhizFileList
{
public:
	FileList();
	~FileList();

	virtual int GetCount(void) const;
	virtual WWhizFile* Get(int index) const;
	virtual WWhizFile* Find(CString fullName) const;
	virtual int FindExact(WWhizFile& file) const;
	virtual int FindNext(int startPos, WWhizFile& file) const;
	virtual int FindPrevious(int startPos, WWhizFile& file) const;

	virtual WWhizFile& Create(const CString& filename);

// Operations	
	File* Add(const CString& fullPath);
	bool Add(File* file);
	void Remove(int index);
	void RemoveAll(void);
	void Sort(void);

protected:	
	void BuildArray(void);

	WArray<File*> m_files;
	FileMap m_fileMap;
};

// Here because of #include chain.
inline LPCSTR Tag::GetFilename(void) const
{
	return m_parent->File::GetFullName();
}


/**
**/
inline const CString& File::GetFullName() const
{
	return m_fullName;
}


/**
**/
inline CString File::GetCaseFullName() const
{
	CString temp = m_path + m_title;
	if (!m_ext.IsEmpty())
		temp += "." + m_ext;
	return temp;
}


/**
**/
inline const CString& File::GetShortName(void) const
{
	return m_shortName;
}


/**
**/
inline const CString& File::GetTitle(void) const
{
	return m_title;
}


/**
**/
inline const CString& File::GetExt(void) const
{
	return m_ext;
}


/**
**/
inline const CString& File::GetPath(void) const
{
	return m_path;
}


/**
**/
inline const CTime& File::GetOldTimeStamp(void) const
{
	return m_oldTimeStamp;
}


/**
**/
inline const CTime& File::GetTimeStamp(void) const
{
	return m_timeStamp;
}


/**
**/
inline WWhizTagList& File::GetTagList(void) const
{
	return (WWhizTagList&)m_tagList;
}


/**
**/
inline WWhizTagList& File::GetOrderedTagList(void) const
{
	return (WWhizTagList&)m_orderedTagList;
}


/**
**/
inline void File::SetShortName(const CString& shortName)
{
	m_shortName = shortName;
}


/**
**/
inline void File::SetTitle(const CString& title)
{
	m_title = title;
}


/**
**/
inline void File::SetExt(const CString& ext)
{
	m_ext = ext;
}


/**
**/
inline void File::SetPath(const CString& path)
{
	m_path = path;
}


/**
**/
inline void File::SetTimeStamp(CTime timeStamp)
{
	m_oldTimeStamp = m_timeStamp;
	m_timeStamp = timeStamp;
}


/**
**/
inline bool File::IsWorkspaceFile(void) const
{
	return m_workspaceFile;
}

	
/**
**/
inline int FileList::GetCount(void) const
{
	return m_files.GetCount();
}


/**
**/
inline WWhizFile* FileList::Get(int index) const
{
	return m_files[index];
}


/**
**/
inline WWhizFile* FileList::Find(CString fullName) const
{
	return m_fileMap.Get(fullName);
}
