///////////////////////////////////////////////////////////////////////////////
// $Workfile: VirtualDriveManager.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/VirtualDriveManager.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "VirtualDriveManager.h"
#include "VirtualDrive.h"
#include "VirtualFile.h"

VirtualDriveManager* VirtualDriveManager::s_instance;

///////////////////////////////////////////////////////////////////////////////
/**
	The constructor.
**/
VirtualDriveManager::VirtualDriveManager()
{
	ASSERT(!s_instance);
	s_instance = this;
} // VirtualDriveManager()


///////////////////////////////////////////////////////////////////////////////
/**
**/
VirtualDriveManager::~VirtualDriveManager()
{
	CloseAll();
} // ~VirtualDriveManager()


///////////////////////////////////////////////////////////////////////////////
/**
**/
VirtualDrive* VirtualDriveManager::Open(const CString& filename, bool create)
{
	// Save the filename.
	DriveInfo driveInfo;
	driveInfo.m_filename = filename;
	driveInfo.m_filename.MakeLower();

	// Search the list.  See if filename is already open.
	POSITION pos = m_openDriveInfo.GetHeadPosition();
	while (pos)
	{
		DriveInfo& testInfo = m_openDriveInfo.GetNext(pos);
		if (testInfo.m_filename == driveInfo.m_filename)
		{
			// Already open.  Just return the drive.
			return testInfo.m_drive;
		}
	}

	// It isn't open.  Make a new entry.
	driveInfo.m_drive = WNEW VirtualDrive;
	if (create)
	{
		if (!driveInfo.m_drive->Create(driveInfo.m_filename))
		{
			return NULL;
		}
	}
	else
	{
		if (!driveInfo.m_drive->Open(driveInfo.m_filename))
		{
			return NULL;
		}
	}

	m_openDriveInfo.AddTail(driveInfo);

	// Opened successfully!
	return driveInfo.m_drive;
} // Open()


///////////////////////////////////////////////////////////////////////////////
/**
	Close all open virtual drives.
**/
bool VirtualDriveManager::CloseAll()
{
	POSITION pos = m_openDriveInfo.GetHeadPosition();
	while (pos)
	{
		DriveInfo& driveInfo = m_openDriveInfo.GetNext(pos);
		delete driveInfo.m_drive;
	}

	m_openDriveInfo.RemoveAll();

	return true;
} // CloseAll()


void VirtualDriveManager::ParseFilename(const CString& filename,
			CString& virtualDriveName, CString& virtualFileName)
{
	int pipePos = filename.Find('|');
	if (pipePos == -1)
	{
		virtualDriveName.Empty();
		virtualFileName.Empty();

		return;
	}

	virtualDriveName = filename.Left(pipePos);
	virtualFileName = filename.Mid(pipePos + 1);
} // ParseFilename()
