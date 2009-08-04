///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceTags.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/WorkspaceTags.cpp $
// $Date: 2003/06/24 $ $Revision: #19 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2006 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizInterface.h"
#include "WorkspaceTags.h"
#include "WorkspaceInfo.h"
#include "MemFile.h"
#include "Timer.h"
#include "VirtualDrive.h"
#include "VirtualFile.h"

const DWORD FILE_FORMAT_VERSION = 0x0210000d;

LPCSTR s_emptyString = "";

#if _MSC_VER > 0x1000
#define INLINE __forceinline
#else
#define INLINE inline
#endif

//errfile FILE* errFile;

#define CTL 1
#define NUM 2
#define LET 4
#define REG 8
#define NUL 0

static const char s_charTypes[257] =
{
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,  // 0
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,  // 16
	NUL,NUL,NUL,NUL, NUL,NUL,NUL,NUL, REG,REG,REG,REG, NUL,NUL,REG,NUL,  // 32
	NUM,NUM,NUM,NUM, NUM,NUM,NUM,NUM, NUM,NUM,NUL,NUL, NUL,NUL,NUL,NUL,	 // 48
	NUL,LET,LET,LET, LET,LET,LET,LET, LET,LET,LET,LET, LET,LET,LET,LET,  // 64
	LET,LET,LET,LET, LET,LET,LET,LET, LET,LET,LET,REG, NUL,REG,NUL,NUL,	 // 80
	NUL,LET,LET,LET, LET,LET,LET,LET, LET,LET,LET,LET, LET,LET,LET,LET,	 // 96
	LET,LET,LET,LET, LET,LET,LET,LET, LET,LET,LET,NUL, NUL,NUL,NUL,NUL,	 // 112
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 128
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 144
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 160
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 176
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 192
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 208
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 224
	CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL, CTL,CTL,CTL,CTL,	 // 240
};

#define lex_isalnum(ch)			(s_charTypes[(ch)] & (LET | NUM))
#define lex_isregexsymbol(ch)	(s_charTypes[(ch)] & REG)


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
	unsigned __int64 time = timeStamp.GetTime();
	file.Write(&time, sizeof(unsigned __int64));
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
	unsigned __int64 time;
	time = *(unsigned __int64*)ptr;			ptr += sizeof(unsigned __int64);
	timeStamp = (time_t)time;
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

CTime s_currentTagUpdateTime;

static Tag* s_matchHead;
static Tag* s_matchTail;
static int s_matchCount;

static TagList s_tags;

static WList<File*> s_filenameList;
static bool s_showTypes[WWhizTag::LAST_TYPE];

static UINT s_numTagChanges = 0;

extern FileMap g_globalFileMap;

static WWhizInterface::TagRefreshCallback s_tagRefreshCallback;
static void* s_tagRefreshUserData;

static bool g_tagsRefreshed;
CTime g_lastTagRefresh;

static WMap<CString, int> s_tagsChangedFileMap;
WMap<CString, int> g_filesChangedFileMap;
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
		if ( ext == "dsw"  ||  ext == "sln"  ||  ext == "dsp"  ||  ext == "vcproj"  ||
			ext == "csproj"  ||  ext == "vbproj"  ||  ext == "stproj" )
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

				
bool CtagsFileCallback(const char* filename)
{
	if (s_iterTagList)
		s_iterTagList->SortByTag_Parent_Namespace();

	bool ret = true;

	if (s_tagRefreshCallback)
	{
		WWhizInterface::TagCallbackInfo info;
		info.m_userData = s_tagRefreshUserData;
		info.m_type = WWhizInterface::TagCallbackInfo::REFRESH;
		info.m_curFile = s_fcWhichFile++;
		info.m_numFiles = s_fcNumFiles;
		info.m_curFilename = filename;
		if (!(*s_tagRefreshCallback)(info))
			ret = false;
	}

	s_iterFile = (File*)WorkspaceInfo::GetFileList().Find(filename);
	s_iterTagList = (TagList*)&s_iterFile->GetTagList();
	s_iterOrderedTagList = (TagList*)&s_iterFile->GetOrderedTagList();

	return ret;
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

static int typesLenList[] =
{
	6, // "access",
	5, // "class",
	4, // "enum",
	4, // "file",
	4, // "kind",
	14, // "implementation",
	8, // "inherits",
	9, // "interface",
	4, // "line",
	9, // "namespace",
	6, // "struct",
	5, // "union",
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
	file->m_tags = WNEW Tag[tagCount];
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
//		Tag* tag = WNEW Tag;
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
		if (flag & TAGFLAG_SHORTNAMESPACE)		count++;
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
		BYTE shortNamespaceLen	= (flag & TAGFLAG_SHORTNAMESPACE)	? *(BYTE*)ptr++ : 0;
		BYTE namespaceLen		= (flag & TAGFLAG_NAMESPACE)		? *(BYTE*)ptr++ : 0;

		// Another sanity check.
		count = shortIdentLen + identLen + searchStringLen + 
				shortParentIdentLen + parentIdentLen + shortNamespaceLen + namespaceLen;
		if (flag & TAGFLAG_SHORTIDENT)			count++;
		if (flag & TAGFLAG_IDENT)				count++;
		if (flag & TAGFLAG_SEARCHSTRING)		count++;
		if (flag & TAGFLAG_SHORTPARENTIDENT)	count++;
		if (flag & TAGFLAG_PARENTIDENT)			count++;
		if (flag & TAGFLAG_SHORTNAMESPACE)		count++;
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
		tag->m_shortNamespace = s_emptyString;
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
		if (shortNamespaceLen > 0)
		{
			tag->m_shortNamespace = (LPCSTR)ptr;
			ptr += shortNamespaceLen + 1;
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


static BYTE* s_tagWriteBuffer = NULL;
static DWORD s_tagWriteBufferSize = 0;


CTimer partsBreakdownTimer;
CTimer searchFixupTimer;


/**
**/
static char shortParentIdent[4096];
static BYTE shortParentIdentLen;
static char shortIdent[200];
static BYTE shortIdentLen;
static char searchString[4096];
static WORD searchStringLen;
static WWhizTag::Type type;
static WWhizTag::Access accessType;
static const char* ident;
static BYTE identLen;
static const char* parentIdent;
static BYTE parentIdentLen;
static int lineNumber;
static char shortNamespace[4096];
static BYTE shortNamespaceLen;
static const char* namespaceStr;
static BYTE namespaceLen;
WWhizTag::ImplementationType implementationType;

inline int myToLower (int c)
{
	if ( (c >= 'A') && (c <= 'Z') )
		c = c + ('a' - 'A');
	return c;
}

CTimer tagWriteTimer;
CTimer shortIdentFixupTimer;
CTimer extensionFlagsTimer;

void WriteTag(CFile& file)
{
	// Figure out the size.
	DWORD tagSize = sizeof(BYTE) + sizeof(lineNumber) + sizeof(BYTE) + sizeof(BYTE) + sizeof(BYTE);

	BYTE flags = 0;
	if (shortIdentLen > 0)
	{
		flags |= TAGFLAG_SHORTIDENT;
		tagSize += 1 + shortIdentLen + 1;
	}
	if (identLen > 0)
	{
		flags |= TAGFLAG_IDENT;
		tagSize += 1 + identLen + 1;
	}
	if (searchStringLen > 0)
	{
		flags |= TAGFLAG_SEARCHSTRING;
		tagSize += 2 + searchStringLen + 1;
	}
	if (shortParentIdentLen > 0)
	{
		flags |= TAGFLAG_SHORTPARENTIDENT;
		tagSize += 1 + shortParentIdentLen + 1;
	}
	if (parentIdentLen > 0)
	{
		flags |= TAGFLAG_PARENTIDENT;
		tagSize += 1 + parentIdentLen + 1;
	}
	if (shortNamespaceLen > 0)
	{
		flags |= TAGFLAG_SHORTNAMESPACE;
		tagSize += 1 + shortNamespaceLen + 1;
	}
	if (namespaceLen > 0)
	{
		flags |= TAGFLAG_NAMESPACE;
		tagSize += 1 + namespaceLen + 1;
	}
	
//	file.SetLength(file.GetLength() + tagSize);
	
	if (s_tagWriteBufferSize < tagSize)
	{
		delete [] s_tagWriteBuffer;
		s_tagWriteBuffer = NULL;
	}
	
	if (!s_tagWriteBuffer)
	{
		s_tagWriteBufferSize = tagSize;
		s_tagWriteBuffer = WNEW BYTE[s_tagWriteBufferSize];
	}

	BYTE* ptr = s_tagWriteBuffer;
	*ptr++ = type;
	*(int*)ptr = lineNumber;		ptr += sizeof(int);
	*ptr++ = accessType;
	*ptr++ = implementationType;
	*ptr++ = flags;

//	file.Write(&type, sizeof(BYTE));
//	file.Write(&lineNumber, sizeof(lineNumber));
//	file.Write(&access, sizeof(BYTE));
//	file.Write(&implementationType, sizeof(BYTE));

//	file.Write(&flags, sizeof(BYTE));

	if (shortIdentLen > 0)
		*ptr++ = shortIdentLen;
//		file.Write(&shortIdentLen, sizeof(BYTE));
	if (identLen > 0)
		*ptr++ = identLen;
//		file.Write(&identLen, sizeof(BYTE));
	if (searchStringLen > 0)
	{
		*(WORD*)ptr = searchStringLen;		ptr += sizeof(WORD);
	}

	if (shortParentIdentLen > 0)
		*ptr++ = shortParentIdentLen;
	if (parentIdentLen > 0)
		*ptr++ = parentIdentLen;
	if (shortNamespaceLen > 0)
		*ptr++ = shortNamespaceLen;
	if (namespaceLen > 0)
		*ptr++ = namespaceLen;

	if (shortIdentLen > 0)
	{
		memcpy(ptr, shortIdent, shortIdentLen + 1);		ptr += shortIdentLen + 1;
	}
	if (identLen > 0)
	{
		memcpy(ptr, ident, identLen + 1);		ptr += identLen + 1;
	}
	if (searchStringLen > 0)
	{
		memcpy(ptr, searchString, searchStringLen + 1);		ptr += searchStringLen + 1;
	}
	if (shortParentIdentLen > 0)
	{
		memcpy(ptr, shortParentIdent, shortParentIdentLen + 1);		ptr += shortParentIdentLen + 1;
	}
	if (parentIdentLen > 0)
	{
		memcpy(ptr, parentIdent, parentIdentLen + 1);		ptr += parentIdentLen + 1;
	}
	if (shortNamespaceLen > 0)
	{
		memcpy(ptr, shortNamespace, shortNamespaceLen + 1);		ptr += shortNamespaceLen + 1;
	}
	if (namespaceLen > 0)
	{
		memcpy(ptr, namespaceStr, namespaceLen + 1);		ptr += namespaceLen + 1;
	}

	if ((int)tagSize != ptr - s_tagWriteBuffer)
	{
		OutputDebugString("WWhiz: Bad size.\n");
	}
	file.Write(s_tagWriteBuffer, tagSize);
}


bool Tag::ConvertFromText(CFile& file, TCHAR* line)
{
	lineNumber = -1;

//	fprintf(errFile, line);
//	fprintf(errFile, "\n");
//	fflush(errFile);

//	partsBreakdownTimer.Start();
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
			partPtr = lastPtr;
			while (*partPtr  &&  *partPtr != '\t')
				partPtr++;
			if (*partPtr == 0)
			{
				parts[numParts + 1] = partPtr + 1;
				partPtr = NULL;
			}
//			partPtr = strchr(lastPtr, '\t');
		}

		if (!partPtr)
		{
			numParts++;
			break;
		}

		if (*partPtr)
		{
			*partPtr++ = 0;
			// Skip extraneous tabs... the Ctags Lua parser is flawed this way.
			// A quick scan of the Ctags code may indicate other parts are
			// flawed, too.
			while (*partPtr  &&  *partPtr == '\t')
				partPtr++;
			lastPtr = partPtr;
		}

		numParts++;
	} while (partPtr);

//	partsBreakdownTimer.Stop();

	if (numParts < 4)
		return false;

	// Create a new tag.
	
	// Write the identifier.
	type = GetTagTypeFromLetter(parts[3][0]);
	ident = parts[0];
	identLen = parts[1] - parts[0] - 1;

	// Build the shortened identifier (no symbols).
//	shortIdentFixupTimer.Start();

	int sLen = 0;
	char* inPtr = (char*)ident;
	char* outPtr = shortIdent;
	char ch;
	while (ch = *inPtr++)
	{
//		if (isalnum(ch))
		if (lex_isalnum(ch))
//			shortIdent[sLen++] = (char)myToLower(*ptr);
			*outPtr++ = (char)myToLower(ch);
//			*outPtr++ = (char)lex_tolower(ch);
		else if (ch == '.')
			*outPtr++ = ch;
	}
	*outPtr = 0;
//	shortIdent[sLen] = 0;
	shortIdentLen = outPtr - shortIdent;
//	shortIdentFixupTimer.Stop();
	
	// Skip parts[1], because it is the tag filename, and we don't need it.

//	searchFixupTimer.Start();
	// The next part is a little tougher...
	searchString[0] = 0;
	searchStringLen = 0;
	if (parts[2][0] == '/')
	{
		// Chances are, it's a search string.
		//   Find the closing slash.
//		ptr = parts[2] + _tcslen(parts[2]) - 3;
		char* ptr = parts[2] + (parts[3] - parts[2]) - 4;
		*ptr = 0;
//		int len = ptr - parts[2] - 1;

		// Find the fixup letters.
//		searchString[0] = 0;
//		int searchStringPos = 0;
//		memcpy(searchString, &parts[2][1], len);
		char* inPtr = (char*)&parts[2][1];
		outPtr = searchString;
//		for (int i = 0; i < len; i++)
		while (ch = *inPtr++)
		{
//			char c = parts[2][i + 1];
//			char c = *in++;
//			if (ch == '.'  ||  ch == '*' ||  ch == '['  ||  ch == ']'  ||  ch == '+')
//			if (lex_isregexsymbol(ch))
//				*outPtr++ = '\\';
			// Get the character following the backslash.
			if ( ch == '\\' )
				ch = *inPtr++;

			*outPtr++ = ch;
		}

		*outPtr = 0;
		searchStringLen = outPtr - searchString;
//		searchString[searchStringPos] = 0;
//		searchString[len] = 0;
	}
//	searchFixupTimer.Stop();
	else
	{
		char* ptr = strchr(parts[2], ';');
		if (ptr)
		{
			*ptr = 0;
			lineNumber = atoi(parts[2]);
		}
		else
		{
			lineNumber = -1;
		}
	}

	// Get the type.
	implementationType = WWhizTag::DEFAULT;

	// Walk all extension flags.
//	extensionFlagsTimer.Start();
	shortParentIdent[0] = 0;
	shortParentIdentLen = 0;
	shortNamespace[0] = 0;
	shortNamespaceLen = 0;
	namespaceStr = s_emptyString;
	parentIdent = s_emptyString;
	parentIdentLen = 0;
	namespaceLen = 0;

	for (int whichPart = 4; whichPart < numParts; whichPart++)
	{
		char* label = parts[whichPart];
		int labelLen;
		char* value = strchr(parts[whichPart], ':');
		if (value)
		{
			labelLen = value - label;
			*value++ = 0;
		}
		else
			labelLen = parts[whichPart + 1] - label - 1;

		int i;
		for (i = 0; i < _countof(typesList); i++)
		{
			if (labelLen == typesLenList[i]  &&  label[0] == typesList[i][0]  &&
				strcmp(label, typesList[i]) == 0)
				break;
		}

		if (i == _countof(typesList))
			continue;	// Unknown.

		switch (i)
		{
			case TYPE_ACCESS:		// access
				if (strcmp(value, "public") == 0)
					accessType = WWhizTag::PUBLIC;
				else if (strcmp(value, "protected") == 0)
					accessType = WWhizTag::PROTECTED;
				else if (strcmp(value, "private") == 0)
					accessType = WWhizTag::PRIVATE;
				else if (strcmp(value, "?") == 0)
					accessType = WWhizTag::PRIVATE;
				else if (strcmp(value, "default") == 0)
					accessType = WWhizTag::PRIVATE;
				else if (strcmp(value, "friend") == 0)
					accessType = WWhizTag::FRIEND;
				break;

			case TYPE_CLASS:		// class
			case TYPE_ENUM:			// enum
			case TYPE_INHERITS:		// inherits
			case TYPE_STRUCT:		// struct
			case TYPE_UNION:		// union
				parentIdent = value;
				parentIdentLen = parts[whichPart + 1] - value - 1;

				if (parentIdent  &&  parentIdent[0] != 0)
				{
					// Build the shortened identifier (no symbols).
					char* inPtr = (char*)parentIdent;

					char* lastColonPtr = 0;
					char ch;
					while (ch = *inPtr++)
					{
						if (ch == ':')
							lastColonPtr = inPtr - 1;
					}
					inPtr = lastColonPtr ? lastColonPtr + 1 : (char*)parentIdent;
					if (lastColonPtr)
					{
						while (*lastColonPtr == ':')
							*lastColonPtr-- = 0;
						if (i == TYPE_CLASS  ||  i == TYPE_INHERITS  ||  i == TYPE_STRUCT)
						{
							namespaceStr = parentIdent;
							namespaceLen = lastColonPtr - parentIdent + 1;
							parentIdent = inPtr;
						}
					}

					char* outPtr = shortParentIdent;
					while (ch = *inPtr++)
					{
						if (lex_isalnum(ch))
							*outPtr++ = (char)myToLower(ch);
					}
					*outPtr = 0;
					shortParentIdentLen = outPtr - shortParentIdent;
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
					namespaceLen = parts[whichPart + 1] - value - 1;
				}
				break;
			case TYPE_LINE:
				lineNumber = atoi(value);
				break;
		}
	}
//	extensionFlagsTimer.Stop();
	
	if (namespaceStr)
	{
		const char* inPtr = namespaceStr;
		char* outPtr = shortNamespace;
		while (ch = *inPtr++)
		{
			if (lex_isalnum(ch))
				*outPtr++ = (char)myToLower(ch);
		}
		*outPtr = 0;
		shortNamespaceLen = outPtr - shortNamespace;
	}

//	tagWriteTimer.Start();
	WriteTag(file);
//	tagWriteTimer.Stop();

	return true;
}


/**
**/
void WorkspaceTags::Initialize()
{
	g_globalFileMap.InitHashTable(1023);

	for (int i = 0; i < (int)WWhizTag::LAST_TYPE; i++)
		s_showTypes[i] = true;
	s_showTypes[WWhizTag::DECLARATION] = false;
	s_showTypes[WWhizTag::FILE] = false;
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
		int tagLineNumber = tag->GetLineNumber();
		if (tagLineNumber <= (int)y)
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
	
static INLINE void MakeCopy(LPCTSTR& ptr, char*& outPtr, LPCTSTR slashStrings[3][MAX_SLASHES], int numSlashes[3],
								   bool doExactLen[3], bool doCase[3])
{
	LPCTSTR editText = ptr;
	int editTextLen = strlen(ptr);
	int lastThreePeriods[4] = { -1, -1, 0, editTextLen };

	// Find symbols.
	while (*ptr)
	{
		// Check for special characters.
		if (*ptr == '.')
		{
			lastThreePeriods[0] = lastThreePeriods[1];
			lastThreePeriods[1] = lastThreePeriods[2];
			lastThreePeriods[2] = (ptr - editText) + 1;
		}

		ptr++;
	}

	numSlashes[0] = numSlashes[1] = numSlashes[2] = 0;

	int whichOutPtr = 2;
	if (lastThreePeriods[0] != -1)
		whichOutPtr = 0;
	else if (lastThreePeriods[1] != -1)
		whichOutPtr = 1;

	ptr = editText;

	// Find symbols.
	LPCTSTR startPtr = ptr;
	while (*ptr)
	{
		// Check for special characters.
		if (*ptr == '.')
		{
			if (ptr - editText + 1 == lastThreePeriods[whichOutPtr + 1])
			{
				whichOutPtr++;
			}
		}
		if (*ptr == '-')
		{
			doCase[whichOutPtr] = false;
		}
		else if (*ptr == '+')
		{
			doCase[whichOutPtr] = true;
		}
		else if (*ptr == '\\')
		{
			doExactLen[whichOutPtr] = true;
		}
		else if (*ptr == '=')
		{
			doExactLen[whichOutPtr] = true;
			doCase[whichOutPtr] = true;
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
			doCase[whichOutPtr] = true;
		}

		ptr++;
	}

	ptr = editText;

//	char* outPtrs[] = { NULL, NULL, NULL };
	int maxOutPtrs = 0;

	*outPtr = 0;
	slashStrings[0][0] = outPtr;
	slashStrings[1][0] = outPtr;
	slashStrings[2][0] = outPtr;
	outPtr++;

//	CString idents[3];
	char* identPtr = NULL;
	if (lastThreePeriods[2] != -1)
	{
//		identPtr = idents[2].GetBufferSetLength(editText.GetLength() - lastThreePeriods[2]);
//		outPtrs[2] = identPtr;
		whichOutPtr = 2;
	}

	char* parentPtr = NULL;
	if (lastThreePeriods[1] != -1)
	{
//		parentPtr = idents[1].GetBufferSetLength(lastThreePeriods[2] - lastThreePeriods[1] - 1);
//		outPtrs[1] = parentPtr;
		whichOutPtr = 1;
	}

	char* namespacePtr = NULL;
	if (lastThreePeriods[0] != -1)
	{
//		namespacePtr = idents[0].GetBufferSetLength(lastThreePeriods[1] - lastThreePeriods[0] - 1);
//		outPtrs[0] = namespacePtr;
		whichOutPtr = 0;
	}

	bool copySymbols = (doCase[whichOutPtr] | doExactLen[whichOutPtr]);

//	char* outPtr = outPtrs[whichOutPtr];
	slashStrings[whichOutPtr][0] = outPtr;

	// Make the copy.
	while (*ptr)
	{
		// Is it a character we care about?
		if (*ptr == '.')
		{
			if (ptr - editText + 1 == lastThreePeriods[whichOutPtr + 1])
			{
//				idents[whichOutPtr].ReleaseBuffer();
				whichOutPtr++;
				*outPtr++ = 0;
//				outPtr = outPtrs[whichOutPtr];
				slashStrings[whichOutPtr][0] = outPtr;
				copySymbols = (doCase[whichOutPtr] | doExactLen[whichOutPtr]);
			}
		}
		else if (*ptr == '\'')
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
				*outPtr++ = (char)myToLower(*ptr);
		}
		else if (copySymbols  &&  (*ptr == '_'  ||  *ptr == '~'  ||  *ptr == ':'))
		{
			*outPtr++ = *ptr;
		}
		else if (*ptr == '/'  ||  *ptr == '*')
		{
			*outPtr++ = 0;
			if (numSlashes[whichOutPtr] == MAX_SLASHES)
				return;
			slashStrings[whichOutPtr][1 + numSlashes[whichOutPtr]++] = outPtr;
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
		if (myToLower(searchName[0]) < myToLower(checkName[0])  &&
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
		if (myToLower(searchName[0]) < myToLower(checkName[0])  &&
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

        while ( *s1 && *s2 && !(myToLower(*s1)-myToLower(*s2)) )
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
	for (int i = 1; i <= numSubStrings; ++i)
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
	\internal
	\author Jack Handy

	Borrowed from http://www.codeproject.com/string/wildcmp.asp.
	Modified by Joshua Jensen.
**/
static bool WildMatch( const char* pattern, const char *string, bool caseSensitive )
{
	// Handle all the letters of the pattern and the string.
	while ( *string != 0  &&  *pattern != '*' )
	{
		if ( *pattern != '?' )
		{
			if ( caseSensitive )
			{
				if ( *pattern != *string )
					return false;
			}
			else
			{
				if ( toupper( *pattern ) != toupper( *string ) )
					return false;
			}
		}

		pattern++;
		string++;
	}

	const char* mp = NULL;
	const char* cp = NULL;
	while ( *string != 0 )
	{
		if (*pattern == '*')
		{
			// It's a match if the wildcard is at the end.
			if ( *++pattern == 0 )
			{
				return true;
			}

			mp = pattern;
			cp = string + 1;
		}
		else
		{
			if ( caseSensitive )
			{
				if ( *pattern == *string  ||  *pattern == '?' )
				{
					pattern++;
					string++;
				}
				else 
				{
					pattern = mp;
					string = cp++;
				}
			}
			else
			{
				if ( toupper( *pattern ) == toupper( *string )  ||  *pattern == '?' )
				{
					pattern++;
					string++;
				}
				else
				{
					pattern = mp;
					string = cp++;
				}
			}
		}
	}

	// Collapse remaining wildcards.
	while ( *pattern == '*' )
		pattern++;

	return !*pattern;
}


/**
**/
void WorkspaceTags::MatchTag(const TagList& tags, LPCTSTR tagSourceName, WWhizTag::Type forceType) 
{
	CTimer matchTimer;
	matchTimer.Start();

	// Set the linked list to NULL.
	s_matchHead = s_matchTail = NULL;
	s_matchCount = 0;

	// Knock out the last matched tag.
	if (g_wwhizReg)
		g_wwhizReg->GotoTag(NULL);

	enum
	{
		NAMESPACE,
		PARENT,
		TAG,
	};
	LPCTSTR slashStrings[3][MAX_SLASHES];
	int slashLens[3][MAX_SLASHES];
	int numSlashes[3] = { 0, 0, 0 };
	bool doExactLen[3] = { false, false, false };
	bool doCaseSensitive[3] = { false, false, false };
	memset(slashStrings, 0, sizeof(slashStrings));
	memset(slashLens, 0, sizeof(slashLens));

	char tagNameBuf[2048];

	// Is there a second semicolon?  Chop off the directory path, if there is one.
	CString fixTag = tagSourceName;
	CString wildPath;
	const char* semicolon = strrchr(fixTag, ';');
	const char* semicolon2 = strchr(fixTag, ';');
	if (semicolon  &&  semicolon != semicolon2)
	{
		semicolon++;
		wildPath = CString("*\\") + semicolon + "*";
		wildPath.MakeLower();
		wildPath.Replace('/', '*');
		fixTag = fixTag.Left(semicolon - (const char*)fixTag - 1);
	}

	int semiPos = fixTag.Find(';');
	if (semiPos != -1)
		fixTag = fixTag.Mid(semiPos + 1) + "." + fixTag.Left(semiPos);

	tagSourceName = fixTag;

	LPCTSTR ptr = tagSourceName;
	char* outPtr = tagNameBuf;
//		tagName = outPtr;

	MakeCopy(ptr, outPtr, slashStrings, numSlashes, doExactLen, doCaseSensitive);

	slashLens[0][0] = strlen(slashStrings[0][0]);
	slashLens[1][0] = strlen(slashStrings[1][0]);
	slashLens[2][0] = strlen(slashStrings[2][0]);

	int i;
	for (i = 0; i < 3; ++i)
	{
		for (int j = 1; j <= numSlashes[i]; ++j)
		{
			slashLens[i][j] = strlen(slashStrings[i][j]);
		}
	}


/*	bool doCaseSensitiveTag = false;
	bool doCaseSensitiveParent = false;
	bool doExactTagLen = false;
	bool doExactParentLen = false;

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
			doCaseSensitiveTag);

	// Calculate the lengths.
	tagNameLen = strlen(tagName);
	int i;
	for (i = 0; i < numTagSlashStrings; ++i)
	{
		tagSlashLens[i] = strlen(tagSlashStrings[i]);
	}

	// Make a copy of the parent, removing symbols.
	CString parentNameTemp;
	parentName = outPtr;
	MakeCopy(ptr, outPtr, parentSlashStrings, numParentSlashStrings, doExactParentLen,
			doCaseSensitiveParent);
*/
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
	int parentNameLen = slashLens[PARENT][0];
	for (i = 1; i <= numSlashes[PARENT]; ++i)
	{
		allParentSlashLens += slashLens[PARENT][i];
	}

	// Calculate the lengths.
	int allNamespaceSlashLens = 0;
	int namespaceNameLen = slashLens[PARENT][0];
	for (i = 1; i <= numSlashes[PARENT]; ++i)
	{
		allNamespaceSlashLens += slashLens[PARENT][i];
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

		CString tagCheckIdent = (doCaseSensitive[TAG] | doExactLen[TAG]) ? tag->GetIdent() : tag->GetShortIdent();

		// The tags are alphabetical, so we can quit early.
		int tagNameLen = slashLens[TAG][0];
		const char* tagName = slashStrings[TAG][0];
		if (tagNameLen > 0)
		{
			int ret = CompareTag(tagName, tagNameLen, tagCheckIdent, 
					doExactLen[TAG], doCaseSensitive[TAG]);
			if (ret == 0)
				break;
			if (ret == 1)
				continue;
		}

		if (!CompareSubstring(tagCheckIdent, tagCheckIdent.GetLength(), tagNameLen,
				numSlashes[TAG], slashStrings[TAG], slashLens[TAG],
				doCaseSensitive[TAG]))
			continue;

		// Compare the parent name.
		if (slashLens[PARENT][0] > 0  ||  slashLens[PARENT][1] != 0  ||  doExactLen[PARENT])
		{
			CString parentCheckIdent = (doCaseSensitive[PARENT] | doExactLen[PARENT]) ? tag->GetParentIdent() : tag->GetShortParentIdent();
			if (parentCheckIdent.GetLength() > 0)
			{
				if (slashLens[PARENT][0] > 0  ||  doExactLen[PARENT])
				{
					int ret = CompareTag(slashStrings[PARENT][0], slashLens[PARENT][0], parentCheckIdent,
							doExactLen[PARENT], doCaseSensitive[PARENT]);
					if (ret == 0  ||  ret == 1)
						continue;
				}

				// Compare the substrings.
				if (!CompareSubstring(parentCheckIdent, parentCheckIdent.GetLength(), slashLens[PARENT][0],
						numSlashes[PARENT], slashStrings[PARENT], slashLens[PARENT],
						doCaseSensitive[PARENT]))
					continue;
			}
			else if (slashLens[PARENT][0] != 0  ||  allParentSlashLens != 0)
				continue;
		}

		// Compare the namespace name.
		if (slashLens[NAMESPACE][0] > 0  ||  slashLens[NAMESPACE][1] != 0  ||  doExactLen[NAMESPACE])
		{
			CString namespaceCheckIdent = (doCaseSensitive[NAMESPACE] | doExactLen[NAMESPACE]) ? tag->GetNamespace() : tag->GetShortNamespace();
			if (namespaceCheckIdent.GetLength() > 0)
			{
				if (slashLens[NAMESPACE][0] > 0  ||  doExactLen[NAMESPACE])
				{
					int ret = CompareTag(slashStrings[NAMESPACE][0], slashLens[NAMESPACE][0], namespaceCheckIdent,
						doExactLen[NAMESPACE], doCaseSensitive[NAMESPACE]);
					if (ret == 0  ||  ret == 1)
						continue;
				}

				// Compare the substrings.
				if (!CompareSubstring(namespaceCheckIdent, namespaceCheckIdent.GetLength(), slashLens[NAMESPACE][0],
					numSlashes[NAMESPACE], slashStrings[NAMESPACE], slashLens[NAMESPACE],
					doCaseSensitive[NAMESPACE]))
					continue;
			}
			else if (slashLens[NAMESPACE][0] != 0  ||  allNamespaceSlashLens != 0)
				continue;
		}

		// If the user specified a wild path, test for it.
		if (!wildPath.IsEmpty())
		{
			const char* fileCmpPath = tag->GetFile()->GetPath();
			if (!WildMatch(wildPath, fileCmpPath, false))
			{
				continue;
			}
		}

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

	matchTimer.Stop();
	ODS("Match: %0.2f\n", matchTimer.GetMillisecs());
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

CTimer tagCreateTimer;
CTimer parseTextTimer;
CTimer tagParseLoadTimer;

//
bool WorkspaceTags::ParseTextTags(BYTE* mem, DWORD memSize)
{
	BYTE* memEnd = mem + memSize;
	POSITION pos = s_filenameList.GetHeadPosition();
	bool earlyExit = false;
	while (pos)
	{
		MemFile memFile(10 * 1024);

		DWORD tagCount = 0;
		DWORD tagCountPos = memFile.GetPosition();
		memFile.Write(&tagCount, sizeof(DWORD));

		WArray<DWORD> tagOffsets;
		tagOffsets.SetCount(0, 10000);

		File* file = s_filenameList.GetNext(pos);
		const CString& fullPath = file->GetFullName();

		if (!CtagsFileCallback(fullPath))
			return true;

		s_iterFile->ClearTags();
//		s_iterFile->m_lastTagUpdateTime = s_currentTagUpdateTime;
//		s_iterFile->SetTimeStamp(s_iterFile->m_oldTimeStamp);

		// Read the file.
		while (true)
		{
	//		parseTextTimer.Start();

			if (memEnd - mem == NULL)
				goto Done;
			char* line = (char*)mem;
			mem = (BYTE*)memchr(line, '\r', memEnd - mem);
			if (!mem)
				goto Done;
			*mem = 0;
			mem++;
			if (*mem == '\n')
				mem++;
			int lineLen = mem - (BYTE*)line;

			// If there is nothing there, exit.
			if (lineLen == 0)
				continue;

			// If it's a tag comment line, skip it.
			if (_tcsncmp(line, _T("!_TAG_"), 6) == 0)
				continue;
	//		parseTextTimer.Stop();
			
	//		tagCreateTimer.Start();
			if (!stristr(line, fullPath))
			{
				*(mem - 2) = '\r';
				*(mem - 1) = '\n';
				mem = (BYTE*)line;
				break;
			}

			tagOffsets.Add(memFile.GetPosition());
			if (Tag::ConvertFromText(memFile, line))
			{
				tagCount++;
			}
			else
			{
				tagOffsets.RemoveAt( tagOffsets.GetCount() - 1 );
			}
	//		tagCreateTimer.Stop();
		}

Done:
	//	tagParseLoadTimer.Start();
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
		BYTE* newTagBuffer = WNEW BYTE[memFileSize];
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

	//	tagParseLoadTimer.Stop();
	}

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
	BOOL ret = CreateProcess(NULL, (LPTSTR)cmdLine, NULL, NULL, FALSE, CREATE_NO_WINDOW | DETACHED_PROCESS,
			NULL, NULL, &sinfo, &pinfo);
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
		CloseHandle(ChildOut);
		CloseHandle(ChildErr);
		CloseHandle(ChildIn);
    }
    else
	{
		// Error.
	}

    return 0;
}


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
			CString ext = tagsFilename.Mid(dotPos + 1);
			ext.MakeLower();
			if (ext == "dsw"  ||  ext == "sln")
			{
				continue;
			}
		}
		
		FileList& fileList = (FileList&)project->GetFileList();

		fileArrayTimer.Start();
		WArray<File*> fileArray;
		fileArray.SetCount(0, fileList.GetCount());

		// Add files to the file array.
		UINT fileListCount = fileList.GetCount();
		for (i = 0; i < fileListCount; i++)
		{
			File* file = (File*)fileList.Get(i);

/*			// Only do certain extensions.
			const CString& ext = file->GetExt();
			int numExts = config.TagsExtGetCount();
			int j;
			for (j = 0; j < numExts; j++)
				if (ext == config.TagsExtGet(j))
					break;
			if (j == numExts)
				continue;
*/
			fileArray.Add(file);
		}
		fileArrayTimer.Stop();

		if (dotPos != -1)
			tagsFilename = tagsFilename.Left(dotPos);
		tagsFilename += ".wwdb";

		DWORD fileCount = fileArray.GetCount();
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
			if (!drive.Create(tagsFilename))
			{
				continue;
			}
			createdDrive = true;
		}
		else if (!drive.Open(tagsFilename))
		{
			if (!drive.Create(tagsFilename))
			{
				continue;
			}
			createdDrive = true;
		}

		if (drive.IsReadOnly())
			continue;
		
		UINT numChangedTags = drive.GetHeaderUserDataDWORD(0);

		bool earlyExit = false;
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
				if (!(*s_tagRefreshCallback)(info))
					earlyExit = true;
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
				if (!(*s_tagRefreshCallback)(info))
					earlyExit = true;
			}
			
			diskSaveTimer.Start();

			drive.FileCopy(infoFile, file->GetFullName(), &file->GetTimeStamp());
			numChangedTags += tagCount;

			diskSaveTimer.Stop();

			if (earlyExit)
				break;
		}

		// Do we need to pack this drive?
		if (!createdDrive  &&  numChangedTags >= config.GetTagAutoPackAmount()  &&  !earlyExit)
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
	int i;
	for (i = 0; i < tagCount; i++)
	{
		Tag* tag = (Tag*)tagList.Get(i);
		tag->m_buddy = NULL;
	}

	for (i = 0; i < tagCount; i++)
	{
		Tag* tag = (Tag*)tagList.Get(i);
		Tag* closeMatchTag = NULL;
		if (tag->Tag::GetType() == WWhizTag::DECLARATION  &&  !tag->Tag::GetBuddy())
		{
			bool isTagDestructor = tag->Tag::GetIdent()[0] == '~';

			// Find the implementation.
			for (int j = i + 1; j < tagCount; j++)
			{
				Tag* implTag = (Tag*)tagList.Get(j);
				if (implTag->Tag::GetType() == WWhizTag::DECLARATION  ||  implTag->Tag::GetBuddy())
					continue;

				bool ret;
				int implLen = FastStrlen(implTag->Tag::GetIdent());
				int tagLen = FastStrlen(tag->Tag::GetIdent());

				if (implLen != tagLen)
					break;

				ret = FastCompareExactString(implTag->Tag::GetIdent(), tag->Tag::GetIdent(), implLen);
				if (!ret)
				{
					if (implTag->Tag::GetIdent()[0] == '~'  ||  isTagDestructor)
						continue;
					break;
				}

				implLen = FastStrlen(implTag->Tag::GetParentIdent());
				tagLen = FastStrlen(tag->Tag::GetParentIdent());
				if (implLen != tagLen)
					break;

				ret = FastCompareExactString(implTag->Tag::GetParentIdent(), tag->Tag::GetParentIdent(), implLen);
				if (!ret)
					break;

				implLen = FastStrlen(implTag->Tag::GetNamespace());
				tagLen = FastStrlen(tag->Tag::GetNamespace());
				if (implLen != tagLen)
					break;

				ret = FastCompareExactString(implTag->Tag::GetNamespace(), tag->Tag::GetNamespace(), implLen);
				if (!ret)
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

	CString tagsFilename = project->Project::GetName();
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
	tagsFilename += ".wwdb";

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

//		s_loadFindTimer.Start();
		FileList& fileList = (FileList&)project->GetFileList();
		File* file = (File*)fileList.Find(entry->GetFilename());
//		s_loadFindTimer.Stop();

		if (!file)
			continue;
		
		if (autoRefreshTags)
		{
			// Check the time stamp.
//			s_loadStatTimer.Start();
			struct stat fileStatus;
			if (stat(entry->GetFilename(), &fileStatus) == -1)
				continue;
			if (entry->GetTimeStamp() != fileStatus.st_mtime)
				continue;
//			s_loadStatTimer.Stop();
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
			file->m_tagBuffer = WNEW BYTE[vFileSize];
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
	ProjectList& projectList = WorkspaceInfo::GetProjectList();
	for (int i = 0; i < projectList.GetProjectCount(); ++i)
	{
		// Build the tag file list.
		Project* project = (Project*)projectList.GetProjectByIndex(i);
		if ((project->m_noRefresh  &&  project->m_tagList.GetCount() > 0)  ||  !project->m_active)
			continue;

		FileList& fileList = (FileList&)project->GetFileList();
		for (int j = 0; j < fileList.GetCount(); ++j)
		{
			// Write each file name.
			File* file = (File*)fileList.Get(j);

			// Pick up a couple aliases.
			AddChangedFile(file->GetFullName());
		}
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

	bool earlyExit = false;
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
/*				int numExts = config.TagsExtGetCount();
				int j;
				for (j = 0; j < numExts; j++)
				{
					if (ext == config.TagsExtGet(j))
						break;
				}
				if (j == numExts)
				{
					file->m_lastTagUpdateTime = s_currentTagUpdateTime;
					continue;
				}
*/
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
//				file->m_oldTimeStamp = fileStatus.m_mtime;
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

			// If there is extra configuration for ctags, read it in now.
			CFile confFile;
			if (confFile.Open(modulePath + CString("ctags.conf"), CFile::modeRead))
			{
				DWORD size = confFile.GetLength();
				char* buffer = new char[size + 2];
				confFile.Read(buffer, size);
				buffer[size] = '\n';
				buffer[size + 1] = 0;
				confFile.Close();

				cmdLine += buffer;
			}

			// Build the rest of the command line.
			int ignoreCount = g_wwhizInterface->GetConfig().IgnoreTokensGetCount();
			int i;
			for (i = 0; i < ignoreCount; i++)
			{
				const CString& token = g_wwhizInterface->GetConfig().IgnoreTokensGet(i);
				temp.Format("-I %s\n", token);
				cmdLine += temp;
			}
			cmdLine += "--c-types=+p\n--fields=+afikmns\n";
			cmdLine += "--file-tags=yes\n-h +.inl\n";
//			cmdLine += "--langmap=default\n";
			cmdLine += "--langmap=c++:+";
			int filesExtCount = g_wwhizInterface->GetConfig().TagsExtGetCount();
			for (i = 0; i < filesExtCount; i++)
			{
				const CString& ext = g_wwhizInterface->GetConfig().TagsExtGet(i);
				temp.Format(".%s", ext);
				cmdLine += temp;
			}
			cmdLine += "\n--excmd=pattern\n";
			cmdLine += "--sort=no\n";

			// The ctags path.
			CString ctagsPath = modulePath + CString("ctags.exe");
			if (_access(ctagsPath, 00) == -1)
			{
				AfxMessageBox("Unable to find ctags.exe");
				return;
			}
			
			// Generate a unique temporary name.
			char* asciiTempName = _tempnam(NULL, "WW300TAGRESP_");
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

			// Generate a unique temporary name.
			asciiTempName = _tempnam(NULL, "WW300TAGFILES_");
			CString asciiFilename2 = asciiTempName;
			free(asciiTempName);

			// Generate a unique temporary output name.
			asciiTempName = _tempnam(NULL, "WW300TAGS_");
			CString tagsFileName = asciiTempName;
			free(asciiTempName);

			f = fopen(asciiFilename2, "wt");
			if (!f)
			{
				AfxMessageBox("Unable to open ctags.exe");
				return;
			}

			File* file = NULL;
			POSITION pos = s_filenameList.GetHeadPosition();
			while (pos)
			{
				file = s_filenameList.GetNext(pos);
				const CString& fullPath = file->GetFullName();

				fprintf(f, "%s\n", fullPath);
			}
			fclose(f);

			cmdLine.Format("%s --options=\"%s\" -L \"%s\" -f \"%s\" --verbose=yes",
					(LPCTSTR)ctagsPath, asciiFilename, asciiFilename2, tagsFileName);

			CTimer shellTimer;
			shellTimer.Start();
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
			
			pos = s_filenameList.GetHeadPosition();

//errfile			errFile = fopen("c:\\wwhizerr.log", "wt");

			CString fullPath;
			const int BUF_SIZE = 1024;
			char bufferStack[BUF_SIZE];
			char* buffer = bufferStack;
			char* bufferEnd = buffer + BUF_SIZE;
			char* curBufPos = buffer;
			while (pos)
			{
				DWORD used;
				DWORD rc = ReadFile(hOut, curBufPos, bufferEnd - curBufPos, &used, NULL);
				if (rc != TRUE)
				{
					used = 0;
					break;
				}

				used += curBufPos - buffer;
				curBufPos = buffer;
				while (pos)
				{
					char* ptr = (char*)memchr(curBufPos, '\n', used - (curBufPos - buffer));
					if (ptr)
					{
						*ptr = 0;

						if (strncmp(curBufPos, "OPENING", 7) == 0)
						{
							file = s_filenameList.GetNext(pos);
							fullPath = file->GetFullName();

							if (!CtagsFileCallback(fullPath))
								earlyExit = true;
						}
						else if (strncmp(curBufPos, "ignoring", 8) == 0)
						{
							file = s_filenameList.GetNext(pos);
							fullPath = file->GetFullName();
						}

//errfile						fprintf(errFile, "%s\t\t%s\n", fullPath, curBufPos);
//errfile						fflush(errFile);

						curBufPos = ptr + 1;
					}
					else
					{
						DWORD leftSize = used - (curBufPos - buffer);
						memcpy(buffer, curBufPos, leftSize);
						curBufPos = buffer + leftSize;
						used = leftSize;
						break;
					}

					if (earlyExit)
						break;
				}

				if (earlyExit)
					break;
			}

			if (earlyExit)
			{
				// Terminate the process.  It sucks, but it is the best way to ensure Ctags goes away.
				BOOL exited = TerminateProcess(child, 0);  (void)exited;
			}

			WaitForSingleObject(child, INFINITE);

			CloseHandle(hIn);
			CloseHandle(hOut);
			CloseHandle(hErr);

			shellTimer.Stop();

			s_fcWhichFile = 0;

			MemFile memFile(10 * 1024);

			tagWriteTimer.Reset();
			partsBreakdownTimer.Reset();
			tagCreateTimer.Reset();
			shortIdentFixupTimer.Reset();
			extensionFlagsTimer.Reset();
			parseTextTimer.Reset();
			searchFixupTimer.Reset();
			tagParseLoadTimer.Reset();

			CFile tagsFile;
			CFileException fileException;
			tagsFile.Open(tagsFileName, CFile::modeRead, &fileException);
			int bufLen = tagsFile.GetLength();
			BYTE* bufferOrig = WNEW BYTE[bufLen + 2];
			tagsFile.Read(bufferOrig, bufLen);
			bufferOrig[bufLen] = '\r';
			bufferOrig[bufLen + 1] = '\n';
			tagsFile.Close();

			if (bufLen > 0  &&  !earlyExit)
			{
				ParseTextTags(bufferOrig, bufLen + 2);
			}
			
			delete [] bufferOrig;

			parseTimer.Stop();

//errfile			fclose(errFile);

			_unlink(asciiFilename);
			_unlink(asciiFilename2);
			_unlink(tagsFileName);
			
			ODS("Shell Time: %0.2f  Total Parse Time: %0.2f  Parse Text: %0.2f  Reading time: %0.2f  Create time: %0.2f\n"
				"Parts: %0.2f  SearchFixup: %0.2f\n"
				"Short Ident Fixup: %0.2f  Extension Flags: %0.2f  Write: %0.2f  Load: %0.2f\n",
				shellTimer.GetMillisecs(), parseTimer.GetMillisecs(), parseTextTimer.GetMillisecs(),
				tagsReadingTimer.GetMillisecs(), tagCreateTimer.GetMillisecs(),
				partsBreakdownTimer.GetMillisecs(), searchFixupTimer.GetMillisecs(),
				shortIdentFixupTimer.GetMillisecs(), extensionFlagsTimer.GetMillisecs(),
				tagWriteTimer.GetMillisecs(), tagParseLoadTimer.GetMillisecs());

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
		int i;
		for (i = 0; i < fileList.GetCount(); i++)
		{
			File& file = *(File*)fileList.Get(i);
			totalTagCount += file.GetTagList().GetCount();
		}

		s_tags.RemoveAll();
		s_tags.SetSize(totalTagCount);

		CTimer addTimer;
		addTimer.Start();

		int fileListCount = fileList.GetCount();
		for (i = 0; i < fileListCount; ++i)
		{
			File* file = (File*)fileList.Get(i);
			TagList& tagList = (TagList&)file->GetTagList();
			int tagListCount = tagList.GetCount();

			for (int j = 0; j < tagListCount; ++j)
			{
				Tag* tag = (Tag*)tagList.TagList::Get(j);
				s_tags.TagList::Add(tag);
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

	if (refreshTags  &&  !earlyExit)
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

	if (!earlyExit)
		s_filenameList.RemoveAll();
	s_tagRefreshCallback = NULL;

	g_filesRefreshed = false;

	if (g_tagsRefreshed)
		g_lastTagRefresh = CTime::GetCurrentTime();
}
