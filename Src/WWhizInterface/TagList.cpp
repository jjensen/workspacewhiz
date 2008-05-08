///////////////////////////////////////////////////////////////////////////////
// $Workfile: TagList.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/TagList.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 18   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "TagList.h"
#include "FileList.h"

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
	m_tags.SetSize(0, 100);
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
	m_tags.SetSize(0, 100);

	m_changed = true;
}


static int __cdecl CompareArray(const void* elem1, const void* elem2)
{
	Tag* tag1 = *(Tag**)elem1;
	Tag* tag2 = *(Tag**)elem2;

	int ret = _tcscmp(tag1->Tag::GetShortIdent(), tag2->Tag::GetShortIdent());
	if (ret != 0)
		return ret;

	// If the names match, compare against the parent identifiers.
	ret = _tcscmp(tag1->Tag::GetShortParentIdent(), tag2->Tag::GetShortParentIdent());
	if (ret != 0)
		return ret;

	// If the names match, compare against the namespaces.  Blank namespaces should
	// sort last.
	bool tag1NamespaceEmpty = tag1->m_namespace[0] == 0;
	bool tag2NamespaceEmpty = tag2->m_namespace[0] == 0;
	if (tag1NamespaceEmpty  &&  !tag2NamespaceEmpty)
		return 1;

	if (tag2NamespaceEmpty)
		ret = 0;
	else
		ret = _tcscmp(tag1->Tag::GetNamespace(), tag2->Tag::GetNamespace());
	if (ret == 0)
	{
		if (tag1->Tag::GetType() != WWhizTag::DECLARATION  &&  tag2->Tag::GetType() == WWhizTag::DECLARATION)
		{
			return 1;
		}
		else if (tag1->Tag::GetType() == WWhizTag::DECLARATION  &&  tag2->Tag::GetType() == WWhizTag::DECLARATION)
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
     __forceinline bool operator()(Tag*& tag1, Tag*& tag2)
     {

		int ret = _tcscmp(tag1->Tag::GetShortIdent(), tag2->Tag::GetShortIdent());
		if (ret == 0)
		{
			// If the names match, compare against the parent identifiers.
			ret = _tcscmp(tag1->Tag::GetShortParentIdent(), tag2->Tag::GetShortParentIdent());
			if (ret == 0)
			{
				// If the names match, compare against the namespaces.  Blank namespaces should
				// sort last.
				bool tag1NamespaceEmpty = tag1->m_namespace[0] == 0;
				bool tag2NamespaceEmpty = tag2->m_namespace[0] == 0;
				if (tag1NamespaceEmpty  &&  !tag2NamespaceEmpty)
					ret = 1;

				if (tag2NamespaceEmpty)
					ret = 0;
				else
					ret = _tcscmp(tag1->Tag::GetNamespace(), tag2->Tag::GetNamespace());
				if (ret == 0)
				{
					if (tag1->Tag::GetType() != WWhizTag::DECLARATION  &&  tag2->Tag::GetType() == WWhizTag::DECLARATION)
					{
						ret = 1;
					}
					else if (tag1->Tag::GetType() == WWhizTag::DECLARATION  &&  tag2->Tag::GetType() == WWhizTag::DECLARATION)
					{
						ret = 0;
					}
					else
					{
						ret = -1;
					}

						// If the extensions match, compare against the path.
			//					ret = _tcscmp(tag1->Tag::GetFilename(), tag2->Tag::GetFilename());
				}
			}
		}

		if (ret == -1)
			return true;
		return false;
	 }
};
#endif USE_STL

void TagList::SortByTag_Parent_Namespace()
{
	// Sort the file array.
#ifndef USE_STL
	qsort(m_tags.GetData(), m_tags.GetSize(), sizeof(Tag*), CompareArray);
#else
	std::sort(m_tags.GetData(), m_tags.GetData() + m_tags.GetSize(), CompareTags());
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
	qsort(m_tags.GetData(), m_tags.GetSize(), sizeof(Tag*), CompareByLineNumbers);
}



/**
	Add file to the end of the file list.
**/
void TagList::Add(Tag* tag)
{
	m_tags.Add(tag);
	m_changed = true;
}

	
void TagList::Add(Tag* tag, int index)
{
	m_tags.InsertAt(index, tag);
	m_changed = true;
}

	
void TagList::SetSize(int size)
{
	m_tags.SetSize(0, size);
}



