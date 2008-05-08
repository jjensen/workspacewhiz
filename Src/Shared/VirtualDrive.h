///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDrive.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDrive.h $
// $Date: 2003/01/05 $ $Revision: #8 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <afx.h>
#include <afxtempl.h>

#include "../3rdParty/WCollection.h"

#define NEED_VIRTUAL_DRIVE_HASHKEY
#ifdef NEED_VIRTUAL_DRIVE_HASHKEY

template<> inline UINT AFXAPI HashKey<CString> (CString _key)
{
	LPCSTR key = _key;
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

template<> inline UINT AFXAPI HashKey<const CString&> (const CString& _key)
{
	LPCSTR key = _key;
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

#endif NEED_VIRTUAL_DRIVE_HASHKEY

class VirtualFile;

/**
	A virtual drive is housed within a physical disk file.  The disk file
	contains one or more files, known as virtual files, and a directory
	describing all the files.  Similar in concept to WAD files.

	@see VirtualFile
**/
class VirtualDrive
{
public:
	enum {  INVALID_FILE_ENTRY = 0xFFFFFFFF  };

	/**
		Represents a file entry within the virtual drive's directory.
	**/
	class FileEntry
	{
	public:
		const CString& GetFilename() const			{  return m_filename;  }
		const CTime& GetTimeStamp() const			{  return m_fileTime;  }
		DWORD GetCRC() const						{  return m_crc;  }

		BYTE GetUserData(UINT which) const			{  return m_userData[which];  }
		DWORD GetUserDataDWORD(UINT which) const	{  return *(DWORD*)&m_userData[which];  }
		void SetUserData(UINT which, BYTE data)		{  m_userData[which] = data;  m_parentDrive->m_changed = true;  }
		void SetUserDataDWORD(UINT which, DWORD data) {  *(DWORD*)&m_userData[which] = data;  m_parentDrive->m_changed = true;  }
		void SetTimeStamp(const CTime& fileTime)	{  m_fileTime = fileTime;  m_parentDrive->m_changed = true;  }
		void SetCRC(DWORD crc)						{  m_crc = crc;  m_parentDrive->m_changed = true;  }

	protected:
		CString			m_filename;
		CTime			m_fileTime;
		DWORD			m_offset;
		DWORD			m_size;
		DWORD			m_crc;
		BYTE			m_userData[16];

		VirtualDrive*	m_parentDrive;

		friend class VirtualDrive;
		friend class VirtualFile;
	};

	///////////////////////////////////////////////////////////////////////////
	VirtualDrive();
	~VirtualDrive();

	bool Create(const CString& filename);
	bool Open(const CString& filename);
	bool Close(void);
	bool Flush();

	bool IsReadOnly() const					{  return m_readOnly;  }

	bool FileCreate(const CString& filename, VirtualFile& virtualFile, const CTime* fileTime = NULL);
	bool FileOpen(const CString& filename, VirtualFile& virtualFile);
	bool FileClose(VirtualFile& datafile);
	void FileCloseAll();

	bool FileErase(const CString& filename);
	bool FileRename(const CString& oldName, const CString& newName);

	bool FileCopy(CFile& srcFile, const CString& destFilename, const CTime* fileTime = NULL);

	bool Pack();

	CFile* GetParentFile() const;
	const CString& GetFilename() const;

	UINT GetFileEntryCount(void) const;
	VirtualDrive::FileEntry* GetFileEntry(UINT entry);
	VirtualDrive::FileEntry* GetFileEntry(const CString& Filename);

	UINT GetFileEntryIndex(const CString& filename);
	int	GetNumberOfFilesOpen(void) const;

	BYTE GetHeaderUserData(int which) const				{  return m_header.m_userData[which];  }
	DWORD GetHeaderUserDataDWORD(int which) const		{  return *(DWORD*)&m_header.m_userData[which];  }
	void SetHeaderUserData(int which, BYTE data)		{  m_header.m_userData[which] = data;  m_changed = true;  }
	void SetHeaderUserDataDWORD(int which, DWORD data)	{  *(DWORD*)&m_header.m_userData[which] = data;  m_changed = true;  }

private:
#include <pshpack1.h>
    struct FileHeader
    {
        char    m_id[8];
		WORD	m_version;
		WORD	m_fileEntryCount;
        DWORD   m_dirOffset;
		DWORD	m_dirSize;			// Directory size in bytes.
		bool	m_needPack;
		BYTE	m_userData[15];

		void Reset();
    };
#include <poppack.h>

	CString m_filename;
	FileHeader m_header;
	WArray<FileEntry> m_dir;

	bool m_readOnly;
	CFile* m_parentFile;
	bool m_ownParentFile;

	WList<VirtualFile*> m_openFiles;			//!< All open files.
	bool m_changed;								//!< Whether the file needs to be flushed.
	VirtualFile* m_curWriteFile;				//!< The current file being written to.
	WMap<CString, UINT> m_filenameMap;	//!< The filename to directory index map.

	friend class VirtualDriveManager;
	friend class VirtualFile;
	friend class FileEntry;
};


/**
	@return Returns the pointer to the CFile-derived object used for all virtual
		drive file operations.
**/
inline CFile* VirtualDrive::GetParentFile() const
{
	return m_parentFile;
}


/**
	@return Retrieves the filename of the virtual drive.
**/
inline const CString& VirtualDrive::GetFilename() const
{
	return m_filename;
}


/**
	@return Returns the number of file entries in the virtual drive.
**/
inline UINT VirtualDrive::GetFileEntryCount(void) const
{
	return m_header.m_fileEntryCount;
}


