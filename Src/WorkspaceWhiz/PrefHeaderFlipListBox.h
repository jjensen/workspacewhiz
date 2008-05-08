///////////////////////////////////////////////////////////////////////////////
// $Workfile: PrefHeaderFlipListBox.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/PrefHeaderFlipListBox.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 6    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PREFHEADERFLIPLISTBOX_H__36B38224_8A5A_11D3_9B27_444553540000__INCLUDED_)
#define AFX_PREFHEADERFLIPLISTBOX_H__36B38224_8A5A_11D3_9B27_444553540000__INCLUDED_

#pragma once
// PrefHeaderFlipListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefHeaderFlipListBox window

class CPrefHeaderFlipListBox : public CDragListBox
{
// Construction
public:
	CPrefHeaderFlipListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefHeaderFlipListBox)
	//}}AFX_VIRTUAL
	virtual void Dropped( int nSrcIndex, CPoint pt );

// Implementation
public:
	virtual ~CPrefHeaderFlipListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrefHeaderFlipListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFHEADERFLIPLISTBOX_H__36B38224_8A5A_11D3_9B27_444553540000__INCLUDED_)
