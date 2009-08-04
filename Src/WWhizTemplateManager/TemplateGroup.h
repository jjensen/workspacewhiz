///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateGroup.h $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/TemplateGroup.h $
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

class Template;

/**
**/
class TemplateGroup : public WWhizTemplateGroup
{
public:
	virtual int GetCount();
	virtual WWhizTemplate* Get(int index);
	virtual int Find(LPCTSTR name);

	virtual bool Refresh();
	virtual const CString& GetName() const;

	virtual const CString& GetFilename() const;
	virtual const CTime& GetTimeStamp() const;

	virtual bool IsActive() const;
	virtual void SetActive(bool active);

public:
	friend class TemplateManager;
	friend class Template;

	TemplateGroup(TemplateManager& manager, CString filename);
	~TemplateGroup();

protected:
	TemplateManager& m_manager;
	CString m_name;
	CString m_filename;
	CTime m_timeStamp;		// The time stamp of the code template file.
	bool m_active;

	WArray<Template*> m_templates;

private:
};


/**
**/
inline const CString& TemplateGroup::GetName() const
{
	return m_name;
}


/**
**/
inline bool TemplateGroup::IsActive() const
{
	return m_active;
}


/**
**/
inline void TemplateGroup::SetActive(bool active)
{
	m_active = active;
}

	
/**
**/
inline const CString& TemplateGroup::GetFilename() const
{
	return m_filename;
}


/**
**/
inline const CTime& TemplateGroup::GetTimeStamp() const
{
	return m_timeStamp;
}


/**
**/
inline int TemplateGroup::GetCount()
{
	return m_templates.GetCount();
}


