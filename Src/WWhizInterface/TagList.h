///////////////////////////////////////////////////////////////////////////////
// $Workfile: TagList.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/TagList.h $
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

class WorkspaceTags;
class File;

const DWORD TAG_TYPE_OFFSET = 0;
const DWORD TAG_LINENUMBER_OFFSET = 1;
const DWORD TAG_ACCESS_OFFSET = 5;
const DWORD TAG_IMPLEMENTATION_OFFSET = 6;
const DWORD TAG_FLAG_OFFSET = 7;
const DWORD TAG_SHORTIDENTLEN_OFFSET = 8;
const DWORD TAG_IDENTLEN_OFFSET = 9;
const DWORD TAG_SEARCHSTRINGLEN_OFFSET = 10;
const DWORD TAG_SHORTPARENTIDENTLEN_OFFSET = 12;
const DWORD TAG_PARENTIDENTLEN_OFFSET = 13;
const DWORD TAG_NAMESPACELEN_OFFSET = 14;
const DWORD TAG_SHORTIDENT_OFFSET = 15;

const BYTE TAGFLAG_SHORTIDENT		= 0x01;
const BYTE TAGFLAG_IDENT			= 0x02;
const BYTE TAGFLAG_SEARCHSTRING		= 0x04;
const BYTE TAGFLAG_SHORTPARENTIDENT = 0x08;
const BYTE TAGFLAG_PARENTIDENT		= 0x10;
const BYTE TAGFLAG_NAMESPACE		= 0x20;

	
class Tag : public WWhizTag
{
	friend class WWhizInterfaceImpl;
	friend class WorkspaceTags;

public:
	Tag();

	virtual Type GetType(void) const;
	virtual LPCSTR GetFilename(void) const;
	virtual LPCSTR GetShortIdent(void) const;
	virtual LPCSTR GetIdent(void) const;
	virtual LPCSTR GetSearchString(void) const;
	virtual LPCSTR GetShortParentIdent(void) const;
	virtual LPCSTR GetParentIdent(void) const;
	virtual LPCSTR GetNamespace(void) const;

	virtual int GetLineNumber(void) const;

	virtual WWhizTag* GetMatchNext(void) const;
	virtual WWhizTag* GetMatchPrev(void) const;

	virtual Access GetAccess(void) const;
	virtual WWhizTag* GetBuddy(void) const;
	virtual ImplementationType GetImplementationType(void) const;

	static bool ConvertFromText(CFile& file, TCHAR* strLine);

protected:
	Tag(const Tag& src);				// Hidden
	Tag& operator=(const Tag& src);		// Hidden

public: //Hack.
	File* m_parent;
	BYTE* m_tagBuffer;

//	WWhizTag::Type m_type;
//	WWhizTag::Access m_access;
	LPCSTR m_shortIdent;
	LPCSTR m_ident;
	LPCSTR m_searchString;
	LPCSTR m_shortParentIdent;
	LPCSTR m_parentIdent;
//	int m_lineNumber;
	LPCSTR m_namespace;
//	ImplementationType m_implementationType;

	Tag* m_buddy;

	Tag* m_matchNext;
	Tag* m_matchPrev;
};


/**
**/
class TagList : public WWhizTagList
{
public:
	TagList();
	~TagList();

// Attributes
	virtual int GetCount(void) const;
	virtual WWhizTag* Get(int index) const;

// Operations	
	void Add(Tag* tag);
	void Add(Tag* tag, int index);
	void RemoveAll(void);
	virtual void SortByTag_Parent_Namespace(void);
	virtual void SortByLineNumber(void);

	void SetSize(int size);

protected:
	WArray<Tag*> m_tags;
	bool m_changed;
};


inline WWhizTag::Type Tag::GetType(void) const
{
//	return m_type;
	return (WWhizTag::Type)m_tagBuffer[TAG_TYPE_OFFSET];
}


inline WWhizTag::Access Tag::GetAccess(void) const
{
//	return m_access;
	return (WWhizTag::Access)m_tagBuffer[TAG_ACCESS_OFFSET];
}


inline LPCSTR Tag::GetShortIdent(void) const
{
	return m_shortIdent;
}


inline LPCSTR Tag::GetIdent(void) const
{
	return m_ident;
}


inline LPCSTR Tag::GetSearchString(void) const
{
	return m_searchString;
}


inline LPCSTR Tag::GetShortParentIdent(void) const
{
	return m_shortParentIdent;
}


inline LPCSTR Tag::GetParentIdent(void) const
{
	return m_parentIdent;
}


inline LPCSTR Tag::GetNamespace(void) const
{
	return m_namespace;
}


inline WWhizTag::ImplementationType Tag::GetImplementationType(void) const
{
//	return m_implementationType;
	return (WWhizTag::ImplementationType)m_tagBuffer[TAG_IMPLEMENTATION_OFFSET];
}

	
inline WWhizTag* Tag::GetBuddy(void) const
{
	return m_buddy;
}


inline int Tag::GetLineNumber(void) const
{
//	return m_lineNumber;
	return *(int*)&m_tagBuffer[TAG_LINENUMBER_OFFSET];
}


inline WWhizTag* Tag::GetMatchNext(void) const
{
	return m_matchNext;
}


inline WWhizTag* Tag::GetMatchPrev(void) const
{
	return m_matchPrev;
}


/**
**/
inline int TagList::GetCount(void) const
{
	return m_tags.GetCount();
}


/**
**/
inline WWhizTag* TagList::Get(int index) const
{
	return m_tags[index];
}


/**
	Add file to the end of the file list.
**/
inline void TagList::Add(Tag* tag)
{
	m_tags.Add(tag);
	m_changed = true;
}

	
