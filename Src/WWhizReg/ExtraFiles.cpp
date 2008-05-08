///////////////////////////////////////////////////////////////////////////////
// $Workfile: ExtraFiles.cpp $
// $Archive: /WWhizReg/ExtraFiles.cpp $
// $Date:: 3/28/01 7:36p   $ $Revision:: 13   $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////

#include "pchWWhizReg.h"

WWhizReg::ExtraFilesList s_extraFilesList;

bool WriteExtraFiles()
{
	CMemFile memFile;
    char id[5] = "WWEF";
    memFile.Write(&id, 4);
	WORD version = 1;
	memFile.Write(&version, sizeof(WORD));

    DWORD count = s_extraFilesList.GetCount();
    memFile.Write(&count, sizeof(DWORD));

    POSITION pos = s_extraFilesList.GetHeadPosition();
    while (pos)
    {
		WWhizReg::ExtraFilesInfo& info = s_extraFilesList.GetNext(pos);
		BYTE flags = (info.m_active ? 1 : 0) | (info.m_noRefresh ? 2 : 0);
		memFile.Write(&flags, sizeof(BYTE));
		CString& str = info.m_name;
		WORD len = str.GetLength();
		memFile.Write(&len, sizeof(WORD));
        memFile.Write((LPCTSTR)str, len);
    }

	DWORD memSize = memFile.GetLength();
	BYTE* mem = memFile.Detach();
	BYTE* origMem = mem;

	// Encrypt the memory.
	mem += 6;		// Skip the ID.
	BYTE whichBit = 1 << 7;
	while (mem < origMem + memSize)
	{
		*mem++ ^= whichBit;
		whichBit >>= 1;
		if (whichBit == 0)
			whichBit = 128;
	}

	// Create the ExtraFiles.WW file.
	CFile file;
    if (file.Open(g_wwhizInterface->GetExtraFilename(),
        	CFile::modeCreate | CFile::modeWrite))
    {
    	file.Write(origMem, memSize);
    	file.Close();
    }

	free(origMem);

	return true;
}


static inline void ReadString(BYTE*& ptr, CString& str)
{
	WORD len = *(WORD*)ptr;			ptr += sizeof(WORD);
	LPTSTR buf = str.GetBufferSetLength(len);
	if (len > 0)
	{
		memcpy(buf, ptr, len);
		ptr += len;
	}
	str.ReleaseBuffer(len);
}


bool ReadExtraFiles(bool addToProjectList)
{
	// Remove files from the s_extraFilesList.
	s_extraFilesList.RemoveAll();

	// Is there an add-on file?
	const CString& wwFilename = g_wwhizInterface->GetExtraFilename();
	if (_access(wwFilename, 00) == -1)
		return true;

	// Open the add-on file.
    CFile file;
    if (!file.Open(wwFilename, CFile::modeRead))
    	return false;

	// Read the ID.
    char id[5] = "WWEF";
    char fileID[4];
    file.Read(&fileID, 4);
    if (strncmp(fileID, id, 4) != 0)
    {
        // Invalid ExtraFiles.WW file.
        rename(wwFilename, wwFilename + ".old");
        _unlink(wwFilename);

        return false;
    }

	// Read the version.
	WORD version;
	file.Read(&version, sizeof(WORD));
	if (version != 1)
	{
        // Invalid ExtraFiles.WW file.
        rename(wwFilename, wwFilename + ".old");
        _unlink(wwFilename);

        return false;
	}

	// Read the whole file into memory.
    DWORD memSize = file.GetLength() - 6;
    BYTE* mem = WNEW BYTE[memSize];
    BYTE* origMem = mem;
    file.Read(mem, memSize);

    // Decrypt the memory.
	BYTE whichBit = 1 << 7;
    while (mem < origMem + memSize)
    {
        *mem++ ^= whichBit;
        whichBit >>= 1;
        if (whichBit == 0)
            whichBit = 128;
    }

    // Read the item count.
    mem = origMem;
    DWORD count = *(DWORD*)mem;     mem += sizeof(DWORD);

    // Read each string.
	while (count--)
    {
		WWhizReg::ExtraFilesInfo info;
		BYTE flags = *mem++;
		info.m_active = (flags & 1) != 0;
		info.m_noRefresh = (flags & 2) != 0;
        ReadString(mem, info.m_name);

    	s_extraFilesList.AddTail(info);
	}

    delete [] origMem;

	if (addToProjectList  &&  s_extraFilesList.GetCount() > 0)
	{
		CString projFilename = wwFilename + ".vcproj";
		CStdioFile outFile;
		if (outFile.Open(projFilename, CFile::modeCreate | CFile::modeWrite))
		{
			outFile.WriteString("<Files>\n");
			POSITION pos = s_extraFilesList.GetHeadPosition();
			while (pos)
			{
				WWhizReg::ExtraFilesInfo& info = s_extraFilesList.GetNext(pos);
				if (info.m_active)
					outFile.WriteString("\t<File RelativePath=\"" + info.m_name + "\"/>\n");
			}
			outFile.WriteString("</Files>\n");
			outFile.Close();

			CFileStatus wwFileStatus;
			CFile::GetStatus(wwFilename, wwFileStatus);
			CFileStatus fileStatus;
			if (CFile::GetStatus(projFilename, fileStatus))
			{
				fileStatus.m_mtime = wwFileStatus.m_mtime;
				CFile::SetStatus(projFilename, fileStatus);
				g_wwhizInterface->AddProject(projFilename);
			}

			_unlink(projFilename);

			WWhizProjectList& projectList = g_wwhizInterface->GetProjectList();
			pos = s_extraFilesList.GetHeadPosition();
			while (pos)
			{
				WWhizReg::ExtraFilesInfo& info = s_extraFilesList.GetNext(pos);
				WWhizProject* project = projectList.Find(info.m_name);
				if (project)
				{
					if (project->IsActive() != info.m_active)
						project->SetActive(info.m_active);
					project->SetNoRefresh(info.m_noRefresh);
				}
				else
				{
					// Huh?
				}
			}
		}
	}

	return true;
}


void GetExtraFiles(WWhizReg::ExtraFilesList& extraFiles)
{
	extraFiles.RemoveAll();
	POSITION pos = s_extraFilesList.GetHeadPosition();
	while (pos)
	{
		extraFiles.AddTail(s_extraFilesList.GetNext(pos));
	}
}


void SetExtraFiles(const WWhizReg::ExtraFilesList& extraFiles)
{
	s_extraFilesList.RemoveAll();
	POSITION pos = extraFiles.GetHeadPosition();
	while (pos)
	{
		s_extraFilesList.AddTail(extraFiles.GetNext(pos));
	}
}

