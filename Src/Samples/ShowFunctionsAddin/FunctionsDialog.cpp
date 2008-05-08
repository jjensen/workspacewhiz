///////////////////////////////////////////////////////////////////////////////
// $Workfile: FunctionsDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowFunctionsAddin/FunctionsDialog.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 2    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ShowFunctionsAddin.h"
#include "FunctionsDialog.h"
#include "ObjModelHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFunctionsDialog dialog


CFunctionsDialog::CFunctionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFunctionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFunctionsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFunctionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFunctionsDialog)
	DDX_Control(pDX, IDC_FUNCTIONCODE, m_functionCode);
	DDX_Control(pDX, IDC_FUNCTIONLIST, m_functionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFunctionsDialog, CDialog)
	//{{AFX_MSG_MAP(CFunctionsDialog)
	ON_LBN_SELCHANGE(IDC_FUNCTIONLIST, OnSelchangeFunctionlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFunctionsDialog message handlers

BOOL CFunctionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Refresh all the files.
	g_wwhizInterface->RefreshFileList();

	// Refresh the tags.
	g_wwhizInterface->RefreshTagList();

	// Get the current filename.
	CString currentFilename;
	g_wwhizInterface->GetCurrentFilename(currentFilename);

	// Get the WWhizFile pointer based on the current filename.
	WWhizFile* file = g_wwhizInterface->GetFileList().Find(currentFilename);

	// Retrieve the ordered tag list.
	m_orderedTagList = &file->GetOrderedTagList();

	// Iterate the ordered tag list looking for functions.
	UINT tagListCount = m_orderedTagList->GetCount();
	for (UINT i = 0; i < tagListCount; ++i)
	{
		// Get a pointer to the tag.
		WWhizTag* tag = m_orderedTagList->Get(i);

		// Is it a function?
		if (tag->GetType() == WWhizTag::FUNCTION)
		{
			// Yes, build the qualified name.
			CString fullName = tag->GetParentIdent() + CString("::") + tag->GetIdent();

			// Add it to the list box.
			int index = m_functionList.AddString(fullName);

			// Set the list box item's data to be the index of the tag within
			// the ordered tag list.
			m_functionList.SetItemData(index, i);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFunctionsDialog::OnSelchangeFunctionlist() 
{
	// Get the current selection.
	int curSel = m_functionList.GetCurSel();
	if (curSel == LB_ERR)
		return;

	// Set up the object model for the active document.
	ObjModelHelper objModel;
	objModel.GetActiveDocument();
	
	// Retrieve the ordered tag list index from the current selection's item data.
	int tagNumber = m_functionList.GetItemData(curSel);

	// Get the selected tag.
	WWhizTag* tag = m_orderedTagList->Get(tagNumber);

	// Move to the line number the selected tag starts at.
	objModel.MoveTo(tag->GetLineNumber(), 1, dsMove);

	// Now extend the selection to the top of the next tag.
	WWhizTag* nextTag = NULL;
	if (tagNumber + 1 < m_orderedTagList->GetCount())
	{
		// There was another tag to go off.  Use it.
		nextTag = m_orderedTagList->Get(tagNumber + 1);
		objModel.MoveTo(nextTag->GetLineNumber(), 1, dsExtend);
	}
	else
	{
		// The selected tag was the last one in the file.
		objModel.EndOfDocument(dsExtend);
	}
	
	// Retrieve the text.
	CString text = objModel.GetText();

	// Set it into the rich edit control.
	m_functionCode.SetWindowText(text);
}
