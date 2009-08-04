///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefHeaderFlipListBox.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefHeaderFlipListBox.cpp $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "PrefHeaderFlipListBox.h"
#include "PrefHeaderFlipPage.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
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
