///////////////////////////////////////////////////////////////////////////////
// $Workfile: TagList.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/TagList.cpp $
// $Date: 2003/01/05 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2006 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizInterface.h"
#include "TagList.h"
#include "FileList.h"
//#include "../sqlite/sqlite3.h"

#ifdef WWHIZ_VSNET
//#define USE_STL
#endif WWHIZ_VSNET
#ifdef USE_STL
#include <algorithm>
#endif USE_STL

/**
**/
Tag::Tag() :
	m_buddy(NULL)
{
}


///////////////////////////////////////////////////////////////////////////////
// TagList
///////////////////////////////////////////////////////////////////////////////

TagList::TagList() :
	m_changed(false)
{
	m_tags.SetCount(0, 100);
}


TagList::~TagList()
{
	RemoveAll();
}


/**
	Remove all files from the container.
**/
void TagList::RemoveAll()
{
	m_tags.RemoveAll();
	m_tags.SetCount(0, 100);

	m_changed = true;
}


static int __cdecl CompareArray(const void* elem1, const void* elem2)
{
	Tag* tag1 = *(Tag**)elem1;
	Tag* tag2 = *(Tag**)elem2;

	int ret = strcmp(tag1->Tag::GetShortIdent(), tag2->Tag::GetShortIdent());
	if (ret != 0)
		return ret;

	// If the names match, compare against the parent identifiers.
	ret = strcmp(tag1->Tag::GetShortParentIdent(), tag2->Tag::GetShortParentIdent());
	if (ret != 0)
		return ret;

	// If the names match, compare against the namespaces.  Blank namespaces should
	// sort last.
	bool tag1NamespaceEmpty = tag1->m_shortNamespace[0] == 0;
	bool tag2NamespaceEmpty = tag2->m_shortNamespace[0] == 0;
	if (tag1NamespaceEmpty  &&  !tag2NamespaceEmpty)
		return 1;

	if (tag2NamespaceEmpty)
		ret = 0;
	else
		ret = strcmp(tag1->Tag::GetShortNamespace(), tag2->Tag::GetShortNamespace());
	if (ret == 0)
	{
		WWhizTag::Type tag1Type = tag1->Tag::GetType();
		WWhizTag::Type tag2Type = tag2->Tag::GetType();
		if (tag1Type != WWhizTag::DECLARATION  &&  tag2Type == WWhizTag::DECLARATION)
		{
			return 1;
		}
		else if (tag1Type == WWhizTag::DECLARATION  &&  tag2Type == WWhizTag::DECLARATION)
		{
			return 0;
		}

		return -1;
	}

	return ret;
}
	

#ifdef USE_STL
struct CompareTags
{
    __forceinline bool operator()(Tag* tag1, Tag* tag2)
    {
		int ret = strcmp(tag1->Tag::GetShortIdent(), tag2->Tag::GetShortIdent());
		if (ret != 0)
			return ret < 0;

		// If the names match, compare against the parent identifiers.
		ret = strcmp(tag1->Tag::GetShortParentIdent(), tag2->Tag::GetShortParentIdent());
		if (ret != 0)
			return ret < 0;

		// If the names match, compare against the namespaces.  Blank namespaces should
		// sort last.
		bool tag1NamespaceEmpty = tag1->m_shortNamespace[0] == 0;
		bool tag2NamespaceEmpty = tag2->m_shortNamespace[0] == 0;
		if (tag1NamespaceEmpty  &&  !tag2NamespaceEmpty)
			return false;

		if (tag2NamespaceEmpty)
			ret = 0;
		else
			ret = strcmp(tag1->Tag::GetShortNamespace(), tag2->Tag::GetShortNamespace());
		if (ret == 0)
		{
			WWhizTag::Type tag1Type = tag1->Tag::GetType();
			WWhizTag::Type tag2Type = tag2->Tag::GetType();
			if (tag1Type != WWhizTag::DECLARATION  &&  tag2Type == WWhizTag::DECLARATION)
			{
				return false;		//ret = 1;
			}
			else if (tag1Type == WWhizTag::DECLARATION  &&  tag2Type == WWhizTag::DECLARATION)
			{
				return false;		//ret = 0;
			}

			// Last resort.
			ret = strcmp(tag1->Tag::GetSearchString(), tag2->Tag::GetSearchString());
			return ret < 0;

//			return true;		//ret = -1;
		}

		return ret < 0;
	 }
};
#endif USE_STL

void TagList::SortByTag_Parent_Namespace()
{
	// Sort the file array.
#ifndef USE_STL
	qsort(m_tags.GetData(), m_tags.GetCount(), sizeof(Tag*), CompareArray);
#else
	std::sort(m_tags.GetData(), m_tags.GetData() + m_tags.GetCount(), CompareTags());
#endif USE_STL
}


/**
**/
static int __cdecl CompareByLineNumbers(const void* elem1, const void* elem2)
{
	Tag* tag1 = *(Tag**)elem1;
	Tag* tag2 = *(Tag**)elem2;

	if (tag1->GetLineNumber() < tag2->GetLineNumber())
		return -1;
	else if (tag1->GetLineNumber() == tag2->GetLineNumber())
		return 0;
	else
		return 1;
}
	

/**
**/
void TagList::SortByLineNumber()
{
	// Sort the file array.
	qsort(m_tags.GetData(), m_tags.GetCount(), sizeof(Tag*), CompareByLineNumbers);
}



void TagList::Add(Tag* tag, int index)
{
	m_tags.InsertAt(index, tag);
	m_changed = true;
}

	
void TagList::SetSize(int size)
{
	m_tags.SetCount(0, size);
}



