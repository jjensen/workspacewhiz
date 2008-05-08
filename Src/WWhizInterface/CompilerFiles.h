///////////////////////////////////////////////////////////////////////////////
// $Workfile: CompilerFiles.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/CompilerFiles.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

class FileList;

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
}; //CompilerFiles

