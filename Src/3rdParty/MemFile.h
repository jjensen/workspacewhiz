#pragma once
#ifndef MEMFILE_H
#define MEMFILE_H

#include <afx.h>

////////////////////////////////////////////////////////////////////////////
// Memory based file implementation

class MemFile : public CFile
{
	DECLARE_DYNAMIC(MemFile)

public:
// Constructors
	MemFile(UINT nGrowBytes = 1024);
	MemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);

// Operations
	void Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);
	BYTE* Detach();
	BYTE* Peek();

// Advanced Overridables
protected:
#ifdef WWHIZ_VC6
	virtual BYTE* Alloc(DWORD nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, DWORD nBytes);
	virtual BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource, UINT nBytes);
	virtual void GrowFile(DWORD dwNewLen);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	virtual BYTE* Alloc(SIZE_T nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, SIZE_T nBytes);
	virtual BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource, SIZE_T nBytes);
	virtual void GrowFile(SIZE_T dwNewLen);
#endif WWHIZ_VSNET
	virtual void Free(BYTE* lpMem);

// Implementation
protected:
#ifdef WWHIZ_VC6
	UINT m_nGrowBytes;
	DWORD m_nPosition;
	DWORD m_nBufferSize;
	DWORD m_nFileSize;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
	SIZE_T m_nGrowBytes;
	SIZE_T m_nPosition;
	SIZE_T m_nBufferSize;
	SIZE_T m_nFileSize;
#endif WWHIZ_VSNET
	BYTE* m_lpBuffer;
	BOOL m_bAutoDelete;

public:
	virtual ~MemFile();
	BOOL GetStatus(CFileStatus& rStatus) const;
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
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();
};

#endif //MEMFILE_H




