///////////////////////////////////////////////////////////////////////////////
// $Workfile: Config.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/Config.h $
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

class Config
{
public:	
	class Keys
	{
	public:
		CString m_keyStr;
		bool m_assignKey;
	};

	WArray<Keys> m_keys;

	bool m_dsFirstTime;
	bool m_useFindTagAtCursorDialog;
	bool m_autoSaveAllPerTagCommand;

public:
	Config();

	void LoadRegistry();
	void SaveRegistry();

	void SaveRegistry_FirstTime();
};


