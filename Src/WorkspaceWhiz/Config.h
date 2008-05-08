///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/Config.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 14   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

class Config
{
public:	
	class Keys
	{
	public:
		CString m_keyStr;
		bool m_assignKey;
	};

	CArray<Keys, Keys&> m_keys;

	bool m_dsFirstTime;
	bool m_useFindTagAtCursorDialog;
	bool m_autoSaveAllPerTagCommand;

public:
	Config();

	void LoadRegistry();
	void SaveRegistry();

	void SaveRegistry_FirstTime();
};


