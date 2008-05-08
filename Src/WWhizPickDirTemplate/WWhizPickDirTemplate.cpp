///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizPickDirTemplate.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizPickDirTemplate/WWhizPickDirTemplate.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 3    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "PickDirDlg.h"

class PickDirCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CPickDirDlg dlg(args.Get(0), args.Get(1));
		args.Return(dlg.GetDirectory());
	}
};


// Regular commands.
static PickDirCommand			s_pickDirCommand;

/////////////////////////////////////////////////////////////////////////////

struct IApplication;
extern "C" __declspec(dllexport)
bool WWhizTemplateRegister(WWhizTemplateManager* templateManager,
		IApplication* pApplication)
{
	templateManager->RegisterCommand("PickDir",		s_pickDirCommand);

	return true;
}


