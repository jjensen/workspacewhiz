///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDriveManager.h $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDriveManager.h $
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

	WList<DriveInfo> m_openDriveInfo;

	static VirtualDriveManager* s_instance;
};


inline VirtualDriveManager* VirtualDriveManager::GetInstance()
{
	return s_instance;
}

