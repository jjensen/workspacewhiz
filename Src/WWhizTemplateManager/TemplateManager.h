///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateManager.h $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/TemplateManager.h $
// $Date: 2003/01/05 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../3rdParty/WCollection.h"
#include "../3rdParty/MemFile.h"

/**
**/
class TemplateManager : public WWhizTemplateManager
{
public:
	static TemplateManager* GetInstance()
	{
		return s_instance;
	}

	virtual int Add(CString filename);
	virtual bool Remove(int index);
	virtual bool Refresh();

	virtual int GetCount();
	virtual WWhizTemplateGroup* GetTemplateGroup(int index);

	virtual void RegisterCommand(CString commandName, WWhizTemplateCommand& templateCommand);
	virtual WWhizTemplateCommand* GetCommand(CString name);

	virtual CFile& GetTreeStateFile();

	virtual void LoadRegistry();
	virtual void SaveRegistry();

	virtual bool RestoreTreeState(CTreeCtrlEx& tree);
	virtual void SaveTreeState(CTreeCtrlEx& tree);

	virtual CString ParseCode(LPCTSTR codePtr, LPCTSTR bolPtr, WWhizTemplate* file = NULL);

	virtual void SetTemplateDialog(WWhizTemplateDialog* templateDialog);

public:
	friend class TemplateGroup;
	friend class Template;

	TemplateManager();
	~TemplateManager();

	bool Create();

protected:
	bool LoadTplFile(const CString& name);

	bool m_changed;

	// List of templates.
	struct Info
	{
		Info() : m_found(false), m_file(NULL) {  }
		~Info();

		bool m_found;
		TemplateGroup* m_file;
	};
	WArray<Info*> m_files;
	WArray<HINSTANCE> m_loadedCommandModules;
	WMap<CString, WWhizTemplateCommand*> m_commands;

	MemFile m_treeStateFile;

	WWhizTemplateDialog* m_templateDialog;
	static TemplateManager* s_instance;
};


/**
**/
inline CFile& TemplateManager::GetTreeStateFile()
{
	return m_treeStateFile;
}


inline void TemplateManager::SetTemplateDialog(WWhizTemplateDialog* templateDialog)
{
	m_templateDialog = templateDialog;
}
