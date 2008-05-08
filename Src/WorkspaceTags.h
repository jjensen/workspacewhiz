#ifndef WORKSPACETAGS
#define WORKSPACETAGS

#include "WorkspaceInfo.h"

class Status
{
public:
	static void Create(void);
	static void Print(LPCTSTR msg, ...);
	static void Destroy(bool modal = false);
	static void SysEvent(void);

protected:
	static int m_refCount;
	static CDialog* m_dlg;
};

class WorkspaceTags
{
public:
	class Tag
	{
		friend class WorkspaceTags;
	public:
		char GetType(void) const;
		const CString& GetShortIdent(void) const;
		const CString& GetIdent(void) const;
		const CString& GetFilename(void) const;
		const CString& GetSearchString(void) const;
		const CString& GetShortParentIdent(void) const;
		const CString& GetParentIdent(void) const;
		int GetLineNumber(void) const;

		Tag* GetMatchNext(void) const;
		Tag* GetMatchPrev(void) const;

	protected:
		char m_type;
		CString m_shortIdent;
		CString m_ident;
		CString m_filename;
		CString m_searchString;
		CString m_shortParentIdent;
		CString m_parentIdent;
		int m_lineNumber;

		Tag* m_matchNext;
		Tag* m_matchPrev;
	};

	typedef CList<Tag, Tag&> TagList;

public:
// Attributes
	static const Tag* GetMatchHead(void);
	static const Tag* GetMatchTail(void);
	static int GetMatchCount(void);
	static const Tag* GetLastMatchedTag(void);

	static const TagList& GetTagList(void);

// Operations
	static void Update(void);
	static void RemoveAll(void);

	static bool ReadTags(LPCTSTR filename);
	static void RefreshAll(void);

	static void MatchIdentifier(LPCTSTR name);

	static void GotoTag(const Tag* ident);

	static IApplication * m_pApplication;

protected:
	static bool BuildTagsFile(const WorkspaceInfo::Project& prj);

	static Tag* m_matchHead;
	static Tag* m_matchTail;
	static int m_matchCount;
	static Tag* m_lastMatchedTag;
	static CString m_lastMatchedName;

	static TagList m_tags;
}; //WorkspaceTags


inline char WorkspaceTags::Tag::GetType(void) const
{  return m_type;  }
inline const CString& WorkspaceTags::Tag::GetShortIdent(void) const
{  return m_shortIdent;  }
inline const CString& WorkspaceTags::Tag::GetIdent(void) const
{  return m_ident;  }
inline const CString& WorkspaceTags::Tag::GetFilename(void) const
{  return m_filename;  }
inline const CString& WorkspaceTags::Tag::GetSearchString(void) const
{  return m_searchString;  }
inline const CString& WorkspaceTags::Tag::GetShortParentIdent(void) const
{  return m_shortParentIdent;  }
inline const CString& WorkspaceTags::Tag::GetParentIdent(void) const
{  return m_parentIdent;  }
inline int WorkspaceTags::Tag::GetLineNumber(void) const
{  return m_type;  }
inline WorkspaceTags::Tag* WorkspaceTags::Tag::GetMatchNext(void) const
{  return m_matchNext;  }
inline WorkspaceTags::Tag* WorkspaceTags::Tag::GetMatchPrev(void) const
{  return m_matchPrev;  }


//
inline const WorkspaceTags::Tag* WorkspaceTags::GetMatchHead(void)
{  return m_matchHead;  }
inline const WorkspaceTags::Tag* WorkspaceTags::GetMatchTail(void)
{  return m_matchTail;  }
inline int WorkspaceTags::GetMatchCount(void)
{  return m_matchCount;  }
inline const WorkspaceTags::Tag* WorkspaceTags::GetLastMatchedTag(void)
{  return m_lastMatchedTag;  }
inline const WorkspaceTags::TagList& WorkspaceTags::GetTagList(void)
{  return m_tags;  }

#endif WORKSPACETAGS
