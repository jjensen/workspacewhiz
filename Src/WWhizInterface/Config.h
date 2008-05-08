///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/Config.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 11   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

class WWhizConfigImpl : public WWhizConfig
{
public:
	WWhizConfigImpl();

	// Files extension list.
	virtual void FilesExtReset();
	virtual void FilesExtRemoveAll();
	virtual void FilesExtAdd(CString ext);
	virtual void FilesExtRemove(int index);
	virtual const CString& FilesExtGet(int index);
	virtual int FilesExtGetCount();

	// Tags extension list.
	virtual void TagsExtReset();
	virtual void TagsExtRemoveAll();
	virtual void TagsExtAdd(CString ext);
	virtual void TagsExtRemove(int index);
	virtual int TagsExtGetCount();
	virtual const CString& TagsExtGet(int index);

	// Misc.
	virtual UINT GetTagAutoPackAmount();
	virtual void SetTagAutoPackAmount(UINT amount);
	virtual bool GetAutoRefreshTags();
	virtual void SetAutoRefreshTags(bool autoRefresh);
	virtual bool GetScanFilesTaskSwitch();
	virtual void SetScanFilesTaskSwitch(bool scanFiles);

	///////////////////////////////////////////////////////////////////////////
	// Tokens to ignore.
	///////////////////////////////////////////////////////////////////////////
	virtual void IgnoreTokensReset();
	virtual void IgnoreTokensRemoveAll();
	virtual void IgnoreTokensAdd(CString ext);
	virtual void IgnoreTokensRemove(int index);
	virtual int IgnoreTokensGetCount();
	virtual const CString& IgnoreTokensGet(int index);

	virtual UINT GetTagAutoSaveAmount();
	virtual void SetTagAutoSaveAmount(UINT amount);

protected:
	CArray<CString, CString&> m_filesExtList;
	CArray<CString, CString&> m_tagsExtList;
	CArray<CString, CString&> m_ignoreTokensList;

	int m_tagAutoPackAmount;
	bool m_autoRefreshTags;
	bool m_scanFilesTaskSwitch;
	UINT m_tagAutoSaveAmount;
};

