///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefHeaderFlipListBox.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefHeaderFlipListBox.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 5    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "workspacewhiz.h"
#include "PrefHeaderFlipListBox.h"
#include "PrefHeaderFlipPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefHeaderFlipListBox

CPrefHeaderFlipListBox::CPrefHeaderFlipListBox()
{
}

CPrefHeaderFlipListBox::~CPrefHeaderFlipListBox()
{
}


BEGIN_MESSAGE_MAP(CPrefHeaderFlipListBox, CDragListBox)
	//{{AFX_MSG_MAP(CPrefHeaderFlipListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefHeaderFlipListBox message handlers

void CPrefHeaderFlipListBox::Dropped( int nSrcIndex, CPoint pt )
{
	CDragListBox::Dropped(nSrcIndex, pt);

	((CPrefHeaderFlipPage*)GetParent())->SetModified(TRUE);
}
