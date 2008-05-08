///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDriveCompiler.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDriveCompiler.h $
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

class VirtualDriveCompiler
{
public:
	VirtualDriveCompiler();
	~VirtualDriveCompiler();

	void SetCompression(bool compressFiles)							{  m_compressFiles = compressFiles;  }
	void SetRecurseDirectories(bool recurseDirectories)				{  m_recurseDirectories = recurseDirectories;  }
	void SetRecreate(bool recreate)									{  m_recreate = recreate;  }
	void SetPackOnChange(bool packOnChange)							{  m_packOnChange = packOnChange;  }
	void SetPackAlways(bool packAlways)								{  m_packAlways = packAlways;  }
	void SetDeleteNonExistentFiles(bool deleteNonExistantFiles)		{  m_deleteNonExistentFiles = deleteNonExistantFiles;  }
	
	void AddIgnoreExtension(CString extensionToIgnore);
	void AddForceExtension(CString extensionToForce);
	
	void ProcessFiles(CString srcDir, CString origSrcDir, CString origDestDir);

	DWORD GetFileCRC(CFile& file, DWORD skipBytes);

protected:
    bool m_compressFiles:1;
    bool m_recurseDirectories:1;
    bool m_recreate:1;
    bool m_packOnChange:1;
    bool m_packAlways:1;
    bool m_deleteNonExistentFiles:1;

    CList<CString, CString&> m_ignoreExtensions;
    CList<CString, CString&> m_forceExtensions;
};


inline void VirtualDriveCompiler::AddIgnoreExtension(CString extensionToIgnore)
{
	extensionToIgnore.MakeLower();
	m_ignoreExtensions.AddTail(extensionToIgnore);
}


inline void VirtualDriveCompiler::AddForceExtension(CString extensionToForce)
{
	extensionToForce.MakeLower();
	m_forceExtensions.AddTail(extensionToForce);
}

