///////////////////////////////////////////////////////////////////////////////
// $Workfile: WorkspaceCommands.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/WorkspaceCommands.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 8    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef WORKSPACECOMMANDS_H
#define WORKSPACECOMMANDS_H

class Template;

class WorkspaceCommands
{
public:
	static int PutTemplate(WWhizTemplate& code);
	static int HeaderFlipDlg(CString fullName);
};

#endif //WORKSPACECOMMANDS_H
