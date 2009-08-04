///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateManager.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/TemplateManager.cpp $
// $Date: 2003/01/16 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizTemplateManager.h"
#include "TemplateManager.h"
#include "TreeCtrlEx.h"
#include "TemplateGroup.h"
#include "Template.h"

TemplateManager* TemplateManager::s_instance;

/**
**/
TemplateManager::Info::~Info()
{
	delete m_file;
}


#ifdef WWHIZ_VC6
typedef bool (*pfnWWhizTemplateRegister)(WWhizTemplateManager* templateManager,
		IApplication* pApplication);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
typedef bool (*pfnWWhizTemplateRegister)(WWhizInterface* wwhizInterface,
										 WWhizTemplateManager* templateManager,
										 void* pDTE);
#endif WWHIZ_VSNET

typedef void (*pfnWWhizTemplateDestroy)();

		
/**
**/
TemplateManager::TemplateManager() :
	m_changed(false)
{
	s_instance = this;
}


/**
**/
TemplateManager::~TemplateManager()
{
	size_t i;
	for (i = 0; i < m_files.GetCount(); i++)
	{
		delete m_files[i];
	}

	for (i = 0; i < m_loadedCommandModules.GetCount(); i++)
	{
		HINSTANCE module = m_loadedCommandModules[i];
		pfnWWhizTemplateDestroy wwhizTemplateDestroy =
			(pfnWWhizTemplateDestroy)::GetProcAddress(module, "WWhizTemplateDestroy");
		(*wwhizTemplateDestroy)();
		AfxFreeLibrary(m_loadedCommandModules[i]);
	}
}

#ifdef _DEBUG
const LPCTSTR TEMPLATE_WILDCARD = "dtpl";
#else
const LPCTSTR TEMPLATE_WILDCARD = "tpl";
#endif

bool TemplateManager::LoadTplFile(const CString& filename)
{
	// Load the DLL.
	HINSTANCE module = AfxLoadLibrary(filename);
	if (module)
	{
		pfnWWhizTemplateRegister wwhizTemplateRegister =
			(pfnWWhizTemplateRegister)::GetProcAddress(module, "WWhizTemplateRegister");
		if (!wwhizTemplateRegister)
		{
			AfxFreeLibrary(module);
			return false;
		}
		else
		{
			m_loadedCommandModules.Add(module);
#ifdef WWHIZ_VC6
			(*wwhizTemplateRegister)(this, ObjModelHelper::GetInterface());
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
			(*wwhizTemplateRegister)(g_wwhizInterface, this, g_pDTE);
#endif WWHIZ_VSNET
			return true;
		}
	}

	return false;
}


/**
**/
bool TemplateManager::Create()
{
	if (!LoadTplFile(g_modulePath + "WWhizTemplates." + TEMPLATE_WILDCARD))
	{
		AfxMessageBox("Unable to load WWhizTemplates.tpl.  Full template support will be unavailable.");
		return false;
	}

	// Search the disk for all template commands.
	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile(g_modulePath + "TemplateCommands\\*." + TEMPLATE_WILDCARD, &fd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		while (true)
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				LoadTplFile(g_modulePath + "TemplateCommands\\" + fd.cFileName);
			}

			if (!FindNextFile(handle, &fd))
				break;
		}
	}

	return true;
}


/**
**/
int TemplateManager::Add(CString filename)
{
	// Does the file exist?
	if (_access(filename, 00) == -1)
		return -1;
	
	int index = -1;

	// See if it is in our list.
	bool found = false;
	for (size_t i = 0; i < m_files.GetCount(); i++)
	{
		Info* info = m_files[i];
		if (info->m_file->GetFilename().CompareNoCase(filename) == 0)
		{
			info->m_found = true;

			// Found it.  Do a refresh.
			info->m_file->Refresh();

			index = i;
			break;
		}
	}

	// Was it in the list?
	if (index == -1)
	{
		// Nope.  Make a new one.
		Info* info = WNEW Info;
		info->m_file = WNEW TemplateGroup(*this, filename);
		info->m_found = true;
		info->m_file->Refresh();			
		index = m_files.Add(info);
	}

	m_changed = true;

	return index;
}

	
/**
**/
bool TemplateManager::Remove(int index)
{
	m_files.RemoveAt(index);

	m_changed = true;

	return true;
}

	
/**
**/
bool TemplateManager::Refresh()
{
	bool refreshed = false;

	// Search the disk for all templates.
	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile(g_modulePath + "Templates\\*.WWTpl", &fd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		while (true)
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString filename = g_modulePath + "Templates\\" + fd.cFileName;

				// See if it is in our list.
				bool found = false;
				for (size_t i = 0; i < m_files.GetCount(); i++)
				{
					Info* info = m_files[i];
					if (info->m_file->GetFilename().CompareNoCase(filename) == 0)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					Add(filename);
					refreshed = true;
				}
			}

			if (!FindNextFile(handle, &fd))
				break;
		}
	}

	// Refresh each template file.
	for (int i = 0; i < GetCount(); i++)
	{
		Info* info = m_files[i];

		refreshed |= info->m_file->Refresh();
	}

	refreshed |= m_changed;
	m_changed = false;

	return refreshed;
}

	
/**
**/
int TemplateManager::GetCount()
{
	return m_files.GetCount();
}


/**
**/
WWhizTemplateGroup* TemplateManager::GetTemplateGroup(int index)
{
	return m_files[index]->m_file;
}


static inline void ReadStringGT255(CFile& file, CString& str)
{
	WORD len;
	if (file.Read(&len, sizeof(WORD)) != sizeof(WORD))
		return;
	LPTSTR buf = str.GetBufferSetLength(len);
	if (len > 0)
	{
		if (file.Read(buf, len) != len)
		{
			str.ReleaseBuffer(0);
			return;
		}
	}
	str.ReleaseBuffer(len);
}


static inline void ReadTimeStamp(CFile& file, CTime& timeStamp)
{
	unsigned __int64 time;
	file.Read(&time, sizeof(unsigned __int64));
	timeStamp = (time_t)time;
}

/**
**/
void TemplateManager::LoadRegistry()
{
	for (size_t i = 0; i < m_files.GetCount(); i++)
	{
		delete m_files[i];
	}
	m_files.RemoveAll();
	
	UINT size;
	BYTE* mem;
	BYTE* mem2;

	// Read in the filename file.
	if (AfxGetApp()->GetProfileBinary("Templates", "FileList2", &mem, &size)  &&  size != 0)
	{
		MemFile filenameFile;
		filenameFile.Write(mem, size);
		filenameFile.Seek(0, CFile::begin);

		WORD count;
		filenameFile.Read(&count, sizeof(WORD));
		size_t i;
		for (i = 0; i < count; i++)
		{
			CString filename;
			bool isActive;
			filenameFile.Read(&isActive, sizeof(bool));
			ReadStringGT255(filenameFile, filename);
			if (filename.IsEmpty())
				break;		// Error.

			bool oldChanged = m_changed;

			int which = Add(filename);
			if (which == -1)
			{
				m_changed = true;
			}
			else
			{
				WWhizTemplateGroup* file = GetTemplateGroup(which);
				file->SetActive(isActive);

				CTime timeStamp;
				ReadTimeStamp(filenameFile, timeStamp);

				if (timeStamp != file->GetTimeStamp())
					m_changed = true;
				else
					m_changed = oldChanged;
			}
		}
	}
	else if (AfxGetApp()->GetProfileBinary("Templates", "FileList", &mem2, &size))
	{
		MemFile filenameFile;
		filenameFile.Write(mem2, size);
		filenameFile.Seek(0, CFile::begin);
		delete [] mem2;

		WORD count;
		filenameFile.Read(&count, sizeof(WORD));
		size_t i;
		for (i = 0; i < count; i++)
		{
			CString filename;
			ReadStringGT255(filenameFile, filename);
			if (filename.IsEmpty())
				break;		// Error.

			bool oldChanged = m_changed;

			int which = Add(filename);
			if (which == -1)
			{
				m_changed = true;
			}
			else
			{
				WWhizTemplateGroup* file = GetTemplateGroup(which);

				CTime timeStamp;
				ReadTimeStamp(filenameFile, timeStamp);

				if (timeStamp != file->GetTimeStamp())
					m_changed = true;
				else
					m_changed = oldChanged;
			}
		}
	}
		
	delete [] mem;
	mem = NULL;

	// Read in the tree state.
	if (AfxGetApp()->GetProfileBinary("Templates", "TreeState", &mem, &size))
	{
		m_treeStateFile.SetLength(0);
		m_treeStateFile.Write(mem, size);
		m_treeStateFile.Seek(0, CFile::begin);
	}
	delete [] mem;
	mem = NULL;
	
	// Read in the parameters.
	if (AfxGetApp()->GetProfileBinary("Templates", "SavedParameters", &mem, &size))
	{
		MemFile paramFile;
		paramFile.Write(mem, size);
		paramFile.Seek(0, CFile::begin);
		delete [] mem;
		mem = NULL;

		if (size > 0)
		{
			// The saved parameters SHOULD be in the same order.
			for (size_t i = 0; i < (size_t)GetCount(); i++)
			{
				WWhizTemplateGroup* file = GetTemplateGroup(i);

				for (int j = 0; j < file->GetCount(); j++)
				{
					WWhizTemplate* code = file->Get(j);
					CString menuName = file->GetName() + "|" + code->GetName();

					WORD nameLen;
					if (paramFile.Read(&nameLen, sizeof(WORD)) != sizeof(WORD))
						break;				// Not enough info in the file.
					CString newMenuName;
					LPTSTR buf = newMenuName.GetBufferSetLength(nameLen);
					if (paramFile.Read(buf, nameLen) != nameLen)
						break;				// Not enough info in the file.
					newMenuName.ReleaseBuffer(nameLen);

					if (menuName.CompareNoCase(newMenuName) != 0)
					{
//						AfxMessageBox("Doesn't match.\n");
						goto DoneWithParams;
					}

					WORD dicCount;
					paramFile.Read(&dicCount, sizeof(WORD));
					for (int k = 0; k < dicCount; k++)
					{
						CString entry;
						CString value;

						BYTE bLen;
						paramFile.Read(&bLen, sizeof(BYTE));
						LPTSTR buf = entry.GetBufferSetLength(bLen);
						paramFile.Read(buf, bLen);
						entry.ReleaseBuffer(bLen);

						WORD wLen;
						paramFile.Read(&wLen, sizeof(WORD));
						buf = value.GetBufferSetLength(wLen);
						paramFile.Read(buf, wLen);
						value.ReleaseBuffer(wLen);

						code->SetEntry(entry, value);
					}
				}
			}
		}
	}

	delete[] mem;
	
DoneWithParams:
	int hi = 5;
}


/**
**/
void TemplateManager::SaveRegistry()
{
	int i;
	WORD count = GetCount();
	if (count > 0)
	{
		MemFile filenameFile;
		filenameFile.Write(&count, sizeof(count));
		for (i = 0; i < GetCount(); i++)
		{
			WWhizTemplateGroup* file = GetTemplateGroup(i);
			bool isActive = file->IsActive();
			filenameFile.Write(&isActive, sizeof(bool));
			WORD len = file->GetFilename().GetLength();
			filenameFile.Write(&len, sizeof(WORD));
			filenameFile.Write((LPCTSTR)file->GetFilename(), len);
			unsigned __int64 time = file->GetTimeStamp().GetTime();
			filenameFile.Write(&time, sizeof(unsigned __int64));
		}

		LONG filenameFileSize = filenameFile.GetLength();
		BYTE* filenameFileMem = filenameFile.Detach();
		AfxGetApp()->WriteProfileBinary("Templates", "FileList2",
			filenameFileMem, filenameFileSize);
		free(filenameFileMem);
	}
	else
	{
		AfxGetApp()->WriteProfileBinary("Templates", "FileList2",
			NULL, 0);
	}

	// Write out the tree state.
	LONG treeStateSize = m_treeStateFile.GetLength();
	BYTE* treeStateMem = m_treeStateFile.Detach();
	if (treeStateSize > 0)
	{
		AfxGetApp()->WriteProfileBinary("Templates", "TreeState", treeStateMem, treeStateSize);

		// Reattach the memory.
		m_treeStateFile.SetLength(0);
		m_treeStateFile.Write(treeStateMem, treeStateSize);

		free(treeStateMem);
	}

	// Write out all parameters.
	MemFile paramFile;
	for (i = 0; i < GetCount(); i++)
	{
		WWhizTemplateGroup* file = GetTemplateGroup(i);

		for (int j = 0; j < file->GetCount(); j++)
		{
			Template* code = (Template*)file->Get(j);
			CString menuName = file->GetName() + "|" + code->GetName();

			WORD nameLen = menuName.GetLength();
			paramFile.Write(&nameLen, sizeof(WORD));
			paramFile.Write((LPCTSTR)menuName, nameLen);

			WORD dicCount = code->m_dictionary.GetCount();
			paramFile.Write(&dicCount, sizeof(WORD));
			POSITION pos = code->m_dictionary.GetStartPosition();
			while (pos)
			{
				CString entry;
				CString value;
				code->m_dictionary.GetNextAssoc(pos, entry, value);

				BYTE bLen = entry.GetLength();
				paramFile.Write(&bLen, sizeof(BYTE));
				paramFile.Write((LPCTSTR)entry, bLen);

				WORD wLen;
				wLen = value.GetLength();
				paramFile.Write(&wLen, sizeof(WORD));
				paramFile.Write((LPCTSTR)value, wLen);
			}
		}
	}

	UINT paramFileSize = paramFile.GetLength();
	BYTE* paramFileMem = paramFile.Detach();
	AfxGetApp()->WriteProfileBinary("Templates", "SavedParameters", paramFileMem, paramFileSize);
	free(paramFileMem);
}


/**
	Restores a tree control's state.
**/
bool TemplateManager::RestoreTreeState(CTreeCtrlEx& tree)
{
	// Check the file length.
	if (m_treeStateFile.GetLength() == 0)
		return false;		// Unchanged.

	// Restore the tree state.
	m_treeStateFile.Seek(0, CFile::begin);

	// Read the template file count.
	WORD wCount;
	m_treeStateFile.Read(&wCount, sizeof(WORD));
	if ((int)wCount != GetCount())
		return true;		// Changed.

	// Run each template file.
	for (int i = 0; i < (int)wCount; i++)
	{
		CString filename;

		// Read in the template file name.
		WORD len;
		m_treeStateFile.Read(&len, sizeof(WORD));
		LPTSTR buf = filename.GetBufferSetLength(len);
		m_treeStateFile.Read(buf, len);
		filename.ReleaseBuffer(len);

		// Write out the time stamp.
		unsigned __int64 timeIn;
		m_treeStateFile.Read(&timeIn, sizeof(unsigned __int64));
		time_t time = timeIn;

		// Now, find it.
		int j;
		for (j = 0; j < GetCount(); j++)
		{
			WWhizTemplateGroup* file = GetTemplateGroup(j);

			// If we found a match, continue in the outer loop.
			if (file->GetFilename() == filename  &&  file->GetTimeStamp() == time)
				break;
		}

		// If we reached the end, then the entry wasn't found.
		if (j == GetCount())
			return true;	// Changed.
	}
	
	// Read the tree item count.
	DWORD dwCount;
	m_treeStateFile.Read(&dwCount, sizeof(DWORD));

	// One more sanity check.
	if (dwCount != tree.GetCount())
		return true;		// Changed.
	
	// Read the tree info.
	CTreeCursor selectedCursor;
	CTreeCursor cursor = tree.GetRootItem();
	while (cursor)
	{
		UINT state;
		state = cursor.GetState(0xFF);
		m_treeStateFile.Read(&state, sizeof(UINT));
		if (state & TVIS_EXPANDED)
			cursor.Expand();
		else
			cursor.Expand(TVE_COLLAPSE);
		if (state & TVIS_SELECTED)
		{
			cursor.Select(TVGN_CARET | TVGN_FIRSTVISIBLE);
			cursor.Select();
			selectedCursor = cursor;
		}
		cursor = cursor.GetNext();
	}

	selectedCursor.EnsureVisible();

	return false;
}


/**
	Saves a tree control's state.
**/
void TemplateManager::SaveTreeState(CTreeCtrlEx& tree)
{
	// Save the tree state.
	m_treeStateFile.SetLength(0);

	// Grab all the template file stuff and save it, so we can later save this
	// information to the registry.
	WORD wCount = GetCount();
	m_treeStateFile.Write(&wCount, sizeof(WORD));
	for (int i = 0; i < (int)wCount; i++)
	{
		// Get the template file pointer.
		WWhizTemplateGroup* file = GetTemplateGroup(i);

		// Write out the template file name.
		WORD len = file->GetFilename().GetLength();
		m_treeStateFile.Write(&len, sizeof(WORD));
		m_treeStateFile.Write((LPCTSTR)file->GetFilename(), len);

		// Write out the time stamp.
		unsigned __int64 time = file->GetTimeStamp().GetTime();
		m_treeStateFile.Write(&time, sizeof(unsigned __int64));
	}

	// Write the tree item count.
	DWORD dwCount = tree.GetCount();
	m_treeStateFile.Write(&dwCount, sizeof(DWORD));

	// Now save all the tree branch stuff.
	CTreeCursor cursor = tree.GetRootItem();
	while (cursor)
	{
		UINT state = cursor.GetState(0xFF);
		m_treeStateFile.Write(&state, sizeof(UINT));
		cursor = cursor.GetNext();
	}
}



CString TemplateManager::ParseCode(LPCTSTR codePtr, LPCTSTR bolPtr, WWhizTemplate* file)
{
	return Template::ParseCode(codePtr, bolPtr, file);
}


void TemplateManager::RegisterCommand(CString commandName, WWhizTemplateCommand& templateCommand)
{
	commandName.MakeLower();
	m_commands[commandName] = &templateCommand;
}


WWhizTemplateCommand* TemplateManager::GetCommand(CString name)
{
	name.MakeLower();
	WWhizTemplateCommand* templateCommand;
	BOOL found = m_commands.Lookup(name, templateCommand);
	if (found)
		return templateCommand;
	else
		return NULL;
}







