///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualFile.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualFile.h $
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

#include "VirtualDrive.h"

/**
	VirtualFile is a CFile-derived class for providing access to a virtual file
	within a virtual drive.
**/
class VirtualFile : public CFile
{
public:
// Constructors
	VirtualFile();
	virtual ~VirtualFile();

// Attributes
	VirtualDrive* GetParent(void) const;

#ifdef WWHIZ_VC6
	virtual DWORD GetPosition() const;
	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);
	virtual DWORD GetLength() const;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	virtual ULONGLONG GetPosition() const;
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual ULONGLONG GetLength() const;
#endif WWHIZ_VSNET

// Operations
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

// Overridables
	virtual CFile* Duplicate() const;

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

private:
	VirtualDrive* m_parentDrive;
	VirtualDrive::FileEntry* m_fileEntry;
	UINT m_fileEntryIndex;
	POSITION m_openFilesPos;
	DWORD m_curPosition;

    friend class VirtualDrive;
};


