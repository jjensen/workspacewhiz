///////////////////////////////////////////////////////////////////////////////
// $Workfile: RegHelperWnd.cpp $
// $Archive: /WWhizReg/RegHelperWnd.cpp $
// $Date:: 4/12/01 8:19p   $ $Revision:: 7    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizReg.h"
#include "RegHelperWnd.h"
#include "WWhizReg.h"
#include "WWhizRegExt.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegHelperWnd

CRegHelperWnd::CRegHelperWnd()
{
}

CRegHelperWnd::~CRegHelperWnd()
{
}


BEGIN_MESSAGE_MAP(CRegHelperWnd, CWnd)
	//{{AFX_MSG_MAP(CRegHelperWnd)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_DOREG, OnDoRegSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegHelperWnd message handlers

extern void BuildWWhizReg(WWhizReg* reg);

LRESULT CRegHelperWnd::OnDoRegSetup(WPARAM wParam, LPARAM lParam)
{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (wParam == 0x1234)
    {
    	BuildWWhizReg((WWhizReg*)lParam);
//		g_wwhizInterface->LoadTags();
		DestroyWindow();
    }
	else if (wParam == 0x1235)
	{
		WWhizReg* reg = (WWhizReg*)lParam;

		reg->RegistrationDialog		= RegistrationDialog;
		reg->CreateWFOControls		= CreateWFOControls;
		reg->DestroyWFOControls		= DestroyWFOControls;
		reg->WFOAddEditString		= WFOAddEditString;
		reg->CreateTagControls		= CreateTagControls;
		reg->DestroyTagControls		= DestroyTagControls;
		reg->TagAddEditTagString	= TagAddEditTagString;
		reg->TagAddEditParentString = TagAddEditParentString;
		reg->ReadExtraFiles			= ReadExtraFiles;
		reg->WriteExtraFiles		= WriteExtraFiles;
		reg->GetExtraFiles			= GetExtraFiles;
		reg->SetExtraFiles			= SetExtraFiles;
		reg->GetLastMatchedTag		= GetLastMatchedTag;
		reg->GotoTag				= GotoTag;
	}

    return Default();
}


