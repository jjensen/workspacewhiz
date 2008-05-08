///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateGroup.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/TemplateGroup.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 5    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizTemplateManager.h"
#include "TemplateGroup.h"
#include "TemplateManager.h"
#include "Template.h"

/**
**/
TemplateGroup::TemplateGroup(TemplateManager& manager, CString filename) :
	m_manager(manager),
	m_filename(filename)
{
}


/**
**/
TemplateGroup::~TemplateGroup()
{
	for (int i = 0; i < m_templates.GetSize(); i++)
	{
		delete m_templates[i];
	}
}


/**
**/
bool TemplateGroup::Refresh()
{
	CString line;
	CStdioFile file;

	// Check the date and time stamp.
	CFileStatus fileStatus;
	if (!CFile::GetStatus(m_filename, fileStatus))
	{
		return false;
	}
	if (m_timeStamp == fileStatus.m_mtime)
	{
		// Matches... don't reload.
		return false;
	}

	// Destroy the old stuff.
	for (int i = 0; i < m_templates.GetSize(); i++)
		delete m_templates[i];
	m_templates.RemoveAll();

	// Open the file.
	if (!file.Open(m_filename, CFile::modeRead))
	{
		return false;
	}

	// Assign as the new time stamp.
	m_timeStamp = fileStatus.m_mtime;

	// Possible states.
	enum State
	{
		FINDTEMPLATE,
		PARSETEMPLATE,
		PARSECODE,
		PARSEMEMO,
		PARSEDIALOG,
		PARSEPARAMS,
		PARSEKEY,
	};

	State state = FINDTEMPLATE;
	Template* curTemplate = NULL;
	CString* curDialog = NULL;
	
	// Read the template file name.
	file.ReadString(m_name);
	int lineNumber = 1;

	// Go until we're through.
	while (true)
	{
		// Read a line.
		lineNumber++;
		if (!file.ReadString(line))
		{
			// End of file.
			break;
		}

TopState:
		// Check the state.
		if (state == FINDTEMPLATE)
		{
			// Find the block.
			if (line.GetLength() >= 1  &&  line[0] == '[')
			{
				if (line[line.GetLength() - 1] == ']')
				{
					state = PARSETEMPLATE;

					curTemplate = new Template(*this);
					curTemplate->m_lineNumber = lineNumber;
					m_templates.Add(curTemplate);
					curTemplate->m_name = line.Mid(1, line.GetLength() - 2);
				}
			}
		}

		///////////////////////////////////////////////////////////////////////
		// Template parsing.
		///////////////////////////////////////////////////////////////////////
		else if (state == PARSETEMPLATE)
		{
			ASSERT(curTemplate);

			// Did we find a new block?
			if (line.GetLength() >= 1  &&  line[0] == '[')
			{
				curTemplate = NULL;
				state = FINDTEMPLATE;
				goto TopState;
			}

			// Search for the template sub-type to process.
			if (line.CompareNoCase("!!code") == 0)
			{
				state = PARSECODE;
				continue;
			}
			else if (line.CompareNoCase("!!memo") == 0)
			{
				state = PARSEMEMO;
				continue;
			}
			else if (line.CompareNoCase("!!dialog") == 0)
			{
				state = PARSEDIALOG;
				continue;
			}
			else if (line.CompareNoCase("!!params") == 0)
			{
				state = PARSEPARAMS;
				continue;
			}
			else if (line.CompareNoCase("!!key") == 0)
			{
				state = PARSEKEY;
				continue;
			}
		}

		///////////////////////////////////////////////////////////////////////
		// Code parsing
		///////////////////////////////////////////////////////////////////////
		else if (state == PARSECODE)
		{
			if (line.CompareNoCase("!!end") == 0)
			{
				state = PARSETEMPLATE;
				continue;
			}

			// Add the line to our list.
			Template::OffsetInfo offsetInfo;
			offsetInfo.m_offset = curTemplate->m_code.GetLength();
			offsetInfo.m_line = lineNumber;
			curTemplate->m_codeOffsets.Add(offsetInfo);
			curTemplate->m_code += line;
			curTemplate->m_code += "\n";
		}

		///////////////////////////////////////////////////////////////////////
		// Memo parsing
		///////////////////////////////////////////////////////////////////////
		else if (state == PARSEMEMO)
		{
			if (line.CompareNoCase("!!end") == 0)
			{
				state = PARSETEMPLATE;
				continue;
			}

			// Add the line to our list.
			curTemplate->m_memo += line;
			curTemplate->m_memo += " ";
		}

		///////////////////////////////////////////////////////////////////////
		// Dialog parsing
		///////////////////////////////////////////////////////////////////////
		else if (state == PARSEDIALOG)
		{
			if (line.CompareNoCase("!!end") == 0)
			{
				curDialog = NULL;
				state = PARSETEMPLATE;
				continue;
			}

			if (!curDialog  ||  line.CompareNoCase("!!next") == 0)
			{
				int index = curTemplate->m_pages.Add(CString(""));
				curDialog = &curTemplate->m_pages[index];
				if (line.CompareNoCase("!!next") == 0)
					continue;
			}
			
			if (line.IsEmpty())
			{
				line = "<br>";
			}

			// Add the line to our list.
			*curDialog += line;
			*curDialog += " ";
		}

		///////////////////////////////////////////////////////////////////////
		// Default parameters parsing
		///////////////////////////////////////////////////////////////////////
		else if (state == PARSEPARAMS)
		{
			if (line.CompareNoCase("!!end") == 0)
			{
				state = PARSETEMPLATE;
				continue;
			}

			// Add the line to our list.
			LPCTSTR ptr = (LPCTSTR)line;
			CharArray buffer;
			CString param = ParseToken(ptr, NULL);

			while (*ptr != 0  &&  *ptr == ' ')
				ptr++;

			if (*ptr == 0)
			{
				// Error!!!
			}

			Template::DefaultInfo info;
			info.m_entry = param;
			info.m_value = ptr;
			curTemplate->m_defaults.AddTail(info);
		}

		///////////////////////////////////////////////////////////////////////
		// Default parameters parsing
		///////////////////////////////////////////////////////////////////////
		else if (state == PARSEKEY)
		{
			state = PARSETEMPLATE;

			// Get the keys.
			LPCTSTR ptr = (LPCTSTR)line;
			CharArray buffer;
			CString param = ParseToken(ptr, NULL);

			curTemplate->m_key = param;
		}
	}
	
	// Close the template file.	
	file.Close();

	return true;
}


/**
**/
WWhizTemplate* TemplateGroup::Get(int index)
{
	return m_templates[index];
}


/**
**/
int TemplateGroup::Find(LPCTSTR name)
{
	for (int i = 0; i < m_templates.GetSize(); i++)
	{
		CString curName = m_templates[i]->GetName();
		LPCTSTR curNamePtr = curName;
		if (*curNamePtr == '+')
			curNamePtr++;
		if (strcmp(curNamePtr, name) == 0)
			return i;
	}

	return -1;
}

