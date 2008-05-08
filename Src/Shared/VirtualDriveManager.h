///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDriveManager.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDriveManager.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 3    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

class VirtualDrive;

/**
**/
class VirtualDriveManager
{
public:
	static VirtualDriveManager* GetInstance();

	VirtualDriveManager();
	virtual ~VirtualDriveManager();

	virtual void ParseFilename(const CString& filename, CString& virtualDriveName, CString& virtualFileName);

	VirtualDrive* Open(const CString& filename, bool create);
	bool CloseAll();

private:
	class DriveInfo
	{
	public:
		CString m_filename;
		VirtualDrive* m_drive;
	};

	CList<DriveInfo, DriveInfo&> m_openDriveInfo;

	static VirtualDriveManager* s_instance;
};


inline VirtualDriveManager* VirtualDriveManager::GetInstance()
{
	return s_instance;
}

