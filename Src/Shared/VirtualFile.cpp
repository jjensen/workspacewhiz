///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualFile.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualFile.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 4    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "VirtualFile.h"
#include "VirtualDrive.h"
#include "VirtualDriveManager.h"
#include <errno.h>

#define CHECK_ACTIVE() \
	if (!m_parentDrive) \
		CFileException::ThrowErrno(EBADF); 


VirtualFile::VirtualFile() :
	m_parentDrive(NULL),
	m_fileEntry(NULL),
	m_fileEntryIndex(VirtualDrive::INVALID_FILE_ENTRY),
	m_openFilesPos(NULL),
	m_curPosition(0)
{
	m_bCloseOnDelete = FALSE;
}



VirtualFile::~VirtualFile()
{
	Close();
}


DWORD VirtualFile::GetPosition() const
{
	if (m_parentDrive)
		return 0xFFFFFFFF;

	return m_curPosition;
}



VirtualDrive* VirtualFile::GetParent(void) const
{
	return m_parentDrive;
}



BOOL VirtualFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* /*pError*/)
{
	// Open() only works if a VirtualDriveManager is present.
	VirtualDriveManager* manager = VirtualDriveManager::GetInstance();
	if (!manager)
	{
		ASSERT(0);
		return FALSE;
	}

	// Parse the incoming filename.
	CString virtualDriveName;
	CString virtualFileName;
	manager->ParseFilename(lpszFileName, virtualDriveName, virtualFileName);

	VirtualDrive* drive = manager->Open(virtualDriveName, false);
	if (nOpenFlags & modeCreate)
	{
		if (!drive)
		{
			drive = manager->Open(virtualDriveName, true);
			if (!drive)
				return false;
		}

		return drive->FileCreate(virtualFileName, *this);
	}

	return drive->FileOpen(virtualFileName, *this);
}



CFile *VirtualFile::Duplicate() const
{
	ASSERT(0);
	return NULL;
}



LONG VirtualFile::Seek(LONG lOff, UINT nFrom)
{
	if (!m_parentDrive)
		return -1;

	switch (nFrom)
	{
		case begin:
			if (lOff < 0  ||  (DWORD)lOff > m_fileEntry->m_size)
				return -1;
			m_curPosition = lOff;
			break;

		case current:
			if ((lOff + (LONG)m_curPosition < 0)  ||
				(lOff + (LONG)m_curPosition > (LONG)m_fileEntry->m_size))
				return -1;
			m_curPosition = lOff + m_curPosition;
			break;

		case end:
			if ((lOff > 0)  ||  ((LONG)m_curPosition + lOff < 0))
				return -1;
			m_curPosition = m_fileEntry->m_size - lOff;
			break;
	}

	return m_curPosition;
}



void VirtualFile::SetLength(DWORD newSize)
{
	if (newSize <= m_fileEntry->m_size)
	{
		m_fileEntry->m_size = newSize;
		m_parentDrive->m_changed = true;
		m_parentDrive->m_header.m_needPack = true;
		return;
	}

	ASSERT(0);
}



DWORD VirtualFile::GetLength(void) const
{
	return m_fileEntry->m_size;
}


UINT VirtualFile::Read(void* lpBuf, UINT nCount)
{
	if (!m_parentDrive)
		return 0;

	// Read the block.
	UINT endPos = m_curPosition + nCount;
	if (endPos > m_fileEntry->m_size)
		nCount -= endPos - m_fileEntry->m_size;

	CFile* parentFile = m_parentDrive->GetParentFile();
	parentFile->Seek(m_curPosition + m_fileEntry->m_offset, CFile::begin);
	parentFile->Read(lpBuf, nCount);

	m_curPosition += nCount;

	return nCount;
}

void VirtualFile::Write(const void* lpBuf, UINT nCount)
{
	if (!m_parentDrive)
		return;

	// Write the block.
	CFile* parentFile = m_parentDrive->GetParentFile();
	parentFile->Seek(m_curPosition + m_fileEntry->m_offset, CFile::begin);
	parentFile->Write(lpBuf, nCount);

	// Update pointer info.
	m_curPosition += nCount;
	if (m_curPosition > m_fileEntry->m_size)
		m_fileEntry->m_size = m_curPosition;
}



void VirtualFile::Abort()
{
	ASSERT(0);
}



void VirtualFile::Flush()
{
	ASSERT(0);
}



void VirtualFile::Close()
{
	if (m_parentDrive)
	{
		m_parentDrive->FileClose(*this);
		m_parentDrive = NULL;
	}

	m_fileEntry = NULL;  
	m_fileEntryIndex = VirtualDrive::INVALID_FILE_ENTRY;
	m_openFilesPos = NULL;
	m_curPosition = 0xFFFFFFFF;
}

