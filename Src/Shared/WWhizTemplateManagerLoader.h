///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizTemplateManagerLoader.h $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizTemplateManagerLoader.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 4    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

/**
	Loads the Workspace Whiz! template interface library.
*/
WWhizTemplateManager* __cdecl WWhizTemplateManagerCreate(
		IApplication* pApplication, WWhizTemplateDialog* templateDialog);

/**
	Destroys a loaded copy of the Workspace Whiz! template interface library.
**/
void __cdecl WWhizTemplateManagerDestroy();

