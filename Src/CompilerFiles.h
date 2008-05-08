#ifndef COMPILERFILES_H
#define COMPILERFILES_H

#include "WorkspaceInfo.h"

///
class CompilerFiles
{
public:
// Attributes
	static FileList& GetFileList(void);			// Not const on purpose.

// Operations
	static bool Refresh(bool force = false);
	static void ProcessPaths(char* buffer);
	static void Add(CString projectName);
	static void RemoveAll();

protected:
	static FileList m_fileList;
	static bool m_updated;
}; //CompilerFiles


inline FileList& CompilerFiles::GetFileList(void)
{  return m_fileList;  }

#endif COMPILERFILES_H

