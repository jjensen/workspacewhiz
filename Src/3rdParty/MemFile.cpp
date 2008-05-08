// A dupe of the MFC MemFile to get rid of the blasted AfxIsValidAddress
// calls in Debug mode.
 #include "MemFile.h"

IMPLEMENT_DYNAMIC(MemFile, CFile)

////////////////////////////////////////////////////////////////////////////
// MemFile implementation

MemFile::MemFile(UINT nGrowBytes)
{
	ASSERT(nGrowBytes <= UINT_MAX);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
	m_lpBuffer = NULL;
	m_bAutoDelete = TRUE;
}

MemFile::MemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes)
{
	ASSERT(nGrowBytes <= UINT_MAX);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = nBufferSize;
	m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
	m_lpBuffer = lpBuffer;
	m_bAutoDelete = FALSE;
}

void MemFile::Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes)
{
	ASSERT(m_lpBuffer == NULL);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = nBufferSize;
	m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
	m_lpBuffer = lpBuffer;
	m_bAutoDelete = FALSE;
}

BYTE* MemFile::Detach()
{
	BYTE* lpBuffer = m_lpBuffer;
	m_lpBuffer = NULL;
	m_nFileSize = 0;
	m_nBufferSize = 0;
	m_nPosition = 0;

	return lpBuffer;
}

MemFile::~MemFile()
{
	// Close should have already been called, but we check anyway
	if (m_lpBuffer)
		Close();
	ASSERT(m_lpBuffer == NULL);

	m_nGrowBytes = 0;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
}

BYTE* MemFile::Alloc(DWORD nBytes)
{
	return (BYTE*)malloc((UINT)nBytes);
}

BYTE* MemFile::Realloc(BYTE* lpMem, DWORD nBytes)
{
	return (BYTE*)realloc(lpMem, (UINT)nBytes);
}

#pragma intrinsic(memcpy)
BYTE* MemFile::Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource,
	UINT nBytes)
{
	ASSERT(lpMemTarget != NULL);
	ASSERT(lpMemSource != NULL);

	return (BYTE*)memcpy(lpMemTarget, lpMemSource, nBytes);
}
#pragma function(memcpy)

void MemFile::Free(BYTE* lpMem)
{
	ASSERT(lpMem != NULL);

	free(lpMem);
}

DWORD MemFile::GetPosition() const
{
	ASSERT_VALID(this);
	return m_nPosition;
}

void MemFile::GrowFile(DWORD dwNewLen)
{
	ASSERT_VALID(this);

	if (dwNewLen > m_nBufferSize)
	{
		// grow the buffer
		DWORD dwNewBufferSize = (DWORD)m_nBufferSize;

		// watch out for buffers which cannot be grown!
		ASSERT(m_nGrowBytes != 0);
		if (m_nGrowBytes == 0)
			AfxThrowMemoryException();

		// determine new buffer size
		while (dwNewBufferSize < dwNewLen)
			dwNewBufferSize += m_nGrowBytes;

		// allocate new buffer
		BYTE* lpNew;
		if (m_lpBuffer == NULL)
			lpNew = Alloc(dwNewBufferSize);
		else
			lpNew = Realloc(m_lpBuffer, dwNewBufferSize);

		if (lpNew == NULL)
			AfxThrowMemoryException();

		m_lpBuffer = lpNew;
		m_nBufferSize = dwNewBufferSize;
	}
	ASSERT_VALID(this);
}

void MemFile::SetLength(DWORD dwNewLen)
{
	ASSERT_VALID(this);

	if (dwNewLen > m_nBufferSize)
		GrowFile(dwNewLen);

	if (dwNewLen < m_nPosition)
		m_nPosition = dwNewLen;

	m_nFileSize = dwNewLen;
	ASSERT_VALID(this);
}

UINT MemFile::Read(void* lpBuf, UINT nCount)
{
	ASSERT_VALID(this);

	if (nCount == 0)
		return 0;

	ASSERT(lpBuf != NULL);

	//JJ Bug fix...
	if (m_nPosition >= m_nFileSize)
		return 0;

	UINT nRead;
	if (m_nPosition + nCount > m_nFileSize)
		nRead = (UINT)(m_nFileSize - m_nPosition);
	else
		nRead = nCount;

	memcpy(lpBuf, m_lpBuffer + m_nPosition, nRead);
//	Memcpy((BYTE*)lpBuf, (BYTE*)m_lpBuffer + m_nPosition, nRead);
	m_nPosition += nRead;

	ASSERT_VALID(this);

	return nRead;
}

void MemFile::Write(const void* lpBuf, UINT nCount)
{
	ASSERT_VALID(this);

	if (nCount == 0)
		return;

	ASSERT(lpBuf != NULL);

	if (m_nPosition + nCount > m_nBufferSize)
		GrowFile(m_nPosition + nCount);

	ASSERT(m_nPosition + nCount <= m_nBufferSize);

//	Memcpy((BYTE*)m_lpBuffer + m_nPosition, (BYTE*)lpBuf, nCount);
	memcpy(m_lpBuffer + m_nPosition, lpBuf, nCount);

	m_nPosition += nCount;

	if (m_nPosition > m_nFileSize)
		m_nFileSize = m_nPosition;

	ASSERT_VALID(this);
}

LONG MemFile::Seek(LONG lOff, UINT nFrom)
{
	ASSERT_VALID(this);
	ASSERT(nFrom == begin || nFrom == end || nFrom == current);

	LONG lNewPos = m_nPosition;

	if (nFrom == begin)
		lNewPos = lOff;
	else if (nFrom == current)
		lNewPos += lOff;
	else if (nFrom == end)
		lNewPos = m_nFileSize + lOff;
	else
		return -1;

	if (lNewPos < 0)
		AfxThrowFileException(CFileException::badSeek);

	m_nPosition = lNewPos;

	ASSERT_VALID(this);
	return m_nPosition;
}

void MemFile::Flush()
{
	ASSERT_VALID(this);
}

void MemFile::Close()
{
	ASSERT(m_nFileSize <= m_nBufferSize);

	m_nGrowBytes = 0;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
	if (m_lpBuffer && m_bAutoDelete)
		Free(m_lpBuffer);
	m_lpBuffer = NULL;
}

void MemFile::Abort()
{
	ASSERT_VALID(this);

	Close();
}

void MemFile::LockRange(DWORD /* dwPos */, DWORD /* dwCount */)
{
	ASSERT_VALID(this);
	AfxThrowNotSupportedException();
}


void MemFile::UnlockRange(DWORD /* dwPos */, DWORD /* dwCount */)
{
	ASSERT_VALID(this);
	AfxThrowNotSupportedException();
}

CFile* MemFile::Duplicate() const
{
	ASSERT_VALID(this);
	AfxThrowNotSupportedException();
	return NULL;
}

// only MemFile supports "direct buffering" interaction with CArchive
UINT MemFile::GetBufferPtr(UINT nCommand, UINT nCount,
	void** ppBufStart, void**ppBufMax)
{
	ASSERT(nCommand == bufferCheck || nCommand == bufferCommit ||
		nCommand == bufferRead || nCommand == bufferWrite);

	if (nCommand == bufferCheck)
		return 1;   // just a check for direct buffer support

	if (nCommand == bufferCommit)
	{
		// commit buffer
		ASSERT(ppBufStart == NULL);
		ASSERT(ppBufMax == NULL);
		m_nPosition += nCount;
		if (m_nPosition > m_nFileSize)
			m_nFileSize = m_nPosition;
		return 0;
	}

	ASSERT(nCommand == bufferWrite || nCommand == bufferRead);
	ASSERT(ppBufStart != NULL);
	ASSERT(ppBufMax != NULL);

	// when storing, grow file as necessary to satisfy buffer request
	if (nCommand == bufferWrite && m_nPosition + nCount > m_nBufferSize)
		GrowFile(m_nPosition + nCount);

	// store buffer max and min
	*ppBufStart = m_lpBuffer + m_nPosition;

	// end of buffer depends on whether you are reading or writing
	if (nCommand == bufferWrite)
		*ppBufMax = m_lpBuffer + min(m_nBufferSize, m_nPosition + nCount);
	else
	{
		if (nCount == (UINT)-1)
			nCount = m_nBufferSize - m_nPosition;
		*ppBufMax = m_lpBuffer + min(m_nFileSize, m_nPosition + nCount);
		m_nPosition += LPBYTE(*ppBufMax) - LPBYTE(*ppBufStart);
	}

	// return number of bytes in returned buffer space (may be <= nCount)
	return LPBYTE(*ppBufMax) - LPBYTE(*ppBufStart);
}


