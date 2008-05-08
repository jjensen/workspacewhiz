///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizInterface3Loader.h $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizInterface3Loader.h $
// $Date: 2003/05/01 $ $Revision: #5 $ $Author: jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <afx.h>

class WWhizInterface;
struct WWhizReg;

#ifdef WWHIZ_VC6
struct IApplication;
typedef IApplication* MyApplicationType;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
//typedef EnvDTE::_DTE* MyApplicationType;
typedef void* MyApplicationType;
#endif WWHIZ_VSNET

/**
	Loads the Workspace Whiz Interface library.
**/
WWhizInterface* __cdecl WWhizInterfaceCreate(HINSTANCE hInstance, MyApplicationType application);

/**
	Destroys a loaded copy of the Workspace Whiz Interface library.
**/
void __cdecl WWhizInterfaceDestroy();
