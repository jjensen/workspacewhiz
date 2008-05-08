///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/Config.h $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
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
	virtual void FilesExtRemove(size_t index);
	virtual const CString& FilesExtGet(size_t index);
	virtual size_t FilesExtGetCount();

	// Tags extension list.
	virtual void TagsExtReset();
	virtual void TagsExtRemoveAll();
	virtual void TagsExtAdd(CString ext);
	virtual void TagsExtRemove(size_t index);
	virtual size_t TagsExtGetCount();
	virtual const CString& TagsExtGet(size_t index);

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
	virtual void IgnoreTokensRemove(size_t index);
	virtual size_t IgnoreTokensGetCount();
	virtual const CString& IgnoreTokensGet(size_t index);

	virtual UINT GetTagAutoSaveAmount();
	virtual void SetTagAutoSaveAmount(UINT amount);

protected:
	WArray<CString> m_filesExtList;
	WArray<CString> m_tagsExtList;
	WArray<CString> m_ignoreTokensList;

	int m_tagAutoPackAmount;
	bool m_autoRefreshTags;
	bool m_scanFilesTaskSwitch;
	UINT m_tagAutoSaveAmount;
};

