#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "WorkspaceTags.h"
#include "WorkspaceInfo.h"

void Status::SysEvent()
{
	MSG temp;
	while (PeekMessage(&temp, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
		{
			AfxPostQuitMessage(0);
			return;
		}
	}
}


int Status::m_refCount;
CDialog* Status::m_dlg;

void Status::Create(void)
{
	if (!m_refCount)
	{
		m_dlg = new CDialog;
		m_dlg->Create(IDD_STATUS);
		m_dlg->CenterWindow();
		m_dlg->ShowWindow(SW_SHOW);
		SysEvent();
	}
	
	m_refCount++;
}

void Status::Print(LPCTSTR msg, ...)
{
	if (!m_dlg)
		return;
	
	va_list args;
	char textBuffer[1024];

	va_start(args, msg);
	_vsnprintf(textBuffer, 1023, msg, args);
	va_end(args);

	CListBox* list = (CListBox*)m_dlg->GetDlgItem(IDC_ST_LIST);
	int which = list->AddString(textBuffer);
	list->SetCurSel(which);
}

void Status::Destroy(bool modal)
{
	m_refCount--;
	if (!m_refCount)
	{
		if (modal)
			m_dlg->RunModalLoop(MLF_NOIDLEMSG);
		delete m_dlg;
		m_dlg = NULL;

		SysEvent();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

IApplication *WorkspaceTags::m_pApplication = NULL;
WorkspaceTags::Tag* WorkspaceTags::m_matchHead;
WorkspaceTags::Tag* WorkspaceTags::m_matchTail;
int WorkspaceTags::m_matchCount;
WorkspaceTags::Tag* WorkspaceTags::m_lastMatchedTag;
CString WorkspaceTags::m_lastMatchedName;

WorkspaceTags::TagList WorkspaceTags::m_tags;

void WorkspaceTags::MatchIdentifier(LPCTSTR tagSourceName) 
{
	// Don't rematch if they're the same.
	if (tagSourceName == m_lastMatchedName)
		return;

	// Set the last matched name.
	m_lastMatchedName = tagSourceName;

	// Set the linked list to NULL.
	m_matchHead = m_matchTail = NULL;
	m_matchCount = 0;
	m_lastMatchedTag = NULL;
	
	// Make a copy of the tag, removing symbols.
	char tagNameBuf[256];
	int tagSrcLen = strlen(tagSourceName);
	int parentPos = -1;
	int slashPos[2] = { -1, -1 };
	int curLen = 0;
	int curSrc = 0;
	while (tagSourceName[curSrc])
	{
		if (isalnum(tagSourceName[curSrc]))
		{
			tagNameBuf[curLen++] = tolower(tagSourceName[curSrc]);
		}
		else if (tagSourceName[curSrc] == '/')
		{
			tagNameBuf[curLen++] = 0;
			if (parentPos == -1)
				slashPos[0] = curLen;
			else
				slashPos[1] = curLen;
		}
		else if (tagSourceName[curSrc] == ';')
		{
			tagNameBuf[curLen++] = 0;
			parentPos = curLen;
		}

		curSrc++;
	}

	tagNameBuf[curLen] = 0;

	///////////////////////////////////////////////////////////////////////////
	// Break it down.  Tons faster to do it within the buffer than to use CStrings.
	// tag;parent with slashes, potentially once per side.
	char* parentSlashString = NULL;
	int parentSlashLen = 0;

	// Is there a ;parent in there?
	char* parentIdent = NULL;
	int parentLen = 0;
	if (parentPos != -1)
	{
		parentIdent = (char*)&tagNameBuf + parentPos;
		parentLen = strlen(parentIdent);

		if (slashPos[1] != -1)
		{
			parentSlashString = (char*)&tagNameBuf + slashPos[1];
			parentSlashLen = strlen(parentSlashString);
		}
	}

	char* tagName = (char*)&tagNameBuf;
	int tagNameLen = strlen(tagName);

	char* tagSlashString = NULL;
	int tagSlashLen = 0;

	if (slashPos[0] != -1)
	{
		tagSlashString = (char*)&tagNameBuf + slashPos[0];
		tagSlashLen = strlen(tagSlashString);
	}

	///////////////////////////////////////////////////////////////////////////
	// Start looking for it.
	Tag* lastTag = NULL;
	int topOne = -1;
	int curPos = 0;
	POSITION pos = m_tags.GetHeadPosition();
	while (pos != NULL)
	{
		POSITION oldPos = pos;
		Tag& tag = m_tags.GetNext(pos);

		// Compare the identifier.
		if (tagNameLen)
		{
			// Do the compare.
			if (tag.GetShortIdent().GetLength() < tagNameLen)
				continue;
			if (strnicmp(tagName, tag.GetShortIdent(), tagNameLen) != 0)
				continue;
		}

		//
		if (tagSlashLen)
		{
			if (tagNameLen + tagSlashLen >= tag.GetShortIdent().GetLength())
				continue;
			if (strstr((LPCTSTR)tag.GetShortIdent() + tagNameLen, tagSlashString) == NULL)
				continue;
		}
		
		// Compare the parent name.
		if (tag.GetShortParentIdent().GetLength())
		{
			if (parentLen != 0)
			{
				// Do the compare.
				if (tag.GetShortParentIdent().GetLength() < parentLen)
					continue;
				if (strnicmp(parentIdent, tag.GetShortParentIdent(), parentLen) != 0)
					continue;
			}

			if (parentSlashLen != 0)
			{
				if (parentLen + parentSlashLen >= tag.GetShortParentIdent().GetLength())
					continue;
				if (strstr((LPCTSTR)tag.GetShortParentIdent() + parentLen, parentSlashString) == NULL)
					continue;
			}
		}
		else if (parentLen != 0  ||  parentSlashLen != 0)
			continue;

		// Found one.
		if (m_matchHead == NULL)
			m_matchHead = &tag;
		if (lastTag != NULL)
			lastTag->m_matchNext = &tag;
		tag.m_matchPrev = lastTag;
		tag.m_matchNext = NULL;
		m_matchTail = &tag;
		lastTag = &tag;
		m_matchCount++;
	}
}

void WorkspaceTags::GotoTag(const WorkspaceTags::Tag* tag)
{
	if (!tag)
	{
		m_lastMatchedTag = NULL;
		return;
	}
	
	// Get the documents dispatch.
	CComPtr<IDispatch> p_Documents;
	m_pApplication->get_Documents(&p_Documents);
	CComQIPtr<IDocuments, &IID_IDocuments> pDocuments(p_Documents);

	// Open the document specified by the tag.
	CComPtr<IDispatch> p_Document;
	pDocuments->Open(CComBSTR(tag->GetFilename()), CComVariant("Auto"), CComVariant(VARIANT_FALSE), &p_Document);
	CComQIPtr<ITextDocument, &IID_ITextDocument> pText(p_Document);

	// Get the text selection COM interface.
	CComPtr<IDispatch> p_TextSelection;
	pText->get_Selection(&p_TextSelection);
	CComQIPtr<ITextSelection, &IID_ITextSelection> pSelection(p_TextSelection);

	// Is it a define?
	if (tag->GetType() == 'd')
	{
		// Yes, only a line number exists.  Highlight the line.
		pSelection->GoToLine(tag->GetLineNumber() + 1, CComVariant());
		pSelection->LineUp(CComVariant(dsExtend), CComVariant((long)1));
	}
	else
	{
		// Match it via regular expression.
		long flags = dsMatchCase + dsMatchFromStart + dsMatchRegExp;
		short findReturn;
		pSelection->FindText(CComBSTR(tag->GetSearchString()), CComVariant(flags), &findReturn);
		if (findReturn == 0)
		{
			AfxMessageBox(CString("Unable to find ") + tag->GetIdent() + CString(".  Please refresh tags.\n"));
		}
	}
	m_pApplication->ExecuteCommand(CComBSTR("WindowScrollToCenter"));

	m_lastMatchedTag = (WorkspaceTags::Tag*)tag;
}


void WorkspaceTags::RemoveAll(void)
{
	m_tags.RemoveAll();
}

static BOOL CmdExec(LPCTSTR cmdLine, BOOL closeHandles, HANDLE *processHandle)
{
	STARTUPINFO sinfo;
	memset(&sinfo, 0, sizeof(sinfo));
	sinfo.cb = sizeof(STARTUPINFO);
	sinfo.dwFlags = STARTF_USESHOWWINDOW;
	sinfo.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pinfo;
	BOOL ret = CreateProcess(NULL, (LPTSTR)cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &sinfo, &pinfo);
	if (closeHandles)
	{
		CloseHandle(pinfo.hProcess);
	}
	if (processHandle != NULL)
		*processHandle = pinfo.hProcess;
	CloseHandle(pinfo.hThread);

	return ret;
}


bool WorkspaceTags::BuildTagsFile(const WorkspaceInfo::Project& prj)
{
	Status::Print("Processing file %s.", prj.GetName());
	Status::SysEvent();
	
	// Build the filename of the file where all of the filenames to be
	// processed will be written.
	CString tagsSourceFilename = prj.GetName();
	int extPos = tagsSourceFilename.ReverseFind('.');
	_ASSERT(extPos != -1);
	tagsSourceFilename.GetBufferSetLength(extPos);
	tagsSourceFilename.ReleaseBuffer(extPos);
	tagsSourceFilename += ".tagssourceinput";

	// Write the filenames.
	CStdioFile diskFile;
	diskFile.Open(tagsSourceFilename, CFile::modeCreate | CFile::modeWrite);
	FileList& fileList = WorkspaceInfo::GetFileList();
	CString fullFilename;	// Here so it doesn't fall out of scope each time through the while.
	for (int i = 0; i < fileList.GetCount(); i++)
	{
		File& file = fileList[i];
		if (file.GetParent() == (void*)&prj)
		{
			// Insert the file name.
			fullFilename = file.GetPath() + file.GetTitle() + "." + file.GetExt() + "\n";
			diskFile.WriteString(fullFilename);
		}
	}
	diskFile.Close();

	// Build the .tags filenames.
	CString tagsDestFilename = prj.GetName();
	extPos = tagsDestFilename.ReverseFind('.');
	_ASSERT(extPos != -1);
	tagsDestFilename.GetBufferSetLength(extPos);
	tagsDestFilename.ReleaseBuffer(extPos);
	tagsDestFilename += ".tags";

	// Build the command line.
	CString cmdLine;
	cmdLine.Format("ctags -L \"%s\" -f \"%s\"", tagsSourceFilename, tagsDestFilename);

	// Run the process and wait for it to shut down.
	DWORD time = GetTickCount();
	HANDLE handle;
	CmdExec(cmdLine, FALSE, &handle);
	WaitForSingleObject(handle, INFINITE);
	CloseHandle(handle);
	Status::Print("Time running: %u", GetTickCount() - time);

	_unlink(tagsSourceFilename);

	return true;
}


//
bool WorkspaceTags::ReadTags(LPCTSTR filename)
{
	// Print the status.
	Status::Print("Reading tag file %s", filename);
	Status::SysEvent();
	
	// Open the file.
	CStdioFile file;
	if (!file.Open(filename, CFile::modeRead))
		return false;

	POSITION lastSortPos = m_tags.GetHeadPosition();
	DWORD totalParseTime = 0;
	DWORD totalSortTime = 0;
	bool done = false;

	// Read the file.
	while (done == false)
	{
		DWORD startParseTime = GetTickCount();

		TCHAR line[500];
		if (!file.ReadString(line, 500))
			break;
		int lineLen = strlen(line);
		while (lineLen  &&  line[lineLen - 1] == '\r'  ||  line[lineLen - 1] == '\n')
			lineLen--;
		line[lineLen] = 0;

		// If there is nothing there, exit.
		if (lineLen == 0)
			continue;

		// If it's a tag comment line, skip it.
		if (_tcsncmp(line, _T("!_TAG_"), 6) == 0)
			continue;

		// Okay, split the tag line up by TAB characters.
		char* parts[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
		int numParts = 0;
		char* ptr;
		char* lastPtr = line;
		do
		{
			if (numParts == 6)
				break;

			parts[numParts] = lastPtr;
			if (numParts == 2  &&  lastPtr[0] == '/')
			{
				ptr = strstr(lastPtr + 1, ";\"\x009");
				_ASSERT(ptr != NULL);
				ptr += 2;
			}
			else
			{
				ptr = strchr(lastPtr, 9);
			}

			if (ptr)
			{
				*ptr++ = 0;
				lastPtr = ptr;
			}

			numParts++;
		} while (ptr);

		// Okay, now pick out the pieces we need.
		Tag tag;
		if (parts[0])
		{
			tag.m_ident = parts[0];

			// Build the shortened identifier (no symbols).
			char shortIdent[200];
			int sLen = 0;
			for (int i = 0; i < tag.m_ident.GetLength(); i++)
				if (isalnum(tag.m_ident[i]))
					shortIdent[sLen++] = tolower(tag.m_ident[i]);
			shortIdent[sLen] = 0;
			tag.m_shortIdent = shortIdent;
		}
		if (parts[1])
			tag.m_filename = parts[1];

		// The next part is a little tougher...
		if (parts[2][0] == '/')
		{
			// Chances are, it's a search string.
			//   Find the closing slash.
			ptr = parts[2] + _tcslen(parts[2]) - 3;
			int len = ptr - parts[2] - 1;

			// Find the fixup letters.
			line[0] = 0;
			int linePos = 0;
			for (int i = 0; i < len; i++)
			{
				char c = parts[2][i + 1];
				if (c == '.'  ||  c == '*' ||  c == '['  ||  c == ']'  ||  c == '+')
					line[linePos++] = '\\';
				line[linePos++] = c;
			}
			line[linePos] = 0;
			
			tag.m_searchString = line;
		}
		else
		{
			ptr = strchr(parts[2], ';');
			*ptr = 0;
			tag.m_lineNumber = atoi(parts[2]);
		}

		// Get the type.
		tag.m_type = parts[3][0];

		// Get the parent, if any.
		if (parts[4])
		{
			ptr = strchr(parts[4], ':');
			if (ptr != NULL)
			{
				ptr++;
				tag.m_parentIdent = ptr;

				if (tag.m_parentIdent.GetLength())
				{
					// Build the shortened identifier (no symbols).
					char shortIdent[200];
					int sLen = 0;
					for (int i = 0; i < tag.m_parentIdent.GetLength(); i++)
						if (isalnum(tag.m_parentIdent[i]))
							shortIdent[sLen++] = tolower(tag.m_parentIdent[i]);
					shortIdent[sLen] = 0;
					tag.m_shortParentIdent = shortIdent;
				}
			}
		}

		totalParseTime += GetTickCount() - startParseTime;

		DWORD startSortTime = GetTickCount();

		// Now, sort it in.
		POSITION pos = lastSortPos;
		while (pos != NULL)
		{
			Tag& tagComp = m_tags.GetAt(pos);

			int ret = stricmp(tagComp.m_ident, tag.m_ident);
			if (ret == 1)
				break;

			m_tags.GetNext(pos);
		}
		if (pos != NULL)
			m_tags.InsertBefore(pos, tag);
		else
			m_tags.AddTail(tag);
		lastSortPos = pos;

		totalSortTime += GetTickCount() - startSortTime;
	}

	Status::Print("Parse Time: %u milliseconds", totalParseTime);
	Status::Print("Sort  Time: %u milliseconds", totalSortTime);
	return true;
}


//
void WorkspaceTags::Update(void)
{
	// Exit if there are tags.
	if (m_tags.GetCount() > 0)
		return;

	// No more matches.
	m_matchHead = m_matchTail = NULL;
	m_matchCount = 0;
	m_lastMatchedName = "\x01";
	m_lastMatchedTag = NULL;

	// Create the status window.
	Status::Create();

	// Run all the files in the project list.
	POSITION pos = WorkspaceInfo::GetProjectList().GetHeadPosition();
	while (pos != NULL)
	{
		WorkspaceInfo::Project& prj = WorkspaceInfo::GetProjectList().GetNext(pos);

		// Only refresh the tags if the project is active.
		if (prj.IsActive())
		{
			// Build the .tags filenames.
			CString tagsFilename = prj.GetName();
			int extPos = tagsFilename.ReverseFind('.');
			_ASSERT(extPos != -1);
			tagsFilename.GetBufferSetLength(extPos);
			tagsFilename.ReleaseBuffer(extPos);
			tagsFilename += ".tags";

			if (ReadTags(tagsFilename) == false)
			{
				BuildTagsFile(prj);
				ReadTags(tagsFilename);
			}
		}
	}

	MatchIdentifier("");

	Status::Destroy();
}


void WorkspaceTags::RefreshAll(void)
{
	RemoveAll();

	Status::Create();
	
	// No more matches.
	m_matchHead = m_matchTail = NULL;
	m_matchCount = 0;
	m_lastMatchedName = "\x01";
	m_lastMatchedTag = NULL;

	POSITION pos = WorkspaceInfo::GetProjectList().GetHeadPosition();
	while (pos != NULL)
	{
		WorkspaceInfo::Project& prj = WorkspaceInfo::GetProjectList().GetNext(pos);
	
		// Only refresh the tags if the project is active.
		if (prj.IsActive())
		{
			BuildTagsFile(prj);
		}
	}

	Update();
	
	Status::Destroy();
}

