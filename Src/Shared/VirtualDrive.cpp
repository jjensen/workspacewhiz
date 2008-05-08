///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDrive.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDrive.cpp $
// $Date: 2003/01/05 $ $Revision: #10 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "VirtualDrive.h"
#include "VirtualFile.h"

const LPCTSTR DRIVE_ID = "VRTDRIVE";		//!< The header ID of a virtual drive.
const WORD DRIVE_VERSION = 0x0203;			//!< The virtual drive version.
const LPCTSTR DIR_ID = "VDRIVDIR";			//!< The directory ID of a virtual drive.

///////////////////////////////////////////////////////////////////////////////
/**
	\internal Used with qsort().
**/
static int __cdecl Compare(VirtualDrive::FileEntry* item1, VirtualDrive::FileEntry* item2)
{
	return (stricmp(item1->GetFilename(), item2->GetFilename()));
}


///////////////////////////////////////////////////////////////////////////////
/**
	\internal
**/
static inline void ReadString(BYTE*& ptr, CString& str)
{
	BYTE len = *(BYTE*)ptr;			ptr += sizeof(BYTE);
	LPTSTR buf = str.GetBufferSetLength(len + 1);
	if (len > 0)
	{
		memcpy(buf, ptr, len);
		ptr += len;
	}
	str.ReleaseBuffer(len);
}


/**
	\internal
**/
static inline void ReadBool(BYTE*& ptr, bool& b)
{
	b = *(bool*)ptr;		ptr += sizeof(bool);
}


/**
	\internal
**/
static inline void ReadDWORD(BYTE*& ptr, DWORD& dw)
{
	dw = *(DWORD*)ptr;		ptr += sizeof(DWORD);
}


/**
	\internal
**/
static inline void ReadTimeStamp(BYTE*& ptr, CTime& timeStamp)
{
	time_t time;
	time = *(time_t*)ptr;			ptr += sizeof(time_t);
	timeStamp = time;
}


/**
	\internal
**/
static inline void WriteString(CFile& file, const CString& str)
{
	BYTE len = (BYTE)str.GetLength();
	file.Write(&len, sizeof(BYTE));
	if (len > 0)
		file.Write((LPCTSTR)str, len);
}


/**
	\internal
**/
static inline void WriteDWORD(CFile& file, DWORD dw)
{
	file.Write(&dw, sizeof(DWORD));
}


/**
	\internal
**/
static inline void WriteBool(CFile& file, bool b)
{
	file.Write(&b, sizeof(bool));
}


/**
	\internal
**/
static inline void WriteTimeStamp(CFile& file, const CTime& timeStamp)
{
	time_t time = timeStamp.GetTime();
	file.Write(&time, sizeof(time_t));
}


///////////////////////////////////////////////////////////////////////////////
/**
	\internal
**/
void VirtualDrive::FileHeader::Reset()
{
	m_version = DRIVE_VERSION;
	m_fileEntryCount = 0;
	m_dirOffset = sizeof(*this);
	m_dirSize = 0;
	strncpy(m_id, DRIVE_ID, 8);
	m_needPack = false;
	memset(m_userData, 0, sizeof(m_userData));
}


///////////////////////////////////////////////////////////////////////////////
/**
	The constructor.
**/
VirtualDrive::VirtualDrive() :
	m_ownParentFile(false),
	m_parentFile(NULL),
	m_changed(false),
	m_curWriteFile(NULL),
	m_readOnly(false)
{
	m_header.Reset();
} // VirtualDrive()


///////////////////////////////////////////////////////////////////////////////
/**
	The destructor automatically closes all open virtual files and the virtual
	drive.
**/
VirtualDrive::~VirtualDrive()
{
	Close();
} // ~VirtualDrive()


///////////////////////////////////////////////////////////////////////////////
/**
	Creates a new virtual drive on the disk.  The new virtual drive has no
	file entries.

	@param filename The full path of the virtual drive.
	@return Returns true if the virtual drive was created successfully, false
		otherwise.
**/
bool VirtualDrive::Create(const CString& filename)
{
	// Save the filename.
	m_filename = filename;

	// Create the virtual drive disk file.
	m_ownParentFile = true;
	m_parentFile = WNEW CFile();
	if (!m_parentFile->Open(m_filename, CFile::modeCreate | CFile::modeReadWrite))
	{
		// Couldn't create!
		Close();

		return false;
	}

	m_readOnly = false;

	// Write the header.
	m_header.Reset();
	m_parentFile->Write(&m_header, sizeof(m_header));

	// Remove all directory entries.
	m_dir.RemoveAll();
	m_changed = true;
	m_curWriteFile = NULL;
	m_filenameMap.RemoveAll();

	// Created successfully!
	return true;
} // Create()


///////////////////////////////////////////////////////////////////////////////
/**
	Opens an existing virtual drive from the disk.

	@param filename The full path of the virtual drive.
	@return Returns true if the virtual drive was opened successfully, false
		otherwise.
**/
bool VirtualDrive::Open(const CString& filename)
{
	// Save the filename.
	m_filename = filename;

	// Open the file.
	m_ownParentFile = true;
	m_parentFile = WNEW CFile();
	if (!m_parentFile->Open(m_filename, CFile::modeReadWrite))
	{
		if (!m_parentFile->Open(m_filename, CFile::modeRead))
		{
			// Couldn't open the file!
			Close();

			return false;
		}
		
		m_readOnly = true;
	}

	// Read the header.
	m_parentFile->Read(&m_header, sizeof(FileHeader));

	// Is it a header we recognize?
	if (strncmp(m_header.m_id, DRIVE_ID, 8) != 0)
	{
		Close();
		return false;
	}

	// Is it a version we recognize?
	if (m_header.m_version != DRIVE_VERSION)
	{
		Close();
		return false;
	}

	// Are there any file entries?
	m_dir.RemoveAll();
	if (m_header.m_fileEntryCount > 0)
	{
		// Yes. Seek to the directory.
		m_parentFile->Seek(m_header.m_dirOffset, CFile::begin);

		// Make sure the directory is valid.
		char dirID[8];
		m_parentFile->Read(&dirID, 8);
		if (strncmp(dirID, DIR_ID, 8) != 0)
		{
			Close();
			return false;
		}

		// Allocate memory for the directory.
		WArray<BYTE> dirBuffer;
		dirBuffer.SetCount(m_header.m_dirSize);
		m_parentFile->Read(dirBuffer.GetData(), m_header.m_dirSize);

		// Now, process the directory.
		m_dir.SetCount(m_header.m_fileEntryCount, 100);

		// Read in the directory..
		BYTE* ptr = dirBuffer.GetData();
		for (UINT i = 0; i < m_header.m_fileEntryCount; ++i)
		{
			FileEntry& entry = m_dir[i];

			ReadString(ptr, entry.m_filename);
			ReadTimeStamp(ptr, entry.m_fileTime);
			ReadDWORD(ptr, entry.m_offset);
			ReadDWORD(ptr, entry.m_size);
			ReadDWORD(ptr, entry.m_crc);
			memcpy(entry.m_userData, ptr, sizeof(entry.m_userData));
			ptr += sizeof(entry.m_userData);
			entry.m_parentDrive = this;

			m_filenameMap[entry.m_filename] = i;
		}
	}

	// Finish setting it up.
	m_changed = false;
	m_curWriteFile = NULL;

	// Opened successfully!
	return true;
} // Open()


///////////////////////////////////////////////////////////////////////////////
/**
	Closes the virtual drive, flushing any unsaved changed to disk.  All open
	virtual files are closed.

	@return Returns true if successful.
**/
bool VirtualDrive::Close()
{
	// Flush unsaved changed.
	Flush();

	// Close all open virtual files.
	FileCloseAll();

	// Remove all entries from the directory.
	m_dir.RemoveAll();
	m_filenameMap.RemoveAll();

	// Clear out the drive's filename.
	m_filename.Empty();

	// Destroy the CFile.
	if (m_ownParentFile)
	{
		delete m_parentFile;
	}
	m_parentFile = NULL;
	m_ownParentFile = false;

	return true;
} // Close()


///////////////////////////////////////////////////////////////////////////////
/**
	Sorts the directory entries by filename and writes it all to disk.
**/
bool VirtualDrive::Flush()
{
	// If there is no parent file, then the drive has never been opened.  Exit.
	if (!m_parentFile)
		return false;
	
	// If there have been no changes made, exit successfully.
	if (!m_changed)
		return true;

	// If there is more than one file entry, sort it.  Using qsort() is safe
	// because CString is only a pointer.
	if (m_header.m_fileEntryCount > 1)
	{
		qsort(m_dir.GetData(), m_header.m_fileEntryCount, sizeof(FileEntry),
			  (int (__cdecl *)(const void *, const void *)) Compare );

		m_filenameMap.RemoveAll();
		for (UINT i = 0; i < m_header.m_fileEntryCount; ++i)
		{
			FileEntry& entry = m_dir[i];

			m_filenameMap[entry.m_filename] = i;
		}
	}

	// Write the directory header id.
	m_parentFile->Seek(m_header.m_dirOffset, CFile::begin);
	m_parentFile->Write(DIR_ID, 8);
	DWORD curPos = m_parentFile->GetPosition();

	// Write all the directory entries.
	for (UINT i = 0; i < m_header.m_fileEntryCount; ++i)
	{
		FileEntry& entry = m_dir[i];

		WriteString(*m_parentFile, entry.m_filename);
		WriteTimeStamp(*m_parentFile, entry.m_fileTime);
		WriteDWORD(*m_parentFile, entry.m_offset);
		WriteDWORD(*m_parentFile, entry.m_size);
		WriteDWORD(*m_parentFile, entry.m_crc);
		m_parentFile->Write(entry.m_userData, sizeof(entry.m_userData));
	}

	// Write the file entry count at the end of the file for debug purposes.
	m_parentFile->Write(&m_header.m_fileEntryCount, sizeof(m_header.m_fileEntryCount));

	m_parentFile->SetLength(m_parentFile->GetPosition());
	m_header.m_dirSize = m_parentFile->GetPosition() - curPos;

	// Write the drive header.
	m_parentFile->Seek(0, CFile::begin);
	m_parentFile->Write(&m_header, sizeof(FileHeader));

	// Reset to no changes.
	m_changed = false;

	return true;
} // Flush()


///////////////////////////////////////////////////////////////////////////////
/**
	Creates a new virtual file within the virtual drive for writing. Only one
	virtual file may be written to at any one time, because the new file is
	appended to the end of the virtual drive.

	If the new virtual file already exists within the directory entries, the
	old file of the same name is invalidated. The space the old file took
	becomes unused and not retrievable.  To "retrieve" the space, the virtual
	drive must be packed (see Pack()).

	@param filename The filename of the new virtual file to create.
	@param virtualFile A reference to the VirtualFile object to be filled in
		with the newly created virtual file's information.
	@param fileTime A CTime object describing the virtual file's creation date
		and time.  The current date and time is used if fileTime is NULL,
	@return Returns true if the virtual file was created successfully, false
		otherwise.
**/
bool VirtualDrive::FileCreate(const CString& filename, VirtualFile& virtualFile,
							  const CTime* fileTime)
{	
	// If the drive is read-only, then exit.
	if (m_readOnly)
		return false;

	// If there is a file currently being written to, then it must be closed
	// first.  Abort the file creation.
	if (m_curWriteFile)
		return false;

	// Lowercase the filename.
	CString newFilename = filename;
	newFilename.MakeLower();
	
	// Retrieve the index of the file entry called filename.
	UINT index = GetFileEntryIndex(newFilename);

	// Does the entry exist?
	if (index == INVALID_FILE_ENTRY)
	{
		// No. It needs to be added.  Increase the file entry count by 1.
		m_header.m_fileEntryCount++;
		m_dir.SetCount(m_header.m_fileEntryCount);
		index = m_header.m_fileEntryCount - 1;

		// Add the filename to the filename map.
		m_filenameMap[newFilename] = index;

		// Reassign all the open file file entry pointers, since the array
		// of directory entries may have changed locations in memory.
		POSITION pos = m_openFiles.GetHeadPosition();
		while (pos)
		{
			VirtualFile* openFile = m_openFiles.GetNext(pos);
			openFile->m_fileEntry = &m_dir[openFile->m_fileEntryIndex];
		}
	}
	else
	{
		m_header.m_needPack = true;
	}

	// Fill in the new (or existing file entry).
	FileEntry& fileEntry = m_dir[index];
	fileEntry.m_filename = newFilename;

	// Add the proper file time.
	if (!fileTime)
	{
		FILETIME filetime;
		GetSystemTimeAsFileTime(&filetime);

		fileEntry.m_fileTime = CTime(filetime);
	}
	else
	{
		fileEntry.m_fileTime = *fileTime;
	}

	fileEntry.m_offset = m_header.m_dirOffset;
	fileEntry.m_size = 0;
	fileEntry.m_crc = 0;
	memset(fileEntry.m_userData, 0, sizeof(fileEntry.m_userData));
	fileEntry.m_parentDrive = this;

	// Assign the appropriate information to the virtual file object.
	virtualFile.m_parentDrive = this;
	virtualFile.m_fileEntry = &fileEntry;
	virtualFile.m_fileEntryIndex = index;
	virtualFile.m_curPosition = 0;

	// Assign this file to be our current write file.
	m_curWriteFile = &virtualFile;

	// Add this virtual file to the open files list.
	virtualFile.m_openFilesPos = m_openFiles.AddTail(&virtualFile);

	// A change to the directory has occurred.
	m_changed = true;

	return true;
} // FileCreate()


///////////////////////////////////////////////////////////////////////////////
/**
	Opens an existing virtual file within the virtual drive.

	@param filename The filename of the existing virtual file to open.
	@param virtualFile A reference to the VirtualFile object to be filled in
		with the newly opened virtual file's information.
	@return Returns true if the virtual file was opened successfully, false
		otherwise.
**/
bool VirtualDrive::FileOpen(const CString& filename, VirtualFile& virtualFile)
{
	// Retrieve the index of the file entry called filename.
	UINT index = GetFileEntryIndex(filename);

	// If the entry doesn't exist, then exit.
	if (index == INVALID_FILE_ENTRY)
		return false;

	// Grab the entry.
	FileEntry& fileEntry = m_dir[index];

	// Assign the internal virtual file information.
	virtualFile.m_parentDrive = this;
	virtualFile.m_fileEntry = &fileEntry;
	virtualFile.m_fileEntryIndex = index;
	virtualFile.m_curPosition = 0;

	// Add this virtual file to the open files list.
	virtualFile.m_openFilesPos = m_openFiles.AddTail(&virtualFile);

	return true;
} // FileOpen()


///////////////////////////////////////////////////////////////////////////////
/**
	Closes an open virtual file.  This function should never really need to be
	called directory, as VirtualFile::Close() can be used instead.

	@param virtualFile The VirtualFile object to be closed.
	@return Returns true if the file was closed successfully.
**/
bool VirtualDrive::FileClose(VirtualFile& virtualFile)
{
	// Is the virtual file being closed the current virtual file being written
	// to?
	if (&virtualFile == m_curWriteFile)
	{
		// Yes.  Reassign the file header's directory offset.
		m_header.m_dirOffset = virtualFile.m_fileEntry->m_offset + virtualFile.m_curPosition;

		// Turn off the current write file.
		m_curWriteFile = NULL;
	}

	// Remove the file from the open files list.
	m_openFiles.RemoveAt(virtualFile.m_openFilesPos);

	return true;
} // FileClose()


///////////////////////////////////////////////////////////////////////////////
/**
	Closes all open virtual files for the current virtual drive.
**/
void VirtualDrive::FileCloseAll()
{
	// Wander the open files list, closing each file.
	POSITION pos = m_openFiles.GetHeadPosition();
	while (pos)
	{
		VirtualFile* openFile = m_openFiles.GetNext(pos);
		openFile->Close();
	}

	// Remove all files from the open files list.
	m_openFiles.RemoveAll();
}


///////////////////////////////////////////////////////////////////////////////
/**
	Erases the virtual file called [filename].  The space the virtual file
	occupied is now unused and irretrievable, without using the Pack()
	function.

	@param filename The filename of the virtual file to be erased.
	@return Returns true if the file was erased or false if an error occured
		(such as the file specified by filename not existing or being open
		already).
**/
bool VirtualDrive::FileErase(const CString& filename)
{
	if (m_readOnly)
		return false;

	// Retrieve the index of the file entry called filename.
	UINT index = GetFileEntryIndex(filename);

	// If the entry doesn't exist, then exit.
	if (index == INVALID_FILE_ENTRY)
		return false;

	// Lowercase the filename.
	CString newFilename = filename;
	newFilename.MakeLower();
	
	// Wander the list of open files, checking if the file is not already open.
	POSITION pos = m_openFiles.GetHeadPosition();
	while (pos)
	{
		VirtualFile* openFile = m_openFiles.GetNext(pos);
		if (openFile->m_fileEntry->GetFilename() == newFilename)
			return false;
	}

	// Remove the filename from the directory.
	m_filenameMap.RemoveKey(newFilename);
	m_dir.RemoveAt(index);

	// Decrease the header file entry count.
	m_header.m_fileEntryCount--;

	// Reinsert all following entries into the map.
	for (int i = index; i < m_header.m_fileEntryCount; ++i)
	{
		m_filenameMap[m_dir[i].GetFilename()] = i;
	}

	// A directory change has occurred.
	m_changed = true;
	m_header.m_needPack = true;

	return true;
} // FileErase()


///////////////////////////////////////////////////////////////////////////////
/**
	Renames a virtual file from [oldFilename] to [newFilename].

	@param oldFilename The filename of the virtual file to rename.
	@param newFilename The filename to rename [oldFilename] to.
	@return Returns true if successful, false if an error occurred.
**/
bool VirtualDrive::FileRename(const CString& oldFilename, const CString& newFilename)
{
	if (m_readOnly)
		return false;

	// Retrieve the index of the file entry called oldFilename.
	UINT index = GetFileEntryIndex(oldFilename);

	// If it exists, then an entry called oldFilename is already there.
	if (index == INVALID_FILE_ENTRY)
		return false;

	// Get the file entry.
	FileEntry& fileEntry = m_dir[index];

	// Lowercase the filename.
	CString newOldFilename = oldFilename;
	newOldFilename.MakeLower();
	CString newNewFilename = newFilename;
	newNewFilename.MakeLower();
	
	// Assign the new filename.
	m_filenameMap.RemoveKey(newOldFilename);
	fileEntry.m_filename = newNewFilename;
	m_filenameMap[newNewFilename] = index;

	// The directory changed.
	m_changed = true;

	return true;
} // FileRename()


///////////////////////////////////////////////////////////////////////////////
/**
	Copies a file from [srcFile] to a virtual file called [destFilename].

	@param srcFile An open CFile object to copy the data from.
	@param destFilename The filename of the virtual file to copy the data from
		[srcFile] to.
	@param fileTime A CTime object describing the virtual file's creation date
		and time.  The current date and time is used if fileTime is NULL,
**/	
bool VirtualDrive::FileCopy(CFile& srcFile, const CString& destFilename, const CTime* fileTime)
{
	if (m_readOnly)
		return false;

	// Operate in 16k buffers.
	const DWORD BUFFER_SIZE = 16 * 1024;

	// Get the source file's size.
	DWORD fileSize = srcFile.GetLength();

	// See if the destination file exists.
	VirtualFile destFile;
	bool opened = false;
	FileEntry* fileEntry = GetFileEntry(destFilename);
	if (fileEntry)
	{
		// It does.  Will the file we're copying fit in the existing dead
		// space?
		if (fileSize <= fileEntry->m_size)
		{
			// Yes.  So open the file.
			if (FileOpen(destFilename, destFile))
			{
				// Set the time stamp.
				if (fileTime)
					destFile.m_fileEntry->SetTimeStamp(*fileTime);

				// If we're filling less than the size available, then set
				// the pack flag.
				if (fileSize < fileEntry->m_size)
				{
					destFile.SetLength(fileSize);
					m_header.m_needPack = true;
				}

				opened = true;
			}
		}
	}

	// Was the file opened yet?
	if (!opened)
	{
		// Create the destination virtual file.
		if (!FileCreate(destFilename, destFile, fileTime))
			return false;
	}

	// Allocate the buffer space.
	BYTE* buffer = WNEW BYTE[BUFFER_SIZE];

	// Go to the beginning of the source file.
	srcFile.SeekToBegin();

	// Keep copying until there is no more file left to copy.
	while (fileSize > 0)
	{
		// Copy the minimum of BUFFER_SIZE or the fileSize.
		DWORD readSize = min(BUFFER_SIZE, fileSize);
		srcFile.Read(buffer, readSize);
		destFile.Write(buffer, readSize);
		fileSize -= readSize;
	}

	// Close the destination virtual file.
	destFile.Close();

	// Destroy the buffer.
	delete [] buffer;

	return true;
} // FileCopy()


///////////////////////////////////////////////////////////////////////////////
/**
	Packs a virtual drive, removing all unused space.
**/	
bool VirtualDrive::Pack()
{	
	if (m_readOnly)
		return false;

	// Do we need to pack it?
	if (!m_header.m_needPack)
		return true;

	// Flush.
	Flush();

	// Generate a unique temporary name.
	char* asciiTempName = _tempnam(NULL, "VDRIVEPACK_");
	CString newFile = asciiTempName;
	free(asciiTempName);

	// Create a new virtual drive.
	VirtualDrive newDrive;
	newDrive.Create(newFile);
	memcpy(newDrive.m_header.m_userData, m_header.m_userData, sizeof(m_header.m_userData));

	// Go through the entire directory.
	for (UINT i = 0; i < m_header.m_fileEntryCount; i++)
	{
		// Grab the file entry.
		FileEntry& fileEntry = m_dir[i];
		
		// Open the source file.
		VirtualFile srcFile;
		if (!FileOpen(fileEntry.GetFilename(), srcFile))
		{
			newDrive.Close();
			_unlink(newFile);
			return false;
		}

		// Copy the source file to the new virtual drive's destination virtual
		// file.
		if (!newDrive.FileCopy(srcFile, fileEntry.GetFilename(), &fileEntry.GetTimeStamp()))
		{
			newDrive.Close();
			_unlink(newFile);
			return false;
		}

		// Copy the user data.
		FileEntry* newFileEntry = newDrive.GetFileEntry(fileEntry.GetFilename());
		memcpy(newFileEntry->m_userData, srcFile.m_fileEntry->m_userData, 
				sizeof(newFileEntry->m_userData));

		// Close the source file.
		srcFile.Close();
	}

	// Close the new virtual drive.
	newDrive.Close();

	// Close the current virtual drive.
	CString oldDriveName = m_filename;
	Close();

	// Copy the packed drive.
	::CopyFile(newFile, oldDriveName, FALSE);

	_unlink(newFile);

	// Open it with the new 
	if (!Open(oldDriveName))
		return false;

	return true;
} // Pack()


///////////////////////////////////////////////////////////////////////////////
/**
	Retrieves the directory file entry at [index].

	@param index The index of the file entry, range 0 to GetFileEntryCount() - 1.
	@return Returns the FileEntry object describing the requested file entry or
		NULL if the index is out of range.
**/
VirtualDrive::FileEntry* VirtualDrive::GetFileEntry(UINT index)
{
	// See if the requested index is in range.
	if (index >= (int)m_header.m_fileEntryCount)
		return NULL;

	// Return the appropriate file entry.
	return &m_dir[index];
}


///////////////////////////////////////////////////////////////////////////////
/**
	Retrieves the FileEntry object of the directory file entry called [filename].

	@param filename The non-case sensitive filename of the file entry to find.
	@return Returns the FileEntry object describing the requested file entry or
		NULL if it isn't found.
**/
VirtualDrive::FileEntry* VirtualDrive::GetFileEntry(const CString& filename)
{
	// Lookup the filename.
	UINT index = GetFileEntryIndex(filename);
	if (index == INVALID_FILE_ENTRY)
		return NULL;

	return &m_dir[index];
}


///////////////////////////////////////////////////////////////////////////////
/**
	Retrieves the index of the directory file entry called [filename].

	@param filename The non-case sensitive filename of the file entry to find.
	@return Returns the index of the found file entry or INVALID_FILE_ENTRY if
		it wasn't found.
**/
UINT VirtualDrive::GetFileEntryIndex(const CString& filename)
{
	// If there are no file entries, it is pointless to perform the rest of the
	// search.
	if (m_header.m_fileEntryCount == 0)
		return INVALID_FILE_ENTRY;

	// Make a copy of the filename string, only make it lowercase (to act as
	// non-case sensitive.
	CString newFilename = filename;
	newFilename.MakeLower();

	// Look up the filename in the map.
	UINT index;
	if (!m_filenameMap.Lookup(newFilename, index))
		return INVALID_FILE_ENTRY;

	// Return the found index.
	return index;
} // GetFileEntryIndex


