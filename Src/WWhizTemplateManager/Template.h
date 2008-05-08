///////////////////////////////////////////////////////////////////////////////
// $Workfile: Template.h $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/Template.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 9    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

class TemplateGroup;
class TemplateManager;

///////////////////////////////////////////////////////////////////////////////

class CTreeCtrlEx;

class TemplateHelper;

/**
**/
class Template : public WWhizTemplate
{
public:
	virtual WWhizTemplateGroup& GetParent() const;
	virtual const CString& GetName() const;
	virtual const CString& GetCode() const;
	virtual const CString& GetMemo() const;
	virtual const CString& GetPage(int page);
	virtual int GetPageCount() const;

	virtual bool Parse();
	virtual void FlushText();
	virtual void RefreshFileInfo();
	virtual void SetCodePtr(LPCTSTR codePtr);

	virtual WWhizTemplateCommandArgs* CreateArgs();
	virtual void DestroyArgs(WWhizTemplateCommandArgs* args);

	virtual void SetEntry(const CString& dicEntry, CString value);
	virtual CString GetEntry(const CString& entry) const;

	virtual void ResetDictionary();
	virtual void ResetDictionaryDefaults();

	virtual const CString& GetKey() const;

	virtual int GetLineNumber() const;

	virtual WWhizTemplateDialog* GetTemplateDialog() const;

	static CString ParseCode(LPCTSTR codePtr, LPCTSTR bolPtr, WWhizTemplate* file = NULL);

	void SkipToEol(LPCTSTR& codePtr, CharArray* buffer = NULL);

public:
	friend class TemplateGroup;
	friend class TemplateManager;

	Template(TemplateGroup& templateGroup);
	~Template();

protected:
	bool ParseHelper();

	CString m_name;
	CString m_code;
	CString m_memo;
	CArrayEx<CString, CString&> m_pages;
	CString m_key;

	// Template parameters.
	CMapStringToString m_dictionary;

	struct DefaultInfo
	{
		CString m_entry;
		CString m_value;
	};
	CListEx<DefaultInfo, DefaultInfo&> m_defaults;

	struct OffsetInfo
	{
		DWORD m_offset;
		DWORD m_line;
	};
	CArrayEx<OffsetInfo, OffsetInfo&> m_codeOffsets;

	TemplateGroup& m_parent;

	TemplateHelper* m_helper;

	int m_lineNumber;
};


/**
**/
inline WWhizTemplateGroup& Template::GetParent() const
{
	return (WWhizTemplateGroup&)m_parent;
}

	
/**
**/
inline const CString& Template::GetName() const
{
	return m_name;
}


/**
**/
inline const CString& Template::GetCode() const
{
	return m_code;
}


/**
**/
inline const CString& Template::GetMemo() const
{
	return m_memo;
}


/**
**/
inline const CString& Template::GetPage(int page)
{
	return m_pages[page];
}

	
/**
**/
inline int Template::GetPageCount() const
{
	return m_pages.GetSize();
}


/**
**/
inline CString Template::GetEntry(const CString& entry) const
{
	CString str;
	BOOL found = m_dictionary.Lookup(entry, str);
	if (found)
		return str;
	else
		return "";
}


/**
**/
inline const CString& Template::GetKey() const
{
	return m_key;
}


/**
**/
inline int Template::GetLineNumber() const
{
	return m_lineNumber;
}

	
class TokenHelper;
CString ParseToken(LPCTSTR& codePtr, TokenHelper* helper = NULL);

