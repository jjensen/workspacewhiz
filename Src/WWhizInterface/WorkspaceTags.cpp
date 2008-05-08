///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceTags.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceTags.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 43   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "WorkspaceTags.h"
#include "WorkspaceInfo.h"
#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\bldguid.h>
#include <ObjModel\textguid.h>
#include <ObjModel\dbgguid.h>
#include "MemFile.h"
#include "Timer.h"
#include "VirtualDrive.h"
#include "VirtualFile.h"

const DWORD FILE_FORMAT_VERSION = 0x0210000b;

LPCSTR s_emptyString = "";

#if _MSC_VER > 0x1000
#define INLINE __forceinline
#else
#define INLINE inline
#endif


#define SLOW_TAG_READ
//#define FAST_TAG_READ

void ODS(LPCTSTR msg, ...)
{
	va_list args;
	char textBuffer[1024];

	va_start(args, msg);
	_vsnprintf(textBuffer, 1023, msg, args);
	va_end(args);

	OutputDebugString(textBuffer);
}

static INLINE void WriteString(CFile& file, const CString& str)
{
	BYTE len = (BYTE)str.GetLength();
	file.Write(&len, sizeof(BYTE));
	if (len > 0)
		file.Write((LPCTSTR)str, len);
}


static INLINE void WriteStringGT255(CFile& file, const CString& str)
{
	WORD len = (WORD)str.GetLength();
	file.Write(&len, sizeof(WORD));
	if (len > 0)
		file.Write((LPCTSTR)str, len);
}


static INLINE void WriteString(CFile& file, LPCTSTR str)
{
	BYTE len = (BYTE)strlen(str);
	file.Write(&len, sizeof(BYTE));
	if (len > 0)
		file.Write(str, len + 1);
}


static INLINE void WriteStringGT255(CFile& file, LPCTSTR str)
{
	WORD len = (WORD)strlen(str);
	file.Write(&len, sizeof(WORD));
	if (len > 0)
		file.Write(str, len + 1);
}



static INLINE void WriteString(CFile& file, LPCTSTR str, BYTE len)
{
	if (len > 0)
	{
		file.Write(&len, sizeof(BYTE));
		file.Write(str, len + 1);
	}
}


static INLINE void WriteStringGT255(CFile& file, LPCTSTR str, WORD len)
{
	if (len > 0)
	{
		file.Write(&len, sizeof(WORD));
		file.Write(str, len + 1);
	}
}



static INLINE void WriteTimeStamp(CFile& file, const CTime& timeStamp)
{
	time_t time = timeStamp.GetTime();
	file.Write(&time, sizeof(time_t));
}


static INLINE void ReadString(BYTE*& ptr, CString& str)
{
	BYTE len = *(BYTE*)ptr;			ptr += sizeof(BYTE);
	LPTSTR buf = str.GetBufferSetLength(len + 1);
	if (len > 0)
	{
		memcpy(buf, ptr, len);
		ptr += len;
	}
	str.ReleaseBuffer(len);
}


static INLINE void ReadStringGT255(BYTE*& ptr, CString& str)
{
	WORD len = *(WORD*)ptr;			ptr += sizeof(WORD);
	LPTSTR buf = str.GetBufferSetLength(len + 1);
	if (len > 0)
	{
		memcpy(buf, ptr, len);
		ptr += len;
	}
	str.ReleaseBuffer(len);
}


static INLINE void ReadDWORD(BYTE*& ptr, DWORD& dw)
{
	dw = *(DWORD*)ptr;		ptr += sizeof(DWORD);
}

static INLINE void ReadTimeStamp(BYTE*& ptr, CTime& timeStamp)
{
	time_t time;
	time = *(time_t*)ptr;			ptr += sizeof(time_t);
	timeStamp = time;
}


static INLINE void ReadString(BYTE*& ptr, LPCSTR& str)
{
	BYTE len = *(BYTE*)ptr;			ptr += sizeof(BYTE);
	if (len > 0)
	{
		str = (LPCSTR)ptr;
		ptr += len + 1;
	}
	else 
		str = s_emptyString;
}


static INLINE void ReadStringGT255(BYTE*& ptr, LPCSTR& str)
{
	WORD len = *(WORD*)ptr;			ptr += sizeof(WORD);
	if (len > 0)
	{
		str = (LPCSTR)ptr;
		ptr += len + 1;
	}
	else 
		str = s_emptyString;
}



CTimer s_loadSortTimer;

	
//////////////////////////////////////////////////////////////////////////////////////////////

static Tag* s_matchHead;
static Tag* s_matchTail;
static int s_matchCount;

static TagList s_tags;

static CListEx<File*, File*> s_filenameList;
static bool s_showTypes[WWhizTag::LAST_TYPE];

static UINT s_numTagChanges = 0;

extern FileMap g_globalFileMap;

static WWhizInterface::TagRefreshCallback s_tagRefreshCallback;
static void* s_tagRefreshUserData;

static bool g_tagsRefreshed;
CTime g_lastTagRefresh;

static CMap<CString, LPCTSTR, int, int> s_tagsChangedFileMap;
CMap<CString, LPCTSTR, int, int> g_filesChangedFileMap;
static bool s_checkActiveFilesForChanges;

void WorkspaceTags::AddChangedFile(CString filename)
{
	// Resolve the filename.
	WorkspaceInfo::ResolveFilename(WorkspaceInfo::GetWorkspaceLocation(), filename);
	s_tagsChangedFileMap[filename] = 0;

	int dotPos = filename.ReverseFind('.');
	if (dotPos != -1)
	{
		CString ext = filename.Mid(dotPos + 1);
		ext.MakeLower();
		if (ext == "dsw"  ||  ext == "vcw"  ||  ext == "dsp"  ||  ext == "vcp")
		{
			g_filesChangedFileMap[filename] = 0;
		}
	}
}


static UINT s_fcNumFiles;
static UINT s_fcWhichFile;
static File* s_iterFile;
static TagList* s_iterTagList;
static TagList* s_iterOrderedTagList;
static UINT s_numTags;

				
void CtagsFileCallback(const char* filename)
{
	if (s_iterTagList)
		s_iterTagList->SortByTag_Parent_Namespace();

	if (s_tagRefreshCallback)
	{
		WWhizInterface::TagCallbackInfo info;
		info.m_userData = s_tagRefreshUserData;
		info.m_type = WWhizInterface::TagCallbackInfo::REFRESH;
		info.m_curFile = s_fcWhichFile++;
		info.m_numFiles = s_fcNumFiles;
		info.m_curFilename = filename;
		(*s_tagRefreshCallback)(info);
	}

	s_iterFile = (File*)WorkspaceInfo::GetFileList().Find(filename);
	s_iterTagList = (TagList*)&s_iterFile->GetTagList();
	s_iterOrderedTagList = (TagList*)&s_iterFile->GetOrderedTagList();
}


inline bool isalnum(TCHAR ch)
{
	return ((ch >= 'A'  &&  ch <= 'Z')  ||  (ch >= 'a'  &&  ch <= 'z')  ||
		(ch >= '0'  &&  ch <= '9'));
}

enum TypesEnum
{
	TYPE_ACCESS,
	TYPE_CLASS,
	TYPE_ENUM,
	TYPE_FILE,
	TYPE_KIND,
	TYPE_IMPLEMENTATION,
	TYPE_INHERITS,
	TYPE_INTERFACE,
	TYPE_LINE,
	TYPE_NAMESPACE,
	TYPE_STRUCT,
	TYPE_UNION,
};

static char* typesList[] =
{
	"access",
	"class",
	"enum",
	"file",
	"kind",
	"implementation",
	"inherits",
	"interface",
	"line",
	"namespace",
	"struct",
	"union",
};

inline WWhizTag::Type GetTagTypeFromLetter(char ch)
{
	WWhizTag::Type tagType;

	switch (ch)
	{
		case 'c':	tagType = WWhizTag::CLASS;		break;
		case 'd':	tagType = WWhizTag::DEFINE;		break;
		case 'e':	tagType = WWhizTag::ENUM;		break;
		case 'f':	tagType = WWhizTag::FUNCTION;	break;
		case 'g':	tagType = WWhizTag::ENUMMEMBER;	break;
		case 'm':	tagType = WWhizTag::MEMBER;		break;
		case 'n':	tagType = WWhizTag::NAMESPACE;	break;
		case 'p':	tagType = WWhizTag::DECLARATION;break;
		case 's':	tagType = WWhizTag::STRUCTURE;	break;
		case 't':	tagType = WWhizTag::TYPEDEF;	break;
		case 'u':	tagType = WWhizTag::UNION;		break;
		case 'v':	tagType = WWhizTag::VARIABLE;	break;
		case 'F':	tagType = WWhizTag::FILE;		break;
		default:	tagType = WWhizTag::LAST_TYPE;	break;
	}

	return tagType;
}


int ReadTags(File* file, bool needSort = false)
{
	TagList& tagList = (TagList&)file->GetTagList();
	TagList& orderedTagList = (TagList&)file->GetOrderedTagList();

	BYTE* ptr = file->m_tagBuffer;
	BYTE* tagBufferEnd = ptr + file->m_tagBufferSize;

	// Quick sanity check.
	if (ptr + sizeof(DWORD) > tagBufferEnd)
	{
		file->ClearTags();
		return -1;
	}

	DWORD tagCount;
	ReadDWORD(ptr, tagCount);

	// Check for obscene tag count sizes.
	if (tagCount > 10000000)
	{
		file->ClearTags();
		return -1;
	}

	tagList.SetSize(tagCount);
	ASSERT(!file->m_tags);
	file->m_tags = new Tag[tagCount];
	if (!file->m_tags)
	{
		file->ClearTags();
		return -1;
	}
	for (DWORD j = 0; j < tagCount; j++)
	{
		// Quick sanity check.
		if (ptr + TAG_FLAG_OFFSET + sizeof(BYTE) > tagBufferEnd)
		{
			file->ClearTags();
			return -1;
		}

		// Create a new tag.
		Tag* tag = &file->m_tags[j];
//		Tag* tag = new Tag;
		tag->m_parent = file;
		tag->m_tagBuffer = ptr;
		ptr += TAG_FLAG_OFFSET;
//		tag->m_type = (WWhizTag::Type)*(BYTE*)ptr;  ptr += sizeof(BYTE);
//		tag->m_lineNumber = *(int*)ptr;			ptr += sizeof(int);
//		tag->m_access = (WWhizTag::Access)*(BYTE*)ptr;  ptr += sizeof(BYTE);
//		tag->m_implementationType = (WWhizTag::ImplementationType)*(BYTE*)ptr;  ptr += sizeof(BYTE);

		BYTE flag = *(BYTE*)ptr;  ptr += sizeof(BYTE);

		// More sanity checks.
		DWORD count = 0;
		if (flag & TAGFLAG_SHORTIDENT)			count++;
		if (flag & TAGFLAG_IDENT)				count++;
		if (flag & TAGFLAG_SEARCHSTRING)		count += 2;
		if (flag & TAGFLAG_SHORTPARENTIDENT)	count++;
		if (flag & TAGFLAG_PARENTIDENT)			count++;
		if (flag & TAGFLAG_NAMESPACE)			count++;
		if (ptr + count > tagBufferEnd)
		{
			file->ClearTags();
			return -1;
		}

		BYTE shortIdentLen		= (flag & TAGFLAG_SHORTIDENT)		? *(BYTE*)ptr++ : 0;
		BYTE identLen			= (flag & TAGFLAG_IDENT)			? *(BYTE*)ptr++ : 0;
		WORD searchStringLen	= 0;
		if (flag & TAGFLAG_SEARCHSTRING)
		{
			searchStringLen = *(WORD*)ptr;
			ptr += 2;
		}
		BYTE shortParentIdentLen= (flag & TAGFLAG_SHORTPARENTIDENT)	? *(BYTE*)ptr++ : 0;
		BYTE parentIdentLen		= (flag & TAGFLAG_PARENTIDENT)		? *(BYTE*)ptr++ : 0;
		BYTE namespaceLen		= (flag & TAGFLAG_NAMESPACE)		? *(BYTE*)ptr++ : 0;

		// Another sanity check.
		count = shortIdentLen + identLen + searchStringLen + 
				shortParentIdentLen + parentIdentLen + namespaceLen;
		if (flag & TAGFLAG_SHORTIDENT)			count++;
		if (flag & TAGFLAG_IDENT)				count++;
		if (flag & TAGFLAG_SEARCHSTRING)		count++;
		if (flag & TAGFLAG_SHORTPARENTIDENT)	count++;
		if (flag & TAGFLAG_PARENTIDENT)			count++;
		if (flag & TAGFLAG_NAMESPACE)			count++;
		if (ptr + count > tagBufferEnd)
		{
			file->ClearTags();
			return -1;
		}

		tag->m_shortIdent = s_emptyString;
		tag->m_ident = s_emptyString;
		tag->m_searchString = s_emptyString;
		tag->m_shortParentIdent = s_emptyString;
		tag->m_parentIdent = s_emptyString;
		tag->m_namespace = s_emptyString;

		if (shortIdentLen > 0)
		{
			tag->m_shortIdent = (LPCSTR)ptr;
			ptr += shortIdentLen + 1;
		}
		if (identLen > 0)
		{
			tag->m_ident = (LPCSTR)ptr;
			ptr += identLen + 1;
		}
		if (searchStringLen > 0)
		{
			tag->m_searchString = (LPCSTR)ptr;
			ptr += searchStringLen + 1;
		}
		if (shortParentIdentLen > 0)
		{
			tag->m_shortParentIdent = (LPCSTR)ptr;
			ptr += shortParentIdentLen + 1;
		}
		if (parentIdentLen > 0)
		{
			tag->m_parentIdent = (LPCSTR)ptr;
			ptr += parentIdentLen + 1;
		}
		if (namespaceLen > 0)
		{
			tag->m_namespace = (LPCSTR)ptr;
			ptr += namespaceLen + 1;
		}

		// This is a placeholder for the sort so we know what order the ordered
		// tag list was in.
		tag->m_matchNext = (Tag*)j;
		
/*		ReadString(ptr, tag->m_shortIdent);
		ReadString(ptr, tag->m_ident);
		ReadStringGT255(ptr, tag->m_searchString);
		ReadString(ptr, tag->m_shortParentIdent);
		ReadString(ptr, tag->m_parentIdent);
		ReadString(ptr, tag->m_namespace);
*/
		tagList.Add(tag);
		orderedTagList.Add(tag);
	}

	if (needSort)
	{
		s_loadSortTimer.Start();
		tagList.SortByTag_Parent_Namespace();
		s_loadSortTimer.Stop();
	}
	else
	{
		s_loadSortTimer.Start();
		orderedTagList.SortByLineNumber();
		s_loadSortTimer.Stop();
	}

	return tagCount;
}



void WriteTag(CFile& file, WWhizTag::Type type, int lineNumber, WWhizTag::Access access,
			  WWhizTag::ImplementationType implementationType, LPCSTR shortIdent,
			  LPCSTR ident, LPCSTR searchString, LPCSTR shortParentIdent, LPCSTR parentIdent,
			  LPCSTR namespaceStr)
{
	file.Write(&type, sizeof(BYTE));
	file.Write(&lineNumber, sizeof(lineNumber));
	file.Write(&access, sizeof(BYTE));
	file.Write(&implementationType, sizeof(BYTE));

	BYTE shortIdentLen = strlen(shortIdent);
	BYTE identLen = strlen(ident);
	WORD searchStringLen = strlen(searchString);
	BYTE shortParentIdentLen = strlen(shortParentIdent);
	BYTE parentIdentLen = strlen(parentIdent);
	BYTE namespaceLen = strlen(namespaceStr);

	BYTE flags = 0;
	if (shortIdentLen > 0)
		flags |= TAGFLAG_SHORTIDENT;
	if (identLen > 0)
		flags |= TAGFLAG_IDENT;
	if (searchStringLen > 0)
		flags |= TAGFLAG_SEARCHSTRING;
	if (shortParentIdentLen > 0)
		flags |= TAGFLAG_SHORTPARENTIDENT;
	if (parentIdentLen > 0)
		flags |= TAGFLAG_PARENTIDENT;
	if (namespaceLen > 0)
		flags |= TAGFLAG_NAMESPACE;
	file.Write(&flags, sizeof(BYTE));

	if (shortIdentLen > 0)
		file.Write(&shortIdentLen, sizeof(BYTE));
	if (identLen > 0)
		file.Write(&identLen, sizeof(BYTE));
	if (searchStringLen > 0)
		file.Write(&searchStringLen, sizeof(WORD));
	if (shortParentIdentLen > 0)
		file.Write(&shortParentIdentLen, sizeof(BYTE));
	if (parentIdentLen > 0)
		file.Write(&parentIdentLen, sizeof(BYTE));
	if (namespaceLen > 0)
		file.Write(&namespaceLen, sizeof(BYTE));

	if (shortIdentLen > 0)
		file.Write(shortIdent, shortIdentLen + 1);
	if (identLen > 0)
		file.Write(ident, identLen + 1);
	if (searchStringLen > 0)
		file.Write(searchString, searchStringLen + 1);
	if (shortParentIdentLen > 0)
		file.Write(shortParentIdent, shortParentIdentLen + 1);
	if (parentIdentLen > 0)
		file.Write(parentIdent, parentIdentLen + 1);
	if (namespaceLen > 0)
		file.Write(namespaceStr, namespaceLen + 1);
}


/**
**/
bool Tag::ConvertFromText(CFile& file, TCHAR* line)
{
	// Okay, split the tag line up by TAB characters.
	char* parts[40];
	memset(parts, 0, sizeof(parts));
	int numParts = 0;
	char* partPtr;
	char* lastPtr = line;
	do
	{
		if (numParts == 40)
			break;

		parts[numParts] = lastPtr;
		if (numParts == 2  &&  lastPtr[0] == '/')
		{
			partPtr = strstr(lastPtr + 1, ";\"\x009");
			_ASSERT(partPtr != NULL);
			partPtr += 2;
		}
		else
		{
			partPtr = strchr(lastPtr, 9);
		}

		if (partPtr)
		{
			*partPtr++ = 0;
			lastPtr = partPtr;
		}

		numParts++;
	} while (partPtr);

	if (numParts < 4)
		return false;

	// Create a new tag.
	WWhizTag::Type type;
	WWhizTag::Access access;
	LPCSTR ident;
	LPCSTR parentIdent;
	int lineNumber;
	
	// Write the identifier.
	type = GetTagTypeFromLetter(parts[3][0]);
	ident = parts[0];

	// Build the shortened identifier (no symbols).
	char shortParentIdent[4096];
	char shortIdent[200];
	LPCSTR namespaceStr;

	int sLen = 0;
	LPCTSTR ptr = (LPCTSTR)ident;
	while (*ptr)
	{
		if (isalnum(*ptr))
			shortIdent[sLen++] = (char)tolower(*ptr);
		else if (*ptr == '.')
			shortIdent[sLen++] = '.';
		ptr++;
	}
	shortIdent[sLen] = 0;

	// Skip parts[1], because it is the tag filename, and we don't need it.

	// The next part is a little tougher...
	char searchString[4096];
	searchString[0] = 0;
	if (parts[2][0] == '/')
	{
		// Chances are, it's a search string.
		//   Find the closing slash.
		ptr = parts[2] + _tcslen(parts[2]) - 3;
		int len = ptr - parts[2] - 1;

		// Find the fixup letters.
		searchString[0] = 0;
		int searchStringPos = 0;
		for (int i = 0; i < len; i++)
		{
			char c = parts[2][i + 1];
			if (c == '.'  ||  c == '*' ||  c == '['  ||  c == ']'  ||  c == '+')
				searchString[searchStringPos++] = '\\';
			searchString[searchStringPos++] = c;
		}
		searchString[searchStringPos] = 0;
	}
/*	else
	{
		ptr = strchr(parts[2], ';');
		*ptr = 0;
		tagLineNumber = atoi(parts[2]);
	}
*/

	// Get the type.
	WWhizTag::ImplementationType implementationType = WWhizTag::DEFAULT;

	// Walk all extension flags.
	shortParentIdent[0] = 0;
	namespaceStr = s_emptyString;
	parentIdent = s_emptyString;
	for (int whichPart = 4; whichPart < numParts; whichPart++)
	{
		char* label = parts[whichPart];
		char* value = strchr(parts[whichPart], ':');
		if (value)
		{
			*value++ = 0;
		}

		for (int i = 0; i < _countof(typesList); i++)
		{
			if (label[0] == typesList[i][0]  &&  strcmp(label, typesList[i]) == 0)
				break;
		}

		if (i == _countof(typesList))
			continue;	// Unknown.

		switch (i)
		{
			case TYPE_ACCESS:		// access
				if (strcmp(value, "public") == 0)
					access = WWhizTag::PUBLIC;
				else if (strcmp(value, "protected") == 0)
					access = WWhizTag::PROTECTED;
				else if (strcmp(value, "private") == 0)
					access = WWhizTag::PRIVATE;
				else if (strcmp(value, "?") == 0)
					access = WWhizTag::PRIVATE;
				else if (strcmp(value, "default") == 0)
					access = WWhizTag::PRIVATE;
				else if (strcmp(value, "friend") == 0)
					access = WWhizTag::FRIEND;
				break;

			case TYPE_CLASS:		// class
			case TYPE_ENUM:			// enum
			case TYPE_INHERITS:		// inherits
			case TYPE_STRUCT:		// struct
			case TYPE_UNION:		// union
				parentIdent = value;

				if (parentIdent  &&  parentIdent[0] != 0)
				{
					// Build the shortened identifier (no symbols).
					int sLen = 0;
					LPCSTR ptr = (LPCSTR)parentIdent;
					while (*ptr)
					{
						if (isalnum(*ptr))
							shortParentIdent[sLen++] = (char)tolower(*ptr);
						ptr++;
					}
					shortParentIdent[sLen] = 0;
				}
				break;

			case TYPE_FILE:		// file
				break;
			case TYPE_KIND:		// kind
				break;
			case TYPE_IMPLEMENTATION:		// implementation
				if (strcmp(value, "virtual") == 0)
					implementationType = WWhizTag::VIRTUAL;
				else if (strcmp(value, "pure virtual") == 0)
					implementationType = WWhizTag::PUREVIRTUAL;
				else if (strcmp(value, "abstract") == 0)
					implementationType = WWhizTag::ABSTRACT;
				break;
			case TYPE_NAMESPACE:		// namespace
				if (value[0] != 0)
				{
					namespaceStr = value;
				}
				break;
			case TYPE_LINE:
				lineNumber = atoi(value);
				break;
		}
	}

	WriteTag(file, type, lineNumber, access, implementationType, shortIdent,
			ident, searchString, shortParentIdent, parentIdent, namespaceStr);

	return true;
}


#ifdef FAST_TAG_READ

namespace Ctags
{
#include "\ctags403\entry.h"

void CtagsTagWriteCallback(const tagEntryInfo* const tag, const char* line)
{
	CString tagIdent;
	CString tagShortIdent;
	CString tagSearchString;
	WWhizTag::Type tagType;
	WWhizTag::Access tagAccess;
	CString tagParentIdent;
	CString tagShortParentIdent;
	CString tagNamespace;
	CString tagInherits;

	tagIdent = tag->name;

	// Build the shortened identifier (no symbols).
	char shortIdent[200];
	int sLen = 0;
	LPCTSTR ptr = (LPCTSTR)tagIdent;
	while (*ptr)
	{
		if (isalnum(*ptr))
			shortIdent[sLen++] = tolower(*ptr);
		ptr++;
	}
	shortIdent[sLen] = 0;
	tagShortIdent = shortIdent;

	char buf[4096];
	char* bufPtr = buf;

	// Find the fixup letters.
	UINT len = strlen(line);
	buf[0] = 0;
	for (UINT i = 0; i < len; i++)
	{
		char c = line[i];
		if (c == 13  ||  c == 10)
			break;
		if (c == '.'  ||  c == '*' ||  c == '['  ||  c == ']'  ||  c == '+')
			*bufPtr++ = '\\';
		*bufPtr++ = c;
	}
	*bufPtr = 0;
	
	tagSearchString = buf;

	// Get the type.
	switch (tag->kind)
	{
		case 'c':	tagType = WWhizTag::CLASS;		break;
		case 'd':	tagType = WWhizTag::DEFINE;		break;
		case 'e':	tagType = WWhizTag::ENUM;		break;
		case 'f':	tagType = WWhizTag::FUNCTION;	break;
		case 'g':	tagType = WWhizTag::ENUMMEMBER;	break;
		case 'm':	tagType = WWhizTag::MEMBER;		break;
		case 'n':	tagType = WWhizTag::NAMESPACE;	break;
		case 's':	tagType = WWhizTag::STRUCTURE;	break;
		case 't':	tagType = WWhizTag::TYPEDEF;	break;
		case 'u':	tagType = WWhizTag::UNION;		break;
		case 'v':	tagType = WWhizTag::VARIABLE;	break;
		case 'F':	tagType = WWhizTag::FILE;		break;
		default :	tagType = WWhizTag::LAST_TYPE;	break;
	}

	for (i = 0  ;  i < tag->otherFields.count  ;  ++i)
	{
	    const char *const label = tag->otherFields.label[i];
	    const char *const value = tag->otherFields.value[i];

	    if (!label)
			continue;

		for (int i = 0; i < _countof(typesList); i++)
		{
			if (label[0] == typesList[i][0]  &&  strcmp(label, typesList[i]) == 0)
				break;
		}

		if (i == _countof(typesList))
			continue;	// Unknown.

		if (strcmp(value, "ATL") == 0)
		{
			int hi = 5;
		}
		
		switch (i)
		{
			case 0:		// access
				if (strcmp(value, "public") == 0)
					tagAccess = WWhizTag::PUBLIC;
				else if (strcmp(value, "protected") == 0)
					tagAccess = WWhizTag::PROTECTED;
				else if (strcmp(value, "private") == 0)
					tagAccess = WWhizTag::PRIVATE;
				else if (strcmp(value, "?") == 0)
					tagAccess = WWhizTag::PRIVATE;
				else if (strcmp(value, "default") == 0)
					tagAccess = WWhizTag::PRIVATE;
				break;

			case 1:		// class
			case 2:		// enum
			case 3:		// file
			case 9:		// struct
			case 10:	// union
				tagParentIdent = value;

				if (tagParentIdent.GetLength() > 0)
				{
					// Build the shortened identifier (no symbols).
					char shortIdent[4096];
					int sLen = 0;
					LPCTSTR ptr = (LPCTSTR)tagParentIdent;
					while (*ptr)
					{
						if (isalnum(*ptr))
							shortIdent[sLen++] = tolower(*ptr);
						ptr++;
					}
					shortIdent[sLen] = 0;
					tagShortParentIdent = shortIdent;
				}
				break;

			case 4:		// kind
				break;
			case 5:		// implementation
				break;
			case 6:		// inherits
				tagInherits = value;
				break;
			case 8:		// namespace
				tagNamespace = value;
				break;
		}
	}

	// Create a new tag.
	Tag* newTag = new Tag;
	newTag->m_parent = s_iterFile;
	newTag->m_type = tagType;
	newTag->m_access = tagAccess;
	newTag->m_shortIdent = tagShortIdent;
	newTag->m_ident = tagIdent;
	newTag->m_searchString = tagSearchString;
	newTag->m_shortParentIdent = tagShortParentIdent;
	newTag->m_parentIdent = tagParentIdent;
	newTag->m_inherits = tagInherits;
	newTag->m_namespace = tagNamespace;

	s_iterTagList->Add(newTag);
	s_numTags++;
}

};



HINSTANCE s_ctagsLib;

typedef int (*fnCtagsParse)(const char* commandLine);
typedef void (*fnTagWriteCallback)(const Ctags::tagEntryInfo* const tag, const char* line);
typedef void (*fnSetTagWriteCallback)(fnTagWriteCallback callback);
typedef void (*fnCtagsFileCallback)(const char* filename);
typedef void (*fnSetFileCallback)(fnCtagsFileCallback callback);

fnSetFileCallback m_setFileCallback;
fnCtagsParse m_ctagsParse;
fnSetTagWriteCallback m_setTagWriteCallback;

#endif FAST_TAG_READ

/**
**/
void WorkspaceTags::Initialize()
{
	g_globalFileMap.InitHashTable(1023);

	for (int i = 0; i < (int)WWhizTag::LAST_TYPE; i++)
		s_showTypes[i] = true;
#ifdef FAST_TAG_READ
	// Try the add-in directory.
	// Get the module name and strip the module filename from it, leaving the
	// module path.
	TCHAR modulePath[_MAX_PATH];
	modulePath[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&modulePath, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(modulePath, '\\');
	if (ptr)
	{
		ptr++;
		*ptr = 0;
	}

	// Ctags DLL path.
	CString ctagsPath = modulePath + CString("CTagsHelper.dll");
	if (_access(ctagsPath, 00) == -1)
	{
		AfxMessageBox("Unable to find CTagsHelper.dll");
		return;
	}

	s_ctagsLib = AfxLoadLibrary(ctagsPath);

	m_setFileCallback = (fnSetFileCallback)GetProcAddress(s_ctagsLib, "SetFileCallback");

	m_ctagsParse = (fnCtagsParse)GetProcAddress(s_ctagsLib, "CtagsParse");
	m_setTagWriteCallback = (fnSetTagWriteCallback)GetProcAddress(s_ctagsLib, "SetTagWriteCallback");
	(*m_setTagWriteCallback)(Ctags::CtagsTagWriteCallback);
	(*m_setFileCallback)(CtagsFileCallback);
#endif FAST_TAG_READ
}


/**
**/
const Tag* WorkspaceTags::GetMatchHead(void)
{
	return s_matchHead;
}


/**
**/
const Tag* WorkspaceTags::GetMatchTail(void)
{
	return s_matchTail;
}


/**
**/
int WorkspaceTags::GetMatchCount(void)
{
	return s_matchCount;
}


/**
**/
const TagList& WorkspaceTags::GetTagList(void)
{
	return s_tags;
}


/**
**/
bool WorkspaceTags::GetShowType(WWhizTag::Type type)
{
	return s_showTypes[type];
}


/**
**/
void WorkspaceTags::SetShowType(WWhizTag::Type type, bool show)
{
	s_showTypes[type] = show;
}


/**
**/
const WWhizTag* WorkspaceTags::GetTagScope(const CString& filename, UINT y)
{
	FileList& fileList = (FileList&)g_wwhizInterface->GetActiveFileList();
	File* file = (File*)fileList.Find(filename);
	if (!file)
		return NULL;

	TagList& orderedTagList = (TagList&)file->GetOrderedTagList();
	int tagCount = orderedTagList.GetCount();
	WWhizTag* retTag = NULL;
	for (int i = 0; i < tagCount; i++)
	{
		Tag* tag = (Tag*)orderedTagList.Get(i);
		UINT tagLineNumber = tag->GetLineNumber();
		if (tagLineNumber <= y)
			retTag = tag;
		else
			break;
	}

	return retTag;
}


/**
**/
void WorkspaceTags::SetCallback(WWhizInterface::TagRefreshCallback callback, void* userData)
{
	s_tagRefreshCallback = callback;
	s_tagRefreshUserData = userData;
}

static bool s_showTypesChanged = false;
static bool s_showTypesOrig[WWhizTag::LAST_TYPE];

const int MAX_SLASHES = 10;
	
static INLINE void MakeCopy(LPCTSTR& ptr, char*& outPtr, LPCTSTR slashStrings[], int& numSlashes,
								   bool& doExactLen, bool& doCase, bool& isClassMember)
{
	numSlashes = 0;

	// Find symbols.
	LPCTSTR startPtr = ptr;
	while (*ptr)
	{
		// Check for special characters.
		if (*ptr == '-')
		{
			doCase = false;
		}
		else if (*ptr == '+')
		{
			doCase = true;
		}
		else if (*ptr == '\\')
		{
			doExactLen = true;
		}
		else if (*ptr == '=')
		{
			doExactLen = true;
			doCase = true;
		}
		else if (*ptr == ';')
		{
			break;
		}
		else if (*ptr == '!')
		{
			isClassMember = true;
		}
		else if (*ptr == '\'')
		{
			if (!s_showTypesChanged)
			{
				for (int i = 0; i < (int)WWhizTag::LAST_TYPE; i++)
				{
					s_showTypesOrig[i] = s_showTypes[i];
					s_showTypes[i] = false;
				}
				s_showTypesChanged = true;
			}

			ptr++;
			while (*ptr  &&  *ptr != '\'')
			{
				s_showTypes[GetTagTypeFromLetter(*ptr)] = true;
				ptr++;
			}

			if (!*ptr)
				ptr--;
		}
		else if (*ptr >= 'A'  &&  *ptr <= 'Z')
		{
			doCase = true;
		}

		ptr++;
	}
	ptr = startPtr;

	bool copySymbols = (doCase | doExactLen);

	// Make the copy.
	while (*ptr)
	{
		// Is it a character we care about?
		if (*ptr == '\'')
		{
			ptr++;
			while (*ptr  &&  *ptr != '\'')
			{
				ptr++;
			}
			if (!*ptr)
				ptr--;
		}
		else if (isalnum(*ptr))
		{
			// Yep, make it lowercase.
			if (doCase)
				*outPtr++ = *ptr;
			else
				*outPtr++ = (char)tolower(*ptr);
		}
		else if (copySymbols  &&  (*ptr == '_'  ||  *ptr == '~'  ||  *ptr == ':'))
		{
			*outPtr++ = *ptr;
		}
		else if (*ptr == '/'  ||  *ptr == '*')
		{
			*outPtr++ = 0;
			if (numSlashes == MAX_SLASHES)
				return;
			slashStrings[numSlashes++] = outPtr;
		}
		else if (*ptr == '.')
		{
			*outPtr++ = 0;
			if (numSlashes == MAX_SLASHES)
				return;
			slashStrings[numSlashes++] = outPtr;
			*outPtr++ = '.';
		}
		else if (*ptr == ';')
		{
			ptr++;
			*outPtr++ = 0;
			break;
		}

		ptr++;
	}

	*outPtr++ = 0;
}


/**
	\return Returns 0 to break, 1 to continue.
**/
static INLINE int CompareTag(LPCTSTR searchName, int searchNameLen,
								const CString& checkName, bool doExactLen,
								bool doCaseSensitive)
{
	// Compare the tag, if it exists.
	int checkNameLen = checkName.GetLength();

	// Do the compare.
	if (doExactLen)
	{
		if (checkNameLen != searchNameLen)
			return 1;

		// Are we past it alphabetically?
		if (tolower(searchName[0]) < tolower(checkName[0])  &&
			checkName[0] != '_'  &&  checkName[0] != '~')
			return 0;

		int ret;
		if (doCaseSensitive)
			ret = strcmp(searchName, checkName);
		else
			ret = stricmp(searchName, checkName);
		if (ret != 0)
			return 1;
	}
	else if (doCaseSensitive)
	{
		if (checkNameLen < searchNameLen)
			return 1;

		// Are we past it alphabetically?
		if (tolower(searchName[0]) < tolower(checkName[0])  &&
			checkName[0] != '_'  &&  checkName[0] != '~')
			return 0;

		int ret = strncmp(searchName, checkName, searchNameLen);
		if (ret != 0)
			return 1;
	}
	else //!doExactMatch
	{
		if (checkNameLen < searchNameLen)
			return 1;
		int ret = strncmp(searchName, checkName, searchNameLen);
		if (ret < 0)
			return 0;
		if (ret > 0)
			return 1;
	}

	return 2;
}


static char* __cdecl stristr(LPCTSTR str1, LPCTSTR str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;

    if ( !*str2 )
        return((char *)str1);

    while (*cp)
    {
        s1 = cp;
        s2 = (char *) str2;

        while ( *s1 && *s2 && !(tolower(*s1)-tolower(*s2)) )
            s1++, s2++;

        if (!*s2)
            return(cp);

        cp++;
    }

    return(NULL);
}


static INLINE bool CompareSubstring(LPCTSTR searchName, int searchNameLen, int curSubPos,
							 int numSubStrings,
							 LPCTSTR subStrings[], int subLens[],
							 bool doCaseSensitiveTag)
{
	// Compare the substring.
	for (int i = 0; i < numSubStrings; ++i)
	{
		if (subLens[i] > 0)
		{
			if (curSubPos + subLens[i] > searchNameLen)
				return false;

			LPCTSTR foundPtr;
			if (doCaseSensitiveTag)
			{
				foundPtr = strstr((LPCTSTR)searchName + curSubPos, subStrings[i]);
			}
			else
			{
				foundPtr = stristr((LPCTSTR)searchName + curSubPos, subStrings[i]);
			}

			if (!foundPtr)
				return false;

			curSubPos = foundPtr - searchName + subLens[i];
		}
	}

	return true;
}
		

/**
**/
void WorkspaceTags::MatchTag(const TagList& tags, LPCTSTR tagSourceName, WWhizTag::Type forceType) 
{
	// Set the linked list to NULL.
	s_matchHead = s_matchTail = NULL;
	s_matchCount = 0;

	// Knock out the last matched tag.
	if (g_wwhizReg)
		g_wwhizReg->GotoTag(NULL);
	
	bool doCaseSensitiveTag = false;
	bool doCaseSensitiveParent = false;
	bool doExactTagLen = false;
	bool doExactParentLen = false;
	bool isClassMember = false;

	///////////////////////////////////////////////////////////////////////////
	// Variables
	LPCTSTR	tagName = NULL;
	int		tagNameLen = 0;
	int		numTagSlashStrings;
	LPCTSTR	tagSlashStrings[MAX_SLASHES];
	int		tagSlashLens[MAX_SLASHES];
	LPCTSTR	parentName = NULL;
	int		parentNameLen = 0;
	int		numParentSlashStrings;
	LPCTSTR	parentSlashStrings[MAX_SLASHES];
	int		parentSlashLens[MAX_SLASHES];

	char tagNameBuf[512];

	// Make a copy of the tag, removing symbols.
	LPCTSTR ptr = tagSourceName;
	char* outPtr = tagNameBuf;
	tagName = outPtr;
	MakeCopy(ptr, outPtr, tagSlashStrings, numTagSlashStrings, doExactTagLen,
			doCaseSensitiveTag, isClassMember);
	isClassMember = false;

	// Calculate the lengths.
	tagNameLen = strlen(tagName);
	for (int i = 0; i < numTagSlashStrings; ++i)
	{
		tagSlashLens[i] = strlen(tagSlashStrings[i]);
	}

	// Make a copy of the parent, removing symbols.
	CString parentNameTemp;
	parentName = outPtr;
	MakeCopy(ptr, outPtr, parentSlashStrings, numParentSlashStrings, doExactParentLen,
			doCaseSensitiveParent, isClassMember);
	isClassMember = false;

	// Check for a class member.
/*	if (isClassMember)
	{
		// Match against the class member.
		MatchTag(tags, parentName);

		// Grab the first class name.
		Tag* tag = (Tag*)s_matchHead;
		while (tag  &&  tag->GetType() != WWhizTag::MEMBER)
			tag = (Tag*)tag->GetMatchNext();

		// Set the linked list to NULL.
		s_matchHead = s_matchTail = NULL;
		s_matchCount = 0;

		if (!tag)
		{
			return;
		}

		const CString& searchString = tag->GetSearchString();
		LPCTSTR ptrEnd = (LPCTSTR)searchString + searchString.GetLength();
		LPCTSTR ptr = searchString;
		while (ptr != ptrEnd  &&  !(isalnum(*ptr)  ||  *ptr == '_'))
			ptr++;
		LPCTSTR startPos = ptr;
		while (ptr != ptrEnd  &&  (isalnum(*ptr)  ||  *ptr == '_'))
			ptr++;
		LPCTSTR endPos = ptr;
		parentNameTemp = searchString.Mid(startPos - (LPCTSTR)searchString, endPos - startPos);
		parentName = parentNameTemp;
		parentSlashString = NULL;
	}
*/
	// Calculate the lengths.
	int allParentSlashLens = 0;
	parentNameLen = strlen(parentName);
	for (i = 0; i < numParentSlashStrings; ++i)
	{
		parentSlashLens[i] = strlen(parentSlashStrings[i]);
		allParentSlashLens += parentSlashLens[i];
	}

	///////////////////////////////////////////////////////////////////////////
	Tag* lastTag = NULL;
	int pos = 0;
	int tagCount = tags.GetCount();

	// Start looking for it.
	while (pos < tagCount)
	{
		Tag* tag = (Tag*)tags.Get(pos++);

		// Test the search type.
		if (!s_showTypes[tag->GetType()])
			continue;
		
		// Don't match against declarations with buddies.
		if (tag->Tag::GetType() == WWhizTag::DECLARATION  &&  tag->Tag::GetBuddy())
			continue;

		CString tagCheckIdent = (doCaseSensitiveTag | doExactTagLen) ? tag->GetIdent() : tag->GetShortIdent();

		// The tags are alphabetical, so we can quit early.
		if (tagNameLen > 0)
		{
			int ret = CompareTag(tagName, tagNameLen, tagCheckIdent, 
					doExactTagLen, doCaseSensitiveTag);
			if (ret == 0)
				break;
			if (ret == 1)
				continue;
		}

		if (!CompareSubstring(tagCheckIdent, tagCheckIdent.GetLength(), tagNameLen,
				numTagSlashStrings, tagSlashStrings, tagSlashLens,
				doCaseSensitiveTag))
			continue;

		// Compare the parent name.
		CString parentCheckIdent;
		if (doCaseSensitiveParent | doExactParentLen)
			parentCheckIdent = tag->GetParentIdent();
		else 
			parentCheckIdent = tag->GetShortParentIdent();
		if (parentCheckIdent.GetLength() > 0)
		{
			if (parentNameLen > 0)
			{
				int ret = CompareTag(parentName, parentNameLen, parentCheckIdent,
						doExactParentLen, doCaseSensitiveParent);
				if (ret == 0  ||  ret == 1)
					continue;
			}

			// Compare the substrings.
			if (!CompareSubstring(parentCheckIdent, parentCheckIdent.GetLength(), parentNameLen,
					numParentSlashStrings, parentSlashStrings, parentSlashLens,
					doCaseSensitiveTag))
				continue;
		}
		else if (parentNameLen != 0  ||  allParentSlashLens != 0)
			continue;

		// Found one.
		if (s_matchHead == NULL)
			s_matchHead = tag;
		if (lastTag != NULL)
			lastTag->m_matchNext = tag;
		tag->m_matchPrev = lastTag;
		tag->m_matchNext = NULL;
		s_matchTail = tag;
		lastTag = tag;
		s_matchCount++;
	}

	if (s_showTypesChanged)
	{
		for (int i = 0; i < (int)WWhizTag::LAST_TYPE; i++)
		{
			s_showTypes[i] = s_showTypesOrig[i];
		}
		s_showTypesChanged = false;
	}
}


/**
**/
void WorkspaceTags::RemoveAll(void)
{
	ProjectList& projectList = WorkspaceInfo::GetProjectList();
	POSITION pos = projectList.GetHeadPosition();
	while (pos)
	{
		Project* project = projectList.GetNext(pos);

		FileList& fileList = (FileList&)project->GetFileList();
		int fileListCount = fileList.GetCount();
		for (int i = 0; i < fileListCount; i++)
		{
			File* file = (File*)fileList.Get(i);
			file->SetTimeStamp(CTime(0));
			((TagList&)file->GetTagList()).RemoveAll();
			((TagList&)file->GetOrderedTagList()).RemoveAll();
		}
	}

	s_tags.RemoveAll();
}

#ifdef SLOW_TAG_READ

CTimer tagCreateTimer;

//
bool WorkspaceTags::ParseTextTags(BYTE* mem, DWORD memSize)
{
	CMemFile memFile(10 * 1024);

	DWORD tagCount = 0;
	DWORD tagCountPos = memFile.GetPosition();
	memFile.Write(&tagCount, sizeof(DWORD));

	CArrayEx<DWORD, DWORD> tagOffsets;
	tagOffsets.SetSize(0, 10000);

	CTimer parseTimer;

	// Read the file.
	BYTE* memEnd = mem + memSize;
	while (true)
	{
		tagOffsets.Add(memFile.GetPosition());
		parseTimer.Start();

		TCHAR* line = (TCHAR*)mem;
		while (true)
		{
			if (mem == memEnd)
			{
				*mem = 0;
				goto Done;
			}
			else if (*mem == '\n')
			{
				*mem++ = 0;
				break;
			}
			else if (*mem == '\r')
			{
				*mem = 0;
			}

			mem++;
		}
		int lineLen = mem - (BYTE*)line;

		// If there is nothing there, exit.
		if (lineLen == 0)
			continue;

		// If it's a tag comment line, skip it.
		if (_tcsncmp(line, _T("!_TAG_"), 6) == 0)
			continue;

		tagCreateTimer.Start();
		if (Tag::ConvertFromText(memFile, line))
			tagCount++;
		tagCreateTimer.Stop();
		
		parseTimer.Stop();
	}

Done:
	tagOffsets.Add(memFile.GetPosition());
	memFile.Seek(tagCountPos, CFile::begin);
	memFile.Write(&tagCount, sizeof(DWORD));

	memFile.SeekToBegin();

	DWORD memFileSize = memFile.GetLength();
	BYTE* memFileBuffer = memFile.Detach();

	s_iterFile->m_tagBufferSize = memFileSize;
	s_iterFile->m_tagBuffer = memFileBuffer;

	ReadTags(s_iterFile, true);
	s_iterFile->m_tagBuffer.Detach();

	// Okay, the tags are sorted... now remake the buffer.
	BYTE* newTagBuffer = new BYTE[memFileSize];
	BYTE* newBufferPos = newTagBuffer;
	memcpy(newBufferPos, memFileBuffer, sizeof(DWORD));
	newBufferPos += sizeof(DWORD);

	TagList& tagList = (TagList&)s_iterFile->GetTagList();
	for (UINT j = 0; j < tagCount; j++)
	{
		Tag* tag = (Tag*)tagList.Get(j);
		int origPos = (int)tag->m_matchNext;
		DWORD startPos = tagOffsets[origPos];
		DWORD endPos = tagOffsets[origPos + 1];

		DWORD count = endPos - startPos;
		memcpy(newBufferPos, memFileBuffer + startPos, count);
		newBufferPos += count;
		ASSERT(newBufferPos - newTagBuffer <= (int)memFileSize);
	}

	free(memFileBuffer);

	s_iterFile->ClearTags();

	s_iterFile->m_tagBuffer = newTagBuffer;
	s_iterFile->m_tagBufferSize = memFileSize;

	ReadTags(s_iterFile);

	return true;
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


// From some MSDN sample, I think.
static int CreatePipeChild(HANDLE& child, HANDLE* inH, HANDLE* outH, HANDLE* errH, LPCTSTR Command)
{
    static int PCount = 0;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);                        // Security descriptor for INHERIT.
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle       = 0;

    SECURITY_ATTRIBUTES lsa;     
    HANDLE ChildIn;
	HANDLE ChildOut;
	HANDLE ChildErr;
	lsa.nLength=sizeof(SECURITY_ATTRIBUTES);
	lsa.lpSecurityDescriptor=NULL;
	lsa.bInheritHandle=TRUE;
	
	// Create Parent_Write to ChildStdIn Pipe
	if (!CreatePipe(&ChildIn,inH,&lsa,0))
	{
		// Error.
	}

	// Create ChildStdOut to Parent_Read pipe
	if (!CreatePipe(outH,&ChildOut,&lsa,0))
	{
		// Error.
	}

	// Create ChildStdOut to Parent_Read pipe
	if (!CreatePipe(errH,&ChildErr,&lsa,0))
	{
		// Error.
	}

	// Lets Redirect Console StdHandles - easy enough
    PROCESS_INFORMATION pi;
    STARTUPINFO             si;
    HANDLE hNul;

	// Dup the child handle to get separate handles for stdout and err,
    hNul = CreateFile("NUL",
                      GENERIC_READ | GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL, OPEN_EXISTING,
                      0,
                      NULL);

    if (hNul != NULL)
	{
        // Set up members of STARTUPINFO structure.
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
        si.hStdOutput = ChildOut;
        si.hStdError    = ChildErr;
        si.hStdInput    = ChildIn;
        if (CreateProcess(NULL, (char*)Command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == TRUE)
        {
            CloseHandle(pi.hThread);        // Thread handle not needed
            //fprintf(stderr, "create process success\n");
            child = pi.hProcess;  // Return process handle (to get RC)
        } else
			return -1;
        CloseHandle(hNul);                                      // Close error handle,
    }
    else
	{
		// Error.
	}

    return 0;
}

#endif SLOW_TAG_READ



void WorkspaceTags::Save(bool forceSave)
{
	UINT i;

	WWhizConfig& config = g_wwhizInterface->GetConfig();

	CTimer fileArrayTimer;
	CTimer saveTimer;
	CTimer diskSaveTimer;

	ProjectList& projectList = WorkspaceInfo::GetProjectList();
	int numProjects = projectList.GetCount();
	for (int whichProject = 0; whichProject < numProjects; whichProject++)
	{
		Project* project = (Project*)projectList.GetProjectByIndex(whichProject);
		CString tagsFilename = project->GetName();
		int dotPos = tagsFilename.ReverseFind('.');
		if (dotPos != -1)
		{
			if (tagsFilename.Mid(dotPos + 1) == "dsw")
			{
				continue;
			}
		}
		
		FileList& fileList = (FileList&)project->GetFileList();

		fileArrayTimer.Start();
		CArrayEx<File*, File*> fileArray;
		fileArray.SetSize(0, fileList.GetCount());

		// Add files to the file array.
		UINT fileListCount = fileList.GetCount();
		for (i = 0; i < fileListCount; i++)
		{
			File* file = (File*)fileList.Get(i);

			// Only do certain extensions.
			const CString& ext = file->GetExt();
			int numExts = config.TagsExtGetCount();
			for (int j = 0; j < numExts; j++)
				if (ext == config.TagsExtGet(j))
					break;
			if (j == numExts)
				continue;

			fileArray.Add(file);
		}
		fileArrayTimer.Stop();

		if (dotPos != -1)
			tagsFilename = tagsFilename.Left(dotPos);
		tagsFilename += ".Tags.WW";

		DWORD fileCount = fileArray.GetSize();
		if (fileCount == 0)
		{
			_unlink(tagsFilename);
			continue;
		}

		// Check for the file being read-only.
		if (_access(tagsFilename, 00) == 0  &&  _access(tagsFilename, 02) == -1)
		{
			int ret =
				AfxMessageBox("The file " + tagsFilename + " is read-only.  It must be writable "
					"to proceed saving the tags.\n\nDo you want to make it writable?",
					MB_YESNO);
			if (ret == IDNO)
			{
				continue;
			}
			else
			{
				_chmod(tagsFilename, _S_IREAD | _S_IWRITE);
			}
		}

		bool createdDrive = false;

		VirtualDrive drive;
		if (forceSave)
		{
			drive.Create(tagsFilename);
			createdDrive = true;
		}
		else if (!drive.Open(tagsFilename))
		{
			drive.Create(tagsFilename);
			createdDrive = true;
		}
		
		UINT numChangedTags = drive.GetHeaderUserDataDWORD(0);

		for (i = 0; i < fileCount; ++i)
		{
			File* file = fileArray[i];

			VirtualDrive::FileEntry* checkEntry = drive.GetFileEntry(file->GetFullName());
			if (checkEntry  &&  checkEntry->GetTimeStamp() == file->GetTimeStamp())
				continue;

			if (s_tagRefreshCallback)
			{
				WWhizInterface::TagCallbackInfo info;
				info.m_userData = s_tagRefreshUserData;
				info.m_type = WWhizInterface::TagCallbackInfo::SAVE;
				info.m_curFile = i;
				info.m_numFiles = fileCount;
				info.m_curFilename = file->GetFullName();
				(*s_tagRefreshCallback)(info);
			}
			
			saveTimer.Start();

			MemFile infoFile(50 * 1024);
			char magic[5] = "WWIF";
			infoFile.Write(magic, 4);
			DWORD version = FILE_FORMAT_VERSION;
			infoFile.Write(&version, sizeof(DWORD));

			infoFile.Write(file->m_tagBuffer, file->m_tagBufferSize);

			TagList& tagList = (TagList&)file->GetTagList();
			UINT tagCount = tagList.GetCount();
			saveTimer.Stop();

			if (s_tagRefreshCallback)
			{
				WWhizInterface::TagCallbackInfo info;
				info.m_userData = s_tagRefreshUserData;
				info.m_type = WWhizInterface::TagCallbackInfo::SAVE;
				info.m_curFile = i;
				info.m_numFiles = fileCount;
				info.m_curFilename = "";
				(*s_tagRefreshCallback)(info);
			}
			
			diskSaveTimer.Start();

			drive.FileCopy(infoFile, file->GetFullName(), &file->GetTimeStamp());
			numChangedTags += tagCount;

			diskSaveTimer.Stop();
		}

		// Do we need to pack this drive?
		if (!createdDrive  &&  numChangedTags >= config.GetTagAutoPackAmount())
		{
			// Time to pack
			drive.Pack();
			drive.SetHeaderUserDataDWORD(0, 0);
		}
		else
		{
			if (createdDrive)
				drive.SetHeaderUserDataDWORD(0, 0);
			else
				drive.SetHeaderUserDataDWORD(0, drive.GetHeaderUserDataDWORD(0) + numChangedTags);
		}
		
		drive.Close();
	}

	ODS("File array time: %0.2f  Save time: %0.2f  Disk save time: %0.2f\n",
		fileArrayTimer.GetMillisecs(), saveTimer.GetMillisecs(),
		diskSaveTimer.GetMillisecs());
}


void MatchBuddies()
{
	// Wander the tag list and do matching.
	TagList& tagList = s_tags;
	int tagCount = tagList.GetCount();
	for (int i = 0; i < tagCount; i++)
	{
		Tag* tag = (Tag*)tagList.Get(i);
		tag->m_buddy = NULL;
	}

	for (i = 0; i < tagCount; i++)
	{
		Tag* tag = (Tag*)tagList.Get(i);
		Tag* closeMatchTag = NULL;
		if (!tag->Tag::GetBuddy()  &&  tag->Tag::GetType() == WWhizTag::DECLARATION)
		{
			bool isTagDestructor = tag->Tag::GetIdent()[0] == '~';
			// Find the implementation.
			for (int j = i + 1; j < tagCount; j++)
			{
				Tag* implTag = (Tag*)tagList.Get(j);
				if (implTag->Tag::GetBuddy())
					continue;
				if (implTag->Tag::GetType() == WWhizTag::DECLARATION)
					continue;
				int ret = _tcscmp(implTag->Tag::GetIdent(), tag->Tag::GetIdent());
				if (ret != 0)
				{
					if (implTag->Tag::GetIdent()[0] == '~'  ||  isTagDestructor)
						continue;
					break;
				}
				ret = _tcscmp(implTag->Tag::GetParentIdent(), tag->Tag::GetParentIdent());
				if (ret != 0)
					break;
				ret = _tcscmp(implTag->Tag::GetNamespace(), tag->Tag::GetNamespace());
				if (ret != 0)
				{
					closeMatchTag = implTag;
					continue;
				}

				// Close enough.  Match it.
				tag->m_buddy = implTag;
				implTag->m_buddy = tag;
				closeMatchTag = NULL;
				break;
			}

			if (closeMatchTag)
			{
				tag->m_buddy = closeMatchTag;
				closeMatchTag->m_buddy = tag;
				closeMatchTag = NULL;
			}
		}
	}
}


int s_loadTotalTagCount = 0;
CTimer s_loadDiskTimer;
CTimer s_loadReadTimer;
CTimer s_loadFindTimer;
CTimer s_loadStatTimer;
CTimer s_loadAllocateTimer;

	
void WorkspaceTags::Load(bool newProjectsOnly)
{
	g_wwhizInterface->RefreshFileList();

	s_tags.RemoveAll();

	bool autoRefreshTags = g_wwhizInterface->GetConfig().GetAutoRefreshTags();
	s_loadTotalTagCount = 0;

	LoadProjects(newProjectsOnly);
	
	CTimer addTimer;
	addTimer.Start();

	s_tags.SetSize(s_loadTotalTagCount);
	FileList& activeFileList = (FileList&)g_wwhizInterface->GetActiveFileList();
	DWORD activeFileListCount = (DWORD)activeFileList.GetCount();
	for (DWORD i = 0; i < activeFileListCount; i++)
	{	 
		File* file = (File*)activeFileList.Get(i);
		TagList& tagList = (TagList&)file->GetTagList();

		int tagCount = tagList.GetCount();
		for (int j = 0; j < tagCount; j++)
		{
			Tag* tag = (Tag*)tagList.Get(j);
			s_tags.Add(tag);
		}
	}

	addTimer.Stop();

	CTimer sortTimer;
	sortTimer.Start();
	s_tags.SortByTag_Parent_Namespace();
	sortTimer.Stop();

	MatchBuddies();

	ODS("WWhiz: Add: %0.2f  Sort: %0.2f\n", addTimer.GetMillisecs(),
		sortTimer.GetMillisecs());
}


CTime s_currentTagUpdateTime;


bool WorkspaceTags::LoadProjects(bool newProjectsOnly)
{
	s_loadDiskTimer.Reset();
	s_loadReadTimer.Reset();
	s_loadFindTimer.Reset();
	s_loadStatTimer.Reset();
	s_loadAllocateTimer.Reset();
	s_loadSortTimer.Reset();

	s_loadReadTimer.Start();

	ProjectList& projectList = WorkspaceInfo::GetProjectList();
	int numProjects = projectList.GetCount();
	for (int whichProject = 0; whichProject < numProjects; whichProject++)
	{
		Project* project = (Project*)projectList.GetProjectByIndex(whichProject);
		if (newProjectsOnly  &&  !project->m_newProject)
			continue;
		LoadProjectTags(project);
		project->m_newProject = false;
	}

	s_loadReadTimer.Stop();
	ODS("Read time: %0.2f  Disk time: %0.2f  Find time: %0.2f  Status time: %0.2f  Project Sort: %0.2f\n",
		s_loadReadTimer.GetMillisecs(),
		s_loadDiskTimer.GetMillisecs(), s_loadFindTimer.GetMillisecs(),
		s_loadStatTimer.GetMillisecs(), s_loadSortTimer.GetMillisecs());

	return true;
}


bool WorkspaceTags::LoadProjectTags(Project* project)
{
	bool autoRefreshTags = g_wwhizInterface->GetConfig().GetAutoRefreshTags();

	CString tagsFilename = project->GetName();
	int dotPos = tagsFilename.ReverseFind('.');
	if (dotPos != -1)
	{
		if (tagsFilename.Mid(dotPos + 1) == "dsw")
		{
			return true;
		}
	}
	
	if (dotPos != -1)
		tagsFilename = tagsFilename.Left(dotPos);
	tagsFilename += ".Tags.WW";

	VirtualDrive drive;
	if (!drive.Open(tagsFilename))
		return false;
	
	DWORD fileCount = drive.GetFileEntryCount();
	for (DWORD i = 0; i < fileCount; i++)
	{
		VirtualFile vFile;
		VirtualDrive::FileEntry* entry = drive.GetFileEntry(i);
		if (!drive.FileOpen(entry->GetFilename(), vFile))
			continue;

		s_loadFindTimer.Start();
		FileList& fileList = (FileList&)project->GetFileList();
		File* file = (File*)fileList.Find(entry->GetFilename());
		s_loadFindTimer.Stop();

		if (!file)
			continue;
		
		if (autoRefreshTags)
		{
			// Check the time stamp.
			s_loadStatTimer.Start();
			struct stat fileStatus;
			if (stat(entry->GetFilename(), &fileStatus) == -1)
				continue;
			if (entry->GetTimeStamp() != fileStatus.st_mtime)
				continue;
			s_loadStatTimer.Stop();
		}

		file->ClearTags();

		// Check the header.
		char magic[4];
		vFile.Read(&magic, 4);
		if (strncmp(magic, "WWIF", 4) != 0)
		{
			vFile.Close();
			drive.FileErase(entry->GetFilename());
			i--;
			fileCount--;
			continue;
		}

		DWORD version;
		vFile.Read(&version, sizeof(DWORD));
		if (version != FILE_FORMAT_VERSION)
		{
			vFile.Close();
			drive.FileErase(entry->GetFilename());
			i--;
			fileCount--;
			continue;
		}

		// Read in the file chunk.
		s_loadDiskTimer.Start();
		
		DWORD vFileSize = vFile.GetLength() - sizeof(DWORD) - 4;// - tagCount * sizeof(UINT);

		if (vFileSize > 0)
		{
			file->m_tagBuffer = new BYTE[vFileSize];
			vFile.Read(file->m_tagBuffer, vFileSize);
			file->m_tagBufferSize = vFileSize;
		}

		s_loadDiskTimer.Stop();

		int numTags = 0;
		if (vFileSize > 0)
		{
			s_loadReadTimer.Start();
			numTags = ReadTags(file);
			s_loadReadTimer.Stop();
		}

		vFile.Close();

		if (numTags != -1)
		{
			s_loadTotalTagCount += numTags;
			file->SetTimeStamp(entry->GetTimeStamp());
			file->m_lastTagUpdateTime = s_currentTagUpdateTime;
		}

		if (s_tagRefreshCallback)
		{
			WWhizInterface::TagCallbackInfo info;
			info.m_userData = s_tagRefreshUserData;
			info.m_type = WWhizInterface::TagCallbackInfo::LOAD;
			info.m_curFile = i;
			info.m_numFiles = fileCount;
			info.m_curFilename = file->GetFullName();
			(*s_tagRefreshCallback)(info);
		}
	}

	return true;
}



extern bool g_filesRefreshed;


void WorkspaceTags::CheckActiveFilesForChanges()
{
	s_checkActiveFilesForChanges = true;
}


void WorkspaceTags::CheckDateTimeStamps()
{
	// Build the tag file list.
	FileList& fileList = WorkspaceInfo::GetActiveFileList();

	for (int i = 0; i < fileList.GetCount(); i++)
	{
		// Write each file name.
		File* file = (File*)fileList.Get(i);

		// Pick up a couple aliases.
		AddChangedFile(file->GetFullName());
	}
}



void WorkspaceTags::Refresh(bool forceRefresh, bool forceSave)
{
	// No more matches.
	s_matchHead = s_matchTail = NULL;
	s_matchCount = 0;

	// Knock out the last matched tag.
	if (g_wwhizReg)
		g_wwhizReg->GotoTag(NULL);

	// Check for auto-refresh.
	WWhizConfig& config = g_wwhizInterface->GetConfig();
	bool refreshTags = config.GetAutoRefreshTags() | forceRefresh;
	
	s_currentTagUpdateTime = CTime::GetCurrentTime();
	
	// Run the project list.
	LoadProjects(true);

	// Build the tag file list.
	FileList& fileList = WorkspaceInfo::GetActiveFileList();

	bool buildGlobalList = false;
	if (refreshTags)
	{
		if (s_tagRefreshCallback)
		{
			WWhizInterface::TagCallbackInfo info;
			info.m_userData = s_tagRefreshUserData;
			info.m_type = WWhizInterface::TagCallbackInfo::REFRESH;
			info.m_curFile = 0;
			info.m_numFiles = s_filenameList.GetCount();
			info.m_curFilename = NULL;
			(*s_tagRefreshCallback)(info);
		}

		if (g_filesRefreshed  ||  s_checkActiveFilesForChanges  ||  forceRefresh)
		{
			CheckDateTimeStamps();
			s_checkActiveFilesForChanges = false;
		}

		if (s_filenameList.GetCount() != 0)
			g_filesRefreshed = true;

		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		// Automatically add the changed file map entries.
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		POSITION pos = s_tagsChangedFileMap.GetStartPosition();
		while (pos)
		{
			CString filename;
			int temp;
			s_tagsChangedFileMap.GetNextAssoc(pos, filename, temp);

			File* file = (File*)WorkspaceInfo::GetActiveFileList().Find(filename);

			if (file)
			{
				if (file->m_lastTagUpdateTime == s_currentTagUpdateTime)
					continue;
				file->m_lastTagUpdateTime = s_currentTagUpdateTime;

				// Only do certain extensions.
				const CString& ext = file->GetExt();
				int numExts = config.TagsExtGetCount();
				for (int j = 0; j < numExts; j++)
					if (ext == config.TagsExtGet(j))
						break;
				if (j == numExts)
					continue;

				CFileStatus fileStatus;
				if (!CFile::GetStatus(filename, fileStatus))
				{
					// File doesn't exist!!!!
					continue;
				}

				if (file->GetTimeStamp() == fileStatus.m_mtime)
				{
					// Nothing has changed!!!
					continue;
				}

				// Set the time stamp.
				file->SetTimeStamp(fileStatus.m_mtime);

				// Remove all the tags.
				TagList& tags = (TagList&)file->GetTagList();
				tags.RemoveAll();

				// Remove all the tags.
				g_tagsRefreshed = true;
				file->m_changed = false;

				s_filenameList.AddTail(file);
			}
		}

		s_tagsChangedFileMap.RemoveAll();

		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		// Parse the files.
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		if (s_filenameList.GetCount() > 0)
		{
			s_fcWhichFile = 0;
			s_fcNumFiles = s_filenameList.GetCount();
			s_iterFile = NULL;
			s_iterTagList = NULL;
			s_numTags = 0;

			CTimer tagsReadingTimer;
			CTimer parseTimer;
			parseTimer.Start();

			CString temp;
			CString cmdLine;

			// Build the command line.
			int ignoreCount = g_wwhizInterface->GetConfig().IgnoreTokensGetCount();
			for (int i = 0; i < ignoreCount; i++)
			{
				const CString& token = g_wwhizInterface->GetConfig().IgnoreTokensGet(i);
				temp.Format("-I %s ", token);
				cmdLine += temp;
			}
			cmdLine += "--file-tags=yes --c-types=+A+p -h +.inl --langmap=default --langmap=c++:+";
			int filesExtCount = g_wwhizInterface->GetConfig().FilesExtGetCount();
			for (i = 0; i < filesExtCount; i++)
			{
				const CString& ext = g_wwhizInterface->GetConfig().FilesExtGet(i);
				temp.Format(".%s", ext);
				cmdLine += temp;
			}
			cmdLine += " --excmd=pattern --sort=no ";

#ifdef FAST_TAG_READ
			POSITION pos = s_filenameList.GetHeadPosition();
			while (pos)
			{
				File* file = s_filenameList.GetNext(pos);
				cmdLine += "\"" + file->GetFullName() + "\" ";
			}

			tagsReadingTimer.Start();

			(*m_ctagsParse)(cmdLine);
			ODS("%d\n", s_numTags);

			tagsReadingTimer.Stop();
#endif FAST_TAG_READ

#ifdef SLOW_TAG_READ			
			cmdLine += " -u";
			
			// Try the add-in directory.
			// Get the module name and strip the module filename from it, leaving the
			// module path.
			TCHAR modulePath[_MAX_PATH];
			modulePath[0] = 0;
			::GetModuleFileName(g_wwhizInterfaceInstance, (TCHAR*)&modulePath, _MAX_PATH);
			TCHAR* ptr = _tcsrchr(modulePath, '\\');
			if (ptr)
			{
				ptr++;
				*ptr = 0;
			}

			// The ctags path.
			CString ctagsPath = modulePath + CString("ctags.exe");
			if (_access(ctagsPath, 00) == -1)
			{
				AfxMessageBox("Unable to find ctags.exe");
				return;
			}
			
			const int FILTER_CHARS = 1;
			char filterTerminator[FILTER_CHARS + 1];
			filterTerminator[0] = 0x01;
			filterTerminator[1] = 0x00;

			// Generate a unique temporary name.
			char* asciiTempName = _tempnam(NULL, "WW210TAGRESP_");
			CString asciiFilename = asciiTempName;
			free(asciiTempName);

			FILE* f = fopen(asciiFilename, "wt");
			if (!f)
			{
				AfxMessageBox("Unable to open ctags.exe");
				return;
			}
			fprintf(f, "%s\n", cmdLine);
			fclose(f);

			cmdLine.Format("%s --options=\"%s\" --filter=yes --filter-terminator=%s", (LPCTSTR)ctagsPath, asciiFilename, filterTerminator);

			HANDLE child;
			HANDLE hIn;
			HANDLE hOut;
			HANDLE hErr;
			int rc = CreatePipeChild(child, &hIn, &hOut, &hErr, cmdLine);
			if (rc == -1)
			{
				AfxMessageBox("Unable to open CTags process.");
				return;
			}
			
			MemFile memFile(10 * 1024);

			tagCreateTimer.Reset();
			char bufferOrig[10 * 1024];
			char* buffer;
			int bufLen = 10 * 1024;
			CString lastFullPath;

			POSITION pos = s_filenameList.GetHeadPosition();
			while (pos)
			{
				buffer = bufferOrig;

				// Write each file name.
				File* file = s_filenameList.GetNext(pos);
				const CString& fullPath = file->GetFullName();

				CtagsFileCallback(fullPath);

				tagsReadingTimer.Start();

				DWORD written;
				WriteFile(hIn, fullPath, fullPath.GetLength(), &written, NULL);
				if (written != (DWORD)fullPath.GetLength())
				{
					// Early termination.
					break;
				}
				WriteFile(hIn, "\n", 1, &written, NULL);
				if (written != 1)
				{
					// Early termination.
					break;
				}

				bool done = false;
				while (!done)
				{
					DWORD used;
					rc = ReadFile(hOut, buffer, bufLen, &used, NULL);
					if (rc != TRUE)
					{
						used = 0;
						break;
					}

					LPCTSTR ptr = buffer;
					LPCTSTR ptrEnd = buffer + used;
					while (ptr != ptrEnd)
					{
						if (*ptr == 0x01)
						{
							used = ptr - buffer + 1;
							done = true;
							break;
						}

						ptr++;
					}
					
					if (used)
						memFile.Write(buffer, used);
				}

				tagsReadingTimer.Stop();

				DWORD memSize = memFile.GetLength() - FILTER_CHARS;
				BYTE* mem = memFile.Detach();
				s_iterFile->ClearTags();
				ParseTextTags(mem, memSize);
				free(mem);
			}
			

			char c26 = 26;
			DWORD written;
			WriteFile(hIn, &c26, 1, &written, NULL);
			if (written != 1)
			{
				// Early termination.
			}

			CloseHandle(hIn);
			CloseHandle(hOut);
			CloseHandle(hErr);

			parseTimer.Stop();

			_unlink(asciiFilename);
#endif SLOW_TAG_READ
			
			ODS("Total Parse Time: %0.2f  Reading time: %0.2f\n", //  Create time: %0.2f\n",
				parseTimer.GetMillisecs(), tagsReadingTimer.GetMillisecs());

			buildGlobalList = true;
		}
	}

	if (g_filesRefreshed)
		buildGlobalList = true;

	if (buildGlobalList)
	{
		ProjectList& projectList = WorkspaceInfo::GetProjectList();
		POSITION pos = projectList.GetHeadPosition();
		while (pos)
		{
			Project* project = projectList.GetNext(pos);
			project->RemoveAllTags();
		}
		
		// Get the total tag count.
		int totalTagCount = 0;
		for (int i = 0; i < fileList.GetCount(); i++)
		{
			File& file = *(File*)fileList.Get(i);
			totalTagCount += file.GetTagList().GetCount();
		}

		s_tags.RemoveAll();
		s_tags.SetSize(totalTagCount);

		CTimer addTimer;
		addTimer.Start();

		for (i = 0; i < fileList.GetCount(); i++)
		{
			File* file = (File*)fileList.Get(i);
			TagList& tagList = (TagList&)file->GetTagList();

			for (int j = 0; j < tagList.GetCount(); j++)
			{
				Tag* tag = (Tag*)tagList.Get(j);
				s_tags.Add(tag);
			}
		}

		addTimer.Stop();

		CTimer sortTimer;
		sortTimer.Start();
		s_tags.SortByTag_Parent_Namespace();
		sortTimer.Stop();

		CTimer buddyMatchTimer;
		buddyMatchTimer.Start();
		MatchBuddies();
		buddyMatchTimer.Stop();
		
		ODS("Global Add Time: %0.2f  Global Sort Time: %0.2f  Buddy Match: %0.2f\n", addTimer.GetMillisecs(),
			sortTimer.GetMillisecs(), buddyMatchTimer.GetMillisecs());
	}

	if (refreshTags)
	{
		s_numTagChanges += s_filenameList.GetCount();
		if (g_wwhizInterface->GetConfig().GetTagAutoSaveAmount() != 0  &&
			s_numTagChanges >= g_wwhizInterface->GetConfig().GetTagAutoSaveAmount())
		{
			s_numTagChanges = 0;
			Save(forceSave);
		}
	}
		
	if (s_tagRefreshCallback)
	{
		WWhizInterface::TagCallbackInfo info;
		info.m_userData = s_tagRefreshUserData;
		info.m_type = WWhizInterface::TagCallbackInfo::REFRESH;
		info.m_curFile = 1;
		info.m_numFiles = 1;
		info.m_curFilename = NULL;
		(*s_tagRefreshCallback)(info);
	}

	s_filenameList.RemoveAll();
	s_tagRefreshCallback = NULL;

	g_filesRefreshed = false;

	if (g_tagsRefreshed)
		g_lastTagRefresh = CTime::GetCurrentTime();
}
