///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefTagsListBox.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefTagsListBox.cpp $
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
#include "PrefTagsListBox.h"
#include "PrefTagsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefTagsListBox

CPrefTagsListBox::CPrefTagsListBox()
{
}

CPrefTagsListBox::~CPrefTagsListBox()
{
}


BEGIN_MESSAGE_MAP(CPrefTagsListBox, CDragListBox)
	//{{AFX_MSG_MAP(CPrefTagsListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefTagsListBox message handlers

void CPrefTagsListBox::Dropped( int nSrcIndex, CPoint pt )
{
	CDragListBox::Dropped(nSrcIndex, pt);

	((CPrefTagsPage*)GetParent())->SetModified(TRUE);
}
