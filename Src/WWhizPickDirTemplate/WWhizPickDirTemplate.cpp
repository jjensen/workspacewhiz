///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizPickDirTemplate.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizPickDirTemplate/WWhizPickDirTemplate.cpp $
// $Date: 2003/01/30 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "PickDirDlg.h"

class PickDirCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* /*curTemplate*/, WWhizTemplateCommandArgs& args)
	{
		CPickDirDlg dlg(args.Get(0), args.Get(1));
		args.Return(dlg.GetDirectory());
	}
};


// Regular commands.
static PickDirCommand			s_pickDirCommand;

/////////////////////////////////////////////////////////////////////////////

class WWhizInterface;

extern "C" __declspec(dllexport)
#ifdef WWHIZ_VC6
bool WWhizTemplateRegister(WWhizTemplateManager* templateManager,
		IApplication* pApplication)
{
	g_pApplication = pApplication;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
bool WWhizTemplateRegister(WWhizInterface* /*wwhizInterface*/,
						   WWhizTemplateManager* templateManager,
						   void* pDTE)
{
	g_pDTE = (EnvDTE80::DTE2*)pDTE;
#endif WWHIZ_VSNET
	templateManager->RegisterCommand("PickDir",		s_pickDirCommand);

	return true;
}


extern "C" __declspec(dllexport)
void WWhizTemplateDestroy()
{
#ifdef WWHIZ_VSNET
	g_pDTE = NULL;
#endif WWHIZ_VSNET
}

						   
